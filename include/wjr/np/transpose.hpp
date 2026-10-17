#ifndef WJR_NP_TRANSPOSE_HPP__
#define WJR_NP_TRANSPOSE_HPP__

#include <concepts>
#include <type_traits>

#include <wjr/np/view.hpp>

namespace wjr::np {

// Precondition: source is a valid view. Swap the two axes by changing only
// metadata; elements and storage lifetime are unchanged. No validation occurs.
template <class T, class Extents, class Layout>
requires(Extents::rank() == 2 &&
         (std::same_as<Layout, layout_left> || std::same_as<Layout, layout_right>))
[[nodiscard]] constexpr auto transpose_view(tensor_view<T, Extents, Layout> source) noexcept {
    using result_extents =
        extents<typename Extents::index_type, Extents::static_extent(1), Extents::static_extent(0)>;
    using result_layout =
        std::conditional_t<std::same_as<Layout, layout_left>, layout_right, layout_left>;
    return tensor_view<T, result_extents, result_layout>(
        source.data_handle(), result_extents(source.extent(1), source.extent(0)));
}

} // namespace wjr::np

#endif // WJR_NP_TRANSPOSE_HPP__
