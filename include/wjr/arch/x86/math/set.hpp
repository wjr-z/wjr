#ifndef WJR_ARCH_X86_MATH_SET_HPP__
#define WJR_ARCH_X86_MATH_SET_HPP__

#include <cstring>

#include <wjr/arch/x86/simd/simd.hpp>
#include <wjr/math/rotate.hpp>

namespace wjr {

#if WJR_HAS_SIMD(SSE2)
    #define WJR_HAS_BUILTIN_SET_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SET_N)

namespace set_detail {
inline constexpr bool is_avx = WJR_HAS_SIMD(AVX);
using simd = std::conditional_t<is_avx, avx, sse>;
} // namespace set_detail

template <typename simd, typename T>
WJR_HOT extern void large_builtin_set_n(T *dst, T val, size_t n) noexcept;

extern template void large_builtin_set_n<set_detail::simd, uint16_t>(uint16_t *, uint16_t,
                                                                     size_t) noexcept;
extern template void large_builtin_set_n<set_detail::simd, uint32_t>(uint32_t *, uint32_t,
                                                                     size_t) noexcept;
extern template void large_builtin_set_n<set_detail::simd, uint64_t>(uint64_t *, uint64_t,
                                                                     size_t) noexcept;

template <typename T>
WJR_INTRINSIC_INLINE void builtin_set_n(T *dst, T val, size_t n) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto is_avx = set_detail::is_avx;

    using simd = set_detail::simd;
    constexpr auto type_width = simd::width() / nd;

    constexpr auto sse_width = sse::width();
    constexpr auto sse_loop = sse_width / nd;

    if (WJR_UNLIKELY(n == 0)) {
        return;
    }

    if (WJR_LIKELY(n >= type_width)) {
        if (WJR_BUILTIN_CONSTANT_P(n) && n <= type_width * 8) {
    #if defined(WJR_COMPILER_CLANG)
            for (size_t i = 0; i < n; ++i) {
                dst[i] = val;
            }
    #else
            const auto y = simd::set1(val, T());
            if (n > type_width * 4) {
                simd::storeu(dst, y);
                simd::storeu(dst + type_width, y);
                simd::storeu(dst + type_width * 2, y);
                simd::storeu(dst + type_width * 3, y);
                simd::storeu(dst + n - type_width * 4, y);
                simd::storeu(dst + n - type_width * 3, y);
                simd::storeu(dst + n - type_width * 2, y);
                simd::storeu(dst + n - type_width, y);
                return;
            }

            if (n > type_width * 2) {
                simd::storeu(dst, y);
                simd::storeu(dst + type_width, y);
                simd::storeu(dst + n - type_width * 2, y);
                simd::storeu(dst + n - type_width, y);
                return;
            }

            simd::storeu(dst, y);
            simd::storeu(dst + n - type_width, y);
    #endif
            return;
        }

        // todo : Use memset or this function.
        return large_builtin_set_n<simd>(dst, val, n);
    }

    const auto y = sse::set1(val, T());
    const auto x = simd_cast<typename sse::int_tag_type, uint64_t>(y);

    if constexpr (is_avx) {
        if (WJR_UNLIKELY(n >= sse_loop)) {
            sse::storeu(dst, y);
            sse::storeu(dst + n - sse_loop, y);
            return;
        }
    }

    if constexpr (sse_loop == 2) {
        builtin_memcpy(dst, &x, 8);
        return;
    } else {
        if (WJR_UNLIKELY(n >= sse_loop / 2)) {
            builtin_memcpy(dst, &x, 8);
            builtin_memcpy(dst + n - sse_loop / 2, &x, 8);
            return;
        }
    }

    if (WJR_UNLIKELY(n >= sse_loop)) {
        sse::storeu(dst, y);
        sse::storeu(dst + n - sse_loop, y);
        return;
    }

    if constexpr (sse_loop == 2) {
        builtin_memcpy(dst, &x, 8);
        return;
    } else {
        if (WJR_UNLIKELY(n >= sse_loop / 2)) {
            builtin_memcpy(dst, &x, 8);
            builtin_memcpy(dst + n - sse_loop / 2, &x, 8);
            return;
        }
    }

    if constexpr (sse_loop >= 4) {
        if constexpr (sse_loop == 4) {
            builtin_memcpy(dst, &x, 4);
            return;
        } else {
            if (WJR_UNLIKELY(n >= sse_loop / 4)) {
                builtin_memcpy(dst, &x, 4);
                builtin_memcpy(dst + n - sse_loop / 4, &x, 4);
                return;
            }
        }
    }

    if constexpr (sse_loop >= 8) {
        if constexpr (sse_loop == 8) {
            builtin_memcpy(dst, &x, 2);
            return;
        } else {
            if (WJR_UNLIKELY(n >= sse_loop / 8)) {
                builtin_memcpy(dst, &x, 2);
                builtin_memcpy(dst + n - sse_loop / 8, &x, 2);
                return;
            }
        }
    }

    if constexpr (sse_loop >= 16) {
        if constexpr (sse_loop == 16) {
            builtin_memcpy(dst, &x, 1);
            return;
        } else {
            if (WJR_UNLIKELY(n >= sse_loop / 16)) {
                builtin_memcpy(dst, &x, 16);
                builtin_memcpy(dst + n - sse_loop / 16, &x, 16);
                return;
            }
        }
    }
}

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_SET_HPP__
