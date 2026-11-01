#ifndef WJR_CONCURRENCY_SCHEDULER_SCOPED_EXECUTOR_HPP__
#define WJR_CONCURRENCY_SCHEDULER_SCOPED_EXECUTOR_HPP__

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include <wjr/container/list.hpp>

namespace wjr::sched {

enum class task_step { again, finished };
enum class task_outcome { not_submitted, completed, cancelled };
enum class executor_status {
    success,
    closed,
    capacity_limit,
    already_submitted,
    unavailable,
    would_deadlock
};

class cooperative_executor;
class task_scope;
class submitter;

namespace detail {
struct task_queue_tag {};
struct task_scope_tag {};
class task_base;
struct executor_domain;
struct scope_state;
} // namespace detail

class task_context {
    friend struct detail::executor_domain;
    task_context(const std::atomic<bool> &task_stop, const std::atomic<bool> &scope_stop) noexcept
        : m_task_stop(task_stop), m_scope_stop(scope_stop) {}

public:
    /// @brief Poll cooperative cancellation; the flags publish no accompanying user data.
    bool stop_requested() const noexcept {
        return m_task_stop.load(std::memory_order_relaxed) ||
               m_scope_stop.load(std::memory_order_relaxed);
    }

private:
    const std::atomic<bool> &m_task_stop;
    const std::atomic<bool> &m_scope_stop;
};

namespace detail {

class task_base : private intrusive::list_node<task_base, task_queue_tag>,
                  private intrusive::list_node<task_base, task_scope_tag> {
    using queue_hook = intrusive::list_node<task_base, task_queue_tag>;
    using scope_hook = intrusive::list_node<task_base, task_scope_tag>;
    friend class intrusive::list_node_base<task_base, task_queue_tag>;
    friend class intrusive::list_node_base<task_base, task_scope_tag>;
    friend struct executor_domain;
    friend struct scope_state;
    friend class wjr::sched::submitter;
    friend class wjr::sched::task_scope;

protected:
    using invoke_type = task_step (*)(task_base &, task_context &) noexcept;
    explicit task_base(invoke_type invoke) noexcept
        : queue_hook(default_construct), scope_hook(default_construct), m_invoke(invoke) {}
    ~task_base() noexcept { WJR_ASSERT_L2(m_scope == nullptr); }

public:
    task_base(const task_base &) = delete;
    task_base(task_base &&) = delete;
    task_base &operator=(const task_base &) = delete;
    task_base &operator=(task_base &&) = delete;

    /// @brief Request stop without unlinking or destroying storage. Safe while this object lives.
    void request_stop() noexcept { m_stop.store(true, std::memory_order_relaxed); }

    /// @pre Observe only before submission or after successful scope join; never during execution.
    task_outcome outcome() const noexcept {
        WJR_ASSERT_L2(m_scope == nullptr);
        return m_outcome;
    }

private:
    invoke_type m_invoke;
    scope_state *m_scope = nullptr;
    std::atomic<bool> m_stop{false};
    task_outcome m_outcome = task_outcome::not_submitted;
};

} // namespace detail

/**
 * @brief Stable one-shot task owning its callable; accepted storage lives through scope join.
 * @note Calls execute on one background worker. Returning again cooperatively rotates the task.
 * Captured references remain borrowed. Callable construction may throw before any submission.
 */
template <class F>
requires std::is_nothrow_invocable_r_v<task_step, F &, task_context &>
class cooperative_task final : public detail::task_base {
public:
    template <class G>
    requires std::is_constructible_v<F, G &&>
    explicit cooperative_task(G &&function) noexcept(std::is_nothrow_constructible_v<F, G &&>)
        : task_base(&invoke), m_function(std::forward<G>(function)) {}

private:
    static task_step invoke(detail::task_base &base, task_context &context) noexcept {
        return std::invoke(static_cast<cooperative_task &>(base).m_function, context);
    }
    WJR_NO_UNIQUE_ADDRESS F m_function;
};

template <class F>
cooperative_task(F &&) -> cooperative_task<std::decay_t<F>>;

/**
 * @brief Cached submission capability retaining closed scope/domain storage once per handle.
 * @note Sharing const submit calls is supported. Do not destroy/move the handle during a call.
 */
class submitter {
    friend class task_scope;
    explicit submitter(std::shared_ptr<detail::scope_state> state) noexcept
        : m_state(std::move(state)) {}

public:
    submitter() = default;
    /// @pre Concurrent producers use distinct tasks; submission of the same object is serialized.
    executor_status submit(detail::task_base &task) const;

private:
    std::shared_ptr<detail::scope_state> m_state;
};

/**
 * @brief One background CPU worker with cooperative FIFO/round-robin, not EEVDF or I/O.
 * @pre Control/destruction has one owner and must not run on the worker itself.
 * @note Destruction drains. Noncooperating or blocking callbacks can delay close indefinitely.
 * Construction and native synchronization/thread operations may fail with standard exceptions.
 */
class cooperative_executor {
    friend class task_scope;

public:
    cooperative_executor();
    cooperative_executor(const cooperative_executor &) = delete;
    cooperative_executor(cooperative_executor &&) = delete;
    cooperative_executor &operator=(const cooperative_executor &) = delete;
    cooperative_executor &operator=(cooperative_executor &&) = delete;
    ~cooperative_executor() noexcept;

    executor_status close_drain();
    executor_status close_cancel();

private:
    std::shared_ptr<detail::executor_domain> m_domain;
};

/**
 * @brief Structured borrowed-task lifetime and explicit acceptance budget, independent of workers.
 * @pre Accepted tasks and their captured resources outlive join. Destruction must not be on worker.
 * @note Destruction seals/joins; cached submitters can outlive it and safely reject later calls.
 */
class task_scope {
public:
    task_scope(cooperative_executor &executor, size_t maximum_tasks);
    task_scope(const task_scope &) = delete;
    task_scope(task_scope &&) = delete;
    task_scope &operator=(const task_scope &) = delete;
    task_scope &operator=(task_scope &&) = delete;
    ~task_scope() noexcept;

    submitter make_submitter() const noexcept { return submitter(m_state); }
    void request_stop();
    executor_status join();

private:
    std::shared_ptr<detail::scope_state> m_state;
};

} // namespace wjr::sched

#endif // WJR_CONCURRENCY_SCHEDULER_SCOPED_EXECUTOR_HPP__
