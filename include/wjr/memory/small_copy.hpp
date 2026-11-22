#ifndef WJR_MEMORY_SMALL_COPY_HPP__
#define WJR_MEMORY_SMALL_COPY_HPP__

#include <cstring>

#include <wjr/math/bit.hpp>

namespace wjr {
/// @private Internal implementation details for memory operations
namespace detail {
/// @private Small copy optimization details using SIMD instructions
namespace _scopy_detail {

#if WJR_HAS_SIMD(SSE)
    #define WJR_ENABLE_SCOPY
#endif

#if WJR_HAS_SIMD(AVX2)
    #define WJR_ENABLE_SCOPY_16
#endif

/// @private SIMD-based copy for small fixed-size ranges
template <size_t N, size_t MaxN, typename T>
WJR_INTRINSIC_INLINE void copy(const T *first, const T *last, T *ds, T *de) noexcept {
    constexpr size_t Max = std::min(N * 2, MaxN);
    constexpr size_t rest = N == Max ? 0 : constant::bit_ceil(std::min(N, Max - N));

    // Load all data first - this pattern is safe for overlapping regions
    auto x0 = _simd_load<sizeof(T) * N>(first);
    auto x1 = _simd_load<sizeof(T) * rest>(last - rest);
    // Now store the data
    _simd_store(ds, x0);
    _simd_store(de - rest, x1);
}

} // namespace _scopy_detail

/// @private Implementation for small copy optimization based on size range
template <size_t Min, size_t Max, typename T>
WJR_INTRINSIC_INLINE void _small_copy_impl(const T *first, const T *last, T *dst) noexcept {
    if constexpr (Min == 0) {
        if (WJR_UNLIKELY(first == last)) {
            return;
        }
    }

    const auto n = to_unsigned(last - first);
    WJR_ASSERT_ASSUME_L3(n >= Min && n <= Max);

#if defined(WJR_ENABLE_SCOPY)
    #if defined(WJR_ENABLE_SCOPY_16)
    if constexpr (sizeof(T) <= 4) {
        if constexpr (Max > 16) {
            if (WJR_UNLIKELY(n > 16)) {
                std::memmove(dst, first, n * sizeof(T));
                return;
            }
        }

        if constexpr (Max > 8) {
            if (WJR_UNLIKELY(n > 8)) {
                _scopy_detail::copy<8, Max>(first, last, dst, dst + n);
                return;
            }
        }
    } else {
    #endif
        if constexpr (Max > 8) {
            if (WJR_UNLIKELY(n > 8)) {
                std::memmove(dst, first, n * sizeof(T));
                return;
            }
        }
    #if defined(WJR_ENABLE_SCOPY_16)
    }
    #endif

    if constexpr (Max > 4) {
        if (WJR_UNLIKELY(n > 4)) {
            _scopy_detail::copy<4, Max>(first, last, dst, dst + n);
            return;
        }
    }

    if constexpr (Max > 2) {
        if (WJR_LIKELY(n >= 2)) {
            _scopy_detail::copy<2, Max>(first, last, dst, dst + n);
            return;
        }
    } else if constexpr (Max == 2) {
        if (WJR_LIKELY(n == 2)) {
            _scopy_detail::copy<2, 2>(first, last, dst, dst + n);
            return;
        }
    }
#else
    if constexpr (Max > 1) {
        if (WJR_UNLIKELY(n != 1)) {
            std::memmove(dst, first, n * sizeof(T));
            return;
        }
    }
#endif

    if constexpr (std::is_scalar_v<T>) {
        dst[0] = first[0];
    } else {
        _simd_store(dst, _simd_load<sizeof(T)>(first));
    }
}
} // namespace detail

/// @brief Copy a small range of trivially copyable objects, including overlapping ranges.
/// @pre The source has between Min and Max elements and the destination has room for them.
template <size_t Min = 0, size_t Max = std::numeric_limits<size_t>::max(), typename T>
WJR_INTRINSIC_INLINE void small_copy(T const *first, T const *last, T *dst) noexcept {
    detail::_small_copy_impl<Min, Max>(first, last, dst);
}
} // namespace wjr

#endif // WJR_MEMORY_SMALL_COPY_HPP__