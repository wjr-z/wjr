/**
 * @file container/container_of.hpp
 * @brief container_of utility for intrusive containers
 * @author wjr
 *
 * Provides container_of functionality similar to Linux kernel's container_of macro,
 * allowing retrieval of container object from member pointer.
 */

#ifndef WJR_CONTAINER_CONTAINER_OF_HPP__
#define WJR_CONTAINER_CONTAINER_OF_HPP__

#include <cstddef>

namespace wjr {

/**
 * @brief Functor for container_of operation
 * @tparam Mybase Base member type
 * @tparam Tag Tag for disambiguation
 * @tparam Enable SFINAE enabler
 */
template <typename Mybase, typename Tag = void, typename Enable = void>
struct container_of_fn;

/// container_of function object
template <typename Mybase, typename Tag = void, typename Enable = void>
inline container_of_fn<Mybase, Tag, Enable> container_of;

} // namespace wjr

#endif // WJR_CONTAINER_CONTAINER_OF_HPP__