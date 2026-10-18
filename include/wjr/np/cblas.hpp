#ifndef WJR_NP_CBLAS_HPP__
#define WJR_NP_CBLAS_HPP__

#if !defined(WJR_NP_CBLAS_LP64) || WJR_NP_CBLAS_LP64 != 1
    #error "Include wjr/np/cblas.hpp through the wjr::np_cblas target."
#endif

#include <cblas.h>

#include <limits>

#include <wjr/np/copy.hpp>
#include <wjr/np/transform.hpp>

namespace wjr::np::cblas {

static_assert(std::is_same_v<blasint, int> && sizeof(blasint) == 4,
              "wjr::np_cblas requires an OpenBLAS LP64 header");

enum class transpose { none, transposed };

namespace detail {
template <class T>
concept element =
    std::same_as<std::remove_cv_t<T>, float> || std::same_as<std::remove_cv_t<T>, double>;

template <class A, class B>
inline constexpr bool compatible_order =
    std::same_as<A, layout_stride> || std::same_as<B, layout_stride> ||
    np::detail::left_layout<A> == np::detail::left_layout<B>;

constexpr int integer(size_t value) noexcept {
    WJR_ASSERT_L2(value <= static_cast<size_t>(std::numeric_limits<int>::max()));
    return static_cast<int>(value);
}

template <class View>
bool row_major(View view) noexcept {
    if constexpr (std::same_as<typename View::layout_type, layout_stride>)
        return view.stride(1) == 1;
    else
        return !np::detail::left_layout<typename View::layout_type>;
}

template <class View>
int leading_dimension(View view, bool row_major) noexcept {
    const size_t inner = row_major ? 1 : 0;
    const auto pitch = static_cast<size_t>(view.stride(1 - inner));
    WJR_ASSERT_L2(view.stride(inner) == 1);
    WJR_ASSERT_L2(pitch >= static_cast<size_t>(view.extent(inner)));
    return integer(pitch);
}

template <class View, class T>
void scale(View dst, T beta) noexcept {
    if (beta == T{0})
        fill_to(dst, T{0});
    else if (beta != T{1})
        transform_to(dst, dst, [beta](T value) noexcept { return beta * value; });
}
} // namespace detail

// Preconditions: valid borrowed views, equal lengths, and positive increments
// representable as LP64 BLAS integers. Empty input does not inspect strides.
template <class X, class XE, class XL, class Y, class YE, class YL>
requires(detail::element<X> && !std::is_volatile_v<X> && !std::is_volatile_v<Y> &&
         std::same_as<std::remove_cv_t<X>, std::remove_cv_t<Y>> && XE::rank() == 1 &&
         YE::rank() == 1 && np::detail::kernel_layout<XL> && np::detail::kernel_layout<YL> &&
         np::detail::compatible_kernel_shape<XE, YE>)
[[nodiscard]] std::remove_cv_t<X> dot(tensor_view<X, XE, XL> x, tensor_view<Y, YE, YL> y) noexcept {
    using value_type = std::remove_cv_t<X>;
    WJR_ASSERT_L2(x.extent(0) == y.extent(0));
    if (x.empty())
        return value_type{0};
    const auto count = detail::integer(static_cast<size_t>(x.extent(0)));
    const auto inc_x = detail::integer(static_cast<size_t>(x.stride(0)));
    const auto inc_y = detail::integer(static_cast<size_t>(y.stride(0)));
    WJR_ASSERT_L2(inc_x > 0 && inc_y > 0);
    if constexpr (std::same_as<value_type, float>)
        return cblas_sdot(count, x.data_handle(), inc_x, y.data_handle(), inc_y);
    else
        return cblas_ddot(count, x.data_handle(), inc_x, y.data_handle(), inc_y);
}

// C = alpha * op(A) * op(B) + beta * C, with one common physical major order.
// C must be logically disjoint from A/B; all provider arguments must fit int.
// K==0 or alpha==0 explicitly means beta*C, even for nonfinite alpha at K==0.
// beta==0 never reads C; nonempty C otherwise contains initialized values.
template <class T, class CE, class CL, class A, class AE, class AL, class B, class BE, class BL>
requires(detail::element<T> && !std::is_const_v<T> && !std::is_volatile_v<T> &&
         !std::is_volatile_v<A> && !std::is_volatile_v<B> && std::same_as<T, std::remove_cv_t<A>> &&
         std::same_as<T, std::remove_cv_t<B>> && CE::rank() == 2 && AE::rank() == 2 &&
         BE::rank() == 2 && np::detail::kernel_layout<CL> && np::detail::kernel_layout<AL> &&
         np::detail::kernel_layout<BL> && detail::compatible_order<CL, AL> &&
         detail::compatible_order<CL, BL>)
void gemm_to(tensor_view<T, CE, CL> c, tensor_view<A, AE, AL> a, tensor_view<B, BE, BL> b,
             std::type_identity_t<T> alpha = T{1}, std::type_identity_t<T> beta = T{0},
             transpose op_a = transpose::none, transpose op_b = transpose::none) noexcept {
    WJR_ASSERT_L2(op_a == transpose::none || op_a == transpose::transposed);
    WJR_ASSERT_L2(op_b == transpose::none || op_b == transpose::transposed);
    const bool trans_a = op_a == transpose::transposed;
    const bool trans_b = op_b == transpose::transposed;
    const auto m = static_cast<size_t>(c.extent(0));
    const auto n = static_cast<size_t>(c.extent(1));
    const auto k = static_cast<size_t>(a.extent(!trans_a));
    WJR_ASSERT_L2(static_cast<size_t>(a.extent(trans_a)) == m);
    WJR_ASSERT_L2(static_cast<size_t>(b.extent(!trans_b)) == n);
    WJR_ASSERT_L2(static_cast<size_t>(b.extent(trans_b)) == k);
    if (c.empty())
        return;
    if (k == 0 || alpha == T{0}) {
        detail::scale(c, beta);
        return;
    }
    const bool row_major = detail::row_major(c);
    const auto lda = detail::leading_dimension(a, row_major);
    const auto ldb = detail::leading_dimension(b, row_major);
    const auto ldc = detail::leading_dimension(c, row_major);
    const auto rows = detail::integer(m), cols = detail::integer(n), inner = detail::integer(k);
    const auto order = row_major ? CblasRowMajor : CblasColMajor;
    const auto ta = trans_a ? CblasTrans : CblasNoTrans;
    const auto tb = trans_b ? CblasTrans : CblasNoTrans;
    if constexpr (std::same_as<T, float>)
        cblas_sgemm(order, ta, tb, rows, cols, inner, alpha, a.data_handle(), lda, b.data_handle(),
                    ldb, beta, c.data_handle(), ldc);
    else
        cblas_dgemm(order, ta, tb, rows, cols, inner, alpha, a.data_handle(), lda, b.data_handle(),
                    ldb, beta, c.data_handle(), ldc);
}

// y = alpha * op(A) * x + beta * y. Output is disjoint from A and x.
// Positive vector increments and provider arguments must fit LP64 integers.
// Zero inner dimension or alpha explicitly means beta*y, as for gemm_to.
// beta==0 never reads y; otherwise written output values must be initialized.
template <class T, class YE, class YL, class A, class AE, class AL, class X, class XE, class XL>
requires(detail::element<T> && !std::is_const_v<T> && !std::is_volatile_v<T> &&
         !std::is_volatile_v<A> && !std::is_volatile_v<X> && std::same_as<T, std::remove_cv_t<A>> &&
         std::same_as<T, std::remove_cv_t<X>> && YE::rank() == 1 && AE::rank() == 2 &&
         XE::rank() == 1 && np::detail::kernel_layout<YL> && np::detail::kernel_layout<AL> &&
         np::detail::kernel_layout<XL>)
void gemv_to(tensor_view<T, YE, YL> y, tensor_view<A, AE, AL> a, tensor_view<X, XE, XL> x,
             std::type_identity_t<T> alpha = T{1}, std::type_identity_t<T> beta = T{0},
             transpose op = transpose::none) noexcept {
    WJR_ASSERT_L2(op == transpose::none || op == transpose::transposed);
    const bool transposed = op == transpose::transposed;
    const auto rows = static_cast<size_t>(a.extent(0));
    const auto columns = static_cast<size_t>(a.extent(1));
    const auto inner = transposed ? rows : columns;
    WJR_ASSERT_L2(static_cast<size_t>(y.extent(0)) == (transposed ? columns : rows));
    WJR_ASSERT_L2(static_cast<size_t>(x.extent(0)) == inner);
    if (y.empty())
        return;
    if (inner == 0 || alpha == T{0}) {
        detail::scale(y, beta);
        return;
    }
    const bool row_major = detail::row_major(a);
    const auto lda = detail::leading_dimension(a, row_major);
    const auto inc_x = detail::integer(static_cast<size_t>(x.stride(0)));
    const auto inc_y = detail::integer(static_cast<size_t>(y.stride(0)));
    WJR_ASSERT_L2(inc_x > 0 && inc_y > 0);
    const auto m = detail::integer(rows), n = detail::integer(columns);
    const auto order = row_major ? CblasRowMajor : CblasColMajor;
    const auto operation = transposed ? CblasTrans : CblasNoTrans;
    if constexpr (std::same_as<T, float>)
        cblas_sgemv(order, operation, m, n, alpha, a.data_handle(), lda, x.data_handle(), inc_x,
                    beta, y.data_handle(), inc_y);
    else
        cblas_dgemv(order, operation, m, n, alpha, a.data_handle(), lda, x.data_handle(), inc_x,
                    beta, y.data_handle(), inc_y);
}

} // namespace wjr::np::cblas

#endif // WJR_NP_CBLAS_HPP__
