#ifndef WJR_NP_RESHAPE_HPP__
#define WJR_NP_RESHAPE_HPP__

#include <wjr/np/shape.hpp>

namespace wjr::np {

namespace detail {

template <class Layout>
concept contiguous_layout = std::same_as<Layout, layout_right> || std::same_as<Layout, layout_left>;

template <class Extents>
inline constexpr size_t flattened_extent = [] {
    if constexpr (Extents::rank_dynamic() != 0) {
        return dynamic_extent;
    } else {
        using index_type = typename Extents::index_type;
        // dynamic_extent is a sentinel, not a representable static extent.
        constexpr size_t max_count = [] {
            if constexpr (std::numeric_limits<index_type>::digits >=
                          std::numeric_limits<size_t>::digits) {
                return dynamic_extent - 1;
            } else {
                return static_cast<size_t>(std::numeric_limits<index_type>::max());
            }
        }();
        bool empty = false;
        for (size_t r = 0; r != Extents::rank(); ++r) {
            if (!wjr::in_range<index_type>(Extents::static_extent(r))) {
                return dynamic_extent;
            }
            empty |= Extents::static_extent(r) == 0;
        }
        if (empty)
            return size_t{0};
        size_t count = 1;
        for (size_t r = 0; r != Extents::rank(); ++r) {
            const auto dimension = Extents::static_extent(r);
            if (count > max_count / dimension)
                return dynamic_extent;
            count *= dimension;
        }
        return count;
    }
}();

template <class Extents>
struct flattened_extents {
    static_assert(Extents::rank_dynamic() != 0 || flattened_extent<Extents> != dynamic_extent,
                  "flattened static extent is not representable");
    using type = extents<typename Extents::index_type, flattened_extent<Extents>>;
};

} // namespace detail

// Precondition: source is a valid contiguous view. Physical storage order and
// the backing objects' lifetime are unchanged; no shape or storage check occurs.
template <class T, class Extents, class Layout>
requires detail::contiguous_layout<Layout>
[[nodiscard]] constexpr auto flatten_view(tensor_view<T, Extents, Layout> source) noexcept {
    using flat_extents = typename detail::flattened_extents<Extents>::type;
    return tensor_view<T, flat_extents, Layout>(
        source.data_handle(), flat_extents(source.mapping().required_span_size()));
}

// Precondition: source is valid. Only the new shape is validated; its element
// count must equal source's. Layout is preserved, so this performs no transpose.
template <class NewExtents, class T, class Extents, class Layout, std::integral Integer>
requires(detail::is_extents<NewExtents> && detail::contiguous_layout<Layout> &&
         !std::same_as<std::remove_cv_t<Integer>, bool> && std::is_object_v<T> &&
         requires { sizeof(T); })
[[nodiscard]] constexpr shape_result<tensor_view<T, NewExtents, Layout>>
try_reshape_view(tensor_view<T, Extents, Layout> source,
                 const std::array<Integer, NewExtents::rank()> &shape) noexcept {
    const auto count = source.mapping().required_span_size();
    auto metadata = detail::check_metadata<T, NewExtents>(shape);
    if (!metadata)
        return unexpected(metadata.error());
    if (!wjr::cmp_equal(metadata->size, count)) {
        return unexpected(shape_error::element_count_mismatch);
    }
    using mapping_type = typename Layout::template mapping<NewExtents>;
    return tensor_view<T, NewExtents, Layout>(source.data_handle(),
                                              mapping_type(NewExtents(metadata->dimensions)));
}

} // namespace wjr::np

#endif // WJR_NP_RESHAPE_HPP__
