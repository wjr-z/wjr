#ifndef WJR_X86_MATH_PREFIX_XOR_HPP__
#define WJR_X86_MATH_PREFIX_XOR_HPP__

#include <wjr/preprocessor.hpp>
#include <wjr/x86/simd/simd.hpp>

namespace wjr {

#if WJR_HAS_SIMD(PCLMUL) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_PREFIX_XOR WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(PREFIX_XOR)

template <typename T>
T __builtin_prefix_xor(T x) {
    __m128i __result =
        _mm_clmulepi64_si128((simd_cast<T, __m128i_t>(x)), sse::set1_epi8(0xFF), 0);
    return simd_cast<__m128i_t, T>(__result);
}

template <typename T>
T builtin_prefix_xor(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    if constexpr (nd < 32) {
        return static_cast<T>(__builtin_prefix_xor(static_cast<uint32_t>(x)));
    } else {
        return __builtin_prefix_xor(x);
    }
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_PREFIX_XOR_HPP__