#ifndef __WJR_PREPROCESSOR_CPP_H
#define __WJR_PREPROCESSOR_CPP_H

#include <wjr/preprocessor/compiler.h>

#if defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC)
#	define WJR_CPP_STANDARD __cplusplus
#elif defined(WJR_COMPILER_MSVC)
#	define WJR_CPP_STANDARD _MSVC_LANG
#endif

#if WJR_CPP_STANDARD >= 199711L
#	define WJR_CPP_03
#endif
#if WJR_CPP_STANDARD >= 201103L
#	define WJR_CPP_11
#endif
#if WJR_CPP_STANDARD >= 201402L
#	define WJR_CPP_14
#endif
#if WJR_CPP_STANDARD >= 201703L
#	define WJR_CPP_17
#endif
#if WJR_CPP_STANDARD >= 202002L
#	define WJR_CPP_20
#endif

#if defined(__cpp_char8_t)
#	define WJR_CHAR8_T
#endif // __cpp_char8_t

#endif // __WJR_PREPROCESSOR_CPP_H