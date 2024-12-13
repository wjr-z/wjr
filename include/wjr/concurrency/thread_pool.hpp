/**
 * @file thread_pool.hpp
 * @author your name (you@domain.com)
 * @brief
 * @todo
 * 1. C++ executor.
 * 2. Linux scheduler algorithm.
 * 3. Trace.
 * @version 0.1
 * @date 2024-12-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WJR_CONCURRENCY_THREAD_POOL_HPP__
#define WJR_CONCURRENCY_THREAD_POOL_HPP__

#include <wjr/container/list.hpp>

namespace wjr {
class task_struct : private intrusive::list_node<task_struct> {};
} // namespace wjr

#endif // WJR_CONCURRENCY_THREAD_POOL_HPP__