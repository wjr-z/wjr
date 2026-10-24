#ifndef WJR_NP_PUT_HPP__
#define WJR_NP_PUT_HPP__

#include <wjr/np/reduce.hpp>

namespace wjr::np {
namespace detail {

// Only the written span is restricted: the two read-only inputs may overlap.
template <class D, class I, class T, class DS, class IS, class VS>
constexpr void put_linear(D *WJR_RESTRICT output, const I *indices, const T *values, size_t count,
                          [[maybe_unused]] size_t destination_count, DS output_step, IS index_step,
                          VS value_step) noexcept {
    for (size_t i = 0; i != count; ++i) {
        const auto value = indices[i * index_step];
        if constexpr (std::is_signed_v<I>) {
            WJR_ASSERT_L2(value >= 0);
        }
        WJR_ASSERT_L2(static_cast<std::make_unsigned_t<I>>(value) < destination_count);
        const auto index = static_cast<size_t>(value);
        output[index * output_step] = static_cast<D>(values[i * value_step]);
    }
}

} // namespace detail

// Scatter in logical index order. Repeated indices keep the last value;
// destination storage is disjoint from both inputs and other entries are intact.
template <class D, class DE, class DL, class I, class IE, class IL, class T, class VE, class VL>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_element<T> &&
         std::is_integral_v<I> && !std::same_as<std::remove_cv_t<I>, bool> &&
         !std::is_volatile_v<I> && DE::rank() == 1 && IE::rank() == 1 && VE::rank() == 1 &&
         detail::kernel_layout<DL> && detail::kernel_layout<IL> && detail::kernel_layout<VL> &&
         detail::compatible_kernel_shape<IE, VE>)
constexpr void put_to(tensor_view<D, DE, DL> destination, tensor_view<I, IE, IL> indices,
                      tensor_view<T, VE, VL> values) noexcept {
    detail::assert_kernel_shape(indices, values);
    const auto count = static_cast<size_t>(indices.extent(0));
    if (count == 0)
        return;
    detail::put_linear(destination.data_handle(), indices.data_handle(), values.data_handle(),
                       count, static_cast<size_t>(destination.extent(0)),
                       detail::axis_stride<0>(destination), detail::axis_stride<0>(indices),
                       detail::axis_stride<0>(values));
}

} // namespace wjr::np

#endif // WJR_NP_PUT_HPP__
