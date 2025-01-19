#ifndef WJR_BIGINTEGER_DETAIL_NOT_HPP__
#define WJR_BIGINTEGER_DETAIL_NOT_HPP__

#include <wjr/math/bitwise.hpp>
#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
    #include <wjr/arch/x86/biginteger/detail/not.hpp>
#endif

namespace wjr::math {

WJR_INTRINSIC_CONSTEXPR20 void bi_not_n(uint64_t *dst, const uint64_t *src, size_t n) noexcept {
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

#if WJR_HAS_BUILTIN(BI_NOT_N)
    return builtin_bi_not_n(dst, src, n);
#else
    return not_n(dst, src, n);
#endif
}

} // namespace wjr::math

#endif // WJR_BIGINTEGER_DETAIL_NOT_HPP__