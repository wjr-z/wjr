#ifndef WJR_NP_REDUCE_HPP__
#define WJR_NP_REDUCE_HPP__

#include <wjr/np/detail/elementwise.hpp>

namespace wjr::np {

template <size_t... Axes>
struct axes_t {};
template <size_t... Axes>
inline constexpr axes_t<Axes...> axes{};

struct keepdims_t {};
inline constexpr keepdims_t keepdims{};

namespace detail {

template <class T>
concept sum_accumulator_type =
    kernel_element<T> && std::same_as<T, std::remove_cv_t<T>> && requires(T value) {
        { value + value } -> std::same_as<T>;
    };

template <class Accumulator, class T>
using sum_accumulator =
    std::conditional_t<std::is_void_v<Accumulator>, decltype(std::declval<T>() + std::declval<T>()),
                       Accumulator>;

// Ask the mapping at constant evaluation when only static extents contribute.
// Some compilers otherwise retain the backend's runtime stride-product loop.
template <size_t Axis, class View>
constexpr auto axis_stride(const View &view) noexcept {
    using extents_type = typename View::extents_type;
    constexpr bool fixed = [] {
        if constexpr (std::same_as<typename View::layout_type, layout_right>) {
            for (size_t i = Axis + 1; i != View::rank(); ++i)
                if (extents_type::static_extent(i) == dynamic_extent)
                    return false;
            return true;
        } else if constexpr (std::same_as<typename View::layout_type, layout_left>) {
            for (size_t i = 0; i != Axis; ++i)
                if (extents_type::static_extent(i) == dynamic_extent)
                    return false;
            return true;
        } else {
            return false;
        }
    }();
    if constexpr (fixed) {
        constexpr auto stride = (typename View::mapping_type{}).stride(Axis);
        return stride;
    } else {
        return view.stride(Axis);
    }
}

// Adjacent columns keep independent ordered accumulators. Scan reads every
// lane before writing it back, including when input and output are identical.
template <bool Scan, class Accumulator, class D, class T>
constexpr void axis_columns_to(D *output, const T *input, size_t rows, size_t columns) noexcept {
    const auto bound = columns - columns % 4;
    size_t first = 0;
    for (; first != bound; first += 4) {
        Accumulator a{}, b{}, c{}, d{};
        for (size_t row = 0; row != rows; ++row) {
            const auto offset = row * columns + first;
            a += static_cast<Accumulator>(input[offset]);
            b += static_cast<Accumulator>(input[offset + 1]);
            c += static_cast<Accumulator>(input[offset + 2]);
            d += static_cast<Accumulator>(input[offset + 3]);
            if constexpr (Scan) {
                output[offset] = a;
                output[offset + 1] = b;
                output[offset + 2] = c;
                output[offset + 3] = d;
            }
        }
        if constexpr (!Scan) {
            output[first] = a;
            output[first + 1] = b;
            output[first + 2] = c;
            output[first + 3] = d;
        }
    }
    for (; first != columns; ++first) {
        Accumulator value{};
        for (size_t row = 0; row != rows; ++row) {
            const auto offset = row * columns + first;
            value += static_cast<Accumulator>(input[offset]);
            if constexpr (Scan)
                output[offset] = value;
        }
        if constexpr (!Scan)
            output[first] = value;
    }
}

template <class DstExtents, class SrcExtents, size_t Axis, bool Keep>
inline constexpr bool compatible_sum_shape = [] {
    if constexpr (Axis >= SrcExtents::rank() || DstExtents::rank() != SrcExtents::rank() - !Keep) {
        return false;
    } else {
        for (size_t dst_axis = 0; dst_axis != DstExtents::rank(); ++dst_axis) {
            const auto dst = DstExtents::static_extent(dst_axis);
            const auto src =
                Keep && dst_axis == Axis
                    ? size_t{1}
                    : SrcExtents::static_extent(dst_axis + (!Keep && dst_axis >= Axis));
            if (dst != dynamic_extent && src != dynamic_extent && dst != src)
                return false;
        }
        return true;
    }
}();

// Visit one line base per remaining coordinate. Keep means the selected axis
// remains in dst's metadata; the caller processes that axis itself.
template <size_t Level, size_t Axis, bool Keep, class Dst, class Src, class Function>
constexpr void walk_axis_lines(const Dst &dst, const Src &src, Function &function,
                               size_t output_offset, size_t input_offset) noexcept {
    if constexpr (Level == Dst::rank()) {
        function(output_offset, input_offset);
    } else if constexpr (Keep && Level == Axis) {
        walk_axis_lines<Level + 1, Axis, Keep>(dst, src, function, output_offset, input_offset);
    } else {
        constexpr auto source_axis = Level + (!Keep && Level >= Axis);
        const auto count = static_cast<size_t>(dst.extent(Level));
        const auto output_step = static_cast<size_t>(axis_stride<Level>(dst));
        const auto input_step = static_cast<size_t>(axis_stride<source_axis>(src));
        for (size_t i = 0; i != count; ++i) {
            walk_axis_lines<Level + 1, Axis, Keep>(dst, src, function, output_offset, input_offset);
            output_offset += output_step;
            input_offset += input_step;
        }
    }
}

template <class Accumulator, size_t Axis, bool Keep, class Dst, class Src>
constexpr void sum_axis_to(const Dst &dst, const Src &src) noexcept {
    using accumulator_type = sum_accumulator<Accumulator, typename Src::element_type>;
    constexpr auto rank = Dst::rank();
    for (size_t dst_axis = 0; dst_axis != rank; ++dst_axis) {
        const auto expected =
            Keep && dst_axis == Axis
                ? size_t{1}
                : static_cast<size_t>(src.extent(dst_axis + (!Keep && dst_axis >= Axis)));
        WJR_ASSERT_L2(static_cast<size_t>(dst.extent(dst_axis)) == expected);
    }
    if (dst.empty())
        return;
    const auto count = static_cast<size_t>(src.extent(Axis));
    if (count == 0) {
        apply_kernel(dst, [](typename Dst::reference value) noexcept { value = {}; });
        return;
    }
    if constexpr (Axis == 0 && Src::rank() == 2 &&
                  std::same_as<typename Src::layout_type, layout_right> &&
                  (std::same_as<typename Dst::layout_type, layout_right> ||
                   (!Keep && std::same_as<typename Dst::layout_type, layout_left>))) {
        axis_columns_to<false, accumulator_type>(dst.data_handle(), src.data_handle(), count,
                                                 static_cast<size_t>(src.extent(1)));
        return;
    }
    const auto step = static_cast<size_t>(axis_stride<Axis>(src));
    auto reduce = [input = src.data_handle(), output = dst.data_handle(), count,
                   step](size_t output_offset, size_t input_offset) noexcept {
        accumulator_type result{};
        constexpr auto static_count = Src::static_extent(Axis);
        const auto bound = static_count == dynamic_extent ? count : static_count;
        for (size_t i = 0; i != bound; ++i)
            result += static_cast<accumulator_type>(input[input_offset + i * step]);
        output[output_offset] = result;
    };
    walk_axis_lines<0, Axis, Keep>(dst, src, reduce, 0, 0);
}

} // namespace detail

// A left fold from zero in logical row-major order, independent of layout.
// Accumulator defaults to the scalar T+T result; output/storage is not allocated.
template <class Accumulator = void, class T, class Extents, class Layout>
requires(detail::kernel_element<T> && detail::kernel_layout<Layout> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>))
[[nodiscard]] constexpr auto sum(tensor_view<T, Extents, Layout> source) noexcept {
    using accumulator_type = detail::sum_accumulator<Accumulator, T>;
    accumulator_type result{};
    if (source.empty())
        return result;
    if constexpr (std::same_as<Layout, layout_right> || Extents::rank() == 0 ||
                  (Extents::rank() == 1 && std::same_as<Layout, layout_left>)) {
        const auto count = static_cast<size_t>(source.size());
        for (size_t i = 0; i != count; ++i)
            result += static_cast<accumulator_type>(source.data_handle()[i]);
    } else if constexpr (Extents::rank() == 1) {
        const auto count = static_cast<size_t>(source.extent(0));
        const auto step = static_cast<size_t>(source.stride(0));
        for (size_t i = 0; i != count; ++i)
            result += static_cast<accumulator_type>(source.data_handle()[i * step]);
    } else {
        constexpr auto rank = Extents::rank();
        std::array<size_t, rank> shape{};
        std::array<std::array<size_t, rank>, 1> strides{};
        for (size_t axis = 0; axis != rank; ++axis) {
            shape[axis] = static_cast<size_t>(source.extent(axis));
            strides[0][axis] = static_cast<size_t>(source.stride(axis));
        }
        auto add = [&](size_t offset) noexcept {
            result += static_cast<accumulator_type>(source.data_handle()[offset]);
        };
        detail::walk_kernel<0, false>(shape, strides, add, std::index_sequence<0>{}, size_t{0});
    }
    return result;
}

// Preconditions: valid views, matching reduced output shape, and disjoint
// logical input/output storage. Each result folds its selected axis from zero.
template <class Accumulator = void, class D, class DstExtents, class DstLayout, class T,
          class SrcExtents, class SrcLayout, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_element<T> &&
         detail::kernel_layout<DstLayout> && detail::kernel_layout<SrcLayout> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>) &&
         detail::compatible_sum_shape<DstExtents, SrcExtents, Axis, false>)
constexpr void sum_to(tensor_view<D, DstExtents, DstLayout> dst,
                      tensor_view<T, SrcExtents, SrcLayout> src, axes_t<Axis>) noexcept {
    detail::sum_axis_to<Accumulator, Axis, false>(dst, src);
}

template <class Accumulator = void, class D, class DstExtents, class DstLayout, class T,
          class SrcExtents, class SrcLayout, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_element<T> &&
         detail::kernel_layout<DstLayout> && detail::kernel_layout<SrcLayout> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>) &&
         detail::compatible_sum_shape<DstExtents, SrcExtents, Axis, true>)
constexpr void sum_to(tensor_view<D, DstExtents, DstLayout> dst,
                      tensor_view<T, SrcExtents, SrcLayout> src, axes_t<Axis>,
                      keepdims_t) noexcept {
    detail::sum_axis_to<Accumulator, Axis, true>(dst, src);
}

} // namespace wjr::np

#endif // WJR_NP_REDUCE_HPP__
