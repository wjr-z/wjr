#ifndef WJR_CONTAINER_DETAIL_RANGE_SIZE_HPP__
#define WJR_CONTAINER_DETAIL_RANGE_SIZE_HPP__

#include <ranges>
#include <stdexcept>

#include <wjr/macros/exceptions.hpp>

namespace wjr::container_detail {

template <typename Range>
concept countable_range = std::ranges::sized_range<Range> || std::ranges::forward_range<Range>;

/// @private Check a known range length before narrowing it to the storage size type.
/// @pre range denotes a valid range and its length is nonnegative.
template <typename Size, countable_range Range>
constexpr Size checked_range_size(Range &range, Size limit, [[maybe_unused]] const char *message) {
    const auto size = [&] {
        if constexpr (std::ranges::sized_range<Range>) {
            return std::ranges::size(range);
        } else {
            return std::ranges::distance(range);
        }
    }();

    if (size > limit) {
        WJR_THROW(std::length_error(message));
    }
    return static_cast<Size>(size);
}

} // namespace wjr::container_detail

#endif // WJR_CONTAINER_DETAIL_RANGE_SIZE_HPP__
