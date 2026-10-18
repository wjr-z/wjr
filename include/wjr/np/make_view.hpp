#ifndef WJR_NP_MAKE_VIEW_HPP__
#define WJR_NP_MAKE_VIEW_HPP__

#include <wjr/np/detail/strided_mapping.hpp>

namespace wjr::np {

// Optional recoverable construction. Every result contains the same ordinary
// tensor_view; trusted callers can construct it directly without this layer.

// Checks metadata before constructing the unchecked backend mapping. The caller
// still owns the storage and must provide correctly aligned, live T objects.
template <class Extents, class Layout = layout_right, class T, std::integral Integer>
requires(detail::is_extents<Extents> &&
         (std::same_as<Layout, layout_right> || std::same_as<Layout, layout_left>) &&
         !std::same_as<std::remove_cv_t<Integer>, bool> && std::is_object_v<T> &&
         requires { sizeof(T); })
[[nodiscard]] constexpr shape_result<tensor_view<T, Extents, Layout>>
try_make_tensor_view(T *data, size_t available_elements,
                     const std::array<Integer, Extents::rank()> &shape) noexcept {
    using view_type = tensor_view<T, Extents, Layout>;
    auto metadata = detail::check_metadata<T, Extents>(shape);
    if (!metadata) {
        return unexpected(metadata.error());
    }
    if (metadata->size > available_elements) {
        return unexpected(shape_error::insufficient_storage);
    }
    if (data == nullptr && available_elements != 0) {
        return unexpected(shape_error::null_storage);
    }

    const Extents extents(metadata->dimensions);
    const typename view_type::mapping_type mapping(extents);
    return view_type(data, mapping);
}

// Strides count T objects, not bytes. Only the backend's ordered, nonoverlapping
// stride family is accepted; arbitrary injective mappings need another layout.
template <class Extents, class T, std::integral Integer, std::integral StrideInteger>
requires(detail::is_extents<Extents> && !std::same_as<std::remove_cv_t<Integer>, bool> &&
         !std::same_as<std::remove_cv_t<StrideInteger>, bool> && std::is_object_v<T> &&
         requires { sizeof(T); })
[[nodiscard]] constexpr shape_result<tensor_view<T, Extents, layout_stride>>
try_make_strided_tensor_view(
    T *data, size_t available_elements, const std::array<Integer, Extents::rank()> &shape,
    const std::array<StrideInteger, Extents::rank()> &input_strides) noexcept {
    return detail::with_strided_mapping<Extents, T>(
        shape, input_strides,
        [=](const auto &mapping,
            size_t span) noexcept -> shape_result<tensor_view<T, Extents, layout_stride>> {
            if (span > available_elements)
                return unexpected(shape_error::insufficient_storage);
            if (data == nullptr && available_elements != 0)
                return unexpected(shape_error::null_storage);
            return tensor_view<T, Extents, layout_stride>(data, mapping);
        });
}

} // namespace wjr::np

#endif // WJR_NP_MAKE_VIEW_HPP__
