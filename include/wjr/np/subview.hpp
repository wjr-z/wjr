#ifndef WJR_NP_SUBVIEW_HPP__
#define WJR_NP_SUBVIEW_HPP__

#include <concepts>
#include <limits>
#include <optional>

#include <wjr/np/view.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::np {

// Precondition: source is a valid CPU view. Only the selected index is checked;
// returned descriptors borrow the same objects and retain their lifetime contract.
// The pinned backend list-initializes promoted stride products into index_type,
// so narrow index types are excluded until that backend limitation changes.
template <class T, class Extents, class Layout, std::integral Integer>
requires(Extents::rank() == 2 &&
         std::numeric_limits<typename Extents::index_type>::digits >=
             std::numeric_limits<int>::digits &&
         (std::same_as<Layout, layout_left> || std::same_as<Layout, layout_right>) &&
         !std::same_as<std::remove_cv_t<Integer>, bool>)
[[nodiscard]] constexpr auto try_row_view(tensor_view<T, Extents, Layout> source,
                                          Integer index) noexcept {
    using index_type = typename Extents::index_type;
    using result_type =
        std::optional<decltype(Kokkos::submdspan(source, index_type{}, Kokkos::full_extent))>;
    if (wjr::cmp_less(index, 0) || !wjr::cmp_less(index, source.extent(0))) {
        return result_type{};
    }
    // The bounds check also proves representability in the source index_type.
    return result_type(
        Kokkos::submdspan(source, static_cast<index_type>(index), Kokkos::full_extent));
}

template <class T, class Extents, class Layout, std::integral Integer>
requires(Extents::rank() == 2 &&
         std::numeric_limits<typename Extents::index_type>::digits >=
             std::numeric_limits<int>::digits &&
         (std::same_as<Layout, layout_left> || std::same_as<Layout, layout_right>) &&
         !std::same_as<std::remove_cv_t<Integer>, bool>)
[[nodiscard]] constexpr auto try_column_view(tensor_view<T, Extents, Layout> source,
                                             Integer index) noexcept {
    using index_type = typename Extents::index_type;
    using result_type =
        std::optional<decltype(Kokkos::submdspan(source, Kokkos::full_extent, index_type{}))>;
    if (wjr::cmp_less(index, 0) || !wjr::cmp_less(index, source.extent(1))) {
        return result_type{};
    }
    return result_type(
        Kokkos::submdspan(source, Kokkos::full_extent, static_cast<index_type>(index)));
}

} // namespace wjr::np

#endif // WJR_NP_SUBVIEW_HPP__
