#ifndef WJR_CONCURRENCY_SCHEDULER_WORKER_HPP__
#define WJR_CONCURRENCY_SCHEDULER_WORKER_HPP__

#include <wjr/container/list.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr::thr {

inline constexpr size_t worker_group_size = 4;
static_assert(worker_group_size <= 16);

class worker : public intrusive::list_node<worker> {};

class worker_group {
public:
private:
    worker *m_workers[worker_group_size];
    uninitialized<worker> m_storage[worker_group_size];
};

} // namespace wjr::thr

#endif // WJR_CONCURRENCY_SCHEDULER_WORKER_HPP__