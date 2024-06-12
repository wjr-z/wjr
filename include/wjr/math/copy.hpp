#ifndef WJR_MATH_COPY_HPP__
#define WJR_MATH_COPY_HPP__

#include <wjr/iterator/details.hpp>

namespace wjr {

/// @private
template <typename InputIt, typename OutputIt>
struct __is_builtin_copy
    : std::conjunction<
          std::is_trivially_copyable<iterator_value_t<InputIt>>,
          std::is_same<iterator_value_t<InputIt>, iterator_value_t<OutputIt>>,
          is_contiguous_iterator<InputIt>, is_contiguous_iterator<OutputIt>> {};

/// @private
template <typename InputIt, typename OutputIt>
inline constexpr bool __is_builtin_copy_v = __is_builtin_copy<InputIt, OutputIt>::value;

} // namespace wjr

#endif // WJR_MATH_COPY_HPP__