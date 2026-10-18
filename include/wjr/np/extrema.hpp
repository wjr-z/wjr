#ifndef WJR_NP_EXTREMA_HPP__
#define WJR_NP_EXTREMA_HPP__

#include <wjr/np/detail/elementwise.hpp>

namespace wjr::np {

namespace detail {

// The initial value is already consumed. Skip it only on the first line,
// propagate an early NaN exit, and retain logical row-major order.
template <size_t Level, size_t Rank, class Function>
constexpr bool walk_extrema(const std::array<size_t, Rank> &shape,
                            const std::array<size_t, Rank> &strides, Function &function,
                            size_t offset, bool first) noexcept {
    if constexpr (Level + 1 == Rank) {
        size_t i = first ? 1 : 0;
        offset += i * strides[Level];
        for (; i != shape[Level]; ++i) {
            if (!function(offset))
                return false;
            offset += strides[Level];
        }
    } else {
        for (size_t i = 0; i != shape[Level]; ++i) {
            if (!walk_extrema<Level + 1>(shape, strides, function, offset, first))
                return false;
            first = false;
            offset += strides[Level];
        }
    }
    return true;
}

template <bool Maximum, bool Index, class View>
constexpr auto extremum(View source) noexcept {
    WJR_ASSERT_L2(!source.empty());
    using value_type = typename View::value_type;
    const auto *input = source.data_handle();
    value_type best = input[0];
    size_t position = 1, selected = 0;
    auto result = [&]() noexcept {
        if constexpr (Index)
            return selected;
        else
            return best;
    };
    if constexpr (std::floating_point<value_type>)
        if (best != best)
            return result();
    auto visit = [&](size_t offset) noexcept {
        const value_type value = input[offset];
        if constexpr (std::floating_point<value_type>) {
            if (value != value) {
                best = value;
                if constexpr (Index)
                    selected = position;
                return false;
            }
        }
        if (Maximum ? best < value : value < best) {
            best = value;
            if constexpr (Index)
                selected = position;
        }
        if constexpr (Index)
            ++position;
        return true;
    };
    if constexpr (std::same_as<typename View::layout_type, layout_right> || View::rank() == 0 ||
                  (View::rank() == 1 && std::same_as<typename View::layout_type, layout_left>)) {
        const auto count = static_cast<size_t>(source.size());
        for (size_t i = 1; i != count; ++i)
            if (!visit(i))
                break;
    } else if constexpr (View::rank() == 1) {
        const auto count = static_cast<size_t>(source.extent(0));
        const auto step = static_cast<size_t>(source.stride(0));
        for (size_t i = 1; i != count; ++i)
            if (!visit(i * step))
                break;
    } else {
        constexpr auto rank = View::rank();
        std::array<size_t, rank> shape{}, strides{};
        for (size_t axis = 0; axis != rank; ++axis) {
            shape[axis] = static_cast<size_t>(source.extent(axis));
            strides[axis] = static_cast<size_t>(source.stride(axis));
        }
        walk_extrema<0>(shape, strides, visit, 0, true);
    }
    return result();
}

} // namespace detail

// Nonempty trusted views only. Equal values (including signed zeros) retain
// the first logical row-major occurrence; the first NaN propagates.
template <class T, class Extents, class Layout>
requires(detail::kernel_element<T> && detail::kernel_layout<Layout>)
[[nodiscard]] constexpr auto(min)(tensor_view<T, Extents, Layout> source) noexcept {
    return detail::extremum<false, false>(source);
}

template <class T, class Extents, class Layout>
requires(detail::kernel_element<T> && detail::kernel_layout<Layout>)
[[nodiscard]] constexpr auto(max)(tensor_view<T, Extents, Layout> source) noexcept {
    return detail::extremum<true, false>(source);
}

// Indices are logical row-major ordinals, independent of physical strides.
template <class T, class Extents, class Layout>
requires(detail::kernel_element<T> && detail::kernel_layout<Layout>)
[[nodiscard]] constexpr size_t argmin(tensor_view<T, Extents, Layout> source) noexcept {
    return detail::extremum<false, true>(source);
}

template <class T, class Extents, class Layout>
requires(detail::kernel_element<T> && detail::kernel_layout<Layout>)
[[nodiscard]] constexpr size_t argmax(tensor_view<T, Extents, Layout> source) noexcept {
    return detail::extremum<true, true>(source);
}

} // namespace wjr::np

#endif // WJR_NP_EXTREMA_HPP__
