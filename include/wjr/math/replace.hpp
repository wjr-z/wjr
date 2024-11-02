#ifndef WJR_MATH_REPLACE_HPP__
#define WJR_MATH_REPLACE_HPP__

#include <wjr/math/find.hpp>
#include <wjr/math/set.hpp>

namespace wjr {

// find the first position(ret) that is not equal to number "from"
// and replace [0, ret) to number "to"
// For example, inc replaces a continuous segment of -1 with 0. And dec replaces
// a continuous segment of 0 with -1
template <typename T>
WJR_INTRINSIC_CONSTEXPR20 size_t replace_find_not(T *dst, const T *src, size_t n,
                                                  type_identity_t<T> from,
                                                  type_identity_t<T> to) noexcept {

    const size_t ret = find_not_n(src, from, n);
    if (WJR_UNLIKELY(ret != 0) && WJR_LIKELY(dst != src || from != to)) {
        set_n(dst, to, ret);
    }

    return ret;
}

// find the last position(ret-1) that is not equal to number "from"
// and replace [ret, n) to number "to"
template <typename T>
WJR_INTRINSIC_CONSTEXPR20 size_t reverse_replace_find_not(T *dst, const T *src, size_t n,
                                                          type_identity_t<T> from,
                                                          type_identity_t<T> to) noexcept {
    const size_t ret = reverse_find_not_n(src, from, n);
    if (WJR_UNLIKELY(ret != n) && WJR_LIKELY(dst != src || from != to)) {
        set_n(dst + ret, to, n - ret);
    }

    return ret;
}

} // namespace wjr

#endif // WJR_MATH_REPLACE_HPP__