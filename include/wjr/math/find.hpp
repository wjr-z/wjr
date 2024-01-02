#ifndef WJR_MATH_FIND_HPP__
#define WJR_MATH_FIND_HPP__

#include <wjr/simd/simd.hpp>

#if defined(WJR_X86)
#include <wjr/x86/find.hpp>
#endif

namespace wjr {

template <typename Ret>
using __find_p_ret_t = std::invoke_result_t<Ret>;

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret> fallback_find_p(const T *src0, const T *src1,
                                                            size_t n, Ret ret) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[idx] == src1[idx]) {
            return ret(idx);
        }
    }

    return ret();
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret> find_p(const T *src0, const T *src1, size_t n,
                                                   Ret ret) {
#if WJR_HAS_BUILTIN(FIND_P)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_p(src0, src1, n, ret);
        }

        return builtin_find_p(src0, src1, n, ret);
    } else {
        return fallback_find_p(src0, src1, n, ret);
    }
#else
    return fallback_find_p(src0, src1, n, ret);
#endif
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret> fallback_find_p(const T *src, T val, size_t n,
                                                            Ret ret) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] == val) {
            return ret(idx);
        }
    }

    return ret();
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret> find_p(const T *src, type_identity_t<T> val,
                                                   size_t n, Ret ret) {
#if WJR_HAS_BUILTIN(FIND_P)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_p(src, val, n, ret);
        }

        return builtin_find_p(src, val, n, ret);
    } else {
        return fallback_find_p(src, val, n, ret);
    }
#else
    return fallback_find_p(src, val, n, ret);
#endif
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret>
fallback_find_not_p(const T *src0, const T *src1, size_t n, Ret ret) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[idx] != src1[idx]) {
            return ret(idx);
        }
    }

    return ret();
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret> find_not_p(const T *src0, const T *src1,
                                                       size_t n, Ret ret) {
#if WJR_HAS_BUILTIN(FIND_P)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_not_p(src0, src1, n, ret);
        }

        return builtin_find_not_p(src0, src1, n, ret);
    } else {
        return fallback_find_not_p(src0, src1, n, ret);
    }
#else
    return fallback_find_not_p(src0, src1, n, ret);
#endif
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret> fallback_find_not_p(const T *src, T val,
                                                                size_t n, Ret ret) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != val) {
            return ret(idx);
        }
    }

    return ret();
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret>
find_not_p(const T *src, type_identity_t<T> val, size_t n, Ret ret) {
#if WJR_HAS_BUILTIN(FIND_P)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_not_p(src, val, n, ret);
        }

        return builtin_find_not_p(src, val, n, ret);
    } else {
        return fallback_find_not_p(src, val, n, ret);
    }
#else
    return fallback_find_not_p(src, val, n, ret);
#endif
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret>
fallback_reverse_find_p(const T *src0, const T *src1, size_t n, Ret ret) {
    size_t idx = 0;

    src0 += n;
    src1 += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[-1 - idx] == src1[-1 - idx]) {
            return ret(idx);
        }
    }

    return ret();
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret> reverse_find_p(const T *src0, const T *src1,
                                                           size_t n, Ret ret) {
#if WJR_HAS_BUILTIN(FIND_P)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_p(src0, src1, n, ret);
        }

        return builtin_reverse_find_p(src0, src1, n, ret);
    } else {
        return fallback_reverse_find_p(src0, src1, n, ret);
    }
#else
    return fallback_reverse_find_p(src0, src1, n, ret);
#endif
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret> fallback_reverse_find_p(const T *src, T val,
                                                                    size_t n, Ret ret) {
    size_t idx = 0;

    src += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[-1 - idx] == val) {
            return ret(idx);
        }
    }

    return ret();
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret>
reverse_find_p(const T *src, type_identity_t<T> val, size_t n, Ret ret) {
#if WJR_HAS_BUILTIN(FIND_P)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_p(src, val, n, ret);
        }

        return builtin_reverse_find_p(src, val, n, ret);
    } else {
        return fallback_reverse_find_p(src, val, n, ret);
    }
#else
    return fallback_reverse_find_p(src, val, n, ret);
#endif
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret>
fallback_reverse_find_not_p(const T *src0, const T *src1, size_t n, Ret ret) {
    size_t idx = 0;

    src0 += n;
    src1 += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[-1 - idx] != src1[-1 - idx]) {
            return ret(idx);
        }
    }

    return ret();
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret>
reverse_find_not_p(const T *src0, const T *src1, size_t n, Ret ret) {
#if WJR_HAS_BUILTIN(FIND_P)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_not_p(src0, src1, n, ret);
        }

        return builtin_reverse_find_not_p(src0, src1, n, ret);
    } else {
        return fallback_reverse_find_not_p(src0, src1, n, ret);
    }
#else
    return fallback_reverse_find_not_p(src0, src1, n, ret);
#endif
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret>
fallback_reverse_find_not_p(const T *src, type_identity_t<T> val, size_t n, Ret ret) {
    size_t idx = 0;

    src += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[-1 - idx] != val) {
            return ret(idx);
        }
    }

    return ret();
}

template <typename T, typename Ret>
WJR_INTRINSIC_CONSTEXPR __find_p_ret_t<Ret> reverse_find_not_p(const T *src, T val,
                                                               size_t n, Ret ret) {
#if WJR_HAS_BUILTIN(FIND_P)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_not_p(src, val, n, ret);
        }

        return builtin_reverse_find_not_p(src, val, n, ret);
    } else {
        return fallback_reverse_find_not_p(src, val, n, ret);
    }
#else
    return fallback_reverse_find_not_p(src, val, n, ret);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t find_n(const T *src0, const T *src1, size_t n) {
    return find_p(src0, src1, n, [n](auto &&...args) -> size_t {
        if constexpr (sizeof...(args) == 0) {
            return n;
        } else {
            return [](auto x) { return x; }(std::forward<decltype(args)>(args)...);
        }
        WJR_UNREACHABLE;
    });
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t find_n(const T *src, type_identity_t<T> val, size_t n) {
    return find_p(src, val, n, [n](auto &&...args) -> size_t {
        if constexpr (sizeof...(args) == 0) {
            return n;
        } else {
            return [](auto x) { return x; }(std::forward<decltype(args)>(args)...);
        }
        WJR_UNREACHABLE;
    });
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t find_not_n(const T *src0, const T *src1, size_t n) {
    return find_not_p(src0, src1, n, [n](auto &&...args) -> size_t {
        if constexpr (sizeof...(args) == 0) {
            return n;
        } else {
            return [](auto x) { return x; }(std::forward<decltype(args)>(args)...);
        }
        WJR_UNREACHABLE;
    });
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t find_not_n(const T *src, type_identity_t<T> val,
                                          size_t n) {
    return find_not_p(src, val, n, [n](auto &&...args) -> size_t {
        if constexpr (sizeof...(args) == 0) {
            return n;
        } else {
            return [](auto x) { return x; }(std::forward<decltype(args)>(args)...);
        }
        WJR_UNREACHABLE;
    });
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t reverse_find_n(const T *src0, const T *src1, size_t n) {
    return reverse_find_p(src0, src1, n, [n_copy = n](auto &&...args) -> size_t {
        if constexpr (sizeof...(args) == 0) {
            return 0;
        } else {
            return
                [=](auto x) { return n_copy - x; }(std::forward<decltype(args)>(args)...);
        }
        WJR_UNREACHABLE;
    });
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t reverse_find_n(const T *src, type_identity_t<T> val,
                                              size_t n) {
    return reverse_find_p(src, val, n, [n_copy = n](auto &&...args) -> size_t {
        if constexpr (sizeof...(args) == 0) {
            return 0;
        } else {
            return
                [=](auto x) { return n_copy - x; }(std::forward<decltype(args)>(args)...);
        }
        WJR_UNREACHABLE;
    });
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t reverse_find_not_n(const T *src0, const T *src1,
                                                  size_t n) {
    return reverse_find_not_p(src0, src1, n, [n_copy = n](auto &&...args) -> size_t {
        if constexpr (sizeof...(args) == 0) {
            return 0;
        } else {
            return
                [=](auto x) { return n_copy - x; }(std::forward<decltype(args)>(args)...);
        }
        WJR_UNREACHABLE;
    });
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t reverse_find_not_n(const T *src, type_identity_t<T> val,
                                                  size_t n) {
    return reverse_find_not_p(src, val, n, [n_copy = n](auto &&...args) -> size_t {
        if constexpr (sizeof...(args) == 0) {
            return 0;
        } else {
            return
                [=](auto x) { return n_copy - x; }(std::forward<decltype(args)>(args)...);
        }
        WJR_UNREACHABLE;
    });
}

} // namespace wjr

#endif // WJR_MATH_FIND_HPP__