#include <wjr/concurrency/scheduler/scoped_executor.hpp>

#include <array>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

namespace {
using namespace wjr::sched;

class gate {
public:
    void wait() {
        std::unique_lock lock(m_mutex);
        m_changed.wait(lock, [this] { return m_open; });
    }
    void open() {
        std::lock_guard lock(m_mutex);
        m_open = true;
        m_changed.notify_all();
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_changed;
    bool m_open = false;
};

struct count_task {
    size_t *count;
    std::thread::id *owner;
    size_t remaining;
    task_step operator()(task_context &) noexcept {
        ++*count;
        *owner = std::this_thread::get_id();
        return --remaining == 0 ? task_step::finished : task_step::again;
    }
};

struct throwing_task {
    task_step operator()(task_context &) { return task_step::finished; }
};

template <size_t N>
auto make_tasks(std::array<size_t, N> &counts, std::array<std::thread::id, N> &owners,
                size_t remaining) {
    return [&]<size_t... I>(std::index_sequence<I...>) {
        return std::array<cooperative_task<count_task>, N>{
            cooperative_task(count_task{&counts[I], &owners[I], remaining})...};
    }(std::make_index_sequence<N>{});
}

template <class S>
concept admitted_batch = requires(const submitter &submit, S tasks) { submit.submit_batch(tasks); };
static_assert(admitted_batch<std::span<cooperative_task<count_task>>>);
static_assert(!admitted_batch<std::span<const cooperative_task<count_task>>>);
static_assert(!admitted_batch<std::span<cooperative_task<count_task> *>>);
template <class F>
concept admitted_task = requires { typename cooperative_task<F>; };
static_assert(!admitted_task<throwing_task>);
static_assert(admitted_task<count_task>);
static_assert(!std::is_move_constructible_v<cooperative_task<count_task>>);
static_assert(!std::is_copy_constructible_v<task_scope>);

TEST(ScopedExecutor, AdmissionAndOneShotStorage) {
    cooperative_executor executor;
    size_t count = 0;
    std::thread::id owner;
    cooperative_task task(count_task{&count, &owner, 3});
    EXPECT_EQ(submitter{}.submit(task), executor_status::unavailable);
    EXPECT_EQ(task.outcome(), task_outcome::not_submitted);
    task_scope empty(executor, 0);
    EXPECT_EQ(empty.make_submitter().submit(task), executor_status::capacity_limit);
    task_scope scope(executor, 1);
    const auto submit = scope.make_submitter();
    ASSERT_EQ(submit.submit(task), executor_status::success);
    EXPECT_EQ(submit.submit(task), executor_status::already_submitted);
    cooperative_task extra(count_task{&count, &owner, 1});
    EXPECT_EQ(submit.submit(extra), executor_status::capacity_limit);
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(count, 3u);
    EXPECT_NE(owner, std::this_thread::get_id());
    EXPECT_EQ(task.outcome(), task_outcome::completed);
    EXPECT_EQ(submit.submit(extra), executor_status::closed);
    task_scope next(executor, 1);
    EXPECT_EQ(next.make_submitter().submit(task), executor_status::already_submitted);
}

TEST(ScopedExecutor, FifoRoundRobinAfterPublication) {
    cooperative_executor executor;
    task_scope scope(executor, 4);
    gate started, release;
    cooperative_task blocker([&](task_context &) noexcept {
        started.open();
        release.wait();
        return task_step::finished;
    });
    const auto submit = scope.make_submitter();
    ASSERT_EQ(submit.submit(blocker), executor_status::success);
    started.wait();
    std::vector<int> trace;
    trace.reserve(9);
    auto make = [&](int id) {
        return [&, id, remaining = 3](task_context &) mutable noexcept {
            trace.push_back(id);
            return --remaining == 0 ? task_step::finished : task_step::again;
        };
    };
    cooperative_task a(make(1)), b(make(2)), c(make(3));
    EXPECT_EQ(submit.submit(a), executor_status::success);
    EXPECT_EQ(submit.submit(b), executor_status::success);
    EXPECT_EQ(submit.submit(c), executor_status::success);
    release.open();
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(trace, (std::vector<int>{1, 2, 3, 1, 2, 3, 1, 2, 3}));
}

TEST(ScopedExecutor, ConcurrentProducersAndMoreThan256Tasks) {
    constexpr size_t producers = 4, per_producer = 257, total = producers * per_producer;
    cooperative_executor executor;
    task_scope scope(executor, total);
    const auto submit = scope.make_submitter();
    std::array<size_t, total> counts{};
    std::array<std::thread::id, total> owners{};
    std::vector<std::unique_ptr<cooperative_task<count_task>>> tasks;
    for (size_t i = 0; i < total; ++i) {
        tasks.push_back(
            std::make_unique<cooperative_task<count_task>>(count_task{&counts[i], &owners[i], 5}));
    }
    std::atomic<size_t> failures{0};
    std::vector<std::thread> threads;
    for (size_t p = 0; p < producers; ++p) {
        threads.emplace_back([&, p] {
            for (size_t i = p * per_producer; i < (p + 1) * per_producer; ++i) {
                if (submit.submit(*tasks[i]) != executor_status::success) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }
    for (auto &thread : threads) {
        thread.join();
    }
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(failures.load(), 0u);
    for (size_t i = 0; i < total; ++i) {
        EXPECT_EQ(counts[i], 5u);
        EXPECT_EQ(owners[i], owners[0]);
        EXPECT_EQ(tasks[i]->outcome(), task_outcome::completed);
    }
}

TEST(ScopedExecutor, ScopeCancellationAndFinishedWins) {
    cooperative_executor executor;
    task_scope scope(executor, 3);
    gate started, release;
    cooperative_task active([&](task_context &context) noexcept {
        started.open();
        release.wait();
        return context.stop_requested() ? task_step::again : task_step::finished;
    });
    size_t count = 0;
    std::thread::id owner;
    cooperative_task waiting(count_task{&count, &owner, 1});
    const auto submit = scope.make_submitter();
    ASSERT_EQ(submit.submit(active), executor_status::success);
    started.wait();
    EXPECT_EQ(submit.submit(waiting), executor_status::success);
    scope.request_stop();
    release.open();
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(active.outcome(), task_outcome::cancelled);
    EXPECT_EQ(waiting.outcome(), task_outcome::cancelled);
    EXPECT_EQ(count, 0u);

    task_scope finished_scope(executor, 1);
    gate finishing, finish_release;
    cooperative_task finished([&](task_context &) noexcept {
        finishing.open();
        finish_release.wait();
        return task_step::finished;
    });
    ASSERT_EQ(finished_scope.make_submitter().submit(finished), executor_status::success);
    finishing.wait();
    finished_scope.request_stop();
    finish_release.open();
    ASSERT_EQ(finished_scope.join(), executor_status::success);
    EXPECT_EQ(finished.outcome(), task_outcome::completed);
}

TEST(ScopedExecutor, IndividualStopAndMoveOnlyCallable) {
    cooperative_executor executor;
    task_scope scope(executor, 2);
    size_t count = 0;
    cooperative_task cancelled([&](task_context &) noexcept {
        ++count;
        return task_step::finished;
    });
    cancelled.request_stop();
    auto owned = std::make_unique<size_t>(42);
    size_t result = 0;
    cooperative_task moved([&, payload = std::move(owned)](task_context &) noexcept {
        result = *payload;
        return task_step::finished;
    });
    EXPECT_EQ(scope.make_submitter().submit(cancelled), executor_status::success);
    EXPECT_EQ(scope.make_submitter().submit(moved), executor_status::success);
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(count, 0u);
    EXPECT_EQ(result, 42u);
    EXPECT_EQ(cancelled.outcome(), task_outcome::cancelled);
}

TEST(ScopedExecutor, CloseCancelPollAndSealedAdmission) {
    cooperative_executor executor;
    task_scope scope(executor, 2);
    gate started;
    cooperative_task task([&](task_context &context) noexcept {
        started.open();
        while (!context.stop_requested()) {
            std::this_thread::yield();
        }
        return task_step::again;
    });
    ASSERT_EQ(scope.make_submitter().submit(task), executor_status::success);
    started.wait();
    EXPECT_EQ(executor.close_cancel(), executor_status::success);
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(task.outcome(), task_outcome::cancelled);
    cooperative_task late([](task_context &) noexcept { return task_step::finished; });
    EXPECT_EQ(scope.make_submitter().submit(late), executor_status::closed);
    EXPECT_EQ(executor.close_drain(), executor_status::success);
}

TEST(ScopedExecutor, DrainAndCachedHandlesOutliveFacades) {
    submitter retained;
    size_t count = 0;
    std::thread::id owner;
    cooperative_task task(count_task{&count, &owner, 100});
    {
        cooperative_executor executor;
        task_scope scope(executor, 1);
        retained = scope.make_submitter();
        ASSERT_EQ(retained.submit(task), executor_status::success);
        ASSERT_EQ(executor.close_drain(), executor_status::success);
        EXPECT_EQ(count, 100u);
        EXPECT_EQ(scope.join(), executor_status::success);
    }
    EXPECT_EQ(task.outcome(), task_outcome::completed);
    cooperative_task late(count_task{&count, &owner, 1});
    EXPECT_EQ(retained.submit(late), executor_status::closed);
}

TEST(ScopedExecutor, WorkerCannotJoinOrCloseItself) {
    cooperative_executor executor;
    task_scope scope(executor, 1);
    std::array<executor_status, 3> results{};
    cooperative_task task([&](task_context &) noexcept {
        results = {scope.join(), executor.close_drain(), executor.close_cancel()};
        return task_step::finished;
    });
    ASSERT_EQ(scope.make_submitter().submit(task), executor_status::success);
    ASSERT_EQ(scope.join(), executor_status::success);
    for (auto status : results) {
        EXPECT_EQ(status, executor_status::would_deadlock);
    }
}

TEST(ScopedExecutor, ScopeOutlivesExecutorFacade) {
    size_t count = 0;
    std::thread::id owner;
    cooperative_task task(count_task{&count, &owner, 5});
    auto executor = std::make_unique<cooperative_executor>();
    task_scope scope(*executor, 1);
    ASSERT_EQ(scope.make_submitter().submit(task), executor_status::success);
    executor.reset();
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(count, 5u);
    EXPECT_EQ(task.outcome(), task_outcome::completed);
}

TEST(ScopedExecutor, RepeatedIdlePublicationAndImmediateReclamation) {
    cooperative_executor executor;
    for (size_t batch = 0; batch < 200; ++batch) {
        task_scope empty(executor, 0);
        empty.request_stop();
        size_t count = 0;
        std::thread::id owner;
        task_scope scope(executor, 1);
        auto task = std::make_unique<cooperative_task<count_task>>(count_task{&count, &owner, 2});
        ASSERT_EQ(scope.make_submitter().submit(*task), executor_status::success);
        ASSERT_EQ(scope.join(), executor_status::success);
        EXPECT_EQ(count, 2u);
        task.reset();
    }
}

TEST(ScopedExecutor, ConcurrentCloseAndJoinPreserveLastAccess) {
    for (size_t iteration = 0; iteration < 50; ++iteration) {
        cooperative_executor executor;
        task_scope scope(executor, 1);
        size_t count = 0;
        std::thread::id owner;
        auto task = std::make_unique<cooperative_task<count_task>>(count_task{&count, &owner, 20});
        ASSERT_EQ(scope.make_submitter().submit(*task), executor_status::success);
        std::thread closer([&] { EXPECT_EQ(executor.close_drain(), executor_status::success); });
        ASSERT_EQ(scope.join(), executor_status::success);
        task.reset();
        closer.join();
        EXPECT_EQ(count, 20u);
    }
}

TEST(ScopedExecutor, AdmissionRaceWithGlobalDrain) {
    for (size_t iteration = 0; iteration < 20; ++iteration) {
        constexpr size_t total = 128;
        cooperative_executor executor;
        task_scope scope(executor, total);
        const auto submit = scope.make_submitter();
        std::array<size_t, total> counts{};
        std::array<std::thread::id, total> owners{};
        std::array<executor_status, total> statuses{};
        std::vector<std::unique_ptr<cooperative_task<count_task>>> tasks;
        for (size_t i = 0; i < total; ++i) {
            tasks.push_back(std::make_unique<cooperative_task<count_task>>(
                count_task{&counts[i], &owners[i], 3}));
        }
        gate start;
        std::array<std::thread, 2> producers;
        for (size_t p = 0; p < producers.size(); ++p) {
            producers[p] = std::thread([&, p] {
                start.wait();
                for (size_t i = p; i < total; i += producers.size()) {
                    statuses[i] = submit.submit(*tasks[i]);
                }
            });
        }
        start.open();
        EXPECT_EQ(executor.close_drain(), executor_status::success);
        for (auto &producer : producers) {
            producer.join();
        }
        ASSERT_EQ(scope.join(), executor_status::success);
        for (size_t i = 0; i < total; ++i) {
            if (statuses[i] == executor_status::success) {
                EXPECT_EQ(counts[i], 3u);
                EXPECT_EQ(tasks[i]->outcome(), task_outcome::completed);
            } else {
                EXPECT_EQ(statuses[i], executor_status::closed);
                EXPECT_EQ(tasks[i]->outcome(), task_outcome::not_submitted);
                EXPECT_EQ(counts[i], 0u);
            }
        }
    }
}

TEST(ScopedExecutor, EmptyAndSingletonBatches) {
    using task_type = cooperative_task<count_task>;
    EXPECT_EQ(submitter{}.submit_batch(std::span<task_type>{}), executor_status::success);
    cooperative_executor executor;
    task_scope scope(executor, 1);
    std::array<size_t, 1> counts{};
    std::array<std::thread::id, 1> owners{};
    auto tasks = make_tasks(counts, owners, 2);
    EXPECT_EQ(submitter{}.submit_batch(std::span(tasks)), executor_status::unavailable);
    EXPECT_EQ(scope.make_submitter().submit_batch(std::span(tasks)), executor_status::success);
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(counts[0], 2u);
    EXPECT_EQ(tasks[0].outcome(), task_outcome::completed);
    EXPECT_EQ(scope.make_submitter().submit_batch(std::span<task_type>{}),
              executor_status::success);
}

TEST(ScopedExecutor, BatchBudgetRejectionDoesNotConsumeMembership) {
    cooperative_executor executor;
    task_scope scope(executor, 2);
    std::array<size_t, 3> counts{};
    std::array<std::thread::id, 3> owners{};
    auto tasks = make_tasks(counts, owners, 3);
    const auto submit = scope.make_submitter();
    EXPECT_EQ(submitter{}.submit_batch(std::span(tasks)), executor_status::unavailable);
    EXPECT_EQ(submit.submit_batch(std::span(tasks)), executor_status::capacity_limit);
    for (auto &task : tasks) {
        EXPECT_EQ(task.outcome(), task_outcome::not_submitted);
    }
    ASSERT_EQ(submit.submit_batch(std::span(tasks).first<2>()), executor_status::success);
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(counts, (std::array<size_t, 3>{3, 3, 0}));
    EXPECT_EQ(tasks[2].outcome(), task_outcome::not_submitted);
}

TEST(ScopedExecutor, BatchAlreadyAcceptedMemberRejectsEveryFreshMember) {
    cooperative_executor executor;
    task_scope scope(executor, 3);
    std::array<size_t, 3> counts{};
    std::array<std::thread::id, 3> owners{};
    auto tasks = make_tasks(counts, owners, 2);
    const auto submit = scope.make_submitter();
    ASSERT_EQ(submit.submit(tasks[1]), executor_status::success);
    EXPECT_EQ(submit.submit_batch(std::span(tasks)), executor_status::already_submitted);
    EXPECT_EQ(tasks[0].outcome(), task_outcome::not_submitted);
    EXPECT_EQ(tasks[2].outcome(), task_outcome::not_submitted);
    EXPECT_EQ(submit.submit(tasks[0]), executor_status::success);
    EXPECT_EQ(submit.submit(tasks[2]), executor_status::success);
    ASSERT_EQ(scope.join(), executor_status::success);
    EXPECT_EQ(counts, (std::array<size_t, 3>{2, 2, 2}));
    task_scope next(executor, 3);
    EXPECT_EQ(next.make_submitter().submit_batch(std::span(tasks)),
              executor_status::already_submitted);
}

TEST(ScopedExecutor, BatchScopeCancellationBeforeWorkerIngestion) {
    cooperative_executor executor;
    task_scope blocker_scope(executor, 1), scope(executor, 3), sealed(executor, 3);
    gate started, release;
    cooperative_task blocker([&](task_context &) noexcept {
        started.open();
        release.wait();
        return task_step::finished;
    });
    ASSERT_EQ(blocker_scope.make_submitter().submit(blocker), executor_status::success);
    started.wait();
    std::array<size_t, 3> counts{};
    std::array<std::thread::id, 3> owners{};
    auto tasks = make_tasks(counts, owners, 2);
    sealed.request_stop();
    EXPECT_EQ(sealed.make_submitter().submit_batch(std::span(tasks)), executor_status::closed);
    ASSERT_EQ(scope.make_submitter().submit_batch(std::span(tasks)), executor_status::success);
    scope.request_stop();
    release.open();
    ASSERT_EQ(scope.join(), executor_status::success);
    ASSERT_EQ(blocker_scope.join(), executor_status::success);
    for (auto &task : tasks) {
        EXPECT_EQ(task.outcome(), task_outcome::cancelled);
    }
    EXPECT_EQ(counts, (std::array<size_t, 3>{0, 0, 0}));
}

TEST(ScopedExecutor, BatchGlobalCancelAndImmediateReclamation) {
    cooperative_executor executor;
    task_scope blocker_scope(executor, 1), scope(executor, 4);
    gate started;
    cooperative_task blocker([&](task_context &context) noexcept {
        started.open();
        while (!context.stop_requested()) {
            std::this_thread::yield();
        }
        return task_step::finished;
    });
    ASSERT_EQ(blocker_scope.make_submitter().submit(blocker), executor_status::success);
    started.wait();
    std::array<size_t, 4> counts{};
    std::array<std::thread::id, 4> owners{};
    using task_array = decltype(make_tasks(counts, owners, 2));
    auto tasks = std::unique_ptr<task_array>(new task_array(make_tasks(counts, owners, 2)));
    ASSERT_EQ(scope.make_submitter().submit_batch(std::span(*tasks)), executor_status::success);
    EXPECT_EQ(executor.close_cancel(), executor_status::success);
    ASSERT_EQ(scope.join(), executor_status::success);
    ASSERT_EQ(blocker_scope.join(), executor_status::success);
    for (auto &task : *tasks) {
        EXPECT_EQ(task.outcome(), task_outcome::cancelled);
    }
    tasks.reset();
    EXPECT_EQ(counts, (std::array<size_t, 4>{0, 0, 0, 0}));
}

TEST(ScopedExecutor, BatchAdmissionRaceWithGlobalDrainIsAllOrNone) {
    constexpr size_t producers = 4, batch = 17, total = producers * batch;
    for (size_t iteration = 0; iteration < 10; ++iteration) {
        cooperative_executor executor;
        task_scope scope(executor, total);
        const auto submit = scope.make_submitter();
        std::array<size_t, total> counts{};
        std::array<std::thread::id, total> owners{};
        auto tasks = make_tasks(counts, owners, 3);
        std::array<executor_status, producers> statuses{};
        std::array<std::thread, producers> threads;
        gate start;
        for (size_t p = 0; p < producers; ++p) {
            threads[p] = std::thread([&, p] {
                start.wait();
                statuses[p] = submit.submit_batch(std::span(tasks).subspan(p * batch, batch));
            });
        }
        start.open();
        EXPECT_EQ(executor.close_drain(), executor_status::success);
        for (auto &thread : threads) {
            thread.join();
        }
        ASSERT_EQ(scope.join(), executor_status::success);
        for (size_t p = 0; p < producers; ++p) {
            const bool accepted = statuses[p] == executor_status::success;
            EXPECT_TRUE(accepted || statuses[p] == executor_status::closed);
            for (size_t i = p * batch; i < (p + 1) * batch; ++i) {
                EXPECT_EQ(counts[i], accepted ? 3u : 0u);
                EXPECT_EQ(tasks[i].outcome(),
                          accepted ? task_outcome::completed : task_outcome::not_submitted);
            }
        }
    }
}

TEST(ScopedExecutor, ConcurrentDisjointBatchesAllAccepted) {
    constexpr size_t producers = 4, batch = 17, total = producers * batch;
    cooperative_executor executor;
    task_scope scope(executor, total);
    const auto submit = scope.make_submitter();
    std::array<size_t, total> counts{};
    std::array<std::thread::id, total> owners{};
    auto tasks = make_tasks(counts, owners, 2);
    std::array<executor_status, producers> statuses{};
    std::array<std::thread, producers> threads;
    for (size_t p = 0; p < producers; ++p) {
        threads[p] = std::thread([&, p] {
            statuses[p] = submit.submit_batch(std::span(tasks).subspan(p * batch, batch));
        });
    }
    for (auto &thread : threads) {
        thread.join();
    }
    ASSERT_EQ(scope.join(), executor_status::success);
    for (auto status : statuses) {
        EXPECT_EQ(status, executor_status::success);
    }
    for (size_t i = 0; i < total; ++i) {
        EXPECT_EQ(counts[i], 2u);
        EXPECT_EQ(owners[i], owners[0]);
    }
}

TEST(ScopedExecutor, BatchJoinAllowsReclaimWhileWorkerLives) {
    cooperative_executor executor;
    for (size_t iteration = 0; iteration < 100; ++iteration) {
        task_scope scope(executor, 4);
        std::array<size_t, 4> counts{};
        std::array<std::thread::id, 4> owners{};
        using task_array = decltype(make_tasks(counts, owners, 2));
        auto tasks = std::unique_ptr<task_array>(new task_array(make_tasks(counts, owners, 2)));
        ASSERT_EQ(scope.make_submitter().submit_batch(std::span(*tasks)), executor_status::success);
        ASSERT_EQ(scope.join(), executor_status::success);
        tasks.reset();
        EXPECT_EQ(counts, (std::array<size_t, 4>{2, 2, 2, 2}));
    }
}
} // namespace
