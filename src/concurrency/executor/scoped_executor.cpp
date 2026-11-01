#include <wjr/concurrency/scheduler/scoped_executor.hpp>

#include <condition_variable>
#include <exception>
#include <mutex>
#include <thread>

namespace wjr::sched::detail {

struct scope_state : intrusive::list_node<scope_state> {
    scope_state(std::shared_ptr<executor_domain> domain, size_t capacity);
    ~scope_state();
    std::shared_ptr<executor_domain> domain;
    intrusive::list_node<task_base, task_scope_tag> tasks;
    std::condition_variable done;
    std::atomic<bool> stop{false};
    const size_t capacity;
    size_t accepted = 0;
    size_t pending = 0;
    bool sealed = false;
};

struct executor_domain {
    using queue_hook = intrusive::list_node<task_base, task_queue_tag>;
    using scope_hook = intrusive::list_node<task_base, task_scope_tag>;

    void start() {
        worker = std::thread([this] { run(); });
        owner = worker.get_id();
    }
    bool on_worker() const noexcept {
        return running.load(std::memory_order_acquire) && std::this_thread::get_id() == owner;
    }

    /// @brief Notify only after making the wait predicate true, before releasing publication lock.
    void notify_locked() noexcept {
        WJR_ASSERT_L2(closed || !inbox.empty());
        if (sleeping) {
            changed.notify_one();
            sleeping = false;
        }
    }

    executor_status close(bool cancel) {
        if (on_worker()) {
            return executor_status::would_deadlock;
        }
        {
            std::lock_guard lock(mutex);
            closed = true;
            if (cancel) {
                for (auto &scope : scopes) {
                    scope.stop.store(true, std::memory_order_relaxed);
                    scope.sealed = true;
                }
            }
            notify_locked();
        }
        if (worker.joinable()) {
            worker.join();
        }
        return executor_status::success;
    }

    void finish(task_base &task, task_outcome outcome) noexcept {
        std::lock_guard lock(mutex);
        auto &scope = *task.m_scope;
        task.m_outcome = outcome;
        --scope.pending;
        if (scope.pending == 0) {
            scope.done.notify_all();
        }
        /// No task/scope access after this lock releases: join may reclaim borrowed storage.
    }

    void run_task(task_base &task, queue_hook &ready) noexcept {
        auto &scope = *task.m_scope;
        task_context context(task.m_stop, scope.stop);
        if (context.stop_requested()) {
            finish(task, task_outcome::cancelled);
            return;
        }
        const auto result = task.m_invoke(task, context);
        if (result == task_step::again && !context.stop_requested()) {
            ready.push_back(static_cast<queue_hook *>(&task));
        } else {
            finish(task, result == task_step::finished ? task_outcome::completed
                                                       : task_outcome::cancelled);
        }
    }

    bool ingest(queue_hook &ready) {
        std::unique_lock lock(mutex);
        if (ready.empty() && inbox.empty() && !closed) {
            sleeping = true;
            changed.wait(lock, [this] { return closed || !inbox.empty(); });
            sleeping = false;
        }
        ready.splice_all_before(&inbox);
        return !ready.empty();
    }

    void run() noexcept {
        queue_hook ready;
        size_t until_ingest = 0;
        for (;;) {
            if (ready.empty() || until_ingest == 0) {
                if (!ingest(ready)) {
                    running.store(false, std::memory_order_release);
                    return;
                }
                until_ingest = 64;
            }
            auto *task = ready.pop_front()->self();
            run_task(*task, ready);
            --until_ingest;
        }
    }

    std::mutex mutex;
    std::condition_variable changed;
    queue_hook inbox;
    intrusive::list_node<scope_state> scopes;
    std::thread worker;
    std::thread::id owner;
    std::atomic<bool> running{true};
    bool closed = false;
    bool sleeping = false;
};

scope_state::scope_state(std::shared_ptr<executor_domain> domain_, size_t capacity_)
    : domain(std::move(domain_)), capacity(capacity_) {
    std::lock_guard lock(domain->mutex);
    sealed = domain->closed;
    domain->scopes.push_back(this);
}

scope_state::~scope_state() {
    WJR_ASSERT_L2(pending == 0 && tasks.empty());
    std::lock_guard lock(domain->mutex);
    remove();
}

} // namespace wjr::sched::detail

namespace wjr::sched {

cooperative_executor::cooperative_executor()
    : m_domain(std::make_shared<detail::executor_domain>()) {
    m_domain->start();
}

cooperative_executor::~cooperative_executor() noexcept {
    if (close_drain() != executor_status::success) {
        std::terminate();
    }
}

executor_status cooperative_executor::close_drain() { return m_domain->close(false); }
executor_status cooperative_executor::close_cancel() { return m_domain->close(true); }

task_scope::task_scope(cooperative_executor &executor, size_t maximum_tasks)
    : m_state(std::make_shared<detail::scope_state>(executor.m_domain, maximum_tasks)) {}

task_scope::~task_scope() noexcept {
    if (join() != executor_status::success) {
        std::terminate();
    }
}

executor_status submitter::submit(detail::task_base &task) const {
    if (!m_state) {
        return executor_status::unavailable;
    }
    auto &scope = *m_state;
    auto &domain = *scope.domain;
    std::lock_guard lock(domain.mutex);
    if (domain.closed || scope.sealed) {
        return executor_status::closed;
    }
    if (task.m_scope != nullptr || task.m_outcome != task_outcome::not_submitted) {
        return executor_status::already_submitted;
    }
    if (scope.accepted == scope.capacity) {
        return executor_status::capacity_limit;
    }
    task.m_scope = &scope;
    task.m_outcome = task_outcome::pending;
    scope.tasks.push_back(static_cast<detail::executor_domain::scope_hook *>(&task));
    domain.inbox.push_back(static_cast<detail::executor_domain::queue_hook *>(&task));
    ++scope.accepted;
    ++scope.pending;
    domain.notify_locked();
    /// After unlock, the owner may complete and scope join may release task storage.
    return executor_status::success;
}

void task_scope::request_stop() {
    auto &domain = *m_state->domain;
    std::lock_guard lock(domain.mutex);
    m_state->sealed = true;
    m_state->stop.store(true, std::memory_order_relaxed);
}

executor_status task_scope::join() {
    auto &scope = *m_state;
    auto &domain = *scope.domain;
    if (domain.on_worker()) {
        return executor_status::would_deadlock;
    }
    std::unique_lock lock(domain.mutex);
    scope.sealed = true;
    scope.done.wait(lock, [&scope] { return scope.pending == 0; });
    while (!scope.tasks.empty()) {
        auto *task = scope.tasks.pop_front()->self();
        task->m_scope = nullptr;
    }
    return executor_status::success;
}

} // namespace wjr::sched
