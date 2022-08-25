#ifndef __WJR_BIGINTEGER_H
#define __WJR_BIGINTEGER_H

#include <array>
#include <malloc.h>

#include "../generic/mallocator.h"
#include "../generic/mString.h"
#include "../generic/masm.h"
#include "../generic/mmath.h"

extern "C" {
    extern void cdft(int, int, double*, int*, double*);
}

namespace wjr::math::biginteger {

#if defined(__SIZEOF_INT128__) && !(defined(__clang__) && defined(_MSC_VER))
#define WJR_BIGINTEGER_USE_X64
#endif

    template<typename T>
    struct __Biginteger_is_unsigned : std::is_unsigned<T> {};
    template<typename T>
    constexpr bool __Biginteger_is_unsigned_v = __Biginteger_is_unsigned<T>::value;

    template<typename T>
    struct __Biginteger_is_singed : std::is_signed<T> {};
    template<typename T>
    constexpr bool __Biginteger_is_singed_v = __Biginteger_is_singed<T>::value;

    template<typename T>
    struct __Biginteger_is_integral : std::is_integral<T> {};
    template<typename T>
    constexpr bool __Biginteger_is_integral_v = __Biginteger_is_integral<T>::value;

    template<typename T>
    struct __Bigintger_is_unsigned_integral : std::conjunction<__Biginteger_is_unsigned<T>,
        __Biginteger_is_integral<T>> {};
    template<typename T>
    constexpr bool __Biginteger_is_unsigned_integral_v = __Bigintger_is_unsigned_integral<T>::value;

    template<typename T>
    struct __Bigintger_is_signed_integral : std::conjunction<__Biginteger_is_singed<T>,
        __Biginteger_is_integral<T>> {};
    template<typename T>
    constexpr bool __Biginteger_is_signed_integral_v = __Bigintger_is_signed_integral<T>::value;

#if defined WJR_BIGINTEGER_USE_X64
    using limb_t = uint64_t;
    using double_limb_t = __uint128_t;
    using random_device_type = std::mt19937_64;
    constexpr int limb_bits = 64;
    constexpr int limb_bytes = 8;

    template<>
    struct __Biginteger_is_unsigned<__uint128_t> : std::true_type {};
    template<>
    struct __Biginteger_is_integral<__uint128_t> : std::true_type {};

    template<>
    struct __Biginteger_is_singed<__int128_t> : std::true_type {};
    template<>
    struct __Biginteger_is_integral<__int128_t> : std::true_type {};

#else
    using limb_t = uint32_t;
    using double_limb_t = uint64_t;
    using random_device_type = std::mt19937;
    constexpr int limb_bits = 32;
    constexpr int limb_bytes = 4;
#endif

    using limb_ptr = limb_t*;
    using double_limb_ptr = double_limb_t*;
    using limb_const_ptr = const limb_t*;
    using double_limb_const_ptr = const double_limb_t*;
    using limb_size_t = size_t;

	inline random_device_type static_random_device = 
        random_device_type(std::random_device{}());

    template<size_t base, typename T>
    constexpr bool __cqis_power_of(T val) noexcept {
        while (val >= base) {
            val /= base;
        }
        return val == 0;
    }

    template<typename T>
    struct __get_undefined_max {
        constexpr static T value1 = std::numeric_limits<T>::max();
        constexpr static T value2 = static_cast<T>(static_cast<T>(0) - 1);
        constexpr static T value = std::max(value1, value2);
    };

#if defined(WJR_USE_LIBDIVIDE)
    template<typename T>
    struct biginteger_libdivider {};

    template<>
    struct biginteger_libdivider<uint32_t> {
       WJR_CONSTEXPR explicit biginteger_libdivider(uint32_t d) {
            WASSERT_LEVEL_2(d != 0);
            auto floor_log_2_d = math::integer_log2(d);

            // Power of 2
            if ((d & (d - 1)) == 0) {
                // We need to subtract 1 from the shift value in case of an unsigned
                // branchfree divider because there is a hardcoded right shift by 1
                // in its division algorithm. Because of this we also need to add back
                // 1 in its recovery algorithm.
                magic = 0;
                more = static_cast<uint8_t>(floor_log_2_d);
            }
            else {
                uint64_t proposed_m;
                uint32_t rem;
                // (1 << (64 + floor_log_2_d)) / d
                uint32_t _R;
                proposed_m = math::u64_div_32_to_32(
                    static_cast<uint32_t>(1u << floor_log_2_d), 0, d, &_R);
                proposed_m <<= 32;
                proposed_m |= math::u64_div_32_to_32(_R, 0, d, &rem);
                WASSERT_LEVEL_2(rem > 0 && rem < d);
                const uint32_t e = d - rem;
                // This power works if e < 2**floor_log_2_d.
                if (e < (static_cast<uint32_t>(1 << floor_log_2_d))) {
                    // This power works
                    more = static_cast<uint8_t>(floor_log_2_d);
                }
                else {
                    // We have to use the general 65-bit algorithm.  We need to compute
                    // (2**power) / d. However, we already have (2**(power-1))/d and
                    // its remainder. By doubling both, and then correcting the
                    // remainder, we can compute the larger division.
                    // don't care about overflow here - in fact, we expect it
                    proposed_m += proposed_m;
                    const uint64_t twice_rem = rem + rem;
                    if (twice_rem >= d || twice_rem < rem) proposed_m += 1;
                    more = (uint8_t)(floor_log_2_d | libdivide::LIBDIVIDE_ADD_MARKER);
                }
                magic = 1 + proposed_m;
                // result.more's shift should in general be ceil_log_2_d. But if we
                // used the smaller power, we subtract one from the shift because we're
                // using the smaller power. If we're using the larger power, we
                // subtract one from the shift because it's taken care of by the add
                // indicator. So floor_log_2_d happens to be correct in both cases,
                // which is why we do it outside of the if statement.
            }
        }
        //private:
        uint64_t magic;
        uint8_t more;
    };

    WJR_CONSTEXPR uint64_t operator/(uint64_t n, const biginteger_libdivider<uint32_t>& d) {
        uint8_t more = d.more;
        if (!d.magic) {
            return n >> more;
        }
        else {
            uint64_t q = math::mulhi_u64(d.magic, n);
            if (more & libdivide::LIBDIVIDE_ADD_MARKER) {
                uint64_t t = ((n - q) >> 1) + q;
                return t >> (more & libdivide::LIBDIVIDE_64_SHIFT_MASK);
            }
            else {
                // All upper bits are 0,
                // don't need to mask them off.
                return q >> more;
            }
        }
    }

#if defined (WJR_BIGINTEGER_USE_X64)
    template<>
    struct biginteger_libdivider<uint64_t> {
        WJR_CONSTEXPR explicit biginteger_libdivider(uint64_t d) {
            WASSERT_LEVEL_2(d != 0);
            auto floor_log_2_d = math::integer_log2(d);

            // Power of 2
            if ((d & (d - 1)) == 0) {
                // We need to subtract 1 from the shift value in case of an unsigned
                // branchfree divider because there is a hardcoded right shift by 1
                // in its division algorithm. Because of this we also need to add back
                // 1 in its recovery algorithm.
                magic = 0;
                more = (uint8_t)(floor_log_2_d);
            }
            else {
                double_limb_t proposed_m;
                uint64_t rem;
                // (1 << (128 + floor_log_2_d)) / d
                uint64_t _R;
                proposed_m = math::u128_div_64_to_64(
                    static_cast<uint64_t>(1ull << floor_log_2_d), 0, d, &_R);
                proposed_m <<= 64;
                proposed_m |= math::u128_div_64_to_64(_R, 0, d, &rem);
                WASSERT_LEVEL_2(rem > 0 && rem < d);
                const uint64_t e = d - rem;
                // This power works if e < 2**floor_log_2_d.
                if (e < (static_cast<uint64_t>(1ull << floor_log_2_d))) {
                    // This power works
                    more = (uint8_t)floor_log_2_d;
                }
                else {
                    // We have to use the general 129-bit algorithm.  We need to compute
                    // (2**power) / d. However, we already have (2**(power-1))/d and
                    // its remainder. By doubling both, and then correcting the
                    // remainder, we can compute the larger division.
                    // don't care about overflow here - in fact, we expect it
                    proposed_m += proposed_m;
                    const double_limb_t twice_rem = rem + rem;
                    if (twice_rem >= d || twice_rem < rem) proposed_m += 1;
                    more = (uint8_t)(floor_log_2_d | libdivide::LIBDIVIDE_ADD_MARKER);
                }
                magic = 1 + proposed_m;
                // result.more's shift should in general be ceil_log_2_d. But if we
                // used the smaller power, we subtract one from the shift because we're
                // using the smaller power. If we're using the larger power, we
                // subtract one from the shift because it's taken care of by the add
                // indicator. So floor_log_2_d happens to be correct in both cases,
                // which is why we do it outside of the if statement.
            }
        }
        //private:
        double_limb_t magic;
        uint8_t more;
    };

    WJR_CONSTEXPR double_limb_t
        biginteger_libdivide_mullhi_u128(
            double_limb_t x,
            double_limb_t y) {
        constexpr static uint64_t mask = (uint64_t)(-1);
        uint64_t x0 = (uint64_t)(x & mask);
        uint64_t x1 = (uint64_t)(x >> 64);
        uint64_t y0 = (uint64_t)(y & mask);
        uint64_t y1 = (uint64_t)(y >> 64);

        uint64_t carry = math::mulhi_u64(x0, y0);
        __uint128_t m = (__uint128_t)x0 * y1 + carry;
        uint64_t high1 = m >> 64;

        uint64_t m_lo = m;
        uint64_t high2 = ((__uint128_t)x1 * y0 + m_lo) >> 64;
        return (__uint128_t)x1 * y1 + high1 + high2;
    }

    WJR_CONSTEXPR double_limb_t operator/(
        double_limb_t n,
        const biginteger_libdivider<uint64_t>& d) {
        uint8_t more = d.more;
        if (!d.magic) {
            return n >> more;
        }
        else {
            double_limb_t q =
                biginteger_libdivide_mullhi_u128(d.magic, n);
            if (more & libdivide::LIBDIVIDE_ADD_MARKER) {
                double_limb_t t = ((n - q) >> 1) + q;
                return t >> (more & libdivide::LIBDIVIDE_64_SHIFT_MASK);
            }
            else {
                // All upper bits are 0,
                // don't need to mask them off.
                return q >> more;
            }
        }
    }

    struct force_biginteger_libdivider {
        constexpr explicit force_biginteger_libdivider(uint64_t d) : magic(0), more(0) {
            auto floor_log_2_d = math::force_constexpr::integer_log2(d);

            // Power of 2
            if ((d & (d - 1)) == 0) {
                // We need to subtract 1 from the shift value in case of an unsigned
                // branchfree divider because there is a hardcoded right shift by 1
                // in its division algorithm. Because of this we also need to add back
                // 1 in its recovery algorithm.
                magic = 0;
                more = (uint8_t)(floor_log_2_d);
            }
            else {
                double_limb_t proposed_m = 0;
                uint64_t rem = 0;
                // (1 << (128 + floor_log_2_d)) / d
                uint64_t _R = 0;
                proposed_m = math::force_constexpr::u128_div_64_to_64(
                    static_cast<uint64_t>(1ull << floor_log_2_d), 0, d, &_R);
                proposed_m <<= 64;
                proposed_m |= math::force_constexpr::u128_div_64_to_64(_R, 0, d, &rem);
                WASSERT_LEVEL_2(rem > 0 && rem < d);
                const uint64_t e = d - rem;
                // This power works if e < 2**floor_log_2_d.
                if (e < (static_cast<uint64_t>(1ull << floor_log_2_d))) {
                    // This power works
                    more = (uint8_t)floor_log_2_d;
                }
                else {
                    // We have to use the general 129-bit algorithm.  We need to compute
                    // (2**power) / d. However, we already have (2**(power-1))/d and
                    // its remainder. By doubling both, and then correcting the
                    // remainder, we can compute the larger division.
                    // don't care about overflow here - in fact, we expect it
                    proposed_m += proposed_m;
                    const double_limb_t twice_rem = rem + rem;
                    if (twice_rem >= d || twice_rem < rem) proposed_m += 1;
                    more = (uint8_t)(floor_log_2_d | libdivide::LIBDIVIDE_ADD_MARKER);
                }
                magic = 1 + proposed_m;
                // result.more's shift should in general be ceil_log_2_d. But if we
                // used the smaller power, we subtract one from the shift because we're
                // using the smaller power. If we're using the larger power, we
                // subtract one from the shift because it's taken care of by the add
                // indicator. So floor_log_2_d happens to be correct in both cases,
                // which is why we do it outside of the if statement.
            }
        }
        //private:
        double_limb_t magic;
        uint8_t more;
    };

    constexpr double_limb_t operator/(
        double_limb_t n,
        const force_biginteger_libdivider& d) {
        uint8_t more = d.more;
        if (!d.magic) {
            return n >> more;
        }
        else {
            double_limb_t q =
                biginteger_libdivide_mullhi_u128(d.magic, n);
            if (more & libdivide::LIBDIVIDE_ADD_MARKER) {
                double_limb_t t = ((n - q) >> 1) + q;
                return t >> (more & libdivide::LIBDIVIDE_64_SHIFT_MASK);
            }
            else {
                // All upper bits are 0,
                // don't need to mask them off.
                return q >> more;
            }
        }
    }

#endif
#else
    template<typename T>
    using biginteger_libdivider = T;
#endif

    template<size_t base>
    struct biginteger_traits {
        static_assert(base != 0, "base must be non-zero");

        constexpr static bool has_single_bit = math::force_constexpr::has_single_bit(base);
        constexpr static int integer_log2 =
            math::force_constexpr::integer_log(std::integral_constant<size_t, 2>{}, base);

        constexpr static int __get_bits() {
            if constexpr (has_single_bit && (limb_bits % integer_log2 == 0)) {
                return limb_bits / integer_log2;
            }
            else {
                return math::force_constexpr::integer_log(
                    std::integral_constant<size_t, base>{}, std::numeric_limits<limb_t>::max());
            }
        }

        constexpr static int bits = __get_bits();
        constexpr static limb_t max() {
            constexpr limb_t __Max = math::force_constexpr::integer_power(std::integral_constant<limb_t, base>{}, bits) - 1;
            return __Max;
        }
        constexpr static limb_t mod() {
            constexpr limb_t __Mod = max() + 1;
            return __Mod;
        }
        constexpr static double_limb_t max_value() {
            constexpr double_limb_t __Max_value = static_cast<double_limb_t>(max()) + 1;
            return __Max_value;
        }

        template<typename T>
        constexpr static int __get_bits_of_t() {
            auto v = __get_undefined_max<T>::value;
            int result = 1;
            while (v >= max_value()) {
                v /= max_value();
                ++result;
            }
            return result;
        }

        template<typename T>
        constexpr static int bits_of_t = __get_bits_of_t<T>();

        constexpr static bool is_full() {
            constexpr bool __Is_full = max() == std::numeric_limits<limb_t>::max();
            return __Is_full;
        }

        template<typename T, std::enable_if_t<__Biginteger_is_unsigned_integral_v<T>, int> = 0>
        constexpr static T get_high(T x) {
            if constexpr (__get_undefined_max<T>::value <= max()) {
                return 0;
            }
            else {
                if constexpr (is_full()) {
                    x >>= (limb_bits - 1);
                    return x >> 1;
                }
                else {
#if defined(WJR_BIGINTEGER_USE_X64)
                    if constexpr (std::is_same_v<T, double_limb_t>) {
                        constexpr force_biginteger_libdivider div(mod());
                        return static_cast<T>(x / div);
                    }
                    else {
#endif
                        return x / mod();
#if defined(WJR_BIGINTEGER_USE_X64)
                    }
#endif
                }
            }
        }

        template<typename T, std::enable_if_t<__Biginteger_is_unsigned_integral_v<T>, int> = 0>
        constexpr static limb_t get_low(T x) {
            if constexpr (__get_undefined_max<T>::value <= max() || is_full()) {
                return static_cast<limb_t>(x);
            }
            else {
#if defined(WJR_BIGINTEGER_USE_X64)
                if constexpr (std::is_same_v<T, double_limb_t>) {
                    return x - get_high(x) * mod();
                }
                else {
#endif
                    return x % mod();
#if defined(WJR_BIGINTEGER_USE_X64)
            }
#endif
            }
        }

        constexpr static double_limb_t mul(limb_t a, limb_t b) {
            return static_cast<double_limb_t>(a) * b;
        }

        WJR_CONSTEXPR static unsigned char
            add(
                limb_ptr result,
                limb_t lhs,
                limb_t rhs
            ) {
            if constexpr (is_full()) {
                return math::add(result, lhs, rhs);
            }
            else {
                lhs += rhs;
                unsigned char cf = lhs < rhs;
                cf |= mod() <= lhs;
                *result = lhs - cf * mod();
                return cf;
            }
        }

        template<typename C = tag::empty_tag, std::enable_if_t<tag::is_tag_set_v<C>, int> = 0>
        WJR_CONSTEXPR static unsigned char
            inc(
                unsigned char cf,
                limb_ptr result,
                limb_const_ptr lp,
                size_t n,
                C = C{}
            ) {
            size_t i = 0;
            if (cf) {
                for (;;) {
                    if (i == n) {
                        return 1;
                    }
                    if (lp[i] != max()) {
                        result[i] = lp[i] + 1;
                        ++i;
                        break;
                    }
                    result[i] = 0;
                    ++i;
                }
            }
            if constexpr (!C{}(tag::same_address)) {
                if (likely(lp != result)) {
                    std::copy(lp + i, lp + n, result + i);
                }
            }
            else {
                WASSERT_LEVEL_1(lp == result);
            }
            return 0;
        }

        template<typename C = tag::empty_tag, std::enable_if_t<tag::is_tag_set_v<C>, int> = 0>
        WJR_CONSTEXPR static unsigned char
            inc(
                limb_ptr result,
                limb_const_ptr lp,
                size_t n,
                C = C{}
            ) {
            return inc(1, result, lp, n, C{});
        }

        WJR_CONSTEXPR static unsigned char
            adc(
                unsigned char cf,
                limb_ptr result,
                limb_t lhs,
                limb_t rhs
            ) {
            if constexpr (is_full()) {
                return math::adc(cf, result, lhs, rhs);
            }
            else {
                lhs += cf;
                lhs += rhs;
                cf = lhs < rhs;
                cf |= mod() <= lhs;
                *result = lhs - cf * mod();
                return cf;
            }
        }

        WJR_CONSTEXPR static unsigned char
            add(
                limb_ptr result,
                limb_const_ptr lhs,
                limb_const_ptr rhs,
                size_t n
            ) {
            if constexpr (is_full()) {
                return math::add(result, lhs, rhs, n);
            }
            else {
                unsigned char cf = 0;
                size_t r = n & 3;
                for (size_t i = 0; i < r; ++i) {
                    cf = adc(cf, result + i, lhs[i], rhs[i]);
                }
                for (size_t i = r; i < n; i += 4) {
                    cf = adc(cf, result + i + 0, lhs[i + 0], rhs[i + 0]);
                    cf = adc(cf, result + i + 1, lhs[i + 1], rhs[i + 1]);
                    cf = adc(cf, result + i + 2, lhs[i + 2], rhs[i + 2]);
                    cf = adc(cf, result + i + 3, lhs[i + 3], rhs[i + 3]);
                }
                return cf;
            }
        }

        template<typename C = tag::empty_tag, std::enable_if_t<tag::is_tag_set_v<C>, int> = 0>
        WJR_CONSTEXPR static unsigned char
            add(
                limb_ptr result,
                limb_const_ptr lp,
                size_t n,
                limb_const_ptr rp,
                size_t m,
                C = C{}
            ) {
            WASSERT_LEVEL_1(n >= m);
            unsigned char cf = add(result, lp, rp, m);
            return inc(cf, result + m, lp + m, n - m, C{});
        }

        WJR_CONSTEXPR static unsigned char
            sub(
                limb_ptr result,
                limb_t lhs,
                limb_t rhs
            ) {

            if constexpr (is_full()) {
                return math::sub(result, lhs, rhs);
            }
            else {
                unsigned char cf = math::sub(result, lhs, rhs);
                *result += cf * mod();
                return cf;
            }
        }

        template<typename C = tag::empty_tag, std::enable_if_t<tag::is_tag_set_v<C>, int> = 0>
        WJR_CONSTEXPR static unsigned char
            dec(
                unsigned char cf,
                limb_ptr result,
                limb_const_ptr lp,
                size_t n,
                C = C{}
            ) {
            size_t i = 0;
            if (cf) {
                for (; i != n && lp[i] == 0; ++i) {
                    result[i] = max();
                }
                if (i == n)return 1;
                result[i] = lp[i] - 1;
                ++i;
            }
            if constexpr (!C{}(tag::same_address)) {
                if (likely(lp != result)) {
                    std::copy(lp + i, lp + n, result + i);
                }
            }
            else {
                WASSERT_LEVEL_1(lp == result);
            }
            return 0;
        }

        template<typename C = tag::empty_tag, std::enable_if_t<tag::is_tag_set_v<C>, int> = 0>
        WJR_CONSTEXPR static unsigned char
            dec(
                limb_ptr result,
                limb_const_ptr lp,
                size_t n,
                C = C{}
            ) {
            return dec(1, result, lp, n, C{});
        }

        WJR_CONSTEXPR static unsigned char
            sbb(
                unsigned char cf,
                limb_ptr result,
                limb_t lhs,
                limb_t rhs
            ) {
            if constexpr (is_full()) {
                return math::sbb(cf, result, lhs, rhs);
            }
            else {
                *result = lhs - rhs - cf;
                cf = lhs < *result;
                *result += cf * mod();
                return cf;
            }
        }

        WJR_CONSTEXPR static unsigned char
            sub(
                limb_ptr result,
                limb_const_ptr lhs,
                limb_const_ptr rhs,
                size_t n
            ) {
            if constexpr (is_full()) {
                return math::sub(result, lhs, rhs, n);
            }
            else {
                unsigned char cf = 0;
                size_t r = n & 3;
                for (size_t i = 0; i < r; ++i) {
                    cf = sbb(cf, result + i, lhs[i], rhs[i]);
                }
                for (size_t i = r; i < n; i += 4) {
                    cf = sbb(cf, result + i + 0, lhs[i + 0], rhs[i + 0]);
                    cf = sbb(cf, result + i + 1, lhs[i + 1], rhs[i + 1]);
                    cf = sbb(cf, result + i + 2, lhs[i + 2], rhs[i + 2]);
                    cf = sbb(cf, result + i + 3, lhs[i + 3], rhs[i + 3]);
                }
                return cf;
            }
        }

        WJR_CONSTEXPR static unsigned char
            sub(
                limb_ptr result,
                limb_const_ptr lp,
                size_t n,
                limb_const_ptr rp,
                size_t m
            ) {
            WASSERT_LEVEL_1(n >= m);
            unsigned char cf = sub(result, lp, rp, m);
            return dec(cf, result + m, lp + m, n - m);
        }

        WJR_CONSTEXPR static limb_t
            mul(
                limb_ptr result,
                limb_const_ptr lp,
                size_t n,
                limb_t rv
            ) {
            if constexpr (is_full()) {
                return math::vector_mul_1(result, lp, n, rv);
            }
            else {
                limb_t cf = 0;
#pragma GCC unroll(4)
                for (size_t i = 0; i < n; ++i) {
                    auto val = mul(lp[i], rv) + cf;
                    cf = get_high(val);
                    result[i] = get_low(val);
                }
                return cf;
            }
        }

        WJR_CONSTEXPR static limb_t
            addmul(
                limb_ptr result,
                limb_const_ptr lp,
                size_t n,
                limb_t rv
            ) {
            if constexpr (is_full()) {
                return math::vector_addmul_1(result, lp, n, rv);
            }
            else {
                limb_t cf = 0;
#pragma GCC unroll(4)
                for (size_t i = 0; i != n; ++i) {
                    auto val = mul(lp[i], rv) + result[i] + cf;
                    cf = get_high(val);
                    result[i] = get_low(val);
                }
                return cf;
            }
        }

        WJR_CONSTEXPR static void
            mul(
                limb_ptr result,
                limb_const_ptr lp,
                size_t ls,
                limb_const_ptr rp,
                size_t rs
            ) {
            WASSERT_LEVEL_1(rs >= 2);
            result[ls] = mul(result, lp, ls, rp[0]);
            size_t i = 1;
            do {
                result[ls + i] = addmul(result + i, lp, ls, rp[i]);
                ++i;
            } while (i != rs);
        }

        WJR_CONSTEXPR static limb_t
            div(
                limb_ptr result,
                limb_const_ptr lp,
                size_t n,
                limb_t rv
            ) {
            WASSERT_LEVEL_1(rv != 0);
            size_t i = n - 1;
            double_limb_t _Val = 0;

            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                if (n >= 4) {
                    biginteger_libdivider<limb_t> _Divider(rv);

                    limb_t _First = lp[i];
                    result[i] = static_cast<limb_t>(lp[i] / _Divider);
                    _Val = _First - mul(result[i], rv);

                    for (; i--;) {
                        _Val = _Val * max_value() + lp[i];
                        result[i] = static_cast<limb_t>(_Val / _Divider);
                        _Val = _Val - mul(result[i], rv);
                    }
                    return static_cast<limb_t>(_Val);
                }
            WJR_IS_NOT_CONSTANT_EVALUATED_END

                limb_t _First = lp[i];
            result[i] = static_cast<limb_t>(lp[i] / rv);
            _Val = _First - mul(result[i], rv);
            for (; i--;) {
                _Val = _Val * max_value() + lp[i];
                result[i] = static_cast<limb_t>(_Val / rv);
                _Val = _Val - mul(result[i], rv);
            }

            return static_cast<limb_t>(_Val);
        }

        WJR_CONSTEXPR static limb_t
            div(
                limb_ptr result,
                size_t n,
                limb_t rv
            ) {
            return div(result, result, n, rv);
        }

        WJR_CONSTEXPR static limb_t
            mod(
                limb_const_ptr lp,
                size_t n,
                limb_t rv
            ) {
            WASSERT_LEVEL_1(rv != 0);
            double_limb_t _Val = 0;

            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                if (n >= 8) {
                    biginteger_libdivider<limb_t> _Divider(rv);
                    for (size_t i = n; i--;) {
                        _Val = _Val * max_value() + lp[i];
                        _Val -= mul(static_cast<limb_t>(_Val / _Divider), rv);
                    }
                    return static_cast<limb_t>(_Val);
                }
            WJR_IS_NOT_CONSTANT_EVALUATED_END

                for (size_t i = n; i--;) {
                    _Val = _Val * max_value() + lp[i];
                    _Val -= mul(static_cast<limb_t>(_Val / rv), rv);
                }

            return static_cast<limb_t>(_Val);
        }

        WJR_CONSTEXPR static random_device_type& random_device() {
            return static_random_device;
        }

        WJR_CONSTEXPR static limb_t random() {
            std::uniform_int_distribution<limb_t> rt(0, max());
            return rt(random_device());
        }

        WJR_CONSTEXPR static bool random_bool() {
            return random_device()() & 1;
        }

        WJR_CONSTEXPR static int bit_size(limb_t v) {
            if constexpr (has_single_bit) {
                return (math::integer_log2(v) / integer_log2) + 1;
            }
            else {
                return math::force_constexpr::integer_log(std::integral_constant<limb_t, base>{}, v) + 1;
            }
        }

        struct __constexpr_base_power_array {
            constexpr __constexpr_base_power_array() : arr() {
                arr[0] = 1;
                for (size_t i = 1; i < bits; ++i) {
                    arr[i] = arr[i - 1] * base;
                }
            }
            constexpr limb_t operator[](size_t i) const {
                return arr[i];
            }
        private:
            std::array<limb_t, bits> arr;
        };

        constexpr static auto
            __base_power_array = __constexpr_base_power_array();

        constexpr static limb_t get_base_power(size_t kth) noexcept {
            WASSERT_LEVEL_2(kth < bits);
            return __base_power_array[kth];
        }
        constexpr static limb_t mul_base_power(limb_t val, size_t kth) noexcept {
            WASSERT_LEVEL_2(kth < bits);
            if constexpr (has_single_bit) {
                return val << (integer_log2 * kth);
            }
            else {
                return val * get_base_power(kth);
            }
        }
        struct __magic_divider_of_base_power {
            using divider_type = libdivide::divider<limb_t>;
            __magic_divider_of_base_power() {
                for (size_t i = 0; i < bits; ++i) {
                    new (&magic_divider[i]) divider_type(get_base_power(i));
                }
            }
            constexpr const divider_type& operator[](size_t i) const {
                return magic_divider[i];
            }
        private:
            std::array<divider_type, bits> magic_divider;
        };
        static __magic_divider_of_base_power __magic_divider;

        WJR_CONSTEXPR static limb_t div_base_power(limb_t val, size_t kth) noexcept {
            WASSERT_LEVEL_2(kth < bits);
            if constexpr (has_single_bit) {
                return val >> (integer_log2 * kth);
            }
            else {
                WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                    return val / __magic_divider[kth];
                WJR_IS_NOT_CONSTANT_EVALUATED_END
                    return val / get_base_power(kth);
            }
        }
        WJR_CONSTEXPR static limb_t mod_base_power(limb_t val, size_t kth) noexcept {
            WASSERT_LEVEL_2(kth < bits);
            if constexpr (has_single_bit) {
                return val & ((static_cast<limb_t>(1) << (integer_log2 * kth)) - 1);
            }
            else {
                WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                    return val - div_base_power(val, kth) * get_base_power(kth);
                WJR_IS_NOT_CONSTANT_EVALUATED_END
                    return val % get_base_power(kth);
            }
        }

        struct _From_string {
            constexpr limb_t operator()(char ch) const {
                if constexpr (base <= 10) {
                    return ch - '0';
                }
                else {
                    WASSERT_LEVEL_2(base <= 36);
                    return get_digit(ch);
                }
            }
        };

        inline constexpr static _From_string from_string = _From_string();

        struct _To_string {
            constexpr char operator()(limb_t val)const {
                WASSERT_LEVEL_2(val >= 0 && val < base);
                if constexpr (base <= 10) {
                    return '0' + val;
                }
                else {
                    return val < 10 ? '0' + val :
                        'a' + (val - 10);
                }
            }
        };
        inline constexpr static _To_string to_string = _To_string();

        struct mul_threshold {
            size_t slow_threshold;
            size_t karatsuba_threshold;
            size_t toom_cook_3_threshold;
        };

        struct threshold {
            constexpr static mul_threshold get_mul_threshold() {
#if defined(WJR_BIGINTEGER_USE_X64)
                return {
                    60,
                    320,
                    0
                };
#else
                return {
                    48,
                    112,
                    0
                };
#endif
            }
        };

    };

    template<size_t base>
    typename biginteger_traits<base>::__magic_divider_of_base_power
        biginteger_traits<base>::__magic_divider = __magic_divider_of_base_power();

    template<size_t base>
    class unsigned_biginteger;

    template<size_t base>
    class biginteger;

    template<size_t base, size_t length>
    class unsigned_integral_view;

    template<size_t base>
    class unsigned_biginteger_view;

    template<size_t base, size_t length>
    class signed_integral_view;

    template<size_t base>
    class signed_biginteger_view;

    template<size_t base>
    class unsigned_biginteger_modifier;

    template<typename T>
    struct __Biginteger_base_info {
        constexpr static size_t value = __Biginteger_is_integral_v<T> ? 0 : -1;
    };

    template<size_t base, size_t length>
    struct __Biginteger_base_info<unsigned_integral_view<base, length>> {
        constexpr static size_t value = base;
    };

    template<size_t base>
    struct __Biginteger_base_info<unsigned_biginteger_view<base>> {
        constexpr static size_t value = base;
    };

    template<size_t base>
    struct __Biginteger_base_info<unsigned_biginteger_modifier<base>> {
		constexpr static size_t value = base;
    };

    template<size_t base>
    struct __Biginteger_base_info<unsigned_biginteger<base>> {
        constexpr static size_t value = base;
    };

    template<size_t base, size_t length>
    struct __Biginteger_base_info<signed_integral_view<base, length>> {
        constexpr static size_t value = base;
    };

    template<size_t base>
    struct __Biginteger_base_info<signed_biginteger_view<base>> {
        constexpr static size_t value = base;
    };

    template<size_t base>
    struct __Biginteger_base_info<biginteger<base>> {
        constexpr static size_t value = base;
    };

    template<typename...Args>
    struct __Merge_biginteger_base_info;

    template<typename T, typename... Args>
    struct __Merge_biginteger_base_info<T, Args...> {
    private:
        constexpr static size_t get() {
            constexpr size_t _Now = __Biginteger_base_info<T>::value;
            constexpr size_t _Next = __Merge_biginteger_base_info<Args...>::value;
            if constexpr (_Now == -1 || _Next == -1) {
                return -1;
            }
            else if constexpr (_Now == 0 || _Next == 0) {
                return _Now | _Next;
            }
            else if constexpr (_Now != _Next) {
                return -1;
            }
            else return _Now;
        }
    public:
        constexpr static size_t value = get();
    };

    template<typename T>
    struct __Merge_biginteger_base_info<T> {
        constexpr static size_t value = __Biginteger_base_info<T>::value;
    };

    template<typename...Args>
    constexpr size_t __biginteger_base = __Merge_biginteger_base_info<std::decay_t<Args>...>::value;

    template<size_t base>
    struct __is_accepted_base {
        constexpr static bool value = !(base == 0 || base == 1 || base == -1);
    };

    template<size_t base>
    constexpr bool __is_accepted_base_v = __is_accepted_base<base>::value;

    template<size_t base, typename T>
    struct __Is_unsigned_biginteger_view : std::false_type {};

    template<size_t base, size_t length>
    struct __Is_unsigned_biginteger_view<base, unsigned_integral_view<base, length>> : std::true_type {};

    template<size_t base>
    struct __Is_unsigned_biginteger_view<base, unsigned_biginteger_view<base>> : std::true_type {};

    template<size_t base, typename T>
    constexpr bool __Is_unsigned_biginteger_view_v = __Is_unsigned_biginteger_view<base, T>::value;

    template<size_t base, typename T>
    struct __Is_signed_biginteger_view : std::false_type {};

    template<size_t base, size_t length>
    struct __Is_signed_biginteger_view<base, signed_integral_view<base, length>> : std::true_type {};

    template<size_t base>
    struct __Is_signed_biginteger_view<base, signed_biginteger_view<base>> : std::true_type {};

    template<size_t base, typename T>
    constexpr bool __Is_signed_biginteger_view_v = __Is_signed_biginteger_view<base, T>::value;

    template<size_t base, size_t length>
    class unsigned_integral_view {
    public:
        static_assert(length <= 4, "length must be less than 4");
        using traits = biginteger_traits<base>;
        constexpr static size_t max_bit = length;
        constexpr static bool is_unsigned_biginteger = false;
    private:
        template<size_t index, typename T>
        constexpr void _Make(T val) noexcept {
            if constexpr (index == length - 1) {
                m_data[index] = val;
                m_size = index + 1;
                WASSERT_LEVEL_2(val <= traits::max());
            }
            else {
                m_data[index] = traits::get_low(val);
                val = traits::get_high(val);
                if (val) {
                    _Make<index + 1>(val);
                }
                else {
                    m_size = index + 1;
                }
            }
        }
    public:
        template<typename T, std::enable_if_t<__Biginteger_is_unsigned_integral_v<T>, int> = 0>
        constexpr explicit unsigned_integral_view(const T& val) noexcept {
            _Make<0>(val);
        }
        constexpr unsigned_integral_view(
            const unsigned_integral_view& other,
            size_t offset,
            size_t newsize
        ) noexcept : m_size(newsize) {
            WASSERT_LEVEL_1(offset + newsize <= other.m_size);
            for (size_t i = 0; i < newsize; ++i) {
                m_data[i] = other.m_data[i + offset];
            }
        }
        constexpr unsigned_integral_view(const unsigned_integral_view&) noexcept = default;
        constexpr unsigned_integral_view& operator=(const unsigned_integral_view&) noexcept = default;

        constexpr limb_const_ptr data() const {
            return m_data;
        }
        constexpr size_t size() const {
            if constexpr (length == 1) {
                return 1;
            }
            else {
                return m_size;
            }
        }
        constexpr bool test_byte(limb_t val) const {
            return size() == 1 && *data() == val;
        }
        constexpr unsigned_integral_view& maintain() noexcept {
            return *this;
        }
    private:
        size_t m_size;
        limb_t m_data[length];
    };

    template<size_t base>
    class unsigned_biginteger_view {
        template<size_t b>
        friend class unsigned_biginteger_modifier;
    public:
        using traits = biginteger_traits<base>;
        constexpr static size_t max_bit = -1;
        constexpr static bool is_unsigned_biginteger = true;
        WJR_CONSTEXPR20 unsigned_biginteger_view(
            const unsigned_biginteger<base>& it,
            size_t _offset,
            size_t _size
        ) noexcept : m_ptr(std::addressof(it)), m_offset(_offset), m_size(_size) {
            WASSERT_LEVEL_2(m_offset + m_size <= it.size());
        }
        WJR_CONSTEXPR20 unsigned_biginteger_view(
            const unsigned_biginteger_view& other,
            size_t offset,
            size_t newsize
        ) : m_ptr(other.m_ptr), m_offset(other.m_offset + offset), m_size(newsize) {
            WASSERT_LEVEL_2(m_offset + m_size <= m_ptr->size());
        }
        WJR_CONSTEXPR20 unsigned_biginteger_view(const unsigned_biginteger_view&) = default;
        WJR_CONSTEXPR20 unsigned_biginteger_view& operator=(const unsigned_biginteger_view&) = default;
        WJR_CONSTEXPR20 limb_const_ptr data() const {
            WASSERT_LEVEL_2(m_offset + m_size <= m_ptr->size());
            return m_ptr->data() + m_offset;
        }
        WJR_CONSTEXPR20 size_t size() const {
            WASSERT_LEVEL_2(m_offset + m_size <= m_ptr->size());
            return m_size;
        }
        WJR_CONSTEXPR20 bool test_byte(limb_t val) const {
            return size() == 1 && *data() == val;
        }
        WJR_CONSTEXPR20 unsigned_biginteger_view& maintain();

    private:
        const unsigned_biginteger<base>* m_ptr;
        size_t m_offset;
        size_t m_size;
    };

    template<size_t base>
    struct __unsigned_biginteger_view {
        using traits = biginteger_traits<base>;

        template<typename T, std::enable_if_t<__Biginteger_is_unsigned_integral_v<T>, int> = 0>
        constexpr decltype(auto) operator()(const T& val) const {
            return unsigned_integral_view<base, traits::template bits_of_t<T>>(val);
        }

        template<size_t length>
        constexpr decltype(auto) operator()(const unsigned_integral_view<base, length>& val) const {
            return val;
        }

        template<size_t length>
        constexpr decltype(auto) operator()(
            const unsigned_integral_view<base, length>& val,
            size_t offset,
            size_t newsize
            ) const {
            return unsigned_integral_view<base, length>(val, offset, newsize);
        }

        constexpr decltype(auto) operator()(const unsigned_biginteger<base>& val) const {
            return unsigned_biginteger_view<base>(val, 0, val.size());
        }

        constexpr decltype(auto) operator()(const unsigned_biginteger<base>& val, size_t _Offset, size_t _Size) const {
            return unsigned_biginteger_view<base>(val, _Offset, _Size);
        }

        constexpr unsigned_biginteger_view<base> operator()(const unsigned_biginteger<base>& val, size_t _Offset) const {
            size_t _Size = val.size();
            if (_Offset < _Size) {
                return unsigned_biginteger_view<base>(val, _Offset, _Size - _Offset);
            }
            else {
                return unsigned_biginteger_view<base>(val, 0, 0).maintain();
            }
        }

        constexpr decltype(auto) operator()(const unsigned_biginteger_view<base>& val) const {
            return val;
        }

        constexpr decltype(auto) operator()(
            const unsigned_biginteger_view<base>& val,
            size_t offset,
            size_t newsize
            ) const {
            return unsigned_biginteger_view<base>(val, offset, newsize);
        }

        constexpr decltype(auto) operator()(const unsigned_biginteger_modifier<base>& val) const {
            return unsigned_biginteger_view<base>(*val.m_ptr, val.m_offset, val.size());
        }

        constexpr decltype(auto) operator()(
            const unsigned_biginteger_modifier<base>& val, size_t _Offset, size_t _Size) const {
            return unsigned_biginteger_view<base>(*val.m_ptr, val.m_offset + _Offset, _Size);
        }

    };

    template<size_t base>
    inline constexpr __unsigned_biginteger_view<base> unsigned_view;

    template<typename _Enable, size_t base, typename...Args>
    struct __Can_make_unsigned_view_helper : std::false_type {};

    template<size_t base, typename...Args>
    struct __Can_make_unsigned_view_helper<
        std::void_t<decltype(std::declval<__unsigned_biginteger_view<base>>()(std::declval<Args>()...))>,
        base, Args...> : std::true_type {};

    template<size_t base, typename...Args>
    struct __Can_make_unsigned_view : std::conjunction<__Can_make_unsigned_view_helper<void, base, Args...>> {};

    template<size_t base, typename...Args>
    constexpr bool __Can_make_unsigned_view_v = __Can_make_unsigned_view<base, Args...>::value;

    template<size_t base, typename T>
    struct __Need_to_make_unsigned : std::conjunction<
        std::negation<__Is_unsigned_biginteger_view<base, T>>,
        __Can_make_unsigned_view<base, T>> {};

    template<size_t base, typename T>
    constexpr bool __Need_to_make_unsigned_v = __Need_to_make_unsigned<base, T>::value;

    template<size_t base, size_t length>
    class signed_integral_view : public unsigned_integral_view<base, length> {
        using _Base = unsigned_integral_view<base, length>;
    public:
        using traits = biginteger_traits<base>;
        constexpr static size_t max_bit = length;
        constexpr static bool is_unsigned_biginteger = false;

        template<typename T, std::enable_if_t<__Biginteger_is_unsigned_integral_v<T>, int> = 0>
        constexpr explicit signed_integral_view(const T& val) : _Base(val) {}
        template<typename T, std::enable_if_t<__Biginteger_is_signed_integral_v<T>, int> = 0>
        constexpr explicit signed_integral_view(const T& val)
            : m_signal(val >= 0),
            _Base(static_cast<std::make_unsigned_t<T>>(val >= 0 ? val : (0 - val))) {}
        constexpr signed_integral_view(bool _Signal, const unsigned_integral_view<base, length>& val)
            : m_signal(_Signal), _Base(val) {}
        constexpr signed_integral_view(
            const signed_integral_view& other,
            size_t offset,
            size_t newsize
        ) : _Base(other.get_unsigned(), offset, newsize), m_signal(other.m_signal) {}
        constexpr signed_integral_view(const signed_integral_view&) = default;
        constexpr signed_integral_view& operator=(const signed_integral_view&) = default;
        constexpr _Base& get_unsigned() { return *this; }
        constexpr const _Base& get_unsigned() const { return *this; }
        constexpr bool signal() const { return m_signal; }
        constexpr void maintain_signal() {
            m_signal = m_signal || _Base::test_byte(0);
        }
        constexpr signed_integral_view& maintain() {
            _Base::maintan();
            maintain_signal();
            return *this;
        }
        constexpr signed_integral_view& negate() {
            m_signal = (!m_signal) || _Base::test_byte(0);
            return *this;
        }
        constexpr signed_integral_view& abs() {
            m_signal = true;
            return *this;
        }
    private:
        bool m_signal = true;
    };

    template<size_t base>
    class signed_biginteger_view : public unsigned_biginteger_view<base> {
        using _Base = unsigned_biginteger_view<base>;
    public:
        using traits = biginteger_traits<base>;
        constexpr static size_t max_bit = -1;
        constexpr static bool is_unsigned_biginteger = true;
        WJR_CONSTEXPR20 signed_biginteger_view(
            const unsigned_biginteger<base>& it,
            size_t _offset,
            size_t _size
        ) : _Base(it, _offset, _size) {}
        WJR_CONSTEXPR20 signed_biginteger_view(
            const biginteger<base>& it,
            size_t _offset,
            size_t _size
        ) : _Base(it.get_unsigned(), _offset, _size), m_signal(it.signal()) {}
        WJR_CONSTEXPR20 signed_biginteger_view(
            bool _Signal,
            const unsigned_biginteger_view<base>& val
        ) : m_signal(_Signal), _Base(val) {}
        WJR_CONSTEXPR20 signed_biginteger_view(
            const signed_biginteger_view& other,
            size_t offset,
            size_t newsize
        ) : _Base(other.get_unsigned(), offset, newsize), m_signal(other.m_signal) {}
        WJR_CONSTEXPR20 signed_biginteger_view(const signed_biginteger_view&) = default;
        WJR_CONSTEXPR20 signed_biginteger_view& operator=(const signed_biginteger_view&) = default;
        WJR_CONSTEXPR20 const _Base& get_unsigned() const { return *this; }
        WJR_CONSTEXPR20 _Base& get_unsigned() { return *this; }
        WJR_CONSTEXPR20 bool signal() const { return m_signal; }
        WJR_CONSTEXPR20 void maintain_signal() {
            m_signal = m_signal || _Base::test_byte(0);
        }
        WJR_CONSTEXPR20 signed_biginteger_view& maintain() {
            _Base::maintain();
            maintain_signal();
            return *this;
        }
        WJR_CONSTEXPR20 signed_biginteger_view negate() {
            m_signal = (!m_signal) || _Base::test_byte(0);
            return *this;
        }
        WJR_CONSTEXPR20 signed_biginteger_view abs() {
            m_signal = true;
            return *this;
        }
    private:
        bool m_signal = true;
    };

    template<size_t base>
    struct __signed_biginteger_view {
        using traits = biginteger_traits<base>;
        template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
        constexpr decltype(auto) operator()(const T& t) const {
            return signed_integral_view<base, traits::template bits_of_t<T>>(t);
        }

        template<size_t length>
        constexpr decltype(auto) operator()(const unsigned_integral_view<base, length>& val) const {
            return signed_integral_view<base, length>(true, val);
        }

        template<size_t length>
        constexpr decltype(auto) operator()(const signed_integral_view<base, length>& val) const {
            return val;
        }

        template<size_t length>
        constexpr decltype(auto) operator()(
            const signed_integral_view<base, length>& val,
            size_t offset,
            size_t newsize
            ) const {
            return signed_integral_view<base, length>(val, offset, newsize);
        }

        constexpr decltype(auto) operator()(const unsigned_biginteger<base>& val) const {
            return signed_biginteger_view<base>(val, 0, val.size());
        }

        constexpr decltype(auto) operator()(const unsigned_biginteger<base>& val, size_t _Offset, size_t _Size) const {
            return signed_biginteger_view<base>(val, _Offset, _Size);
        }

        constexpr decltype(auto) operator()(const unsigned_biginteger_view<base>& val) const {
            return signed_biginteger_view<base>(true, val);
        }

        constexpr decltype(auto) operator()(const biginteger<base>& val) const {
            return signed_biginteger_view<base>(val, 0, val.size());
        }

        constexpr decltype(auto) operator()(const biginteger<base>& val, size_t _Offset, size_t _Size) const {
            return signed_biginteger_view<base>(val, _Offset, _Size);
        }

        constexpr decltype(auto) operator()(const signed_biginteger_view<base>& val) const {
            return val;
        }

        constexpr decltype(auto) operator()(
            const signed_biginteger_view<base>& val,
            size_t offset,
            size_t newsize
            ) const {
            return signed_biginteger_view<base>(val, offset, newsize);
        }

        constexpr decltype(auto) operator()(const unsigned_biginteger_modifier<base>& val) const {
            return signed_biginteger_view<base>(*val.m_ptr, val.m_offset, val.size());
        }

        constexpr decltype(auto) operator()(
            const unsigned_biginteger_modifier<base>& val, size_t _Offset, size_t _Size) const {
            return signed_biginteger_view<base>(*val.m_ptr, val.m_offset + _Offset, _Size);
        }

    };

    template<size_t base>
    inline constexpr __signed_biginteger_view<base> signed_view;

    template<typename _Enable, size_t base, typename...Args>
    struct __Can_make_signed_view_helper : std::false_type {};

    template<size_t base, typename...Args>
    struct __Can_make_signed_view_helper<
        std::void_t<decltype(std::declval<__signed_biginteger_view<base>>()(std::declval<Args>()...))>,
        base, Args...> : std::true_type {};

    template<size_t base, typename...Args>
    struct __Can_make_signed_view : std::conjunction<__Can_make_signed_view_helper<void, base, Args...>> {};

    template<size_t base, typename...Args>
    constexpr bool __Can_make_signed_view_v = __Can_make_signed_view<base, Args...>::value;

    template<size_t base, typename T>
    struct __Need_to_make_signed : std::conjunction<
        std::negation<__Is_signed_biginteger_view<base, T>>,
        __Can_make_signed_view<base, T>> {};

    template<size_t base, typename T>
    constexpr bool __Need_to_make_signed_v = __Need_to_make_signed<base, T>::value;

    template<size_t base>
    class unsigned_biginteger_modifier {
        template<size_t b>
        friend struct __unsigned_biginteger_view;
        template<size_t b>
        friend struct __signed_biginteger_view;
    public:
        using traits = biginteger_traits<base>;
        WJR_CONSTEXPR20 unsigned_biginteger_modifier(
            unsigned_biginteger<base>& it,
            size_t _offset
        ) noexcept : m_ptr(std::addressof(it)), m_offset(_offset) {
        }
        WJR_CONSTEXPR20 unsigned_biginteger_modifier(
            unsigned_biginteger_modifier it,
            size_t _offset
        ) noexcept : m_ptr(it.m_ptr), m_offset(_offset) {}
        WJR_CONSTEXPR20 unsigned_biginteger_modifier(const unsigned_biginteger_modifier&) = default;
        WJR_CONSTEXPR20 unsigned_biginteger_modifier& operator=(const unsigned_biginteger_modifier&) = default;
        WJR_CONSTEXPR20 size_t offset() const {
            return m_offset;
        }
        WJR_CONSTEXPR20 limb_ptr data() {
            WASSERT_LEVEL_2(m_offset < m_ptr->capacity());
            return m_ptr->data() + m_offset;
        }
        WJR_CONSTEXPR20 limb_const_ptr data() const {
            WASSERT_LEVEL_2(m_offset < m_ptr->capacity());
            return m_ptr->data() + m_offset;
        }
        WJR_CONSTEXPR20 size_t size() const {
            WASSERT_LEVEL_2(m_offset <= m_ptr->size());
            return m_ptr->size() - m_offset;
        }
        WJR_CONSTEXPR20 size_t capacity() const {
            WASSERT_LEVEL_2(m_offset <= m_ptr->capacity());
            return m_ptr->capacity() - m_offset;
        }
        WJR_CONSTEXPR20 bool test_byte(limb_t val) const {
            return size() == 1 && *data() == val;
        }
        WJR_CONSTEXPR20 void reserve(const size_t _Newsize) {
            m_ptr->reserve(m_offset + _Newsize);
        }
        WJR_CONSTEXPR20 void set_size(const size_t _Newsize) {
            m_ptr->set_size(m_offset + _Newsize);
        }
        WJR_CONSTEXPR20 void push_back(limb_t _Val) {
            m_ptr->push_back(_Val);
        }
        WJR_CONSTEXPR20 unsigned_biginteger_modifier& maintain() {
            m_ptr->maintain(m_offset + 1);
            return *this;
        }

    private:
        unsigned_biginteger<base>* m_ptr;
        size_t m_offset;
    };

    template<size_t base>
    struct __unsigned_biginteger_modifier {
        constexpr decltype(auto) operator()(unsigned_biginteger<base>& val) const {
            return unsigned_biginteger_modifier<base>(val, 0);
        }
        constexpr decltype(auto) operator()(unsigned_biginteger<base>& val, size_t _Offset) const {
            return unsigned_biginteger_modifier<base>(val, _Offset);
        }
        constexpr decltype(auto) operator()(unsigned_biginteger_modifier<base> val) const {
            return val;
        }
        constexpr decltype(auto) operator()(unsigned_biginteger_modifier<base> val, size_t _Offset) const {
            return unsigned_biginteger_modifier<base>(val, _Offset);
        }
    };

    template<size_t base>
    inline constexpr __unsigned_biginteger_modifier<base> unsigned_modifier;

    template<typename _Enable, size_t base, typename...Args>
    struct __Can_make_unsigned_modifier_helper : std::false_type {};

    template<size_t base, typename...Args>
    struct __Can_make_unsigned_modifier_helper<
        std::void_t<decltype(unsigned_modifier<base>(std::declval<Args>()...))>, base, Args...> : std::true_type {};

    template<size_t base, typename...Args>
    struct __Can_make_unsigned_modifier : std::conjunction<
        __Can_make_unsigned_modifier_helper<void, base, Args...>> {};

    template<size_t base, typename...Args>
    constexpr bool __Can_make_unsigned_modifier_v =
        __Can_make_unsigned_modifier<base, Args...>::value;

    template<size_t base>
    class unsigned_biginteger_reference;

    template<size_t base>
    class unsigned_biginteger_const_reference {
        using traits = biginteger_traits<base>;
        template<size_t _Base>
        friend class unsigned_biginteger_reference;
    public:
        constexpr unsigned_biginteger_const_reference(
            limb_const_ptr _Data, size_t _Size, size_t _K, size_t _Index
        ) noexcept : m_data(const_cast<limb_ptr>(_Data)), m_size(_Size), m_k(_K), m_index(_Index) {}
        constexpr unsigned_biginteger_const_reference(
            const unsigned_biginteger_const_reference&) noexcept = default;
        constexpr unsigned_biginteger_const_reference& operator=(
            const unsigned_biginteger_const_reference&) noexcept = default;
        constexpr size_t pos1()const noexcept { return m_index / traits::bits; }
        constexpr size_t pos2()const noexcept { return m_index % traits::bits; }
        WJR_CONSTEXPR operator limb_t() const noexcept {
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            auto low = traits::div_base_power(m_data[_Pos1], _Pos2);
            size_t low_l = traits::bits - _Pos2;
            if (m_k <= low_l || _Pos1 == m_size - 1) {
                return get_mod(low);
            }
            size_t high_l = m_k - low_l;
            auto high = traits::mod_base_power(m_data[_Pos1 + 1], high_l);
            return traits::mul_base_power(high, low_l) + low;
        }
    private:

        WJR_CONSTEXPR limb_t get_mod(limb_t val) const noexcept {
            if constexpr (traits::is_full()) {
                return m_k != traits::bits ?
                    traits::mod_base_power(val, m_k) : val;
            }
            else {
                return traits::mod_base_power(val, m_k);
            }
        }

        limb_ptr m_data;
        size_t m_size;
        const size_t m_k;
        size_t m_index;
    };

    template<size_t base>
    class unsigned_biginteger_reference
        : public unsigned_biginteger_const_reference<base> {
        using traits = biginteger_traits<base>;
        using _Base = unsigned_biginteger_const_reference<base>;
    public:
        constexpr unsigned_biginteger_reference(
            limb_ptr _Data, size_t _Size, size_t _K, size_t _Index
        ) noexcept : _Base(_Data, _Size, _K, _Index) {}
        constexpr unsigned_biginteger_reference(
            const unsigned_biginteger_reference&) noexcept = default;
        constexpr unsigned_biginteger_reference& operator=(
            const unsigned_biginteger_reference&) noexcept = default;
        WJR_CONSTEXPR unsigned_biginteger_reference& operator=(limb_t val) noexcept {
            WASSERT_LEVEL_2(val >= 0 && val <= traits::max());
            size_t _Pos1 = _Base::pos1();
            size_t _Pos2 = _Base::pos2();
            auto low = traits::div_base_power(_Base::m_data[_Pos1], _Pos2);
            size_t low_l = traits::bits - _Pos2;
            if (_Base::m_k <= low_l || _Pos1 == _Base::m_size - 1) {
                _Base::m_data[_Pos1] += traits::mul_base_power((val - _Base::get_mod(low)), _Pos2);
            }
            else {
                size_t high_l = _Base::m_k - low_l;
                auto high = traits::mod_base_power(_Base::m_data[_Pos1 + 1], high_l);
                auto val_high = traits::div_base_power(val, low_l);
                auto val_low = val - traits::mul_base_power(val_high, low_l);
                _Base::m_data[_Pos1] += traits::mul_base_power((val_low - low), _Pos2);
                _Base::m_data[_Pos1 + 1] += (val_high - high);
            }
            return *this;
        }

        WJR_CONSTEXPR unsigned_biginteger_reference& zero_change(limb_t val) noexcept {
            WASSERT_LEVEL_1(val >= 0 && val <= traits::max());
            WASSERT_LEVEL_2((limb_t)(*this) == 0);
            size_t _Pos1 = _Base::pos1();
            size_t _Pos2 = _Base::pos2();
            size_t low_l = traits::bits - _Pos2;
            if (_Base::m_k <= low_l || _Pos1 == _Base::m_size - 1) {
                _Base::m_data[_Pos1] += traits::mul_base_power(val, _Pos2);
            }
            else {
                auto val_high = traits::div_base_power(val, low_l);
                auto val_low = val - traits::mul_base_power(val_high, low_l);
                _Base::m_data[_Pos1] += traits::mul_base_power(val_low, _Pos2);
                _Base::m_data[_Pos1 + 1] += val_high;
            }
            return *this;
        }

    };

    template<size_t base>
    class unsigned_biginteger_const_iterator {
        using traits = biginteger_traits<base>;
    public:
        using value_type = limb_t;
        using pointer = limb_ptr;
        using const_pointer = limb_const_ptr;
        using reference = unsigned_biginteger_const_reference<base>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;
        constexpr unsigned_biginteger_const_iterator(
            limb_const_ptr _Data, size_t _Size, size_t _K, size_t _Index = 0)
            noexcept : m_data(const_cast<limb_ptr>(_Data)),
            m_size(_Size), m_k(_K), m_index(_Index) {
        }
        constexpr unsigned_biginteger_const_iterator(
            const unsigned_biginteger_const_iterator&) noexcept = default;
        constexpr unsigned_biginteger_const_iterator& operator=(
            const unsigned_biginteger_const_iterator&) noexcept = default;
        constexpr size_t pos1()const noexcept {
            return m_index / traits::bits;
        }
        constexpr size_t pos2()const noexcept {
            return m_index % traits::bits;
        }
        constexpr bool vaild()const noexcept {
            return pos1() < m_size;
        }
        constexpr bool is_last()const noexcept {
            return pos1() + 1 == m_size;
        }
        constexpr unsigned_biginteger_const_iterator& operator++() {
            m_index += m_k;
            return *this;
        }
        constexpr unsigned_biginteger_const_iterator operator++(int) {
            unsigned_biginteger_const_iterator it(*this);
            ++(*this);
            return it;
        }
        constexpr unsigned_biginteger_const_iterator& operator+=(size_t delta) {
            m_index += delta * m_k;
            return *this;
        }
        constexpr unsigned_biginteger_const_iterator operator+(size_t delta) const {
            unsigned_biginteger_const_iterator it(*this);
            it += delta;
            return it;
        }
        constexpr unsigned_biginteger_const_iterator& operator--() {
            m_index -= m_k;
            return *this;
        }
        constexpr unsigned_biginteger_const_iterator operator--(int) {
            unsigned_biginteger_const_iterator it(*this);
            --(*this);
            return it;
        }
        constexpr unsigned_biginteger_const_iterator& operator-=(size_t delta) {
            m_index -= m_k * delta;
            return *this;
        }
        constexpr unsigned_biginteger_const_iterator operator-(size_t delta)const {
            unsigned_biginteger_const_iterator it(*this);
            it -= delta;
            return *this;
        }
        constexpr reference operator*()const noexcept {
            WASSERT_LEVEL_1(vaild());
            return reference(m_data, m_size, m_k, m_index);
        }
        constexpr reference operator[](const size_t n) const noexcept {
            return *((*this) + n);
        }
        constexpr limb_const_ptr operator->()const noexcept {
            return m_data;
        }
        friend difference_type operator-(
            const unsigned_biginteger_const_iterator& lhs,
            const unsigned_biginteger_const_iterator& rhs
            ) {
            WASSERT_LEVEL_1(lhs.m_k == rhs.m_k);
            return (rhs.m_index - lhs.m_index) / lhs.m_k;
        }
        constexpr friend bool operator==(
            const unsigned_biginteger_const_iterator& lhs,
            const unsigned_biginteger_const_iterator& rhs
            ) {
            return lhs.m_data == rhs.m_data && lhs.m_size == rhs.m_size &&
                lhs.m_k == rhs.m_k && lhs.m_index == rhs.m_index;
        }
        constexpr friend bool operator!=(
            const unsigned_biginteger_const_iterator& lhs,
            const unsigned_biginteger_const_iterator& rhs
            ) {
            return !(lhs == rhs);
        }
    protected:
        limb_ptr m_data;
        size_t m_size;
        const size_t m_k;
        size_t m_index = 0;
    };

    template<size_t base>
    class unsigned_biginteger_iterator
        : public unsigned_biginteger_const_iterator<base> {
        using traits = biginteger_traits<base>;
        using _Base = unsigned_biginteger_const_iterator<base>;
    public:
        using value_type = limb_t;
        using pointer = limb_ptr;
        using const_pointer = limb_const_ptr;
        using reference = unsigned_biginteger_reference<base>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;
        constexpr unsigned_biginteger_iterator(
            limb_ptr _Data, size_t _Size, size_t _K, size_t _Index = 0)
            noexcept : _Base(_Data, _Size, _K, _Index) {}
        constexpr unsigned_biginteger_iterator(
            const unsigned_biginteger_iterator&) noexcept = default;
        constexpr unsigned_biginteger_iterator& operator=(
            const unsigned_biginteger_iterator&) noexcept = default;
        constexpr unsigned_biginteger_iterator& operator++() {
            _Base::operator++();
            return *this;
        }
        constexpr unsigned_biginteger_iterator operator++(int) {
            unsigned_biginteger_iterator it(*this);
            _Base::operator++();
            return *this;
        }
        constexpr unsigned_biginteger_iterator& operator+=(size_t index) {
            _Base::operator+=(index);
            return *this;
        }
        constexpr unsigned_biginteger_iterator operator+(size_t index) const {
            unsigned_biginteger_iterator it(*this);
            it += index;
            return it;
        }
        constexpr unsigned_biginteger_iterator& operator--() {
            _Base::operator--();
            return *this;
        }
        constexpr unsigned_biginteger_iterator operator--(int) {
            unsigned_biginteger_iterator it(*this);
            _Base::operator--();
            return it;
        }
        constexpr unsigned_biginteger_iterator& operator-=(size_t index) const {
            _Base::operator-=(index);
            return *this;
        }
        constexpr unsigned_biginteger_iterator operator-(size_t index) {
            unsigned_biginteger_iterator it(*this);
            it -= index;
            return *this;
        }
        constexpr reference operator*() noexcept {
            return reference(_Base::m_data, _Base::m_size, _Base::m_k, _Base::m_index);
        }
        constexpr reference operator[](const size_t n) noexcept {
            return *((*this) + n);
        }
        constexpr limb_ptr operator->() noexcept {
            return _Base::m_data;
        }
        constexpr friend difference_type operator-(
            const unsigned_biginteger_iterator& lhs,
            const unsigned_biginteger_iterator& rhs
            ) {
            WASSERT_LEVEL_1(lhs.m_k == rhs.m_k);
            return (lhs.m_index - rhs.m_index) / lhs.m_k;
        }
        constexpr friend bool operator==(
            const unsigned_biginteger_iterator& lhs,
            const unsigned_biginteger_iterator& rhs
            ) {
            return lhs.m_data == rhs.m_data && lhs.m_size == rhs.m_size
                && lhs.m_k == rhs.m_k && lhs.m_index == rhs.m_index;
        }
        constexpr friend bool operator!=(
            const unsigned_biginteger_iterator& lhs,
            const unsigned_biginteger_iterator& rhs
            ) {
            return !(lhs == rhs);
        }
    };

    template<typename T, typename = void>
    struct __has_data : std::false_type {};

    template<typename T>
    struct __has_data<T, std::void_t<decltype(std::declval<T>().data())>> : std::true_type {};

    template<typename T, typename = void>
    struct __has_size : std::false_type {};

    template<typename T>
    struct __has_size<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};

    template<typename T>
    struct __has_data_and_size : std::conjunction<__has_data<T>, __has_size<T>> {};

    template<typename T>
    constexpr bool __has_data_and_size_v = __has_data_and_size<T>::value;

    template<size_t base>
    struct __unsigned_iterator {
        template<typename T, std::enable_if_t<__has_data_and_size_v<const T&>, int> = 0>
        constexpr decltype(auto) operator()(
            const T& v, size_t k = 1, size_t index = 0
            ) const {
            return unsigned_biginteger_const_iterator<base>(v.data(), v.size(), k, index);
        }

        template<typename T, std::enable_if_t<__has_data_and_size_v<T&>, int> = 0>
        constexpr decltype(auto) operator()(
            T& v, size_t k = 1, size_t index = 0
            ) const {
            return unsigned_biginteger_iterator<base>(v.data(), v.size(), k, index);
        }

    };

    template<size_t base>
    inline constexpr __unsigned_iterator<base> unsigned_iterator;

#define REGISTER_BIGINTEGER_FUNCTION(index,rt,FUNC,...)		        \
        if constexpr (index == 1) {					    	        \
            return FUNC<1>(__VA_ARGS__);					        \
        }													        \
        else {												        \
            if(rt.size() != 1){                                     \
                return FUNC<0>(__VA_ARGS__);                        \
            }else{                                                  \
                return FUNC<1>(__VA_ARGS__);                        \
            }                                                       \
        }

#define REGISTER_BETTER_BIGINTEGER_FUNCTION(il,ir,FUNC,result,lhs,rhs)                              \
        if constexpr(ir == 1){                                                                      \
            return FUNC(result,lhs,rhs);                                                            \
        }                                                                                           \
        else {                                                                                      \
            if constexpr (il == 1) {                                                                \
                return FUNC(result, rhs, lhs);                                                      \
            }                                                                                       \
            else {                                                                                  \
                if(lhs.size() >= rhs.size()){                                                       \
                    return FUNC(result, lhs, rhs);                                                  \
                }                                                                                   \
                else {                                                                              \
                    return FUNC(result, rhs, lhs);                                                  \
                }                                                                                   \
            }                                                                                       \
        }

#define REGISTER_BETTER_BIGINTEGER_WITH_TAG_FUNCTION(il,ir,FUNC, result, lhs, rhs, utag, vtag)      \
        if constexpr(ir == 1){                                                                      \
            return FUNC(result, lhs, rhs, utag);                                                    \
        }                                                                                           \
        else {                                                                                      \
            if constexpr (il == 1) {                                                                \
                return FUNC(result, rhs, lhs, vtag);                                                \
            }                                                                                       \
            else {                                                                                  \
                if(lhs.size() >= rhs.size()){                                                       \
                    return FUNC(result, lhs, rhs, utag);                                            \
                }                                                                                   \
                else {                                                                              \
                    return FUNC(result, rhs, lhs, vtag);                                            \
                }                                                                                   \
            }                                                                                       \
        }

#define UNSIGNED_BIGINTEGER_VIEW_SINGLE_DECLARATION                                             \
    template<typename T, std::enable_if_t<                                                      \
            __Can_make_unsigned_view_v<base, std::dacay_t<T>>, int> = 0>

#define UNSIGNED_BIGINTEGER_VIEW_BINARY_DECLARATION                                             \
    template<typename T, typename U,std::enable_if_t<                                           \
            std::conjunction_v<__Can_make_unsigned_view<base, std::decay_t<T>>,                 \
            __Can_make_unsigned_view<base,std::decay_t<U>>>, int> = 0>

#define UNSIGNED_BIGINTEGER_VIEW_BINARY_AND_TAG_DECLARATION                                     \
    template<typename T, typename U, typename V, typename C = tag::empty_tag, std::enable_if_t< \
            std::conjunction_v<__Can_make_unsigned_modifier<base, T&&>,                         \
            __Can_make_unsigned_view<base, std::decay_t<U>>,                                    \
            __Can_make_unsigned_view<base, std::decay_t<V>>,                                    \
            tag::is_tag_set<C>                                                                  \
            >, int> = 0>

#define UNSIGNED_BIGINTEGER_VIEW_SINGLE_AND_TAG_DECLARATION                                     \
    template<typename T, typename U, typename C = tag::empty_tag, std::enable_if_t<             \
            std::conjunction_v<__Can_make_unsigned_modifier<base, T&&>,                          \
            __Can_make_unsigned_view<base,std::decay_t<U>>, tag::is_tag_set<C>                  \
            >, int> = 0>

    class __Biginteger_vector {
    public:
        using value_type = limb_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using allocator_type = mallocator<value_type>;

        WJR_CONSTEXPR20 __Biginteger_vector() = default;
        WJR_CONSTEXPR20 __Biginteger_vector(pointer ptr, size_t _Size, size_t _Capacity, tag::store_tag)
            : m_first(ptr), m_last(ptr + _Size), m_end(ptr + _Capacity) {}
        WJR_CONSTEXPR20 __Biginteger_vector(size_type _Count, value_type _Value = value_type())
            : m_first(allocate(_Count)), m_last(m_first + _Count), m_end(m_last) {
            std::fill_n(m_first, _Count, _Value);
        }
        WJR_CONSTEXPR20 __Biginteger_vector(size_type _Count, tag::uninitialized_tag)
            : m_first(allocate(_Count)), m_last(m_first + _Count), m_end(m_last) {
            WASSERT_LEVEL_1(_Count >= 1);
        }
        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 __Biginteger_vector(iter _First, iter _Last, std::input_iterator_tag) {
            while (_First != _Last) {
                push_back(*_First);
                ++_First;
            }
        }
        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 __Biginteger_vector(iter _First, iter _Last, std::forward_iterator_tag) {
            size_t _Count = std::distance(_First, _Last);
            m_first = allocate(_Count);
            m_last = m_first + _Count;
            m_end = m_last;
            std::copy_n(_First, _Count, m_first);
        }
        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 __Biginteger_vector(iter _First, iter _Last)
            : __Biginteger_vector(_First, _Last, typename std::iterator_traits<iter>::iterator_category{}) {
        }
        WJR_CONSTEXPR20 __Biginteger_vector(const __Biginteger_vector& _Right)
            : m_first(allocate(_Right.size())),
            m_last(m_first + _Right.size()), m_end(m_last) {
            std::copy_n(_Right.m_first, _Right.size(), m_first);
        }
        WJR_CONSTEXPR20 __Biginteger_vector(__Biginteger_vector&& _Right) noexcept
            : m_first(std::exchange(_Right.m_first, nullptr)),
            m_last(std::exchange(_Right.m_last, nullptr)),
            m_end(std::exchange(_Right.m_end, nullptr)) {
        }
        WJR_CONSTEXPR20 __Biginteger_vector& operator=(const __Biginteger_vector& _Right) {
            if (unlikely(this == std::addressof(_Right))) {
                return *this;
            }
            const size_type _Newsize = _Right.size();
            const size_type _Oldcapacity = capacity();
            if (_Newsize > _Oldcapacity) {
                const size_type _Newcapacity = _Calcuate_growth(_Newsize);
                m_first = reallocate(m_first, _Oldcapacity, _Newcapacity, tag::uninitialized);
                m_end = m_first + _Newcapacity;
            }
            std::copy_n(_Right.m_first, _Newsize, m_first);
            m_last = m_first + _Newsize;
            return *this;
        }
        WJR_CONSTEXPR20 __Biginteger_vector& operator=(__Biginteger_vector&& _Right) noexcept {
            if (unlikely(this == std::addressof(_Right))) {
                return *this;
            }
            _Tidy();
            m_first = std::exchange(_Right.m_first, nullptr);
            m_last = std::exchange(_Right.m_last, nullptr);
            m_end = std::exchange(_Right.m_end, nullptr);
            return *this;
        }

        WJR_CONSTEXPR20 ~__Biginteger_vector() {
            _Tidy();
        }

        WJR_CONSTEXPR20 void swap(__Biginteger_vector& _Right) noexcept {
            std::swap(m_first, _Right.m_first);
            std::swap(m_last, _Right.m_last);
            std::swap(m_end, _Right.m_end);
        }

        WJR_CONSTEXPR20 __Biginteger_vector& assign(pointer ptr, size_t _Size, size_t _Capacity, tag::store_tag) {
            if (ptr >= m_first && ptr < m_end) {
                WASSERT_LEVEL_2(ptr + _Size <= m_end);
                std::copy_n(ptr, _Size, m_first);
                m_last = m_first + _Size;
            }
            else {
                _Tidy();
                m_first = ptr;
                m_last = ptr + _Size;
                m_end = ptr + _Capacity;
            }
            return *this;
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 __Biginteger_vector& assign(iter _First, iter _Last, std::input_iterator_tag) {
            clear();
            while (_First != _Last) {
                push_back(*_First);
                ++_First;
            }
            return *this;
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 __Biginteger_vector& assign(iter _First, iter _Last, std::forward_iterator_tag) {
            size_t _Count = std::distance(_First, _Last);
            reserve(_Count, tag::uninitialized);
            set_size(_Count);
            std::copy_n(_First, _Count, m_first);
            return *this;
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 __Biginteger_vector& assign(iter _First, iter _Last) {
            return assign(_First, _Last, typename std::iterator_traits<iter>::iterator_category{});
        }

        WJR_CONSTEXPR20 void reserve(size_type _Count) noexcept {
            const size_type _Oldcapacity = capacity();
            if (_Count > _Oldcapacity) {
                const size_type _Oldsize = size();
                const size_type _Newcapacity = _Calcuate_growth(_Count);
                m_first = reallocate(m_first, _Oldsize, _Oldcapacity, _Newcapacity);
                m_last = m_first + _Oldsize;
                m_end = m_first + _Newcapacity;
            }
        }

        WJR_CONSTEXPR20 void reserve(size_type _Count, tag::uninitialized_tag) noexcept {
            const size_type _Oldcapacity = capacity();
            if (_Count > _Oldcapacity) {
                const size_type _Oldsize = size();
                const size_type _Newcapacity = _Calcuate_growth(_Count);
                m_first = reallocate(m_first, _Oldcapacity, _Newcapacity, tag::uninitialized);
                m_last = m_first + _Oldsize;
                m_end = m_first + _Newcapacity;
            }
        }

        WJR_CONSTEXPR20 void set_size(size_type _Newsize) {
            WASSERT_LEVEL_1(_Newsize <= capacity());
            m_last = m_first + _Newsize;
        }

        WJR_CONSTEXPR20 void push_back(const value_type _Value) {
            if (m_last == m_end) {
                const size_type _Oldcapacity = capacity();
                reserve(_Oldcapacity + 1);
            }
            *m_last = _Value;
            ++m_last;
        }

        WJR_CONSTEXPR void push_back(const value_type _Value, tag::reserved_tag) {
            *m_last = _Value;
            ++m_last;
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 void insert_back(iter _First, iter _Last, std::forward_iterator_tag) {
            const size_t _Oldsize = size();
            const size_t _Count = std::distance(_First, _Last);
            reserve(_Oldsize + _Count);
            set_size(_Oldsize + _Count);
            std::copy_n(_First, _Count, m_last);
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 void insert_back(iter _First, iter _Last) {
            insert_back(_First, _Last, typename std::iterator_traits<iter>::iterator_category{});
        }

        WJR_CONSTEXPR20 void insert_back(const size_type _Count, const value_type _Val = value_type{}) {
            const size_type _Oldsize = size();
            reserve(_Oldsize + _Count);
            set_size(_Oldsize + _Count);
            std::fill_n(m_last, _Count, _Val);
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 void insert_front(iter _First, iter _Last, std::forward_iterator_tag) {
            const size_t _Count = std::distance(_First, _Last);
            if (_Count) {
                const size_t _Oldsize = size();
                reserve(_Oldsize + _Count);
                set_size(_Oldsize + _Count);
                std::copy_backward(m_first, m_last, m_last + _Count);
                std::copy_n(_First, _Count, m_first);
            }
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 void insert_front(iter _First, iter _Last) {
            insert_front(_First, _Last, typename std::iterator_traits<iter>::iterator_category{});
        }

        WJR_CONSTEXPR20 void insert_front(const size_type _Count, const value_type _Val = value_type{}) {
            if (_Count) {
                const size_type _Oldsize = size();
                reserve(_Oldsize + _Count);
                std::copy_backward(m_first, m_last, m_last + _Count);
                set_size(_Oldsize + _Count);
                std::fill_n(m_first, _Count, _Val);
            }
        }

        WJR_CONSTEXPR20 void erase_front(const size_type _Count) {
            WASSERT_LEVEL_2(_Count <= size());
            if (_Count) {
                std::copy(m_first + _Count, m_last, m_first);
                set_size(size() - _Count);
            }
        }

        WJR_CONSTEXPR20 void clear() { m_last = m_first; }
        WJR_CONSTEXPR20 size_t size() const { return m_last - m_first; }
        WJR_CONSTEXPR20 size_t capacity() const { return m_end - m_first; }
        WJR_CONSTEXPR20 pointer data() noexcept { return m_first; }
        WJR_CONSTEXPR20 const_pointer data() const noexcept { return m_first; }
        WJR_CONSTEXPR20 iterator begin() noexcept { return m_first; }
        WJR_CONSTEXPR20 const_iterator begin() const noexcept { return m_first; }
        WJR_CONSTEXPR20 const_iterator cbegin() const noexcept { return m_first; }
        WJR_CONSTEXPR20 iterator end() noexcept { return m_last; }
        WJR_CONSTEXPR20 const_iterator end() const noexcept { return m_last; }
        WJR_CONSTEXPR20 const_iterator cend() const noexcept { return m_last; }
        WJR_CONSTEXPR20 reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        WJR_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        WJR_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
        WJR_CONSTEXPR20 reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        WJR_CONSTEXPR20 const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        WJR_CONSTEXPR20 const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
        WJR_CONSTEXPR20 reference operator[](size_type _Index) noexcept {
            WASSERT_LEVEL_2(_Index < size());
            return m_first[_Index];
        }
        WJR_CONSTEXPR20 const_reference operator[](size_type _Index) const {
            WASSERT_LEVEL_2(_Index < size());
            return m_first[_Index];
        }
        WJR_CONSTEXPR20 reference front() noexcept {
            WASSERT_LEVEL_2(0 < size());
            return m_first[0];
        }
        WJR_CONSTEXPR20 const_reference front() const noexcept {
            WASSERT_LEVEL_2(0 < size());
            return m_first[0];
        }
        WJR_CONSTEXPR20 reference back() noexcept {
            WASSERT_LEVEL_2(0 < size());
            return m_last[-1];
        }
        WJR_CONSTEXPR20 const_reference back() const noexcept {
            WASSERT_LEVEL_2(0 < size());
            return m_last[-1];
        }
    private:
        WJR_CONSTEXPR20 static pointer allocate(size_t _Count) noexcept {
            return allocator_type{}.allocate(_Count);
        }
        WJR_CONSTEXPR20 static void deallocate(pointer ptr, size_t _Count) noexcept {
            allocator_type{}.deallocate(ptr, _Count);
        }
        WJR_CONSTEXPR20 static pointer reallocate(
            pointer ptr, size_t _Oldsize, size_t _Oldcapacity, size_t _Newcapacity) noexcept {
            WASSERT_LEVEL_2(_Newcapacity >= _Oldcapacity);
            pointer new_ptr = allocate(_Newcapacity);
            std::copy_n(ptr, _Oldsize, new_ptr);
            deallocate(ptr, _Oldcapacity);
            return new_ptr;

        }
        WJR_CONSTEXPR20 static pointer reallocate(
            pointer ptr, size_t _Oldcapacity, size_t _Newcapacity, tag::uninitialized_tag) noexcept {
            WASSERT_LEVEL_2(_Newcapacity >= _Oldcapacity);
            deallocate(ptr, _Oldcapacity);
            return allocate(_Newcapacity);
        }

        WJR_CONSTEXPR20 size_t _Calcuate_growth(const size_type _Newsize) const {
            const size_type _Oldcapacity = capacity();
            const size_type _Newcapacity = _Oldcapacity + _Oldcapacity / 2;
            return _Newcapacity > _Newsize ? _Newcapacity : _Newsize;
        }

        WJR_CONSTEXPR20 void _Tidy() noexcept {
            deallocate(m_first, capacity());
        }

        pointer m_first = nullptr;
        pointer m_last = nullptr;
        pointer m_end = nullptr;
    };

    enum class radix_conversion_type {
        same,
        multiple,
        normal

    };

    template<size_t from_base, size_t to_base>
    struct __radix_conversion_helper {
    private:
        constexpr static radix_conversion_type __get() {
            if constexpr (from_base == to_base) {
                return radix_conversion_type::same;
            }
            else {
                if constexpr (from_base > to_base) {
                    return __radix_conversion_helper<to_base, from_base>::value;
                }
                else {
                    constexpr int q =
                        math::force_constexpr::integer_log(std::integral_constant<size_t, from_base>{}, to_base);
                    if constexpr (math::force_constexpr::integer_power(std::integral_constant<size_t, from_base>{}, q) == to_base) {
                        return radix_conversion_type::multiple;
                    }
                    else {
                        return radix_conversion_type::normal;
                    }
                }
            }
        }
    public:
        constexpr static radix_conversion_type value = __get();
    };

    template<size_t base>
    class unsigned_biginteger : public __Biginteger_vector {

        using _Base = __Biginteger_vector;

        template<size_t b>
        friend class unsigned_biginteger;

        template<size_t b>
        friend class biginteger;

        using traits = biginteger_traits<base>;

    public:
        using value_type = limb_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using allocator_type = mallocator<value_type>;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        using bit_iterator = unsigned_biginteger_iterator<base>;
        using bit_const_iterator = unsigned_biginteger_const_iterator<base>;

        WJR_CONSTEXPR20 unsigned_biginteger() : _Base(1, 0) {}
        WJR_CONSTEXPR20 unsigned_biginteger(size_t n, tag::uninitialized_tag)
            : _Base(n, tag::uninitialized) {}
        WJR_CONSTEXPR20 unsigned_biginteger(const unsigned_biginteger& other) = default;
        WJR_CONSTEXPR20 unsigned_biginteger(unsigned_biginteger&& other) noexcept = default;
        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 unsigned_biginteger(iter _First, iter _Last)
            : _Base(_First, _Last) {}

        template<typename T, std::enable_if_t<
            __Is_unsigned_biginteger_view_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 explicit unsigned_biginteger(const T& val)
            noexcept : _Base(val.data(), val.data() + val.size()) {
            if (unlikely(_Base::size() == 0)) {
                set_byte(0);
            }
        }

        template<typename T, std::enable_if_t<
            __Need_to_make_unsigned_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 explicit unsigned_biginteger(const T& val)
            : unsigned_biginteger(unsigned_view<base>(val)) {}

        template<typename iter, typename Func,
            std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 void in_bits(iter _First, iter _Last, Func&& fn) {
            size_t n = std::distance(_First, _Last);
            if (n == 0) {
                set_byte(0);
            }
            else {
                size_t q = (n + traits::bits - 1) / traits::bits;
                _Base::reserve(q, tag::uninitialized);
                _Base::set_size(q);
                _Base::back() = 0;
                auto _Write = unsigned_iterator<base>(*this, 1, n);
                while (_First != _Last) {
                    --_Write;
                    *_Write = fn(*_First);
                    ++_First;
                }
            }
        }

        template<typename iter, typename Func,
            std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 std::optional<iter> out_bits(iter _First, iter _Last, Func&& fn) const {
            size_t n = bit_size();
            auto _Read = unsigned_iterator<base>(*this, 1, n);
            while (n && _First != _Last) {
                --_Read;
                *_First = fn(*_Read);
                ++_First;
                --n;
            }
            if (n) { return {}; }
            return _First;
        }

        WJR_CONSTEXPR20 unsigned_biginteger& operator=(const unsigned_biginteger&) = default;
        WJR_CONSTEXPR20 unsigned_biginteger& operator=(unsigned_biginteger&& other) noexcept = default;
        template<typename T, std::enable_if_t<
            __Is_unsigned_biginteger_view_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 unsigned_biginteger& operator=(const T& rhs) {
            if constexpr (T::is_unsigned_biginteger) {
                if (unlikely(data() == rhs.data())) {
                    WASSERT_LEVEL_1(rhs.size() <= size());
                    _Base::set_size(rhs.size());
                    return *this;
                }
            }
            else {
                WASSERT_LEVEL_1(data() != rhs.data());
            }
            _Base::assign(rhs.data(), rhs.data() + rhs.size());
            return *this;
        }
        template<typename T, std::enable_if_t<
            __Need_to_make_unsigned_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 unsigned_biginteger& operator=(const T& val) {
            return ((*this) = unsigned_view<base>(val));
        }

        WJR_CONSTEXPR20 bool test_byte(limb_t val) const { return _Base::size() == 1 && _Base::front() == val; }
        WJR_CONSTEXPR20 void set_byte(limb_t val) { _Base::set_size(1); _Base::front() = val; }
        WJR_CONSTEXPR20 size_type bit_size() const {
            size_t _Pos = _Base::size() - 1;
            return _Pos * traits::bits + traits::bit_size(_Base::operator[](_Pos));
        }
        WJR_CONSTEXPR20 void bit_reserve(size_t n) {
            reserve((n + traits::bits - 1) / traits::bits + 1);
        }

        WJR_CONSTEXPR20 void mul_base_power(size_t index) {
            if (!index || test_byte(0))return;
            const size_t n = _Base::size();
            const size_t k = index / traits::bits;
            const size_t r = index % traits::bits;
            if constexpr (traits::is_full()) {
                _Base::reserve(n + k + 1);
                auto p = _Base::data();
                auto v = math::shl(p, n, r * traits::integer_log2);
                if (v) {
                    _Base::push_back(v);
                }
                _Base::insert_front(k, 0);
            }
            else {
                if (!r) {
                    _Base::insert_front(k, 0);
                }
                else {
                    size_t br = traits::bits - r;
                    bool is = _Base::operator[](n - 1) >= traits::get_base_power(br);
                    _Base::reserve(n + k + is);
                    _Base::set_size(n + k + is);
                    auto p = _Base::data();
                    limb_t _High = 0;
                    limb_t _Low;
                    if (is) {
                        p[n + k] = _High = traits::div_base_power(p[n - 1], br);
                    }
                    for (size_t i = n - 1; i; --i) {
                        _Low = p[i] - traits::mul_base_power(_High, br);
                        _High = traits::div_base_power(p[i - 1], br);
                        p[i + k] = traits::mul_base_power(_Low, r) + _High;
                    }
                    _Low = p[0] - traits::mul_base_power(_High, br);
                    p[k] = traits::mul_base_power(_Low, r);
                    std::fill_n(p, k, 0);
                }
            }
        }

        WJR_CONSTEXPR20 void div_base_power(size_t index) {
            if (!index || test_byte(0))return;
            size_t n = _Base::size();
            size_t k = index / traits::bits;
            size_t r = index % traits::bits;
            if (k >= n) {
                set_byte(0);
                return;
            }
            _Base::erase_front(k);
            n = _Base::size();
            auto p = _Base::data();
            if constexpr (traits::is_full()) {
                math::shr(p, n, r * traits::integer_log2);
                if (!p[n - 1]) {
                    _Base::set_size(n - 1);
                }
            }
            else {
                if (r) {
                    limb_t mod_tag = traits::get_base_power(r);
                    limb_t mul_tag = traits::get_base_power(traits::bits - r);
                    for (size_t i = 0; i != n - 1; ++i) {
                        p[i] = traits::div_base_power(p[i], r) +
                            ((p[i + 1] - traits::div_base_power(p[i + 1], r) * mod_tag) * mul_tag);
                    }
                    p[n - 1] = traits::div_base_power(p[n - 1], r);
                    maintain();
                }
            }
        }

        WJR_CONSTEXPR20 void mod_base_power(size_t index) {
            if (!index) {
                set_byte(0);
                return;
            }
            size_t _Oldsize = _Base::size();
            size_t _Newsize = (index + traits::bits - 1) / traits::bits;
            if (_Oldsize >= _Newsize) {
                _Base::set_size(_Newsize);
                size_t r = index % traits::bits;
                if (r) {
                    limb_ptr p = _Base::data() + (_Newsize - 1);
                    *p = traits::mod_base_power(*p, r);
                }
                maintain();
            }
        }

        WJR_CONSTEXPR20 unsigned_biginteger& maintain(size_t _Minsize) {
            WASSERT_LEVEL_1(_Minsize <= _Base::size());
            size_t n = _Base::size();
            auto p = _Base::data();
            while (n != _Minsize && !p[n - 1])--n;
            _Base::set_size(n);
            return *this;
        }

        WJR_CONSTEXPR20 unsigned_biginteger& maintain() {
            return maintain(1);
        }

    private:
        template<typename T, typename U>
        static WJR_CONSTEXPR20 bool __Equal(const T& lhs, const U& rhs) {
            constexpr size_t il = T::max_bit;
            constexpr size_t ir = U::max_bit;
            constexpr size_t im = std::min(il, ir);

            size_t n = lhs.size();
            size_t m = rhs.size();
            if (n != m)return false;
            auto p = lhs.data();
            auto q = rhs.data();
            if constexpr (im == 1) {
                return p[0] == q[0];
            }
            else {
                return std::equal(p, p + n, q);
            }
        }
        template<typename T, typename U>
        static WJR_CONSTEXPR20 bool __Less(const T& lhs, const U& rhs) {
            constexpr size_t il = T::max_bit;
            constexpr size_t ir = U::max_bit;
            constexpr size_t im = std::min(il, ir);

            size_t n = lhs.size();
            size_t m = rhs.size();
            if (n != m)return n < m;
            auto p = lhs.data();
            auto q = rhs.data();
            if constexpr (im == 1) {
                return p[0] < q[0];
            }
            else {
                std::reverse_iterator<limb_const_ptr> rps(p + n), rpe(p), rqs(q + n), rqe(q);
                return std::lexicographical_compare(rps, rpe, rqs, rqe);
            }
        }
        template<typename T, typename U>
        static WJR_CONSTEXPR20 int __Cmp(const T& lhs, const U& rhs) {
            constexpr size_t il = T::max_bit;
            constexpr size_t ir = U::max_bit;
            constexpr size_t im = std::min(il, ir);

            size_t n = lhs.size();
            size_t m = rhs.size();
            if (n != m) return n < m ? -1 : 1;
            auto p = lhs.data();
            auto q = rhs.data();
            if constexpr (im == 1) {
                return p[0] != q[0] ? (p[0] < q[0] ? -1 : 1) : 0;
            }
            else {
                while (n--) {
                    if (p[n] != q[n]) return p[n] < q[n] ? -1 : 1;
                }
                return 0;
            }
        }
    public:

        UNSIGNED_BIGINTEGER_VIEW_BINARY_DECLARATION
            static WJR_CONSTEXPR20 bool equal(const T& lhs, const U& rhs) {
            return unsigned_biginteger::__Equal(
                unsigned_view<base>(lhs),
                unsigned_view<base>(rhs)
            );
        }
        UNSIGNED_BIGINTEGER_VIEW_BINARY_DECLARATION
            static WJR_CONSTEXPR20 bool not_equal(const T& lhs, const U& rhs) {
            return !unsigned_biginteger::equal(lhs, rhs);
        }
        UNSIGNED_BIGINTEGER_VIEW_BINARY_DECLARATION
            static WJR_CONSTEXPR20 bool less(const T& lhs, const U& rhs) {
            return unsigned_biginteger::__Less(
                unsigned_view<base>(lhs),
                unsigned_view<base>(rhs)
            );
        }
        UNSIGNED_BIGINTEGER_VIEW_BINARY_DECLARATION
            static WJR_CONSTEXPR20 bool less_equal(const T& lhs, const U& rhs) {
            return !unsigned_biginteger::less(rhs, lhs);
        }
        UNSIGNED_BIGINTEGER_VIEW_BINARY_DECLARATION
            static WJR_CONSTEXPR20 bool greater(const T& lhs, const U& rhs) {
            return unsigned_biginteger::less(rhs, lhs);
        }
        UNSIGNED_BIGINTEGER_VIEW_BINARY_DECLARATION
            static WJR_CONSTEXPR20 bool greater_equal(const T& lhs, const U& rhs) {
            return !unsigned_biginteger::less(lhs, rhs);
        }
        UNSIGNED_BIGINTEGER_VIEW_BINARY_DECLARATION
            static WJR_CONSTEXPR20 int cmp(const T& lhs, const U& rhs) {
            return unsigned_biginteger::__Cmp(
                unsigned_view<base>(lhs),
                unsigned_view<base>(rhs)
            );
        }

    private:

        template<size_t index, typename T, typename U, typename C>
        WJR_CONSTEXPR20 static void QAdd(
            unsigned_biginteger_modifier<base> modifier,
            const T& lhs,
            const U& rhs,
            C
        );

        template<typename T, typename U, typename C>
        WJR_CONSTEXPR20 static void CAdd(
            unsigned_biginteger_modifier<base> modifier,
            const T& lhs,
            const U& rhs,
            C
        ) {
            REGISTER_BIGINTEGER_FUNCTION(U::max_bit, rhs, QAdd, modifier, lhs, rhs, C{});
        }

        template<typename T, typename U, typename C>
        WJR_CONSTEXPR20 static void Add(
            unsigned_biginteger_modifier<base> modifier,
            const T& lhs,
            const U& rhs,
            C
        ) {
            constexpr auto utag = C{};
            constexpr auto vtag = C{};
            REGISTER_BETTER_BIGINTEGER_WITH_TAG_FUNCTION(T::max_bit, U::max_bit, CAdd, modifier, lhs, rhs, utag, vtag);
        }

        template<size_t index, typename T, typename U, typename C>
        WJR_CONSTEXPR20 static void QSub(
            unsigned_biginteger_modifier<base> modifier,
            const T& lhs,
            const U& rhs,
            C
        );

        template<typename T, typename U, typename C>
        WJR_CONSTEXPR20 static void Sub(
            unsigned_biginteger_modifier<base> modifier,
            const T& lhs,
            const U& rhs,
            C
        ) {
            REGISTER_BIGINTEGER_FUNCTION(U::max_bit, rhs, QSub, modifier, lhs, rhs, C{});
        }

        template<size_t index, typename T, typename U, typename C = tag::empty_tag>
        WJR_CONSTEXPR20 static void slow_mul(
            unsigned_biginteger& result,
            const T& lhs,
            const U& rhs,
            C = C{}
        );

        template<typename T, typename U>
        WJR_CONSTEXPR20 static void karatsuba(
            unsigned_biginteger& result,
            const T& lhs,
            const U& rhs
        );

        template<typename T, typename U>
        WJR_CONSTEXPR20 static void toom_cook_3(
            unsigned_biginteger& result,
            const T& lhs,
            const U& rhs
        );

        template<size_t i, size_t index, typename T, typename U>
        WJR_CONSTEXPR20 static void dac_mul(
            unsigned_biginteger& result,
            const T& lhs,
            const U& rhs,
            unsigned_biginteger& temp,
            unsigned_biginteger& r
        );

        template<size_t index, typename T, typename U>
        WJR_CONSTEXPR20 static void dac_mul(
            unsigned_biginteger& result,
            const T& lhs,
            const U& rhs
        );

        template<typename T, typename U>
        WJR_CONSTEXPR20 static void fft_mul(
            unsigned_biginteger& result,
            const T& lhs,
            const U& rhs
        );

        template<size_t index, typename T, typename U>
        WJR_CONSTEXPR20 static void QMul(
            unsigned_biginteger& result,
            const T& lhs,
            const U& rhs
        );
        template<typename T, typename U>
        WJR_CONSTEXPR20 static void CMul(
            unsigned_biginteger& result,
            const T& lhs,
            const U& rhs
        ) {
            REGISTER_BIGINTEGER_FUNCTION(U::max_bit, rhs, QMul, result, lhs, rhs);
        }
        template<typename T, typename U>
        WJR_CONSTEXPR20 static void Mul(
            unsigned_biginteger& result,
            const T& lhs,
            const U& rhs
        ) {
            REGISTER_BETTER_BIGINTEGER_FUNCTION(T::max_bit, U::max_bit, CMul, result, lhs, rhs);
        }

        template<typename C>
        WJR_CONSTEXPR20 static void Random(
            unsigned_biginteger_modifier<base> modifier,
            size_t n,
            C
        );

        template<typename T>
        WJR_CONSTEXPR20 static void Interval_random(
            unsigned_biginteger_modifier<base> modifier,
            const T& _Max
        );

        template<size_t to_base, typename T>
        WJR_CONSTEXPR20 static void Binary_radix_conversion(
            unsigned_biginteger<to_base>& dst,
            const T& src,
            std::vector<std::pair<size_t, unsigned_biginteger<to_base>>>& cache
        );

        template<size_t to_base, typename T, typename C>
        WJR_CONSTEXPR20 static void Radix_conversion(
            unsigned_biginteger<to_base>& dst,
            const T& src,
            C
        );

    public:

        UNSIGNED_BIGINTEGER_VIEW_BINARY_AND_TAG_DECLARATION
            WJR_CONSTEXPR20 static void add(
                T&& result,
                const U& lhs,
                const V& rhs,
                C = C{}
            ) {
            Add(unsigned_modifier<base>(std::forward<T>(result)),
                unsigned_view<base>(lhs),
                unsigned_view<base>(rhs),
                C{}
            );
        }

        UNSIGNED_BIGINTEGER_VIEW_BINARY_AND_TAG_DECLARATION
            WJR_CONSTEXPR20 static void sub(
                T&& result,
                const U& lhs,
                const V& rhs,
                C = C{}
            ) {
            Sub(unsigned_modifier<base>(std::forward<T>(result)),
                unsigned_view<base>(lhs),
                unsigned_view<base>(rhs),
                C{}
            );
        }

        UNSIGNED_BIGINTEGER_VIEW_BINARY_DECLARATION
            WJR_CONSTEXPR20 static void mul(
                unsigned_biginteger& result,
                const T& lhs,
                const U& rhs
            ) {
            Mul(result,
                unsigned_view<base>(lhs),
                unsigned_view<base>(rhs));
        }

        template<typename T, typename C = tag::empty_tag,
            std::enable_if_t<std::conjunction_v<__Can_make_unsigned_modifier<base, T&&>, tag::is_tag_set<C>>, int> = 0>
        WJR_CONSTEXPR20 static void random(
            T&& result,
            size_t n,
            C = C{}
        ) {
            Random(unsigned_modifier<base>(std::forward<T>(result)), n, C{});
        }

        template<typename T, typename U, std::enable_if_t<
            std::conjunction_v<
            __Can_make_unsigned_modifier<base, T&&>,
            __Can_make_unsigned_view<base, U>>, int> = 0>
            WJR_CONSTEXPR static void interval_random(
                T&& result,
                const U& _Max
            ) {
            Interval_random(unsigned_modifier<base>(std::forward<T>(result)), unsigned_view<base>(_Max));
        }

        template<typename T, typename U, typename V, std::enable_if_t<
            std::conjunction_v<
            __Can_make_unsigned_modifier<base, T&&>,
            __Can_make_unsigned_view<base, U>,
            __Can_make_unsigned_view<base, V>>, int> = 0>
            WJR_CONSTEXPR20 static void interval_random(
                T&& result,
                const U& _Min,
                const V& _Max
            ) {
            unsigned_biginteger delta;
            sub(delta, _Max, _Min);
            interval_random(result, delta);
            add(result, result, _Min);
        }

        template<size_t to_base, typename T, typename C = tag::empty_tag,
            std::enable_if_t<
            std::conjunction_v<__Can_make_unsigned_view<base, T>
            , tag::is_tag_set<C>>, int> = 0>
            WJR_CONSTEXPR20 static void radix_conversion(
                unsigned_biginteger<to_base>& dst,
                const T& src,
                C = C{}
            ) {
            constexpr auto tag = C{};
            Radix_conversion(dst, unsigned_view<base>(src), tag);
        }

    };

    template<size_t base>
    template<size_t index, typename T, typename U, typename C>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::QAdd(
        unsigned_biginteger_modifier<base> modifier,
        const T& lhs,
        const U& rhs,
        C
    ) {
        WASSERT_LEVEL_2(lhs.size() >= rhs.size());
        WASSERT_LEVEL_2((!index) || (index == rhs.size()));
        if constexpr (!C{}(tag::reserved)) {
            modifier.reserve(lhs.size() + 1);
        }
        WASSERT_LEVEL_1(modifier.capacity() >= lhs.size());
        auto rep = modifier.data();
        auto lp = lhs.data();
        auto ls = lhs.size();
        auto rp = rhs.data();
        auto rs = rhs.size();
        unsigned char cf = 0;
        if constexpr (index == 1) {
            cf = traits::add(rep + 0, lp[0], rp[0]);
        }
        else {
            cf = traits::add(rep, lp, rp, rs);
        }

        cf = traits::inc(cf, rep + rs, lp + rs, ls - rs, C{});
        modifier.set_size(ls);
        if (cf)modifier.push_back(1);
    }

    template<size_t base>
    template<size_t index, typename T, typename U, typename C>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::QSub(
        unsigned_biginteger_modifier<base> modifier,
        const T& lhs,
        const U& rhs,
        C
    ) {
        WASSERT_LEVEL_3(greater_equal(lhs, rhs));
        if constexpr (!C{}(tag::reserved)) {
            modifier.reserve(lhs.size());
        }
        WASSERT_LEVEL_1(modifier.capacity() >= lhs.size());
        auto rep = modifier.data();
        auto lp = lhs.data();
        auto ls = lhs.size();
        auto rp = rhs.data();
        auto rs = rhs.size();
        unsigned char cf = 0;
        if constexpr (index == 1) {
            cf = traits::sub(rep + 0, lp[0], rp[0]);
        }
        else {
            cf = traits::sub(rep, lp, rp, rs);
        }
        cf = traits::dec(cf, rep + rs, lp + rs, ls - rs, C{});
        WASSERT_LEVEL_2(cf == 0);
        modifier.set_size(ls);
        modifier.maintain();
    }

    template<size_t base>
    template<size_t index, typename T, typename U, typename C>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::slow_mul(
        unsigned_biginteger& result,
        const T& lhs,
        const U& rhs,
        C
    ) {
        constexpr size_t cache_size = 256;

        const size_t len = lhs.size() + rhs.size();

        if constexpr (index == 1) {
            if constexpr (!C{}(tag::reserved)) {
                result.reserve(len);
            }
            auto rep = result.data();
            auto res = result.size();
            auto lp = lhs.data();
            auto ls = lhs.size();
            auto rv = *rhs.data();
            auto _Val = traits::mul(rep, lp, ls, rv);
            result.set_size(ls);
            if (_Val) {
                result.push_back(_Val);
            }
        }
        else {
            auto rep = result.data();
            auto res = result.size();
            auto lp = lhs.data();
            auto ls = lhs.size();
            auto rp = rhs.data();
            auto rs = rhs.size();
            WASSERT_LEVEL_1(rs >= 2);

            if (len <= 256) {
                limb_t tar[256];
                traits::mul(tar, lp, ls, rp, rs);
                if constexpr (!C{}(tag::reserved)) {
                    result.reserve(len);
                    rep = result.data();
                }
                std::copy_n(tar, len, rep);
            }
            else {
                auto tar = (limb_ptr)malloc(len * sizeof(limb_t));
                traits::mul(tar, lp, ls, rp, rs);
                result.assign(tar, len, len, tag::store);
                rep = tar;
            }
            if (!rep[len - 1]) {
                result.set_size(len - 1);
            }
            else result.set_size(len);
        }

    }

    template<size_t base>
    template<typename T, typename U>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::karatsuba(
        unsigned_biginteger& result,
        const T& lhs,
        const U& rhs
    ) {

        size_t n = lhs.size();
        size_t m = rhs.size();
        size_t mid = n >> 1;
        WASSERT_LEVEL_1(m > mid);
        auto l_high = unsigned_view<base>(lhs, mid, n - mid);
        auto l_low = unsigned_view<base>(lhs, 0, mid);
        auto r_high = unsigned_view<base>(rhs, mid, m - mid);
        auto r_low = unsigned_view<base>(rhs, 0, mid);
        l_low.maintain();
        r_low.maintain();
        unsigned_biginteger A(n + 1, tag::uninitialized);
        unsigned_biginteger B(n + 3, tag::uninitialized);
        unsigned_biginteger C(2 * mid, tag::uninitialized);
        auto ma = unsigned_modifier<base>(A);
        auto mb = unsigned_modifier<base>(B);
        auto mc = unsigned_modifier<base>(C);

        add(ma, l_high, l_low, tag::reserved);
        add(mc, r_high, r_low, tag::reserved);
        mul(B, A, C);
        mul(A, l_high, r_high);
        mul(C, l_low, r_low);
        sub(mb, B, A, tag::reserved);
        sub(mb, B, C, tag::reserved);

        result.reserve(n + m, tag::uninitialized);
        result.set_byte(0);

        add(unsigned_modifier<base>(result, 0), unsigned_view<base>(result, 0), C, tag::reserved);
        add(unsigned_modifier<base>(result, mid), unsigned_view<base>(result, mid), B, tag::reserved);
        add(unsigned_modifier<base>(result, 2 * mid), unsigned_view<base>(result, 2 * mid), A, tag::reserved);
        result.maintain();
    }

    template<size_t base>
    template<size_t i, size_t index, typename T, typename U>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::dac_mul(
        unsigned_biginteger& result,
        const T& lhs,
        const U& rhs,
        unsigned_biginteger& temp,
        unsigned_biginteger& r
    ) {
        size_t n = lhs.size();
        size_t l = n / index;
        size_t len;
        if constexpr (i != index - 1) {
            len = l;
        }
        else {
            len = n - i * l;
        }
        auto g = unsigned_view<base>(lhs, i * l, len);
        if constexpr (i != index - 1) {
            g.maintain();
        }
        mul(temp, g, rhs);
        add(unsigned_modifier<base>(r, i * l), unsigned_view<base>(r, i * l), temp);
        if constexpr (i == index - 1) {
            result = std::move(r);
            result.maintain();
        }
        else {
            dac_mul<i + 1, index>(result, lhs, rhs, temp, r);
        }
    }

    template<size_t base>
    template<size_t index, typename T, typename U>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::dac_mul(
        unsigned_biginteger& result,
        const T& lhs,
        const U& rhs
    ) {
        size_t n = lhs.size();
        size_t m = rhs.size();
        unsigned_biginteger temp(2 * m, tag::uninitialized);
        unsigned_biginteger r(n + m, tag::uninitialized);
        r.set_byte(0);
        if constexpr (index != 0) {
            WASSERT_LEVEL_1(n >= index * m);
            dac_mul<0, index>(result, lhs, rhs, temp, r);
        }
        else {
            size_t k = (n / m) / 2 + 1;
            if ((n - 1) / k + 1 >= 2 * m) {
                ++k;
            }
            size_t floor_block = n / k;
            size_t ceil_block = (n - 1) / k + 1;
            size_t mid = k - (n % k);
            size_t pos = 0;
            for (size_t i = 0; i < k; ++i) {
                size_t block = i < mid ? floor_block : ceil_block;
                auto view = unsigned_view<base>(lhs, pos, block).maintain();
                mul(temp, view, rhs);
                add(unsigned_modifier<base>(r, pos), unsigned_view<base>(r, pos), temp);
                pos += block;
            }

            result = std::move(r);
            result.maintain();
        }
    }

    struct biginteger_fft_cache {
        biginteger_fft_cache() = default;
        ~biginteger_fft_cache() = default;
        void make(size_t n) {
            if (sz < n) {
                size_t c = sz + sz / 2;
                if (c < n)c = n;
                size_t p1 = 3 + (size_t)sqrt(c);
                size_t p2 = (c >> 2) + 2;
                auto newptr = std::make_unique<char[]>(sizeof(int) * p1 + sizeof(double) * p2);
                ptr.swap(newptr);
                p = p1;
                sz = c;
            }
        }

        int* get_ip() {
            return (int*)ptr.get();
        }

        double* get_w() {
            return (double*)(ptr.get() + sizeof(int) * p);
        }

    private:
        std::unique_ptr<char[]> ptr;
        size_t p = 0;
        size_t sz = 0;
    };

    inline biginteger_fft_cache __biginteger_fft_cache;

    template<size_t base>
    template<typename T, typename U>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::fft_mul(
        unsigned_biginteger& result,
        const T& lhs,
        const U& rhs
    ) {
        constexpr uint64_t _Maxn = (uint64_t)(1) << 49;

        size_t pn = lhs.size();
        size_t pm = rhs.size();
        size_t plen = (pn + pm) * traits::bits;
        size_t pk = 1;
        uint64_t G = base * base;
        uint64_t X = G;
        uint64_t maxn = _Maxn / G;
        // X * G * plen / (pk + 1) <= Maxn
        for (; X <= maxn / ((plen + pk) / (pk + 1)); ++pk, X *= G) {
            WASSERT_LEVEL_2(pk <= traits::bits); 
        }

        auto lhs_iter = unsigned_iterator<base>(lhs, pk, 0);
        auto rhs_iter = unsigned_iterator<base>(rhs, pk, 0);
        size_t n = (pn * traits::bits + pk - 1) / pk;
        size_t m = (pm * traits::bits + pk - 1) / pk;
        size_t len = n + m - 1;
        auto s = wjr::math::bit_ceil(len);

        auto A = std::make_unique<double[]>(s << 1);
        auto a = A.get();
        __biginteger_fft_cache.make(s << 1);
        for (size_t i = 0; i < n; ++lhs_iter, ++i) {
            auto val = (limb_t)(*lhs_iter);
            a[i << 1] = val;
            a[i << 1 | 1] = val;
        }
        for (size_t i = (n << 1); i < (s << 1); ++i) {
            a[i] = 0;
        }
        for (size_t i = 0; i < m; ++rhs_iter, ++i) {
            auto val = (limb_t)(*rhs_iter);
            a[i << 1] += val;
            a[i << 1 | 1] -= val;
        }
        for (size_t i = 0; i < n; ++i) {
            a[i << 1] *= 0.5;
            a[i << 1 | 1] *= 0.5;
        }
        for (size_t i = n; i < m; ++i) {
            a[i << 1] *= 0.5;
            a[i << 1 | 1] *= 0.5;
        }

        cdft(s << 1, 1, a, __biginteger_fft_cache.get_ip(), __biginteger_fft_cache.get_w());
        for (size_t i = 0; i < s; ++i) {
            double sa = a[i << 1];
            double sb = a[i << 1 | 1];
            a[i << 1] = sa * sa - sb * sb;
            a[i << 1 | 1] = 2 * sa * sb;
        }

        cdft(s << 1, -1, a, __biginteger_fft_cache.get_ip(), __biginteger_fft_cache.get_w());
        double invs = 1.0 / static_cast<double>(s);
        for (size_t i = 0; i < s; ++i) {
            a[i << 1] *= invs;
        }

        result.reserve(pn + pm, tag::uninitialized);
        result.set_size(pn + pm);
        std::fill_n(result.data(), pn + pm, 0);
        auto write = unsigned_iterator<base>(result, pk, 0);
        uint64_t val = 0;
        uint32_t Mod = traits::get_base_power(pk);
        WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
            biginteger_libdivider<uint32_t> divider(Mod);
        for (size_t i = 0; i < len; ++i, ++write) {
#if defined(_MSC_VER)
            val += _cvt_dtoull_fast(a[i << 1] + 0.5);
#else
            val += static_cast<uint64_t>(a[i << 1] + 0.5);
#endif
            uint64_t d = val / divider;
            (*write).zero_change(val - d * Mod);
            val = d;
        }
        while (val) {
            uint64_t d = val / divider;
            (*write).zero_change(val - d * Mod);
            val = d;
        }
        WJR_IS_NOT_CONSTANT_EVALUATED_END
        else {
        for (size_t i = 0; i < len; ++i, ++write) {
            val += static_cast<uint64_t>(a[i << 1] + 0.5);
            uint64_t d = val / Mod;
            (*write).zero_change(val - d * Mod);
            val = d;
        }
        while (val) {
            uint64_t d = val / Mod;
            (*write).zero_change(val - d * Mod);
            val = d;
        }
        }
        result.maintain();
    }

    template<size_t base>
    template<size_t index, typename T, typename U>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::QMul(
        unsigned_biginteger& result,
        const T& lhs,
        const U& rhs
    ) {
        if constexpr (index == 1) {
            if (lhs.test_byte(0) || rhs.test_byte(0)) {
                result.set_byte(0);
                return;
            }
            slow_mul<index>(result, lhs, rhs);
        }
        else {
            size_t n = lhs.size();
            size_t m = rhs.size();
            constexpr auto mul_info = traits::threshold::get_mul_threshold();
            if (m <= mul_info.slow_threshold) {
                slow_mul<index>(result, lhs, rhs);
            }
            else {
                if (m <= mul_info.karatsuba_threshold) {
                    if (n >= 2 * m) {
                        dac_mul<0>(result, lhs, rhs);
                    }
                    else {
                        karatsuba(result, lhs, rhs);
                    }
                }
                else {
                    if (n >= 15 * m) {
                        dac_mul<4>(result, lhs, rhs);
                    }
                    else {
                        if (n >= 7 * m) {
                            dac_mul<2>(result, lhs, rhs);
                        }
                        else {
                            fft_mul(result, lhs, rhs);
                        }
                    }
                }
            }
        }
    }

    template<size_t base>
    template<typename C>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::Random(
        unsigned_biginteger_modifier<base> modifier,
        size_t n,
        C
    ) {
        if (!n) {
            modifier.reserve(1);
            modifier.set_size(1);
            *modifier.data() = 0;
            return;
        }
        size_t k = (n - 1) / traits::bits;
        size_t r = (n - 1) % traits::bits + 1;
        modifier.reserve(k + 1);
        modifier.set_size(k + 1);
        auto p = modifier.data();
        for (size_t i = 0; i != k; ++i) {
            p[i] = traits::random();
        }
        limb_t _Max;
        if (r != traits::bits) {
            _Max = traits::get_base_power(r) - 1;
        }
        else {
            _Max = traits::max();
        }
        limb_t _Min;
        if constexpr (C{}(tag::exact)) {
            _Min = traits::get_base_power(r - 1);
        }
        else {
            _Min = 0;
        }
        std::uniform_int_distribution<limb_t> rt(_Min, _Max);
        p[k] = rt(traits::random_device());
        if constexpr (!C{}(tag::exact)) {
            modifier.maintain();
        }
    }

    template<size_t base>
    template<typename T>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::Interval_random(
        unsigned_biginteger_modifier<base> modifier,
        const T& _Max
    ) {
        auto p = _Max.data();
        size_t n = _Max.size();
        std::uniform_int_distribution<limb_t> head(0, p[n - 1]);
        modifier.reserve(n);
        modifier.set_size(n);
        auto rep = modifier.data();
        while (true) {
            size_t pos = n - 1;
            rep[pos] = head(traits::random_device());
            while (rep[pos] == p[pos] && pos) {
                --pos;
                rep[pos] = traits::random();
            }
            if (rep[pos] > p[pos])continue;
            while (pos) {
                --pos;
                rep[pos] = traits::random();
            }
            break;
        }
        modifier.maintain();
    }

    template<size_t base>
    template<size_t to_base, typename T>
    WJR_CONSTEXPR20  void unsigned_biginteger<base>::Binary_radix_conversion(
        unsigned_biginteger<to_base>& dst,
        const T& src,
        std::vector<std::pair<size_t, unsigned_biginteger<to_base>>>& cache
    ) {
        size_t n = src.size();
        if (n == 1) {
            dst = src.data()[0];
            return;
        }
        constexpr auto use_cache = [](unsigned_biginteger<to_base>& r, auto& chc, size_t k) {
            constexpr auto single_val = biginteger_traits<base>::max_value();
            auto first_try = std::lower_bound(chc.begin(), chc.end(), k,
                [](const auto& x, auto y) {return x.first < y; });
            if (first_try != chc.end() && first_try->first == k) {
                unsigned_biginteger<to_base>::mul(r, r, first_try->second);
            }
            else {
                auto second_try = std::lower_bound(chc.begin(), chc.end(), k / 2,
                    [](const auto& x, auto y) {return x.first < y; });
                WASSERT_LEVEL_2(second_try != chc.end() && second_try->first == k / 2);
                unsigned_biginteger<to_base> _Val;
                unsigned_biginteger<to_base>::mul(_Val, second_try->second, second_try->second);
                if (k & 1) {
                    WASSERT_LEVEL_2(chc.size() >= 1 && chc[0].first == 1);
                    unsigned_biginteger<to_base>::mul(_Val, _Val, chc[0].second);
                }
                unsigned_biginteger<to_base>::mul(r, r, _Val);
                chc.emplace(first_try, std::make_pair(k, std::move(_Val)));
            }
        };
        size_t mid = n >> 1;
        auto lsrc = unsigned_view<base>(src, mid, n - mid);
        auto rsrc = unsigned_view<base>(src, 0, mid).maintain();
        Binary_radix_conversion(dst, lsrc, cache);
        unsigned_biginteger<to_base> temp;
        Binary_radix_conversion(temp, rsrc, cache);
        use_cache(dst, cache, mid);
        unsigned_biginteger<to_base>::add(dst, dst, temp);
    }

    template<size_t base>
    template<size_t to_base, typename T, typename C>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::Radix_conversion(
        unsigned_biginteger<to_base>& dst,
        const T& src,
        C
    ) {
        constexpr auto type = __radix_conversion_helper<base, to_base>::value;
        if constexpr (type == radix_conversion_type::same) {
            dst = src;
        }
        else if constexpr (type == radix_conversion_type::multiple) {
            if constexpr (biginteger_traits<base>::max() == biginteger_traits<to_base>::max()) {
                dst.assign(src.data(), src.data() + src.size());
            }
            else {
                if constexpr (base < to_base) {
                    constexpr int k = math::force_constexpr::integer_log(std::integral_constant<size_t, base>{}, to_base);
                    size_t old_size = src.size();
                    size_t old_max_bits = old_size * traits::bits;
                    size_t new_max_bits = (old_max_bits - 1) / k + 1;
                    size_t new_max_size = (new_max_bits - 1) / biginteger_traits<to_base>::bits + 1;
                    dst.reserve(new_max_size);
                    dst.set_size(new_max_size);
                    auto reader = unsigned_iterator<base>(src, k);
                    auto write = unsigned_iterator<base>(dst, 1);
                    while (reader.vaild()) {
                        *write = *reader;
                        ++reader;
                        ++write;
                    }
                    dst.maintain();
                }
                else {
                    constexpr int k = math::force_constexpr::integer_log(std::integral_constant<size_t, to_base>{}, base);
                    size_t old_size = src.size();
                    size_t new_max_size = old_size * k;
                    dst.reserve(new_max_size);
                    dst.set_size(new_max_size);
                    auto reader = unsigned_iterator<base>(src, 1);
                    auto write = unsigned_iterator<base>(dst, k);
                    while (reader.vaild()) {
                        *write = *reader;
                        ++reader;
                        ++write;
                    }
                    dst.maintain();
                }
            }
        }
        else {
            std::vector<std::pair<size_t, unsigned_biginteger<to_base>>> cache;
            cache.reserve(30);
            cache.emplace_back(std::make_pair(1,
                unsigned_biginteger<to_base>(biginteger_traits<base>::max_value())));
            Binary_radix_conversion(dst, src, cache);
        }
    }

    template<size_t base>
    const inline unsigned_biginteger<base> unsigned_biginteger_zero = unsigned_biginteger<base>(0u);

    template<size_t base>
    WJR_CONSTEXPR20 unsigned_biginteger_view<base>& unsigned_biginteger_view<base>::maintain() {
        if (unlikely(m_size == 0)) {
            m_ptr = std::addressof(unsigned_biginteger_zero<base>);
            m_offset = 0;
            m_size = 1;
            return *this;
        }
        auto ptr = data();
        while (m_size != 1 && ptr[m_size - 1] == 0)--m_size;
        return *this;
    }

#define BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION                                                 \
    template<typename T, typename U, size_t _Base =                                             \
            __biginteger_base<std::decay_t<T>, std::decay_t<U>>,                                \
            std::enable_if_t<__is_accepted_base_v<_Base>, int> = 0>

#define BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION                                                  \
    template<typename T, typename U, size_t _Base,                                              \
            std::enable_if_t<__is_accepted_base_v<_Base>, int> >

    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 int cmp(const T& lhs, const U& rhs);
    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 void add(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        );
    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 void sub(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        );
    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 void mul(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        );

    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 bool operator==(const T& lhs, const U& rhs);
    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 bool operator!=(const T& lhs, const U& rhs);
    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 bool operator<(const T& lhs, const U& rhs);
    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 bool operator<=(const T& lhs, const U& rhs);
    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 bool operator>(const T& lhs, const U& rhs);
    BIGINTEGER_VIEW_BINARY_AUTO_DECLARATION
        WJR_CONSTEXPR20 bool operator>=(const T& lhs, const U& rhs);

    template<size_t _Base>
    WJR_CONSTEXPR20 void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);

    // generate a biginteger of length n
    template<size_t _Base, typename C = tag::empty_tag, std::enable_if_t<tag::is_tag_set_v<C>, int> = 0>
    WJR_CONSTEXPR20 void random(biginteger<_Base>& result, size_t n, C = C{});

    template<size_t _Base, typename C = tag::empty_tag, std::enable_if_t<tag::is_tag_set_v<C>, int> = 0>
    WJR_CONSTEXPR20 biginteger<_Base> random(size_t n, C = C{});

    template<size_t _Base, typename T,
        std::enable_if_t<std::conjunction_v<
        __Can_make_signed_view<_Base, T>>, int> = 0>
        WJR_CONSTEXPR20 void interval_random(biginteger<_Base>& result, const T& _Max);

    template<size_t _Base, typename T,
        std::enable_if_t<std::conjunction_v<
        __Can_make_signed_view<_Base, T>>, int> = 0>
        WJR_CONSTEXPR20 biginteger<_Base> interval_random(const T& _Max);

    template<size_t _Base, typename T, typename U,
        std::enable_if_t<std::conjunction_v<
        __Can_make_signed_view<_Base, T>,
        __Can_make_signed_view<_Base, U>>, int> = 0>
        WJR_CONSTEXPR20 void interval_random(biginteger<_Base>& result, const T& _Min, const U& _Max);

    template<size_t _Base, typename T, typename U,
        std::enable_if_t<std::conjunction_v<
        __Can_make_signed_view<_Base, T>,
        __Can_make_signed_view<_Base, U>>, int> = 0>
        WJR_CONSTEXPR20 biginteger<_Base> interval_random(const T& _Min, const U& _Max);

    template<size_t to_base, typename T, typename C = tag::empty_tag,
        size_t base = __biginteger_base<std::decay_t<T>>,
        std::enable_if_t<
        std::conjunction_v<
        __Can_make_signed_view<base, T>,
        tag::is_tag_set<C>,
        __is_accepted_base<base>>, int> = 0>
        WJR_CONSTEXPR20 void radix_conversion(biginteger<to_base>& dst, const T& src, C = C{});

    template<size_t to_base, typename T, typename C = tag::empty_tag,
        size_t base = __biginteger_base<std::decay_t<T>>,
        std::enable_if_t<
        std::conjunction_v<
        __Can_make_signed_view<base, T>,
        tag::is_tag_set<C>,
        __is_accepted_base<base>>, int> = 0>
        WJR_CONSTEXPR20 biginteger<to_base> radix_conversion(std::in_place_index_t<to_base>, const T& src, C = C{});

    template<size_t to_base, typename T, typename C = tag::empty_tag,
        size_t base = __biginteger_base<std::decay_t<T>>,
        std::enable_if_t<
        std::conjunction_v<
        __Can_make_signed_view<base, T>,
        tag::is_tag_set<C>,
        __is_accepted_base<base>>, int> = 0>
        WJR_CONSTEXPR20 biginteger<to_base> radix_conversion(const T& src, C = C{});

    template<size_t base>
    class biginteger {
        template<size_t _Base>
        friend class biginteger;
        template<size_t _Base>
        friend class unsigned_biginteger;

        using traits = biginteger_traits<base>;
        using unsigned_traits = unsigned_biginteger<base>;

        template<typename T>
        struct __Can_move : std::conjunction<
            std::is_rvalue_reference<T>,
            std::disjunction<
            std::is_same<std::decay_t<T>, biginteger<base>>,
            std::is_same<std::decay_t<T>, unsigned_biginteger<base>>
            >
        > {};

        template<typename T>
        constexpr static bool __Can_move_v = __Can_move<T>::value;

    public:

        using value_type = limb_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using allocator_type = mallocator<value_type>;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        using bit_iterator = unsigned_biginteger_iterator<base>;
        using bit_const_iterator = unsigned_biginteger_const_iterator<base>;
        using bit_reverse_iterator = std::reverse_iterator<bit_iterator>;
		using bit_const_reverse_iterator = std::reverse_iterator<bit_const_iterator>;
        using bit_reference = unsigned_biginteger_reference<base>;
		using bit_const_reference = unsigned_biginteger_const_reference<base>;

        WJR_CONSTEXPR20 biginteger() = default;
        WJR_CONSTEXPR20 biginteger(const biginteger&) = default;
        WJR_CONSTEXPR20 biginteger(biginteger&&) noexcept = default;
        WJR_CONSTEXPR20 explicit biginteger(const unsigned_biginteger<base>& other)
            : ubint(other) {}
        WJR_CONSTEXPR20 explicit biginteger(unsigned_biginteger<base>&& other) noexcept
            : ubint(std::move(other)) {}
        WJR_CONSTEXPR20 explicit biginteger(bool _Signal, const unsigned_biginteger<base>& other)
            : _Signal(_Signal), ubint(other) {}
        WJR_CONSTEXPR20 explicit biginteger(bool _Signal, unsigned_biginteger<base>&& other)
            : _Signal(_Signal), ubint(std::move(other)) {}

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 biginteger(iter _First, iter _Last)
            : ubint(_First, _Last) {}
        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 biginteger(bool _Signal, iter _First, iter _Last)
            : _Signal(_Signal), ubint(_First, _Last) {}

        template<typename T, std::enable_if_t<
            __Is_signed_biginteger_view_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 explicit biginteger(const T& rhs)
            : _Signal(rhs.signal()), ubint(rhs.get_unsigned()) {}
        template<typename T, std::enable_if_t<
            __Need_to_make_signed_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 explicit biginteger(const T& val)
            : biginteger(signed_view<base>(val)) {}

		WJR_CONSTEXPR20 biginteger(limb_ptr ptr, size_t _Size, size_t _Capacity, tag::store_tag)
			: ubint(ptr, _Size, _Capacity, tag::store) {}
        WJR_CONSTEXPR20 biginteger(size_t _Count, tag::uninitialized_tag)
            : ubint(_Count, tag::uninitialized) {}

        template<typename iter, typename Func, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 void in_bits(iter _First, iter _Last, Func&& fn) {
            size_t n = std::distance(_First, _Last);
            if (!n) {
                set_byte(0);
            }
            else {
                auto ch = fn(*_First);
                if (ch == '-' || ch == '+') {
                    _Signal = (ch == '+');
                    ++_First;
                }
                ubint.in_bits(_First, _Last, std::forward<Func>(fn));
            }
        }

        template<typename iter, typename Func, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 std::optional<iter> out_bits(iter _First, iter _Last, Func&& fn) const {
            if (_First == _Last)return {};
            if (!_Signal) {
                *_First = '-';
                ++_First;
            }
            return ubint.out_bits(_First, _Last, std::forward<Func>(fn));
        }

        WJR_CONSTEXPR20 biginteger& operator=(const biginteger&) = default;
        WJR_CONSTEXPR20 biginteger& operator=(biginteger&&) noexcept = default;
        template<typename T, std::enable_if_t<
            __Is_signed_biginteger_view_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 biginteger& operator=(const T& rhs) {
            _Signal = rhs.signal();
            ubint = rhs.get_unsigned();
            return *this;
        }
        template<typename T, std::enable_if_t<
            __Need_to_make_signed_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 biginteger& operator=(const T& rhs) {
            (*this) = signed_view<base>(rhs);
            return *this;
        }
        WJR_CONSTEXPR20 biginteger& operator=(unsigned_biginteger<base>&& other) {
            _Signal = true;
            ubint = std::move(other);
            return *this;
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 biginteger& assign(iter _First, iter _Last) {
            ubint.assign(_First, _Last);
            return *this;
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        WJR_CONSTEXPR20 biginteger& assign(bool _Sgn, iter _First, iter _Last) {
            _Signal = _Sgn;
            ubint.assign(_First, _Last);
            return *this;
        }

		WJR_CONSTEXPR20 biginteger& assign(limb_ptr ptr, size_t _Size, size_t _Capacity, tag::store_tag) {
			ubint.assign(ptr, _Size, _Capacity, tag::store);
			return *this;
		}

        WJR_CONSTEXPR20 void swap(biginteger& other) noexcept {
            std::swap(_Signal, other._Signal);
            ubint.swap(other.ubint);
        }

        WJR_CONSTEXPR20 unsigned_biginteger<base>& get_unsigned() {
            return ubint;
        }
        WJR_CONSTEXPR20 const unsigned_biginteger<base>& get_unsigned() const {
            return ubint;
        }

        WJR_CONSTEXPR20 size_t size() const { return ubint.size(); }
        WJR_CONSTEXPR20 size_t capacity() const { return ubint.capacity(); }
        WJR_CONSTEXPR20 pointer data() noexcept { return ubint.data(); }
        WJR_CONSTEXPR20 const_pointer data() const noexcept { return ubint.data(); }
        WJR_CONSTEXPR20 iterator begin() noexcept { return ubint.begin(); }
        WJR_CONSTEXPR20 const_iterator begin() const noexcept { return ubint.begin(); }
        WJR_CONSTEXPR20 const_iterator cbegin() const noexcept { return ubint.cbegin(); }
        WJR_CONSTEXPR20 iterator end() noexcept { return ubint.end(); }
        WJR_CONSTEXPR20 const_iterator end() const noexcept { return ubint.end(); }
        WJR_CONSTEXPR20 const_iterator cend() const noexcept { return ubint.cend(); }
        WJR_CONSTEXPR20 reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        WJR_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        WJR_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
        WJR_CONSTEXPR20 reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        WJR_CONSTEXPR20 const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        WJR_CONSTEXPR20 const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
        WJR_CONSTEXPR20 reference operator[](size_type _Index) noexcept { return ubint[_Index]; }
        WJR_CONSTEXPR20 const_reference operator[](size_type _Index) const { return ubint[_Index]; }
        WJR_CONSTEXPR20 reference front() noexcept { return ubint.front(); }
        WJR_CONSTEXPR20 const_reference front() const noexcept { return ubint.front(); }
        WJR_CONSTEXPR20 reference back() noexcept { return ubint.back(); }
        WJR_CONSTEXPR20 const_reference back() const noexcept { return ubint.back(); }

        WJR_CONSTEXPR20 size_t bit_size() const { return ubint.bit_size(); }
        WJR_CONSTEXPR20 bit_iterator bit_begin() noexcept { 
            return unsigned_iterator<base>(*this, 1, 0); 
        }
		WJR_CONSTEXPR20 bit_const_iterator bit_begin() const noexcept { 
            return unsigned_iterator<base>(*this, 1, 0); 
        }
		WJR_CONSTEXPR20 bit_const_iterator bit_cbegin() const noexcept { 
            return unsigned_iterator<base>(*this, 1, 0); 
        }
		WJR_CONSTEXPR20 bit_iterator bit_end() noexcept { 
            return unsigned_iterator<base>(*this, 1, bit_size()); 
        }
		WJR_CONSTEXPR20 bit_const_iterator bit_end() const noexcept { 
            return unsigned_iterator<base>(*this, 1, bit_size()); 
        }
		WJR_CONSTEXPR20 bit_const_iterator bit_cend() const noexcept { 
            return unsigned_iterator<base>(*this, 1, bit_size()); 
        }
		WJR_CONSTEXPR20 bit_reverse_iterator bit_rbegin() noexcept { 
            return bit_reverse_iterator(bit_end()); 
        }
		WJR_CONSTEXPR20 bit_const_reverse_iterator bit_rbegin() const noexcept { 
            return bit_const_reverse_iterator(bit_end()); 
        }
		WJR_CONSTEXPR20 bit_const_reverse_iterator bit_crbegin() const noexcept { 
            return bit_const_reverse_iterator(bit_end()); 
        }
		WJR_CONSTEXPR20 bit_reverse_iterator bit_rend() noexcept { 
            return bit_reverse_iterator(bit_begin()); 
        }
		WJR_CONSTEXPR20 bit_const_reverse_iterator bit_rend() const noexcept { 
            return bit_const_reverse_iterator(bit_begin()); 
        }
		WJR_CONSTEXPR20 bit_const_reverse_iterator bit_crend() const noexcept { 
            return bit_const_reverse_iterator(bit_begin()); 
        }
		WJR_CONSTEXPR20 bit_reference operator()(const size_type n) { return bit_begin()[n]; }
		WJR_CONSTEXPR20 bit_const_reference operator()(const size_type n) const { return bit_begin()[n]; }
		WJR_CONSTEXPR20 bit_reference bit_front() { return *bit_begin(); }
		WJR_CONSTEXPR20 bit_const_reference bit_front() const { return *bit_begin(); }
		WJR_CONSTEXPR20 bit_reference bit_back() { return *(--bit_end()); }
		WJR_CONSTEXPR20 bit_const_reference bit_back() const { return *(--bit_end()); }

        WJR_CONSTEXPR20 void reserve(const size_type n) { ubint.reserve(n); }
        WJR_CONSTEXPR20 void bit_reserve(const size_type n) { ubint.reserve((n + traits::bits - 1) / traits::bits); }
        WJR_CONSTEXPR20 bool test_byte(const value_type val) { return ubint.test_byte(val); }
        WJR_CONSTEXPR20 void set_byte(const value_type val) { ubint.set_byte(val); }
        WJR_CONSTEXPR20 bool signal() const noexcept { return _Signal; }
        WJR_CONSTEXPR20 void maintain_signal() { _Signal |= ubint.test_byte(0); }
        WJR_CONSTEXPR20 void set_size(const size_type n) { ubint.set_size(n); }
        WJR_CONSTEXPR20 biginteger& maintain() {
            ubint.maintain();
            maintain_signal();
            return *this;
        }
        WJR_CONSTEXPR20 void negate() { _Signal = (!_Signal) || ubint.test_byte(0); }
        WJR_CONSTEXPR20 void set_signal(bool f) { _Signal = f || ubint.test_byte(0); }
        WJR_CONSTEXPR20 void abs() { _Signal = true; }

        WJR_CONSTEXPR20 biginteger& mul_base_power(size_t index) {
            ubint.mul_base_power(index);
            return *this;
        }

        WJR_CONSTEXPR20 biginteger& div_base_power(size_t index) {
            ubint.div_base_power(index);
            maintain_signal();
            return *this;
        }

        WJR_CONSTEXPR20 biginteger& mod_base_power(size_t index) {
            ubint.mod_base_power(index);
            return *this;
        }

        WJR_CONSTEXPR20 String tostr() const {
            size_t n = bit_size();
            String str;
            str.reserve(n + 1);
            str.set_size(n + 1);
            auto it = out_bits(str.begin(), str.end(), traits::to_string);
            size_t g = it.value() - str.begin();
            str.set_size(g);
            return str;
        }

    private:

        template<typename T, typename U>
        WJR_CONSTEXPR20 static int signed_cmp(
            const T& lhs,
            const U& rhs) {
            // if lhs < rhs , return -1
            // if lhs > rhs , return 1
            // if lhs == rhs , return 0
            int f = lhs.signal() != rhs.signal() ?
                1 : unsigned_traits::cmp(lhs.get_unsigned(), rhs.get_unsigned());
            return lhs.signal() ? f : -f;
        }

        template<typename T, typename U>
        WJR_CONSTEXPR20 static void unsigned_add(
            biginteger& result,
            const T& lhs,
            const U& rhs
        ) {
            unsigned_traits::add(unsigned_modifier<base>(result.ubint), lhs.get_unsigned(), rhs.get_unsigned());
            result._Signal = lhs.signal();
        }

        template<typename T, typename U>
        WJR_CONSTEXPR20 static void unsigned_sub(
            biginteger& result,
            const T& lhs,
            const U& rhs
        ) {
            int _cmp = unsigned_traits::cmp(lhs.get_unsigned(), rhs.get_unsigned());
            if (_cmp > 0) {
                unsigned_traits::sub(unsigned_modifier<base>(result.ubint), lhs.get_unsigned(), rhs.get_unsigned());
                result._Signal = lhs.signal();
            }
            else if (_cmp < 0) {
                unsigned_traits::sub(unsigned_modifier<base>(result.ubint), rhs.get_unsigned(), lhs.get_unsigned());
                result._Signal = !lhs.signal();
            }
            else {
                result.set_byte(0);
            }
        }

        template<typename T, typename U>
        WJR_CONSTEXPR20 static void signed_add(
            biginteger& result,
            const T& lhs,
            const U& rhs
        ) {
            if (lhs.signal() == rhs.signal()) {
                unsigned_add(result, lhs, rhs);
            }
            else {
                unsigned_sub(result, lhs, rhs);
            }
        }

        template<typename T, typename U>
        WJR_CONSTEXPR20 static void signed_sub(
            biginteger& result,
            const T& lhs,
            const U& rhs
        ) {
            if (lhs.signal() == rhs.signal()) {
                unsigned_sub(result, lhs, rhs);
            }
            else {
                unsigned_add(result, lhs, rhs);
            }
        }

        enum class div_mode {
            none,
            lhs_is_power_of_base,
        };

        template<limb_t ir>
        WJR_CONSTEXPR20 static limb_t one_divmod(
            biginteger& lhs
        ) {
            size_t n = lhs.size();
            auto p = lhs.data();
            double_limb_t _Val = 0;
            for (size_t i = n; i--;) {
                _Val = _Val * traits::max_value() + p[i];
                p[i] = static_cast<limb_t>(_Val / ir);
                _Val = _Val - p[i] * ir;
            }
            if (n != 1 && !p[n - 1]) {
                lhs.ubint.set_size(n - 1);
                WASSERT_LEVEL_1(lhs.ubint.back() != 0);
            }
            return static_cast<limb_t>(_Val);
        }

        WJR_CONSTEXPR20 static limb_t one_divmod(
            biginteger& lhs,
            limb_t v) {
            size_t n = lhs.size();
            auto p = lhs.data();
            WASSERT_LEVEL_1(v != 0);

            limb_t _Val = traits::div(p, p, n, v);

            if (n != 1 && !p[n - 1]) {
                lhs.ubint.set_size(n - 1);
                WASSERT_LEVEL_1(lhs.ubint.back() != 0);
            }
            return _Val;
        }

        template<div_mode mode>
        WJR_CONSTEXPR20 static void knuth_divmod(biginteger& lhs, biginteger& rhs) {
            auto vr = rhs.data()[rhs.size() - 1];
            auto kth = static_cast<limb_t>(((traits::max_value() >> 1) + vr - 1) / vr);
            if (kth != 1) {
                auto val = traits::mul(lhs.data(), lhs.data(), lhs.size(), kth);
                if (val)lhs.ubint.push_back(val);
                val = traits::mul(rhs.data(), rhs.data(), rhs.size(), kth);
                if (val) rhs.ubint.push_back(val);
            }
            auto pl = lhs.data();
            size_t n = rhs.size();
            size_t m = lhs.size() - rhs.size();

            auto val = rhs.data()[n - 1];
            WASSERT_LEVEL_1(val >= (traits::max_value() >> 1));
            libdivide::divider<limb_t> _Divider1(val);
            biginteger_libdivider<limb_t> _Divider2(val);
            biginteger result;
            result.ubint.reserve(m + 1);
            result.ubint.set_size(m + 1);
            auto presult = result.data();
            biginteger r;
            r.ubint.reserve(n + 1);
            r.assign(pl + 0, pl + n + m);

            biginteger temp;
            temp.ubint.reserve(n + 1);
            pl = pl + m - 1;
            for (size_t j = m + 1; j--; --pl) {
                limb_t q = 0;
                auto mr = unsigned_modifier<base>(r.ubint, j);
                mr.maintain();
                if (mr.size() == n + 1) {
                    q = std::min(traits::max(),
                        static_cast<limb_t>((traits::max_value() * mr.data()[n] + mr.data()[n - 1]) / _Divider2));
                    auto _Val = traits::mul(temp.data(), rhs.data(), rhs.size(), q);
                    temp.ubint.set_size(rhs.size());
                    if (_Val) temp.ubint.push_back(_Val);
                    auto cf = traits::sub(mr.data(), mr.data(), mr.size(), temp.data(), temp.size());
                    if (cf) {
                        do {
                            --q;
                            cf = traits::add(mr.data(), mr.data(), mr.size(), rhs.data(), rhs.size());
                        } while (!cf);
                    }
                }
                else if (r.size() == n) {
                    if (mr.data()[n - 1] > val) {
                        q = 1;
                        unsigned_traits::sub(mr, mr, rhs.ubint, tag::reserved);
                    }
                    else if (mr.data()[n - 1] == val) {
                        q = 1;
                        auto cf = traits::sub(mr.data(), mr.data(), mr.size(), rhs.data(), rhs.size());
                        if (cf) {
                            q = 0;
                            traits::add(mr.data(), mr.data(), mr.size(), rhs.data(), rhs.size());
                        }
                    }
                }
                presult[j] = q;
            }
            result.maintain();
            lhs = std::move(result);
            rhs = std::move(r);

            if (kth != 1) {
                one_divmod(rhs, kth);
            }
        }

        template<div_mode mode>
        WJR_CONSTEXPR20 static void small_divmod(biginteger& lhs, biginteger& rhs) {
            size_t n = lhs.size();
            size_t m = rhs.size();
            size_t k = m << 1;
            size_t l = n;

            biginteger temp;

            biginteger E;
            E.ubint.reserve(k + 2);
            E.ubint.set_size(k + 2);
            std::fill_n(E.ubint.data(), k + 1, 0);
            E.ubint[k + 1] = 1;
            biginteger mo(rhs);
            quick_divmod<div_mode::lhs_is_power_of_base>(E, mo);
            size_t g = k + 1 - E.size();
            biginteger ans;
            ans.ubint.reserve(n - m + 1);
            ans.ubint.set_size(n - m + 1);
            std::fill_n(ans.ubint.data(), n - m + 1, 0);
            biginteger mid_result;
            mo.set_byte(0);

            while (l) {
                size_t ml = std::min(l, k - mo.size());
                mo.mul_base_power(ml * traits::bits);
                auto cl = signed_view<base>(lhs, l - ml, ml).maintain().abs();
                if (mo.size() < cl.size()) {
                    mo.ubint.reserve(cl.size());
                    mo.ubint.set_size(cl.size());
                }
                std::copy_n(cl.data(), cl.size(), mo.data());
                if (mo.size() > g) {
                    mul(mid_result, signed_view<base>(mo, g, mo.size() - g).abs(), E);
                    mid_result.div_base_power(E.size() * traits::bits);
                }
                else {
                    mid_result.set_byte(0);
                }
                mul(temp, mid_result, rhs);
                sub(mo, mo, temp);
#if WDEBUG_LEVEL >= 1
                size_t step = 0;
#endif
                while (!mo.signal()) {
                    --mid_result;
                    mo += rhs;
                    WASSERT_LEVEL_1(++step <= 2);
                }
#if WDEBUG_LEVEL >= 1
                step = 0;
#endif
                while (mo >= rhs) {
                    ++mid_result;
                    mo -= rhs;
                    WASSERT_LEVEL_1(++step <= 2);
                }
                std::copy_n(mid_result.data(), mid_result.size(), ans.data() + l - ml);
                l -= ml;
            }
            lhs = std::move(ans);
            lhs.maintain();
            rhs = std::move(mo);
        }

        template<div_mode mode>
        WJR_CONSTEXPR20 static void mid_divmod(biginteger& lhs, biginteger& rhs) {
            size_t n = lhs.size();
            size_t m = rhs.size();
            size_t mid = (n - m) >> 1;
            size_t p = 2 * m - (n - mid) - 2;
            size_t k = n - mid - p;

            biginteger temp;

            biginteger E;
            // initialize E as base ^ ((k+1) * bit_length)
            E.ubint.reserve(k + 2);
            E.ubint.set_size(k + 2);
            std::fill_n(E.ubint.data(), k + 1, 0);
            E.ubint[k + 1] = 1;

            biginteger ans1(signed_view<base>(rhs, p, m - p).abs());
            quick_divmod<div_mode::lhs_is_power_of_base>(E, ans1);
            size_t g = k + 1 - E.size();
            if constexpr (mode == div_mode::none) {
                mul(ans1, signed_view<base>(lhs, mid + p + g, k - g).abs(), E);
                ans1.div_base_power(E.size() * traits::bits);
            }
            else {
                size_t bl = lhs.bit_size();
                ans1 = E;
                ans1.div_base_power(2 * traits::bits - ((bl - 1) % traits::bits));
            }

            mul(temp, ans1, rhs);

            biginteger mo1;
            sub(mo1, signed_view<base>(lhs, mid, n - mid).abs(), temp);

#if WDEBUG_LEVEL >= 1
            size_t step = 0;
#endif
            while (!mo1.signal()) {
                --ans1;
                mo1 += rhs;
                WASSERT_LEVEL_1((++step) <= 2);
            }
#if WDEBUG_LEVEL >= 1
            step = 0;
#endif
            while (mo1 >= rhs) {
                ++ans1;
                mo1 -= rhs;
                WASSERT_LEVEL_1((++step) <= 2);
            }

            mo1.mul_base_power(mid * traits::bits);
            if constexpr (mode == div_mode::none) {
                auto view = signed_view<base>(lhs, 0, mid).maintain();
                if (mo1.size() < view.size()) {
                    mo1.ubint.reserve(view.size());
                    mo1.ubint.set_size(view.size());
                }
                std::copy_n(view.data(), view.size(), mo1.data());
            }
            else {
#if WDEBUG_LEVEL >= 2
                for (size_t i = 0; i < mid; ++i)WASSERT_LEVEL_2(lhs.data()[i] == 0);
#endif
            }

            lhs = std::move(ans1);
            lhs.mul_base_power(mid * traits::bits);

            size_t ml = std::min(p + g, mo1.size());
            biginteger ans2;
            mul(ans2, signed_view<base>(mo1, p + g, mo1.size() - ml).maintain().abs(), E);
            ans2.div_base_power(E.size() * traits::bits);
            mul(temp, rhs, ans2);
            sub(mo1, mo1, temp);

            if (lhs.size() < ans2.size()) {
                lhs.ubint.reserve(ans2.size());
                lhs.ubint.set_size(ans2.size());
            }
            std::copy_n(ans2.data(), ans2.size(), lhs.data());

#if WDEBUG_LEVEL >= 1
            step = 0;
#endif
            while (!mo1.signal()) {
                --lhs;
                mo1 += rhs;
                WASSERT_LEVEL_1((++step) <= 2);
            }
#if WDEBUG_LEVEL >= 1
            step = 0;
#endif
            while (mo1 >= rhs) {
                ++lhs;
                mo1 -= rhs;
                WASSERT_LEVEL_1((++step) <= 2);
            }

            rhs = std::move(mo1);
        }

        template<div_mode mode>
        WJR_CONSTEXPR20 static void large_divmod(biginteger& lhs, biginteger& rhs) {
            size_t n = lhs.size();
            size_t m = rhs.size();
            size_t mid = 2 * m - n - 2;
            biginteger copyA(signed_view<base>(lhs, mid, n - mid).abs());
            biginteger copyB(signed_view<base>(rhs, mid, m - mid).abs());
            ++copyA;
            quick_divmod<div_mode::none>(copyA, copyB);
            --copyB;

            auto view = signed_view<base>(lhs, 0, mid).maintain().abs();
            if (!copyB.signal()) {
                copyB.mul_base_power(mid * traits::bits);
                copyB += view;
            }
            else {
                copyB.mul_base_power(mid * traits::bits);
                if (copyB.size() < view.size()) {
                    copyB.ubint.reserve(view.size());
                    copyB.ubint.set_size(view.size());
                }
                std::copy_n(view.data(), view.size(), copyB.data());
            }

            mul(lhs, signed_view<base>(rhs, 0, mid).maintain().abs(), copyA);
            sub(copyB, copyB, lhs);
            if (!copyB.signal()) {
                --copyA;
                copyB += rhs;
            }

            lhs = std::move(copyA);
            rhs = std::move(copyB);
        }

        template<div_mode mode = div_mode::none>
        WJR_CONSTEXPR20 static void quick_divmod(biginteger& lhs, biginteger& rhs) {
            WASSERT_LEVEL_1(!rhs.test_byte(0) && lhs.signal() && rhs.signal());
            WASSERT_LEVEL_1(std::addressof(lhs) != std::addressof(rhs));
            int cmp = unsigned_biginteger<base>::cmp(lhs.ubint, rhs.ubint);
            if (cmp < 0) {
                lhs.swap(rhs);
                lhs.set_byte(0);
                return;
            }
            if (cmp == 0) {
                lhs.set_byte(1u);
                rhs.set_byte(0);
                return;
            }
            size_t n = lhs.size();
            size_t m = rhs.size();
#if WDEBUG_LEVEL >= 3
            biginteger p(lhs), q(rhs);
#endif
            if (m == 1) {
                rhs = one_divmod(lhs, rhs.data()[0]);
            }
            else if (m <= 8 || (m <= 64 && ((size_t)(1) << ((m >> 1) - 4)) <= n)) {
                knuth_divmod<mode>(lhs, rhs);
            }
            else if (n * 3 <= m * 5) {
                large_divmod<mode>(lhs, rhs);
            }
            else if (n * 2 >= m * 5) {
                small_divmod<mode>(lhs, rhs);
            }
            else {
                mid_divmod<mode>(lhs, rhs);
            }
#if WDEBUG_LEVEL >= 3
            biginteger qc(q);
            mul(q, q, lhs);
            add(q, q, rhs);
            WASSERT_LEVEL_3(q == p);
#endif
        }

    public:

        template<typename T, std::enable_if_t<__Can_make_signed_view_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 biginteger& operator+=(const T& rhs) {
            add(*this, *this, rhs);
            return *this;
        }

        template<typename T, std::enable_if_t<__Can_make_signed_view_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 biginteger& operator-=(const T& rhs) {
            sub(*this, *this, rhs);
            return *this;
        }

        template<typename T, std::enable_if_t<__Can_make_signed_view_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 biginteger& operator*=(const T& rhs) {
            mul(*this, *this, rhs);
            return *this;
        }

        template<typename T, std::enable_if_t<__Can_make_signed_view_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 biginteger& operator/=(T&& rhs) {
            auto _Rhs = signed_view<base>(std::forward<T>(rhs));
            bool f = signal() ^ (!_Rhs.signal());
            abs();
            _Rhs.abs();
            if constexpr (decltype(_Rhs)::max_bit == 1) {
                one_divmod(*this, *_Rhs.data());
            }
            else {
                if constexpr (__Can_move_v<T&&>) {
                    biginteger R(std::forward<T>(rhs));
                    R.abs();
                    quick_divmod(*this, R);
                }
                else {
                    biginteger R(_Rhs);
                    quick_divmod(*this, R);
                }
            }
            set_signal(f);
            return *this;
        }

        template<typename T, std::enable_if_t<__Can_make_signed_view_v<base, std::decay_t<T>>, int> = 0>
        WJR_CONSTEXPR20 biginteger& operator%=(T&& rhs) {
            auto _Rhs = signed_view<base>(std::forward<T>(rhs));
            bool f = signal();
            abs();
            _Rhs.abs();
            if constexpr (decltype(_Rhs)::max_bit == 1) {
                *this = one_divmod(*this, *_Rhs.data());
            }
            else {
                if constexpr (__Can_move_v<T&&>) {
                    biginteger R(std::forward<T>(rhs));
                    R.abs();
                    quick_divmod(*this, R);
                    *this = std::move(R);
                }
                else {
                    biginteger R(_Rhs);
                    R.abs();
                    quick_divmod(*this, R);
                    *this = std::move(R);
                }
            }
            set_signal(f);
            return *this;
        }

        WJR_CONSTEXPR20 biginteger& operator++() {
            (*this) += signed_integral_view<base, 1>(1u);
            return *this;
        }

        WJR_CONSTEXPR20 biginteger operator++(int) {
            biginteger it(*this);
            ++(*this);
            return it;
        }

        WJR_CONSTEXPR20 biginteger& operator--() {
            (*this) -= signed_integral_view<base, 1>(1u);
            return *this;
        }

        WJR_CONSTEXPR20 biginteger operator--(int) {
            biginteger it(*this);
            --(*this);
            return it;
        }

        friend WJR_CONSTEXPR20 bool operator!(const biginteger& rhs) {
            return rhs.test_byte(0);
        }

        template<typename C = tag::empty_tag, std::enable_if_t<tag::is_tag_set_v<C>, int> = 0>
        WJR_CONSTEXPR20 biginteger& random(size_t n, C = C{}) {
            random<base>(*this, n, C{});
            return *this;
        }

        template<typename T, std::enable_if_t<
            std::conjunction_v<__Can_make_signed_view<base, T>>, int> = 0>
        WJR_CONSTEXPR20 biginteger& interval_random(const T& _Max) {
            interval_random<base>(*this, _Max);
            return *this;
        }

        template<typename T, typename U, std::enable_if_t<
            std::conjunction_v<__Can_make_signed_view<base, T>,
            __Can_make_signed_view<base, U>>, int> = 0>
            WJR_CONSTEXPR20 biginteger& interval_random(const T& _Min, const U& _Max) {
            interval_random<base>(*this, _Min, _Max);
            return *this;
        }

    private:

        template<typename T, typename U>
        WJR_CONSTEXPR20 static bool __Equal(
            const T& lhs,
            const U& rhs) {
            if (lhs.signal() != rhs.signal())return false;
            return unsigned_traits::equal(lhs.get_unsigned(), rhs.get_unsigned());
        }
        template<typename T, typename U>
        WJR_CONSTEXPR20 static bool __Less(
            const T& lhs,
            const U& rhs) {
            if (lhs.signal() != rhs.signal()) {
                return rhs.signal();
            }
            if (lhs.signal()) {
                return unsigned_traits::less(lhs, rhs);
            }
            else {
                return unsigned_traits::greater(lhs, rhs);
            }
        }

        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 int cmp(const T&, const U&);
        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 void add(
                biginteger<_Base>& result,
                const T& lhs,
                const U& rhs
            );
        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 void sub(
                biginteger<_Base>& result,
                const T& lhs,
                const U& rhs
            );
        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 void mul(
                biginteger<_Base>& result,
                const T& lhs,
                const U& rhs
            );


        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 bool operator==(const T&, const U&);
        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 bool operator!=(const T&, const U&);
        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 bool operator<(const T&, const U&);
        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 bool operator<=(const T&, const U&);
        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 bool operator>(const T&, const U&);
        BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
            friend WJR_CONSTEXPR20 bool operator>=(const T&, const U&);

        template<size_t _Base>
        friend WJR_CONSTEXPR20 void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);

        bool _Signal = true;
        unsigned_biginteger<base> ubint;
    };

    template<size_t base>
    template<typename T, typename U>
    WJR_CONSTEXPR20 void unsigned_biginteger<base>::toom_cook_3(
        unsigned_biginteger& result,
        const T& lhs,
        const U& rhs
    ) {
        size_t n = lhs.size(), m = rhs.size();
        size_t n3 = n / 3;
        WASSERT_LEVEL_1(m > n3);
        auto U0 = unsigned_view<base>(lhs, 0, n3);
        auto U1 = unsigned_view<base>(lhs, n3, n3);
        auto U2 = unsigned_view<base>(lhs, 2 * n3, n - 2 * n3);
        auto V0 = unsigned_view<base>(rhs, 0, n3);
        size_t _L1 = m < 2 * n3 ? m - n3 : n3;
        auto V1 = unsigned_view<base>(rhs, n3, _L1);
        size_t _L2 = m < 2 * n3 ? 0 : m - 2 * n3;
        auto V2 = unsigned_view<base>(rhs, 2 * n3, _L2);
        U0.maintain();
        U1.maintain();
        V0.maintain();
        V1.maintain();
        V2.maintain();

        biginteger<base> W0, W1, W2, W3, W4;
        math::biginteger::add(W0, U0, U2);
        math::biginteger::sub(W3, W0, U1);
        math::biginteger::add(W0, W0, U1);
        math::biginteger::add(W4, V0, V2);
        math::biginteger::sub(W2, W4, V1);
        math::biginteger::add(W4, W4, V1);

        math::biginteger::mul(W1, W3, W2);
        math::biginteger::mul(W2, W0, W4);

        W0 += U2;
        W0 *= 2u;
        W0 -= U0;
        W4 += V2;
        W4 *= 2u;
        W4 -= V0;

        math::biginteger::mul(W3, W0, W4);
        math::biginteger::mul(W0, U0, V0);
        math::biginteger::mul(W4, U2, V2);

        W3 -= W1;
        biginteger<base>::template one_divmod<3>(W3);
        math::biginteger::sub(W1, W2, W1);
        biginteger<base>::template one_divmod<2>(W1);
        W2 -= W0;
        W3 -= W2;
        biginteger<base>::template one_divmod<2>(W3);
        W3 -= W4.mul_base_power(1);
        W2 -= W1;

        W4.mul_base_power(n3 * traits::bits);
        W2.mul_base_power(n3 * traits::bits);

        W4 += W3;
        W2 += W1;
        W3 = std::move(W4);
        W1 = std::move(W2);
        W1 -= W3;

        WASSERT_LEVEL_1(W3.signal());
        WASSERT_LEVEL_1(W1.signal());
        WASSERT_LEVEL_1(W0.signal());

        result.reserve(n + m);
        result.set_byte(0);

        add(unsigned_modifier<base>(result, 0), unsigned_view<base>(result, 0), W0.ubint);
        add(unsigned_modifier<base>(result, n3), unsigned_view<base>(result, n3), W1.ubint);
        add(unsigned_modifier<base>(result, 3 * n3), unsigned_view<base>(result, 3 * n3), W3.ubint);
        result.maintain();
    }

    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 int cmp(const T& lhs, const U& rhs) {
        return biginteger<_Base>::signed_cmp(
            signed_view<_Base>(lhs),
            signed_view<_Base>(rhs)
        );
    }

    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 void add(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        ) {
        biginteger<_Base>::signed_add(
            result,
            signed_view<_Base>(lhs),
            signed_view<_Base>(rhs)
        );
    }

    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 void sub(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        ) {
        biginteger<_Base>::signed_sub(
            result,
            signed_view<_Base>(lhs),
            signed_view<_Base>(rhs)
        );
    }

    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 void mul(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        ) {
        using unsigned_traits = unsigned_biginteger<_Base>;
        const auto l = signed_view<_Base>(lhs);
        const auto r = signed_view<_Base>(rhs);
        unsigned_traits::mul(result.ubint, l.get_unsigned(), r.get_unsigned());
        result.set_signal(!(l.signal() ^ r.signal()));
    }

    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 bool operator==(const T& lhs, const U& rhs) {
        return biginteger<_Base>::__Equal(
            signed_view<_Base>(lhs),
            signed_view<_Base>(rhs)
        );
    }
    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 bool operator!=(const T& lhs, const U& rhs) {
        return !(lhs == rhs);
    }
    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 bool operator<(const T& lhs, const U& rhs) {
        return biginteger<_Base>::__Less(
            signed_view<_Base>(lhs),
            signed_view<_Base>(rhs)
        );
    }
    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 bool operator<=(const T& lhs, const U& rhs) {
        return !(rhs < lhs);
    }
    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 bool operator>(const T& lhs, const U& rhs) {
        return rhs < lhs;
    }
    BIGINTEGER_VIEW_BINARY_AUTO_DEFINITION
        WJR_CONSTEXPR20 bool operator>=(const T& lhs, const U& rhs) {
        return !(lhs < rhs);
    }

    template<size_t _Base>
    WJR_CONSTEXPR20 void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs) {
        WASSERT_LEVEL_1(std::addressof(lhs) != std::addressof(rhs));
        biginteger<_Base>::quick_divmod(lhs, rhs);
    }

    template<size_t _Base, typename C, std::enable_if_t<tag::is_tag_set_v<C>, int>>
    WJR_CONSTEXPR20 void random(biginteger<_Base>& result, size_t n, C) {
        constexpr auto tag = C{} &(tag::exact | tag::_signed);
        unsigned_biginteger<_Base>::random(result.get_unsigned(), n, tag);
        if constexpr (tag(tag::_signed)) {
            result.set_signal(biginteger_traits<_Base>::random_bool());
        }
        else {
            result.set_signal(true);
        }
    }

    template<size_t _Base, typename C, std::enable_if_t<tag::is_tag_set_v<C>, int>>
    WJR_CONSTEXPR20 biginteger<_Base> random(size_t n, C) {
        biginteger<_Base> result;
        random(result, n, C{});
        return result;
    }

    template<size_t _Base, typename T,
        std::enable_if_t<std::conjunction_v<
        __Can_make_signed_view<_Base, T>>, int>>
        WJR_CONSTEXPR20 void interval_random(biginteger<_Base>& result, const T& _Max) {
        const auto view = signed_view<_Base>(_Max);
        WASSERT_LEVEL_1(view.signal());
        unsigned_biginteger<_Base>::interval_random(result.get_unsigned(), view.get_unsigned());
        result.set_signal(true);
    }

    template<size_t _Base, typename T,
        std::enable_if_t<std::conjunction_v<
        __Can_make_signed_view<_Base, T>>, int>>
        WJR_CONSTEXPR20 biginteger<_Base> interval_random(const T& _Max) {
        biginteger<_Base> result;
        interval_random(result, _Max);
        return result;
    }

    template<size_t _Base, typename T, typename U,
        std::enable_if_t<std::conjunction_v<
        __Can_make_signed_view<_Base, T>,
        __Can_make_signed_view<_Base, U>>, int>>
        WJR_CONSTEXPR20 biginteger<_Base> interval_random(const T& _Min, const U& _Max) {
        biginteger<_Base> result;
        interval_random(result, _Min, _Max);
        return result;
    }

    template<size_t to_base, typename T, typename C,
        size_t base,
        std::enable_if_t<
        std::conjunction_v<
        __Can_make_signed_view<base, T>,
        tag::is_tag_set<C>,
        __is_accepted_base<base>>, int>>
        WJR_CONSTEXPR20 void radix_conversion(biginteger<to_base>& dst, const T& src, C) {
        auto view = signed_view<base>(src);
        unsigned_biginteger<base>::radix_conversion(dst.get_unsigned(), src.get_unsigned(), C{});
        dst.set_signal(view.signal());
    }

    template<size_t to_base, typename T, typename C,
        size_t base,
        std::enable_if_t<
        std::conjunction_v<
        __Can_make_signed_view<base, T>,
        tag::is_tag_set<C>,
        __is_accepted_base<base>>, int>>
        WJR_CONSTEXPR20 biginteger<to_base> radix_conversion(std::in_place_index_t<to_base>, const T& src, C) {
        biginteger<to_base> result;
        radix_conversion(result, src, C{});
        return result;
    }

    template<size_t to_base, typename T, typename C,
        size_t base,
        std::enable_if_t<
        std::conjunction_v<
        __Can_make_signed_view<base, T>,
        tag::is_tag_set<C>,
        __is_accepted_base<base>>, int>>
        WJR_CONSTEXPR20 biginteger<to_base> radix_conversion(const T& src, C) {
        return radix_conversion(std::in_place_index_t<to_base>{}, src, C{});
    }

}

#endif
