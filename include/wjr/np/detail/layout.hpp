#ifndef WJR_NP_DETAIL_LAYOUT_HPP__
#define WJR_NP_DETAIL_LAYOUT_HPP__

#include <concepts>

#include <wjr/np/view.hpp>

namespace wjr::np::detail {

template <class Layout>
concept packed_layout = std::same_as<Layout, layout_left> || std::same_as<Layout, layout_right>;

// submdspan can preserve a packed source's leading dimension in these tags.
// They are deliberately not a general layout customization point.
template <class Layout>
inline constexpr bool padded_layout = false;
template <size_t Padding>
inline constexpr bool padded_layout<Kokkos::Experimental::layout_left_padded<Padding>> = true;
template <size_t Padding>
inline constexpr bool padded_layout<Kokkos::Experimental::layout_right_padded<Padding>> = true;

template <class Layout>
inline constexpr bool left_layout = std::same_as<Layout, layout_left>;
template <size_t Padding>
inline constexpr bool left_layout<Kokkos::Experimental::layout_left_padded<Padding>> = true;

} // namespace wjr::np::detail

#endif // WJR_NP_DETAIL_LAYOUT_HPP__
