#ifndef WJR_MASM_H
#define WJR_MASM_H

#include <cstdint>
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
        __asm__ __volatile__(\
            "add $255 , %[u1]\n"    \
            "mov %[v1], %%eax\n"    \
            "adc %[v2], %%eax\n"    \
            "mov %%eax, %[u2]\n"    \
            "setb       %[u1]\n"    \
            : [u1] "+&g"(c_in), [u2] "+g"(*out)  \
            : [v1] "g"(in_1), [v2] "g"(in_2)     \
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
        __asm__ __volatile__(\
            "add $255 , %[u1]\n"    \
            "mov %[v1], %%rax\n"    \
            "adc %[v2], %%rax\n"    \
            "mov %%rax, %[u2]\n"    \
            "setb       %[u1]\n"    \
            : [u1] "+&g"(c_in), [u2] "+g"(*out)  \
            : [v1] "g"(in_1), [v2] "g"(in_2)     \
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
        __asm__ __volatile__(\
            "add $255 , %[u1]\n"    \
            "mov %[v1], %%eax\n"    \
            "sbb %[v2], %%eax\n"    \
            "mov %%eax, %[u2]\n"    \
            "setb       %[u1]\n"    \
            : [u1] "+&g"(c_in), [u2] "+g"(*out)  \
            : [v1] "g"(in_1), [v2] "g"(in_2)     \
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
        __asm__ __volatile__(\
            "add $255 , %[u1]\n"    \
            "mov %[v1], %%rax\n"    \
            "sbb %[v2], %%rax\n"    \
            "mov %%rax, %[u2]\n"    \
            "setb       %[u1]\n"    \
            : [u1] "+&g"(c_in), [u2] "+g"(*out)  \
            : [v1] "g"(in_1), [v2] "g"(in_2)     \
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

    static uint32_t wjr_shl_epi32(uint32_t* src, size_t n, int imm) {
        WASSERT_LEVEL_2(imm < 32);
        if (!imm) return 0;
        auto p = src + n;
        uint32_t v = *(p - 1) >> (32 - imm);
#if defined(__AVX512F__)
        size_t c = ((n - 1) & 15) + 1;
        auto g = src + c;
        while (p != g) {
            p -= 16;
            auto a = _mm512_loadu_si512((const __m512i*)p);
            auto b = _mm512_loadu_si512((const __m512i*)(p - 1));
            auto x = _mm512_slli_epi32(a, imm);
            auto y = _mm512_srli_epi32(b, 32 - imm);
            auto z = _mm512_or_si512(x, y);
            _mm512_storeu_si512((__m512i*)p, z);
        }
#elif defined(__AVX2__)
        size_t c = ((n - 1) & 7) + 1;
        auto g = src + c;
        while (p != g) {
            p -= 8;
            auto a = _mm256_loadu_si256((const __m256i*)p);
            auto b = _mm256_loadu_si256((const __m256i*)(p - 1));
            auto x = _mm256_slli_epi32(a, imm);
            auto y = _mm256_srli_epi32(b, 32 - imm);
            auto z = _mm256_or_si256(x, y);
            _mm256_storeu_si256((__m256i*)p, z);
        }
#elif defined(__SSE2__)
        size_t c = ((n - 1) & 3) + 1;
        auto g = src + c;
        while (p != g) {
            p -= 4;
            auto a = _mm_loadu_si128((const __m128i*)p);
            auto b = _mm_loadu_si128((const __m128i*)(p - 1));
            auto x = _mm_slli_epi32(a, imm);
            auto y = _mm_srli_epi32(b, 32 - imm);
            auto z = _mm_or_si128(x, y);
            _mm_storeu_si128((__m128i*)p, z);
        }
#else 
        size_t c = n;
#endif
        while (--c) {
            --p;
            *p = (*p) << imm | (*(p - 1)) >> (32 - imm);
        }
        --p; *p = (*p) << imm;
        return v;
    }

    static uint64_t wjr_shl_epi64(uint64_t* src, size_t n, int imm) {
        WASSERT_LEVEL_2(imm < 64);
        if (!imm) return 0;
        auto p = src + n;
        uint64_t v = *(p - 1) >> (64 - imm);
#if defined(__AVX512F__)
        size_t c = ((n - 1) & 7) + 1;
        auto g = src + c;
        while (p != g) {
            p -= 8;
            auto a = _mm512_loadu_si512((const __m512i*)p);
            auto b = _mm512_loadu_si512((const __m512i*)(p - 1));
            auto x = _mm512_slli_epi64(a, imm);
            auto y = _mm512_srli_epi64(b, 64 - imm);
            auto z = _mm512_or_si512(x, y);
            _mm512_storeu_si512((__m512i*)p, z);
        }
#elif defined(__AVX2__)
        size_t c = ((n - 1) & 3) + 1;
        auto g = src + c;
        while (p != g) {
            p -= 4;
            auto a = _mm256_loadu_si256((const __m256i*)p);
            auto b = _mm256_loadu_si256((const __m256i*)(p - 1));
            auto x = _mm256_slli_epi64(a, imm);
            auto y = _mm256_srli_epi64(b, 64 - imm);
            auto z = _mm256_or_si256(x, y);
            _mm256_storeu_si256((__m256i*)p, z);
        }
#elif defined(__SSE2__)
        size_t c = ((n - 1) & 1) + 1;
        auto g = src + c;
        while (p != g) {
            p -= 2;
            auto a = _mm_loadu_si128((const __m128i*)p);
            auto b = _mm_loadu_si128((const __m128i*)(p - 1));
            auto x = _mm_slli_epi64(a, imm);
            auto y = _mm_srli_epi64(b, 64 - imm);
            auto z = _mm_or_si128(x, y);
            _mm_storeu_si128((__m128i*)p, z);
        }
#else
		size_t c = n;
#endif
        while (--c) {
            --p;
            *p = (*p) << imm | (*(p - 1)) >> (64 - imm);
        }
        --p; *p = (*p) << imm;
        return v;
    }

    static void wjr_shr_epi32(uint32_t* src, size_t n, int imm) {
        WASSERT_LEVEL_2(imm < 32);
        if (!imm) return;
        auto p = src;
#if defined(__AVX512F__)
        size_t c = ((n - 1) & 15) + 1;
        auto g = src + n - c;
        while (p != g) {
            auto a = _mm512_loadu_si512((const __m512i*)p);
            auto b = _mm512_loadu_si512((const __m512i*)(p + 1));
            auto x = _mm512_srli_epi32(a, imm);
            auto y = _mm512_slli_epi32(b, 32 - imm);
            auto z = _mm512_or_si512(x, y);
            _mm512_storeu_si512((__m512i*)p, z);
            p += 16;
        }
#elif defined(__AVX2__)
        size_t c = ((n - 1) & 7) + 1;
        auto g = src + n - c;
        while (p != g) {
            auto a = _mm256_loadu_si256((const __m256i*)p);
            auto b = _mm256_loadu_si256((const __m256i*)(p + 1));
            auto x = _mm256_srli_epi32(a, imm);
            auto y = _mm256_slli_epi32(b, 32 - imm);
            auto z = _mm256_or_si256(x, y);
            _mm256_storeu_si256((__m256i*)p, z);
            p += 8;
        }
#elif defined(__SSE2__)
        size_t c = ((n - 1) & 3) + 1;
        auto g = src + n - c;
        while (p != g) {
            auto a = _mm_loadu_si128((const __m128i*)p);
            auto b = _mm_loadu_si128((const __m128i*)(p + 1));
            auto x = _mm_srli_epi32(a, imm);
            auto y = _mm_slli_epi32(b, 32 - imm);
            auto z = _mm_or_si128(x, y);
            _mm_storeu_si128((__m128i*)p, z);
            p += 4;
        }
#else
        size_t c = n;
#endif
        while (--c) {
            *p = (*p) >> imm | (*(p + 1)) << (32 - imm);
            ++p;
        }
        *p = (*p) >> imm;
    }

    // AVX512F or AVX2 or SSE2
    static void wjr_shr_epi64(uint64_t* src, size_t n, int imm) {
        WASSERT_LEVEL_2(imm < 64);
        if (!imm) return;
        auto p = src;
#if defined(__AVX512F__)
        size_t c = ((n - 1) & 7) + 1;
        auto g = src + n - c;
        while (p != g) {
            auto a = _mm512_loadu_si512((const __m512i*)p);
            auto b = _mm512_loadu_si512((const __m512i*)(p + 1));
            auto x = _mm512_srli_epi64(a, imm);
            auto y = _mm512_slli_epi64(b, 64 - imm);
            auto z = _mm512_or_si512(x, y);
            _mm512_storeu_si512((__m512i*)p, z);
            p += 8;
        }
#elif defined(__AVX2__)
        size_t c = ((n - 1) & 3) + 1;
        auto g = src + n - c;
        while (p != g) {
            auto a = _mm256_loadu_si256((const __m256i*)p);
            auto b = _mm256_loadu_si256((const __m256i*)(p + 1));
            auto x = _mm256_srli_epi64(a, imm);
            auto y = _mm256_slli_epi64(b, 64 - imm);
            auto z = _mm256_or_si256(x, y);
            _mm256_storeu_si256((__m256i*)p, z);
            p += 4;
        }
#elif defined(__SSE2__)
        size_t c = ((n - 1) & 1) + 1;
        auto g = src + n - c;
        while (p != g) {
            auto a = _mm_loadu_si128((const __m128i*)p);
            auto b = _mm_loadu_si128((const __m128i*)(p + 1));
            auto x = _mm_srli_epi64(a, imm);
            auto y = _mm_slli_epi64(b, 64 - imm);
            auto z = _mm_or_si128(x, y);
            _mm_storeu_si128((__m128i*)p, z);
            p += 2;
        }
#else
        size_t c = n;
#endif
        while (--c) {
            *p = (*p) >> imm | (*(p + 1)) << (64 - imm);
            ++p;
        }
        *p = (*p) >> imm;
    }

}

#endif
