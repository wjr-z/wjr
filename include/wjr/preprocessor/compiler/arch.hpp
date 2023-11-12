#ifndef WJR_PREPROCESSOR_COMPILER_ARCH_HPP__
#define WJR_PREPROCESSOR_COMPILER_ARCH_HPP__

#if defined(__pnacl__) || defined(__CLR_VER)
#define WJR_VM
#endif

#if (defined(_M_IX86) || defined(__i386__)) && !defined(WJR_VM)
#define WJR_X86_32
#endif

#if (defined(_M_X64) || defined(__x86_64__)) && !defined(WJR_VM)
#define WJR_X86_64
#endif

#if defined(WJR_X86_32) || defined(WJR_X86_64)
#define WJR_X86
#endif

#if (defined(__arm__) || defined(_M_ARM))
#define WJR_ARM
#endif

#if defined(__aarch64__)
#define WJR_AARCH64
#endif

#if defined(__powerpc64__)
#define WJR_PPC64
#endif

#if !defined(WJR_X86)
#error "ARM is not supported"
#endif

#if defined(WJR_X86_64)
#if defined(__i386__) || defined(_M_IX86) || defined(_X86_)
#define CPU_INTEL
#elif defined(_M_AMD64)
#define CPU_AMD
#endif
#else
#define CPU_UNKNOWN
#endif

#endif // !WJR_PREPROCESSOR_COMPILER_ARCH_HPP__