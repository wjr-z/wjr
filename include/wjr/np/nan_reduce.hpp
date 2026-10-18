#ifndef WJR_NP_NAN_REDUCE_HPP__
#define WJR_NP_NAN_REDUCE_HPP__

#include <array>
#include <limits>
#include <type_traits>

#include <wjr/np/mean.hpp>

namespace wjr::np {

namespace detail {

struct no_nan_count {};

template <class Accumulator, bool Mean, class T, class Extents, class Layout>
constexpr auto nan_reduce_view(tensor_view<T, Extents, Layout> source) noexcept {
    using count_type = std::conditional_t<Mean, size_t, no_nan_count>;
    Accumulator result{};
    [[maybe_unused]] count_type valid_count{};
    if (source.empty()) {
        if constexpr (Mean)
            return std::numeric_limits<Accumulator>::quiet_NaN();
        else
            return result;
    }
    auto add = [&](size_t offset) constexpr noexcept {
        const auto value = source.data_handle()[offset];
        if constexpr (std::is_floating_point_v<std::remove_cv_t<T>>) {
            if (value != value)
                return;
        }
        result += static_cast<Accumulator>(value);
        if constexpr (Mean)
            ++valid_count;
    };
    if constexpr (std::same_as<Layout, layout_right> || Extents::rank() == 0 ||
                  (Extents::rank() == 1 && std::same_as<Layout, layout_left>)) {
        const auto count = static_cast<size_t>(source.size());
        for (size_t i = 0; i != count; ++i)
            add(i);
    } else if constexpr (Extents::rank() == 1) {
        const auto count = static_cast<size_t>(source.extent(0));
        const auto step = static_cast<size_t>(source.stride(0));
        for (size_t i = 0; i != count; ++i)
            add(i * step);
    } else {
        constexpr auto rank = Extents::rank();
        std::array<size_t, rank> shape{};
        std::array<std::array<size_t, rank>, 1> strides{};
        for (size_t axis = 0; axis != rank; ++axis) {
            shape[axis] = static_cast<size_t>(source.extent(axis));
            strides[0][axis] = static_cast<size_t>(source.stride(axis));
        }
        auto visit = [&](size_t offset) constexpr noexcept { add(offset); };
        walk_kernel<0, false>(shape, strides, visit, std::index_sequence<0>{}, size_t{0});
    }
    if constexpr (Mean) {
        if (valid_count == 0)
            return std::numeric_limits<Accumulator>::quiet_NaN();
        return result / static_cast<Accumulator>(valid_count);
    } else {
        return result;
    }
}

} // namespace detail

// NaNs are skipped before conversion to the accumulator. The fold keeps the
// logical row-major order; integer views dispatch directly to sum.
template <class Accumulator = void, class T, class Extents, class Layout>
requires(detail::kernel_element<T> && detail::kernel_layout<Layout> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>))
[[nodiscard]] constexpr auto nansum(tensor_view<T, Extents, Layout> source) noexcept {
    if constexpr (std::is_floating_point_v<std::remove_cv_t<T>>)
        return detail::nan_reduce_view<detail::sum_accumulator<Accumulator, T>, false>(source);
    else
        return sum<Accumulator>(source);
}

// NaNs are skipped before conversion and do not contribute to the divisor.
// Empty and all-NaN views return quiet NaN; integer views dispatch directly to mean.
template <class Accumulator = void, class T, class Extents, class Layout>
requires(detail::kernel_element<T> && detail::kernel_layout<Layout> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>))
[[nodiscard]] constexpr auto nanmean(tensor_view<T, Extents, Layout> source) noexcept {
    if constexpr (std::is_floating_point_v<std::remove_cv_t<T>>)
        return detail::nan_reduce_view<detail::mean_accumulator<Accumulator, T>, true>(source);
    else
        return mean<Accumulator>(source);
}

} // namespace wjr::np

#endif // WJR_NP_NAN_REDUCE_HPP__
