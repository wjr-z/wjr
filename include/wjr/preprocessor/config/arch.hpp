#ifndef WJR_PREPROCESSOR_CONFIG_ARCH_HPP__
#define WJR_PREPROCESSOR_CONFIG_ARCH_HPP__

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

#endif // !WJR_PREPROCESSOR_CONFIG_ARCH_HPP__