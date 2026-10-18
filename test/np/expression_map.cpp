#include <array>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_cast.hpp>
#include <wjr/np/expression_reduce.hpp>

namespace {
namespace np = wjr::np;
using line = np::dextents<size_t, 1>;
template <class T>
using line_view = np::tensor_view<T, line>;
struct identity {
    constexpr int operator()(int value) const noexcept { return value; }
};
template <class F, class... Operands>
concept can_map = requires(F &&function, Operands &&...operands) {
    np::map(std::forward<F>(function), std::forward<Operands>(operands)...);
};
struct reference_result {
    const int &operator()(const int &value) const { return value; }
};
struct void_result {
    void operator()(int) const {}
};
struct proxy {
    operator int() const { return 1; }
};
struct proxy_result {
    proxy operator()(int) const { return {}; }
};
struct mutable_function {
    int operator()(int value) { return value; }
};
struct lvalue_argument {
    int operator()(int &) const;
};
struct value_overload {
    constexpr int operator()(int &&value) const noexcept { return value + 1; }
    constexpr int operator()(const int &) const noexcept { return -100; }
};
static_assert(can_map<identity, int> && can_map<identity, line_view<int>>);
static_assert(!can_map<identity> && !can_map<reference_result, int> && !can_map<void_result, int>);
static_assert(!can_map<proxy_result, int> && !can_map<mutable_function, int>);
static_assert(!can_map<lvalue_argument, line_view<int>>);
static_assert(!can_map<identity, np::tensor<int, line>>);
using two = np::tensor_view<int, np::extents<size_t, 2>>;
using three = np::tensor_view<int, np::extents<size_t, 3>>;
struct add {
    constexpr int operator()(int a, int b) const noexcept { return a + b; }
};
static_assert(!can_map<add, two, three>);

constexpr bool constant_map() {
    int values[]{1, 2, 3, 4};
    int output[4]{};
    const auto source = np::as_expr(np::tensor_view<int, np::extents<size_t, 4>>(values));
    const auto expression = -np::map(value_overload{}, source + 1) + np::map(identity{}, 2);
    np::eval_to(np::tensor_view<int, np::extents<size_t, 4>>(output), expression);
    if (output[0] != -1 || output[3] != -4 || np::sum(expression) != -10)
        return false;
    const auto ranks = np::map(add{}, np::tensor_view<int, np::extents<size_t>>(&values[0]), 3);
    static_assert(!decltype(ranks)::is_scalar && decltype(ranks)::rank() == 0);
    np::eval_to(np::tensor_view<int, np::extents<size_t>>(&values[0]), ranks);
    return values[0] == 4;
}
static_assert(constant_map());
static_assert(noexcept(np::map(identity{}, 1)));

TEST(NpMap, DtypePrvaluesAndPointwiseAlias) {
    int x[]{3, 5, 7}, y[]{2, 2, 2};
    double output[3]{};
    auto first = np::as_expr(line_view<int>(x, 3));
    auto second = np::as_expr(line_view<int>(y, 3));
    const auto expression = np::map([](int value) noexcept { return value + 0.5; }, first / second);
    static_assert(std::same_as<typename decltype(expression)::value_type, double>);
    np::eval_to(line_view<double>(output, 3), expression);
    EXPECT_EQ((std::array{output[0], output[1], output[2]}), (std::array{1.5, 2.5, 3.5}));
    np::eval_to(line_view<int>(x, 3), np::map(value_overload{}, first));
    EXPECT_EQ((std::array{x[0], x[1], x[2]}), (std::array{4, 6, 8}));
}

TEST(NpMap, CastKeepsConversionAtItsNode) {
    int input[]{3, 5};
    const auto mapped = np::map([](int x) noexcept { return x / 2; }, line_view<int>(input, 2));
    const auto widened = np::eval(np::cast<double>(mapped) + 0.25);
    ASSERT_TRUE(widened);
    EXPECT_EQ(widened->view()(0), 1.25);
    EXPECT_EQ(widened->view()(1), 2.25);
    const auto flags =
        np::eval(np::map([](int x) noexcept { return x > 3; }, line_view<int>(input, 2)));
    ASSERT_TRUE(flags);
    EXPECT_FALSE(flags->view()(0));
    EXPECT_TRUE(flags->view()(1));
}

TEST(NpMap, NaryBroadcastAndStaticDomain) {
    using a_shape = np::extents<size_t, 2, 1, 3>;
    using b_shape = np::extents<size_t, 4, 1>;
    int a[]{1, 2, 3, 4, 5, 6}, b[]{10, 20, 30, 40};
    const auto expression =
        np::map([](int x, int y, int z) noexcept { return x + y + z; },
                np::tensor_view<int, a_shape>(a), np::tensor_view<int, b_shape>(b), 2);
    static_assert(
        std::same_as<typename decltype(expression)::extents_type, np::extents<size_t, 2, 4, 3>>);
    const auto result = np::eval(expression);
    ASSERT_TRUE(result);
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 4; ++j)
            for (size_t k = 0; k != 3; ++k)
                EXPECT_EQ(result->view()(i, j, k), a[3 * i + k] + b[j] + 2);
}

TEST(NpMap, IndependentFunctionAndDataOffsets) {
    int input[]{1, 2, 3}, output[3]{};
    const auto source = np::as_expr(line_view<int>(input, 3));
    const auto left = np::map([](int x, int gain) noexcept { return x * gain; }, source, 2);
    const auto right = np::map([](int x) noexcept { return x + 10; }, source);
    const auto combined = np::map([](int x, int y) noexcept { return x - y; }, left + 1, right);
    np::eval_to(line_view<int>(output, 3), combined);
    EXPECT_EQ((std::array{output[0], output[1], output[2]}), (std::array{-8, -7, -6}));
    EXPECT_EQ(np::sum(combined), -21);
}

TEST(NpMap, CapturingLambdaAndLargeImmutableConfiguration) {
    struct table_function {
        std::array<int, 1024> table{};
        int operator()(int index) const noexcept { return table[static_cast<size_t>(index)]; }
    };
    table_function table;
    table.table[2] = 17;
    table.table[3] = 23;
    int input[]{2, 3}, output[2]{};
    const auto mapped = np::map(std::move(table), line_view<int>(input, 2));
    const auto shifted = np::map([bias = 4](int value) noexcept { return value + bias; }, mapped);
    np::eval_to(line_view<int>(output, 2), shifted);
    EXPECT_EQ(output[0], 21);
    EXPECT_EQ(output[1], 27);
}

TEST(NpMap, StridedMixedLayoutAndAxisReduction) {
    using shape = np::extents<size_t, 2, 3>;
    int row[]{1, 2, 3, 4, 5, 6}, column[]{10, 40, 20, 50, 30, 60};
    const auto expression = np::map(add{}, np::tensor_view<int, shape>(row),
                                    np::tensor_view<int, shape, np::layout_left>(column));
    const auto result = np::eval(expression);
    ASSERT_TRUE(result);
    static_assert(std::same_as<typename decltype(expression)::layout_type, np::layout_right>);
    EXPECT_EQ(result->view()(1, 2), 66);
    int sums[2]{};
    np::sum_to(np::tensor_view<int, np::extents<size_t, 2>>(sums), expression, np::axes<1>);
    EXPECT_EQ(sums[0], 66);
    EXPECT_EQ(sums[1], 165);
    using strided = np::tensor_view<int, np::extents<size_t, 3>, np::layout_stride>;
    const typename strided::mapping_type mapping(np::extents<size_t, 3>{},
                                                 std::array<size_t, 1>{2});
    const auto every_second = np::map(identity{}, strided(row, mapping));
    EXPECT_EQ(np::sum(every_second), 9);
}

TEST(NpMap, EmptyAndDynamicMismatchDoNotInvoke) {
    using empty = np::extents<size_t, 2, 0>;
    const auto fail = [](int) -> int { std::abort(); };
    const auto expression = np::map(fail, np::tensor_view<int, empty>(nullptr));
    EXPECT_NO_THROW(np::eval_to(np::tensor_view<int, empty>(nullptr), expression));
    EXPECT_EQ(np::sum(expression), 0);
    const auto result = np::eval(expression);
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->empty());
    int data[3]{};
    const auto mismatch = np::map(add{}, line_view<int>(data, 2), line_view<int>(data, 3));
    const auto rejected = np::eval(mismatch);
    ASSERT_FALSE(rejected);
    EXPECT_EQ(rejected.error(), np::shape_error::expression_shape_mismatch);
}

struct observed_empty {
    static inline int alive = 0, copies = 0, moves = 0;
    observed_empty() noexcept { ++alive; }
    observed_empty(const observed_empty &) noexcept {
        ++alive;
        ++copies;
    }
    observed_empty(observed_empty &&) noexcept {
        ++alive;
        ++moves;
    }
    ~observed_empty() { --alive; }
    int operator()(int value) const noexcept { return value + 1; }
};
static_assert(std::is_empty_v<observed_empty>);

TEST(NpMap, RealEmptyObjectAndNoEvaluationTimeCopies) {
    observed_empty::copies = observed_empty::moves = 0;
    int input[]{1, 2, 3, 4}, output[4]{};
    {
        const auto expression =
            np::map(observed_empty{}, np::tensor_view<int, np::extents<size_t, 2, 2>>(input));
        ASSERT_EQ(observed_empty::alive, 1);
        EXPECT_EQ(observed_empty::copies, 0);
        EXPECT_EQ(observed_empty::moves, 1);
        const auto copies = observed_empty::copies, moves = observed_empty::moves;
        for (int i = 0; i != 3; ++i)
            np::eval_to(np::tensor_view<int, np::extents<size_t, 2, 2>>(output), expression);
        EXPECT_EQ(np::sum(expression), 14);
        int sums[2]{};
        np::sum_to(np::tensor_view<int, np::extents<size_t, 2>>(sums), expression, np::axes<1>);
        np::sum_to(np::tensor_view<int, np::extents<size_t, 2, 1>>(sums), expression, np::axes<1>,
                   np::keepdims);
        const auto owner = np::eval(expression);
        ASSERT_TRUE(owner);
        EXPECT_EQ(owner->view()(1, 1), 5);
        EXPECT_EQ(observed_empty::copies, copies);
        EXPECT_EQ(observed_empty::moves, moves);
    }
    EXPECT_EQ(observed_empty::alive, 0);
}

struct move_only {
    std::unique_ptr<int> gain;
    explicit move_only(int value) : gain(std::make_unique<int>(value)) {}
    move_only(const move_only &) = delete;
    move_only(move_only &&) noexcept = default;
    int operator()(int value) const noexcept { return *gain * value; }
};
using moving_expression =
    decltype(np::map(move_only{2}, np::as_expr(std::declval<line_view<int>>())));
static_assert(!can_map<identity, moving_expression &> && can_map<identity, moving_expression>);
static_assert(!std::is_copy_constructible_v<moving_expression>);
static_assert(noexcept(-std::declval<moving_expression>()));
static_assert(noexcept(std::declval<moving_expression>() + 1));

TEST(NpMap, MoveOnlyClosureNestedUnderArithmetic) {
    int input[]{1, 2, 3}, output[3]{};
    auto inner = np::map(move_only{2}, line_view<int>(input, 3));
    const auto expression = -(np::map(identity{}, std::move(inner)) + 1) * 2;
    for (int i = 0; i != 2; ++i)
        np::eval_to(line_view<int>(output, 3), expression);
    EXPECT_EQ(output[2], -14);
    EXPECT_EQ(np::sum(expression), -30);
}

#ifndef WJR_DISABLE_EXCEPTIONS
struct construction_error {};
struct throwing_state {
    static inline int alive = 0;
    static inline bool fail_copy = false, fail_move = false;
    throwing_state() noexcept { ++alive; }
    throwing_state(const throwing_state &) {
        if (fail_copy)
            throw construction_error{};
        ++alive;
    }
    throwing_state(throwing_state &&) {
        if (fail_move)
            throw construction_error{};
        ++alive;
    }
    ~throwing_state() { --alive; }
    int operator()(int value) const noexcept { return value; }
};
using throwing_expression = decltype(np::map(throwing_state{}, np::as_expr(1)));
static_assert(!noexcept(np::map(throwing_state{}, 1)));
static_assert(!noexcept(+std::declval<const throwing_expression &>()));
static_assert(!noexcept(-std::declval<throwing_expression>()));
static_assert(!noexcept(std::declval<const throwing_expression &>() + 1));
static_assert(!noexcept(np::map(identity{}, std::declval<const throwing_expression &>())));

TEST(NpMap, ThrowingConstructionPropagatesThroughWrappers) {
    {
        const auto expression = np::map(throwing_state{}, 1);
        ASSERT_EQ(throwing_state::alive, 1);
        throwing_state::fail_copy = true;
        EXPECT_THROW((void)np::as_expr(expression), construction_error);
        EXPECT_THROW((void)+expression, construction_error);
        EXPECT_THROW((void)-expression, construction_error);
        EXPECT_THROW((void)(expression + 1), construction_error);
        EXPECT_THROW((void)(expression - 1), construction_error);
        EXPECT_THROW((void)(expression * 1), construction_error);
        EXPECT_THROW((void)(expression / 1), construction_error);
        EXPECT_THROW((void)np::map(identity{}, expression), construction_error);
        throwing_state::fail_copy = false;
        EXPECT_EQ(throwing_state::alive, 1);
    }
    {
        auto expression = np::map(throwing_state{}, 1);
        throwing_state::fail_move = true;
        EXPECT_THROW((void)-std::move(expression), construction_error);
        EXPECT_THROW((void)(std::move(expression) + 1), construction_error);
        EXPECT_THROW((void)np::map(identity{}, std::move(expression)), construction_error);
        throwing_state::fail_move = false;
        EXPECT_EQ(throwing_state::alive, 1);
    }
    EXPECT_EQ(throwing_state::alive, 0);
}

struct copy_safe_state {
    static inline int alive = 0;
    static inline bool fail_move = false;
    copy_safe_state() noexcept { ++alive; }
    copy_safe_state(const copy_safe_state &) noexcept { ++alive; }
    copy_safe_state(copy_safe_state &&) {
        if (fail_move)
            throw construction_error{};
        ++alive;
    }
    ~copy_safe_state() { --alive; }
    int operator()(int value) const noexcept { return value; }
};
using copy_safe_expression = decltype(np::map(copy_safe_state{}, 1));
static_assert(noexcept(np::as_expr(std::declval<const copy_safe_expression &>())));
static_assert(!noexcept(np::cast<double>(std::declval<const copy_safe_expression &>())));
static_assert(!noexcept(std::declval<const copy_safe_expression &>() + 1));

TEST(NpMap, InternalMoveAfterNothrowCopyIsNotNoexcept) {
    {
        const auto expression = np::map(copy_safe_state{}, 1);
        copy_safe_state::fail_move = true;
        EXPECT_THROW((void)np::cast<double>(expression), construction_error);
        EXPECT_THROW((void)(expression + 1), construction_error);
        EXPECT_THROW((void)np::map(identity{}, expression), construction_error);
        copy_safe_state::fail_move = false;
        EXPECT_EQ(copy_safe_state::alive, 1);
    }
    EXPECT_EQ(copy_safe_state::alive, 0);
}

struct invocation_error {};
struct reject_three {
    int operator()(int value) const {
        if (value == 3)
            throw invocation_error{};
        return value + 1;
    }
};
using invoking_expression =
    decltype(np::map(reject_three{}, np::as_expr(std::declval<line_view<int>>())));
static_assert(!noexcept(np::eval_to(std::declval<line_view<int>>(),
                                    std::declval<const invoking_expression &>())));
static_assert(!noexcept(np::sum(std::declval<const invoking_expression &>())));

struct allocation_counter {
    static inline int alive = 0;
};
template <class T>
struct counted_allocator {
    using value_type = T;
    counted_allocator() = default;
    template <class U>
    explicit counted_allocator(const counted_allocator<U> &) noexcept {}
    T *allocate(size_t count) {
        auto *result = std::allocator<T>{}.allocate(count);
        ++allocation_counter::alive;
        return result;
    }
    void deallocate(T *ptr, size_t count) noexcept {
        --allocation_counter::alive;
        std::allocator<T>{}.deallocate(ptr, count);
    }
    bool operator==(const counted_allocator &) const noexcept = default;
};

TEST(NpMap, InvocationExceptionAndOwningCleanup) {
    int input[]{1, 2, 3, 4}, output[4]{};
    const auto expression =
        np::map(reject_three{}, np::tensor_view<int, np::extents<size_t, 2, 2>>(input));
    EXPECT_THROW(np::eval_to(np::tensor_view<int, np::extents<size_t, 2, 2>>(output), expression),
                 invocation_error);
    EXPECT_EQ(output[0], 2);
    EXPECT_THROW((void)np::sum(expression), invocation_error);
    int sums[2]{};
    EXPECT_THROW(
        (np::sum_to(np::tensor_view<int, np::extents<size_t, 2>>(sums), expression, np::axes<1>)),
        invocation_error);
    EXPECT_THROW((void)np::eval(expression, std::allocator_arg, counted_allocator<int>{}),
                 invocation_error);
    EXPECT_EQ(allocation_counter::alive, 0);
}
#endif
} // namespace
