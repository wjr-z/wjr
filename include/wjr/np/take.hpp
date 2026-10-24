#ifndef WJR_NP_TAKE_HPP__
#define WJR_NP_TAKE_HPP__

#include <wjr/np/reduce.hpp>

namespace wjr::np {
namespace detail {

// Packed disjoint spans allow the same alias information as a handwritten gather.
template <class D, class T, class I>
constexpr void take_linear(D *WJR_RESTRICT output, const T *WJR_RESTRICT input,
                           const I *WJR_RESTRICT indices, size_t count,
                           [[maybe_unused]] size_t source_count) noexcept {
    for (size_t i = 0; i != count; ++i) {
        const auto index = static_cast<size_t>(indices[i]);
        WJR_ASSERT_L2(index < source_count);
        output[i] = static_cast<D>(input[index]);
    }
}

} // namespace detail

// Gather from a rank-one source. Output has the indices' shape. Indices are
// nonnegative and in range; output storage is disjoint from both inputs.
template <class D, class DE, class DL, class T, class SE, class SL, class I, class IE, class IL>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_element<T> &&
         std::is_integral_v<I> && !std::same_as<std::remove_cv_t<I>, bool> &&
         !std::is_volatile_v<I> && SE::rank() == 1 && detail::kernel_layout<DL> &&
         detail::kernel_layout<SL> && detail::kernel_layout<IL> &&
         detail::compatible_kernel_shape<DE, IE>)
constexpr void take_to(tensor_view<D, DE, DL> destination, tensor_view<T, SE, SL> source,
                       tensor_view<I, IE, IL> indices) noexcept {
    detail::assert_kernel_shape(destination, indices);
    if (destination.empty())
        return;
    const auto *input = source.data_handle();
    const auto source_count = static_cast<size_t>(source.extent(0));
    if constexpr (detail::packed_layout<SL> &&
                  (DE::rank() == 0 || (detail::packed_layout<DL> && detail::packed_layout<IL> &&
                                       (DE::rank() <= 1 || std::same_as<DL, IL>)))) {
        detail::take_linear(destination.data_handle(), input, indices.data_handle(),
                            destination.size(), source_count);
    } else {
        const auto step = static_cast<size_t>(detail::axis_stride<0>(source));
        detail::apply_kernel(
            destination,
            [input, step, source_count](D &output, const I &value) noexcept {
                const auto index = static_cast<size_t>(value);
                WJR_ASSERT_L2(index < source_count);
                output = static_cast<D>(input[index * step]);
            },
            indices);
    }
}

} // namespace wjr::np

#endif // WJR_NP_TAKE_HPP__
