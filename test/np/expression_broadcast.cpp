#include <array>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/expression.hpp>

namespace {
namespace np = wjr::np;

template <class Shape, class Layout = np::layout_right>
using input = np::tensor_view<const int, Shape, Layout>;
template <class Shape, class Layout = np::layout_right>
using output = np::tensor_view<int, Shape, Layout>;
template <class L, class R>
concept can_add = requires(L left, R right) { left + right; };
template <class Dst, class E>
concept can_evaluate = requires(Dst dst, E expression) { np::eval_to(dst, expression); };
template <class Shape>
using leaf = decltype(np::as_expr(std::declval<input<Shape>>()));

using cube = np::extents<size_t, 2, 1, 3>;
using column = np::extents<size_t, 4, 1>;
using broadcast = decltype(std::declval<leaf<cube>>() + std::declval<leaf<column>>());
static_assert(std::same_as<typename broadcast::extents_type, np::extents<size_t, 2, 4, 3>>);
using dynamic = np::extents<int, np::dynamic_extent, 1>;
using refined =
    decltype(std::declval<leaf<dynamic>>() + std::declval<leaf<np::extents<size_t, 3, 4>>>());
static_assert(std::same_as<typename refined::extents_type,
                           np::extents<std::common_type_t<int, size_t>, 3, 4>>);
static_assert(can_add<leaf<np::extents<size_t, 0>>, leaf<np::extents<size_t, 1>>>);
static_assert(!can_add<leaf<np::extents<size_t, 0>>, leaf<np::extents<size_t, 2>>>);
static_assert(can_evaluate<output<np::extents<size_t, 2, 3>>, leaf<np::extents<size_t, 3>>>);
static_assert(!can_evaluate<output<np::extents<size_t, 3>>, leaf<np::extents<size_t, 1, 3>>>);
static_assert(!can_evaluate<output<np::extents<size_t, 1>>, leaf<np::extents<size_t, 0>>>);

constexpr bool constant_broadcast() {
    const int source[] = {1, 2, 3, 4, 5, 6};
    const int bias[] = {10, 20, 30, 40};
    int result[24]{};
    const auto expression = np::as_expr(input<cube>(source)) + np::as_expr(input<column>(bias));
    output<np::extents<size_t, 2, 4, 3>> dst(result);
    np::eval_to(dst, expression);
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 4; ++j)
            for (size_t k = 0; k != 3; ++k)
                if (dst(i, j, k) != source[i * 3 + k] + bias[j])
                    return false;
    return true;
}
static_assert(constant_broadcast());

struct wrapped_value {
    int value;
};
int operator+(wrapped_value left, int right) {
#ifndef WJR_DISABLE_EXCEPTIONS
    if (left.value == 2)
        throw std::runtime_error("operation");
#endif
    return left.value + right;
}
struct wrapping_operation {
    static constexpr wrapped_value apply(int value) noexcept { return {value}; }
};
using wrapped = np::unary_expression<wrapping_operation, leaf<np::extents<size_t, 3>>>;
using throwing = decltype(std::declval<wrapped>() + 1);
static_assert(!noexcept(std::declval<const throwing &>().read(0)));
static_assert(!noexcept(np::eval_to(std::declval<output<np::extents<size_t, 3>>>(),
                                    std::declval<const throwing &>())));
} // namespace

TEST(np_expression_broadcast, right_aligned_static_shape_and_constexpr) {
    EXPECT_TRUE(constant_broadcast());
}

TEST(np_expression_broadcast, row_bias_column_scale_and_rank_zero) {
    using matrix = np::extents<size_t, 2, 3>;
    int values[] = {1, 2, 3, 4, 5, 6};
    const int bias[] = {10, 20, 30};
    const int scale[] = {2, 3};
    const int offset = 4;
    const auto expression =
        np::as_expr(input<matrix>(values)) * np::as_expr(input<np::extents<size_t, 2, 1>>(scale)) +
        np::as_expr(input<np::extents<size_t, 3>>(bias)) +
        np::as_expr(input<np::extents<size_t>>(&offset));
    // Only the matrix leaf aliases output, at exactly the same logical points.
    np::eval_to(output<matrix>(values), expression);
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 3; ++j)
            EXPECT_EQ(values[i * 3 + j], int(i * 3 + j + 1) * scale[i] + bias[j] + offset);
}

TEST(np_expression_broadcast, mixed_layouts_follow_coordinates) {
    using matrix = np::extents<size_t, 2, 3>;
    const int right[] = {1, 2, 3, 4, 5, 6};
    const int left[] = {10, 40, 20, 50, 30, 60};
    int result[6]{};
    const auto expression =
        np::as_expr(input<matrix>(right)) + np::as_expr(input<matrix, np::layout_left>(left));
    static_assert(std::same_as<typename decltype(expression)::layout_type, np::layout_right>);
    output<matrix, np::layout_left> dst(result);
    np::eval_to(dst, expression);
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 3; ++j)
            EXPECT_EQ(dst(i, j), 11 * right[i * 3 + j]);
}

TEST(np_expression_broadcast, padded_input_and_output_leave_gaps_untouched) {
    using matrix = np::extents<size_t, 2, 3>;
    const np::layout_stride::mapping<matrix> src_mapping(matrix{}, std::array<size_t, 2>{5, 1});
    const np::layout_stride::mapping<matrix> dst_mapping(matrix{}, std::array<size_t, 2>{7, 2});
    // Exact required-span allocations make an extra row or tail load visible to ASan.
    const int source[] = {1, 2, 3, -99, -99, 4, 5, 6};
    const int bias[] = {10, 20, 30};
    std::array<int, 12> result;
    result.fill(-99);
    const auto expression = np::as_expr(input<matrix, np::layout_stride>(source, src_mapping)) +
                            np::as_expr(input<np::extents<size_t, 3>>(bias));
    output<matrix, np::layout_stride> dst(result.data(), dst_mapping);
    np::eval_to(dst, expression);
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 3; ++j)
            EXPECT_EQ(dst(i, j), int(i * 3 + j + 1) + bias[j]);
    for (size_t i : {1, 3, 5, 6, 8, 10})
        EXPECT_EQ(result[i], -99);
}

TEST(np_expression_broadcast, dynamic_domains_and_allocation_ends) {
    using matrix = np::dextents<size_t, 2>;
    using line = np::dextents<size_t, 1>;
    for (size_t rows = 0; rows != 5; ++rows) {
        for (size_t columns = 0; columns != 18; ++columns) {
            const size_t count = rows * columns;
            auto source = std::make_unique<int[]>(count);
            auto bias = std::make_unique<int[]>(columns);
            auto result = std::make_unique<int[]>(count);
            for (size_t i = 0; i != count; ++i)
                source[i] = int(i);
            for (size_t j = 0; j != columns; ++j)
                bias[j] = int(2 * j);
            const auto expression =
                np::as_expr(input<matrix>(count ? source.get() : nullptr, rows, columns)) +
                np::as_expr(input<line>(columns ? bias.get() : nullptr, columns));
            np::eval_to(output<matrix>(count ? result.get() : nullptr, rows, columns), expression);
            for (size_t i = 0; i != rows; ++i)
                for (size_t j = 0; j != columns; ++j)
                    EXPECT_EQ(result[i * columns + j], source[i * columns + j] + bias[j]);
        }
    }
}

TEST(np_expression_broadcast, empty_shape_skips_operations) {
    using empty = np::extents<size_t, 2, 0, 3>;
    const int denominator[] = {0, 0, 0};
    const auto expression = np::as_expr(input<empty>(nullptr)) /
                            np::as_expr(input<np::extents<size_t, 3>>(denominator));
    np::eval_to(output<empty>(nullptr), expression);
    std::array<size_t, 3> shape{};
    EXPECT_TRUE(np::detail::try_expression_shape(expression, shape));
    EXPECT_EQ(shape, (std::array<size_t, 3>{2, 0, 3}));
}

#ifndef WJR_DISABLE_EXCEPTIONS
TEST(np_expression_broadcast, operation_exceptions_propagate) {
    using line = np::extents<size_t, 3>;
    const int source[] = {1, 2, 3};
    int result[] = {-1, -1, -1};
    const wrapped expression(np::as_expr(input<line>(source)));
    EXPECT_THROW(np::eval_to(output<line>(result), expression + 1), std::runtime_error);
    // Completed points stay assigned; the operation has no transactional promise.
    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[1], -1);
    int broadcast_result[] = {-1, -1, -1, -1, -1, -1};
    EXPECT_THROW((np::eval_to(output<np::extents<size_t, 2, 3>>(broadcast_result), expression + 1)),
                 std::runtime_error);
    EXPECT_EQ(broadcast_result[0], 2);
    EXPECT_EQ(broadcast_result[1], -1);
}
#endif
