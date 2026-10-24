#ifndef WJR_NP_BINCOUNT_HPP__
#define WJR_NP_BINCOUNT_HPP__

#include <wjr/np/reduce.hpp>

namespace wjr::np {

namespace detail {

template <class T>
concept bincount_index =
    std::is_integral_v<T> && !std::is_volatile_v<T> && !std::same_as<std::remove_cv_t<T>, bool>;

template <bool Weighted, class D, class I, class W, class OutputStep, class IndexStep,
          class WeightStep>
constexpr void bincount_line(D *WJR_RESTRICT output, size_t bins, OutputStep output_step,
                             const I *indices, size_t count, IndexStep index_step, const W *weights,
                             WeightStep weight_step) noexcept {
    for (size_t i = 0; i != bins; ++i)
        output[i * output_step] = D{};
    for (size_t i = 0; i != count; ++i) {
        const auto value = indices[i * index_step];
        if constexpr (std::is_signed_v<I>)
            WJR_ASSERT_L2(value >= 0);
        WJR_ASSERT_L2(static_cast<std::make_unsigned_t<I>>(value) < bins);
        auto &bin = output[static_cast<size_t>(value) * output_step];
        if constexpr (Weighted)
            bin += static_cast<D>(weights[i * weight_step]);
        else
            ++bin;
    }
}

} // namespace detail

// Zero logical bins, then count indices. Output must be disjoint from inputs;
// indices are nonnegative and in range, and every count is representable in D.
template <class D, class DE, class DL, class I, class IE, class IL>
requires(detail::bincount_index<D> && !std::is_const_v<D> && detail::bincount_index<I> &&
         DE::rank() == 1 && IE::rank() == 1 && detail::kernel_layout<DL> &&
         detail::kernel_layout<IL>)
constexpr void bincount_to(tensor_view<D, DE, DL> dst, tensor_view<I, IE, IL> indices) noexcept {
    detail::bincount_line<false>(
        dst.data_handle(), static_cast<size_t>(dst.extent(0)), detail::axis_stride<0>(dst),
        indices.data_handle(), static_cast<size_t>(indices.extent(0)),
        detail::axis_stride<0>(indices), static_cast<const D *>(nullptr), 1);
}

// Each weight is converted to D before ordered accumulation into its bin.
template <class D, class DE, class DL, class I, class IE, class IL, class W, class WE, class WL>
requires(std::is_floating_point_v<D> && !std::is_const_v<D> && !std::is_volatile_v<D> &&
         detail::bincount_index<I> && detail::kernel_element<W> && DE::rank() == 1 &&
         IE::rank() == 1 && detail::compatible_kernel_shape<IE, WE> && detail::kernel_layout<DL> &&
         detail::kernel_layout<IL> && detail::kernel_layout<WL>)
constexpr void bincount_to(tensor_view<D, DE, DL> dst, tensor_view<I, IE, IL> indices,
                           tensor_view<W, WE, WL> weights) noexcept {
    detail::assert_kernel_shape(indices, weights);
    detail::bincount_line<true>(
        dst.data_handle(), static_cast<size_t>(dst.extent(0)), detail::axis_stride<0>(dst),
        indices.data_handle(), static_cast<size_t>(indices.extent(0)),
        detail::axis_stride<0>(indices), weights.data_handle(), detail::axis_stride<0>(weights));
}

} // namespace wjr::np

#endif // WJR_NP_BINCOUNT_HPP__
