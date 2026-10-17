#include <array>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/reshape.hpp>

namespace {
namespace np = wjr::np;
using fixed_shape = np::extents<size_t, 2, 3>;
using dynamic_shape = np::dextents<size_t, 2>;
using fixed_view = np::tensor_view<int, fixed_shape>;

template <class View>
concept flattens = requires(View view) { np::flatten_view(view); };
template <class View, class Shape, class Integer, size_t Rank>
concept reshapes = requires(View view, std::array<Integer, Rank> shape) {
    np::try_reshape_view<Shape>(view, shape);
};

struct custom_accessor : np::default_accessor<int> {};
using custom_view = np::tensor_view<int, fixed_shape, np::layout_right, custom_accessor>;
using strided_view = np::tensor_view<int, fixed_shape, np::layout_stride>;
static_assert(flattens<fixed_view>);
static_assert(!flattens<custom_view> && !flattens<strided_view>);
static_assert(!flattens<std::array<int, 6>>);
static_assert(!reshapes<strided_view, dynamic_shape, int, 2>);
static_assert(!reshapes<custom_view, dynamic_shape, int, 2>);
static_assert(!reshapes<fixed_view, dynamic_shape, bool, 2>);
static_assert(!reshapes<fixed_view, dynamic_shape, int, 1>);
static_assert(!reshapes<fixed_view, dynamic_shape, double, 2>);

constexpr bool constant_adapters() {
    int values[] = {0, 1, 2, 3, 4, 5};
    fixed_view source(values);
    auto flat = np::flatten_view(source);
    static_assert(decltype(flat)::rank() == 1);
    static_assert(decltype(flat)::static_extent(0) == 6);
    static_assert(std::same_as<typename decltype(flat)::layout_type, np::layout_right>);
    auto reshaped = np::try_reshape_view<np::extents<int, 3, 2>>(source, std::array{3, 2});
    if (!reshaped || (*reshaped)(2, 1) != 5)
        return false;
    flat(5) = 7;
    return (*reshaped)(2, 1) == 7 && reshaped->data_handle() == values;
}
static_assert(constant_adapters());

constexpr bool constant_scalar_and_empty() {
    int value = 3;
    auto flat_scalar = np::flatten_view(np::tensor_view<int, np::extents<size_t>>(&value));
    static_assert(decltype(flat_scalar)::static_extent(0) == 1);
    auto scalar = np::try_reshape_view<np::extents<size_t>>(flat_scalar, std::array<int, 0>{});
    auto flat_empty = np::flatten_view(np::tensor_view<int, np::extents<size_t, 3, 0>>(nullptr));
    static_assert(decltype(flat_empty)::static_extent(0) == 0);
    return scalar && (*scalar)() == 3 && flat_empty.empty() && flat_empty.data_handle() == nullptr;
}
static_assert(constant_scalar_and_empty());
static_assert(noexcept(np::flatten_view(std::declval<fixed_view>())));
static_assert(noexcept(np::try_reshape_view<dynamic_shape>(std::declval<fixed_view>(),
                                                           std::array{3, 2})));
} // namespace

TEST(np_reshape, static_dynamic_and_mixed_flatten_preserve_aliasing) {
    int values[] = {0, 1, 2, 3, 4, 5};
    const fixed_view fixed(values);
    auto flat = np::flatten_view(fixed);
    static_assert(std::same_as<decltype(flat(0)), int &>);
    EXPECT_EQ(flat.data_handle(), values);
    flat(3) = 7;
    EXPECT_EQ(fixed(1, 0), 7);
    np::tensor_view<int, dynamic_shape> dynamic(values, 2, 3);
    auto dynamic_flat = np::flatten_view(dynamic);
    static_assert(decltype(dynamic_flat)::static_extent(0) == np::dynamic_extent);
    EXPECT_EQ(dynamic_flat.extent(0), 6u);
    using mixed_shape = np::extents<size_t, np::dynamic_extent, 3>;
    auto mixed = np::flatten_view(np::tensor_view<const int, mixed_shape>(values, 2));
    static_assert(std::same_as<typename decltype(mixed)::element_type, const int>);
    EXPECT_EQ(mixed(3), 7);
}

TEST(np_reshape, row_and_column_major_keep_physical_order) {
    int values[] = {0, 1, 2, 3, 4, 5};
    fixed_view row(values);
    np::tensor_view<int, fixed_shape, np::layout_left> column(values);
    auto row_result = np::try_reshape_view<dynamic_shape>(row, std::array{3, 2});
    auto column_result = np::try_reshape_view<dynamic_shape>(column, std::array{3, 2});
    ASSERT_TRUE(row_result);
    ASSERT_TRUE(column_result);
    static_assert(
        std::same_as<typename decltype(column_result)::value_type::layout_type, np::layout_left>);
    EXPECT_EQ((*row_result)(1, 0), 2);
    EXPECT_EQ((*column_result)(1, 0), 1);
    auto flat = np::flatten_view(column);
    static_assert(std::same_as<typename decltype(flat)::layout_type, np::layout_left>);
    for (size_t i = 0; i != 6; ++i)
        EXPECT_EQ(flat(i), values[i]);
    (*column_result)(1, 1) = 9;
    EXPECT_EQ(column(0, 2), 9);
}

TEST(np_reshape, element_count_mismatch_in_both_directions) {
    int values[6]{};
    fixed_view source(values);
    for (auto dimensions : {std::array{2, 2}, std::array{2, 4}, std::array{0, 6}}) {
        auto mismatch = np::try_reshape_view<dynamic_shape>(source, dimensions);
        ASSERT_FALSE(mismatch);
        EXPECT_EQ(mismatch.error(), np::shape_error::element_count_mismatch);
    }
    auto negative = np::try_reshape_view<dynamic_shape>(source, std::array{-1, 6});
    ASSERT_FALSE(negative);
    EXPECT_EQ(negative.error(), np::shape_error::negative_extent);
    auto static_mismatch = np::try_reshape_view<fixed_shape>(source, std::array{3, 2});
    ASSERT_FALSE(static_mismatch);
    EXPECT_EQ(static_mismatch.error(), np::shape_error::static_extent_mismatch);
}

TEST(np_reshape, empty_and_rank_zero) {
    np::tensor_view<int, dynamic_shape> source(nullptr, 0, 3);
    auto reshaped = np::try_reshape_view<dynamic_shape>(source, std::array{4, 0});
    ASSERT_TRUE(reshaped);
    EXPECT_EQ(reshaped->extent(0), 4u);
    EXPECT_TRUE(reshaped->empty());
    EXPECT_EQ(reshaped->data_handle(), nullptr);
    auto scalar_mismatch = np::try_reshape_view<np::extents<size_t>>(source, std::array<int, 0>{});
    ASSERT_FALSE(scalar_mismatch);
    EXPECT_EQ(scalar_mismatch.error(), np::shape_error::element_count_mismatch);
    int scalar = 7;
    auto value = np::tensor_view<int, np::extents<size_t>>(&scalar);
    auto single = np::try_reshape_view<dynamic_shape>(value, std::array{1, 1});
    ASSERT_TRUE(single);
    EXPECT_EQ((*single)(0, 0), 7);
    auto empty_mismatch = np::try_reshape_view<dynamic_shape>(value, std::array{0, 1});
    ASSERT_FALSE(empty_mismatch);
    EXPECT_EQ(empty_mismatch.error(), np::shape_error::element_count_mismatch);
}

TEST(np_reshape, destination_integer_and_stride_overflow) {
    int values[6]{};
    fixed_view source(values);
    auto narrow =
        np::try_reshape_view<np::dextents<uint8_t, 1>>(source, std::array<uint64_t, 1>{256});
    ASSERT_FALSE(narrow);
    EXPECT_EQ(narrow.error(), np::shape_error::extent_out_of_range);
    using shape = np::dextents<int, 3>;
    auto bad_empty = np::try_reshape_view<shape>(source, std::array{0, 46341, 46341});
    ASSERT_FALSE(bad_empty);
    EXPECT_EQ(bad_empty.error(), np::shape_error::index_overflow);
    auto overflowing = np::try_reshape_view<shape>(source, std::array{46341, 46341, -1});
    ASSERT_FALSE(overflowing);
    EXPECT_EQ(overflowing.error(), np::shape_error::negative_extent);
}

TEST(np_reshape, temporary_descriptor_and_real_bool) {
    bool flags[] = {false, true};
    using shape = np::extents<size_t, 2>;
    auto reshaped = np::try_reshape_view<np::extents<size_t, 1, 2>>(
        np::tensor_view<bool, shape>(flags), std::array{1, 2});
    ASSERT_TRUE(reshaped);
    (*reshaped)(0, 0) = true;
    EXPECT_TRUE(flags[0]);
    auto flat = np::flatten_view(*reshaped);
    EXPECT_EQ(flat.data_handle(), flags);
    EXPECT_TRUE(flat(1));
}
