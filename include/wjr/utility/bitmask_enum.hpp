/**
 * @file utility/bitmask_enum.hpp
 * @brief Bitmask operations for enum types
 * @author wjr
 *
 * Provides bitwise operators for enums marked as bitmask types.
 * Use WJR_DECLARE_ENUM_AS_BITMASK to enable bitmask operations for an enum.
 */

#ifndef WJR_UTILITY_BITMASK_ENUM_HPP__
#define WJR_UTILITY_BITMASK_ENUM_HPP__

#include <wjr/type_traits.hpp>

#define WJR_DECLARE_ENUM_AS_BITMASK(Enum)                                                          \
    template <>                                                                                    \
    struct is_bitmask_enum<Enum> : std::true_type {}

#define WJR_ENABLE_BITMASK_ENUMS_IN_NAMESPACE()                                                    \
    using ::wjr::bitmask_enum_detail::operator|;                                                   \
    using ::wjr::bitmask_enum_detail::operator&;                                                   \
    using ::wjr::bitmask_enum_detail::operator^;                                                   \
    using ::wjr::bitmask_enum_detail::operator<<;                                                  \
    using ::wjr::bitmask_enum_detail::operator>>;                                                  \
    using ::wjr::bitmask_enum_detail::operator|=;                                                  \
    using ::wjr::bitmask_enum_detail::operator&=;                                                  \
    using ::wjr::bitmask_enum_detail::operator^=;                                                  \
    using ::wjr::bitmask_enum_detail::operator<<=;                                                 \
    using ::wjr::bitmask_enum_detail::operator>>=;                                                 \
    using ::wjr::bitmask_enum_detail::operator!;                                                   \
    /* Force a semicolon at the end of this macro. */                                              \
    using ::wjr::bitmask_enum_detail::any

namespace wjr {

/// Traits class to determine whether an enum has a
/// WJR_BITMASK_LARGEST_ENUMERATOR enumerator.
template <typename E, typename Enable = void>
struct is_bitmask_enum : std::false_type {};

namespace bitmask_enum_detail {

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
constexpr bool operator!(E Val) {
    return Val == static_cast<E>(0);
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
constexpr bool any(E Val) {
    return Val != static_cast<E>(0);
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
constexpr E operator|(E LHS, E RHS) {
    return static_cast<E>(to_underlying(LHS) | to_underlying(RHS));
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
constexpr E operator&(E LHS, E RHS) {
    return static_cast<E>(to_underlying(LHS) & to_underlying(RHS));
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
constexpr E operator^(E LHS, E RHS) {
    return static_cast<E>(to_underlying(LHS) ^ to_underlying(RHS));
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
constexpr E operator<<(E LHS, E RHS) {
    return static_cast<E>(to_underlying(LHS) << to_underlying(RHS));
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
constexpr E operator>>(E LHS, E RHS) {
    return static_cast<E>(to_underlying(LHS) >> to_underlying(RHS));
}

// |=, &=, and ^= return a reference to LHS, to match the behavior of the
// operators on builtin types.

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
E &operator|=(E &LHS, E RHS) {
    LHS = LHS | RHS;
    return LHS;
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
E &operator&=(E &LHS, E RHS) {
    LHS = LHS & RHS;
    return LHS;
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
E &operator^=(E &LHS, E RHS) {
    LHS = LHS ^ RHS;
    return LHS;
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
E &operator<<=(E &lhs, E rhs) {
    lhs = lhs << rhs;
    return lhs;
}

template <typename E, typename = std::enable_if_t<is_bitmask_enum<E>::value>>
E &operator>>=(E &lhs, E rhs) {
    lhs = lhs >> rhs;
    return lhs;
}

} // namespace bitmask_enum_detail

// Enable bitmask enums in namespace ::wjr and all nested namespaces.
WJR_ENABLE_BITMASK_ENUMS_IN_NAMESPACE();

} // namespace wjr

#endif // WJR_UTILITY_BITMASK_ENUM_HPP__