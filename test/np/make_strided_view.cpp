#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/make_view.hpp>

namespace {
namespace np = wjr::np;
using fixed_shape = np::extents<size_t, 2, 3>;
using dynamic_shape = np::dextents<size_t, 2>;

template <class Shape, class T, class Integer, class Stride, size_t Rank>
concept can_make =
    requires(T *data, std::array<Integer, Rank> shape, std::array<Stride, Rank> strides) {
        np::try_make_strided_tensor_view<Shape>(data, size_t{}, shape, strides);
    };
static_assert(can_make<fixed_shape, const double, int, uint64_t, 2>);
static_assert(!can_make<fixed_shape, int, bool, int, 2>);
static_assert(!can_make<fixed_shape, int, int, bool, 2>);
static_assert(!can_make<fixed_shape, int, double, int, 2>);
static_assert(!can_make<fixed_shape, int, int, double, 2>);
static_assert(!can_make<fixed_shape, int, int, int, 1>);
static_assert(!can_make<fixed_shape, void, int, int, 2>);

constexpr bool constant_pitched_views() {
    int values[11]{};
    auto row = np::try_make_strided_tensor_view<fixed_shape>(values, 11, std::array{2, 3},
                                                             std::array{8, 1});
    auto transpose = np::try_make_strided_tensor_view<np::extents<size_t, 3, 2>>(
        values, 11, std::array{3, 2}, std::array{1, 8});
    if (!row || !transpose)
        return false;
    (*row)(1, 2) = 7;
    return (*transpose)(2, 1) == 7 && row->mapping().required_span_size() == 11;
}
static_assert(constant_pitched_views());
static_assert(noexcept(np::try_make_strided_tensor_view<dynamic_shape>(static_cast<int *>(nullptr),
                                                                       0, std::array{2, 0},
                                                                       std::array{3, 1})));
} // namespace

TEST(np_make_strided_view, pitched_transposed_const_and_padding) {
    int values[16]{};
    auto row = np::try_make_strided_tensor_view<fixed_shape>(values, 16, std::array{2, 3},
                                                             std::array{8, 1});
    auto transpose = np::try_make_strided_tensor_view<np::extents<size_t, 3, 2>>(
        static_cast<const int *>(values), 16, std::array{3, 2}, std::array{1, 8});
    ASSERT_TRUE(row);
    ASSERT_TRUE(transpose);
    static_assert(std::same_as<typename decltype(row)::value_type,
                               np::tensor_view<int, fixed_shape, np::layout_stride>>);
    static_assert(std::same_as<decltype((*transpose)(0, 0)), const int &>);
    EXPECT_EQ(row->size(), 6u);
    EXPECT_EQ(row->mapping().required_span_size(), 11u);
    EXPECT_FALSE(row->is_exhaustive());
    (*row)(1, 2) = 9;
    EXPECT_EQ(values[10], 9);
    EXPECT_EQ((*transpose)(2, 1), 9);
    for (size_t i = 3; i != 8; ++i)
        EXPECT_EQ(values[i], 0);
    auto insufficient = np::try_make_strided_tensor_view<fixed_shape>(values, 10, std::array{2, 3},
                                                                      std::array{8, 1});
    ASSERT_FALSE(insufficient);
    EXPECT_EQ(insufficient.error(), np::shape_error::insufficient_storage);
}

TEST(np_make_strided_view, rejects_nonpositive_and_wide_strides) {
    int values[6]{};
    for (auto strides : {std::array{3, 0}, std::array{-3, 1}}) {
        auto result =
            np::try_make_strided_tensor_view<fixed_shape>(values, 6, std::array{2, 3}, strides);
        ASSERT_FALSE(result);
        EXPECT_EQ(result.error(), np::shape_error::nonpositive_stride);
    }
    auto wide = np::try_make_strided_tensor_view<np::dextents<uint8_t, 2>>(
        values, 6, std::array{2, 3}, std::array<uint64_t, 2>{256, 1});
    ASSERT_FALSE(wide);
    EXPECT_EQ(wide.error(), np::shape_error::stride_out_of_range);
    auto negative_shape = np::try_make_strided_tensor_view<dynamic_shape>(
        values, 6, std::array{-1, 3}, std::array{3, 1});
    ASSERT_FALSE(negative_shape);
    EXPECT_EQ(negative_shape.error(), np::shape_error::negative_extent);
    auto static_mismatch = np::try_make_strided_tensor_view<fixed_shape>(
        values, 6, std::array{3, 2}, std::array{3, 1});
    ASSERT_FALSE(static_mismatch);
    EXPECT_EQ(static_mismatch.error(), np::shape_error::static_extent_mismatch);
}

TEST(np_make_strided_view, permutation_witness_and_singleton_ties) {
    int values[6]{};
    for (auto strides : {std::array{1, 1}, std::array{2, 3}}) {
        // [2,3] has distinct offsets {0,2,3,5}, but does not satisfy the pinned
        // layout_stride constructor's stronger permutation precondition.
        auto result =
            np::try_make_strided_tensor_view<dynamic_shape>(values, 6, std::array{2, 2}, strides);
        ASSERT_FALSE(result);
        EXPECT_EQ(result.error(), np::shape_error::unsupported_stride_order);
    }
    auto singleton = np::try_make_strided_tensor_view<dynamic_shape>(values, 2, std::array{2, 1},
                                                                     std::array{1, 1});
    ASSERT_TRUE(singleton);
    EXPECT_TRUE(singleton->is_unique());
    EXPECT_EQ(singleton->mapping().required_span_size(), 2u);
}

TEST(np_make_strided_view, last_axis_product_need_not_fit_index_type) {
    char values[255]{};
    auto result = np::try_make_strided_tensor_view<np::dextents<uint8_t, 2>>(
        values, 255, std::array{2, 2}, std::array{1, 253});
    ASSERT_TRUE(result);
    EXPECT_EQ(result->mapping().required_span_size(), 255);
    (*result)(1, 1) = 7;
    EXPECT_EQ(values[254], 7);
    auto overflow = np::try_make_strided_tensor_view<np::dextents<uint8_t, 2>>(
        values, 255, std::array{2, 2}, std::array{1, 254});
    ASSERT_FALSE(overflow);
    EXPECT_EQ(overflow.error(), np::shape_error::index_overflow);
}

TEST(np_make_strided_view, empty_shape_still_checks_strides_and_prefix_span) {
    auto empty = np::try_make_strided_tensor_view<np::dextents<int, 2>>(
        static_cast<int *>(nullptr), 0, std::array{2, 0}, std::array{100, 1});
    ASSERT_TRUE(empty);
    EXPECT_TRUE(empty->empty());
    EXPECT_EQ(empty->mapping().required_span_size(), 0);
    auto zero_stride = np::try_make_strided_tensor_view<dynamic_shape>(
        static_cast<int *>(nullptr), 0, std::array{0, 2}, std::array{1, 0});
    ASSERT_FALSE(zero_stride);
    EXPECT_EQ(zero_stride.error(), np::shape_error::nonpositive_stride);
    auto overflow = np::try_make_strided_tensor_view<np::dextents<int, 2>>(
        static_cast<int *>(nullptr), 0, std::array{2, 0},
        std::array{std::numeric_limits<int>::max(), 1});
    ASSERT_FALSE(overflow);
    EXPECT_EQ(overflow.error(), np::shape_error::index_overflow);
    auto conservative = np::try_make_strided_tensor_view<np::dextents<int, 3>>(
        static_cast<int *>(nullptr), 0, std::array{2, 0, 2}, std::array{1, 100, 1});
    ASSERT_FALSE(conservative);
    EXPECT_EQ(conservative.error(), np::shape_error::unsupported_stride_order);
}

TEST(np_make_strided_view, scalar_null_byte_span_and_real_bool) {
    int value = 7;
    auto scalar = np::try_make_strided_tensor_view<np::extents<size_t>>(
        &value, 1, std::array<int, 0>{}, std::array<int, 0>{});
    ASSERT_TRUE(scalar);
    EXPECT_EQ((*scalar)(), 7);
    auto null = np::try_make_strided_tensor_view<fixed_shape>(static_cast<int *>(nullptr), 11,
                                                              std::array{2, 3}, std::array{8, 1});
    ASSERT_FALSE(null);
    EXPECT_EQ(null.error(), np::shape_error::null_storage);
    constexpr size_t excessive =
        static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max()) / sizeof(int);
    auto bytes = np::try_make_strided_tensor_view<np::extents<size_t, 2>>(&value, 1, std::array{2},
                                                                          std::array{excessive});
    ASSERT_FALSE(bytes);
    EXPECT_EQ(bytes.error(), np::shape_error::size_overflow);
    bool flags[3]{};
    auto booleans = np::try_make_strided_tensor_view<np::extents<size_t, 2>>(
        flags, 3, std::array{2}, std::array{2});
    ASSERT_TRUE(booleans);
    (*booleans)(1) = true;
    EXPECT_TRUE(flags[2]);
}
