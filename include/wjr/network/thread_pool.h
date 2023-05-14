#ifndef __WJR_NETWORK_THREAD_POOL_H
#define __WJR_NETWORK_THREAD_POOL_H

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <chrono>

#include <wjr/vector.h>
#include <wjr/circular_buffer.h>
#include <wjr/crtp.h>
#include <wjr/allocator.h>

_WJR_BEGIN

class thread_pool : public nocopy_and_moveable {
public:
    using queue_type = circular_buffer<std::function<void()>,
        aligned_allocator<std::function<void()>>>;

    static size_t default_threads_size();

    thread_pool(
        unsigned int core_threads_size = default_threads_size());
    ~thread_pool();

    std::mutex& get_mutex();

    void pause();
    void unpause();
    void flush();

    // if is pause, only flush active tasks
    // else flush all tasks
    // then destroy threads
    void shutdown();
    // if is pause, only flush active tasks
    // else flush all tasks
    // then destroy threads
    // return tasks that not done
    // notice that it will return nothing if not in a paused state
    queue_type shutdown(bool);

    template<typename Func, typename...Args>
    void push(Func&& func, Args&&...args);

    template<typename Func, typename...Args, typename R = std::invoke_result_t<Func&&, Args&&...>>
    WJR_NODISCARD std::future<R> submit(Func&& func, Args&&... args);

    template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
    void append(iter _First, iter _Last);

    unsigned int get_threads_size() const;

private:
    void core_work();

    void create_all_threads(unsigned int core_threads_size);

    alignas(8) std::mutex m_task_mutex;
    alignas(8) queue_type m_task_queue;
    alignas(8) bool m_valid = true;
	
    alignas(64) std::condition_variable m_task_cv;
    // Not used frequently, so average consumption is reduced through mutual exclusion protection
    alignas(8) bool m_pause = false;
    alignas(8) std::atomic<size_t> m_real_tasks = 0;
    // padding
    alignas(8) wjr::vector<std::thread> m_core_threads;

    alignas(64) bool m_flush = false;
    alignas(8) std::condition_variable m_flush_cv;
};

template<typename Func, typename...Args>
void thread_pool::push(Func&& func, Args&&...args) {
    std::function<void()> function = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
    {
        std::unique_lock task_lock(m_task_mutex);
        m_task_queue.push_back(std::move(function));
#if defined(_WJR_EXCEPTION)
        if (WJR_UNLIKELY(!m_valid)) {
            throw std::runtime_error("can't put task into invalid thread_pool");
        }
#endif // _WJR_EXCEPTION
    }
    m_real_tasks.fetch_add(1, std::memory_order_relaxed);
    m_task_cv.notify_one();
}

template<typename Func, typename...Args, typename R>
WJR_NODISCARD std::future<R> thread_pool::submit(Func&& func, Args&&... args) {

    std::function<R()> task_function = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
    std::shared_ptr<std::promise<R>> task_promise = std::make_shared<std::promise<R>>();
    push(
        [func = std::move(task_function), task_promise]() {
#if defined(_WJR_EXCEPTION)
            try {
#endif // _WJR_EXCEPTION
                if constexpr (std::is_void_v<R>) {
                    std::invoke(func);
                    task_promise->set_value();
                }
                else {
                    task_promise->set_value(std::invoke(func));
                }
#if defined(_WJR_EXCEPTION)
            }
            catch (...) {
                try {
                    task_promise->set_exception(std::current_exception());
                }
                catch (...) {
                }
            }
#endif // _WJR_EXCEPTION
        });
    return task_promise->get_future();
}

template<typename iter, std::enable_if_t<is_iterator_v<iter>, int>>
void thread_pool::append(iter _First, iter _Last) {
    std::unique_lock task_lock(m_task_mutex);
    const auto _Oldsize = m_task_queue.size();
    m_task_queue.append(_First, _Last);
    const auto _Newsize = m_task_queue.size();
    task_lock.unlock();
    size_t n;
    if constexpr (is_random_iterator_v<iter>) {
        n = std::distance(_First, _Last);
    }
    else {
        n = _Newsize - _Oldsize;
    }
    if (n == 0) {
    }
    else {
        m_real_tasks.fetch_add(n, std::memory_order_relaxed);
        if (n > get_threads_size()) {
            m_task_cv.notify_all();
        }
        else {
            for (; n != 0; --n) {
                m_task_cv.notify_one();
            }
        }
    }
}

_WJR_END

#endif // __WJR_NETWORK_THREAD_POOL_H