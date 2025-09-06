#ifndef WJR_ARCH_X86_MATH_FIND_HPP__
#define WJR_ARCH_X86_MATH_FIND_HPP__

#include <wjr/arch/x86/math/large-find-impl.hpp>

namespace wjr {

#if WJR_HAS_BUILTIN(FIND_N)

template <typename T>
WJR_PURE WJR_INTRINSIC_INLINE size_t builtin_find_n(const T *src0, const T *src1,
                                                    size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0 || src0[0] == src1[0])) {
        return 0;
    }

    if (WJR_UNLIKELY(n == 1 || src0[1] == src1[1])) {
        return 1;
    }

    if (WJR_UNLIKELY(n == 2 || src0[2] == src1[2])) {
        return 2;
    }

    if (WJR_UNLIKELY(n == 3 || src0[3] == src1[3])) {
        return 3;
    }

    if (WJR_UNLIKELY(n == 4)) {
        return 4;
    }

    size_t ret = large_builtin_find_n(src0, src1, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_INLINE size_t builtin_find_n(const T *src, T val, size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0 || src[0] == val)) {
        return 0;
    }

    if (WJR_UNLIKELY(n == 1 || src[1] == val)) {
        return 1;
    }

    if (WJR_UNLIKELY(n == 2 || src[2] == val)) {
        return 2;
    }

    if (WJR_UNLIKELY(n == 3 || src[3] == val)) {
        return 3;
    }

    if (WJR_UNLIKELY(n == 4)) {
        return 4;
    }

    size_t ret = large_builtin_find_n(src, val, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(FIND_N)

#if WJR_HAS_BUILTIN(FIND_NOT_N)

template <typename T>
WJR_PURE WJR_INTRINSIC_INLINE size_t builtin_find_not_n(const T *src0, const T *src1,
                                                        size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src0[0] != src1[0])) {
        return 0;
    }

    if (WJR_UNLIKELY(n == 1) || WJR_LIKELY(src0[1] != src1[1])) {
        return 1;
    }

    if (WJR_UNLIKELY(n == 2)) {
        return 2;
    }

    const size_t ret = large_builtin_find_not_n(src0, src1, n);
    WJR_ASSUME(ret >= 2 && ret <= n);
    return ret;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_INLINE size_t builtin_find_not_n(const T *src, T val, size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src[0] != val)) {
        return 0;
    }

    if (WJR_UNLIKELY(n == 1) || WJR_LIKELY(src[1] != val)) {
        return 1;
    }

    if (WJR_UNLIKELY(n == 2)) {
        return 2;
    }

    const size_t ret = large_builtin_find_not_n(src, val, n);
    WJR_ASSUME(ret >= 2 && ret <= n);
    return ret;
}

#endif // WJR_HAS_BUILTIN(FIND_NOT_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_N)

template <typename T>
WJR_PURE WJR_INTRINSIC_INLINE size_t builtin_reverse_find_n(const T *src0, const T *src1,
                                                            size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0 || src0[n - 1] == src1[n - 1])) {
        return n;
    }

    if (WJR_UNLIKELY(n == 1 || src0[n - 2] == src1[n - 2])) {
        return n - 1;
    }

    if (WJR_UNLIKELY(n == 2 || src0[n - 3] == src1[n - 3])) {
        return n - 2;
    }

    if (WJR_UNLIKELY(n == 3 || src0[n - 4] == src1[n - 4])) {
        return n - 3;
    }

    if (WJR_UNLIKELY(n == 4)) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_n(src0, src1, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_INLINE size_t builtin_reverse_find_n(const T *src, T val,
                                                            size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0 || src[n - 1] == val)) {
        return n;
    }

    if (WJR_UNLIKELY(n == 1 || src[n - 2] == val)) {
        return n - 1;
    }

    if (WJR_UNLIKELY(n == 2 || src[n - 3] == val)) {
        return n - 2;
    }

    if (WJR_UNLIKELY(n == 3 || src[n - 4] == val)) {
        return n - 3;
    }

    if (WJR_UNLIKELY(n == 4)) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_n(src, val, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_not_n(const T *src0, const T *src1,
                                                       size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src0[n - 1] != src1[n - 1])) {
        return n;
    }

    if (WJR_UNLIKELY(n == 1) || WJR_LIKELY(src0[n - 2] != src1[n - 2])) {
        return n - 1;
    }

    if (WJR_UNLIKELY(n == 2)) {
        return n - 2;
    }

    const size_t ret = large_builtin_reverse_find_not_n(src0, src1, n);
    WJR_ASSUME(ret >= 0 && ret <= n - 2);
    return ret;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_INLINE size_t builtin_reverse_find_not_n(const T *src, T val,
                                                                size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src[n - 1] != val)) {
        return n;
    }

    if (WJR_UNLIKELY(n == 1) || WJR_LIKELY(src[n - 2] != val)) {
        return n - 1;
    }

    if (WJR_UNLIKELY(n == 2)) {
        return n - 2;
    }

    const size_t ret = large_builtin_reverse_find_not_n(src, val, n);
    WJR_ASSUME(ret >= 0 && ret <= n - 2);
    return ret;
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_FIND_HPP__