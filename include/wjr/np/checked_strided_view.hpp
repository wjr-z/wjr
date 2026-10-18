#ifndef WJR_NP_CHECKED_STRIDED_VIEW_HPP__
#define WJR_NP_CHECKED_STRIDED_VIEW_HPP__

#include <utility>

#include <wjr/np/checked_view.hpp>

namespace wjr::np {

namespace detail {

template <class Extents, class T, std::integral Integer, std::integral StrideInteger, class Factory>
constexpr auto with_strided_mapping(
    const std::array<Integer, Extents::rank()> &shape,
    const std::array<StrideInteger, Extents::rank()> &input_strides,
    const Factory
        &factory) noexcept(noexcept(factory(std::declval<const typename layout_stride::
                                                             template mapping<Extents> &>(),
                                            size_t{})))
    -> decltype(factory(std::declval<const typename layout_stride::template mapping<Extents> &>(),
                        size_t{})) {
    using index_type = typename Extents::index_type;
    using mapping_type = typename layout_stride::template mapping<Extents>;
    auto metadata = detail::check_metadata<T, Extents>(shape);
    if (!metadata)
        return unexpected(metadata.error());
    const auto &dimensions = metadata->dimensions;

    std::array<index_type, Extents::rank()> strides{};
    for (size_t r = 0; r != Extents::rank(); ++r) {
        if (input_strides[r] <= 0)
            return unexpected(shape_error::nonpositive_stride);
        if (!wjr::in_range<index_type>(input_strides[r])) {
            return unexpected(shape_error::stride_out_of_range);
        }
        strides[r] = static_cast<index_type>(input_strides[r]);
    }

    // A fixed-rank insertion sort keeps this boundary free of allocation. For
    // nonempty shapes, equal strides must put singleton axes first. Empty axes
    // go first; this is a conservative witness for the backend's precondition.
    std::array<size_t, Extents::rank()> order{};
    const auto before = [&](size_t a, size_t b) {
        const auto ea = dimensions[a], eb = dimensions[b];
        if ((ea == 0) != (eb == 0))
            return ea == 0;
        if (strides[a] != strides[b])
            return strides[a] < strides[b];
        return ea < eb;
    };
    for (size_t r = 0; r != Extents::rank(); ++r) {
        order[r] = r;
        for (size_t p = r; p != 0 && before(order[p], order[p - 1]); --p) {
            std::swap(order[p], order[p - 1]);
        }
    }
    for (size_t r = 1; r < Extents::rank(); ++r) {
        const auto a = order[r - 1], b = order[r];
        const auto extent = dimensions[a];
        // Compare the mathematical product without forming it. In particular,
        // no stride * extent is required for the final axis in the permutation.
        if (extent != 0 && strides[a] > strides[b] / extent) {
            return unexpected(shape_error::unsupported_stride_order);
        }
    }

    // Match the pinned backend's observer order: it accumulates before checking
    // later zero extents, so even an empty mapping needs safe earlier prefixes.
    constexpr index_type max_index = std::numeric_limits<index_type>::max();
    index_type span = 1;
    for (size_t r = 0; r != Extents::rank(); ++r) {
        const auto dimension = dimensions[r];
        if (dimension == 0) {
            span = 0;
            break;
        }
        const index_type steps = dimension - 1;
        if (steps != 0 && strides[r] > (max_index - span) / steps) {
            return unexpected(shape_error::index_overflow);
        }
        span += steps * strides[r];
    }
    if (!wjr::in_range<size_t>(span))
        return unexpected(shape_error::size_overflow);
    const auto required = static_cast<size_t>(span);
    constexpr auto max_bytes = static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max());
    if (required > max_bytes / sizeof(T))
        return unexpected(shape_error::size_overflow);
    const Extents extents(dimensions);
    return factory(mapping_type(extents, strides), required);
}

} // namespace detail

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

#endif // WJR_NP_CHECKED_STRIDED_VIEW_HPP__
