#ifndef WJR_NP_DETAIL_XSIMD_OPERATION_HPP__
#define WJR_NP_DETAIL_XSIMD_OPERATION_HPP__

#include <wjr/np/expression_mask.hpp>
#include <wjr/simd/xsimd.hpp>

namespace wjr::np::detail {

struct minimum_operation;
struct maximum_operation;
struct square_operation;

// Eligibility and implementation belong to the same operation adapter.
// Predicate lanes belong to the numeric batch, never batch<bool>.
enum class xsimd_result_kind { unsupported, numeric, predicate };

template <size_t Arity, xsimd_result_kind Input, xsimd_result_kind Result>
struct xsimd_operation_signature {
    template <xsimd_result_kind... Children>
    static constexpr auto result = sizeof...(Children) == Arity && ((Children == Input) && ...)
                                       ? Result
                                       : xsimd_result_kind::unsupported;
};

template <class Op>
struct xsimd_operation
    : xsimd_operation_signature<0, xsimd_result_kind::unsupported, xsimd_result_kind::unsupported> {
};

template <size_t Arity>
using xsimd_numeric_signature =
    xsimd_operation_signature<Arity, xsimd_result_kind::numeric, xsimd_result_kind::numeric>;
using xsimd_comparison_signature =
    xsimd_operation_signature<2, xsimd_result_kind::numeric, xsimd_result_kind::predicate>;
template <size_t Arity>
using xsimd_predicate_signature =
    xsimd_operation_signature<Arity, xsimd_result_kind::predicate, xsimd_result_kind::predicate>;

// Ordinary arithmetic already accepts numeric batches through the scalar tag.
template <class Op, size_t Arity>
struct xsimd_arithmetic_operation : xsimd_numeric_signature<Arity> {
    template <class... Batch>
    static WJR_INTRINSIC_INLINE auto apply(Batch... values) noexcept {
        return Op::apply(values...);
    }
};

template <>
struct xsimd_operation<positive_operation> : xsimd_arithmetic_operation<positive_operation, 1> {};
template <>
struct xsimd_operation<add_operation> : xsimd_arithmetic_operation<add_operation, 2> {};
template <>
struct xsimd_operation<subtract_operation> : xsimd_arithmetic_operation<subtract_operation, 2> {};
template <>
struct xsimd_operation<multiply_operation> : xsimd_arithmetic_operation<multiply_operation, 2> {};

template <>
struct xsimd_operation<square_operation> : xsimd_arithmetic_operation<square_operation, 1> {};
template <>
struct xsimd_operation<divide_operation> : xsimd_arithmetic_operation<divide_operation, 2> {};

template <>
struct xsimd_operation<negative_operation> : xsimd_numeric_signature<1> {
    template <class Batch>
    static WJR_INTRINSIC_INLINE auto apply(Batch value) noexcept {
        // xsimd 14.3's AVX-512 neg uses 0 - x, which loses the sign of +0.
        using lane_type = typename Batch::value_type;
        return value ^ Batch(-lane_type{0});
    }
};

template <>
struct xsimd_operation<less_operation> : xsimd_comparison_signature {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        return left < right;
    }
};
template <>
struct xsimd_operation<less_equal_operation> : xsimd_comparison_signature {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        return left <= right;
    }
};
template <>
struct xsimd_operation<greater_operation> : xsimd_comparison_signature {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        return left > right;
    }
};
template <>
struct xsimd_operation<greater_equal_operation> : xsimd_comparison_signature {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        return left >= right;
    }
};
template <>
struct xsimd_operation<equal_operation> : xsimd_comparison_signature {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        return left == right;
    }
};
template <>
struct xsimd_operation<not_equal_operation> : xsimd_comparison_signature {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        return left != right;
    }
};

template <>
struct xsimd_operation<logical_not_operation> : xsimd_predicate_signature<1> {
    template <class Batch>
    static WJR_INTRINSIC_INLINE auto apply(Batch value) noexcept {
        return !value;
    }
};
template <>
struct xsimd_operation<logical_and_operation> : xsimd_predicate_signature<2> {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        return left & right;
    }
};
template <>
struct xsimd_operation<logical_or_operation> : xsimd_predicate_signature<2> {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        return left | right;
    }
};

template <>
struct xsimd_operation<minimum_operation> : xsimd_numeric_signature<2> {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        // Raw SIMD min/max do not preserve our NaN and left-tie rules.
        return xsimd::select((left != left) | (right >= left), left, right);
    }
};
template <>
struct xsimd_operation<maximum_operation> : xsimd_numeric_signature<2> {
    template <class L, class R>
    static WJR_INTRINSIC_INLINE auto apply(L left, R right) noexcept {
        return xsimd::select((left != left) | (left >= right), left, right);
    }
};

template <>
struct xsimd_operation<select_operation> {
    template <xsimd_result_kind Mask, xsimd_result_kind Yes, xsimd_result_kind No>
    static constexpr auto result =
        Mask == xsimd_result_kind::predicate && Yes == No ? Yes : xsimd_result_kind::unsupported;

    template <class Mask, class Yes, class No>
    static WJR_INTRINSIC_INLINE auto apply(Mask mask, Yes yes, No no) noexcept {
        return xsimd::select(mask, yes, no);
    }
};

} // namespace wjr::np::detail

#endif // WJR_NP_DETAIL_XSIMD_OPERATION_HPP__
