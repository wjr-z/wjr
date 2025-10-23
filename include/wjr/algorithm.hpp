/**
 * @file algorithm.hpp
 * @brief Algorithm utilities and constexpr-friendly wrappers
 * @author wjr
 *
 * Provides constexpr algorithm implementations and optimized wrappers
 * for standard algorithms including sort, merge, and searching.
 */

#ifndef WJR_ALGORITHM_HPP__
#define WJR_ALGORITHM_HPP__

#include <algorithm>

#include <wjr/iterator/detail.hpp>
#include <wjr/memory/detail.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {
namespace constant {
#if defined(__cpp_lib_algorithms)

template <typename InputIt, typename OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt dst) {
    return std::copy(first, last, dst);
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt move_backward(InputIt first, InputIt last, OutputIt dst) {
    return std::move_backward(first, last, dst);
}

template <typename InputIt1, typename InputIt2, typename OutputIt, typename Pred>
constexpr OutputIt merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         OutputIt dst, Pred pred) {
    return std::merge(first1, last1, first2, last2, dst, pred);
}

template <typename Iter, typename Pred>
constexpr Iter inplace_merge(Iter first, Iter mid, Iter last, Pred pred) {
    return std::inplace_merge(first, mid, last, pred);
}

template <typename Iter, typename Pred>
constexpr void sort(Iter first, Iter last, Pred pred) {
    std::sort(first, last, pred);
}

#else

template <typename InputIt, typename OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt dst) {
    for (; first != last; (void)++first, (void)++dst) {
        *dst = *first;
    }

    return dst;
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt move_backward(InputIt first, InputIt last, OutputIt dst) {
    while (first != last) {
        *(--dst) = std::move(*(--last));
    }

    return dst;
}

template <typename InputIt1, typename InputIt2, typename OutputIt, typename Pred>
constexpr OutputIt merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         OutputIt dst, Pred pred) {
    const auto count1 = std::distance(first1, last1);
    const auto count2 = std::distance(first2, last2);
    if (first1 != last1 && first2 != last2) {
        for (;;) {
            if (pred(*first2, *first1)) {
                *dst = *first2;
                ++dst;
                ++first2;

                if (first2 == last2) {
                    break;
                }
            } else {
                *dst = *first1;
                ++dst;
                ++first1;

                if (first1 == last1) {
                    break;
                }
            }
        }
    }

    dst = copy(first1, last1, dst);
    dst = copy(first2, last2, dst);
    return dst;
}

/// @private Internal implementation details for sorting algorithms
namespace algorithm_detail {

/// @private Insertion sort implementation for small ranges
template <typename Iter, typename Pred>
constexpr void __insertion_sort(Iter first, Iter last, Pred pred) {
    if (first != last) {
        for (Iter mid = first; ++mid != last;) {
            iterator_reference_t<Iter> ref = *mid;

            if (pred(ref, *first)) {
                iterator_value_t<Iter> tmp = std::move(ref);
                Iter hole = mid;
                move_backward(first, mid, ++hole);
                *first = std::move(tmp);
            } else {
                Iter prev = mid;
                if (pred(ref, *--prev)) {
                    iterator_value_t<Iter> tmp = std::move(ref);
                    *mid = *prev;

                    Iter pprev = prev;
                    for (; pred(tmp, *--pprev); prev = pprev) {
                        *prev = std::move(*pprev);
                    }

                    *prev = std::move(tmp);
                }
            }
        }
    }
}

/// @private Threshold for switching to insertion sort
inline constexpr size_t __insertion_sort_threshold = in_place_max;

/// @private Main sort implementation dispatcher
template <typename Iter, typename Pred>
constexpr void __sort_impl(Iter first, Iter last, Pred pred) {
    __insertion_sort(first, last, pred);
}

} // namespace algorithm_detail

/// @todo
template <typename Iter, typename Pred>
constexpr void sort(Iter first, Iter last, Pred pred) {
    algorithm_detail::__sort_impl(first, last, pred);
}

#endif

template <typename Iter>
constexpr void sort(Iter first, Iter last) {
    constant::sort(first, last, std::less<>{});
}

} // namespace constant

} // namespace wjr

#endif // WJR_ALGORITHM_HPP__