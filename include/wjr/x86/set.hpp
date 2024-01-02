#ifndef WJR_X86_SET_HPP__
#define WJR_X86_SET_HPP__

#include <wjr/gen/nofast_1_2_8.hpp>
#include <wjr/simd/simd.hpp>

#if !defined(WJR_X86)
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_SET_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SET_N)

template <typename T>
WJR_INLINE void builtin_set_n(T *dst, T val, size_t n) {
#define WJR_REGISTER_SET_N_L1(index) dst[(index)] = val
#define WJR_REGISTER_SET_N_L2(index)                                                     \
    do {                                                                                 \
        sse::storeu((__m128i *)(dst + (index)), y);                                      \
    } while (0)
#define WJR_REGISTER_SET_N_L8(index)                                                     \
    WJR_REGISTER_SET_N_L2((index));                                                      \
    WJR_REGISTER_SET_N_L2((index) + 2);                                                  \
    WJR_REGISTER_SET_N_L2((index) + 4);                                                  \
    WJR_REGISTER_SET_N_L2((index) + 6);

#define WJR_REGISTER_SET_N_I2(...) auto y = sse::set1_epi64(val);

    WJR_GEN_NOFAST_1_2_8(WJR_REGISTER_SET_N_L1, WJR_REGISTER_SET_N_L2,
                         WJR_REGISTER_SET_N_L8, WJR_PP_EMPTY, WJR_REGISTER_SET_N_I2,
                         WJR_PP_EMPTY, WJR_PP_EMPTY);

#undef WJR_REGISTER_SET_N_I2
#undef WJR_REGISTER_SET_N_L8
#undef WJR_REGISTER_SET_N_L2
#undef WJR_REGISTER_SET_N_L1
}

#endif

} // namespace wjr

#endif // WJR_X86_SET_HPP__