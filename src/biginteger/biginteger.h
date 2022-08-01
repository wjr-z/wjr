#ifndef __WJR_BIGINTEGER_H
#define __WJR_BIGINTEGER_H

#include <array>

#include "../generic/mallocator.h"
#include "../generic/mString.h"
#include "../generic/masm.h"
#include "../generic/mmath.h"

extern "C" {
    extern void cdft(int, int, double*, int*, double*);
}

namespace wjr {

    namespace math {

#if defined(__SIZEOF_INT128__) && !(defined(__clang__) && defined(_MSC_VER))
#define WJR_BIGINTEGER_USE_X64
#endif

        namespace biginteger {

#if defined WJR_BIGINTEGER_USE_X64
            using limb_t = uint64_t;
            using double_limb_t = __uint128_t;
            constexpr static int limb_bits = 64;
            constexpr static int limb_bytes = 8;
#else
            using limb_t = uint32_t;
            using double_limb_t = uint64_t;
            constexpr static int limb_bits = 32;
            constexpr static int limb_bytes = 4;
#endif
            using limb_ptr = limb_t*;
            using double_limb_ptr = double_limb_t*;
            using limb_const_ptr = const limb_t*;
            using double_limb_const_ptr = const double_limb_t*;

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
                    return math::force_constexpr::integer_power(std::integral_constant<limb_t, base>{}, bits) - 1;
                }
                constexpr static limb_t mod() {
                    return max() + 1;
                }
                constexpr static double_limb_t max_value() {
                    return static_cast<double_limb_t>(max()) + 1;
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
                    return max() == std::numeric_limits<limb_t>::max();
                }

                template<typename T>
                constexpr static limb_t get_low(T x) {
                    if constexpr (__get_undefined_max<T>::value <= max() || is_full()) {
                        return static_cast<limb_t>(x);
                    }
                    else {
                        return x % mod();
                    }
                }

                template<typename T>
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
                            return static_cast<T>(x / mod());
                        }
                    }
                }

                constexpr static double_limb_t mul(limb_t a, limb_t b) {
                    return static_cast<double_limb_t>(a) * b;
                }

                WJR_CONSTEXPR static unsigned char
                    add(
                        limb_t lhs,
                        limb_t rhs,
                        limb_ptr result
                    ) {
                    unsigned char cf = math::add(lhs, rhs, result);
                    if constexpr (is_full()) {
                        return cf;
                    }
                    else {
                        cf |= mod() <= *result;
                        return cf ? (*result -= mod(), 1) : 0;
                    }
                }

                WJR_CONSTEXPR static unsigned char
                    adc(
                        unsigned char cf,
                        limb_t lhs,
                        limb_t rhs,
                        limb_ptr result) {
                    cf = math::adc(cf, lhs, rhs, result);
                    if constexpr (is_full()) {
                        return cf;
                    }
                    else {
                        cf |= mod() <= *result;
                        return cf ? (*result -= mod(), 1) : 0;
                    }
                }

                WJR_CONSTEXPR static unsigned char
                    adc(
                        limb_const_ptr lhs,
                        limb_const_ptr rhs,
                        limb_ptr result,
                        size_t n
                    ) {
                    if constexpr (is_full()) {
                        return math::adc(lhs, rhs, result, n);
                    }
                    else {
                        unsigned char cf = 0;
                        size_t r = n & 3;
                        for (size_t i = 0; i < r; ++i) {
                            cf = adc(cf, lhs[i], rhs[i], &result[i]);
                        }
                        for (size_t i = r; i < n; i += 4) {
                            cf = adc(cf, lhs[i], rhs[i], &result[i]);
                            cf = adc(cf, lhs[i + 1], rhs[i + 1], &result[i + 1]);
                            cf = adc(cf, lhs[i + 2], rhs[i + 2], &result[i + 2]);
                            cf = adc(cf, lhs[i + 3], rhs[i + 3], &result[i + 3]);
                        }
                        return cf;
                    }
                }

                WJR_CONSTEXPR static unsigned char
                    sub(
                        limb_t lhs,
                        limb_t rhs,
                        limb_ptr result) {
                    unsigned char cf = math::sub(lhs, rhs, result);
                    if constexpr (is_full()) {
                        return cf;
                    }
                    else {
                        return cf ? (*result += mod(), 1) : 0;
                    }
                }

                WJR_CONSTEXPR static unsigned char
                    sbb(
                        unsigned char cf,
                        limb_t lhs,
                        limb_t rhs,
                        limb_ptr result) {
                    cf = math::sbb(cf, lhs, rhs, result);
                    if constexpr (is_full()) {
                        return cf;
                    }
                    else {
                        return cf ? (*result += mod(), 1) : 0;
                    }
                }

                WJR_CONSTEXPR static unsigned char
                    sbb(
                        limb_const_ptr lhs,
                        limb_const_ptr rhs,
                        limb_ptr result,
                        size_t n
                    ) {
                    if constexpr (is_full()) {
                        return math::sbb(lhs, rhs, result, n);
                    }
                    else {
                        unsigned char cf = 0;
                        size_t r = n & 3;
                        for (size_t i = 0; i < r; ++i) {
                            cf = sbb(cf, lhs[i], rhs[i], &result[i]);
                        }
                        for (size_t i = r; i < n; i += 4) {
                            cf = sbb(cf, lhs[i], rhs[i], &result[i]);
                            cf = sbb(cf, lhs[i + 1], rhs[i + 1], &result[i + 1]);
                            cf = sbb(cf, lhs[i + 2], rhs[i + 2], &result[i + 2]);
                            cf = sbb(cf, lhs[i + 3], rhs[i + 3], &result[i + 3]);
                        }
                        return cf;
                    }
                }

                WJR_CONSTEXPR static limb_t
                    vector_mul_1(
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
                    vector_addmul_1(
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

                WJR_CONSTEXPR static int bit_size(limb_t v) {
                    if constexpr(has_single_bit){
                        return (math::integer_log2(v) / integer_log2) + 1;
                    }else{
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
                public:
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

#if defined(WJR_USE_LIBDIVIDE)
            template<typename T>
            struct biginteger_libdivider {};

            template<>
            struct biginteger_libdivider<uint32_t> {
                explicit biginteger_libdivider(uint32_t d) {
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
                        proposed_m = libdivide::libdivide_64_div_32_to_32(
                                    static_cast<uint32_t>(1u << floor_log_2_d), 0, d, &_R);
                        proposed_m <<= 32;
                        proposed_m |= libdivide::libdivide_64_div_32_to_32(_R, 0, d, &rem);
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

            uint64_t operator/(uint64_t n, const biginteger_libdivider<uint32_t>& d) {
                uint8_t more = d.more;
                if (!d.magic) {
                    return n >> more;
                }
                else {
                    uint64_t q = libdivide::libdivide_mullhi_u64(d.magic, n);
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
                biginteger_libdivider(uint64_t d) {
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
                        proposed_m = libdivide::libdivide_128_div_64_to_64(
                                    static_cast<uint64_t>(1ull << floor_log_2_d), 0, d, &_R);
                        proposed_m <<= 64;
                        proposed_m |= libdivide::libdivide_128_div_64_to_64(_R, 0, d, &rem);
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

            static double_limb_t
                biginteger_libdivide_mullhi_u128(
                    double_limb_t x,
                    double_limb_t y) {
                constexpr static uint64_t mask = (uint64_t)(-1);
                uint64_t x0 = (uint64_t)(x & mask);
                uint64_t x1 = (uint64_t)(x >> 64);
                uint64_t y0 = (uint64_t)(y & mask);
                uint64_t y1 = (uint64_t)(y >> 64);
                uint64_t x0y0_hi = libdivide::libdivide_mullhi_u64(x0, y0);
                double_limb_t x0y1 = biginteger_traits<2>::mul(x0, y1);
                double_limb_t x1y0 = biginteger_traits<2>::mul(x1, y0);
                double_limb_t x1y1 = biginteger_traits<2>::mul(x1, y1);
                double_limb_t temp = x1y0 + x0y0_hi;
                limb_t temp_lo = static_cast<uint64_t>(temp);
                limb_t temp_hi = static_cast<uint64_t>(temp >> 64);
                return x1y1 + temp_hi + ((temp_lo + x0y1) >> 64);
            }

            static double_limb_t operator/(
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
#endif
#else
            template<typename T>
            using biginteger_libdivider = T;
#endif

            template<size_t base>
            class unsigned_biginteger;

            template<size_t base>
            class biginteger;

            template<size_t base>
            typename biginteger_traits<base>::__magic_divider_of_base_power
                biginteger_traits<base>::__magic_divider = __magic_divider_of_base_power();

            constexpr static const limb_t __biginteger_zero_area[] = { 0 };

            template<size_t base, size_t length>
            class unsigned_biginteger_view;

            template<size_t base, size_t length>
            class biginteger_view;

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
                    WASSERT_LEVEL_1(vaild());
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
                    return reference(m_data, m_size, m_k, m_index);
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

            template<size_t base>
            struct _Unsigned_iterator {
                constexpr decltype(auto) operator()(
                    const unsigned_biginteger<base>& v, size_t k = 1, size_t index = 0) const{
                    return unsigned_biginteger_const_iterator<base>(v.data(), v.size(), k, index);
                }
                constexpr decltype(auto) operator()(
                    const biginteger<base>& v, size_t k = 1, size_t index = 0) const{
                    return unsigned_biginteger_const_iterator<base>(v.data(), v.size(), k, index);
                }
                constexpr decltype(auto) operator()(
                    unsigned_biginteger<base>& v, size_t k = 1, size_t index = 0) const {
                    return unsigned_biginteger_iterator<base>(v.data(), v.size(), k, index);
                }
                constexpr decltype(auto) operator()(
                    biginteger<base>& v, size_t k = 1, size_t index = 0) const {
                    return unsigned_biginteger_iterator<base>(v.data(), v.size(), k, index);
                }
                template<size_t length>
                constexpr decltype(auto) operator()(
                    const unsigned_biginteger_view<base, length>& v, size_t k = 1, size_t index = 0) const {
                    return unsigned_biginteger_const_iterator<base>(v.data(), v.size(), k, index);
                }
                template<size_t length>
                constexpr decltype(auto) operator()(
                    const biginteger_view<base, length>& v, size_t k = 1, size_t index = 0) const {
                    return unsigned_biginteger_const_iterator<base>(v.data(), v.size(), k, index);
                }
            };

            template<size_t base>
            inline constexpr _Unsigned_iterator<base> unsigned_iterator;

            // unsigned biginteger or integral view
            template<size_t base, size_t length>
            class unsigned_biginteger_view {
                static_assert(length <= 4, "");
                using traits = biginteger_traits<base>;
                template<size_t index, typename T>
                constexpr void _Make(T val) noexcept {
                    if constexpr (index == traits::template bits_of_t<T> - 1) {
                        m_data[index] = val;
                        m_size = index + 1;
                        WASSERT_LEVEL_2(traits::get_high(val) == 0);
                    }
                    else {
                        m_data[index] = traits::get_low(val);
                        val = traits::get_high(val);
                        if (val) {
                            _Make<index + 1>(val);
                        }
                        else m_size = index + 1;
                    }
                }
            public:
                using const_iterator = unsigned_biginteger_const_iterator<base>;
                using iterator = unsigned_biginteger_iterator<base>;
                template<typename T, std::enable_if_t<_Is_unsigned_integral_v<T>, int> = 0>
                constexpr explicit unsigned_biginteger_view(T val) noexcept {
                    _Make<0>(val);
                }
                constexpr unsigned_biginteger_view(const unsigned_biginteger_view&) noexcept = default;
                constexpr unsigned_biginteger_view& operator=(
                    const unsigned_biginteger_view&) noexcept = default;
                constexpr limb_const_ptr data()const noexcept { return m_data; }
                constexpr size_t size() const noexcept {
                    if constexpr (length == 1) {
                        return 1;
                    }
                    else {
                        return m_size;
                    }
                }
                constexpr limb_const_ptr begin()const noexcept { return m_data; }
                constexpr limb_const_ptr end()const noexcept { return m_data + m_size; }
                constexpr bool test_byte(limb_t val) const noexcept {
                    return m_size == 1 && m_data[0] == val;
                }

                template<typename T>
                constexpr T to()const noexcept {
                    T _Val = 0;
                    if constexpr (length == 1) {
                        _Val = m_data[0];
                    }
                    else {
                        for (size_t i = m_size; i--;) {
                            _Val = _Val * traits::max_value() + m_data[i];
                        }
                    }
                    return _Val;
                }
                template<typename T>
                constexpr explicit operator T()const noexcept {
                    return to<T>();
                }
                constexpr unsigned_biginteger_view& maintain() noexcept {
                    return *this;
                }
            private:
                size_t m_size;
                limb_t m_data[length];
            };

            template<size_t base>
            class unsigned_biginteger_view<base, 0> {
                using traits = biginteger_traits<base>;
            public:
                using const_iterator = unsigned_biginteger_const_iterator<base>;
                using iterator = unsigned_biginteger_iterator<base>;
                constexpr unsigned_biginteger_view(
                    limb_const_ptr _Data, size_t _Size)
                    noexcept : m_data(_Data), m_size(_Size) {}
                constexpr unsigned_biginteger_view(
                    const unsigned_biginteger_view&) noexcept = default;
                constexpr unsigned_biginteger_view& operator=(
                    const unsigned_biginteger_view&) noexcept = default;

                constexpr limb_const_ptr data()const noexcept {
                    WASSERT_LEVEL_1(m_size != 0);
                    return m_data;
                }
                constexpr size_t size()const noexcept { return m_size; }
                constexpr limb_const_ptr begin()const noexcept {
                    return data();
                }
                constexpr limb_const_ptr end()const noexcept {
                    return begin() + m_size;
                }
                constexpr bool test_byte(limb_t val) const noexcept {
                    WASSERT_LEVEL_1(m_size != 0);
                    return size() == 1 && m_data[0] == val;
                }

                template<typename T>
                constexpr T to()const noexcept {
                    WASSERT_LEVEL_1(m_size != 0);
                    T _Val = 0;
                    for (size_t i = m_size; i--;) {
                        _Val = _Val * traits::max_value() + m_data[i];
                    }
                    return _Val;
                }
                template<typename T>
                constexpr explicit operator T() const noexcept {
                    return to<T>();
                }
                WJR_CONSTEXPR unsigned_biginteger_view& maintain() noexcept {
                    if (!m_size) {
                        m_data = __biginteger_zero_area;
                        m_size = 1;
                        return *this;
                    }
                    while (m_size != 1 && !m_data[m_size - 1])--m_size;
                    return *this;
                }
            private:
                limb_const_ptr m_data;
                size_t m_size;
            };

            template<typename T>
            struct unsigned_biginteger_base_helper {
                constexpr static size_t value = _Is_unsigned_integral_v<T> ? 0 : -1;
            };

            template<size_t base>
            struct unsigned_biginteger_base_helper<unsigned_biginteger<base>> {
                constexpr static size_t value = base;
            };

            template<size_t base, size_t length>
            struct unsigned_biginteger_base_helper<unsigned_biginteger_view<base, length>> {
                constexpr static size_t value = base;
            };

            template<typename T>
            struct unsigned_biginteger_base : unsigned_biginteger_base_helper<std::decay_t<T>> {
            };

            template<typename T>
            constexpr static size_t unsigned_biginteger_base_v = unsigned_biginteger_base<T>::value;

            template<typename T, typename...Args>
            struct unsigned_biginteger_base_mask {
            private:
                constexpr static size_t lvalue = unsigned_biginteger_base_v<T>;
                constexpr static size_t rvalue = unsigned_biginteger_base_mask<Args...>::value;
                constexpr static size_t get_value() {
                    if constexpr (lvalue == size_t(-1) || rvalue == size_t(-1)) {
                        // at least one is illegal
                        return -1;
                    }
                    else if constexpr (lvalue == 0) {
                        // if one is integral
                        return rvalue;
                    }
                    else if constexpr (rvalue == 0) {
                        return lvalue;
                    }
                    else if constexpr (lvalue == rvalue) {
                        return lvalue;
                    }
                    else return -1; // illegal
                }
            public:
                constexpr static size_t value = get_value();
            };

            template<typename T>
            struct unsigned_biginteger_base_mask<T> {
                constexpr static size_t value = unsigned_biginteger_base_v<T>;
            };

            template<typename T, typename...Args>
            constexpr static size_t unsigned_biginteger_base_mask_v =
                unsigned_biginteger_base_mask<T, Args...>::value;

            template<typename T>
            struct _Is_unsigned_biginteger : std::false_type {};

            template<size_t base>
            struct _Is_unsigned_biginteger<unsigned_biginteger<base>> : std::true_type {};

            template<typename T>
            constexpr static bool _Is_unsigned_biginteger_v = _Is_unsigned_biginteger<T>::value;

            template<typename T>
            struct _Is_original_unsigned_biginteger_view : std::false_type {};

            template<size_t base, size_t length>
            struct _Is_original_unsigned_biginteger_view<
                unsigned_biginteger_view<base, length>> : std::true_type {};

            template<typename T>
            constexpr static bool _Is_original_unsigned_biginteger_view_v =
                _Is_original_unsigned_biginteger_view<T>::value;

            template<size_t base, size_t s>
            struct _Is_base_mask {
                constexpr static bool value = (base != -1) && (s == 0 || s == base);
            };

            template<size_t base, size_t s>
            constexpr static bool _Is_base_mask_v = _Is_base_mask<base, s>::value;

            template<size_t base, size_t...s>
            struct _Is_base_masks : std::conjunction<_Is_base_mask<base, s>...> {};

            template<size_t base, size_t...s>
            constexpr static bool _Is_base_masks_v = _Is_base_masks<base, s...>::value;

            template<size_t base, typename...Args>
            struct _Is_unsigned_biginteger_base_masks :
                std::conjunction<_Is_base_mask<base, unsigned_biginteger_base_v<Args>>...> {};

            template<size_t base, typename...Args>
            constexpr static bool _Is_unsigned_biginteger_base_masks_v =
                _Is_unsigned_biginteger_base_masks<base, Args...>::value;

            template<size_t base>
            struct __unsigned_biginteger_view {
                template<typename T, std::enable_if_t<_Is_base_mask_v<base, unsigned_biginteger_base_v<T>>, int> = 0>
                constexpr decltype(auto) operator()(const T& t) const {
                    using traits = biginteger_traits<base>;
                    if constexpr (_Is_unsigned_integral_v<T>) {
                        return unsigned_biginteger_view<base, traits::template bits_of_t<T>>(t);
                    }
                    else if constexpr (_Is_unsigned_biginteger_v<T>) {
                        return unsigned_biginteger_view<base, 0>(t.data(), t.size());
                    }
                    else {
                        return t;
                    }
                }
            };

            template<size_t base>
            inline constexpr __unsigned_biginteger_view<base> unsigned_view;

#define REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(index,rt,FUNC,...)		\
        if constexpr (index == 1) {					    	        \
            return FUNC<1>(__VA_ARGS__);					        \
        }													        \
        else if constexpr (index == 2) {				    	    \
            switch (rt.size()) {							        \
            case 2: return FUNC<2>(__VA_ARGS__);			        \
            case 1: return FUNC<1>(__VA_ARGS__);			        \
            }												        \
        }													        \
        else if constexpr (index == 3) {					        \
            switch (rt.size()) {							        \
            case 3: return FUNC<3>(__VA_ARGS__);			        \
            case 2: return FUNC<2>(__VA_ARGS__);			        \
            case 1: return FUNC<1>(__VA_ARGS__);			        \
            }												        \
        }													        \
        else if constexpr(index == 4){                              \
            switch (rt.size()) {							        \
            case 4: return FUNC<4>(__VA_ARGS__);                    \
            case 3: return FUNC<3>(__VA_ARGS__);			        \
            case 2: return FUNC<2>(__VA_ARGS__);			        \
            case 1: return FUNC<1>(__VA_ARGS__);			        \
            }												        \
        }                                                           \
        else {												        \
            if(rt.size() != 1){                                     \
                return FUNC<0>(__VA_ARGS__);                        \
            }else{                                                  \
                return FUNC<1>(__VA_ARGS__);                        \
            }                                                       \
        }													        \

#define REGISTER_BETTER_VIRTUAL_BIGINTEGER_FUNCTION(il,ir,FUNC,result,lhs,rhs)                      \
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
        }                                                                                           \


#define VIRTUAL_UNSIGNED_BIGINTEGER_SINGLE_DECLARATION                                          \
    template<typename T                                                                         \
    std::enable_if_t<_Is_unsigned_biginteger_base_masks_v<base, T>, int> = 0>                   \

#define VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION                                          \
    template<typename T, typename U,                                                            \
    std::enable_if_t<_Is_unsigned_biginteger_base_masks_v<base, T, U>, int> = 0>                \

            template<typename _InIt, typename _Diff, typename _OutIt>
            WJR_CONSTEXPR20 void _Biginteger_equal_copy_n(
                _InIt _First, _Diff _Count, _OutIt _Dest
            ) {
                if (_First != _Dest) {
                    std::copy_n(_First, _Count, _Dest);
                }
            }

            class _Biginteger_vector {
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

                WJR_CONSTEXPR20 _Biginteger_vector() = default;
                WJR_CONSTEXPR20 _Biginteger_vector(size_type _Count, value_type _Value = value_type())
                    : m_first(allocator_type{}.allocate(_Count)), m_last(m_first + _Count), m_end(m_last) {
                    std::fill_n(m_first, _Count, _Value);
                }
                template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
                WJR_CONSTEXPR20 _Biginteger_vector(iter _First, iter _Last, std::input_iterator_tag) {
                    while (_First != _Last) {
                        push_back(*_First);
                        ++_First;
                    }
                }
                template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
                WJR_CONSTEXPR20 _Biginteger_vector(iter _First, iter _Last, std::forward_iterator_tag) {
                    size_t _Count = std::distance(_First, _Last);
                    m_first = allocator_type{}.allocate(_Count);
                    m_last = m_first + _Count;
                    m_end = m_last;
                    std::copy_n(_First, _Count, m_first);
                }
                template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
                WJR_CONSTEXPR20 _Biginteger_vector(iter _First, iter _Last)
                    : _Biginteger_vector(_First, _Last, typename std::iterator_traits<iter>::iterator_category{}) {
                }
                WJR_CONSTEXPR20 _Biginteger_vector(const _Biginteger_vector& _Right)
                    : m_first(allocator_type{}.allocate(_Right.size())),
                    m_last(m_first + _Right.size()), m_end(m_last) {
                    std::copy_n(_Right.m_first, _Right.size(), m_first);
                }
                WJR_CONSTEXPR20 _Biginteger_vector(_Biginteger_vector&& _Right) noexcept
                    : m_first(std::exchange(_Right.m_first, nullptr)),
                    m_last(std::exchange(_Right.m_last, nullptr)),
                    m_end(std::exchange(_Right.m_end, nullptr)) {
                }
                WJR_CONSTEXPR20 _Biginteger_vector& operator=(const _Biginteger_vector& _Right) {
                    if (this == std::addressof(_Right)) {
                        return *this;
                    }
                    const size_type _Newsize = _Right.size();
                    const size_type _Oldcapacity = capacity();
                    if (_Newsize > _Oldcapacity) {
                        const size_type _Newcapacity = _Calcuate_growth(_Newsize);
                        const auto _Newvec = allocator_type{}.allocate(_Newcapacity);
                        std::copy_n(_Right.m_first, _Newsize, _Newvec);
                        _Tidy();
                        m_first = _Newvec;
                        m_last = m_first + _Newsize;
                        m_end = m_first + _Newcapacity;
                    }
                    else {
                        std::copy_n(_Right.m_first, _Newsize, m_first);
                        m_last = m_first + _Newsize;
                    }
                    return *this;
                }
                WJR_CONSTEXPR20 _Biginteger_vector& operator=(_Biginteger_vector&& _Right) noexcept {
                    if (this == std::addressof(_Right)) {
                        return *this;
                    }
                    _Tidy();
                    m_first = std::exchange(_Right.m_first, nullptr);
                    m_last = std::exchange(_Right.m_last, nullptr);
                    m_end = std::exchange(_Right.m_end, nullptr);
                    return *this;
                }

                WJR_CONSTEXPR20 ~_Biginteger_vector() {
                    _Tidy();
                }

                WJR_CONSTEXPR20 void swap(_Biginteger_vector& _Right) noexcept {
                    std::swap(m_first, _Right.m_first);
                    std::swap(m_last, _Right.m_last);
                    std::swap(m_end, _Right.m_end);
                }

                template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
                WJR_CONSTEXPR20 _Biginteger_vector& assign(iter _First, iter _Last, std::input_iterator_tag) {
                    clear();
                    while (_First != _Last) {
                        push_back(*_First);
                        ++_First;
                    }
                    return *this;
                }
				
                template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
                WJR_CONSTEXPR20 _Biginteger_vector& assign(iter _First, iter _Last, std::forward_iterator_tag) {
                    size_t _Count = std::distance(_First, _Last);
                    reserve(_Count);
                    set_size(_Count);
                    std::copy_n(_First, _Count, m_first);
                    return *this;
                }

                template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
                WJR_CONSTEXPR20 _Biginteger_vector& assign(iter _First, iter _Last) {
                    return assign(_First, _Last, typename std::iterator_traits<iter>::iterator_category{});
                }

                WJR_CONSTEXPR20 void reserve(size_type _Count) {
                    const size_type _Oldcapacity = capacity();
                    if (_Count > _Oldcapacity) {
                        const size_type _Oldsize = size();
                        const size_type _Newcapacity = _Calcuate_growth(_Count);
                        const auto _Newvec = allocator_type{}.allocate(_Newcapacity);
                        std::copy_n(m_first, _Oldsize, _Newvec);
                        _Tidy();
                        m_first = _Newvec;
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
                WJR_CONSTEXPR20 size_t _Calcuate_growth(const size_type _Newsize) const {
                    const size_type _Oldcapacity = capacity();
                    const size_type _Newcapacity = _Oldcapacity + _Oldcapacity / 2;
                    return _Newcapacity > _Newsize ? _Newcapacity : _Newsize;
                }

                WJR_CONSTEXPR20 void _Tidy() {
                    if (m_first != nullptr) {
                        allocator_type{}.deallocate(m_first, capacity());
                    }
                }

                pointer m_first = nullptr;
                pointer m_last = nullptr;
                pointer m_end = nullptr;
            };

            template<size_t base>
            class unsigned_biginteger : public _Biginteger_vector {

                using _Base = _Biginteger_vector;

                template<size_t _Base>
                friend class unsigned_biginteger;

                template<size_t _Base>
                friend class biginteger;

                using traits = biginteger_traits<base>;
                template<size_t length>
                using unsigned_biginteger_view = unsigned_biginteger_view<base, length>;
                template<typename T>
                using _Is_unsigned_biginteger_view =
                    _Is_base_mask<base, unsigned_biginteger_base_v<T>>;
                template<typename T>
                constexpr static bool _Is_unsigned_biginteger_view_v =
                    _Is_unsigned_biginteger_view<T>::value;
                template<typename T>
                using _Is_rubint = std::conjunction<
                    std::is_rvalue_reference<T>,
                    _Is_unsigned_biginteger<T>,
                    _Is_unsigned_biginteger_view<T>>;
                template<typename T>
                constexpr static bool _Is_rubint_v = _Is_rubint<T>::value;
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
                WJR_CONSTEXPR20 unsigned_biginteger(const unsigned_biginteger& other) = default;
                WJR_CONSTEXPR20 unsigned_biginteger(unsigned_biginteger&& other) noexcept = default;
                template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
                WJR_CONSTEXPR20 unsigned_biginteger(iter _First, iter _Last)
                    : _Base(_First, _Last) {}

                template<size_t length>
                WJR_CONSTEXPR20 explicit unsigned_biginteger(const unsigned_biginteger_view<length>& rhs)
                    noexcept : _Base(rhs.begin(), rhs.end()) {
                    if (unlikely(_Base::size() == 0)) {
                        set_byte(0);
                    }
                }
                template<typename T, std::enable_if_t<_Is_unsigned_biginteger_view_v<T>, int> = 0>
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
                        size_t q = (n + traits::btis - 1) / traits::bits;
                        _Base::reserve(q);
                        _Base::set_size(q);
                        _Base::back() = 0;
                        auto _Write = unsigned_iterator<base>(*this, 1, 0);
                        while (_First != _Last) {
                            *_Write = fn(*_First);
                            ++_First;
                            ++_Write;
                        }
                    }
                }

                WJR_CONSTEXPR20 explicit unsigned_biginteger(String_view str) {
                    in_bits(str.begin(), str.end(), traits::from_string);
                }

                template<typename iter, typename Func,
                    std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
                WJR_CONSTEXPR20 std::optional<iter> out_bits(iter _First, iter _Last, Func&& fn) const {
                    size_t l = bit_size();
                    auto _Read = unsigned_iterator<base>(*this, 1, 0);
                    while(l && _First != _Last) {
                        *_First = fn(*_Read);
                        ++_First;
                        ++_Read;
                        --l;
                    }
                    if (l) { return {}; }
                    return _First;
                }

                WJR_CONSTEXPR20 unsigned_biginteger& operator=(const unsigned_biginteger&) = default;
                WJR_CONSTEXPR20 unsigned_biginteger& operator=(unsigned_biginteger&& other) noexcept = default;
                template<size_t length>
                WJR_CONSTEXPR20 unsigned_biginteger& operator=(const unsigned_biginteger_view<length>& rhs) {
                    if constexpr (length == 0) {
                        if (unlikely(data() == rhs.data())) {
                            WASSERT_LEVEL_1(rhs.size() <= size());
                            _Base::set_size(rhs.size());
                            return *this;
                        }
                    }
                    else {
                        WASSERT_LEVEL_1(data() != rhs.data());
                    }
                    _Base::assign(rhs.begin(), rhs.end());
                    return *this;
                }
                template<typename T, std::enable_if_t<_Is_unsigned_biginteger_view_v<T>, int> = 0>
                WJR_CONSTEXPR20 unsigned_biginteger& operator=(const T& val) {
                    return ((*this) = unsigned_view<base>(val));
                }

                WJR_CONSTEXPR20 unsigned_biginteger& operator=(String_view str) {
                    in_bits(str.begin(), str.end(), traits::from_string);
                    return *this;
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

                WJR_CONSTEXPR20 void maintain() {
                    size_t n = _Base::size();
                    auto p = _Base::data();
                    while (n != 1 && !p[n - 1])--n;
                    _Base::set_size(n);
                }

                WJR_CONSTEXPR20 void pow(size_t n) {
                    if (!n) {
                        set_byte(1);
                    }
                    else {
                        while (!(n & 1)) {
                            mul(*this, *this, *this);
                            n >>= 1;
                        }
                        if (n == 1)return;
                        unsigned_biginteger s(*this);
                        n >>= 1;
                        while (n) {
                            if (n & 1) {
                                mul(*this, *this, *this);
                            }
                            mul(s, s, *this);
                            n >>= 1;
                        }
                        *this = std::move(s);
                    }
                }

                WJR_CONSTEXPR20 static unsigned_biginteger pow(const unsigned_biginteger& a, size_t n) {
                    if (!n) {
                        return unsigned_biginteger(unsigned_biginteger_view<1>(1u));
                    }
                    else {
                        unsigned_biginteger c(a);
                        while (!(n & 1)) {
                            mul(c, c, c);
                            n >>= 1;
                        }
                        if (n == 1)return c;
                        unsigned_biginteger s(c);
                        n >>= 1;
                        while (n) {
                            if (n & 1) {
                                mul(c, c, c);
                            }
                            mul(s, s, c);
                            n >>= 1;
                        }
                        return s;
                    }
                }

                WJR_CONSTEXPR static unsigned_biginteger pow(unsigned_biginteger&& a, size_t n) {
                    a.pow(n);
                    return std::move(a);
                }

            private:

                template<size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static int QCmp(
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                );

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static int Cmp(
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                ) {
                    REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QCmp, lhs, rhs);
                }

                template<size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static bool QEqual(
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs);
                template<size_t il, size_t ir>
                WJR_CONSTEXPR static bool Equal(
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs) {
                    REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QEqual, lhs, rhs);
                }

                template<size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static bool QLess(
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs);
                template<size_t il, size_t ir>
                WJR_CONSTEXPR static bool Less(
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs) {
                    REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QLess, lhs, rhs);
                }

                template<size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static bool QLess_eq(
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs);
                template<size_t il, size_t ir>
                WJR_CONSTEXPR static bool Less_eq(
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs) {
                    REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QLess_eq, lhs, rhs);
                }

                template<size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static void QAdd(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                );

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void CAdd(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                ) {
                    REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QAdd, result, lhs, rhs);
                }

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void Add(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                ) {
                    REGISTER_BETTER_VIRTUAL_BIGINTEGER_FUNCTION(il, ir, CAdd, result, lhs, rhs);
                }

                template<size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static void QSub(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                );

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void Sub(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                ) {
                    REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QSub, result, lhs, rhs);
                }

                template<size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static void slow_mul(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                );

                template<size_t ir>
                WJR_CONSTEXPR static void pos_add(
                    limb_ptr result,
                    const unsigned_biginteger_view<ir>& rhs
                );

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void karatsuba(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                );

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void toom_cook_3(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                );

                template<size_t i, size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static void dac_mul(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs,
                    unsigned_biginteger& temp,
                    unsigned_biginteger& r
                );

                template<size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static void dac_mul(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                );

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void fft_mul(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                );

                template<size_t index, size_t il, size_t ir>
                WJR_CONSTEXPR static void QMul(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                );
                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void CMul(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                ) {
                    REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QMul, result, lhs, rhs);
                }
                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void Mul(
                    unsigned_biginteger& result,
                    const unsigned_biginteger_view<il>& lhs,
                    const unsigned_biginteger_view<ir>& rhs
                ) {
                    REGISTER_BETTER_VIRTUAL_BIGINTEGER_FUNCTION(il, ir, CMul, result, lhs, rhs);
                }

            public:

                VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
                    WJR_CONSTEXPR static int cmp(const T& lhs, const U& rhs) {
                    return Cmp(
                        unsigned_view<base>(lhs),
                        unsigned_view<base>(rhs)
                    );
                }

                VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
                    WJR_CONSTEXPR static bool equal(const T& lhs, const U& rhs) {
                    return Equal(
                        unsigned_view<base>(lhs),
                        unsigned_view<base>(rhs)
                    );
                }
                VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
                    WJR_CONSTEXPR static bool nequal(const T& lhs, const U& rhs) {
                    return !equal(lhs, rhs);
                }
                VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
                    WJR_CONSTEXPR static bool less(const T& lhs, const U& rhs) {
                    return Less(
                        unsigned_view<base>(lhs),
                        unsigned_view<base>(rhs)
                    );
                }
                VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
                    WJR_CONSTEXPR static bool less_eq(const T& lhs, const U& rhs) {
                    return Less_eq(
                        unsigned_view<base>(lhs),
                        unsigned_view<base>(rhs)
                    );
                }

                VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
                    WJR_CONSTEXPR static void add(unsigned_biginteger& result, const T& lhs, const U& rhs) {
                    Add(result,
                        unsigned_view<base>(lhs),
                        unsigned_view<base>(rhs));
                }

                VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
                    WJR_CONSTEXPR static void sub(unsigned_biginteger& result, const T& lhs, const U& rhs) {
                    Sub(result,
                        unsigned_view<base>(lhs),
                        unsigned_view<base>(rhs));
                }

                VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
                    WJR_CONSTEXPR static void mul(unsigned_biginteger& result, const T& lhs, const U& rhs) {
                    Mul(result,
                        unsigned_view<base>(lhs),
                        unsigned_view<base>(rhs));
                }

            };

            template<size_t base>
            template<size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR int unsigned_biginteger<base>::QCmp(
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs
            ) {
                size_t n = lhs.size();
                if constexpr (index != 0) {
                    if (n != index) return n < index ? -1 : 1;
                    auto p = lhs.data();
                    auto q = rhs.data();
                    if constexpr (index == 1) {
                        return p[0] != q[0] ? (p[0] < q[0] ? -1 : 1) : 0;
                    }
                    else if constexpr (index == 2) {
                        return (p[1] != q[1]) ? (p[1] < q[1] ? -1 : 1) :
                            (p[0] != q[0]) ? (p[0] < q[0] ? -1 : 1) : 0;
                    }
                    else if constexpr (index == 3) {
                        return (p[2] != q[2]) ? (p[2] < q[2] ? -1 : 1) :
                            (p[1] != q[1]) ? (p[1] < q[1] ? -1 : 1) :
                            (p[0] != q[0]) ? (p[0] < q[0] ? -1 : 1) : 0;
                    }
                    else {
                        return (p[3] != q[3]) ? (p[3] < q[3] ? -1 : 1) :
                            (p[2] != q[2]) ? (p[2] < q[2] ? -1 : 1) :
                            (p[1] != q[1]) ? (p[1] < q[1] ? -1 : 1) :
                            (p[0] != q[0]) ? (p[0] < q[0] ? -1 : 1) : 0;
                    }
                }
                else {
                    size_t m = rhs.size();
                    if (n != m) return n < m ? -1 : 1;
                    auto p = lhs.data() + n;
                    auto q = rhs.data() + n;
                    while (n--) {
                        --p;
                        --q;
                        if (*p != *q)return *p < *q ? -1 : 1;
                    }
                    return 0;
                }
            }

            template<size_t base>
            template<size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR bool unsigned_biginteger<base>::QEqual(
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs) {
                size_t n = lhs.size();
                if constexpr (index != 0) {
                    if (n != index)return false;
                    auto p = lhs.data();
                    auto q = rhs.data();
                    if constexpr (index == 1) {
                        return p[0] == q[0];
                    }
                    else if constexpr (index == 2) {
                        return p[0] == q[0] && p[1] == q[1];
                    }
                    else if constexpr (index == 3) {
                        return p[0] == q[0] && p[1] == q[1] && p[2] == q[2];
                    }
                    else {
                        return p[0] == q[0] && p[1] == q[1] && p[2] == q[2] && p[3] == q[3];
                    }
                }
                else {
                    return n == rhs.size() && std::equal(lhs.data(), lhs.data() + n, rhs.data());
                }
            }

#define GENERATE_BIGINTEGER_OPERATOR(op,F)						\
    size_t n = lhs.size();										\
    if constexpr(index != 0){									\
        if (n != index) return n op index;						\
        auto p = lhs.data();									\
        auto q = rhs.data();									\
        if constexpr(index == 1) {								\
            return p[0] op q[0];								\
        }else if constexpr (index == 2){						\
            return p[1] != q[1] ? p[1] op q[1] : p[0] op q[0];	\
        }else if constexpr(index == 3){						    \
            return p[2] != q[2] ? p[2] op q[2] : p[1] != q[1] ?	\
                p[1] op q[1] : p[0] op q[0];					\
        }else{                                                  \
            return p[3] != q[3] ? p[3] op q[3] : p[2] != q[2] ? \
                p[2] op q[2] : p[1] != q[1] ?                   \
                p[1] op q[1] : p[0] op q[0];					\
        }                                                       \
    }else {														\
        size_t m = rhs.size();									\
        if (n != m) return n op m;								\
        auto p = lhs.data() + n;								\
        auto q = rhs.data() + n;								\
        for (; n--;) {											\
            --p;												\
            --q;												\
            if (*p != *q)return *p op * q;						\
        }														\
        return F;												\
    }

            template<size_t base>
            template<size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR bool unsigned_biginteger<base>::QLess(
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs) {
                GENERATE_BIGINTEGER_OPERATOR(< , false);
            }

            template<size_t base>
            template<size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR bool unsigned_biginteger<base>::QLess_eq(
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs) {
                GENERATE_BIGINTEGER_OPERATOR(<= , true);
            }

            template<size_t base>
            template<size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::QAdd(
                unsigned_biginteger& result,
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs) {
                WASSERT_LEVEL_2(lhs.size() >= rhs.size());
                WASSERT_LEVEL_2((!index) || (index == rhs.size()));
                auto res = result.size();
                auto lp = lhs.data();
                auto ls = lhs.size();
                auto rp = rhs.data();
                auto rs = rhs.size();
                unsigned char cf = 0;
                if constexpr (index != 1) {
                    // if res < rs
                    // then lhs or rhs won't be a part of result
                    if (res < rs) {
                        result.reserve(ls + 1);
                    }
                }
                // we may need to reserve result
                // so need to get data() after it
                auto rep = result.data();
                if constexpr (index != 0) {
                    if constexpr (index >= 1) {
                        cf = traits::add(lp[0], rp[0], rep + 0);
                    }
                    if constexpr (index >= 2) {
                        cf = traits::adc(cf, lp[1], rp[1], rep + 1);
                    }
                    if constexpr (index >= 3) {
                        cf = traits::adc(cf, lp[2], rp[2], rep + 2);
                    }
                    if constexpr (index >= 4) {
                        cf = traits::adc(cf, lp[3], rp[3], rep + 3);
                    }
                }
                else {
                    cf = traits::adc(lp, rp, rep, rs);
                }
                // first use memcpy,then append
                // if res < ls => lhs isn't a part of result
                // obviously we can do it correcly
                // else lhs may ba a part of result
                // mid = ls - rs
                // if lhs is a part of result,then we have result.begin() <= lhs.data() < result.end()
                // so memcpy will work correctly and if rep == lp , then we don't need to memcpy
                // then the insert won't work,and resize to a corrct size
                if (ls <= res) {
                    _Biginteger_equal_copy_n(lp + rs, ls - rs, rep + rs);
                }
                else {
                    result.reserve(ls + 1);
                    rep = result.data();
                    std::copy(lp + rs, lp + ls, rep + rs);
                }
                result.set_size(ls);
                if (cf) {
                    size_t pos = rs;
                    while (pos != ls && rep[pos] == traits::max()) {
                        rep[pos] = 0;
                        ++pos;
                    }
                    if (pos == ls) {
                        result.push_back(1);
                    }
                    else {
                        ++rep[pos];
                    }
                }
            }

            template<size_t base>
            template<size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::QSub(
                unsigned_biginteger& result,
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs
            ) {
                WASSERT_LEVEL_3(less_eq(rhs, lhs));
                auto res = result.size();
                auto lp = lhs.data();
                auto ls = lhs.size();
                auto rp = rhs.data();
                auto rs = rhs.size();
                unsigned char cf = 0;
                if constexpr (index != 1) {
                    if (res < rs) {
                        result.reserve(ls + 1);
                    }
                }
                auto rep = result.data();
                if constexpr (index != 0) {
                    if constexpr (index >= 1) {
                        cf = traits::sub(lp[0], rp[0], &rep[0]);
                    }
                    if constexpr (index >= 2) {
                        cf = traits::sbb(cf, lp[1], rp[1], &rep[1]);
                    }
                    if constexpr (index >= 3) {
                        cf = traits::sbb(cf, lp[2], rp[2], &rep[2]);
                    }
                    if constexpr (index >= 4) {
                        cf = traits::sbb(cf, lp[3], rp[3], &rep[3]);
                    }
                }
                else {
                    cf = traits::sbb(lp, rp, rep, rs);
                }
                if (ls <= res) {
                    _Biginteger_equal_copy_n(lp + rs, ls - rs, rep + rs);
                }
                else {
                    result.reserve(ls + 1);
                    rep = result.data();
                    std::copy(lp + rs, lp + ls, rep + rs);
                }
                result.set_size(ls);
                if (cf) {
                    size_t pos = rs;
#if WDEBUG_LEVEL >= 2
                    while (pos != ls && rep[pos] == 0) {
                        rep[pos] = traits::max();
                        ++pos;
                    }
                    WASSERT_LEVEL_2(pos != ls);
#else
                    while (rep[pos] == 0) {
                        rep[pos] = traits::max();
                        ++pos;
                    }
#endif
                    --rep[pos];
                }
                result.maintain();
            }

            template<typename _Ty, size_t _Size>
            USE_THREAD_LOCAL static std::array<_Ty, _Size> _Biginteger_mul_cache = {};

            template<size_t base>
            template<size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::slow_mul(
                unsigned_biginteger& result,
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs
            ) {
                constexpr size_t cache_size = 256;

                auto lp = lhs.data();
                auto ls = lhs.size();
                auto rp = rhs.data();

                if constexpr (index == 1) {
                    auto rv = *rp;
                    auto res = result.size();
                    if (res < ls) {
                        result.reserve(ls + 1);
                    }
                    result.set_size(ls);
                    auto rep = result.data();
                    auto _Val = traits::vector_mul_1(rep, lp, ls, rv);
                    if (_Val) {
                        result.push_back(_Val);
                    }
                }
                else {
                    auto rs = rhs.size();
                    const size_t len = ls + rs;
                    limb_ptr temp_array;

                    WJR_IS_CONSTANT_EVALUATED_BEGIN
                        temp_array = std::allocator<limb_t>().allocate(len);
                    WJR_IS_CONSTANT_EVALUATED_END
                    else {
                    if (len <= cache_size) {
                        temp_array = _Biginteger_mul_cache<limb_t, cache_size>.data();
                    }
                    else {
                        temp_array = std::allocator<limb_t>().allocate(len);
                    }
                    }

                    auto rep = temp_array;
                    rep[ls] = traits::vector_mul_1(rep, lp, ls, *rp);
                    ++rep;
                    ++rp;
                    --rs;
                    if constexpr (index >= 2) {
                        rep[ls] = traits::vector_addmul_1(rep, lp, ls, *rp);
                        ++rep;
                        ++rp;
                        --rs;
                    }
                    if constexpr (index >= 3) {
                        rep[ls] = traits::vector_addmul_1(rep, lp, ls, *rp);
                        ++rep;
                        ++rp;
                        --rs;
                    }
                    if constexpr (index >= 4) {
                        rep[ls] = traits::vector_addmul_1(rep, lp, ls, *rp);
                        ++rep;
                        ++rp;
                        --rs;
                    }
                    if constexpr (!index) {
                        WASSERT_LEVEL_2(rs >= 1);
                        do {
                            rep[ls] = traits::vector_addmul_1(rep, lp, ls, *rp);
                            ++rep;
                            ++rp;
                        } while (--rs != 0);
                    }
                    result.assign(temp_array, temp_array + len);
                    if (!temp_array[len - 1]) {
                        result.set_size(len - 1);
                    }
                    WJR_IS_CONSTANT_EVALUATED_BEGIN
                        std::allocator<limb_t>().deallocate(temp_array, len);
                    WJR_IS_CONSTANT_EVALUATED_END
                    else {
                        if (len > cache_size) {
                            std::allocator<limb_t>().deallocate(temp_array, len);
                        }
                    }
                }

            }

            template<size_t base>
            template<size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::pos_add(
                limb_ptr result,
                const unsigned_biginteger_view<ir>& rhs
            ) {
                auto m = rhs.size();
                auto rp = rhs.data();
                size_t i = 0;
                unsigned char cf = 0;
                for (; i < m; ++i) {
                    cf = traits::adc(cf, result[i], rp[i], &result[i]);
                }
                if (cf) {
                    while (result[i] == traits::max()) {
                        result[i] = 0;
                        ++i;
                    }
                    ++result[i];
                }
            }

            template<size_t base>
            template<size_t il, size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::karatsuba(
                unsigned_biginteger& result,
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs
            ) {
                size_t n = lhs.size();
                size_t m = rhs.size();
                size_t mid = n >> 1;
                WASSERT_LEVEL_1(m > mid);
                unsigned_biginteger_view<0> l_high(lhs.data() + mid, n - mid);
                unsigned_biginteger_view<0> l_low(lhs.data(), mid);
                unsigned_biginteger_view<0> r_high(rhs.data() + mid, m - mid);
                unsigned_biginteger_view<0> r_low(rhs.data(), mid);
                l_low.maintain();
                r_low.maintain();
                unsigned_biginteger A, B, C;
                add(A, l_high, l_low);
                add(C, r_high, r_low);
                mul(B, A, C);
                mul(A, l_high, r_high);
                mul(C, l_low, r_low);
                sub(B, B, A);
                sub(B, B, C);

                result.reserve(n + m);
                result.set_size(n + m);
                std::fill_n(result.data(), n + m, 0);
                auto rep = result.data();
                std::copy_n(A.data(), A.size(), rep + 2 * mid);
                size_t l = B.size() < mid ? B.size() : mid;
                std::copy_n(B.data(), l, rep + mid);
                if (l != B.size()) {
                    pos_add(rep + 2 * mid, unsigned_biginteger_view<0>(B.data() + l, B.size() - l));
                }
                l = C.size() < mid ? C.size() : mid;
                std::copy_n(C.data(), l, rep);
                if (l != C.size()) {
                    pos_add(rep + mid, unsigned_biginteger_view<0>(C.data() + l, C.size() - l));
                }
                result.maintain();
            }

            template<size_t base>
            template<size_t i, size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::dac_mul(
                unsigned_biginteger& result,
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs,
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
                unsigned_biginteger_view<0> g(lhs.data() + i * l, len);
                if constexpr (i != index - 1) {
                    g.maintain();
                }
                mul(temp, g, rhs);
                pos_add(r.data() + i * l, unsigned_biginteger_view<0>(temp.data(), temp.size()));
                if constexpr (i == index - 1) {
                    result = std::move(r);
                    result.maintain();
                }
                else {
                    dac_mul<i + 1, index>(result, lhs, rhs, temp, r);
                }
            }

            template<size_t base>
            template<size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::dac_mul(
                unsigned_biginteger& result,
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs
            ) {
                size_t n = lhs.size();
                size_t m = rhs.size();
                unsigned_biginteger temp, r;
                r.reserve(n + m);
                r.set_size(n + m);
                std::fill_n(r.data(), n + m, 0);
                if constexpr (index != 0) {
                    WASSERT_LEVEL_1(n >= index * m);
                    dac_mul<0, index>(result, lhs, rhs, temp, r);
                }
                else {
                    size_t idx = (n + m - 1) / m;
                    WASSERT_LEVEL_1(n > (idx - 1) * m && n <= idx * m);
                    for (size_t i = 0; i < idx; ++i) {
                        size_t s = i == idx - 1 ? n - i * m : m;
                        unsigned_biginteger_view<0> g(lhs.data() + i * m, s);
                        g.maintain();
                        mul(temp, g, rhs);
                        pos_add(r.data() + i * m, unsigned_biginteger_view<0>(temp.data(), temp.size()));
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

            static biginteger_fft_cache __biginteger_fft_cache;

            template<size_t base>
            template<size_t il, size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::fft_mul(
                unsigned_biginteger& result,
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs
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
                    auto val = *lhs_iter;
                    a[i << 1] = val;
                    a[i << 1 | 1] = val;
                }
                for (size_t i = (n << 1); i < (s << 1); ++i) {
                    a[i] = 0;
                }
                for (size_t i = 0; i < m; ++rhs_iter, ++i) {
                    auto val = *rhs_iter;
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

                result.reserve(pn + pm);
                result.set_size(pn + pm);
                std::fill_n(result.data(), pn + pm, 0);
                unsigned_biginteger_iterator<base>
                    write(result.data(), result.size(), pk);
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
            template<size_t index, size_t il, size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::QMul(
                unsigned_biginteger& result,
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs
            ) {
                if constexpr (index != 0) {
                    if constexpr (index == 1) {
                        if (lhs.test_byte(0) || rhs.test_byte(0)) {
                            result.set_byte(0);
                            return;
                        }
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
#if defined(WJR_CPP_20)
                            if (std::is_constant_evaluated()) {
                                if (3 * m <= n) {
                                    dac_mul<0>(result, lhs, rhs);
                                }
                                else {
                                    toom_cook_3(result, lhs, rhs);
                                }
                                return;
                            }
#endif
                            if (m <= mul_info.toom_cook_3_threshold) {
                                if (m <= n / 3) {
                                    dac_mul<0>(result, lhs, rhs);
                                }
                                else {
                                    toom_cook_3(result, lhs, rhs);
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
            }

            // signed biginteger or integral view
            template<size_t base, size_t length>
            class biginteger_view : public unsigned_biginteger_view<base, length> {
                using _Base = unsigned_biginteger_view<base, length>;
            public:
                template<typename T, std::enable_if_t<_Is_unsigned_integral_v<T>, int> = 0>
                constexpr explicit biginteger_view(T val)
                    noexcept : _Base(val) {}
                template<typename T, std::enable_if_t<_Is_signed_integral_v<T>, int> = 0>
                constexpr explicit biginteger_view(T val)
                    noexcept : _Signal(val >= 0),
                    _Base(static_cast<std::make_unsigned_t<T>>(val >= 0 ? val : (0 - val))) {}

                constexpr biginteger_view(const biginteger_view&) noexcept = default;
                constexpr biginteger_view& operator=(const biginteger_view&) noexcept = default;
                constexpr bool signal()const noexcept { return _Signal; }
                constexpr _Base& get_unsigned() noexcept { return *this; }
                constexpr const _Base& get_unsigned()const noexcept { return *this; }
                template<typename T>
                constexpr explicit operator T()const noexcept {
                    auto _Val = _Base::template to<T>();
                    return _Signal ? _Val : (0 - _Val);
                }
                constexpr void maintain_signal() noexcept {
                    _Signal = _Signal || _Base::test_byte(0);
                }

                constexpr void negate() noexcept {
                    _Signal = (!_Signal) || _Base::test_byte(0);
                }
                constexpr void abs() noexcept {
                    _Signal = true;
                }
            private:
                bool _Signal = true;
            };

            template<size_t base>
            class biginteger_view<base, 0> : public unsigned_biginteger_view<base, 0> {
                using _Base = unsigned_biginteger_view<base, 0>;
            public:
                constexpr biginteger_view(limb_const_ptr _Data, size_t _Size)
                    noexcept : _Base(_Data, _Size) {}
                constexpr biginteger_view(bool _Signal, limb_const_ptr _Data, size_t _Size)
                    noexcept : _Base(_Data, _Size), _Signal(_Signal) {
                    maintain_signal();
                }

                constexpr explicit biginteger_view(const biginteger<base>& rhs)
                    noexcept : _Base(rhs.data(), rhs.size()), _Signal(rhs.signal()) {
                    // don't need to maintain signal
                }
                constexpr biginteger_view(const biginteger_view&) noexcept = default;
                constexpr biginteger_view& operator=(const biginteger_view&) noexcept = default;
                constexpr bool signal()const noexcept {
                    WASSERT_LEVEL_1(!(_Base::test_byte(0) && !_Signal));
                    return _Signal;
                }
                constexpr _Base& get_unsigned() noexcept {
                    return *this;
                }
                constexpr const _Base& get_unsigned()const noexcept {
                    return *this;
                }
                template<typename T>
                constexpr explicit operator T()const noexcept {
                    auto _Val = _Base::template to<T>();
                    return _Signal ? _Val : (0 - _Val);
                }
                constexpr biginteger_view& maintain() noexcept {
                    _Base::maintain();
                    maintain_signal();
                    return *this;
                }
                constexpr void maintain_signal() noexcept {
                    _Signal = _Signal || _Base::test_byte(0);
                }
                constexpr void negate() noexcept {
                    _Signal = (!_Signal) || _Base::test_byte(0);
                }
                constexpr void abs() noexcept {
                    _Signal = true;
                }
            private:
                bool _Signal = true;
            };

            template<typename T>
            struct biginteger_base_helper {
                constexpr static size_t value = std::is_integral_v<T> ? 0 : -1;
            };

            template<size_t base>
            struct biginteger_base_helper<unsigned_biginteger<base>> {
                constexpr static size_t value = base;
            };

            template<size_t base>
            struct biginteger_base_helper<biginteger<base>> {
                constexpr static size_t value = base;
            };

            template<size_t base, size_t length>
            struct biginteger_base_helper<unsigned_biginteger_view<base, length>> {
                constexpr static size_t value = base;
            };

            template<size_t base, size_t length>
            struct biginteger_base_helper<biginteger_view<base, length>> {
                constexpr static size_t value = base;
            };

            template<typename T>
            struct biginteger_base : biginteger_base_helper<std::decay_t<T>> {
            };

            template<typename T>
            constexpr static size_t biginteger_base_v = biginteger_base<T>::value;

            template<typename T, typename...Args>
            struct biginteger_base_mask {
            private:
                constexpr static size_t lvalue = biginteger_base_v<T>;
                constexpr static size_t rvalue = biginteger_base_mask<Args...>::value;
                constexpr static size_t get_value() {
                    if constexpr (lvalue == size_t(-1) || rvalue == size_t(-1)) {
                        // at least one is illegal
                        return -1;
                    }
                    else if constexpr (lvalue == 0) {
                        // if one is integral
                        return rvalue;
                    }
                    else if constexpr (rvalue == 0) {
                        return lvalue;
                    }
                    else if constexpr (lvalue == rvalue) {
                        return lvalue;
                    }
                    else return -1; // illegal
                }
            public:
                constexpr static size_t value = get_value();
            };

            template<typename T>
            struct biginteger_base_mask<T> {
                constexpr static size_t value = biginteger_base_v<T>;
            };

            template<typename T, typename...Args>
            constexpr static size_t biginteger_base_mask_v =
                biginteger_base_mask<T, Args...>::value;

            template<typename T>
            struct _Is_biginteger : std::false_type {};

            template<size_t base>
            struct _Is_biginteger<biginteger<base>> : std::true_type {};

            template<typename T>
            constexpr static bool _Is_biginteger_v = _Is_biginteger<T>::value;

            template<typename T>
            struct _Is_original_biginteger_view : std::false_type {};

            template<size_t base, size_t length>
            struct _Is_original_biginteger_view<
                biginteger_view<base, length>> : std::true_type {};

            template<typename T>
            constexpr static bool _Is_original_biginteger_view_v = _Is_original_biginteger_view<T>::value;

            template<size_t base>
            struct __biginteger_view {
                template<typename T, std::enable_if_t<_Is_base_mask_v<base, biginteger_base_v<T>>, int> = 0>
                constexpr decltype(auto) operator()(const T& t) const {
                    using traits = biginteger_traits<base>;
                    if constexpr (std::is_integral_v<T>) {
                        return biginteger_view<base, traits::template bits_of_t<T>>(t);
                    }
                    else if constexpr (_Is_unsigned_biginteger_v<T>) {
                        return biginteger_view<base, 0>(true, t.data(), t.size());
                    }
                    else if constexpr (_Is_biginteger_v<T>) {
                        return biginteger_view<base, 0>(t.signal(), t.data(), t.size());
                    }
                    else if constexpr (_Is_original_unsigned_biginteger_view_v<T>) {
                        return biginteger_view<base, 0>(true, t.data(), t.size());
                    }
                    else return t;
                }
            };

            template<size_t base>
            inline constexpr __biginteger_view<base> view;

            template<size_t base, typename...Args>
            struct _Is_biginteger_base_masks :
                std::conjunction<_Is_base_mask<base, biginteger_base_v<Args>>...> {};

            template<size_t base, typename...Args>
            constexpr static bool _Is_biginteger_base_masks_v =
                _Is_biginteger_base_masks<base, Args...>::value;

#define VIRTUAL_BIGINTEGER_BINARY_DECLARATION                                                   \
    template<typename T, typename U,                                                            \
    std::enable_if_t<_Is_biginteger_base_masks_v<base, T, U>, int> = 0>                         \

#define VIRTUAL_BIGINTEGER_SINGLE_AUTO_DECLARATION                                              \
    template<typename T, size_t _Base = biginteger_base_mask_v<T>,                              \
    std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> = 0>                           \

#define VIRTUAL_BIGINTEGER_SINGLE_AUTO_DEFINITION                                               \
    template<typename T, size_t _Base,                                                          \
    std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> >                              \


#define VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION                                              \
    template<typename T, typename U, size_t _Base = biginteger_base_mask_v<T, U>,               \
    std::enable_if_t<                                                                           \
    std::conjunction_v<_Is_biginteger_base_masks<_Base, T, U>>, int> = 0>                       \

#define VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION                                               \
    template<typename T, typename U, size_t _Base,                                              \
    std::enable_if_t<                                                                           \
    std::conjunction_v<_Is_biginteger_base_masks<_Base, T, U>>, int> >                          \

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR int cmp(const T& lhs, const U& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR void add(
                    biginteger<_Base>& result,
                    const T& lhs,
                    const U& rhs
                );
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR void sub(
                    biginteger<_Base>& result,
                    const T& lhs,
                    const U& rhs
                );
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR void mul(
                    biginteger<_Base>& result,
                    const T& lhs,
                    const U& rhs
                );

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR bool operator==(const T& lhs, const U& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR bool operator!=(const T& lhs, const U& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR bool operator<(const T& lhs, const U& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR bool operator<=(const T& lhs, const U& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR bool operator>(const T& lhs, const U& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR bool operator>=(const T& lhs, const U& rhs);

#if defined(WJR_CPP_20)
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR std::strong_ordering operator<=>(const T& lhs, const U& rhs) {
                int f = cmp(lhs, rhs);
                if (f < 0) return std::strong_ordering::less;
                if (f > 0) return std::strong_ordering::greater;
                return std::strong_ordering::equal;
            }
#endif

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR biginteger<_Base> operator+(T&& lhs, U&& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR biginteger<_Base> operator-(T&& lhs, U&& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR biginteger<_Base> operator*(T&& lhs, U&& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR biginteger<_Base> operator/(T&& lhs, U&& rhs);
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
                WJR_CONSTEXPR biginteger<_Base> operator%(T&& lhs, U&& rhs);

            template<size_t _Base>
            WJR_CONSTEXPR void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);
            template<size_t _Base>
            WJR_CONSTEXPR biginteger<_Base> pow(const biginteger<_Base>& a, size_t n);
            template<size_t _Base>
            WJR_CONSTEXPR biginteger<_Base> pow(biginteger<_Base>&& a, size_t n);

            template<size_t toBase, typename T, size_t fromBase = biginteger_base_v<T>,
                std::enable_if_t<_Is_biginteger_base_masks_v<fromBase, T>, int> = 0>
            WJR_CONSTEXPR biginteger<toBase> base_conversion(T&&);

            // generate a biginteger of length n
            template<size_t _Base>
            WJR_CONSTEXPR void
                random_biginteger(
                    biginteger<_Base>& result,
                    size_t n,
                    bool _unsigned = true,
                    bool exact = true
                );
            template<size_t _Base>
            WJR_CONSTEXPR biginteger<_Base>
                random_biginteger(
                    size_t n,
                    bool _unsigned = true,
                    bool exact = true
                );

            template<size_t _Base, typename T,
                std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> = 0>
            WJR_CONSTEXPR void
                random_biginteger_max(
                    biginteger<_Base>& result,
                    const T& _Max
                );
            template<size_t _Base, typename T, typename U,
                std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T, U>, int> = 0>
            WJR_CONSTEXPR void
                random_biginteger_min_max(
                    biginteger<_Base>& result,
                    const T& _Min,
                    const U& _Max
                );

            template<size_t _Base, typename T,
                std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> = 0>
            WJR_CONSTEXPR biginteger<_Base> random_biginteger_max(const T& _Max);
            template<size_t _Base, typename T, typename U,
                std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T, U>, int> = 0>
            WJR_CONSTEXPR biginteger<_Base> random_biginteger_min_max(const T& _Min, const U& _Max);

            template<size_t base>
            class biginteger {
                template<size_t _Base>
                friend class biginteger;
                template<size_t _Base>
                friend class unsigned_biginteger;

                using traits = biginteger_traits<base>;
                using unsigned_traits = unsigned_biginteger<base>;
                template<size_t length>
                using biginteger_view = biginteger_view<base, length>;
                template<typename T>
                using _Is_biginteger_view =
                    _Is_base_mask<base, biginteger_base_v<T>>;
                template<typename T>
                constexpr static bool _Is_biginteger_view_v =
                    _Is_biginteger_view<T>::value;
                template<typename T>
                using _Is_rbint = std::conjunction<
                    std::is_rvalue_reference<T>,
                    _Is_biginteger<std::decay_t<T>>>;
                template<typename T>
                constexpr static bool _Is_rbint_v = _Is_rbint<T>::value;
                template<typename T>
                using _Is_rubint = std::conjunction<
                    std::is_rvalue_reference<T>,
                    _Is_unsigned_biginteger<std::decay_t<T>>>;
                template<typename T>
                constexpr static bool _Is_rubint_v = _Is_rubint<T>::value;
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

                using view_type = biginteger_view<base>;

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

                template<size_t length>
                WJR_CONSTEXPR20 explicit biginteger(const biginteger_view<length>& rhs)
                    : _Signal(rhs.signal()), ubint(rhs.get_unsigned()) {}
                template<typename T, std::enable_if_t<_Is_biginteger_view_v<T>, int> = 0>
                WJR_CONSTEXPR20 explicit biginteger(const T& val)
                    : biginteger(view<base>(val)) {}

                template<typename iter, typename Func, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
                WJR_CONSTEXPR20 void in_bits(iter _First, iter _Last, Func&&fn) {
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

                WJR_CONSTEXPR20 explicit biginteger(String_view str) {
                    in_bits(str.begin(), str.end(), traits::from_string);
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
                template<size_t length>
                WJR_CONSTEXPR20 biginteger& operator=(const biginteger_view<length>& rhs) {
                    _Signal = rhs.signal();
                    ubint = rhs.get_unsigned();
                    return *this;
                }
                template<typename T, std::enable_if_t<_Is_biginteger_view_v<T>, int> = 0>
                WJR_CONSTEXPR20 biginteger& operator=(const T& rhs) {
                    (*this) = view<base>(rhs);
                    return *this;
                }
                WJR_CONSTEXPR20 biginteger& operator=(unsigned_biginteger<base>&& other) {
                    _Signal = true;
                    ubint = std::move(other);
                    return *this;
                }
                WJR_CONSTEXPR20 biginteger& operator=(String_view str) {
                    in_bits(str.begin(), str.end(), traits::from_string);
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

                WJR_CONSTEXPR20 void swap(biginteger& other) noexcept {
                    std::swap(_Signal, other._Signal);
                    ubint.swap(other.ubint);
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

                WJR_CONSTEXPR20 void reserve(const size_type n) { ubint.reserve(n); }
                WJR_CONSTEXPR20 void bit_reserve(const size_type n) { ubint.reserve((n + traits::bits - 1) / traits::bits); }
                WJR_CONSTEXPR20 bool test_byte(const value_type val) { return ubint.test_byte(val); }
                WJR_CONSTEXPR20 void set_byte(const value_type val) { ubint.set_byte(val); }
                WJR_CONSTEXPR20 bool signal() const noexcept { return _Signal; }
                WJR_CONSTEXPR20 void maintain_signal() { _Signal |= ubint.test_byte(0); }
                WJR_CONSTEXPR20 biginteger& maintain() {
                    ubint.maintain();
                    maintain_signal();
                    return *this;
                }
                WJR_CONSTEXPR20 void negate() { _Signal = (!_Signal) || ubint.test_byte(0); }
                WJR_CONSTEXPR20 void set_signal(bool f) { _Signal = f || ubint.test_byte(0); }
                WJR_CONSTEXPR20 void abs() { _Signal = true; }

                WJR_CONSTEXPR20 size_t bit_size() const {
                    return ubint.bit_size();
                }

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

                WJR_CONSTEXPR20 biginteger& pow(size_t n) {
                    ubint.pow(n);
                    _Signal = _Signal || (!(n & 1));
                    return *this;
                }

                template<typename T>
                WJR_CONSTEXPR20 explicit operator T()const {
                    return static_cast<T>(view<base>(*this));
                }

                WJR_CONSTEXPR String tostr() const {
                    size_t n = bit_size();
                    String str;
                    str.resize(n + 1);
                    auto it = out_bits(str.begin(), str.end(), traits::to_string);
                    size_t g = it.value() - str.begin();
                    str.resize(g);
                    return str;
                }

            private:

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static int signed_cmp(
                    const biginteger_view<il>& lhs,
                    const biginteger_view<ir>& rhs) {
                    // if lhs < rhs , return -1
                    // if lhs > rhs , return 1
                    // if lhs == rhs , return 0
                    int f = lhs.signal() != rhs.signal() ?
                        1 : unsigned_traits::cmp(lhs.get_unsigned(), rhs.get_unsigned());
                    return lhs.signal() ? f : -f;
                }

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void unsigned_add(
                    biginteger& result,
                    const biginteger_view<il>& lhs,
                    const biginteger_view<ir>& rhs
                ) {
                    unsigned_traits::add(result.ubint, lhs.get_unsigned(), rhs.get_unsigned());
                    result._Signal = lhs.signal();
                }

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void unsigned_sub(
                    biginteger& result,
                    const biginteger_view<il>& lhs,
                    const biginteger_view<ir>& rhs
                ) {
                    int _cmp = unsigned_traits::cmp(lhs.get_unsigned(), rhs.get_unsigned());
                    if (_cmp > 0) {
                        unsigned_traits::sub(result.ubint, lhs.get_unsigned(), rhs.get_unsigned());
                        result._Signal = lhs.signal();
                    }
                    else if (_cmp < 0) {
                        unsigned_traits::sub(result.ubint, rhs.get_unsigned(), lhs.get_unsigned());
                        result._Signal = !lhs.signal();
                    }
                    else {
                        result.set_byte(0);
                    }
                }

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void signed_add(
                    biginteger& result,
                    const biginteger_view<il>& lhs,
                    const biginteger_view<ir>& rhs
                ) {
                    return lhs.signal() == rhs.signal() ?
                        unsigned_add(result, lhs, rhs) : unsigned_sub(result, lhs, rhs);
                }

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static void signed_sub(
                    biginteger& result,
                    const biginteger_view<il>& lhs,
                    const biginteger_view<ir>& rhs
                ) {
                    return lhs.signal() == rhs.signal() ?
                        unsigned_sub(result, lhs, rhs) : unsigned_add(result, lhs, rhs);
                }

                enum class div_mode {
                    none,
                    lhs_is_power_of_base,
                };

                template<limb_t ir>
                WJR_CONSTEXPR static limb_t one_divmod(
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

                WJR_CONSTEXPR static limb_t one_divmod(
                    biginteger& lhs,
                    limb_t v) {
                    size_t n = lhs.size();
                    auto p = lhs.data();
                    double_limb_t _Val = 0;
                    WASSERT_LEVEL_1(v != 0);
                    WJR_IS_CONSTANT_EVALUATED_BEGIN
                        for (size_t i = n; i--;) {
                            _Val = _Val * traits::max_value() + p[i];
                            p[i] = static_cast<limb_t>(_Val / v);
                            _Val = _Val - traits::mul(p[i], v);
                        }
                    WJR_IS_CONSTANT_EVALUATED_END
                    else {
                    if (n >= 8) {
                        biginteger_libdivider<limb_t> _Divider(v);
                        for (size_t i = n; i--;) {
                            _Val = _Val * traits::max_value() + p[i];
                            p[i] = static_cast<limb_t>(_Val / _Divider);
                            _Val = _Val - traits::mul(p[i], v);
                        }
                    }
                    else {
                        for (size_t i = n; i--;) {
                            _Val = _Val * traits::max_value() + p[i];
                            p[i] = static_cast<limb_t>(_Val / v);
                            _Val = _Val - traits::mul(p[i], v);
                        }
                    }
                    }
                    if (n != 1 && !p[n - 1]) {
                        lhs.ubint.set_size(n - 1);
                        WASSERT_LEVEL_1(lhs.ubint.back() != 0);
                    }
                    return static_cast<limb_t>(_Val);
                }

                template<div_mode mode>
                WJR_CONSTEXPR static void knuth_divmod(biginteger& lhs, biginteger& rhs) {
                    auto vr = rhs.data()[rhs.size() - 1];
                    auto kth = static_cast<limb_t>(((traits::max_value() >> 1) + vr - 1) / vr);
                    if (kth != 1) {
                        lhs *= kth;
                        rhs *= kth;
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
                    r = biginteger_view<0>(true, pl + m, n);
					
                    biginteger temp;
                    temp.ubint.reserve(m + 1);
                    pl = pl + m - 1;
                    for (size_t j = m + 1; j--; --pl) {
                        limb_t q = 0;
                        if (r.size() == n + 1) {
                            q = std::min(traits::max(),
                                static_cast<limb_t>((traits::max_value() * r.data()[n] + r.data()[n - 1]) / _Divider2));
                        }
                        else if (r.size() == n) {
                            q = std::min(traits::max(), r.data()[n - 1] / _Divider1);
                        }
                        else q = 0;

                        mul(temp, rhs, q);
                        sub(r, r, temp);
                        while (!r.signal()) {
                            --q;
                            add(r, r, rhs);
                        }

                        presult[j] = q;
                        if (j != 0) {
                            r.mul_base_power(traits::bits);
                            r.data()[0] = *pl;
                        }
                    }
                    result.maintain();
                    lhs = std::move(result);
                    rhs = std::move(r);

                    if (kth != 1) {
                        one_divmod(rhs, kth);
                    }
                }

                template<div_mode mode>
                WJR_CONSTEXPR static void small_divmod(biginteger& lhs, biginteger& rhs) {
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
                        auto cl = biginteger_view<0>(true, lhs.data() + l - ml, ml).maintain();
                        if (mo.size() < cl.size()) {
                            mo.ubint.reserve(cl.size());
                            mo.ubint.set_size(cl.size());
                        }
                        std::copy_n(cl.data(), cl.size(), mo.data());
                        if (mo.size() > g) {
                            mul(mid_result, biginteger_view<0>(true, mo.data() + g, mo.size() - g), E);
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
                WJR_CONSTEXPR static void mid_divmod(biginteger& lhs, biginteger& rhs) {
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

                    biginteger ans1(biginteger_view<0>(true, rhs.data() + p, m - p));
                    quick_divmod<div_mode::lhs_is_power_of_base>(E, ans1);
                    size_t g = k + 1 - E.size();
                    if constexpr (mode == div_mode::none) {
                        mul(ans1, biginteger_view<0>(true, lhs.data() + mid + p + g, k - g), E);
                        ans1.div_base_power(E.size() * traits::bits);
                    }
                    else {
                        size_t bl = lhs.bit_size();
                        ans1 = E;
                        ans1.div_base_power(2 * traits::bits - ((bl - 1) % traits::bits));
                    }

                    mul(temp, ans1, rhs);

                    biginteger mo1;
                    sub(mo1, biginteger_view<0>(true, lhs.data() + mid, n - mid), temp);

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
                        auto view = biginteger_view<0>(true, lhs.data(), mid).maintain();
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
                    mul(ans2, biginteger_view<0>(true, mo1.data() + p + g, mo1.size() - ml).maintain(), E);
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
                WJR_CONSTEXPR static void large_divmod(biginteger& lhs, biginteger& rhs) {
                    size_t n = lhs.size();
                    size_t m = rhs.size();
                    size_t mid = 2 * m - n - 2;
                    biginteger copyA(biginteger_view<0>(true, lhs.data() + mid, n - mid));
                    biginteger copyB(biginteger_view<0>(true, rhs.data() + mid, m - mid));
                    ++copyA;
                    quick_divmod<div_mode::none>(copyA, copyB);
                    --copyB;

                    auto view = biginteger_view<0>(true, lhs.data(), mid).maintain();
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

                    mul(lhs, biginteger_view<0>(true, rhs.data(), mid).maintain(), copyA);
                    sub(copyB, copyB, lhs);
                    if (!copyB.signal()) {
                        --copyA;
                        copyB += rhs;
                    }

                    lhs = std::move(copyA);
                    rhs = std::move(copyB);
                }

                template<div_mode mode = div_mode::none>
                WJR_CONSTEXPR static void quick_divmod(biginteger& lhs, biginteger& rhs) {
                    WASSERT_LEVEL_1(!rhs.test_byte(0) && lhs.signal() && rhs.signal());
                    WASSERT_LEVEL_1(std::addressof(lhs) != std::addressof(rhs));
                    int cmp = unsigned_biginteger<base>::cmp(lhs.ubint, rhs.ubint);
                    if (cmp < 0) {
                        lhs.swap(rhs);
                        lhs.set_byte(0);
                        return;
                    }
                    if (cmp == 0) {
                        lhs = biginteger_view<1>(1u);
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
                    WASSERT_LEVEL_3(q * lhs + rhs == p);
                }

            public:

                template<typename T, std::enable_if_t<_Is_biginteger_view_v<T>, int> = 0>
                WJR_CONSTEXPR biginteger& operator+=(const T& rhs) {
                    add(*this, *this, rhs);
                    return *this;
                }

                template<typename T, std::enable_if_t<_Is_biginteger_view_v<T>, int> = 0>
                WJR_CONSTEXPR biginteger& operator-=(const T& rhs) {
                    sub(*this, *this, rhs);
                    return *this;
                }

                template<typename T, std::enable_if_t<_Is_biginteger_view_v<T>, int> = 0>
                WJR_CONSTEXPR biginteger& operator*=(const T& rhs) {
                    mul(*this, *this, rhs);
                    return *this;
                }

                template<typename T, std::enable_if_t<_Is_biginteger_view_v<T>, int> = 0>
                WJR_CONSTEXPR biginteger& operator/=(T&& rhs) {
                    auto _Rhs = view<base>(std::forward<T>(rhs));
                    bool f = signal() ^ (!_Rhs.signal());
                    abs();
                    _Rhs.abs();
                    if constexpr (std::is_same_v<decltype(_Rhs), biginteger_view<1>>) {
                        one_divmod(*this, *_Rhs.data());
                    }
                    else {
                        if constexpr (std::disjunction_v<_Is_rbint<T&&>, _Is_rubint<T&&>>) {
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

                template<typename T, std::enable_if_t<_Is_biginteger_view_v<T>, int> = 0>
                WJR_CONSTEXPR biginteger& operator%=(T&& rhs) {
                    auto _Rhs = view<base>(std::forward<T>(rhs));
                    bool f = signal();
                    abs();
                    _Rhs.abs();
                    if constexpr (std::is_same_v<decltype(_Rhs), biginteger_view<1>>) {
                        *this = one_divmod(*this, *_Rhs.data());
                    }
                    else {
                        if constexpr (std::disjunction_v<_Is_rbint<T&&>, _Is_rubint<T&&>>) {
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

                WJR_CONSTEXPR biginteger& operator++() {
                    (*this) += biginteger_view<1>(1u);
                    return *this;
                }

                WJR_CONSTEXPR biginteger operator++(int) {
                    biginteger it(*this);
                    ++(*this);
                    return it;
                }

                WJR_CONSTEXPR biginteger& operator--() {
                    (*this) -= biginteger_view<1>((uint32_t)1);
                    return *this;
                }

                WJR_CONSTEXPR biginteger operator--(int) {
                    biginteger it(*this);
                    --(*this);
                    return it;
                }

                friend WJR_CONSTEXPR bool operator!(const biginteger& rhs) {
                    return rhs.test_byte(0);
                }

                WJR_CONSTEXPR biginteger& random(size_t n, bool _unsigned = true, bool _exact = false) {
                    random_biginteger<base>(*this, n, _unsigned, _exact);
                    return *this;
                }

            private:

                template<size_t il, size_t ir>
                WJR_CONSTEXPR static bool equal(
                    const biginteger_view<il>& lhs,
                    const biginteger_view<ir>& rhs) {
                    return lhs.signal() == rhs.signal()
                        && unsigned_traits::equal(lhs.get_unsigned(), rhs.get_unsigned());
                }
                template<size_t il, size_t ir>
                WJR_CONSTEXPR static bool less(
                    const biginteger_view<il>& lhs,
                    const biginteger_view<ir>& rhs) {
                    return lhs.signal() != rhs.signal() ?
                        lhs.signal() < rhs.signal() :
                        (unsigned_traits::less(lhs.get_unsigned(), rhs.get_unsigned()) ^ (!lhs.signal()));
                }
                template<size_t il, size_t ir>
                WJR_CONSTEXPR static bool less_eq(
                    const biginteger_view<il>& lhs,
                    const biginteger_view<ir>& rhs) {
                    return lhs.signal() != rhs.signal() ?
                        lhs.signal() < rhs.signal() :
                        lhs.signal() ? unsigned_traits::less_eq(lhs.get_unsigned(), rhs.get_unsigned())
                        : !unsigned_traits::less(lhs.get_unsigned(), rhs.get_unsigned());
                }

                template<size_t toBase, size_t length>
                WJR_CONSTEXPR static biginteger<toBase>
                    dc_base_conversion_helper(
                        const unsigned_biginteger_view<base, length>& v,
                        std::vector<std::pair<size_t,
                        biginteger<toBase>>>& cache
                    ) {
                    size_t n = v.size();
                    if (n <= 2) {
                        return biginteger<toBase>(static_cast<double_limb_t>(v));
                    }
                    size_t mid = n >> 1;
                    unsigned_biginteger_view<base, 0> lhs(v.data() + mid, n - mid);
                    unsigned_biginteger_view<base, 0> rhs(v.data(), mid);
                    rhs.maintain();
                    auto _Lhs = dc_base_conversion_helper<toBase>(lhs, cache);
                    auto _Rhs = dc_base_conversion_helper<toBase>(rhs, cache);
                    auto iter =
                        std::lower_bound(cache.begin(), cache.end(), mid,
                            [](const auto& x, size_t c) {return x.first < c; });

                    if (iter != cache.end() && iter->first == mid) {
                        _Lhs *= iter->second;
                    }
                    else {
                        auto iter_mid = std::lower_bound(cache.begin(), cache.end(), mid / 2,
                            [](const auto& x, size_t c) {return x.first < c; });
                        biginteger<toBase> _Mid = iter_mid->second * iter_mid->second;
                        if (mid & 1)_Mid *= cache[0].second;
                        _Lhs *= _Mid;
                        cache.emplace(iter, mid, std::move(_Mid));
                    }
                    return _Lhs += _Rhs;
                }

                template<size_t toBase, typename T>
                WJR_CONSTEXPR static biginteger<toBase> base_conversion_helper(T&& v) {
                    if constexpr (toBase == base) {
                        return biginteger<toBase>(std::forward<T>(v));
                    }
                    else {
                        std::vector<std::pair<size_t, biginteger<toBase>>> cache;
                        cache.reserve(30);
                        cache.emplace_back(1, biginteger<toBase>(traits::max_value()));
                        cache.emplace_back(2, cache[0].second * cache[0].second);
                        auto _view = view<base>(v);
                        biginteger<toBase> result = dc_base_conversion_helper<toBase>(
                            _view.get_unsigned(), cache);
                        if (!_view.signal())result.negate();
                        return result;
                    }
                }

                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR int cmp(const T&, const U&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR void add(
                        biginteger<_Base>& result,
                        const T& lhs,
                        const U& rhs
                    );
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR void sub(
                        biginteger<_Base>& result,
                        const T& lhs,
                        const U& rhs
                    );
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR void mul(
                        biginteger<_Base>& result,
                        const T& lhs,
                        const U& rhs
                    );


                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR bool operator==(const T&, const U&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR bool operator!=(const T&, const U&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR bool operator<(const T&, const U&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR bool operator<=(const T&, const U&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR bool operator>(const T&, const U&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR bool operator>=(const T&, const U&);

                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR biginteger<_Base> operator+(T&&, U&&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR biginteger<_Base> operator-(T&&, U&&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR biginteger<_Base> operator*(T&&, U&&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR biginteger<_Base> operator/(T&&, U&&);
                VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                    friend WJR_CONSTEXPR biginteger<_Base> operator%(T&&, U&&);

                template<size_t _Base>
                friend WJR_CONSTEXPR void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);

                template<size_t _Base>
                friend WJR_CONSTEXPR biginteger<_Base> pow(const biginteger<_Base>& a, size_t n);
                template<size_t _Base>
                friend WJR_CONSTEXPR biginteger<_Base> pow(biginteger<_Base>&& a, size_t n);

                template<size_t toBase, typename T, size_t fromBase,
                    std::enable_if_t<_Is_biginteger_base_masks_v<fromBase, T>, int> >
                friend WJR_CONSTEXPR biginteger<toBase> base_conversion(T&&);
                template<size_t _Base>
                friend WJR_CONSTEXPR void random_biginteger(biginteger<_Base>& result, size_t n, bool, bool);
                template<size_t _Base, typename T,
                    std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> >
                friend WJR_CONSTEXPR void random_biginteger_max(biginteger<_Base>& result, const T& _Max);
                bool _Signal = true;
                unsigned_biginteger<base> ubint;
            };

            template<size_t base>
            template<size_t il, size_t ir>
            WJR_CONSTEXPR void unsigned_biginteger<base>::toom_cook_3(
                unsigned_biginteger& result,
                const unsigned_biginteger_view<il>& lhs,
                const unsigned_biginteger_view<ir>& rhs
            ) {
                size_t n = lhs.size(), m = rhs.size();
                size_t n3 = n / 3;
                WASSERT_LEVEL_1(m > n3);
                unsigned_biginteger_view<0> U0(lhs.data(), n3);
                unsigned_biginteger_view<0> U1(lhs.data() + n3, n3);
                unsigned_biginteger_view<0> U2(lhs.data() + 2 * n3, n - 2 * n3);
                unsigned_biginteger_view<0> V0(rhs.data(), n3);
                size_t _L1 = m < 2 * n3 ? m - n3 : n3;
                unsigned_biginteger_view<0> V1(_L1 ? rhs.data() + n3 : 0, _L1);
                size_t _L2 = m < 2 * n3 ? 0 : m - 2 * n3;
                unsigned_biginteger_view<0> V2(_L2 ? rhs.data() + 2 * n3 : 0, _L2);
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
                result.set_size(n + m);
                std::fill_n(result.data(), n + m, 0);

                std::copy_n(W3.data(), W3.size(), result.data() + 3 * n3);
                size_t l = W1.size() < 2 * n3 ? W1.size() : 2 * n3;
                std::copy_n(W1.data(), l, result.data() + n3);
                if (l != W1.size()) {
                    pos_add(result.data() + 3 * n3, unsigned_biginteger_view<0>(W1.data() + l, W1.size() - l));
                }
                l = W0.size() < n3 ? W0.size() : n3;
                std::copy_n(W0.data(), l, result.data());
                if (l != W0.size()) {
                    pos_add(result.data() + n3, unsigned_biginteger_view<0>(W0.data() + l, W0.size() - l));
                }
                result.maintain();
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR int cmp(const T& lhs, const U& rhs) {
                return biginteger<_Base>::signed_cmp(
                    view<_Base>(lhs),
                    view<_Base>(rhs)
                );
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR void add(
                    biginteger<_Base>& result,
                    const T& lhs,
                    const U& rhs
                ) {
                biginteger<_Base>::signed_add(
                    result,
                    view<_Base>(lhs),
                    view<_Base>(rhs)
                );
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR void sub(
                    biginteger<_Base>& result,
                    const T& lhs,
                    const U& rhs
                ) {
                biginteger<_Base>::signed_sub(
                    result,
                    view<_Base>(lhs),
                    view<_Base>(rhs)
                );
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR void mul(
                    biginteger<_Base>& result,
                    const T& lhs,
                    const U& rhs
                ) {
                using unsigned_traits = unsigned_biginteger<_Base>;
                const auto l = view<_Base>(lhs);
                const auto r = view<_Base>(rhs);
                unsigned_traits::mul(result.ubint, l.get_unsigned(), r.get_unsigned());
                result.set_signal(!(l.signal() ^ r.signal()));
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR bool operator==(const T& lhs, const U& rhs) {
                return biginteger<_Base>::equal(
                    view<_Base>(lhs),
                    view<_Base>(rhs)
                );
            }
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR bool operator!=(const T& lhs, const U& rhs) {
                return !(lhs == rhs);
            }
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR bool operator<(const T& lhs, const U& rhs) {
                return biginteger<_Base>::less(
                    view<_Base>(lhs),
                    view<_Base>(rhs)
                );
            }
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR bool operator<=(const T& lhs, const U& rhs) {
                return biginteger<_Base>::less_eq(
                    view<_Base>(lhs),
                    view<_Base>(rhs)
                );
            }
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR bool operator>(const T& lhs, const U& rhs) {
                return rhs < lhs;
            }
            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR bool operator>=(const T& lhs, const U& rhs) {
                return rhs <= lhs;
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR biginteger<_Base> operator+(T&& lhs, U&& rhs) {
                if constexpr (biginteger<_Base>::template _Is_rbint_v<T&&>) {
                    lhs += std::forward<U>(rhs);
                    return std::forward<T>(lhs);
                }
                else if constexpr (biginteger<_Base>::template _Is_rbint_v<U&&>) {
                    rhs += std::forward<T>(lhs);
                    return std::forward<U>(rhs);
                }
                else if constexpr (biginteger<_Base>::template _Is_rubint_v<T&&>) {
                    biginteger<_Base> result(std::forward<T>(lhs));
                    result += std::forward<U>(rhs);
                    return result;
                }
                else if constexpr (biginteger<_Base>::template _Is_rubint_v<U&&>) {
                    biginteger<_Base> result(std::forward<U>(rhs));
                    result += std::forward<T>(lhs);
                    return result;
                }
                else {
                    biginteger<_Base> result;
                    add(result, std::forward<T>(lhs), std::forward<U>(rhs));
                    return result;
                }
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR biginteger<_Base> operator-(T&& lhs, U&& rhs) {
                if constexpr (biginteger<_Base>::template _Is_rbint_v<T&&>) {
                    lhs -= std::forward<U>(rhs);
                    return std::forward<T>(lhs);
                }
                else if constexpr (biginteger<_Base>::template _Is_rbint_v<U&&>) {
                    rhs -= std::forward<T>(lhs);
                    rhs.negate();
                    return std::forward<U>(rhs);
                }
                else if constexpr (biginteger<_Base>::template _Is_rubint_v<T&&>) {
                    biginteger<_Base> result(std::forward<T>(lhs));
                    result -= std::forward<U>(rhs);
                    return result;
                }
                else if constexpr (biginteger<_Base>::template _Is_rubint_v<U&&>) {
                    biginteger<_Base> result(std::forward<U>(rhs));
                    result -= std::forward<T>(lhs);
                    result.negate();
                    return result;
                }
                else {
                    biginteger<_Base> result;
                    sub(result, std::forward<T>(lhs), std::forward<U>(rhs));
                    return result;
                }
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR biginteger<_Base> operator*(T&& lhs, U&& rhs) {
                if constexpr (biginteger<_Base>::template _Is_rbint_v<T&&>) {
                    lhs *= std::forward<U>(rhs);
                    return std::forward<T>(lhs);
                }
                else if constexpr (biginteger<_Base>::template _Is_rbint_v<U&&>) {
                    rhs *= std::forward<T>(lhs);
                    return std::forward<U>(rhs);
                }
                else if constexpr (biginteger<_Base>::template _Is_rubint_v<T&&>) {
                    biginteger<_Base> result(std::forward<T>(lhs));
                    result *= std::forward<U>(rhs);
                    return result;
                }
                else if constexpr (biginteger<_Base>::template _Is_rubint_v<U&&>) {
                    biginteger<_Base> result(std::forward<U>(rhs));
                    result *= std::forward<T>(lhs);
                    return result;
                }
                else {
                    biginteger<_Base> result;
                    mul(result, std::forward<T>(lhs), std::forward<U>(rhs));
                    return result;
                }
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR biginteger<_Base> operator/(T&& lhs, U&& rhs) {
                biginteger<_Base> result(std::forward<T>(lhs));
                result /= std::forward<U>(rhs);
                return result;
            }

            VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
                WJR_CONSTEXPR biginteger<_Base> operator%(T&& lhs, U&& rhs) {
                biginteger<_Base> result(std::forward<T>(lhs));
                result %= std::forward<U>(rhs);
                return result;
            }

            VIRTUAL_BIGINTEGER_SINGLE_AUTO_DEFINITION
                WJR_CONSTEXPR bool operator!(T&& x) {
                if constexpr (std::is_integral_v<T>) {
                    return !x;
                }
                else {
                    return std::forward<T>(x).test_byte(0);
                }
            }

            template<size_t _Base>
            WJR_CONSTEXPR void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs) {
                WASSERT_LEVEL_1(std::addressof(lhs) != std::addressof(rhs));
                biginteger<_Base>::quick_divmod(lhs, rhs);
            }

            template<size_t _Base>
            WJR_CONSTEXPR biginteger<_Base> pow(const biginteger<_Base>& a, size_t n) {
                return biginteger<_Base>(a._Signal || (!(n & 1)), unsigned_biginteger<_Base>::pow(a.ubint, n));
            }

            template<size_t _Base>
            WJR_CONSTEXPR biginteger<_Base> pow(biginteger<_Base>&& a, size_t n) {
                a._Signal = a._Signal || (!(n & 1));
                a.ubint.pow(n);
                return std::move(a);
            }

            template<size_t toBase, typename T, size_t fromBase,
                std::enable_if_t<_Is_biginteger_base_masks_v<fromBase, T>, int> >
            WJR_CONSTEXPR biginteger<toBase> base_conversion(T&& v) {
#if WDEBUG_LEVEL >= 3
                biginteger<fromBase> origin(v);
#endif
                auto result = biginteger<fromBase>::
                    template base_conversion_helper<toBase>(std::forward<T>(v));
#if WDEBUG_LEVEL >= 3
                auto iresult = biginteger<toBase>::
                    template base_conversion_helper<fromBase>(result);
                WASSERT_LEVEL_3(iresult == origin);
#endif
                return result;
            }

            template<size_t _Base>
            WJR_CONSTEXPR void random_biginteger(biginteger<_Base>& result, size_t n, bool _unsigned, bool exact) {
                using traits = biginteger_traits<_Base>;
                std::random_device rd;
#if defined(WJR_BIGINTEGER_USE_X64)
                USE_THREAD_LOCAL static std::mt19937_64 mt_rand(rd());
#else
                USE_THREAD_LOCAL static std::mt19937 mt_rand(rd());
#endif
                if (!n)return;
                if (!_unsigned) {
                    result._Signal = mt_rand() & 1;
                }
                size_t k = (n - 1) / traits::bits + 1;
                size_t r = (n - 1) % traits::bits + 1;
                std::uniform_int_distribution<limb_t> r1(0, traits::max());
                auto& ubint = result.ubint;
                ubint.reserve(k);
                ubint.set_size(k);
                for (size_t i = 0; i < k - 1; ++i) {
                    ubint[i] = r1(mt_rand);
                }
                auto _max = traits::max();
                if (r != traits::bits)_max = traits::get_base_power(r);
                std::uniform_int_distribution<limb_t> r2(exact, _max);
                ubint[k - 1] = r2(mt_rand);
                result.maintain();
            }

            template<size_t _Base>
            WJR_CONSTEXPR biginteger<_Base> random_biginteger(size_t n, bool _unsigned, bool exact) {
                biginteger<_Base> result;
                random_biginteger(result, n, _unsigned, exact);
                return result;
            }

            template<size_t _Base, typename T, std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> >
            WJR_CONSTEXPR void random_biginteger_max(biginteger<_Base>& result, const T& _Max) {
                using traits = biginteger_traits<_Base>;

                USE_THREAD_LOCAL static std::random_device rd;
#if defined(WJR_BIGINTEGER_USE_X64)
                USE_THREAD_LOCAL static std::mt19937_64 mt_rand(rd());
#else
                USE_THREAD_LOCAL static std::mt19937 mt_rand(rd());
#endif
                auto _view = view<_Base>(_Max);
                WASSERT_LEVEL_1(_view.signal() == true);
                size_t n = _view.size();
                auto p = _view.data();
                auto _Head_val = p[n - 1];
                std::uniform_int_distribution<limb_t> head(0, p[n - 1]);
                std::uniform_int_distribution<limb_t> tail(0, traits::max());
                result.get_vec().resize(n);
                auto rep = result.data();
                while (true) {
                    limb_t val = head(mt_rand);
                    size_t pos = n - 1;
                    rep[n - 1] = val;
                    while (val == rep[pos] && pos) {
                        --pos;
                        rep[pos] = (val = tail(mt_rand));
                    }
                    if (val > p[pos])continue;
                    while (pos) {
                        --pos;
                        rep[pos] = tail(mt_rand);
                    }
                    break;
                }
                result.maintain();
            }

            template<size_t _Base, typename T, typename U,
                std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T, U>, int> >
            WJR_CONSTEXPR void random_biginteger_min_max(biginteger<_Base>& result, const T& _Min, const U& _Max) {
                random_biginteger_max(result, _Max - _Min);
                result += _Min;
            }

            template<size_t _Base, typename T,
                std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> >
            WJR_CONSTEXPR biginteger<_Base> random_biginteger_max(const T& _Max) {
                biginteger<_Base> result;
                random_biginteger_max(result, _Max);
                return result;
            }

            template<size_t _Base, typename T, typename U,
                std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T, U>, int> >
            WJR_CONSTEXPR biginteger<_Base> random_biginteger_min_max(const T& _Min, const U& _Max) {
                biginteger<_Base> result;
                random_biginteger_min_max(result, _Min, _Max);
                return result;
            }

        }
    }

}

#endif
