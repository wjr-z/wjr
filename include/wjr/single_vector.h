#pragma once
#ifndef __WJR_VECTOR_H
#define __WJR_VECTOR_H

#pragma push_macro("new")
#undef new

#include <stdexcept>
#include <string>

#pragma once
#ifndef __WJR_PAIR_H
#define __WJR_PAIR_H

#pragma once
#ifndef __WJR_TYPE_TRAITS_H
#define __WJR_TYPE_TRAITS_H

#include <functional>
#include <limits>
#pragma once
#ifndef __WJR_MMACRO_H
#define __WJR_MMACRO_H

#include <cstdint>

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
/* GCC-compatible compiler, targeting ARM with WMMX */
#include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__))
/* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
#include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
/* GCC-compatible compiler, targeting PowerPC with SPE */
#include <spe.h>
#endif

#if defined(_MSC_VER)
#if !(defined(__AVX__) || defined(__AVX2__))
#if (defined(_M_AMD64) || defined(_M_X64))
#ifndef __SSE2__
#define __SSE2__
#endif // __SSE2__
#elif _M_IX86_FP == 2
#ifndef __SSE2__
#define __SSE2__
#endif // __SSE2__
#elif _M_IX86_FP == 1
#ifndef __SSE__
#define __SSE__
#endif // __SSE__
#endif
#endif
#endif

#if defined ( __AVX512VL__ ) && defined ( __AVX512BW__ ) && defined ( __AVX512DQ__ )
#ifndef __AVX512F__
#define __AVX512F__
#endif
#ifndef __AVX512__
#define __AVX512__
#endif
#endif

#if defined(__AVX512__) || defined(__AVX512F__)
#ifndef __AVX2__
#define __AVX2__
#endif
#endif

#if defined(__AVX2__)
#ifndef __AVX__
#define __AVX__
#endif
#endif

#if defined(__AVX__)
#ifndef __SSE4_2__
#define __SSE4_2__
#endif
#endif

#if defined(__SSE4_2__)
#ifndef __SSE4_1__
#define __SSE4_1__
#endif
#endif

#if defined(__SSE4_1__)
#ifndef __SSSE3__
#define __SSSE3__
#endif
#endif

#if defined(__SSSE3__)
#ifndef __SSE3__
#define __SSE3__
#endif
#endif

#if defined(__SSE3__)
#ifndef __SSE2__
#define __SSE2__
#endif
#endif

#if defined(__SSE2__)
#ifndef __SSE__
#define __SSE__
#endif
#endif

#if defined(__SSE__)
#ifndef __MMX__
#define __MMX__
#endif
#endif

#if defined(__SSE2__)
#define __HAS_FAST_MEMCHR
#define __HAS_FAST_MEMCMP
#define __HAS_FAST_MEMMIS
//#define __HAS_FAST_MEMCNT
#define __HAS_FAST_MEMSET
//#define __HAS_FAST_MEMCPY
#endif // __SSE2__

#if defined(__clang__)
#define WJR_COMPILER_CLANG
#elif defined(__GNUC__)
#define WJR_COMPILER_GCC
#elif defined(_MSC_VER)
#define WJR_COMPILER_MSVC
#endif

#if defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC)
#define WJR_CPP_STANDARD __cplusplus
#elif defined(WJR_COMPILER_MSVC)
#define WJR_CPP_STANDARD _MSVC_LANG
#endif

#define WJR_OVERLAP_P(p1, s1, p2, s2) \
	((p1) + (s1) > (p2) && (p2) + (s2) > (p1))

#define WJR_OVERLAP_P_N(p1, p2, n)	\
	WJR_OVERLAP_P(p1, n, p2, n)

#define WJR_SAME_OR_SEPARATE_P(p1, s1, p2, s2)	\
	((p1) == (p2) || !WJR_OVERLAP_P(p1, s1, p2, s2))

#define WJR_SAME_OR_SEPARATE_P_N(p1, p2, n)	\
	WJR_SAME_OR_SEPARATE_P(p1, n, p2, n)

#define WJR_PRE_OR_SEPARATE_P(p1, s1, p2, s2) \
	((p1) <= (p2) || !WJR_OVERLAP_P(p1, s1, p2, s2))

#define WJR_PRE_OR_SEPARATE_P_N(p1, p2, n)	\
	WJR_PRE_OR_SEPARATE_P(p1, n, p2, n)

#define WJR_POST_OR_SEPARATE_P(p1, s1, p2, s2) \
	((p1) >= (p2) || !WJR_OVERLAP_P(p1, s1, p2, s2))

#define WJR_POST_OR_SEPARATE_P_N(p1, p2, n)	\
	WJR_POST_OR_SEPARATE_P(p1, n, p2, n)

#if WJR_CPP_STANDARD >= 199711L
#define WJR_CPP_03
#endif
#if WJR_CPP_STANDARD >= 201103L
#define WJR_CPP_11
#endif
#if WJR_CPP_STANDARD >= 201402L
#define WJR_CPP_14
#endif
#if WJR_CPP_STANDARD >= 201703L
#define WJR_CPP_17
#endif
#if WJR_CPP_STANDARD >= 202002L
#define WJR_CPP_20
#endif

#if defined(__GNUC__)
#define WJR_HAS_GCC(major, minor, patchlevel)	            \
((__GNUC__ > (major)) ||					                \
	(__GNUC__ == (major) && __GNUC_MINOR__ > (minor)) ||	\
	(__GNUC__ == (major) && __GNUC_MINOR__ == (minor) && __GNUC_PATCHLEVEL__ >= (patchlevel)))
#else
#define WJR_HAS_GCC(major, minor, patchlevel) 0
#endif // __GNUC__

#if defined(__clang__)
#define WJR_HAS_CLANG(major, minor, patchlevel)	                    \
((__clang_major__ > (major)) ||					                    \
	(__clang_major__ == (major) && __clang_minor__ > (minor)) ||	\
	(__clang_major__ == (major) && __clang_minor__ == (minor) && __clang_patchlevel__ >= (patchlevel)))
#else
#define WJR_HAS_CLANG(major, minor, patchlevel) 0
#endif

#if defined(_MSC_VER)
#define WJR_HAS_MSVC(minor, level)	\
(_MSC_VER >= (minor) * 100 + (level))
#else
#define WJR_HAS_MSVC(minor, level) 0
#endif

#if (defined(WJR_COMPILER_GCC) && !WJR_HAS_GCC(7, 1, 0)) ||	\
	(defined(WJR_COMPILER_CLANG) && !WJR_HAS_CLANG(5, 0, 0))	
#error "GCC 7.1.0 or Clang 5.0.0 or later is required"
#endif 

#if (defined(WJR_COMPILER_GCC) && WJR_HAS_GCC(10, 1, 0)) ||	\
	(defined(WJR_COMPILER_CLANG) && WJR_HAS_CLANG(10, 0, 0))	|| \
	(!defined(WJR_COMPILER_GCC) && !defined(WJR_COMPILER_CLANG) && defined(__has_builtin))
#define WJR_HAS_BUILTIN(x) __has_builtin(x)
#else
#define WJR_HAS_BUILTIN(x) 0
#endif 

#if defined(__has_attribute)
#define WJR_HAS_ATTRIBUTE(x) __has_attribute(x)
#elif defined(__has_cpp_attribute)
#define WJR_HAS_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define WJR_HAS_ATTRIBUTE(x) 0
#endif

#if defined(__has_include)
#define WJR_HAS_INCLUDE(x) __has_include(x)
#else
#define WJR_HAS_INCLUDE(x) 0
#endif // __has_include

#if WJR_HAS_INCLUDE(<execution>)
//#define WJR_HAS_EXECUTION
#endif // WJR_HAS_INCLUDE(<execution>)

#ifndef WJR_NODISCARD
#ifndef __has_cpp_attribute
#define WJR_NODISCARD
#elif __has_cpp_attribute(nodiscard) >= 201603L // TRANSITION, VSO#939899 (need toolset update)
#define WJR_NODISCARD [[nodiscard]]
#else
#define WJR_NODISCARD
#endif
#endif

#ifdef WJR_CPP_20
#define WJR_EXPLICIT(expression) explicit(expression)
#define WJR_EXPLICIT20(expreesion) explicit(expression)
#else
#define WJR_EXPLICIT(expression) explicit
#define WJR_EXPLICIT20(expression)
#endif

#if WJR_HAS_ATTRIBUTE(always_inline)
#define WJR_FORCEINLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#else
#define WJR_FORCEINLINE
#endif

#if defined(WJR_FORCEINLINE)
#define WJR_INTRINSIC_INLINE inline WJR_FORCEINLINE
#else
#define WJR_INTRINSIC_INLINE inline
#endif

#if defined(WJR_CPP_20)
#define WJR_CONSTEXPR20 constexpr
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE constexpr
#define WJR_CONSTEVAL20 consteval
#else
#define WJR_CONSTEXPR20 inline
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE
#define WJR_CONSTEVAL20 constexpr
#endif

#define WJR_INTRINSIC_CONSTEXPR WJR_INTRINSIC_INLINE constexpr

#if defined(__x86_64__) || (defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC)))
#define WJR_X86_64
#elif defined(__arm__) || (defined(_M_ARM) || defined(_M_ARM64))
#define WJR_ARM
#endif

#if defined(WJR_ARM)
#error "ARM is not supported"
#endif

// judge if CPU is intel
#if defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__ICL)
#define WJR_INTEL
#endif

// judge if CPU is AMD
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#define WJR_AMD
#endif

#if SIZE_MAX == 0xffffffffffffffffull
#define WJR_X64
#else
#define WJR_X86
#endif

#if defined(WJR_X64)
#define WJR_BYTE_WIDTH 8
#else
#define WJR_BYTE_WIDTH 4
#endif 

#define WJR_CONCAT(x, y) x##y
#define WJR_MACRO_CONCAT(x, y) WJR_CONCAT(x, y)

#define WJR_EXPAND(x) x

#define __WJR_DEFINE_0(x)
#define __WJR_DEFINE_1(x) x
#define WJR_DEFINE(x, y) __WJR_DEFINE_##y (x)

#define WJR_STR(x) #x
#define WJR_MACRO_STR(x) WJR_STR(x)

#define WJR_MACRO_NULL(...)

#define WJR_MACRO_LABEL(NAME) __wjr_label_##NAME

// judge if i can use inline asm
#if defined(WJR_X86_64) && (defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG))
#define WJR_INLINE_ASM
#if defined(WJR_COMPILER_GCC)
#define WJR_BETTER_INLINE_ASM
#endif // WJR_COMPILER_GCC
#endif

#if (defined(WJR_INLINE_ASM) && defined(WJR_INTEL))
#define WJR_ENHANCED_REP
#endif

#define _WJR_BEGIN namespace wjr{
#define _WJR_END }
#define _WJR_SIMD_BEGIN _WJR_BEGIN namespace simd{
#define _WJR_SIMD_END } _WJR_END
#define _WJR_ASM_BEGIN _WJR_BEGIN namespace masm{
#define _WJR_ASM_END } _WJR_END
#define _WJR_ALGO_BEGIN _WJR_BEGIN namespace algo{
#define _WJR_ALGO_END } _WJR_END
#define _WJR_MP_BEGIN _WJR_BEGIN namespace mp{
#define _WJR_MP_END } _WJR_END
#define _WJR_LITERALS_BEGIN _WJR_BEGIN namespace literals{
#define _WJR_LITERALS_END } _WJR_END

#ifndef _WJR_NOEXCEPTION
#define _WJR_TRY try{
#define _WJR_CATCH(x) }catch(x){
#define _WJR_CATCH_ALL }catch(...){
#define _WJR_END_CATCH }
#define _WJR_THROW(x) throw x
#define _WJR_RELEASE throw
#else
#define _WJR_TRY { if constexpr(true){
#define _WJR_CATCH(x) } if constexpr(false){
#define _WJR_CATCH_ALL } if constexpr(false){
#define _WJR_END_CATCH }}
#define _WJR_THROW(x) 
#define _WJR_RELEASE
#endif

#endif                                                                                                                                                                                                                                                                                                                                                                       

_WJR_BEGIN

#define REGISTER_HAS_MEMBER_FUNCTION(FUNC, NAME)								\
template<typename Enable, typename T, typename...Args>							\
struct __has_member_function_##NAME : std::false_type {};						\
template<typename T, typename...Args>											\
struct __has_member_function_##NAME <std::void_t<decltype(						\
	std::declval<T>(). FUNC (std::declval<Args>()...))>, T, Args...>			\
	: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_member_function_##NAME :												\
	__has_member_function_##NAME<void, T, Args...> {};							\
template<typename T, typename...Args>											\
constexpr bool has_member_function_##NAME##_v =									\
	has_member_function_##NAME<T, Args...>::value;

#define REGISTER_HAS_STATIC_MEMBER_FUNCTION(FUNC, NAME)							\
template<typename Enable, typename T, typename...Args>							\
struct __has_static_member_function_##NAME : std::false_type {};				\
template<typename T, typename...Args>											\
struct __has_static_member_function_##NAME <std::void_t<decltype(				\
	T:: FUNC (std::declval<Args>()...))>, T, Args...>							\
	: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_static_member_function_##NAME :										\
	__has_static_member_function_##NAME<void, T, Args...> {};					\
template<typename T, typename...Args>											\
constexpr bool has_static_member_function_##NAME##_v =							\
	has_static_member_function_##NAME<T, Args...>::value;

#define REGISTER_HAS_GLOBAL_FUNCTION(FUNC, NAME)								\
template<typename Enable, typename...Args>										\
struct __has_global_function_##NAME : std::false_type {};						\
template<typename...Args>														\
struct __has_global_function_##NAME <std::void_t<decltype(						\
	FUNC (std::declval<Args>()...))>, Args...>									\
	: std::true_type {};														\
template<typename...Args>														\
struct has_global_function_##NAME :												\
	__has_global_function_##NAME<void, Args...> {};								\
template<typename...Args>														\
constexpr bool has_global_function_##NAME##_v =									\
	has_global_function_##NAME<Args...>::value;

#define REGISTER_HAS_GLOBAL_BINARY_OPERATOR(OP, NAME)							\
template<typename Enable, typename T, typename U>								\
struct __has_global_binary_operator_##NAME : std::false_type {};				\
template<typename T, typename U>												\
struct __has_global_binary_operator_##NAME <std::void_t<decltype(				\
	std::declval<T>() OP std::declval<U>())>, T, U>								\
	: std::true_type {};														\
template<typename T, typename U>												\
struct has_global_binary_operator_##NAME :										\
	__has_global_binary_operator_##NAME<void, T, U> {};							\
template<typename T, typename U>												\
constexpr bool has_global_binary_operator_##NAME##_v =							\
	has_global_binary_operator_##NAME<T, U>::value;								\
template<typename T, typename U>												\
struct has_global_binary_operator<T, U, std:: NAME<>> :							\
	has_global_binary_operator_##NAME<T, U> {};

#define REGISTER_HAS_GLOBAL_UNARY_OPERATOR(OP, NAME)							\
template<typename Enable, typename T>											\
struct __has_global_unary_operator_##NAME : std::false_type {};					\
template<typename T>															\
struct __has_global_unary_operator_##NAME <std::void_t<decltype(				\
	OP std::declval<T>())>, T>													\
	: std::true_type {};														\
template<typename T>															\
struct has_global_unary_operator_##NAME :										\
	__has_global_unary_operator_##NAME<void, T> {};								\
template<typename T>															\
constexpr bool has_global_unary_operator_##NAME##_v =							\
	has_global_unary_operator_##NAME<T>::value;

#define REGISTER_HAS_TYPE(TYPE, NAME)											\
template<typename Enable, typename T>							                \
struct __has_type_##NAME : std::false_type {};				                    \
template<typename T>											                \
struct __has_type_##NAME <std::void_t<decltype(				                    \
	std::declval<typename T::TYPE>())>, T>							            \
	: std::true_type {};														\
template<typename T>											                \
struct has_type_##NAME :										                \
	__has_static_member_function_##NAME<void, T> {};					        \
template<typename T>											                \
constexpr bool has_type_##NAME##_v =							                \
	has_type_##NAME<T>::value;

#define REGISTER_HAS_TYPE_ARGS(TYPE, NAME)										\
template<typename Enable, typename T, typename...Args>							\
struct __has_type_##NAME : std::false_type {};				                    \
template<typename T, typename...Args>											\
struct __has_type_##NAME <std::void_t<decltype(				                    \
	std::declval<typename T::template TYPE<Args...>>())>, T, Args...>			\
	: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_type_##NAME :										                \
	__has_type_##NAME<void, T, Args...> {};					                    \
template<typename T, typename...Args>											\
constexpr bool has_type_##NAME##_v =							                \
	has_type_##NAME<T, Args...>::value;

WJR_INTRINSIC_CONSTEXPR bool is_likely(bool f) noexcept {
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_expect(f, true);
#else
	return f;
#endif
}

WJR_INTRINSIC_CONSTEXPR bool is_unlikely(bool f) noexcept {
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_expect(f, false);
#else
	return f;
#endif
}

WJR_INTRINSIC_CONSTEXPR bool is_constant_evaluated() noexcept {
#if defined(__cpp_lib_is_constant_evaluated) 
	return std::is_constant_evaluated();
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated) || WJR_HAS_GCC(9,1,0) || WJR_HAS_CLANG(9,0,0)
	return __builtin_is_constant_evaluated();
#else
	return false;
#endif
}

WJR_INTRINSIC_INLINE void unreachable() noexcept {
#if defined(_cpp_lib_unreachable)
	return std::unreachable();
#elif WJR_HAS_BUILTIN(__builtin_unreachable) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	__builtin_unreachable();
#elif defined(WJR_COMPILER_MSVC)
	__assume(0);
#endif
}

WJR_INTRINSIC_CONSTEXPR void assume(bool cond) noexcept {
#if WJR_HAS_BUILTIN(__builtin_assume)
	__builtin_assume(cond);
#else
	if (is_unlikely(!cond)) {
		unreachable();
	}
#endif 
}

template<typename T>
WJR_INTRINSIC_CONSTEXPR bool is_constant_p(T x) noexcept {
#if WJR_HAS_BUILTIN(__builtin_constant_p) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_constant_p(x);
#else
	return is_constant_evaluated();
#endif
}

#if defined(__cpp_lib_is_constant_evaluated) || WJR_HAS_BUILTIN(__builtin_is_constant_evaluated) ||		\
	WJR_HAS_GCC(9,1,0) || WJR_HAS_CLANG(9,0,0)
#define WJR_HAS_CONSTANT_EVALUATED
#endif 

#if WJR_HAS_BUILTIN(__builtin_constant_p) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
#define WJR_HAS_CONSTANT_P
#define WJR_HAS_STRONG_CONSTANT_P
#define WJR_HAS_WEAK_CONSTANT_P
#elif defined(WJR_HAS_CONSTANT_EVALUATED)
#define WJR_HAS_WEAK_CONSTANT_P
#endif

struct move_tag {};

struct power_of_two_tag {
	unsigned long long value;
};

struct zero_tag {};

struct disable_tag {};

constexpr size_t byte_width = WJR_BYTE_WIDTH;

template<typename T, typename U, typename _Pred>
struct has_global_binary_operator : std::false_type {};

template<typename T, typename U, typename _Pred>
constexpr bool has_global_binary_operator_v = has_global_binary_operator<T, U, _Pred>::value;

REGISTER_HAS_MEMBER_FUNCTION(operator(), call_operator);
REGISTER_HAS_MEMBER_FUNCTION(operator[], subscript_operator);
REGISTER_HAS_MEMBER_FUNCTION(operator->, point_to_operator);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(+, plus);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(-, minus);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(&, bit_and);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(| , bit_or);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(^, bit_xor);
REGISTER_HAS_GLOBAL_UNARY_OPERATOR(~, bit_not);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(== , equal_to);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(!= , not_equal_to);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(> , greater);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(>= , greater_equal);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(< , less);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(<= , less_equal);
REGISTER_HAS_STATIC_MEMBER_FUNCTION(min, min);
REGISTER_HAS_STATIC_MEMBER_FUNCTION(max, max);

template<typename T, typename...Args>
struct is_any_of : std::disjunction<std::is_same<T, Args>...> {};

template<typename T, typename...Args>
constexpr bool is_any_of_v = is_any_of<T, Args...>::value;

template<typename T>
using remove_ref_t = std::remove_reference_t<T>;

template<typename T>
using remove_cvref_t = std::remove_cv_t<remove_ref_t<T>>;

template<typename T>
using remove_cref_t = std::remove_const_t<remove_ref_t<T>>;

template<typename T>
using add_lref_t = std::add_lvalue_reference_t<T>;

template<typename T>
using add_rref_t = std::add_rvalue_reference_t<T>;

template<typename T>
using add_cvref_t = add_lref_t<std::add_cv_t<T>>;

template<typename T>
using add_cref_t = add_lref_t<std::add_const_t<T>>;

template<typename T>
struct is_standard_comparator :
	is_any_of<remove_cvref_t<T>,
	std::less<>,
	std::less_equal<>,
	std::equal_to<>,
	std::not_equal_to<>,
	std::greater<>,
	std::greater_equal<>
	> {};

template<typename T>
constexpr bool is_standard_comparator_v = is_standard_comparator<T>::value;

template<typename T>
struct is_left_standard_comparator :
	is_any_of<std::remove_cv_t<T>,
	std::less<>,
	std::less_equal<>
	> {};

template<typename T>
constexpr bool is_left_standard_comparator_v = is_left_standard_comparator<T>::value;

template<typename T>
struct is_right_standard_comparator :
	is_any_of<remove_cvref_t<T>,
	std::greater<>,
	std::greater_equal<>
	> {};

template<typename T>
constexpr bool is_right_standard_comparator_v = is_right_standard_comparator<T>::value;

template<typename T>
struct unrefwrap {
	using type = T;
};

template<typename T>
struct unrefwrap<std::reference_wrapper<T>> {
	using type = T&;
};

template<typename T>
using unrefwrap_t = typename unrefwrap<T>::type;

template<typename T>
using iter_cat_t = typename std::iterator_traits<T>::iterator_category;

template<typename T>
using iter_diff_t = typename std::iterator_traits<T>::difference_type;

template<typename T>
using iter_val_t = typename std::iterator_traits<T>::value_type;

template<typename T>
using iter_ref_t = typename std::iterator_traits<T>::reference;

template<typename T, typename = void>
struct is_iterator : std::false_type {};

template<typename T>
struct is_iterator<T, std::void_t<iter_cat_t<T>>> : std::true_type {};

template<typename T>
constexpr bool is_iterator_v = is_iterator<T>::value;

template<typename _Iter, typename = void>
struct _Is_contiguous_iterator_helper : std::false_type {};

template<typename _Iter>
struct _Is_contiguous_iterator_helper<_Iter, typename _Iter::is_contiguous_iterator> : std::true_type {};

template<typename T>
struct _Is_contiguous_iterator_helper<T*, void> : std::true_type {};

#if defined(WJR_CPP_20)
template<typename T>
struct is_contiguous_iterator : std::bool_constant<std::contiguous_iterator<T> || _Is_contiguous_iterator_helper<T>::value> {};
#else
template<typename T>
struct is_contiguous_iterator : _Is_contiguous_iterator_helper<T> {};
#endif

template<typename _Iter>
struct is_contiguous_iterator<std::reverse_iterator<_Iter>> : is_contiguous_iterator<_Iter> {};

template<typename T>
constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<T>::value;

template<typename T>
struct is_input_iter : std::is_convertible<iter_cat_t<T>, std::input_iterator_tag> {};

template<typename T>
constexpr bool is_input_iter_v = is_input_iter<T>::value;

template<typename T>
struct is_output_iter : std::is_convertible<iter_cat_t<T>, std::output_iterator_tag> {};

template<typename T>
constexpr bool is_output_iter_v = is_output_iter<T>::value;

template<typename T>
struct is_forward_iter : std::is_convertible<iter_cat_t<T>, std::forward_iterator_tag> {};

template<typename T>
constexpr bool is_forward_iter_v = is_forward_iter<T>::value;

template<typename T>
struct is_bidir_iter : std::is_convertible<iter_cat_t<T>, std::bidirectional_iterator_tag> {};

template<typename T>
constexpr bool is_bidir_iter_v = is_bidir_iter<T>::value;

template<typename T>
struct is_random_iter : std::is_convertible<iter_cat_t<T>, std::random_access_iterator_tag> {};

template<typename T>
constexpr bool is_random_iter_v = is_random_iter<T>::value;

template<size_t n>
struct __uint_helper {};

template<>
struct __uint_helper<8> {
	using type = unsigned char;
};
template<>
struct __uint_helper<16> {
	using type = unsigned short;
};
template<>
struct __uint_helper<32> {
	using type = std::conditional_t<sizeof(unsigned int) == 4, unsigned int, unsigned long>;
};
template<>
struct __uint_helper<64> {
	using type = unsigned long long;
};

template<size_t n>
using uint_t = typename __uint_helper<n>::type;

template<size_t n>
struct __int_helper {};

template<>
struct __int_helper<8> {
	using type = signed char;
};
template<>
struct __int_helper<16> {
	using type = signed short;
};
template<>
struct __int_helper<32> {
	using type = std::conditional_t<sizeof(int) == 4, int, long>;
};
template<>
struct __int_helper<64> {
	using type = long long;
};

template<size_t n>
using int_t = typename __int_helper<n>::type;

using int8_t = int_t<8>;
using int16_t = int_t<16>;
using int32_t = int_t<32>;
using int64_t = int_t<64>;

using uint8_t = uint_t<8>;
using uint16_t = uint_t<16>;
using uint32_t = uint_t<32>;
using uint64_t = uint_t<64>;

using intptr_t = int_t<sizeof(void*) * 8>;
using uintptr_t = uint_t<sizeof(void*) * 8>;

template<size_t n, bool __s>
using int_or_uint_t = std::conditional_t<__s, int_t<n>, uint_t<n>>;

template<typename T>
struct make_integral {
	static_assert(std::is_integral_v<T>, "T must be an integral type");
	using type = int_or_uint_t<sizeof(T) * 8, std::is_signed_v<T>>;
};

template<typename T>
using make_integral_t = typename make_integral<T>::type;

template<typename T>
constexpr std::make_unsigned_t<T> make_unsigned(T t) {
	return static_cast<std::make_unsigned_t<T>>(t);
}

template<typename T, typename U, typename _Pred>
struct is_comparable : std::conjunction<
	has_global_binary_operator<T, U, _Pred>,
	has_global_binary_operator<U, T, _Pred>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool is_comparable_v = is_comparable<T, U, _Pred>::value;

// note that (int8_t)(-1) != (uint8_t)(-1)
// but in memory they are the same
template<typename T, typename U, bool = sizeof(T) == sizeof(U) && std::is_integral_v<T>&& std::is_integral_v<U>>
constexpr bool __is_memory_comparable_helper_v =
std::is_same_v<T, bool> || std::is_same_v<U, bool> || static_cast<T>(-1) == static_cast<U>(-1);

template<typename T, typename U>
constexpr bool __is_memory_comparable_helper_v<T, U, false> = false;

template<typename T, typename U, typename _Pred>
struct is_memory_comparable : std::conjunction<
	is_standard_comparator<_Pred>,
	is_comparable<T, U, _Pred>,
	std::bool_constant<__is_memory_comparable_helper_v<T, U>>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool is_memory_comparable_v = is_memory_comparable<T, U, _Pred>::value;

enum class ipmc_result {
	none,
	all,
	exit
};

template<typename T, typename U, typename _Pred,
	std::enable_if_t<is_any_of_v<_Pred, std::equal_to<>, std::not_equal_to<>>, int> = 0>
constexpr ipmc_result is_possible_memory_comparable(const U& v, _Pred pred) {
	static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "T and U must be integral types");
	using nt = T;
	using nu = U;
	using cat = std::common_type_t<nt, nu>;
	auto _Val = static_cast<cat>(v);
	if constexpr (std::is_signed_v<nt> && std::is_unsigned_v<cat>) {
		static_assert(static_cast<nt>(-1) == std::numeric_limits<cat>::max(), "error");
		if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
			return ((std::numeric_limits<nt>::min() <= _Val && _Val <= static_cast<nt>(-1))
				|| _Val <= std::numeric_limits<nt>::max()) ? ipmc_result::exit : ipmc_result::none;
		}
		else {
			return (std::numeric_limits<nt>::max() < _Val && _Val < std::numeric_limits<nt>::min()) ?
				ipmc_result::all : ipmc_result::exit;
		}
	}
	else {
		if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
			return (std::numeric_limits<nt>::min() <= _Val && _Val <= std::numeric_limits<nt>::max()) ?
				ipmc_result::exit : ipmc_result::none;
		}
		else {
			return (std::numeric_limits<nt>::max() < _Val || _Val < std::numeric_limits<nt>::min()) ?
				ipmc_result::all : ipmc_result::exit;
		}
	}
}

template<typename T>
struct _Auto_variable_helper {
	using type = std::conditional_t<sizeof(T) <= 256 / 8, T, const T&>;
};

template<typename T>
using auto_var_t = typename _Auto_variable_helper<T>::type;

template<size_t i, size_t..._Index>
struct is_any_index_of : std::disjunction<std::bool_constant<i == _Index>...> {};

template<size_t i, size_t..._Index>
constexpr bool is_any_index_of_v = is_any_index_of<i, _Index...>::value;

template<typename T>
struct ref_wrapper {
	using type = T;
};

template<typename T>
struct ref_wrapper<T&> {
	using type = std::reference_wrapper<T>;
};

template<typename T>
struct ref_wrapper<T&&> {
	using type = T;
};

template<typename T>
using ref_wrapper_t = typename ref_wrapper<T>::type;

template<typename...Args>
struct is_integrals : std::conjunction<std::is_integral<Args>...> {};

template<typename...Args>
constexpr bool is_integrals_v = is_integrals<Args...>::value;

template<typename...Args>
struct is_floating_points : std::conjunction<std::is_floating_point<Args>...> {};

template<typename...Args>
constexpr bool is_floating_points_v = is_floating_points<Args...>::value;

template<typename...Args>
struct is_arithmetics : std::conjunction<std::is_arithmetic<Args>...> {};

template<typename...Args>
constexpr bool is_arithmetics_v = is_arithmetics<Args...>::value;

template<typename T>
struct is_unsigned_integral : std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {};

template<typename T>
constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

template<typename T>
struct is_signed_integral : std::conjunction<std::is_integral<T>, std::is_signed<T>> {};

template<typename T>
constexpr bool is_signed_integral_v = is_signed_integral<T>::value;

template<typename T>
struct is_reverse_iterator : std::false_type {};

template<typename _Iter>
struct is_reverse_iterator<std::reverse_iterator<_Iter>> : std::negation<is_reverse_iterator<_Iter>> {};

template<typename T>
constexpr bool is_reverse_iterator_v = is_reverse_iterator<T>::value;

template<typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
constexpr auto enum_cast(T t) noexcept {
	return static_cast<std::underlying_type_t<T>>(t);
}

namespace enum_ops {
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T> operator|(T lhs, T rhs) noexcept {
		return static_cast<T>(enum_cast(lhs) | enum_cast(rhs));
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T> operator&(T lhs, T rhs) noexcept {
		return static_cast<T>(enum_cast(lhs) & enum_cast(rhs));
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T> operator^(T lhs, T rhs) noexcept {
		return static_cast<T>(enum_cast(lhs) ^ enum_cast(rhs));
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T> operator~(T t) noexcept {
		return static_cast<T>(~enum_cast(t));
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T&> operator|=(T& lhs, T rhs) noexcept {
		return lhs = lhs | rhs;
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T&> operator&=(T& lhs, T rhs) noexcept {
		return lhs = lhs & rhs;
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T&> operator^=(T& lhs, T rhs) noexcept {
		return lhs = lhs ^ rhs;
	}
}

namespace __To_address {
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(to_address, to_address);
}

template<typename T>
constexpr auto to_address(T&& p) noexcept {
	if constexpr (__To_address::has_static_member_function_to_address_v<std::pointer_traits<std::decay_t<T&&>>, T&&>) {
		return std::pointer_traits<std::decay_t<T&&>>::to_address(std::forward<T>(p));
	}
	else if constexpr (wjr::has_member_function_point_to_operator_v<T&&>) {
		return to_address(std::forward<T>(p).operator->());
	}
	else {
		return std::forward<T>(p);
	}
}

template<typename T>
using to_address_t = decltype(to_address(std::declval<T>()));

template<typename T>
struct type_identity {
	using type = T;
};

template<typename T>
using type_identity_t = typename type_identity<T>::type;

template<typename T, typename = void>
struct _Is_default_convertible : std::false_type {};

template<typename T>
void _Test_default_convertible(const T&);

template<typename T>
struct _Is_default_convertible < T, std::void_t<decltype(_Test_default_convertible<T>({})) >> : std::true_type{};

template<typename T>
using is_default_convertible = _Is_default_convertible<T>;

template<typename T>
constexpr bool is_default_convertible_v = is_default_convertible<T>::value;

template<typename T, typename U, typename = void>
struct _Is_swappable_with : std::false_type {};

template<typename T, typename U>
struct _Is_swappable_with<T, U, std::void_t<decltype(std::swap(std::declval<T>(), std::declval<U>()))>>
	: std::true_type {};

template<typename T, typename U>
struct is_swappable_with :
	std::conjunction<_Is_swappable_with<T, U>, _Is_swappable_with<U, T>> {};

template<typename T, typename U>
constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

template<typename T>
struct is_swappable : is_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>> {};

template<typename T>
constexpr bool is_swappable_v = is_swappable<T>::value;

template<typename T, typename U>
struct _Is_nothrow_swappable_with : std::bool_constant<noexcept(std::swap(std::declval<T>(), std::declval<U>()))
	&& noexcept(std::swap(std::declval<U>(), std::declval<T>()))> {};

template<typename T, typename U>
struct is_nothrow_swappable_with :
	std::conjunction<is_swappable_with<T, U>, _Is_nothrow_swappable_with<T, U>> {};

template<typename T, typename U>
constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;

template<typename T>
struct is_nothrow_swappable :
	is_nothrow_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>> {};

template<typename T>
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
struct __make_iter_wrapper : public std::tuple<iter, iter> {
	using _Mybase = std::tuple<iter, iter>;
	using _Mybase::_Mybase;
	using _Mybase::operator=;

	constexpr iter begin() const noexcept { return std::get<0>(*this); }
	constexpr iter end() const noexcept { return std::get<1>(*this); }

	constexpr decltype(auto) size() const noexcept {
		return std::distance(begin(), end());
	}

};

template<typename iter>
constexpr __make_iter_wrapper<iter> make_iter_wrapper(iter first, iter last) {
	return { first, last };
}

template<typename T>
using alloc_ptr_t = typename std::allocator_traits<T>::pointer;

template<typename T>
using alloc_value_t = typename std::allocator_traits<T>::value_type;

template<typename T>
using alloc_size_t = typename std::allocator_traits<T>::size_type;

template<typename T>
using alloc_diff_t = typename std::allocator_traits<T>::difference_type;

// The default allocator uses the default constructor and destructor, which is beneficial to optimization.
// But if the constructor and destructor of the allocator are not default, then the allocator cannot be used.
template<typename T>
struct is_default_allocator : std::false_type {};

template<typename T>
struct is_default_allocator<std::allocator<T>> : std::true_type {};

template<typename T>
constexpr bool is_default_allocator_v = is_default_allocator<T>::value;

REGISTER_HAS_MEMBER_FUNCTION(destroy, destroy);
REGISTER_HAS_MEMBER_FUNCTION(construct, construct);

// Use the default destructor, which is beneficial for optimization.
// But if the destructor of the allocator has side effects, then do not use this allocator.

template<typename Alloc, typename Ptr, typename...Args>
struct is_default_allocator_construct : std::disjunction<is_default_allocator<Alloc>,
	std::negation<has_member_function_construct<Alloc, Ptr, Args...>>> {};

template<typename Alloc, typename Ptr, typename...Args>
constexpr bool is_default_allocator_construct_v = is_default_allocator_construct<Alloc, Ptr, Args...>::value;

template<typename Alloc, typename _Ptr>
struct is_default_allocator_destroy : std::disjunction<is_default_allocator<Alloc>,
	std::negation<has_member_function_destroy<Alloc, _Ptr>>> {};

template<typename Alloc, typename _Ptr>
constexpr bool is_default_allocator_destroy_v = is_default_allocator_destroy<Alloc, _Ptr>::value;

_WJR_END

namespace std {
	REGISTER_HAS_GLOBAL_FUNCTION(begin, begin);
	REGISTER_HAS_GLOBAL_FUNCTION(cbegin, cbegin);
	REGISTER_HAS_GLOBAL_FUNCTION(end, end);
	REGISTER_HAS_GLOBAL_FUNCTION(cend, cend);
	REGISTER_HAS_GLOBAL_FUNCTION(rbegin, rbegin);
	REGISTER_HAS_GLOBAL_FUNCTION(crbegin, crbegin);
	REGISTER_HAS_GLOBAL_FUNCTION(rend, rend);
	REGISTER_HAS_GLOBAL_FUNCTION(crend, crend);
	REGISTER_HAS_GLOBAL_FUNCTION(data, data);
	REGISTER_HAS_GLOBAL_FUNCTION(size, size);
	REGISTER_HAS_MEMBER_FUNCTION(size, size);
	REGISTER_HAS_GLOBAL_FUNCTION(swap, swap);
}

_WJR_BEGIN

template<typename _Container>
constexpr auto size(const _Container& c) {
	if constexpr (std::has_member_function_size_v<_Container>) {
		return c.size();
	}
	else {
		return std::distance(std::begin(c), std::end(c));
	}
}

template<typename _Ty, size_t _Size>
constexpr size_t size(const _Ty(&)[_Size]) noexcept {
	return _Size;
}

_WJR_END

#endif                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       

_WJR_BEGIN

// using empty base optimization

template<size_t index, typename T>
struct _Pair_wrapper1 {
	template<typename _Ty = T, std::enable_if_t<std::is_default_constructible_v<_Ty>, int> = 0>
	constexpr _Pair_wrapper1() noexcept(std::is_nothrow_default_constructible_v<_Ty>)
		: val() {}
	template<typename _Ty = T, std::enable_if_t<std::is_copy_constructible_v<_Ty>, int> = 0>
	constexpr _Pair_wrapper1(const _Ty& other) noexcept(std::is_nothrow_copy_constructible_v<_Ty>)
		: val(other) {}
	template<typename _Ty = T, std::enable_if_t<std::is_move_constructible_v<_Ty>, int> = 0>
	constexpr _Pair_wrapper1(_Ty&& other) noexcept(std::is_nothrow_move_constructible_v<_Ty>)
		: val(std::forward<_Ty>(other)) {}
	template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
	constexpr _Pair_wrapper1(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
		: val(std::forward<Args>(args)...) {}
	_Pair_wrapper1(const _Pair_wrapper1&) = default;
	_Pair_wrapper1(_Pair_wrapper1&&) noexcept = default;
	_Pair_wrapper1& operator=(const _Pair_wrapper1&) = default;
	_Pair_wrapper1& operator=(_Pair_wrapper1&&) noexcept = default;
	constexpr T& value() noexcept { return val; }
	constexpr const T& value() const noexcept { return val; }
private:
	T val;
};

template<size_t index, typename T>
struct _Pair_wrapper2 : private T {
	using _Mybase = T;
	template<typename _Ty = T, std::enable_if_t<std::is_default_constructible_v<_Ty>, int> = 0>
	constexpr _Pair_wrapper2() noexcept(std::is_nothrow_default_constructible_v<_Ty>)
		: _Mybase() {}
	template<typename _Ty = T, std::enable_if_t<std::is_copy_constructible_v<_Ty>, int> = 0>
	constexpr _Pair_wrapper2(const _Ty& other) noexcept(std::is_nothrow_copy_constructible_v<_Ty>)
		: _Mybase(other) {}
	template<typename _Ty = T, std::enable_if_t<std::is_move_constructible_v<_Ty>, int> = 0>
	constexpr _Pair_wrapper2(_Ty&& other) noexcept(std::is_nothrow_move_constructible_v<_Ty>)
		: _Mybase(std::forward<_Ty>(other)) {}
	template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
	constexpr _Pair_wrapper2(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
		: _Mybase(std::forward<Args>(args)...) {}
	_Pair_wrapper2(const _Pair_wrapper2&) = default;
	_Pair_wrapper2(_Pair_wrapper2&&) noexcept = default;
	_Pair_wrapper2& operator=(const _Pair_wrapper2&) = default;
	_Pair_wrapper2& operator=(_Pair_wrapper2&&) noexcept = default;
	constexpr T& value() noexcept { return *this; }
	constexpr const T& value() const noexcept { return *this; }
};

template<size_t index, typename T>
using _Pair_wrapper = std::conditional_t<
	std::conjunction_v<std::is_class<T>, std::is_empty<T>, std::negation<std::is_final<T>>>,
	_Pair_wrapper2<index, T>,
	_Pair_wrapper1<index, T>
>;

template<typename T, typename U>
class pair : private _Pair_wrapper<0, T>, private _Pair_wrapper<1, U> {

	template<typename _Ty, typename _Uy>
	using _Is_default_constructible = std::conjunction <
		std::is_default_constructible<_Ty>,
		std::is_default_constructible<_Uy>>;
	template<typename _Ty, typename _Uy>
	using _Is_default_convertible = std::conjunction <
		is_default_convertible<_Ty>,
		is_default_convertible<_Uy>>;

	template<typename _Ty, typename _Uy>
	using _Is_copy_constructible = std::conjunction <
		std::is_copy_constructible<_Ty>,
		std::is_copy_constructible<_Uy>>;

	template<typename _Ty, typename _Uy, typename _Vty, typename _Wuy>
	using _Is_convertible = std::conjunction <
		std::is_convertible<_Vty, _Ty>,
		std::is_convertible<_Wuy, _Uy>>;

	template<typename _Ty, typename _Uy, typename _Vty, typename _Wuy>
	using _Is_constructible = std::conjunction<
		std::is_constructible<_Ty, _Vty>,
		std::is_constructible<_Uy, _Wuy>>;

public:
	using _Mybase1 = _Pair_wrapper<0, T>;
	using _Mybase2 = _Pair_wrapper<1, U>;

	using first_type = T;
	using second_type = U;

	template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
		std::conjunction_v<_Is_default_constructible<_Ty, _Uy>,
		_Is_default_convertible<_Ty, _Uy>>, bool> = true>
		constexpr pair() noexcept(std::conjunction_v<std::is_nothrow_default_constructible<_Ty>,
			std::is_nothrow_default_constructible<_Uy>>)
		: _Mybase1(), _Mybase2() {}

	template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
		std::conjunction_v<_Is_default_constructible<_Ty, _Uy>,
		std::negation<_Is_default_convertible<_Ty, _Uy>>>, bool> = false>
	constexpr explicit pair() noexcept(std::conjunction_v<std::is_nothrow_default_constructible<_Ty>,
		std::is_nothrow_default_constructible<_Uy>>)
		: _Mybase1(), _Mybase2() {}

	template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
		std::conjunction_v<_Is_copy_constructible<_Ty, _Uy>,
		_Is_convertible<_Ty, _Uy, const _Ty&, const _Uy&>>, bool> = true>
		constexpr pair(const T& _First, const U& _Second)
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<_Ty>, std::is_nothrow_copy_constructible<_Uy>>)
		: _Mybase1(_First), _Mybase2(_Second) {}

	template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
		std::conjunction_v<_Is_copy_constructible<_Ty, _Uy>,
		std::negation<_Is_convertible<_Ty, _Uy, const _Ty&, const _Uy&>>>, bool> = false>
	constexpr explicit pair(const T& _First, const U& _Second)
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<_Ty>, std::is_nothrow_copy_constructible<_Uy>>)
		: _Mybase1(_First), _Mybase2(_Second) {}

	template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
		_Is_convertible<T, U, _Other1, _Other2>>, bool> = true>
		constexpr pair(_Other1&& _First, _Other2&& _Second)
		noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
		: _Mybase1(std::forward<_Other1>(_First)), _Mybase2(std::forward<_Other2>(_Second)) {}

	template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
		std::negation<_Is_convertible<T, U, _Other1, _Other2>>>, bool> = false>
	constexpr explicit pair(_Other1&& _First, _Other2&& _Second)
		noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
		: _Mybase1(std::forward<_Other1>(_First)), _Mybase2(std::forward<_Other2>(_Second)) {}

	pair(const pair&) = default;
	pair(pair&&) noexcept = default;

	template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<_Is_constructible<T, U, const _Other1&, const _Other2&>,
		_Is_convertible<T, U, const _Other1&, const _Other2&>>, bool> = true>
		constexpr pair(const pair<_Other1, _Other2>& other)
		noexcept(std::conjunction_v<std::is_nothrow_constructible<T, const _Other1&>, std::is_nothrow_constructible<U, const _Other2&>>)
		: _Mybase1(other.first()), _Mybase2(other.second()) {}

	template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<_Is_constructible<T, U, const _Other1&, const _Other2&>,
		std::negation<_Is_convertible<T, U, const _Other1&, const _Other2&>>>, bool> = false>
	constexpr explicit pair(const pair<_Other1, _Other2>& other)
		noexcept(std::conjunction_v<std::is_nothrow_constructible<T, const _Other1&>, std::is_nothrow_constructible<U, const _Other2&>>)
		: _Mybase1(other.first()), _Mybase2(other.second()) {}

	template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
		_Is_convertible<T, U, _Other1, _Other2>>, bool> = true>
		constexpr pair(pair<_Other1, _Other2>&& other)
		noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
		: _Mybase1(std::forward<_Other1>(other.first())), _Mybase2(std::forward<_Other2>(other.second())) {}

	template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
		std::negation<_Is_convertible<T, U, _Other1, _Other2>>>, bool> = false>
	constexpr explicit pair(pair<_Other1, _Other2>&& other)
		noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
		: _Mybase1(std::forward<_Other1>(other.first())), _Mybase2(std::forward<_Other2>(other.second())) {}

	template<typename _Tuple1, typename _Tuple2, size_t...N1, size_t...N2>
	constexpr pair(_Tuple1& tp1, _Tuple2& tp2, std::index_sequence<N1...>, std::index_sequence<N2...>)
		: _Mybase1(std::get<N1>(std::move(tp1))...), _Mybase2(std::get<N2>(std::move(tp2))...) {}

	template<typename...Args1, typename...Args2>
	constexpr pair(std::piecewise_construct_t, std::tuple<Args1...> tp1, std::tuple<Args2...> tp2)
		: pair(tp1, tp2, std::index_sequence_for<Args1...>{}, std::index_sequence_for<Args2...>{}) {}

	constexpr pair&
		operator=(std::conditional_t<std::conjunction_v<std::is_copy_assignable<T>,
			std::is_copy_assignable<U>>, const pair&, const wjr::disable_tag&> other)
		noexcept(std::conjunction_v<std::is_nothrow_copy_assignable<T>, std::is_nothrow_copy_assignable<U>>) {
		first() = other.first();
		second() = other.second();
		return *this;
	}

	constexpr pair&
		operator=(std::conditional_t<std::conjunction_v<std::is_move_assignable<T>,
			std::is_move_assignable<U>>, pair&&, wjr::disable_tag&&> other)
		noexcept(std::conjunction_v<std::is_nothrow_move_assignable<T>, std::is_nothrow_move_assignable<U>>) {
		first() = std::forward<T>(other.first());
		second() = std::forward<U>(other.second());
		return *this;
	}

	template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<std::negation<std::is_same<pair, pair<_Other1, _Other2>>>,
		std::is_assignable<T&, const _Other1&>,
		std::is_assignable<U&, const _Other2&>>, int> = 0>
		constexpr pair& operator=(const pair<_Other1, _Other2>& other)
		noexcept(std::conjunction_v<std::is_nothrow_assignable<T&, const _Other1&>,
			std::is_nothrow_assignable<U&, const _Other2&>>) {
		first() = other.first();
		second() = other.second();
		return *this;
	}

	template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<std::negation<std::is_same<pair, pair<_Other1, _Other2>>>,
		std::is_assignable<T&, _Other1>,
		std::is_assignable<U&, _Other2>>, int> = 0>
		constexpr pair& operator=(pair<_Other1, _Other2>&& other)
		noexcept(std::conjunction_v<std::is_nothrow_assignable<T&, _Other1>,
			std::is_nothrow_assignable<U&, _Other2>>) {
		first() = std::forward<_Other1>(other.first());
		second() = std::forward<_Other2>(other.second());
		return *this;
	}

	constexpr void swap(pair& other)
		noexcept(std::conjunction_v<is_nothrow_swappable<T>, is_nothrow_swappable<U>>) {
		using std::swap;
		swap(first(), other.first());
		swap(second(), other.second());
	}

	constexpr T& first() noexcept { return _Mybase1::value(); }
	constexpr const T& first() const noexcept { return _Mybase1::value(); }
	constexpr U& second() noexcept { return _Mybase2::value(); }
	constexpr const U& second() const noexcept { return _Mybase2::value(); }
};

#ifdef WJR_CPP17
template<typename T, typename U>
pair(T, U) -> pair<T, U>;
#endif

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator==(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return lhs.first() == rhs.first() && lhs.second() == rhs.second();
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator!=(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return !(lhs == rhs);
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator<(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return lhs.first() < rhs.first() || (!(rhs.first() < lhs.first()) && lhs.second() < rhs.second());
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator>(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return rhs < lhs;
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator<=(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return !(rhs < lhs);
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator>=(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return !(lhs < rhs);
}

template<typename T, typename U>
WJR_NODISCARD constexpr pair<unrefwrap_t<T>, unrefwrap_t<U>> make_pair(T&& t, U&& u)
noexcept(std::conjunction_v<std::is_nothrow_constructible<unrefwrap_t<T>, T>,
	std::is_nothrow_constructible<unrefwrap_t<U>, U>>) {
	return pair<unrefwrap_t<T>, unrefwrap_t<U>>(std::forward<T>(t), std::forward<U>(u));
}

_WJR_END

namespace std {

	template<typename T, typename U>
	using _Wjr_pair = wjr::pair<T, U>;

	template<typename T, typename U, enable_if_t<conjunction_v<wjr::is_swappable<T>, wjr::is_swappable<U>>, int> = 0>
	void swap(_Wjr_pair<T, U>& lhs, _Wjr_pair<T, U>& rhs)
		noexcept(noexcept(lhs.swap(rhs))) {
		lhs.swap(rhs);
	}
	template<typename T, typename U>
	struct tuple_size<_Wjr_pair<T, U>> : integral_constant<size_t, 2> {};

	template<size_t I, typename T, typename U>
	struct tuple_element<I, _Wjr_pair<T, U>> {
		static_assert(I < 2, "tuple_element index out of bounds");
		using type = conditional_t<I == 0, T, U>;
	};

	template<typename _Ret, typename _Pair>
	WJR_NODISCARD constexpr _Ret& _Wjr_pair_get(_Pair& p, integral_constant<size_t, 0>) noexcept {
		return p.first();
	}

	template<typename _Ret, typename _Pair>
	WJR_NODISCARD constexpr _Ret& _Wjr_pair_get(_Pair& p, integral_constant<size_t, 1>) noexcept {
		return p.second();
	}

	template<size_t I, typename T, typename U>
	WJR_NODISCARD constexpr tuple_element_t<I, _Wjr_pair<T, U>>& get(_Wjr_pair<T, U>& p) noexcept {
		using _Ret = tuple_element_t<I, _Wjr_pair<T, U>>&;
		return _Wjr_pair_get<_Ret>(p, integral_constant<size_t, I>{});
	}

	template<typename T, typename U>
	WJR_NODISCARD constexpr T& get(_Wjr_pair<T, U>& p) noexcept {
		return get<0>(p);
	}

	template<typename U, typename T>
	WJR_NODISCARD constexpr U& get(_Wjr_pair<T, U>& p) noexcept {
		return get<1>(p);
	}

	template<size_t I, typename T, typename U>
	WJR_NODISCARD constexpr const tuple_element_t<I, _Wjr_pair<T, U>>& get(const _Wjr_pair<T, U>& p) noexcept {
		using _Ret = const tuple_element_t<I, _Wjr_pair<T, U>>&;
		return _Wjr_pair_get<_Ret>(p, integral_constant<size_t, I>{});
	}

	template<typename T, typename U>
	WJR_NODISCARD constexpr const T& get(const _Wjr_pair<T, U>& p) noexcept {
		return get<0>(p);
	}

	template<typename U, typename T>
	WJR_NODISCARD constexpr const U& get(const _Wjr_pair<T, U>& p) noexcept {
		return get<1>(p);
	}


	template <size_t I, typename T, typename U>
	WJR_NODISCARD constexpr tuple_element_t<I, _Wjr_pair<T, U>>&& get(
		_Wjr_pair<T, U>&& p) noexcept {
		using _RRtype = tuple_element_t<I, _Wjr_pair<T, U>>&&;
		return forward<_RRtype>(get<I>(p));
	}

	template <typename T, typename U>
	WJR_NODISCARD constexpr T&& get(_Wjr_pair<T, U>&& p) noexcept {
		return get<0>(move(p));
	}

	template <typename U, typename T>
	WJR_NODISCARD constexpr U&& get(_Wjr_pair<T, U>&& p) noexcept {
		return get<1>(move(p));
	}

	template <size_t I, typename T, typename U>
	WJR_NODISCARD constexpr const tuple_element_t<I, _Wjr_pair<T, U>>&& get(
		const _Wjr_pair<T, U>&& p) noexcept {
		using _RRtype = const tuple_element_t<I, _Wjr_pair<T, U>>&&;
		return forward<_RRtype>(get<I>(p));
	}

	template <typename T, typename U>
	WJR_NODISCARD constexpr const T&& get(
		const _Wjr_pair<T, U>&& p) noexcept {
		return get<0>(move(p));
	}

	template <typename U, typename T>
	WJR_NODISCARD constexpr const U&& get(
		const _Wjr_pair<T, U>&& p) noexcept {
		return get<1>(move(p));
	}

}

#endif // __WJR_COMPRESSED_PAIR_H                                                                                                                                                                                                                                                                                                                                                                                          
#pragma once
#ifndef __WJR_ALGORITHM_H__
#define __WJR_ALGORITHM_H__

#include <algorithm>
#include <memory>
#include <limits>
#include <cstring>

#pragma once
#ifndef __WJR_ALGO_ALOG_H
#define __WJR_ALGO_ALOG_H

#pragma once
#pragma once
#include <array>


_WJR_BEGIN

template<size_t N>
using byte_array = std::array<uint8_t, N>;

template<typename T>
constexpr byte_array<sizeof(T)> to_byte_array(const T& _Val) {
	auto ptr = reinterpret_cast<const uint8_t*>(&_Val);
	byte_array<sizeof(T)> ret;
	for (size_t i = 0; i < sizeof(T); ++i) {
		ret[i] = ptr[i];
	}
	return ret;
}

template<size_t M, size_t N, std::enable_if_t<is_any_index_of_v<M, 1, 2, 4, 8>, int> = 0>
constexpr uint_t<M * 8> to_uint(const byte_array<N>& _Val) {
	return *reinterpret_cast<const uint_t<M * 8>*>(_Val.data());
}

_WJR_END

_WJR_BEGIN

template<size_t N>
constexpr size_t _Get_max_bytes_num() {
	if constexpr (N % 8 == 0)return 8;
	else if constexpr (N % 4 == 0)return 4;
	else if constexpr (N % 2 == 0)return 2;
	else return 1;
}

template<size_t N>
size_t _Get_bytes_num(const byte_array<N>& a) {
	constexpr size_t M = _Get_max_bytes_num<N>();
	auto ptr = a.data();
	if constexpr (N > 8) {
		if (!is_constant_p(a)) {
			return 0;
		}
		else {
			for (size_t i = 0; i < N - M; ++i) {
				if (ptr[i] != ptr[i + M]) {
					return 0;
				}
			}
			for (size_t i = 0; i < M - 1; ++i) {
				if (ptr[i] != ptr[i + 1]) {
					return M;
				}
			}
			return 1;
		}
	}
	if constexpr (M == 1) {
		return 1;
	}
	else if constexpr (M == 2) {
		return (ptr[0] == ptr[1]) ? 1 : 2;
	}
	else if constexpr (M == 3) {
		return (*reinterpret_cast<const uint16_t*>(ptr) == *reinterpret_cast<const uint16_t*>(ptr + 1)) ? 1 : 0;
	}
	else if constexpr (M == 4) {
		auto x = *reinterpret_cast<const uint32_t*>(ptr);
		return ((x >> 8) == (x & 0x00FFFFFF)) ? 1 : 4;
	}
	else if constexpr (M == 5) {
		return (*reinterpret_cast<const uint32_t*>(ptr) == *reinterpret_cast<const uint32_t*>(ptr + 1)) ? 1 : 0;
	}
	else if constexpr (M == 6) {
		return (*reinterpret_cast<const uint32_t*>(ptr) == *reinterpret_cast<const uint32_t*>(ptr + 2)) ? 2 : 6;
	}
	else if constexpr (M == 7) {
		auto x = *reinterpret_cast<const uint32_t*>(ptr);
		auto y = *reinterpret_cast<const uint32_t*>(ptr + 4);
		return ((x >> 8) == (x & 0x00FFFFFF)) && ((y >> 8) == (y & 0x00FFFFFF)) ? 1 : 0;
	}
	else {
		auto x = *reinterpret_cast<const uint64_t*>(ptr);
		return ((x >> 8) == (x & 0x00FFFFFFFFFFFFFF)) ? 1 : 8;
	}
}

template<typename T>
struct is_bit_default_constructible : std::is_trivially_default_constructible<T> {};

template<typename T, typename U,
	bool =
	std::is_same_v<bool, std::remove_reference_t<U>> >= std::is_same_v<bool, T> &&
	((std::is_integral_v<T>&& std::is_integral_v<std::remove_reference_t<U>>) ||
		(std::is_floating_point_v<T> && std::is_floating_point_v<std::remove_reference_t<U>>)) &&
	!std::is_volatile_v<T> &&
	!std::is_volatile_v<std::remove_reference_t<U>>
>
struct __is_byte_constructible {
	constexpr static bool is_copy = sizeof(T) == sizeof(U);
	constexpr static bool is_fill = true;
	constexpr static byte_array<sizeof(T)> get(const U& src) {
		return to_byte_array(static_cast<T>(src));
	}
};

template<typename T, bool =
	std::is_trivially_copyable_v<T> &&
	!std::is_volatile_v<T>>
	struct __is_byte_copy_constructible_helper {
	constexpr static bool is_copy = true;
	constexpr static bool is_fill = !std::is_empty_v<T>;
	constexpr static byte_array<sizeof(T)> get(const T& src) {
		return to_byte_array(src);
	}
};

template<typename T>
struct __is_byte_copy_constructible_helper<T, false> {
	constexpr static bool is_copy = false;
	constexpr static bool is_fill = false;
};

template<typename T, bool =
	std::is_trivially_copyable_v<T> &&
	!std::is_volatile_v<T>>
	struct __is_byte_move_constructible_helper {
	constexpr static bool is_copy = true;
	constexpr static bool is_fill = !std::is_empty_v<T>;
	constexpr static byte_array<sizeof(T)> get(const T& src) {
		return to_byte_array(src);
	}
};

template<typename T>
struct __is_byte_move_constructible_helper<T, false> {
	constexpr static bool is_copy = false;
	constexpr static bool is_fill = false;
};

template<typename T, typename U>
struct __is_byte_constructible<T, U, false> {
	constexpr static bool is_copy = false;
	constexpr static bool is_fill = false;
};

template<typename T>
struct __is_byte_constructible<T, T, false> : __is_byte_copy_constructible_helper<T> {};

template<typename T>
struct __is_byte_constructible<T, const T, false> : __is_byte_copy_constructible_helper<T> {};

template<typename T>
struct __is_byte_constructible<T, T&, false> : __is_byte_copy_constructible_helper<T> {};

template<typename T>
struct __is_byte_constructible<T, const T&, false> : __is_byte_copy_constructible_helper<T> {};

template<typename T>
struct __is_byte_constructible<T, T&&, false> : __is_byte_move_constructible_helper<T> {};

template<typename T, typename U>
struct is_byte_constructible : __is_byte_constructible<std::remove_reference_t<T>, U> {};

template<typename T, typename U>
struct is_byte_assignable : __is_byte_constructible<std::remove_reference_t<T>, U> {};

template<typename T>
struct is_bit_destructible : std::is_trivially_destructible<T> {};

_WJR_END

#pragma once
#ifndef __WJR_ALGO_MEMCHR_H__
#define __WJR_ALGO_MEMCHR_H__

#pragma once



_WJR_LITERALS_BEGIN

constexpr unsigned long long operator"" _KiB(unsigned long long n) {
	return n * 1024;
}

constexpr unsigned long long operator"" _MiB(unsigned long long n) {
	return n * 1024 * 1024;
}

constexpr unsigned long long operator"" _GiB(unsigned long long n) {
	return n * 1024 * 1024 * 1024;
}

_WJR_LITERALS_END
#pragma once
#ifndef __WJR_SIMD_SIMD_INTRIN_H
#define __WJR_SIMD_SIMD_INTRIN_H

#pragma once
#ifndef __WJR_MATH_H__
#define __WJR_MATH_H__

#pragma once
#ifndef __WJR_ASM_ASM_H
#define __WJR_ASM_ASM_H

#pragma once
#ifndef __WJR_ASM_ADC_H
#define __WJR_ASM_ADC_H



_WJR_ASM_BEGIN

#if WJR_HAS_BUILTIN(__builtin_addc) || WJR_HAS_CLANG(5, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_builtin_adc(T a, T b, T carry_in, T* carry_out) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= _Nd_ub) {
		unsigned char CF = 0;
		T ret = __builtin_addcb(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_us) {
		unsigned short CF = 0;
		T ret = __builtin_addcs(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned int CF = 0;
		T ret = __builtin_addc(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned long CF = 0;
		T ret = __builtin_addcl(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long CF = 0;
		T ret = __builtin_addcll(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_msvc_adc(T a, T b, T carry_in, T* carry_out) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	//constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= _Nd_ub) {
		unsigned char ret = 0;
		*carry_out = _addcarry_u8(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_us) {
		unsigned short ret = 0;
		*carry_out = _addcarry_u16(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned int ret = 0;
		*carry_out = _addcarry_u32(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long ret = 0;
		*carry_out = _addcarry_u64(carry_in, a, b, &ret);
		return ret;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 static T __adc(T a, T b, T carry_in, T* carry_out) {
	if (!wjr::is_constant_evaluated()) {
		if (!((is_constant_p(a) && is_constant_p(b)) || (is_constant_p(carry_in) && carry_in == 0))) {
#if defined(WJR_COMPILER_MSVC)
			return __wjr_msvc_adc(a, b, carry_in, carry_out);
#elif defined(WJR_INLINE_ASM)
#if defined(WJR_BETTER_INLINE_ASM)
			asm("add $255, %b0\n\t"
				"adc %3, %2\n\t"
				"setb %b1"
				: "+r"(carry_in), "=rm"(*carry_out), "+%r"(a)
				: "rm"(b)
				: "cc");
#else
			asm("add $255, %b0\n\t"
				"adc %3, %2\n\t"
				"setb %b1"
				: "+r"(carry_in), "=r"(*carry_out), "+%r"(a)
				: "r"(b)
				: "cc");
			return a;
#endif // WJR_BETTER_INLINE_ASM
#endif
		}
	}
	T c = 0;
	a += b;
	c = a < b;
	a += carry_in;
	c |= a < carry_in;
	*carry_out = c;
	return a;
}

WJR_INTRINSIC_CONSTEXPR20 uint8_t adc(uint8_t a, uint8_t b, uint8_t carry_in, uint8_t* carry_out) {
	return __adc(a, b, carry_in, carry_out);
}
WJR_INTRINSIC_CONSTEXPR20 uint16_t adc(uint16_t a, uint16_t b, uint16_t carry_in, uint16_t* carry_out) {
	return __adc(a, b, carry_in, carry_out);
}
WJR_INTRINSIC_CONSTEXPR20 uint32_t adc(uint32_t a, uint32_t b, uint32_t carry_in, uint32_t* carry_out) {
	return __adc(a, b, carry_in, carry_out);
}
#if defined(WJR_X64)
WJR_INTRINSIC_CONSTEXPR20 uint64_t adc(uint64_t a, uint64_t b, uint64_t carry_in, uint64_t* carry_out) {
	return __adc(a, b, carry_in, carry_out);
}
#endif // WJR_X64

_WJR_ASM_END

#endif // __WJR_ASM_ADC_H                                                                                                                                              
#pragma once
#ifndef __WJR_ASM_SBB_H
#define __WJR_ASM_SBB_H



_WJR_ASM_BEGIN

#if WJR_HAS_BUILTIN(__builtin_subc) || WJR_HAS_CLANG(5, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_builtin_sbb(T a, T b, T carry_in, T* carry_out) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= _Nd_ub) {
		unsigned char CF = 0;
		T ret = __builtin_subcb(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_us) {
		unsigned short CF = 0;
		T ret = __builtin_subcs(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned int CF = 0;
		T ret = __builtin_subc(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned long CF = 0;
		T ret = __builtin_subcl(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long CF = 0;
		T ret = __builtin_subcll(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_msvc_sbb(T a, T b, T carry_in, T* carry_out) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	//constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= _Nd_ub) {
		unsigned char ret = 0;
		*carry_out = _subborrow_u8(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_us) {
		unsigned short ret = 0;
		*carry_out = _subborrow_u16(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned int ret = 0;
		*carry_out = _subborrow_u32(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long ret = 0;
		*carry_out = _subborrow_u64(carry_in, a, b, &ret);
		return ret;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 static T __sbb(T a, T b, T carry_in, T* carry_out) {
	if (!wjr::is_constant_evaluated()) {
		if (!((is_constant_p(a) && is_constant_p(b)) || (is_constant_p(carry_in) && carry_in == 0))) {
#if defined(WJR_COMPILER_MSVC)
			return __wjr_msvc_sbb(a, b, carry_in, carry_out);
#elif defined(WJR_INLINE_ASM) // Clang does not need inline assembly
#if defined(WJR_BETTER_INLINE_ASM)
			asm("add $255, %b0\n\t"
				"sbb %3, %2\n\t"
				"setb %b1"
				: "+r"(carry_in), "=rm"(*carry_out), "+r"(a)
				: "rm"(b)
				: "cc");
#else
			asm("add $255, %b0\n\t"
				"sbb %3, %2\n\t"
				"setb %b1"
				: "+r"(carry_in), "=r"(*carry_out), "+r"(a)
				: "r"(b)
				: "cc");
			return a;
#endif // WJR_BETTER_INLINE_ASM
#endif
		}
	}
	T c = 0;
	c = a < b;
	a -= b;
	c |= a < carry_in;
	a -= carry_in;
	*carry_out = c;
	return a;
}

WJR_INTRINSIC_CONSTEXPR20 uint8_t sbb(uint8_t a, uint8_t b, uint8_t carry_in, uint8_t* carry_out) {
	return __sbb(a, b, carry_in, carry_out);
}
WJR_INTRINSIC_CONSTEXPR20 uint16_t sbb(uint16_t a, uint16_t b, uint16_t carry_in, uint16_t* carry_out) {
	return __sbb(a, b, carry_in, carry_out);
}
WJR_INTRINSIC_CONSTEXPR20 uint32_t sbb(uint32_t a, uint32_t b, uint32_t carry_in, uint32_t* carry_out) {
	return __sbb(a, b, carry_in, carry_out);
}
#if defined(WJR_X64)
WJR_INTRINSIC_CONSTEXPR20 uint64_t sbb(uint64_t a, uint64_t b, uint64_t carry_in, uint64_t* carry_out) {
	return __sbb(a, b, carry_in, carry_out);
}
#endif // WJR_X64

_WJR_ASM_END

#endif // __WJR_ASM_SBB_H                                                                                                                                              
#pragma once
#ifndef __WJR_ASM_CLZ_H__
#define __WJR_ASM_CLZ_H__



_WJR_ASM_BEGIN

const static int _WJR_LOG_TABLE[256] = {
		0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
};

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR static int __wjr_fallback_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}

	int n = 0;
	// use _Table

	if constexpr (_Nd > 32) {
		if (x >> 32) {
			n += 32;
			x >>= 32;
		}
	}
	if constexpr (_Nd > 16) {
		if (x >> 16) {
			n += 16;
			x >>= 16;
		}
	}
	if constexpr (_Nd > 8) {
		if (x >> 8) {
			n += 8;
			x >>= 8;
		}
	}
	return  _Nd - (n + _WJR_LOG_TABLE[x]);
}

#if WJR_HAS_BUILTIN(__builtin_clz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_INTRINSIC_INLINE static int __wjr_builtin_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;

	if constexpr (_Nd <= _Nd_ui) {
		constexpr auto __diff = _Nd_ui - _Nd;
		return __builtin_clz(x) - __diff;
	}
	else if constexpr (_Nd <= _Nd_ul) {
		constexpr auto __diff = _Nd_ul - _Nd;
		return __builtin_clzl(x) - __diff;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		constexpr auto __diff = _Nd_ull - _Nd;
		return __builtin_clzll(x) - __diff;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "countl_zero is not implemented for this type");
	}
}
#elif defined(WJR_COMPILER_MSVC)
#if defined(WJR_X86_64)
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
#if defined(__AVX2__)
	if constexpr (_Nd <= 16) {
		return static_cast<int>(__lzcnt16(x) - (16 - _Nd));
	}
	else if constexpr (_Nd <= 32) {
		return static_cast<int>(__lzcnt(x));
	}
	else {
#if defined(WJR_X86)
		const auto xh = static_cast<unsigned int>(x >> 32);
		const auto xl = static_cast<unsigned int>(x);
		if (xh == 0) {
			return 32 + static_cast<int>(__lzcnt(xl));
		}
		else {
			return static_cast<int>(__lzcnt(xh));
		}
#else
		return static_cast<int>(__lzcnt64(x));
#endif
	}
#else
	unsigned long _Result;
	if constexpr (_Nd <= 32) {
		if (!_BitScanReverse(&_Result, x)) {
			return _Nd;
		}
	}
	else {
#if defined(WJR_X86)
		const auto xh = static_cast<unsigned int>(x >> 32);
		if (_BitScanReverse(&_Result, xh)) {
			return static_cast<int>(31 - _Result);
		}
		const auto xl = static_cast<unsigned int>(x);
		if (!_BitScanReverse(&_Result, xl)) {
			return _Nd;
		}
#else
		if (!_BitScanReverse64(&_Result, x)) {
			return _Nd;
		}
#endif
	}
	return static_cast<int>(_Nd - 1 - _Result);
#endif 
}
#elif defined(WJR_ARM)
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_INLINE static int __wjr_msvc_arm_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}
	if constexpr (_Nd <= 32) {
		return static_cast<int>(_CountLeadingZeros(x));
	}
	else {
		return static_cast<int>(_CountLeadingZeros64(x));
	}
}
#endif
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int clz(T x) noexcept {
	if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_clz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
		return __wjr_builtin_clz(x);
#elif defined(WJR_COMPILER_MSVC)
#if defined(WJR_X86_64)
		return __wjr_msvc_x86_64_clz(x);
#elif defined(WJR_ARM)
		return __wjr_msvc_arm_clz(x);
#endif
#endif
	}
	return __wjr_fallback_clz(x);
}

_WJR_ASM_END

#endif // __WJR_ASM_CLZ_H__                                                                                                                                                                             
#pragma once
#ifndef __WJR_ASM_CTZ_H__
#define __WJR_ASM_CTZ_H__



_WJR_ASM_BEGIN

#if WJR_HAS_BUILTIN(__builtin_ctz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_INTRINSIC_INLINE static int __wjr_builtin_ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;

	if constexpr (_Nd <= _Nd_ui) {
		return __builtin_ctz(x);
	}
	else if constexpr (_Nd <= _Nd_ul) {
		return __builtin_ctzl(x);
	}
	else if constexpr (_Nd <= _Nd_ull) {
		return __builtin_ctzll(x);
	}
	else {
		static_assert(_Nd <= _Nd_ull, "countr_zero is not implemented for this type");
	}
}
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
    && !defined(__INTEL_COMPILER)
template<typename T>
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	constexpr T _Max = std::numeric_limits<T>::max();
#if defined(__AVX2__)
	if constexpr (_Nd <= 32) {
		return static_cast<int>(_tzcnt_u32(static_cast<unsigned int>(~_Max | x)));
	}
	else {
#if defined(WJR_X86)
		const auto xh = static_cast<unsigned int>(x >> 32);
		const auto xl = static_cast<unsigned int>(x);
		if (xh == 0) {
			return 32 + static_cast<int>(_tzcnt_u32(xl));
		}
		else {
			return static_cast<int>(_tzcnt_u32(xh));
		}
#else
		return static_cast<int>(_tzcnt_u64(x));
#endif // WJR_X86
	}
#else
	unsigned long _Result;
	if constexpr (_Nd <= 32) {
		if (!_BitScanForward(&_Result, x)) {
			return _Nd;
		}
	}
	else {
#if defined(WJR_X86)
		const auto xl = static_cast<unsigned int>(x);
		if (_BitScanForward(&_Result, xl)) {
			return static_cast<int>(_Result);
		}
		const auto xh = static_cast<unsigned int>(x >> 32);
		if (!_BitScanForward(&_Result, xh)) {
			return _Nd;
		}
		else {
			return static_cast<int>(32 + _Result);
		}

#else
		if (!_BitScanForward64(&_Result, x)) {
			return _Nd;
		}
#endif
	}
	return static_cast<int>(_Result);
#endif
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_ctz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
		return __wjr_builtin_ctz(x);
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
	&& !defined(__INTEL_COMPILER)
		return __wjr_msvc_x86_64_ctz(x);
#endif
	}
	return _Nd - clz(static_cast<T>(static_cast<T>(~x) & static_cast<T>(x - 1)));
}

_WJR_ASM_END

#endif // __WJR_ASM_CTZ_H__                                                                                                         
#pragma once
#ifndef __WJR_ASM_POPCNT_H__
#define __WJR_ASM_POPCNT_H__



_WJR_ASM_BEGIN

template<typename _Ty>
WJR_INTRINSIC_CONSTEXPR static int __wjr_fallback_popcount(_Ty _Val) noexcept {
	constexpr int _Digits = std::numeric_limits<_Ty>::digits;
#if defined(WJR_X86) || defined(WJR_ARM)
	if constexpr (_Digits == 64) {
		// 64-bit bit operations on architectures without 64-bit registers are less efficient,
		// hence we split the value so that it fits in 32-bit registers
		return _Popcount_fallback(static_cast<unsigned long>(_Val))
			+ _Popcount_fallback(static_cast<unsigned long>(_Val >> 32));
	}
#endif // defined(WJR_X86) || defined(WJR_ARM)
	// we static_cast these bit patterns in order to truncate them to the correct size
	_Val = static_cast<_Ty>(_Val - ((_Val >> 1) & static_cast<_Ty>(0x5555'5555'5555'5555ull)));
	_Val = static_cast<_Ty>((_Val & static_cast<_Ty>(0x3333'3333'3333'3333ull))
		+ ((_Val >> 2) & static_cast<_Ty>(0x3333'3333'3333'3333ull)));
	_Val = static_cast<_Ty>((_Val + (_Val >> 4)) & static_cast<_Ty>(0x0F0F'0F0F'0F0F'0F0Full));
	// Multiply by one in each byte, so that it will have the sum of all source bytes in the highest byte
	_Val = static_cast<_Ty>(_Val * static_cast<_Ty>(0x0101'0101'0101'0101ull));
	// Extract highest byte
	return static_cast<int>(_Val >> (_Digits - 8));
}

#if WJR_HAS_BUILTIN(__builtin_popcount) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_INTRINSIC_INLINE static int __wjr_builtin_popcount(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;

	if constexpr (_Nd <= _Nd_ui) {
		return __builtin_popcount(static_cast<unsigned>(x));
	}
	else if constexpr (_Nd <= _Nd_ul) {
		return __builtin_popcountl(static_cast<unsigned long>(x));
	}
	else if constexpr (_Nd <= _Nd_ull) {
		return __builtin_popcountll(static_cast<unsigned long long>(x));
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}

}
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
	&& !defined(__INTEL_COMPILER)
template <typename T>
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_popcount(T x) noexcept {
	constexpr int _Digits = std::numeric_limits<T>::digits;
	if constexpr (_Digits <= 16) {
		return static_cast<int>(__popcnt16(x));
	}
	else if constexpr (_Digits == 32) {
		return static_cast<int>(__popcnt(x));
	}
	else {
#ifdef WJR_X86
		return static_cast<int>(__popcnt(x >> 32) + __popcnt(static_cast<unsigned int>(x)));
#else
		return static_cast<int>(__popcnt64(x));
#endif // WJR_X86
	}
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int popcnt(T x) noexcept {
	if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_popcount) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
		return __wjr_builtin_popcount(x);
#elif defined(_MSC_VER) && defined(WJR_X86_64) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
	&& !defined(__INTEL_COMPILER)
		return __wjr_msvc_x86_64_popcount(x);
#endif 
	}
	return __wjr_fallback_popcount(x);
}

_WJR_ASM_END

#endif // __WJR_ASM_POPCNT_H__                                                                                         

#endif // __WJR_ASM_ASM_H          

_WJR_BEGIN

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
constexpr bool has_single_bit(T x) noexcept {
	return x != 0 && (x & (x - 1)) == 0;
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int countl_zero(T x) noexcept {
	return wjr::masm::clz(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int countl_one(T x) noexcept {
	return wjr::countl_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int countr_zero(T x) noexcept {
	return wjr::masm::ctz(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int countr_one(T x) noexcept {
	return wjr::countr_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int bit_width(T x) noexcept {
	return std::numeric_limits<T>::digits - wjr::countl_zero(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 T bit_floor(T x) noexcept {
	if (x != 0) {
		return static_cast<T>(T{ 1 } << (wjr::bit_width(x) - 1));
	}
	return T{ 0 };
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 T bit_ceil(T x) noexcept {
	if (x <= 1) {
		return T{ 1 };
	}
	return static_cast<T>(T{ 1 } << wjr::bit_width(x - 1));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int popcount(T x) noexcept {
	return wjr::masm::popcnt(x);
}

template< typename T, typename U >
constexpr bool cmp_equal(T t, U u) noexcept
{
	using UT = std::make_unsigned_t<T>;
	using UU = std::make_unsigned_t<U>;
	if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
		return t == u;
	else if constexpr (std::is_signed_v<T>)
		return t < 0 ? false : UT(t) == u;
	else
		return u < 0 ? false : t == UU(u);
}

template< typename T, typename U >
constexpr bool cmp_not_equal(T t, U u) noexcept {
	return !cmp_equal(t, u);
}

template< typename T, typename U >
constexpr bool cmp_less(T t, U u) noexcept {
	using UT = std::make_unsigned_t<T>;
	using UU = std::make_unsigned_t<U>;
	if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
		return t < u;
	else if constexpr (std::is_signed_v<T>)
		return t < 0 ? true : UT(t) < u;
	else
		return u < 0 ? false : t < UU(u);
}

template< typename T, typename U >
constexpr bool cmp_greater(T t, U u) noexcept {
	return wjr::cmp_less(u, t);
}

template< typename T, typename U >
constexpr bool cmp_less_equal(T t, U u) noexcept {
	return !wjr::cmp_greater(t, u);
}

template< typename T, typename U >
constexpr bool cmp_greater_equal(T t, U u) noexcept {
	return !wjr::cmp_less(t, u);
}

template< typename R, typename T>
constexpr bool in_range(T t) noexcept {
	return wjr::cmp_greater_equal(t, std::numeric_limits<R>::min()) &&
		wjr::cmp_less_equal(t, std::numeric_limits<R>::max());
}

template<typename R, typename T>
struct _Broadcast;

template<typename R, typename T>
inline _Broadcast<R, T> broadcast{};

template<>
struct _Broadcast<uint8_t, uint8_t> {
	constexpr uint8_t operator()(uint8_t x)const {
		return x;
	}
};
template<>
struct _Broadcast<uint16_t, uint16_t> {
	constexpr uint16_t operator()(uint16_t x)const {
		return x;
	}
};
template<>
struct _Broadcast<uint32_t, uint32_t> {
	constexpr uint32_t operator()(uint32_t x)const {
		return x;
	}
};
template<>
struct _Broadcast<uint64_t, uint64_t> {
	constexpr uint64_t operator()(uint64_t x)const {
		return x;
	}
};

template<>
struct _Broadcast<uint16_t, uint8_t> {
	constexpr uint16_t operator()(uint8_t x)const {
		return x | ((uint16_t)x << 8);
	}
};

template<>
struct _Broadcast<uint32_t, uint16_t> {
	constexpr uint32_t operator()(uint16_t x)const {
		return x | ((uint32_t)x << 16);
	}
};
template<>
struct _Broadcast<uint64_t, uint32_t> {
	constexpr uint64_t operator()(uint32_t x)const {
		return x | ((uint64_t)x << 32);
	}
};

template<>
struct _Broadcast<uint32_t, uint8_t> {
	constexpr uint32_t operator()(uint8_t x)const {
		return x * (uint32_t)0x01010101;
	}
};
template<>
struct _Broadcast<uint64_t, uint16_t> {
	constexpr uint64_t operator()(uint16_t x)const {
		return x * (uint64_t)0x0001000100010001;
	}
};

template<>
struct _Broadcast<uint64_t, uint8_t> {
	constexpr uint64_t operator()(uint8_t x)const {
		return x * (uint64_t)0x0101010101010101;
	}
};

_WJR_END

#endif // __WJR_MATH_H__                                                                                                                                                                                       
#pragma once
#ifndef __WJR_SIMD_SIMD_CAST_H__
#define __WJR_SIMD_SIMD_CAST_H__



_WJR_SIMD_BEGIN

struct lo {};
struct hi {};

template<typename T, typename U>
WJR_INTRINSIC_INLINE T simd_cast(U);

/* TODO */
template<typename T1, typename T2, typename U>
WJR_INTRINSIC_INLINE U simd_cast(U, T2);

#if defined(__SSE__)
#endif // __SSE__

#if defined(__SSE2__)

template<>
WJR_INTRINSIC_INLINE __m128 simd_cast<__m128>(__m128i v) {
	return _mm_castsi128_ps(v);
}
template<>
WJR_INTRINSIC_INLINE __m128i simd_cast<__m128i>(__m128 v) {
	return _mm_castps_si128(v);
}
template<>
WJR_INTRINSIC_INLINE __m128d simd_cast<__m128d>(__m128 v) {
	return _mm_castps_pd(v);
}
template<>
WJR_INTRINSIC_INLINE __m128 simd_cast<__m128>(__m128d v) {
	return _mm_castpd_ps(v);
}
template<>
WJR_INTRINSIC_INLINE __m128i simd_cast<__m128i>(__m128d v) {
	return _mm_castpd_si128(v);
}
template<>
WJR_INTRINSIC_INLINE __m128d simd_cast<__m128d>(__m128i v) {
	return _mm_castsi128_pd(v);
}

#define __WJR_REGISTER_SIMD_CAST_WITH_INTEGER(IT, SUF)	            \
template<>	                                                        \
WJR_INTRINSIC_INLINE IT simd_cast<IT>(__m128i v){	                \
	return static_cast<IT>(_mm_cvtsi128_si##SUF(v));	            \
}	                                                                \
template<>	                                                        \
WJR_INTRINSIC_INLINE __m128i simd_cast<__m128i>(IT v){	            \
	return _mm_cvtsi##SUF##_si128(v);	                            \
}

__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int8_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int16_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int32_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int64_t, 64);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint8_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint16_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint32_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint64_t, 64);

#undef __WJR_REGISTER_SIMD_CAST_WITH_INTEGER

#endif // __SSE2__

#if defined(__AVX__)

template<>
WJR_INTRINSIC_INLINE __m256 simd_cast<__m256>(__m256i v) {
	return _mm256_castsi256_ps(v);
}
template<>
WJR_INTRINSIC_INLINE __m256i simd_cast<__m256i>(__m256 v) {
	return _mm256_castps_si256(v);
}
template<>
WJR_INTRINSIC_INLINE __m256d simd_cast<__m256d>(__m256 v) {
	return _mm256_castps_pd(v);
}
template<>
WJR_INTRINSIC_INLINE __m256 simd_cast<__m256>(__m256d v) {
	return _mm256_castpd_ps(v);
}
template<>
WJR_INTRINSIC_INLINE __m256i simd_cast<__m256i>(__m256d v) {
	return _mm256_castpd_si256(v);
}
template<>
WJR_INTRINSIC_INLINE __m256d simd_cast<__m256d>(__m256i v) {
	return _mm256_castsi256_pd(v);
}

template<>
WJR_INTRINSIC_INLINE __m256i simd_cast<__m256i>(__m128i v) {
	return _mm256_castsi128_si256(v);
}

template<>
WJR_INTRINSIC_INLINE __m128i simd_cast<__m128i>(__m256i v) {
	return _mm256_castsi256_si128(v);
}

#define __WJR_REGISTER_SIMD_CAST_WITH_INTEGER(IT, SUF)	            \
template<>	                                                        \
WJR_INTRINSIC_INLINE IT simd_cast<IT>(__m256i v){	                \
	return simd_cast<IT>(simd_cast<__m128i>(v));					\
}	                                                                \
template<>	                                                        \
WJR_INTRINSIC_INLINE __m256i simd_cast<__m256i>(IT v){	            \
	return simd_cast<__m256i>(simd_cast<__m128i>(v));				\
}

__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int8_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int16_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int32_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int64_t, 64);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint8_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint16_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint32_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint64_t, 64);

#undef __WJR_REGISTER_SIMD_CAST_WITH_INTEGER

#endif // __AVX__

_WJR_SIMD_END

#endif // __WJR_SIMD_SIMD_CAST_H__                                                                                                                                     

_WJR_BEGIN

#if defined(__SSE2__)
template<>
struct _Broadcast<__m128i, uint8_t> {
	__m128i operator()(uint8_t v) const {
		return _mm_set1_epi8(v);
	}
};
template<>
struct _Broadcast<__m128i, uint16_t> {
	__m128i operator()(uint16_t v) const {
		return _mm_set1_epi16(v);
	}
};
template<>
struct _Broadcast<__m128i, uint32_t> {
	__m128i operator()(uint32_t v) const {
		return _mm_set1_epi32(v);
	}
};
template<>
struct _Broadcast<__m128i, uint64_t> {
	__m128i operator()(uint64_t v) const {
		return _mm_set1_epi64x(v);
	}
};
#endif // __SSE2__

#if defined(__AVX__)
template<>
struct _Broadcast<__m256i, uint8_t> {
	__m256i operator()(uint8_t v) const {
		return _mm256_set1_epi8(v);
	}
};
template<>
struct _Broadcast<__m256i, uint16_t> {
	__m256i operator()(uint16_t v) const {
		return _mm256_set1_epi16(v);
	}
};
template<>
struct _Broadcast<__m256i, uint32_t> {
	__m256i operator()(uint32_t v) const {
		return _mm256_set1_epi32(v);
	}
};
template<>
struct _Broadcast<__m256i, uint64_t> {
	__m256i operator()(uint64_t v) const {
		return _mm256_set1_epi64x(v);
	}
};
template<>
struct _Broadcast<__m256i, __m128i> {
	__m256i operator()(__m128i v) const {
#if defined(__AVX2__)
		return _mm256_broadcastsi128_si256(v);
#else
		return _mm256_insertf128_si256(_mm256_castsi128_si256(v), v, 1);
#endif // __AVX2__
	}
};

#endif // __AVX__

_WJR_END

_WJR_SIMD_BEGIN

#if defined(__SSE2__)

WJR_INTRINSIC_INLINE static __m128i mm_loadu_si16(void const* p) {
	return simd_cast<__m128i>(*reinterpret_cast<uint16_t const*>(p));
}

WJR_INTRINSIC_INLINE static __m128i mm_loadu_si32(void const* p) {
	return simd_cast<__m128i>(*reinterpret_cast<int32_t const*>(p));
}

WJR_INTRINSIC_INLINE static __m128i mm_loadu_si64(void const* p) {
	return simd_cast<__m128i>(*reinterpret_cast<int64_t const*>(p));
}

#endif // __SSE2__

#undef __TEST_SIMD_FUNCTION

struct sse {
	using mask_type = uint16_t;
#if defined(__SSE__)
	using float_type = __m128;
#endif // __SSE__
#if defined(__SSE2__)
	using int_type = __m128i;
	using double_type = __m128d;
#endif // __SSE2__

	constexpr static size_t width() { return 128; }

	constexpr static mask_type mask() { return 0xFFFF; }

#if defined(__SSE__)
	WJR_INTRINSIC_INLINE static mask_type movemask_ps(__m128 v) {
		return _mm_movemask_ps(v);
	}

	WJR_INTRINSIC_INLINE static void sfence() { return _mm_sfence(); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128 shuffle_ps(__m128 a, __m128 b) {
		static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
		return _mm_shuffle_ps(a, b, imm8);
	}
#endif // __SSE__

#if defined(__SSE2__)

	WJR_INTRINSIC_INLINE static __m128i add_epi8(__m128i a, __m128i b) { return _mm_add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add_epi16(__m128i a, __m128i b) { return _mm_add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add_epi32(__m128i a, __m128i b) { return _mm_add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add_epi64(__m128i a, __m128i b) { return _mm_add_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int8_t) { return add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int16_t) { return add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int32_t) { return add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int64_t) { return add_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint8_t) { return add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint16_t) { return add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint32_t) { return add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint64_t) { return add_epi64(a, b); }

	WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m128i a) {
		auto b = shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a);
		a = add_epi8(a, b);
		b = zeros();
		a = sad_epu8(a, b);
		return simd_cast<uint8_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m128i a) {
		a = add_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = add_epi16(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a));
		a = add_epi16(a, srli<2>(a));
		return simd_cast<uint16_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m128i a) {
		a = add_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = add_epi32(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a));
		return simd_cast<uint32_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m128i a) {
		a = add_epi64(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		return simd_cast<uint64_t>(a);
	}
	WJR_INTRINSIC_INLINE static int8_t add_epi8(__m128i a) { return add_epu8(a); }
	WJR_INTRINSIC_INLINE static int16_t add_epi16(__m128i a) { return add_epu16(a); }
	WJR_INTRINSIC_INLINE static int32_t add_epi32(__m128i a) { return add_epu32(a); }
	WJR_INTRINSIC_INLINE static int64_t add_epi64(__m128i a) { return add_epu64(a); }

	WJR_INTRINSIC_INLINE static int8_t add(__m128i a, int8_t) { return add_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t add(__m128i a, int16_t) { return add_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t add(__m128i a, int32_t) { return add_epi32(a); }
	WJR_INTRINSIC_INLINE static int64_t add(__m128i a, int64_t) { return add_epi64(a); }
	WJR_INTRINSIC_INLINE static uint8_t add(__m128i a, uint8_t) { return add_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t add(__m128i a, uint16_t) { return add_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t add(__m128i a, uint32_t) { return add_epu32(a); }
	WJR_INTRINSIC_INLINE static uint64_t add(__m128i a, uint64_t) { return add_epu64(a); }

	WJR_INTRINSIC_INLINE static __m128i adds_epi8(__m128i a, __m128i b) { return _mm_adds_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds_epi16(__m128i a, __m128i b) { return _mm_adds_epi16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i adds_epu8(__m128i a, __m128i b) { return _mm_adds_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds_epu16(__m128i a, __m128i b) { return _mm_adds_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int8_t) { return adds_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int16_t) { return adds_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint8_t) { return adds_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint16_t) { return adds_epu16(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b) {
		constexpr int s = imm8 & 0x1F;
#if defined(__SSSE3__)
		return _mm_alignr_epi8(a, b, s);
#else
		if constexpr (s == 0) {
			return b;
		}
		if constexpr (s == 16) {
			return a;
		}
		if constexpr (s < 16) {
			return Or(slli<16 - s>(a), srli<s>(b));
		}
		return srli<s - 16>(a);
#endif // __SSSE3__
	}

	WJR_INTRINSIC_INLINE static __m128i alignl_epi16(__m128i a, __m128i b, int c) {
		return Or(slli_epi16(a, c), srli_epi16(b, 16 - c));
	}
	WJR_INTRINSIC_INLINE static __m128i alignl_epi32(__m128i a, __m128i b, int c) {
		return Or(slli_epi32(a, c), srli_epi32(b, 32 - c));
	}
	WJR_INTRINSIC_INLINE static __m128i alignl_epi64(__m128i a, __m128i b, int c) {
		return Or(slli_epi64(a, c), srli_epi64(b, 64 - c));
	}

	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, int16_t) {
		return alignl_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, int32_t) {
		return alignl_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, int64_t) {
		return alignl_epi64(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, uint16_t) {
		return alignl_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, uint32_t) {
		return alignl_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, uint64_t) {
		return alignl_epi64(a, b, c);
	}

	WJR_INTRINSIC_INLINE static __m128i alignr_epi16(__m128i a, __m128i b, int c) {
		return Or(slli_epi16(a, 16 - c), srli_epi16(b, c));
	}
	WJR_INTRINSIC_INLINE static __m128i alignr_epi32(__m128i a, __m128i b, int c) {
		return Or(slli_epi32(a, 32 - c), srli_epi32(b, c));
	}
	WJR_INTRINSIC_INLINE static __m128i alignr_epi64(__m128i a, __m128i b, int c) {
		return Or(slli_epi64(a, 64 - c), srli_epi64(b, c));
	}

	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int16_t) {
		return alignr_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int32_t) {
		return alignr_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int64_t) {
		return alignr_epi64(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint16_t) {
		return alignr_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint32_t) {
		return alignr_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint64_t) {
		return alignr_epi64(a, b, c);
	}

	WJR_INTRINSIC_INLINE static __m128i And(__m128i a, __m128i b) { return _mm_and_si128(a, b); }

	WJR_INTRINSIC_INLINE static __m128i AndNot(__m128i a, __m128i b) { return _mm_andnot_si128(a, b); }

	WJR_INTRINSIC_INLINE static __m128i avg_epu8(__m128i a, __m128i b) { return _mm_avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i avg_epu16(__m128i a, __m128i b) { return _mm_avg_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int8_t) { return avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int16_t) { return avg_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint8_t) { return avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint16_t) { return avg_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i blendv_epi8(__m128i a, __m128i b, __m128i mask) {
#if defined(__SSE4_1__)
		return _mm_blendv_epi8(a, b, mask);
#else
		return Or(AndNot(mask, a), And(mask, b));
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static __m128i blendv_epi16(__m128i a, __m128i b, __m128i mask) {
		return blendv_epi8(b, a, logical_not(mask, uint16_t()));
	}
	WJR_INTRINSIC_INLINE static __m128i blendv_epi32(__m128i a, __m128i b, __m128i mask) {
		return blendv_epi8(b, a, logical_not(mask, uint32_t()));
	}

	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int8_t) {
		return blendv_epi8(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int16_t) {
		return blendv_epi16(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int32_t) {
		return blendv_epi32(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint8_t) {
		return blendv_epi8(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint16_t) {
		return blendv_epi16(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint32_t) {
		return blendv_epi32(a, b, mask);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i bslli(__m128i val) { return _mm_bslli_si128(val, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i bsrli(__m128i val) { return _mm_bsrli_si128(val, imm8); }

	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi8(__m128i a, __m128i b) { return _mm_cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi16(__m128i a, __m128i b) { return _mm_cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi32(__m128i a, __m128i b) { return _mm_cmpeq_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int8_t) { return cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int16_t) { return cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int32_t) { return cmpeq_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint8_t) { return cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint16_t) { return cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint32_t) { return cmpeq_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpge_epi8(__m128i a, __m128i b) {
		return Not(cmpgt_epi8(b, a));
	}
	WJR_INTRINSIC_INLINE static __m128i cmpge_epi16(__m128i a, __m128i b) {
		return Not(cmpgt_epi16(b, a));
	}
	WJR_INTRINSIC_INLINE static __m128i cmpge_epi32(__m128i a, __m128i b) {
		return Not(cmpgt_epi32(b, a));
	}

	WJR_INTRINSIC_INLINE static __m128i cmpge_epu8(__m128i a, __m128i b) {
		return Not(cmpgt_epu8(b, a));
	}
	WJR_INTRINSIC_INLINE static __m128i cmpge_epu16(__m128i a, __m128i b) {
		return Not(cmpgt_epu16(b, a));
	}
	WJR_INTRINSIC_INLINE static __m128i cmpge_epu32(__m128i a, __m128i b) {
		return Not(cmpgt_epu32(b, a));
	}

	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int8_t) { return cmpge_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int16_t) { return cmpge_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int32_t) { return cmpge_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint8_t) { return cmpge_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint16_t) { return cmpge_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint32_t) { return cmpge_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi8(__m128i a, __m128i b) { return _mm_cmpgt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi16(__m128i a, __m128i b) { return _mm_cmpgt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi32(__m128i a, __m128i b) { return _mm_cmpgt_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpgt_epu8(__m128i a, __m128i b) {
#if defined(__XOP__)
		return _mm_comgt_epu8(a, b);
#else
		return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epu16(__m128i a, __m128i b) {
#if defined(__XOP__)
		return _mm_comgt_epu16(a, b);
#else
		return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epu32(__m128i a, __m128i b) {
#if defined(__XOP__)
		return _mm_comgt_epu32(a, b);
#else
		return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
#endif // __XOP__
	}

	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int8_t) { return cmpgt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int16_t) { return cmpgt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int32_t) { return cmpgt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint8_t) { return cmpgt_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint16_t) { return cmpgt_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint32_t) { return cmpgt_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmple_epi8(__m128i a, __m128i b) {
		return Not(cmpgt_epi8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i cmple_epi16(__m128i a, __m128i b) {
		return Not(cmpgt_epi16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i cmple_epi32(__m128i a, __m128i b) {
		return Not(cmpgt_epi32(a, b));
	}

	WJR_INTRINSIC_INLINE static __m128i cmple_epu8(__m128i a, __m128i b) {
		return Not(cmpgt_epu8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i cmple_epu16(__m128i a, __m128i b) {
		return Not(cmpgt_epu16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i cmple_epu32(__m128i a, __m128i b) {
		return Not(cmpgt_epu32(a, b));
	}

	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int8_t) { return cmple_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int16_t) { return cmple_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int32_t) { return cmple_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint8_t) { return cmple_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint16_t) { return cmple_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint32_t) { return cmple_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmplt_epi8(__m128i a, __m128i b) { return _mm_cmplt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt_epi16(__m128i a, __m128i b) { return _mm_cmplt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt_epi32(__m128i a, __m128i b) { return _mm_cmplt_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmplt_epu8(__m128i a, __m128i b) {
		return cmpgt_epu8(b, a);
	}
	WJR_INTRINSIC_INLINE static __m128i cmplt_epu16(__m128i a, __m128i b) {
		return cmpgt_epu16(b, a);
	}
	WJR_INTRINSIC_INLINE static __m128i cmplt_epu32(__m128i a, __m128i b) {
		return cmpgt_epu32(b, a);
	}

	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int8_t) { return cmplt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int16_t) { return cmplt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int32_t) { return cmplt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint8_t) { return cmplt_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint16_t) { return cmplt_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint32_t) { return cmplt_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpne_epi8(__m128i a, __m128i b) { return Not(cmpeq_epi8(a, b)); }
	WJR_INTRINSIC_INLINE static __m128i cmpne_epi16(__m128i a, __m128i b) { return Not(cmpeq_epi16(a, b)); }
	WJR_INTRINSIC_INLINE static __m128i cmpne_epi32(__m128i a, __m128i b) { return Not(cmpeq_epi32(a, b)); }

	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int8_t) { return cmpne_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int16_t) { return cmpne_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int32_t) { return cmpne_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint8_t) { return cmpne_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint16_t) { return cmpne_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint32_t) { return cmpne_epi32(a, b); }

	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::equal_to<>, T) {
		return cmpeq(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::not_equal_to<>, T) {
		return cmpne(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater<>, T) {
		return cmpgt(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater_equal<>, T) {
		return cmpge(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less<>, T) {
		return cmplt(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less_equal<>, T) {
		return cmple(a, b, T());
	}

	WJR_INTRINSIC_INLINE static __m128i concat(uint64_t lo, uint64_t hi) {
		return set_epi64x(hi, lo);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi8(__m128i a) {
		static_assert(imm8 >= 0 && imm8 < 16, "imm8 must be in range [0, 15]");
#if defined(__SSE4_1__)
		return _mm_extract_epi8(a, imm8);
#else
		if constexpr (imm8 & 1) {
			return extract_epi16(a, imm8 >> 1) >> 8;
		}
		else {
			return extract_epi16(a, imm8 >> 1) & 0xff;
		}
#endif // __SSE4_1__
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi16(__m128i a) {
		static_assert(imm8 >= 0 && imm8 < 8, "imm8 must be in range [0, 7]");
		return _mm_extract_epi16(a, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi32(__m128i a) {
		static_assert(imm8 >= 0 && imm8 < 4, "imm8 must be in range [0, 3]");
#if defined(__SSE4_1__)
		return _mm_extract_epi32(a, imm8);
#else
		if constexpr (imm8 == 0) {
			return simd_cast<uint32_t>(a);
		}
		else if constexpr (imm8 == 1) {
			return static_cast<uint32_t>(simd_cast<uint64_t>(a) >> 32);
		}
		else if constexpr (imm8 == 2) {
			return simd_cast<uint32_t>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		}
		else {
			return simd_cast<uint32_t>(shuffle_epi32<_MM_SHUFFLE(3, 3, 3, 3)>(a));
		}
#endif // __SSE4_1__
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m128i a) {
		static_assert(imm8 >= 0 && imm8 < 2, "imm8 must be in range [0, 1]");
#if defined(__SSE4_1__)
		return _mm_extract_epi64(a, imm8);
#else
		if constexpr (imm8 == 0) {
			return simd_cast<uint64_t>(a);
		}
		else {
			return simd_cast<uint64_t>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		}
#endif // __SSE4_1__
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, int8_t) { return extract_epi8<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, int16_t) { return extract_epi16<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, int32_t) { return extract_epi32<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, int64_t) { return extract_epi64<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, uint8_t) { return extract_epi8<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, uint16_t) { return extract_epi16<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, uint32_t) { return extract_epi32<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, uint64_t) { return extract_epi64<imm8>(a); }

	WJR_INTRINSIC_INLINE static uint64_t getlow(__m128i v) {
		return simd_cast<uint64_t>(v);
	}
	WJR_INTRINSIC_INLINE static uint64_t gethigh(__m128i v) {
		return extract_epi64<1>(v);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi16(__m128i a, int i) {
		return _mm_insert_epi16(a, i, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int16_t) { return insert_epi16<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint16_t) { return insert_epi16<imm8>(a, i); }

	WJR_INTRINSIC_INLINE static void lfence() {
		_mm_lfence();
	}

	WJR_INTRINSIC_INLINE static __m128i load(const __m128i* ptr) { return _mm_load_si128(ptr); }
	WJR_INTRINSIC_INLINE static __m128i loadu(const __m128i* ptr) { return _mm_loadu_si128(ptr); }
	WJR_INTRINSIC_INLINE static __m128i loadu_si16(const void* ptr) { return simd::mm_loadu_si16(ptr); }
	WJR_INTRINSIC_INLINE static __m128i loadu_si32(const void* ptr) { return simd::mm_loadu_si32(ptr); }
	WJR_INTRINSIC_INLINE static __m128i loadu_si64(const void* ptr) { return simd::mm_loadu_si64(ptr); }

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m128i logical_and(__m128i a, __m128i b, T) {
		return Not(Or(logical_not(a, T()), logical_not(b, T())));
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m128i logical_not(__m128i v, T) {
		auto Zero = zeros();
		return cmpeq(v, Zero, T());
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m128i logical_or(__m128i a, __m128i b, T) {
		return Not(logical_not(Or(a, b), T()));
	}

	WJR_INTRINSIC_INLINE static __m128i madd_epi16(__m128i a, __m128i b) {
		return _mm_madd_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static void maskmoveu(__m128i a, __m128i mask, char* mem_addr) {
		return _mm_maskmoveu_si128(a, mask, mem_addr);
	}

	WJR_INTRINSIC_INLINE static __m128i max_epi16(__m128i a, __m128i b) {
		return _mm_max_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu8(__m128i a, __m128i b) {
		return _mm_max_epu8(a, b);
	}

#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static __m128i max_epi8(__m128i a, __m128i b) {
		return _mm_max_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i max_epi32(__m128i a, __m128i b) {
		return _mm_max_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu16(__m128i a, __m128i b) {
		return _mm_max_epu16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu32(__m128i a, __m128i b) {
		return _mm_max_epu32(a, b);
	}
#else
	WJR_INTRINSIC_INLINE static __m128i max_epi8(__m128i a, __m128i b) {
		return blendv_epi8(b, a, cmpgt_epi8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i max_epi32(__m128i a, __m128i b) {
		return blendv_epi8(b, a, cmpgt_epi32(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu16(__m128i a, __m128i b) {
		return blendv_epi8(b, a, cmpgt_epu16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu32(__m128i a, __m128i b) {
		return blendv_epi8(b, a, cmpgt_epu32(a, b));
	}
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int8_t) { return max_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int16_t) { return max_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int32_t) { return max_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint8_t) { return max_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint16_t) { return max_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint32_t) { return max_epu32(a, b); }

	WJR_INTRINSIC_INLINE static int8_t max_epi8(__m128i a) {
		return 0x7fu ^ min_epu8(Xor(a, set1_epi8(0x7fu)));
	}

	WJR_INTRINSIC_INLINE static int16_t max_epi16(__m128i a) {
#if defined(__SSE4_1__)
		return 0x7fffu ^ min_epu16(Xor(a, set1_epi16(0x7fffu)));
#else
		a = max_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<int16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static int32_t max_epi32(__m128i a) {
		a = max_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<int32_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m128i a) {
#if defined(__SSE4_1__)
		return 0xffu ^ min_epu8(Xor(a, ones()));
#else
		a = max_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::max((uint8_t)X, (uint8_t)(X >> 8));
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m128i a) {
#if defined(__SSE4_1__)
		return 0xffffu ^ min_epu16(Xor(a, ones()));
#else
		a = max_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<uint16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m128i a) {
		a = max_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<uint32_t>(a);
	}

	WJR_INTRINSIC_INLINE static int8_t max(__m128i a, int8_t) { return max_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t max(__m128i a, int16_t) { return max_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t max(__m128i a, int32_t) { return max_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t max(__m128i a, uint8_t) { return max_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t max(__m128i a, uint16_t) { return max_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t max(__m128i a, uint32_t) { return max_epu32(a); }

	WJR_INTRINSIC_INLINE static void mfence() { _mm_mfence(); }

	WJR_INTRINSIC_INLINE static __m128i min_epi16(__m128i a, __m128i b) {
		return _mm_min_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu8(__m128i a, __m128i b) {
		return _mm_min_epu8(a, b);
	}

#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static __m128i min_epi8(__m128i a, __m128i b) {
		return _mm_min_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i min_epi32(__m128i a, __m128i b) {
		return _mm_min_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu16(__m128i a, __m128i b) {
		return _mm_min_epu16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu32(__m128i a, __m128i b) {
		return _mm_min_epu32(a, b);
	}
#else
	WJR_INTRINSIC_INLINE static __m128i min_epi8(__m128i a, __m128i b) {
		return blendv_epi8(a, b, cmpgt_epi8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i min_epi32(__m128i a, __m128i b) {
		return blendv_epi8(a, b, cmpgt_epi32(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu16(__m128i a, __m128i b) {
		return blendv_epi8(a, b, cmpgt_epu16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu32(__m128i a, __m128i b) {
		return blendv_epi8(a, b, cmpgt_epu32(a, b));
	}
#endif // __SSE4_1__
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int8_t) { return min_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int16_t) { return min_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int32_t) { return min_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint8_t) { return min_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint16_t) { return min_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint32_t) { return min_epu32(a, b); }

	WJR_INTRINSIC_INLINE static int8_t min_epi8(__m128i a) {
		return 0x80u ^ min_epu8(Xor(a, setmin_epi8()));
	}
	WJR_INTRINSIC_INLINE static int16_t min_epi16(__m128i a) {
#if defined(__SSE4_1__)
		return 0x8000u ^ min_epu16(Xor(a, setmin_epi16()));
#else
		a = min_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<int16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static int32_t min_epi32(__m128i a) {
		a = min_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<int32_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m128i a) {
#if defined(__SSE4_1__)
		a = min_epu8(a, srli_epi16(a, 8));
		a = _mm_minpos_epu16(a);
		return simd_cast<uint8_t>(a);
#else
		a = min_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::min((uint8_t)X, (uint8_t)(X >> 8));
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m128i a) {
#if defined(__SSE4_1__)
		return simd_cast<uint16_t>(_mm_minpos_epu16(a));
#else
		a = min_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<uint16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m128i a) {
		a = min_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<uint32_t>(a);
	}

	WJR_INTRINSIC_INLINE static int8_t min(__m128i a, int8_t) { return min_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t min(__m128i a, int16_t) { return min_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t min(__m128i a, int32_t) { return min_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t min(__m128i a, uint8_t) { return min_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t min(__m128i a, uint16_t) { return min_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t min(__m128i a, uint32_t) { return min_epu32(a); }

	WJR_INTRINSIC_INLINE static __m128i move_epi64(__m128i a) {
		return _mm_move_epi64(a);
	}

	WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m128i a) {
		return _mm_movemask_epi8(a);
	}
	WJR_INTRINSIC_INLINE static mask_type movemask_pd(__m128d v) {
		return _mm_movemask_pd(v);
	}

	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int8_t) { return movemask_epi8(v); }
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int32_t) { return movemask_ps(simd_cast<__m128>(v)); }
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int64_t) { return movemask_pd(simd_cast<__m128d>(v)); }
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint8_t) { return movemask(v, int8_t()); }
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint32_t) { return movemask(v, int32_t()); }
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint64_t) { return movemask(v, int64_t()); }

	WJR_INTRINSIC_INLINE static __m128i mul_epu32(__m128i a, __m128i b) {
		return _mm_mul_epu32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i mulhi_epi16(__m128i a, __m128i b) {
		return _mm_mulhi_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i mulhi_epu16(__m128i a, __m128i b) {
		return _mm_mulhi_epu16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i mullo_epi16(__m128i a, __m128i b) {
		return _mm_mullo_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i negate_epi8(__m128i a) {
#if defined(__SSSE3__)
		return sign_epi8(ones(), a);
#else
		return sub_epi8(zeros(), a);
#endif // __SSSE3__
	}
	WJR_INTRINSIC_INLINE static __m128i negate_epi16(__m128i a) {
#if defined(__SSSE3__)
		return sign_epi16(ones(), a);
#else
		return sub_epi16(zeros(), a);
#endif // __SSSE3__
	}
	WJR_INTRINSIC_INLINE static __m128i negate_epi32(__m128i a) {
#if defined(__SSSE3__)
		return sign_epi32(ones(), a);
#else
		return sub_epi32(zeros(), a);
#endif // __SSSE3__
	}
	WJR_INTRINSIC_INLINE static __m128i negate_epi64(__m128i a) {
		return sub_epi64(zeros(), a);
	}

	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int8_t) { return negate_epi8(a); }
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int16_t) { return negate_epi16(a); }
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int32_t) { return negate_epi32(a); }
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int64_t) { return negate_epi64(a); }

	WJR_INTRINSIC_INLINE static __m128i Not(__m128i v) { return Xor(v, ones()); }

	WJR_INTRINSIC_INLINE static __m128i Or(__m128i a, __m128i b) { return _mm_or_si128(a, b); }

	WJR_INTRINSIC_INLINE static __m128i packs_epi16(__m128i a, __m128i b) {
		return _mm_packs_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i packs_epi32(__m128i a, __m128i b) {
		return _mm_packs_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i packus_epi16(__m128i a, __m128i b) {
		return _mm_packus_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i preloadu_si16(const void* ptr) { return loadu_si16(ptr); }
	WJR_INTRINSIC_INLINE static __m128i preloadu_si32(const void* ptr) { return loadu_si32(ptr); }
	WJR_INTRINSIC_INLINE static __m128i preloadu_si48(const void* ptr) {
		return insert_epi16<2>(preloadu_si32(ptr), reinterpret_cast<const uint16_t*>(ptr)[2]);
	}
	WJR_INTRINSIC_INLINE static __m128i preloadu_si64(const void* ptr) { return loadu_si64(ptr); }
	WJR_INTRINSIC_INLINE static __m128i preloadu_si80(const void* ptr) {
		return insert_epi16<4>(preloadu_si64(ptr), reinterpret_cast<const uint16_t*>(ptr)[4]);
	}
	WJR_INTRINSIC_INLINE static __m128i preloadu_si96(const void* ptr) {
#if defined(__SSE4_1__)
		return insert_epi32<2>(preloadu_si64(ptr), reinterpret_cast<const uint32_t*>(ptr)[2]);
#else
		return insert_epi16<5>(preloadu_si80(ptr), reinterpret_cast<const uint16_t*>(ptr)[5]);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static __m128i preloadu_si112(const void* ptr) {
		return insert_epi16<6>(preloadu_si96(ptr), reinterpret_cast<const uint16_t*>(ptr)[6]);
	}
	WJR_INTRINSIC_INLINE static __m128i preloadu_si128(const void* ptr) { return loadu((__m128i*)ptr); }

	WJR_INTRINSIC_INLINE static __m128i preloadu_si16x(const void* ptr, int n) {
		// preloadu_si(n * 16)
		switch (n) {
		case 0:return zeros();
		case 1:return preloadu_si16(ptr);
		case 2:return preloadu_si32(ptr);
		case 3:return preloadu_si48(ptr);
		case 4:return preloadu_si64(ptr);
		case 5:return preloadu_si80(ptr);
		case 6:return preloadu_si96(ptr);
		case 7:return preloadu_si112(ptr);
		default:return preloadu_si128(ptr);
		}
	}

	WJR_INTRINSIC_INLINE static __m128i sad_epu8(__m128i a, __m128i b) {
		return _mm_sad_epu8(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i zeros() { return _mm_setzero_si128(); }
	WJR_INTRINSIC_INLINE static __m128i ones() { return _mm_set1_epi32(-1); }

	WJR_INTRINSIC_INLINE static __m128i set_epi8(
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm_set_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m128i set_epi16(
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm_set_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m128i set_epi32(int e3, int e2, int e1, int e0) {
		return _mm_set_epi32(e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m128i set_epi64x(long long e1, long long e0) {
		return _mm_set_epi64x(e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m128i setr_epi8(
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm_setr_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m128i setr_epi16(
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm_setr_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m128i setr_epi32(int e3, int e2, int e1, int e0) {
		return _mm_setr_epi32(e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m128i set1_epi8(int8_t val) { return _mm_set1_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i set1_epi16(int16_t val) { return _mm_set1_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i set1_epi32(int32_t val) { return _mm_set1_epi32(val); }
	WJR_INTRINSIC_INLINE static __m128i set1_epi64(int64_t val) { return _mm_set1_epi64x(val); }

	WJR_INTRINSIC_INLINE static __m128i set1(int8_t val, int8_t) { return set1_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(int16_t val, int16_t) { return set1_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(int32_t val, int32_t) { return set1_epi32(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(int64_t val, int64_t) { return set1_epi64(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(uint8_t val, uint8_t) { return set1_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(uint16_t val, uint16_t) { return set1_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(uint32_t val, uint32_t) { return set1_epi32(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(uint64_t val, uint64_t) { return set1_epi64(val); }

	WJR_INTRINSIC_INLINE static __m128i setmin_epi8() { return set1_epi8(0x80u); }
	WJR_INTRINSIC_INLINE static __m128i setmin_epi16() { return set1_epi16(0x8000u); }
	WJR_INTRINSIC_INLINE static __m128i setmin_epi32() { return set1_epi32(0x80000000u); }

	WJR_INTRINSIC_INLINE static __m128i setmin(int8_t) { return setmin_epi8(); }
	WJR_INTRINSIC_INLINE static __m128i setmin(int16_t) { return setmin_epi16(); }
	WJR_INTRINSIC_INLINE static __m128i setmin(int32_t) { return setmin_epi32(); }
	WJR_INTRINSIC_INLINE static __m128i setmin(uint8_t) { return set1_epi32(0); }
	WJR_INTRINSIC_INLINE static __m128i setmin(uint16_t) { return set1_epi32(0); }
	WJR_INTRINSIC_INLINE static __m128i setmin(uint32_t) { return set1_epi32(0); }

	WJR_INTRINSIC_INLINE static __m128i setmax_epi8() { return set1_epi8(0x7F); }
	WJR_INTRINSIC_INLINE static __m128i setmax_epi16() { return set1_epi16(0x7FFF); }
	WJR_INTRINSIC_INLINE static __m128i setmax_epi32() { return set1_epi32(0x7FFFFFFF); }

	WJR_INTRINSIC_INLINE static __m128i setmax(int8_t) { return setmax_epi8(); }
	WJR_INTRINSIC_INLINE static __m128i setmax(int16_t) { return setmax_epi16(); }
	WJR_INTRINSIC_INLINE static __m128i setmax(int32_t) { return setmax_epi32(); }
	WJR_INTRINSIC_INLINE static __m128i setmax(uint8_t) { return set1_epi32(0xFFFFFFFF); }
	WJR_INTRINSIC_INLINE static __m128i setmax(uint16_t) { return set1_epi32(0xFFFFFFFF); }
	WJR_INTRINSIC_INLINE static __m128i setmax(uint32_t) { return set1_epi32(0xFFFFFFFF); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i shuffle_epi32(__m128i v) {
		static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
		return _mm_shuffle_epi32(v, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i shufflehi_epi16(__m128i v) {
		return _mm_shufflehi_epi16(v, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i shufflelo_epi16(__m128i v) {
		return _mm_shufflelo_epi16(v, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i sll_epi16(__m128i a, __m128i b) {
		return _mm_sll_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sll_epi32(__m128i a, __m128i b) {
		return _mm_sll_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sll_epi64(__m128i a, __m128i b) {
		return _mm_sll_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int16_t) { return sll_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int32_t) { return sll_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int64_t) { return sll_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint64_t) { return sll_epi64(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i v) {
		return _mm_slli_si128(v, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i slli_epi16(__m128i a, int imm8) {
		return _mm_slli_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i slli_epi32(__m128i a, int imm8) {
		return _mm_slli_epi32(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i slli_epi64(__m128i a, int imm8) {
		return _mm_slli_epi64(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }

	WJR_INTRINSIC_INLINE static __m128i sra_epi16(__m128i a, __m128i b) {
		return _mm_sra_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sra_epi32(__m128i a, __m128i b) {
		return _mm_sra_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int16_t) { return sra_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int32_t) { return sra_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i srai_epi16(__m128i a, int imm8) {
		return _mm_srai_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i srai_epi32(__m128i a, int imm8) {
		return _mm_srai_epi32(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int32_t) { return srai_epi32(a, imm8); }

	WJR_INTRINSIC_INLINE static __m128i srl_epi16(__m128i a, __m128i b) {
		return _mm_srl_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i srl_epi32(__m128i a, __m128i b) {
		return _mm_srl_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i srl_epi64(__m128i a, __m128i b) {
		return _mm_srl_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int16_t) { return srl_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int32_t) { return srl_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int64_t) { return srl_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint64_t) { return srl_epi64(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i v) {
		return _mm_srli_si128(v, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i srli_epi16(__m128i a, int imm8) {
		return _mm_srli_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i srli_epi32(__m128i a, int imm8) {
		return _mm_srli_epi32(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i srli_epi64(__m128i a, int imm8) {
		return _mm_srli_epi64(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }

	WJR_INTRINSIC_INLINE static void stream(__m128i* ptr, __m128i v) {
		_mm_stream_si128(ptr, v);
	}

	WJR_INTRINSIC_INLINE static void store(__m128i* ptr, __m128i val) { _mm_store_si128(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu(__m128i* ptr, __m128i val) { _mm_storeu_si128(ptr, val); }
	/*
	WJR_INTRINSIC_INLINE static void storeu_si16(void* ptr, __m128i val) { _mm_storeu_si16(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu_si32(void* ptr, __m128i val) { _mm_storeu_si32(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu_si64(void* ptr, __m128i val) { _mm_storeu_si64(ptr, val); }
	*/

	WJR_INTRINSIC_INLINE static __m128i sub_epi8(__m128i a, __m128i b) {
		return _mm_sub_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sub_epi16(__m128i a, __m128i b) {
		return _mm_sub_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sub_epi32(__m128i a, __m128i b) {
		return _mm_sub_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sub_epi64(__m128i a, __m128i b) {
		return _mm_sub_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int8_t) { return sub_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int16_t) { return sub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int32_t) { return sub_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int64_t) { return sub_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint8_t) { return sub_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint16_t) { return sub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint32_t) { return sub_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint64_t) { return sub_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i subs_epi8(__m128i a, __m128i b) {
		return _mm_subs_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i subs_epi16(__m128i a, __m128i b) {
		return _mm_subs_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i subs_epu8(__m128i a, __m128i b) {
		return _mm_subs_epu8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i subs_epu16(__m128i a, __m128i b) {
		return _mm_subs_epu16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int8_t) { return subs_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int16_t) { return subs_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint8_t) { return subs_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint16_t) { return subs_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi8(__m128i a, __m128i b) {
		return _mm_unpackhi_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi16(__m128i a, __m128i b) {
		return _mm_unpackhi_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi32(__m128i a, __m128i b) {
		return _mm_unpackhi_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi64(__m128i a, __m128i b) {
		return _mm_unpackhi_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int8_t) { return unpackhi_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int16_t) { return unpackhi_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int32_t) { return unpackhi_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int64_t) { return unpackhi_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint8_t) { return unpackhi_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint16_t) { return unpackhi_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint32_t) { return unpackhi_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint64_t) { return unpackhi_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi8(__m128i a, __m128i b) {
		return _mm_unpacklo_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi16(__m128i a, __m128i b) {
		return _mm_unpacklo_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi32(__m128i a, __m128i b) {
		return _mm_unpacklo_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi64(__m128i a, __m128i b) {
		return _mm_unpacklo_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int8_t) { return unpacklo_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int16_t) { return unpacklo_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int32_t) { return unpacklo_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int64_t) { return unpacklo_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint8_t) { return unpacklo_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint16_t) { return unpacklo_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint32_t) { return unpacklo_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i Xor(__m128i a, __m128i b) { return _mm_xor_si128(a, b); }

#endif // __SSE2__

#if defined(__SSE3__)
	WJR_INTRINSIC_INLINE static __m128i lddqu(const __m128i* ptr) { return _mm_lddqu_si128(ptr); }
#endif // __SSE3__

#if defined(__SSSE3__)

	WJR_INTRINSIC_INLINE static __m128i abs_epi8(__m128i val) { return _mm_abs_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i abs_epi16(__m128i val) { return _mm_abs_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i abs_epi32(__m128i val) { return _mm_abs_epi32(val); }

	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int8_t) { return abs_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int16_t) { return abs_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int32_t) { return abs_epi32(val); }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint8_t) { return val; }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint16_t) { return val; }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint32_t) { return val; }

	WJR_INTRINSIC_INLINE static __m128i shuffle_epi8(__m128i v, __m128i imm8) {
		return _mm_shuffle_epi8(v, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i sign_epi8(__m128i a, __m128i b) {
		return _mm_sign_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sign_epi16(__m128i a, __m128i b) {
		return _mm_sign_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sign_epi32(__m128i a, __m128i b) {
		return _mm_sign_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int8_t) { return sign_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int16_t) { return sign_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int32_t) { return sign_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint8_t) { return sign_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint16_t) { return sign_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint32_t) { return sign_epi32(a, b); }

#endif // __SSSE3__

#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i blend_epi16(__m128i a, __m128i b) {
		return _mm_blend_epi16(a, b, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi64(__m128i a, __m128i b) { return _mm_cmpeq_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int64_t) { return cmpeq_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint64_t) { return cmpeq_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi64(__m128i a, __m128i b) { return _mm_cmpgt_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int64_t) { return cmpgt_epi64(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi8(__m128i a, int i) {
		return _mm_insert_epi8(a, i, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi32(__m128i a, int i) {
		return _mm_insert_epi32(a, i, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi64(__m128i a, int64_t i) {
		return _mm_insert_epi64(a, i, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int8_t) { return insert_epi8<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int32_t) { return insert_epi32<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, int64_t) { return insert_epi64<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint8_t) { return insert_epi8<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint32_t) { return insert_epi32<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, uint64_t) { return insert_epi64<imm8>(a, i); }

	WJR_INTRINSIC_INLINE static __m128i minpos_epu16(__m128i a) { return _mm_minpos_epu16(a); }

	WJR_INTRINSIC_INLINE static __m128i mul_epi32(__m128i a, __m128i b) {
		return _mm_mul_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i mullo_epi32(__m128i a, __m128i b) {
		return _mm_mullo_epi32(a, b);
	}


	WJR_INTRINSIC_INLINE static __m128i packus_epi32(__m128i a, __m128i b) {
		return _mm_packus_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i stream_load(__m128i* p) {
		return _mm_stream_load_si128(p);
	}

	WJR_INTRINSIC_INLINE static int test_all_ones(__m128i a) {
		return _mm_test_all_ones(a);
	}

	WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a, __m128i b) {
		return _mm_test_all_zeros(a, b);
	}

	WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a) {
		return _mm_test_all_zeros(a, a);
	}

	WJR_INTRINSIC_INLINE static int test_mix_ones_zeros(__m128i a, __m128i b) {
		return _mm_test_mix_ones_zeros(a, b);
	}

	WJR_INTRINSIC_INLINE static int testc(__m128i a, __m128i b) {
		return _mm_testc_si128(a, b);
	}

	WJR_INTRINSIC_INLINE static int testnzc(__m128i a, __m128i b) {
		return _mm_testnzc_si128(a, b);
	}

	WJR_INTRINSIC_INLINE static int testz(__m128i a, __m128i b) {
		return _mm_testz_si128(a, b);
	}
#endif // __SSE4_1__

};

struct avx {
	using mask_type = uint32_t;
#if defined(__AVX__)
	using float_type = __m256;
	using int_type = __m256i;
	using double_type = __m256d;
#endif // __AVX__

	constexpr static size_t width() { return 256; }

	constexpr static mask_type mask() { return 0xffffffff; }

#if defined(__AVX__)

	WJR_INTRINSIC_INLINE static __m256i concat(__m128i a, __m128i b) {
		return insert_si128<1>(simd_cast<__m256i>(a), b);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi32(__m256i v) { return _mm256_extract_epi32(v, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m256i v) { return _mm256_extract_epi64(v, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m256i v, int32_t) { return extract_epi32<imm8>(v); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(__m256i v, int64_t) { return extract_epi64<imm8>(v); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i extract_si128(__m256i v) {
#if defined(__AVX2__)
		return _mm256_extracti128_si256(v, imm8);
#else
		return _mm256_extractf128_si256(v, imm8);
#endif // __AVX2__
	}

	WJR_INTRINSIC_INLINE static __m128i getlow(__m256i a) {
		return simd_cast<__m128i>(a);
	}

	WJR_INTRINSIC_INLINE static __m128i gethigh(__m256i a) {
		return extract_si128<1>(a);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi8(__m256i v, int8_t i) { return _mm256_insert_epi8(v, i, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi16(__m256i v, int16_t i) { return _mm256_insert_epi16(v, i, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi32(__m256i v, int32_t i) { return _mm256_insert_epi32(v, i, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi64(__m256i v, int64_t i) { return _mm256_insert_epi64(v, i, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_si128(__m256i a, __m128i b) {
#if defined(__AVX2__)
		return _mm256_inserti128_si256(a, b, imm8);
#else
		return _mm256_insertf128_si256(a, b, imm8);
#endif // __AVX2__
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m256i logical_and(__m256i a, __m256i b, T) {
		return Not(Or(logical_not(a, T()), logical_not(b, T())));
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m256i logical_not(__m256i v, T) {
		auto Zero = zeros();
		return cmpeq(v, Zero, T());
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m256i logical_or(__m256i a, __m256i b, T) {
		return Not(logical_not(Or(a, b), T()));
	}

	WJR_INTRINSIC_INLINE static __m256i zeros() { return _mm256_setzero_si256(); }
	WJR_INTRINSIC_INLINE static __m256i ones() { return _mm256_set1_epi32(-1); }

	WJR_INTRINSIC_INLINE static __m256i set_epi8(
		char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
		char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm256_set_epi8(
			e31, e30, e29, e28, e27, e26, e25, e24,
			e23, e22, e21, e20, e19, e18, e17, e16,
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i set_epi16(
		short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm256_set_epi16(
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i set_epi32(
		int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
		return _mm256_set_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i set_epi64x(
		long long e3, long long e2, long long e1, long long e0) {
		return _mm256_set_epi64x(e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m256i setr_epi8(
		char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
		char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm256_setr_epi8(
			e31, e30, e29, e28, e27, e26, e25, e24,
			e23, e22, e21, e20, e19, e18, e17, e16,
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i setr_epi16(
		short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm256_setr_epi16(
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i setr_epi32(
		int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
		return _mm256_setr_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i setr_epi64x(
		long long e3, long long e2, long long e1, long long e0) {
		return _mm256_setr_epi64x(e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m256i set1_epi8(int8_t a) { return _mm256_set1_epi8(a); }
	WJR_INTRINSIC_INLINE static __m256i set1_epi16(int16_t a) { return _mm256_set1_epi16(a); }
	WJR_INTRINSIC_INLINE static __m256i set1_epi32(int32_t a) { return _mm256_set1_epi32(a); }
	WJR_INTRINSIC_INLINE static __m256i set1_epi64(int64_t a) { return _mm256_set1_epi64x(a); }

	WJR_INTRINSIC_INLINE static __m256i set1(int8_t a, int8_t) { return set1_epi8(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(int16_t a, int16_t) { return set1_epi16(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(int32_t a, int32_t) { return set1_epi32(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(int64_t a, int64_t) { return set1_epi64(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(uint8_t a, uint8_t) { return set1_epi8(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(uint16_t a, uint16_t) { return set1_epi16(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(uint32_t a, uint32_t) { return set1_epi32(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(uint64_t a, uint64_t) { return set1_epi64(a); }

	WJR_INTRINSIC_INLINE static __m256i setmin_epi8() { return set1_epi8(0x80u); }
	WJR_INTRINSIC_INLINE static __m256i setmin_epi16() { return set1_epi16(0x8000u); }
	WJR_INTRINSIC_INLINE static __m256i setmin_epi32() { return set1_epi32(0x80000000u); }
	WJR_INTRINSIC_INLINE static __m256i setmin_epi64() { return set1_epi64(0x8000000000000000ull); }

	WJR_INTRINSIC_INLINE static __m256i setmin(int8_t) { return setmin_epi8(); }
	WJR_INTRINSIC_INLINE static __m256i setmin(int16_t) { return setmin_epi16(); }
	WJR_INTRINSIC_INLINE static __m256i setmin(int32_t) { return setmin_epi32(); }
	WJR_INTRINSIC_INLINE static __m256i setmin(int64_t) { return setmin_epi64(); }

	WJR_INTRINSIC_INLINE static __m256i setmax_epi8() { return set1_epi8(0x7f); }
	WJR_INTRINSIC_INLINE static __m256i setmax_epi16() { return set1_epi16(0x7fff); }
	WJR_INTRINSIC_INLINE static __m256i setmax_epi32() { return set1_epi32(0x7fffffff); }
	WJR_INTRINSIC_INLINE static __m256i setmax_epi64() { return set1_epi64(0x7fffffffffffffff); }

	WJR_INTRINSIC_INLINE static __m256i setmax(int8_t) { return setmax_epi8(); }
	WJR_INTRINSIC_INLINE static __m256i setmax(int16_t) { return setmax_epi16(); }
	WJR_INTRINSIC_INLINE static __m256i setmax(int32_t) { return setmax_epi32(); }
	WJR_INTRINSIC_INLINE static __m256i setmax(int64_t) { return setmax_epi64(); }

	WJR_INTRINSIC_INLINE static void stream(__m256i* p, __m256i a) { _mm256_stream_si256(p, a); }

	//WJR_INTRINSIC_INLINE static __m256i zextsi128_si256(__m128i v) { return _mm256_zextsi128_si256(v); }

	WJR_INTRINSIC_INLINE static __m256i load(const __m256i* p) { return _mm256_load_si256(p); }
	WJR_INTRINSIC_INLINE static __m256i loadu(const __m256i* p) { return _mm256_loadu_si256(p); }

	WJR_INTRINSIC_INLINE static __m256i preloadu_si16(const void* ptr) {
		return simd_cast<__m256i, __m128i>(sse::preloadu_si16(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si32(const void* ptr) {
		return simd_cast<__m256i, __m128i>(sse::preloadu_si32(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si48(const void* ptr) {
		return simd_cast<__m256i, __m128i>(sse::preloadu_si48(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si64(const void* ptr) {
		return simd_cast<__m256i, __m128i>(sse::preloadu_si64(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si80(const void* ptr) {
		return simd_cast<__m256i, __m128i>(sse::preloadu_si80(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si96(const void* ptr) {
		return simd_cast<__m256i, __m128i>(sse::preloadu_si96(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si112(const void* ptr) {
		return simd_cast<__m256i, __m128i>(sse::preloadu_si112(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si128(const void* ptr) {
		return simd_cast<__m256i, __m128i>(sse::preloadu_si128(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si144(const void* ptr) {
		return concat(sse::preloadu_si128(ptr),
			sse::preloadu_si16((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si160(const void* ptr) {
		return concat(sse::preloadu_si128(ptr),
			sse::preloadu_si32((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si176(const void* ptr) {
		return concat(sse::preloadu_si128(ptr),
			sse::preloadu_si48((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si192(const void* ptr) {
		return concat(sse::preloadu_si128(ptr),
			sse::preloadu_si64((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si208(const void* ptr) {
		return concat(sse::preloadu_si128(ptr),
			sse::preloadu_si80((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si224(const void* ptr) {
		return concat(sse::preloadu_si128(ptr),
			sse::preloadu_si96((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si240(const void* ptr) {
		return concat(sse::preloadu_si128(ptr),
			sse::preloadu_si112((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preloadu_si256(const void* ptr) {
		return loadu((const __m256i*)ptr);
	}

	WJR_INTRINSIC_INLINE static __m256i preloadu_si16x(const void* ptr, int n) {
		switch (n) {
		case 0:return zeros();
		case 1:return preloadu_si16(ptr);
		case 2:return preloadu_si32(ptr);
		case 3:return preloadu_si48(ptr);
		case 4:return preloadu_si64(ptr);
		case 5:return preloadu_si80(ptr);
		case 6:return preloadu_si96(ptr);
		case 7:return preloadu_si112(ptr);
		case 8:return preloadu_si128(ptr);
		case 9:return preloadu_si144(ptr);
		case 10:return preloadu_si160(ptr);
		case 11:return preloadu_si176(ptr);
		case 12:return preloadu_si192(ptr);
		case 13:return preloadu_si208(ptr);
		case 14:return preloadu_si224(ptr);
		case 15:return preloadu_si240(ptr);
		default:return preloadu_si256(ptr);
		}
	}

	WJR_INTRINSIC_INLINE static void store(__m256i* p, const __m256i& a) { _mm256_store_si256(p, a); }
	WJR_INTRINSIC_INLINE static void storeu(__m256i* p, const __m256i& a) { _mm256_storeu_si256(p, a); }

	WJR_INTRINSIC_INLINE static int test_all_ones(__m256i a) {
		return testc(a, cmpeq_epi32(a, a));
	}

	WJR_INTRINSIC_INLINE static int test_all_zeros(__m256i a) {
		return testz(a, a);
	}

	WJR_INTRINSIC_INLINE static int testc(__m256i a, __m256i b) {
		return _mm256_testc_si256(a, b);
	}

	WJR_INTRINSIC_INLINE static int testnzc(__m256i a, __m256i b) {
		return _mm256_testnzc_si256(a, b);
	}

	WJR_INTRINSIC_INLINE static int testz(__m256i a, __m256i b) {
		return _mm256_testz_si256(a, b);
	}

#endif // __AVX__

#if defined(__AVX2__)
	WJR_INTRINSIC_INLINE static __m256i And(__m256i a, __m256i b) {
		return _mm256_and_si256(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i AndNot(__m256i a, __m256i b) {
		return _mm256_andnot_si256(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i Or(__m256i a, __m256i b) { return _mm256_or_si256(a, b); }

	WJR_INTRINSIC_INLINE static __m256i Xor(__m256i a, __m256i b) { return _mm256_xor_si256(a, b); }

	WJR_INTRINSIC_INLINE static __m256i Not(__m256i v) { return _mm256_xor_si256(v, ones()); }

	WJR_INTRINSIC_INLINE static __m256i abs_epi8(__m256i v) { return _mm256_abs_epi8(v); }
	WJR_INTRINSIC_INLINE static __m256i abs_epi16(__m256i v) { return _mm256_abs_epi16(v); }
	WJR_INTRINSIC_INLINE static __m256i abs_epi32(__m256i v) { return _mm256_abs_epi32(v); }

	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int8_t) { return abs_epi8(v); }
	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int16_t) { return abs_epi16(v); }
	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int32_t) { return abs_epi32(v); }
	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int64_t) { return abs_epi32(v); }

	WJR_INTRINSIC_INLINE static __m256i add_epi8(__m256i a, __m256i b) { return _mm256_add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add_epi16(__m256i a, __m256i b) { return _mm256_add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add_epi32(__m256i a, __m256i b) { return _mm256_add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add_epi64(__m256i a, __m256i b) { return _mm256_add_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int8_t) { return add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int16_t) { return add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int32_t) { return add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int64_t) { return add_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint8_t) { return add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint16_t) { return add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint32_t) { return add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint64_t) { return add_epi64(a, b); }

	WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m256i v) {
		return sse::add_epu8(sse::add_epi8(getlow(v), gethigh(v)));
	}
	WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m256i v) {
		return sse::add_epu16(sse::add_epi16(getlow(v), gethigh(v)));
	}
	WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m256i v) {
		return sse::add_epu32(sse::add_epi32(getlow(v), gethigh(v)));
	}
	WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m256i v) {
		return sse::add_epu64(sse::add_epi64(getlow(v), gethigh(v)));
	}
	WJR_INTRINSIC_INLINE static int8_t add_epi8(__m256i v) { return add_epu8(v); }
	WJR_INTRINSIC_INLINE static int16_t add_epi16(__m256i v) { return add_epu16(v); }
	WJR_INTRINSIC_INLINE static int32_t add_epi32(__m256i v) { return add_epu32(v); }
	WJR_INTRINSIC_INLINE static int64_t add_epi64(__m256i v) { return add_epu64(v); }

	WJR_INTRINSIC_INLINE static int8_t add(__m256i v, int8_t) { return add_epi8(v); }
	WJR_INTRINSIC_INLINE static int16_t add(__m256i v, int16_t) { return add_epi16(v); }
	WJR_INTRINSIC_INLINE static int32_t add(__m256i v, int32_t) { return add_epi32(v); }
	WJR_INTRINSIC_INLINE static int64_t add(__m256i v, int64_t) { return add_epi64(v); }
	WJR_INTRINSIC_INLINE static uint8_t add(__m256i v, uint8_t) { return add_epu8(v); }
	WJR_INTRINSIC_INLINE static uint16_t add(__m256i v, uint16_t) { return add_epu16(v); }
	WJR_INTRINSIC_INLINE static uint32_t add(__m256i v, uint32_t) { return add_epu32(v); }
	WJR_INTRINSIC_INLINE static uint64_t add(__m256i v, uint64_t) { return add_epu64(v); }

	WJR_INTRINSIC_INLINE static __m256i adds_epi8(__m256i a, __m256i b) { return _mm256_adds_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds_epi16(__m256i a, __m256i b) { return _mm256_adds_epi16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i adds_epu8(__m256i a, __m256i b) { return _mm256_adds_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds_epu16(__m256i a, __m256i b) { return _mm256_adds_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int8_t) { return adds_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int16_t) { return adds_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint8_t) { return adds_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint16_t) { return adds_epu16(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b) { return _mm256_alignr_epi8(a, b, imm8); }

	WJR_INTRINSIC_INLINE static __m256i alignl_epi16(__m256i a, __m256i b, int c) {
		return Or(slli_epi16(a, c), srli_epi16(b, 16 - c));
	}
	WJR_INTRINSIC_INLINE static __m256i alignl_epi32(__m256i a, __m256i b, int c) {
		return Or(slli_epi32(a, c), srli_epi32(b, 32 - c));
	}
	WJR_INTRINSIC_INLINE static __m256i alignl_epi64(__m256i a, __m256i b, int c) {
		return Or(slli_epi64(a, c), srli_epi64(b, 64 - c));
	}

	WJR_INTRINSIC_INLINE static __m256i alignl(__m256i a, __m256i b, int c, int16_t) {
		return alignl_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignl(__m256i a, __m256i b, int c, int32_t) {
		return alignl_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignl(__m256i a, __m256i b, int c, int64_t) {
		return alignl_epi64(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignl(__m256i a, __m256i b, int c, uint16_t) {
		return alignl_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignl(__m256i a, __m256i b, int c, uint32_t) {
		return alignl_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignl(__m256i a, __m256i b, int c, uint64_t) {
		return alignl_epi64(a, b, c);
	}

	WJR_INTRINSIC_INLINE static __m256i alignr_epi16(__m256i a, __m256i b, int c) {
		return Or(slli_epi16(a, 16 - c), srli_epi16(b, c));
	}
	WJR_INTRINSIC_INLINE static __m256i alignr_epi32(__m256i a, __m256i b, int c) {
		return Or(slli_epi32(a, 32 - c), srli_epi32(b, c));
	}
	WJR_INTRINSIC_INLINE static __m256i alignr_epi64(__m256i a, __m256i b, int c) {
		return Or(slli_epi64(a, 64 - c), srli_epi64(b, c));
	}

	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int16_t) {
		return alignr_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int32_t) {
		return alignr_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int64_t) {
		return alignr_epi64(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint16_t) {
		return alignr_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint32_t) {
		return alignr_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint64_t) {
		return alignr_epi64(a, b, c);
	}

	WJR_INTRINSIC_INLINE static __m256i avg_epu8(__m256i a, __m256i b) { return _mm256_avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i avg_epu16(__m256i a, __m256i b) { return _mm256_avg_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int8_t) { return avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int16_t) { return avg_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint8_t) { return avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint16_t) { return avg_epu16(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i blend_epi16(__m256i a, __m256i b) {
		return _mm256_blend_epi16(a, b, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i blend_epi32(__m256i a, __m256i b) {
		return _mm256_blend_epi32(a, b, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i blendv_epi8(__m256i a, __m256i b, __m256i mask) {
		return _mm256_blendv_epi8(a, b, mask);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i bslli_epi128(__m256i a) { return _mm256_bslli_epi128(a, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i bsrli_epi128(__m256i a) { return _mm256_bsrli_epi128(a, imm8); }

	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi8(__m256i a, __m256i b) { return _mm256_cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi16(__m256i a, __m256i b) { return _mm256_cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi32(__m256i a, __m256i b) { return _mm256_cmpeq_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi64(__m256i a, __m256i b) { return _mm256_cmpeq_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int8_t) { return cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int16_t) { return cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int32_t) { return cmpeq_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int64_t) { return cmpeq_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint8_t) { return cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint16_t) { return cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint32_t) { return cmpeq_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint64_t) { return cmpeq_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpge_epi8(__m256i a, __m256i b) {
		return Not(cmpgt_epi8(b, a));
	}
	WJR_INTRINSIC_INLINE static __m256i cmpge_epi16(__m256i a, __m256i b) {
		return Not(cmpgt_epi16(b, a));
	}
	WJR_INTRINSIC_INLINE static __m256i cmpge_epi32(__m256i a, __m256i b) {
		return Not(cmpgt_epi32(b, a));
	}

	WJR_INTRINSIC_INLINE static __m256i cmpge_epu8(__m256i a, __m256i b) {
		return Not(cmpgt_epu8(b, a));
	}
	WJR_INTRINSIC_INLINE static __m256i cmpge_epu16(__m256i a, __m256i b) {
		return Not(cmpgt_epu16(b, a));
	}
	WJR_INTRINSIC_INLINE static __m256i cmpge_epu32(__m256i a, __m256i b) {
		return Not(cmpgt_epu32(b, a));
	}

	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int8_t) { return cmpge_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int16_t) { return cmpge_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int32_t) { return cmpge_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint8_t) { return cmpge_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint16_t) { return cmpge_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint32_t) { return cmpge_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi8(__m256i a, __m256i b) { return _mm256_cmpgt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi16(__m256i a, __m256i b) { return _mm256_cmpgt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi32(__m256i a, __m256i b) { return _mm256_cmpgt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi64(__m256i a, __m256i b) { return _mm256_cmpgt_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu8(__m256i a, __m256i b) {
#if defined(__XOP__)
		return concat(sse::cmpgt_epu8(getlow(a), getlow(b)), sse::cmpgt_epu8(gethigh(a), gethigh(b)));
#else
		return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu16(__m256i a, __m256i b) {
#if defined(__XOP__)
		return concat(sse::cmpgt_epu16(getlow(a), getlow(b)), sse::cmpgt_epu16(gethigh(a), gethigh(b)));
#else
		return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu32(__m256i a, __m256i b) {
#if defined(__XOP__)
		return concat(sse::cmpgt_epu32(getlow(a), getlow(b)), sse::cmpgt_epu32(gethigh(a), gethigh(b)));
#else
		return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu64(__m256i a, __m256i b) {
#if defined(__XOP__)
		return concat(sse::cmpgt_epu64(getlow(a), getlow(b)), sse::cmpgt_epu64(gethigh(a), gethigh(b)));
#else
		return cmpgt_epi64(Xor(a, setmin_epi64()), Xor(b, setmin_epi64()));
#endif // __XOP__
	}

	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int8_t) { return cmpgt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int16_t) { return cmpgt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int32_t) { return cmpgt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int64_t) { return cmpgt_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint8_t) { return cmpgt_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint16_t) { return cmpgt_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint32_t) { return cmpgt_epu32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint64_t) { return cmpgt_epu64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmple_epi8(__m256i a, __m256i b) {
		return Not(cmpgt_epi8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m256i cmple_epi16(__m256i a, __m256i b) {
		return Not(cmpgt_epi16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m256i cmple_epi32(__m256i a, __m256i b) {
		return Not(cmpgt_epi32(a, b));
	}

	WJR_INTRINSIC_INLINE static __m256i cmple_epu8(__m256i a, __m256i b) {
		return Not(cmpgt_epu8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m256i cmple_epu16(__m256i a, __m256i b) {
		return Not(cmpgt_epu16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m256i cmple_epu32(__m256i a, __m256i b) {
		return Not(cmpgt_epu32(a, b));
	}

	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int8_t) { return cmple_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int16_t) { return cmple_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int32_t) { return cmple_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint8_t) { return cmple_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint16_t) { return cmple_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint32_t) { return cmple_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmplt_epi8(__m256i a, __m256i b) { return cmpgt_epi8(b, a); }
	WJR_INTRINSIC_INLINE static __m256i cmplt_epi16(__m256i a, __m256i b) { return cmpgt_epi16(b, a); }
	WJR_INTRINSIC_INLINE static __m256i cmplt_epi32(__m256i a, __m256i b) { return cmpgt_epi32(b, a); }

	WJR_INTRINSIC_INLINE static __m256i cmplt_epu8(__m256i a, __m256i b) {
		return cmpgt_epu8(b, a);
	}
	WJR_INTRINSIC_INLINE static __m256i cmplt_epu16(__m256i a, __m256i b) {
		return cmpgt_epu16(b, a);
	}
	WJR_INTRINSIC_INLINE static __m256i cmplt_epu32(__m256i a, __m256i b) {
		return cmpgt_epu32(b, a);
	}

	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int8_t) { return cmplt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int16_t) { return cmplt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int32_t) { return cmplt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint8_t) { return cmplt_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint16_t) { return cmplt_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint32_t) { return cmplt_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpne_epi8(__m256i a, __m256i b) { return Not(cmpeq_epi8(a, b)); }
	WJR_INTRINSIC_INLINE static __m256i cmpne_epi16(__m256i a, __m256i b) { return Not(cmpeq_epi16(a, b)); }
	WJR_INTRINSIC_INLINE static __m256i cmpne_epi32(__m256i a, __m256i b) { return Not(cmpeq_epi32(a, b)); }

	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int8_t) { return cmpne_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int16_t) { return cmpne_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int32_t) { return cmpne_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint8_t) { return cmpne_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint16_t) { return cmpne_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint32_t) { return cmpne_epi32(a, b); }

	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::equal_to<>, T) {
		return cmpeq(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::not_equal_to<>, T) {
		return cmpne(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater<>, T) {
		return cmpgt(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater_equal<>, T) {
		return cmpge(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less<>, T) {
		return cmplt(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less_equal<>, T) {
		return cmple(a, b, T());
	}

	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu8(__m128i v);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu16(__m128i v);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu32(__m128i v);

	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, int8_t) { return cvtepu8<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, int16_t) { return cvtepu16<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, int32_t) { return cvtepu32<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, uint8_t) { return cvtepu8<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, uint16_t) { return cvtepu16<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, uint32_t) { return cvtepu32<T>(v); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi8(__m256i v) { return _mm256_extract_epi8(v, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi16(__m256i v) { return _mm256_extract_epi16(v, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m256i v, int8_t) { return extract_epi8<imm8>(v); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m256i v, int16_t) { return extract_epi16<imm8>(v); }

	WJR_INTRINSIC_INLINE static __m256i hadd_epi16(__m256i a, __m256i b) { return _mm256_hadd_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i hadd_epi32(__m256i a, __m256i b) { return _mm256_hadd_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int16_t) { return hadd_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int32_t) { return hadd_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hadds_epi16(__m256i a, __m256i b) { return _mm256_hadds_epi16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hsub_epi16(__m256i a, __m256i b) { return _mm256_hsub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i hsub_epi32(__m256i a, __m256i b) { return _mm256_hsub_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int16_t) { return hsub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int32_t) { return hsub_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hsubs_epi16(__m256i a, __m256i b) { return _mm256_hsubs_epi16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i madd_epi16(__m256i a, __m256i b) {
		return _mm256_madd_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i max_epi8(__m256i a, __m256i b) {
		return _mm256_max_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epi16(__m256i a, __m256i b) {
		return _mm256_max_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epi32(__m256i a, __m256i b) {
		return _mm256_max_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epu8(__m256i a, __m256i b) {
		return _mm256_max_epu8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epu16(__m256i a, __m256i b) {
		return _mm256_max_epu16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epu32(__m256i a, __m256i b) {
		return _mm256_max_epu32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int8_t) { return max_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int16_t) { return max_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint8_t) { return max_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint16_t) { return max_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int32_t) { return max_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint32_t) { return max_epu32(a, b); }

	WJR_INTRINSIC_INLINE static int8_t max_epi8(__m256i a) {
		return sse::max_epi8(sse::max_epi8(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static int16_t max_epi16(__m256i a) {
		return sse::max_epi16(sse::max_epi16(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static int32_t max_epi32(__m256i a) {
		return sse::max_epi32(sse::max_epi32(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m256i a) {
		return sse::max_epu8(sse::max_epu8(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m256i a) {
		return sse::max_epu16(sse::max_epu16(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m256i a) {
		return sse::max_epu32(sse::max_epu32(getlow(a), gethigh(a)));
	}

	WJR_INTRINSIC_INLINE static int8_t max(__m256i a, int8_t) { return max_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t max(__m256i a, int16_t) { return max_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t max(__m256i a, int32_t) { return max_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t max(__m256i a, uint8_t) { return max_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t max(__m256i a, uint16_t) { return max_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t max(__m256i a, uint32_t) { return max_epu32(a); }

	WJR_INTRINSIC_INLINE static __m256i min_epi8(__m256i a, __m256i b) {
		return _mm256_min_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epi16(__m256i a, __m256i b) {
		return _mm256_min_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epi32(__m256i a, __m256i b) {
		return _mm256_min_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epu8(__m256i a, __m256i b) {
		return _mm256_min_epu8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epu16(__m256i a, __m256i b) {
		return _mm256_min_epu16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epu32(__m256i a, __m256i b) {
		return _mm256_min_epu32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int8_t) { return min_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int16_t) { return min_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint8_t) { return min_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint16_t) { return min_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int32_t) { return min_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint32_t) { return min_epu32(a, b); }

	WJR_INTRINSIC_INLINE static int8_t min_epi8(__m256i a) {
		return sse::min_epi8(sse::min_epi8(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static int16_t min_epi16(__m256i a) {
		return sse::min_epi16(sse::min_epi16(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static int32_t min_epi32(__m256i a) {
		return sse::min_epi32(sse::min_epi32(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m256i a) {
		return sse::min_epu8(sse::min_epu8(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m256i a) {
		return sse::min_epu16(sse::min_epu16(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m256i a) {
		return sse::min_epu32(sse::min_epu32(getlow(a), gethigh(a)));
	}

	WJR_INTRINSIC_INLINE static int8_t min(__m256i a, int8_t) { return min_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t min(__m256i a, int16_t) { return min_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t min(__m256i a, int32_t) { return min_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t min(__m256i a, uint8_t) { return min_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t min(__m256i a, uint16_t) { return min_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t min(__m256i a, uint32_t) { return min_epu32(a); }

	WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m256i a) {
		return _mm256_movemask_epi8(a);
	}

	WJR_INTRINSIC_INLINE static __m256i mul_epi32(__m256i a, __m256i b) {
		return _mm256_mul_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i mul_epu32(__m256i a, __m256i b) {
		return _mm256_mul_epu32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i mulhi_epi16(__m256i a, __m256i b) {
		return _mm256_mulhi_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i mulhi_epu16(__m256i a, __m256i b) {
		return _mm256_mulhi_epu16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i mullo_epi16(__m256i a, __m256i b) {
		return _mm256_mullo_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i packs_epi16(__m256i a, __m256i b) {
		return _mm256_packs_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i packs_epi32(__m256i a, __m256i b) {
		return _mm256_packs_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i packus_epi16(__m256i a, __m256i b) {
		return _mm256_packus_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i packus_epi32(__m256i a, __m256i b) {
		return _mm256_packus_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i shuffle_epi8(__m256i a, __m256i b) {
		return _mm256_shuffle_epi8(a, b);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i shuffle_epi32(__m256i a) {
		return _mm256_shuffle_epi32(a, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i shufflehi_epi16(__m256i a) {
		return _mm256_shufflehi_epi16(a, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i shufflelo_epi16(__m256i a) {
		return _mm256_shufflelo_epi16(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i sll_epi16(__m256i a, __m128i b) {
		return _mm256_sll_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sll_epi32(__m256i a, __m128i b) {
		return _mm256_sll_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sll_epi64(__m256i a, __m128i b) {
		return _mm256_sll_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int16_t) { return sll_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int32_t) { return sll_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int64_t) { return sll_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint64_t) { return sll_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i slli_epi16(__m256i a, int imm8) {
		return _mm256_slli_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i slli_epi32(__m256i a, int imm8) {
		return _mm256_slli_epi32(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i slli_epi64(__m256i a, int imm8) {
		return _mm256_slli_epi64(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }

	WJR_INTRINSIC_INLINE static __m256i sra_epi16(__m256i a, __m128i b) {
		return _mm256_sra_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sra_epi32(__m256i a, __m128i b) {
		return _mm256_sra_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int16_t) { return sra_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int32_t) { return sra_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i srai_epi16(__m256i a, int imm8) {
		return _mm256_srai_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i srai_epi32(__m256i a, int imm8) {
		return _mm256_srai_epi32(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int32_t) { return srai_epi32(a, imm8); }

	WJR_INTRINSIC_INLINE static __m256i stream_load(__m256i const* p) {
		return _mm256_stream_load_si256(p);
	}

	WJR_INTRINSIC_INLINE static __m256i srl_epi16(__m256i a, __m128i b) {
		return _mm256_srl_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i srl_epi32(__m256i a, __m128i b) {
		return _mm256_srl_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i srl_epi64(__m256i a, __m128i b) {
		return _mm256_srl_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int16_t) { return srl_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int32_t) { return srl_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int64_t) { return srl_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint64_t) { return srl_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i srli_epi16(__m256i a, int imm8) {
		return _mm256_srli_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i srli_epi32(__m256i a, int imm8) {
		return _mm256_srli_epi32(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i srli_epi64(__m256i a, int imm8) {
		return _mm256_srli_epi64(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }

	WJR_INTRINSIC_INLINE static __m256i sub_epi8(__m256i a, __m256i b) {
		return _mm256_sub_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sub_epi16(__m256i a, __m256i b) {
		return _mm256_sub_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sub_epi32(__m256i a, __m256i b) {
		return _mm256_sub_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sub_epi64(__m256i a, __m256i b) {
		return _mm256_sub_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int8_t) { return sub_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int16_t) { return sub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int32_t) { return sub_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int64_t) { return sub_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint8_t) { return sub_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint16_t) { return sub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint32_t) { return sub_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint64_t) { return sub_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i subs_epi8(__m256i a, __m256i b) {
		return _mm256_subs_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i subs_epi16(__m256i a, __m256i b) {
		return _mm256_subs_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i subs_epu8(__m256i a, __m256i b) {
		return _mm256_subs_epu8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i subs_epu16(__m256i a, __m256i b) {
		return _mm256_subs_epu16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int8_t) { return subs_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int16_t) { return subs_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint8_t) { return subs_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint16_t) { return subs_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi8(__m256i a, __m256i b) {
		return _mm256_unpackhi_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi16(__m256i a, __m256i b) {
		return _mm256_unpackhi_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi32(__m256i a, __m256i b) {
		return _mm256_unpackhi_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi64(__m256i a, __m256i b) {
		return _mm256_unpackhi_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int8_t) { return unpackhi_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int16_t) { return unpackhi_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int32_t) { return unpackhi_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int64_t) { return unpackhi_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint8_t) { return unpackhi_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint16_t) { return unpackhi_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint32_t) { return unpackhi_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint64_t) { return unpackhi_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi8(__m256i a, __m256i b) {
		return _mm256_unpacklo_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi16(__m256i a, __m256i b) {
		return _mm256_unpacklo_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi32(__m256i a, __m256i b) {
		return _mm256_unpacklo_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi64(__m256i a, __m256i b) {
		return _mm256_unpacklo_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int8_t) { return unpacklo_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int16_t) { return unpacklo_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int32_t) { return unpacklo_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int64_t) { return unpacklo_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint8_t) { return unpacklo_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint16_t) { return unpacklo_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint32_t) { return unpacklo_epi32(a, b); }
#endif // __AVX2__

};

_WJR_SIMD_END

#endif // __WJR_SIMD_SIMD_INTRIN_H                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      

#if defined(__HAS_FAST_MEMCHR)

#define __WJR_MEMCHR_ONE(st, _s)	                            \
	auto r = st::cmp(x, y, pred, T());	                        \
	st::mask_type z = st::movemask_epi8(r);	                    \
	if(z != 0){	                                                \
		return (_s) + wjr::countr_zero(z) / _Mysize;	        \
	}

#define __WJR_MEMCHR_FOUR(st, _s0, _s1, _s2, _s3)	            \
	auto r0 = st::cmp(x0, y, pred, T());	                    \
	auto r1 = st::cmp(x1, y, pred, T());	                    \
	auto r2 = st::cmp(x2, y, pred, T());	                    \
	auto r3 = st::cmp(x3, y, pred, T());	                    \
	                                                            \
	r3 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	            \
	st::mask_type z = st::movemask_epi8(r3);	                \
	if(z != 0){	                                                \
		st::mask_type tmp = st::movemask_epi8(r0);	            \
		if(tmp != 0){	                                        \
			return (_s0) + wjr::countr_zero(tmp) / _Mysize;	    \
		}	                                                    \
		tmp = st::movemask_epi8(r1);	                        \
		if(tmp != 0){	                                        \
			return (_s1) + wjr::countr_zero(tmp) / _Mysize;	    \
		}	                                                    \
		tmp = st::movemask_epi8(r2);	                        \
		if(tmp != 0){	                                        \
			return (_s2) + wjr::countr_zero(tmp) / _Mysize;	    \
		}	                                                    \
		tmp = z;	                                            \
		return (_s3) + wjr::countr_zero(tmp) / _Mysize;	        \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memchr(const T* s, T val, size_t n, _Pred pred) {
	using namespace wjr::literals;
	constexpr size_t _Mysize = sizeof(T);

#if defined(__AVX2__)
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // __AVX2__
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	if (is_unlikely(n == 0)) return s;

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			const T* _lst;

			auto y = simd_t::set1(val, T());

			if (is_constant_p(reinterpret_cast<uintptr_t>(s) % bound) &&
				reinterpret_cast<uintptr_t>(s) % bound == 0) {
				// do nothing
			}
			else if (_Mysize == 1 ||
				reinterpret_cast<uintptr_t>(s) % _Mysize == 0) {
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

				__WJR_MEMCHR_ONE(simd_t, s);

				auto __align_s = bound - reinterpret_cast<uintptr_t>(s) % bound;
				s += __align_s / _Mysize;
				n -= __align_s / _Mysize;
				if (is_unlikely(n < width * 4)) {
					_lst = s + n;
					goto WJR_MACRO_LABEL(aft_align);
				}
			}
			else {
				// unalign algorithm
				do {
					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s));
					auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s + width));
					auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 3));

					__WJR_MEMCHR_FOUR(simd_t, s, s + width, s + width * 2, s + width * 3);

					s += width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				_lst = s + n;
				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMCHR_ONE(simd_t, s);
						s += width;
					}
					case 3: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMCHR_ONE(simd_t, s);
						s += width;
					}
					case 2: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMCHR_ONE(simd_t, s);
					}
					case 1: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst - width));

						__WJR_MEMCHR_ONE(simd_t, _lst - width);
					}
					}
				}
				return _lst;
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s + width));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s + width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s + width * 3));

				__WJR_MEMCHR_FOUR(simd_t, s, s + width, s + width * 2, s + width * 3);

				s += width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			_lst = s + n;
			if (n != 0) {
				switch ((n + width - 1) / width) {
				default: unreachable(); break;
				case 4: {
					WJR_MACRO_LABEL(aft_align) :
						auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMCHR_ONE(simd_t, s);
					s += width;
				}
				case 3: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMCHR_ONE(simd_t, s);
					s += width;
				}
				case 2: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMCHR_ONE(simd_t, s);
				}
				case 1: {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst - width));

					__WJR_MEMCHR_ONE(simd_t, _lst - width);
				}
				}
			}
			return _lst;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto y = simd::avx::set1(val, T());

			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + 32 / _Mysize));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + n - 64 / _Mysize));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + n - 32 / _Mysize));

			__WJR_MEMCHR_FOUR(simd::avx, s, s + 32 / _Mysize, s + n - 64 / _Mysize, s + n - 32 / _Mysize);

			return s + n;
		}
#endif // __AVX2__

		auto y = simd::sse::set1(val, T());
		auto delta = (n & (32 / _Mysize)) >> 1;

		auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s));
		auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + delta));
		auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + n - 16 / _Mysize - delta));
		auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + n - 16 / _Mysize));

		__WJR_MEMCHR_FOUR(simd::sse, s, s + delta, s + n - 16 / _Mysize - delta, s + n - 16 / _Mysize);

		return s + n;
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(*s, val) ? s : s + 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A = *reinterpret_cast<const uint64_t*>(s);
			auto B = *reinterpret_cast<const uint64_t*>(s + n - 4);

			auto x = simd::sse::set_epi64x(B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0)return s + n;
			auto i = wjr::countr_zero(z) / _Mysize;
			auto q = i >= 4 ? i + n - 8 : i;
			return s + q;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A = *reinterpret_cast<const uint32_t*>(s);
			auto B = *reinterpret_cast<const uint32_t*>(s + delta);
			auto C = *reinterpret_cast<const uint32_t*>(s + n - 4 - delta);
			auto D = *reinterpret_cast<const uint32_t*>(s + n - 4);

			auto x = simd::sse::set_epi32(D, C, B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0) return s + n;
			auto i = wjr::countr_zero(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s + q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (pred(s[0], val)) return s;
		if (n == 1) return s + 1;
		if (pred(s[1], val)) return s + 1;
		if (n == 2) return s + 2;
		return pred(s[2], val) ? s + 2 : s + 3;
	}
}

_WJR_ALGO_END

#undef __WJR_MEMCHR_FOUR
#undef __WJR_MEMCHR_ONE

#endif // __HAS_FAST_MEMCHR

#endif // WJR_ALGO_MEMCHR_H                                                                                                                                                                                                                                                                         
#pragma once
#ifndef __WJR_ALGO_MEMRCHR_H
#define __WJR_ALGO_MEMRCHR_H




#if defined(__HAS_FAST_MEMCHR)

#define __WJR_MEMRCHR_ONE(st, _s)	                            \
	auto r = st::cmp(x, y, pred, T());	                        \
	st::mask_type z = st::movemask_epi8(r);	                    \
	if(z != 0){	                                                \
		return (_s) - wjr::countl_zero(z) / _Mysize;	        \
	}

#define __WJR_MEMRCHR_FOUR(st, _s0, _s1, _s2, _s3)	            \
	auto r0 = st::cmp(x0, y, pred, T());	                    \
	auto r1 = st::cmp(x1, y, pred, T());	                    \
	auto r2 = st::cmp(x2, y, pred, T());	                    \
	auto r3 = st::cmp(x3, y, pred, T());	                    \
	                                                            \
	r0 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	            \
	st::mask_type z = st::movemask_epi8(r0);	                \
	if(z != 0){	                                                \
		st::mask_type tmp = st::movemask_epi8(r3);	            \
		if(tmp != 0){	                                        \
			return (_s3) - wjr::countl_zero(tmp) / _Mysize;	    \
		}	                                                    \
		tmp = st::movemask_epi8(r2);	                        \
		if(tmp != 0){	                                        \
			return (_s2) - wjr::countl_zero(tmp) / _Mysize;	    \
		}	                                                    \
		tmp = st::movemask_epi8(r1);	                        \
		if(tmp != 0){	                                        \
			return (_s1) - wjr::countl_zero(tmp) / _Mysize;	    \
		}	                                                    \
		tmp = z;	                                            \
		return (_s0) - wjr::countl_zero(tmp) / _Mysize;	        \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memrchr(const T* s, T val, size_t n, _Pred pred) {
	using namespace wjr::literals;
	constexpr size_t _Mysize = sizeof(T);

#if defined(__AVX2__)
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // __AVX2__
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	if (is_unlikely(n == 0)) return s;

	s += n;

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			const T* _lst;

			auto y = simd_t::set1(val, T());

			if (is_constant_p(reinterpret_cast<uintptr_t>(s) % bound) &&
				reinterpret_cast<uintptr_t>(s) % bound == 0) {
				// do nothing
			}
			else if (_Mysize == 1 ||
				reinterpret_cast<uintptr_t>(s) % _Mysize == 0) {
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s - width));

				__WJR_MEMRCHR_ONE(simd_t, s);

				auto __align_s = reinterpret_cast<uintptr_t>(s) % bound;
				s -= __align_s / _Mysize;
				n -= __align_s / _Mysize;
				if (is_unlikely(n < width * 4)) {
					// n = [width * 3, width * 4)
					_lst = s - n;
					goto WJR_MACRO_LABEL(aft_align);
				}
			}
			else {
				// unalign algorithm
				do {
					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 4));
					auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 3));
					auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s - width));

					__WJR_MEMRCHR_FOUR(simd_t, s - width * 3, s - width * 2, s - width, s);

					s -= width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				_lst = s - n;
				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						s -= width;
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMRCHR_ONE(simd_t, s + width);
					}
					case 3: {
						s -= width;
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMRCHR_ONE(simd_t, s + width);
					}
					case 2: {
						s -= width;
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMRCHR_ONE(simd_t, s + width);
					}
					case 1: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst));

						__WJR_MEMRCHR_ONE(simd_t, _lst + width);
					}
					}
				}
				return _lst;
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s - width * 4));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s - width * 3));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s - width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s - width));

				__WJR_MEMRCHR_FOUR(simd_t, s - width * 3, s - width * 2, s - width, s);

				s -= width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			_lst = s - n;
			if (n != 0) {
				switch ((n + width - 1) / width) {
				default: unreachable(); break;
				case 4: {
					WJR_MACRO_LABEL(aft_align) :
						s -= width;
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMRCHR_ONE(simd_t, s + width);
				}
				case 3: {
					s -= width;
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMRCHR_ONE(simd_t, s + width);
				}
				case 2: {
					s -= width;
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMRCHR_ONE(simd_t, s + width);
				}
				case 1: {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst));

					__WJR_MEMRCHR_ONE(simd_t, _lst + width);
				}
				}
			}
			return _lst;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto y = simd::avx::set1(val, T());

			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - n));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - n + 32 / _Mysize));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - 64 / _Mysize));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - 32 / _Mysize));

			__WJR_MEMRCHR_FOUR(simd::avx, s - n + 32 / _Mysize, s - n + 64 / _Mysize, s - 32 / _Mysize, s);

			return s - n;
		}
#endif // __AVX2__

		auto y = simd::sse::set1(val, T());
		auto delta = (n & (32 / _Mysize)) >> 1;

		auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - n));
		auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - n + delta));
		auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - 16 / _Mysize - delta));
		auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - 16 / _Mysize));

		__WJR_MEMRCHR_FOUR(simd::sse, s - n + 16 / _Mysize, s - n + delta + 16 / _Mysize, s - delta, s);

		return s - n;
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s[-1], val) ? s : s - 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A = *reinterpret_cast<const uint64_t*>(s - n);
			auto B = *reinterpret_cast<const uint64_t*>(s - 4);

			auto x = simd::sse::set_epi64x(B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0)return s - n;
			auto i = wjr::countl_zero(z) / _Mysize;
			auto q = i >= 4 ? i + n - 4 : i;
			return s - q;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A = *reinterpret_cast<const uint32_t*>(s - n);
			auto B = *reinterpret_cast<const uint32_t*>(s - n + delta);
			auto C = *reinterpret_cast<const uint32_t*>(s - 4 - delta);
			auto D = *reinterpret_cast<const uint32_t*>(s - 4);

			auto x = simd::sse::set_epi32(D, C, B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0) return s - n;
			size_t i = wjr::countl_zero(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s - q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (pred(s[-1], val)) return s;
		if (n == 1) return s - 1;
		if (pred(s[-2], val)) return s - 1;
		if (n == 2) return s - 2;
		return pred(s[-3], val) ? s - 2 : s - 3;
	}
}

_WJR_ALGO_END

#undef __WJR_MEMRCHR_FOUR
#undef __WJR_MEMRCHR_ONE

#endif // __HAS_FAST_MEMCHR

#endif // __WJR_ALGO_MEMRCHR_H                                                                                                                                                                                                                                                                              
#pragma once
#ifndef __WJR_ALGO_MEMCMP_H
#define __WJR_ALGO_MEMCMP_H




#if defined(__HAS_FAST_MEMCMP)

#define __WJR_MEMCMP_ONE_NORMAL(st)														\
	auto r = st::cmp(x, y, pred, T());	                                                \
	st::mask_type z = st::movemask_epi8(r);		                                        \
	if(z != st::mask()){	                                                            \
		return false;	                                                                \
	}

#define __WJR_MEMCMP_FOUR_NORMAL(st)													\
	auto r0 = st::cmp(x0, y0, pred, T());	                                            \
	auto r1 = st::cmp(x1, y1, pred, T());	                                            \
	auto r2 = st::cmp(x2, y2, pred, T());	                                            \
	auto r3 = st::cmp(x3, y3, pred, T());	                                            \
		                                                                                \
	r3 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
	st::mask_type z = st::movemask_epi8(r3);		                                    \
	if(z != st::mask()){	                                                            \
		return false;	                                                                \
	}

#define __WJR_MEMCMP_ONE(st) __WJR_MEMCMP_ONE_NORMAL(st)
#define __WJR_MEMCMP_FOUR(st) __WJR_MEMCMP_FOUR_NORMAL(st)

#if defined(__SSE4_1__)
#undef __WJR_MEMCMP_ONE
#undef __WJR_MEMCMP_FOUR

#define __WJR_MEMCMP_ONE(st)															\
	if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                            \
		auto r = st::Xor(x, y);	                                                        \
		auto z = st::test_all_zeros(r);	                                                \
		if(!z){	                                                                        \
			return false;	                                                            \
		}	                                                                            \
	}	                                                                                \
	else{	                                                                            \
		__WJR_MEMCMP_ONE_NORMAL(st)														\
	}

#define __WJR_MEMCMP_FOUR(st)															\
	if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                            \
		auto r0 = st::Xor(x0, y0);	                                                    \
		auto r1 = st::Xor(x1, y1);	                                                    \
		auto r2 = st::Xor(x2, y2);	                                                    \
		auto r3 = st::Xor(x3, y3);	                                                    \
				                                                                        \
		r3 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	                                \
		auto z = st::test_all_zeros(r3);	                                            \
		if(!z){	                                                                        \
			return false;	                                                            \
		}	                                                                            \
	}	                                                                                \
	else{	                                                                            \
		__WJR_MEMCMP_FOUR_NORMAL(st)													\
	}
#endif // __SSE4_1__

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
bool __memcmp(const T* s0, const T* s1, size_t n, _Pred pred) {
	using namespace wjr::literals;
	constexpr size_t _Mysize = sizeof(T);

#if defined(__AVX2__)
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // __AVX2__
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	if (is_unlikely(n == 0)) return true;

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			const T* _lst0;
			const T* _lst1;

			auto _off0 = reinterpret_cast<uintptr_t>(s0) % bound;
			auto _off1 = reinterpret_cast<uintptr_t>(s1) % bound;

			// align 2 pointer
			if (_off0 == _off1 &&
				(_Mysize == 1 || _off0 % _Mysize == 0)) {
				if (is_constant_p(_off0) && _off0 == 0) {
					// do nothing
				}
				else {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

					__WJR_MEMCMP_ONE(simd_t);

					auto __align_s = bound - _off0;
					s0 += __align_s / _Mysize;
					s1 += __align_s / _Mysize;
					n -= __align_s / _Mysize;
					if (is_unlikely(n < width * 4)) {
						_lst0 = s0 + n;
						_lst1 = s1 + n;
						goto WJR_MACRO_LABEL(aft_align);
					}
				}
			}
			// unalign algorithm
			else {
				do {
					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
					auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
					auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

					auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
					auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
					auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
					auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

					__WJR_MEMCMP_FOUR(simd_t);

					s0 += width * 4;
					s1 += width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				_lst0 = s0 + n;
				_lst1 = s1 + n;

				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

						__WJR_MEMCMP_ONE(simd_t);
						s0 += width;
						s1 += width;
					}
					case 3: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

						__WJR_MEMCMP_ONE(simd_t);
						s0 += width;
						s1 += width;
					}
					case 2: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

						__WJR_MEMCMP_ONE(simd_t);
						s0 += width;
						s1 += width;
					}
					case 1: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst0 - width));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(_lst1 - width));

						__WJR_MEMCMP_ONE(simd_t);
					}
					}
				}
				return true;
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 + width));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 3));

				auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 + width));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 2));
				auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 3));

				__WJR_MEMCMP_FOUR(simd_t);

				s0 += width * 4;
				s1 += width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			_lst0 = s0 + n;
			_lst1 = s1 + n;

			if (n != 0) {
				switch ((n + width - 1) / width) {
				default: unreachable(); break;
				case 4: {
					WJR_MACRO_LABEL(aft_align) :
						auto x = simd_t::load(reinterpret_cast<const sint*>(s0));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1));

					__WJR_MEMCMP_ONE(simd_t);
					s0 += width;
					s1 += width;
				}
				case 3: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s0));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1));

					__WJR_MEMCMP_ONE(simd_t);
					s0 += width;
					s1 += width;
				}
				case 2: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s0));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1));

					__WJR_MEMCMP_ONE(simd_t);
					s0 += width;
					s1 += width;
				}
				case 1: {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst0 - width));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(_lst1 - width));

					__WJR_MEMCMP_ONE(simd_t);
				}
				}
			}

			return true;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + 32 / _Mysize));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + n - 64 / _Mysize));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + n - 32 / _Mysize));

			auto y0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1));
			auto y1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + 32 / _Mysize));
			auto y2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + n - 64 / _Mysize));
			auto y3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + n - 32 / _Mysize));

			__WJR_MEMCMP_FOUR(simd::avx);

			return true;
		}
#endif // __AVX2__

		auto delta = (n & (32 / _Mysize)) >> 1;

		auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
		auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + delta));
		auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + n - 16 / _Mysize - delta));
		auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + n - 16 / _Mysize));

		auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));
		auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + delta));
		auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + n - 16 / _Mysize - delta));
		auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + n - 16 / _Mysize));

		__WJR_MEMCMP_FOUR(simd::sse);

		return true;
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s0[0], s1[0]);
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A0 = *reinterpret_cast<const uint64_t*>(s0);
			auto B0 = *reinterpret_cast<const uint64_t*>(s0 + n - 4);

			auto A1 = *reinterpret_cast<const uint64_t*>(s1);
			auto B1 = *reinterpret_cast<const uint64_t*>(s1 + n - 4);

			auto x = simd::sse::set_epi64x(B0, A0);
			auto y = simd::sse::set_epi64x(B1, A1);

			__WJR_MEMCMP_ONE(simd::sse);

			return true;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A0 = *reinterpret_cast<const uint32_t*>(s0);
			auto B0 = *reinterpret_cast<const uint32_t*>(s0 + delta);
			auto C0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4 - delta);
			auto D0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4);

			auto A1 = *reinterpret_cast<const uint32_t*>(s1);
			auto B1 = *reinterpret_cast<const uint32_t*>(s1 + delta);
			auto C1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4 - delta);
			auto D1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4);

			auto x = simd::sse::set_epi32(D0, C0, B0, A0);
			auto y = simd::sse::set_epi32(D1, C1, B1, A1);

			__WJR_MEMCMP_ONE(simd::sse);

			return true;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (!pred(s0[0], s1[0])) return false;
		if (n == 1) return true;
		if (!pred(s0[1], s1[1])) return false;
		if (n == 2) return true;
		return pred(s0[2], s1[2]);
	}
}

_WJR_ALGO_END

#undef __WJR_MEMCMP_ONE
#undef __WJR_MEMCMP_FOUR
#undef __WJR_MEMCMP_ONE_NORMAL
#undef __WJR_MEMCMP_FOUR_NORMAL

#endif // __AVX2__ || __SSE2__

#endif // __WJR_ALGO_MEMCMP_H                                                                                                                                                                                                                                                                                                                                                    
#pragma once
#ifndef __WJR_ALGO_MEMMIS_H
#define __WJR_ALGO_MEMMIS_H




#if defined(__HAS_FAST_MEMMIS)

#define __WJR_MEMMIS_ONE(st, _s)														\
	auto r = st::cmp(x, y, pred, T());	                                                \
	st::mask_type z = st::movemask_epi8(r);		                                        \
	if(z != st::mask()){	                                                            \
		return (_s) + wjr::countr_one(z) / _Mysize;		                                \
	}

#define __WJR_MEMMIS_FOUR(st, _s0, _s1, _s2, _s3)										\
	auto r0 = st::cmp(x0, y0, pred, T());	                                            \
	auto r1 = st::cmp(x1, y1, pred, T());	                                            \
	auto r2 = st::cmp(x2, y2, pred, T());	                                            \
	auto r3 = st::cmp(x3, y3, pred, T());	                                            \
		                                                                                \
	r3 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
	st::mask_type z = st::movemask_epi8(r3);		                                    \
	if(z != st::mask()){	                                                            \
		st::mask_type tmp = st::movemask_epi8(r0);	                                    \
		if(tmp != st::mask()){	                                                        \
			return (_s0) + wjr::countr_one(tmp) / _Mysize;	                            \
		}	                                                                            \
		tmp = st::movemask_epi8(r1);	                                                \
		if(tmp != st::mask()){	                                                        \
			return (_s1) + wjr::countr_one(tmp) / _Mysize;	                            \
		}	                                                                            \
		tmp = st::movemask_epi8(r2);	                                                \
		if(tmp != st::mask()){	                                                        \
			return (_s2) + wjr::countr_one(tmp) / _Mysize;	                            \
		}	                                                                            \
		tmp = z;	                                                                    \
		return (_s3) + wjr::countr_one(tmp) / _Mysize;	                                \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memmis(const T* s0, const T* s1, size_t n, _Pred pred) {
	using namespace wjr::literals;
	constexpr size_t _Mysize = sizeof(T);

#if defined(__AVX2__)
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // __AVX2__
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	if (is_unlikely(n == 0)) return s0;

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			const T* _lst0;
			const T* _lst1;

			auto _off0 = reinterpret_cast<uintptr_t>(s0) % bound;
			auto _off1 = reinterpret_cast<uintptr_t>(s1) % bound;

			// align 2 pointer
			if (_off0 == _off1 &&
				(_Mysize == 1 || _off0 % _Mysize == 0)) {
				if (is_constant_p(_off0) && _off0 == 0) {
					// do nothing
				}
				else {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

					__WJR_MEMMIS_ONE(simd_t, s0);

					auto __align_s = bound - _off0;
					s0 += __align_s / _Mysize;
					s1 += __align_s / _Mysize;
					n -= __align_s / _Mysize;
					if (is_unlikely(n < width * 4)) {
						_lst0 = s0 + n;
						_lst1 = s1 + n;
						goto WJR_MACRO_LABEL(aft_align);
					}
				}
			}
			// unalign algorithm
			else {
				do {
					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
					auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
					auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

					auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
					auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
					auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
					auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

					__WJR_MEMMIS_FOUR(simd_t, s0, s0 + width, s0 + width * 2, s0 + width * 3);

					s0 += width * 4;
					s1 += width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				_lst0 = s0 + n;
				_lst1 = s1 + n;

				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

						__WJR_MEMMIS_ONE(simd_t, s0);
						s0 += width;
						s1 += width;
					}
					case 3: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

						__WJR_MEMMIS_ONE(simd_t, s0);
						s0 += width;
						s1 += width;
					}
					case 2: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

						__WJR_MEMMIS_ONE(simd_t, s0);
					}
					case 1: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst0 - width));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(_lst1 - width));

						__WJR_MEMMIS_ONE(simd_t, _lst0 - width);
					}
					}
				}
				return _lst0;
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 + width));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 3));

				auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 + width));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 2));
				auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 3));

				__WJR_MEMMIS_FOUR(simd_t, s0, s0 + width, s0 + width * 2, s0 + width * 3);

				s0 += width * 4;
				s1 += width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			_lst0 = s0 + n;
			_lst1 = s1 + n;

			if (n != 0) {
				switch ((n + width - 1) / width) {
				default: unreachable(); break;
				case 4: {
					WJR_MACRO_LABEL(aft_align) :
						auto x = simd_t::load(reinterpret_cast<const sint*>(s0));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1));

					__WJR_MEMMIS_ONE(simd_t, s0);
					s0 += width;
					s1 += width;
				}
				case 3: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s0));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1));

					__WJR_MEMMIS_ONE(simd_t, s0);
					s0 += width;
					s1 += width;
				}
				case 2: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s0));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1));

					__WJR_MEMMIS_ONE(simd_t, s0);
				}
				case 1: {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst0 - width));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(_lst1 - width));

					__WJR_MEMMIS_ONE(simd_t, _lst0 - width);
				}
				}
			}

			return _lst0;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + 32 / _Mysize));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + n - 64 / _Mysize));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + n - 32 / _Mysize));

			auto y0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1));
			auto y1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + 32 / _Mysize));
			auto y2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + n - 64 / _Mysize));
			auto y3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + n - 32 / _Mysize));

			__WJR_MEMMIS_FOUR(simd::avx, s0, s0 + 32 / _Mysize, s0 + n - 64 / _Mysize, s0 + n - 32 / _Mysize);

			return s0 + n;
		}
#endif // __AVX2__

		auto delta = (n & (32 / _Mysize)) >> 1;

		auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
		auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + delta));
		auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + n - 16 / _Mysize - delta));
		auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + n - 16 / _Mysize));

		auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));
		auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + delta));
		auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + n - 16 / _Mysize - delta));
		auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + n - 16 / _Mysize));

		__WJR_MEMMIS_FOUR(simd::sse, s0, s0 + delta, s0 + n - 16 / _Mysize - delta, s0 + n - 16 / _Mysize);

		return s0 + n;
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s0[0], s1[0]) ? s0 + 1 : s0;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A0 = *reinterpret_cast<const uint64_t*>(s0);
			auto B0 = *reinterpret_cast<const uint64_t*>(s0 + n - 4);

			auto A1 = *reinterpret_cast<const uint64_t*>(s1);
			auto B1 = *reinterpret_cast<const uint64_t*>(s1 + n - 4);

			auto x = simd::sse::set_epi64x(B0, A0);
			auto y = simd::sse::set_epi64x(B1, A1);
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == simd::sse::mask())return s0 + n;
			auto i = wjr::countr_one(z) / _Mysize;
			auto q = i >= 4 ? i + n - 8 : i;
			return s0 + q;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A0 = *reinterpret_cast<const uint32_t*>(s0);
			auto B0 = *reinterpret_cast<const uint32_t*>(s0 + delta);
			auto C0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4 - delta);
			auto D0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4);

			auto A1 = *reinterpret_cast<const uint32_t*>(s1);
			auto B1 = *reinterpret_cast<const uint32_t*>(s1 + delta);
			auto C1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4 - delta);
			auto D1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4);

			auto x = simd::sse::set_epi32(D0, C0, B0, A0);
			auto y = simd::sse::set_epi32(D1, C1, B1, A1);

			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == simd::sse::mask()) return s0 + n;
			auto i = wjr::countr_one(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s0 + q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (!pred(s0[0], s1[0])) return s0;
		if (n == 1) return s0 + 1;
		if (!pred(s0[1], s1[1])) return s0 + 1;
		if (n == 2) return s0 + 2;
		return pred(s0[2], s1[2]) ? s0 + 3 : s0 + 2;
	}
}

_WJR_ALGO_END

#undef __WJR_MEMMIS_FOUR
#undef __WJR_MEMMIS_ONE

#endif // __HAS_FAST_MEMMIS

#endif // __WJR_ALGO_MEMMIS_H                                                                                                                                                                                                                                                                                                                             
#pragma once
#ifndef __WJR_ALGO_MEMRMIS_H
#define __WJR_ALGO_MEMRMIS_H




#if defined(__HAS_FAST_MEMMIS)

#define __WJR_MEMRMIS_ONE(st, _s)														\
	auto r = st::cmp(x, y, pred, T());	                                                \
	st::mask_type z = st::movemask_epi8(r);		                                        \
	if(z != st::mask()){	                                                            \
		return (_s) - wjr::countl_one(z) / _Mysize;		                                \
	}

#define __WJR_MEMRMIS_FOUR(st, _s0, _s1, _s2, _s3)										\
	auto r0 = st::cmp(x0, y0, pred, T());	                                            \
	auto r1 = st::cmp(x1, y1, pred, T());	                                            \
	auto r2 = st::cmp(x2, y2, pred, T());	                                            \
	auto r3 = st::cmp(x3, y3, pred, T());	                                            \
		                                                                                \
	r0 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
	st::mask_type z = st::movemask_epi8(r0);		                                    \
	if(z != st::mask()){	                                                            \
		st::mask_type tmp = st::movemask_epi8(r3);	                                    \
		if(tmp != st::mask()){	                                                        \
			return (_s3) - wjr::countl_one(tmp) / _Mysize;	                            \
		}	                                                                            \
		tmp = st::movemask_epi8(r2);	                                                \
		if(tmp != st::mask()){	                                                        \
			return (_s2) - wjr::countl_one(tmp) / _Mysize;	                            \
		}	                                                                            \
		tmp = st::movemask_epi8(r1);	                                                \
		if(tmp != st::mask()){	                                                        \
			return (_s1) - wjr::countl_one(tmp) / _Mysize;	                            \
		}	                                                                            \
		tmp = z;	                                                                    \
		return (_s0) - wjr::countl_one(tmp) / _Mysize;	                                \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memrmis(const T* s0, const T* s1, size_t n, _Pred pred) {
	using namespace wjr::literals;
	constexpr size_t _Mysize = sizeof(T);

#if defined(__AVX2__)
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // __AVX2__
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	if (is_unlikely(n == 0)) return s0;

	s0 += n;
	s1 += n;

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			const T* lst0;
			const T* lst1;

			auto _off0 = reinterpret_cast<uintptr_t>(s0) % bound;
			auto _off1 = reinterpret_cast<uintptr_t>(s1) % bound;

			// align 2 pointer
			if (_off0 == _off1 &&
				(_Mysize == 1 || _off0 % _Mysize == 0)) {
				if (is_constant_p(_off0) && _off0 == 0) {
					// do nothing
				}
				else {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_ONE(simd_t, s0);

					auto __align_s = _off0;
					s0 -= __align_s / _Mysize;
					s1 -= __align_s / _Mysize;
					n -= __align_s / _Mysize;
					if (is_unlikely(n < width * 4)) {
						lst0 = s0 - n;
						lst1 = s1 - n;
						goto WJR_MACRO_LABEL(aft_align);
					}
				}
			}
			// unalign algorithm
			else {
				do {
					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 4));
					auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 3));
					auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

					auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 4));
					auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 3));
					auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 2));
					auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

					s0 -= width * 4;
					s1 -= width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				lst0 = s0 - n;
				lst1 = s1 - n;

				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

						__WJR_MEMRMIS_ONE(simd_t, s0);
						s0 -= width;
						s1 -= width;
					}
					case 3: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

						__WJR_MEMRMIS_ONE(simd_t, s0);
						s0 -= width;
						s1 -= width;
					}
					case 2: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

						__WJR_MEMRMIS_ONE(simd_t, s0);
					}
					case 1: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(lst0));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(lst1));

						__WJR_MEMRMIS_ONE(simd_t, lst0 + width);
					}
					}
				}
				return lst0;
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 4));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 3));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 - width));

				auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 4));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 3));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 2));
				auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

				__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

				s0 -= width * 4;
				s1 -= width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			lst0 = s0 - n;
			lst1 = s1 - n;

			if (n != 0) {
				switch ((n + width - 1) / width) {
				default: unreachable(); break;
				case 4: {
					WJR_MACRO_LABEL(aft_align) :
						auto x = simd_t::load(reinterpret_cast<const sint*>(s0 - width));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_ONE(simd_t, s0);
					s0 -= width;
					s1 -= width;
				}
				case 3: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s0 - width));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_ONE(simd_t, s0);
					s0 -= width;
					s1 -= width;
				}
				case 2: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s0 - width));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_ONE(simd_t, s0);
				}
				case 1: {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(lst0));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(lst1));

					__WJR_MEMRMIS_ONE(simd_t, lst0 + width);
				}
				}
			}

			return lst0;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - n));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - n + 32 / _Mysize));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - 64 / _Mysize));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - 32 / _Mysize));

			auto y0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - n));
			auto y1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - n + 32 / _Mysize));
			auto y2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - 64 / _Mysize));
			auto y3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - 32 / _Mysize));

			__WJR_MEMRMIS_FOUR(simd::avx, s0 - n + 32 / _Mysize, s0 - n + 64 / _Mysize, s0 - 32 / _Mysize, s0);

			return s0 - n;
		}
#endif // __AVX2__

		auto delta = (n & (32 / _Mysize)) >> 1;

		auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - n));
		auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - n + delta));
		auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - 16 / _Mysize - delta));
		auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - 16 / _Mysize));

		auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - n));
		auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - n + delta));
		auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - 16 / _Mysize - delta));
		auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - 16 / _Mysize));

		__WJR_MEMRMIS_FOUR(simd::sse, s0 - n + 16 / _Mysize, s0 - n + delta + 16 / _Mysize, s0 - delta, s0);

		return s0 - n;
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s0[-1], s1[-1]) ? s0 : s0 - 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A0 = *reinterpret_cast<const uint64_t*>(s0 - n);
			auto B0 = *reinterpret_cast<const uint64_t*>(s0 - 4);

			auto A1 = *reinterpret_cast<const uint64_t*>(s1 - n);
			auto B1 = *reinterpret_cast<const uint64_t*>(s1 - 4);

			auto x = simd::sse::set_epi64x(B0, A0);
			auto y = simd::sse::set_epi64x(B1, A1);
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == simd::sse::mask())return s0 - n;
			auto i = wjr::countl_one(z) / _Mysize;
			auto q = i >= 4 ? i + n - 4 : i;
			return s0 - q;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A0 = *reinterpret_cast<const uint32_t*>(s0 - n);
			auto B0 = *reinterpret_cast<const uint32_t*>(s0 - n + delta);
			auto C0 = *reinterpret_cast<const uint32_t*>(s0 - 4 - delta);
			auto D0 = *reinterpret_cast<const uint32_t*>(s0 - 4);

			auto A1 = *reinterpret_cast<const uint32_t*>(s1 - n);
			auto B1 = *reinterpret_cast<const uint32_t*>(s1 - n + delta);
			auto C1 = *reinterpret_cast<const uint32_t*>(s1 - 4 - delta);
			auto D1 = *reinterpret_cast<const uint32_t*>(s1 - 4);

			auto x = simd::sse::set_epi32(D0, C0, B0, A0);
			auto y = simd::sse::set_epi32(D1, C1, B1, A1);

			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == simd::sse::mask()) return s0 - n;
			auto i = wjr::countl_one(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s0 - q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (!pred(s0[-1], s1[-1])) return s0;
		if (n == 1) return s0 - 1;
		if (!pred(s0[-2], s1[-2])) return s0 - 1;
		if (n == 2) return s0 - 2;
		return pred(s0[-3], s1[-3]) ? s0 - 3 : s0 - 2;
	}
}

_WJR_ALGO_END

#undef __WJR_MEMRMIS_FOUR
#undef __WJR_MEMRMIS_ONE

#endif // __HAS_FAST_MEMMIS

#endif // __WJR_ALGO_MEMRMIS_H                                                                                                                                                                                                                                                                                                                                
#pragma once
#ifndef __WJR_ALGO_MEMCNT_H
#define __WJR_ALGO_MEMCNT_H




#if defined(__HAS_FAST_MEMCNT)

#define __WJR_MEMCNT_SMALL_INIT_BEGIN(...)	size_t cnt = 0;
#define __WJR_MEMCNT_SMALL_INIT_END(...) return cnt;

#define __WJR_MEMCNT_SMALL_FUNC(s, val, i, n)	                                \
	{	                                                                        \
		if(n <= 4){	                                                            \
			for(size_t j = 0;j < n;++j){	                                    \
				cnt += (s[i + j] == val);	                                    \
			}	                                                                \
		}else{	                                                                \
			auto q = traits::set1(val, T());	                                \
			auto ns = n * sizeof(T);	                                        \
			auto ql = ns >> 1;	                                                \
			auto sl = ql << 1;	                                                \
			auto x = traits::preloadu_si16x(s + i, ql);	                        \
			auto r = traits::cmpeq(x, q, T());	                                \
			auto z = traits::movemask_epi8(r) & ((1u << sl) - 1);	            \
			cnt += wjr::popcount(static_cast<value_type>(z)) / _Mycor;	        \
			if constexpr(sizeof(T) == 1){	                                    \
				if(ns & 1){	                                                    \
					cnt += (s[i + n - 1] == val);	                            \
				}	                                                            \
			}	                                                                \
		}	                                                                    \
	}

#define __WJR_MEMCNT_MID_INIT_BEGIN(...)	                                    \
	auto c = traits::zeros();	                                                \
	auto step = _Maxstep;	                                                    \
	auto q = traits::set1(val, T());

#define __WJR_MEMCNT_MID_INIT_END(s, val)	cnt += traits::add(c, T());

#define __WJR_MEMCNT_MID_FUNC(s, val, i)	                                    \
	{	                                                                        \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>(s + i));	    \
		auto r = traits::cmpeq(x, q, T{});	                                    \
		c = traits::sub(c, r, T{});				                                \
	}

#define __WJR_MEMCNT_LARGE_FUNC(s, val, i)		                                \
	{	                                                                        \
		__WJR_MEMCNT_MID_FUNC(s, val, i)	                                    \
		__WJR_MEMCNT_MID_FUNC(s, val, (i + _Mysize * 1))	                    \
		__WJR_MEMCNT_MID_FUNC(s, val, (i + _Mysize * 2))	                    \
		__WJR_MEMCNT_MID_FUNC(s, val, (i + _Mysize * 3))	                    \
		--step;																	\
		if (!step) {	                                                        \
			cnt += traits::add(c, T{});	                                        \
			c = traits::zeros();	                                            \
			step = _Maxstep;	                                                \
		}	                                                                    \
	}

	_WJR_ALGO_BEGIN

	template<typename T>
size_t __memcnt(const T* s, T val, size_t n) {
	constexpr bool is_avx =
#if defined(__AVX2__)
		true;
#else
		false;
#endif
	using traits = std::conditional_t<is_avx, simd::avx, simd::sse>;
	using simd_type = typename traits::int_type;
	using value_type = typename traits::mask_type;
	constexpr int _Mysize = traits::width() / (8 * sizeof(T));
	constexpr int _Mycor = sizeof(T) / sizeof(uint8_t);
	// Prevent overflow
	constexpr auto _Maxstep_four = std::numeric_limits<T>::max() / _Mysize + 1;
	static_assert(_Maxstep_four >= 4 && _Maxstep_four % 4 == 0, "invalid _Maxstep");
	constexpr auto _Maxstep = _Maxstep_four / 4;

	using namespace enum_ops;
	constexpr auto tag =
		GetSmallThreshold(_Mysize) |
		GetVectorizedLength(_Mysize);

	WJR_AUTO_VECTORIZE(tag, n,
		__WJR_MEMCNT_SMALL_FUNC, __WJR_MEMCNT_MID_FUNC, __WJR_MEMCNT_LARGE_FUNC,
		__WJR_MEMCNT_SMALL_INIT_BEGIN, __WJR_MEMCNT_SMALL_INIT_END,
		__WJR_MEMCNT_MID_INIT_BEGIN, __WJR_MEMCNT_MID_INIT_END,
		s, val);
}

_WJR_ALGO_END

#endif // __HAS_FAST_MEMCNT

#endif // __WJR_ALGO_MEMCNT_H                                                                                                   
#pragma once
#ifndef __WJR_ALGO_MEMSET_H
#define __WJR_ALGO_MEMSET_H




#if defined(__HAS_FAST_MEMSET)

_WJR_ALGO_BEGIN

template<typename T>
void __memset(T* s, T val, size_t n) {
	using namespace wjr::literals;
	constexpr size_t _Mysize = sizeof(T);

#if defined(__AVX2__)
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // __AVX2__
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	if constexpr (_Mysize != 1) {
		if (
			is_constant_p(val) &&
			broadcast<T, uint8_t>(static_cast<uint8_t>(val)) == val) {
			return __memset(reinterpret_cast<uint8_t*>(s), static_cast<uint8_t>(val), n * _Mysize);
		}
	}

	constexpr size_t __nt_threshold = 6_MiB / _Mysize;
	constexpr size_t __rep_threshold = 2_KiB / _Mysize;

	if (is_constant_p(n) && n <= 4_KiB / _Mysize) {
		for (size_t i = 0; i < n; ++i) {
			s[i] = val;
		}
		return;
	}

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			auto q = simd_t::set1(val, T());

			// non-temporal algorithm
			if (_Mysize == 1 ||
				(is_likely(reinterpret_cast<uintptr_t>(s) % _Mysize == 0))) {

				if (is_unlikely(n >= __nt_threshold)) {

					// align(64)
					if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 64) &&
						reinterpret_cast<uintptr_t>(s) % 64 == 0) {
						// do nothing
					}
					else {
#if defined(__AVX2__)
						simd_t::storeu(reinterpret_cast<sint*>(s), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width), q);
#else // SSE2
						simd_t::storeu(reinterpret_cast<sint*>(s), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width * 2), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width * 3), q);
#endif // __AVX2__
						auto __align_s = 64 - reinterpret_cast<uintptr_t>(s) % 64;
						s += __align_s / _Mysize;
						n -= __align_s / _Mysize;
					}

					do {
						simd_t::stream(reinterpret_cast<sint*>(s), q);
						simd_t::stream(reinterpret_cast<sint*>(s + width), q);
						simd_t::stream(reinterpret_cast<sint*>(s + width * 2), q);
						simd_t::stream(reinterpret_cast<sint*>(s + width * 3), q);
						s += width * 4;
						n -= width * 4;
					} while (n >= width * 4);
					simd::sse::sfence();
					goto WJR_MACRO_LABEL(aft_align);
				}
			}

			// align algorithm
			if (is_constant_p(reinterpret_cast<uintptr_t>(s) % bound) &&
				reinterpret_cast<uintptr_t>(s) % bound == 0) {
				// do nothing
			}
			else if (_Mysize == 1 ||
				is_likely(reinterpret_cast<uintptr_t>(s) % _Mysize == 0)) {
				simd_t::storeu(reinterpret_cast<sint*>(s), q);
				auto __align_s = bound - reinterpret_cast<uintptr_t>(s) % bound;
				s += __align_s / _Mysize;
				n -= __align_s / _Mysize;
				if (is_unlikely(n < width * 4)) {
					// n = [width * 4 - width / _Mysize, width * 4)
					goto WJR_MACRO_LABEL(aft_align);
				}
			}
			else {
				// unalign algorithm
				do {
					simd_t::storeu(reinterpret_cast<sint*>(s), q);
					simd_t::storeu(reinterpret_cast<sint*>(s + width), q);
					simd_t::storeu(reinterpret_cast<sint*>(s + width * 2), q);
					simd_t::storeu(reinterpret_cast<sint*>(s + width * 3), q);
					s += width * 4;
					n -= width * 4;
				} while (n >= width * 4);
				s += n;

				simd_t::storeu(reinterpret_cast<sint*>(s - width * 4), q);
				simd_t::storeu(reinterpret_cast<sint*>(s - width * 3), q);
				simd_t::storeu(reinterpret_cast<sint*>(s - width * 2), q);
				simd_t::storeu(reinterpret_cast<sint*>(s - width), q);
				return;
			}

			do {
				simd_t::store(reinterpret_cast<sint*>(s), q);
				simd_t::store(reinterpret_cast<sint*>(s + width), q);
				simd_t::store(reinterpret_cast<sint*>(s + width * 2), q);
				simd_t::store(reinterpret_cast<sint*>(s + width * 3), q);
				s += width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			WJR_MACRO_LABEL(aft_align) :

				auto t = s + n - width;
			s = reinterpret_cast<T*>(
				(reinterpret_cast<uintptr_t>(s + n - width * 3)) & (~(bound - 1)));
			simd_t::store(reinterpret_cast<sint*>(s), q);
			simd_t::store(reinterpret_cast<sint*>(s + width), q);
			simd_t::store(reinterpret_cast<sint*>(s + width * 2), q);
			simd_t::storeu(reinterpret_cast<sint*>(t), q);

			return;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto q = simd::avx::set1(val, T());
			simd::avx::storeu(reinterpret_cast<__m256i*>(s), q);
			simd::avx::storeu(reinterpret_cast<__m256i*>(s + 32 / _Mysize), q);
			simd::avx::storeu(reinterpret_cast<__m256i*>(s + n - 64 / _Mysize), q);
			simd::avx::storeu(reinterpret_cast<__m256i*>(s + n - 32 / _Mysize), q);
			return;
		}
#endif // __AVX2__
		auto q = simd::sse::set1(val, T());
		auto delta = (n & (32 / _Mysize)) >> 1;
		simd::sse::storeu(reinterpret_cast<__m128i*>(s), q);
		simd::sse::storeu(reinterpret_cast<__m128i*>(s + delta), q);
		simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize) - delta), q);
		simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize)), q);
		return;
	}

	if constexpr (_Mysize == 8) {
		if (n != 0) {
			*s = val;
		}
		return;
	}

	if constexpr (_Mysize <= 4) {
		if (n >= 4 / _Mysize) {
			auto u32v = broadcast<uint32_t, T>(val);
			auto delta = (n & (8 / _Mysize)) >> 1;
			*reinterpret_cast<uint32_t*>(s) = u32v;
			*reinterpret_cast<uint32_t*>(s + delta) = u32v;
			if constexpr (_Mysize != 4) {
				*reinterpret_cast<uint32_t*>(s + n - (4 / _Mysize) - delta) = u32v;
			}
			*reinterpret_cast<uint32_t*>(s + n - (4 / _Mysize)) = u32v;
			return;
		}
	}

	if constexpr (_Mysize == 4) {
		return;
	}

	if constexpr (_Mysize == 2) {
		if (n != 0) *s = val;
		return;
	}

	if constexpr (_Mysize == 1) {
		if (n != 0) {
			*s = val;
			if (n != 1) {
				*reinterpret_cast<uint16_t*>(s + n - 2) = broadcast<uint16_t, uint8_t>(val);
			}
		}
		return;
	}
}

_WJR_ALGO_END

#endif // __HAS_FAST_MEMSET

#endif // __WJR_ALGO_MEMSET_H                                                                                                                                                                                                                

_WJR_ALGO_BEGIN

#if defined(__HAS_FAST_MEMCHR)
template<typename T, typename U, typename _Pred>
struct __has_fast_memchr : std::conjunction<
	is_any_of<_Pred, std::equal_to<>, std::not_equal_to<>>,
	is_comparable<T, U, _Pred>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memchr_v = __has_fast_memchr<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_fast_memrchr : __has_fast_memchr<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrchr_v = __has_fast_memrchr<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memchr_v<T, U, _Pred>, int> = 0>
const T* memchr(const T* s, U val, size_t n, _Pred pred) {
	auto p = is_possible_memory_comparable<T>(val, pred);
	if (p == ipmc_result::none) {
		return s + n;
	}
	if (p == ipmc_result::all) {
		return s;
	}
	using value_type = std::make_unsigned_t<make_integral_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
	auto __val = static_cast<value_type>(static_cast<T>(val));
	return reinterpret_cast<const T*>(__memchr(__s, __val, n, pred));
}
template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memchr_v<T, U, _Pred>, int> = 0>
const T* memrchr(const T* s, U val, size_t n, _Pred pred) {
	auto p = is_possible_memory_comparable<T>(val, pred);
	if (p == ipmc_result::none) {
		return s;
	}
	if (p == ipmc_result::all) {
		return s + n;
	}
	using value_type = std::make_unsigned_t<make_integral_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
	auto __val = static_cast<value_type>(val);
	return reinterpret_cast<const T*>(__memrchr(__s, __val, n, pred));
}
#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memchr : std::conjunction<
	std::is_same<_Pred, std::equal_to<>>,
	is_comparable<T, U, _Pred>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), sizeof(char), sizeof(wchar_t)>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memchr_v = __has_fast_memchr<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_fast_memrchr : std::false_type {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrchr_v = __has_fast_memrchr<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memchr_v<T, U, _Pred>, int> = 0>
const T* memchr(const T* s, U val, size_t n, _Pred pred) {
	auto p = is_possible_memory_comparable<T>(val, pred);
	if (p == ipmc_result::none) {
		return s + n;
	}
	if (p == ipmc_result::all) {
		return s;
	}
	if constexpr (sizeof(T) == sizeof(char)) {
		using value_type = char;
		auto __s = reinterpret_cast<const value_type*>(s);
		auto __val = static_cast<value_type>(val);
		auto pos = ::memchr(__s, __val, n);
		if (pos == nullptr) {
			return s + n;
		}
		return reinterpret_cast<const T*>(pos);
	}
	else {
		using value_type = wchar_t;
		auto __s = reinterpret_cast<const value_type*>(s);
		auto __val = static_cast<value_type>(val);
		auto pos = ::wmemchr(__s, __val, n);
		if (pos == nullptr) {
			return s + n;
		}
		return reinterpret_cast<const T*>(pos);
	}
}

#endif // __HAS_FAST_MEMCHR

#if defined(__HAS_FAST_MEMCMP)
template<typename T, typename U, typename _Pred>
struct __has_fast_memcmp : std::conjunction<
	is_memory_comparable<T, U, _Pred>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::not_equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::not_equal_to<>>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memcmp_v = __has_fast_memcmp<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memcmp_v<T, U, _Pred>, int> = 0>
bool memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (is_any_of_v<_Pred, std::equal_to<>, std::not_equal_to<>>) {
		using value_type = uint8_t;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return __memcmp(__s0, __s1, n * sizeof(T), pred);
	}
	else {
		using value_type = make_integral_t<T>;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return __memcmp(__s0, __s1, n, pred);
	}
}

#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memcmp : std::false_type {};

template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U, typename _Pred>
inline constexpr bool __has_fast_memcmp_v = __has_fast_memcmp<T, U, _Pred>::type::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memcmp_v<T, U, _Pred>, int> = 0>
bool memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
	return ::memcmp(s0, s1, n * sizeof(T)) == 0;
}

#endif // __HAS_FAST_MEMCMP

#if defined(__HAS_FAST_MEMMIS)
template<typename T, typename U, typename _Pred>
struct __has_fast_memmis : std::conjunction<
	is_memory_comparable<T, U, _Pred>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U>
struct __has_fast_memmis<T, U, std::equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U>
struct __has_fast_memmis<T, U, std::not_equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::not_equal_to<>>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memmis_v = __has_fast_memmis<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_fast_memrmis : __has_fast_memmis<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrmis_v = __has_fast_memrmis<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<
	__has_fast_memmis_v<T, U, _Pred>, int> = 0>
const T* memmis(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (is_any_of_v<_Pred, std::equal_to<>, std::not_equal_to<>>) {
		using value_type = uint8_t;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		auto __pos =
			__memmis(__s0, __s1, n * sizeof(T), pred);
		auto __dif = (__pos - __s0) / sizeof(T);
		return s0 + __dif;
	}
	else {
		using value_type = make_integral_t<T>;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return reinterpret_cast<const T*>(__memmis(__s0, __s1, n, pred));
	}
}

template<typename T, typename U, typename _Pred, std::enable_if_t<
	__has_fast_memmis_v<T, U, _Pred>, int> = 0>
const T* memrmis(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (is_any_of_v<_Pred, std::equal_to<>, std::not_equal_to<>>) {
		using value_type = uint8_t;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		auto __pos =
			__memrmis(__s0, __s1, n * sizeof(T), pred);
		auto __e0 = __s0 + n * sizeof(T);
		auto __dif = (__e0 - __pos) / sizeof(T);
		return (s0 + n) - __dif;
	}
	else {
		using value_type = make_integral_t<T>;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return reinterpret_cast<const T*>(__memrmis(__s0, __s1, n, pred));
	}
}

#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memmis : std::false_type {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memmis_v = __has_fast_memmis<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_fast_memrmis : __has_fast_memmis<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrmis_v = __has_fast_memrmis<T, U, _Pred>::value;

#endif // __HAS_FAST_MEMMIS

#if defined(__HAS_FAST_MEMCNT)
template<typename T, typename U>
struct __has_fast_memcnt : std::conjunction<
	is_comparable<T, U, std::equal_to<>>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U>
constexpr bool __has_fast_memcnt_v = __has_fast_memcnt<T, U>::value;

template<typename T, typename U, std::enable_if_t<__has_fast_memcnt_v<T, U>, int> = 0>
size_t memcnt(const T* s, U val, size_t n) {
	auto p = is_possible_memory_comparable<T>(val, std::equal_to<>{});
	if (p == ipmc_result::none) {
		return 0;
	}
	using value_type = std::make_unsigned_t<make_integral_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
	auto __val = static_cast<value_type>(val);
	return __memcnt(__s, __val, n);
}

#else
template<typename T, typename U>
struct __has_fast_memcnt : std::false_type {};

template<typename T, typename U>
constexpr bool __has_fast_memcnt_v = __has_fast_memcnt<T, U>::value;

#endif // __HAS_FAST_MEMCNT

template<template<typename X, typename Y> typename TEST, typename T, typename U>
struct __has_fast_memset_helper : std::conjunction<
	std::bool_constant<TEST<T, U>::is_fill>
> {};

template<template<typename X, typename Y> typename TEST, typename T, typename U>
constexpr bool __has_fast_memset_helper_v = __has_fast_memset_helper<TEST, T, U>::value;

template<template<typename X, typename Y> typename TEST, typename T, typename U>
static void __memset_helper(T* s, const U& val, size_t n) {
	auto __byte_array = TEST<T, const U&>::get(val);
#if defined(__HAS_FAST_MEMSET)
	constexpr auto _max_bytes_num = _Get_max_bytes_num<sizeof(T)>();
	auto _bytes_num = _Get_bytes_num(__byte_array);

	if (_bytes_num == 0) {
		std::fill_n(s, n, val);
		return;
	}

	using value_type = uint_t<_max_bytes_num * 8>;
	auto __s = reinterpret_cast<value_type*>(s);
	auto __val = to_uint<_max_bytes_num>(__byte_array);
	static_assert(std::is_same_v<decltype(__val), value_type>, "type mismatch");
	__memset(__s, __val, n * (sizeof(T) / _max_bytes_num));
#else
	constexpr auto _max_bytes_num = 1;
	auto _bytes_num = _Get_bytes_num(__byte_array);

	if (_bytes_num != 1) {
		std::fill_n(s, n, val);
		return;
	}

	using value_type = uint_t<_max_bytes_num * 8>;
	auto __s = reinterpret_cast<value_type*>(s);
	auto __val = to_uint<_max_bytes_num>(__byte_array);
	static_assert(std::is_same_v<decltype(__val), value_type>, "type mismatch");
	::memset(__s, __val, n * (sizeof(T) / _max_bytes_num));
#endif
}

template<typename T, typename U>
struct __has_fast_construct_memset : __has_fast_memset_helper<is_byte_constructible, T, U> {};

template<typename T, typename U>
constexpr bool __has_fast_construct_memset_v = __has_fast_construct_memset<T, U>::value;

template<typename T, typename U>
void construct_memset(T* s, const U& val, size_t n) {
	__memset_helper<is_byte_constructible>(s, val, n);
}

template<typename T, typename U>
struct __has_fast_assign_memset : __has_fast_memset_helper<is_byte_assignable, T, U> {};

template<typename T, typename U>
constexpr bool __has_fast_assign_memset_v = __has_fast_assign_memset<T, U>::value;

template<typename T, typename U>
void assign_memset(T* s, const U& val, size_t n) {
	__memset_helper<is_byte_assignable>(s, val, n);
}

template<template<typename X, typename Y> typename TEST, typename T, typename U>
struct __has_fast_memcpy_helper : std::conjunction <
	std::bool_constant<TEST<T, U>::is_copy>
> {};

template<template<typename X, typename Y> typename TEST, typename T, typename U>
constexpr bool __has_fast_memcpy_helper_v = __has_fast_memcpy_helper<TEST, T, U>::value;

template<template<typename X, typename Y> typename TEST, typename T, typename U>
static void __memcpy_helper(T* s, const U* t, size_t n) {
	static_assert(sizeof(T) == sizeof(U), "type mismatch");
#if defined(__HAS_FAST_MEMCPY)
#else
	::memcpy(s, t, n * sizeof(T));
#endif
}

template<template<typename X, typename Y> typename TEST, typename T, typename U>
static void __memmove_helper(T* s, const U* t, size_t n) {
	static_assert(sizeof(T) == sizeof(U), "type mismatch");
#if defined(__HAS_FAST_MEMCPY)
#else
	::memmove(s, t, n * sizeof(T));
#endif
}

template<typename T, typename U>
struct __has_fast_construct_memcpy : __has_fast_memcpy_helper<is_byte_constructible, T, U> {};

template<typename T, typename U>
constexpr bool __has_fast_construct_memcpy_v = __has_fast_construct_memcpy<T, U>::value;

template<typename T, typename U>
void construct_memcpy(T* s, const U* t, size_t n) {
	__memcpy_helper<is_byte_constructible>(s, t, n);
}

template<typename T, typename U>
void construct_memmove(T* s, const U* t, size_t n) {
	__memmove_helper<is_byte_constructible>(s, t, n);
}

template<typename T, typename U>
struct __has_fast_assign_memcpy : __has_fast_memcpy_helper<is_byte_assignable, T, U> {};

template<typename T, typename U>
constexpr bool __has_fast_assign_memcpy_v = __has_fast_assign_memcpy<T, U>::value;

template<typename T, typename U>
void assign_memcpy(T* s, const U* t, size_t n) {
	__memcpy_helper<is_byte_assignable>(s, t, n);
}

template<typename T, typename U>
void assign_memmove(T* s, const U* t, size_t n) {
	__memmove_helper<is_byte_assignable>(s, t, n);
}

_WJR_ALGO_END

#endif // __WJR_ALGO_ALOG_H                                                                                                                                                                                                                                                                                                                                                                                                                        

#if defined(WJR_HAS_EXECUTION)
#include <execution>
#endif // WJR_HAS_EXECUTION

_WJR_BEGIN

template<typename _Iter, typename _Val, typename _Pred,
	typename _Iter_value = iter_val_t<_Iter>>
	struct __has_fast_find : std::conjunction<
	is_contiguous_iterator<_Iter>,
	std::conditional_t<
	wjr::is_reverse_iterator_v<_Iter>,
	algo::__has_fast_memrchr<_Iter_value, _Val, _Pred>,
	algo::__has_fast_memchr<_Iter_value, _Val, _Pred>
	>
	> {};

template<typename _Iter, typename _Val, typename _Pred>
constexpr bool __has_fast_find_v = __has_fast_find<_Iter, _Val, _Pred>::value;

struct _Find_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename T, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, const T& value) const {
		return std::find(std::forward<ExecutionPolicy>(policy), first, last, value);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Ty, typename _Pred = std::equal_to<>>
	WJR_CONSTEXPR20 _Iter operator()(
		_Iter _First, _Iter _Last, const _Ty& _Val, _Pred pred = _Pred()) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_find_v<_Iter, _Ty, _Pred>) {
				const auto n = std::distance(_First, _Last);
				if constexpr (!wjr::is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::to_address(_First);
					return _First + (algo::memchr(first, _Val, n, pred) - first);
				}
#if defined(__HAS_FAST_MEMCHR) // use algo::memchr
				else {
					const auto first = wjr::to_address(_Last - 1);
					return _Last - (algo::memrchr(first, _Val, n, pred) - first);
				}
#endif // __HAS_FAST_MEMCHR
			}
		}
		if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
			return std::find(_First, _Last, _Val);
		}
		else {
			for (; _First != _Last; ++_First) {
				if (pred(*_First, _Val)) {
					return _First;
				}
			}
			return _Last;
		}
	}

};

constexpr _Find_fn find{};

struct _Find_if_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryPredicate p) const {
		return std::find_if(std::forward<ExecutionPolicy>(policy), first, last, p);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::find_if(_First, _Last, _Pred);
	}
};

constexpr _Find_if_fn find_if{};

struct _Find_if_not_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryPredicate p) const {
		return std::find_if_not(std::forward<ExecutionPolicy>(policy), first, last, p);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::find_if_not(_First, _Last, _Pred);
	}
};

constexpr _Find_if_not_fn find_if_not{};

template<typename _Iter, typename _Val,
	typename _Iter_value = iter_val_t<_Iter>>
	struct __has_fast_count : std::conjunction<
	wjr::is_contiguous_iterator<_Iter>,
	algo::__has_fast_memcnt<_Iter_value, _Val>
	> {};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_count_v = __has_fast_count<_Iter, _Val>::value;

struct _Count_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename T, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	typename std::iterator_traits<ForwardIt>::difference_type
		operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, const T& value) const {
		return std::count(std::forward<ExecutionPolicy>(policy), first, last, value);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Ty>
	WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
		operator()(_Iter _First, _Iter _Last, const _Ty& _Val) const {
		if (!wjr::is_constant_evaluated()) {
#if defined(__HAS_FAST_MEMCNT)
			if constexpr (__has_fast_count_v<_Iter, _Ty>) {
				const auto n = _Last - _First;
				if constexpr (!wjr::is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::to_address(_First);
					return algo::memcnt(first, _Val, n);
				}
				else {
					const auto first = wjr::to_address(_Last - 1);
					return algo::memcnt(first, _Val, n);
				}
			}
#endif // __HAS_FAST_MEMCNT
		}
		return std::count(_First, _Last, _Val);
	}
};

constexpr _Count_fn count{};

struct _Count_if_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	typename std::iterator_traits<ForwardIt>::difference_type
		operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryPredicate p) const {
		return std::count_if(std::forward<ExecutionPolicy>(policy), first, last, p);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
		operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::count_if(_First, _Last, _Pred);
	}
};

constexpr _Count_if_fn count_if{};

// First use algo::memcmp
// Then use memcmp
template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = iter_val_t<_Iter1>,
	typename _Iter_value2 = iter_val_t<_Iter2>>
	struct __has_fast_equal_helper : std::conjunction<
	wjr::is_contiguous_iterator<_Iter1>,
	wjr::is_contiguous_iterator<_Iter2>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
	algo::__has_fast_memcmp<_Iter_value1, _Iter_value2, _Pred>
	> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
struct __has_fast_equal : std::bool_constant<__has_fast_equal_helper<_Iter1, _Iter2, _Pred>::value> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_equal_v = __has_fast_equal<_Iter1, _Iter2, _Pred>::value;

template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = iter_val_t<_Iter1>,
	typename _Iter_value2 = iter_val_t<_Iter2>>
	struct __has_fast_mismatch : std::conjunction <
	wjr::is_contiguous_iterator<_Iter1>,
	wjr::is_contiguous_iterator<_Iter2>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
	std::conditional_t<
	wjr::is_reverse_iterator_v<_Iter1>,
	algo::__has_fast_memrmis<_Iter_value1, _Iter_value2, _Pred>,
	algo::__has_fast_memmis<_Iter_value1, _Iter_value2, _Pred>
	>
	> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_mismatch_v = __has_fast_mismatch<_Iter1, _Iter2, _Pred>::value;

struct _Mismatch_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	std::pair<_Iter1, _Iter2> operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return std::mismatch(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	std::pair<_Iter1, _Iter2> operator()(_ExPolicy&& _Policy,
		_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return std::mismatch(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, _Last2);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	std::pair<_Iter1, _Iter2> operator()(_ExPolicy&& _Policy,
		_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		return std::mismatch(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, pred);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	std::pair<_Iter1, _Iter2> operator()(_ExPolicy&& _Policy,
		_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		return std::mismatch(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, _Last2, pred);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return this->operator()(_First1, _Last1, _First2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		if (!wjr::is_constant_evaluated()) {
#if defined(__HAS_FAST_MEMMIS)
			if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
				const auto n = std::distance(_First1, _Last1);
				if (is_unlikely(n == 0)) { return std::make_pair(_First1, _First2); }
				if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
					const auto first1 = wjr::to_address(_First1);
					const auto first2 = wjr::to_address(_First2);

					auto pos = algo::memmis(first1, first2, n, pred) - first1;
					return std::make_pair(_First1 + pos, _First2 + pos);
				}
				else {
					const auto first1 = wjr::to_address(_Last1 - 1);
					const auto _Last2 = _First2 + n;
					const auto first2 = wjr::to_address(_Last2 - 1);

					auto pos = (first1 + n) - algo::memrmis(first1, first2, n, pred);
					return std::make_pair(_First1 + pos, _First2 + pos);
				}
			}
#endif // __HAS_FAST_MEMMIS
		}
		return std::mismatch(_First1, _Last1, _First2, pred);
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
				const auto n = _Last1 - _First1;
				const auto m = _Last2 - _First2;
				const auto _M = n < m ? n : m;
				return this->operator()(_First1, _First1 + _M, _First2, pred);
			}
		}
		return std::mismatch(_First1, _Last1, _First2, _Last2, pred);
	}

};

constexpr _Mismatch_fn mismatch;

struct _Equal_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, _Last2);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, pred);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, _Last2, pred);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return this->operator()(_First1, _Last1, _First2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
				const auto n = _Last1 - _First1;
				if (is_unlikely(n == 0)) { return true; }
				if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
					const auto first1 = wjr::to_address(_First1);
					const auto first2 = wjr::to_address(_First2);
					return algo::memcmp(first1, first2, n, pred);
				}
				else {
					const auto first1 = wjr::to_address(_Last1 - 1);
					const auto _Last2 = _First2 + n;
					const auto first2 = wjr::to_address(_Last2 - 1);
					return algo::memcmp(first1, first2, n, pred);
				}
			}
		}
		return std::equal(_First1, _Last1, _First2, pred);
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
				const auto n = std::distance(_First1, _Last1);
				const auto m = std::distance(_First2, _Last2);
				if (n != m) { return false; }
				return this->operator()(_First1, _Last1, _First2, pred);
			}
		}
		return std::equal(_First1, _Last1, _First2, _Last2, pred);
	}

};

constexpr _Equal_fn equal{};

template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = iter_val_t<_Iter1>,
	typename _Iter_value2 = iter_val_t<_Iter2>>
	struct __has_fast_lexicographical_compare : std::conjunction<
	wjr::is_any_of<_Pred, std::less<>, std::not_equal_to<>>,
	__has_fast_mismatch<_Iter1, _Iter2, _Pred>
	> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_lexicographical_compare_v = __has_fast_lexicographical_compare<_Iter1, _Iter2, _Pred>::value;

struct _Lexicographical_compare_fn {

	template<typename _ExPolicy, typename _Iter1, typename _Iter2>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_Policy, _First1, _Last1, _First2, _Last2, std::less<>{});
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		return std::lexicographical_compare(_Policy, _First1, _Last1, _First2, _Last2, pred);
	}

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_First1, _Last1, _First2, _Last2, std::less<>{});
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_lexicographical_compare_v<_Iter1, _Iter2, _Pred>) {
				const auto n = std::distance(_First1, _Last1);
				const auto m = std::distance(_First2, _Last2);
				auto cl = std::min(n, m);
				auto e = _First1 + cl;
				auto pos = wjr::mismatch(_First1, e, _First2, std::equal_to<>{}).first - _First1;
				if (pos != cl) {
					if (pred(_First1 + pos, _First2 + pos)) {
						return true;
					}
					return false;
				}
				return n < m;
			}
		}
		return std::lexicographical_compare(_First1, _Last1, _First2, _Last2, pred);
	}
};

constexpr _Lexicographical_compare_fn lexicographical_compare{};

template<typename _Iter, typename _Val,
	typename _Iter_ref = iter_ref_t<_Iter>>
	struct __has_fast_fill : std::conjunction<
	is_contiguous_iterator<_Iter>,
	algo::__has_fast_assign_memset<remove_cref_t<_Iter_ref>, add_cref_t<_Val>>
	> {};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_fill_v = __has_fast_fill<_Iter, _Val>::value;

struct _Fill_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename _Iter, typename _Val>
	void operator()(ExecutionPolicy&& policy,
		_Iter first, _Iter last, const _Val& value) const {
		return std::fill(policy, first, last, value);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Val>
	WJR_CONSTEXPR20 void operator()(_Iter _First, _Iter _Last, const _Val& value) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_fill_v<_Iter, _Val>) {
				const auto n = std::distance(_First, _Last);
				if constexpr (!is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::to_address(_First);
					algo::assign_memset(first, value, n);
				}
				else {
					if (is_unlikely(n == 0)) { return; }
					const auto first = wjr::to_address(_Last - 1);
					algo::assign_memset(first, value, n);
				}
				return;
			}
		}
		return std::fill(_First, _Last, value);
	}

};

constexpr _Fill_fn fill{};

struct _Fill_n_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename _Iter, typename _Size, typename _Val>
	_Iter operator()(ExecutionPolicy&& policy,
		_Iter first, _Size count, const _Val& value) const {
		return std::fill_n(policy, first, count, value);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Size, typename _Val>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Size count, const _Val& value) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_fill_v<_Iter, _Val>) {
				if (count <= 0) { return _First; }
				wjr::fill(_First, _First + count, value);
				return _First + count;
			}
		}
		return std::fill_n(_First, count, value);
	}

};

constexpr _Fill_n_fn fill_n{};

template<typename _Input, typename _Output,
	typename _Input_ref = iter_ref_t<_Input>,
	typename _Output_ref = iter_ref_t<_Output>>
	struct __has_fast_copy : std::conjunction<
	is_contiguous_iterator<_Input>,
	is_contiguous_iterator<_Output>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Input> == wjr::is_reverse_iterator_v<_Output>>,
	algo::__has_fast_assign_memcpy<remove_ref_t<_Output_ref>, _Input_ref>
	> {};

template<typename _Input, typename _Output>
constexpr bool __has_fast_copy_v = __has_fast_copy<_Input, _Output>::value;

struct _Copy_fn {
#if defined(WJR_HAS_EXECUTION)
	template<typename _Exec, typename _Iter1, typename _Iter2>
	_Iter2 operator()(_Exec&& exec, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return std::copy(exec, _First1, _Last1, _First2);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Input, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Input _Last1, _Output _First2) const {
		static_assert(!std::is_const_v<iter_ref_t<_Output>>, "Output iterator must not be const");
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_copy_v<_Input, _Output>) {
				const auto n = std::distance(_First1, _Last1);
				if (is_unlikely(n == 0)) { return _First2; }
				if constexpr (!wjr::is_reverse_iterator_v<_Input>) {
					const auto first1 = wjr::to_address(_First1);
					const auto first2 = wjr::to_address(_First2);

					algo::assign_memcpy(first2, first1, n);
				}
				else {
					const auto first1 = wjr::to_address(_Last1 - 1);
					const auto _Last2 = _First2 + n;
					const auto first2 = wjr::to_address(_Last2 - 1);

					algo::assign_memmove(first2, first1, n);
				}
				return _First2 + n;
			}
		}
		return std::copy(_First1, _Last1, _First2);
	}
};

constexpr _Copy_fn copy{};

struct _Copy_n_fn {
#if defined(WJR_HAS_EXECUTION)
	template<typename _Exec, typename _Iter1, typename _Size, typename _Iter2>
	_Iter2 operator()(_Exec&& exec, _Iter1 _First1, _Size count, _Iter2 _First2) const {
		return std::copy_n(exec, _First1, count, _First2);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Input, typename _Size, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Size count, _Output _First2) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_copy_v<_Input, _Output>) {
				if (count <= 0) { return _First2; }
				return wjr::copy(_First1, _First1 + count, _First2);
			}
		}
		return std::copy_n(_First1, count, _First2);
	}
};

constexpr _Copy_n_fn copy_n{};

struct _Copy_backward_fn {
#if defined(WJR_HAS_EXECUTION)
	template<typename _Exec, typename _Iter1, typename _Iter2>
	_Iter2 operator()(_Exec&& exec, _Iter1 _First1, _Iter1 _Last1, _Iter2 _Last2) const {
		return std::copy_backward(exec, _First1, _Last1, _Last2);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Input, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Input _Last1, _Output _Last2) const {
		return wjr::copy(std::make_reverse_iterator(_Last1),
			std::make_reverse_iterator(_First1),
			std::make_reverse_iterator(_Last2));
	}
};

constexpr _Copy_backward_fn copy_backward{};

struct _Move_fn {
#if defined(WJR_HAS_EXECUTION)
	template<typename _Exec, typename _Iter1, typename _Iter2>
	_Iter2 operator()(_Exec&& exec, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return std::move(exec, _First1, _Last1, _First2);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Input, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Input _Last1, _Output _First2) const {
		return wjr::copy(std::make_move_iterator(_First1), std::make_move_iterator(_Last1), _First2);
	}
};

constexpr _Move_fn move{};

struct _Move_backward_fn {
#if defined(WJR_HAS_EXECUTION)
	template<typename _Exec, typename _Iter1, typename _Iter2>
	_Iter2 operator()(_Exec&& exec, _Iter1 _First1, _Iter1 _Last1, _Iter2 _Last2) const {
		return std::move_backward(exec, _First1, _Last1, _Last2);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Input, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Input _Last1, _Output _Last2) const {
		return wjr::copy_backward(std::make_move_iterator(_First1), std::make_move_iterator(_Last1), _Last2);
	}
};

constexpr _Move_backward_fn move_backward{};

template<typename _Iter>
struct __has_fast_uninitialized_default_construct : std::conjunction<
	is_bit_default_constructible<remove_ref_t<iter_ref_t<_Iter>>>
> {};

template<typename _Iter>
constexpr bool __has_fast_uninitialized_default_construct_v = __has_fast_uninitialized_default_construct<_Iter>::value;

struct _Uninitialized_default_construct_fn {
#if defined(WJR_HAS_EXECUTION)
	template<typename _Exec, typename _Iter>
	void operator()(_Exec&& exec, _Iter _First, _Iter _Last) const {
		return std::uninitialized_default_construct(exec, _First, _Last);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Forward>
	WJR_CONSTEXPR20 void operator()(_Forward _First, _Forward _Last) const {
		if constexpr (__has_fast_uninitialized_default_construct_v<_Forward>) {
			return;
		}
		return std::uninitialized_default_construct(_First, _Last);
	}
};

constexpr _Uninitialized_default_construct_fn uninitialized_default_construct{};

template<typename Alloc>
WJR_CONSTEXPR20 void destroy_at_a(alloc_ptr_t<Alloc> _Where, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	if constexpr (is_default_allocator_destroy_v<Alloc, pointer>) {
		std::destroy_at(_Where);
	}
	else {
		std::allocator_traits<Alloc>::destroy(al, std::addressof(*_Where));
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void destroy_a(alloc_ptr_t<Alloc> _First, alloc_ptr_t<Alloc> _Last, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	using value_type = alloc_value_t<Alloc>;
	if constexpr (!std::conjunction_v<std::is_trivially_destructible<value_type>,
		is_default_allocator_destroy<Alloc, pointer>>) {
		for (; _First != _Last; ++_First) {
			destroy_at_a(_First, al);
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void destroy_n_a(alloc_ptr_t<Alloc> _First, alloc_size_t<Alloc> n, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	using value_type = alloc_value_t<Alloc>;
	if constexpr (!std::conjunction_v<std::is_trivially_destructible<value_type>,
		is_default_allocator_destroy<Alloc, pointer>>) {
		for (; n > 0; --n, ++_First) {
			destroy_at_a(_First, al);
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void uninitialized_default_construct_a(
	alloc_ptr_t<Alloc> _First, alloc_ptr_t<Alloc> _Last, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	using value_type = alloc_value_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer>) {
		std::uninitialized_default_construct(_First, _Last);
	}
	else {
		if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
			for (; _First != _Last; ++_First) {
				std::allocator_traits<Alloc>::construct(al, std::addressof(*_First));
			}
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void uninitialized_default_construct_n_a(
	alloc_ptr_t<Alloc> _First, alloc_size_t<Alloc> _Count, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	using value_type = alloc_value_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer>) {
		std::uninitialized_default_construct_n(_First, _Count);
	}
	else {
		if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
			for (; _Count > 0; --_Count, ++_First) {
				std::allocator_traits<Alloc>::construct(al, std::addressof(*_First));
			}
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_a(
	alloc_ptr_t<Alloc> _First, alloc_ptr_t<Alloc> _Last, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer>) {
		std::uninitialized_value_construct(_First, _Last);
	}
	else {
		pointer _Next = _First;
		for (; _Next != _Last; ++_Next) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next));
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 alloc_ptr_t<Alloc> uninitialized_value_construct_n_a(
	alloc_ptr_t<Alloc> _First, alloc_size_t<Alloc> _Count, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer>) {
		return std::uninitialized_value_construct_n(_First, _Count);
	}
	else {
		pointer _Next = _First;
		for (; _Count > 0; --_Count) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next));
			++_Next;
		}
		return _Next;
	}
}

template<typename iter, typename Alloc, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 alloc_ptr_t<Alloc> uninitialized_copy_a(
	iter _First, iter _Last, alloc_ptr_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer, decltype(*_First)>) {
		return std::uninitialized_copy(_First, _Last, _Dest);
	}
	else {
		pointer _Next = _Dest;
		for (; _First != _Last; ++_First) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), *_First);
			++_Next;
		}
		return _Next;
	}
}

template<typename iter, typename Alloc, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 alloc_ptr_t<Alloc> uninitialized_copy_n_a(
	iter _First, size_t _Count, alloc_ptr_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer, decltype(*_First)>) {
		return std::uninitialized_copy_n(_First, _Count, _Dest);
	}
	else {
		pointer _Next = _Dest;
		for (; _Count > 0; --_Count) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), *_First);
			++_Next;
			++_First;
		}
		return _Next;
	}
}

template<typename iter, typename Alloc, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 alloc_ptr_t<Alloc> uninitialized_move_a(
	iter _First, iter _Last, alloc_ptr_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer, decltype(std::move(*_First))>) {
		return std::uninitialized_move(_First, _Last, _Dest);
	}
	else {
		pointer _Next = _Dest;
		for (; _First != _Last; ++_First) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), std::move(*_First));
			++_Next;
		}
		return _Next;
	}
}

template<typename iter, typename Alloc, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 std::pair<iter, alloc_ptr_t<Alloc>> uninitialized_move_n_a(
	iter _First, size_t _Count, alloc_ptr_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer, decltype(std::move(*_First))>) {
		return std::uninitialized_move_n(_First, _Count, _Dest);
	}
	else {
		pointer _Next = _Dest;
		for (; _Count > 0; --_Count) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), std::move(*_First));
			++_Next;
			++_First;
		}
		return { _First, _Next };
	}
}

template<typename Alloc, typename ValueType>
WJR_CONSTEXPR20 void uninitialized_fill_a(
	alloc_ptr_t<Alloc> _First, alloc_ptr_t<Alloc> _Last,
	const ValueType& _Val, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer, decltype(_Val)>) {
		std::uninitialized_fill(_First, _Last, _Val);
	}
	else {
		pointer _Next = _First;
		for (; _Next != _Last; ++_Next) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), _Val);
		}
	}
}

template<typename Alloc, typename ValueType>
WJR_CONSTEXPR20 alloc_ptr_t<Alloc> uninitialized_fill_n_a(
	alloc_ptr_t<Alloc> _First,
	alloc_size_t<Alloc> _Count,
	const ValueType& _Val, Alloc& al) {
	using pointer = alloc_ptr_t<Alloc>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer, decltype(_Val)>) {
		return std::uninitialized_fill_n(_First, _Count, _Val);
	}
	else {
		pointer _Next = _First;
		for (; 0 < _Count; --_Count) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), _Val);
			++_Next;
		}
		return _Next;
	}
}

_WJR_END

#endif

#pragma once
#ifndef __WJR_ALLOCATOR_H
#define __WJR_ALLOCATOR_H

#include <new>


_WJR_BEGIN

template<typename _Ty>
struct __aligned_helper {
	constexpr static size_t value = std::max((size_t)alignof(_Ty), (size_t)16ull);
};

template<>
struct __aligned_helper<void> {
	constexpr static size_t value = 16;
};

//#define WJR_TEST_ALLOCATOR

#if defined(WJR_TEST_ALLOCATOR)
struct __test_allocator {
	~__test_allocator();
	size_t _Count = 0;
};
extern __test_allocator __test_allocator_instance;
#endif

template<typename _Ty, size_t _Al = __aligned_helper<_Ty>::value, size_t _Off = 0>
class aligned_allocator {
public:
	static_assert(!std::is_const_v<_Ty>, "The C++ Standard forbids containers of const elements "
		"because allocator<const T> is ill-formed.");
	static_assert(_Al >= 8, "The alignment must be greater than or equal to 8.");
	static_assert(_Off >= 0 && _Off < _Al,
		"The offset must be greater than or equal to 0 and less than the alignment.");

	using value_type = _Ty;

#if !defined(WJR_CPP_20)
	using pointer = _Ty*;
	using const_pointer = const _Ty*;

	using reference = _Ty&;
	using const_reference = const _Ty&;
#endif // !WJR_CPP_20

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

#if !defined(WJR_CPP_20)
	template <typename _Other>
	struct rebind {
		using other = aligned_allocator<_Other, _Al, _Off>;
	};

	WJR_NODISCARD _Ty* address(_Ty& _Val) const noexcept {
		return std::addressof(_Val);
	}

	WJR_NODISCARD const _Ty* address(const _Ty& _Val) const noexcept {
		return std::addressof(_Val);
	}
#endif // !WJR_CPP_20

	constexpr aligned_allocator() noexcept {}

	constexpr aligned_allocator(const aligned_allocator&) noexcept = default;
	template <typename _Other>
	constexpr aligned_allocator(const aligned_allocator<_Other, _Al, _Off>&) noexcept {}
	WJR_CONSTEXPR20 ~aligned_allocator() = default;
	WJR_CONSTEXPR20 aligned_allocator& operator=(const aligned_allocator&) = default;

	WJR_CONSTEXPR20 void deallocate(_Ty* const _Ptr, const size_t _Count) {
#if defined(WJR_TEST_ALLOCATOR)
		__test_allocator_instance._Count -= _Count * sizeof(_Ty);
#endif
		assume(reinterpret_cast<uintptr_t>(_Ptr) % _Al == _Off);
		auto ptr = reinterpret_cast<char*>(_Ptr) - _Off;
		::operator delete(ptr, _Count * sizeof(_Ty) + _Off, static_cast<std::align_val_t>(_Al));
	}

	WJR_NODISCARD WJR_CONSTEXPR20 _Ty* allocate(const size_t _Count) {
#if defined(WJR_TEST_ALLOCATOR)
		__test_allocator_instance._Count += _Count * sizeof(_Ty) + _Off;
#endif
		auto ptr = static_cast<char*>(::operator new(_Count * sizeof(_Ty) + _Off,
			static_cast<std::align_val_t>(_Al))) + _Off;
		assume(reinterpret_cast<uintptr_t>(ptr) % _Al == _Off);
		return reinterpret_cast<_Ty*>(ptr);
	}

#if !defined(WJR_CPP_20)
	WJR_NODISCARD _Ty* allocate(const size_t _Count, const void*) {
		return allocate(_Count);
	}

	WJR_NODISCARD size_t max_size() const noexcept {
		return static_cast<size_t>(-1) / sizeof(_Ty);
	}
#endif // !WJR_CPP_20
};

template <size_t _Al, size_t _Off>
class aligned_allocator<void, _Al, _Off> {
public:
	using value_type = void;
#if !defined(WJR_CPP_20)
	using pointer = void*;
	using const_pointer = const void*;
#endif // !WJR_CPP_20

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

#if !defined(WJR_CPP_20)
	template <class _Other>
	struct rebind {
		using other = aligned_allocator<_Other>;
	};
#endif // !WJR_CPP_20
};

template <typename _Ty, typename _Other, size_t _Al, size_t _Off>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
	const aligned_allocator<_Ty, _Al, _Off>&,
	const aligned_allocator<_Other, _Al, _Off>&) noexcept {
	return true;
}

template <typename _Ty, size_t _Al1, size_t _Off1, typename _Other, size_t _Al2, size_t _Off2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
	const aligned_allocator<_Ty, _Al1, _Off1>& lhs,
	const aligned_allocator<_Other, _Al2, _Off2>& rhs) noexcept {
	return false;
}

template <typename _Ty, size_t _Al1, size_t _Off1, typename _Other, size_t _Al2, size_t _Off2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
	const aligned_allocator<_Ty, _Al1, _Off1>& lhs,
	const aligned_allocator<_Other, _Al2, _Off2>& rhs) noexcept {
	return !(lhs == rhs);
}

template<typename T, size_t _Al, size_t _Off>
struct is_default_allocator<aligned_allocator<T, _Al, _Off>> : std::true_type {};

template<typename Alloc>
class temporary_allocator_value {
public:
	using value_type = alloc_value_t<Alloc>;
	using traits = std::allocator_traits<Alloc>;

	template<typename...Args>
	constexpr explicit temporary_allocator_value(Alloc& al, Args&&...args) noexcept
		: al(al) {
		traits::construct(al, get_ptr(), std::forward<Args>(args)...);
	}

	temporary_allocator_value(const temporary_allocator_value&) = delete;
	temporary_allocator_value& operator=(const temporary_allocator_value&) = delete;

	~temporary_allocator_value() {
		destroy_at_a(get_ptr(), al);
	}

	constexpr value_type* get_ptr() {
		return reinterpret_cast<value_type*>(std::addressof(vl));
	}

	constexpr const value_type* get_ptr() const {
		return reinterpret_cast<const value_type*>(std::addressof(vl));
	}

	constexpr value_type& value() {
		return *get_ptr();
	}

	constexpr const value_type& value() const {
		return *get_ptr();
	}

private:

	Alloc& al;
	std::aligned_storage_t<sizeof(value_type), alignof(value_type)> vl;
};

_WJR_END

#endif // __WJR_ALLOCATOR_H                                                                                                                                                                                                     

_WJR_BEGIN

/*
* vector_data ->
* vector_data must be shallow copy in copy constructor and copy assignment operator,
* but deep copy in copy_construct, move_construct can be shallow copy/deep copy
* 1.static vector_data Allocate(_Al& al, size_t _Size, size_t _Capacity)
* 2.static void copyConstruct(_Al& al, const vector_data& _Src, vector_data& _Dest)
* 3.static void moveConstruct(_Al& al, vector_data&& _Src, vector_data& _Dest)
* 5.static void Swap(_Al& al, vector_data& _Left, vector_data& _Right) [noexcept] [optional]
* 7.static void Deallocate(_Al& al, vector_data& _Data)
* 9.static size_t getGrowthCapacity(const size_t _Oldcapacity, const size_t _Newsize) [optinal]
* 10.static void shrinkToFit(_Al&al, vector_data& _Data) [optional]
* 11.size_t size() const;
* 12.size_t capacity() const;
* 13.[const] _Ty* data() [const]
* 14.const _Ty* cdata() const
* 15._Ty* mutable_data() [TODO]
* 16.void set_size();
*/

/*
* vector_core ->
* 1.vector_core() [noexcept] = default;
* 2.vector_core(const allocator_type& al) [noexcept];
* 3.template<typename _Alloc> vector_core(const vector_core& other, _Alloc&& al) [noexcept];
* 4.vector_core(vector_core&& other) [noexcept];
* 5.vector_core(vector_core&& other, const allocator_type& al) [noexcept];
* 6.vector_core& operator=(const vector_core&) = delete;
* 7.vector_core& operator=(vector_core&&) = delete;
* 8.~vector_core() [noexcept];
* 9.static vector_core Allocate(_Al& al, size_t _Size, size_t _Capacity);
* 10.static void copyConstruct(_Al& al, const vector_data& _Src, vector_data& _Dest)
* 11.static void moveConstruct(_Al& al, vector_data&& _Src, vector_data& _Dest)
* 14.static void Swap(_Al& al, vector_data& _Left, vector_data& _Right) [noexcept]
* 15.static void Destroy(_Al& al, vector_data& _Data);
* 16.static void Deallocate(_Al& al, vector_data& _Data);
* 17.static void Tidy(_Al& al, vector_data& _Data);
* 18.static size_t getGrowthCapacity(const size_t _Oldcapacity, const size_t _Newsize);
* 19.static void shrinkToFit(_Al&al, vector_data& _Data);
* 20.void tidy() [noexcept];
* 21.void swap(vector_core& other) [noexcept];
* 22.size_t size() const;
* 23.size_t capacity() const;
* 24.[const] _Ty* data() [const]
* 25.const _Ty* cdata() const
* 26._Ty* mutable_data() [TODO]
* 27.void set_size();
* 28.[const] _Al& getAllocator() [const];
* 29.[const] vector_data& getData() [const];
*/

// support small-size-optimize
// don't support copy-on-write yet

template<typename T, typename Alloc>
struct vector_data {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
	using value_type = T;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;

	WJR_CONSTEXPR20 static vector_data Allocate(_Alty& al, size_type _Size, size_type _Capacity) {
		auto ptr = _Alty_traits::allocate(al, _Capacity);
		return { ptr, ptr + _Size, ptr + _Capacity };
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_data& _Src, vector_data& _Dest) {
		auto data = Allocate(al, _Src.size(), _Src.capacity());
		wjr::uninitialized_copy_a(_Src._Myfirst, _Src._Mylast, data._Myfirst, al);
		_Dest = data;
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, vector_data&& _Src, vector_data& _Dest) noexcept {
		_Dest = _Src;
		_Src._Myfirst = _Src._Mylast = _Src._Myend = nullptr;
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, vector_data& _Data) {
		_Alty_traits::deallocate(al, _Data._Myfirst, _Data.capacity());
		_Data._Myfirst = _Data._Mylast = _Data._Myend = nullptr;
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return static_cast<size_type>(_Mylast - _Myfirst);
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return static_cast<size_type>(_Myend - _Myfirst);
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return _Myfirst;
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return _Myfirst;
	}

	WJR_CONSTEXPR20 const_pointer cdata() const noexcept {
		return _Myfirst;
	}

	WJR_CONSTEXPR20 void set_size(size_type _Size) noexcept {
		_Mylast = _Myfirst + _Size;
	}

	pointer _Myfirst = nullptr;
	pointer _Mylast = nullptr;
	pointer _Myend = nullptr;
};

template<typename T, typename Alloc, size_t N>
struct vector_static_data {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
	using value_type = T;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;

	static_assert(N > 0, "N must be greater than 0");

private:
	constexpr static size_t _MaxAlignment = std::max(alignof(T), std::max(alignof(size_type), alignof(max_align_t)));
	constexpr static size_t _MaxMemroy = (sizeof(T) * N + _MaxAlignment - 1) & (~(_MaxAlignment - 1));
	constexpr static size_t _MaxCapacity = _MaxMemroy / sizeof(T);
	static_assert(_MaxCapacity >= N, "");
public:

	WJR_CONSTEXPR20 static vector_static_data Allocate(_Alty& al, size_type _Size, size_type _Capacity) {
		std::string str = "vector_static_data is too small to hold the requested data";
		str += "\n old capacity = " + std::to_string(_MaxCapacity);
		str += "\n allocate new size = " + std::to_string(_Size);
		str += "\n allocate new capacity = " + std::to_string(_Capacity);
		throw std::length_error(str);
		unreachable();
		return vector_static_data{};
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_static_data& _Src, vector_static_data& _Dest) {
		wjr::uninitialized_copy_n_a(_Src.data(), _Src.size(), _Dest.data(), al);
		_Dest.set_size(_Src.size());
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, vector_static_data&& _Src, vector_static_data& _Dest) {
		auto n = _Src.size();
		if (n != 0) {
			wjr::uninitialized_move_n_a(_Src.data(), n, _Dest.data(), al);
			wjr::destroy_n_a(_Src.data(), n, al);
			_Src.set_size(0);
			_Dest.set_size(n);
		}
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, vector_static_data& _Data) noexcept {
		// do nothing
	}

	WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, vector_static_data& _Data) {
#ifndef _WJR_NOEXCEPTION
		throw std::logic_error("vector_static_data does not support shrink_to_fit");
#endif // !_WJR_NOEXCEPTION
		unreachable();
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return _M_size;
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return _MaxCapacity;
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return reinterpret_cast<pointer>(_M_storage);
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return reinterpret_cast<const_pointer>(_M_storage);
	}

	WJR_CONSTEXPR20 const_pointer cdata() const noexcept {
		return reinterpret_cast<const_pointer>(_M_storage);
	}

	WJR_CONSTEXPR20 void set_size(size_type _Size) noexcept {
		_M_size = _Size;
	}

	alignas(_MaxAlignment) uint8_t _M_storage[_MaxMemroy];
	alignas(_MaxAlignment) size_type _M_size;
};


namespace _Vector_helper {
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(Swap, Swap);
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(getGrowthCapacity, getGrowthCapacity);
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(shrinkToFit, shrinkToFit);
}

template<typename T, typename Alloc, typename Data = vector_data<T, Alloc>>
struct vector_core {
public:
	using _Alty = typename Data::_Alty;
	using _Alty_traits = typename Data::_Alty_traits;
	using value_type = typename Data::value_type;
	using pointer = typename Data::pointer;
	using const_pointer = typename Data::const_pointer;
	using size_type = typename Data::size_type;
	using difference_type = typename Data::difference_type;

	using allocator_type = Alloc;
	using reference = value_type&;
	using const_reference = const value_type&;

	using data_type = Data;

	WJR_CONSTEXPR20 vector_core() noexcept(std::is_nothrow_default_constructible_v<_Alty>) = default;

	WJR_CONSTEXPR20 vector_core(const allocator_type& al)
		noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>) :
		_Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {}

	template<typename _Alloc>
	WJR_CONSTEXPR20 vector_core(const vector_core& other, _Alloc&& al)
		noexcept(std::is_nothrow_constructible_v<_Alty&, _Alloc&&> &&
			noexcept(copyConstruct(std::declval<_Alty&>(), std::declval<const data_type&>(), std::declval<data_type&>())))
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::forward<_Alloc>(al)),
			std::forward_as_tuple()) {
		copyConstruct(getAllocator(), other.getData(), getData());
	}

	WJR_CONSTEXPR20 vector_core(vector_core&& other)
		noexcept(std::is_nothrow_move_constructible_v<_Alty> &&
			noexcept(moveConstruct(std::declval<_Alty&>(), std::declval<data_type&&>(), std::declval<data_type&>())))
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::move(other.getAllocator())),
			std::forward_as_tuple()) {
		moveConstruct(getAllocator(), std::move(other.getData()), getData());
	}

private:
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& al, std::true_type) noexcept
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {
		moveConstruct(getAllocator(), std::move(other.getData()), getData());
	}
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& _Al, std::false_type)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(_Al),
			std::forward_as_tuple()) {
		auto& al = getAllocator();
		if (al == other.getAllocator()) {
			moveConstruct(al, std::move(other.getData()), getData());
		}
		else {
			const auto _Size = other.size();
			if (is_likely(_Size != 0)) {
				if (capacity() < _Size) {
					auto _Data = Allocate(al, _Size, _Size);
					moveConstruct(al, std::move(_Data), getData());
				}
				else {
					set_size(_Size);
				}
				wjr::uninitialized_move_n_a(other.data(), _Size, data(), al);
			}
		}
	}

public:
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& al)
		noexcept(noexcept(vector_core(std::declval<vector_core&&>(),
			std::declval<const allocator_type&>(), std::declval<typename _Alty_traits::is_always_equal>())))
		: vector_core(std::move(other), al, typename _Alty_traits::is_always_equal{}) {}

	vector_core& operator=(const vector_core&) = delete;
	vector_core& operator=(vector_core&&) = delete;

	WJR_CONSTEXPR20 ~vector_core() noexcept {
		tidy();
	}

	WJR_CONSTEXPR20 static Data Allocate(_Alty& al, size_type _Count, size_type _Capacity)
		noexcept(noexcept(Data::Allocate(al, _Count, _Capacity))) {
		return Data::Allocate(al, _Count, _Capacity);
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const Data& _Src, Data& _Dest)
		noexcept(noexcept(Data::copyConstruct(al, _Src, _Dest))) {
		return Data::copyConstruct(al, _Src, _Dest);
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, Data&& _Src, Data& _Dest)
		noexcept(noexcept(Data::moveConstruct(al, std::move(_Src), _Dest))) {
		return Data::moveConstruct(al, std::move(_Src), _Dest);
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_Swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void Swap(_Alty& al, _D& _Left, _D& _Right)
		noexcept(noexcept(Data::Swap(al, _Left, _Right))) {
		return Data::Swap(al, _Left, _Right);
	}

	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_Swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void Swap(_Alty& al, _D& _Left, _D& _Right)
		noexcept(noexcept(moveConstruct(std::declval<_Alty&>(), std::declval<_D&&>(), std::declval<_D&>()))) {
		_D _Tmp;
		moveConstruct(al, std::move(_Left), _Tmp);
		moveConstruct(al, std::move(_Right), _Left);
		moveConstruct(al, std::move(_Tmp), _Right);
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, Data& _Data) noexcept {
		wjr::destroy_n_a(_Data.data(), _Data.size(), al);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, Data& _Data)
		noexcept(noexcept(Data::Deallocate(al, _Data))) {
		Data::Deallocate(al, _Data);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, Data& _Data) noexcept {
		if (_Data.capacity() != 0) {
			Destroy(al, _Data);
			Deallocate(al, _Data);
		}
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_getGrowthCapacity_v<_D, const size_type, const size_type>, int> = 0>
	WJR_CONSTEXPR20 static size_type getGrowthCapacity(const size_type _Oldcapacity, const size_type _Newsize) noexcept {
		return Data::getGrowthCapacity(_Oldcapacity, _Newsize);
	}
	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_getGrowthCapacity_v<_D, const size_type, const size_type>, int> = 0>
	WJR_CONSTEXPR20 static size_type getGrowthCapacity(const size_type _Oldcapacity, const size_type _Newsize) noexcept {
		assume(_Newsize > _Oldcapacity);
		const auto _Newcapacity = _Oldcapacity + _Oldcapacity;
		const auto _FixedCapacity = _Newcapacity < _Newsize ? _Newsize : _Newcapacity;
		return _FixedCapacity;
		/*return _FixedCapacity < 32 ? ((_FixedCapacity + 3) & (-4)) :
			_FixedCapacity < 256 ? ((_FixedCapacity + 15) & (-16)) :
			((_FixedCapacity + 63) & (-64));*/
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_shrinkToFit_v<_D, _Alty&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, _D& _Data) noexcept {
		return Data::shrinkToFit(al, _Data);
	}

	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_shrinkToFit_v<_D, _Alty&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, _D& _Data) noexcept {
		auto _Oldsize = _Data.size();
		auto _Oldcapacity = _Data.capacity();
		if (_Oldsize != _Oldcapacity) {
			if (_Oldsize == 0) {
				Tidy(al, _Data);
			}
			else {
				auto _Newdata = Allocate(al, _Oldsize, _Oldsize);
				wjr::uninitialized_move_n_a(_Data.data(), _Oldsize, _Newdata.data(), al);
				Tidy(al, _Data);
				moveConstruct(al, std::move(_Newdata), _Data);
			}
		}
	}

	WJR_CONSTEXPR20 void tidy() noexcept {
		Tidy(getAllocator(), getData());
	}

	WJR_CONSTEXPR20 void swap(vector_core& other) noexcept {
		Swap(getAllocator(), getData(), other.getData());
		if constexpr (_Alty_traits::propagate_on_container_swap::value) {
			std::swap(getAllocator(), other.getAllocator());
		}
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return getData().size();
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return getData().capacity();
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return getData().data();
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return getData().data();
	}

	WJR_CONSTEXPR20 const_pointer cdata() const noexcept {
		return getData().cdata();
	}

	WJR_CONSTEXPR20 void set_size(size_type _Size) noexcept {
		getData().set_size(_Size);
	}

	WJR_CONSTEXPR20 _Alty& getAllocator() noexcept {
		return _Myval.first();
	}

	WJR_CONSTEXPR20 const _Alty& getAllocator() const noexcept {
		return _Myval.first();
	}

	WJR_CONSTEXPR20 Data& getData() noexcept {
		return _Myval.second();
	}

	WJR_CONSTEXPR20 const Data& getData() const noexcept {
		return _Myval.second();
	}

	wjr::pair<_Alty, Data> _Myval;
};

template<typename T, typename Alloc = std::allocator<T>, typename Core = vector_core<T, Alloc>>
class vector : private Core {

	using _Mybase = Core;

	using _Alty = typename Core::_Alty;
	using _Alty_traits = typename Core::_Alty_traits;

	static_assert(std::is_same_v<typename _Alty_traits::value_type, T>,
		"Alloc::value_type must be the same as T");

	static_assert(std::is_same_v<typename Core::allocator_type, _Alty>,
		"Core::allocator_type must be the same as _Alty");

public:

	using value_type = typename Core::value_type;
	using allocator_type = typename Core::allocator_type;
	using size_type = typename Core::size_type;
	using difference_type = typename Core::difference_type;
	using reference = typename Core::reference;
	using const_reference = typename Core::const_reference;
	using pointer = typename Core::pointer;
	using const_pointer = typename Core::const_pointer;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using core_type = Core;
	using data_type = typename Core::data_type;

	WJR_CONSTEXPR20 vector() noexcept(std::is_nothrow_constructible_v<core_type>) = default;

	WJR_CONSTEXPR20 explicit vector(const allocator_type& al) noexcept
		: _Mybase(al) {}

	WJR_CONSTEXPR20 explicit vector(const size_type _Count, const allocator_type& al = allocator_type())
		: _Mybase(al) {
		_M_construct_n(_Count);
	}

	WJR_CONSTEXPR20 vector(size_type _Count, const value_type& _Val, const allocator_type& al = allocator_type())
		: _Mybase(al) {
		_M_construct_n(_Count, _Val);
	}

	WJR_CONSTEXPR20 vector(const vector& other)
		: _Mybase(other, _Alty_traits::select_on_container_copy_construction(other.getAllocator())) {}

	WJR_CONSTEXPR20 vector(vector&& other) noexcept = default;

	WJR_CONSTEXPR20 vector(const vector& other, const allocator_type& al)
		: _Mybase(other, al) {}

	WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& al)
		noexcept(std::is_nothrow_constructible_v<core_type, core_type&&, const allocator_type&>)
		: _Mybase(std::move(other), al) {}

	template<typename _Iter, std::enable_if_t<wjr::is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 vector(_Iter _First, _Iter _Last, const allocator_type& al = allocator_type())
		: _Mybase(al) {
		_M_range_init(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
	}

	WJR_CONSTEXPR20 vector(std::initializer_list<value_type> _Ilist, const allocator_type& al = allocator_type())
		: _Mybase(al) {
		_M_range_init(_Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
	}

	WJR_CONSTEXPR20 ~vector() noexcept = default;

	WJR_CONSTEXPR20 vector& operator=(const vector& other) {

		if (is_likely(this != std::addressof(other))) {
			auto& al = getAllocator();
			auto& other_al = other.getAllocator();

			if constexpr (std::conjunction_v<typename _Alty_traits::propagate_on_container_copy_assignment
				, std::negation<typename _Alty_traits::is_always_equal>>) {
				if (al != other_al) {
					tidy();
				}
			}

			if constexpr (_Alty_traits::propagate_on_container_copy_assignment::value) {
				al = other_al;
			}

			assign(other.begin(), other.end());
		}

		return *this;
	}

	WJR_CONSTEXPR20 vector& operator=(vector&& other)
		noexcept(_Alty_traits::propagate_on_container_move_assignment::value
			|| _Alty_traits::is_always_equal::value) {

		using type = std::disjunction<typename _Alty_traits::propagate_on_container_move_assignment,
			typename _Alty_traits::is_always_equal>;

		if (is_likely(this != std::addressof(other))) {
			_M_move_assign(std::move(other), type{});
		}

		return *this;
	}

	WJR_CONSTEXPR20 vector& operator=(std::initializer_list<value_type> _Ilist) {
		_M_assign_aux(_Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
		return *this;
	}

	WJR_CONSTEXPR20 vector& assign(size_type _Count, const value_type& _Val) {
		_M_fill_assign(_Count, _Val);
		return *this;
	}

	template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
	WJR_CONSTEXPR20 vector& assign(iter _First, iter _Last) {
		_M_assign_aux(_First, _Last, typename std::iterator_traits<iter>::iterator_category());
		return *this;
	}

	WJR_CONSTEXPR20 vector& assign(std::initializer_list<value_type> _Ilist) {
		_M_assign_aux(_Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
		return *this;
	}


	WJR_CONSTEXPR20 iterator begin() noexcept {
		return data();
	}

	WJR_CONSTEXPR20 const_iterator begin() const noexcept {
		return data();
	}

	WJR_CONSTEXPR20 const_iterator cbegin() const noexcept {
		return data();
	}

	WJR_CONSTEXPR20 iterator end() noexcept {
		return begin() + size();
	}

	WJR_CONSTEXPR20 const_iterator end() const noexcept {
		return begin() + size();
	}

	WJR_CONSTEXPR20 const_iterator cend() const noexcept {
		return cbegin() + size();
	}

	WJR_CONSTEXPR20 reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	WJR_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	WJR_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}

	WJR_CONSTEXPR20 reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	WJR_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	WJR_CONSTEXPR20 const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return _Mybase::size();
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize) {
		auto& al = getAllocator();
		temporary_allocator_value<Alloc> _Tmp(al);
		resize(_Newsize, _Tmp.value());
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize, const value_type& _Val) {
		const auto _Oldsize = size();
		if (_Newsize > _Oldsize) {
			_M_append(_Newsize - _Oldsize, _Val);
		}
		else if (_Newsize < _Oldsize) {
			_M_erase_at_end(data() + _Newsize);
		}
		assume(size() == _Newsize);
	}

	WJR_CONSTEXPR20 void shrink_to_fit() {
		shrinkToFit(getAllocator(), getData());
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return _Mybase::capacity();
	}

	WJR_CONSTEXPR20 bool empty() const noexcept {
		return size() == 0;
	}

	WJR_CONSTEXPR20 void reserve(size_type n) {
		const auto _Oldcapacity = capacity();
		if (_Oldcapacity < n) {
			auto& al = getAllocator();

			const auto _Oldsize = size();
			const auto _Newcapacity = getGrowthCapacity(_Oldcapacity, n);

			auto _Newdata = Allocate(al, _Oldsize, _Newcapacity);
			wjr::uninitialized_move_n_a(data(), _Oldsize, _Newdata.data(), al);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	WJR_CONSTEXPR20 reference operator[](size_type _Pos) noexcept {
		return *(data() + _Pos);
	}

	WJR_CONSTEXPR20 const_reference operator[](size_type _Pos) const noexcept {
		return *(data() + _Pos);
	}

	WJR_CONSTEXPR20 reference at(size_type _Pos) {
		if (_Pos >= size()) {
			throw (std::out_of_range("wjr::vector::at"));
		}
		return (*this)[_Pos];
	}

	WJR_CONSTEXPR20 const_reference at(size_type _Pos) const {
		if (_Pos >= size()) {
			throw (std::out_of_range("wjr::vector::at"));
		}
		return (*this)[_Pos];
	}

	WJR_CONSTEXPR20 reference front() noexcept {
		return *data();
	}

	WJR_CONSTEXPR20 const_reference front() const noexcept {
		return *data();
	}

	WJR_CONSTEXPR20 reference back() noexcept {
		return *(data() + size() - 1);
	}

	WJR_CONSTEXPR20 const_reference back() const noexcept {
		return *(data() + size() - 1);
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return _Mybase::data();
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return _Mybase::data();
	}

	WJR_CONSTEXPR20 const_pointer cdata() const noexcept {
		return _Mybase::cdata();
	}

	template<typename...Args>
	WJR_CONSTEXPR20 reference emplace_back(Args&&... args) {
		auto& al = getAllocator();
		const auto _Myfirst = data();
		const auto _Mylast = data() + size();
		const auto _Myend = data() + capacity();
		if (is_likely(_Mylast != _Myend)) {
			_Alty_traits::construct(al, _Mylast, std::forward<Args>(args)...);
			set_size(size() + 1);
		}
		else {
			_M_realloc_insert_at_end(std::forward<Args>(args)...);
		}
		return back();
	}

	WJR_CONSTEXPR20 void push_back(const value_type& _Val) {
		emplace_back(_Val);
	}

	WJR_CONSTEXPR20 void push_back(value_type&& _Val) {
		emplace_back(std::move(_Val));
	}

	WJR_CONSTEXPR20 void pop_back() noexcept {
		set_size(size() - 1);
		const pointer _Mylast = data() + size();
		_Alty_traits::destroy(getAllocator(), _Mylast);
	}

	template<typename...Args>
	WJR_CONSTEXPR20 iterator emplace(const_iterator _Where, Args&&... args) {
		return _M_emplace_aux(_Where, std::forward<Args>(args)...);
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, const value_type& _Val) {
		return emplace(_Where, _Val);
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, value_type&& _Val) {
		return emplace(_Where, std::move(_Val));
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, std::initializer_list<value_type> _Ilist) {
		const auto __old_pos = _Where - cbegin();
		_M_range_insert(begin() + __old_pos, _Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
		return begin() + __old_pos;
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, size_type _Count, const value_type& _Val) {
		const auto __old_pos = _Where - cbegin();
		_M_fill_insert(begin() + __old_pos, _Count, _Val);
		return begin() + __old_pos;
	}

	template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, iter _First, iter _Last) {
		const auto __old_pos = _Where - cbegin();
		_M_range_insert(begin() + __old_pos, _First, _Last, typename std::iterator_traits<iter>::iterator_category());
		return begin() + __old_pos;
	}

	WJR_CONSTEXPR20 iterator erase(const_iterator _Where) {
		return _M_erase(begin() + (_Where - cbegin()));
	}

	WJR_CONSTEXPR20 iterator erase(const_iterator _First, const_iterator _Last) {
		const auto __beg = begin();
		const auto __cbeg = cbegin();
		return _M_erase(__beg + (_First - __cbeg), __beg + (_Last - __cbeg));
	}

	WJR_CONSTEXPR20 void swap(vector& _Right) noexcept {
		getCore().swap(_Right.getCore());
	}

	WJR_CONSTEXPR20 void clear() {
		_M_erase_at_end(data());
	}

	WJR_CONSTEXPR20 void tidy() noexcept {
		Tidy(getAllocator(), getData());
	}

	WJR_CONSTEXPR20 allocator_type& get_allocator() noexcept {
		return getAllocator();
	}
	WJR_CONSTEXPR20 const allocator_type& get_allocator() const noexcept {
		return getAllocator();
	}

	// unstandard functions

	WJR_CONSTEXPR20 static data_type Allocate(_Alty& al, size_type _Newsize, size_type _Newcapacity) {
		return _Mybase::Allocate(al, _Newsize, _Newcapacity);
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const data_type& _Src, data_type& _Dest) {
		_Mybase::copyConstruct(al, _Src, _Dest);
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, data_type&& _Src, data_type& _Dest) {
		_Mybase::moveConstruct(al, std::move(_Src), _Dest);
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, data_type& _Data) {
		_Mybase::Destroy(al, _Data);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, data_type& _Data) noexcept {
		_Mybase::Deallocate(al, _Data);
		assume(_Data.size() == 0);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, data_type& _Data) noexcept {
		_Mybase::Tidy(al, _Data);
		assume(_Data.size() == 0);
	}

	WJR_CONSTEXPR20 static size_type getGrowthCapacity(const size_type _Oldcapacity, size_type _Newsize) noexcept {
		return _Mybase::getGrowthCapacity(_Oldcapacity, _Newsize);
	}

	WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, data_type& _Data) {
		_Mybase::shrinkToFit(al, _Data);
	}

	WJR_CONSTEXPR20 core_type& getCore() noexcept {
		return *this;
	}
	WJR_CONSTEXPR20 const core_type& getCore() const noexcept {
		return *this;
	}

	WJR_CONSTEXPR20 _Alty& getAllocator() noexcept {
		return _Mybase::getAllocator();
	}
	WJR_CONSTEXPR20 const _Alty& getAllocator() const noexcept {
		return _Mybase::getAllocator();
	}

	WJR_CONSTEXPR20 data_type& getData() noexcept {
		return _Mybase::getData();
	}
	WJR_CONSTEXPR20 const data_type& getData() const noexcept {
		return _Mybase::getData();
	}

	WJR_CONSTEXPR20 void set_size(size_type _Newsize) noexcept {
		_Mybase::set_size(_Newsize);
	}

private:

	template<typename...Args, std::enable_if_t<sizeof...(Args) <= 2, int> = 0>
	WJR_CONSTEXPR20 void _M_construct_n(const size_type _Count, Args&&... args) {
		if (_Count != 0) {
			auto& al = getAllocator();
			if (capacity() < _Count) {
				auto _Newdata = Allocate(al, _Count, _Count);
				moveConstruct(al, std::move(_Newdata), getData());
			}
			else {
				set_size(_Count);
			}
			const pointer _Ptr = data();
			if constexpr (sizeof...(Args) == 0) {
				wjr::uninitialized_value_construct_n_a(_Ptr, _Count, al);
			}
			else if constexpr (sizeof...(Args) == 1) {
				wjr::uninitialized_fill_n_a(_Ptr, _Count, std::forward<Args>(args)..., al);
			}
			else if constexpr (sizeof...(Args) == 2) {
				wjr::uninitialized_copy_a(std::forward<Args>(args)..., _Ptr, al);
			}
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_init(iter _First, iter _Last, std::input_iterator_tag) {
		for (; _First != _Last; ++_First) {
			emplace_back(*_First);
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_init(iter _First, iter _Last, std::forward_iterator_tag) {
		const auto _Count = static_cast<size_type>(std::distance(_First, _Last));
		_M_construct_n(_Count, _First, _Last);
	}

	WJR_CONSTEXPR20 void _M_erase_at_end(pointer _Where) noexcept {
		const auto _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		wjr::destroy_a(_Where, _Mylast, getAllocator());
		const auto __new_size = static_cast<size_type>(_Where - _Myfirst);
		set_size(__new_size);
	}

	WJR_CONSTEXPR20 iterator _M_erase(iterator _Where) noexcept {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		if (_Where + 1 != _Mylast) {
			wjr::move(_Where + 1, _Mylast, _Where);
		}
		wjr::destroy_at_a(_Mylast - 1, getAllocator());
		set_size(size() - 1);
		return _Where;
	}

	WJR_CONSTEXPR20 iterator _M_erase(iterator _First, iterator _Last) noexcept {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		if (_First != _Last) {
			if (_Last != _Mylast) {
				wjr::move(_Last, _Mylast, _First);
			}
			_M_erase_at_end(_Mylast - (_Last - _First));
		}
		return _First;
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_insert(iterator _Where, iter _First, iter _Last, std::input_iterator_tag) {
		if (_Where == end()) {
			_M_range_append(_First, _Last, std::input_iterator_tag{});
		}
		else if (_First != _Last) {
			vector _Tmp(_First, _Last, getAllocator());
			insert(_Where, _Tmp.begin(), _Tmp.end());
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_insert(iterator _Where, iter _First, iter _Last, std::forward_iterator_tag) {
		if (_First != _Last) {
			auto& al = getAllocator();
			const pointer _Myfirst = data();
			const pointer _Mylast = _Myfirst + size();
			const pointer _Myend = _Myfirst + capacity();

			const auto n = static_cast<size_type>(std::distance(_First, _Last));
			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= n) {
				const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
				auto __old_last = _Mylast;
				if (__elements_after > n) {
					wjr::uninitialized_move_a(_Mylast - n, _Mylast, _Mylast, al);
					wjr::move_backward(_Where, __old_last - n, __old_last);
					wjr::copy(_First, _Last, _Where);
				}
				else {
					auto _Mid = _First;
					std::advance(_Mid, __elements_after);
					wjr::uninitialized_copy_a(_Mid, _Last, _Mylast, al);
					wjr::uninitialized_move_a(_Where, __old_last, _Mylast + n - __elements_after, al);
					wjr::copy(_First, _Mid, _Where);
				}
				set_size(size() + n);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
				const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + n);
				auto _Newdata = Allocate(al, __old_size + n, _Newcapacity);
				const pointer _Newfirst = _Newdata.data();

				wjr::uninitialized_copy_a(_First, _Last, _Newfirst + __old_pos, al);
				wjr::uninitialized_move_a(_Myfirst, _Where, _Newfirst, al);
				wjr::uninitialized_move_a(_Where, _Mylast, _Newfirst + __old_pos + n, al);

				tidy();
				moveConstruct(al, std::move(_Newdata), getData());
			}
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_append(iter _First, iter _Last, std::input_iterator_tag) {
		for (; _First != _Last; ++_First) {
			emplace_back(*_First);
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_append(iter _First, iter _Last, std::forward_iterator_tag) {
		if (_First != _Last) {
			auto& al = getAllocator();
			const pointer _Myfirst = data();
			const pointer _Mylast = _Myfirst + size();
			const pointer _Myend = _Myfirst + capacity();

			const auto n = static_cast<size_type>(std::distance(_First, _Last));
			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= n) {
				wjr::uninitialized_copy_a(_First, _Last, _Mylast, al);
				set_size(size() + n);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + n);

				auto _Newdata = Allocate(al, __old_size + n, _Newcapacity);
				const pointer _Newfirst = _Newdata.data();

				wjr::uninitialized_copy_a(_First, _Last, _Newfirst + __old_size, al);
				wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, al);

				tidy();
				moveConstruct(al, std::move(_Newdata), getData());
			}
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_assign_aux(iter _First, iter _Last, std::input_iterator_tag) {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		pointer cur = _Myfirst;

		for (; _First != _Last && cur != _Mylast; ++_First, ++cur) {
			*cur = *_First;
		}

		if (_First == _Last) {
			_M_erase_at_end(cur);
		}
		else {
			_M_range_append(_First, _Last, std::input_iterator_tag{});
		}
	}

	WJR_CONSTEXPR20 void _M_fill_assign(size_type _Count, const value_type& _Val) {
		auto& al = getAllocator();
		if (_Count > capacity()) {
			tidy();
			auto _Data = Allocate(al, _Count, _Count);
			moveConstruct(al, std::move(_Data), getData());
			wjr::uninitialized_fill_n_a(data(), _Count, _Val, al);
		}
		else if (_Count > size()) {
			wjr::fill(begin(), end(), _Val);
			wjr::uninitialized_fill_n_a(end(), _Count - size(), _Val, al);
			set_size(_Count);
		}
		else {
			_M_erase_at_end(wjr::fill_n(begin(), _Count, _Val));
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_assign_aux(iter _First, iter _Last, std::forward_iterator_tag) {
		auto _Count = static_cast<size_type>(std::distance(_First, _Last));
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		if (_Count <= size()) {
			wjr::copy(_First, _Last, _Myfirst);
			_M_erase_at_end(_Myfirst + _Count);
		}
		else if (_Count <= capacity()) {
			auto _Mid = _First;
			std::advance(_Mid, size());
			wjr::copy(_First, _Mid, begin());
			wjr::uninitialized_copy_a(_Mid, _Last, _Mylast, al);
			set_size(_Count);
		}
		else {
			auto _Newcapacity = getGrowthCapacity(capacity(), _Count);
			auto _Newdata = Allocate(al, _Count, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();
			wjr::uninitialized_copy_a(_First, _Last, _Newfirst, al);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	WJR_CONSTEXPR20 void _M_move_assign(vector&& other, std::true_type) {
		auto& al = getAllocator();
		tidy();
		moveConstruct(al, std::move(other.getData()), getData());
		if constexpr (_Alty_traits::propagate_on_container_move_assignment::value) {
			al = std::move(other.getAllocator());
		}
	}

	WJR_CONSTEXPR20 void _M_move_assign(vector&& other, std::false_type) {
		if (getAllocator() == other.getAllocator()) {
			_M_move_assign(std::move(other), std::true_type{});
		}
		else {
			_M_assign_aux(
				std::make_move_iterator(other.begin()),
				std::make_move_iterator(other.end()),
				std::random_access_iterator_tag{});
			clear();
		}
	}

	template<typename...Args>
	WJR_CONSTEXPR20 void _M_realloc_insert(iterator _Where, Args&&...args) {
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
		const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
		const auto __new_size = __old_size + 1;
		const auto _Newcapacity = getGrowthCapacity(__old_size, __new_size);

		auto _Newdata = Allocate(al, __new_size, _Newcapacity);

		const pointer _Newfirst = _Newdata.data();
		const pointer _Newwhere = _Newfirst + __old_pos;

		_Alty_traits::construct(al, _Newwhere, std::forward<Args>(args)...);

		wjr::uninitialized_move_a(_Myfirst, _Where, _Newfirst, al);
		wjr::uninitialized_move_a(_Where, _Mylast, _Newwhere + 1, al);

		tidy();
		moveConstruct(al, std::move(_Newdata), getData());
	}

	template<typename...Args>
	WJR_CONSTEXPR20 void _M_realloc_insert_at_end(Args&&...args) {
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
		const auto __new_size = __old_size + 1;
		const auto _Newcapacity = getGrowthCapacity(__old_size, __new_size);

		auto _Newdata = Allocate(al, __new_size, _Newcapacity);
		const pointer _Newfirst = _Newdata.data();

		const pointer _Newwhere = _Newfirst + __old_size;
		_Alty_traits::construct(al, _Newwhere, std::forward<Args>(args)...);

		wjr::uninitialized_move_n_a(_Myfirst, __old_size, _Newfirst, al);

		tidy();
		moveConstruct(al, std::move(_Newdata), getData());
	}

	WJR_CONSTEXPR20 void _M_fill_insert(iterator _Where, size_type n, const value_type& _Val) {
		if (n == 0) {
			return;
		}

		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();

		const auto __rest = static_cast<size_type>(_Myend - _Mylast);

		if (__rest >= n) {
			temporary_allocator_value<Alloc> _Tmp(al, _Val);
			auto& _Copy = _Tmp.value();
			const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
			if (__elements_after > n) {
				wjr::uninitialized_move_a(_Mylast - n, _Mylast, _Mylast, al);
				wjr::move_backward(_Where, _Mylast - n, _Mylast);
				wjr::fill_n(_Where, n, _Copy);
			}
			else {
				wjr::uninitialized_fill_n_a(_Mylast, n - __elements_after, _Copy, al);
				wjr::uninitialized_move_a(_Where, _Mylast, _Where + n, al);
				wjr::fill(_Where, _Mylast, _Copy);
			}
			set_size(size() + n);
		}
		else {
			const auto _Newcapacity = getGrowthCapacity(capacity(), size() + n);
			auto _Newdata = Allocate(al, size() + n, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();

			const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);

			wjr::uninitialized_fill_n_a(_Newfirst + __old_pos, n, _Val, al);
			wjr::uninitialized_move_a(_Myfirst, _Where, _Newfirst, al);
			wjr::uninitialized_move_a(_Where, _Mylast, _Newfirst + __old_pos + n, al);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	WJR_CONSTEXPR20 void _M_append(size_type n, const value_type& _Val) {
		auto& al = getAllocator();

		const auto _Oldsize = size();
		const auto _Oldcapacity = capacity();

		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + _Oldsize;
		const pointer _Myend = _Myfirst + _Oldcapacity;

		const auto __rest = _Oldcapacity - _Oldsize;
		const auto _Newsize = _Oldsize + n;

		if (__rest >= n) {
			wjr::uninitialized_fill_n_a(_Mylast, n, _Val, al);
			set_size(_Newsize);
		}
		else {
			auto _Newcapacity = getGrowthCapacity(_Oldcapacity, _Newsize);
			auto _Newdata = Allocate(al, _Newsize, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();

			wjr::uninitialized_fill_n_a(_Newfirst + _Oldsize, n, _Val, al);
			wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, al);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	template<typename Args>
	WJR_CONSTEXPR20 void _M_insert_aux(iterator _Where, Args&& args) {
		auto& al = getAllocator();
		const pointer _Mylast = data() + size();

		_Alty_traits::construct(al, _Mylast, std::move(*(_Mylast - 1)));

		wjr::move_backward(_Where, _Mylast - 1, _Mylast);
		*_Where = std::forward<Args>(args);

		set_size(size() + 1);
	}

	template<typename...Args>
	WJR_CONSTEXPR20 iterator _M_emplace_aux(const_iterator _Where, Args&&...args) {
		const auto __offset = _Where - cbegin();

		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();

		if (_Mylast != _Myend) {
			if (_Where == _Mylast) {
				_Alty_traits::construct(al, _Mylast, std::forward<Args>(args)...);
				set_size(size() + 1);
			}
			else {
				temporary_allocator_value<Alloc> _Tmp(al, std::forward<Args>(args)...);
				_M_insert_aux(begin() + __offset, std::move(_Tmp.value()));
			}
		}
		else {
			_M_realloc_insert(begin() + __offset, std::forward<Args>(args)...);
		}

		return begin() + __offset;
	}

};

template<typename iter,
	typename v = typename std::iterator_traits<iter>::value_type,
	typename Alloc = std::allocator<v>,
	typename Core = vector_core<v, Alloc>,
	std::enable_if_t<is_iterator_v<iter>, int> = 0>
vector(iter, iter, Alloc = Alloc()) -> vector<v, Core>;

template<typename T, size_t N, typename Alloc = std::allocator<T>>
using static_vector = vector<T, Alloc, vector_core<T, Alloc, vector_static_data<T, Alloc, N>>>;

template<typename T, typename Alloc, typename Core>
bool operator==(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Alloc, typename Core>
bool operator!=(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return !(lhs == rhs);
}

template<typename T, typename Alloc, typename Core>
bool operator<(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return wjr::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, typename Alloc, typename Core>
bool operator>(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return rhs < lhs;
}

template<typename T, typename Alloc, typename Core>
bool operator<=(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return !(rhs < lhs);
}

template<typename T, typename Alloc, typename Core>
bool operator>=(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return !(lhs < rhs);
}

//template<typename T, typename Alloc, size_t C = 0>
//using sso_vector = vector<T, Alloc, vector_sso_core<T, Alloc, C>>;

_WJR_END

namespace std {
	template<typename T, typename Alloc, typename Core>
	WJR_CONSTEXPR20 void swap(wjr::vector<T, Alloc, Core>& lhs, wjr::vector<T, Alloc, Core>& rhs)
		noexcept(noexcept(lhs.swap(rhs))) {
		lhs.swap(rhs);
	}

	template<typename T, typename Alloc, typename Core>
	WJR_CONSTEXPR20 typename wjr::vector<T, Alloc, Core>::size_type erase(wjr::vector<T, Alloc, Core>& c, const T& value) {
		auto it = std::remove(c.begin(), c.end(), value);
		const auto n = std::distance(it, c.end());
		c.erase(it, c.end());
		return n;
	}

	template<typename T, typename Alloc, typename Core, typename Pred>
	WJR_CONSTEXPR20 typename wjr::vector<T, Alloc, Core>::size_type erase_if(wjr::vector<T, Alloc, Core>& c, Pred pred) {
		auto it = std::remove_if(c.begin(), c.end(), pred);
		const auto n = std::distance(it, c.end());
		c.erase(it, c.end());
		return n;
	}

}

#pragma pop_macro("new")

#endif // !__WJR_VECTOR_H 