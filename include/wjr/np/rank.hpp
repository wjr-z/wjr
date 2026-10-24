#ifndef WJR_NP_RANK_HPP__
#define WJR_NP_RANK_HPP__

#include <algorithm>
#include <span>

#include <wjr/np/reduce.hpp>

namespace wjr::np {

enum class rank_method { average, min, max, dense, ordinal };
enum class rank_nan_policy { propagate, omit };

namespace detail {

template <rank_method Method, class D>
constexpr D tied_rank(size_t first, size_t last, size_t dense) noexcept {
    if constexpr (Method == rank_method::average)
        return static_cast<D>(first + 1) + static_cast<D>(last - first - 1) / D{2};
    else if constexpr (Method == rank_method::min)
        return static_cast<D>(first + 1);
    else if constexpr (Method == rank_method::max)
        return static_cast<D>(last);
    else
        return static_cast<D>(dense);
}

template <size_t Axis, class View>
constexpr auto rank_axis_step(const View &view) noexcept {
    using layout = typename View::layout_type;
    if constexpr ((packed_layout<layout> || padded_layout<layout>) &&
                  (left_layout<layout> ? Axis == 0 : Axis + 1 == View::rank()))
        return std::integral_constant<size_t, 1>{};
    else
        return axis_stride<Axis>(view);
}

template <rank_method Method, rank_nan_policy NaN, class D, class T, class OutputStep,
          class InputStep>
void rank_line_to(D *output, const T *input, size_t count, OutputStep output_step,
                  InputStep input_step, size_t *order) noexcept {
    const auto read = [input, input_step] {
        // Do not capture an empty stride object in the sort comparator: even
        // an unused empty capture can change its ABI and prevent scalar passing.
        if constexpr (std::is_empty_v<InputStep>)
            return [input](size_t i) noexcept { return input[i]; };
        else
            return [input, input_step](size_t i) noexcept { return input[i * input_step]; };
    }();
    size_t valid = 0;
    if constexpr (std::is_floating_point_v<T>) {
        for (size_t i = 0; i != count; ++i) {
            const auto value = read(i);
            if (value != value) {
                if constexpr (NaN == rank_nan_policy::propagate) {
                    for (size_t j = 0; j != count; ++j)
                        output[j * output_step] = std::numeric_limits<D>::quiet_NaN();
                    return;
                } else {
                    output[i * output_step] = std::numeric_limits<D>::quiet_NaN();
                }
            } else {
                order[valid++] = i;
            }
        }
    } else {
        for (size_t i = 0; i != count; ++i)
            order[i] = i;
        valid = count;
    }
    // In particular, avoid pointer arithmetic on an empty workspace's null handle.
    if (valid == 0)
        return;
    std::sort(order, order + valid, [read](size_t a, size_t b) noexcept {
        const auto left = read(a);
        const auto right = read(b);
        if constexpr (Method == rank_method::ordinal)
            return left < right || (left == right && a < b);
        else
            return left < right;
    });
    if constexpr (Method == rank_method::ordinal) {
        for (size_t i = 0; i != valid; ++i)
            output[order[i] * output_step] = static_cast<D>(i + 1);
    } else {
        size_t first = 0;
        size_t dense = 1;
        while (first != valid) {
            size_t last = first + 1;
            const auto value = read(order[first]);
            while (last != valid && read(order[last]) == value)
                ++last;
            const D rank = tied_rank<Method, D>(first, last, dense);
            for (size_t i = first; i != last; ++i)
                output[order[i] * output_step] = rank;
            first = last;
            ++dense;
        }
    }
}

} // namespace detail

// One-based ascending ranks along Axis. Source and destination have equal shape
// and disjoint storage. Workspace has at least extent(Axis) live size_t objects,
// disjoint from both views; the same workspace is reused for every line.
// NaNs propagate to their whole line, or are omitted and keep NaN output slots.
// Infinities participate in ordering; signed zeros tie. No storage is allocated.
template <rank_method Method = rank_method::average,
          rank_nan_policy NaN = rank_nan_policy::propagate, class D, class DstExtents,
          class DstLayout, class T, class SrcExtents, class SrcLayout, size_t Axis>
requires(std::floating_point<D> && std::same_as<D, std::remove_cv_t<D>> &&
         detail::kernel_element<T> && detail::kernel_layout<DstLayout> &&
         detail::kernel_layout<SrcLayout> && Axis < SrcExtents::rank() &&
         detail::compatible_kernel_shape<DstExtents, SrcExtents>)
void rankdata_to(tensor_view<D, DstExtents, DstLayout> dst,
                 tensor_view<T, SrcExtents, SrcLayout> src, std::span<size_t> workspace,
                 axes_t<Axis>) noexcept {
    detail::assert_kernel_shape(dst, src);
    if (src.empty())
        return;
    const auto count = static_cast<size_t>(src.extent(Axis));
    WJR_ASSERT_L2(workspace.size() >= count);
    const auto output_step = detail::rank_axis_step<Axis>(dst);
    const auto input_step = detail::rank_axis_step<Axis>(src);
    auto rank = [output = dst.data_handle(), input = src.data_handle(), order = workspace.data(),
                 count, output_step,
                 input_step](size_t output_offset, size_t input_offset) noexcept {
        detail::rank_line_to<Method, NaN>(output + output_offset, input + input_offset, count,
                                          output_step, input_step, order);
    };
    detail::walk_axis_lines<0, Axis, true>(dst, src, rank, 0, 0);
}

template <rank_method Method = rank_method::average,
          rank_nan_policy NaN = rank_nan_policy::propagate, class D, class DstExtents,
          class DstLayout, class T, class SrcExtents, class SrcLayout>
requires(DstExtents::rank() == 1 && SrcExtents::rank() == 1 &&
         requires(tensor_view<D, DstExtents, DstLayout> dst,
                  tensor_view<T, SrcExtents, SrcLayout> src, std::span<size_t> work) {
             np::rankdata_to<Method, NaN>(dst, src, work, axes<0>);
         })
void rankdata_to(tensor_view<D, DstExtents, DstLayout> dst,
                 tensor_view<T, SrcExtents, SrcLayout> src, std::span<size_t> workspace) noexcept {
    np::rankdata_to<Method, NaN>(dst, src, workspace, axes<0>);
}

} // namespace wjr::np

#endif // WJR_NP_RANK_HPP__
