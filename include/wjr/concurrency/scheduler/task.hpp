#ifndef WJR_CONCURRENCY_THREAD_POOL_TASK_HPP__
#define WJR_CONCURRENCY_THREAD_POOL_TASK_HPP__

#include <functional>

#include <wjr/container/list.hpp>

namespace wjr::thr {
struct task : public intrusive::list_node<task> {
    task() = delete;
    task(std::function<void()> func) : m_func(std::move(func)) {}

    task(const task &) = delete;
    task(task &&) = default;
    task &operator=(const task &) = delete;
    task &operator=(task &&) = default;

    std::function<void()> m_func;
};
} // namespace wjr::thr

#endif // WJR_CONCURRENCY_THREAD_POOL_TASK_HPP__