#include <wjr/concurrency/scheduler/scoped_executor.hpp>

#include <array>

struct increment {
    size_t *result;
    size_t left;
    wjr::sched::task_step operator()(wjr::sched::task_context &) noexcept {
        ++*result;
        return --left == 0 ? wjr::sched::task_step::finished : wjr::sched::task_step::again;
    }
};

int main() {
    using namespace wjr::sched;
    size_t result = 0;
    std::array tasks{cooperative_task(increment{&result, 3}),
                     cooperative_task(increment{&result, 2})};
    cooperative_executor executor;
    task_scope scope(executor, 2);
    if (scope.make_submitter().submit_batch(std::span(tasks)) != executor_status::success ||
        scope.join() != executor_status::success) {
        return 1;
    }
    return result == 5 && tasks[0].outcome() == task_outcome::completed &&
                   tasks[1].outcome() == task_outcome::completed
               ? 0
               : 2;
}
