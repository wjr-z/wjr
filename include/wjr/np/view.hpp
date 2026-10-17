#ifndef WJR_NP_VIEW_HPP__
#define WJR_NP_VIEW_HPP__

#include <wjr/config/compiler.hpp>

#if !defined(WJR_CPP_20)
    #error "wjr::np requires C++20."
#endif

// Link wjr::np so every translation unit sees the same backend configuration.
#if !defined(MDSPAN_USE_PAREN_OPERATOR) || MDSPAN_USE_PAREN_OPERATOR != 1 ||                       \
    !defined(MDSPAN_USE_BRACKET_OPERATOR) || MDSPAN_USE_BRACKET_OPERATOR != 0
    #error "Include wjr/np/view.hpp through the wjr::np CMake target."
#endif

#include <mdspan/mdspan.hpp>

namespace wjr::np {

using Kokkos::default_accessor;
using Kokkos::dextents;
using Kokkos::dynamic_extent;
using Kokkos::extents;
using Kokkos::layout_left;
using Kokkos::layout_right;
using Kokkos::layout_stride;

// A borrowed descriptor. Construction follows mdspan's unchecked preconditions;
// neither the descriptor nor its copies retain the storage's lifetime.
template <class T, class Extents, class Layout = layout_right, class Accessor = default_accessor<T>>
using tensor_view = Kokkos::mdspan<T, Extents, Layout, Accessor>;

} // namespace wjr::np

#endif // WJR_NP_VIEW_HPP__
