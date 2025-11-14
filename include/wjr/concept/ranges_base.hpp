#ifndef WJR_CONCEPT_RANGES_BASE_HPP__
#define WJR_CONCEPT_RANGES_BASE_HPP__

#include <type_traits>

#include <wjr/ranges.hpp>
#include <wjr/tp/base.hpp>

namespace wjr {
namespace detail {
template <typename Range, typename T>
concept _container_compatible_range =
    ranges::input_range<Range> && std::convertible_to<ranges::range_reference_t<Range>, T>;
} // namespace detail
} // namespace wjr

#endif // WJR_CONCEPT_RANGES_BASE_HPP__