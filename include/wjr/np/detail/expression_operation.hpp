#ifndef WJR_NP_DETAIL_EXPRESSION_OPERATION_HPP__
#define WJR_NP_DETAIL_EXPRESSION_OPERATION_HPP__

#include <wjr/np/expression.hpp>

namespace wjr::np::detail {

struct minimum_operation;
struct maximum_operation;
struct square_operation;

// This scalar ordering contract is independent of SIMD eligibility. Neither
// noexcept nor a compilable batch overload proves an unknown operation pure.
template <class Op>
struct expression_operation_traits {
    static constexpr bool interleavable_sum = false;
};

struct interleavable_expression_operation {
    static constexpr bool interleavable_sum = true;
};

template <>
struct expression_operation_traits<add_operation> : interleavable_expression_operation {};
template <>
struct expression_operation_traits<subtract_operation> : interleavable_expression_operation {};
template <>
struct expression_operation_traits<multiply_operation> : interleavable_expression_operation {};

template <>
struct expression_operation_traits<square_operation> : interleavable_expression_operation {};
template <>
struct expression_operation_traits<divide_operation> : interleavable_expression_operation {};
template <>
struct expression_operation_traits<positive_operation> : interleavable_expression_operation {};
template <>
struct expression_operation_traits<negative_operation> : interleavable_expression_operation {};
template <>
struct expression_operation_traits<minimum_operation> : interleavable_expression_operation {};
template <>
struct expression_operation_traits<maximum_operation> : interleavable_expression_operation {};

} // namespace wjr::np::detail

#endif // WJR_NP_DETAIL_EXPRESSION_OPERATION_HPP__
