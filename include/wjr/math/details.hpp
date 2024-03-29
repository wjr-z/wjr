#ifndef WJR_MATH_DETAILS_HPP__
#define WJR_MATH_DETAILS_HPP__

#include <wjr/memory/stack_allocator.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/details.hpp>
#endif

namespace wjr {

namespace math_details {

template <typename T, T seed>
class de_bruijn {
public:
    constexpr static uint8_t digits = std::numeric_limits<T>::digits;
    constexpr static uint8_t mv = digits == 32 ? 27 : 58;
    constexpr de_bruijn() : lookup(), lookupr() { initialize(); }

    WJR_INTRINSIC_CONSTEXPR int get(T idx) const { return lookup[(idx * seed) >> mv]; }
    WJR_INTRINSIC_CONSTEXPR int getr(T idx) const { return lookupr[(idx * seed) >> mv]; }

private:
    constexpr void initialize() {
        for (uint8_t i = 0; i < digits; ++i) {
            auto idx = (seed << i) >> mv;
            lookup[idx] = i;
            lookupr[idx] = i == 0 ? 0 : digits - i;
        }
    }

    uint8_t lookup[digits];
    uint8_t lookupr[digits];
};

inline constexpr de_bruijn<uint32_t, 0x077C'B531> de_bruijn32 = {};
inline constexpr de_bruijn<uint64_t, 0x03f7'9d71'b4ca'8b09> de_bruijn64 = {};

using stack_alloc_object = singleton_stack_allocator_object<16 * 1024, 36 * 1024>;
using unique_stack_alloc = unique_stack_allocator<stack_alloc_object>;
template <typename T>
using weak_stack_alloc = weak_stack_allocator<T, stack_alloc_object>;
inline constexpr stack_alloc_object stack_alloc = {};

} // namespace math_details

// preview ...

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool is_zero_or_single_bit(T n) noexcept {
    return (n & (n - 1)) == 0;
}

/**
 * @brief
 *
 * @note `n & -n` is the lowest bit of n.
 */
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T lowbit(T n) noexcept {
    return n & -n;
}

// preview :

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool __has_high_bit(T n) noexcept {
    return n >> (std::numeric_limits<T>::digits - 1);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T __align_down(T n, type_identity_t<T> alignment) {
    WJR_ASSERT_ASSUME_L1(is_zero_or_single_bit(alignment));
    return n & (-alignment);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T __align_up(T n, type_identity_t<T> alignment) {
    WJR_ASSERT_ASSUME_L1(is_zero_or_single_bit(alignment));
    return (n + alignment - 1) & (-alignment);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T __align_up_offset(T n, type_identity_t<T> alignment) {
    WJR_ASSERT_ASSUME_L1(is_zero_or_single_bit(alignment));
    return (-n) & (alignment - 1);
}

template <typename T, typename U = std::make_unsigned_t<T>,
          std::enable_if_t<std::is_integral_v<T>, int> = 0>
WJR_CONST constexpr U __fasts_sign_mask() {
    return (U)(1) << (std::numeric_limits<U>::digits - 1);
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr T __fasts_get_sign_mask(T x) {
    return x & __fasts_sign_mask<T>();
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr bool __fasts_is_negative(T x) {
    return __fasts_get_sign_mask<T>(x) != 0;
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr bool __fasts_is_positive(T x) {
    return __fasts_get_sign_mask<T>(x) == 0;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST constexpr std::make_signed_t<T> __fasts_from_unsigned(T x) {
    WJR_ASSERT_ASSUME_L1(!(x & __fasts_sign_mask<T>()));
    std::make_signed_t<T> ret = x;
    WJR_ASSERT_ASSUME_L1(__fasts_is_positive(ret));
    return ret;
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr T __fasts_abs(T x) {
    return x & ~__fasts_sign_mask<T>();
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr T __fasts_negate(T x) {
    return x ^ __fasts_sign_mask<T>();
}

template <typename T, typename U = std::make_unsigned_t<T>,
          std::enable_if_t<std::is_integral_v<T>, int> = 0>
WJR_CONST constexpr U __fasts_conditional_negate(bool condition, T x) {
    return (U)x ^ ((U)(condition) << (std::numeric_limits<U>::digits - 1));
}

} // namespace wjr

#endif // WJR_MATH_DETAILS_HPP__