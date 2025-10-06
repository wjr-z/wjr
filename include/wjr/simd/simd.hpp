/**
 * @file simd.hpp
 * @brief SIMD (Single Instruction Multiple Data) operations
 * @author wjr
 *
 * Provides portable SIMD abstractions for x86 SSE/AVX and other architectures.
 * Enables vectorized operations for performance-critical code.
 */

#ifndef WJR_SIMD_SIMD_HPP__
#define WJR_SIMD_SIMD_HPP__

#include <wjr/math/broadcast.hpp>
#include <wjr/memory/detail.hpp>
#include <wjr/simd/detail.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/simd/simd.hpp>
#endif

#endif // WJR_SIMD_SIMD_HPP__