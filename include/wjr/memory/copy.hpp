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

/// @private Internal implementation for optimized copy operation
template <typename InputIt, typename OutputIt>
constexpr OutputIt _copy_impl(InputIt first, InputIt last, OutputIt d_first) {
    using Out = remove_cvref_t<OutputIt>;

    if constexpr (is_back_insert_iterator_v<Out> || is_insert_iterator_v<Out>) {
        using Container = typename Out::container_type;

        if constexpr (is_back_insert_iterator_v<Out>) {
            if constexpr (has_container_append_v<Container, InputIt, InputIt>) {
                append(get_inserter_container(d_first), first, last);
                return d_first;
            } else if constexpr (has_container_insert_v<Container, InputIt, InputIt>) {
                auto &cont = get_inserter_container(d_first);
                cont.insert(cont.cend(), first, last);
                return d_first;
            } else {
                return std::copy(first, last, d_first);
            }
        } else {
            if constexpr (has_container_insert_v<Container, InputIt, InputIt>) {
                auto &cont = get_inserter_container(d_first);
                const auto pos = get_inserter_iterator(d_first);
                cont.insert(pos, first, last);
                return d_first;
            } else {
                return std::copy(first, last, d_first);
            }
        }
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
    return _copy_impl(wjr::_iter_base(first), wjr::_iter_base(last), wjr::_iter_base(d_first));
}

/// @private
template <typename InputIt, typename OutputIt>
constexpr OutputIt _copy_restrict_aux(add_restrict_t<InputIt> first, InputIt last,
                                      add_restrict_t<OutputIt> d_first) noexcept {
    return wjr::copy(first, last, d_first);
}

/// @private
template <typename InputIt, typename OutputIt>
constexpr OutputIt _copy_restrict_impl(InputIt first, InputIt last, OutputIt d_first) noexcept {
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
    using Out = remove_cvref_t<OutputIt>;

    if constexpr (is_random_access_iterator_v<InputIt> &&
                  (is_back_insert_iterator_v<Out> || is_insert_iterator_v<Out>)) {
        using Container = typename Out::container_type;

        if constexpr (is_back_insert_iterator_v<Out>) {
            if constexpr (has_container_append_v<Container, InputIt, InputIt>) {
                append(get_inserter_container(d_first), first, std::next(first, count));
                return d_first;
            } else if constexpr (has_container_insert_v<Container, InputIt, InputIt>) {
                auto &cont = get_inserter_container(d_first);
                cont.insert(cont.cend(), first, std::next(first, count));
                return d_first;
            } else {
                return std::copy_n(first, count, d_first);
            }
        } else {
            if constexpr (has_container_insert_v<Container, InputIt, InputIt>) {
                auto &cont = get_inserter_container(d_first);
                const auto pos = get_inserter_iterator(d_first);
                cont.insert(pos, first, std::next(first, count));
                return d_first;
            } else {
                return std::copy_n(first, std::next(first, count), d_first);
            }
        }
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
    return _copy_n_impl(wjr::_iter_base(first), count, wjr::_iter_base(d_first));
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