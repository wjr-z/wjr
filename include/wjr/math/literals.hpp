/**
 * @file math/literals.hpp
 * @brief User-defined literals for size units
 * @author wjr
 *
 * Provides KB, MB, GB suffixes for size calculations.
 */

#ifndef WJR_MATH_LITERALS_HPP__
#define WJR_MATH_LITERALS_HPP__

namespace wjr {

/// Kilobyte literal (n * 1024)
constexpr unsigned long long operator""_KB(unsigned long long n) noexcept { return n * 1024; }

/// Megabyte literal (n * 1024^2)
constexpr unsigned long long operator""_MB(unsigned long long n) noexcept { return n * 1024_KB; }

/// Gigabyte literal (n * 1024^3)
constexpr unsigned long long operator""_GB(unsigned long long n) noexcept { return n * 1024_MB; }

} // namespace wjr

#endif // WJR_MATH_LITERALS_HPP__