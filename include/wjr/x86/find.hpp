#ifndef WJR_X86_FIND_HPP__
#define WJR_X86_FIND_HPP__

#include <wjr/gen/fast_1_2_8.hpp>
#include <wjr/simd/simd.hpp>

#if !defined(WJR_X86)
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_FIND_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_FIND_NOT_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_NOT_N WJR_HAS_DEF
#endif

#define WJR_REGISTER_FIND_VAL_I2(...) auto y = sse::set1_epi64(val)

#if WJR_HAS_BUILTIN(FIND_N)

#define WJR_REGISTER_FIND_N_L2(index)                                                    \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 + (index)));                                \
        auto y = sse::loadu((__m128i *)(src1 + (index)));                                \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)

template <typename T>
size_t large_builtin_find_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_FIND_N_L8(index)                                                    \
    WJR_REGISTER_FIND_N_L2((index));                                                     \
    WJR_REGISTER_FIND_N_L2((index) + 2);                                                 \
    WJR_REGISTER_FIND_N_L2((index) + 4);                                                 \
    WJR_REGISTER_FIND_N_L2((index) + 6)

#define WJR_REGISTER_FIND_N_RET() n

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_FIND_N_L2, WJR_REGISTER_FIND_N_L8,
                             WJR_PP_EMPTY, WJR_PP_EMPTY, WJR_REGISTER_FIND_N_RET);

#undef WJR_REGISTER_FIND_N_RET
#undef WJR_REGISTER_FIND_N_L8
}

template <typename T>
WJR_INLINE size_t builtin_find_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_FIND_N_L1(index)                                                    \
    if (src0[(index)] == src1[(index)]) {                                                \
        return (index);                                                                  \
    }

#define WJR_REGISTER_FIND_N_RET() n
#define WJR_REGISTER_LARGE_FIND_N(gen_offset, gen_n, ...)                                \
    return gen_offset + large_builtin_find_n(src0 + gen_offset, src1 + gen_offset, gen_n)

    WJR_GEN_SMALL_FAST_1_2_8(n, WJR_REGISTER_FIND_N_L1, WJR_REGISTER_FIND_N_L2,
                             WJR_PP_EMPTY, WJR_PP_EMPTY, WJR_REGISTER_FIND_N_RET,
                             WJR_REGISTER_LARGE_FIND_N);

#undef WJR_REGISTER_LARGE_FIND_N
#undef WJR_REGISTER_FIND_N_RET
#undef WJR_REGISTER_FIND_N_L1
}

#undef WJR_REGISTER_FIND_N_L2

#define WJR_REGISTER_FIND_N_L2(index)                                                    \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src + (index)));                                 \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)

template <typename T>
size_t large_builtin_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_N_L8(index)                                                    \
    WJR_REGISTER_FIND_N_L2((index));                                                     \
    WJR_REGISTER_FIND_N_L2((index) + 2);                                                 \
    WJR_REGISTER_FIND_N_L2((index) + 4);                                                 \
    WJR_REGISTER_FIND_N_L2((index) + 6)

#define WJR_REGISTER_FIND_N_RET() n

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_FIND_N_L2, WJR_REGISTER_FIND_N_L8,
                             WJR_REGISTER_FIND_VAL_I2, WJR_PP_EMPTY,
                             WJR_REGISTER_FIND_N_RET);

#undef WJR_REGISTER_FIND_N_RET
#undef WJR_REGISTER_FIND_N_L8
}

template <typename T>
WJR_INLINE size_t builtin_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_N_L1(index)                                                    \
    if (src[(index)] == val) {                                                           \
        return (index);                                                                  \
    }

#define WJR_REGISTER_FIND_N_RET() n
#define WJR_REGISTER_LARGE_FIND_N(gen_offset, gen_n, ...)                                \
    return gen_offset + large_builtin_find_n(src + gen_offset, val, gen_n)

    WJR_GEN_SMALL_FAST_1_2_8(n, WJR_REGISTER_FIND_N_L1, WJR_REGISTER_FIND_N_L2,
                             WJR_PP_EMPTY, WJR_REGISTER_FIND_VAL_I2,
                             WJR_REGISTER_FIND_N_RET, WJR_REGISTER_LARGE_FIND_N);

#undef WJR_REGISTER_LARGE_FIND_N
#undef WJR_REGISTER_FIND_N_RET
#undef WJR_REGISTER_FIND_N_L1
}

#undef WJR_REGISTER_FIND_N_L2

#endif // WJR_HAS_BUILTIN(FIND_N)

#if WJR_HAS_BUILTIN(FIND_NOT_N)

#define WJR_REGISTER_FIND_NOT_N_L2(index)                                                \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 + (index)));                                \
        auto y = sse::loadu((__m128i *)(src1 + (index)));                                \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return (index) + (mask == 0x00FF);                                           \
        }                                                                                \
    } while (0)

template <typename T>
size_t large_builtin_find_not_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_FIND_NOT_N_L8(index)                                                \
    WJR_REGISTER_FIND_NOT_N_L2((index));                                                 \
    WJR_REGISTER_FIND_NOT_N_L2((index) + 2);                                             \
    WJR_REGISTER_FIND_NOT_N_L2((index) + 4);                                             \
    WJR_REGISTER_FIND_NOT_N_L2((index) + 6)

#define WJR_REGISTER_FIND_NOT_N_RET() n

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_FIND_NOT_N_L2, WJR_REGISTER_FIND_NOT_N_L8,
                             WJR_PP_EMPTY, WJR_PP_EMPTY, WJR_REGISTER_FIND_NOT_N_RET);

#undef WJR_REGISTER_FIND_NOT_N_RET
#undef WJR_REGISTER_FIND_NOT_N_L8
}

template <typename T>
WJR_INLINE size_t builtin_find_not_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_FIND_NOT_N_L1(index)                                                \
    if (src0[(index)] != src1[(index)]) {                                                \
        return (index);                                                                  \
    }

#define WJR_REGISTER_FIND_NOT_N_RET() n
#define WJR_REGISTER_LARGE_FIND_NOT_N(gen_offset, gen_n, ...)                            \
    return gen_offset +                                                                  \
           large_builtin_find_not_n(src0 + gen_offset, src1 + gen_offset, gen_n)

    WJR_GEN_SMALL_FAST_1_2_8(n, WJR_REGISTER_FIND_NOT_N_L1, WJR_REGISTER_FIND_NOT_N_L2,
                             WJR_PP_EMPTY, WJR_PP_EMPTY, WJR_REGISTER_FIND_NOT_N_RET,
                             WJR_REGISTER_LARGE_FIND_NOT_N);

#undef WJR_REGISTER_LARGE_FIND_NOT_N
#undef WJR_REGISTER_FIND_NOT_N_RET
#undef WJR_REGISTER_FIND_NOT_N_L1
}

#undef WJR_REGISTER_FIND_NOT_N_L2

#define WJR_REGISTER_FIND_NOT_N_L2(index)                                                \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src + (index)));                                 \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return (index) + (mask == 0x00FF);                                           \
        }                                                                                \
    } while (0)

template <typename T>
size_t large_builtin_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_NOT_N_L8(index)                                                \
    WJR_REGISTER_FIND_NOT_N_L2((index));                                                 \
    WJR_REGISTER_FIND_NOT_N_L2((index) + 2);                                             \
    WJR_REGISTER_FIND_NOT_N_L2((index) + 4);                                             \
    WJR_REGISTER_FIND_NOT_N_L2((index) + 6)

#define WJR_REGISTER_FIND_NOT_N_RET() n

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_FIND_NOT_N_L2, WJR_REGISTER_FIND_NOT_N_L8,
                             WJR_REGISTER_FIND_VAL_I2, WJR_PP_EMPTY,
                             WJR_REGISTER_FIND_NOT_N_RET);

#undef WJR_REGISTER_FIND_NOT_N_RET
#undef WJR_REGISTER_FIND_NOT_N_L8
}

template <typename T>
WJR_INLINE size_t builtin_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_NOT_N_L1(index)                                                \
    if (src[(index)] != val) {                                                           \
        return (index);                                                                  \
    }

#define WJR_REGISTER_FIND_NOT_N_RET() n
#define WJR_REGISTER_LARGE_FIND_NOT_N(gen_offset, gen_n, ...)                            \
    return gen_offset + large_builtin_find_not_n(src + gen_offset, val, gen_n)

    WJR_GEN_SMALL_FAST_1_2_8(n, WJR_REGISTER_FIND_NOT_N_L1, WJR_REGISTER_FIND_NOT_N_L2,
                             WJR_PP_EMPTY, WJR_REGISTER_FIND_VAL_I2,
                             WJR_REGISTER_FIND_NOT_N_RET, WJR_REGISTER_LARGE_FIND_NOT_N);

#undef WJR_REGISTER_LARGE_FIND_NOT_N
#undef WJR_REGISTER_FIND_NOT_N_RET
#undef WJR_REGISTER_FIND_NOT_N_L1
}

#undef WJR_REGISTER_FIND_NOT_N_L2

#endif // WJR_HAS_BUILTIN(FIND_NOT_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_N)

template <typename T>
size_t large_builtin_reverse_find_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_L2(index)                                            \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + (mask == 0x00FF);                                           \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_REVERSE_FIND_N_L8(index)                                            \
    WJR_REGISTER_REVERSE_FIND_N_L2((index));                                             \
    WJR_REGISTER_REVERSE_FIND_N_L2((index) + 2);                                         \
    WJR_REGISTER_REVERSE_FIND_N_L2((index) + 4);                                         \
    WJR_REGISTER_REVERSE_FIND_N_L2((index) + 6)

#define WJR_REGISTER_REVERSE_FIND_N_RET() n

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_REVERSE_FIND_N_L2,
                             WJR_REGISTER_REVERSE_FIND_N_L8, WJR_PP_EMPTY, WJR_PP_EMPTY,
                             WJR_REGISTER_REVERSE_FIND_N_RET);

#undef WJR_REGISTER_REVERSE_FIND_N_RET
#undef WJR_REGISTER_REVERSE_FIND_N_L8
#undef WJR_REGISTER_REVERSE_FIND_N_L2
}

template <typename T>
WJR_INLINE size_t builtin_reverse_find_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_L1(index)                                            \
    if (src0[-1 - (index)] == src1[-1 - (index)]) {                                      \
        return n - (index);                                                              \
    }
#define WJR_REGISTER_REVERSE_FIND_N_L2(index)                                            \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return n - (index) - (mask == 0x00FF);                                       \
        }                                                                                \
    } while (0)

#define WJR_REGISTER_REVERSE_FIND_N_RET() 0
#define WJR_REGISTER_LARGE_REVERSE_FIND_N(gen_offset, gen_n, n, ...)                     \
    return n - gen_offset -                                                              \
           large_builtin_reverse_find_n(src0 - gen_offset, src1 - gen_offset, gen_n)

    src0 += n;
    src1 += n;

    WJR_GEN_SMALL_FAST_1_2_8(
        n, WJR_REGISTER_REVERSE_FIND_N_L1, WJR_REGISTER_REVERSE_FIND_N_L2, WJR_PP_EMPTY,
        WJR_PP_EMPTY, WJR_REGISTER_REVERSE_FIND_N_RET, WJR_REGISTER_LARGE_REVERSE_FIND_N);

#undef WJR_REGISTER_LARGE_REVERSE_FIND_N
#undef WJR_REGISTER_REVERSE_FIND_N_RET
#undef WJR_REGISTER_REVERSE_FIND_N_L2
#undef WJR_REGISTER_REVERSE_FIND_N_L1
}

template <typename T>
size_t large_builtin_reverse_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_L2(index)                                            \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src - 2 - (index)));                             \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + (mask == 0x00FF);                                           \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_REVERSE_FIND_N_L8(index)                                            \
    WJR_REGISTER_REVERSE_FIND_N_L2((index));                                             \
    WJR_REGISTER_REVERSE_FIND_N_L2((index) + 2);                                         \
    WJR_REGISTER_REVERSE_FIND_N_L2((index) + 4);                                         \
    WJR_REGISTER_REVERSE_FIND_N_L2((index) + 6)

#define WJR_REGISTER_REVERSE_FIND_N_RET() n

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_REVERSE_FIND_N_L2,
                             WJR_REGISTER_REVERSE_FIND_N_L8, WJR_REGISTER_FIND_VAL_I2,
                             WJR_PP_EMPTY, WJR_REGISTER_REVERSE_FIND_N_RET);

#undef WJR_REGISTER_REVERSE_FIND_N_RET
#undef WJR_REGISTER_REVERSE_FIND_N_L8
#undef WJR_REGISTER_REVERSE_FIND_N_L2
}

template <typename T>
WJR_INLINE size_t builtin_reverse_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_L1(index)                                            \
    if (src[-1 - (index)] == val) {                                                      \
        return n - (index);                                                              \
    }
#define WJR_REGISTER_REVERSE_FIND_N_L2(index)                                            \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src - 2 - (index)));                             \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return n - (index) - (mask == 0x00FF);                                       \
        }                                                                                \
    } while (0)

#define WJR_REGISTER_REVERSE_FIND_N_RET() 0
#define WJR_REGISTER_LARGE_REVERSE_FIND_N(gen_offset, gen_n, n, ...)                     \
    return n - gen_offset - large_builtin_reverse_find_n(src - gen_offset, val, gen_n)

    src += n;

    WJR_GEN_SMALL_FAST_1_2_8(n, WJR_REGISTER_REVERSE_FIND_N_L1,
                             WJR_REGISTER_REVERSE_FIND_N_L2, WJR_PP_EMPTY,
                             WJR_REGISTER_FIND_VAL_I2, WJR_REGISTER_REVERSE_FIND_N_RET,
                             WJR_REGISTER_LARGE_REVERSE_FIND_N);

#undef WJR_REGISTER_LARGE_REVERSE_FIND_N
#undef WJR_REGISTER_REVERSE_FIND_N_RET
#undef WJR_REGISTER_REVERSE_FIND_N_L2
#undef WJR_REGISTER_REVERSE_FIND_N_L1
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

template <typename T>
size_t large_builtin_reverse_find_not_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_NOT_N_L2(index)                                        \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return (index) + (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_REVERSE_FIND_NOT_N_L8(index)                                        \
    WJR_REGISTER_REVERSE_FIND_NOT_N_L2((index));                                         \
    WJR_REGISTER_REVERSE_FIND_NOT_N_L2((index) + 2);                                     \
    WJR_REGISTER_REVERSE_FIND_NOT_N_L2((index) + 4);                                     \
    WJR_REGISTER_REVERSE_FIND_NOT_N_L2((index) + 6)

#define WJR_REGISTER_REVERSE_FIND_NOT_N_RET() n

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_REVERSE_FIND_NOT_N_L2,
                             WJR_REGISTER_REVERSE_FIND_NOT_N_L8, WJR_PP_EMPTY,
                             WJR_PP_EMPTY, WJR_REGISTER_REVERSE_FIND_NOT_N_RET);

#undef WJR_REGISTER_REVERSE_FIND_NOT_N_RET
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_L8
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_L2
}

template <typename T>
WJR_INLINE size_t builtin_reverse_find_not_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_NOT_N_L1(index)                                        \
    if (src0[-1 - (index)] != src1[-1 - (index)]) {                                      \
        return n - (index);                                                              \
    }
#define WJR_REGISTER_REVERSE_FIND_NOT_N_L2(index)                                        \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return n - (index) - (mask == 0xFF00);                                       \
        }                                                                                \
    } while (0)

#define WJR_REGISTER_REVERSE_FIND_NOT_N_RET() 0
#define WJR_REGISTER_LARGE_REVERSE_FIND_NOT_N(gen_offset, gen_n, n, ...)                 \
    return n - gen_offset -                                                              \
           large_builtin_reverse_find_not_n(src0 - gen_offset, src1 - gen_offset, gen_n)

    src0 += n;
    src1 += n;

    WJR_GEN_SMALL_FAST_1_2_8(n, WJR_REGISTER_REVERSE_FIND_NOT_N_L1,
                             WJR_REGISTER_REVERSE_FIND_NOT_N_L2, WJR_PP_EMPTY,
                             WJR_PP_EMPTY, WJR_REGISTER_REVERSE_FIND_NOT_N_RET,
                             WJR_REGISTER_LARGE_REVERSE_FIND_NOT_N);

#undef WJR_REGISTER_LARGE_REVERSE_FIND_NOT_N
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_RET
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_L2
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_L1
}

template <typename T>
size_t large_builtin_reverse_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_NOT_N_L2(index)                                        \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src - 2 - (index)));                             \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return (index) + (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_REVERSE_FIND_NOT_N_L8(index)                                        \
    WJR_REGISTER_REVERSE_FIND_NOT_N_L2((index));                                         \
    WJR_REGISTER_REVERSE_FIND_NOT_N_L2((index) + 2);                                     \
    WJR_REGISTER_REVERSE_FIND_NOT_N_L2((index) + 4);                                     \
    WJR_REGISTER_REVERSE_FIND_NOT_N_L2((index) + 6)

#define WJR_REGISTER_REVERSE_FIND_NOT_N_RET() n

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_REVERSE_FIND_NOT_N_L2,
                             WJR_REGISTER_REVERSE_FIND_NOT_N_L8, WJR_REGISTER_FIND_VAL_I2,
                             WJR_PP_EMPTY, WJR_REGISTER_REVERSE_FIND_NOT_N_RET);

#undef WJR_REGISTER_REVERSE_FIND_NOT_N_RET
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_L8
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_L2
}

template <typename T>
WJR_INLINE size_t builtin_reverse_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_NOT_N_L1(index)                                        \
    if (src[-1 - (index)] != val) {                                                      \
        return n - (index);                                                              \
    }
#define WJR_REGISTER_REVERSE_FIND_NOT_N_L2(index)                                        \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src - 2 - (index)));                             \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return n - (index) - (mask == 0xFF00);                                       \
        }                                                                                \
    } while (0)

#define WJR_REGISTER_REVERSE_FIND_NOT_N_RET() 0
#define WJR_REGISTER_LARGE_REVERSE_FIND_NOT_N(gen_offset, gen_n, n, ...)                 \
    return n - gen_offset - large_builtin_reverse_find_not_n(src - gen_offset, val, gen_n)

    src += n;

    WJR_GEN_SMALL_FAST_1_2_8(
        n, WJR_REGISTER_REVERSE_FIND_NOT_N_L1, WJR_REGISTER_REVERSE_FIND_NOT_N_L2,
        WJR_PP_EMPTY, WJR_REGISTER_FIND_VAL_I2, WJR_REGISTER_REVERSE_FIND_NOT_N_RET,
        WJR_REGISTER_LARGE_REVERSE_FIND_NOT_N);

#undef WJR_REGISTER_LARGE_REVERSE_FIND_NOT_N
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_RET
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_L2
#undef WJR_REGISTER_REVERSE_FIND_NOT_N_L1
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

#undef WJR_REGISTER_FIND_VAL_I2

} // namespace wjr

#endif // WJR_X86_FIND_HPP__