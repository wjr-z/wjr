#ifndef WJR_MEMORY_SMALL_COPY_HPP__
#define WJR_MEMORY_SMALL_COPY_HPP__

#include <cstring>

#include <wjr/math/bit.hpp>

namespace wjr {
namespace detail {
namespace __scopy_detail {

#if WJR_HAS_SIMD(SSE)
    #define WJR_ENABLE_SCOPY
#endif

template <size_t Size, size_t N, size_t MaxN>
WJR_INTRINSIC_INLINE void copy(const uint_t<Size> *first, const uint_t<Size> *last,
                               uint_t<Size> *ds, uint_t<Size> *de) noexcept {
    constexpr size_t Max = std::min(N * 2, MaxN);
    constexpr size_t rest = N == Max ? 0 : constant::bit_ceil(std::min(N, Max - N));

    // Load all data first - this pattern is safe for overlapping regions
    auto x0 = __simd_load<(Size / 8) * N>(first);
    auto x1 = __simd_load<(Size / 8) * rest>(last - rest);
    // Now store the data
    __simd_store(ds, x0);
    __simd_store(de - rest, x1);
}

} // namespace __scopy_detail

template <size_t Size, size_t Min, size_t Max>
WJR_INTRINSIC_INLINE void __small_copy_impl(const uint_t<Size> *first, const uint_t<Size> *last,
                                            uint_t<Size> *dst) noexcept {
    if constexpr (Min == 0) {
        if (WJR_UNLIKELY(first == last)) {
            return;
        }
    }

    const auto n = to_unsigned(last - first);
    WJR_ASSERT_ASSUME_L3(n >= Min && n <= Max);

#if defined(WJR_ENABLE_SCOPY)
    if constexpr (Max > 8) {
        if (WJR_UNLIKELY(n > 8)) {
            std::memmove(dst, first, n * (Size / 8));
            return;
        }
    }

    if constexpr (Max > 4) {
        if (WJR_UNLIKELY(n > 4)) {
            __scopy_detail::copy<Size, 4, Max>(first, last, dst, dst + n);
            return;
        }
    }

    if constexpr (Max > 2) {
        if (WJR_LIKELY(n >= 2)) {
            __scopy_detail::copy<Size, 2, Max>(first, last, dst, dst + n);
            return;
        }
    } else if constexpr (Max == 2) {
        if (WJR_LIKELY(n == 2)) {
            __scopy_detail::copy<Size, 2, 2>(first, last, dst, dst + n);
            return;
        }
    }
#else
    if constexpr (Max > 1) {
        if (WJR_UNLIKELY(n != 1)) {
            std::memmove(dst, first, n * (Size / 8));
            return;
        }
    }
#endif

    dst[0] = first[0];
}
} // namespace detail

// todo: requires T is nonbool integral
template <size_t Min = 0, size_t Max = std::numeric_limits<size_t>::max(), typename T>
WJR_INTRINSIC_INLINE void small_copy(T const *first, T const *last, T *dst) noexcept {
    constexpr size_t Size = sizeof(T) * 8;

    detail::__small_copy_impl<Size, Min, Max>((const uint_t<Size> *)(first),
                                              (const uint_t<Size> *)(last), (uint_t<Size> *)(dst));
}
} // namespace wjr

#endif // WJR_MEMORY_SMALL_COPY_HPP__