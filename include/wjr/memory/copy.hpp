/**
 * @file copy.hpp
 * @author wjr
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 * @todo 1. More fast small copy.
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef WJR_MEMORY_COPY_HPP__
#define WJR_MEMORY_COPY_HPP__

#include <algorithm>

#include <wjr/container/detail.hpp>
#include <wjr/iterator/inserter.hpp>
#include <wjr/memory/detail.hpp>

namespace wjr {

namespace copy_detail {

template <typename Container, typename InputIt>
concept bulk_appendable = appendable_container<Container, InputIt &, InputIt &> ||
                          requires(Container &cont, InputIt &first, InputIt &last) {
                              cont.insert(cont.cend(), first, last);
                          };

template <typename Container, typename InputIt>
requires bulk_appendable<Container, InputIt>
WJR_INTRINSIC_INLINE constexpr std::back_insert_iterator<Container>
copy_to_inserter(InputIt first, InputIt last, std::back_insert_iterator<Container> out) {
    auto &cont = get_inserter_container(out);
    if constexpr (appendable_container<Container, InputIt &, InputIt &>) {
        append(cont, first, last);
    } else {
        cont.insert(cont.cend(), first, last);
    }
    return out;
}

template <typename Container, typename InputIt>
concept bulk_position_insertable =
    std::ranges::random_access_range<Container> && std::ranges::sized_range<Container> &&
    requires(Container &cont, std::ranges::iterator_t<Container> pos, InputIt &first,
             InputIt &last) {
        { cont.insert(pos, first, last) } -> std::same_as<std::ranges::iterator_t<Container>>;
    };

template <typename Container, typename InputIt>
requires bulk_position_insertable<Container, InputIt>
WJR_INTRINSIC_INLINE constexpr std::insert_iterator<Container>
copy_to_inserter(InputIt first, InputIt last, std::insert_iterator<Container> out) {
    auto &cont = get_inserter_container(out);
    const auto old_size = std::ranges::size(cont);
    const auto inserted = cont.insert(get_inserter_iterator(out), first, last);
    const auto count = std::ranges::size(cont) - old_size;
    // The original position may have been invalidated by insert.
    return std::inserter(cont,
                         inserted + static_cast<std::ranges::range_difference_t<Container>>(count));
}

template <typename InputIt, typename OutputIt>
concept bulk_copyable =
    requires(InputIt first, OutputIt out) { copy_detail::copy_to_inserter(first, first, out); };

} // namespace copy_detail

/// @private Internal implementation for optimized copy operation.
template <typename InputIt, typename OutputIt>
constexpr OutputIt _copy_impl(InputIt first, InputIt last, OutputIt d_first) {
    if constexpr (copy_detail::bulk_copyable<InputIt, OutputIt>) {
        return copy_detail::copy_to_inserter(first, last, d_first);
    } else {
        return std::copy(first, last, d_first);
    }
}

/**
 * @fn copy
 *
 * @details Optimized for back_insert_iterator and insert_iterator.
 *
 */
template <typename InputIt, typename OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
    return wjr::_iter_wrap(d_first, _copy_impl(wjr::_iter_base(first), wjr::_iter_base(last),
                                               wjr::_iter_base(d_first)));
}

/// @private
template <typename InputIt, typename OutputIt>
constexpr OutputIt _copy_restrict_aux(add_restrict_t<InputIt> first, InputIt last,
                                      add_restrict_t<OutputIt> d_first) {
    return wjr::copy(first, last, d_first);
}

/// @private
template <typename InputIt, typename OutputIt>
constexpr OutputIt _copy_restrict_impl(InputIt first, InputIt last, OutputIt d_first) {
    return _copy_restrict_aux<InputIt, OutputIt>(first, last, d_first);
}

/**
 * @brief Copy elements from a range to another range with restricted pointers.
 *
 * @details Use @ref wjr::copy. \n
 * If iterator is contiguouse, then get restricted pointer
 * by iterator to optimize.
 */
template <typename InputIt, typename OutputIt>
constexpr OutputIt copy_restrict(InputIt first, InputIt last, OutputIt d_first) {
    const auto _first = wjr::_iter_base(std::move(first));
    const auto _last = wjr::_iter_base(std::move(last));
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto _d_first = wjr::to_address(d_first);
        const auto _d_last = _copy_restrict_impl(_first, _last, _d_first);
        return std::next(d_first, std::distance(_d_first, _d_last));
    } else {
        return _copy_restrict_impl(_first, _last, d_first);
    }
}

/// @private Internal implementation for optimized copy_n operation
template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt _copy_n_impl(InputIt first, Size count, OutputIt d_first) {
    if constexpr (is_random_access_iterator_v<InputIt> &&
                  copy_detail::bulk_copyable<InputIt, OutputIt>) {
        if (count <= 0) {
            return d_first;
        }
        return copy_detail::copy_to_inserter(first, std::next(first, count), d_first);
    } else {
        return std::copy_n(first, count, d_first);
    }
}

/**
 * @fn wjr::copy_n
 *
 * @details Optimized for back_insert_iterator and insert_iterator.
 *
 */
template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt copy_n(InputIt first, Size count, OutputIt d_first) {
    return wjr::_iter_wrap(d_first,
                           _copy_n_impl(wjr::_iter_base(first), count, wjr::_iter_base(d_first)));
}

/// @private
template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt _copy_n_restrict_aux(add_restrict_t<InputIt> first, Size count,
                                        add_restrict_t<OutputIt> d_first) {
    return wjr::copy_n(first, count, d_first);
}

/// @private
template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt _copy_n_restrict_impl(InputIt first, Size count, OutputIt d_first) {
    return _copy_n_restrict_aux<InputIt, Size, OutputIt>(first, count, d_first);
}

/**
 * @brief Copy elements from a range to another range with restricted pointers.
 *
 * @details @see wjr::copy_restrict. \n
 *
 */
template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt copy_n_restrict(InputIt first, Size count, OutputIt d_first) {
    const auto _first = wjr::_iter_base(std::move(first));
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto _d_first = wjr::to_address(d_first);
        const auto _d_last = _copy_n_restrict_impl(_first, count, _d_first);
        return std::next(d_first, std::distance(_d_first, _d_last));
    } else {
        return _copy_n_restrict_impl(_first, count, d_first);
    }
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt move(InputIt first, InputIt last, OutputIt d_first) {
    return wjr::copy(std::make_move_iterator(first), std::make_move_iterator(last), d_first);
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt move_restrict(InputIt first, InputIt last, OutputIt d_first) {
    return wjr::copy_restrict(std::make_move_iterator(first), std::make_move_iterator(last),
                              d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt move_n(InputIt first, Size count, OutputIt d_first) {
    return wjr::copy_n(std::make_move_iterator(first), count, d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt move_n_restrict(InputIt first, Size count, OutputIt d_first) {
    return wjr::copy_n_restrict(std::make_move_iterator(first), count, d_first);
}

} // namespace wjr

#endif // WJR_MEMORY_COPY_HPP__