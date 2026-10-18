#ifndef WJR_NP_SLICE_HPP__
#define WJR_NP_SLICE_HPP__

#include <array>
#include <limits>
#include <type_traits>
#include <utility>

#include <wjr/np/detail/layout.hpp>

namespace wjr::np {

using Kokkos::full_extent;
using Kokkos::full_extent_t;
using Kokkos::strided_slice;

namespace detail {
template <class Slice>
inline constexpr bool supported_static_slice = true;
template <class Offset, class Extent, class Step>
inline constexpr bool supported_static_slice<strided_slice<Offset, Extent, Step>> = [] {
    if constexpr (requires {
                      Extent::value;
                      Step::value;
                  }) {
        // This backend passes floor(extent/step) to a ceil-sized static extent.
        return Extent::value >= 0 && Step::value > 0 && Extent::value % Step::value == 0;
    }
    return true;
}();

template <class View, class... Slices>
inline constexpr bool supported_slice_mapping = [] {
    if constexpr (packed_layout<typename View::layout_type> && View::rank_dynamic() == 0 &&
                  View::rank() != 0) {
        using result = decltype(Kokkos::submdspan(std::declval<View>(), std::declval<Slices>()...));
        if constexpr (std::same_as<typename result::layout_type, layout_stride>) {
            constexpr std::array retained{
                !std::is_convertible_v<Slices, typename View::index_type>...};
            constexpr typename View::mapping_type mapping{};
            for (size_t axis = 0; axis != View::rank(); ++axis)
                if (retained[axis] && mapping.stride(axis) == 0)
                    return false;
        }
    }
    return true;
}();
} // namespace detail

// Preconditions: a valid borrowed source and valid backend slice specifiers.
// Steps are positive; bounds, offset/stride products and the addressed span
// must be representable. No validation, allocation or lifetime retention occurs.
// The fixed backend narrows promoted stride products for sub-int index types.
template <class T, class Extents, class Layout, class... Slices>
requires(sizeof...(Slices) == Extents::rank() &&
         std::numeric_limits<typename Extents::index_type>::digits >=
             std::numeric_limits<int>::digits &&
         (detail::packed_layout<Layout> || std::same_as<Layout, layout_stride> ||
          detail::padded_layout<Layout>) &&
         (detail::supported_static_slice<Slices> && ...) &&
         detail::supported_slice_mapping<tensor_view<T, Extents, Layout>, Slices...>)
[[nodiscard]] constexpr auto
slice_view(tensor_view<T, Extents, Layout> source,
           Slices... slices) noexcept(noexcept(Kokkos::submdspan(source, slices...))) {
    return Kokkos::submdspan(source, slices...);
}

} // namespace wjr::np

#endif // WJR_NP_SLICE_HPP__
