#ifndef WJR_MATH_LITERAL_HPP__
#define WJR_MATH_LITERAL_HPP__

namespace wjr {

constexpr unsigned long long operator"" _KB(unsigned long long n) noexcept { return n * 1024; }
constexpr unsigned long long operator"" _MB(unsigned long long n) noexcept { return n * 1024_KB; }
constexpr unsigned long long operator"" _GB(unsigned long long n) noexcept { return n * 1024_MB; }

} // namespace wjr

#endif // WJR_MATH_LITERAL_HPP__