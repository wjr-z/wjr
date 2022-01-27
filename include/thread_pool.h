//Copyright(c) 2012 Jakob Progsch, V¨¢clav Zeman
//
//This software is provided 'as-is', without any express or implied
//warranty.In no event will the authors be held liable for any damages
//arising from the use of this software.
//
//Permission is granted to anyone to use this software for any purpose,
//including commercial applications, and to alter itand redistribute it
//freely, subject to the following restrictions :
//
//1. The origin of this software must not be misrepresented; you must not
//claim that you wrote the original software.If you use this software
//in a product, an acknowledgment in the product documentation would be
//appreciated but is not required.
//
//2. Altered source versions must be plainly marked as such, and must not be
//misrepresented as being the original software.
//
//3. This notice may not be removed or altered from any source
//distribution.


#ifndef __WJR_THREAD_POOL_H
#define __WJR_THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>
#include <thread>
#include <functional>
#include <stdexcept>
#include <iostream>
#include "mallocator.h"

namespace wjr {

    template<typename Alloc>
    class basic_thread_pool {
    public:
        basic_thread_pool(size_t);
        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args)
            ->std::future<wjr_result_of_t<F,Args...>>;
        ~basic_thread_pool();
    private:
        using func = std::function<void()>;
        // need to keep track of threads so we can join them
        std::vector< std::thread > workers;
        // the task queue
        std::queue<func,std::deque<func,typename Alloc::template rebind<func>::other>> tasks;

        // synchronization
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
    };

    // the constructor just launches some amount of workers
    template<typename Alloc>
    inline basic_thread_pool<Alloc>::basic_thread_pool(size_t threads)
        : stop(false) {
        workers.reserve(threads);
        for (size_t i = 0; i < threads; ++i)
            workers.emplace_back(
                [this] {
                    std::function<void()> task;
                    for (;;) {

                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock,
                                [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                }
        );
    }

    // add new work item to the pool
    template<typename Alloc>
    template<class F, class... Args>
    auto basic_thread_pool<Alloc>::enqueue(F&& f, Args&&... args)
        -> std::future<wjr_result_of_t<F,Args...>> {
        using return_type = wjr_result_of_t<F, Args...>;
        using allocator_type = typename Alloc::template rebind<std::packaged_task<return_type()>>::other;

        auto task = std::allocate_shared< std::packaged_task<return_type()> >(
            allocator_type(),
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped basic_thread_pool");

            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    // the destructor joins all threads
    template<typename Alloc>
    inline basic_thread_pool<Alloc>::~basic_thread_pool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers)
            worker.join();
    }

    using thread_pool = basic_thread_pool<mallocator<int>>;

}

#endif