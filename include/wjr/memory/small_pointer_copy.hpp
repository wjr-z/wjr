#ifndef WJR_MEMORY_COPY_SMALL_POINTER_HPP__
#define WJR_MEMORY_COPY_SMALL_POINTER_HPP__

#include <wjr/type_traits.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/memory/small_pointer_copy.hpp>
#endif

namespace wjr {

template <size_t Min, size_t Max>
WJR_INTRINSIC_INLINE void fallback_small_pointer_copy(const void *const *first,
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
            char x0[32];
            char x1[32];
            builtin_memcpy(x0, first, 32);
            builtin_memcpy(x1, last - 4, 32);
            builtin_memcpy(dst, x0, 32);
            builtin_memcpy(dst + n - 4, x1, 32);
            return;
        }
    }

    if constexpr (Max > 2) {
        if (WJR_LIKELY(n >= 2)) {
            char x0[16];
            char x1[16];
            builtin_memcpy(x0, first, 16);
            builtin_memcpy(x1, last - 2, 16);
            builtin_memcpy(dst, x0, 16);
            builtin_memcpy(dst + n - 2, x1, 16);
            return;
        }
    }

    dst[0] = first[0];
}

template <size_t Min, size_t Max, typename Ptr>
WJR_INTRINSIC_INLINE void small_pointer_copy(Ptr const *first, Ptr const *last, Ptr *dst) noexcept {
    static_assert(Max > 2 && Max <= 8);

#if WJR_HAS_BUILTIN(SMALL_POINTER_COPY)
    builtin_small_pointer_copy<Min, Max>((const void *const *)(first), (const void *const *)(last),
                                         (const void **)(dst));
#else
    fallback_small_pointer_copy<Min, Max>((const void *const *)(first), (const void *const *)(last),
                                          (const void **)(dst));
#endif
}
} // namespace wjr

#endif // WJR_MEMORY_COPY_SMALL_POINTER_HPP__