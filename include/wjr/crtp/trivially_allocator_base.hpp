#ifndef WJR_CRTP_TRIVIALLY_ALLOCATOR_BASE_HPP__
#define WJR_CRTP_TRIVIALLY_ALLOCATOR_BASE_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

WJR_REGISTER_HAS_TYPE(is_trivially_allocator,
                      std::declval<typename Alloc::is_trivially_allocator>(), Alloc);
WJR_REGISTER_HAS_TYPE(
    is_trivially_allocator_constructible,
    std::declval<typename Alloc::is_trivially_allocator_constructible>(), Alloc);
WJR_REGISTER_HAS_TYPE(is_trivially_allocator_destructible,
                      std::declval<typename Alloc::is_trivially_allocator_destructible>(),
                      Alloc);

/// @private
template <typename Alloc, typename = void>
struct __is_trivially_allocator_impl : std::false_type {};

/// @private
template <typename Alloc>
struct __is_trivially_allocator_impl<
    Alloc, std::enable_if_t<has_is_trivially_allocator_v<Alloc>>>
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
template <typename Alloc, typename = void>
struct __is_trivially_allocator_constructible_impl : std::false_type {};

/// @private
template <typename Alloc>
struct __is_trivially_allocator_constructible_impl<
    Alloc, std::enable_if_t<has_is_trivially_allocator_constructible_v<Alloc>>>
    : Alloc::is_trivially_allocator_constructible {};

template <typename Alloc>
struct is_trivially_allocator_constructible
    : std::disjunction<__is_trivially_allocator_constructible_impl<Alloc>,
                       is_trivially_allocator<Alloc>> {};

template <typename Alloc>
inline constexpr bool is_trivially_allocator_constructible_v =
    is_trivially_allocator_constructible<Alloc>::value;

/// @private
template <typename Alloc, typename = void>
struct __is_trivially_allocator_destructible_impl : std::false_type {};

/// @private
template <typename Alloc>
struct __is_trivially_allocator_destructible_impl<
    Alloc, std::enable_if_t<has_is_trivially_allocator_destructible_v<Alloc>>>
    : Alloc::is_trivially_allocator_destructible {};

template <typename Alloc>
struct is_trivially_allocator_destructible
    : std::disjunction<__is_trivially_allocator_destructible_impl<Alloc>,
                       is_trivially_allocator<Alloc>> {};

template <typename Alloc>
inline constexpr bool is_trivially_allocator_destructible_v =
    is_trivially_allocator_destructible<Alloc>::value;

template <typename Alloc>
struct trivially_allocator_traits {
    using is_trivially = is_trivially_allocator<Alloc>;
    using is_trivially_constructible = is_trivially_allocator_constructible<Alloc>;
    using is_trivially_destructible = is_trivially_allocator_destructible<Alloc>;
};

} // namespace wjr

#endif // WJR_CRTP_TRIVIALLY_ALLOCATOR_BASE_HPP__