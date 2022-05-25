#ifndef __WJR_BIGINTEGER_H
#define __WJR_BIGINTEGER_H

#include <emmintrin.h>

#include "mallocator.h"
#include "mString.h"
#include "mtool.h"
#include "masm.h"

extern "C" {
    extern void cdft(int, int, double*, int*, double*);
}

namespace wjr {

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

    static biginteger_basic_twice_value_type operator/(biginteger_basic_twice_value_type n,
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

    template<size_t base>
    struct biginteger_traits {
        using value_type = biginteger_basic_value_type;
        using twice_value_type = biginteger_basic_twice_value_type;
        constexpr static bool is_power_of_two = (base & (base - 1)) == 0;
        constexpr static size_t cqlog2_base = cqlog2(base);
        constexpr static size_t __get_bit_length() {
            if constexpr (is_power_of_two && (biginteger_basic_bit % cqlog2_base == 0)) {
                return biginteger_basic_bit;
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

        enum class mode {
            is_full = 0,
            is_le_mid = 1,
            is_mt_mid = 2
        };

        constexpr static mode __get_mode() {
            if constexpr (_max == biginteger_integral_max_value<value_type>) {
                return mode::is_full;
            }
            else if constexpr (_max <= (biginteger_integral_max_value<value_type> / 2)) {
                return mode::is_le_mid;
            }
            else return mode::is_mt_mid;
        }

        constexpr static mode get_mode = __get_mode();

        template<typename T>
        constexpr static value_type get_low(T x) {
            if constexpr (biginteger_integral_max_value<T> <= _max || get_mode == mode::is_full) {
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
                if constexpr (get_mode == mode::is_full) {
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
#if !defined(WJR_BIGINTEGER_USE_X64) && (defined(_MSC_VER) || defined(__ICC__))
            return __emulu(a, b);
#else
            return static_cast<twice_value_type>(a) * b;
#endif
        }

        static unsigned char quick_add(unsigned char jw, value_type lhs, value_type rhs, value_type* result) {
#if defined(WJR_BIGINTEGER_USE_X64)
            jw = wjr_addcarry_u64(jw, lhs, rhs, result);
#else
            jw = wjr_addcarry_u32(jw, lhs, rhs, result);
#endif
            if constexpr (get_mode == mode::is_full) {
                return jw;
            }
            else {
                jw |= _mod <= *result;
                return jw ? (*result -= _mod, 1) : 0;
            }
        }

        static unsigned char quick_sub(
            unsigned char jw, value_type lhs, value_type rhs, value_type* result) {
#if defined(WJR_BIGINTEGER_USE_X64)
            jw = wjr_subborrow_u64(jw, lhs, rhs, result);
#else
            jw = wjr_subborrow_u32(jw, lhs, rhs, result);
#endif
            if constexpr (get_mode == mode::is_full) {
                return jw;
            }
            else {
                return jw ? (*result += _mod, 1) : 0;
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
        constexpr static value_type get_base_power(size_t kth) {
            WASSERT_LEVEL_2(kth < bit_length);
            return static_base_power_array.arr[kth];
        }
        constexpr static value_type quick_mul_base_power(value_type val, size_t kth) {
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
        constexpr static value_type quick_div_base_power(value_type val, size_t kth) {
            WASSERT_LEVEL_2(kth < bit_length);
            if constexpr (is_power_of_two) {
                return val >> (cqlog2_base * kth);
            }
            else {
                return val / static_magic_divider.magic_divider[kth];
            }
        }
        constexpr static value_type quick_mod_base_power(value_type val, size_t kth) {
            WASSERT_LEVEL_2(kth < bit_length);
            if constexpr (is_power_of_two) {
                return val & (((value_type)(1) << (cqlog2_base * kth)) - 1);
            }
            else {
                return val - quick_div_base_power(val, kth) * get_base_power(kth);
            }
        }
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

    template<size_t base, size_t length>
    class virtual_unsigned_biginteger_reference;

    template<size_t base, size_t length>
    class virtual_unsigned_biginteger_const_reference {
        using traits = biginteger_traits<base>;
        template<size_t _Base, size_t _Length>
        friend class virtual_unsigned_biginteger_reference;
    public:
        using value_type = typename traits::value_type;
        constexpr virtual_unsigned_biginteger_const_reference(
            const value_type* _Data, size_t _Size, size_t _K, size_t _Index
        ) : _Data(const_cast<value_type*>(_Data)), _Size(_Size), _Index(_Index), _K(_K) {}
        constexpr size_t pos1()const { return _Index / traits::bit_length; }
        constexpr size_t pos2()const { return _Index % traits::bit_length; }
        constexpr virtual_unsigned_biginteger_const_reference(
            const virtual_unsigned_biginteger_const_reference&) = default;
        constexpr virtual_unsigned_biginteger_const_reference& operator=(
            const virtual_unsigned_biginteger_const_reference&) = default;
        constexpr operator value_type() const {
            auto p = _Data;
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            value_type low = traits::quick_div_base_power(p[_Pos1], _Pos2);
            size_t low_l = traits::bit_length - _Pos2;
            if (_K <= low_l || _Pos1 == _Size - 1) {
                return get_mod(low);
            }
            size_t high_l = _K - low_l;
            value_type high = traits::quick_mod_base_power(p[_Pos1 + 1], high_l);
            return traits::quick_mul_base_power(high, low_l) + low;
        }
    private:
        constexpr value_type get_mod(value_type val) const {
            if constexpr (traits::get_mode == traits::mode::is_full) {
                return _K != traits::bit_length ? traits::quick_mod_base_power(val, _K)
                    : val;
            }
            else {
                return traits::quick_mod_base_power(val, _K);
            }
        }
        value_type* _Data;
        size_t _Size;
        size_t _Index;
        const size_t _K;
    };

    template<size_t base, size_t length>
    class virtual_unsigned_biginteger_reference
        : public virtual_unsigned_biginteger_const_reference<base, length> {
        using traits = biginteger_traits<base>;
        using _Base = virtual_unsigned_biginteger_const_reference<base, length>;
    public:
        using value_type = typename traits::value_type;
        constexpr virtual_unsigned_biginteger_reference(
            value_type* _Data, size_t _Size, size_t _K, size_t _Index)
            : _Base(_Data, _Size, _K, _Index) {}
        constexpr size_t pos1()const { return _Base::_Index / traits::bit_length; }
        constexpr size_t pos2()const { return _Base::_Index % traits::bit_length; }
        constexpr virtual_unsigned_biginteger_reference(
            const virtual_unsigned_biginteger_reference&) = default;
        constexpr virtual_unsigned_biginteger_reference& operator=(
            const virtual_unsigned_biginteger_reference&) = default;
        constexpr virtual_unsigned_biginteger_reference& operator=(value_type val) {
            WASSERT_LEVEL_1(val >= 0 && val <= traits::_max);
            auto p = _Base::_Data;
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            value_type low = traits::quick_div_base_power(p[_Pos1], _Pos2);
            size_t low_l = traits::bit_length - _Pos2;
            if (_Base::_K <= low_l || _Pos1 == _Base::_Size - 1) {
                p[_Pos1] += traits::quick_mul_base_power((val - _Base::get_mod(low)), _Pos2);
            }
            else {
                size_t high_l = _Base::_K - low_l;
                value_type high = traits::quick_mod_base_power(p[_Pos1 + 1], high_l);
                value_type val_high = traits::quick_div_base_power(val, low_l);
                value_type val_low = val - traits::quick_mul_base_power(val_high, low_l);
                p[_Pos1] += traits::quick_mul_base_power((val_low - low), _Pos2);
                p[_Pos1 + 1] += (val_high - high);
            }
            return *this;
        }
        constexpr virtual_unsigned_biginteger_reference& zero_change(value_type val) {
            WASSERT_LEVEL_1(val >= 0 && val <= traits::_max);
            WASSERT_LEVEL_2((value_type)(*this) == 0);
            auto p = _Base::_Data;
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            size_t low_l = traits::bit_length - _Pos2;
            if (_Base::_K <= low_l || _Pos1 == _Base::_Size - 1) {
                p[_Pos1] += traits::quick_mul_base_power(val, _Pos2);
            }
            else {
                size_t high_l = _Base::_K - low_l;
                value_type val_high = traits::quick_div_base_power(val, low_l);
                value_type val_low = val - traits::quick_mul_base_power(val_high, low_l);
                p[_Pos1] += traits::quick_mul_base_power(val_low, _Pos2);
                p[_Pos1 + 1] += val_high;
            }
            return *this;
        }
    };

    template<size_t base, size_t length>
    class virtual_unsigned_biginteger_const_iterator {
        using traits = biginteger_traits<base>;
    public:
        using value_type = typename traits::value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = virtual_unsigned_biginteger_const_reference<base, length>;
        //using const_reference = virtual_unsigned_biginteger_const_reference<base,length>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;
        constexpr virtual_unsigned_biginteger_const_iterator(
            const value_type* _Data, size_t _Size, size_t _K, size_t _Index = 0)
            : _Data(const_cast<value_type*>(_Data)), _Size(_Size), _Index(_Index), _K(_K) {
            WASSERT_LEVEL_1(vaild());
        }
        constexpr virtual_unsigned_biginteger_const_iterator(
            const virtual_unsigned_biginteger_const_iterator&) = default;
        constexpr virtual_unsigned_biginteger_const_iterator& operator=(
            const virtual_unsigned_biginteger_const_iterator&) = default;
        constexpr size_t pos1()const {
            return _Index / traits::bit_length;
        }
        constexpr size_t pos2()const {
            return _Index % traits::bit_length;
        }
        constexpr bool vaild()const {
            return pos1() < _Size;
        }
        constexpr bool is_last()const {
            return pos1() == _Size - 1;
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
        constexpr reference operator*()const {
            return reference(_Data, _Size, _K, _Index);
        }
        constexpr const_pointer operator->()const {
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
                && lhs._Index == rhs._Index && lhs._K == rhs._K;
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
        size_t _Index = 0;
        const size_t _K;
    };

    template<size_t base, size_t length>
    class virtual_unsigned_biginteger_iterator
        : public virtual_unsigned_biginteger_const_iterator<base, length> {
        using _Base = virtual_unsigned_biginteger_const_iterator<base, length>;
        using traits = biginteger_traits<base>;
    public:
        using value_type = typename traits::value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = virtual_unsigned_biginteger_reference<base, length>;
        //using const_reference = const reference;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;
        constexpr virtual_unsigned_biginteger_iterator(
            value_type* _Data, size_t _Size, size_t _K, size_t _Index = 0)
            : _Base(_Data, _Size, _K, _Index) {}
        constexpr virtual_unsigned_biginteger_iterator(
            const virtual_unsigned_biginteger_iterator&) = default;
        constexpr virtual_unsigned_biginteger_iterator& operator=(
            const virtual_unsigned_biginteger_iterator&) = default;
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
        constexpr reference operator*() {
            return reference(_Base::_Data, _Base::_Size, _Base::_K, _Base::_Index);
        }
        constexpr pointer operator->() {
            return _Base::_Data;
        }
        constexpr friend difference_type operator-(
            const virtual_unsigned_biginteger_iterator& lhs,
            const virtual_unsigned_biginteger_iterator& rhs
            ) {
            WASSERT_LEVEL_1(lhs._K == rhs._K);
            return (rhs.index - lhs.index) / lhs._K;
        }
        constexpr friend bool operator==(
            const virtual_unsigned_biginteger_iterator& lhs,
            const virtual_unsigned_biginteger_iterator& rhs
            ) {
            return lhs._Data == rhs._Data && lhs._Size == rhs._Size
                && lhs._Index == rhs._Index && lhs._K == rhs._K;
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
        constexpr void _Make(T val) {
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
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base, length>;
        using iterator = virtual_unsigned_biginteger_iterator<base, length>;
        template<typename T, std::enable_if_t<_Is_unsigned_integral_v<T>, int> = 0>
        constexpr explicit virtual_unsigned_biginteger(T val) {
            _Make<0>(val);
        }
        constexpr virtual_unsigned_biginteger(const virtual_unsigned_biginteger&) = default;
        constexpr virtual_unsigned_biginteger& operator=(
            const virtual_unsigned_biginteger&) = default;
        constexpr const value_type* data()const { return _Data; }
        constexpr size_t size() const {
            if constexpr (length == 1) {
                return 1;
            }
            else {
                return _Size;
            }
        }
        constexpr const value_type* begin()const { return _Data; }
        constexpr const value_type* end()const { return _Data + _Size; }
        constexpr bool zero()const {
            if constexpr (length == 1) {
                return _Data[0] == 0;
            }
            else {
                return _Size == 1 && _Data[0] == 0;
            }
        }

        constexpr const_iterator begin(size_t k)const {
            return const_iterator(_Data, _Size, k);
        }
        constexpr const_iterator cbegin(size_t k)const {
            return const_iterator(_Data, _Size, k);
        }
        template<typename T>
        constexpr T to()const {
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
        constexpr explicit operator T()const {
            return to<T>();
        }
        constexpr virtual_unsigned_biginteger& maintain() {
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
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base, 0>;
        using iterator = virtual_unsigned_biginteger_iterator<base, 0>;
        constexpr virtual_unsigned_biginteger(
            const value_type* _Data, size_t _Size)
            : _Data(_Data), _Size(_Size) {}
        constexpr virtual_unsigned_biginteger(const virtual_unsigned_biginteger&) = default;
        constexpr explicit virtual_unsigned_biginteger(const unsigned_biginteger<base>& rhs)
            : _Data(rhs.data()), _Size(rhs.size()) {}
        constexpr virtual_unsigned_biginteger& operator=(
            const virtual_unsigned_biginteger&) = default;
        constexpr const value_type* data()const {
            WASSERT_LEVEL_1(_Size != 0);
            return _Data;
        }
        constexpr size_t size()const { return _Size; }
        constexpr const value_type* begin()const {
            return data();
        }
        constexpr const value_type* end()const { return begin() + _Size; }
        constexpr bool zero()const {
            WASSERT_LEVEL_1(_Size != 0);
            return _Size == 1 && _Data[0] == 0;
        }
        constexpr const_iterator begin(size_t k)const {
            return const_iterator(data(), size(), k);
        }
        constexpr const_iterator cbegin(size_t k)const {
            return const_iterator(data(), size(), k);
        }
        template<typename T>
        constexpr T to()const {
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
        constexpr explicit operator T() const {
            return to<T>();
        }
        constexpr virtual_unsigned_biginteger& maintain() {
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
            if (rt.size() == 1) {							        \
                return FUNC<1>(__VA_ARGS__);				        \
            }												        \
            else {											        \
                return FUNC<2>(__VA_ARGS__);				        \
            }												        \
        }													        \
        else if constexpr (index == 3) {					        \
            switch (rt.size()) {							        \
            case 1: return FUNC<1>(__VA_ARGS__);			        \
            case 2: return FUNC<2>(__VA_ARGS__);			        \
            case 3: return FUNC<3>(__VA_ARGS__);			        \
            }												        \
        }													        \
        else {												        \
            switch (rt.size()) {							        \
            case 1: return FUNC<1>(__VA_ARGS__);			        \
            case 2: return FUNC<2>(__VA_ARGS__);			        \
            case 3: return FUNC<3>(__VA_ARGS__);			        \
            case 4: return FUNC<4>(__VA_ARGS__);			        \
            default:return FUNC<0>(__VA_ARGS__);			        \
            }												        \
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
        return _Dst != _Src ? memcpy(_Dst, _Src, _Size) : _Dst;
    }

    /*
    // just for biginteger and unsigned biginteger
    // continuous memory, but it can be expanded back and forth
    class biginteger_vector {
    private:
        constexpr static size_t __reserved_size = 128 / biginteger_basic_bit;
        constexpr static size_t __growth_rate   = 2;
        constexpr static size_t __forward_rate  = 2;

    public:
        using value_type = biginteger_basic_value_type;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using allocator_type = mallocator<value_type>;

    private:
        struct __Vector_val {
            __Vector_val()
                : _Mydata(allocator_type().allocate(__reserved_size)),
                _Myfirst(_Mydata),_Mylast(_Myfirst + __reserved_size),_Myend(_Mydata + __reserved_size) {
            }
            __Vector_val(size_t _Count)
                : _Mydata(allocator_type().allocate(_Count)),
                _Myfirst(_Mydata),_Mylast(_Myfirst + _Count),_Myend(_Mydata + _Count) {
            }
            __Vector_val(const __Vector_val& other) = default;
            void set_nullptr() {
                _Mydata = nullptr;
                _Myfirst = nullptr;
                _Mylast = nullptr;
                _Myend = nullptr;
            }
            value_type* _Mydata;    // real data
            value_type* _Myfirst;   // point to the first element
            value_type* _Mylast;    // point to the last element
            value_type* _Myend;     // point to the end of the data
        };
    public:

        // default
        // set zero
        biginteger_vector() : _Myval()  {
            *_Myval._Myfirst = 0;
        }
        explicit biginteger_vector(size_t _Count) : _Myval(_Count) {
            memset(_Myval._Myfirst, 0, sizeof(value_type) * _Count);
        }
        biginteger_vector(size_t _Count, const value_type& _Val) : _Myval(_Count) {
            std::fill_n(_Myval._Mydata, _Count, _Val);
        }
        biginteger_vector(const biginteger_vector& other) : _Myval(other.size()) {
            memcpy(_Myval._Mydata, other._Myval._Mydata, sizeof(value_type) * other.size());
        }
        biginteger_vector(biginteger_vector&& other) noexcept : _Myval(other._Myval) {
            other._Myval.set_nullptr();
        }

        biginteger_vector& operator=(const biginteger_vector& other) {
            if (this == std::addressof(other)) {
                return *this;
            }
            size_t _Oldcapacity = capacity();
            if (_Oldcapacity  >= other.size()) {
                size_t _Newhead = _Forward_reserve(_Oldcapacity, other.size());
                _Copy_to_original_place(other.data(), other.size(), _Newhead);
            }
            else {
                size_t _Newcapacity = _Calculate_Growth(other.size());
                size_t _Newhead = _Forward_reserve(_Newcapacity, other.size());
                _Copy_to_new_place(other.data(), other.size(), _Newcapacity, _Newhead);
            }
            return *this;
        }
        biginteger_vector& operator=(biginteger_vector&& other) noexcept {
            if (this == std::addressof(other)) {
                return *this;
            }
            _Tidy();
            _Myval = other._Myval;
            other._Myval.set_nullptr();
            return *this;
        }

        ~biginteger_vector() {
            _Tidy();
        }

        template<typename _Ty>
        void emplace_back(_Ty&& _Val) {
            if (_Myval._Mylast == _Myval._Myend) {
                size_t _Oldcapacity = capacity();
                size_t _Oldsize = size();
                if (_Oldcapacity >= _Oldsize + (_Oldsize / 2)) {
                    size_t _Newhead = _Forward_reserve(_Oldcapacity,_Oldsize);
                    _Copy_to_original_place(data(), _Oldsize, _Newhead);
                }
                else {
                    size_t _Newcapacity = _Calculate_Growth(_Oldsize + 1);
                    size_t _Newhead = _Forward_reserve(_Newcapacity, _Oldsize + 1);
                    _Copy_to_new_place(data(), _Oldsize, _Newcapacity, _Newhead);
                }
            }
            *_Myval._Mylast = std::forward<_Ty>(_Val);
            ++_Myval._Mylast;
        }

        void push_back(const value_type& _Val) {
            emplace_back(_Val);
        }

        void push_back(value_type&& _Val) {
            emplace_back(std::move(_Val));
        }

        template<typename iter,std::enable_if_t<wjr_is_iterator_v<iter>,int> = 0>
        void assign(iter _First,iter _Last) {
            if constexpr (std::is_same_v<typename std::iterator_traits<iter>::iterator_category,
                std::input_iterator_tag>) {
                pointer _Myfirst = _Myval._Myfirst;
                for(;_First != _Last && _Myfirst != _Myval._Mylast; ++_First, ++_Myfirst) {
                    *_Myfirst = *_First;
                }
                _Myval._Mylast = _Myfirst;
                for (; _First != _Last; ++_First) {
                    emplace_back(*_First);
                }
            }
            else {
                size_t _Count = std::distance(_First, _Last);
                if (_Count > capacity()) {
                    size_t _Newcapacity = _Calculate_Growth(_Count);
                    size_t _Newhead = _Forward_reserve(_Newcapacity, _Count);
                    _Copy_to_new_place(_First, _Count, _Newcapacity, _Newhead);
                }
                else {
                    size_t _Newhead = _Forward_reserve(capacity(), _Count);
                    _Copy_to_original_place(_First, _Count, _Newhead);
                }
            }
        }

        void reserve(size_t _Newcapacity) {
            if (_Newcapacity > capacity()) {
                size_t _Newhead = _Forward_reserve(_Newcapacity, size());
                _Copy_to_new_place(data(), size(), _Newcapacity, _Newhead);
            }
        }

        void resize(size_t _Newsize, value_type _Val = 0) {
            size_t _Oldsize = size();
            if (_Oldsize < _Newsize) {
                _Myval._Mylast = _Myval._Myfirst + _Newsize;
            }
            if (_Oldsize > _Newsize) {

                if (_Newsize > _Oldcapacity) {
                    size_t _Newcapacity = _Calculate_Growth(_Newsize);
                    size_t _Newhead = _Forward_reserve(_Newcapacity, _Newsize);
                    _Copy_to_new_place(data(), _Oldsize, _Newcapacity, _Newhead);
                    memset(_Myval._Mylast, 0, sizeof(value_type) * _Newsize);
                    _Myval._Mylast = _Myval._Myfirst + _Newsize;
                }
                else {

                }
            }

        }

        pointer begin() {
            return _Myval._Myfirst;
        }
        const_pointer begin() const {
            return _Myval._Myfirst;
        }
        const_pointer cbegin()const {
            return _Myval._Myfirst;
        }
        pointer end() {
            return _Myval._Mylast;
        }
        const_pointer end() const {
            return _Myval._Mylast;
        }
        const_pointer cend() const {
            return _Myval._Mylast;
        }
        pointer data() {
            return _Myval._Myfirst;
        }
        const_pointer data() const {
            return _Myval._Myfirst;
        }
        pointer real_begin() {
            return _Myval._Mydata;
        }
        const_pointer real_begin() const {
            return _Myval._Mydata;
        }
        pointer real_end() {
            return _Myval._Myend;
        }
        const_pointer real_end()const {
            return _Myval._Myend;
        }
        size_t size()const {
            return end() - begin();
        }
        size_t capacity() const {
            return real_end() - real_begin();
        }
        size_t unused_capacity()const {
            return capacity() - size();
        }
        size_t unused_front_capacity()const {
            return begin() - real_begin();
        }
        size_t unused_back_capacity()const {
            return real_end() - end();
        }

    private:
        void _Tidy() {
            if (_Myval._Mydata != nullptr) {
                allocator_type().deallocate(_Myval._Mydata, capacity());
                _Myval.set_nullptr();
            }
        }
        size_t _Calculate_Growth(size_t _Newsize) const{
            size_t _Oldcapacity = capacity();
            size_t _Newcapacity = _Oldcapacity + _Oldcapacity / __growth_rate;
            return _Newcapacity >= _Newsize ? _Newcapacity : _Newsize;
        }
        static size_t _Forward_reserve(size_t C, size_t S) {
            return (C - S) / (__forward_rate + 1);
        }
        template<typename iter>
        void _Copy_to_new_place(iter _First,size_t _Size,size_t _Newcapacity,size_t _Newhead) {
            pointer _Newdata = allocator_type().allocate(_Newcapacity);
            std::copy_n(_First, _Size, _Newdata + _Newhead);
            _Tidy();
            _Myval._Mydata = _Newdata;
            _Myval._Myfirst = _Newdata + _Newhead;
            _Myval._Mylast = _Newdata + _Size;
            _Myval._Myend = _Newdata + _Newcapacity;
        }
        template<typename iter>
        void _Copy_to_original_place(iter _First, size_t _Size, size_t _Newhead) {
            std::copy_n(_First, _Size, _Myval._Mydata + _Newhead);
            _Myval._Myfirst = _Myval._Mydata + _Newhead;
            _Myval._Mylast = _Myval._Mydata + _Size;
        }
        void _Copy_place(size_t _Backwardsize) {
            size_t _Backwardcapacity = _Myval._Myend - _Myval._Myfirst;
            if (_Backwardcapacity < _Backwardsize) {

            }
        }

        __Vector_val _Myval;
    };
    */

    template<size_t fromBase, size_t toBase>
    struct _Quick_base_conversion {
        constexpr static bool value = (fromBase != toBase) &&
            (biginteger_traits<fromBase>::_max == biginteger_traits<toBase>::_max);
    };

    template<size_t fromBase, size_t toBase>
    constexpr static bool _Quick_base_conversion_v = _Quick_base_conversion<fromBase, toBase>::value;

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
        unsigned_biginteger() : vec(1, 0) {}
        unsigned_biginteger(const unsigned_biginteger& other) = default;
        unsigned_biginteger(unsigned_biginteger&& other) noexcept = default;
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_v<fromBase, base>, int> = 0>
        explicit unsigned_biginteger(const unsigned_biginteger<fromBase>& other)
            : vec(other.vec) {}
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_v<fromBase, base>, int> = 0>
        explicit unsigned_biginteger(unsigned_biginteger<fromBase>&& other)
            : vec(std::move(other.vec)) {}
        template<size_t length>
        explicit unsigned_biginteger(const virtual_unsigned_biginteger<length>& rhs)
            : vec(rhs.begin(), rhs.end()) {
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
            if (unlikely(first == last)) {
                set_zero();
                return;
            }
            auto n = last - first;
            vec.resize((n - 1) / traits::bit_length + 1);
            virtual_unsigned_biginteger_iterator<base, 0>
                write(data(), size(), 1, n);
            while (first != last) {
                *(--write) = (std::forward<Func>(fn))(*first);
                ++first;
            }
            maintain();
        }
        unsigned_biginteger(const value_type* _Data, size_t _Size)
            : vec(_Data, _Data + _Size) {}
        explicit unsigned_biginteger(const String_view& str) {
            size_t n = str.size();
            if (!n) {
                set_zero();
                return;
            }
            size_t _Size = (n - 1) / traits::bit_length + 1;
            vec.resize(_Size);
            virtual_unsigned_biginteger_iterator<base, 0>
                write(data(), size(), 1, n);
            for (auto i : str) {
                *(--write) = i - '0';
            }
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

        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_v<fromBase, base>, int> = 0>
        unsigned_biginteger& operator=(const unsigned_biginteger<fromBase>& other) {
            vec = other.vec;
            return *this;
        }

        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_v<fromBase, base>, int> = 0>
        unsigned_biginteger& operator=(unsigned_biginteger<fromBase>&& other) {
            vec = std::move(other.vec);
            return *this;
        }

        unsigned_biginteger& operator=(const String_view& str) {
            size_t n = str.size();
            if (!n) {
                set_zero();
                return *this;
            }
            size_t _Size = (n - 1) / traits::bit_length + 1;
            vec.resize(_Size);
            virtual_unsigned_biginteger_iterator<base, 0>
                write(data(), size(), 1, n);
            for (auto i : str) {
                *(--write) = i - '0';
            }
            return *this;
        }

        template<typename iter, typename Func>
        unsigned_biginteger& assign(iter first, iter last, Func fn) {
            WASSERT_LEVEL_2(first <= last);
            if (first == last) {
                set_zero();
                return *this;
            }
            auto n = last - first;
            size_t k = (n - 1) / traits::bit_length + 1;
            vec.clear();
            vec.resize(k);
            virtual_unsigned_biginteger_iterator<base, 0>
                write(get_virtual_unsigned_biginteger<base>(*this), 1, n);
            while (first != last) {
                *(--write) = fn(*first);
                ++first;
            }
            maintain();
            return *this;
        }

        unsigned_biginteger& assign(const value_type* _Data, size_t _Size) {
            vec.assign(_Data, _Data + _Size);
            return *this;
        }

        void swap(unsigned_biginteger& other) noexcept {
            vec.swap(other.vec);
        }

        value_type* data() { return vec.data(); }
        const value_type* data()const { return vec.data(); }
        size_t size()const { return vec.size(); }
        bool zero()const { return size() == 1 && *data() == 0; }
        void set_zero() { this->vec.clear(); this->vec.push_back(0); }

        unsigned_biginteger& quick_mul_base_power(size_t index) {
            if (!index || zero())return *this;
            size_t n = size();
            size_t k = index / traits::bit_length;
            size_t r = index % traits::bit_length;
            if (!r) {
                vec.insert(vec.begin(), k, 0);
            }
            else {
                size_t br = traits::bit_length - r;
                value_type mod_tag = traits::get_base_power(br);
                size_t delta = k + (vec[n - 1] >= mod_tag);
                vec.resize(n + delta);
                auto p = data();
                value_type _High = 0;
                value_type _Low;
                if (p[n - 1] >= mod_tag) {
                    p[n + delta - 1] = _High = traits::quick_div_base_power(p[n - 1], br);
                    --delta;
                }
                for (size_t i = n - 1; i; --i) {
                    _Low = p[i] - traits::quick_mul_base_power(_High, br);
                    value_type _Nxt_high = traits::quick_div_base_power(p[i - 1], br);
                    p[i + delta] = traits::quick_mul_base_power(_Low, r) + _Nxt_high;
                    _High = _Nxt_high;
                }
                _Low = p[0] - traits::quick_mul_base_power(_High, br);
                p[delta] = traits::quick_mul_base_power(_Low, r);
                memset(p, 0, sizeof(value_type) * delta);
            }
            return *this;
        }

        unsigned_biginteger& quick_divide_base_power(size_t index) {
            if (!index || zero())return *this;
            size_t n = size();
            size_t k = index / traits::bit_length;
            size_t r = index % traits::bit_length;
            if (k >= n) {
                set_zero();
                return *this;
            }
            vec.erase(vec.begin(), vec.begin() + k);
            n = size();
            if (r) {
                value_type mod_tag = traits::get_base_power(r);
                value_type mul_tag = traits::get_base_power(traits::bit_length - r);
                auto p = data();
                for (size_t i = 0; i != n - 1; ++i) {
                    p[i] = traits::quick_div_base_power(p[i], r) +
                        ((p[i + 1] - traits::quick_div_base_power(p[i + 1], r) * mod_tag) * mul_tag);
                }
                p[n - 1] = traits::quick_div_base_power(p[n - 1], r);
                while (n && !p[n - 1])--n;
                vec.resize(n);
            }
            return *this;
        }

        void maintain() {
            size_t n = size();
            auto p = data();
            while (n != 1 && !p[n - 1])--n;
            vec.resize(n);
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

        template<size_t index, size_t il, size_t ir>
        static void slow_mul(
            unsigned_biginteger& result,
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t ir>
        static void pos_add(
            unsigned_biginteger& result,
            size_t pos,
            const virtual_unsigned_biginteger<ir>& rhs
        );

        template<size_t il, size_t ir>
        static void karatsuba(
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

        std::vector<value_type, mallocator<value_type>> vec;
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
            for (; n--;) {
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
            else if constexpr (index == 3) {
                return p[0] == q[0] && p[1] == q[1] && p[2] == q[2];
            }
            else {
                return p[0] == q[0] && p[1] == q[1] && p[2] == q[2] && p[3] == q[3];
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
        }else if constexpr (index == 3){						\
            return p[2] != q[2] ? p[2] op q[2] : p[1] != q[1] ?	\
                p[1] op q[1] : p[0] op q[0];					\
        }else {													\
            return p[3] != q[3] ? p[3] op q[3] : p[2] != q[2] ?	\
                p[2] op q[2] : p[1] != q[1] ? p[1] op q[1] :	\
                p[0] op q[0];									\
        }														\
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
        auto lp = lhs.data();
        auto rp = rhs.data();
        auto res = result.size();
        auto ls = lhs.size();
        auto rs = rhs.size();
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
                cf = traits::quick_add(0, lp[0], rp[0], &rep[0]);
            }
            if constexpr (index >= 2) {
                cf = traits::quick_add(cf, lp[1], rp[1], &rep[1]);
            }
            if constexpr (index >= 3) {
                cf = traits::quick_add(cf, lp[2], rp[2], &rep[2]);
            }
            if constexpr (index >= 4) {
                cf = traits::quick_add(cf, lp[3], rp[3], &rep[3]);
            }
        }
        else {
            size_t i = 0;
            switch (rs & 3) {
            case 3:
                cf = traits::quick_add(cf, lp[i], rp[i], &rep[i]);
                ++i;
            case 2:
                cf = traits::quick_add(cf, lp[i], rp[i], &rep[i]);
                ++i;
            case 1:
                cf = traits::quick_add(cf, lp[i], rp[i], &rep[i]);
                ++i;
            default:
                while (i != rs) {
                    cf = traits::quick_add(cf, lp[i], rp[i], &rep[i]);
                    cf = traits::quick_add(cf, lp[i + 1], rp[i + 1], &rep[i + 1]);
                    cf = traits::quick_add(cf, lp[i + 2], rp[i + 2], &rep[i + 2]);
                    cf = traits::quick_add(cf, lp[i + 3], rp[i + 3], &rep[i + 3]);
                    i += 4;
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
            biginteger_ememcpy(rep + rs, lp + rs, sizeof(value_type) * (ls - rs));
            result.vec.resize(ls);
        }
        else {
            biginteger_ememcpy(rep + rs, lp + rs, sizeof(value_type) * (res - rs));
            result.vec.insert(result.vec.end(), lp + res, lp + ls);
            rep = result.data();
        }
        if (cf) {
            size_t pos = rs;
            for (; pos != ls && rep[pos] == traits::_max; ++pos)rep[pos] = 0;
            if (pos == ls)result.vec.push_back(1);
            else ++rep[pos];
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
                cf = traits::quick_sub(0, lp[0], rp[0], &rep[0]);
            }
            if constexpr (index >= 2) {
                cf = traits::quick_sub(cf, lp[1], rp[1], &rep[1]);
            }
            if constexpr (index >= 3) {
                cf = traits::quick_sub(cf, lp[2], rp[2], &rep[2]);
            }
            if constexpr (index >= 4) {
                cf = traits::quick_sub(cf, lp[3], rp[3], &rep[3]);
            }
        }
        else {
            size_t i = 0;
            switch (rs & 3) {
            case 3:
                cf = traits::quick_sub(cf, lp[i], rp[i], &rep[i]);
                ++i;
            case 2:
                cf = traits::quick_sub(cf, lp[i], rp[i], &rep[i]);
                ++i;
            case 1:
                cf = traits::quick_sub(cf, lp[i], rp[i], &rep[i]);
                ++i;
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
        if (ls < res) {
            biginteger_ememcpy(rep + rs, lp + rs, sizeof(value_type) * (ls - rs));
            result.vec.resize(ls);
        }
        else {
            biginteger_ememcpy(rep + rs, lp + rs, sizeof(value_type) * (res - rs));
            result.vec.insert(result.vec.end(), lp + res, lp + ls);
            rep = result.data();
        }
        if (cf) {
            size_t pos = rs;
#if WDEBUG_LEVEL >= 2
            for (; pos != ls && rep[pos] == 0; ++pos)rep[pos] = traits::_max;
            WASSERT_LEVEL_2(pos != ls);
#else
            for (; rep[pos] == 0; ++pos)rep[pos] = traits::_max;
#endif
            --rep[pos];
        }
        while (ls != 1 && !rep[ls - 1])--ls;
        result.vec.resize(ls);
    }

    template<size_t base>
    template<size_t index, size_t il, size_t ir>
    void unsigned_biginteger<base>::slow_mul(
        unsigned_biginteger& result,
        const virtual_unsigned_biginteger<il>& lhs,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
        constexpr static size_t cache_size = 256;
        USE_THREAD_LOCAL static twice_value_type static_array[cache_size];

        auto lp = lhs.data();
        auto rp = rhs.data();
        auto res = result.size();
        auto ls = lhs.size();
        auto rs = rhs.size();
        auto len = ls + rs;
        twice_value_type* temp_array;
        if constexpr (index != 1) {
            if (len <= cache_size) temp_array = static_array;
            else temp_array = new twice_value_type[len];
            memset(temp_array, 0, sizeof(twice_value_type) * len);
        }

        if constexpr (index != 0) {
            if constexpr (index == 1) {
                value_type rv = *rp;
                twice_value_type _Val = 0;
                if (res < ls) {
                    result.vec.reserve(ls + 1);
                }
                result.vec.resize(ls);
                auto rep = result.data();
                for (size_t i = 0; i < ls; ++i) {
                    _Val += traits::quick_mul(lp[i], rv);
                    rep[i] = traits::get_low(_Val);
                    _Val = traits::get_high(_Val);
                }
                if (_Val) {
                    WASSERT_LEVEL_2(_Val < traits::max_value);
                    result.vec.push_back(static_cast<value_type>(_Val));
                }
            }
            else {
                if constexpr (index >= 1) {
                    for (size_t i = 0; i < ls; ++i) {
                        auto val = traits::quick_mul(lp[i], rp[0]) + temp_array[i];
                        temp_array[i + 1] += traits::get_high(val);
                        temp_array[i] = traits::get_low(val);
                    }
                }
                if constexpr (index >= 2) {
                    for (size_t i = 0; i < ls; ++i) {
                        auto val = traits::quick_mul(lp[i], rp[1]) + temp_array[i + 1];
                        temp_array[i + 2] += traits::get_high(val);
                        temp_array[i + 1] = traits::get_low(val);
                    }
                }
                if constexpr (index >= 3) {
                    for (size_t i = 0; i < ls; ++i) {
                        auto val = traits::quick_mul(lp[i], rp[2]) + temp_array[i + 2];
                        temp_array[i + 3] += traits::get_high(val);
                        temp_array[i + 2] = traits::get_low(val);
                    }
                }
                if constexpr (index >= 4) {
                    for (size_t i = 0; i < ls; ++i) {
                        auto val = traits::quick_mul(lp[i], rp[3]) + temp_array[i + 3];
                        temp_array[i + 4] += traits::get_high(val);
                        temp_array[i + 3] = traits::get_low(val);
                    }
                }
            }
        }
        else {
            for (size_t j = 0; j < rs; ++j) {
                auto rv = rp[j];
                for (size_t i = 0; i < ls; ++i) {
                    auto val = traits::quick_mul(lp[i], rv) + temp_array[i + j];
                    temp_array[i + j + 1] += traits::get_high(val);
                    temp_array[i + j] = traits::get_low(val);
                }
            }
        }

        if constexpr (index != 1) {
            result.vec.reserve(len);
            result.vec.assign(temp_array, temp_array + len - 1);
            if (temp_array[len - 1]) {
                WASSERT_LEVEL_2(temp_array[len - 1] < traits::max_value);
                result.vec.push_back(static_cast<value_type>(temp_array[len - 1]));
            }
            if (len > cache_size)delete[]temp_array;
        }
    }

    template<size_t base>
    template<size_t ir>
    void unsigned_biginteger<base>::pos_add(
        unsigned_biginteger& result,
        size_t pos,
        const virtual_unsigned_biginteger<ir>& rhs
    ) {
#if WDEBUG_LEVEL >= 2
        auto n = result.size();
#endif
        auto m = rhs.size();
        WASSERT_LEVEL_2(m + pos <= n);
        auto rep = result.data();
        auto rp = rhs.data();
        size_t i = 0;
        unsigned char cf = 0;
        for (; i < m; ++i) {
            cf = traits::quick_add(cf, rep[pos + i], rp[i], &rep[pos + i]);
        }
        i += pos;
        if (cf) {
            for (;
#if WDEBUG_LEVEL >= 2
                i < n &&
#endif
                rep[i] == traits::_max; ++i) {
                rep[i] = 0;
            }
            WASSERT_LEVEL_2(i != n);
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
        pos_add(result, 0, virtual_unsigned_biginteger<0>(C.data(), C.size()).maintain());
        pos_add(result, mid, virtual_unsigned_biginteger<0>(B.data(), B.size()).maintain());
        pos_add(result, 2 * mid, virtual_unsigned_biginteger<0>(A.data(), A.size()).maintain());
        size_t s = n + m;
        auto rep = result.data();
        while (s != 1 && !rep[s - 1])--s;
        result.vec.resize(s);
        /*result = std::move(A);
        result.quick_mul_base_power(mid * traits::bit_length);
        sub(result, result, C);
        C = result;
        add(result, result, B);
        result.quick_mul_base_power(mid * traits::bit_length);
        sub(result, result, C);*/
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
        size_t _i = i;
        size_t _index = index;
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
        pos_add(r, i * l, virtual_unsigned_biginteger<0>(temp.data(), temp.size()).maintain());
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
            WASSERT_LEVEL_1(n >= idx * m);
            for (size_t i = 0; i < idx; ++i) {
                size_t s = i == idx - 1 ? n - i * m : m;
                virtual_unsigned_biginteger<0> g(lhs.data() + i * m, s);
                g.maintain();
                mul(temp, g, rhs);
                pos_add(r, i * m, virtual_unsigned_biginteger<0>(temp.data(), temp.size()));
            }
            result = std::move(r);
            size_t s = n + m;
            auto rep = result.data();
            while (s != 1 && !rep[s - 1])--s;
            result.vec.resize(s);
        }
        /*size_t mid = n >> 1;
        virtual_unsigned_biginteger<0> high(lhs.data() + mid, n - mid);
        virtual_unsigned_biginteger<0> low(lhs.data(), mid);
        low.maintain();
        unsigned_biginteger rhigh, rlow;
        mul(rhigh, high, rhs);
        mul(rlow, low, rhs);
        result = std::move(rhigh);
        result.quick_mul_base_power(mid * traits::bit_length);
        add(result, result, rlow);*/
    }

    struct biginteger_fft_cache {
        biginteger_fft_cache() {
            ip[0] = 0;
        }
        ~biginteger_fft_cache() = default;
        void make(size_t n) {
            if (sz < n) {
                size_t c = sz + sz / 2;
                if (c < n)c = n;
                auto ne = std::make_unique<double[]>(c);
                w = std::move(ne);
                sz = c;
            }
        }
        std::unique_ptr<int[]> ip = std::make_unique<int[]>(1 << 16);
        std::unique_ptr<double[]> w;
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

        auto a = new double[s << 1];
        __biginteger_fft_cache.make((s >> 1) + 4);
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

        cdft(s << 1, 1, a, __biginteger_fft_cache.ip.get(), __biginteger_fft_cache.w.get());
        for (size_t i = 0; i < s; ++i) {
            double sa = a[i << 1];
            double sb = a[i << 1 | 1];
            a[i << 1] = sa * sa - sb * sb;
            a[i << 1 | 1] = 2 * sa * sb;
        }

        cdft(s << 1, -1, a, __biginteger_fft_cache.ip.get(), __biginteger_fft_cache.w.get());
        double invs = 1.0 / static_cast<double>(s);
        for (size_t i = 0; i < s; ++i) {
            a[i << 1] *= invs;
        }

        result.vec.clear();
        result.vec.resize(pn + pm, 0);
        virtual_unsigned_biginteger_iterator<base, 0>
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
        delete[]a;
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

            if (m <= 40) {
                slow_mul<index>(result, lhs, rhs);
            }
            else {
                if (m <= 96) {
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

    // signed biginteger or integral view
    template<size_t base, size_t length>
    class virtual_biginteger : public virtual_unsigned_biginteger<base, length> {
        using _Base = virtual_unsigned_biginteger<base, length>;
    public:
        template<typename T, std::enable_if_t<_Is_unsigned_integral_v<T>, int> = 0>
        constexpr explicit virtual_biginteger(T val)
            : _Base(val) {}
        template<typename T, std::enable_if_t<_Is_signed_integral_v<T>, int> = 0>
        constexpr explicit virtual_biginteger(T val)
            : _Signal(val >= 0),
            _Base(static_cast<std::make_unsigned_t<T>>(val >= 0 ? val : (0 - val))) {}
        constexpr explicit virtual_biginteger(
            const virtual_unsigned_biginteger<base, length>& rhs) : _Base(rhs) {}
        constexpr explicit virtual_biginteger(
            bool _Signal,
            const virtual_unsigned_biginteger<base, length>& rhs) : _Signal(_Signal), _Base(rhs) {
            maintain_signal();
        }
        constexpr virtual_biginteger(const virtual_biginteger&) = default;
        constexpr virtual_biginteger& operator=(const virtual_biginteger&) = default;
        constexpr bool signal()const { return _Signal; }
        constexpr _Base& get_unsigned() { return *this; }
        constexpr const _Base& get_unsigned()const { return *this; }
        template<typename T>
        explicit operator T()const {
            auto _Val = _Base::template to<T>();
            return _Signal ? _Val : (0 - _Val);
        }
        constexpr void maintain_signal() {
            _Signal = _Signal || _Base::zero();
        }
        constexpr void change_signal() {
            _Signal = _Base::zero() || (!_Signal);
        }
        constexpr void abs() {
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
        constexpr virtual_biginteger(value_type* _Data, size_t _Size)
            : _Base(_Data, _Size) {}
        constexpr virtual_biginteger(const value_type* _Data, size_t _Size)
            : _Base(_Data, _Size) {}
        constexpr virtual_biginteger(bool _Signal, value_type* _Data, size_t _Size)
            : _Signal(_Signal), _Base(_Data, _Size) {
            maintain_signal();
        }
        constexpr virtual_biginteger(bool _Signal, const value_type* _Data, size_t _Size)
            : _Signal(_Signal), _Base(_Data, _Size) {
            maintain_signal();
        }
        constexpr explicit virtual_biginteger(
            const virtual_unsigned_biginteger<base, 0>& rhs) : _Base(rhs) {}
        constexpr explicit virtual_biginteger(
            bool _Signal,
            const virtual_unsigned_biginteger<base, 0>& rhs) : _Signal(_Signal), _Base(rhs) {
            maintain_signal();
        }
        constexpr explicit virtual_biginteger(const biginteger<base>& rhs)
            : _Signal(rhs.signal()), _Base(rhs.data(), rhs.size()) {
            // don't need to maintain signal
        }
        constexpr virtual_biginteger(const virtual_biginteger&) = default;
        constexpr virtual_biginteger& operator=(const virtual_biginteger&) = default;
        constexpr bool signal()const {
            WASSERT_LEVEL_1(!(_Base::zero() && !_Signal));
            return _Signal;
        }
        constexpr _Base& get_unsigned() {
            return *this;
        }
        constexpr const _Base& get_unsigned()const {
            return *this;
        }
        template<typename T>
        explicit operator T()const {
            auto _Val = _Base::template to<T>();
            return _Signal ? _Val : (0 - _Val);
        }
        virtual_biginteger& maintain() {
            _Base::maintain();
            maintain_signal();
            return *this;
        }
        void maintain_signal() {
            _Signal = _Signal || _Base::zero();
        }
        void change_signal() {
            _Signal = (!_Signal) || _Base::zero();
        }
        void abs() {
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
    VIRTUAL_BIGINTEGER_SINGLE_AUTO_DECLARATION
        biginteger<_Base> operator<<(T&& lhs, size_t delta);
    VIRTUAL_BIGINTEGER_SINGLE_AUTO_DECLARATION
        biginteger<_Base> operator>>(T&& lhs, size_t delta);

    template<size_t _Base>
    void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);

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
        template<size_t Base>
        friend class biginteger;

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
            _Is_biginteger<T>,
            _Is_virtual_biginteger<T>>;
        template<typename T>
        constexpr static bool _Is_rbint_v = _Is_rbint<T>::value;
    public:
        using value_type = typename traits::value_type;
        using twice_value_type = typename traits::twice_value_type;
        using iterator = virtual_unsigned_biginteger_iterator<base, 0>;
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base, 0>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using reverse_const_iterator = std::reverse_iterator<const_iterator>;
        biginteger() = default;
        biginteger(const biginteger&) = default;
        biginteger(biginteger&&) noexcept = default;
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_v<fromBase, base>, int> = 0>
        explicit biginteger(const biginteger<fromBase>& other)
            : _Signal(other.signal()), ubint(other.ubint) {}
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_v<fromBase, base>, int> = 0>
        explicit biginteger(biginteger<fromBase>&& other)
            : _Signal(other.signal()), ubint(std::move(other.ubint)) {}
        template<size_t length>
        explicit biginteger(const virtual_biginteger<length>& rhs)
            : _Signal(rhs.signal()), ubint(rhs.get_unsigned()) {}
        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        explicit biginteger(const T& val)
            : biginteger(get_virtual_biginteger<base>(val)) {}
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
        explicit biginteger(const String_view& str) {
            size_t n = str.size();
            if (!n) {
                set_zero();
                return;
            }
            auto ptr = str.data();
            if ((*ptr) == '-' || (*ptr) == '+') {
                _Signal = (*ptr) == '+';
                --n;
                ++ptr;
            }
            WASSERT_LEVEL_1(n != 0);
            size_t _Size = (n - 1) / traits::bit_length + 1;
            ubint.vec.resize(_Size);
            virtual_unsigned_biginteger_iterator<base, 0>
                write(data(), size(), 1, n);
            while (n--) {
                *(--write) = (*ptr) - '0';
                ++ptr;
            }
        }
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
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_v<fromBase, base>, int> = 0>
        biginteger& operator=(const biginteger& other) {
            _Signal = other.signal();
            ubint = other.ubint;
            return *this;
        }
        template<size_t fromBase, std::enable_if_t<_Quick_base_conversion_v<fromBase, base>, int> = 0>
        biginteger& operator=(biginteger&& other) {
            _Signal = other.signal();
            ubint = std::move(other.ubint);
            return *this;
        }
        biginteger& operator=(const String_view& str) {
            size_t n = str.size();
            if (!n) {
                set_zero();
                return *this;
            }
            auto ptr = str.data();
            if ((*ptr) == '-' || (*ptr) == '+') {
                _Signal = (*ptr) == '+';
                --n;
                ++ptr;
            }
            WASSERT_LEVEL_1(n != 0);
            size_t _Size = (n - 1) / traits::bit_length + 1;
            ubint.vec.clear();
            ubint.vec.resize(_Size);
            virtual_unsigned_biginteger_iterator<base, 0>
                write(data(), size(), 1, n);
            while (n--) {
                *(--write) = (*ptr) - '0';
                ++ptr;
            }
            return *this;
        }

        template<typename iter, typename Func>
        biginteger& assign(iter first, iter last, Func&& fn) {
            return this->assign(true, first, last, std::forward<Func>(fn));
        }
        template<typename iter, typename Func>
        biginteger& assign(bool _Sign, iter first, iter last, Func&& fn) {
            _Signal = _Sign;
            ubint.assign(first, last, std::forward<Func>(fn));
            maintain_signal();
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
        bool signal()const { return _Signal; }
        void set_zero() { _Signal = true; ubint.set_zero(); }
        void maintain_signal() { _Signal = _Signal || ubint.zero(); }
        void change_signal() { _Signal = (!_Signal) || ubint.zero(); }
        void set_signal(bool f) { _Signal = f || ubint.zero(); }
        void reserve(size_t k) {
            ubint.vec.reserve((k + traits::bit_length - 1) / traits::bit_length);
        }
        void resize(size_t k) {
            size_t _Oldsize = size();
            size_t _Newsize = (k + traits::bit_length - 1) / traits::bit_length;
            ubint.vec.resize(_Newsize);
            if (_Oldsize >= _Newsize) {
                size_t r = k % traits::bit_length;
                if (r) {
                    value_type* val = data() + (_Newsize - 1);
                    *val = traits::quick_mod_base_power(*val, r);
                }
            }
        }
        void abs() { _Signal = true; }

        size_t bit_length() const {
            size_t _Pos = ubint.vec.size() - 1;
            return _Pos * traits::bit_length + cqlog<base>(ubint.vec[_Pos]) + 1;
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

        virtual_biginteger<0> get_virtual()const {
            return virtual_biginteger<0>(*this);
        }

        virtual_unsigned_biginteger<base, 0> get_unsigned_virtual()const {
            return get_virtual().get_unsigned();
        }

        template<typename T>
        explicit operator T()const {
            return T(get_virtual());
        }

        String tostr()const {
            size_t n = bit_length();
            String str;
            str.reserve(n + 1);
            if (!_Signal)str.push_back('-');
            for (auto _View = rbegin(); _View != rend(); ++_View) {
                str.push_back('0' + *_View);
            }
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

        template<size_t il, size_t ir>
        static void signed_mul(
            biginteger& result,
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs
        ) {
            unsigned_traits::mul(result.ubint, lhs.get_unsigned(), rhs.get_unsigned());
            result._Signal = !(lhs.signal() ^ rhs.signal());
            result.maintain_signal();
        }

        enum class div_mode {
            none,
            lhs_is_power_of_base,
            //rhs_is_power_of_base
        };

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

            biginteger mid_result;
            mid_result.reserve(m + 1);
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

                mul(mid_result, rhs, q);
                r -= mid_result;
                while (!r.signal()) {
                    --q;
                    r += rhs;
                }

                presult[j] = q;
                if (j != 0) {
                    r <<= traits::bit_length;
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
                mo <<= ml * traits::bit_length;
                auto cl = virtual_biginteger<0>(true, lhs.data() + l - ml, ml).maintain();
                if (mo.size() < cl.size())mo.ubint.vec.resize(cl.size());
                memcpy(mo.data(), cl.data(), sizeof(value_type) * cl.size());
                size_t lc = std::min(mo.size(), g);
                mid_result = virtual_biginteger<0>(true, mo.data() + lc, mo.size() - lc);
                mid_result *= E;
                mid_result >>= E.size() * traits::bit_length;
                mo -= mid_result * rhs;
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

            biginteger E;
            // initialize E as base ^ ((k+1) * bit_length)
            E.ubint.vec.resize(k + 2, 0);
            E.ubint.vec[k + 1] = 1;

            biginteger ans1(virtual_biginteger<0>(true, rhs.data() + p, m - p));
            quick_divmod<div_mode::lhs_is_power_of_base>(E, ans1);
            size_t g = k + 1 - E.size();
            if constexpr (mode == div_mode::none) {
                ans1 = virtual_biginteger<0>(true, lhs.data() + mid + p + g, k - g);
                ans1 *= E;
                ans1 >>= E.size() * traits::bit_length;
            }
            else {
                size_t bl = lhs.bit_length();
                ans1 = E;
                ans1 >>= (2 * traits::bit_length - ((bl - 1) % traits::bit_length));
            }

            biginteger mo1(virtual_biginteger<0>(true, lhs.data() + mid, n - mid));
            mo1 -= ans1 * rhs;

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

            mo1 <<= (mid * traits::bit_length);
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
            lhs <<= (mid * traits::bit_length);

            size_t ml = std::min(p + g, mo1.size());
            auto view = virtual_biginteger<0>(true, mo1.data() + p + g, mo1.size() - ml).maintain();
            biginteger ans2(view);
            ans2 *= E;
            ans2 >>= E.size() * traits::bit_length;

            mo1 -= rhs * ans2;

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
                copyB <<= (mid * traits::bit_length);
                copyB += view;
            }
            else {
                copyB <<= (mid * traits::bit_length);
                if (copyB.size() < view.size())copyB.ubint.vec.resize(view.size());
                memcpy(copyB.data(), view.data(), sizeof(value_type) * view.size());
            }

            copyB -= virtual_biginteger<0>(true, rhs.data(), mid).maintain() * copyA;
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
            if (unsigned_biginteger<base>::less(lhs.ubint, rhs.ubint)) {
                lhs.swap(rhs);
                lhs.set_zero();
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
                if constexpr (_Is_rbint_v<T>) {
                    biginteger R(std::forward<T>(rhs));
                    R.abs();
                    quick_divmod(*this, R);
                }
                else {
                    biginteger R(_Rhs);
                    quick_divmod(*this, R);
                }
            }
            this->_Signal = f;
            maintain_signal();
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
                if constexpr (_Is_rbint_v<T>) {
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

        biginteger& operator<<=(size_t index) {
            // quick mul base ^ index
            ubint.quick_mul_base_power(index);
            return *this;
        }

        biginteger& operator>>=(size_t index) {
            // quick divide base ^ index
            ubint.quick_divide_base_power(index);
            maintain_signal();
            return *this;
        }

        biginteger& maintain() {
            ubint.maintain();
            maintain_signal();
            return *this;
        }

    private:

        std::vector<value_type, mallocator<value_type>>&
            get_vec() {
            return ubint.vec;
        }

        const std::vector<value_type, mallocator<value_type>>&
            get_vec()const {
            return ubint.vec;
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
            if constexpr (toBase == base) {
                return biginteger<toBase>(std::forward<T>(v));
            }
            else {
                if constexpr (traits::_max == biginteger_traits<toBase>::_max) {
                    return biginteger<toBase>(std::forward<T>(v));
                }
                else {
                    std::vector<std::pair<size_t, biginteger<toBase>>> cache;
                    cache.reserve(30);
                    cache.emplace_back(1, biginteger<toBase>(traits::max_value));
                    cache.emplace_back(2, cache[0].second * cache[0].second);
                    biginteger<toBase> result = dc_base_conversion_helper<toBase>(
                        get_virtual_biginteger<base>(v).get_unsigned(), cache);
                    if (!v.signal())result.change_signal();
                    return result;
                }
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
        VIRTUAL_BIGINTEGER_SINGLE_AUTO_DEFINITION
            friend biginteger<_Base> operator<<(T&& lhs, size_t delta);
        VIRTUAL_BIGINTEGER_SINGLE_AUTO_DEFINITION
            friend biginteger<_Base> operator>>(T&& lhs, size_t delta);

        template<size_t _Base>
        friend void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);
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

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        int cmp(const T& lhs, const U& rhs) {
        return biginteger<_Base>::signed_cmp(
            get_virtual_biginteger<T>(lhs),
            get_virtual_biginteger<U>(rhs)
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
        biginteger<_Base>::signed_mul(
            result,
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
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
        if constexpr (biginteger<_Base>::template _Is_rbint_v<T>) {
            lhs += std::forward<U>(rhs);
            return std::forward<T>(lhs);
        }
        else if constexpr (biginteger<_Base>::template _Is_rbint_v<U>) {
            rhs += std::forward<T>(lhs);
            return std::forward<U>(rhs);
        }
        else {
            biginteger<_Base> result;
            add(result, std::forward<T>(lhs), std::forward<U>(rhs));
            return result;
        }
    }

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        biginteger<_Base> operator-(T&& lhs, U&& rhs) {
        using unsigned_traits = typename biginteger<_Base>::unsigned_traits;
        if constexpr (biginteger<_Base>::template _Is_rbint_v<T>) {
            lhs -= std::forward<U>(rhs);
            return std::forward<T>(lhs);
        }
        else if constexpr (biginteger<_Base>::template _Is_rbint_v<U>) {
            rhs -= std::forward<T>(lhs);
            rhs.change_signal();
            return std::forward<U>(rhs);
        }
        else {
            biginteger<_Base> result;
            sub(result, std::forward<T>(lhs), std::forward<U>(rhs));
            return result;
        }
    }

    VIRTUAL_BIGINTEGER_BINARY_AUTO_DEFINITION
        biginteger<_Base> operator*(T&& lhs, U&& rhs) {
        if constexpr (biginteger<_Base>::template _Is_rbint_v<T>) {
            lhs *= std::forward<U>(rhs);
            return std::forward<T>(lhs);
        }
        else if constexpr (biginteger<_Base>::template _Is_rbint_v<U>) {
            rhs *= std::forward<T>(lhs);
            return std::forward<U>(rhs);
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
        biginteger<_Base> operator<<(T&& lhs, size_t delta) {
        biginteger<_Base> result(std::forward<T>(lhs));
        result <<= delta;
        return result;
    }
    VIRTUAL_BIGINTEGER_SINGLE_AUTO_DEFINITION
        biginteger<_Base> operator>>(T&& lhs, size_t delta) {
        biginteger<_Base> result(std::forward<T>(lhs));
        result >>= delta;
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
        biginteger<_Base>::quick_divmod(lhs, rhs);
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
            for (; val == rep[pos] && pos--;) {
                rep[pos] = (val = tail(mt_rand));
            }
            if (val > rep[pos])continue;
            if (pos == -1)
                break;
            for (; pos--;) {
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

}

#endif
