#ifndef __WJR_MY_STL_H
#define __WJR_MY_STL_H

#include <array>
#include <cassert>
#include <cstdint>
#include <ctime>
#include <functional>
#include <locale>
#include <memory>
#include <optional>
#include <random>
#include <type_traits>
#include <map>

#include "mmacro.h"
#include "../generic/grisu/double.h"

#if defined(WJR_CPP_20)
#include <concepts>
#endif

#ifdef WJR_USE_LIBDIVIDE
#include "libdivide.h"
#else
namespace libdivide {
    enum Branching {
        BRANCHFULL,  // use branching algorithms
        BRANCHFREE   // use branchfree algorithms
    };
    template<typename T, Branching ALGO = BRANCHFULL>
    using divider = T;
}
#endif

namespace wjr {

    enum wbyte_order {
        w_little_endian = 0,
        w_big_endian = 1,
        w_endian = is_little_endian ? w_little_endian : w_big_endian
    };

    inline namespace wjr_type_traits {
        using wjr_ssize_t = std::make_signed_t<size_t>;

        struct wjr_empty_tag {};

        struct wjr_uninitialized_tag {};

        struct wjr_reserved_tag {};

        template<typename T>
        struct midentity {
            using type = T;
        };

        template<typename T>
        using midentity_t = typename midentity<T>::type;

        template<template<typename, typename>typename judger, typename...>
        struct is_any_of_helper {
            static constexpr bool value = false;
        };

        template<template<typename, typename>typename judger, typename T, typename U>
        struct is_any_of_helper<judger, T, U> {
            static constexpr bool value = judger<T, U>::value;
        };

        template<template<typename, typename>typename judger, typename T, typename U, typename... Args>
        struct is_any_of_helper<judger, T, U, Args...> {
            static constexpr bool value =
                std::disjunction_v<
                judger<T, U>,
                is_any_of_helper<judger, T, Args...>>;
        };

        template<typename T, typename...Args>
        struct is_any_of {
            static constexpr bool value = is_any_of_helper<std::is_same, T, Args...>::value;
        };

        template<typename T, typename...args>
        constexpr static bool wjr_is_any_of_v = is_any_of_helper<std::is_same, T, args...>::value;

        template<typename T, typename = void>
        struct wjr_is_iterator : std::false_type {};

        template<typename T>
        struct wjr_is_iterator<T, std::void_t<typename
            std::iterator_traits<T>::iterator_category>> : std::true_type {};

        template<typename T>
        constexpr static bool wjr_is_iterator_v = wjr_is_iterator<T>::value;

#if defined(WJR_CPP_20)
        template<typename T>
        concept wjr_iterator = wjr_is_iterator_v<T>;
#endif

        template<typename T>
        struct is_reverse_iterator : std::false_type {};

        template<typename T>
        struct is_reverse_iterator<std::reverse_iterator<T>> : std::true_type {};

        template<typename T>
		constexpr static bool is_reverse_iterator_v = is_reverse_iterator<T>::value;

#if defined(WJR_CPP_20)
		template<typename T>
		concept wjr_reverse_iterator = is_reverse_iterator_v<T>;
#endif

        template<typename T>
        struct _Is_signed_integral :
            std::conjunction<std::is_integral<T>, std::is_signed<T>> {};

        template<typename T>
        constexpr static bool _Is_signed_integral_v = _Is_signed_integral<T>::value;

        template<typename T>
        struct _Is_unsigned_integral :
            std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {};

        template<typename T>
        constexpr static bool _Is_unsigned_integral_v = _Is_unsigned_integral<T>::value;

        template<typename T>
        struct can_make_unsigned {
			constexpr static bool value = 
                (std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>)
				|| std::is_enum_v<T>;
        };
		
        template<typename T>
		constexpr static bool can_make_unsigned_v = can_make_unsigned<T>::value;

        template<typename T>
        struct forward_wrapper {
            static_assert(!std::is_lvalue_reference_v<T>, "");
            using type = std::remove_reference_t<T>;
        };

        template<typename T>
        struct forward_wrapper<T&> {
            using type = std::reference_wrapper<T>;
        };

        template<typename T>
        using forward_wrapper_t = typename forward_wrapper<T>::type;

        template<typename T>
        constexpr decltype(auto) get_forward_wrapper(T&& val) {
            return static_cast<forward_wrapper_t<T>>(std::forward<T>(val));
        }

        template<typename T, typename = void>
        struct wjr_has_size : std::false_type {};

		template<typename T>
        struct wjr_has_size<T, std::void_t<decltype(std::size(std::declval<T>()))>> : std::true_type{};

        template<typename T>
		constexpr static bool wjr_has_size_v = wjr_has_size<T>::value;

        template<typename T, typename = void>
        struct wjr_has_begin : std::false_type {};

        template<typename T>
        struct wjr_has_begin<T, std::void_t<decltype(std::begin(std::declval<T>()))>> : std::true_type {};

        template<typename T>
        constexpr static bool wjr_has_begin_v = wjr_has_begin<T>::value;

        template<typename T, typename = void>
        struct wjr_has_end : std::false_type {};

        template<typename T>
        struct wjr_has_end<T, std::void_t<decltype(std::end(std::declval<T>()))>> : std::true_type {};

        template<typename T>
        constexpr static bool wjr_has_end_v = wjr_has_end<T>::value;

        template<typename T>
        struct wjr_is_container : std::conjunction<wjr_has_begin<T>, wjr_has_end<T>> {};

        template<typename T>
        constexpr static bool wjr_is_container_v = wjr_is_container<T>::value;

        enum class empty_base_optimize {
            first_empty,
            second_empty,
            non_empty,
            all_empty
        };

        template<typename T, typename U, empty_base_optimize = empty_base_optimize::non_empty>
        class base_mcompressed_pair {
        public:
            template<typename Other1, typename Other2, std::enable_if_t<
                std::is_constructible_v<T, Other1>&& std::is_constructible_v<U, Other2>, int> = 0>
                constexpr base_mcompressed_pair(Other1&& lhs, Other2&& rhs)
                : Myfirst(std::forward<Other1>(lhs)), Mysecond(std::forward<Other2>(rhs)) {
            }
            constexpr T& first() {
                return Myfirst;
            }
            constexpr const T& first()const {
                return Myfirst;
            }
            constexpr U& second() {
                return Mysecond;
            }
            constexpr const U& second()const {
                return Mysecond;
            }
        private:
            T Myfirst;
            U Mysecond;
        };

        template<typename T, typename U>
        class base_mcompressed_pair<T, U, empty_base_optimize::first_empty>
            : private T {
            using base = T;
        public:
            template<typename Other1, typename Other2, std::enable_if_t<
                std::is_constructible_v<T, Other1>&& std::is_constructible_v<U, Other2>, int> = 0>
                constexpr base_mcompressed_pair(Other1&& lhs, Other2&& rhs)
                : base(std::forward<Other1>(lhs)), Mysecond(std::forward<Other2>(rhs)) {
            }
            constexpr T& first() {
                return *this;
            }
            constexpr const T& first()const {
                return *this;
            }
            constexpr U& second() {
                return Mysecond;
            }
            constexpr const U& second()const {
                return Mysecond;
            }
        private:
            U Mysecond;
        };

        template<typename T, typename U>
        class base_mcompressed_pair<T, U, empty_base_optimize::second_empty>
            : private U {
            using base = U;
        public:
            template<typename Other1, typename Other2, std::enable_if_t<
                std::is_constructible_v<T, Other1>&& std::is_constructible_v<U, Other2>, int> = 0>
                constexpr base_mcompressed_pair(Other1&& lhs, Other2&& rhs)
                : Myfirst(std::forward<Other1>(lhs)), base(std::forward<Other2>(rhs)) {
            }
            constexpr T& first() {
                return Myfirst;
            }
            constexpr const T& first()const {
                return Myfirst;
            }
            constexpr U& second() {
                return *this;
            }
            constexpr const U& second()const {
                return *this;
            }
        private:
            T Myfirst;
        };

        template<typename T, typename U>
        using mcompressed_pair = std::conditional_t<std::is_empty_v<T>,
            base_mcompressed_pair<T, U, empty_base_optimize::first_empty>,
            std::conditional_t<std::is_empty_v<U>, base_mcompressed_pair<T, U, empty_base_optimize::second_empty>,
            base_mcompressed_pair<T, U, empty_base_optimize::non_empty>>>;

    }

    inline namespace wjr_hash {
#if defined(_WIN64)
        constexpr static size_t _FNV_offset_basis = 14695981039346656037ULL;
        constexpr static size_t _FNV_prime = 1099511628211ULL;
#else // defined(_WIN64)
        constexpr static size_t _FNV_offset_basis = 2166136261U;
        constexpr static size_t _FNV_prime = 16777619U;
#endif // defined(_WIN64)

        constexpr size_t normal_fnv1a_append_bytes(size_t _Val, const unsigned char* const _First,
            const size_t _Count) noexcept { // accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
            for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
                _Val ^= static_cast<size_t>(_First[_Idx]);
                _Val *= _FNV_prime;
            }
            return _Val;
        }

        template<size_t byte_size, typename const_pointer>
        constexpr size_t constexpr_fnv1a_append_bytes(size_t _Val, const_pointer _First,
            const size_t _Count)noexcept {
            for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
                auto val = static_cast<size_t>(_First[_Idx]);
#if is_little_endian
                for (size_t i = 0; i < byte_size; ++i) {
                    _Val ^= (val >> (i << 3)) & 0xFF;
                    _Val *= _FNV_prime;
                }
#else
                for (size_t i = byte_size - 1; i > 0; ++i) {
                    _Val ^= (val >> (i << 3)) & 0xFF;
                    _Val *= _FNV_prime;
                }
#endif
            }
            return _Val;
        }

        template<typename const_pointer>
        constexpr size_t fnv1a_append_bytes(size_t _Val, const_pointer _First,
            const size_t _Count)noexcept {
            using value_type = typename std::iterator_traits<const_pointer>::value_type;
            constexpr size_t byte_size = sizeof(value_type);
            if constexpr (byte_size >= 1 && byte_size <= sizeof(size_t)) {
                return constexpr_fnv1a_append_bytes<byte_size>(_Val, _First, _Count);
            }
            else {
                return normal_fnv1a_append_bytes(_Val,
                    reinterpret_cast<const unsigned char*>(_First), byte_size * _Count);
            }
        }

        template <class _Ty>
        constexpr size_t fnv1a_append_range(const size_t _Val, const _Ty* const _First,
            const _Ty* const _Last) noexcept { // accumulate range [_First, _Last) into partial FNV-1a hash _Val
            static_assert(std::is_trivial_v<_Ty>, "Only trivial types can be directly hashed.");
            return fnv1a_append_bytes(_Val, _First, static_cast<size_t>(_Last - _First));
        }

        template <class _Kty>
        constexpr size_t fnv1a_append_value(
            const size_t _Val, const _Kty& _Keyval) noexcept { // accumulate _Keyval into partial FNV-1a hash _Val
            static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
            return fnv1a_append_bytes(_Val, reinterpret_cast<const _Kty*>(&_Keyval), 1);
        }

        // FUNCTION TEMPLATE _Hash_representation
        template <class _Kty>
        constexpr size_t hash_representation(const _Kty& _Keyval) noexcept { // bitwise hashes the representation of a key
            return fnv1a_append_value(_FNV_offset_basis, _Keyval);
        }

        // FUNCTION TEMPLATE _Hash_array_representation
        template <class _Kty>
        constexpr size_t hash_array_representation(
            const _Kty* const _First, const size_t _Count) noexcept { // bitwise hashes the representation of an array
            static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
            return fnv1a_append_bytes(
                _FNV_offset_basis, _First, _Count);
        }
    }

    inline namespace wjr_mem {
        constexpr static char white_space[16] = " \n\r\t";

        template<typename T>
        constexpr decltype(auto) get_unsigned_value(T&& val) {
            using origin_type = std::decay_t<T&&>;
            if constexpr (can_make_unsigned_v<origin_type>) {
                return static_cast<std::make_unsigned_t<origin_type>>(std::forward<T>(val));
            }
            else {
                return std::forward<T>(val);
            }
        }

        template<typename T>
        constexpr bool is_whitespace(T&& val) {
            using origin_type = std::decay_t<T&&>;
            auto uval = get_unsigned_value(std::forward<T>(val));
            if constexpr (can_make_unsigned_v<origin_type>) {
                switch (uval) {
                case ' ':
                case '\n':
                case '\r':
                case '\t': return true;
                default:   return false;
                }
            }
            else {
                if (uval == ' ' || uval == '\n' || uval == '\r' || uval == '\t') {
                    return true;
                }
                return false;
            }
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        constexpr void skip_whitespace(iter& first, iter last) {
            while (first != last && is_whitespace(*first))++first;
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        constexpr bool skip_comments(iter& first, iter last) {
            // return false if and only if an error occurs
			if (first == last) return true;
			if (*first == '/') {
				if (++first == last) return false;
				if (*first == '/') {
                    while (first != last && *first != '\n') {
                        ++first;
                    }
                    return true;
				}
                if (*first == '*') {
                    ++first;
                    while (first != last) {
                        if (*first == '*') {
                            if (++first == last) {
                                return false;
                            }
                            if (*first == '/') {
                                ++first;
                                return first;
                            }
                        }
                        else {
                            ++first;
                        }
                    }
                }
                return false;
			}
            return true;
		}

        template<typename T>
        constexpr bool qisdigit(T ch) {
            return (std::integral_constant<T, '0'>() <= ch) && (ch <= std::integral_constant<T, '9'>());
        }

        template<typename T>
        constexpr bool qis_hex_digit(T ch) {
			return qisdigit(ch) 
                || (std::integral_constant<T, 'a'>() <= ch && ch <= std::integral_constant<T, 'f'>())
                || (std::integral_constant<T, 'A'>() <= ch && ch <= std::integral_constant<T, 'F'>());
        }

        template<typename T>
        constexpr  bool qislower(T ch) {
            return (std::integral_constant<T, 'a'>() <= ch) && (ch <= std::integral_constant<T, 'z'>());
        }

        template<typename T>
        constexpr bool qisupper(T ch) {
            return (std::integral_constant<T, 'A'>() <= ch) && (ch <= std::integral_constant<T, 'Z'>());
        }

        template<typename T>
        constexpr bool isdigit_or_sign(T ch) {
            return qisdigit(ch) || (ch == std::integral_constant<T, '+'>()) || (ch == std::integral_constant<T, '-'>());
        }

        constexpr static std::array<uint32_t, 256> _Digit_Map =
        {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,10,11,12,13,
            14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,0,0,
            0,0,0,0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
            31,32,33,34,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        };

        template<typename T>
        constexpr uint32_t get_digit(T ch) { // must judge ch at first
            return _Digit_Map[static_cast<uint8_t>(ch)];
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        constexpr bool check_double(iter& s, iter e) {
            if (s == e) return false;
            if (*s == '-' || *s == '+' )
                ++s;

            if (s == e)
                return false;

            if (*s == '0') {
                ++s;
            }
            else {
                if (!qisdigit(*s)) {
                    return false;
                }
                for (++s; s != e && qisdigit(*s); ++s);
            }

            if (s == e)return true;

            if (*s == '.') {
                if (++s == e)return false;
                if (!qisdigit(*s)) {
                    return false;
                }
                for (++s; s != e && qisdigit(*s); ++s);
                if (s == e)return true;
            }

            if (*s == 'e' || *s == 'E') {
                if (++s == e)return false;
                if (*s == '-' || *s == '+') {
                    if (++s == e)return false;
                }
                if (!qisdigit(*s)) {
                    return false;
                }
                for (++s; s != e && qisdigit(*s); ++s);
            }

            return true;
        }

        template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
        inline double read_double(iter& first, [[maybe_unused]] iter last) {
            bool sgn = true;
            if (*first == '-') {
                sgn = false; ++first;
            }
            else if (*first == '+') {
                ++first;
            }

            uint64_t v = 0;
            int num = 17;
            int pw10 = 0;

            if (*first == '0') {
                ++first;
                if (*first == '.') {
                    ++first;
                    for (; *first == '0'; ++first, --pw10);
                    for (; qisdigit(*first); ++first, --num, --pw10) {
                        if (!num) {
                            for (; qisdigit(*first); ++first);
                            break;
                        }
                        v = v * 10 + (*first - '0');
                    }
                }
            }
            else {
                for (; qisdigit(*first) ; ++first, --num) {
                    if (!num) {
                        for (; qisdigit(*first); ++first, ++pw10);
                        break;
                    }
                    v = v * 10 + (*first - '0');
                }
                if (*first == '.') {
                    ++first;
                    for (; qisdigit(*first); ++first, --num, --pw10) {
                        if (!num) {
                            for (; qisdigit(*first); ++first);
                            break;
                        }
                        v = v * 10 + (*first - '0');
                    }
                }
            }

            if ((*first == 'e') || (*first == 'E')) {
                ++first;
                int pw = 0;
                bool _Sgn = true;
                if (*first == '-') {
                    sgn = false;
                    ++first;
                }
                else if (*first == '+') {
                    ++first;
                }
                for (; qisdigit(*first); ++first) {
                    pw = pw * 10 + (*first - '0');
                }

                pw10 += _Sgn ? pw : -pw;
            }

            diy_fp_t U = uint64_t2diy_fp_t(v);
            diy_fp_t V = cached_power(pw10);
            auto W = multiply(U, V);
            double val = diy_fp2double(W);
            return sgn ? val : -val;
        }

    }
}

#endif
