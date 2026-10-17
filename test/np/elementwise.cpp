#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/copy.hpp>
#include <wjr/np/transform.hpp>
#include <wjr/np/transpose.hpp>

namespace {
namespace np = wjr::np;
using shape = np::extents<size_t, 2, 3>;
using view = np::tensor_view<int, shape>;
using const_view = np::tensor_view<const int, shape>;

template <class Dst, class Src>
concept copies = requires(Dst dst, Src src) { np::copy_to(dst, src); };
template <class Dst>
concept fills = requires(Dst dst) { np::fill_to(dst, 1); };
template <class Dst, class Src, class Op>
concept transforms = requires(Dst dst, Src src, Op op) { np::transform_to(dst, src, op); };

struct custom_accessor : np::default_accessor<int> {};
struct readonly_op {
    constexpr int operator()(const int &value) const noexcept { return value * 2 + 1; }
};
struct mutable_op {
    int operator()(int &) const;
};
struct throwing_op {
    int operator()(const int &) const;
};
struct result_with_throwing_conversion {
    operator int() const noexcept(false);
};
struct converting_op {
    result_with_throwing_conversion operator()(const int &) const noexcept;
};

static_assert(copies<view, const_view>);
static_assert(!copies<const_view, view> && !fills<const_view>);
static_assert(!copies<view, np::tensor_view<double, shape>>);
static_assert(!copies<view, np::tensor_view<volatile int, shape>>);
static_assert(!copies<view, np::tensor_view<int, np::extents<size_t, 3, 2>>>);
static_assert(!copies<view, np::tensor_view<int, np::extents<size_t, 6>>>);
static_assert(!fills<np::tensor_view<int, shape, np::layout_right, custom_accessor>>);
static_assert(transforms<view, const_view, readonly_op>);
static_assert(!transforms<view, view, mutable_op>);
static_assert(!transforms<const_view, view, readonly_op>);
static_assert(noexcept(np::transform_to(std::declval<view>(), std::declval<view>(),
                                        readonly_op{})));
static_assert(!noexcept(np::transform_to(std::declval<view>(), std::declval<view>(),
                                         throwing_op{})));
static_assert(!noexcept(np::transform_to(std::declval<view>(), std::declval<view>(),
                                         converting_op{})));

template <class Layout>
constexpr bool constant_copy() {
    const int source[] = {1, 2, 3, 4, 5, 6};
    int result[6]{};
    np::tensor_view<const int, shape> src(source);
    np::tensor_view<int, shape, Layout> dst(result);
    np::copy_to(dst, src);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 3; ++col)
            if (dst(row, col) != src(row, col))
                return false;
    np::copy_to(dst, dst);
    np::fill_to(dst, dst(1, 1));
    for (auto value : result)
        if (value != 5)
            return false;
    return true;
}
static_assert(constant_copy<np::layout_right>());
static_assert(constant_copy<np::layout_left>());

constexpr bool constant_columns() {
    int matrix[] = {1, 10, -1, 2, 20, -1, 3, 30, -1};
    using column_shape = np::extents<size_t, 3>;
    using column = np::tensor_view<int, column_shape, np::layout_stride>;
    const np::layout_stride::mapping<column_shape> mapping(column_shape{}, std::array{3});
    column first(matrix, mapping);
    column second(matrix + 1, mapping);
    np::copy_to(second, first);
    np::transform_to(second, first, second,
                     [](int lhs, int rhs) noexcept { return lhs + 2 * rhs; });
    np::transform_to(first, second, readonly_op{});
    return matrix[0] == 7 && matrix[1] == 3 && matrix[2] == -1 && matrix[3] == 13 &&
           matrix[4] == 6 && matrix[6] == 19 && matrix[7] == 9;
}
static_assert(constant_columns());

constexpr bool constant_scalar() {
    int scalar = 3;
    np::tensor_view<int, np::extents<size_t>> value(&scalar);
    np::copy_to(value, value);
    np::transform_to(value, value, readonly_op{});
    if (scalar != 7)
        return false;
    np::fill_to(value, 9);
    using scalar_shape = np::extents<size_t>;
    np::tensor_view<int, scalar_shape, np::layout_stride> strided(
        &scalar, np::layout_stride::mapping<scalar_shape>(scalar_shape{}, std::array<size_t, 0>{}));
    np::transform_to(strided, value, readonly_op{});
    np::copy_to(value, strided);
    return scalar == 19;
}
static_assert(constant_scalar());

constexpr bool constant_materialized_transpose() {
    int source[] = {1, 2, 3, 4, 5, 6};
    int output[6]{};
    np::tensor_view<int, shape> src(source);
    np::tensor_view<int, np::extents<size_t, 3, 2>> dst(output);
    np::copy_to(dst, np::transpose_view(src));
    return output[0] == 1 && output[1] == 4 && output[2] == 2 && output[3] == 5 && output[4] == 3 &&
           output[5] == 6;
}
static_assert(constant_materialized_transpose());

TEST(np_elementwise, constant_evaluation_and_materialized_transpose) {
    EXPECT_TRUE(constant_copy<np::layout_right>());
    EXPECT_TRUE(constant_copy<np::layout_left>());
    EXPECT_TRUE(constant_columns());
    EXPECT_TRUE(constant_scalar());
    EXPECT_TRUE(constant_materialized_transpose());
}

TEST(np_elementwise, pitched_rank_three_preserves_padding) {
    using cube_shape = np::extents<int, 2, 2, 2>;
    using cube = np::tensor_view<double, cube_shape, np::layout_stride>;
    double input[20];
    double output[20];
    std::fill_n(input, 20, -7);
    std::fill_n(output, 20, -9);
    const np::layout_stride::mapping<cube_shape> mapping(cube_shape{}, std::array{12, 4, 1});
    cube src(input, mapping), dst(output, mapping);
    np::fill_to(src, 3);
    np::transform_to(dst, src, [](double value) noexcept { return value * 2 - 1; });
    for (size_t i = 0; i != 20; ++i) {
        const bool logical =
            i == 0 || i == 1 || i == 4 || i == 5 || i == 12 || i == 13 || i == 16 || i == 17;
        EXPECT_EQ(input[i], logical ? 3 : -7);
        EXPECT_EQ(output[i], logical ? 5 : -9);
    }
    np::copy_to(dst, src);
    EXPECT_EQ(output[17], 3);
    EXPECT_EQ(output[18], -9);
}

TEST(np_elementwise, mixed_layout_fused_binary_and_output_conversion) {
    const double lhs[] = {2, 4, 6, 8, 10, 12};
    const float rhs[] = {1, 2, 3, 4, 5, 6};
    int output[6]{};
    np::tensor_view<const double, shape> left(lhs);
    np::tensor_view<const float, shape> right(rhs);
    np::tensor_view<int, shape, np::layout_left> dst(output);
    np::transform_to(dst, left, right, [](double a, float b) noexcept { return (a - b) / 2; });
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 3; ++col)
            EXPECT_EQ(dst(row, col), static_cast<int>((left(row, col) - right(row, col)) / 2));
}

TEST(np_elementwise, inplace_alias_and_move_only_callable) {
    int data[] = {-3, -1, 0, 1, 3, 8};
    view dst(data);
    int calls = 0;
    auto op = [limit = std::make_unique<int>(2), &calls](const int &value) noexcept {
        ++calls;
        return std::clamp(value, -*limit, *limit);
    };
    np::transform_to(dst, dst, std::move(op));
    EXPECT_EQ(calls, 6);
    EXPECT_EQ(data[0], -2);
    EXPECT_EQ(data[5], 2);
    np::transform_to(dst, dst, dst, [](int a, int b) noexcept { return a + b; });
    EXPECT_EQ(data[0], -4);
    EXPECT_EQ(data[5], 4);
}

TEST(np_elementwise, empty_null_and_singleton_effective_mapping) {
    using dynamic_shape = np::dextents<size_t, 3>;
    np::tensor_view<int, dynamic_shape, np::layout_left> dst(nullptr, 5, 0, 3);
    np::tensor_view<const int, dynamic_shape> src(nullptr, 5, 0, 3);
    np::copy_to(dst, src);
    np::fill_to(dst, 1);
    int calls = 0;
    np::transform_to(dst, src, [&calls](int value) {
        ++calls;
        return value;
    });
    np::transform_to(dst, src, src, [&calls](int a, int b) {
        ++calls;
        return a + b;
    });
    EXPECT_EQ(calls, 0);
    using singleton_shape = np::extents<size_t, 1, 3>;
    int data[] = {1, 2, 3};
    np::tensor_view<int, singleton_shape, np::layout_stride> a(
        data, np::layout_stride::mapping<singleton_shape>(singleton_shape{}, std::array{3, 1}));
    np::tensor_view<const int, singleton_shape, np::layout_stride> b(
        data, np::layout_stride::mapping<singleton_shape>(singleton_shape{}, std::array{99, 1}));
    np::copy_to(a, b);
    EXPECT_EQ(data[2], 3);
    np::fill_to(a, a(0, 2));
    EXPECT_EQ(data[0], 3);
}

TEST(np_elementwise, narrow_index_and_real_bool_arrays) {
    using narrow_shape = np::extents<std::uint8_t, 2, 3>;
    bool source[] = {false, true, false, true, true, false};
    bool output[6]{};
    np::tensor_view<const bool, narrow_shape> src(source);
    np::tensor_view<bool, narrow_shape, np::layout_left> dst(output);
    np::copy_to(dst, src);
    np::transform_to(dst, dst, [](bool value) noexcept { return !value; });
    EXPECT_FALSE(dst(0, 1));
    EXPECT_TRUE(dst(1, 2));
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(np_elementwise, throwing_callable_leaves_completed_prefix) {
    int input[] = {1, 2, 3, 4, 5, 6};
    int output[6]{};
    int calls = 0;
    EXPECT_THROW(np::transform_to(view(output), const_view(input),
                                  [&calls](int value) {
                                      if (++calls == 3)
                                          throw std::runtime_error("operation failed");
                                      return value + 10;
                                  }),
                 std::runtime_error);
    EXPECT_EQ(calls, 3);
    EXPECT_EQ(output[0], 11);
    EXPECT_EQ(output[1], 12);
    EXPECT_EQ(output[2], 0);
}
#endif
} // namespace
