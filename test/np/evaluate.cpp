#include <array>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/evaluate.hpp>

namespace {
namespace np = wjr::np;
using dynamic_shape = np::dextents<size_t, 2>;
template <class T, class Shape = dynamic_shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;

template <class E>
concept evaluates = requires(const E &expression) { np::eval(expression); };
template <class E, class Allocator>
concept allocates = requires(const E &expression, const Allocator &allocator) {
    np::eval(expression, std::allocator_arg, allocator);
};

static_assert(evaluates<np::scalar_expression<bool>>);
static_assert(!evaluates<np::tensor<double, dynamic_shape>>);
static_assert(allocates<np::scalar_expression<double>, std::allocator<double>>);
static_assert(!allocates<np::scalar_expression<double>, std::allocator<float>>);
static_assert(!allocates<np::scalar_expression<double>, int>);

struct allocation_state {
    size_t limit = 100;
    int allocations = 0;
    int deallocations = 0;
    int identity = 0;
    bool fail = false;
};

template <class T>
struct evaluation_allocator {
    using value_type = T;
    allocation_state *state;
    int identity;

    evaluation_allocator(allocation_state &s, int id) noexcept : state(&s), identity(id) {}
    template <class U>
    explicit evaluation_allocator(const evaluation_allocator<U> &other) noexcept
        : state(other.state), identity(other.identity) {}
    size_t max_size() const noexcept { return state->limit; }
    T *allocate(size_t count) {
        ++state->allocations;
        state->identity = identity;
        if (state->fail)
            throw std::bad_alloc();
        return std::allocator<T>{}.allocate(count);
    }
    void deallocate(T *data, size_t count) noexcept {
        ++state->deallocations;
        std::allocator<T>{}.deallocate(data, count);
    }
    template <class U>
    bool operator==(const evaluation_allocator<U> &other) const noexcept {
        return state == other.state && identity == other.identity;
    }
};

TEST(NpEvaluate, MaterializesFusedExpressionWithItsValueType) {
    std::array<int, 6> numerator{3, 7, 8, 9, 11, 13};
    std::array<int, 6> denominator{2, 2, 3, 2, 3, 2};
    std::array<float, 6> bias{.5f, .25f, 1.f, .5f, .25f, 1.f};
    const auto expression = np::as_expr(view<int>(numerator.data(), 2, 3)) /
                                np::as_expr(view<int>(denominator.data(), 2, 3)) +
                            np::as_expr(view<float>(bias.data(), 2, 3));
    auto result = np::eval(expression);
    using owner = typename decltype(result)::value_type;
    static_assert(std::same_as<owner, np::tensor<float, dynamic_shape>>);
    ASSERT_TRUE(result);
    EXPECT_EQ(result->extent(0), 2);
    EXPECT_EQ(result->extent(1), 3);
    EXPECT_EQ(result->view()(0, 0), 1.5f);
    EXPECT_EQ(result->view()(0, 1), 3.25f);
    EXPECT_EQ(result->view()(1, 2), 7.f);
}

TEST(NpEvaluate, PreservesStaticRefinementsAndColumnMajorOrder) {
    using first_shape = np::extents<int, np::dynamic_extent, 3>;
    using second_shape = np::extents<int, 2, np::dynamic_extent>;
    std::array<double, 6> x{1, 4, 2, 5, 3, 6}, y{6, 3, 5, 2, 4, 1};
    const auto a = np::as_expr(view<double, first_shape, np::layout_left>(x.data(), 2));
    const auto b = np::as_expr(view<double, second_shape, np::layout_left>(y.data(), 3));
    auto result = np::eval(2. * a - b);
    using owner = typename decltype(result)::value_type;
    static_assert(std::same_as<owner, np::tensor<double, np::extents<int, 2, 3>, np::layout_left>>);
    ASSERT_TRUE(result);
    EXPECT_EQ(result->view().stride(0), 1);
    EXPECT_EQ(result->view().stride(1), 2);
    EXPECT_EQ(result->view()(0, 0), -4.);
    EXPECT_EQ(result->view()(1, 2), 11.);
}

TEST(NpEvaluate, ProducesIndependentStorageEvenForAViewExpression) {
    auto result = [] {
        std::array<double, 3> input{1., 2., 3.};
        return np::eval(np::as_expr(view<double, np::extents<size_t, 3>>(input.data())));
    }();
    ASSERT_TRUE(result);
    EXPECT_EQ(result->view()(0), 1.);
    EXPECT_EQ(result->view()(2), 3.);

    std::array<double, 3> input{1., 2., 3.};
    const auto source = np::as_expr(view<double, np::extents<size_t, 3>>(input.data()));
    auto copy = np::eval(source);
    ASSERT_TRUE(copy);
    EXPECT_NE(copy->view().data_handle(), input.data());
    input[0] = 42.;
    EXPECT_EQ(copy->view()(0), 1.);
    auto second = np::eval(source);
    ASSERT_TRUE(second);
    EXPECT_EQ(second->view()(0), 42.);
}

TEST(NpEvaluate, MaterializesRankZeroScalarsAndScalarTrees) {
    auto scalar = np::eval(np::as_expr(3));
    ASSERT_TRUE(scalar);
    static_assert(decltype(scalar)::value_type::rank() == 0);
    EXPECT_EQ(scalar->size(), 1);
    EXPECT_EQ(scalar->view()(), 3);
    auto tree = np::eval(-np::as_expr(3) + 2.5);
    ASSERT_TRUE(tree);
    EXPECT_EQ(tree->view()(), -.5);
    double value = 7.;
    auto rank_zero_view = np::eval(np::as_expr(view<double, np::extents<size_t>>(&value)));
    ASSERT_TRUE(rank_zero_view);
    EXPECT_EQ(rank_zero_view->view()(), 7.);
    EXPECT_NE(rank_zero_view->view().data_handle(), &value);
    auto promoted_bool = np::eval(+np::as_expr(true));
    ASSERT_TRUE(promoted_bool);
    static_assert(std::same_as<typename decltype(promoted_bool)::value_type::element_type, int>);
    EXPECT_EQ(promoted_bool->view()(), 1);
}

TEST(NpEvaluate, MaterializesRealBooleanStorage) {
    auto scalar = np::eval(np::as_expr(true));
    ASSERT_TRUE(scalar);
    EXPECT_TRUE(scalar->view()());
    static_assert(std::same_as<decltype(scalar->view()()), bool &>);
    std::array<bool, 3> values{true, false, true};
    auto result = np::eval(np::as_expr(view<const bool, np::extents<size_t, 3>>(values.data())));
    ASSERT_TRUE(result);
    EXPECT_NE(result->view().data_handle(), values.data());
    EXPECT_TRUE(result->view()(0));
    EXPECT_FALSE(result->view()(1));
    EXPECT_TRUE(result->view()(2));
}

TEST(NpEvaluate, AllocatesOneResultAndRetainsAllocatorState) {
    allocation_state state;
    std::array<double, 6> x{1, 2, 3, 4, 5, 6};
    {
        const auto input = np::as_expr(view<double>(x.data(), 2, 3));
        auto result = np::eval(input * input + 1., std::allocator_arg,
                               evaluation_allocator<double>(state, 17));
        using owner = typename decltype(result)::value_type;
        static_assert(std::same_as<typename owner::container_type,
                                   wjr::vector<double, evaluation_allocator<double>>>);
        ASSERT_TRUE(result);
        EXPECT_EQ(state.allocations, 1);
        EXPECT_EQ(state.deallocations, 0);
        EXPECT_EQ(state.identity, 17);
        EXPECT_EQ(result->view()(1, 2), 37.);
    }
    EXPECT_EQ(state.deallocations, 1);
}

TEST(NpEvaluate, EmptyResultsAvoidAllocationAndInputReads) {
    allocation_state state;
    const auto input = np::as_expr(view<double>(nullptr, 3, 0));
    auto result = np::eval(input / 0., std::allocator_arg, evaluation_allocator<double>(state, 1));
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->empty());
    EXPECT_TRUE(result->has_storage());
    EXPECT_EQ(result->extent(0), 3);
    EXPECT_EQ(result->extent(1), 0);
    EXPECT_EQ(state.allocations, 0);
}

TEST(NpEvaluate, RejectsDifferentShapesBeforeAllocating) {
    allocation_state state;
    std::array<double, 6> input{};
    const auto a = np::as_expr(view<double>(input.data(), 2, 3));
    const auto b = np::as_expr(view<double>(input.data(), 3, 2));
    auto result = np::eval(-(a + b), std::allocator_arg, evaluation_allocator<double>(state, 1));
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), np::shape_error::expression_shape_mismatch);
    EXPECT_EQ(state.allocations, 0);
    EXPECT_EQ(state.deallocations, 0);
}

TEST(NpEvaluate, RejectsDynamicStaticConflictBeforeAllocating) {
    allocation_state state;
    std::array<double, 4> input{};
    const auto a = np::as_expr(view<double, np::dextents<size_t, 1>>(input.data(), 3));
    const auto b = np::as_expr(view<double, np::extents<size_t, 4>>(input.data()));
    auto result = np::eval(a + b, std::allocator_arg, evaluation_allocator<double>(state, 1));
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), np::shape_error::expression_shape_mismatch);
    EXPECT_EQ(state.allocations, 0);
}

TEST(NpEvaluate, PropagatesContainerLimitWithoutAllocation) {
    allocation_state state;
    state.limit = 5;
    std::array<double, 6> input{};
    auto result = np::eval(np::as_expr(view<double>(input.data(), 2, 3)), std::allocator_arg,
                           evaluation_allocator<double>(state, 1));
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), np::shape_error::container_size_limit);
    EXPECT_EQ(state.allocations, 0);
}

TEST(NpEvaluate, AllocationFailureUsesTheAllocatorExceptionPolicy) {
    allocation_state state;
    state.fail = true;
    EXPECT_THROW(
        (void)np::eval(np::as_expr(2.), std::allocator_arg, evaluation_allocator<double>(state, 1)),
        std::bad_alloc);
    EXPECT_EQ(state.allocations, 1);
    EXPECT_EQ(state.deallocations, 0);
}

struct throwing_operation {
    static int apply(int) { throw std::runtime_error("evaluation failed"); }
};

TEST(NpEvaluate, ReleasesResultWhenAnOperationThrows) {
    allocation_state state;
    const np::unary_expression<throwing_operation, np::scalar_expression<int>> expression(
        np::as_expr(2));
    EXPECT_THROW(
        (void)np::eval(expression, std::allocator_arg, evaluation_allocator<int>(state, 1)),
        std::runtime_error);
    EXPECT_EQ(state.allocations, 1);
    EXPECT_EQ(state.deallocations, 1);
}

TEST(NpEvaluate, BroadcastsRightAlignedRanksIntoTheNaturalShape) {
    using cube_shape = np::extents<size_t, 2, 1, 3>;
    using bias_shape = np::extents<size_t, 4, 1>;
    std::array<double, 6> input{1, 2, 3, 4, 5, 6};
    std::array<double, 4> bias{10, 20, 30, 40};
    const auto x = np::as_expr(view<double, cube_shape>(input.data()));
    const auto y = np::as_expr(view<double, bias_shape>(bias.data()));
    auto result = np::eval(x + y);
    using owner = typename decltype(result)::value_type;
    static_assert(std::same_as<owner, np::tensor<double, np::extents<size_t, 2, 4, 3>>>);
    ASSERT_TRUE(result);
    EXPECT_EQ(result->size(), 24);
    EXPECT_EQ(result->view()(0, 0, 0), 11.);
    EXPECT_EQ(result->view()(1, 2, 2), 36.);
    EXPECT_EQ(result->view()(1, 3, 1), 45.);
}

TEST(NpEvaluate, BroadcastsDynamicAndStaticRefinements) {
    using dynamic_vector = np::dextents<size_t, 1>;
    using fixed_matrix = np::extents<size_t, 2, 3>;
    std::array<double, 3> input{1, 2, 3};
    std::array<double, 6> weights{10, 20, 30, 40, 50, 60};
    const auto x = np::as_expr(view<double, dynamic_vector>(input.data(), 3));
    const auto y = np::as_expr(view<double, fixed_matrix>(weights.data()));
    auto result = np::eval(2. * x + y);
    using owner = typename decltype(result)::value_type;
    static_assert(std::same_as<owner, np::tensor<double, fixed_matrix>>);
    ASSERT_TRUE(result);
    EXPECT_EQ(result->view()(0, 2), 36.);
    EXPECT_EQ(result->view()(1, 0), 42.);
}

TEST(NpEvaluate, BroadcastsSingletonsWithZeroExtentsWithoutAllocating) {
    allocation_state state;
    using empty_shape = np::extents<size_t, 0, 1>;
    using row_shape = np::extents<size_t, 1, 4>;
    std::array<double, 4> row{1, 2, 3, 4};
    const auto x = np::as_expr(view<double, empty_shape>(nullptr));
    const auto y = np::as_expr(view<double, row_shape>(row.data()));
    auto result = np::eval(x + y, std::allocator_arg, evaluation_allocator<double>(state, 1));
    using owner = typename decltype(result)::value_type;
    static_assert(std::same_as<typename owner::extents_type, np::extents<size_t, 0, 4>>);
    ASSERT_TRUE(result);
    EXPECT_EQ(result->extent(0), 0);
    EXPECT_EQ(result->extent(1), 4);
    EXPECT_TRUE(result->empty());
    EXPECT_EQ(state.allocations, 0);
}

TEST(NpEvaluate, RejectsIncompatibleZeroAndNonSingletonExtents) {
    allocation_state state;
    std::array<double, 6> input{};
    const auto x = np::as_expr(view<double>(nullptr, 0, 3));
    const auto y = np::as_expr(view<double>(input.data(), 2, 3));
    auto result = np::eval(x + y, std::allocator_arg, evaluation_allocator<double>(state, 1));
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), np::shape_error::expression_shape_mismatch);
    EXPECT_EQ(state.allocations, 0);
}

TEST(NpEvaluate, BroadcastsRankZeroViewWithoutTreatingItAsLinearInput) {
    double scalar = 10.;
    std::array<double, 6> input{1, 2, 3, 4, 5, 6};
    const auto x = np::as_expr(view<double>(input.data(), 2, 3));
    const auto y = np::as_expr(view<double, np::extents<size_t>>(&scalar));
    auto result = np::eval(y + x);
    ASSERT_TRUE(result);
    EXPECT_EQ(result->extent(0), 2);
    EXPECT_EQ(result->extent(1), 3);
    EXPECT_EQ(result->view()(0, 0), 11.);
    EXPECT_EQ(result->view()(1, 2), 16.);
}

TEST(NpEvaluate, MaterializesStridedColumnsWithRowBroadcasting) {
    using column_shape = np::extents<size_t, 2, 1>;
    using row_shape = np::extents<size_t, 3>;
    std::array<double, 6> input{1, 2, 3, 4, 5, 6};
    std::array<double, 3> bias{10, 20, 30};
    const np::layout_stride::mapping<column_shape> mapping(column_shape{},
                                                           std::array<size_t, 2>{3, 1});
    const auto x =
        np::as_expr(view<double, column_shape, np::layout_stride>(input.data() + 1, mapping));
    const auto y = np::as_expr(view<double, row_shape>(bias.data()));
    auto result = np::eval(x + y);
    using owner = typename decltype(result)::value_type;
    static_assert(std::same_as<typename owner::layout_type, np::layout_right>);
    ASSERT_TRUE(result);
    EXPECT_EQ(result->view()(0, 0), 12.);
    EXPECT_EQ(result->view()(0, 2), 32.);
    EXPECT_EQ(result->view()(1, 0), 15.);
    EXPECT_EQ(result->view()(1, 2), 35.);
}

TEST(NpEvaluate, ResolvesMixedLayoutsByLogicalCoordinates) {
    using matrix_shape = np::extents<size_t, 2, 3>;
    std::array<double, 6> left_data{1, 4, 2, 5, 3, 6};
    std::array<double, 6> right_data{10, 20, 30, 40, 50, 60};
    const auto x = np::as_expr(view<double, matrix_shape, np::layout_left>(left_data.data()));
    const auto y = np::as_expr(view<double, matrix_shape>(right_data.data()));
    auto result = np::eval(x + y);
    using owner = typename decltype(result)::value_type;
    static_assert(std::same_as<typename owner::layout_type, np::layout_right>);
    ASSERT_TRUE(result);
    EXPECT_EQ(result->view()(0, 1), 22.);
    EXPECT_EQ(result->view()(1, 2), 66.);
}

TEST(NpEvaluate, ChecksContainerLimitAgainstTheFullBroadcastShape) {
    allocation_state state;
    state.limit = 5;
    std::array<double, 2> column{1, 2};
    std::array<double, 3> row{3, 4, 5};
    const auto x = np::as_expr(view<double, np::extents<size_t, 2, 1>>(column.data()));
    const auto y = np::as_expr(view<double, np::extents<size_t, 3>>(row.data()));
    auto result = np::eval(x + y, std::allocator_arg, evaluation_allocator<double>(state, 1));
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), np::shape_error::container_size_limit);
    EXPECT_EQ(state.allocations, 0);
}

TEST(NpEvaluate, ValidatesRemainingBroadcastAxesAfterAZeroExtent) {
    allocation_state state;
    std::array<double, 2> row{1, 2};
    const auto x = np::as_expr(view<double>(nullptr, 0, 3));
    const auto y = np::as_expr(view<double>(row.data(), 1, 2));
    auto result = np::eval(x + y, std::allocator_arg, evaluation_allocator<double>(state, 1));
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), np::shape_error::expression_shape_mismatch);
    EXPECT_EQ(state.allocations, 0);
}
} // namespace
