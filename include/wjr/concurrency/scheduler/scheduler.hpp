#ifndef WJR_CONCURRENCY_SCHEDULER_SCHEDULER_HPP__
#define WJR_CONCURRENCY_SCHEDULER_SCHEDULER_HPP__

#include <atomic>
#include <condition_variable>
#include <mutex>

#include <wjr/concurrency/scheduler/task.hpp>
#include <wjr/vector.hpp>

namespace wjr {
namespace thr {

class worker;
class thread_pool;

class worker : public intrusive::list_node<worker> {
    friend class thread_pool;

public:
    worker(thread_pool *pool) : m_pool(pool) {}

    void enqueue(std::function<void()> task);

private:
    void run();

    intrusive::list_node<task> m_tasks;
    bool m_done = false;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    thread_pool *m_pool;
};

class thread_pool {
    friend worker;

public:
    thread_pool(size_t thread_number);
    ~thread_pool();

    void enqueue(std::function<void()> tsk);

private:
    worker *select_worker() noexcept;

    std::atomic<intrusive::list_node<worker> *> m_worker_head;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    fixed_vector<worker> m_workers;
    std::thread m_bg;
    fixed_vector<std::thread> m_threads;
};

} // namespace thr

using thr::thread_pool;

} // namespace wjr

#endif // WJR_CONCURRENCY_SCHEDULER_SCHEDULER_HPP__