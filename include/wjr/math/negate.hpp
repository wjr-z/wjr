#ifndef WJR_MATH_NEG_HPP__
#define WJR_MATH_NEG_HPP__

#include <wjr/math/not.hpp>
#include <wjr/math/replace.hpp>

namespace wjr {

/*
  return true if src is all zero
  calculations : stable n instead of not + inc which maybe n * 2
*/
template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 bool negate_n(T *dst, const T *src, size_t n) noexcept {
    const size_t idx = replace_find_not(dst, src, n, 0, 0);

    if (idx == n) {
        return true;
    }

    dst[idx] = -src[idx];
    not_n(dst + idx + 1, src + idx + 1, n - idx - 1);
    return false;
}

} // namespace wjr

#endif // WJR_MATH_NEG_HPP__