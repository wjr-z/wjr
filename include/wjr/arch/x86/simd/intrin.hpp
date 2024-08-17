#ifndef WJR_X86_SIMD_INTRIN_HPP__
#define WJR_X86_SIMD_INTRIN_HPP__

#include <wjr/preprocessor.hpp>

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__)
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#endif

#endif // WJR_X86_SIMD_INTRIN_HPP__