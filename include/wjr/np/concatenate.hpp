#ifndef WJR_NP_CONCATENATE_HPP__
#define WJR_NP_CONCATENATE_HPP__

#include <algorithm>
#include <array>
#include <utility>

#include <wjr/np/reduce.hpp>

namespace wjr::np {
namespace detail {

template <size_t Axis, class DE, class SE>
inline constexpr bool compatible_concatenate_shape = [] {
    if constexpr (DE::rank() != SE::rank() || Axis >= DE::rank()) {
        return false;
    } else {
        for (size_t i = 0; i != DE::rank(); ++i) {
            const auto dst = DE::static_extent(i);
            const auto src = SE::static_extent(i);
            if (i != Axis && dst != dynamic_extent && src != dynamic_extent && dst != src)
                return false;
        }
        return true;
    }
}();

// Flatten backend mappings once so the recursive loop only sees ordinary
// scalar metadata. No descriptor refers back to an mdspan/mapping subobject.
template <class T, size_t Rank, class Layout>
struct concatenate_descriptor {
    using layout_type = Layout;
    T *data;
    std::array<size_t, Rank> shape;
    std::array<size_t, Rank> steps;

    static constexpr size_t rank() noexcept { return Rank; }
    constexpr size_t extent(size_t axis) const noexcept { return shape[axis]; }
    constexpr size_t stride(size_t axis) const noexcept { return steps[axis]; }
    constexpr T *data_handle() const noexcept { return data; }
};

template <class View, size_t... I>
constexpr auto concatenate_describe(View view, std::index_sequence<I...>) noexcept {
    return concatenate_descriptor<typename View::element_type, View::rank(),
                                  typename View::layout_type>{
        view.data_handle(),
        {static_cast<size_t>(view.extent(I))...},
        {static_cast<size_t>(axis_stride<I>(view))...}};
}

template <class D, class T>
constexpr void concatenate_contiguous(D *WJR_RESTRICT output, const T *input,
                                      size_t count) noexcept {
    std::copy_n(input, count, output);
}

// Output traversal follows its layout. Matching packed inner axes become block
// copies; general positive strides copy only the addressed logical elements.
template <size_t Level, class Dst, class Src, class D, class T>
constexpr void concatenate_copy(D *WJR_RESTRICT output, const T *input, const Dst &dst,
                                const Src &src, size_t output_offset = 0,
                                size_t input_offset = 0) noexcept {
    constexpr size_t axis =
        left_layout<typename Dst::layout_type> ? Dst::rank() - 1 - Level : Level;
    const auto count = static_cast<size_t>(src.extent(axis));
    if constexpr (Level + 1 == Dst::rank()) {
        if constexpr (packed_layout<typename Dst::layout_type> &&
                      packed_layout<typename Src::layout_type> &&
                      (Dst::rank() == 1 ||
                       std::same_as<typename Dst::layout_type, typename Src::layout_type>)) {
            concatenate_contiguous(output + output_offset, input + input_offset, count);
        } else {
            const auto output_step = dst.stride(axis);
            const auto input_step = src.stride(axis);
            for (size_t i = 0; i != count; ++i)
                output[output_offset + i * output_step] = input[input_offset + i * input_step];
        }
    } else {
        const auto output_step = dst.stride(axis);
        const auto input_step = src.stride(axis);
        for (size_t i = 0; i != count; ++i)
            concatenate_copy<Level + 1>(output, input, dst, src, output_offset + i * output_step,
                                        input_offset + i * input_step);
    }
}

template <class Src>
struct concatenate_operand {
    Src view;
    size_t offset;
};

template <size_t Axis, size_t Level, class Dst, class Src>
constexpr void concatenate_source(const Dst &dst, concatenate_operand<Src> source,
                                  size_t output_offset) noexcept {
    const auto &src = source.view;
    const auto count = static_cast<size_t>(src.extent(Axis));
    if (count == 0)
        return;
    auto *output = dst.data_handle() + output_offset;
    const auto *input = src.data_handle() + source.offset;
    constexpr bool dense =
        packed_layout<typename Dst::layout_type> && packed_layout<typename Src::layout_type> &&
        (Dst::rank() == 1 || std::same_as<typename Dst::layout_type, typename Src::layout_type>);
    if constexpr (dense) {
        concatenate_contiguous(output, input, count * src.stride(Axis));
    } else {
        concatenate_copy<Level>(output, input, dst, src);
    }
}

// Coordinates outside the concatenation axis are shared across all inputs.
// Operands contain only pointers and scalar metadata, without backend references.
// Inlining preserves caller-known strides across the recursive loop boundary.
template <size_t Axis, size_t Level, class Dst, class... Src>
WJR_INTRINSIC_INLINE constexpr void
concatenate_outer(const Dst &dst, size_t output_offset,
                  concatenate_operand<Src>... sources) noexcept {
    constexpr size_t axis =
        left_layout<typename Dst::layout_type> ? Dst::rank() - 1 - Level : Level;
    const auto output_step = dst.stride(axis);
    if constexpr (axis == Axis) {
        auto append = [&](auto source) constexpr noexcept {
            concatenate_source<Axis, Level>(dst, source, output_offset);
            output_offset += static_cast<size_t>(source.view.extent(Axis)) * output_step;
        };
        (append(sources), ...);
    } else {
        const auto count = static_cast<size_t>(dst.extent(axis));
        for (size_t i = 0; i != count; ++i) {
            concatenate_outer<Axis, Level + 1>(
                dst, output_offset + i * output_step,
                concatenate_operand<Src>{sources.view,
                                         sources.offset + i * sources.view.stride(axis)}...);
        }
    }
}

} // namespace detail

// All inputs have the same rank and non-axis extents. Output is disjoint from
// every input, while inputs may overlap each other. Empty inputs access no data.
template <class D, class DE, class DL, size_t Axis, class... T, class... SE, class... SL>
requires(sizeof...(T) != 0 && detail::kernel_element<D> && !std::is_const_v<D> &&
         detail::kernel_layout<DL> && Axis < DE::rank() &&
         ((detail::kernel_element<T> && std::same_as<D, std::remove_cv_t<T>> &&
           detail::kernel_layout<SL> && detail::compatible_concatenate_shape<Axis, DE, SE>) &&
          ...))
WJR_INTRINSIC_INLINE constexpr void concatenate_to(tensor_view<D, DE, DL> destination, axes_t<Axis>,
                                                   tensor_view<T, SE, SL>... sources) noexcept {
    [[maybe_unused]] auto remaining = static_cast<size_t>(destination.extent(Axis));
    auto validate = [&](auto source) constexpr noexcept {
        for (size_t axis = 0; axis != DE::rank(); ++axis) {
            WJR_ASSERT_L2(axis == Axis || static_cast<size_t>(destination.extent(axis)) ==
                                              static_cast<size_t>(source.extent(axis)));
        }
        const auto count = static_cast<size_t>(source.extent(Axis));
        WJR_ASSERT_L2(count <= remaining);
        remaining -= count;
    };
    (validate(sources), ...);
    WJR_ASSERT_L2(remaining == 0);
    if (destination.empty())
        return;
    const auto indices = std::make_index_sequence<DE::rank()>{};
    detail::concatenate_outer<Axis, 0>(
        detail::concatenate_describe(destination, indices), 0,
        detail::concatenate_operand{detail::concatenate_describe(sources, indices), size_t{0}}...);
}

} // namespace wjr::np

#endif // WJR_NP_CONCATENATE_HPP__
