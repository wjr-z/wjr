#ifndef WJR_CONFIG_ATTRIBUTE_HPP__
#define WJR_CONFIG_ATTRIBUTE_HPP__

#include <wjr/config/has.hpp>
#include <wjr/config/platform.hpp>

#if defined(WJR_CPP_20)
    #include <version>
#endif

#if WJR_HAS_CPP_ATTRIBUTE(fallthrough)
    #define WJR_FALLTHROUGH [[fallthrough]]
#elif WJR_HAS_ATTRIBUTE(fallthrough)
    #define WJR_FALLTHROUGH __attribute__((fallthrough))
#elif defined(_MSC_VER) && defined(__fallthrough)
    #define WJR_FALLTHROUGH __fallthrough
#else
    #define WJR_FALLTHROUGH
#endif

#if WJR_HAS_CPP_ATTRIBUTE(noreturn)
    #define WJR_NORETURN [[noreturn]]
#elif WJR_HAS_ATTRIBUTE(noreturn)
    #define WJR_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
    #define WJR_NORETURN __declspec(noreturn)
#else
    #define WJR_NORETURN
#endif

#if WJR_HAS_CPP_ATTRIBUTE(nodiscard)
    #define WJR_NODISCARD [[nodiscard]]
#elif WJR_HAS_ATTRIBUTE(nodiscard)
    #define WJR_NODISCARD __attribute__((nodiscard))
#elif defined(_MSC_VER)
    #define WJR_NODISCARD _Check_return_
#else
    #define WJR_NODISCARD
#endif

#if WJR_HAS_CPP_ATTRIBUTE(deprecated)
    #define WJR_DEPRECATED [[deprecated]]
#elif WJR_HAS_ATTRIBUTE(deprecated)
    #define WJR_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
    #define WJR_DEPRECATED __declspec(deprecated)
#else
    #define WJR_DEPRECATED
#endif

#if WJR_HAS_CPP_ATTRIBUTE(maybe_unused)
    #define WJR_MAYBE_UNUSED [[maybe_unused]]
#elif WJR_HAS_ATTRIBUTE(maybe_unused)
    #define WJR_MAYBE_UNUSED __attribute__((maybe_unused))
#elif defined(_MSC_VER)
    #define WJR_MAYBE_UNUSED
#else
    #define WJR_MAYBE_UNUSED
#endif

#if WJR_HAS_CPP_ATTRIBUTE(likely)
    #define WJR_CPP_LIKELY [[likely]]
#else
    #define WJR_CPP_LIKELY
#endif

#if WJR_HAS_CPP_ATTRIBUTE(unlikely)
    #define WJR_CPP_UNLIKELY [[unlikely]]
#else
    #define WJR_CPP_UNLIKELY
#endif

#if WJR_HAS_ATTRIBUTE(always_inline)
    #define WJR_FORCEINLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define WJR_FORCEINLINE __forceinline
#else
    #define WJR_FORCEINLINE
#endif

#if defined(_MSV_VER)
    #define WJR_SAFEBUFFERS __declspec(safebuffers)
#else
    #define WJR_SAFEBUFFERS
#endif

#if WJR_HAS_ATTRIBUTE(flatten)
    #define WJR_FLATTEN __attribute__((flatten))
#elif WJR_HAS_MSVC(17, 00)
    #define WJR_FLATTEN [[msvc::flatten]]
#else
    #define WJR_FLATTEN
#endif

#if WJR_HAS_FEATURE(FORCEINLINE_LAMBDA)
    #define WJR_FORCEINLINE_LAMBDA WJR_FORCEINLINE
#else
    #define WJR_FORCEINLINE_LAMBDA
#endif

// NOINLINE for MSVC/GCC/CLANG ...
#if WJR_HAS_ATTRIBUTE(noinline)
    #define WJR_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
    #define WJR_NOINLINE __declspec(noinline)
#else
    #define WJR_NOINLINE
#endif

#if WJR_HAS_ATTRIBUTE(hot)
    #define WJR_HOT __attribute__((hot))
#elif defined(_MSC_VER)
    #define WJR_HOT
#else
    #define WJR_HOT
#endif

#if WJR_HAS_ATTRIBUTE(cold)
    #define WJR_COLD __attribute__((cold))
#elif defined(_MSC_VER)
    #define WJR_COLD
#else
    #define WJR_COLD
#endif

#if WJR_HAS_ATTRIBUTE(aligned)
    #define WJR_ALIGNED(size) __attribute__((aligned(size)))
#elif defined(_MSC_VER)
    #define WJR_ALIGNED(size)
#else
    #define WJR_ALIGNED(size)
#endif

#if defined(__cpp_lib_unreachable)
    #define WJR_UNREACHABLE() std::unreachable()
#elif WJR_HAS_BUILTIN(__builtin_unreachable)
    #define WJR_UNREACHABLE() __builtin_unreachable()
#elif defined(WJR_COMPILER_MSVC)
    #define WJR_UNREACHABLE() __assume(0)
#else
    #define WJR_UNREACHABLE()
#endif

#if defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC)
    #define WJR_RESTRICT __restrict
#else
    #define WJR_RESTRICT
#endif

#if defined(WJR_COMPILER_MSVC)
    #define WJR_MS_ABI
    #define WJR_HAS_FEATURE_MS_ABI WJR_HAS_DEF
#elif WJR_HAS_ATTRIBUTE(__ms_abi__)
    #define WJR_MS_ABI __attribute__((__ms_abi__))
    #define WJR_HAS_FEATURE_MS_ABI WJR_HAS_DEF
#elif defined(WJR_ENABLE_ASSEMBLY)
    #undef WJR_ENABLE_ASSEMBLY
#endif

#define WJR_ASSUME_MAY_NOT_PURE(expr)                                                              \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            WJR_UNREACHABLE();                                                                     \
        }                                                                                          \
    } while (0)

#if WJR_HAS_BUILTIN(__builtin_assume)
    #define WJR_ASSUME(expr) __builtin_assume(expr)
#elif defined(WJR_COMPILER_MSVC)
    #define WJR_ASSUME(expr) __assume(expr)
#elif WJR_HAS_CPP_ATTRIBUTE(assume)
    #define WJR_ASSUME(expr) [[assume(expr)]]
#else
    #define WJR_ASSUME(expr) WJR_ASSUME_MAY_NOT_PURE(expr)
#endif

#define WJR_BOOL_EXPR(expr) (!!(expr))

#if WJR_HAS_BUILTIN(__builtin_expect)
    #define WJR_EXPECT(expr, expect) __builtin_expect((expr), (expect))
#else
    #define WJR_EXPECT(expr, expect) (expr)
#endif

#define WJR_LIKELY(expr) WJR_EXPECT(WJR_BOOL_EXPR(expr), true)
#define WJR_UNLIKELY(expr) WJR_EXPECT(WJR_BOOL_EXPR(expr), false)

#define WJR_HAS_FEATURE_IS_CONSTANT_EVALUATED WJR_HAS_DEF

#if WJR_HAS_BUILTIN(__builtin_expect_with_probability)
    #define WJR_EXPECT_WITH_PROBABILITY(exp, c, probability)                                       \
        __builtin_expect_with_probability(exp, c, probability)
#else
    #define WJR_EXPECT_WITH_PROBABILITY(exp, c, probability) (expr)
#endif

#if WJR_HAS_BUILTIN(__builtin_expect_with_probability)
    #define WJR_VERY_LIKELY(exp, probability) WJR_EXPECT_WITH_PROBABILITY(exp, true, probability)
    #define WJR_VERY_UNLIKELY(exp, probability) WJR_EXPECT_WITH_PROBABILITY(exp, false, probability)
#else
    #define WJR_VERY_LIKELY(exp, probability) WJR_LIKELY(exp)
    #define WJR_VERY_UNLIKELY(exp, probability) WJR_UNLIKELY(exp)
#endif

#if defined(__cpp_lib_is_constant_evaluated)
    #define WJR_IS_CONSTANT_EVALUATED() std::is_constant_evaluated()
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated)
    #define WJR_IS_CONSTANT_EVALUATED() __builtin_is_constant_evaluated()
#else
    #define WJR_IS_CONSTANT_EVALUATED() false
    #undef WJR_HAS_FEATURE_IS_CONSTANT_EVALUATED
#endif

/**
 * @details For Clang version 14.0.0-, __builtin_constant_p have some
 * performance issue. \n
 * 1. Cannot propagate __builtin_constant_p beyond function \n
 * 2. Maybe prevent function be inlined. For low version clang, it will inline
 * all path even if only one path will be executed, then the function that calls
 * it will not be inlined.
 */
#if WJR_HAS_BUILTIN(__builtin_constant_p)
    #define WJR_BUILTIN_CONSTANT_P(expr) __builtin_constant_p(expr)
    #define WJR_BUILTIN_CONSTANT_P_TRUE(expr) (WJR_BUILTIN_CONSTANT_P(expr) && (expr))
    #define WJR_BUILTIN_CONSTANT_CONSTEXPR
#else
    #define WJR_BUILTIN_CONSTANT_P(expr) false
    #define WJR_BUILTIN_CONSTANT_P_TRUE(expr) false
    #define WJR_BUILTIN_CONSTANT_CONSTEXPR constexpr
#endif

#if WJR_HAS_BUILTIN(__builtin_clear_padding)
    #define WJR_HAS_BULTIN_CLEAR_PADDING WJR_HAS_DEF
    #define WJR_BUILTIN_CLEAR_PADDING(x) __builtin_clear_padding(x)
#elif WJR_HAS_BUILTIN(__builtin_zero_non_value_bits) ||                                            \
    (defined(WJR_COMPILER_MSVC) && WJR_HAS_MSVC(19, 27))
    #define WJR_HAS_BULTIN_CLEAR_PADDING WJR_HAS_DEF
    #define WJR_BUILTIN_CLEAR_PADDING(x) __builtin_zero_non_value_bits(x)
#endif

#if WJR_HAS_BUILTIN(__builtin_memcpy)
    #define WJR_HAS_BUILTIN_MEMCPY WJR_HAS_DEF
    #define WJR_BUILTIN_MEMCPY_THRESHOLD 256
#endif

#if defined(WJR_FORCEINLINE)
    #define WJR_INTRINSIC_INLINE inline WJR_FORCEINLINE
#else
    #define WJR_INTRINSIC_INLINE inline
#endif

// pure attribute
#if WJR_HAS_ATTRIBUTE(pure)
    #define WJR_PURE __attribute__((pure))
#else
    #define WJR_PURE
#endif

// const attribute
#if WJR_HAS_ATTRIBUTE(const)
    #define WJR_CONST __attribute__((const))
#else
    #define WJR_CONST
#endif

#if defined(__clang__) || defined(__GNUC__)
    #define WJR_ALLOCATOR
    #if WJR_HAS_ATTRIBUTE(malloc)
        #define WJR_MALLOC __attribute__((malloc))
    #else
        #define WJR_MALLOC
    #endif
#elif defined(_MSC_VER)
    #define WJR_ALLOCATOR __declspec(allocator) __declspec(restrict)
    #define WJR_MALLOC
#else
    #define WJR_ALLOCATOR
    #define WJR_MALLOC
#endif

#if WJR_HAS_ATTRIBUTE(nonnull)
    #define WJR_ALL_NONNULL __attribute__((nonnull))
    #define WJR_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#else
    #define WJR_ALL_NONNULL
    #define WJR_NONNULL(...)
#endif

#if WJR_HAS_ATTRIBUTE(returns_nonnull)
    #define WJR_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
    #define WJR_RETURNS_NONNULL
#endif

#if WJR_HAS_ATTRIBUTE(__may_alias__)
    #define WJR_MAY_ALIAS __attribute__((__may_alias__))
#else
    #define WJR_MAY_ALIAS
#endif

#define WJR_INLINE inline

#if defined(WJR_CPP_20)
    #define WJR_CONSTEXPR20 constexpr
#else
    #define WJR_CONSTEXPR20
#endif

#define WJR_INTRINSIC_CONSTEXPR WJR_INTRINSIC_INLINE constexpr
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE WJR_CONSTEXPR20

#define WJR_INLINE_CONSTEXPR inline constexpr
#define WJR_INLINE_CONSTEXPR20 inline WJR_CONSTEXPR20

#if defined(__cpp_consteval)
    #define WJR_CONSTEVAL consteval
#else
    #define WJR_CONSTEVAL constexpr
#endif

#if defined(__cpp_static_call_operator)
    #define WJR_STATIC_CALL_OPERATOR static
    #define WJR_CONST_CALL_OPERATOR
#else
    #define WJR_STATIC_CALL_OPERATOR
    #define WJR_CONST_CALL_OPERATOR const
#endif

#define WJR_ATTRIBUTE(attribute) WJR_ATTRIBUTE_I(attribute)
#define WJR_ATTRIBUTE_I(attribute) WJR_##attribute

#if defined(_MSC_VER)
    #define WJR_EMPTY_BASES __declspec(empty_bases)
#else
    #define WJR_EMPTY_BASES
#endif

#if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_MSVC)
    #define WJR_PRAGMA_I(expr) _Pragma(#expr)
    #define WJR_PRAGMA(expr) WJR_PRAGMA_I(expr)
#else
    #define WJR_PRAGMA(expr)
#endif

#if WJR_HAS_FEATURE(PRAGMA_UNROLL)
    #if defined(WJR_COMPILER_GCC)
        #define WJR_UNROLL(loop) WJR_PRAGMA(GCC unroll(loop))
    #else
        #define WJR_UNROLL(loop) WJR_PRAGMA(unroll(loop))
    #endif
#else
    #define WJR_UNROLL(loop)
#endif

#define WJR_HAS_FEATURE_DISABLE_VECTORIZE WJR_HAS_DEF
#define WJR_HAS_FEATURE_DISABLE_UNROLL WJR_HAS_DEF

#if defined(WJR_COMPILER_CLANG)
    #define WJR_DISABLE_VECTORIZE _Pragma("clang loop vectorize(disable)")
    #define WJR_DISABLE_UNROLL _Pragma("clang loop unroll(disable)")
#elif defined(WJR_COMPILER_GCC)
    #if WJR_HAS_GCC(14, 1, 0)
        #define WJR_DISABLE_VECTORIZE _Pragma("GCC novector")
    #else
        #define WJR_DISABLE_VECTORIZE
        #undef WJR_HAS_FEATURE_DISABLE_VECTORIZE
    #endif

    #define WJR_DISABLE_UNROLL _Pragma("GCC unroll(1)")
#else
    #define WJR_DISABLE_VECTORIZE
    #define WJR_DISABLE_UNROLL WJR_UNROLL(1)

    #undef WJR_HAS_FEATURE_DISABLE_VECTORIZE
    #undef WJR_HAS_FEATURE_DISABLE_UNROLL
#endif

#if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG)
    #define WJR_VISIBILITY(value) __attribute__((__visibility__(value)))
#else
    #define WJR_VISIBILITY(value)
#endif

#if defined(WJR_OS_WINDOWS)
    #if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG)
        #define WJR_SYMBOL_EXPORT __attribute__((__dllexport__))
        #define WJR_SYMBOL_IMPORT __attribute__((__dllimport__))
    #elif defined(WJR_COMPILER_MSVC)
        #define WJR_SYMBOL_EXPORT __declspec(dllexport)
        #define WJR_SYMBOL_IMPORT __declspec(dllimport)
    #else
        #error "Not support"
    #endif
#else
    #if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG)
        #define WJR_SYMBOL_EXPORT __attribute__((__visibility__("default")))
        #define WJR_SYMBOL_IMPORT
    #elif defined(WJR_COMPILER_MSVC)
        #error "Not support"
    #else
        #define WJR_SYMBOL_EXPORT
        #define WJR_SYMBOL_IMPORT
    #endif
#endif

#define WJR_HAS_FEATURE_NO_UNIQUE_ADDRESS WJR_HAS_DEF
#if WJR_HAS_CPP_ATTRIBUTE(no_unique_address)
    #define WJR_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
    #define WJR_NO_UNIQUE_ADDRESS
    #undef WJR_HAS_FEATURE_NO_UNIQUE_ADDRESS
#endif

#endif // WJR_CONFIG_ATTRIBUTE_HPP__