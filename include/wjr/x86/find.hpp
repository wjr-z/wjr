#ifndef WJR_X86_FIND_HPP__
#define WJR_X86_FIND_HPP__

#include <wjr/gen/fast_1_2_8.hpp>
#include <wjr/simd/simd.hpp>

#if !defined(WJR_X86)
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_FIND_P WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_P WJR_HAS_DEF
#define WJR_HAS_BUILTIN_FIND_NOT_P WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_NOT_P WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(FIND_P)

template <typename T, typename Ret>
WJR_INLINE std::invoke_result_t<Ret> builtin_find_p(const T *src0, const T *src1,
                                                    size_t n, Ret ret) {
#define WJR_REGISTER_FIND_P_L1(index)                                                    \
    if (src0[(index)] == src1[(index)]) {                                                \
        return ret((index));                                                             \
    }
#define WJR_REGISTER_FIND_P_L2(index)                                                    \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 + (index)));                                \
        auto y = sse::loadu((__m128i *)(src1 + (index)));                                \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return ret((index) + (mask == 0xFF00));                                      \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_FIND_P_L8(index)                                                    \
    WJR_REGISTER_FIND_P_L2((index));                                                     \
    WJR_REGISTER_FIND_P_L2((index) + 2);                                                 \
    WJR_REGISTER_FIND_P_L2((index) + 4);                                                 \
    WJR_REGISTER_FIND_P_L2((index) + 6)

#define WJR_REGISTER_FIND_P_RET() ret()

    WJR_GEN_FAST_1_2_8(WJR_REGISTER_FIND_P_L1, WJR_REGISTER_FIND_P_L2,
                       WJR_REGISTER_FIND_P_L8, WJR_PP_EMPTY, WJR_PP_EMPTY, WJR_PP_EMPTY,
                       WJR_REGISTER_FIND_P_RET);

#undef WJR_REGISTER_FIND_P_RET
#undef WJR_REGISTER_FIND_P_L8
#undef WJR_REGISTER_FIND_P_L2
#undef WJR_REGISTER_FIND_P_L1
}

template <typename T, typename Ret>
WJR_INLINE std::invoke_result_t<Ret> builtin_find_p(const T *src, T val, size_t n,
                                                    Ret ret) {
#define WJR_REGISTER_FIND_P_L1(index)                                                    \
    if (src[(index)] == val) {                                                           \
        return ret((index));                                                             \
    }
#define WJR_REGISTER_FIND_P_L2(index)                                                    \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src + (index)));                                 \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return ret((index) + (mask == 0xFF00));                                      \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_FIND_P_L8(index)                                                    \
    WJR_REGISTER_FIND_P_L2((index));                                                     \
    WJR_REGISTER_FIND_P_L2((index) + 2);                                                 \
    WJR_REGISTER_FIND_P_L2((index) + 4);                                                 \
    WJR_REGISTER_FIND_P_L2((index) + 6)

#define WJR_REGISTER_FIND_P_I2(...) auto y = sse::set1_epi64(val)
#define WJR_REGISTER_FIND_P_RET() ret()

    WJR_GEN_FAST_1_2_8(WJR_REGISTER_FIND_P_L1, WJR_REGISTER_FIND_P_L2,
                       WJR_REGISTER_FIND_P_L8, WJR_PP_EMPTY, WJR_REGISTER_FIND_P_I2,
                       WJR_PP_EMPTY, WJR_REGISTER_FIND_P_RET);

#undef WJR_REGISTER_FIND_P_RET
#undef WJR_REGISTER_FIND_P_I2
#undef WJR_REGISTER_FIND_P_L8
#undef WJR_REGISTER_FIND_P_L2
#undef WJR_REGISTER_FIND_P_L1
}

#endif // WJR_HAS_BUILTIN(FIND_P)

#if WJR_HAS_BUILTIN(FIND_NOT_P)

template <typename T, typename Ret>
WJR_INLINE std::invoke_result_t<Ret> builtin_find_not_p(const T *src0, const T *src1,
                                                        size_t n, Ret ret) {
#define WJR_REGISTER_FIND_NOT_P_L1(index)                                                \
    if (src0[(index)] != src1[(index)]) {                                                \
        return ret((index));                                                             \
    }
#define WJR_REGISTER_FIND_NOT_P_L2(index)                                                \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 + (index)));                                \
        auto y = sse::loadu((__m128i *)(src1 + (index)));                                \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return ret((index) + (mask == 0x00FF));                                      \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_FIND_NOT_P_L8(index)                                                \
    WJR_REGISTER_FIND_NOT_P_L2((index));                                                 \
    WJR_REGISTER_FIND_NOT_P_L2((index) + 2);                                             \
    WJR_REGISTER_FIND_NOT_P_L2((index) + 4);                                             \
    WJR_REGISTER_FIND_NOT_P_L2((index) + 6)

#define WJR_REGISTER_FIND_NOT_P_RET() ret()

    WJR_GEN_FAST_1_2_8(WJR_REGISTER_FIND_NOT_P_L1, WJR_REGISTER_FIND_NOT_P_L2,
                       WJR_REGISTER_FIND_NOT_P_L8, WJR_PP_EMPTY, WJR_PP_EMPTY,
                       WJR_PP_EMPTY, WJR_REGISTER_FIND_NOT_P_RET);

#undef WJR_REGISTER_FIND_NOT_P_RET
#undef WJR_REGISTER_FIND_NOT_P_L8
#undef WJR_REGISTER_FIND_NOT_P_L2
#undef WJR_REGISTER_FIND_NOT_P_L1
}

template <typename T, typename Ret>
WJR_INLINE std::invoke_result_t<Ret> builtin_find_not_p(const T *src, T val, size_t n,
                                                        Ret ret) {
#define WJR_REGISTER_FIND_NOT_P_L1(index)                                                \
    if (src[(index)] != val) {                                                           \
        return ret((index));                                                             \
    }
#define WJR_REGISTER_FIND_NOT_P_L2(index)                                                \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src + (index)));                                 \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return ret((index) + (mask == 0x00FF));                                      \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_FIND_NOT_P_L8(index)                                                \
    WJR_REGISTER_FIND_NOT_P_L2((index));                                                 \
    WJR_REGISTER_FIND_NOT_P_L2((index) + 2);                                             \
    WJR_REGISTER_FIND_NOT_P_L2((index) + 4);                                             \
    WJR_REGISTER_FIND_NOT_P_L2((index) + 6)

#define WJR_REGISTER_FIND_NOT_P_I2(...) auto y = sse::set1_epi64(val)
#define WJR_REGISTER_FIND_NOT_P_RET() ret()

    WJR_GEN_FAST_1_2_8(WJR_REGISTER_FIND_NOT_P_L1, WJR_REGISTER_FIND_NOT_P_L2,
                       WJR_REGISTER_FIND_NOT_P_L8, WJR_PP_EMPTY,
                       WJR_REGISTER_FIND_NOT_P_I2, WJR_PP_EMPTY,
                       WJR_REGISTER_FIND_NOT_P_RET);

#undef WJR_REGISTER_FIND_NOT_P_RET
#undef WJR_REGISTER_FIND_NOT_P_I2
#undef WJR_REGISTER_FIND_NOT_P_L8
#undef WJR_REGISTER_FIND_NOT_P_L2
#undef WJR_REGISTER_FIND_NOT_P_L1
}

#endif // WJR_HAS_BUILTIN(FIND_NOT_P)

#if WJR_HAS_BUILTIN(REVERSE_FIND_P)

template <typename T, typename Ret>
WJR_INLINE std::invoke_result_t<Ret> builtin_reverse_find_p(const T *src0, const T *src1,
                                                            size_t n, Ret ret) {
#define WJR_REGISTER_REVERSE_FIND_P_L1(index)                                            \
    if (src0[-1 - (index)] == src1[-1 - (index)]) {                                      \
        return ret((index));                                                             \
    }
#define WJR_REGISTER_REVERSE_FIND_P_L2(index)                                            \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return ret((index) + (mask == 0x00FF));                                      \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_REVERSE_FIND_P_L8(index)                                            \
    WJR_REGISTER_REVERSE_FIND_P_L2((index));                                             \
    WJR_REGISTER_REVERSE_FIND_P_L2((index) + 2);                                         \
    WJR_REGISTER_REVERSE_FIND_P_L2((index) + 4);                                         \
    WJR_REGISTER_REVERSE_FIND_P_L2((index) + 6)

#define WJR_REGISTER_REVERSE_FIND_P_RET() ret()

    src0 += n;
    src1 += n;

    WJR_GEN_FAST_1_2_8(WJR_REGISTER_REVERSE_FIND_P_L1, WJR_REGISTER_REVERSE_FIND_P_L2,
                       WJR_REGISTER_REVERSE_FIND_P_L8, WJR_PP_EMPTY, WJR_PP_EMPTY,
                       WJR_PP_EMPTY, WJR_REGISTER_REVERSE_FIND_P_RET);

#undef WJR_REGISTER_REVERSE_FIND_P_RET
#undef WJR_REGISTER_REVERSE_FIND_P_L8
#undef WJR_REGISTER_REVERSE_FIND_P_L2
#undef WJR_REGISTER_REVERSE_FIND_P_L1
}

template <typename T, typename Ret>
WJR_INLINE std::invoke_result_t<Ret> builtin_reverse_find_p(const T *src, T val, size_t n,
                                                            Ret ret) {
#define WJR_REGISTER_FIND_P_L1(index)                                                    \
    if (src[-1 - (index)] == val) {                                                      \
        return ret((index));                                                             \
    }
#define WJR_REGISTER_FIND_P_L2(index)                                                    \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src - 2 - (index)));                             \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return ret((index) + (mask == 0x00FF));                                      \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_FIND_P_L8(index)                                                    \
    WJR_REGISTER_FIND_P_L2((index));                                                     \
    WJR_REGISTER_FIND_P_L2((index) + 2);                                                 \
    WJR_REGISTER_FIND_P_L2((index) + 4);                                                 \
    WJR_REGISTER_FIND_P_L2((index) + 6)

#define WJR_REGISTER_FIND_P_I2(...) auto y = sse::set1_epi64(val)
#define WJR_REGISTER_FIND_P_RET() ret()

    src += n;

    WJR_GEN_FAST_1_2_8(WJR_REGISTER_FIND_P_L1, WJR_REGISTER_FIND_P_L2,
                       WJR_REGISTER_FIND_P_L8, WJR_PP_EMPTY, WJR_REGISTER_FIND_P_I2,
                       WJR_PP_EMPTY, WJR_REGISTER_FIND_P_RET);

#undef WJR_REGISTER_FIND_P_RET
#undef WJR_REGISTER_FIND_P_I2
#undef WJR_REGISTER_FIND_P_L8
#undef WJR_REGISTER_FIND_P_L2
#undef WJR_REGISTER_FIND_P_L1
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_P)

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_P)

template <typename T, typename Ret>
WJR_INLINE std::invoke_result_t<Ret>
builtin_reverse_find_not_p(const T *src0, const T *src1, size_t n, Ret ret) {
#define WJR_REGISTER_FIND_NOT_P_L1(index)                                                \
    if (src0[-1 - (index)] != src1[-1 - (index)]) {                                      \
        return ret((index));                                                             \
    }
#define WJR_REGISTER_FIND_NOT_P_L2(index)                                                \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return ret((index) + (mask == 0xFF00));                                      \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_FIND_NOT_P_L8(index)                                                \
    WJR_REGISTER_FIND_NOT_P_L2((index));                                                 \
    WJR_REGISTER_FIND_NOT_P_L2((index) + 2);                                             \
    WJR_REGISTER_FIND_NOT_P_L2((index) + 4);                                             \
    WJR_REGISTER_FIND_NOT_P_L2((index) + 6)

#define WJR_REGISTER_FIND_NOT_P_RET() ret()

    src0 += n;
    src1 += n;

    WJR_GEN_FAST_1_2_8(WJR_REGISTER_FIND_NOT_P_L1, WJR_REGISTER_FIND_NOT_P_L2,
                       WJR_REGISTER_FIND_NOT_P_L8, WJR_PP_EMPTY, WJR_PP_EMPTY,
                       WJR_PP_EMPTY, WJR_REGISTER_FIND_NOT_P_RET);

#undef WJR_REGISTER_FIND_NOT_P_RET
#undef WJR_REGISTER_FIND_NOT_P_L8
#undef WJR_REGISTER_FIND_NOT_P_L2
#undef WJR_REGISTER_FIND_NOT_P_L1
}

template <typename T, typename Ret>
WJR_INLINE std::invoke_result_t<Ret> builtin_reverse_find_not_p(const T *src, T val,
                                                                size_t n, Ret ret) {
#define WJR_REGISTER_REVERSE_FIND_NOT_P_L1(index)                                        \
    if (src[-1 - (index)] != val) {                                                      \
        return ret((index));                                                             \
    }
#define WJR_REGISTER_REVERSE_FIND_NOT_P_L2(index)                                        \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src - 2 - (index)));                             \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != sse::mask())) {                                         \
            return ret((index) + (mask == 0xFF00));                                      \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_REVERSE_FIND_NOT_P_L8(index)                                        \
    WJR_REGISTER_REVERSE_FIND_NOT_P_L2((index));                                         \
    WJR_REGISTER_REVERSE_FIND_NOT_P_L2((index) + 2);                                     \
    WJR_REGISTER_REVERSE_FIND_NOT_P_L2((index) + 4);                                     \
    WJR_REGISTER_REVERSE_FIND_NOT_P_L2((index) + 6)

#define WJR_REGISTER_REVERSE_FIND_NOT_P_I2(...) auto y = sse::set1_epi64(val)
#define WJR_REGISTER_REVERSE_FIND_NOT_P_RET() ret()

    src += n;

    WJR_GEN_FAST_1_2_8(WJR_REGISTER_REVERSE_FIND_NOT_P_L1,
                       WJR_REGISTER_REVERSE_FIND_NOT_P_L2,
                       WJR_REGISTER_REVERSE_FIND_NOT_P_L8, WJR_PP_EMPTY,
                       WJR_REGISTER_REVERSE_FIND_NOT_P_I2, WJR_PP_EMPTY,
                       WJR_REGISTER_REVERSE_FIND_NOT_P_RET);

#undef WJR_REGISTER_REVERSE_FIND_NOT_P_RET
#undef WJR_REGISTER_REVERSE_FIND_NOT_P_I2
#undef WJR_REGISTER_REVERSE_FIND_NOT_P_L8
#undef WJR_REGISTER_REVERSE_FIND_NOT_P_L2
#undef WJR_REGISTER_REVERSE_FIND_NOT_P_L1
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_NOT_P)

} // namespace wjr

#endif // WJR_X86_FIND_HPP__