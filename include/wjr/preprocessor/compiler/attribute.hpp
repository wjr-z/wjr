#ifndef WJR_PREPROCESSOR_COMPILER_ATTRIBUTE_HPP__
#define WJR_PREPROCESSOR_COMPILER_ATTRIBUTE_HPP__

#include <wjr/preprocessor/compiler/has.hpp>

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

#if WJR_HAS_ATTRIBUTE(always_inline)
#define WJR_FORCEINLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#else
#define WJR_FORCEINLINE
#endif

#if WJR_HAS_ATTRIBUTE(FORCEINLINE_LAMBDA)
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

#if WJR_HAS_BUILTIN(__builtin_assume)
#define WJR_ASSUME(expr) __builtin_assume(expr)
#elif defined(WJR_COMPILER_MSVC)
#define WJR_ASSUME(expr) __assume(expr)
#elif WJR_HAS_CPP_ATTRIBUTE(assume)
#define WJR_ASSUME(expr) [[assume(expr)]]
#else
#define WJR_ASSUME(expr)                                                                 \
    do {                                                                                 \
        if (!(expr)) {                                                                   \
            WJR_UNREACHABLE();                                                           \
        }                                                                                \
    } while (0)
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
#define WJR_EXPECT_WITH_PROBABILITY(exp, c, probability)                                 \
    __builtin_expect_with_probability(exp, c, probability)
#else
#define WJR_EXPECT_WITH_PROBABILITY(exp, c, probability) (expr)
#endif

#if WJR_HAS_BUILTIN(__builtin_expect_with_probability)
#define WJR_VERY_LIKELY(exp, probability)                                                \
    WJR_EXPECT_WITH_PROBABILITY(exp, true, probability)
#define WJR_VERY_UNLIKELY(exp, probability)                                              \
    WJR_EXPECT_WITH_PROBABILITY(exp, false, probability)
#else
#define WJR_VERY_LIKELY(exp, probability) WJR_LIKELY((exp))
#define WJR_VERY_UNLIKELY(exp, probability) WJR_UNLIKELY((exp))
#endif

#if defined(__cpp_lib_is_constant_evaluated)
#define WJR_IS_CONSTANT_EVALUATED() std::is_constant_evaluated()
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated)
#define WJR_IS_CONSTANT_EVALUATED() __builtin_is_constant_evaluated()
#else
#define WJR_IS_CONSTANT_EVALUATED() false
#undef WJR_HAS_FEATURE_IS_CONSTANT_EVALUATED
#endif

#if WJR_HAS_BUILTIN(__builtin_constant_p)
#define WJR_BUILTIN_CONSTANT_P(expr) __builtin_constant_p(expr)
#else
#define WJR_BUILTIN_CONSTANT_P(expr) false
#endif

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_COMPILER_BARRIER() asm volatile("" ::: "memory")
#else
#define WJR_COMPILER_BARRIER()
#endif

#define WJR_CONSTEXPR_COMPILER_BARRIER()                                                 \
    do {                                                                                 \
        if (!(WJR_IS_CONSTANT_EVALUATED())) {                                            \
            WJR_COMPILER_BARRIER();                                                      \
        }                                                                                \
    } while (0)

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

#if WJR_HAS_ATTRIBUTE(malloc)
#define WJR_MALLOC __attribute__((malloc))
#else
#define WJR_MALLOC
#endif

#define WJR_INLINE inline
#define WJR_CONSTEXPR constexpr

#if defined(WJR_CPP_20)
#define WJR_CONSTEXPR20 constexpr
#else
#define WJR_CONSTEXPR20
#endif

#if WJR_HAS_FEATURE(IS_CONSTANT_EVALUATED)
#define WJR_CONSTEXPR_E constexpr
#else
#define WJR_CONSTEXPR_E
#endif

#define WJR_INTRINSIC_CONSTEXPR WJR_INTRINSIC_INLINE constexpr
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE WJR_CONSTEXPR20
#define WJR_INTRINSIC_CONSTEXPR_E WJR_INTRINSIC_INLINE WJR_CONSTEXPR_E

#define WJR_INLINE_CONSTEXPR inline constexpr
#define WJR_INLINE_CONSTEXPR20 inline WJR_CONSTEXPR20
#define WJR_INLINE_CONSTEXPR_E inline WJR_CONSTEXPR_E

#define WJR_ATTRIBUTE(attribute) WJR_ATTRIBUTE_I(attribute)
#define WJR_ATTRIBUTE_I(attribute) WJR_##attribute

#if defined(_MSC_VER)
#define WJR_EMPTY_BASES __declspec(empty_bases)
#else
#define WJR_EMPTY_BASES
#endif

#endif // WJR_PREPROCESSOR_COMPILER_ATTRIBUTE_HPP__