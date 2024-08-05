#ifndef WJR_X86_MATH_LARGE_FIND_IMPL_HPP__
#define WJR_X86_MATH_LARGE_FIND_IMPL_HPP__

#include <wjr/math/clz.hpp>
#include <wjr/math/ctz.hpp>
#include <wjr/x86/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1)
#define WJR_HAS_BUILTIN_FIND_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_FIND_NOT_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_NOT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(FIND_NOT_N)

template <typename T>
WJR_PURE size_t large_builtin_find_not_n(const T *src0, const T *src1,
                                         size_t n) noexcept {
#define WJR_REGISTER_FIND_NOT_N_2(index)                                                 \
    do {                                                                                 \
        const auto x = sse::loadu(src0 + (index));                                       \
        const auto y = sse::loadu(src1 + (index));                                       \
        const auto r = sse::cmpeq_epi64(x, y);                                           \
                                                                                         \
        const sse::mask_type mask = ~sse::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)

#if WJR_HAS_SIMD(AVX2)
#define WJR_REGISTER_FIND_NOT_N_4(index)                                                 \
    do {                                                                                 \
        const auto x = avx::loadu(src0 + (index));                                       \
        const auto y = avx::loadu(src1 + (index));                                       \
        const auto r = avx::cmpeq_epi64(x, y);                                           \
                                                                                         \
        const avx::mask_type mask = ~avx::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + ctz(mask) / 8;                                              \
        }                                                                                \
    } while (0)
#else
#define WJR_REGISTER_FIND_NOT_N_4(index)                                                 \
    WJR_REGISTER_FIND_NOT_N_2(index);                                                    \
    WJR_REGISTER_FIND_NOT_N_2((index) + 2)
#endif

#define WJR_REGISTER_FIND_NOT_N_ADVNCE(index)                                            \
    src0 += index;                                                                       \
    src1 += index

#define WJR_REGISTER_FIND_NOT_N_RET(index) index

    WJR_REGISTER_X86_NORMAL_SIMD_FUNCTION(
        n, WJR_REGISTER_FIND_NOT_N_2, WJR_REGISTER_FIND_NOT_N_4, WJR_HAS_SIMD(AVX2),
        WJR_REGISTER_FIND_NOT_N_ADVNCE, const auto __src0 = src0,
        WJR_REGISTER_FIND_NOT_N_RET);

#if !WJR_HAS_SIMD(AVX2)
    do {
        const auto r0 = sse::cmpeq_epi64(sse::loadu(src0), sse::loadu(src1));
        const auto r1 = sse::cmpeq_epi64(sse::loadu(src0 + 2), sse::loadu(src1 + 2));
        const auto r2 = sse::cmpeq_epi64(sse::loadu(src0 + 4), sse::loadu(src1 + 4));
        const auto r3 = sse::cmpeq_epi64(sse::loadu(src0 + 6), sse::loadu(src1 + 6));

        const auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return (src0 - __src0) + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return (src0 - __src0) + 2 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return (src0 - __src0) + 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r3);
            return (src0 - __src0) + 6 + ctz(mask) / 8;
        }

        src0 += 8;
        src1 += 8;
        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    do {
        const auto r0 = avx::cmpeq_epi64(avx::loadu(src0), avx::loadu(src1));
        const auto r1 = avx::cmpeq_epi64(avx::loadu(src0 + 4), avx::loadu(src1 + 4));
        const auto r2 = avx::cmpeq_epi64(avx::loadu(src0 + 8), avx::loadu(src1 + 8));
        const auto r3 = avx::cmpeq_epi64(avx::loadu(src0 + 12), avx::loadu(src1 + 12));

        const auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r0);
            if (mask != 0) {
                return (src0 - __src0) + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return (src0 - __src0) + 4 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return (src0 - __src0) + 8 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r3);
            return (src0 - __src0) + 12 + ctz(mask) / 8;
        }

        src0 += 16;
        src1 += 16;
        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return src0 - __src0;

#undef WJR_REGISTER_FIND_NOT_N_RET
#undef WJR_REGISTER_FIND_NOT_N_ADVNCE
#undef WJR_REGISTER_FIND_NOT_N_4
#undef WJR_REGISTER_FIND_NOT_N_2
}

extern template WJR_PURE size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src0,
                                                                   const uint64_t *src1,
                                                                   size_t n) noexcept;

template <typename T>
WJR_PURE size_t large_builtin_find_not_n(const T *src, T val, size_t n) noexcept {
#define WJR_REGISTER_FIND_NOT_N_2(index)                                                 \
    do {                                                                                 \
        const auto r = sse::cmpeq_epi64(sse::loadu(src + (index)), y2);                  \
                                                                                         \
        const sse::mask_type mask = ~sse::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)

#if WJR_HAS_SIMD(AVX2)
#define WJR_REGISTER_FIND_NOT_N_4(index)                                                 \
    do {                                                                                 \
        const auto r = avx::cmpeq_epi64(avx::loadu(src + (index)), y4);                  \
                                                                                         \
        const avx::mask_type mask = ~avx::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + ctz(mask) / 8;                                              \
        }                                                                                \
    } while (0)
#else
#define WJR_REGISTER_FIND_NOT_N_4(index)                                                 \
    WJR_REGISTER_FIND_NOT_N_2(index);                                                    \
    WJR_REGISTER_FIND_NOT_N_2((index) + 2)
#endif

#define WJR_REGISTER_FIND_NOT_N_ADVANCE(index) src += index

#define WJR_REGISTER_FIND_NOT_N_RET(index) index

    const auto y2 = sse::set1(val, T());
#if WJR_HAS_SIMD(AVX2)
    const auto y4 = broadcast<__m128i_t, __m256i_t>(y2);
#endif

    WJR_REGISTER_X86_NORMAL_SIMD_FUNCTION(
        n, WJR_REGISTER_FIND_NOT_N_2, WJR_REGISTER_FIND_NOT_N_4, WJR_HAS_SIMD(AVX2),
        WJR_REGISTER_FIND_NOT_N_ADVANCE, const auto __src = src,
        WJR_REGISTER_FIND_NOT_N_RET);

#if !WJR_HAS_SIMD(AVX2)
    do {
        const auto r0 = sse::cmpeq_epi64(sse::loadu(src), y2);
        const auto r1 = sse::cmpeq_epi64(sse::loadu(src + 2), y2);
        const auto r2 = sse::cmpeq_epi64(sse::loadu(src + 4), y2);
        const auto r3 = sse::cmpeq_epi64(sse::loadu(src + 6), y2);

        const auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return (src - __src) + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return (src - __src) + 2 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return (src - __src) + 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r3);
            return (src - __src) + 6 + ctz(mask) / 8;
        }

        src += 8;
        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    do {
        const auto r0 = avx::cmpeq_epi64(avx::loadu(src), y4);
        const auto r1 = avx::cmpeq_epi64(avx::loadu(src + 4), y4);
        const auto r2 = avx::cmpeq_epi64(avx::loadu(src + 8), y4);
        const auto r3 = avx::cmpeq_epi64(avx::loadu(src + 12), y4);

        const auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r0);
            if (mask != 0) {
                return (src - __src) + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return (src - __src) + 4 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return (src - __src) + 8 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r3);
            return (src - __src) + 12 + ctz(mask) / 8;
        }

        src += 16;
        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return src - __src;

#undef WJR_REGISTER_FIND_NOT_N_RET
#undef WJR_REGISTER_FIND_NOT_N_ADVANCE
#undef WJR_REGISTER_FIND_NOT_N_4
#undef WJR_REGISTER_FIND_NOT_N_2
}

extern template WJR_PURE size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src,
                                                                   uint64_t val,
                                                                   size_t n) noexcept;

#endif // WJR_HAS_BUILTIN(FIND_NOT_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

template <typename T>
WJR_PURE size_t large_builtin_reverse_find_not_n(const T *src0, const T *src1,
                                                 size_t n) noexcept {
#define WJR_REGISTER_REVERSE_FIND_NOT_N_2(index)                                         \
    do {                                                                                 \
        const auto x = sse::loadu(src0 + (index));                                       \
        const auto y = sse::loadu(src1 + (index));                                       \
        const auto r = sse::cmpeq_epi64(x, y);                                           \
                                                                                         \
        const sse::mask_type mask = ~sse::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + 2 - (mask == 0x00FF);                                       \
        }                                                                                \
    } while (0)

#if WJR_HAS_SIMD(AVX2)
#define WJR_REGISTER_REVERSE_FIND_NOT_N_4(index)                                         \
    do {                                                                                 \
        const auto x = avx::loadu(src0 + (index));                                       \
        const auto y = avx::loadu(src1 + (index));                                       \
        const auto r = avx::cmpeq_epi64(x, y);                                           \
                                                                                         \
        const avx::mask_type mask = ~avx::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + 4 - clz(mask) / 8;                                          \
        }                                                                                \
    } while (0)
#else
#define WJR_REGISTER_REVERSE_FIND_NOT_N_4(index)                                         \
    WJR_REGISTER_REVERSE_FIND_NOT_N_2((index) + 2);                                      \
    WJR_REGISTER_REVERSE_FIND_NOT_N_2(index);
#endif

#define WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE(index)                                   \
    src0 += index;                                                                       \
    src1 += index

#define WJR_REGISTER_REVERSE_FIND_NOT_N_RET(index) 0

    WJR_REGISTER_X86_NORMAL_REVERSE_SIMD_FUNCTION(
        n, WJR_REGISTER_REVERSE_FIND_NOT_N_2, WJR_REGISTER_REVERSE_FIND_NOT_N_4,
        WJR_HAS_SIMD(AVX2), WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE, ,
        WJR_REGISTER_REVERSE_FIND_NOT_N_RET);

#if !WJR_HAS_SIMD(AVX2)
    do {
        const auto r0 = sse::cmpeq_epi64(sse::loadu(src0 - 8), sse::loadu(src1 - 8));
        const auto r1 = sse::cmpeq_epi64(sse::loadu(src0 - 6), sse::loadu(src1 - 6));
        const auto r2 = sse::cmpeq_epi64(sse::loadu(src0 - 4), sse::loadu(src1 - 4));
        const auto r3 = sse::cmpeq_epi64(sse::loadu(src0 - 2), sse::loadu(src1 - 2));

        const auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r3);
            if (mask != 0) {
                return n - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        src0 -= 8;
        src1 -= 8;
        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    do {
        const auto r0 = avx::cmpeq_epi64(avx::loadu(src0 - 16), avx::loadu(src1 - 16));
        const auto r1 = avx::cmpeq_epi64(avx::loadu(src0 - 12), avx::loadu(src1 - 12));
        const auto r2 = avx::cmpeq_epi64(avx::loadu(src0 - 8), avx::loadu(src1 - 8));
        const auto r3 = avx::cmpeq_epi64(avx::loadu(src0 - 4), avx::loadu(src1 - 4));

        const auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r3);
            if (mask != 0) {
                return n - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        src0 -= 16;
        src1 -= 16;
        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_NOT_N_RET
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_4
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_2
}

extern template WJR_PURE size_t large_builtin_reverse_find_not_n<uint64_t>(
    const uint64_t *src0, const uint64_t *src1, size_t n) noexcept;

template <typename T>
WJR_PURE size_t large_builtin_reverse_find_not_n(const T *src, T val, size_t n) noexcept {
#define WJR_REGISTER_REVERSE_FIND_NOT_N_2(index)                                         \
    do {                                                                                 \
        const auto x = sse::loadu(src + (index));                                        \
        const auto r = sse::cmpeq_epi64(x, y2);                                          \
                                                                                         \
        const sse::mask_type mask = ~sse::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + 2 - (mask == 0x00FF);                                       \
        }                                                                                \
    } while (0)

#if WJR_HAS_SIMD(AVX2)
#define WJR_REGISTER_REVERSE_FIND_NOT_N_4(index)                                         \
    do {                                                                                 \
        const auto x = avx::loadu(src + (index));                                        \
        const auto r = avx::cmpeq_epi64(x, y4);                                          \
                                                                                         \
        const avx::mask_type mask = ~avx::movemask_epi8(r);                              \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + 4 - clz(mask) / 8;                                          \
        }                                                                                \
    } while (0)
#else
#define WJR_REGISTER_REVERSE_FIND_NOT_N_4(index)                                         \
    WJR_REGISTER_REVERSE_FIND_NOT_N_2((index) + 2);                                      \
    WJR_REGISTER_REVERSE_FIND_NOT_N_2(index)
#endif

#define WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE(index) src += index

#define WJR_REGISTER_REVERSE_FIND_NOT_N_RET(index) 0

    const auto y2 = sse::set1(val, T());
#if WJR_HAS_SIMD(AVX2)
    const auto y4 = broadcast<__m128i_t, __m256i_t>(y2);
#endif

    WJR_REGISTER_X86_NORMAL_REVERSE_SIMD_FUNCTION(
        n, WJR_REGISTER_REVERSE_FIND_NOT_N_2, WJR_REGISTER_REVERSE_FIND_NOT_N_4,
        WJR_HAS_SIMD(AVX2), WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE, ,
        WJR_REGISTER_REVERSE_FIND_NOT_N_RET);

#if !WJR_HAS_SIMD(AVX2)
    do {
        const auto r0 = sse::cmpeq_epi64(sse::loadu(src - 8), y2);
        const auto r1 = sse::cmpeq_epi64(sse::loadu(src - 6), y2);
        const auto r2 = sse::cmpeq_epi64(sse::loadu(src - 4), y2);
        const auto r3 = sse::cmpeq_epi64(sse::loadu(src - 2), y2);

        const auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r3);
            if (mask != 0) {
                return n - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        src -= 8;
        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    do {
        const auto r0 = avx::cmpeq_epi64(avx::loadu(src - 16), y4);
        const auto r1 = avx::cmpeq_epi64(avx::loadu(src - 12), y4);
        const auto r2 = avx::cmpeq_epi64(avx::loadu(src - 8), y4);
        const auto r3 = avx::cmpeq_epi64(avx::loadu(src - 4), y4);

        const auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r3);
            if (mask != 0) {
                return n - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        src -= 16;
        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_NOT_N_RET
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_ADVANCE
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_4
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_2
}

extern template WJR_PURE size_t large_builtin_reverse_find_not_n<uint64_t>(
    const uint64_t *src, uint64_t val, size_t n) noexcept;

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

} // namespace wjr

#endif // WJR_X86_MATH_LARGE_FIND_IMPL_HPP__