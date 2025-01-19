#ifndef WJR_BIGINTEGER_DETAIL_NEGATE_HPP__
#define WJR_BIGINTEGER_DETAIL_NEGATE_HPP__

#include <wjr/biginteger/detail/not.hpp>
#include <wjr/math/replace.hpp>

namespace wjr::math {

/*
  return true if src is all zero
  calculations : stable n instead of not + inc which maybe n * 2
*/
template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 bool bi_negate_n(T *dst, const T *src, size_t n) noexcept {
    const size_t idx = replace_find_not(dst, src, n, 0, 0);

    if (idx == n) {
        return true;
    }

    dst[idx] = -src[idx];
    bi_not_n(dst + idx + 1, src + idx + 1, n - idx - 1);
    return false;
}

} // namespace wjr::math

#endif // WJR_BIGINTEGER_DETAIL_NEGATE_HPP__