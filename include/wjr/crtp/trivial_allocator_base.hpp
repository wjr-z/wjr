#ifndef WJR_CRTP_TRIVIAL_ALLOCATOR_BASE_HPP__
#define WJR_CRTP_TRIVIAL_ALLOCATOR_BASE_HPP__

#include <memory>
#include <type_traits>

namespace wjr {

struct trivial_allocator_base_t {};
struct trivial_allocator_constructible_t {};
struct trivial_allocator_destructible_t {};

template <typename T>
struct is_trivially_allocator : std::is_base_of<trivial_allocator_base_t, T> {};

template <typename T>
struct is_trivially_allocator<std::allocator<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_trivially_allocator_v = is_trivially_allocator<T>::value;

template <typename T>
struct is_trivially_allocator_constructible
    : std::disjunction<std::is_base_of<trivial_allocator_constructible_t, T>,
                       is_trivially_allocator<T>> {};

template <typename T>
inline constexpr bool is_trivially_allocator_constructible_v =
    is_trivially_allocator_constructible<T>::value;

template <typename T>
struct is_trivially_allocator_destructible
    : std::disjunction<std::is_base_of<trivial_allocator_destructible_t, T>,
                       is_trivially_allocator<T>> {};

template <typename T>
inline constexpr bool is_trivially_allocator_destructible_v =
    is_trivially_allocator_destructible<T>::value;

} // namespace wjr

#endif // WJR_CRTP_TRIVIAL_ALLOCATOR_BASE_HPP__