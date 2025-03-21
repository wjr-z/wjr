/**
 * @file memory_pool.hpp
 * @author wjr
 * @brief
 * @version 0.1
 * @date 2024-09-22
 * @todo Multithread memory pool.
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WJR_MEMORY_MEMORY_POOL_HPP__
#define WJR_MEMORY_MEMORY_POOL_HPP__

#include <memory>
#include <type_traits>

namespace wjr {

static_assert(std::is_empty_v<std::allocator<char>>);

} // namespace wjr

#endif // WJR_MEMORY_MEMORY_POOL_HPP__