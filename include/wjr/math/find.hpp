#ifndef WJR_MATH_FIND_HPP__
#define WJR_MATH_FIND_HPP__

#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
#include <wjr/x86/find.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_n(const T *src0, const T *src1, size_t n) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[idx] == src1[idx]) {
            break;
        }
    }

    return idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t find_n(const T *src0, const T *src1, size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return 0;
    }

#if WJR_HAS_BUILTIN(FIND_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_n(src0, src1, n);
        }

        return builtin_find_n(src0, src1, n);
    } else {
        return fallback_find_n(src0, src1, n);
    }
#else
    return fallback_find_n(src0, src1, n);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_n(const T *src, T val, size_t n) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] == val) {
            break;
        }
    }

    return idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t find_n(const T *src, type_identity_t<T> val, size_t n) {
#if WJR_HAS_BUILTIN(FIND_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_n(src, val, n);
        }

        return builtin_find_n(src, val, n);
    } else {
        return fallback_find_n(src, val, n);
    }
#else
    return fallback_find_n(src, val, n);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_not_n(const T *src0, const T *src1,
                                                   size_t n) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[idx] != src1[idx]) {
            break;
        }
    }

    return idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t find_not_n(const T *src0, const T *src1, size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return n;
    }

#if WJR_HAS_BUILTIN(FIND_NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_not_n(src0, src1, n);
        }

        return builtin_find_not_n(src0, src1, n);
    } else {
        return fallback_find_not_n(src0, src1, n);
    }
#else
    return fallback_find_not_n(src0, src1, n);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_not_n(const T *src, T val, size_t n) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != val) {
            break;
        }
    }

    return idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t find_not_n(const T *src, type_identity_t<T> val,
                                            size_t n) {
#if WJR_HAS_BUILTIN(FIND_NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_not_n(src, val, n);
        }

        return builtin_find_not_n(src, val, n);
    } else {
        return fallback_find_not_n(src, val, n);
    }
#else
    return fallback_find_not_n(src, val, n);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_reverse_find_n(const T *src0, const T *src1,
                                                       size_t n) {
    size_t idx = 0;

    src0 += n;
    src1 += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[-1 - idx] == src1[-1 - idx]) {
            break;
        }
    }

    return n - idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_n(const T *src0, const T *src1, size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return n;
    }

#if WJR_HAS_BUILTIN(REVERSE_FIND_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_n(src0, src1, n);
        }

        return builtin_reverse_find_n(src0, src1, n);
    } else {
        return fallback_reverse_find_n(src0, src1, n);
    }
#else
    return fallback_reverse_find_n(src0, src1, n);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_reverse_find_n(const T *src, T val, size_t n) {
    size_t idx = 0;

    src += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[-1 - idx] == val) {
            break;
        }
    }

    return n - idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_n(const T *src, type_identity_t<T> val,
                                                size_t n) {
#if WJR_HAS_BUILTIN(REVERSE_FIND_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_n(src, val, n);
        }

        return builtin_reverse_find_n(src, val, n);
    } else {
        return fallback_reverse_find_n(src, val, n);
    }
#else
    return fallback_reverse_find_n(src, val, n);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_reverse_find_not_n(const T *src0, const T *src1,
                                                           size_t n) {
    size_t idx = 0;

    src0 += n;
    src1 += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[-1 - idx] != src1[-1 - idx]) {
            break;
        }
    }

    return n - idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_not_n(const T *src0, const T *src1,
                                                    size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return 0;
    }

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_not_n(src0, src1, n);
        }

        return builtin_reverse_find_not_n(src0, src1, n);
    } else {
        return fallback_reverse_find_not_n(src0, src1, n);
    }
#else
    return fallback_reverse_find_not_n(src0, src1, n);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_reverse_find_not_n(const T *src, T val,
                                                           size_t n) {
    size_t idx = 0;

    src += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[-1 - idx] != val) {
            break;
        }
    }

    return n - idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_not_n(const T *src, type_identity_t<T> val,
                                                    size_t n) {
#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_not_n(src, val, n);
        }

        return builtin_reverse_find_not_n(src, val, n);
    } else {
        return fallback_reverse_find_not_n(src, val, n);
    }
#else
    return fallback_reverse_find_not_n(src, val, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_FIND_HPP__