
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

#if defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#elif WJR_HAS_ATTRIBUTE(always_inline)
#define WJR_FORCEINLINE __attribute__((always_inline))
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
#ifndef __WJR_TYPE_TRAITS_H
#define __WJR_TYPE_TRAITS_H

#include <functional>
#include <limits>

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
using alloc_pointer_t = typename std::allocator_traits<T>::pointer;

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
#ifndef __WJR_PAIR_H
#define __WJR_PAIR_H

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


#ifndef __WJR_ALGORITHM_H__
#define __WJR_ALGORITHM_H__

#include <algorithm>
#include <memory>
#include <limits>
#include <cstring>

_WJR_BEGIN

template<typename _Iter>
struct __has_fast_uninitialized_default_construct : std::conjunction<
	std::is_default_constructible<remove_ref_t<iter_ref_t<_Iter>>>
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
WJR_CONSTEXPR20 void destroy_at_a(alloc_pointer_t<Alloc> _Where, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (is_default_allocator_destroy_v<Alloc, pointer>) {
		std::destroy_at(_Where);
	}
	else {
		std::allocator_traits<Alloc>::destroy(al, std::addressof(*_Where));
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void destroy_a(alloc_pointer_t<Alloc> _First, alloc_pointer_t<Alloc> _Last, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	using value_type = alloc_value_t<Alloc>;
	if constexpr (!std::conjunction_v<std::is_trivially_destructible<value_type>,
		is_default_allocator_destroy<Alloc, pointer>>) {
		for (; _First != _Last; ++_First) {
			destroy_at_a(_First, al);
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void destroy_n_a(alloc_pointer_t<Alloc> _First, alloc_size_t<Alloc> n, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
	alloc_pointer_t<Alloc> _First, alloc_pointer_t<Alloc> _Last, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
	alloc_pointer_t<Alloc> _First, alloc_size_t<Alloc> _Count, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
	alloc_pointer_t<Alloc> _First, alloc_pointer_t<Alloc> _Last, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_value_construct_n_a(
	alloc_pointer_t<Alloc> _First, alloc_size_t<Alloc> _Count, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_copy_a(
	iter _First, iter _Last, alloc_pointer_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_copy_n_a(
	iter _First, size_t _Count, alloc_pointer_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_move_a(
	iter _First, iter _Last, alloc_pointer_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
WJR_CONSTEXPR20 std::pair<iter, alloc_pointer_t<Alloc>> uninitialized_move_n_a(
	iter _First, size_t _Count, alloc_pointer_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
	alloc_pointer_t<Alloc> _First, alloc_pointer_t<Alloc> _Last,
	const ValueType& _Val, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_fill_n_a(
	alloc_pointer_t<Alloc> _First,
	alloc_size_t<Alloc> _Count,
	const ValueType& _Val, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
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
	static_assert(_Al >= 16, "The alignment must be greater than or equal to 16.");
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
		auto ptr = static_cast<char*>(::operator new(_Count * sizeof(_Ty) + _Off, static_cast<std::align_val_t>(_Al))) + _Off;
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

#ifndef __WJR_VECTOR_H
#define __WJR_VECTOR_H

#pragma push_macro("new")
#undef new

#include <stdexcept>
#include <string>

_WJR_BEGIN

/*
* vector_data ->
* vector_data must be shallow copy in copy constructor and copy assignment operator,
* but deep copy in copy_construct, move_construct can be shallow copy/deep copy
* 1.static vector_data allocate(_Al& al, size_t _Size, size_t _Capacity)
* 2.static void copy_construct(_Al& al, const vector_data& _Src, vector_data& _Dest)
* 3.static void move_construct(_Al& al, vector_data&& _Src, vector_data& _Dest)
* 5.static void swap(_Al& al, vector_data& _Left, vector_data& _Right) [noexcept] [optional]
* 7.static void deallocate(_Al& al, vector_data& _Data)
* 9.static size_t growth_capacity(const size_t _Oldcapacity, const size_t _Newsize) [optinal]
* 10.static void shrink_to_fit(_Al&al, vector_data& _Data) [optional]
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
* 9.static vector_core allocate(_Al& al, size_t _Size, size_t _Capacity);
* 10.static void copy_construct(_Al& al, const vector_data& _Src, vector_data& _Dest)
* 11.static void move_construct(_Al& al, vector_data&& _Src, vector_data& _Dest)
* 14.static void swap(_Al& al, vector_data& _Left, vector_data& _Right) [noexcept]
* 15.static void destroy(_Al& al, vector_data& _Data);
* 16.static void deallocate(_Al& al, vector_data& _Data);
* 17.static void tidy(_Al& al, vector_data& _Data);
* 18.static size_t growth_capacity(const size_t _Oldcapacity, const size_t _Newsize);
* 19.static void shrink_to_fit(_Al&al, vector_data& _Data);
* 20.void tidy() [noexcept];
* 21.void swap(vector_core& other) [noexcept];
* 22.size_t size() const;
* 23.size_t capacity() const;
* 24.[const] _Ty* data() [const]
* 25.const _Ty* cdata() const
* 26._Ty* mutable_data() [TODO]
* 27.void set_size();
* 28.[const] _Al& get_allocator() [const];
* 29.[const] vector_data& get_data() [const];
*/

template<typename T, typename Alloc>
struct vector_data {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
	using value_type = T;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;

	WJR_CONSTEXPR20 static vector_data allocate(_Alty& al, size_type _Size, size_type _Capacity) {
		auto ptr = _Alty_traits::allocate(al, _Capacity);
		return { ptr, ptr + _Size, ptr + _Capacity };
	}

	WJR_CONSTEXPR20 static void copy_construct(_Alty& al, const vector_data& _Src, vector_data& _Dest) {
		auto data = allocate(al, _Src.size(), _Src.capacity());
		wjr::uninitialized_copy_a(_Src._Myfirst, _Src._Mylast, data._Myfirst, al);
		_Dest = data;
	}

	WJR_CONSTEXPR20 static void move_construct(_Alty& al, vector_data&& _Src, vector_data& _Dest) {
		_Dest = _Src;
		_Src._Myfirst = _Src._Mylast = _Src._Myend = nullptr;
	}

	WJR_CONSTEXPR20 static void deallocate(_Alty& al, vector_data& _Data) {
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
	constexpr static size_t _MaxCapacity = ((sizeof(T) * N + _MaxAlignment - 1) & (~(_MaxAlignment - 1))) / sizeof(T);
public:

	WJR_CONSTEXPR20 static vector_static_data allocate(_Alty& al, size_type _Size, size_type _Capacity) {
		std::string str = "vector_static_data is too small to hold the requested data";
		str += "\n old capacity = " + std::to_string(_MaxCapacity);
		str += "\n allocate new size = " + std::to_string(_Size);
		str += "\n allocate new capacity = " + std::to_string(_Capacity);
		throw std::length_error(str);
		unreachable();
		return vector_static_data{};
	}

	WJR_CONSTEXPR20 static void copy_construct(_Alty& al, const vector_static_data& _Src, vector_static_data& _Dest) {
		wjr::uninitialized_copy_n_a(_Src.data(), _Src.size(), _Dest.data(), al);
		_Dest.set_size(_Src.size());
	}

	WJR_CONSTEXPR20 static void move_construct(_Alty& al, vector_static_data&& _Src, vector_static_data& _Dest) {
		auto n = _Src.size();
		if (n != 0) {
			wjr::uninitialized_move_n_a(_Src.data(), n, _Dest.data(), al);
			wjr::destroy_n_a(_Src.data(), n, al);
			_Src.set_size(0);
			_Dest.set_size(n);
		}
	}

	WJR_CONSTEXPR20 static void deallocate(_Alty& al, vector_static_data& _Data) {
		// do nothing
	}

	WJR_CONSTEXPR20 static void shrink_to_fit(_Alty& al, vector_static_data& _Data) {
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

	std::aligned_storage_t<sizeof(T), alignof(T)> _M_storage[_MaxCapacity];
	alignas(_MaxAlignment) size_type _M_size;
};


namespace _Vector_helper {
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(swap, swap);
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(growth_capacity, growth_capacity);
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(shrink_to_fit, shrink_to_fit);
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
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::forward<_Alloc>(al)),
			std::forward_as_tuple()) {
		copy_construct(get_allocator(), other.get_data(), get_data());
	}

	WJR_CONSTEXPR20 vector_core(vector_core&& other) noexcept
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::move(other.get_allocator())),
			std::forward_as_tuple()) {
		move_construct(get_allocator(), std::move(other.get_data()), get_data());
	}

private:
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& al, std::true_type) noexcept
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {
		move_construct(get_allocator(), std::move(other.get_data()), get_data());
	}
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& _Al, std::false_type)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(_Al),
			std::forward_as_tuple()) {
		auto& al = get_allocator();
		if (al == other.get_allocator()) {
			move_construct(al, std::move(other.get_data()), get_data());
		}
		else {
			const auto _Size = other.size();
			if (is_likely(_Size != 0)) {
				if (capacity() < _Size) {
					auto _Data = allocate(al, _Size, _Size);
					move_construct(al, std::move(_Data), get_data());
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

	WJR_CONSTEXPR20 static Data allocate(_Alty& al, size_type _Count, size_type _Capacity) {
		return Data::allocate(al, _Count, _Capacity);
	}

	WJR_CONSTEXPR20 static void copy_construct(_Alty& al, const Data& _Src, Data& _Dest) {
		return Data::copy_construct(al, _Src, _Dest);
	}

	WJR_CONSTEXPR20 static void move_construct(_Alty& al, Data&& _Src, Data& _Dest) {
		return Data::move_construct(al, std::move(_Src), _Dest);
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void swap(_Alty& al, _D& _Left, _D& _Right) {
		return Data::swap(al, _Left, _Right);
	}

	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void swap(_Alty& al, _D& _Left, _D& _Right) {
		_D _Tmp;
		move_construct(al, std::move(_Left), _Tmp);
		move_construct(al, std::move(_Right), _Left);
		move_construct(al, std::move(_Tmp), _Right);
	}

	WJR_CONSTEXPR20 static void destroy(_Alty& al, Data& _Data) noexcept {
		wjr::destroy_n_a(_Data.data(), _Data.size(), al);
	}

	WJR_CONSTEXPR20 static void deallocate(_Alty& al, Data& _Data) noexcept {
		Data::deallocate(al, _Data);
		assume(_Data.size() == 0);
	}

	WJR_CONSTEXPR20 static void tidy(_Alty& al, Data& _Data) noexcept {
		destroy(al, _Data);
		deallocate(al, _Data);
		assume(_Data.size() == 0);
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_growth_capacity_v<_D, const size_type, const size_type>, int> = 0>
	WJR_CONSTEXPR20 static size_type growth_capacity(const size_type _Oldcapacity, const size_type _Newsize) noexcept {
		return Data::growth_capacity(_Oldcapacity, _Newsize);
	}
	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_growth_capacity_v<_D, const size_type, const size_type>, int> = 0>
	WJR_CONSTEXPR20 static size_type growth_capacity(const size_type _Oldcapacity, const size_type _Newsize) noexcept {
		assume(_Newsize > _Oldcapacity);
		const auto _Newcapacity = _Oldcapacity + _Oldcapacity / 2;
		return _Newcapacity < _Newsize ? _Newsize : _Newcapacity;
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_shrink_to_fit_v<_D, _Alty&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void shrink_to_fit(_Alty& al, _D& _Data) noexcept {
		return Data::shrink_to_fit(al, _Data);
	}

	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_shrink_to_fit_v<_D, _Alty&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void shrink_to_fit(_Alty& al, _D& _Data) noexcept {
		auto _Oldsize = _Data.size();
		auto _Oldcapacity = _Data.capacity();
		if (_Oldsize != _Oldcapacity) {
			if (_Oldsize == 0) {
				tidy(al, _Data);
			}
			else {
				auto _Newdata = allocate(al, _Oldsize, _Oldsize);
				wjr::uninitialized_move_n_a(_Data.data(), _Oldsize, _Newdata.data(), al);
				tidy(al, _Data);
				move_construct(al, std::move(_Newdata), _Data);
			}
		}
	}

	WJR_CONSTEXPR20 void tidy() noexcept {
		tidy(get_allocator(), get_data());
	}

	WJR_CONSTEXPR20 void swap(vector_core& other) noexcept {
		Data::swap(get_allocator(), get_data(), other.get_data());
		if constexpr (_Alty_traits::propagate_on_container_swap::value) {
			std::swap(get_allocator(), other.get_allocator());
		}
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return get_data().size();
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return get_data().capacity();
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return get_data().data();
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return get_data().data();
	}

	WJR_CONSTEXPR20 const_pointer cdata() const noexcept {
		return get_data().cdata();
	}

	WJR_CONSTEXPR20 void set_size(size_type _Size) noexcept {
		get_data().set_size(_Size);
	}

	WJR_CONSTEXPR20 _Alty& get_allocator() noexcept {
		return _Myval.first();
	}

	WJR_CONSTEXPR20 const _Alty& get_allocator() const noexcept {
		return _Myval.first();
	}

	WJR_CONSTEXPR20 Data& get_data() noexcept {
		return _Myval.second();
	}

	WJR_CONSTEXPR20 const Data& get_data() const noexcept {
		return _Myval.second();
	}

	wjr::pair<_Alty, Data> _Myval;
};

template<typename T, typename Alloc = std::allocator<T>, typename Core = vector_core<T, Alloc>>
class vector {
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
		: _Myval(al) {}

	WJR_CONSTEXPR20 explicit vector(const size_type _Count, const allocator_type& al = allocator_type())
		: _Myval(al) {
		_M_construct_n(_Count);
	}

	WJR_CONSTEXPR20 vector(size_type _Count, const value_type& _Val, const allocator_type& al = allocator_type())
		: _Myval(al) {
		_M_construct_n(_Count, _Val);
	}

	WJR_CONSTEXPR20 vector(const vector& other)
		: _Myval(other._Myval, _Alty_traits::select_on_container_copy_construction(other.getAllocator())) {}

	WJR_CONSTEXPR20 vector(vector&& other) noexcept = default;

	WJR_CONSTEXPR20 vector(const vector& other, const allocator_type& al)
		: _Myval(other._Myval, al) {}

	WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& al)
		noexcept(std::is_nothrow_constructible_v<core_type, core_type&&, const allocator_type&>)
		: _Myval(std::move(other._Myval), al) {}

	template<typename _Iter, std::enable_if_t<wjr::is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 vector(_Iter _First, _Iter _Last, const allocator_type& al = allocator_type())
		: _Myval(al) {
		_M_range_init(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
	}

	WJR_CONSTEXPR20 vector(std::initializer_list<value_type> _Ilist, const allocator_type& al = allocator_type())
		: _Myval(al) {
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
		return _Myval.size();
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
		core_type::shrink_to_fit(getAllocator(), getData());
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return _Myval.capacity();
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

	WJR_CONSTEXPR20 reference operator[](size_type _Pos) {
		return *(data() + _Pos);
	}

	WJR_CONSTEXPR20 const_reference operator[](size_type _Pos) const {
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

	WJR_CONSTEXPR20 reference front() {
		return *data();
	}

	WJR_CONSTEXPR20 const_reference front() const {
		return *data();
	}

	WJR_CONSTEXPR20 reference back() {
		return *(data() + size() - 1);
	}

	WJR_CONSTEXPR20 const_reference back() const {
		return *(data() + size() - 1);
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return _Myval.data();
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return _Myval.data();
	}

	template<typename...Args>
	WJR_CONSTEXPR20 reference emplace_back(Args&&... args) {
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();
		if (_Mylast != _Myend) {
			_Alty_traits::construct(al, std::addressof(*_Mylast), std::forward<Args>(args)...);
			set_size(size() + 1);
		}
		else {
			_M_realloc_insert_at_end(std::forward<Args>(args)...);
		}
		return back();
	}

	WJR_CONSTEXPR20 void push_back(const value_type& _Val) noexcept {
		emplace_back(_Val);
	}

	WJR_CONSTEXPR20 void push_back(value_type&& _Val) noexcept {
		emplace_back(std::move(_Val));
	}

	WJR_CONSTEXPR20 void pop_back() {
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
		return core_type::allocate(al, _Newsize, _Newcapacity);
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const data_type& _Src, data_type& _Dest) {
		core_type::copy_construct(al, _Src, _Dest);
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, data_type&& _Src, data_type& _Dest) {
		core_type::move_construct(al, std::move(_Src), _Dest);
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, data_type& _Data) {
		core_type::destroy(al, _Data);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, data_type& _Data) noexcept {
		core_type::deallocate(al, _Data);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, data_type& _Data) noexcept {
		core_type::tidy(al, _Data);
	}

	WJR_CONSTEXPR20 static size_type getGrowthCapacity(const size_type _Oldcapacity, size_type _Newsize) noexcept {
		return core_type::growth_capacity(_Oldcapacity, _Newsize);
	}

	WJR_CONSTEXPR20 core_type& getCore() noexcept {
		return _Myval;
	}
	WJR_CONSTEXPR20 const core_type& getCore() const noexcept {
		return _Myval;
	}

	WJR_CONSTEXPR20 _Alty& getAllocator() noexcept {
		return _Myval.get_allocator();
	}
	WJR_CONSTEXPR20 const _Alty& getAllocator() const noexcept {
		return _Myval.get_allocator();
	}

	WJR_CONSTEXPR20 data_type& getData() noexcept {
		return _Myval.get_data();
	}
	WJR_CONSTEXPR20 const data_type& getData() const noexcept {
		return _Myval.get_data();
	}

	WJR_CONSTEXPR20 void set_size(size_type _Newsize) {
		_Myval.set_size(_Newsize);
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
		const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
		set_size(__old_pos);
	}

	WJR_CONSTEXPR20 iterator _M_erase(iterator _Where) noexcept {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		if (_Where + 1 != _Mylast) {
			std::move(_Where + 1, _Mylast, _Where);
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
				std::move(_Last, _Mylast, _First);
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
					std::move_backward(_Where, __old_last - n, __old_last);
					std::copy(_First, _Last, _Where);
				}
				else {
					auto _Mid = _First;
					std::advance(_Mid, __elements_after);
					wjr::uninitialized_copy_a(_Mid, _Last, _Mylast, al);
					wjr::uninitialized_move_a(_Where, __old_last, _Mylast + n - __elements_after, al);
					std::copy(_First, _Mid, _Where);
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
			std::fill(begin(), end(), _Val);
			wjr::uninitialized_fill_n_a(end(), _Count - size(), _Val, al);
			set_size(_Count);
		}
		else {
			_M_erase_at_end(std::fill_n(begin(), _Count, _Val));
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_assign_aux(iter _First, iter _Last, std::forward_iterator_tag) {
		auto _Count = static_cast<size_type>(std::distance(_First, _Last));
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		if (_Count <= size()) {
			std::copy(_First, _Last, _Myfirst);
			_M_erase_at_end(_Myfirst + _Count);
		}
		else if (_Count <= capacity()) {
			auto _Mid = _First;
			std::advance(_Mid, size());
			std::copy(_First, _Mid, begin());
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

		wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, al);

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
				std::move_backward(_Where, _Mylast - n, _Mylast);
				std::fill_n(_Where, n, _Copy);
			}
			else {
				wjr::uninitialized_fill_n_a(_Mylast, n - __elements_after, _Copy, al);
				wjr::uninitialized_move_a(_Where, _Mylast, _Where + n, al);
				std::fill(_Where, _Mylast, _Copy);
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

		std::move_backward(_Where, _Mylast - 1, _Mylast);
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

	core_type _Myval;
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
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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