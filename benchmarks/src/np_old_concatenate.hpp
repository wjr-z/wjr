#ifndef WJR_NP_OLD_CONCATENATE_HPP__
#define WJR_NP_OLD_CONCATENATE_HPP__

#include <algorithm>

#include <wjr/np/reduce.hpp>

namespace wjr::np::old_concat {
using namespace ::wjr::np;
namespace detail {
using namespace ::wjr::np::detail;

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
            std::copy_n(input + input_offset, count, output + output_offset);
        } else {
            const auto output_step = static_cast<size_t>(axis_stride<axis>(dst));
            const auto input_step = static_cast<size_t>(axis_stride<axis>(src));
            for (size_t i = 0; i != count; ++i)
                output[output_offset + i * output_step] = input[input_offset + i * input_step];
        }
    } else {
        const auto output_step = static_cast<size_t>(axis_stride<axis>(dst));
        const auto input_step = static_cast<size_t>(axis_stride<axis>(src));
        for (size_t i = 0; i != count; ++i)
            concatenate_copy<Level + 1>(output, input, dst, src, output_offset + i * output_step,
                                        input_offset + i * input_step);
    }
}

template <class Src>
struct concatenate_operand {
    const Src &view;
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
        std::copy_n(input, count * static_cast<size_t>(axis_stride<Axis>(src)), output);
    } else {
        concatenate_copy<Level>(output, input, dst, src);
    }
}

// Coordinates outside the concatenation axis are shared across all inputs.
// Operand offsets are scalar loop indices, without owning/copying descriptors.
// Inlining keeps caller-known strides out of an outlined descriptor-based loop.
template <size_t Axis, size_t Level, class Dst, class... Src>
WJR_INTRINSIC_INLINE constexpr void
concatenate_outer(const Dst &dst, size_t output_offset,
                  concatenate_operand<Src>... sources) noexcept {
    constexpr size_t axis =
        left_layout<typename Dst::layout_type> ? Dst::rank() - 1 - Level : Level;
    const auto output_step = static_cast<size_t>(axis_stride<axis>(dst));
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
                concatenate_operand<Src>{
                    sources.view,
                    sources.offset + i * static_cast<size_t>(axis_stride<axis>(sources.view))}...);
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
constexpr void concatenate_to(tensor_view<D, DE, DL> destination, axes_t<Axis>,
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
    detail::concatenate_outer<Axis, 0>(
        destination, 0, detail::concatenate_operand<tensor_view<T, SE, SL>>{sources, 0}...);
}

} // namespace wjr::np::old_concat

#endif // WJR_NP_OLD_CONCATENATE_HPP__
