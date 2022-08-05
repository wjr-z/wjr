#ifndef WJR_MASM_H
#define WJR_MASM_H

#include <cstdint>
#include "mmacro.h"

namespace wjr {

    namespace math {

        template<typename T, std::enable_if_t<
            std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>, int> = 0>
        constexpr unsigned char add(T* result, T lhs, T rhs) {
            *result = lhs + rhs;
            return *result < lhs;
        }

        WJR_CONSTEXPR unsigned char adc(
            unsigned char c_in,
            uint32_t* out,
            uint32_t in_1,
            uint32_t in_2
        ) {
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined (_MSC_VER)
                return _addcarry_u32(c_in, in_1, in_2, out);
#elif WJR_HAS_BUILTIN(__builtin_addc)
                unsigned int CF;
            *out = __builtin_addc(in_1, in_2, c_in, &CF);
            return CF;
#elif defined WJR_GCC_STYLE_ASM
                __asm__(\
                    "add $0xFF , %[u1]\n"    \
                    "adc %[v1], %[u2]\n"    \
                    "mov %[u2], (%[v2])\n"  \
                    "setb       %[u1]\n"    \
                    : [u1] "+r"(c_in), [u2] "+%r"(in_1)   \
                    : [v1] "r"(in_2), [v2] "r"(out)      \
                );
            return c_in;
#endif
            WJR_IS_NOT_CONSTANT_EVALUATED_END
                return add(out, in_1, in_2) | add(out, *out, static_cast<uint32_t>(c_in));
        }

        WJR_CONSTEXPR unsigned char add(
            uint32_t* out,
            const uint32_t* in_1,
            const uint32_t* in_2,
            size_t n
        ) {
            WASSERT_LEVEL_2(!((in_1 < out&& out < in_1 + n) || (in_2 < out&& out < in_2 + n)));
            // can't handle this situation, it is necessary to
            // manually ensure that this situation does not exist

            unsigned char c_in = 0;
            size_t i = 0;
            size_t r = n & 3;
            while (i != r) {
                c_in = adc(c_in, out, *in_1, *in_2);
                ++in_1;
                ++in_2;
                ++out;
                ++i;
            }
#if !WJR_HAS_BUILTIN(__builtin_addc) && defined(WJR_GCC_STYLE_ASM)
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                uint32_t temp;
            while (i != n) {
                __asm__(\
                    "add $0xFF, %[u1]       \n"   \
                    "mov (%[v1]), %[u2]     \n"   \
                    "adc (%[v2]), %[u2]     \n"   \
                    "mov %[u2], (%[v3])     \n"   \
                    "mov 0x4(%[v1]), %[u2]  \n"   \
                    "adc 0x4(%[v2]), %[u2]  \n"   \
                    "mov %[u2], 0x4(%[v3])  \n"   \
                    "mov 0x08(%[v1]), %[u2] \n"   \
                    "adc 0x08(%[v2]), %[u2] \n"   \
                    "mov %[u2], 0x08(%[v3]) \n"   \
                    "mov 0x0C(%[v1]), %[u2] \n"   \
                    "adc 0x0C(%[v2]), %[u2] \n"   \
                    "mov %[u2], 0x0C(%[v3]) \n"   \
                    "setb %[u1]             \n"   \
                    : [u1] "+r"(c_in), [u2] "=r"(temp)                 \
                    : [v1] "%r"(in_1), [v2] "r"(in_2), [v3] "r"(out)    \
                );
                in_1 += 4;
                in_2 += 4;
                out += 4;
                i += 4;
            }
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#endif
                while (i != n) {
                    c_in = adc(c_in, out + 0, in_1[0], in_2[0]);
                    c_in = adc(c_in, out + 1, in_1[1], in_2[1]);
                    c_in = adc(c_in, out + 2, in_1[2], in_2[2]);
                    c_in = adc(c_in, out + 3, in_1[3], in_2[3]);
                    in_1 += 4;
                    in_2 += 4;
                    out += 4;
                    i += 4;
                }
            return c_in;
        }

        WJR_CONSTEXPR unsigned char adc(
            unsigned char c_in,
            uint64_t* out,
            uint64_t in_1,
            uint64_t in_2
        ) {
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined (_MSC_VER) && defined(WJR_X64)
                return _addcarry_u64(c_in, in_1, in_2, out);
#elif WJR_HAS_BUILTIN(__builtin_addcll)
                unsigned long long CF = 0;
            *out = __builtin_addcll(in_1, in_2, c_in, &CF);
            return CF;
#elif defined WJR_GCC_STYLE_ASM && defined(WJR_X64)
                __asm__(\
                    "add $0xFF , %[u1]\n"    \
                    "adc %[v1], %[u2]\n"    \
                    "mov %[u2], (%[v2])\n"  \
                    "setb       %[u1]\n"    \
                    : [u1] "+r"(c_in), [u2] "+%r"(in_1)     \
                    : [v1] "r"(in_2), [v2] "r"(out)        \
                );
            return c_in;
#endif
            WJR_IS_NOT_CONSTANT_EVALUATED_END
                return add(out, in_1, in_2) | add(out, *out, static_cast<uint64_t>(c_in));
        }

        WJR_CONSTEXPR unsigned char add(
            uint64_t* out,
            const uint64_t* in_1,
            const uint64_t* in_2,
            size_t n
        ) {
            WASSERT_LEVEL_2(!((in_1 < out&& out < in_1 + n) || (in_2 < out&& out < in_2 + n)));

            uint64_t cf = 0;
            size_t i = 0;
            size_t r = n & 3;
            for (; i != r; ++i) {
                cf = adc(cf, out + i, in_1[i], in_2[i]);
            }

#if !WJR_HAS_BUILTIN(__builtin_addcll) && defined(WJR_GCC_STYLE_ASM) && defined(WJR_X64)
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                uint64_t temp;
                for (; i != n; i += 4) {
                    __asm__(
                        "addb $0xFF, %%bl                 \n"
                        "mov (%[v1], %[v4], 8), %[u1]     \n"
                        "mov (%[v2], %[v4], 8), %[u2]     \n"
                        "adc %[u2], %[u1]                 \n"
                        "mov %[u1], (%[v3], %[v4], 8)     \n"
                        "mov 0x8(%[v1], %[v4], 8), %[u1]  \n"
                        "mov 0x8(%[v2], %[v4], 8), %[u2]  \n"
                        "adc %[u2], %[u1]                 \n"
                        "mov %[u1], 0x8(%[v3], %[v4], 8)  \n"
                        "mov 0x10(%[v1], %[v4], 8), %[u1]  \n"
                        "mov 0x10(%[v2], %[v4], 8), %[u2]  \n"
                        "adc %[u2], %[u1]                 \n"
                        "mov %[u1], 0x10(%[v3], %[v4], 8)  \n"
                        "mov 0x18(%[v1], %[v4], 8), %[u1]  \n"
                        "mov 0x18(%[v2], %[v4], 8), %[u2]  \n"
                        "adc %[u2], %[u1]                 \n"
                        "mov %[u1], 0x18(%[v3], %[v4], 8)  \n"
                        "setb %%bl                        \n"
                        : [u1] "+b"(cf), [u2] "=r"(temp)
                        : [v1] "%r"(in_1), [v2] "r"(in_2), [v3] "r"(out), [v4] "r"(i));
        }
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#endif
                for (; i != n; i += 4) {
                    cf = adc(cf, out + i + 0, in_1[i + 0], in_2[i + 0]);
                    cf = adc(cf, out + i + 1, in_1[i + 1], in_2[i + 1]);
                    cf = adc(cf, out + i + 2, in_1[i + 2], in_2[i + 2]);
                    cf = adc(cf, out + i + 3, in_1[i + 3], in_2[i + 3]);
                }
            return cf;
        }

        template<typename T, std::enable_if_t<
            std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>, int> = 0>
        constexpr unsigned char sub(T* result, T lhs, T rhs) {
            *result = lhs - rhs;
            return lhs < rhs;
        }

        WJR_CONSTEXPR unsigned char sbb(
            unsigned char c_in,
            uint32_t* out,
            uint32_t in_1,
            uint32_t in_2
        ) {
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined (_MSC_VER)
                return _subborrow_u32(c_in, in_1, in_2, out);
#elif WJR_HAS_BUILTIN(__builtin_subc)
                unsigned int CF = 0;
            *out = __builtin_subc(in_1, in_2, c_in, &CF);
            return CF;
#elif defined WJR_GCC_STYLE_ASM
                __asm__(\
                    "add $0xFF , %[u1]\n"   \
                    "sbb %[v1], %[u2]\n"    \
                    "mov %[u2], (%[v2])\n"  \
                    "setb       %[u1]\n"    \
                    : [u1] "+r"(c_in), [u2] "+%r"(in_1) \
                    : [v1] "r"(in_2), [v2] "r"(out)     \
                );
            return c_in;
#endif
            WJR_IS_NOT_CONSTANT_EVALUATED_END
                return sub(out, in_1, in_2) | sub(out, *out, static_cast<uint32_t>(c_in));
        }

        WJR_CONSTEXPR unsigned char sub(
            uint32_t* out,
            const uint32_t* in_1,
            const uint32_t* in_2,
            size_t n
        ) {
            WASSERT_LEVEL_2(!((in_1 < out&& out < in_1 + n) || (in_2 < out&& out < in_2 + n)));

            unsigned char c_in = 0;
            size_t i = 0;
            size_t r = n & 3;
            while (i != r) {
                c_in = sbb(c_in, out, *in_1, *in_2);
                ++in_1;
                ++in_2;
                ++out;
                ++i;
            }
#if !WJR_HAS_BUILTIN(__builtin_subc) && defined(WJR_GCC_STYLE_ASM)
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                uint32_t temp;
            while (i != n) {
                __asm__(\
                    "add $0xFF, %[u1]       \n"   \
                    "mov (%[v1]), %[u2]     \n"   \
                    "sbb (%[v2]), %[u2]     \n"   \
                    "mov %[u2], (%[v3])     \n"   \
                    "mov 0x04(%[v1]), %[u2] \n"   \
                    "sbb 0x04(%[v2]), %[u2] \n"   \
                    "mov %[u2], 0x04(%[v3]) \n"   \
                    "mov 0x08(%[v1]), %[u2] \n"   \
                    "sbb 0x08(%[v2]), %[u2] \n"   \
                    "mov %[u2], 0x08(%[v3]) \n"   \
                    "mov 0x0C(%[v1]), %[u2] \n"   \
                    "sbb 0x0C(%[v2]), %[u2] \n"   \
                    "mov %[u2], 0x0C(%[v3]) \n"   \
                    "setb %[u1]             \n"   \
                    : [u1] "+r"(c_in), [u2] "=r"(temp)                 \
                    : [v1] "%r"(in_1), [v2] "r"(in_2), [v3] "r"(out)    \
                );
                in_1 += 4;
                in_2 += 4;
                out += 4;
                i += 4;
            }
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#endif
                while (i != n) {
                    c_in = sbb(c_in, out + 0, in_1[0], in_2[0]);
                    c_in = sbb(c_in, out + 1, in_1[1], in_2[1]);
                    c_in = sbb(c_in, out + 2, in_1[2], in_2[2]);
                    c_in = sbb(c_in, out + 3, in_1[3], in_2[3]);
                    in_1 += 4;
                    in_2 += 4;
                    out += 4;
                    i += 4;
                }
            return c_in;
        }

        WJR_CONSTEXPR unsigned char sbb(
            unsigned char c_in,
            uint64_t* out,
            uint64_t in_1,
            uint64_t in_2
        ) {
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined (_MSC_VER) && (!defined(_M_IX86) || defined(_CHPE_ONLY_))
                return _subborrow_u64(c_in, in_1, in_2, out);
#elif WJR_HAS_BUILTIN(__builtin_subcll)
                unsigned long long CF = 0;
            *out = __builtin_subcll(in_1, in_2, c_in, &CF);
            return CF;
#elif defined WJR_GCC_STYLE_ASM && defined(WJR_X64)
                __asm__(\
                    "add $0xFF , %[u1]\n"   \
                    "sbb %[v1], %[u2]\n"    \
                    "mov %[u2], (%[v2])\n"  \
                    "setb       %[u1]\n"    \
                    : [u1] "+r"(c_in), [u2] "+%r"(in_1) \
                    : [v1] "r"(in_2), [v2] "r"(out)     \
                );
            return c_in;
#endif
            WJR_IS_NOT_CONSTANT_EVALUATED_END
                return sub(out, in_1, in_2) | sub(out, *out, static_cast<uint64_t>(c_in));
        }

        WJR_CONSTEXPR unsigned char sub(
            uint64_t* out,
            const uint64_t* in_1,
            const uint64_t* in_2,
            size_t n
        ) {
            WASSERT_LEVEL_2(!((in_1 < out&& out < in_1 + n) || (in_2 < out&& out < in_2 + n)));

            uint64_t cf = 0;
            size_t i = 0;
            size_t r = n & 3;
            for (; i != r; ++i) {
                cf = sbb(cf, out + i, in_1[i], in_2[i]);
            }
#if !WJR_HAS_BUILTIN(__builtin_subcll) && defined(WJR_GCC_STYLE_ASM) && defined(WJR_X64)
            WJR_IS_CONSTANT_EVALUATED_BEGIN
                for (; i != n; i += 4) {
                    __asm__(\
                        "addb $0xFF, %%bl                 \n"   \
                        "mov (%[v1], %[v4], 8), %[u1]     \n"   \
                        "sbb (%[v2], %[v4], 8), %[u1]     \n"   \
                        "mov %[u1], (%[v3], %[v4], 8)     \n"   \
                        "mov 0x8(%[v1], %[v4], 8), %[u1]  \n"   \
                        "sbb 0x8(%[v2], %[v4], 8), %[u1]  \n"   \
                        "mov %[u1], 0x8(%[v3], %[v4], 8)  \n"   \
                        "mov 0x10(%[v1], %[v4], 8), %[u1] \n"   \
                        "sbb 0x10(%[v2], %[v4], 8), %[u1] \n"   \
                        "mov %[u1], 0x10(%[v3], %[v4], 8) \n"   \
                        "mov 0x18(%[v1], %[v4], 8), %[u1] \n"   \
                        "sbb 0x18(%[v2], %[v4], 8), %[u1] \n"   \
                        "mov %[u1], 0x18(%[v3], %[v4], 8) \n"   \
                        "setb %%bl                        \n"   \
                        : [u1] "+b"(cf)                                                 \
                        : [v1] "%r"(in_1), [v2] "r"(in_2), [v3] "r"(out), [v4] "r"(i)   \
                    );
                }
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#endif
                for (; i != n; i += 4) {
                    cf = sbb(cf, out + i + 0, in_1[i + 0], in_2[i + 0]);
                    cf = sbb(cf, out + i + 1, in_1[i + 1], in_2[i + 1]);
                    cf = sbb(cf, out + i + 2, in_1[i + 2], in_2[i + 2]);
                    cf = sbb(cf, out + i + 3, in_1[i + 3], in_2[i + 3]);
                }
            return cf;
        }

        WJR_CONSTEXPR uint32_t shl(uint32_t* src, size_t n, int imm) {
            WASSERT_LEVEL_2(imm < 32);
            if (!imm) return 0;
            auto p = src + n;
            uint32_t v = *(p - 1) >> (32 - imm);
            size_t c = n;
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(__AVX512F__)
                c = ((n - 1) & 15) + 1;
            auto r = src + c;
            while (p != r) {
                p -= 16;
                auto a = _mm512_loadu_si512((const __m512i*)p);
                auto b = _mm512_loadu_si512((const __m512i*)(p - 1));
                auto x = _mm512_slli_epi32(a, imm);
                auto y = _mm512_srli_epi32(b, 32 - imm);
                auto z = _mm512_or_si512(x, y);
                _mm512_storeu_si512((__m512i*)p, z);
            }
#elif defined(__AVX2__)
                c = ((n - 1) & 7) + 1;
            auto r = src + c;
            while (p != r) {
                p -= 8;
                auto a = _mm256_loadu_si256((const __m256i*)p);
                auto b = _mm256_loadu_si256((const __m256i*)(p - 1));
                auto x = _mm256_slli_epi32(a, imm);
                auto y = _mm256_srli_epi32(b, 32 - imm);
                auto z = _mm256_or_si256(x, y);
                _mm256_storeu_si256((__m256i*)p, z);
            }
#elif defined(__SSE2__)
                c = ((n - 1) & 3) + 1;
            auto r = src + c;
            while (p != r) {
                p -= 4;
                auto a = _mm_loadu_si128((const __m128i*)p);
                auto b = _mm_loadu_si128((const __m128i*)(p - 1));
                auto x = _mm_slli_epi32(a, imm);
                auto y = _mm_srli_epi32(b, 32 - imm);
                auto z = _mm_or_si128(x, y);
                _mm_storeu_si128((__m128i*)p, z);
            }
#endif
            WJR_IS_NOT_CONSTANT_EVALUATED_END
                while (--c) {
                    --p;
                    *p = (*p) << imm | (*(p - 1)) >> (32 - imm);
                }
            --p; *p = (*p) << imm;
            return v;
        }

        WJR_CONSTEXPR uint64_t shl(uint64_t* src, size_t n, int imm) {
            WASSERT_LEVEL_2(imm < 64);
            if (!imm) return 0;
            auto p = src + n;
            uint64_t v = *(p - 1) >> (64 - imm);
            size_t c = n;
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(__AVX512F__)
                c = ((n - 1) & 7) + 1;
            auto r = src + c;
            while (p != r) {
                p -= 8;
                auto a = _mm512_loadu_si512((const __m512i*)p);
                auto b = _mm512_loadu_si512((const __m512i*)(p - 1));
                auto x = _mm512_slli_epi64(a, imm);
                auto y = _mm512_srli_epi64(b, 64 - imm);
                auto z = _mm512_or_si512(x, y);
                _mm512_storeu_si512((__m512i*)p, z);
            }
#elif defined(__AVX2__)
                c = ((n - 1) & 3) + 1;
            auto r = src + c;
            while (p != r) {
                p -= 4;
                auto a = _mm256_loadu_si256((const __m256i*)p);
                auto b = _mm256_loadu_si256((const __m256i*)(p - 1));
                auto x = _mm256_slli_epi64(a, imm);
                auto y = _mm256_srli_epi64(b, 64 - imm);
                auto z = _mm256_or_si256(x, y);
                _mm256_storeu_si256((__m256i*)p, z);
            }
#elif defined(__SSE2__)
                c = ((n - 1) & 1) + 1;
            auto r = src + c;
            while (p != r) {
                p -= 2;
                auto a = _mm_loadu_si128((const __m128i*)p);
                auto b = _mm_loadu_si128((const __m128i*)(p - 1));
                auto x = _mm_slli_epi64(a, imm);
                auto y = _mm_srli_epi64(b, 64 - imm);
                auto z = _mm_or_si128(x, y);
                _mm_storeu_si128((__m128i*)p, z);
            }
#endif
            WJR_IS_NOT_CONSTANT_EVALUATED_END
                while (--c) {
                    --p;
                    *p = (*p) << imm | (*(p - 1)) >> (64 - imm);
                }
            --p; *p = (*p) << imm;
            return v;
        }

        WJR_CONSTEXPR void shr(uint32_t* src, size_t n, int imm) {
            WASSERT_LEVEL_2(imm < 32);
            if (!imm) return;
            auto p = src;
            size_t c = n;
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(__AVX512F__)
                c = ((n - 1) & 15) + 1;
            auto r = src + n - c;
            while (p != r) {
                auto a = _mm512_loadu_si512((const __m512i*)p);
                auto b = _mm512_loadu_si512((const __m512i*)(p + 1));
                auto x = _mm512_srli_epi32(a, imm);
                auto y = _mm512_slli_epi32(b, 32 - imm);
                auto z = _mm512_or_si512(x, y);
                _mm512_storeu_si512((__m512i*)p, z);
                p += 16;
            }
#elif defined(__AVX2__)
                c = ((n - 1) & 7) + 1;
            auto r = src + n - c;
            while (p != r) {
                auto a = _mm256_loadu_si256((const __m256i*)p);
                auto b = _mm256_loadu_si256((const __m256i*)(p + 1));
                auto x = _mm256_srli_epi32(a, imm);
                auto y = _mm256_slli_epi32(b, 32 - imm);
                auto z = _mm256_or_si256(x, y);
                _mm256_storeu_si256((__m256i*)p, z);
                p += 8;
            }
#elif defined(__SSE2__)
                c = ((n - 1) & 3) + 1;
            auto r = src + n - c;
            while (p != r) {
                auto a = _mm_loadu_si128((const __m128i*)p);
                auto b = _mm_loadu_si128((const __m128i*)(p + 1));
                auto x = _mm_srli_epi32(a, imm);
                auto y = _mm_slli_epi32(b, 32 - imm);
                auto z = _mm_or_si128(x, y);
                _mm_storeu_si128((__m128i*)p, z);
                p += 4;
            }
#endif
            WJR_IS_NOT_CONSTANT_EVALUATED_END
                while (--c) {
                    *p = (*p) >> imm | (*(p + 1)) << (32 - imm);
                    ++p;
                }
            *p = (*p) >> imm;
        }

        // AVX512F or AVX2 or SSE2
        WJR_CONSTEXPR void shr(uint64_t* src, size_t n, int imm) {
            WASSERT_LEVEL_2(imm < 64);
            if (!imm) return;
            auto p = src;
            size_t c = n;
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(__AVX512F__)
                c = ((n - 1) & 7) + 1;
            auto r = src + n - c;
            while (p != r) {
                auto a = _mm512_loadu_si512((const __m512i*)p);
                auto b = _mm512_loadu_si512((const __m512i*)(p + 1));
                auto x = _mm512_srli_epi64(a, imm);
                auto y = _mm512_slli_epi64(b, 64 - imm);
                auto z = _mm512_or_si512(x, y);
                _mm512_storeu_si512((__m512i*)p, z);
                p += 8;
            }
#elif defined(__AVX2__)
                c = ((n - 1) & 3) + 1;
            auto r = src + n - c;
            while (p != r) {
                auto a = _mm256_loadu_si256((const __m256i*)p);
                auto b = _mm256_loadu_si256((const __m256i*)(p + 1));
                auto x = _mm256_srli_epi64(a, imm);
                auto y = _mm256_slli_epi64(b, 64 - imm);
                auto z = _mm256_or_si256(x, y);
                _mm256_storeu_si256((__m256i*)p, z);
                p += 4;
            }
#elif defined(__SSE2__)
                c = ((n - 1) & 1) + 1;
            auto r = src + n - c;
            while (p != r) {
                auto a = _mm_loadu_si128((const __m128i*)p);
                auto b = _mm_loadu_si128((const __m128i*)(p + 1));
                auto x = _mm_srli_epi64(a, imm);
                auto y = _mm_slli_epi64(b, 64 - imm);
                auto z = _mm_or_si128(x, y);
                _mm_storeu_si128((__m128i*)p, z);
                p += 2;
            }
#endif
            WJR_IS_NOT_CONSTANT_EVALUATED_END
                while (--c) {
                    *p = (*p) >> imm | (*(p + 1)) << (64 - imm);
                    ++p;
                }
            *p = (*p) >> imm;
        }

        WJR_CONSTEXPR uint32_t vector_mul_1(
            uint32_t* result, const uint32_t* lp, size_t n, uint32_t rv
        ) {
            uint32_t cf = 0;
            size_t r = n & 3;
            size_t i = 0;
            for (; i != r; ++i) {
                auto val = lp[i] * static_cast<uint64_t>(rv) + cf;
                cf = val >> 32;
                result[i] = static_cast<uint32_t>(val);
            }
            for (; i != n; i += 4) {
                auto val = lp[i] * static_cast<uint64_t>(rv) + cf;
                cf = val >> 32;
                result[i] = static_cast<uint32_t>(val);

                val = lp[i + 1] * static_cast<uint64_t>(rv) + cf;
                cf = val >> 32;
                result[i + 1] = static_cast<uint32_t>(val);

                val = lp[i + 2] * static_cast<uint64_t>(rv) + cf;
                cf = val >> 32;
                result[i + 2] = static_cast<uint32_t>(val);

                val = lp[i + 3] * static_cast<uint64_t>(rv) + cf;
                cf = val >> 32;
                result[i + 3] = static_cast<uint32_t>(val);
            }
            return cf;
        }

#if defined(__SIZEOF_INT128__)
        uint64_t vector_mul_1(
            uint64_t* result, const uint64_t* lp, size_t n, uint64_t rv) {
            uint64_t cf = 0;
            size_t r = n & 3;
            size_t i = 0;
            for (; i != r; ++i) {
                auto val = lp[i] * static_cast<__uint128_t>(rv) + cf;
                cf = val >> 64;
                result[i] = static_cast<uint64_t>(val);
            }
#if defined(WJR_GCC_STYLE_ASM) && defined(WJR_X64)
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                uint64_t c_in;
            uint64_t _high0, _low0;
            uint64_t _high1, _low1;
            for (; i != n; i += 4) {
                c_in = 0;
                __asm__("mulq (%[v1], %[v2], 8)\n"
                    : "=a"(_low0), "=d"(_high0)
                    : "a"(rv), [v1] "r"(lp), [v2] "r"(i));
                __asm__("mulq 0x8(%[v1], %[v2], 8)\n"
                    : "=a"(_low1), "=d"(_high1)
                    : "a"(rv), [v1] "r"(lp), [v2] "r"(i));
                __asm__(
                    "add %[v1], %[u2]\n"
                    "mov %[u2], (%[v3], %[v5], 8)\n"
                    "adc %[v2], %[u1]\n"
                    "mov %[u1], 0x8(%[v3], %[v5], 8)\n"
                    "adc %[v6], %[u3]\n"
                    : [u1] "+r"(_low1), [u2] "+r"(cf), [u3] "+r"(c_in)
                    : [v1] "r"(_low0), [v2] "r"(_high0), [v3] "r"(result),
                    [v4] "r"(lp), [v5] "r"(i), [v6] "r"(_high1)
                );

                cf = 0;
                __asm__("mulq 0x10(%[v1], %[v2], 8)\n"
                    : "=a"(_low0), "=d"(_high0)
                    : "a"(rv), [v1] "r"(lp), [v2] "r"(i));
                __asm__("mulq 0x18(%[v1], %[v2], 8)\n"
                    : "=a"(_low1), "=d"(_high1)
                    : "a"(rv), [v1] "r"(lp), [v2] "r"(i));
                __asm__(
                    "add %[v1], %[u2]\n"
                    "mov %[u2], 0x10(%[v3], %[v5], 8)\n"
                    "adc %[v2], %[u1]\n"
                    "mov %[u1], 0x18(%[v3], %[v5], 8)\n"
                    "adc %[v6], %[u3]\n"
                    : [u1] "+r"(_low1), [u2] "+r"(c_in), [u3] "+r"(cf)
                    : [v1] "r"(_low0), [v2] "r"(_high0), [v3] "r"(result),
                    [v4] "r"(lp), [v5] "r"(i), [v6] "r"(_high1)
                );
            }
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#endif
                for (; i != n; ++i) {
                    auto val = lp[i] * static_cast<__uint128_t>(rv) + cf;
                    cf = val >> 64;
                    result[i] = static_cast<uint64_t>(val);
                }
            return cf;
        }

#endif

        WJR_CONSTEXPR uint32_t vector_addmul_1(
            uint32_t* result, const uint32_t* lp, size_t n, uint32_t rv) {
            uint32_t cf = 0;
            size_t r = n & 3;
            size_t i = 0;
            for (; i != r; ++i) {
                auto val = lp[i] * static_cast<uint64_t>(rv) + result[i] + cf;
                cf = val >> 32;
                result[i] = static_cast<uint32_t>(val);
            }
            for (; i != n; i += 4) {
                auto val = lp[i] * static_cast<uint64_t>(rv) + result[i] + cf;
                cf = val >> 32;
                result[i] = static_cast<uint32_t>(val);

                val = lp[i + 1] * static_cast<uint64_t>(rv) + result[i + 1] + cf;
                cf = val >> 32;
                result[i + 1] = static_cast<uint32_t>(val);

                val = lp[i + 2] * static_cast<uint64_t>(rv) + result[i + 2] + cf;
                cf = val >> 32;
                result[i + 2] = static_cast<uint32_t>(val);

                val = lp[i + 3] * static_cast<uint64_t>(rv) + result[i + 3] + cf;
                cf = val >> 32;
                result[i + 3] = static_cast<uint32_t>(val);
            }
            return cf;
        }

#if defined(__SIZEOF_INT128__)
        WJR_CONSTEXPR uint64_t vector_addmul_1(
            uint64_t* result, const uint64_t* lp, size_t n, uint64_t rv) {
            uint64_t cf = 0;
            size_t r = n & 3;
            size_t i = 0;
            for (; i != r; ++i) {
                auto val = lp[i] * (__uint128_t)rv + result[i] + cf;
                cf = val >> 64;
                result[i] = static_cast<uint64_t>(val);
            }
#if defined(WJR_GCC_STYLE_ASM) && defined(WJR_X64)
            WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
                uint64_t c_in;
            uint64_t _high0, _low0;
            uint64_t _high1, _low1;
            for (; i != n; i += 4) {
                c_in = 0;
                __asm__("mulq (%[v1], %[v2], 8)\n"
                    : "=a"(_low0), "=d"(_high0)
                    : "a"(rv), [v1] "r"(lp), [v2] "r"(i));
                __asm__("mulq 0x8(%[v1], %[v2], 8)\n"
                    : "=a"(_low1), "=d"(_high1)
                    : "a"(rv), [v1] "r"(lp), [v2] "r"(i));
                __asm__(
                    "add %[v1], %[u2]\n"
                    "mov (%[v3], %[v5], 8), %[v1]\n"
                    "adc %[v2], %[u1]\n"
                    "mov 0x8(%[v3], %[v5], 8), %[v2]\n"
                    "adc %[u3], %[u4]\n"
                    "add %[v1], %[u2]\n"
                    "mov %[u2], (%[v3], %[v5], 8)\n"
                    "adc %[v2], %[u1]\n"
                    "mov %[u1], 0x8(%[v3], %[v5], 8)\n"
                    "adc %[u4], %[u3]\n"
                    : [u1] "+r"(_low1), [u2] "+r"(cf), [u3] "+r"(c_in), [u4] "+r"(_high1)
                    : [v1] "r"(_low0), [v2] "r"(_high0), [v3] "r"(result), [v4] "r"(lp),
                    [v5] "r"(i));

                cf = 0;
                __asm__("mulq 0x10(%[v1], %[v2], 8)\n"
                    : "=a"(_low0), "=d"(_high0)
                    : "a"(rv), [v1] "r"(lp), [v2] "r"(i));
                __asm__("mulq 0x18(%[v1], %[v2], 8)\n"
                    : "=a"(_low1), "=d"(_high1)
                    : "a"(rv), [v1] "r"(lp), [v2] "r"(i));
                __asm__(
                    "add %[v1], %[u2]\n"
                    "mov 0x10(%[v3], %[v5], 8), %[v1]\n"
                    "adc %[v2], %[u1]\n"
                    "mov 0x18(%[v3], %[v5], 8), %[v2]\n"
                    "adc %[u3], %[u4]\n"
                    "add %[v1], %[u2]\n"
                    "mov %[u2], 0x10(%[v3], %[v5], 8)\n"
                    "adc %[v2], %[u1]\n"
                    "mov %[u1], 0x18(%[v3], %[v5], 8)\n"
                    "adc %[u4], %[u3]\n"
                    : [u1] "+r"(_low1), [u2] "+r"(c_in), [u3] "+r"(cf), [u4] "+r"(_high1)
                    : [v1] "r"(_low0), [v2] "r"(_high0), [v3] "r"(result), [v4] "r"(lp),
                    [v5] "r"(i));
            }
            WJR_IS_NOT_CONSTANT_EVALUATED_END
#endif
                for (; i != n; ++i) {
                    auto val = lp[i] * (__uint128_t)rv + result[i] + cf;
                    cf = val >> 64;
                    result[i] = static_cast<uint64_t>(val);
                }
            return cf;
        }
#endif

    }

}

#endif
