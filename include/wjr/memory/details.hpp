#ifndef WJR_MEMORY_TO_ADDRESS_HPP__
#define WJR_MEMORY_TO_ADDRESS_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename Ptr, typename = void>
struct __has_to_address_impl : std::false_type {};

template <typename Ptr>
struct __has_to_address_impl<
    Ptr, std::void_t<decltype(typename std::pointer_traits<Ptr>::to_address(
             std::declval<const Ptr &>()))>> : std::true_type {};

template <typename Ptr>
struct __has_to_address : __has_to_address_impl<remove_cvref_t<Ptr>, void> {};

template <typename T>
constexpr T *to_address(T *p) noexcept {
    static_assert(!std::is_function_v<T>, "T cannot be a function.");
    return p;
}

template <typename Ptr>
constexpr auto to_address(const Ptr &p) noexcept {
    if constexpr (__has_to_address<Ptr>::value) {
        return std::pointer_traits<Ptr>::to_address(p);
    } else {
        return to_address(p.operator->());
    }
}

} // namespace wjr

#endif // WJR_MEMORY_TO_ADDRESS_HPP__