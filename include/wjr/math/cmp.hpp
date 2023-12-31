#ifndef WJR_MATH_CMP_HPP__
#define WJR_MATH_CMP_HPP__

#include <simd/simd.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR int fallback_rcmp_n(const T *src0, const T *src1, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        auto a = src0[n - i - 1];
        auto b = src1[n - i - 1];

        if (a != b) {
            return a < b ? -1 : 1;
        }
    }

    return 0;
}

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_RCMP_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(RCMP_N)

template <typename T>
WJR_INTRINSIC_INLINE int builtin_rcmp_n(const T *src0, const T *src1, size_t n) {}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR int rcmp_n(const T *src0, const T *src1, size_t n) {

    return fallback_rcmp_n(src0, src1, n);
}

} // namespace wjr

#endif // WJR_MATH_CMP_HPP__