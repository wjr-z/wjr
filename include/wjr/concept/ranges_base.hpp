#ifndef WJR_CONCEPT_RANGES_BASE_HPP__
#define WJR_CONCEPT_RANGES_BASE_HPP__

#include <concepts>
#include <ranges>

namespace wjr {
namespace detail {
template <typename Range, typename T>
concept _container_compatible_range = std::ranges::input_range<Range> &&
                                      std::convertible_to<std::ranges::range_reference_t<Range>, T>;
} // namespace detail
} // namespace wjr

#endif // WJR_CONCEPT_RANGES_BASE_HPP__