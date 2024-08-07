#ifndef WJR_MEMORY_COPY_HPP__
#define WJR_MEMORY_COPY_HPP__

#include <algorithm>

#include <wjr/container/generic/detail.hpp>
#include <wjr/iterator/inserter.hpp>
#include <wjr/memory/detail.hpp>

namespace wjr {

template <typename InputIt, typename OutputIt>
constexpr OutputIt __copy_impl(InputIt first, InputIt last, OutputIt d_first) {
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
    return __copy_impl(to_contiguous_address(first), to_contiguous_address(last),
                       to_contiguous_address(d_first));
}

/// @private
template <typename InputIt, typename OutputIt>
constexpr OutputIt __copy_restrict_impl(InputIt first, InputIt last,
                                        OutputIt d_first) noexcept {
    return wjr::copy(make_restrict_iterator(first), make_restrict_iterator(last),
                     make_restrict_iterator(d_first));
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
    const auto __first = to_contiguous_address(std::move(first));
    const auto __last = to_contiguous_address(std::move(last));
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last = __copy_restrict_impl(__first, __last, __d_first);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __copy_restrict_impl(__first, __last, d_first);
    }
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt __copy_n_impl(InputIt first, Size count, OutputIt d_first) {
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
    return __copy_n_impl(to_contiguous_address(first), count,
                         to_contiguous_address(d_first));
}

/// @private
template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt __copy_n_restrict_impl(InputIt first, Size count, OutputIt d_first) {
    return wjr::copy_n(make_restrict_iterator(first), make_restrict_iterator(count),
                       make_restrict_iterator(d_first));
}

/**
 * @brief Copy elements from a range to another range with restricted pointers.
 *
 * @details @see wjr::copy_restrict. \n
 *
 */
template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt copy_n_restrict(InputIt first, Size count, OutputIt d_first) {
    const auto __first = to_contiguous_address(std::move(first));
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last = __copy_n_restrict_impl(__first, count, __d_first);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __copy_n_restrict_impl(__first, count, d_first);
    }
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt move(InputIt first, InputIt last, OutputIt d_first) {
    return wjr::copy(std::make_move_iterator(first), std::make_move_iterator(last),
                     d_first);
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt move_restrict(InputIt first, InputIt last, OutputIt d_first) {
    return wjr::copy_restrict(std::make_move_iterator(first),
                              std::make_move_iterator(last), d_first);
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