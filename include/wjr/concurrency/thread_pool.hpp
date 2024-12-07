#ifndef WJR_CONCURRENCY_THREAD_POOL_HPP__
#define WJR_CONCURRENCY_THREAD_POOL_HPP__

#include <wjr/container/list.hpp>

namespace wjr {
class task_struct : intrusive::list_node<task_struct> {
private:
    std::function<void()> m_task;
};
} // namespace wjr

#endif // WJR_CONCURRENCY_THREAD_POOL_HPP__