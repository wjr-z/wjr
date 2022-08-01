#ifndef __WJR_MMATH_H
#define __WJR_MMATH_H

#include <array>
#include <limits>

#include "mmacro.h"

namespace wjr {

    namespace math {
        template<typename T>
        struct _Is_unsigned_standard_integral {
            constexpr static bool value = std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>;
        };
        template<typename T>
        constexpr bool _Is_unsigned_standard_integral_v = _Is_unsigned_standard_integral<T>::value;
    }

    namespace math::force_constexpr {

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
            __constexpr_log_table<_Base, _Get_log_table_size(std::integral_constant<unsigned long long, _Base>{}) > ();

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
            std::enable_if_t<std::conjunction_v<_Is_unsigned_standard_integral<T>, _Is_unsigned_standard_integral<U>>,int> = 0>
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
            return static_cast<T>(1u << shift);
        }

        template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
        constexpr T bit_floor(const T val) noexcept {
            if (val == 0) {
                return 0;
            }
            auto shift = math::force_constexpr::bit_width(val) - 1;
            return static_cast<T>(1u << shift);
        }
    }

    namespace math {

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

        extern "C" {
            extern int __isa_available;
        }

#if defined(WJR_X86_64) && defined(_MSC_VER)
        template <typename T>
        int _Countl_zero_lzcnt(const T val) noexcept {
            constexpr int digits = std::numeric_limits<T>::digits;

            if constexpr (digits <= 16) {
                return static_cast<int>(__lzcnt16(val) - (16 - digits));
            }
            else if constexpr (digits == 32) {
                return static_cast<int>(__lzcnt(val));
            }
            else {
#if defined(_M_IX86)
                const unsigned int high = val >> 32;
                const auto low = static_cast<unsigned int>(val);
                if (high == 0) {
                    return 32 + math::_Countl_zero_lzcnt(low);
                }
                else {
                    return math::_Countl_zero_lzcnt(high);
                }
#else
                return __lzcnt64(val);
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
#ifdef _M_IX86
                const unsigned int high = val >> 32;
                if (_BitScanReverse(&result, high)) {
                    return static_cast<int>(31 - result);
                }

                const auto low = static_cast<unsigned int>(val);
                if (!_BitScanReverse(&result, low)) {
                    return digits;
                }
#else // ^^^ _M_IX86 / !_M_IX86 vvv
                if (!_BitScanReverse64(&result, val)) {
                    return digits;
                }
#endif // _M_IX86
            }
            return static_cast<int>(digits - 1 - result);
        }

        template<typename T>
        int _X86_64_countl_zero(T val) noexcept {
#ifdef __AVX2__
            return math::_Countl_zero_lzcnt(val);
#else // __AVX2__
            const bool _Definitely_have_lzcnt = __isa_available >= __ISA_AVAILABLE_AVX2;
            if (_Definitely_have_lzcnt) {
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

            return math::force_constexpr::countl_zero(val);
        }

        template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
        constexpr int countl_zero(const T val) noexcept {
#if defined(WJR_X86_64) && defined(_MSC_VER)
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                return math::_X86_64_countl_zero(val);
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#else
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                return math::_Arm_countl_zero(val);
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#endif
                return math::force_constexpr::countl_zero(val);
        }

        template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
        constexpr int countl_one(const T val) noexcept {
            return math::countl_zero(static_cast<T>(~val));
        }

        template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
        constexpr int bit_width(const T val) noexcept {
            return static_cast<int>(std::numeric_limits<T>::digits - math::countl_zero(val));
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
            auto shift = math::bit_width(val - 1u);
            return static_cast<T>(1u << shift);
        }

        template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
        constexpr T bit_floor(const T val) noexcept {
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
#if defined(_M_IX86)
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
#ifdef _M_IX86
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
#else // ^^^ _M_IX86 / !_M_IX86 vvv
                if (!_BitScanForward64(&result, val)) {
                    return digits;
                }
#endif // _M_IX86
            }
            return static_cast<int>(result);
        }

        template<typename T>
        int _X86_64_countr_zero(const T val) noexcept {
#ifdef __AVX2__
            return math::_Countr_zero_tzcnt(val);
#else // __AVX2__
            const bool _Definitely_have_tzcnt = __isa_available >= __ISA_AVAILABLE_AVX2;
            if (_Definitely_have_tzcnt) {
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
        constexpr int countr_zero(const T val) noexcept {
#if defined(WJR_X86_64) && _HAS_TZCNT_BSF_INTRINSICS
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                return math::_X86_64_countr_zero(val);
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#else
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                return math::_Arm_countr_zero(val);
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#endif
                return math::force_constexpr::countr_zero(val);
        }

#undef _HAS_TZCNT_BSF_INTRINSICS

        template<typename T, std::enable_if_t<_Is_unsigned_standard_integral_v<T>, int> = 0>
        constexpr int countr_one(const T val) noexcept {
            return math::countr_zero(static_cast<T>(~val));
        }

    }

}

#endif
