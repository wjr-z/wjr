#ifndef WJR_CRTP_TRIVIAL_ALLOCATOR_BASE_HPP__
#define WJR_CRTP_TRIVIAL_ALLOCATOR_BASE_HPP__

#include <memory>
#include <type_traits>

namespace wjr {

struct trivial_allocator_base {};

template <typename T>
struct is_trivially_allocator : std::is_base_of<trivial_allocator_base, T> {};

template <typename T>
struct is_trivially_allocator<std::allocator<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_trivially_allocator_v = is_trivially_allocator<T>::value;

} // namespace wjr

#endif // WJR_CRTP_TRIVIAL_ALLOCATOR_BASE_HPP__