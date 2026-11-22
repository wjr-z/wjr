#ifndef WJR_CONTAINER_DETAIL_RANGE_SIZE_HPP__
#define WJR_CONTAINER_DETAIL_RANGE_SIZE_HPP__

#include <limits>
#include <ranges>
#include <stdexcept>

#include <wjr/macros/exceptions.hpp>

namespace wjr::container_detail {

template <typename Range>
concept countable_range = std::ranges::sized_range<Range> || std::ranges::forward_range<Range>;

/// @private Validate a known element count before conversion to the storage size type.
template <typename Size, typename Count>
constexpr Size checked_count(Count count, Size limit, [[maybe_unused]] const char *message) {
    if constexpr (std::numeric_limits<Count>::is_signed) {
        if (count < 0) {
            WJR_THROW(std::length_error(message));
        }
    }
    if (count > limit) {
        WJR_THROW(std::length_error(message));
    }
    return static_cast<Size>(count);
}

template <countable_range Range>
constexpr auto range_size(Range &range) {
    if constexpr (std::ranges::sized_range<Range>) {
        return std::ranges::size(range);
    } else {
        return std::ranges::distance(range);
    }
}

template <typename Size, countable_range Range>
constexpr Size checked_range_size(Range &range, Size limit, const char *message) {
    return checked_count(container_detail::range_size(range), limit, message);
}

} // namespace wjr::container_detail

#endif // WJR_CONTAINER_DETAIL_RANGE_SIZE_HPP__
