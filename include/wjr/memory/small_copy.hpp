#ifndef WJR_MEMORY_COPY_SMALL_COPY_HPP__
#define WJR_MEMORY_COPY_SMALL_COPY_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {
namespace __scopy_detail {

template <size_t Size>
WJR_INTRINSIC_INLINE void __small_copy_impl(const char *first, const char *last, char *ds,
                                            char *de) noexcept {
    constexpr size_t batch = Size / 16;
    __simd_storage_t<std::byte, batch> x0, x1;
    builtin_memcpy(&x0, first, batch);
    builtin_memcpy(&x1, last - batch, batch);
    builtin_memcpy(ds, &x0, batch);
    builtin_memcpy(de - batch, &x1, batch);
}

template <size_t Size>
WJR_INTRINSIC_INLINE void __small_copy(const void *first, const void *last, void *ds,
                                       void *de) noexcept {
    __small_copy_impl<Size>((const char *)first, (const char *)last, (char *)ds, (char *)de);
}

} // namespace __scopy_detail

template <size_t Size, size_t Min, size_t Max>
WJR_INTRINSIC_INLINE void fallback_small_copy(const uint_t<Size> *first, const uint_t<Size> *last,
                                              uint_t<Size> *dst) noexcept {
    if constexpr (Min == 0) {
        if (WJR_UNLIKELY(first == last)) {
            return;
        }
    }

    const auto n = to_unsigned(last - first);
    WJR_ASSUME(n >= Min && n <= Max);

    if constexpr (Max > 4) {
        if (WJR_UNLIKELY(n > 4)) {
            __scopy_detail::__small_copy<Size * 8>(first, last, dst, dst + n);
            return;
        }
    }

    if constexpr (Max > 2) {
        if (WJR_LIKELY(n >= 2)) {
            __scopy_detail::__small_copy<Size * 4>(first, last, dst, dst + n);
            return;
        }
    }

    dst[0] = first[0];
}

template <size_t Min, size_t Max, typename T>
WJR_INTRINSIC_INLINE void small_copy(T const *first, T const *last, T *dst) noexcept {
    static_assert(Max > 2 && Max <= 8);
    constexpr size_t Size = sizeof(T) * 8;

    fallback_small_copy<Size, Min, Max>((const uint_t<Size> *)(first), (const uint_t<Size> *)(last),
                                        (uint_t<Size> *)(dst));
}
} // namespace wjr

#endif // WJR_MEMORY_COPY_SMALL_COPY_HPP__