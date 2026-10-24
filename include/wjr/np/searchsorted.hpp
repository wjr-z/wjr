#ifndef WJR_NP_SEARCHSORTED_HPP__
#define WJR_NP_SEARCHSORTED_HPP__

#include <algorithm>
#include <array>
#include <limits>

#include <wjr/np/reduce.hpp>

namespace wjr::np {

enum class search_side { left, right };

namespace detail {

inline constexpr size_t searchsorted_batch_width = 8;

template <search_side Side, class T>
constexpr bool searchsorted_before(T element, T value) noexcept {
    if constexpr (Side == search_side::left) {
        if constexpr (std::is_floating_point_v<T>)
            return element < value || (value != value && element == element);
        else
            return element < value;
    } else {
        if constexpr (std::is_floating_point_v<T>)
            return element <= value || value != value;
        else
            return element <= value;
    }
}

template <search_side Side, bool Contiguous, bool NaN = false, class T, class Step>
constexpr size_t searchsorted_index(const T *input, size_t count, Step step,
                                    std::remove_cv_t<T> value) noexcept {
    const auto before = [value](std::remove_cv_t<T> element) constexpr noexcept {
        if constexpr (NaN)
            return element == element;
        else if constexpr (Side == search_side::left)
            return element < value;
        else
            return element <= value;
    };
    if constexpr (Contiguous) {
        if constexpr (!NaN && Side == search_side::left)
            return static_cast<size_t>(std::lower_bound(input, input + count, value) - input);
        else if constexpr (!NaN && std::is_integral_v<T>)
            return static_cast<size_t>(std::upper_bound(input, input + count, value) - input);
        else
            return static_cast<size_t>(std::partition_point(input, input + count, before) - input);
    }
    size_t first = 0;
    while (count != 0) {
        const auto half = count / 2;
        const auto middle = first + half;
        if (before(input[middle * step])) {
            first = middle + 1;
            count -= half + 1;
        } else {
            count = half;
        }
    }
    return first;
}

template <search_side Side, bool Contiguous, class T, class Step>
constexpr size_t searchsorted_value_index(const T *input, size_t count, Step step,
                                          std::remove_cv_t<T> value) noexcept {
    if constexpr (std::is_floating_point_v<T>) {
        if (value != value) {
            if constexpr (Side == search_side::right)
                return count;
            else
                return searchsorted_index<Side, Contiguous, true>(input, count, step, value);
        }
    }
    return searchsorted_index<Side, Contiguous>(input, count, step, value);
}

template <search_side Side, size_t Block, bool Contiguous, class D, class T, class OutputStep,
          class InputStep, class QueryStep>
WJR_INTRINSIC_INLINE constexpr void
searchsorted_batch_line(D *output, const T *input, size_t count, const T *values,
                        size_t query_count, OutputStep output_step, InputStep input_step,
                        QueryStep query_step) noexcept {
    size_t start = 0;
    for (; query_count - start >= Block; start += Block) {
        std::array<size_t, Block> bases;
        std::array<T, Block> keys;
        auto length = count;
        auto half = length / 2;
        length -= half;
        const auto pivot = input[half * input_step];
        for (size_t i = 0; i != Block; ++i) {
            keys[i] = values[(start + i) * query_step];
            bases[i] = searchsorted_before<Side>(pivot, keys[i]) * half;
        }
        while (length > 1) {
            half = length / 2;
            length -= half;
            for (size_t i = 0; i != Block; ++i)
                bases[i] +=
                    searchsorted_before<Side>(input[(bases[i] + half) * input_step], keys[i]) *
                    half;
        }
        for (size_t i = 0; i != Block; ++i)
            output[(start + i) * output_step] = static_cast<D>(
                bases[i] + searchsorted_before<Side>(input[bases[i] * input_step], keys[i]));
    }
    for (; start != query_count; ++start) {
        const auto value = values[start * query_step];
        output[start * output_step] = static_cast<D>(
            searchsorted_value_index<Side, Contiguous>(input, count, input_step, value));
    }
}

} // namespace detail

// Return insertion positions in an ascending one-dimensional source. Query and
// destination shapes agree; source/query element types agree. All NaNs sort
// after non-NaNs, and signed zeros compare equal. Left selects the first equal
// position, right selects the position after the last equal value.
// Destination storage is disjoint from source and queries; source remains
// sorted/stable during the call. Its length must fit D. No storage is allocated.
template <search_side Side = search_side::left, class D, class DE, class DL, class T, class SE,
          class SL, class Q, class QE, class QL>
requires((Side == search_side::left || Side == search_side::right) && std::integral<D> &&
         !std::same_as<D, bool> && std::same_as<D, std::remove_cv_t<D>> &&
         detail::kernel_element<T> && detail::kernel_element<Q> &&
         std::same_as<std::remove_cv_t<T>, std::remove_cv_t<Q>> && SE::rank() == 1 &&
         detail::kernel_layout<DL> && detail::kernel_layout<SL> && detail::kernel_layout<QL> &&
         detail::compatible_kernel_shape<DE, QE>)
constexpr void searchsorted_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> sorted,
                               tensor_view<Q, QE, QL> queries) noexcept {
    detail::assert_kernel_shape(dst, queries);
    if (dst.empty())
        return;
    const auto count = static_cast<size_t>(sorted.extent(0));
    if (count == 0) {
        detail::apply_kernel(dst, [](D &output) noexcept { output = 0; });
        return;
    }
    if constexpr (std::numeric_limits<D>::digits < std::numeric_limits<size_t>::digits)
        WJR_ASSERT_L2(count <= static_cast<size_t>(std::numeric_limits<D>::max()));
    const auto *input = sorted.data_handle();
    const auto step = detail::axis_stride<0>(sorted);
    detail::apply_kernel(
        dst,
        [input, count, step](D &output, const std::remove_cv_t<Q> &value) noexcept {
            output =
                static_cast<D>(detail::searchsorted_value_index<Side, detail::packed_layout<SL>>(
                    input, count, step, value));
        },
        queries);
}

// Interleave fixed-size blocks of rank-one queries, sharing the initial pivot
// and interval length. Only final results are written; temporary keys and bases
// have fixed size. Contracts and result order match searchsorted_to.
template <search_side Side = search_side::left, class Dst, class Sorted, class Queries>
requires requires(Dst dst, Sorted sorted, Queries queries) {
    searchsorted_to<Side>(dst, sorted, queries);
} && (Dst::rank() == 1)
WJR_INTRINSIC_INLINE
    constexpr void searchsorted_batch_to(Dst dst, Sorted sorted, Queries queries) noexcept {
    detail::assert_kernel_shape(dst, queries);
    if (dst.empty())
        return;
    const auto count = static_cast<size_t>(sorted.extent(0));
    using D = typename Dst::element_type;
    if (count == 0) {
        detail::apply_kernel(dst, [](D &output) noexcept { output = 0; });
        return;
    }
    if constexpr (std::numeric_limits<D>::digits < std::numeric_limits<size_t>::digits)
        WJR_ASSERT_L2(count <= static_cast<size_t>(std::numeric_limits<D>::max()));
    detail::searchsorted_batch_line<Side, detail::searchsorted_batch_width,
                                    detail::packed_layout<typename Sorted::layout_type>>(
        dst.data_handle(), sorted.data_handle(), count, queries.data_handle(),
        static_cast<size_t>(queries.extent(0)), detail::axis_stride<0>(dst),
        detail::axis_stride<0>(sorted), detail::axis_stride<0>(queries));
}

} // namespace wjr::np

#endif // WJR_NP_SEARCHSORTED_HPP__
