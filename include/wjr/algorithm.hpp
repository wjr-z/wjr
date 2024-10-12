#ifndef WJR_FORMAT_ALGORITHM_HPP__
#define WJR_FORMAT_ALGORITHM_HPP__

#include <algorithm>
#include <string_view>

#include <wjr/iterator/detail.hpp>
#include <wjr/math/broadcast.hpp>
#include <wjr/math/integral_constant.hpp>
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

namespace algorithm_detail {

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

inline constexpr size_t __insertion_sort_threshold = in_place_max;

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

template <unsigned int Length, WJR_REQUIRES(Length <= 8)>
WJR_PURE WJR_INTRINSIC_INLINE bool
constant_length_strncasecmp(const char *a, const char *b,
                            integral_constant<unsigned int, Length>) {
    if constexpr (Length == 1) {
        const char diff = (a[0] ^ b[0]) & 0xDF;
        return diff == 0;
    } else if constexpr (Length == 2 || Length == 4 || Length == 8) {
        using diff_type = uint_t<Length * 8>;
        constexpr diff_type mask = broadcast<uint8_t, diff_type>(0xDF);

        diff_type diff = (read_memory<diff_type>(a) ^ read_memory<diff_type>(b)) & mask;
        return diff == 0;
    } else if constexpr (Length == 3) {
        return constant_length_strncasecmp(a, b, 2_u) &
               constant_length_strncasecmp(a + 2, b + 2, 1_u);
    } else if constexpr (Length >= 5 && Length <= 6) {
        constexpr auto RestLength = Length - 4;
        return constant_length_strncasecmp(a, b, 4_u) &
               constant_length_strncasecmp(
                   a + 4, b + 4, integral_constant<unsigned int, RestLength>());
    } else {
        static_assert(Length == 7);
        return constant_length_strncasecmp(a, b, 4_u) &
               constant_length_strncasecmp(a + 3, b + 3, 4_u);
    }
}

} // namespace wjr

#endif // WJR_FORMAT_ALGORITHM_HPP__