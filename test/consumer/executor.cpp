#include <wjr/concurrency/scheduler/scoped_executor.hpp>

int main() {
    using namespace wjr::sched;
    size_t result = 0;
    cooperative_task task([&](task_context &) noexcept {
        ++result;
        return result == 3 ? task_step::finished : task_step::again;
    });
    cooperative_executor executor;
    task_scope scope(executor, 1);
    if (scope.make_submitter().submit(task) != executor_status::success ||
        scope.join() != executor_status::success) {
        return 1;
    }
    return result == 3 && task.outcome() == task_outcome::completed ? 0 : 2;
}
