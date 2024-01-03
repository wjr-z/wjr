#ifndef WJR_X86_COMPARE_HPP__
#define WJR_X86_COMPARE_HPP__

#include <wjr/gen/fast_1_2_8.hpp>
#include <wjr/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_COMPARE_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_COMPARE_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(COMPARE_N)

#define WJR_REGISTER_COMPARE_NOT_N_L2(index)                                             \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 + (index)));                                \
        auto y = sse::loadu((__m128i *)(src1 + (index)));                                \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            if (mask == 0x00FF) {                                                        \
                return src0[(index) + 1] < src1[(index) + 1] ? -1 : 1;                   \
            }                                                                            \
            return src0[(index)] < src1[(index)] ? -1 : 1;                               \
        }                                                                                \
    } while (0)

template <typename T>
int large_builtin_compare_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_COMPARE_NOT_N_L8(index)                                             \
    WJR_REGISTER_COMPARE_NOT_N_L2((index));                                              \
    WJR_REGISTER_COMPARE_NOT_N_L2((index) + 2);                                          \
    WJR_REGISTER_COMPARE_NOT_N_L2((index) + 4);                                          \
    WJR_REGISTER_COMPARE_NOT_N_L2((index) + 6)

#define WJR_REGISTER_COMPARE_NOT_N_RET() 0

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_COMPARE_NOT_N_L2,
                             WJR_REGISTER_COMPARE_NOT_N_L8, WJR_PP_EMPTY, WJR_PP_EMPTY,
                             WJR_REGISTER_COMPARE_NOT_N_RET);

#undef WJR_REGISTER_COMPARE_NOT_N_RET
#undef WJR_REGISTER_COMPARE_NOT_N_L8
}

template <typename T>
WJR_INTRINSIC_INLINE int builtin_compare_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_COMPARE_NOT_N_L1(index)                                             \
    if (src0[(index)] != src1[(index)]) {                                                \
        return src0[(index)] < src1[(index)] ? -1 : 1;                                   \
    }

#define WJR_REGISTER_COMPARE_NOT_N_RET() 0
#define WJR_REGISTER_LARGE_COMPARE_NOT_N(gen_offset, gen_n, ...)                         \
    return large_builtin_compare_n(src0 + gen_offset, src1 + gen_offset, gen_n)

    WJR_GEN_SMALL_FAST_1_2_8(
        n, WJR_REGISTER_COMPARE_NOT_N_L1, WJR_REGISTER_COMPARE_NOT_N_L2, WJR_PP_EMPTY,
        WJR_PP_EMPTY, WJR_REGISTER_COMPARE_NOT_N_RET, WJR_REGISTER_LARGE_COMPARE_NOT_N);

#undef WJR_REGISTER_LARGE_COMPARE_NOT_N
#undef WJR_REGISTER_COMPARE_NOT_N_RET
#undef WJR_REGISTER_COMPARE_NOT_N_L1
}

#undef WJR_REGISTER_COMPARE_NOT_N_L2

#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)

#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_L2(index)                                     \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            if (mask == 0xFF00) {                                                        \
                return src0[-2 - (index)] < src1[-2 - (index)] ? -1 : 1;                 \
            }                                                                            \
            return src0[-1 - (index)] < src1[-1 - (index)] ? -1 : 1;                     \
        }                                                                                \
    } while (0)

template <typename T>
int large_builtin_reverse_compare_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_L8(index)                                     \
    WJR_REGISTER_REVERSE_COMPARE_NOT_N_L2((index));                                      \
    WJR_REGISTER_REVERSE_COMPARE_NOT_N_L2((index) + 2);                                  \
    WJR_REGISTER_REVERSE_COMPARE_NOT_N_L2((index) + 4);                                  \
    WJR_REGISTER_REVERSE_COMPARE_NOT_N_L2((index) + 6)

#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET() 0

    WJR_GEN_LARGE_FAST_1_2_8(n, WJR_REGISTER_REVERSE_COMPARE_NOT_N_L2,
                             WJR_REGISTER_REVERSE_COMPARE_NOT_N_L8, WJR_PP_EMPTY,
                             WJR_PP_EMPTY, WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET);

#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET
#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_L8
}

template <typename T>
WJR_INTRINSIC_INLINE int builtin_reverse_compare_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_L1(index)                                     \
    if (src0[-1 - (index)] != src1[-1 - (index)]) {                                      \
        return src0[-1 - (index)] < src1[-1 - (index)] ? -1 : 1;                         \
    }

#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET() 0
#define WJR_REGISTER_LARGE_REVERSE_COMPARE_NOT_N(gen_offset, gen_n, n, ...)              \
    return large_builtin_reverse_compare_n(src0 - gen_offset, src1 - gen_offset, gen_n)

    src0 += n;
    src1 += n;

    WJR_GEN_SMALL_FAST_1_2_8(n, WJR_REGISTER_REVERSE_COMPARE_NOT_N_L1,
                             WJR_REGISTER_REVERSE_COMPARE_NOT_N_L2, WJR_PP_EMPTY,
                             WJR_PP_EMPTY, WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET,
                             WJR_REGISTER_LARGE_REVERSE_COMPARE_NOT_N);

#undef WJR_REGISTER_LARGE_REVERSE_COMPARE_NOT_N
#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_RET
#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_L1
}

#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_L2

#endif

} // namespace wjr

#endif // WJR_X86_COMPARE_HPP__