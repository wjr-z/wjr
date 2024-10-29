#ifndef WJR_CRTP_TRIVIALLY_ALLOCATOR_BASE_HPP__
#define WJR_CRTP_TRIVIALLY_ALLOCATOR_BASE_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

WJR_REGISTER_HAS_TYPE(is_trivially_allocator,
                      std::declval<typename Alloc::is_trivially_allocator>(), Alloc);

WJR_REGISTER_HAS_TYPE(is_non_trivially_allocator_construct,
                      std::declval<Alloc>().construct(std::declval<Obj *>(),
                                                      std::declval<Args>()...),
                      Alloc, Obj);

WJR_REGISTER_HAS_TYPE(is_non_trivially_allocator_destroy,
                      std::declval<Alloc>().destroy(std::declval<Obj *>()), Alloc, Obj);

/// @private
template <typename Alloc, typename = void>
struct __is_trivially_allocator_impl : std::false_type {};

/// @private
template <typename Alloc>
struct __is_trivially_allocator_impl<Alloc, std::enable_if_t<has_is_trivially_allocator_v<Alloc>>>
    : Alloc::is_trivially_allocator {};

/**
 * @brief Default construct, destruct allocator.
 *
 * @details If `Alloc::is_trivially_allocator` is not defined or
 * `Alloc::is_trivially_allocator` is `std::false_type`, derive from `std::false_type`. \n
 * If is_trivially_allocator_v is true, then `construct_at_using_allocator` and
 * `destroy_at_using_allocator` are same as `construct_at` and `destroy_at`.
 *
 */
template <typename Alloc>
struct is_trivially_allocator : __is_trivially_allocator_impl<Alloc> {};

template <typename T>
struct is_trivially_allocator<std::allocator<T>> : std::true_type {};

template <typename Alloc>
inline constexpr bool is_trivially_allocator_v = is_trivially_allocator<Alloc>::value;

/// @private
template <typename Enable, typename Alloc, typename Obj, typename... Args>
struct __is_trivially_allocator_construct_impl : std::false_type {};

/// @private
template <typename Alloc, typename Obj, typename... Args>
struct __is_trivially_allocator_construct_impl<
    std::enable_if_t<!has_is_non_trivially_allocator_construct_v<Alloc, Obj, Args...>>, Alloc, Obj,
    Args...> : std::true_type {};

template <typename Alloc, typename Obj, typename... Args>
struct is_trivially_allocator_construct
    : std::disjunction<__is_trivially_allocator_construct_impl<void, Alloc, Obj, Args...>,
                       is_trivially_allocator<Alloc>> {};

template <typename Alloc, typename Obj, typename... Args>
inline constexpr bool is_trivially_allocator_construct_v =
    is_trivially_allocator_construct<Alloc, Obj, Args...>::value;

/// @private
template <typename Alloc, typename Obj, typename = void>
struct __is_trivially_allocator_destroy_impl : std::false_type {};

/// @private
template <typename Alloc, typename Obj>
struct __is_trivially_allocator_destroy_impl<
    Alloc, Obj, std::enable_if_t<!has_is_non_trivially_allocator_destroy_v<Alloc, Obj>>>
    : std::true_type {};

template <typename Alloc, typename Obj>
struct is_trivially_allocator_destroy
    : std::disjunction<__is_trivially_allocator_destroy_impl<Alloc, Obj>,
                       is_trivially_allocator<Alloc>> {};

template <typename Alloc, typename Obj>
inline constexpr bool is_trivially_allocator_destroy_v =
    is_trivially_allocator_destroy<Alloc, Obj>::value;

} // namespace wjr

#endif // WJR_CRTP_TRIVIALLY_ALLOCATOR_BASE_HPP__