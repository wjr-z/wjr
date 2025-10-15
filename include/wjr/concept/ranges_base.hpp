#ifndef WJR_CONCEPT_RANGES_BASE_HPP__
#define WJR_CONCEPT_RANGES_BASE_HPP__

#include <type_traits>

#include <wjr/tp/base.hpp>

#include <range/v3/range/concepts.hpp>

namespace wjr {
namespace detail {
CPP_template(typename Range,
             typename T)(concept(range_convertible_to_)(Range, T),
                         ranges::convertible_to<ranges::range_reference_t<Range>, T>);

template <typename Range, typename T>
CPP_concept __container_compatible_range =
    ranges::input_range<Range> && CPP_concept_ref(range_convertible_to_, Range, T);
} // namespace detail
} // namespace wjr

#endif // WJR_CONCEPT_RANGES_BASE_HPP__