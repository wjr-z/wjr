#ifndef WJR_MEMORY_COPY_HPP__
#define WJR_MEMORY_COPY_HPP__

#include <wjr/container/generic/details.hpp>
#include <wjr/iterator/inserter.hpp>
#include <wjr/memory/details.hpp>

namespace wjr {

/**
 * @fn copy
 *
 * @details Optimized for back_insert_iterator and insert_iterator.
 *
 */
template <typename InputIt, typename OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
    using Out = remove_cvref_t<OutputIt>;

    if constexpr (is_back_insert_iterator_v<Out> || is_insert_iterator_v<Out>) {
        using Container = typename Out::container_type;

        if constexpr (is_back_insert_iterator_v<Out>) {
            if constexpr (container_details::has_container_append_v<Container, InputIt,
                                                                    InputIt>) {
                get_inserter_container(d_first).append(first, last);
                return d_first;
            } else if constexpr (container_details::has_container_insert_v<
                                     Container, InputIt, InputIt>) {
                auto &cont = get_inserter_container(d_first);
                cont.insert(cont.cend(), first, last);
                return d_first;
            } else {
                return std::copy(first, last, d_first);
            }
        } else {
            if constexpr (container_details::has_container_insert_v<Container, InputIt,
                                                                    InputIt>) {
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

template <typename InputIt, typename OutputIt>
constexpr OutputIt __copy_restrict_impl_aux(add_restrict_t<InputIt> first,
                                            add_restrict_t<InputIt> last,
                                            add_restrict_t<OutputIt> d_first) {
    return wjr::copy(first, last, d_first);
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt __copy_restrict_impl(InputIt first, InputIt last, OutputIt d_first) {
    return __copy_restrict_impl_aux<InputIt, OutputIt>(first, last, d_first);
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt copy_restrict(InputIt first, InputIt last, OutputIt d_first) {
    const auto __first = try_to_address(std::move(first));
    const auto __last = try_to_address(std::move(last));
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = to_address(d_first);
        const auto __d_last = __copy_restrict_impl(__first, __last, __d_first);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __copy_restrict_impl(__first, __last, d_first);
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
    using Out = remove_cvref_t<OutputIt>;

    if constexpr (is_random_access_iterator_v<InputIt> &&
                  (is_back_insert_iterator_v<Out> || is_insert_iterator_v<Out>)) {
        using Container = typename Out::container_type;

        if constexpr (is_back_insert_iterator_v<Out>) {
            if constexpr (container_details::has_container_append_v<Container, InputIt,
                                                                    InputIt>) {
                get_inserter_container(d_first).append(first, std::next(first, count));
                return d_first;
            } else if constexpr (container_details::has_container_insert_v<
                                     Container, InputIt, InputIt>) {
                auto &cont = get_inserter_container(d_first);
                cont.insert(cont.cend(), first, std::next(first, count));
                return d_first;
            } else {
                return std::copy_n(first, count, d_first);
            }
        } else {
            if constexpr (container_details::has_container_insert_v<Container, InputIt,
                                                                    InputIt>) {
                auto &cont = get_inserter_container(d_first);
                auto pos = get_inserter_iterator(d_first);
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

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt __copy_n_restrict_impl_aux(add_restrict_t<InputIt> first, Size count,
                                              add_restrict_t<OutputIt> d_first) {
    return wjr::copy_n(first, count, d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt __copy_n_restrict_impl(InputIt first, Size count, OutputIt d_first) {
    return __copy_n_restrict_impl_aux<InputIt, Size, OutputIt>(first, count, d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt copy_n_restrict(InputIt first, Size count, OutputIt d_first) {
    const auto __first = try_to_address(std::move(first));
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = to_address(d_first);
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