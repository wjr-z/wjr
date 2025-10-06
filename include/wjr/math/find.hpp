/**
 * @file find.hpp
 * @brief Array search and comparison operations
 * @author wjr
 *
 * Provides optimized functions to find differences between arrays.
 * Supports both forward and reverse iteration for flexible searching.
 */

#ifndef WJR_MATH_FIND_HPP__
#define WJR_MATH_FIND_HPP__

#include <wjr/iterator/detail.hpp>
#include <wjr/math/broadcast.hpp>
#include <wjr/math/clz.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/math/find.hpp>
#endif

namespace wjr {
/// @private Internal helpers for find operations
namespace detail {
/// @private Check if iterator is reverse iterator
template <typename Iter>
struct __find_is_reverse_iter : std::false_type {};

template <typename Iter>
struct __find_is_reverse_iter<std::reverse_iterator<Iter>> : std::true_type {};

template <typename Iter>
inline constexpr bool __find_is_reverse_iter_v = __find_is_reverse_iter<Iter>::value;

/// @private Get offset for find operation considering direction
template <bool reverse>
WJR_INTRINSIC_CONSTEXPR size_t __find_get_offset(WJR_MAYBE_UNUSED size_t block,
                                                 size_t offset) noexcept {
    if constexpr (!reverse) {
        return offset;
    } else {
        return block - offset;
    }
}

template <bool reverse, typename T>
WJR_INTRINSIC_CONSTEXPR size_t __find_get_first_set(T value) noexcept {
    if constexpr (!reverse) {
        return ctz(value);
    } else {
        return clz(value);
    }
}

template <size_t B, typename Iter>
WJR_INTRINSIC_CONSTEXPR auto __find_address(Iter iter) noexcept {
    if constexpr (!__find_is_reverse_iter_v<Iter>) {
        return iter;
    } else {
        return wjr::to_address(iter + (B - 1));
    }
}

/// @private Cast pointer to appropriate type for find operations
template <typename T>
constexpr auto __find_cast_ptr(const T *ptr) {
    if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_same_v<T, bool>)
            return reinterpret_cast<const uint8_t *>(ptr);
        else
            return reinterpret_cast<const std::make_unsigned_t<T> *>(ptr);
    } else
        return ptr;
}

template <typename Iter>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_n_iter_ptr(Iter src0, Iter src1, size_t n) noexcept {
    constexpr auto reverse = __find_is_reverse_iter_v<Iter>;

    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[idx] == src1[idx]) {
            break;
        }
    }

    return __find_get_offset<reverse>(n, idx);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_find_n_impl(const T *src0, const T *src1,
                                                   size_t n) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(src0 == src1)) {
        return 0;
    }

#if WJR_HAS_BUILTIN(FIND_N)
    if constexpr (sizeof(T) == 8 && is_nonbool_integral_v<T>) {
        if (!is_constant_evaluated()) {
            return builtin_find_n(src0, src1, n);
        }
    }
#endif

    return fallback_find_n_iter_ptr(src0, src1, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_find_n(const T *src0, const T *src1, size_t n) noexcept {
    return resolve_find_n_impl(__find_cast_ptr(src0), __find_cast_ptr(src1), n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_reverse_find_n_impl(const T *src0, const T *src1,
                                                           size_t n) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(src0 == src1)) {
        return n;
    }

#if WJR_HAS_BUILTIN(REVERSE_FIND_N)
    if constexpr (sizeof(T) == 8 && is_nonbool_integral_v<T>) {
        if (!is_constant_evaluated()) {
            return builtin_reverse_find_n(src0, src1, n);
        }
    }
#endif

    return fallback_find_n_iter_ptr(std::make_reverse_iterator(src0 + n),
                                    std::make_reverse_iterator(src1 + n), n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_reverse_find_n(const T *src0, const T *src1,
                                                      size_t n) noexcept {
    return resolve_reverse_find_n_impl(__find_cast_ptr(src0), __find_cast_ptr(src1), n);
}

/// @private Large fallback implementation for finding value in iterator range
template <typename Iter, typename T>
constexpr size_t __large_fallback_find_n_iter_val_impl(Iter src, T val, size_t n) noexcept {
    constexpr bool reverse = __find_is_reverse_iter_v<Iter>;

    constexpr uint64_t repeated_one = broadcast<T, uint64_t>(0x01);
    constexpr int shift = std::numeric_limits<T>::digits - 1;
    constexpr size_t step = sizeof(uint64_t) / sizeof(T);

#define WJR_FIND_U64(offset)                                                                       \
    uint64_t longword = read_memory<uint64_t>(__find_address<step>(src + (offset))) ^ repeated_c;  \
    if (uint64_t result = (((longword - repeated_one) & ~longword) & (repeated_one << shift));     \
        result != 0) {                                                                             \
        break;                                                                                     \
    }

    size_t idx = 2;
    do {
        if (n - idx >= step) {
            const uint64_t repeated_c = broadcast<T, uint64_t>(val);
            do {
                WJR_FIND_U64(idx);
                idx += step;
            } while (n - idx >= step);
        }
    } while (0);

#undef WJR_FIND_U64

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] == val) {
            break;
        }
    }

    return __find_get_offset<reverse>(n, idx);
}

template <typename Iter, typename T>
WJR_INTRINSIC_CONSTEXPR size_t __fallback_find_n_iter_val(Iter src, T val, size_t n,
                                                          std::true_type) noexcept {
    constexpr auto reverse = __find_is_reverse_iter_v<Iter>;

    if (n == 0 || src[0] == val)
        return __find_get_offset<reverse>(n, 0);
    if (n == 1 || src[1] == val)
        return __find_get_offset<reverse>(n, 1);
    return __large_fallback_find_n_iter_val_impl(src, val, n);
}

template <typename Iter, typename T>
WJR_INTRINSIC_CONSTEXPR size_t __fallback_find_n_iter_val(Iter src, T val, size_t n,
                                                          std::false_type) noexcept {
    constexpr auto reverse = __find_is_reverse_iter_v<Iter>;

    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] == val) {
            break;
        }
    }

    return __find_get_offset<reverse>(n, idx);
}

template <typename Iter, typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_n_iter_val(Iter src, T val, size_t n) noexcept {
    return __fallback_find_n_iter_val(
        src, val, n,
        std::bool_constant<(sizeof(T) <= sizeof(uint32_t) && is_nonbool_integral_v<T>)>{});
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_find_n_impl(const T *src, type_identity_t<T> val,
                                                   size_t n) noexcept {
#if WJR_HAS_BUILTIN(FIND_N)
    if constexpr (sizeof(T) == 8 && is_nonbool_integral_v<T>) {
        if (!is_constant_evaluated()) {
            return builtin_find_n(src, val, n);
        }
    }
#endif

    return fallback_find_n_iter_val(src, val, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_find_n(const T *src, T val, size_t n) noexcept {
    return resolve_find_n_impl(__find_cast_ptr(src), val, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_reverse_find_n_impl(const T *src, T val, size_t n) noexcept {
#if WJR_HAS_BUILTIN(FIND_N)
    if constexpr (sizeof(T) == 8 && is_nonbool_integral_v<T>) {
        if (!is_constant_evaluated()) {
            return builtin_reverse_find_n(src, val, n);
        }
    }
#endif

    return fallback_find_n_iter_val(std::make_reverse_iterator(src + n), val, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_reverse_find_n(const T *src, T val, size_t n) noexcept {
    return resolve_reverse_find_n_impl(__find_cast_ptr(src), val, n);
}

template <typename Iter>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_not_n_iter_ptr(Iter src0, Iter src1,
                                                            size_t n) noexcept {
    constexpr auto reverse = __find_is_reverse_iter_v<Iter>;

    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[idx] != src1[idx]) {
            break;
        }
    }

    return __find_get_offset<reverse>(n, idx);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_find_not_n_impl(const T *src0, const T *src1,
                                                       size_t n) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(src0 == src1)) {
        return n;
    }

#if WJR_HAS_BUILTIN(FIND_NOT_N)
    if constexpr (sizeof(T) == 8 && is_nonbool_integral_v<T>) {
        if (!is_constant_evaluated()) {
            return builtin_find_not_n(src0, src1, n);
        }
    }
#endif

    return fallback_find_not_n_iter_ptr(src0, src1, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_find_not_n(const T *src0, const T *src1, size_t n) noexcept {
    return resolve_find_not_n_impl(__find_cast_ptr(src0), __find_cast_ptr(src1), n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_reverse_find_not_n_impl(const T *src0, const T *src1,
                                                               size_t n) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(src0 == src1)) {
        return 0;
    }

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)
    if constexpr (sizeof(T) == 8 && is_nonbool_integral_v<T>) {
        if (!is_constant_evaluated()) {
            return builtin_reverse_find_not_n(src0, src1, n);
        }
    }
#endif

    return fallback_find_not_n_iter_ptr(std::make_reverse_iterator(src0 + n),
                                        std::make_reverse_iterator(src1 + n), n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_reverse_find_not_n(const T *src0, const T *src1,
                                                          size_t n) noexcept {
    return resolve_reverse_find_not_n_impl(__find_cast_ptr(src0), __find_cast_ptr(src1), n);
}

/// @private Large fallback implementation for finding non-matching value in iterator range
template <typename Iter, typename T>
constexpr size_t __large_fallback_find_not_n_iter_val_impl(Iter src, T val, size_t n) noexcept {
    constexpr bool reverse = __find_is_reverse_iter_v<Iter>;

    constexpr size_t step = sizeof(uint64_t) / sizeof(T);

#define WJR_FIND_U64(offset)                                                                       \
    uint64_t longword = read_memory<uint64_t>(__find_address<step>(src + (offset)));               \
    if (longword != repeated_c) {                                                                  \
        break;                                                                                     \
    }

    size_t idx = 2;
    do {
        if (n - idx >= step) {
            const uint64_t repeated_c = broadcast<T, uint64_t>(val);
            do {
                WJR_FIND_U64(idx);
                idx += step;
            } while (n - idx >= step);
        }
    } while (0);

#undef WJR_FIND_U64

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != val) {
            break;
        }
    }

    return __find_get_offset<reverse>(n, idx);
}

template <typename Iter, typename T>
WJR_INTRINSIC_CONSTEXPR size_t __fallback_find_not_n_iter_val(Iter src, T val, size_t n,
                                                              std::true_type) noexcept {
    constexpr auto reverse = __find_is_reverse_iter_v<Iter>;

    if (n == 0 || src[0] != val)
        return __find_get_offset<reverse>(n, 0);
    if (n == 1 || src[1] != val)
        return __find_get_offset<reverse>(n, 1);
    return __large_fallback_find_not_n_iter_val_impl(src, val, n);
}

template <typename Iter, typename T>
WJR_INTRINSIC_CONSTEXPR size_t __fallback_find_not_n_iter_val(Iter src, T val, size_t n,
                                                              std::false_type) noexcept {
    constexpr auto reverse = __find_is_reverse_iter_v<Iter>;

    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != val) {
            break;
        }
    }

    return __find_get_offset<reverse>(n, idx);
}

template <typename Iter, typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_not_n_iter_val(Iter src, T val, size_t n) noexcept {
    return __fallback_find_not_n_iter_val(
        src, val, n,
        std::bool_constant<(sizeof(T) <= sizeof(uint32_t) && is_nonbool_integral_v<T>)>{});
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_find_not_n_impl(const T *src, type_identity_t<T> val,
                                                       size_t n) noexcept {
#if WJR_HAS_BUILTIN(FIND_NOT_N)
    if constexpr (sizeof(T) == 8 && is_nonbool_integral_v<T>) {
        if (!is_constant_evaluated()) {
            return builtin_find_not_n(src, val, n);
        }
    }
#endif

    return fallback_find_not_n_iter_val(src, val, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_find_not_n(const T *src, T val, size_t n) noexcept {
    return resolve_find_not_n_impl(__find_cast_ptr(src), val, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_reverse_find_not_n_impl(const T *src, T val,
                                                               size_t n) noexcept {
#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)
    if constexpr (sizeof(T) == 8 && is_nonbool_integral_v<T>) {
        if (!is_constant_evaluated()) {
            return builtin_reverse_find_not_n(src, val, n);
        }
    }
#endif

    return fallback_find_not_n_iter_val(std::make_reverse_iterator(src + n), val, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t resolve_reverse_find_not_n(const T *src, T val, size_t n) noexcept {
    return resolve_reverse_find_not_n_impl(__find_cast_ptr(src), val, n);
}

} // namespace detail

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 size_t find_n(const T *src0, const T *src1, size_t n) noexcept {
    return detail::resolve_find_n(src0, src1, n);
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 size_t find_n(const T *src, type_identity_t<T> val,
                                                 size_t n) noexcept {
    return detail::resolve_find_n(src, val, n);
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 size_t find_not_n(const T *src0, const T *src1,
                                                     size_t n) noexcept {
    return detail::resolve_find_not_n(src0, src1, n);
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 size_t find_not_n(const T *src, type_identity_t<T> val,
                                                     size_t n) noexcept {
    return detail::resolve_find_not_n(src, val, n);
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 size_t reverse_find_n(const T *src0, const T *src1,
                                                         size_t n) noexcept {
    return detail::resolve_reverse_find_n(src0, src1, n);
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 size_t reverse_find_n(const T *src, type_identity_t<T> val,
                                                         size_t n) noexcept {
    return detail::resolve_reverse_find_n(src, val, n);
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 size_t reverse_find_not_n(const T *src0, const T *src1,
                                                             size_t n) noexcept {
    return detail::resolve_reverse_find_not_n(src0, src1, n);
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 size_t reverse_find_not_n(const T *src, type_identity_t<T> val,
                                                             size_t n) noexcept {
    return detail::resolve_reverse_find_not_n(src, val, n);
}

} // namespace wjr

#endif // WJR_MATH_FIND_HPP__
