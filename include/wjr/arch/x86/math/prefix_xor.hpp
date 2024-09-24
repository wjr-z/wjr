#ifndef WJR_ARCH_X86_MATH_PREFIX_XOR_HPP__
#define WJR_ARCH_X86_MATH_PREFIX_XOR_HPP__

#include <wjr/arch/x86/simd/simd.hpp>
#include <wjr/preprocessor.hpp>

namespace wjr {

#if WJR_HAS_SIMD(PCLMUL)
    #define WJR_HAS_BUILTIN_PREFIX_XOR WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(PREFIX_XOR)

template <typename T>
WJR_CONST T __builtin_prefix_xor(T x) noexcept {
    const __m128i __result =
        _mm_clmulepi64_si128((simd_cast<T, __m128i_t>(x)), sse::set1_epi8(0xFF), 0);
    return simd_cast<__m128i_t, T>(__result);
}

template <typename T>
WJR_CONST T builtin_prefix_xor(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    if constexpr (nd < 32) {
        return static_cast<T>(__builtin_prefix_xor(static_cast<uint32_t>(x)));
    } else {
        return __builtin_prefix_xor(x);
    }
}

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_PREFIX_XOR_HPP__