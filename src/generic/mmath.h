#ifndef __WJR_MMATH_H
#define __WJR_MMATH_H

#include <array>
#include <limits>

#include "mmacro.h"

namespace wjr::math {
    template<typename T>
    struct _Is_unsigned_standard_integral {
        constexpr static bool value = std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>;
    };
    template<typename T>
    constexpr bool _Is_unsigned_standard_integral_v = _Is_unsigned_standard_integral<T>::value;
}

namespace wjr::math::force_constexpr {
    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr int has_single_bit(const T val) noexcept {
        return val != 0 && (val & (val - 1)) == 0;
    }

    template<typename T, T _Base, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    constexpr decltype(auto) integer_power(std::integral_constant<T, _Base>, unsigned int kth) noexcept {
        T result = 1;
        T base = _Base;
        while (kth) {
            if (kth & 1) {
                result *= base;
            }
            base *= base;
            kth >>= 1;
        }
        return result;
    }

    template<unsigned long long _Base, size_t _Size>
    struct __constexpr_log_table {
        static_assert(_Size <= 256, "log table size must be less than 256");
        constexpr __constexpr_log_table() : m_table() {
            if constexpr (_Size != 0) {
                m_table[0] = -1;
                for (size_t i = 1; i < _Size; ++i) {
                    m_table[i] = m_table[i / _Base] + 1;
                }
            }
        }
        constexpr int operator[](size_t i) const noexcept {
            return m_table[i];
        }
    private:
        std::array<int, _Size> m_table;
    };

    template<unsigned long long _Base, typename T>
    constexpr unsigned int _Force_constexpr_digits(std::integral_constant<unsigned long long, _Base> u, const T val) {
        unsigned int digits = 0;
        T vc = 1;
        while ((val / vc) >= vc) {
            ++digits;
            vc *= _Base;
        }
        return digits;
    }

    template<unsigned long long _Base>
    constexpr unsigned int _Get_log_table_size(std::integral_constant<unsigned long long, _Base> u) {
        auto max_val = std::numeric_limits<unsigned long long>::max();
        auto mid_digits = _Force_constexpr_digits(u, max_val);
        while (max_val >= 256 && mid_digits != 1) {
            auto mid_val = integer_power(u, mid_digits);
            max_val = mid_val - 1;
            mid_digits = (mid_digits + 1) / 2;
        }
        if (mid_digits == 1) {
            return 0;
        }
        return max_val + 1;
    }

    template<unsigned long long _Base>
    constexpr static auto __integer_log_table =
        __constexpr_log_table < _Base, _Get_log_table_size(std::integral_constant<unsigned long long, _Base>{}) > ();

    template<unsigned long long _Base, typename T, T _Max, int mid_digits>
    constexpr int _Integer_log(std::integral_constant<unsigned long long, _Base> u, const T val) noexcept {
        constexpr auto table_size = _Get_log_table_size(u);
        if constexpr (_Max < table_size) {
            return __integer_log_table<_Base>[val];
        }
        else {
            constexpr auto nxt_digits = (mid_digits + 1) / 2;
            constexpr auto mid_val = integer_power(u, mid_digits);
            if constexpr (mid_digits == 1) {
                if (val >= mid_val) {
                    return 1;
                }
                else {
                    return val == 0 ? -1 : 0;
                }
            }
            else {
                if (val >= mid_val) {
                    return mid_digits + _Integer_log<_Base, T,
                        mid_val - 1, nxt_digits>(u, val / mid_val);
                }
                else {
                    return _Integer_log<_Base, T, mid_val - 1, nxt_digits>(u, val);
                }
            }
        }
    }

    template<typename U, U _Base, typename T,
        std::enable_if_t<std::conjunction_v<_Is_unsigned_standard_integral<T>, _Is_unsigned_standard_integral<U>>, int> = 0>
    constexpr int integer_log(std::integral_constant<U, _Base> u, const T val) {
        static_assert(_Base != 0, "base must be non-zero");
        static_assert(_Base != 1, "base must be non-one");
        static_assert(
            std::is_same_v<std::common_type_t<U, unsigned long long>, unsigned long long>,
            "base must be of type unsigned long long");
        constexpr unsigned long long ull_base = _Base;
        constexpr auto _uic = std::integral_constant<unsigned long long, ull_base>{};
        constexpr auto mid_digits =
            _Force_constexpr_digits(_uic, std::numeric_limits<T>::max());
        return _Integer_log<ull_base, T, std::numeric_limits<T>::max(), mid_digits>(_uic, val);
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr int countl_zero(const T val) noexcept {
        constexpr int digits = std::numeric_limits<T>::digits;
        return digits - 1 - integer_log(std::integral_constant<T, 2>{}, val);
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr int countl_one(const T val) noexcept {
        return static_cast<int>(math::force_constexpr::countl_zero(static_cast<T>(~val)));
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr int countr_zero(const T val) noexcept {
        constexpr int digits = std::numeric_limits<T>::digits;
        return static_cast<int>(digits -
            math::force_constexpr::countl_zero(static_cast<T>(~val) & static_cast<T>(val - 1)));
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr int countr_one(const T val) noexcept {
        return static_cast<int>(math::force_constexpr::countr_zero(static_cast<T>(~val)));
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr int bit_width(const T val) noexcept {
        return static_cast<int>(std::numeric_limits<T>::digits -
            math::force_constexpr::countl_zero(val));
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr int integer_log2(const T val) noexcept {
        return bit_width(val) - 1;
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr T bit_ceil(const T val) noexcept {
        if (val == 0 || val == 1) {
            return 1;
        }
        auto shift = math::force_constexpr::bit_width(val - 1u);
        return static_cast<T>(static_cast<T>(1u) << shift);
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr T bit_floor(const T val) noexcept {
        if (val == 0) {
            return 0;
        }
        auto shift = math::force_constexpr::bit_width(val) - 1;
        return static_cast<T>(static_cast<T>(1u) << shift);
    }

# define __POPCOUNT_BIT2(n)       n,       n+1,       n+1,       n+2
# define __POPCOUNT_BIT4(n) __POPCOUNT_BIT2(n), __POPCOUNT_BIT2(n+1), __POPCOUNT_BIT2(n+1), __POPCOUNT_BIT2(n+2)
# define __POPCOUNT_BIT6(n) __POPCOUNT_BIT4(n), __POPCOUNT_BIT4(n+1), __POPCOUNT_BIT4(n+1), __POPCOUNT_BIT4(n+2)
# define __POPCOUNT_BIT8(n) __POPCOUNT_BIT6(n), __POPCOUNT_BIT6(n+1), __POPCOUNT_BIT6(n+1), __POPCOUNT_BIT6(n+2)
    const inline uint8_t __popcount_table[256] = { __POPCOUNT_BIT8(0) };
#undef __POPCOUNT_BIT8
#undef __POPCOUNT_BIT6
#undef __POPCOUNT_BIT4
#undef __POPCOUNT_BIT2

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr int popcount(const T n) noexcept {
		constexpr int digits = std::numeric_limits<T>::digits;
        if constexpr (digits <= 16) {
            return popcount(static_cast<uint32_t>(n));
        }
        else if constexpr (digits == 32) {
            return __popcount_table[n & 0xFF] + __popcount_table[(n >> 8) & 0xFF] +
                __popcount_table[(n >> 16) & 0xFF] + __popcount_table[(n >> 24) & 0xFF];
        }
        else {
            return popcount(static_cast<uint32_t>(n)) + popcount(static_cast<uint32_t>(n >> 32));
        }
    }

    constexpr uint16_t mulhi_u16(uint16_t x, uint16_t y) {
        return static_cast<uint16_t>((static_cast<uint32_t>(x) * static_cast<uint32_t>(y)) >> 16);
    }

    constexpr int16_t mulhi_s16(int16_t x, int16_t y) {
		return static_cast<int16_t>((static_cast<int32_t>(x) * static_cast<int32_t>(y)) >> 16);
	}

	constexpr uint32_t mulhi_u32(uint32_t x, uint32_t y) {
		return static_cast<uint32_t>((static_cast<uint64_t>(x) * static_cast<uint64_t>(y)) >> 32);
	}
	
	constexpr int32_t mulhi_s32(int32_t x, int32_t y) {
		return static_cast<int32_t>((static_cast<int64_t>(x) * static_cast<int64_t>(y)) >> 32);
	}

	constexpr uint64_t mulhi_u64(uint64_t x, uint64_t y) {
#if defined(__SIZEOF_INT128__)
        return ((__uint128_t)x * y) >> 64;
#else
        constexpr uint32_t mask = 0xFFFFFFFF;
        uint32_t x0 = (uint32_t)(x & mask);
        uint32_t x1 = (uint32_t)(x >> 32);
        uint32_t y0 = (uint32_t)(y & mask);
        uint32_t y1 = (uint32_t)(y >> 32);
        uint32_t x0y0_hi = mulhi_u32(x0, y0);
        uint64_t x0y1 = x0 * (uint64_t)y1;
        uint64_t x1y0 = x1 * (uint64_t)y0;
        uint64_t x1y1 = x1 * (uint64_t)y1;
        uint64_t temp = x1y0 + x0y0_hi;
        uint64_t temp_lo = temp & mask;
        uint64_t temp_hi = temp >> 32;

        return x1y1 + temp_hi + ((temp_lo + x0y1) >> 32);
#endif
	}

    constexpr int64_t mulhi_s64(int64_t x, int64_t y) {
#if defined(__SIZEOF_INT128__)
        return ((__int128_t)x * y) >> 64;
#else
        constexpr uint32_t mask = 0xFFFFFFFF;
        uint32_t x0 = (uint32_t)(x & mask);
        uint32_t y0 = (uint32_t)(y & mask);
        int32_t x1 = (int32_t)(x >> 32);
        int32_t y1 = (int32_t)(y >> 32);
        uint32_t x0y0_hi = mulhi_u32(x0, y0);
        int64_t t = x1 * (int64_t)y0 + x0y0_hi;
        int64_t w1 = x0 * (int64_t)y1 + (t & mask);

        return x1 * (int64_t)y1 + (t >> 32) + (w1 >> 32);
#endif
    }

    constexpr uint64_t u64_mul_64_to_128(uint64_t x, uint64_t y, uint64_t* hi) {
        constexpr uint64_t mask = 0xFFFFFFFF;
        uint32_t x0 = (uint32_t)(x & mask);
        uint32_t x1 = (uint32_t)(x >> 32);
        uint32_t y0 = (uint32_t)(y & mask);
        uint32_t y1 = (uint32_t)(y >> 32);
        uint64_t x0y0 = (uint64_t)x0 * y0;
        uint64_t x1y1 = (uint64_t)x1 * y1;

        uint64_t x0y1 = (uint64_t)x0 * y1;
        uint64_t x0y1_hi = x0y1 >> 32;
        uint64_t x0y1_lo = x0y1 << 32;
        x0y0 += x0y1_lo;
        x1y1 += x0y1_hi + (x0y0 < x0y1_lo);

        uint64_t x1y0 = (uint64_t)x1 * y0;
        uint64_t x1y0_hi = x1y0 >> 32;
        uint64_t x1y0_lo = x1y0 << 32;
        x0y0 += x1y0_lo;
        x1y1 += x1y0_hi + (x0y0 < x1y0_lo);

        *hi = x1y1;
        return x0y0;
    }

    // divide 32-bit unsigned integer by 16-bit unsigned integer 
    // to get 16-bit unsigned integer quotient and remainder
    constexpr uint16_t u32_div_16_to16(
        uint16_t u1, uint16_t u0, uint16_t v, uint16_t* r) {
        uint32_t n = ((uint32_t)u1 << 16) | u0;
        uint16_t result = (uint16_t)(n / v);
        *r = (uint16_t)(n - result * (uint32_t)v);
        return result;
    }

    constexpr uint32_t u64_div_32_to_32(
		uint32_t u1, uint32_t u0, uint32_t v, uint32_t* r) {
		uint64_t n = ((uint64_t)u1 << 32) | u0;
		uint32_t result = (uint32_t)(n / v);
		*r = (uint32_t)(n - result * (uint64_t)v);
		return result;
	}
	
    constexpr uint64_t u128_div_64_to_64(uint64_t numhi, uint64_t numlo, uint64_t den, uint64_t* r) {
        constexpr uint64_t b = ((uint64_t)1 << 32);

        // Check for overflow and divide by 0.
        if (numhi >= den) {
            if (r != nullptr) *r = ~0ull;
            return ~0ull;
        }

        int shift = math::force_constexpr::countl_zero(den);
		
        den <<= shift;
        numhi <<= shift;
        numhi |= (numlo >> (-shift & 63)) & (-(int64_t)shift >> 63);
        numlo <<= shift;

        // Extract the low digits of the numerator and both digits of the denominator.
        uint32_t num1 = (uint32_t)(numlo >> 32);
        uint32_t num0 = (uint32_t)(numlo & 0xFFFFFFFFu);
        uint32_t den1 = (uint32_t)(den >> 32);
        uint32_t den0 = (uint32_t)(den & 0xFFFFFFFFu);

        // We wish to compute q1 = [n3 n2 n1] / [d1 d0].
        // Estimate q1 as [n3 n2] / [d1], and then correct it.
        // Note while qhat may be 2 digits, q1 is always 1 digit.
        uint64_t qhat = numhi / den1;
        uint64_t rhat = numhi % den1;
        uint64_t c1 = qhat * den0;
        uint64_t c2 = rhat * b + num1;
        if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
        uint32_t q1 = (uint32_t)qhat;

        // Compute the true (partial) remainder.
        uint64_t rem = numhi * b + num1 - q1 * den;

        // We wish to compute q0 = [rem1 rem0 n0] / [d1 d0].
        // Estimate q0 as [rem1 rem0] / [d1] and correct it.
        qhat = rem / den1;
        rhat = rem % den1;
        c1 = qhat * den0;
        c2 = rhat * b + num0;
        if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
        uint32_t q0 = (uint32_t)qhat;

        // Return remainder if requested.
        if (r != NULL) *r = (rem * b + num0 - q0 * den) >> shift;
        return ((uint64_t)q1 << 32) | q0;
    }

}

namespace wjr::math {

#if WJR_HAS_BUILTIN(__builtin_bit_cast)
    template <class _To, class _From,
        std::enable_if_t<std::conjunction_v<std::bool_constant<sizeof(_To) == sizeof(_From)>,
        std::is_trivially_copyable<_To>, std::is_trivially_copyable<_From>>, int> = 0>
        constexpr _To bit_cast(const _From& _Val) noexcept {
        return __builtin_bit_cast(_To, _Val);
    }
#endif

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    constexpr int has_single_bit(const T val) noexcept {
        return val != 0 && (val & (val - 1)) == 0;
    }

#if defined(WJR_X86_64) && defined(_MSC_VER)

    extern "C" {
        extern int __isa_available;
    }

    const inline bool _Definitely_have_avx2 = __isa_available >= __ISA_AVAILABLE_AVX2;

    template <typename T>
    int _Countl_zero_lzcnt(const T val) noexcept {
        constexpr int digits = std::numeric_limits<T>::digits;

        if constexpr (digits <= 16) {
#if defined(__AVX2__)
            return static_cast<int>(__lzcnt16(val) - (16 - digits));
#else // __LZCNT__
            return static_cast<int>(_lzcnt_u32(val) - (32 - digits));
#endif
        }
        else if constexpr (digits == 32) {
#if defined(__AVX2__)
            return static_cast<int>(__lzcnt(val));
#else // __LZCNT__
            return static_cast<int>(_lzcnt_u32(val));
#endif
        }
        else {
#if defined(WJR_X86)
            const unsigned int high = val >> 32;
            const auto low = static_cast<unsigned int>(val);
            if (high == 0) {
                return 32 + math::_Countl_zero_lzcnt(low);
            }
            else {
                return math::_Countl_zero_lzcnt(high);
            }
#else
#if defined(__AVX2__)
            return __lzcnt64(val);
#else // __LZCNT__
            return _lzcnt_u64(val);
#endif
#endif
        }
    }

    template <typename T>
    int _Countl_zero_bsr(const T val) noexcept {
        constexpr int digits = std::numeric_limits<T>::digits;

        unsigned long result;
        if constexpr (digits <= 32) {
            if (!_BitScanReverse(&result, val)) {
                return digits;
            }
        }
        else {
#ifdef WJR_X86
            const unsigned int high = val >> 32;
            if (_BitScanReverse(&result, high)) {
                return static_cast<int>(31 - result);
            }

            const auto low = static_cast<unsigned int>(val);
            if (!_BitScanReverse(&result, low)) {
                return digits;
            }
#else // ^^^ WJR_X86 / !WJR_X86 vvv
            if (!_BitScanReverse64(&result, val)) {
                return digits;
            }
#endif // WJR_X86
        }
        return static_cast<int>(digits - 1 - result);
    }

    template<typename T>
    int _X86_64_countl_zero(T val) noexcept {
#if defined(__AVX2__) || defined(__LZCNT__)
        return math::_Countl_zero_lzcnt(val);
#else // __AVX2__
        if (_Definitely_have_avx2) {
            return math::_Countl_zero_lzcnt(val);
        }
        else {
            return math::_Countl_zero_bsr(val);
        }
#endif // __AVX2__
    }

#endif

    template <typename T>
    int _Arm_countl_zero(const T val) noexcept {
        constexpr int digits = std::numeric_limits<T>::digits;
        if (val == 0) {
            return digits;
        }

        constexpr auto u_d = std::numeric_limits<unsigned int>::digits;
        constexpr auto ul_d = std::numeric_limits<unsigned long>::digits;
        constexpr auto ull_d = std::numeric_limits<unsigned long long>::digits;

        if constexpr (digits <= u_d) {
            constexpr auto diff = u_d - digits;
#if defined(__GUNC__) || WJR_HAS_BUILTIN(__builtin_clz)
            return static_cast<int>(__builtin_clz(val) - diff);
#endif
        }
        else if constexpr (digits <= ul_d) {
            constexpr auto diff = ul_d - digits;
#if defined(__GUNC__) || WJR_HAS_BUILTIN(__builtin_clzl)
            return static_cast<int>(__builtin_clzl(val) - diff);
#endif
        }
        else {
            constexpr auto diff = ull_d - digits;
#if defined(__GUNC__) || WJR_HAS_BUILTIN(__builtin_clzll)
            return static_cast<int>(__builtin_clzll(val) - diff);
#endif
        }
        if constexpr (digits <= 32) {
            constexpr auto diff = 32 - digits;
#if defined(__LZCNT__)
            return static_cast<int>(_lzcnt_u32(val) - diff);
#endif
        }
        else {
            constexpr auto diff = 64 - digits;
#if defined(__LZCNT__)
            return static_cast<int>(_lzcnt_u64(val) - diff);
#endif
        }

        return math::force_constexpr::countl_zero(val);
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    WJR_CONSTEXPR_FORCEINLINE int countl_zero(const T val) noexcept {
    WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(WJR_X86_64) && defined(_MSC_VER)
            return math::_X86_64_countl_zero(val);
#else
            return math::_Arm_countl_zero(val);
#endif
    WJR_IS_NOT_CONSTANT_EVALUATED_END
        return math::force_constexpr::countl_zero(val);
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    WJR_CONSTEXPR int countl_one(const T val) noexcept {
        return math::countl_zero(static_cast<T>(~val));
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    WJR_CONSTEXPR int bit_width(const T val) noexcept {
        return static_cast<int>(std::numeric_limits<T>::digits - math::countl_zero(val));
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    WJR_CONSTEXPR int integer_log2(const T val) noexcept {
        return bit_width(val) - 1;
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    WJR_CONSTEXPR T bit_ceil(const T val) noexcept {
        if (val == 0 || val == 1) {
            return 1;
        }
        auto shift = math::bit_width(val - 1u);
        return static_cast<T>(1u << shift);
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    WJR_CONSTEXPR T bit_floor(const T val) noexcept {
        if (val == 0) {
            return 0;
        }
        auto shift = math::bit_width(val) - 1;
        return static_cast<T>(1u << shift);
    }

#if defined(WJR_X86_64) && defined(_MSC_VER) && !defined(_M_CEE_PURE) && !defined(__CUDACC__)  && !defined(__INTEL_COMPILER)
#define _HAS_TZCNT_BSF_INTRINSICS 1
#else
#define _HAS_TZCNT_BSF_INTRINSICS 0
#endif


#if _HAS_TZCNT_BSF_INTRINSICS

#if !(defined(WJR_X86_64) && defined(_MSC_VER))
    extern "C" {
        extern int __isa_available;
    }
    const inline bool _Definitely_have_avx2 = __isa_available >= __ISA_AVAILABLE_AVX2;
#endif
	
#ifdef __clang__
#define _TZCNT_U32 __builtin_ia32_tzcnt_u32
#define _TZCNT_U64 __builtin_ia32_tzcnt_u64
#else // ^^^ __clang__ / !__clang__ vvv
#define _TZCNT_U32 _tzcnt_u32
#define _TZCNT_U64 _tzcnt_u64
#endif // __clang__

    template<typename T>
    int _Countr_zero_tzcnt(const T val) noexcept {
        constexpr int digits = std::numeric_limits<T>::digits;
        constexpr T _Max = std::numeric_limits<T>::max();
        if constexpr (digits <= 32) {
            return static_cast<int>(_TZCNT_U32(static_cast<unsigned int>(~_Max | val)));
        }
        else {
#if defined(WJR_X86)
            const auto low = static_cast<unsigned int>(val);
            if (low == 0) {
                const unsigned int high = val >> 32;
                return static_cast<int>(32 + _TZCNT_U32(high));
            }
            else {
                return static_cast<int>(_TZCNT_U32(low));
            }
#else
            return static_cast<int>(_TZCNT_U64(val));
#endif
        }
    }

#undef _TZCNT_U32
#undef _TZCNT_U64

    template <typename T>
    int _Countr_zero_bsf(const T val) noexcept {
        constexpr int digits = std::numeric_limits<T>::digits;
        constexpr T _Max = (std::numeric_limits<T>::max)();

        unsigned long result;
        if constexpr (digits <= 32) {
            // Intended widening to int. This operation means that a narrow 0 will widen
            // to 0xFFFF....FFFF0... instead of 0. We need this to avoid counting all the zeros
            // of the wider type.
            if (!_BitScanForward(&result, static_cast<unsigned int>(~_Max | val))) {
                return digits;
            }
        }
        else {
#ifdef WJR_X86
            const auto low = static_cast<unsigned int>(val);
            if (_BitScanForward(&result, low)) {
                return result;
            }

            const unsigned int high = val >> 32;
            if (!_BitScanForward(&result, high)) {
                return digits;
            }
            else {
                return static_cast<int>(result + 32);
            }
#else // ^^^ WJR_X86 / !WJR_X86 vvv
            if (!_BitScanForward64(&result, val)) {
                return digits;
            }
#endif // WJR_X86
        }
        return static_cast<int>(result);
    }

    template<typename T>
    int _X86_64_countr_zero(const T val) noexcept {
#ifdef __AVX2__
        return math::_Countr_zero_tzcnt(val);
#else // __AVX2__
        if (_Definitely_have_avx2) {
            return math::_Countr_zero_tzcnt(val);
        }
        else {
            return math::_Countr_zero_bsf(val);
        }
#endif // __AVX2__
    }

#endif

    template<typename T>
    int _Arm_countr_zero(const T val) noexcept {
        constexpr int digits = std::numeric_limits<T>::digits;
        if (val == 0) {
            return digits;
        }

        constexpr auto u_d = std::numeric_limits<unsigned int>::digits;
        constexpr auto ul_d = std::numeric_limits<unsigned long>::digits;
        constexpr auto ull_d = std::numeric_limits<unsigned long long>::digits;

        if constexpr (digits <= u_d) {
#if defined(__GUNC__) || WJR_HAS_BUILTIN(__builtin_ctz)
            return static_cast<int>(__builtin_ctz(val));
#endif
        }
        else if constexpr (digits <= ul_d) {
#if defined(__GUNC__) || WJR_HAS_BUILTIN(__builtin_ctzl)
            return static_cast<int>(__builtin_ctzl(val));
#endif
        }
        else {
#if defined(__GUNC__) || WJR_HAS_BUILTIN(__builtin_ctzll)
            return static_cast<int>(__builtin_ctzll(val));
#endif
        }

        return math::force_constexpr::countr_zero(val);
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    WJR_CONSTEXPR int countr_zero(const T val) noexcept {
    WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(WJR_X86_64) && _HAS_TZCNT_BSF_INTRINSICS
        return math::_X86_64_countr_zero(val);
#else
        return math::_Arm_countr_zero(val);
#endif
     WJR_IS_NOT_CONSTANT_EVALUATED_END
        return math::force_constexpr::countr_zero(val);
    }

#undef _HAS_TZCNT_BSF_INTRINSICS

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    WJR_CONSTEXPR int countr_one(const T val) noexcept {
        return math::countr_zero(static_cast<T>(~val));
    }

    template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
    WJR_CONSTEXPR int popcount(const T n) noexcept {
        return force_constexpr::popcount(n);
    }

    constexpr uint16_t mulhi_u16(uint16_t x, uint16_t y) {
        return force_constexpr::mulhi_u16(x, y);
    }

    constexpr int16_t mulhi_s16(int16_t x, int16_t y) {
        return force_constexpr::mulhi_s16(x, y);
    }

    constexpr uint32_t mulhi_u32(uint32_t x, uint32_t y) {
        return force_constexpr::mulhi_u32(x, y);
    }

    constexpr int32_t mulhi_s32(int32_t x, int32_t y) {
		return force_constexpr::mulhi_s32(x, y);
    }

    WJR_CONSTEXPR uint64_t mulhi_u64(uint64_t x, uint64_t y) {
        WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(_MSC_VER) && defined(WJR_X64)
        return __umulh(x, y);
#elif defined(__SIZEOF_INT128__)
        return static_cast<uint64_t>((static_cast<__uint128_t>(x) * y) >> 64);
#elif defined(WJR_GCC_STYLE_ASM) && defined(WJR_X64)
        __asm__("mulq %[u1]" : [u1] "+d"(x) : "a"(y));
        return x;
#endif
        WJR_IS_NOT_CONSTANT_EVALUATED_END
        return math::force_constexpr::mulhi_u64(x, y);
    }

    WJR_CONSTEXPR int64_t mulhi_s64(int64_t x, int64_t y) {
        WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(_MSC_VER) && defined(WJR_X64)
        return __mulh(x, y);
#elif defined(__SIZEOF_INT128__)
        return static_cast<int64_t>((static_cast<__int128_t>(x) * y) >> 64);
#elif defined(WJR_GCC_STYLE_ASM) && defined(WJR_X64)
        __asm__("imulq %[u1]" : [u1] "+d"(x) : "a"(y));
        return x;
#endif
        WJR_IS_NOT_CONSTANT_EVALUATED_END
            return math::force_constexpr::mulhi_s64(x, y);
    }

    WJR_CONSTEXPR uint64_t u64_mul_64_to_128(uint64_t x, uint64_t y, uint64_t* hi) {
        WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(_MSC_VER) && defined(WJR_X64)
        return _umul128(x, y, hi);
#elif defined(__SIZEOF_INT128__)
        __uint128_t ans = (__uint128_t)x * y;
        *hi = static_cast<uint64_t>(ans >> 64);
        return static_cast<uint64_t>(ans);
#elif defined(WJR_GCC_STYLE_ASM) && defined(WJR_X64)
        uint64_t ret;
        __asm__("mulq %[v]" : "=a"(ret), "=d"(*hi) : "a"(x), [v] "r"(y));
        return ret;
#endif
        WJR_IS_NOT_CONSTANT_EVALUATED_END
        return math::force_constexpr::u64_mul_64_to_128(x, y, hi);
    }

    // divide 32-bit unsigned integer by 16-bit unsigned integer 
    // to get 16-bit unsigned integer quotient and remainder
    constexpr uint16_t u32_div_16_to_16(
        uint16_t u1, uint16_t u0, uint16_t v, uint16_t* r) {
        return force_constexpr::u32_div_16_to16(u1, u0, v, r);
    }

    constexpr uint32_t u64_div_32_to_32(
        uint32_t u1, uint32_t u0, uint32_t v, uint32_t* r) {
        return force_constexpr::u64_div_32_to_32(u1, u0, v, r);
    }

    WJR_CONSTEXPR uint64_t u128_div_64_to_64(uint64_t numhi, uint64_t numlo, uint64_t den, uint64_t* r) {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(WJR_GCC_STYLE_ASM) && defined(WJR_X64)
        uint64_t result;
        __asm__("divq %[v]" : "=a"(result), "=d"(*r) : [v] "r"(den), "a"(numlo), "d"(numhi));
        return result;
#elif defined(_MSC_VER) && defined(WJR_X64)
        return _udiv128(numhi, numlo, den, r);
#endif
        WJR_IS_NOT_CONSTANT_EVALUATED_END
        constexpr uint64_t b = ((uint64_t)1 << 32);

        // Check for overflow and divide by 0.
        if (numhi >= den) {
            if (r != nullptr) *r = ~0ull;
            return ~0ull;
        }

        int shift = math::countl_zero(den);

        den <<= shift;
        numhi <<= shift;
        numhi |= (numlo >> (-shift & 63)) & (-(int64_t)shift >> 63);
        numlo <<= shift;

        // Extract the low digits of the numerator and both digits of the denominator.
        uint32_t num1 = (uint32_t)(numlo >> 32);
        uint32_t num0 = (uint32_t)(numlo & 0xFFFFFFFFu);
        uint32_t den1 = (uint32_t)(den >> 32);
        uint32_t den0 = (uint32_t)(den & 0xFFFFFFFFu);

        // We wish to compute q1 = [n3 n2 n1] / [d1 d0].
        // Estimate q1 as [n3 n2] / [d1], and then correct it.
        // Note while qhat may be 2 digits, q1 is always 1 digit.
        uint64_t qhat = numhi / den1;
        uint64_t rhat = numhi % den1;
        uint64_t c1 = qhat * den0;
        uint64_t c2 = rhat * b + num1;
        if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
        uint32_t q1 = (uint32_t)qhat;

        // Compute the true (partial) remainder.
        uint64_t rem = numhi * b + num1 - q1 * den;

        // We wish to compute q0 = [rem1 rem0 n0] / [d1 d0].
        // Estimate q0 as [rem1 rem0] / [d1] and correct it.
        qhat = rem / den1;
        rhat = rem % den1;
        c1 = qhat * den0;
        c2 = rhat * b + num0;
        if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
        uint32_t q0 = (uint32_t)qhat;

        // Return remainder if requested.
        if (r != NULL) *r = (rem * b + num0 - q0 * den) >> shift;
        return ((uint64_t)q1 << 32) | q0;
    }

}

#endif
