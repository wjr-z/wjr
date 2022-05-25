#ifndef WJR_MASM_H
#define WJR_MASM_H

#include <cstdint>

#if defined (_MSC_VER)
#include <intrin.h>
#endif

#include "mtype_traits.h"

namespace wjr {

	template<typename T>
	static unsigned char wjr_qaddcarry(T* result, T lhs, T rhs) {
		*result = lhs + rhs;
		return *result < lhs;
	}

	static unsigned char wjr_addcarry_u32(
        unsigned char c_in, uint32_t in_1, uint32_t in_2, uint32_t* out) {
#if defined (_MSC_VER)
        return _addcarry_u32(c_in, in_1, in_2, out);
#elif defined WJR_GCC_STYLE_ASM
        __asm__ __volatile__(       \
            "add $255 , %[u1]\n"    \
            "mov %[v1], %%eax\n"    \
            "adc %[v2], %%eax\n"    \
            "mov %%eax, %[u2]\n"    \
            "setb       %[u1]\n"    \
            : [u1] "+&q"(c_in), [u2] "+q"(*out)  \
            : [v1] "q"(in_1), [v2] "q"(in_2)     \
        );
        return c_in;
#elif whas_builtin(__builtin_addcl)
        unsigned long CF = 0;
		*out = __builtin_addcl(in_1, in_2, c_in, &CF);
        return CF;
#else
        return wjr_qaddcarry(out, in_1, in_2) | wjr_qaddcarry(out, *out, (uint32_t)c_in);
#endif
	}

    static unsigned char wjr_addcarry_u64(
        unsigned char c_in, uint64_t in_1, uint64_t in_2, uint64_t* out) {
#if defined (_MSC_VER) && (!defined(_M_IX86) || defined(_CHPE_ONLY_))
        return _addcarry_u64(c_in, in_1, in_2, out);
#elif defined WJR_GCC_STYLE_ASM && defined(__x86_64__)
        __asm__ __volatile__(       \
            "add $255 , %[u1]\n"    \
            "mov %[v1], %%rax\n"    \
            "adc %[v2], %%rax\n"    \
            "mov %%rax, %[u2]\n"    \
            "setb       %[u1]\n"    \
            : [u1] "+&q"(c_in), [u2] "+q"(*out)  \
            : [v1] "q"(in_1), [v2] "q"(in_2)     \
        );
        return c_in;
#elif whas_builtin(__builtin_addcll)
		unsigned long long CF = 0;
		*out = __builtin_addcll(in_1, in_2, c_in, &CF);
		return CF;
#else
        return wjr_qaddcarry(out, in_1, in_2) | wjr_qaddcarry(out, *out, (uint64_t)c_in);
#endif
    }

    template<typename T>
    static unsigned char wjr_qsubborrow(T* result, T lhs, T rhs) {
        *result = lhs - rhs;
        return lhs < rhs;
    }

    static unsigned char wjr_subborrow_u32(
        unsigned char c_in, uint32_t in_1, uint32_t in_2, uint32_t* out) {
#if defined (_MSC_VER)
        return _subborrow_u32(c_in, in_1, in_2, out);
#elif defined WJR_GCC_STYLE_ASM
        __asm__ __volatile__(       \
            "add $255 , %[u1]\n"    \
            "mov %[v1], %%eax\n"    \
            "sbb %[v2], %%eax\n"    \
            "mov %%eax, %[u2]\n"    \
            "setb       %[u1]\n"    \
            : [u1] "+&q"(c_in), [u2] "+q"(*out)  \
            : [v1] "q"(in_1), [v2] "q"(in_2)     \
        );
        return c_in;
#elif whas_builtin(__builtin_subcl)
		unsigned long CF = 0;
		*out = __builtin_subcl(in_1, in_2, c_in, &CF);
		return CF;
#else
		return wjr_qsubborrow(out, in_1, in_2) | wjr_qsubborrow(out, *out, (uint32_t)c_in);
#endif
    }

    static unsigned char wjr_subborrow_u64(
        unsigned char c_in, uint64_t in_1, uint64_t in_2, uint64_t* out) {
#if defined (_MSC_VER) && (!defined(_M_IX86) || defined(_CHPE_ONLY_))
		return _subborrow_u64(c_in, in_1, in_2, out);
#elif defined WJR_GCC_STYLE_ASM && defined(__x86_64__)
		__asm__ __volatile__(       \
			"add $255 , %[u1]\n"    \
			"mov %[v1], %%rax\n"    \
			"sbb %[v2], %%rax\n"    \
			"mov %%rax, %[u2]\n"    \
			"setb       %[u1]\n"    \
			: [u1] "+&q"(c_in), [u2] "+q"(*out)  \
			: [v1] "q"(in_1), [v2] "q"(in_2)     \
		);
        return c_in;
#elif whas_builtin(__builtin_subcll)
        unsigned long long CF = 0;
		*out = __builtin_subcll(in_1, in_2, c_in, &CF);
		return CF;
#else
        return wjr_qsubborrow(out, in_1, in_2) | wjr_qsubborrow(out, *out, (uint64_t)c_in);
#endif
    }

}

#endif