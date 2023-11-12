#ifndef WJR_PREPROCESSOR_COMPILER_COMPILER_H__
#define WJR_PREPROCESSOR_COMPILER_COMPILER_H__

#include <wjr/preprocessor/compiler/has.hpp>
#include <wjr/preprocessor/compiler/arch.hpp>

#if defined(__clang__)
#define WJR_COMPILER_CLANG
#elif defined(__GNUC__)
#define WJR_COMPILER_GCC
#elif defined(_MSC_VER)
#define WJR_COMPILER_MSVC
#endif

#if defined(_MSC_VER)
#define WJR_MSVC
#endif // _MSC_VER

#if defined(__GNUC__)
#define WJR_HAS_GCC(major, minor, patchlevel)                                  \
    ((__GNUC__ > (major)) ||                                                   \
     (__GNUC__ == (major) && __GNUC_MINOR__ > (minor)) ||                      \
     (__GNUC__ == (major) && __GNUC_MINOR__ == (minor) &&                      \
      __GNUC_PATCHLEVEL__ >= (patchlevel)))
#else
#define WJR_HAS_GCC(major, minor, patchlevel) 0
#endif // __GNUC__

#if defined(__clang__)
#define WJR_HAS_CLANG(major, minor, patchlevel)                                \
    ((__clang_major__ > (major)) ||                                            \
     (__clang_major__ == (major) && __clang_minor__ > (minor)) ||              \
     (__clang_major__ == (major) && __clang_minor__ == (minor) &&              \
      __clang_patchlevel__ >= (patchlevel)))
#else
#define WJR_HAS_CLANG(major, minor, patchlevel) 0
#endif

#if defined(_MSC_VER)
#define WJR_HAS_MSVC(minor, level) (_MSC_VER >= (minor)*100 + (level))
#else
#define WJR_HAS_MSVC(minor, level) 0
#endif

#if (defined(WJR_COMPILER_GCC) && !WJR_HAS_GCC(7, 1, 0)) ||                    \
    (defined(WJR_COMPILER_CLANG) && !WJR_HAS_CLANG(5, 0, 0))
#error "GCC 7.1.0 or Clang 5.0.0 or later is required"
#endif

#if defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC)
#define WJR_CPP_STANDARD __cplusplus
#elif defined(WJR_COMPILER_MSVC)
#define WJR_CPP_STANDARD _MSVC_LANG
#endif

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

#if defined(__cpp_char8_t)
#define WJR_CHAR8_T
#endif // __cpp_char8_t

#if defined(WJR_MSVC)
#define WJR_COUNTER __COUNTER__
#else
#define WJR_COUNTER __LINE__
#endif

#if defined(WJR_X86) && (defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG))
#undef WJR_HAS_INLINE_ASM
#define WJR_HAS_INLINE_ASM 1
#endif

#if defined(__SIZEOF_INT128__)
#undef WJR_HAS_INT128
#define WJR_HAS_INT128 1
#if !(defined(__clang__) && defined(LIBDIVIDE_VC))
#undef WJR_HAS_INT128_DIV
#define WJR_HAS_INT128_DIV 1
#endif
#endif

#endif // !WJR_PREPROCESSOR_COMPILER_COMPILER_H__