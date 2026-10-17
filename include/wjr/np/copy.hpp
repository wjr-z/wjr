#ifndef WJR_NP_COPY_HPP__
#define WJR_NP_COPY_HPP__

#include <algorithm>

#include <wjr/np/detail/elementwise.hpp>

namespace wjr::np {

// Preconditions: valid views, equal shapes, and either disjoint logical storage
// or exactly the same element at each logical index. Other overlap is unsupported.
template <class T, class DstExtents, class DstLayout, class S, class SrcExtents, class SrcLayout>
requires(detail::kernel_element<T> && !std::is_const_v<T> && detail::kernel_element<S> &&
         std::same_as<T, std::remove_cv_t<S>> && detail::kernel_layout<DstLayout> &&
         detail::kernel_layout<SrcLayout> &&
         detail::compatible_kernel_shape<DstExtents, SrcExtents>)
constexpr void copy_to(tensor_view<T, DstExtents, DstLayout> dst,
                       tensor_view<S, SrcExtents, SrcLayout> src) noexcept {
    detail::assert_kernel_shape(dst, src);
    if (dst.empty())
        return;
    if (dst.data_handle() == src.data_handle()) {
        bool identical = true;
        if constexpr (DstExtents::rank() != 0) {
            for (size_t axis = 0; axis != DstExtents::rank(); ++axis)
                if (dst.extent(axis) > 1 && dst.stride(axis) != src.stride(axis))
                    identical = false;
        }
        if (identical)
            return;
    }
    if constexpr (!std::same_as<DstLayout, layout_stride> &&
                  !std::same_as<SrcLayout, layout_stride> &&
                  (DstExtents::rank() <= 1 || std::same_as<DstLayout, SrcLayout>)) {
        std::copy_n(src.data_handle(), static_cast<size_t>(dst.size()), dst.data_handle());
    } else {
        detail::apply_kernel(dst, [](T &out, const T &in) noexcept { out = in; }, src);
    }
}

// value is captured once, including when the argument names an output element.
// Only logical elements are written; padding is preserved.
template <class T, class Extents, class Layout>
requires(detail::kernel_element<T> && !std::is_const_v<T> && detail::kernel_layout<Layout>)
constexpr void fill_to(tensor_view<T, Extents, Layout> dst,
                       std::type_identity_t<T> value) noexcept {
    if (dst.empty())
        return;
    if constexpr (!std::same_as<Layout, layout_stride>) {
        std::fill_n(dst.data_handle(), static_cast<size_t>(dst.size()), value);
    } else {
        detail::apply_kernel(dst, [value](T &out) noexcept { out = value; });
    }
}

} // namespace wjr::np

#endif // WJR_NP_COPY_HPP__
