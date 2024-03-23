#ifndef WJR_MEMORY_TO_ADDRESS_HPP__
#define WJR_MEMORY_TO_ADDRESS_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

WJR_REGISTER_HAS_TYPE(
    to_address,
    typename std::pointer_traits<Ptr>::to_address(std::declval<const Ptr &>()), Ptr);

template <typename T>
constexpr T *to_address(T *p) noexcept {
    static_assert(!std::is_function_v<T>, "T cannot be a function.");
    return p;
}

template <typename Ptr>
constexpr auto to_address(const Ptr &p) noexcept {
    if constexpr (has_to_address_v<Ptr>) {
        return std::pointer_traits<Ptr>::to_address(p);
    } else {
        return to_address(p.operator->());
    }
}

} // namespace wjr

#endif // WJR_MEMORY_TO_ADDRESS_HPP__