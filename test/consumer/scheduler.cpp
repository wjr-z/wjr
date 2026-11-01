#include <wjr/concurrency/scheduler/eevdf.hpp>

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
    if (queue.begin_dispatch().entity != &second ||
        queue.complete_dispatch(1, true) != eevdf_status::success ||
        queue.begin_dispatch().entity != &first ||
        queue.complete_dispatch(1, true) != eevdf_status::success) {
        if (queue.current() != nullptr) {
            (void)queue.complete_dispatch(0, true);
        }
        (void)queue.clear();
        return 2;
    }
    return queue.empty() ? 0 : 3;
}
