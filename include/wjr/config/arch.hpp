/**
 * @file config/arch.hpp
 * @brief CPU architecture detection
 * @author wjr
 *
 * Detects target CPU architecture (x86, ARM, MIPS, RISC-V, PowerPC, etc.)
 * and provides architecture-specific macros.
 */

#ifndef WJR_CONFIG_ARCH_HPP__
#define WJR_CONFIG_ARCH_HPP__

////////////////////////////////////////////////////////////////////////////////
// Architectures
////////////////////////////////////////////////////////////////////////////////

#if defined(__pnacl__) || defined(__CLR_VER)
    #define WJR_ARCH_VM
#endif

#if (defined(_M_IX86) || defined(__i386__)) && !defined(WJR_ARCH_VM)
    #define WJR_ARCH_X86_32
#endif

#if (defined(_M_X64) || defined(__x86_64__)) && !defined(WJR_ARCH_VM)
    #define WJR_ARCH_X86_64
#endif

#if defined(WJR_ARCH_X86_32) || defined(WJR_ARCH_X86_64)
    #define WJR_ARCH_X86
#endif

#if (defined(__arm__) || defined(_M_ARM))
    #define WJR_ARCH_ARM
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
    #define WJR_ARCH_AARCH64
#endif

#if (defined(WJR_ARCH_AARCH64) || defined(WJR_ARCH_ARM))
    #define WJR_ARCH_ANY_ARM
#endif

#if defined(__powerpc__)
    #define WJR_ARCH_PPC
#endif

#if defined(__powerpc64__)
    #define WJR_ARCH_PPC64
#endif

#if defined(__mips64)
    #define WJR_ARCH_MIPS64
#endif

#if defined(__mips__) && !defined(__mips64) // mips64 also declares __mips__
    #define WJR_ARCH_MIPS32
#endif

#if defined(WJR_ARCH_MIPS32) || defined(WJR_ARCH_MIPS64)
    #define WJR_ARCH_MIPS
#endif

#if defined(__s390x__)
    #define WJR_ARCH_S390X
#endif

#if defined(__riscv)
    #define WJR_ARCH_RISCV
#endif

#if defined(__riscv) && defined(__riscv_xlen) && __riscv_xlen == 32
    #define WJR_ARCH_RISCV32
#endif

#if defined(__riscv) && defined(__riscv_xlen) && __riscv_xlen == 64
    #define WJR_ARCH_RISCV64
#endif

#if defined(__riscv) && defined(__riscv_xlen) && __riscv_xlen == 128
    #define WJR_ARCH_RISCV128
#endif

#if defined(__loongarch64)
    #define WJR_ARCH_LOONGARCH
#endif

////////////////////////////////////////////////////////////////////////////////
// Virtual address bits
////////////////////////////////////////////////////////////////////////////////

// WJR_PTR_BITS: effective virtual address bits used by user-space pointers.
//
// When WJR_HAS_48BIT_PTR is defined, bits 63:48 of any user-space pointer are
// guaranteed to be zero, making those bits available for pointer tagging,
// NaN-boxing, and similar techniques.
//
// Platform notes
// ──────────────
//  x86-64 (4-level paging, default)
//      48-bit VA; bits 63:48 == 0 in user space.  This is the overwhelmingly
//      common case on Windows, Linux, and macOS.
//
//  x86-64 with Intel LA57 / 5-level paging
//      57-bit VA; only bits 63:57 are zero.  Requires explicit kernel + CPU
//      support and is rarely deployed.  Define WJR_PTR_BITS=57 manually when
//      targeting such environments.
//
//  AArch64 (standard, 4K/64K granule, 4-level)
//      48-bit VA; same guarantee as x86-64 default.
//
//  AArch64 + LPA (52-bit extension)
//      Uncommon.  Define WJR_PTR_BITS=52 manually if needed.
//
//  32-bit architectures
//      32-bit VA; no high bits available for tagging.
//
//  RISC-V 64, MIPS64, PowerPC64, S390x, LoongArch
//      VA size is implementation-defined (Sv39/Sv48/Sv57 for RISC-V, etc.).
//      Conservative default: WJR_PTR_BITS=0 (tagging disabled).
//
// To override auto-detection, define WJR_PTR_BITS before including any
// wjr header.
#if !defined(WJR_PTR_BITS)
    #if defined(WJR_ARCH_X86_64)
        // Default: 4-level paging (48-bit).  LA57 requires rare kernel + HW
        // support; override with -DWJR_PTR_BITS=57 if targeting it.
        #define WJR_PTR_BITS 48
    #elif defined(WJR_ARCH_AARCH64)
        // Standard AArch64: 48-bit.  LPA (52-bit) is uncommon; override if needed.
        #define WJR_PTR_BITS 48
    #elif defined(WJR_ARCH_X86_32) || defined(WJR_ARCH_ARM) || defined(WJR_ARCH_MIPS32) ||         \
        defined(WJR_ARCH_RISCV32)
        #define WJR_PTR_BITS 32
    #else
        // RISC-V 64, MIPS64, PPC64, S390x, LoongArch, VM, unknown
        // VA size not determinable at compile time; disable pointer tagging.
        #define WJR_PTR_BITS 0
    #endif
#endif

#if WJR_PTR_BITS == 48
    /// Defined when user-space pointer bits 63:48 are guaranteed zero.
    /// Safe to use those 16 bits for NaN-boxing, pointer tagging, etc.
    #define WJR_HAS_48BIT_PTR
#endif

#endif // WJR_CONFIG_ARCH_HPP__