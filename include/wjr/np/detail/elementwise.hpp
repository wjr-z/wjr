#ifndef WJR_NP_DETAIL_ELEMENTWISE_HPP__
#define WJR_NP_DETAIL_ELEMENTWISE_HPP__

#include <array>
#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

#include <wjr/assert.hpp>
#include <wjr/np/view.hpp>

namespace wjr::np::detail {

template <class T>
concept kernel_element = std::is_arithmetic_v<T> && !std::is_volatile_v<T>;

template <class Layout>
concept kernel_layout = std::same_as<Layout, layout_left> || std::same_as<Layout, layout_right> ||
                        std::same_as<Layout, layout_stride>;

template <class DstExtents, class SrcExtents>
inline constexpr bool compatible_kernel_shape = [] {
    if constexpr (DstExtents::rank() != SrcExtents::rank()) {
        return false;
    } else {
        for (size_t axis = 0; axis != DstExtents::rank(); ++axis) {
            const auto dst = DstExtents::static_extent(axis);
            const auto src = SrcExtents::static_extent(axis);
            if (dst != dynamic_extent && src != dynamic_extent && dst != src)
                return false;
        }
        return true;
    }
}();

template <class Dst, class Src>
constexpr void assert_kernel_shape(Dst dst, Src src) noexcept {
    for (size_t axis = 0; axis != Dst::rank(); ++axis) {
        // Valid mdspan extents are nonnegative and representable in size_t.
        WJR_ASSERT_L2(static_cast<size_t>(dst.extent(axis)) ==
                      static_cast<size_t>(src.extent(axis)));
    }
}

template <size_t Level, bool Left, size_t Rank, size_t Count, class Function, size_t... I,
          class... Offsets>
constexpr void
walk_kernel(const std::array<size_t, Rank> &shape,
            const std::array<std::array<size_t, Rank>, Count> &strides, Function &function,
            std::index_sequence<I...> operands,
            Offsets... offsets) noexcept(std::is_nothrow_invocable_v<Function &, Offsets...>) {
    if constexpr (Level == Rank) {
        function(offsets...);
    } else {
        constexpr auto axis = Left ? Rank - 1 - Level : Level;
        for (size_t i = 0; i != shape[axis]; ++i) {
            walk_kernel<Level + 1, Left>(shape, strides, function, operands, offsets...);
            ((offsets += strides[I][axis]), ...);
        }
    }
}

// The caller has checked shape once and returned for empty output. The leaf
// receives mutable output and const input references, without mdspan indexing.
template <class Dst, class Function, class... Src>
constexpr void apply_kernel(Dst dst, Function &&function, Src... src) noexcept(
    std::is_nothrow_invocable_v<Function &, typename Dst::reference,
                                const typename Src::value_type &...>) {
    constexpr auto rank = Dst::rank();
    constexpr bool dense =
        rank == 0 ||
        (!std::same_as<typename Dst::layout_type, layout_stride> &&
         ((!std::same_as<typename Src::layout_type, layout_stride> &&
           (rank <= 1 || std::same_as<typename Dst::layout_type, typename Src::layout_type>)) &&
          ...));
    if constexpr (dense) {
        const auto count = static_cast<size_t>(dst.size());
        for (size_t i = 0; i != count; ++i)
            std::invoke(function, dst.data_handle()[i], std::as_const(src.data_handle()[i])...);
    } else if constexpr (rank == 1) {
        const auto count = static_cast<size_t>(dst.extent(0));
        const auto output_stride = static_cast<size_t>(dst.stride(0));
        [&](auto... input_strides) {
            for (size_t i = 0; i != count; ++i)
                std::invoke(function, dst.data_handle()[i * output_stride],
                            std::as_const(src.data_handle()[i * input_strides])...);
        }(static_cast<size_t>(src.stride(0))...);
    } else {
        constexpr size_t operands = 1 + sizeof...(Src);
        std::array<size_t, rank> shape{};
        std::array<std::array<size_t, rank>, operands> strides{};
        for (size_t axis = 0; axis != rank; ++axis) {
            shape[axis] = static_cast<size_t>(dst.extent(axis));
            strides[0][axis] = static_cast<size_t>(dst.stride(axis));
            size_t operand = 1;
            ((strides[operand++][axis] = static_cast<size_t>(src.stride(axis))), ...);
        }
        auto leaf = [&](size_t output_offset, auto... input_offsets) noexcept(
                        std::is_nothrow_invocable_v<Function &, typename Dst::reference,
                                                    const typename Src::value_type &...>) {
            std::invoke(function, dst.data_handle()[output_offset],
                        std::as_const(src.data_handle()[input_offsets])...);
        };
        [&]<size_t... I>(std::index_sequence<I...> indices) {
            walk_kernel<0, std::same_as<typename Dst::layout_type, layout_left>>(
                shape, strides, leaf, indices, decltype(I){}...);
        }(std::make_index_sequence<operands>{});
    }
}

} // namespace wjr::np::detail

#endif // WJR_NP_DETAIL_ELEMENTWISE_HPP__
