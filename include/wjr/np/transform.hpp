#ifndef WJR_NP_TRANSFORM_HPP__
#define WJR_NP_TRANSFORM_HPP__

#include <wjr/np/detail/elementwise.hpp>

namespace wjr::np {

// Preconditions: valid views, equal shapes, and each input either logically
// disjoint from output or identical at every index. No temporary is allocated.
// op is invoked as an lvalue on const input references, once per output element.
template <class T, class DstExtents, class DstLayout, class S, class SrcExtents, class SrcLayout,
          class Op>
requires(detail::kernel_element<T> && !std::is_const_v<T> && detail::kernel_element<S> &&
         detail::kernel_layout<DstLayout> && detail::kernel_layout<SrcLayout> &&
         detail::compatible_kernel_shape<DstExtents, SrcExtents> &&
         requires(T &out, Op &op, const S &in) { out = std::invoke(op, in); })
constexpr void transform_to(
    tensor_view<T, DstExtents, DstLayout> dst, tensor_view<S, SrcExtents, SrcLayout> src,
    Op &&op) noexcept(noexcept(std::declval<T &>() = std::invoke(op, std::declval<const S &>()))) {
    detail::assert_kernel_shape(dst, src);
    if (dst.empty())
        return;
    detail::apply_kernel(
        dst,
        [&](T &out, const S &in) noexcept(noexcept(out = std::invoke(op, in))) {
            out = std::invoke(op, in);
        },
        src);
}

template <class T, class DstExtents, class DstLayout, class L, class LhsExtents, class LhsLayout,
          class R, class RhsExtents, class RhsLayout, class Op>
requires(detail::kernel_element<T> && !std::is_const_v<T> && detail::kernel_element<L> &&
         detail::kernel_element<R> && detail::kernel_layout<DstLayout> &&
         detail::kernel_layout<LhsLayout> && detail::kernel_layout<RhsLayout> &&
         detail::compatible_kernel_shape<DstExtents, LhsExtents> &&
         detail::compatible_kernel_shape<DstExtents, RhsExtents> &&
         requires(T &out, Op &op, const L &lhs, const R &rhs) { out = std::invoke(op, lhs, rhs); })
constexpr void transform_to(
    tensor_view<T, DstExtents, DstLayout> dst, tensor_view<L, LhsExtents, LhsLayout> lhs,
    tensor_view<R, RhsExtents, RhsLayout> rhs,
    Op &&op) noexcept(noexcept(std::declval<T &>() = std::invoke(op, std::declval<const L &>(),
                                                                 std::declval<const R &>()))) {
    detail::assert_kernel_shape(dst, lhs);
    detail::assert_kernel_shape(dst, rhs);
    if (dst.empty())
        return;
    detail::apply_kernel(
        dst,
        [&](T &out, const L &left, const R &right) noexcept(
            noexcept(out = std::invoke(op, left, right))) { out = std::invoke(op, left, right); },
        lhs, rhs);
}

} // namespace wjr::np

#endif // WJR_NP_TRANSFORM_HPP__
