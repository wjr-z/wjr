#ifndef WJR_MATH_REPLACE_HPP__
#define WJR_MATH_REPLACE_HPP__

#include <wjr/math/find.hpp>
#include <wjr/math/set.hpp>

namespace wjr {

// maybe deprected
// size_t ret = find_not_n(src, from, n);
// set_n(dst, to, ret)
// return ret;
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t replace_find_not(T *dst, const T *src, size_t n,
                                                  type_identity_t<T> from,
                                                  type_identity_t<T> to) {

    size_t ret = find_not_n(src, from, n);
    if (WJR_UNLIKELY(dst == src && from == to)) {
        return ret;
    }

    set_n(dst, to, ret);
    return ret;
}

} // namespace wjr

#endif // WJR_MATH_REPLACE_HPP__