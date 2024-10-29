#ifndef WJR_MATH_LITERAL_HPP__
#define WJR_MATH_LITERAL_HPP__

#include <wjr/preprocessor.hpp>

namespace wjr {

WJR_CONST WJR_INTRINSIC_CONSTEXPR unsigned long long operator"" _KB(unsigned long long n) noexcept {
    return n * 1024;
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR unsigned long long operator"" _MB(unsigned long long n) noexcept {
    return n * 1024 * 1024;
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR unsigned long long operator"" _GB(unsigned long long n) noexcept {
    return n * 1024 * 1024 * 1024;
}

} // namespace wjr

#endif // WJR_MATH_LITERAL_HPP__