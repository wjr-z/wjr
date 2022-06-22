#ifndef __WJR_BIGINTEGER_H
#define __WJR_BIGINTEGER_H

#include <array>

#include "../generic/mallocator.h"
#include "../generic/mString.h"
//#include "mtool.h"
#include "../generic/masm.h"

extern "C" {
    extern void cdft(int, int, double*, int*, double*);
}

namespace wjr {

#define BIGINTEGER_TEST

#if defined(__SIZEOF_INT128__) && !(defined(__clang__) && defined(_MSC_VER))
#define WJR_BIGINTEGER_USE_X64
#endif

#if defined(WJR_BIGINTEGER_USE_X64)
    using biginteger_basic_value_type = uint64_t;
    using biginteger_basic_twice_value_type = __uint128_t;
    constexpr static size_t biginteger_basic_bit = 64;
#else
    using biginteger_basic_value_type = uint32_t;
    using biginteger_basic_twice_value_type = uint64_t;
    constexpr static size_t biginteger_basic_bit = 32;
#endif

    // std::numeric_limits<__uint128_t> may not supported
    template<typename T>
    struct __biginteger_integral_max_value {
        constexpr static T value = std::numeric_limits<T>::max();
    };

    template<>
    struct __biginteger_integral_max_value<biginteger_basic_twice_value_type> {
        constexpr static biginteger_basic_twice_value_type value =
            biginteger_basic_twice_value_type(-1);
    };

    template<typename T>
    constexpr T biginteger_integral_max_value = __biginteger_integral_max_value<T>::value;

#if defined(WJR_USE_LIBDIVIDE)
    template<typename T>
    struct biginteger_libdivider {};

    template<>
    struct biginteger_libdivider<uint32_t> {
        explicit biginteger_libdivider(uint32_t d) {
            WASSERT_LEVEL_2(d != 0);
            auto floor_log_2_d = cqlog2(d);

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
                uint64_t proposed_m;
                uint32_t rem;
                // (1 << (64 + floor_log_2_d)) / d
                uint32_t _R;
                proposed_m = libdivide::libdivide_64_div_32_to_32((uint32_t)1 << floor_log_2_d, 0, d, &_R);
                proposed_m <<= 32;
                proposed_m |= libdivide::libdivide_64_div_32_to_32(_R, 0, d, &rem);
                WASSERT_LEVEL_2(rem > 0 && rem < d);
                const uint32_t e = d - rem;
                // This power works if e < 2**floor_log_2_d.
                if (e < ((uint32_t)1 << floor_log_2_d)) {
                    // This power works
                    more = (uint8_t)floor_log_2_d;
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
            uint32_t floor_log_2_d = cqlog2(d);

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
                biginteger_basic_twice_value_type proposed_m;
                uint64_t rem;
                // (1 << (128 + floor_log_2_d)) / d
                uint64_t _R;
                proposed_m = libdivide::libdivide_128_div_64_to_64((uint64_t)1 << floor_log_2_d, 0, d, &_R);
                proposed_m <<= 64;
                proposed_m |= libdivide::libdivide_128_div_64_to_64(_R, 0, d, &rem);
                WASSERT_LEVEL_2(rem > 0 && rem < d);
                const uint64_t e = d - rem;
                // This power works if e < 2**floor_log_2_d.
                if (e < ((uint64_t)1 << floor_log_2_d)) {
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
                    const biginteger_basic_twice_value_type twice_rem = rem + rem;
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
        biginteger_basic_twice_value_type magic;
        uint8_t more;
    };

    static biginteger_basic_twice_value_type
        biginteger_libdivide_mullhi_u128(
            biginteger_basic_twice_value_type x,
            biginteger_basic_twice_value_type y) {
        constexpr static uint64_t mask = (uint64_t)(-1);
        uint64_t x0 = (uint64_t)(x & mask);
        uint64_t x1 = (uint64_t)(x >> 64);
        uint64_t y0 = (uint64_t)(y & mask);
        uint64_t y1 = (uint64_t)(y >> 64);
        uint64_t x0y0_hi = libdivide::libdivide_mullhi_u64(x0, y0);
        biginteger_basic_twice_value_type x0y1 = x0 * (biginteger_basic_twice_value_type)y1;
        biginteger_basic_twice_value_type x1y0 = x1 * (biginteger_basic_twice_value_type)y0;
        biginteger_basic_twice_value_type x1y1 = x1 * (biginteger_basic_twice_value_type)y1;
        biginteger_basic_twice_value_type temp = x1y0 + x0y0_hi;
        biginteger_basic_twice_value_type temp_lo = temp & mask;
        biginteger_basic_twice_value_type temp_hi = temp >> 64;
        return x1y1 + temp_hi + ((temp_lo + x0y1) >> 64);
    }

    static biginteger_basic_twice_value_type operator/(
        biginteger_basic_twice_value_type n,
        const biginteger_libdivider<uint64_t>& d) {
        uint8_t more = d.more;
        if (!d.magic) {
            return n >> more;
        }
        else {
            biginteger_basic_twice_value_type q =
                biginteger_libdivide_mullhi_u128(d.magic, n);
            if (more & libdivide::LIBDIVIDE_ADD_MARKER) {
                biginteger_basic_twice_value_type t = ((n - q) >> 1) + q;
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
        using value_type = biginteger_basic_value_type;
        using twice_value_type = biginteger_basic_twice_value_type;
        constexpr static bool is_power_of_two = (base & (base - 1)) == 0;
        constexpr static size_t cqlog2_base = cqlog2(base);
        constexpr static size_t __get_bit_length() {
            if constexpr (is_power_of_two && (biginteger_basic_bit % cqlog2_base == 0)) {
                return biginteger_basic_bit / cqlog2_base;
            }
            else {
                return cqlog<base>(biginteger_integral_max_value<value_type>);
            }
        }

        constexpr static size_t bit_length = __get_bit_length();
        constexpr static value_type _max = quick_pow((value_type)base, (value_type)bit_length) - 1;
        constexpr static value_type _mod = _max + 1;
        constexpr static twice_value_type max_value = static_cast<twice_value_type>(_max) + 1;

        template<typename T>
        constexpr static size_t __get_bit_of_t() {
            auto v = biginteger_integral_max_value<T>;
            size_t result = 1;
            while (v > _max) {
                v /= max_value;
                ++result;
            }
            return result;
        }

        template<typename T>
        constexpr static size_t bit_of_t = __get_bit_of_t<T>();

        constexpr static bool is_full() {
            return _max == biginteger_integral_max_value<value_type>;
        }

        template<typename T>
        constexpr static value_type get_low(T x) {
            if constexpr (biginteger_integral_max_value<T> <= _max || is_full()) {
                return static_cast<value_type>(x);
            }
            else {
                return x % _mod;
            }
        }

        template<typename T>
        constexpr static T get_high(T x) {
            if constexpr (biginteger_integral_max_value<T> <= _max) {
                return 0;
            }
            else {
                if constexpr (is_full()) {
#if defined(WJR_BIGINTEGER_USE_X64)
                    x >>= 63;
                    return x >> 1;
#else
                    x >>= 31;
                    return x >> 1;
#endif
                }
                else {
                    return static_cast<T>(x / _mod);
                }
            }
        }

        constexpr static twice_value_type quick_mul(value_type a, value_type b) {
            return static_cast<twice_value_type>(a) * b;
        }

        static unsigned char quick_add(value_type lhs, value_type rhs, value_type* result) {
            *result = lhs + rhs;
            unsigned char cf = *result < lhs;
            if constexpr (is_full()) {
                return cf;
            }
            else {
                cf |= _mod <= *result;
                return cf ? (*result -= _mod, 1) : 0;
            }
        }

        static unsigned char quick_add(unsigned char cf, value_type lhs, value_type rhs, value_type* result) {
#if defined(WJR_BIGINTEGER_USE_X64)
            cf = wjr_addcarry_u64(cf, lhs, rhs, result);
#else
            cf = wjr_addcarry_u32(cf, lhs, rhs, result);
#endif
            if constexpr (is_full()) {
                return cf;
            }
            else {
                cf |= _mod <= *result;
                return cf ? (*result -= _mod, 1) : 0;
            }
        }

        static unsigned char quick_sub(
            value_type lhs, value_type rhs, value_type* result) {
            *result = lhs - rhs;
            unsigned char cf = *result > lhs;
            if constexpr (is_full()) {
                return cf;
            }
            else {
                return cf ? (*result += _mod, 1) : 0;
            }
        }

        static unsigned char quick_sub(
            unsigned char cf, value_type lhs, value_type rhs, value_type* result) {
#if defined(WJR_BIGINTEGER_USE_X64)
            cf = wjr_subborrow_u64(cf, lhs, rhs, result);
#else
            cf = wjr_subborrow_u32(cf, lhs, rhs, result);
#endif
            if constexpr (is_full()) {
                return cf;
            }
            else {
                return cf ? (*result += _mod, 1) : 0;
            }
        }

        struct constexpr_base_power_array {
            constexpr constexpr_base_power_array() : arr() {
                arr[0] = 1;
                for (size_t i = 1; i < bit_length; ++i) {
                    arr[i] = arr[i - 1] * base;
                }
            }
            std::array<value_type, bit_length> arr;
        };

        constexpr static constexpr_base_power_array
            static_base_power_array = constexpr_base_power_array();
        constexpr static value_type get_base_power(size_t kth) noexcept {
            WASSERT_LEVEL_2(kth < bit_length);
            return static_base_power_array.arr[kth];
        }
        constexpr static value_type mul_base_power(value_type val, size_t kth) noexcept {
            WASSERT_LEVEL_2(kth < bit_length);
            if constexpr (is_power_of_two) {
                return val << (cqlog2_base * kth);
            }
            else {
                return val * get_base_power(kth);
            }
        }
        struct magic_divider_of_base_power {
            using divider_type = libdivide::divider<value_type>;
            magic_divider_of_base_power() {
                for (size_t i = 0; i < bit_length; ++i) {
                    new (&magic_divider[i]) divider_type(get_base_power(i));
                }
            }
            std::array<divider_type, bit_length> magic_divider;
        };
        static magic_divider_of_base_power static_magic_divider;
        constexpr static value_type div_base_power(value_type val, size_t kth) noexcept {
            WASSERT_LEVEL_2(kth < bit_length);
            if constexpr (is_power_of_two) {
                return val >> (cqlog2_base * kth);
            }
            else {
                return val / static_magic_divider.magic_divider[kth];
            }
        }
        constexpr static value_type mod_base_power(value_type val, size_t kth) noexcept {
            WASSERT_LEVEL_2(kth < bit_length);
            if constexpr (is_power_of_two) {
                return val & (((value_type)(1) << (cqlog2_base * kth)) - 1);
            }
            else {
                return val - div_base_power(val, kth) * get_base_power(kth);
            }
        }

        struct fromString {
            constexpr value_type operator()(char ch) const {
                if constexpr (base <= 10) {
                    return ch - '0';
                }
                else {
                    WASSERT_LEVEL_2(base <= 36);
                    return get_digit(ch);
                }
            }
        };

        struct toString {
            constexpr char operator()(value_type val)const {
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

        struct mul_threshold {
            size_t slow_threshold;
            size_t karatsuba_threshold;
            size_t toom_cook_3_threshold;
        };

        struct threshold {
        private:
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
        public:
            inline static mul_threshold mul_info = get_mul_threshold();
        };

    };

    template<size_t base>
    class unsigned_biginteger;

    template<size_t base>
    class biginteger;

    template<size_t base>
    typename biginteger_traits<base>::magic_divider_of_base_power
        biginteger_traits<base>::static_magic_divider = magic_divider_of_base_power();

    constexpr static biginteger_basic_value_type
        __biginteger_zero_area[1] = { 0 };

    template<size_t base, size_t length>
    class virtual_unsigned_biginteger;

    template<size_t base>
    class virtual_unsigned_biginteger_reference;

    template<size_t base>
    class virtual_unsigned_biginteger_const_reference {
        using traits = biginteger_traits<base>;
        template<size_t _Base>
        friend class virtual_unsigned_biginteger_reference;
    public:
        using value_type = typename traits::value_type;
        constexpr virtual_unsigned_biginteger_const_reference(
            const value_type* _Data, size_t _Size, size_t _K, size_t _Index
        ) noexcept : _Data(const_cast<value_type*>(_Data)),
            _Size(_Size), _K(_K), _Index(_Index) {
            WASSERT_LEVEL_2(pos1() < _Size);
        }
        constexpr size_t pos1()const noexcept { return _Index / traits::bit_length; }
        constexpr size_t pos2()const noexcept { return _Index % traits::bit_length; }
        constexpr virtual_unsigned_biginteger_const_reference(
            const virtual_unsigned_biginteger_const_reference&) noexcept = default;
        constexpr virtual_unsigned_biginteger_const_reference& operator=(
            const virtual_unsigned_biginteger_const_reference&) noexcept = default;
        constexpr operator value_type() const noexcept {
            auto p = _Data;
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            value_type low = traits::div_base_power(p[_Pos1], _Pos2);
            size_t low_l = traits::bit_length - _Pos2;
            if (_K <= low_l || _Pos1 == _Size - 1) {
                return get_mod(low);
            }
            size_t high_l = _K - low_l;
            value_type high = traits::mod_base_power(p[_Pos1 + 1], high_l);
            return traits::mul_base_power(high, low_l) + low;
        }
    private:
        constexpr value_type get_mod(value_type val) const noexcept {
            if constexpr (traits::is_full()) {
                return _K != traits::bit_length ?
                    traits::mod_base_power(val, _K) : val;
            }
            else {
                return traits::mod_base_power(val, _K);
            }
        }
        value_type* _Data;
        size_t _Size;
        const size_t _K;
        size_t _Index;
    };

    template<size_t base>
    class virtual_unsigned_biginteger_reference
        : public virtual_unsigned_biginteger_const_reference<base> {
        using traits = biginteger_traits<base>;
        using _Base = virtual_unsigned_biginteger_const_reference<base>;
    public:
        using value_type = typename traits::value_type;
        constexpr virtual_unsigned_biginteger_reference(
            value_type* _Data, size_t _Size, size_t _K, size_t _Index)
            noexcept : _Base(_Data, _Size, _K, _Index) {}
        constexpr size_t pos1()const noexcept { return _Base::_Index / traits::bit_length; }
        constexpr size_t pos2()const noexcept { return _Base::_Index % traits::bit_length; }
        constexpr virtual_unsigned_biginteger_reference(
            const virtual_unsigned_biginteger_reference&) noexcept = default;
        constexpr virtual_unsigned_biginteger_reference& operator=(
            const virtual_unsigned_biginteger_reference&) noexcept = default;
        constexpr virtual_unsigned_biginteger_reference& operator=(value_type val) noexcept {
            WASSERT_LEVEL_2(val >= 0 && val <= traits::_max);
            auto p = _Base::_Data;
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            value_type low = traits::div_base_power(p[_Pos1], _Pos2);
            size_t low_l = traits::bit_length - _Pos2;
            if (_Base::_K <= low_l || _Pos1 == _Base::_Size - 1) {
                p[_Pos1] += traits::mul_base_power((val - _Base::get_mod(low)), _Pos2);
            }
            else {
                size_t high_l = _Base::_K - low_l;
                value_type high = traits::mod_base_power(p[_Pos1 + 1], high_l);
                value_type val_high = traits::div_base_power(val, low_l);
                value_type val_low = val - traits::mul_base_power(val_high, low_l);
                p[_Pos1] += traits::mul_base_power((val_low - low), _Pos2);
                p[_Pos1 + 1] += (val_high - high);
            }
            return *this;
        }
        constexpr virtual_unsigned_biginteger_reference& zero_change(value_type val) noexcept {
            WASSERT_LEVEL_1(val >= 0 && val <= traits::_max);
            WASSERT_LEVEL_2((value_type)(*this) == 0);
            auto p = _Base::_Data;
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            size_t low_l = traits::bit_length - _Pos2;
            if (_Base::_K <= low_l || _Pos1 == _Base::_Size - 1) {
                p[_Pos1] += traits::mul_base_power(val, _Pos2);
            }
            else {
                value_type val_high = traits::div_base_power(val, low_l);
                value_type val_low = val - traits::mul_base_power(val_high, low_l);
                p[_Pos1] += traits::mul_base_power(val_low, _Pos2);
                p[_Pos1 + 1] += val_high;
            }
            return *this;
        }
    };

    template<size_t base>
    class virtual_unsigned_biginteger_const_iterator {
        using traits = biginteger_traits<base>;
    public:
        using value_type = typename traits::value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = virtual_unsigned_biginteger_const_reference<base>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;
        constexpr virtual_unsigned_biginteger_const_iterator(
            const value_type* _Data, size_t _Size, size_t _K, size_t _Index = 0)
            noexcept : _Data(const_cast<value_type*>(_Data)),
            _Size(_Size), _K(_K), _Index(_Index) {
            WASSERT_LEVEL_1(vaild());
        }
        constexpr virtual_unsigned_biginteger_const_iterator(
            const virtual_unsigned_biginteger_const_iterator&) noexcept = default;
        constexpr virtual_unsigned_biginteger_const_iterator& operator=(
            const virtual_unsigned_biginteger_const_iterator&) noexcept = default;
        constexpr size_t pos1()const noexcept {
            return _Index / traits::bit_length;
        }
        constexpr size_t pos2()const noexcept {
            return _Index % traits::bit_length;
        }
        constexpr bool vaild()const noexcept {
            return pos1() < _Size;
        }
        constexpr bool is_last()const noexcept {
            return pos1() + 1 == _Size;
        }
        constexpr virtual_unsigned_biginteger_const_iterator& operator++() {
            _Index += _K;
            return *this;
        }
        constexpr virtual_unsigned_biginteger_const_iterator operator++(int) {
            virtual_unsigned_biginteger_const_iterator it(*this);
            ++(*this);
            return it;
        }
        constexpr virtual_unsigned_biginteger_const_iterator& operator+=(size_t delta) {
            _Index += delta * _K;
            return *this;
        }
        constexpr virtual_unsigned_biginteger_const_iterator operator+(size_t delta) const {
            virtual_unsigned_biginteger_const_iterator it(*this);
            it += delta;
            return it;
        }
        constexpr virtual_unsigned_biginteger_const_iterator& operator--() {
            _Index -= _K;
            return *this;
        }
        constexpr virtual_unsigned_biginteger_const_iterator operator--(int) {
            virtual_unsigned_biginteger_const_iterator it(*this);
            --(*this);
            return it;
        }
        constexpr virtual_unsigned_biginteger_const_iterator& operator-=(size_t delta) {
            _Index -= _K * delta;
            return *this;
        }
        constexpr virtual_unsigned_biginteger_const_iterator operator-(size_t delta)const {
            virtual_unsigned_biginteger_const_iterator it(*this);
            it -= delta;
            return *this;
        }
        constexpr reference operator*()const noexcept {
            return reference(_Data, _Size, _K, _Index);
        }
        constexpr const_pointer operator->()const noexcept {
            return _Data;
        }
        friend difference_type operator-(
            const virtual_unsigned_biginteger_const_iterator& lhs,
            const virtual_unsigned_biginteger_const_iterator& rhs
            ) {
            WASSERT_LEVEL_1(lhs._K == rhs._K);
            return (rhs._Index - lhs._Index) / lhs._K;
        }
        constexpr friend bool operator==(
            const virtual_unsigned_biginteger_const_iterator& lhs,
            const virtual_unsigned_biginteger_const_iterator& rhs
            ) {
            return lhs._Data == rhs._Data && lhs._Size == rhs._Size
                && lhs._K == rhs._K && lhs._Index == rhs._Index;
        }
        constexpr friend bool operator!=(
            const virtual_unsigned_biginteger_const_iterator& lhs,
            const virtual_unsigned_biginteger_const_iterator& rhs
            ) {
            return !(lhs == rhs);
        }
    protected:
        value_type* _Data;
        size_t _Size;
        const size_t _K;
        size_t _Index = 0;
    };

    template<size_t base>
    class virtual_unsigned_biginteger_iterator
        : public virtual_unsigned_biginteger_const_iterator<base> {
        using _Base = virtual_unsigned_biginteger_const_iterator<base>;
        using traits = biginteger_traits<base>;
    public:
        using value_type = typename traits::value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = virtual_unsigned_biginteger_reference<base>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;
        constexpr virtual_unsigned_biginteger_iterator(
            value_type* _Data, size_t _Size, size_t _K, size_t _Index = 0)
            noexcept : _Base(_Data, _Size, _K, _Index) {}
        constexpr virtual_unsigned_biginteger_iterator(
            const virtual_unsigned_biginteger_iterator&) noexcept = default;
        constexpr virtual_unsigned_biginteger_iterator& operator=(
            const virtual_unsigned_biginteger_iterator&) noexcept = default;
        constexpr virtual_unsigned_biginteger_iterator& operator++() {
            _Base::operator++();
            return *this;
        }
        constexpr virtual_unsigned_biginteger_iterator operator++(int) {
            _Base::operator++(0);
            return *this;
        }
        constexpr virtual_unsigned_biginteger_iterator& operator+=(size_t index) {
            _Base::operator+=(index);
            return *this;
        }
        constexpr virtual_unsigned_biginteger_iterator operator+(size_t index) const {
            virtual_unsigned_biginteger_iterator it(*this);
            it += index;
            return it;
        }
        constexpr virtual_unsigned_biginteger_iterator& operator--() {
            _Base::operator--();
            return *this;
        }
        constexpr virtual_unsigned_biginteger_iterator operator--(int) {
            _Base::operator--(0);
            return *this;
        }
        constexpr virtual_unsigned_biginteger_iterator& operator-=(size_t index) const {
            _Base::operator-=(index);
            return *this;
        }
        constexpr virtual_unsigned_biginteger_iterator operator-(size_t index) {
            virtual_unsigned_biginteger_iterator it(*this);
            it -= index;
            return *this;
        }
        constexpr reference operator*() noexcept {
            return reference(_Base::_Data, _Base::_Size, _Base::_K, _Base::_Index);
        }
        constexpr pointer operator->() noexcept {
            return _Base::_Data;
        }
        constexpr friend difference_type operator-(
            const virtual_unsigned_biginteger_iterator& lhs,
            const virtual_unsigned_biginteger_iterator& rhs
            ) {
            WASSERT_LEVEL_1(lhs._K == rhs._K);
            return (lhs._Index - rhs._Index) / lhs._K;
        }
        constexpr friend bool operator==(
            const virtual_unsigned_biginteger_iterator& lhs,
            const virtual_unsigned_biginteger_iterator& rhs
            ) {
            return lhs._Data == rhs._Data && lhs._Size == rhs._Size
                && lhs._K == rhs._K && lhs._Index == rhs._Index;
        }
        constexpr friend bool operator!=(
            const virtual_unsigned_biginteger_iterator& lhs,
            const virtual_unsigned_biginteger_iterator& rhs
            ) {
            return !(lhs == rhs);
        }
    };

    // unsigned biginteger or integral view
    template<size_t base, size_t length>
    class virtual_unsigned_biginteger {
        static_assert(length <= 4, "");
        using traits = biginteger_traits<base>;
        template<size_t index, typename T>
        constexpr void _Make(T val) noexcept {
            if constexpr (index == traits::template bit_of_t<T> -1) {
                _Data[index] = val;
                _Size = index + 1;
                WASSERT_LEVEL_2(traits::get_high(val) == 0);
            }
            else {
                _Data[index] = traits::get_low(val);
                val = traits::get_high(val);
                if (val) {
                    _Make<index + 1>(val);
                }
                else _Size = index + 1;
            }
        }
    public:
        using value_type = typename traits::value_type;
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base>;
        using iterator = virtual_unsigned_biginteger_iterator<base>;
        template<typename T, std::enable_if_t<_Is_unsigned_integral_v<T>, int> = 0>
        constexpr explicit virtual_unsigned_biginteger(T val) noexcept {
            _Make<0>(val);
        }
        constexpr virtual_unsigned_biginteger(const virtual_unsigned_biginteger&) noexcept = default;
        constexpr virtual_unsigned_biginteger& operator=(
            const virtual_unsigned_biginteger&) noexcept = default;
        constexpr const value_type* data()const noexcept { return _Data; }
        constexpr size_t size() const noexcept {
            if constexpr (length == 1) {
                return 1;
            }
            else {
                return _Size;
            }
        }
        constexpr const value_type* begin()const noexcept { return _Data; }
        constexpr const value_type* end()const noexcept { return _Data + _Size; }
        constexpr bool zero()const noexcept {
            if constexpr (length == 1) {
                return _Data[0] == 0;
            }
            else {
                return _Size == 1 && _Data[0] == 0;
            }
        }

        constexpr bool one()const noexcept {
            if constexpr (length == 1) {
                return _Data[0] == 1;
            }
            else {
                return _Size == 1 && _Data[0] == 1;
            }
        }

        constexpr const_iterator begin(size_t k)const noexcept {
            return const_iterator(_Data, _Size, k);
        }
        constexpr const_iterator cbegin(size_t k)const noexcept {
            return const_iterator(_Data, _Size, k);
        }
        template<typename T>
        constexpr T to()const noexcept {
            T _Val = 0;
            if constexpr (length == 1) {
                _Val = _Data[0];
            }
            else {
                for (size_t i = _Size; i--;) {
                    WASSERT_LEVEL_2(_Data[i] <= biginteger_integral_max_value<T>
                        && (_Val <= (biginteger_integral_max_value<T> -_Data[i]) / traits::max_value));
                    _Val = _Val * traits::max_value + _Data[i];
                }
            }
            return _Val;
        }
        template<typename T>
        constexpr explicit operator T()const noexcept {
            return to<T>();
        }
        constexpr virtual_unsigned_biginteger& maintain() noexcept {
            return *this;
        }
    private:
        size_t _Size;
        value_type _Data[length];
    };

    template<size_t base>
    class virtual_unsigned_biginteger<base, 0> {
        using traits = biginteger_traits<base>;
    public:
        using value_type = typename traits::value_type;
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base>;
        using iterator = virtual_unsigned_biginteger_iterator<base>;
        constexpr virtual_unsigned_biginteger(
            const value_type* _Data, size_t _Size)
            noexcept : _Data(_Data), _Size(_Size) {}
        constexpr virtual_unsigned_biginteger(
            const virtual_unsigned_biginteger&) noexcept = default;
        constexpr explicit virtual_unsigned_biginteger(
            const unsigned_biginteger<base>& rhs)
            noexcept : _Data(rhs.data()), _Size(rhs.size()) {}
        constexpr virtual_unsigned_biginteger& operator=(
            const virtual_unsigned_biginteger&) noexcept = default;
        constexpr const value_type* data()const noexcept {
            WASSERT_LEVEL_1(_Size != 0);
            return _Data;
        }
        constexpr size_t size()const noexcept { return _Size; }
        constexpr const value_type* begin()const noexcept {
            return data();
        }
        constexpr const value_type* end()const noexcept { return begin() + _Size; }
        constexpr bool zero()const noexcept {
            WASSERT_LEVEL_1(_Size != 0);
            return _Size == 1 && _Data[0] == 0;
        }
        constexpr bool one() const noexcept {
            WASSERT_LEVEL_1(_Size != 0);
            return _Size == 1 && _Data[0] == 1;
        }
        constexpr const_iterator begin(size_t k)const noexcept {
            return const_iterator(data(), size(), k);
        }
        constexpr const_iterator cbegin(size_t k)const noexcept {
            return const_iterator(data(), size(), k);
        }
        template<typename T>
        constexpr T to()const noexcept {
            WASSERT_LEVEL_1(_Size != 0);
            T _Val = 0;
            for (size_t i = _Size; i--;) {
                WASSERT_LEVEL_2(_Data[i] <= biginteger_integral_max_value<T>
                    && (_Val <= (biginteger_integral_max_value<T> -_Data[i]) / traits::max_value));
                _Val = _Val * traits::max_value + _Data[i];
            }
            return _Val;
        }
        template<typename T>
        constexpr explicit operator T() const noexcept {
            return to<T>();
        }
        constexpr virtual_unsigned_biginteger& maintain() noexcept {
            if (!_Size) {
                _Data = __biginteger_zero_area;
                _Size = 1;
                return *this;
            }
            while (_Size != 1 && !_Data[_Size - 1])--_Size;
            return *this;
        }
    private:
        const value_type* _Data;
        size_t _Size;
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
    struct unsigned_biginteger_base_helper<virtual_unsigned_biginteger<base, length>> {
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
            if constexpr (lvalue == -1 || rvalue == -1) {
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
    struct _Is_original_virtual_unsigned_biginteger : std::false_type {};

    template<size_t base, size_t length>
    struct _Is_original_virtual_unsigned_biginteger<
        virtual_unsigned_biginteger<base, length>> : std::true_type {};

    template<typename T>
    constexpr static bool _Is_original_virtual_unsigned_biginteger_v =
        _Is_original_virtual_unsigned_biginteger<T>::value;

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

    template<size_t base, typename T,
        std::enable_if_t<_Is_base_mask_v<base, unsigned_biginteger_base_v<T>>, int> = 0>
        constexpr static decltype(auto) get_virtual_unsigned_biginteger(const T& t) {
        using traits = biginteger_traits<base>;
        if constexpr (_Is_unsigned_integral_v<T>) {
            return virtual_unsigned_biginteger<base, traits::template bit_of_t<T>>(t);
        }
        else if constexpr (_Is_unsigned_biginteger_v<T>) {
            return virtual_unsigned_biginteger<base, 0>(t);
        }
        else {
            return t;
        }
    }

#define REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(index,rt,FUNC,...)		\
        if constexpr (index == 1) {					    	        \
            return FUNC<1>(__VA_ARGS__);					        \
        }													        \
        else if constexpr (index == 2) {				    	    \
            return rt.size() == 1 ? FUNC<1>(__VA_ARGS__) :          \
                FUNC<2>(__VA_ARGS__);                               \
        }													        \
        else if constexpr (index == 3) {					        \
            switch (rt.size()) {							        \
            case 1: return FUNC<1>(__VA_ARGS__);			        \
            case 2: return FUNC<2>(__VA_ARGS__);			        \
            case 3: return FUNC<3>(__VA_ARGS__);			        \
            }												        \
        }													        \
        else {												        \
            return rt.size() == 1 ? FUNC<1>(__VA_ARGS__) :          \
                FUNC<0>(__VA_ARGS__);                               \
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
                return lhs.size() < rhs.size() ? FUNC(result, rhs, lhs) : FUNC(result, lhs, rhs);   \
            }                                                                                       \
        }                                                                                           \


#define VIRTUAL_UNSIGNED_BIGINTEGER_SINGLE_DECLARATION                                          \
    template<typename T                                                                         \
    std::enable_if_t<_Is_unsigned_biginteger_base_masks_v<base, T>, int> = 0>                   \

#define VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION                                          \
    template<typename T, typename U,                                                            \
    std::enable_if_t<_Is_unsigned_biginteger_base_masks_v<base, T, U>, int> = 0>                \

    // _dst may be equal to _Src
    // we needn't to memcpy
    inline void* biginteger_ememcpy(
        void* _Dst,
        void const* _Src,
        size_t      _Size
    ) {
        return _Dst != _Src ? memcpy(_Dst, _Src, _Size) : nullptr;
    }

    template<size_t fromBase, size_t toBase>
    struct _Quick_base_conversion_1 {
        constexpr static bool value = (fromBase != toBase) &&
            (biginteger_traits<fromBase>::_max == biginteger_traits<toBase>::_max);
    };

    template<size_t fromBase, size_t toBase>
    constexpr static bool _Quick_base_conversion_1_v = _Quick_base_conversion_1<fromBase, toBase>::value;

    template<size_t fromBase, size_t toBase>
    struct _Quick_base_conversion_2 {
        constexpr static bool value = (fromBase != toBase) &&
            (biginteger_traits<fromBase>::_max != biginteger_traits<toBase>::_max) &&
            quick_is_power_of<fromBase>(toBase);
    };

    template<size_t fromBase, size_t toBase>
    constexpr static bool _Quick_base_conversion_2_v = _Quick_base_conversion_2<fromBase, toBase>::value;

    using __biginteger_vector =
        std::vector<biginteger_basic_value_type,
        mallocator<biginteger_basic_value_type>>;

    template<size_t base>
    class unsigned_biginteger {

        template<size_t _Base>
        friend class unsigned_biginteger;

        template<size_t _Base>
        friend class biginteger;

        using traits = biginteger_traits<base>;
        template<size_t length>
        using virtual_unsigned_biginteger = wjr::virtual_unsigned_biginteger<base, length>;
        template<typename T>
        using _Is_virtual_unsigned_biginteger =
            _Is_base_mask<base, unsigned_biginteger_base_v<T>>;
        template<typename T>
        constexpr static bool _Is_virtual_unsigned_biginteger_v =
            _Is_virtual_unsigned_biginteger<T>::value;
        template<typename T>
        using _Is_rubint = std::conjunction<
            std::is_rvalue_reference<T>,
            _Is_unsigned_biginteger<T>,
            _Is_virtual_unsigned_biginteger<T>>;
        template<typename T>
        constexpr static bool _Is_rubint_v = _Is_rubint<T>::value;
    public:
        using value_type = typename traits::value_type;
        using twice_value_type = typename traits::twice_value_type;
        using iterator = virtual_unsigned_biginteger_iterator<base>;
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using reverse_const_iterator = std::reverse_iterator<const_iterator>;

        template<typename iter, typename Func>
        void in(iter first, iter last, Func&& fn) {
            size_t n = std::distance(first, last);
            if (!n) {
                set_zero();
                return;
            }
            vec.resize((n + traits::bit_length - 1) / traits::bit_length);
            auto write = begin();
            while (first != last) {
                *write = fn(*(--last));
                ++write;
            }
        }

        template<typename iter, typename Func>
        iter out(iter first, [[maybe_unused]] iter last, Func&& fn) const {
            auto read = rbegin();
            auto read_end = rend();
            while (read != read_end) {
                WASSERT_LEVEL_2(first != last);
                *first = fn(*read);
                ++first;
                ++read;
            }
            return first;
        }

        unsigned_biginteger() : vec(1, 0) {}
        unsigned_biginteger(const unsigned_biginteger& other) = default;
        unsigned_biginteger(unsigned_biginteger&& other) noexcept = default;

        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_1_v<fromBase, base>, int> = 0>
        explicit unsigned_biginteger(const unsigned_biginteger<fromBase>& other)
            : vec(other.vec) {}
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_1_v<fromBase, base>, int> = 0>
        explicit unsigned_biginteger(unsigned_biginteger<fromBase>&& other)
            noexcept : vec(std::move(other.vec)) {}

        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_2_v<fromBase, base>, int> = 0>
        explicit unsigned_biginteger(const unsigned_biginteger<fromBase>& other) {
            constexpr size_t k = cqlog<fromBase>(base);
            size_t l = other.bit_length();
            size_t q = (l + k - 1) / k;
            vec.resize((q + traits::bit_length - 1) / traits::bit_length);
            auto read = other.cbegin(k);
            auto read_end = other.cend(k);
            auto write = begin(1);
            auto c(write);
            while (read != read_end) {
                *write = *read;
                ++read;
                ++write;
            }
        }

        template<size_t length>
        explicit unsigned_biginteger(const virtual_unsigned_biginteger<length>& rhs)
            noexcept : vec(rhs.begin(), rhs.end()) {
            if (unlikely(vec.size() == 0)) {
                set_zero();
            }
        }
        template<typename T, std::enable_if_t<_Is_virtual_unsigned_biginteger_v<T>, int> = 0>
        explicit unsigned_biginteger(const T& val)
            : unsigned_biginteger(get_virtual_unsigned_biginteger<base>(val)) {}
        template<typename iter, typename Func>
        unsigned_biginteger(iter first, iter last, Func&& fn) {
            WASSERT_LEVEL_2(first <= last);
            in(first, last, std::forward<Func>(fn));
            maintain();
        }
        unsigned_biginteger(const value_type* _Data, size_t _Size)
            : vec(_Data, _Data + _Size) {}
        template<typename Func = typename traits::fromString>
        explicit unsigned_biginteger(const String_view& str, Func&& fn = Func()) {
            in(str.begin(), str.end(), std::forward<Func>(fn));
        }
        unsigned_biginteger& operator=(const unsigned_biginteger&) = default;
        unsigned_biginteger& operator=(unsigned_biginteger&& other) noexcept = default;
        template<size_t length>
        unsigned_biginteger& operator=(const virtual_unsigned_biginteger<length>& rhs) {
            if constexpr (length == 0) {
                if (unlikely(data() == rhs.data())) {
                    WASSERT_LEVEL_1(rhs.size() <= size());
                    vec.resize(rhs.size());
                    return *this;
                }
            }
            else {
                WASSERT_LEVEL_1(data() != rhs.data());
            }
            vec.assign(rhs.begin(), rhs.end());
            return *this;
        }
        template<typename T, std::enable_if_t<_Is_virtual_unsigned_biginteger_v<T>, int> = 0>
        unsigned_biginteger& operator=(const T& val) {
            return ((*this) = get_virtual_unsigned_biginteger<base>(val));
        }

        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_1_v<fromBase, base>, int> = 0>
        unsigned_biginteger& operator=(const unsigned_biginteger<fromBase>& other) {
            vec = other.vec;
            return *this;
        }

        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_1_v<fromBase, base>, int> = 0>
        unsigned_biginteger& operator=(unsigned_biginteger<fromBase>&& other) noexcept {
            vec = std::move(other.vec);
            return *this;
        }

        template<typename Func = typename traits::fromString>
        void assign(const String_view& str, Func&& fn = Func()) {
            in(str.begin(), str.end(), std::forward<Func>(fn));
        }

        unsigned_biginteger& operator=(const String_view& str) {
            assign(str);
            return *this;
        }

        template<typename iter, typename Func>
        void assign(iter first, iter last, Func&& fn) {
            in(first, last, std::forward<Func>(fn));
        }

        unsigned_biginteger& assign(const value_type* _Data, size_t _Size) {
            vec.assign(_Data, _Data + _Size);
            return *this;
        }

        void swap(unsigned_biginteger& other) noexcept {
            vec.swap(other.vec);
        }

        value_type* data() noexcept { return vec.data(); }
        const value_type* data()const noexcept { return vec.data(); }
        size_t size()const noexcept { return vec.size(); }
        bool zero()const noexcept { return size() == 1 && *data() == 0; }
        bool one()const noexcept { return size() == 1 && *data() == 1; }
        void set_zero() { this->vec.clear(); this->vec.push_back(0); }

        size_t bit_length() const {
            size_t _Pos = vec.size() - 1;
            return _Pos * traits::bit_length + cqlog<base>(vec[_Pos]) + 1;
        }

        iterator begin(size_t k = 1) {
            return iterator(
                data(),
                size(),
                k
            );
        }
        const_iterator begin(size_t k = 1)const {
            return const_iterator(
                data(),
                size(),
                k
            );
        }
        const_iterator cbegin(size_t k = 1)const {
            return const_iterator(
                data(),
                size(),
                k
            );
        }

        iterator end(size_t k = 1) {
            return begin(k) + (bit_length() + k - 1) / k;
        }
        const_iterator end(size_t k = 1)const {
            return begin(k) + (bit_length() + k - 1) / k;
        }
        const_iterator cend(size_t k = 1) const {
            return begin(k) + (bit_length() + k - 1) / k;
        }
        reverse_iterator rbegin(size_t k = 1) {
            return reverse_iterator(end(k));
        }
        reverse_const_iterator rbegin(size_t k = 1)const {
            return reverse_const_iterator(end(k));
        }
        reverse_const_iterator rcbegin(size_t k = 1)const {
            return reverse_const_iterator(end(k));
        }
        reverse_iterator rend(size_t k = 1) {
            return reverse_iterator(begin(k));
        }
        reverse_const_iterator rend(size_t k = 1)const {
            return reverse_const_iterator(begin(k));
        }
        reverse_const_iterator rcend(size_t k = 1)const {
            return reverse_const_iterator(begin(k));
        }

        void mul_base_power(size_t index) {
            if (!index || zero())return;
            size_t n = size();
            size_t k = index / traits::bit_length;
            size_t r = index % traits::bit_length;
            if constexpr (traits::is_full()) {
                vec.reserve(n + k + 1);
                auto p = data();
#if defined(WJR_BIGINTEGER_USE_X64)
                auto v = wjr_shl_epi64(p, n, r * traits::cqlog2_base);
#else
                auto v = wjr_shl_epi32(p, n, r * traits::cqlog2_base);
#endif
                if (v)vec.push_back(v);
                vec.insert(vec.begin(), k, 0);
            }
            else {
                if (!r) {
                    vec.insert(vec.begin(), k, 0);
                }
                else {
                    size_t br = traits::bit_length - r;
                    bool is = vec[n - 1] >= traits::get_base_power(br);
                    vec.resize(n + k + is);
                    auto p = data();
                    value_type _High = 0;
                    value_type _Low;
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
                    memset(p, 0, sizeof(value_type) * k);
                }
            }
        }

        void div_base_power(size_t index) {
            if (!index || zero())return;
            size_t n = size();
            size_t k = index / traits::bit_length;
            size_t r = index % traits::bit_length;
            if (k >= n) {
                set_zero();
                return;
            }
            vec.erase(vec.begin(), vec.begin() + k);
            n = size();
            auto p = data();
            if constexpr (traits::is_full()) {
#if defined(WJR_BIGINTEGER_USE_X64)
                wjr_shr_epi64(p, n, r * traits::cqlog2_base);
#else
                wjr_shr_epi32(p, n, r * traits::cqlog2_base);
#endif
                if (!p[n - 1])vec.resize(n - 1);
            }
            else {
                if (r) {
                    value_type mod_tag = traits::get_base_power(r);
                    value_type mul_tag = traits::get_base_power(traits::bit_length - r);
                    for (size_t i = 0; i != n - 1; ++i) {
                        p[i] = traits::div_base_power(p[i], r) +
                            ((p[i + 1] - traits::div_base_power(p[i + 1], r) * mod_tag) * mul_tag);
                    }
                    p[n - 1] = traits::div_base_power(p[n - 1], r);
                    while (n && !p[n - 1])--n;
                    vec.resize(n);
                }
            }
        }

        void reserve_bit(size_t k) {
			vec.reserve((k + traits::bit_length - 1) / traits::bit_length);
        }

        void resize_bit(size_t k) {
            size_t _Oldsize = size();
            size_t _Newsize = (k + traits::bit_length - 1) / traits::bit_length;
            vec.resize(_Newsize);
            if (_Oldsize >= _Newsize) {
                size_t r = k % traits::bit_length;
                if (r) {
                    value_type* p = data() + (_Newsize - 1);
                    *p = traits::mod_base_power(*p, r);
                }
            }
        }

        void mod_base_power(size_t index) {
            if (!index) {
                set_zero();
                return;
            }
            size_t _Oldsize = size();
            size_t _Newsize = (index + traits::bit_length - 1) / traits::bit_length;
            if (_Oldsize >= _Newsize) {
                vec.resize(_Newsize);
                size_t r = index % traits::bit_length;
                if (r) {
                    value_type* p = data() + (_Newsize - 1);
                    *p = traits::mod_base_power(*p, r);
                }
                maintain();
            }
        }

        void maintain() {
            size_t n = size();
            auto p = data();
            while (n != 1 && !p[n - 1])--n;
            vec.resize(n);
        }

        void pow(size_t n) {
            if (!n) {
                vec.clear();
                vec.push_back(1);
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

        static unsigned_biginteger pow(const unsigned_biginteger& a, size_t n) {
            if (!n) {
                return unsigned_biginteger(virtual_unsigned_biginteger<1>(1u));
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

        static unsigned_biginteger pow(unsigned_biginteger&& a, size_t n) {
            a.pow(n);
            return std::move(a);
        }

    private:

        template<size_t index, size_t il, size_t ir>
        static int QCmp(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t il, size_t ir>
        static int Cmp(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        ) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QCmp, lhs, rhs);
        }

        template<size_t index, size_t il, size_t ir>
        static bool QEqual(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs);
        template<size_t il, size_t ir>
        static bool Equal(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QEqual, lhs, rhs);
        }

        template<size_t index, size_t il, size_t ir>
        static bool QLess(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs);
        template<size_t il, size_t ir>
        static bool Less(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QLess, lhs, rhs);
        }

        template<size_t index, size_t il, size_t ir>
        static bool QLess_eq(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs);
        template<size_t il, size_t ir>
        static bool Less_eq(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QLess_eq, lhs, rhs);
        }

        template<size_t index, size_t il, size_t ir>
        static void QAdd(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t il, size_t ir>
        static void CAdd(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        ) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QAdd, result, lhs, rhs);
        }

        template<size_t il, size_t ir>
        static void Add(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        ) {
            REGISTER_BETTER_VIRTUAL_BIGINTEGER_FUNCTION(il, ir, CAdd, result, lhs, rhs);
        }

        template<size_t index, size_t il, size_t ir>
        static void QSub(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t il, size_t ir>
        static void Sub(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        ) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QSub, result, lhs, rhs);
        }

        static value_type QMul_1(
            value_type* result,
            const value_type* lp,
            value_type rv,
            size_t n
        );

        static value_type QAddmul_1(
            value_type* result,
            const value_type* lp,
            value_type rv,
            size_t n
        );

        template<size_t index, size_t il, size_t ir>
        static void slow_mul(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t ir>
        static void pos_add(
            value_type* result,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t il, size_t ir>
        static void karatsuba(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t il, size_t ir>
        static void toom_cook_3(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t i, size_t index, size_t il, size_t ir>
        static void dac_mul(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs,
            unsigned_biginteger& temp,
            unsigned_biginteger& r
        );

        template<size_t index, size_t il, size_t ir>
        static void dac_mul(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t il, size_t ir>
        static void fft_mul(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t index, size_t il, size_t ir>
        static void QMul(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );
        template<size_t il, size_t ir>
        static void CMul(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        ) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(ir, rhs, QMul, result, lhs, rhs);
        }
        template<size_t il, size_t ir>
        static void Mul(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        ) {
            REGISTER_BETTER_VIRTUAL_BIGINTEGER_FUNCTION(il, ir, CMul, result, lhs, rhs);
        }

    public:

        VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
            static int cmp(const T& lhs, const U& rhs) {
            return Cmp(
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs)
            );
        }

        VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
            static bool equal(const T& lhs, const U& rhs) {
            return Equal(
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs)
            );
        }
        VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
            static bool nequal(const T& lhs, const U& rhs) {
            return !equal(lhs, rhs);
        }
        VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
            static bool less(const T& lhs, const U& rhs) {
            return Less(
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs)
            );
        }
        VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
            static bool less_eq(const T& lhs, const U& rhs) {
            return Less_eq(
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs)
            );
        }

        VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
            static void add(unsigned_biginteger& result, const T& lhs, const U& rhs) {
            Add(result,
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs));
        }

        VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
            static void sub(unsigned_biginteger& result, const T& lhs, const U& rhs) {
            Sub(result,
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs));
        }

        VIRTUAL_UNSIGNED_BIGINTEGER_BINARY_DECLARATION
            static void mul(unsigned_biginteger& result, const T& lhs, const U& rhs) {
            Mul(result,
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs));
        }

    private:

        __biginteger_vector vec;
    };

    template<size_t base>
    template<size_t index, size_t il, size_t ir>
    int unsigned_biginteger<base>::QCmp(
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs
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
            else {
                return (p[2] != q[2]) ? (p[2] < q[2] ? -1 : 1) :
                    (p[1] != q[1]) ? (p[1] < q[1] ? -1 : 1) :
                    (p[0] != q[0]) ? (p[0] < q[0] ? -1 : 1) : 0;
            }
        }
        else {
            size_t m = rhs.size();
            if (n != m) return n < m ? -1 : 1;
            auto p = lhs.data() + n;
            auto q = rhs.data() + n;
            while(n--) {
                --p;
                --q;
                if (*p != *q)return *p < *q ? -1 : 1;
            }
            return 0;
        }
    }

    template<size_t base>
    template<size_t index, size_t il, size_t ir>
    bool unsigned_biginteger<base>::QEqual(
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs) {
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
            else {
                return p[0] == q[0] && p[1] == q[1] && p[2] == q[2];
            }
        }
        else {
            return (n == rhs.size()) &&
                (memcmp(lhs.data(), rhs.data(), sizeof(value_type) * n) == 0);
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
        }else {						                            \
            return p[2] != q[2] ? p[2] op q[2] : p[1] != q[1] ?	\
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
    bool unsigned_biginteger<base>::QLess(
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs) {
        GENERATE_BIGINTEGER_OPERATOR(< , false);
    }

    template<size_t base>
    template<size_t index, size_t il, size_t ir>
    bool unsigned_biginteger<base>::QLess_eq(
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs) {
        GENERATE_BIGINTEGER_OPERATOR(<= , true);
    }

    template<size_t base>
    template<size_t index, size_t il, size_t ir>
    void unsigned_biginteger<base>::QAdd(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs) {
        WASSERT_LEVEL_2(lhs.size() >= rhs.size());
        WASSERT_LEVEL_2((!index) || (index == rhs.size()));
        auto res = result.size();
        auto ls = lhs.size();
        auto lp = lhs.data();
        auto rs = rhs.size();
        auto rp = rhs.data();
        unsigned char cf = 0;
        if constexpr (index != 1) {
            // if res < rs
            // then lhs or rhs won't be a part of result
            if (res < rs) {
                result.vec.reserve(ls + 1);
                result.vec.resize(rs);
                res = rs;
            }
        }
        // we may need to reserve result
        // so need to get data() after it
        auto rep = result.data();
        if constexpr (index != 0) {
            if constexpr (index >= 1) {
                cf = traits::quick_add(    *lp, *rp, rep);
                ++lp;
                ++rp;
                ++rep;
            }
            if constexpr (index >= 2) {
                cf = traits::quick_add(cf, *lp, *rp, rep);
                ++lp;
                ++rp;
                ++rep;
            }
            if constexpr (index >= 3) {
                cf = traits::quick_add(cf, *lp, *rp, rep);
                ++lp;
                ++rp;
                ++rep;
            }
        }
        else {
            size_t q = rs >> 2;
            switch (rs & 3) {
            case 3:
                cf = traits::quick_add(    *lp, *rp, rep);
                ++lp;
                ++rp;
                ++rep;
                [[fallthrough]];
            case 2:
                cf = traits::quick_add(cf, *lp, *rp, rep);
                ++lp;
                ++rp;
                ++rep;
                [[fallthrough]];
            case 1:
                cf = traits::quick_add(cf, *lp, *rp, rep);
                ++lp;
                ++rp;
                ++rep;
                [[fallthrough]];
            default:
                while (q) {
                    cf = traits::quick_add(cf, lp[0], rp[0], rep);
					cf = traits::quick_add(cf, lp[1], rp[1], rep + 1);
					cf = traits::quick_add(cf, lp[2], rp[2], rep + 2);
					cf = traits::quick_add(cf, lp[3], rp[3], rep + 3);
                    lp += 4;
                    rp += 4;
                    rep += 4;
                    --q;
                }
            }
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
            biginteger_ememcpy(rep, lp, sizeof(value_type) * (ls - rs));
            result.vec.resize(ls);
        }
        else {
            result.vec.resize(rs);
            result.vec.insert(result.vec.end(), lp, lp + (ls - rs));
            rep = result.data() + rs;
        }
        if (cf) {
            ls -= rs;
            while (ls && *rep == traits::_max) {
                *rep = 0;
                ++rep;
            }
            if (!ls)result.vec.push_back(1);
            else ++(*rep);
        }
    }

    template<size_t base>
    template<size_t index, size_t il, size_t ir>
    void unsigned_biginteger<base>::QSub(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
        WASSERT_LEVEL_3(less_eq(rhs, lhs));
        auto lp = lhs.data();
        auto rp = rhs.data();
        auto res = result.size();
        auto ls = lhs.size();
        auto rs = rhs.size();
        unsigned char cf = 0;
        if constexpr (index != 1) {
            if (res < rs) {
                result.vec.reserve(ls + 1);
                result.vec.resize(rs);
                res = rs;
            }
        }
        auto rep = result.data();
        if constexpr (index != 0) {
            if constexpr (index >= 1) {
                cf = traits::quick_sub(    lp[0], rp[0], &rep[0]);
            }
            if constexpr (index >= 2) {
                cf = traits::quick_sub(cf, lp[1], rp[1], &rep[1]);
            }
            if constexpr (index >= 3) {
                cf = traits::quick_sub(cf, lp[2], rp[2], &rep[2]);
            }
        }
        else {
            size_t i = 0;
            switch (rs & 3) {
            case 3:
                cf = traits::quick_sub(    lp[i], rp[i], &rep[i]);
                ++i;
                [[fallthrough]];
            case 2:
                cf = traits::quick_sub(cf, lp[i], rp[i], &rep[i]);
                ++i;
                [[fallthrough]];
            case 1:
                cf = traits::quick_sub(cf, lp[i], rp[i], &rep[i]);
                ++i;
                [[fallthrough]];
            default:
                while (i != rs) {
                    cf = traits::quick_sub(cf, lp[i], rp[i], &rep[i]);
                    cf = traits::quick_sub(cf, lp[i + 1], rp[i + 1], &rep[i + 1]);
                    cf = traits::quick_sub(cf, lp[i + 2], rp[i + 2], &rep[i + 2]);
                    cf = traits::quick_sub(cf, lp[i + 3], rp[i + 3], &rep[i + 3]);
                    i += 4;
                }
            }
        }
        if (ls <= res) {
            biginteger_ememcpy(rep + rs, lp + rs, sizeof(value_type) * (ls - rs));
            result.vec.resize(ls);
        }
        else {
            result.vec.resize(rs);
            result.vec.insert(result.vec.end(), lp + rs, lp + ls);
            rep = result.data();
        }
        if (cf) {
            size_t pos = rs;
#if WDEBUG_LEVEL >= 2
            while (pos != ls && rep[pos] == 0) {
                rep[pos] = traits::_max;
                ++pos;
            }
            WASSERT_LEVEL_2(pos != ls);
#else
            while (rep[pos] == 0) {
				rep[pos] = traits::_max;
				++pos;
            }
#endif
            --rep[pos];
        }
        while (ls != 1 && !rep[ls - 1])--ls;
        result.vec.resize(ls);
    }

    template<size_t base>
    typename unsigned_biginteger<base>::value_type
        unsigned_biginteger<base>::QMul_1(
            value_type* result,
            const value_type* lp,
            value_type rv,
            size_t n
        ) {
        value_type lv, cf;
#if defined(WJR_BIGINTEGER_USE_X64)
        value_type _high, _low;
#endif
        cf = 0;
        do
        {
            lv = *(lp++);
#if defined(WJR_BIGINTEGER_USE_X64)
            if constexpr (traits::is_full()) {
                auto val = (twice_value_type)(lv)*rv;
                _high = traits::get_high(val);
                _low = traits::get_low(val);

                _low += cf;
                cf = _high + (_low < cf);

                *(result++) = _low;
            }
            else {
#endif
                auto val = (twice_value_type)(lv)*rv + cf;
                cf = traits::get_high(val);
                *(result++) = traits::get_low(val);
#if defined(WJR_BIGINTEGER_USE_X64)
            }
#endif
        } while (--n != 0);

        return cf;
    }

    template<size_t base>
    typename unsigned_biginteger<base>::value_type
        unsigned_biginteger<base>::QAddmul_1(
            value_type* result,
            const value_type* lp,
            value_type rv,
            size_t n
        ) {
#if defined(WJR_BIGINTEGER_USE_X64)
        value_type lv, cf, c, _high, _low, r0;
#else
        value_type lv, cf;
#endif
        cf = 0;
        do
        {
            lv = *(lp++);
#if defined(WJR_BIGINTEGER_USE_X64)
            if constexpr (traits::is_full()) {
                auto val = (twice_value_type)(lv)*rv;
                _high = traits::get_high(val);
                _low = traits::get_low(val);

                r0 = *result;

                _low = r0 + _low;
                c = r0 > _low;

                _high = _high + c;

                r0 = _low + cf;		/* cycle 0, 3, ... */
                c = _low > r0;		/* cycle 1, 4, ... */

                cf = _high + c;		/* cycle 2, 5, ... */

                *(result++) = r0;
            }
            else {
#endif
                auto val = (twice_value_type)(lv)*rv + (*result) + cf;
                cf = traits::get_high(val);
                *(result++) = traits::get_low(val);
#if defined(WJR_BIGINTEGER_USE_X64)
            }
#endif
        } while (--n != 0);

        return cf;
    }

    template<size_t base>
    template<size_t index, size_t il, size_t ir>
    void unsigned_biginteger<base>::slow_mul(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
        constexpr static size_t cache_size = 256;
        USE_THREAD_LOCAL static value_type static_array[cache_size];

        auto lp = lhs.data();
        auto rp = rhs.data();
        auto res = result.size();
        auto ls = lhs.size();
        auto rs = rhs.size();

        if constexpr (index == 1) {
            auto rv = *rp;
            if (res < ls) {
                result.vec.reserve(ls + 1);
            }
            result.vec.resize(ls);
            auto rep = result.data();
            auto _Val = QMul_1(rep, lp, rv, ls);
            if (_Val) {
                result.vec.push_back(_Val);
            }
        }
        else {
            const size_t len = ls + rs;
            value_type* temp_array;
            if (len <= cache_size) {
                temp_array = static_array;
            }else{
                temp_array = new value_type[len];
			}
            auto rep = temp_array;
            rep[ls] = QMul_1(rep, lp, *rp, ls);
            ++rep;
            ++rp;
            --rs;
            if constexpr (index >= 2) {
                rep[ls] = QAddmul_1(rep, lp, *rp, ls);
                ++rep;
                ++rp;
                --rs;
            }
            if constexpr (index >= 3) {
                rep[ls] = QAddmul_1(rep, lp, *rp, ls);
                ++rep;
                ++rp;
                --rs;
            }
            if constexpr (!index) {
                WASSERT_LEVEL_2(rs >= 1);
                do {
                    rep[ls] = QAddmul_1(rep, lp, *rp, ls);
                    ++rep;
                    ++rp;
                } while (--rs != 0);
            }
            result.vec.assign(temp_array, temp_array + len);
            if (!temp_array[len - 1]) {
                result.vec.pop_back();
            }
            if(len > cache_size) {
				delete[] temp_array;
			}
        }

    }

    template<size_t base>
    template<size_t ir>
    void unsigned_biginteger<base>::pos_add(
        value_type* result,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
        auto m = rhs.size();
        auto rp = rhs.data();
        size_t i = 0;
        unsigned char cf = 0;
        for (; i < m; ++i) {
            cf = traits::quick_add(cf, result[i], rp[i], &result[i]);
        }
        if (cf) {
            while (result[i] == traits::_max) {
                result[i] = 0;
                ++i;
            }
            ++result[i];
        }
    }

    template<size_t base>
    template<size_t il, size_t ir>
    void unsigned_biginteger<base>::karatsuba(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
        size_t n = lhs.size();
        size_t m = rhs.size();
        size_t mid = n >> 1;
        WASSERT_LEVEL_1(m > mid);
        virtual_unsigned_biginteger<0> l_high(lhs.data() + mid, n - mid);
        virtual_unsigned_biginteger<0> l_low(lhs.data(), mid);
        virtual_unsigned_biginteger<0> r_high(rhs.data() + mid, m - mid);
        virtual_unsigned_biginteger<0> r_low(rhs.data(), mid);
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
        result.vec.clear();
        result.vec.resize(n + m);
        auto rep = result.data();
        memcpy(rep + 2 * mid, A.data(), sizeof(value_type) * A.size());
        size_t l = B.size() < mid ? B.size() : mid;
        memcpy(rep + mid, B.data(), sizeof(value_type) * l);
        if (l != B.size()) {
            pos_add(rep + 2 * mid, virtual_unsigned_biginteger<0>(B.data() + l, B.size() - l));
        }
        l = C.size() < mid ? C.size() : mid;
        memcpy(rep, C.data(), sizeof(value_type) * l);
        if (l != C.size()) {
            pos_add(rep + mid, virtual_unsigned_biginteger<0>(C.data() + l, C.size() - l));
        }
        size_t s = n + m;
        while (s != 1 && !rep[s - 1])--s;
        result.vec.resize(s);
    }

    template<size_t base>
    template<size_t i, size_t index, size_t il, size_t ir>
    void unsigned_biginteger<base>::dac_mul(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs,
        unsigned_biginteger& temp,
        unsigned_biginteger& r
    ) {
        size_t n = lhs.size();
        size_t m = rhs.size();
        size_t l = n / index;
        size_t len;
        if constexpr (i != index - 1) {
            len = l;
        }
        else {
            len = n - i * l;
        }
        virtual_unsigned_biginteger<0> g(lhs.data() + i * l, len);
        if constexpr (i != index - 1) {
            g.maintain();
        }
        mul(temp, g, rhs);
        pos_add(r.data() + i * l, virtual_unsigned_biginteger<0>(temp.data(), temp.size()));
        if constexpr (i == index - 1) {
            result = std::move(r);
            size_t s = n + m;
            auto rep = result.data();
            while (s != 1 && !rep[s - 1])--s;
            result.vec.resize(s);
        }
        else {
            dac_mul<i + 1, index>(result, lhs, rhs, temp, r);
        }
    }

    template<size_t base>
    template<size_t index, size_t il, size_t ir>
    void unsigned_biginteger<base>::dac_mul(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
        size_t n = lhs.size();
        size_t m = rhs.size();
        unsigned_biginteger temp, r;
        r.vec.resize(n + m);
        if constexpr (index != 0) {
            WASSERT_LEVEL_1(n >= index * m);
            dac_mul<0, index>(result, lhs, rhs, temp, r);
        }
        else {
            size_t idx = (n + m - 1) / m;
            WASSERT_LEVEL_1(n > (idx - 1) * m && n <= idx * m);
            for (size_t i = 0; i < idx; ++i) {
                size_t s = i == idx - 1 ? n - i * m : m;
                virtual_unsigned_biginteger<0> g(lhs.data() + i * m, s);
                g.maintain();
                mul(temp, g, rhs);
                pos_add(r.data() + i * m, virtual_unsigned_biginteger<0>(temp.data(), temp.size()));
            }
            result = std::move(r);
            size_t s = n + m;
            auto rep = result.data();
            while (s != 1 && !rep[s - 1])--s;
            result.vec.resize(s);
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
    void unsigned_biginteger<base>::fft_mul(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
        constexpr uint64_t _Maxn = (uint64_t)(1) << 49;

        size_t pn = lhs.size();
        size_t pm = rhs.size();
        size_t plen = (pn + pm) * traits::bit_length;
        size_t pk = 1;
        uint64_t G = base * base;
        uint64_t X = G;
        uint64_t maxn = _Maxn / G;
        // X * G * plen / (pk + 1) <= Maxn
        for (; X <= maxn / ((plen + pk) / (pk + 1)); ++pk, X *= G) {
            WASSERT_LEVEL_2(pk <= traits::bit_length);
        }

        auto lhs_iter = lhs.cbegin(pk);
        auto rhs_iter = rhs.cbegin(pk);
        size_t n = (pn * traits::bit_length + pk - 1) / pk;
        size_t m = (pm * traits::bit_length + pk - 1) / pk;
        size_t len = n + m - 1;
        auto bit = cqlog2(len - 1) + 1;
        auto s = ((size_t)1) << bit;

        auto A = std::make_unique<double[]>(s << 1);
        auto a = A.get();
        __biginteger_fft_cache.make(s << 1);
        for (size_t i = 0; i < n; ++lhs_iter, ++i) {
            value_type val = *lhs_iter;
            a[i << 1] = val;
            a[i << 1 | 1] = val;
        }
        for (size_t i = (n << 1); i < (s << 1); ++i) {
            a[i] = 0;
        }
        for (size_t i = 0; i < m; ++rhs_iter, ++i) {
            value_type val = *rhs_iter;
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

        result.vec.clear();
        result.vec.resize(pn + pm, 0);
        virtual_unsigned_biginteger_iterator<base>
            write(result.data(), result.size(), pk);
        uint64_t val = 0;
        uint32_t Mod = traits::get_base_power(pk);
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

        size_t pos = result.size();
        auto p = result.data();
        while (pos != 1 && !p[pos - 1])--pos;
        result.vec.resize(pos, 0);
    }

    template<size_t base>
    template<size_t index, size_t il, size_t ir>
    void unsigned_biginteger<base>::QMul(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
        if constexpr (index != 0) {
            if constexpr (index == 1) {
                if (lhs.zero() || rhs.zero()) {
                    result.set_zero();
                    return;
                }
            }
            slow_mul<index>(result, lhs, rhs);
        }
        else {
            size_t n = lhs.size();
            size_t m = rhs.size();
            auto& mul_info = traits::threshold::mul_info;
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
    class virtual_biginteger : public virtual_unsigned_biginteger<base, length> {
        using _Base = virtual_unsigned_biginteger<base, length>;
    public:
        template<typename T, std::enable_if_t<_Is_unsigned_integral_v<T>, int> = 0>
        constexpr explicit virtual_biginteger(T val)
            noexcept : _Base(val) {}
        template<typename T, std::enable_if_t<_Is_signed_integral_v<T>, int> = 0>
        constexpr explicit virtual_biginteger(T val)
            noexcept : _Signal(val >= 0),
            _Base(static_cast<std::make_unsigned_t<T>>(val >= 0 ? val : (0 - val))) {}
        constexpr explicit virtual_biginteger(
            const virtual_unsigned_biginteger<base, length>& rhs) noexcept : _Base(rhs) {}
        constexpr explicit virtual_biginteger(
            bool _Signal,
            const virtual_unsigned_biginteger<base, length>& rhs) noexcept :
            _Signal(_Signal), _Base(rhs) {
            maintain_signal();
        }
        constexpr virtual_biginteger(const virtual_biginteger&) noexcept = default;
        constexpr virtual_biginteger& operator=(const virtual_biginteger&) noexcept = default;
        constexpr bool signal()const noexcept { return _Signal; }
        constexpr _Base& get_unsigned() noexcept { return *this; }
        constexpr const _Base& get_unsigned()const noexcept { return *this; }
        template<typename T>
        explicit operator T()const noexcept {
            auto _Val = _Base::template to<T>();
            return _Signal ? _Val : (0 - _Val);
        }
        constexpr void maintain_signal() noexcept {
            _Signal = _Signal || _Base::zero();
        }
        constexpr void negate() noexcept {
            _Signal = _Base::zero() || (!_Signal);
        }
        constexpr void abs() noexcept {
            _Signal = true;
        }
    private:
        bool _Signal = true;
    };

    template<size_t base>
    class virtual_biginteger<base, 0> : public virtual_unsigned_biginteger<base, 0> {
        using _Base = virtual_unsigned_biginteger<base, 0>;
        using value_type = typename _Base::value_type;
    public:
        constexpr virtual_biginteger(const value_type* _Data, size_t _Size)
            noexcept : _Base(_Data, _Size) {}
        constexpr virtual_biginteger(bool _Signal, const value_type* _Data, size_t _Size)
            noexcept : _Signal(_Signal), _Base(_Data, _Size) {
            maintain_signal();
        }
        constexpr explicit virtual_biginteger(
            const virtual_unsigned_biginteger<base, 0>& rhs) noexcept : _Base(rhs) {}
        constexpr explicit virtual_biginteger(
            bool _Signal,
            const virtual_unsigned_biginteger<base, 0>& rhs) noexcept : _Signal(_Signal), _Base(rhs) {
            maintain_signal();
        }
        constexpr explicit virtual_biginteger(const biginteger<base>& rhs)
            noexcept : _Signal(rhs.signal()), _Base(rhs.data(), rhs.size()) {
            // don't need to maintain signal
        }
        constexpr virtual_biginteger(const virtual_biginteger&) noexcept = default;
        constexpr virtual_biginteger& operator=(const virtual_biginteger&) noexcept = default;
        constexpr bool signal()const noexcept {
            WASSERT_LEVEL_1(!(_Base::zero() && !_Signal));
            return _Signal;
        }
        constexpr _Base& get_unsigned() noexcept {
            return *this;
        }
        constexpr const _Base& get_unsigned()const noexcept {
            return *this;
        }
        template<typename T>
        explicit operator T()const noexcept {
            auto _Val = _Base::template to<T>();
            return _Signal ? _Val : (0 - _Val);
        }
        virtual_biginteger& maintain() noexcept {
            _Base::maintain();
            maintain_signal();
            return *this;
        }
        void maintain_signal() noexcept {
            _Signal = _Signal || _Base::zero();
        }
        void negate() noexcept {
            _Signal = (!_Signal) || _Base::zero();
        }
        void abs() noexcept {
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
    struct biginteger_base_helper<virtual_unsigned_biginteger<base, length>> {
        constexpr static size_t value = base;
    };

    template<size_t base, size_t length>
    struct biginteger_base_helper<virtual_biginteger<base, length>> {
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
            if constexpr (lvalue == -1 || rvalue == -1) {
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
    struct _Is_original_virtual_biginteger : std::false_type {};

    template<size_t base, size_t length>
    struct _Is_original_virtual_biginteger<
        virtual_biginteger<base, length>> : std::true_type {};

    template<typename T>
    constexpr static bool _Is_original_virtual_biginteger_v = _Is_original_virtual_biginteger<T>::value;

    template<size_t base, typename T,
        std::enable_if_t<_Is_base_mask_v<base, biginteger_base_v<T>>, int> = 0>
        constexpr static decltype(auto) get_virtual_biginteger(const T& t) {
        using traits = biginteger_traits<base>;
        if constexpr (std::is_integral_v<T>) {
            return virtual_biginteger<base, traits::template bit_of_t<T>>(t);
        }
        else if constexpr (_Is_unsigned_biginteger_v<T>) {
            return virtual_biginteger<base, 0>(true, virtual_unsigned_biginteger<base, 0>(t));
        }
        else if constexpr (_Is_biginteger_v<T>) {
            return virtual_biginteger<base, 0>(t);
        }
        else if constexpr (_Is_original_virtual_unsigned_biginteger_v<T>) {
            return virtual_biginteger<base, 0>(true, t);
        }
        else return t;
    }

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
        int cmp(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        void add(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        );
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        void sub(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        );
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        void mul(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        );

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        bool operator==(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        bool operator!=(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        bool operator<(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        bool operator<=(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        bool operator>(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        bool operator>=(const T& lhs, const U& rhs);
	
#if defined(WJR_CPP_20)
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        std::strong_ordering operator<=>(const T& lhs, const U& rhs) {
        int f = cmp(lhs, rhs);
		if (f < 0) return std::strong_ordering::less;
		if (f > 0) return std::strong_ordering::greater;
		return std::strong_ordering::equal;
    }
#endif

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        biginteger<_Base> operator+(T&& lhs, U&& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        biginteger<_Base> operator-(T&& lhs, U&& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        biginteger<_Base> operator*(T&& lhs, U&& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        biginteger<_Base> operator/(T&& lhs, U&& rhs);
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DECLARATION
        biginteger<_Base> operator%(T&& lhs, U&& rhs);

    template<size_t _Base>
    void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);
    template<size_t _Base>
    biginteger<_Base> pow(const biginteger<_Base>& a, size_t n);
    template<size_t _Base>
    biginteger<_Base> pow(biginteger<_Base>&& a, size_t n);

    template<size_t toBase, typename T, size_t fromBase = biginteger_base_v<T>,
        std::enable_if_t<_Is_biginteger_base_masks_v<fromBase, T>, int> = 0>
        biginteger<toBase> base_conversion(T&&);

    // generate a biginteger of length n
    template<size_t _Base>
    void random_biginteger(biginteger<_Base>& result, size_t n, bool _unsigned = true, bool exact = true);
    template<size_t _Base>
    biginteger<_Base> random_biginteger(size_t n, bool _unsigned = true, bool exact = true);

    template<size_t _Base, typename T,
        std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> = 0>
        void random_biginteger_max(biginteger<_Base>& result, const T& _Max);
    template<size_t _Base, typename T, typename U,
        std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T, U>, int> = 0>
        void random_biginteger_min_max(biginteger<_Base>& result, const T& _Min, const U& _Max);

    template<size_t _Base, typename T,
        std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> = 0>
        biginteger<_Base> random_biginteger_max(const T& _Max);
    template<size_t _Base, typename T, typename U,
        std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T, U>, int> = 0>
        biginteger<_Base> random_biginteger_min_max(const T& _Min, const U& _Max);

    template<size_t base>
    class biginteger {
        template<size_t _Base>
        friend class biginteger;
        template<size_t _Base>
        friend class unsigned_biginteger;

        using traits = biginteger_traits<base>;
        using unsigned_traits = unsigned_biginteger<base>;
        template<size_t length>
        using virtual_biginteger = wjr::virtual_biginteger<base, length>;
        template<typename T>
        using _Is_virtual_biginteger =
            _Is_base_mask<base, biginteger_base_v<T>>;
        template<typename T>
        constexpr static bool _Is_virtual_biginteger_v =
            _Is_virtual_biginteger<T>::value;
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
        using traits_type = traits;
        using value_type = typename traits::value_type;
        using twice_value_type = typename traits::twice_value_type;
        using iterator = virtual_unsigned_biginteger_iterator<base>;
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using reverse_const_iterator = std::reverse_iterator<const_iterator>;
        using reference = typename iterator::reference;
        using const_reference = typename const_iterator::reference;

        template<typename iter, typename Func>
        void in(iter first, iter last, Func&& fn) {
            size_t n = std::distance(first, last);
            if (!n) {
                set_zero();
                return;
            }
            char first_val = *first;
            if (first_val == '-' || first_val == '+') {
                _Signal = (first_val == '+');
                ++first;
                --n;
            }
            ubint.in(first, last, std::forward<Func>(fn));
        }

        template<typename iter, typename Func>
        iter out(iter first, iter last, Func&& fn) const {
            if (!_Signal) {
                *first = '-';
                ++first;
            }
            return ubint.out(first, last, std::forward<Func>(fn));
        }

        biginteger() = default;
        biginteger(const biginteger&) = default;
        biginteger(biginteger&&) noexcept = default;

        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_1_v<fromBase, base>, int> = 0>
        explicit biginteger(const biginteger<fromBase>& other)
            : _Signal(other.signal()), ubint(other.ubint) {}
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_1_v<fromBase, base>, int> = 0>
        explicit biginteger(biginteger<fromBase>&& other)
            : _Signal(other.signal()), ubint(std::move(other.ubint)) {}
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_2_v<fromBase, base>, int> = 0>
        explicit biginteger(const biginteger<fromBase>& other)
            : _Signal(other.signal()), ubint(other.ubint) {
        }

        template<size_t length>
        explicit biginteger(const virtual_biginteger<length>& rhs)
            : _Signal(rhs.signal()), ubint(rhs.get_unsigned()) {}
        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        explicit biginteger(const T& val)
            : biginteger(get_virtual_biginteger<base>(val)) {}
        explicit biginteger(unsigned_biginteger<base>&& other)
            : /*_Signal(true), */ubint(std::move(other)) {}
        template<typename iter, typename Func>
        biginteger(iter first, iter last, Func&& fn)
            : ubint(first, last, std::forward<Func>(fn)) {}
        template<typename iter, typename Func>
        biginteger(bool _Signal, iter first, iter last, Func&& fn)
            : _Signal(_Signal), ubint(first, last, std::forward<Func>(fn)) {
            maintain_signal();
        }
        biginteger(const value_type* _Data, size_t _Size)
            : ubint(_Data, _Size) {}
        biginteger(bool _Signal, const value_type* _Data, size_t _Size)
            : _Signal(_Signal), ubint(_Data, _Size) {}
        template<typename Func = typename traits::fromString>
        explicit biginteger(const String_view& str, Func&& fn = Func()) {
            in(str.begin(), str.end(), std::forward<Func>(fn));
        }
        biginteger(bool _Signal, const unsigned_biginteger<base>& ubint)
            noexcept : _Signal(_Signal), ubint(ubint) {}
        biginteger(bool _Signal, unsigned_biginteger<base>&& ubint)
            noexcept : _Signal(_Signal), ubint(std::move(ubint)) {}

        biginteger& operator=(const biginteger&) = default;
        biginteger& operator=(biginteger&&) noexcept = default;
        template<size_t length>
        biginteger& operator=(const virtual_biginteger<length>& rhs) {
            _Signal = rhs.signal();
            ubint = rhs.get_unsigned();
            return *this;
        }
        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator=(const T& rhs) {
            (*this) = get_virtual_biginteger<base>(rhs);
            return *this;
        }
        biginteger& operator=(unsigned_biginteger<base>&& other) {
            _Signal = true;
            ubint = std::move(other);
            return *this;
        }
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_1_v<fromBase, base>, int> = 0>
        biginteger& operator=(const biginteger& other) {
            _Signal = other.signal();
            ubint = other.ubint;
            return *this;
        }
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_1_v<fromBase, base>, int> = 0>
        biginteger& operator=(biginteger&& other) {
            _Signal = other.signal();
            ubint = std::move(other.ubint);
            return *this;
        }

        template<typename Func = typename traits::fromString>
        biginteger& assign(const String_view& str, Func&& fn = Func()) {
            in(str.begin(), str.end(), std::forward<Func>(fn));
            return *this;
        }

        biginteger& operator=(const String_view& str) {
            return assign(str);
        }

        template<typename iter, typename Func>
        biginteger& assign(iter first, iter last, Func&& fn) {
            return this->assign(true, first, last, std::forward<Func>(fn));
        }
        template<typename iter, typename Func>
        biginteger& assign(bool _Sign, iter first, iter last, Func&& fn) {
            ubint.assign(first, last, std::forward<Func>(fn));
            set_signal(_Sign);
            return *this;
        }
        biginteger& assign(const value_type* _Data, size_t _Size) {
            return this->assign(true, _Data, _Size);
        }
        biginteger& assign(bool _Sign, const value_type* _Data, size_t _Size) {
            _Signal = _Sign;
            ubint.assign(_Data, _Size);
            return *this;
        }
        void swap(biginteger& other) noexcept {
            ubint.swap(other.ubint);
            std::swap(_Signal, other._Signal);
        }

        value_type* data() { return ubint.data(); }
        const value_type* data()const { return ubint.data(); }
        size_t size()const { return ubint.size(); }
        bool zero()const {
            bool f = (size() == 1 && *data() == 0);
            WASSERT_LEVEL_1(!(f && !_Signal));
            return f;
        }
        bool one()const {
            return size() == 1 && *data() == 1;
        }
        bool signal()const { return _Signal; }
        void set_zero() { _Signal = true; ubint.set_zero(); }
        void maintain_signal() { _Signal = _Signal || ubint.zero(); }
        void negate() { _Signal = (!_Signal) || ubint.zero(); }
        void set_signal(bool f) { _Signal = f || ubint.zero(); }
        void reserve_bit(size_t k) {
            ubint.reserve_bit((k + traits::bit_length - 1) / traits::bit_length);
        }
        void resize_bit(size_t k) {
            ubint.resize_bit(k);
        }
        void abs() { _Signal = true; }

        biginteger& pow(size_t n) {
            ubint.pow(n);
            _Signal = _Signal || (!(n & 1));
            return *this;
        }

        size_t bit_length() const {
            return ubint.bit_length();
        }

        iterator begin(size_t k = 1) {
            return ubint.begin(k);
        }
        const_iterator begin(size_t k = 1)const {
            return ubint.begin(k);
        }
        const_iterator cbegin(size_t k = 1)const {
            return ubint.cbegin(k);
        }

        iterator end(size_t k = 1) {
            return ubint.end(k);
        }
        const_iterator end(size_t k = 1)const {
            return ubint.end(k);
        }
        const_iterator cend(size_t k = 1) const {
            return ubint.cend(k);
        }
        reverse_iterator rbegin(size_t k = 1) {
            return ubint.rbegin(k);
        }
        reverse_const_iterator rbegin(size_t k = 1)const {
            return ubint.rbegin(k);
        }
        reverse_const_iterator rcbegin(size_t k = 1)const {
            return ubint.rcbegin(k);
        }
        reverse_iterator rend(size_t k = 1) {
            return ubint.rend(k);
        }
        reverse_const_iterator rend(size_t k = 1)const {
            return ubint.rend(k);
        }
        reverse_const_iterator rcend(size_t k = 1)const {
            return ubint.rcend(k);
        }

        reference operator[](size_t k) {
            return reference(data(), size(), 1, k);
        }

        const_reference operator[](size_t k)const {
            return const_reference(data(), size(), 1, k);
        }

        virtual_biginteger<0> get_virtual()const {
            return virtual_biginteger<0>(*this);
        }

        virtual_unsigned_biginteger<base, 0> get_unsigned_virtual()const {
            return get_virtual().get_unsigned();
        }

        biginteger& mul_base_power(size_t index) {
            ubint.mul_base_power(index);
            return *this;
        }

        biginteger& div_base_power(size_t index) {
            ubint.div_base_power(index);
            maintain_signal();
            return *this;
        }

        biginteger& mod_base_power(size_t index) {
            ubint.mod_base_power(index);
            return *this;
        }

        template<typename T>
        explicit operator T()const {
            return T(get_virtual());
        }

        String tostr()const {
            size_t n = bit_length();
            String str;
            str.resize(n + 1);
            size_t q = out(str.begin(), str.end(), traits::toString()) - str.begin();
            str.resize(q);
            return str;
        }

    private:

        template<size_t il, size_t ir>
        static int signed_cmp(
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs) {
            // if lhs < rhs , return -1
            // if lhs > rhs , return 1
            // if lhs == rhs , return 0
            int f = lhs.signal() != rhs.signal() ?
                1 : unsigned_traits::cmp(lhs.get_unsigned(), rhs.get_unsigned());
            return lhs.signal() ? f : -f;
        }

        template<size_t il, size_t ir>
        static void unsigned_add(
            biginteger& result,
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs
        ) {
            unsigned_traits::add(result.ubint, lhs.get_unsigned(), rhs.get_unsigned());
            result._Signal = lhs.signal();
        }

        template<size_t il, size_t ir>
        static void unsigned_sub(
            biginteger& result,
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs
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
                result.set_zero();
            }
        }

        template<size_t il, size_t ir>
        static void signed_add(
            biginteger& result,
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs
        ) {
            return lhs.signal() == rhs.signal() ?
                unsigned_add(result, lhs, rhs) : unsigned_sub(result, lhs, rhs);
        }

        template<size_t il, size_t ir>
        static void signed_sub(
            biginteger& result,
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs
        ) {
            return lhs.signal() == rhs.signal() ?
                unsigned_sub(result, lhs, rhs) : unsigned_add(result, lhs, rhs);
        }

        enum class div_mode {
            none,
            lhs_is_power_of_base,
        };

        template<size_t ir>
        static value_type one_divmod(
            biginteger& lhs,
            const std::in_place_index_t<ir>&
        ) {
            size_t n = lhs.size();
            auto p = lhs.data();
            twice_value_type _Val = 0;
            for (size_t i = n; i--;) {
                _Val = _Val * traits::max_value + p[i];
                p[i] = static_cast<value_type>(_Val / ir);
                _Val = _Val - p[i] * ir;
            }
            if (n != 1 && !p[n - 1]) {
                lhs.ubint.vec.resize(n - 1);
                WASSERT_LEVEL_1(lhs.ubint.vec.back() != 0);
            }
            return static_cast<value_type>(_Val);
        }

        template<size_t ir>
        static value_type one_divmod(
            biginteger& lhs,
            const virtual_biginteger<ir>& rhs) {
            WASSERT_LEVEL_2(rhs.size() == 1); // unlikely
            size_t n = lhs.size();
            auto p = lhs.data();
            twice_value_type _Val = 0;
            value_type v = *rhs.data();
            WASSERT_LEVEL_1(v != 0);
            if (n >= 8) {
                biginteger_libdivider<value_type> _Divider(v);
                for (size_t i = n; i--;) {
                    _Val = _Val * traits::max_value + p[i];
                    p[i] = static_cast<value_type>(_Val / _Divider);
                    _Val = _Val - traits::quick_mul(p[i], v);
                }
            }
            else {
                for (size_t i = n; i--;) {
                    _Val = _Val * traits::max_value + p[i];
                    p[i] = static_cast<value_type>(_Val / v);
                    _Val = _Val - traits::quick_mul(p[i], v);
                }
            }
            if (n != 1 && !p[n - 1]) {
                lhs.ubint.vec.resize(n - 1);
                WASSERT_LEVEL_1(lhs.ubint.vec.back() != 0);
            }
            return static_cast<value_type>(_Val);
        }

        template<size_t il, size_t ir>
        static value_type one_divmod(
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs) {
            WASSERT_LEVEL_2(rhs.size() == 1);
            size_t n = lhs.size();
            auto p = lhs.data();
            twice_value_type _Val = 0;
            value_type v = *rhs.data();
            WASSERT_LEVEL_1(v != 0);
            biginteger_libdivider<value_type> _Divider(v);
            for (size_t i = n; i--;) {
                _Val = _Val * traits::max_value + p[i];
                _Val = _Val - traits::quick_mul(static_cast<value_type>(_Val / _Divider), v);
            }
            return static_cast<value_type>(_Val);
        }

        template<div_mode mode>
        static void knuth_divmod(biginteger& lhs, biginteger& rhs) {
            value_type vr = rhs.data()[rhs.size() - 1];
            auto kth = static_cast<value_type>(((traits::max_value >> 1) + vr - 1) / vr);
            if (kth != 1) {
                lhs *= kth;
                rhs *= kth;
            }
            auto pl = lhs.data();
            size_t n = rhs.size();
            size_t m = lhs.size() - rhs.size();

            biginteger temp;

            auto val = rhs.data()[n - 1];
            WASSERT_LEVEL_1(val >= (traits::max_value >> 1));
            libdivide::divider<value_type> _Divider1(val);
            biginteger_libdivider<value_type> _Divider2(val);
            biginteger result;
            result.ubint.vec.resize(m + 1);
            auto presult = result.data();
            biginteger r;
            r.ubint.vec.reserve(n + 1);
            r = virtual_biginteger<0>(true, pl + m, n);

            temp.ubint.vec.reserve(m + 1);
            pl = pl + m - 1;
            for (size_t j = m + 1; j--; --pl) {
                value_type q = 0;
                if (r.size() == n + 1) {
                    q = std::min(traits::_max,
                        static_cast<value_type>((traits::max_value * r.data()[n] + r.data()[n - 1]) / _Divider2));
                }
                else if (r.size() == n) {
                    q = std::min(traits::_max, r.data()[n - 1] / _Divider1);
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
                    r.mul_base_power(traits::bit_length);
                    r.data()[0] = *pl;
                }
            }
            size_t size = result.size();
            while (size != 1 && !presult[size - 1])--size;
            result.ubint.vec.resize(size);
            lhs = std::move(result);
            rhs = std::move(r);

            if (kth != 1) {
                one_divmod(rhs, get_virtual_biginteger<base>(kth));
            }
        }

        template<div_mode mode>
        static void small_divmod(biginteger& lhs, biginteger& rhs) {
            size_t n = lhs.size();
            size_t m = rhs.size();
            size_t k = m << 1;
            size_t l = n;

            biginteger temp;

            biginteger E;
            E.ubint.vec.resize(k + 2);
            E.ubint.vec[k + 1] = 1;
            biginteger mo(rhs);
            quick_divmod<div_mode::lhs_is_power_of_base>(E, mo);
            size_t g = k + 1 - E.size();
            biginteger ans;
            ans.ubint.vec.resize(n - m + 1);
            biginteger mid_result;
            mo.set_zero();

            while (l) {
                size_t ml = std::min(l, k - mo.size());
                mo.mul_base_power(ml * traits::bit_length);
                auto cl = virtual_biginteger<0>(true, lhs.data() + l - ml, ml).maintain();
                if (mo.size() < cl.size())mo.ubint.vec.resize(cl.size());
                memcpy(mo.data(), cl.data(), sizeof(value_type) * cl.size());
                if (mo.size() > g) {
                    mul(mid_result, virtual_biginteger<0>(true, mo.data() + g, mo.size() - g), E);
                    mid_result.div_base_power(E.size() * traits::bit_length);
                }
                else {
                    mid_result.set_zero();
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
                memcpy(ans.data() + l - ml, mid_result.data(), sizeof(value_type) * mid_result.size());
                l -= ml;
            }
            lhs = std::move(ans);
            lhs.maintain();
            rhs = std::move(mo);
        }

        template<div_mode mode>
        static void mid_divmod(biginteger& lhs, biginteger& rhs) {
            size_t n = lhs.size();
            size_t m = rhs.size();
            size_t mid = (n - m) >> 1;
            size_t p = 2 * m - (n - mid) - 2;
            size_t k = n - mid - p;

            biginteger temp;

            biginteger E;
            // initialize E as base ^ ((k+1) * bit_length)
            E.ubint.vec.resize(k + 2, 0);
            E.ubint.vec[k + 1] = 1;

            biginteger ans1(virtual_biginteger<0>(true, rhs.data() + p, m - p));
            quick_divmod<div_mode::lhs_is_power_of_base>(E, ans1);
            size_t g = k + 1 - E.size();
            if constexpr (mode == div_mode::none) {
                mul(ans1, virtual_biginteger<0>(true, lhs.data() + mid + p + g, k - g), E);
                ans1.div_base_power(E.size() * traits::bit_length);
            }
            else {
                size_t bl = lhs.bit_length();
                ans1 = E;
                ans1.div_base_power(2 * traits::bit_length - ((bl - 1) % traits::bit_length));
            }

            mul(temp, ans1, rhs);

            biginteger mo1;
            sub(mo1, virtual_biginteger<0>(true, lhs.data() + mid, n - mid), temp);

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

            mo1.mul_base_power(mid * traits::bit_length);
            if constexpr (mode == div_mode::none) {
                auto view = virtual_biginteger<0>(true, lhs.data(), mid).maintain();
                if (mo1.size() < view.size())mo1.ubint.vec.resize(view.size());
                memcpy(mo1.data(), view.data(), sizeof(value_type) * view.size());
            }
            else {
#if WDEBUG_LEVEL >= 2
                for (size_t i = 0; i < mid; ++i)WASSERT_LEVEL_2(lhs.data()[i] == 0);
#endif
            }

            lhs = std::move(ans1);
            lhs.mul_base_power(mid * traits::bit_length);

            size_t ml = std::min(p + g, mo1.size());
            biginteger ans2;
            mul(ans2, virtual_biginteger<0>(true, mo1.data() + p + g, mo1.size() - ml).maintain(), E);
            ans2.div_base_power(E.size() * traits::bit_length);
            mul(temp, rhs, ans2);
            sub(mo1, mo1, temp);

            if (lhs.size() < ans2.size())lhs.ubint.vec.resize(ans2.size());
            memcpy(lhs.data(), ans2.data(), sizeof(value_type) * ans2.size());

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
        static void large_divmod(biginteger& lhs, biginteger& rhs) {
            size_t n = lhs.size();
            size_t m = rhs.size();
            size_t mid = 2 * m - n - 2;
            biginteger copyA(virtual_biginteger<0>(true, lhs.data() + mid, n - mid));
            biginteger copyB(virtual_biginteger<0>(true, rhs.data() + mid, m - mid));
            ++copyA;
            quick_divmod<div_mode::none>(copyA, copyB);
            --copyB;

            auto view = virtual_biginteger<0>(true, lhs.data(), mid).maintain();
            if (!copyB.signal()) {
                copyB.mul_base_power(mid * traits::bit_length);
                copyB += view;
            }
            else {
                copyB.mul_base_power(mid * traits::bit_length);
                if (copyB.size() < view.size())copyB.ubint.vec.resize(view.size());
                memcpy(copyB.data(), view.data(), sizeof(value_type) * view.size());
            }

            mul(lhs, virtual_biginteger<0>(true, rhs.data(), mid).maintain(), copyA);
            sub(copyB, copyB, lhs);
            if (!copyB.signal()) {
                --copyA;
                copyB += rhs;
            }

            lhs = std::move(copyA);
            rhs = std::move(copyB);
        }

        template<div_mode mode = div_mode::none>
        static void quick_divmod(biginteger& lhs, biginteger& rhs) {
            WASSERT_LEVEL_1(!rhs.zero() && lhs.signal() && rhs.signal());
            WASSERT_LEVEL_1(std::addressof(lhs) != std::addressof(rhs));
            int cmp = unsigned_biginteger<base>::cmp(lhs.ubint, rhs.ubint);
            if (cmp < 0) {
                lhs.swap(rhs);
                lhs.set_zero();
                return;
            }
            if (cmp == 0) {
                lhs = virtual_biginteger<1>(1u);
                rhs.set_zero();
                return;
            }
            size_t n = lhs.size();
            size_t m = rhs.size();
#if WDEBUG_LEVEL >= 3
            biginteger p(lhs), q(rhs);
#endif
            if (m == 1) {
                rhs = one_divmod(lhs, get_virtual_biginteger<base>(rhs));
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

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator+=(const T& rhs) {
            add(*this, *this, rhs);
            return *this;
        }

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator-=(const T& rhs) {
            sub(*this, *this, rhs);
            return *this;
        }

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator*=(const T& rhs) {
            mul(*this, *this, rhs);
            return *this;
        }

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator/=(T&& rhs) {
            auto _Rhs = get_virtual_biginteger<base>(std::forward<T>(rhs));
            bool f = signal() ^ (!_Rhs.signal());
            abs();
            _Rhs.abs();
            if constexpr (std::is_same_v<decltype(_Rhs), virtual_biginteger<1>>) {
                one_divmod(*this, _Rhs);
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

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator%=(T&& rhs) {
            auto _Rhs = get_virtual_biginteger<base>(std::forward<T>(rhs));
            bool f = signal();
            abs();
            _Rhs.abs();
            if constexpr (std::is_same_v<decltype(_Rhs), virtual_biginteger<1>>) {
                *this = one_divmod(*this, _Rhs);
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

        biginteger& operator++() {
            (*this) += virtual_biginteger<1>((uint32_t)1);
            return *this;
        }

        biginteger operator++(int) {
            biginteger it(*this);
            ++(*this);
            return it;
        }

        biginteger& operator--() {
            (*this) -= virtual_biginteger<1>((uint32_t)1);
            return *this;
        }

        biginteger operator--(int) {
            biginteger it(*this);
            --(*this);
            return it;
        }

        friend bool operator!(const biginteger& rhs) {
            return rhs.zero();
        }

        biginteger& maintain() {
            ubint.maintain();
            maintain_signal();
            return *this;
        }

        biginteger& random(size_t n, bool _unsigned = true, bool _exact = false) {
            random_biginteger<base>(*this, n, _unsigned, _exact);
            return *this;
        }

    private:

        __biginteger_vector&
            get_vec() {
            return (ubint.vec);
        }

        const __biginteger_vector&
            get_vec()const {
            return (ubint.vec);
        }

        template<size_t il, size_t ir>
        static bool equal(
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs) {
            return lhs.signal() == rhs.signal()
                && unsigned_traits::equal(lhs.get_unsigned(), rhs.get_unsigned());
        }
        template<size_t il, size_t ir>
        static bool less(
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs) {
            return lhs.signal() != rhs.signal() ?
                lhs.signal() < rhs.signal() :
                (unsigned_traits::less(lhs.get_unsigned(), rhs.get_unsigned()) ^ (!lhs.signal()));
        }
        template<size_t il, size_t ir>
        static bool less_eq(
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs) {
            return lhs.signal() != rhs.signal() ?
                lhs.signal() < rhs.signal() :
                lhs.signal() ? unsigned_traits::less_eq(lhs.get_unsigned(), rhs.get_unsigned())
                : !unsigned_traits::less(lhs.get_unsigned(), rhs.get_unsigned());
        }

        template<size_t toBase, size_t length>
        static biginteger<toBase> dc_base_conversion_helper(
            const wjr::virtual_unsigned_biginteger<base, length>& v,
            std::vector<std::pair<size_t, biginteger<toBase>>>& cache) {
            size_t n = v.size();
            if (n <= 2) {
                return biginteger<toBase>((twice_value_type)(v));
            }
            size_t mid = n >> 1;
            virtual_unsigned_biginteger<base, 0> lhs(v.data() + mid, n - mid);
            virtual_unsigned_biginteger<base, 0> rhs(v.data(), mid);
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
        static biginteger<toBase> base_conversion_helper(
            T&& v) {
            if constexpr (toBase == base
                || _Quick_base_conversion_1_v<base, toBase>
                || _Quick_base_conversion_2_v<base, toBase>) {
                return biginteger<toBase>(std::forward<T>(v));
            }
            else {
                std::vector<std::pair<size_t, biginteger<toBase>>> cache;
                cache.reserve(30);
                cache.emplace_back(1, biginteger<toBase>(traits::max_value));
                cache.emplace_back(2, cache[0].second * cache[0].second);
                auto view = get_virtual_biginteger<base>(v);
                biginteger<toBase> result = dc_base_conversion_helper<toBase>(
                    view.get_unsigned(), cache);
                if (!view.signal())result.negate();
                return result;
            }
        }

        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend int cmp(const T&, const U&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend void add(
                biginteger<_Base>& result,
                const T& lhs,
                const U& rhs
            );
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend void sub(
                biginteger<_Base>& result,
                const T& lhs,
                const U& rhs
            );
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend void mul(
                biginteger<_Base>& result,
                const T& lhs,
                const U& rhs
            );


        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend bool operator==(const T&, const U&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend bool operator!=(const T&, const U&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend bool operator<(const T&, const U&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend bool operator<=(const T&, const U&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend bool operator>(const T&, const U&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend bool operator>=(const T&, const U&);

        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend biginteger<_Base> operator+(T&&, U&&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend biginteger<_Base> operator-(T&&, U&&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend biginteger<_Base> operator*(T&&, U&&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend biginteger<_Base> operator/(T&&, U&&);
        VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
            friend biginteger<_Base> operator%(T&&, U&&);

        template<size_t _Base>
        friend void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);

        template<size_t _Base>
        friend biginteger<_Base> pow(const biginteger<_Base>& a, size_t n);
        template<size_t _Base>
        friend biginteger<_Base> pow(biginteger<_Base>&& a, size_t n);

        template<size_t toBase, typename T, size_t fromBase,
            std::enable_if_t<_Is_biginteger_base_masks_v<fromBase, T>, int> >
            friend biginteger<toBase> base_conversion(T&&);
        template<size_t _Base>
        friend void random_biginteger(biginteger<_Base>& result, size_t n, bool, bool);
        template<size_t _Base, typename T,
            std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> >
            friend void random_biginteger_max(biginteger<_Base>& result, const T& _Max);

        bool _Signal = true;
        unsigned_biginteger<base> ubint;
    };

    template<size_t base>
    template<size_t il, size_t ir>
    void unsigned_biginteger<base>::toom_cook_3(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
        size_t n = lhs.size(), m = rhs.size();
        size_t n3 = n / 3;
        WASSERT_LEVEL_1(m > n3);
        virtual_unsigned_biginteger<0> U0(lhs.data(), n3);
        virtual_unsigned_biginteger<0> U1(lhs.data() + n3, n3);
        virtual_unsigned_biginteger<0> U2(lhs.data() + 2 * n3, n - 2 * n3);
        virtual_unsigned_biginteger<0> V0(rhs.data(), n3);
        virtual_unsigned_biginteger<0> V1(rhs.data() + n3, m < 2 * n3 ? m - n3 : n3);
        virtual_unsigned_biginteger<0> V2(rhs.data() + 2 * n3, m < 2 * n3 ? 0 : m - 2 * n3);
        U0.maintain();
        U1.maintain();
        V0.maintain();
        V1.maintain();
        V2.maintain();

        biginteger<base> W0, W1, W2, W3, W4;
        wjr::add(W0, U0, U2);
        wjr::sub(W3, W0, U1);
        wjr::add(W0, W0, U1);
        wjr::add(W4, V0, V2);
        wjr::sub(W2, W4, V1);
        wjr::add(W4, W4, V1);

        wjr::mul(W1, W3, W2);
        wjr::mul(W2, W0, W4);

        W0 += U2;
        W0 *= 2u;
        W0 -= U0;
        W4 += V2;
        W4 *= 2u;
        W4 -= V0;

        wjr::mul(W3, W0, W4);
        wjr::mul(W0, U0, V0);
        wjr::mul(W4, U2, V2);

        W3 -= W1;
        biginteger<base>::one_divmod(W3, std::in_place_index_t<3>{});
        wjr::sub(W1, W2, W1);
        W1 /= 2u;
        W2 -= W0;
        W3 -= W2;
        W3 /= 2u;
        W3 -= W4.mul_base_power(1);
        W2 -= W1;

        W4.mul_base_power(n3 * traits::bit_length);
        W2.mul_base_power(n3 * traits::bit_length);

        W4 += W3;
        W2 += W1;
        W3 = std::move(W4);
        W1 = std::move(W2);
        W1 -= W3;

        WASSERT_LEVEL_1(W3.signal());
        WASSERT_LEVEL_1(W1.signal());
        WASSERT_LEVEL_1(W0.signal());

        result.vec.resize(n + m);
        memcpy(result.data() + 3 * n3, W3.data(), sizeof(value_type) * W3.size());
        size_t l = W1.size() < 2 * n3 ? W1.size() : 2 * n3;
        memcpy(result.data() + n3, W1.data(), sizeof(value_type) * l);
        if (l != W1.size()) {
            pos_add(result.data() + 3 * n3, virtual_unsigned_biginteger<0>(W1.data() + l, W1.size() - l));
        }
        l = W0.size() < n3 ? W0.size() : n3;
        memcpy(result.data(), W0.data(), sizeof(value_type) * l);
        if (l != W0.size()) {
            pos_add(result.data() + n3, virtual_unsigned_biginteger<0>(W0.data() + l, W0.size() - l));
        }
        result.maintain();
    }

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        int cmp(const T& lhs, const U& rhs) {
        return biginteger<_Base>::signed_cmp(
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
    }

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        void add(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        ) {
        biginteger<_Base>::signed_add(
            result,
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
    }

    template<size_t _Base>
    void inc(biginteger<_Base>& result) {
        ++result;
    }

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        void sub(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        ) {
        biginteger<_Base>::signed_sub(
            result,
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
    }

    template<size_t _Base>
    void dec(biginteger<_Base>& result) {
        --result;
    }

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        void mul(
            biginteger<_Base>& result,
            const T& lhs,
            const U& rhs
        ) {
        using unsigned_traits = unsigned_biginteger<_Base>;
        auto l = get_virtual_biginteger<_Base>(lhs);
        auto r = get_virtual_biginteger<_Base>(rhs);
        unsigned_traits::mul(result.ubint, l.get_unsigned(), r.get_unsigned());
        result.set_signal(!(l.signal() ^ r.signal()));
    }

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        bool operator==(const T& lhs, const U& rhs) {
        return biginteger<_Base>::equal(
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
    }
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        bool operator!=(const T& lhs, const U& rhs) {
        return !(lhs == rhs);
    }
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        bool operator<(const T& lhs, const U& rhs) {
        return biginteger<_Base>::less(
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
    }
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        bool operator<=(const T& lhs, const U& rhs) {
        return biginteger<_Base>::less_eq(
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
    }
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        bool operator>(const T& lhs, const U& rhs) {
        return rhs < lhs;
    }
    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        bool operator>=(const T& lhs, const U& rhs) {
        return rhs <= lhs;
    }

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        biginteger<_Base> operator+(T&& lhs, U&& rhs) {
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
        biginteger<_Base> operator-(T&& lhs, U&& rhs) {
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
        biginteger<_Base> operator*(T&& lhs, U&& rhs) {
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
        biginteger<_Base> operator/(T&& lhs, U&& rhs) {
        biginteger<_Base> result(std::forward<T>(lhs));
        result /= std::forward<U>(rhs);
        return result;
    }

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        biginteger<_Base> operator%(T&& lhs, U&& rhs) {
        biginteger<_Base> result(std::forward<T>(lhs));
        result %= std::forward<U>(rhs);
        return result;
    }

    VIRTUAL_BIGINTEGER_SINGLE_AUTO_DEFINITION
        bool operator!(T&& x) {
        if constexpr (std::is_integral_v<T>) {
            return !x;
        }
        else {
            return std::forward<T>(x).zero();
        }
    }

    template<size_t _Base>
    void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs) {
        WASSERT_LEVEL_1(std::addressof(lhs) != std::addressof(rhs));
        biginteger<_Base>::quick_divmod(lhs, rhs);
    }

    template<size_t _Base>
    biginteger<_Base> pow(const biginteger<_Base>& a, size_t n) {
        return biginteger<_Base>(a._Signal || (!(n & 1)), unsigned_biginteger<_Base>::pow(a.ubint, n));
    }

    template<size_t _Base>
    biginteger<_Base> pow(biginteger<_Base>&& a, size_t n) {
        a._Signal = a._Signal || (!(n & 1));
        a.ubint.pow(n);
        return std::move(a);
    }

    template<size_t toBase, typename T, size_t fromBase,
        std::enable_if_t<_Is_biginteger_base_masks_v<fromBase, T>, int> >
        biginteger<toBase> base_conversion(T&& v) {
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
    void random_biginteger(biginteger<_Base>& result, size_t n, bool _unsigned, bool exact) {
        using traits = biginteger_traits<_Base>;
        using value_type = typename traits::value_type;
        USE_THREAD_LOCAL static std::random_device rd;
#if defined(WJR_BIGINTEGER_USE_X64)
        USE_THREAD_LOCAL static std::mt19937_64 mt_rand(rd());
#else
        USE_THREAD_LOCAL static std::mt19937 mt_rand(rd());
#endif
        if (!n)return;
        if (!_unsigned) {
            result._Signal = mt_rand() & 1;
        }
        size_t k = (n - 1) / traits::bit_length + 1;
        size_t r = (n - 1) % traits::bit_length + 1;
        std::uniform_int_distribution<value_type> r1(0, traits::_max);
        result.get_vec().resize(k);
        for (size_t i = 0; i < k - 1; ++i) {
            result.get_vec()[i] = r1(mt_rand);
        }
        value_type _max = traits::_max;
        if (r != traits::bit_length)_max = traits::get_base_power(r);
        std::uniform_int_distribution<value_type> r2(exact, _max);
        result.get_vec()[k - 1] = r2(mt_rand);
        result.maintain();
    }

    template<size_t _Base>
    biginteger<_Base> random_biginteger(size_t n, bool _unsigned, bool exact) {
        biginteger<_Base> result;
        random_biginteger(result, n, _unsigned, exact);
        return result;
    }

    template<size_t _Base, typename T, std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> >
    void random_biginteger_max(biginteger<_Base>& result, const T& _Max) {
        using traits = biginteger_traits<_Base>;
        using value_type = typename traits::value_type;
        USE_THREAD_LOCAL static std::random_device rd;
#if defined(WJR_BIGINTEGER_USE_X64)
        USE_THREAD_LOCAL static std::mt19937_64 mt_rand(rd());
#else
        USE_THREAD_LOCAL static std::mt19937 mt_rand(rd());
#endif
        auto view = get_virtual_biginteger<_Base>(_Max);
        WASSERT_LEVEL_1(view.signal() == true);
        size_t n = view.size();
        auto p = view.data();
        auto _Head_val = p[n - 1];
        std::uniform_int_distribution<value_type> head(0, p[n - 1]);
        std::uniform_int_distribution<value_type> tail(0, traits::_max);
        result.get_vec().resize(n);
        auto rep = result.data();
        while (true) {
            value_type val = head(mt_rand);
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
        void random_biginteger_min_max(biginteger<_Base>& result, const T& _Min, const U& _Max) {
        random_biginteger_max(result, _Max - _Min);
        result += _Min;
    }

    template<size_t _Base, typename T,
        std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T>, int> >
        biginteger<_Base> random_biginteger_max(const T& _Max) {
        biginteger<_Base> result;
        random_biginteger_max(result, _Max);
        return result;
    }

    template<size_t _Base, typename T, typename U,
        std::enable_if_t<_Is_biginteger_base_masks_v<_Base, T, U>, int> >
        biginteger<_Base> random_biginteger_min_max(const T& _Min, const U& _Max) {
        biginteger<_Base> result;
        random_biginteger_min_max(result, _Min, _Max);
        return result;
    }

    template<size_t _Base>
    void gcd(biginteger<_Base>& result, biginteger<_Base> a, biginteger<_Base> b) {
        if constexpr (_Base == 2) {
            size_t ct = 0;
            while (true) {
                if (!a) {
                    result = std::move(b.mul_base_power(ct));
                    return;
                }
                if (!b) {
                    result = std::move(a.mul_base_power(ct));
                    return;
                }
                size_t p = 0;
                size_t q = 0;
                for (auto i = a.cbegin(); i != a.cend() && !(*i); ++i, ++p);
                for (auto i = b.cbegin(); i != b.cend() && !(*i); ++i, ++q);
                auto l = std::min(p, q);
                ct += l;
                p -= l;
                q -= l;
                a.div_base_power(p);
                b.div_base_power(q);
                size_t n = a.size();
                size_t m = b.size();
                if (n < m) {
                    std::swap(n, m);
                    std::swap(a, b);
                }
                // m < sqrt(n * n - nlog2n)
                if (n - m >= 6) {
                    a %= b;
                }
                else {
                    if (a >= b) {
                        a -= b;
                    }
                    else {
                        b -= a;
                    }
                }
            }
        }
        else {
            while (!b.zero()) {
                a %= b;
                std::swap(a, b);
            }
            result = std::move(a);
        }
    }

    template<size_t _Base>
    biginteger<_Base> gcd(biginteger<_Base> a, biginteger<_Base> b) {
        biginteger<_Base> result;
        gcd(result, std::move(a), std::move(b));
        return result;
    }

}

#endif
