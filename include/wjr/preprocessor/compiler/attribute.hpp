#ifndef WJR_PREPROCESSOR_COMPILER_ATTRIBUTE_HPP__
#define WJR_PREPROCESSOR_COMPILER_ATTRIBUTE_HPP__

#include <wjr/preprocessor/compiler/compiler.hpp>

#if (defined(WJR_COMPILER_GCC) && WJR_HAS_GCC(10, 1, 0)) ||                    \
    (defined(WJR_COMPILER_CLANG) && WJR_HAS_CLANG(10, 0, 0)) ||                \
    (!defined(WJR_COMPILER_GCC) && !defined(WJR_COMPILER_CLANG) &&             \
     defined(__has_builtin))
#define WJR_HAS_BUILTIN(x) __has_builtin(x)
#else
#define WJR_HAS_BUILTIN(x) 0
#endif

#if defined(__has_include)
#define WJR_HAS_INCLUDE(x) __has_include(x)
#else
#define WJR_HAS_INCLUDE(x) 0
#endif // __has_include

#if defined(__has_attribute)
#define WJR_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define WJR_HAS_ATTRIBUTE(x) 0
#endif

#if defined(__has_cpp_attribute)
#define WJR_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define WJR_HAS_CPP_ATTRIBUTE(x) 0
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

#if WJR_HAS_ATTRIBUTE(always_inline)
#define WJR_FORCEINLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#else
#define WJR_FORCEINLINE
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
#define WJR_UNREACHABLE std::unreachable()
#elif WJR_HAS_BUILTIN(__builtin_unreachable) || WJR_HAS_GCC(7, 1, 0) ||        \
    WJR_HAS_CLANG(5, 0, 0)
#define WJR_UNREACHABLE __builtin_unreachable()
#elif defined(WJR_COMPILER_MSVC)
#define WJR_UNREACHABLE __assume(0)
#else
#define WJR_UNREACHABLE
#endif

#if WJR_HAS_BUILTIN(__builtin_assume)
#define WJR_ASSUME(x) __builtin_assume(x)
#elif defined(WJR_COMPILER_MSVC)
#define WJR_ASSUME(x) __assume(x)
#else
#define WJR_ASSUME(x)                                                          \
    do {                                                                       \
        if (!(x)) {                                                            \
            WJR_UNREACHABLE;                                                   \
        }                                                                      \
    } while (0)
#endif

#define WJR_BOOL_EXPR(expr) (!!(expr))

#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7, 1, 0) ||               \
    WJR_HAS_CLANG(5, 0, 0)
#define WJR_LIKELY(expr) __builtin_expect(WJR_BOOL_EXPR(expr), true)
#else
#define WJR_LIKELY(expr) (expr)
#endif

#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7, 1, 0) ||               \
    WJR_HAS_CLANG(5, 0, 0)
#define WJR_UNLIKELY(expr) __builtin_expect(WJR_BOOL_EXPR(expr), false)
#else
#define WJR_UNLIKELY(expr) (expr)
#endif

#if defined(__cpp_lib_is_constant_evaluated)
#define WJR_IS_CONSTANT_EVALUATED std::is_constant_evaluated()
#undef WJR_HAS_CONSTANT_EVALUATED
#define WJR_HAS_CONSTANT_EVALUATED 1
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated) ||                      \
    WJR_HAS_GCC(9, 1, 0) || WJR_HAS_CLANG(9, 0, 0)
#define WJR_IS_CONSTANT_EVALUATED() __builtin_is_constant_evaluated()
#define WJR_HAS_CONSTANT_EVALUATED 1
#else
#define WJR_IS_CONSTANT_EVALUATED() false
#endif

#if WJR_HAS_BUILTIN(__builtin_constant_p) || WJR_HAS_GCC(7, 1, 0) ||           \
    WJR_HAS_CLANG(5, 0, 0)
#define WJR_BUILTIN_CONSTANT_P(expr) __builtin_constant_p(WJR_BOOL_EXPR(expr))
#undef WJR_HAS_CONSTANT_P
#define WJR_HAS_CONSTANT_P 1
#else
#define WJR_BUILTIN_CONSTANT_P(expr) WJR_IS_CONSTANT_EVALUATED()
#undef WJR_HAS_CONSTANT_P
#define WJR_HAS_CONSTANT_P WJR_HAS_CONSTANT_EVALUATED
#endif

#ifdef WJR_CPP_20
#define WJR_EXPLICIT(expression) explicit(expression)
#define WJR_EXPLICIT20(expreesion) explicit(expression)
#else
#define WJR_EXPLICIT(expression) explicit
#define WJR_EXPLICIT20(expression)
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

#define WJR_INTRINSIC_CONSTEXPR WJR_INTRINSIC_INLINE constexpr
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE WJR_CONSTEXPR20

#define WJR_INLINE_CONSTEXPR inline constexpr
#define WJR_INLINE_CONSTEXPR20 inline WJR_CONSTEXPR20

#define WJR_ATTRIBUTE(attribute) WJR_ATTRIBUTE_I(attribute)
#define WJR_ATTRIBUTE_I(attribute) WJR_##attribute

#endif // WJR_PREPROCESSOR_COMPILER_ATTRIBUTE_HPP__