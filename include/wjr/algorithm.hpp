#ifndef WJR_FORMAT_ALGORITHM_HPP__
#define WJR_FORMAT_ALGORITHM_HPP__

#include <algorithm>
#include <string_view>

#include <wjr/iterator/detail.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

#if defined(__cpp_lib_constexpr_algorithms)

template <typename InputIt, typename OutputIt>
constexpr OutputIt constexpr_copy(InputIt first, InputIt last, OutputIt dst) {
    return std::copy(first, last, dst);
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt constexpr_move_backward(InputIt first, InputIt last, OutputIt dst) {
    return std::move_backward(first, last, dst);
}

template <typename InputIt1, typename InputIt2, typename OutputIt, typename Pred>
constexpr OutputIt constexpr_merge(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                   InputIt2 last2, OutputIt dst, Pred pred) {
    return std::merge(first1, last1, first2, last2, dst, pred);
}

template <typename Iter, typename Pred>
constexpr Iter constexpr_inplace_merge(Iter first, Iter mid, Iter last, Pred pred) {
    return std::inplace_merge(first, mid, last, pred);
}

template <typename Iter, typename Pred>
constexpr void constexpr_sort(Iter first, Iter last, Pred pred) {
    std::sort(first, last, pred);
}

#else

template <typename InputIt, typename OutputIt>
constexpr OutputIt constexpr_copy(InputIt first, InputIt last, OutputIt dst) {
    for (; first != last; (void)++first, (void)++dst) {
        *dst = *first;
    }

    return dst;
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt constexpr_move_backward(InputIt first, InputIt last, OutputIt dst) {
    while (first != last) {
        *(--dst) = std::move(*(--last));
    }

    return dst;
}

template <typename InputIt1, typename InputIt2, typename OutputIt, typename Pred>
constexpr OutputIt constexpr_merge(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                   InputIt2 last2, OutputIt dst, Pred pred) {
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

    dst = constexpr_copy(first1, last1, dst);
    dst = constexpr_copy(first2, last2, dst);
    return dst;
}

namespace algorithm_detail {

template <typename Iter, typename Pred>
constexpr void __constexpr_insertion_sort(Iter first, Iter last, Pred pred) {
    if (first != last) {
        for (Iter mid = first; ++mid != last;) {
            iterator_reference_t<Iter> ref = *mid;

            if (pred(ref, *first)) {
                iterator_value_t<Iter> tmp = std::move(ref);
                Iter hole = mid;
                constexpr_move_backward(first, mid, ++hole);
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

inline constexpr size_t __insertion_sort_threshold = in_place_max;

template <typename Iter, typename Pred>
constexpr void __constexpr_sort_impl(Iter first, Iter last, Pred pred) {
    __constexpr_insertion_sort(first, last, pred);
}

} // namespace algorithm_detail

template <typename Iter, typename Pred>
constexpr void constexpr_sort(Iter first, Iter last, Pred pred) {
    algorithm_detail::__constexpr_sort_impl(first, last, pred);
}

#endif

template <typename Iter>
constexpr void constexpr_sort(Iter first, Iter last) {
    constexpr_sort(first, last, std::less<>{});
}

template <typename CharT, typename Traits>
WJR_PURE WJR_INTRINSIC_INLINE bool
starts_with(std::basic_string_view<CharT, Traits> str,
            std::basic_string_view<CharT, Traits> sv) noexcept {
    const auto length = sv.size();
    if (str.size() < length) {
        return false;
    }

    return Traits::compare(str.data(), sv.data(), length) == 0;
}

template <typename CharT, typename Traits>
WJR_PURE WJR_INTRINSIC_INLINE bool
ends_with(std::basic_string_view<CharT, Traits> str,
          std::basic_string_view<CharT, Traits> sv) noexcept {
    const auto n = str.size();
    const auto length = sv.size();
    if (n < length) {
        return false;
    }

    return Traits::compare(str.data() + n - length, sv.data(), length) == 0;
}

} // namespace wjr

#endif // WJR_FORMAT_ALGORITHM_HPP__