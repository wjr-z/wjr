/**
 * @file algorithm.hpp
 * @brief Algorithm utilities and constexpr-friendly wrappers
 * @author wjr
 *
 * Provides thin wrappers over standard algorithms. C++20 algorithms support
 * constant evaluation; inplace_merge follows the standard library's support.
 */

#ifndef WJR_ALGORITHM_HPP__
#define WJR_ALGORITHM_HPP__

#include <algorithm>
#include <functional>

#include <wjr/config/compiler.hpp>

namespace wjr {
namespace constant {

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

#if defined(__cpp_lib_constexpr_algorithms) && __cpp_lib_constexpr_algorithms >= 202306L
template <typename Iter, typename Pred>
constexpr Iter inplace_merge(Iter first, Iter mid, Iter last, Pred pred) {
    std::inplace_merge(first, mid, last, pred);
    return last;
}
#else
template <typename Iter, typename Pred>
Iter inplace_merge(Iter first, Iter mid, Iter last, Pred pred) {
    std::inplace_merge(first, mid, last, pred);
    return last;
}
#endif

template <typename Iter, typename Pred>
constexpr void sort(Iter first, Iter last, Pred pred) {
    std::sort(first, last, pred);
}

template <typename Iter>
constexpr void sort(Iter first, Iter last) {
    constant::sort(first, last, std::less<>{});
}

} // namespace constant

} // namespace wjr

#endif // WJR_ALGORITHM_HPP__