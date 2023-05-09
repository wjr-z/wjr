#pragma once
#ifndef __WJR_PREPROCESSOR_COMPILER_H
#define __WJR_PREPROCESSOR_COMPILER_H

#include <wjr/preprocessor/arch.h>

#if defined(__clang__)
#	define WJR_COMPILER_CLANG
#elif defined(__GNUC__)
#	define WJR_COMPILER_GCC
#elif defined(_MSC_VER)
#	define WJR_COMPILER_MSVC
#endif

#if defined(_MSC_VER)
#	define WJR_MSVC
#endif // _MSC_VER

#if defined(__GNUC__)
#	define WJR_HAS_GCC(major, minor, patchlevel)	            \
((__GNUC__ > (major)) ||					                \
	(__GNUC__ == (major) && __GNUC_MINOR__ > (minor)) ||	\
	(__GNUC__ == (major) && __GNUC_MINOR__ == (minor) && __GNUC_PATCHLEVEL__ >= (patchlevel)))
#else
#	define WJR_HAS_GCC(major, minor, patchlevel) 0
#endif // __GNUC__

#if defined(__clang__)
#	define WJR_HAS_CLANG(major, minor, patchlevel)	                    \
((__clang_major__ > (major)) ||					                    \
	(__clang_major__ == (major) && __clang_minor__ > (minor)) ||	\
	(__clang_major__ == (major) && __clang_minor__ == (minor) && __clang_patchlevel__ >= (patchlevel)))
#else
#	define WJR_HAS_CLANG(major, minor, patchlevel) 0
#endif

#if defined(_MSC_VER)
#	define WJR_HAS_MSVC(minor, level)	\
(_MSC_VER >= (minor) * 100 + (level))
#else
#	define WJR_HAS_MSVC(minor, level) 0
#endif

#if (defined(WJR_COMPILER_GCC) && !WJR_HAS_GCC(7, 1, 0)) ||	\
	(defined(WJR_COMPILER_CLANG) && !WJR_HAS_CLANG(5, 0, 0))
#	error "GCC 7.1.0 or Clang 5.0.0 or later is required"
#endif 

#endif // __WJR_PREPROCESSOR_COMPILER_H