#ifndef WJR_ARCH_X86_MEMORY_COPY_HPP__
#define WJR_ARCH_X86_MEMORY_COPY_HPP__

#include <wjr/arch/x86/simd/simd.hpp>

namespace wjr {

#if WJR_HAS_SIMD(SSE2)
    #define WJR_HAS_BUILTIN_SMALL_POINTER_COPY WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SMALL_POINTER_COPY)

template <size_t Min, size_t Max>
WJR_INTRINSIC_INLINE void builtin_small_pointer_copy(const void *const *first,
                                                     const void *const *last,
                                                     const void **dst) noexcept {
    if constexpr (Min == 0) {
        if (WJR_UNLIKELY(first == last)) {
            return;
        }
    }

    const auto n = to_unsigned(last - first);
    WJR_ASSUME(n >= Min && n <= Max);

    if constexpr (Max > 4) {
        if (WJR_UNLIKELY(n > 4)) {
    #if WJR_HAS_SIMD(AVX2)
            auto x0 = avx::loadu(first);
            auto x1 = avx::loadu(last - 4);
            avx::storeu(dst, x0);
            avx::storeu(dst + (n - 4), x1);
    #else
            auto x0 = sse::loadu(first);
            auto x1 = sse::loadu(first + 2);
            auto x2 = sse::loadu(last - 4);
            auto x3 = sse::loadu(last - 2);
            sse::storeu(dst, x0);
            sse::storeu(dst + 2, x1);
            sse::storeu(dst + (n - 4), x2);
            sse::storeu(dst + (n - 2), x3);
    #endif
            return;
        }
    }

    if constexpr (Max > 2) {
        if (WJR_LIKELY(n >= 2)) {
            auto x0 = sse::loadu(first);
            auto x1 = sse::loadu(last - 2);
            sse::storeu(dst, x0);
            sse::storeu(dst + (n - 2), x1);
            return;
        }
    }

    dst[0] = first[0];
}

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_MEMORY_COPY_HPP__