#include <wjr/concurrency/scheduler/eevdf.hpp>

static bool finish(wjr::sched::eevdf_queue &queue, wjr::sched::eevdf_entity &expected) {
    const auto dispatch = queue.begin_dispatch();
    return dispatch.status() == wjr::sched::eevdf_status::success &&
           dispatch.entity() == &expected &&
           queue.complete_dispatch(1, true) == wjr::sched::eevdf_status::success;
}

int main() {
    using namespace wjr::sched;
    eevdf_entity first, second;
    eevdf_queue queue;
    if (queue.enqueue(first, {1, 8}) != eevdf_status::success ||
        queue.enqueue(second, {2, 1}) != eevdf_status::success) {
        (void)queue.clear();
        return 1;
    }
    queue.close();
    if (!finish(queue, second) || !finish(queue, first)) {
        if (queue.current() != nullptr) {
            (void)queue.complete_dispatch(0, true);
        }
        (void)queue.clear();
        return 2;
    }
    return queue.empty() ? 0 : 3;
}
