#pragma once
#ifndef __WJR_NETWORK_THREAD_POOL_H
#define __WJR_NETWORK_THREAD_POOL_H

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <chrono>

#include <wjr/vector.h>
#include <wjr/network/spin_mutex.h>
#include <wjr/circular_buffer.h>

_WJR_BEGIN

class thread_pool {
public:

    thread_pool(
        unsigned int core_threads_size = default_threads_size()); 
    ~thread_pool();

    void pause();
    void unpause();
    void flush();

    bool is_valid() const;
    bool is_paused() const;

    template<typename Func, typename...Args>
    void push(Func&& func, Args&&...args);

    template<typename Func, typename...Args, typename R = std::invoke_result_t<Func&&, Args&&...>>
    WJR_NODISCARD std::future<R> submit(Func&& func, Args&&... args);

    unsigned int get_threads_size() const;

    static size_t default_threads_size();

private:
public:
    void core_work();

    void create_all_threads(unsigned int core_threads_size);

    alignas(8) spin_mutex m_fast_task_mutex;
    alignas(8) circular_buffer<std::function<void()>> m_task_queue;
    alignas(8) bool m_valid = true;
    alignas(8) std::atomic_bool m_pause = false;

    alignas(16) std::atomic<size_t> m_real_tasks = 0;
    alignas(16) wjr::vector<std::thread> m_core_threads;
};

template<typename Func, typename...Args>
void thread_pool::push(Func&& func, Args&&...args) {
    std::function<void()> function = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
    {
        std::unique_lock slow_task_lock(m_fast_task_mutex);
        m_task_queue.push_back(std::move(function));
#if defined(_WJR_EXCEPTION)
        if (is_unlikely(!m_valid)) {
            throw std::runtime_error("can't put task into invalid thread_pool");
        }
#endif // _WJR_EXCEPTION
    }
    m_real_tasks.fetch_add(1, std::memory_order_relaxed);
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

_WJR_END

#endif // __WJR_NETWORK_THREAD_POOL_H
