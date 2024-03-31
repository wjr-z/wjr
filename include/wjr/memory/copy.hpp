#ifndef WJR_MEMORY_COPY_HPP__
#define WJR_MEMORY_COPY_HPP__

#include <wjr/container/generic/details.hpp>
#include <wjr/iterator/inserter.hpp>

namespace wjr {

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
                auto pos = get_inserter_iterator(d_first);
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

} // namespace wjr

#endif // WJR_MEMORY_COPY_HPP__