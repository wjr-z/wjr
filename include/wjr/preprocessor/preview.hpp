#ifndef WJR_PREPROCESSOR_PREVIEW_HPP__
#define WJR_PREPROCESSOR_PREVIEW_HPP__

// testing ...

#include <cstdlib>

#include <wjr/preprocessor/arithmatic.hpp>
#include <wjr/preprocessor/config.hpp>
#include <wjr/preprocessor/detail.hpp>
#include <wjr/preprocessor/logical.hpp>

// Due to the fact that call is not a simple expansion, but takes the previous
// output as the next input, the difficulty of implementing recursion is also
// high.
#include <wjr/preprocessor/queue/algorithm.hpp>

#define WJR_PP_BOOL_IF_NE(expr, t, f) WJR_PP_BOOL_IF(WJR_PP_BOOL(expr), t, f)

// (a, b, c) -> f(a) f(b) f(c)
#define WJR_PP_TRANSFORM_PUT(queue, op) WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

// (a, b, c) -> (f(a), f(b), f(c)) (note : f(x) = (g(x))) -> g(a) g(b) g(c)
#define WJR_PP_TRANSFORM_UNWRAP_PUT(queue, op)                                                     \
    WJR_PP_QUEUE_UNWRAP_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

#define WJR_ATTRIBUTES(...) WJR_PP_TRANSFORM_PUT((__VA_ARGS__), WJR_ATTRIBUTES_CALLER)
#define WJR_ATTRIBUTES_CALLER(x) WJR_ATTRIBUTE(x)

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

#define WJR_HAS_FEATURE_NO_UNIQUE_ADDRESS WJR_HAS_DEF
#if WJR_HAS_CPP_ATTRIBUTE(no_unique_address)
    #define WJR_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
    #define WJR_NO_UNIQUE_ADDRESS
    #undef WJR_HAS_FEATURE_NO_UNIQUE_ADDRESS
#endif

#define WJR_IS_OVERLAP_P(p, pn, q, qn) ((p) + (pn) > (q) && (q) + (qn) > (p))
#define WJR_IS_SEPARATE_P(p, pn, q, qn) (!WJR_IS_OVERLAP_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_SEPARATE_P(p, pn, q, qn) (((p) == (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_INCR_P(p, pn, q, qn) (((p) <= (q)) || ((q) + (qn) <= (p)))
#define WJR_IS_SAME_OR_DECR_P(p, pn, q, qn) (((p) >= (q)) || ((p) + (pn) <= (q)))

#define WJR_ASM_PIC_JMPL(LABEL, TABLE) ".long " #LABEL "-" #TABLE
#define WJR_ASM_NOPIC_JMPL(LABEL) ".quad " #LABEL

#if WJR_HAS_FEATURE(INLINE_ASM_CCCOND)
    #define WJR_ASM_CCSET(c) "/* set condition codes */\n\t"
    #define WJR_ASM_CCOUT(c) "=@cc" #c
#else
    #define WJR_ASM_CCSET(c) "set" #c " %[_cc_" #c "]\n\t"
    #define WJR_ASM_CCOUT(c) [_cc_##c] "=r"
#endif

#if defined(WJR_DISABLE_EXCEPTIONS)
    #define WJR_TRY if constexpr (true)
    #define WJR_CATCH(...) if constexpr (false)
    #define WJR_THROW(X) std::exit(EXIT_FAILURE)
    #define WJR_XTHROW std::exit(EXIT_FAILURE)
    #define WJR_NOEXCEPT_X true
#else
    #define WJR_TRY try
    #define WJR_CATCH(...) catch (__VA_ARGS__)
    #define WJR_THROW(X) throw X
    #define WJR_XTHROW throw
    #define WJR_NOEXCEPT_X false
#endif

#define WJR_REQUIRES(...) std::enable_if_t<(__VA_ARGS__), int> = 0
#define WJR_REQUIRES_I(...) std::enable_if_t<(__VA_ARGS__), int>

#define WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(CLASS)                                                  \
    CLASS() = default;                                                                             \
    CLASS(const CLASS &) = default;                                                                \
    CLASS(CLASS &&) = default;                                                                     \
    CLASS &operator=(const CLASS &) = default;                                                     \
    CLASS &operator=(CLASS &&) = default;                                                          \
    ~CLASS() = default

#if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG)
    #define WJR_VISIBILITY(value) __attribute__((__visibility__(value)))
#else
    #define WJR_VISIBILITY(value)
#endif

#if defined(WJR_OS_WINDOWS)
    #if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG)
        #define WJR_HAS_DECLSPEC
        #define WJR_SYMBOL_EXPORT __attribute__((__dllexport__))
        #define WJR_SYMBOL_IMPORT __attribute__((__dllimport__))
    #elif defined(WJR_COMPILER_MSVC)
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

#endif // ! WJR_PREPROCESSOR_PREVIEW_HPP__