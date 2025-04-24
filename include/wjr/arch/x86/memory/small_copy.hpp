#ifndef WJR_ARCH_X86_MEMORY_SMALL_COPY_HPP__
#define WJR_ARCH_X86_MEMORY_SMALL_COPY_HPP__

#include <wjr/arch/x86/simd/simd.hpp>

namespace wjr {

#if WJR_HAS_SIMD(SSE2)
    #define WJR_HAS_BUILTIN_SMALL_COPY WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SMALL_COPY)

namespace __scopy_detail {

template <size_t Size>
struct __builtin_scopy {
    static constexpr bool value = false;
};

template <>
struct __builtin_scopy<512> {
    static constexpr bool value = false;

    static WJR_INTRINSIC_INLINE void copy(const char *first, const char *last, char *ds,
                                          char *de) noexcept {
    #if WJR_HAS_SIMD(AVX2)
        auto x0 = avx::loadu(first);
        auto x1 = avx::loadu(last - 32);
        avx::storeu(ds, x0);
        avx::storeu(de - 32, x1);
    #else
        auto x0 = sse::loadu(first);
        auto x1 = sse::loadu(first + 16);
        auto x2 = sse::loadu(last - 32);
        auto x3 = sse::loadu(last - 16);
        sse::storeu(ds, x0);
        sse::storeu(ds + 16, x1);
        sse::storeu(de - 32, x2);
        sse::storeu(de - 16, x3);
    #endif
    }
};

template <>
struct __builtin_scopy<256> {
    static constexpr bool value = false;

    static WJR_INTRINSIC_INLINE void copy(const char *first, const char *last, char *ds,
                                          char *de) noexcept {
        auto x0 = sse::loadu(first);
        auto x1 = sse::loadu(last - 16);
        sse::storeu(ds, x0);
        sse::storeu(de - 16, x1);
    }
};

} // namespace __scopy_detail

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_MEMORY_SMALL_COPY_HPP__