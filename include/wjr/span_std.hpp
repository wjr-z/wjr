/**
 * @file span_std.hpp
 * @brief Explicit conversions between WJR and standard spans.
 */

#ifndef WJR_SPAN_STD_HPP__
#define WJR_SPAN_STD_HPP__

#include <span>

#include <wjr/span.hpp>

namespace wjr {

template <typename T, std::size_t Extent>
[[nodiscard]] constexpr std::span<T, Extent> as_std_span(span<T, Extent> view) noexcept {
    return std::span<T, Extent>(view.data(), view.size());
}

template <typename T, std::size_t Extent>
[[nodiscard]] constexpr span<T, Extent> as_wjr_span(std::span<T, Extent> view) noexcept {
    return span<T, Extent>(view.data(), view.size());
}

} // namespace wjr

#endif // WJR_SPAN_STD_HPP__
