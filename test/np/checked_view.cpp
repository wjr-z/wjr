#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/checked_view.hpp>

namespace {

namespace np = wjr::np;
using static_shape = np::extents<size_t, 2, 3>;
using mixed_shape = np::extents<size_t, np::dynamic_extent, 3>;
using dynamic_shape = np::dextents<size_t, 2>;
using scalar_shape = np::extents<size_t>;
using static_view = np::tensor_view<int, static_shape>;
static_assert(
    std::same_as<np::shape_result<static_view>, wjr::expected<static_view, np::shape_error>>);
static_assert(std::same_as<typename np::shape_result<void>::error_type, np::shape_error>);
static_assert(std::same_as<decltype(np::try_make_tensor_view<static_shape>(
                               static_cast<int *>(nullptr), size_t{}, std::array{2, 3})),
                           np::shape_result<static_view>>);
static_assert(
    std::same_as<decltype(np::detail::check_metadata<int, static_shape>(std::array{2, 3})),
                 np::shape_result<np::detail::checked_metadata<static_shape>>>);

template <class Shape, class Layout, class T, class Integer, size_t Rank>
concept can_make_view = requires(T *data, std::array<Integer, Rank> shape) {
    np::try_make_tensor_view<Shape, Layout>(data, size_t{}, shape);
};

static_assert(can_make_view<static_shape, np::layout_left, const float, int, 2>);
static_assert(!can_make_view<static_shape, np::layout_stride, int, int, 2>);
static_assert(!can_make_view<static_shape, np::layout_right, int, bool, 2>);
static_assert(!can_make_view<static_shape, np::layout_right, int, double, 2>);
static_assert(!can_make_view<static_shape, np::layout_right, int, int, 1>);
static_assert(!can_make_view<int, np::layout_right, int, int, 2>);
static_assert(!can_make_view<static_shape, np::layout_right, void, int, 2>);

constexpr bool constexpr_views() {
    int values[] = {0, 1, 2, 3, 4, 5};
    auto row = np::try_make_tensor_view<static_shape>(values, 6, std::array{2, 3});
    auto column =
        np::try_make_tensor_view<mixed_shape, np::layout_left>(values, 6, std::array{2, 3});
    if (!row || !column) {
        return false;
    }
    (*row)(1, 2) = 7;
    return (*column)(1, 2) == 7 && row->stride(0) == 3 && column->stride(0) == 1;
}
static_assert(constexpr_views());
static_assert(noexcept(np::try_make_tensor_view<dynamic_shape>(static_cast<int *>(nullptr), 0,
                                                               std::array{0, 3})));

constexpr bool constexpr_errors() {
    auto negative =
        np::try_make_tensor_view<dynamic_shape>(static_cast<int *>(nullptr), 0, std::array{-1, 3});
    auto mismatch =
        np::try_make_tensor_view<static_shape>(static_cast<int *>(nullptr), 0, std::array{3, 2});
    return !negative && negative.error() == np::shape_error::negative_extent && !mismatch &&
           mismatch.error() == np::shape_error::static_extent_mismatch;
}
static_assert(constexpr_errors());

struct alignas(64) aligned_element {
    int value;
};

} // namespace

TEST(np_checked_view, static_dynamic_const_and_layout) {
    const float values[] = {0, 1, 2, 3, 4, 5};
    auto row = np::try_make_tensor_view<static_shape>(values, 6, std::array{2, 3});
    auto column =
        np::try_make_tensor_view<dynamic_shape, np::layout_left>(values, 6, std::array{2, 3});
    ASSERT_TRUE(row);
    ASSERT_TRUE(column);
    static_assert(std::is_same_v<typename decltype(row)::value_type::element_type, const float>);
    EXPECT_EQ(row->data_handle(), values);
    EXPECT_EQ(row->stride(0), 3u);
    EXPECT_EQ(column->stride(0), 1u);
    EXPECT_EQ((*row)(0, 1), 1);
    EXPECT_EQ((*column)(0, 1), 2);
}

TEST(np_checked_view, storage_bounds_and_null) {
    int values[8]{};
    auto larger = np::try_make_tensor_view<static_shape>(values, 8, std::array{2, 3});
    ASSERT_TRUE(larger);
    EXPECT_EQ(larger->size(), 6u);
    auto short_storage = np::try_make_tensor_view<static_shape>(values, 5, std::array{2, 3});
    ASSERT_FALSE(short_storage);
    EXPECT_EQ(short_storage.error(), np::shape_error::insufficient_storage);
    auto null_storage =
        np::try_make_tensor_view<static_shape>(static_cast<int *>(nullptr), 6, std::array{2, 3});
    ASSERT_FALSE(null_storage);
    EXPECT_EQ(null_storage.error(), np::shape_error::null_storage);
}

TEST(np_checked_view, empty_and_scalar) {
    auto empty =
        np::try_make_tensor_view<dynamic_shape>(static_cast<int *>(nullptr), 0, std::array{3, 0});
    ASSERT_TRUE(empty);
    EXPECT_EQ(empty->size(), 0u);
    EXPECT_EQ(empty->data_handle(), nullptr);
    auto negative_after_zero =
        np::try_make_tensor_view<dynamic_shape>(static_cast<int *>(nullptr), 0, std::array{0, -1});
    ASSERT_FALSE(negative_after_zero);
    EXPECT_EQ(negative_after_zero.error(), np::shape_error::negative_extent);

    int value = 3;
    auto scalar = np::try_make_tensor_view<scalar_shape>(&value, 1, std::array<int, 0>{});
    ASSERT_TRUE(scalar);
    EXPECT_EQ(scalar->size(), 1u);
    (*scalar)() = 7;
    EXPECT_EQ(value, 7);
    auto missing = np::try_make_tensor_view<scalar_shape>(static_cast<int *>(nullptr), 0,
                                                          std::array<int, 0>{});
    ASSERT_FALSE(missing);
    EXPECT_EQ(missing.error(), np::shape_error::insufficient_storage);
}

TEST(np_checked_view, negative_and_static_extent_checks_precede_products) {
    using shape = np::dextents<int, 3>;
    auto negative = np::try_make_tensor_view<shape>(static_cast<int *>(nullptr), 0,
                                                    std::array{46341, 46341, -1});
    ASSERT_FALSE(negative);
    EXPECT_EQ(negative.error(), np::shape_error::negative_extent);
    auto mismatch =
        np::try_make_tensor_view<static_shape>(static_cast<int *>(nullptr), 0, std::array{2, 4});
    ASSERT_FALSE(mismatch);
    EXPECT_EQ(mismatch.error(), np::shape_error::static_extent_mismatch);
}

TEST(np_checked_view, integer_width_and_index_overflow) {
    using narrow = np::dextents<uint8_t, 2>;
    auto extent = np::try_make_tensor_view<narrow>(static_cast<int *>(nullptr), 0,
                                                   std::array<uint64_t, 2>{256, 1});
    ASSERT_FALSE(extent);
    EXPECT_EQ(extent.error(), np::shape_error::extent_out_of_range);
    auto product = np::try_make_tensor_view<narrow>(static_cast<int *>(nullptr), 0,
                                                    std::array<uint16_t, 2>{16, 16});
    ASSERT_FALSE(product);
    EXPECT_EQ(product.error(), np::shape_error::index_overflow);
    int values[255]{};
    auto boundary = np::try_make_tensor_view<narrow>(values, 255, std::array<uint16_t, 2>{15, 17});
    ASSERT_TRUE(boundary);
    EXPECT_EQ(boundary->size(), 255);

    using signed_shape = np::dextents<int8_t, 1>;
    auto signed_extent = np::try_make_tensor_view<signed_shape>(static_cast<int *>(nullptr), 0,
                                                                std::array<uint64_t, 1>{128});
    ASSERT_FALSE(signed_extent);
    EXPECT_EQ(signed_extent.error(), np::shape_error::extent_out_of_range);
}

TEST(np_checked_view, empty_shape_still_checks_stride_products) {
    using shape = np::dextents<int, 3>;
    for (const auto dimensions : {std::array{0, 46341, 46341}, std::array{46341, 46341, 0}}) {
        auto row = np::try_make_tensor_view<shape>(static_cast<int *>(nullptr), 0, dimensions);
        auto column = np::try_make_tensor_view<shape, np::layout_left>(static_cast<int *>(nullptr),
                                                                       0, dimensions);
        ASSERT_FALSE(row);
        ASSERT_FALSE(column);
        EXPECT_EQ(row.error(), np::shape_error::index_overflow);
        EXPECT_EQ(column.error(), np::shape_error::index_overflow);
    }
    auto separated = np::try_make_tensor_view<shape>(static_cast<int *>(nullptr), 0,
                                                     std::array{46341, 0, 46341});
    ASSERT_TRUE(separated);
    EXPECT_EQ(separated->size(), 0u);
    EXPECT_EQ(separated->stride(1), 46341);
}

TEST(np_checked_view, byte_span_overflow_precedes_storage) {
    using shape = np::dextents<size_t, 1>;
    constexpr auto too_many = std::numeric_limits<size_t>::max() / sizeof(int) + 1;
    auto overflow =
        np::try_make_tensor_view<shape>(static_cast<int *>(nullptr), 0, std::array{too_many});
    ASSERT_FALSE(overflow);
    EXPECT_EQ(overflow.error(), np::shape_error::size_overflow);
    constexpr auto excessive_difference =
        static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max()) / sizeof(int) + 1;
    auto difference = np::try_make_tensor_view<shape>(static_cast<int *>(nullptr), 0,
                                                      std::array{excessive_difference});
    ASSERT_FALSE(difference);
    EXPECT_EQ(difference.error(), np::shape_error::size_overflow);
}

TEST(np_checked_view, real_bool_and_overalignment) {
    bool flags[] = {false, true};
    using shape = np::extents<int, 2>;
    auto booleans = np::try_make_tensor_view<shape>(flags, 2, std::array{2});
    ASSERT_TRUE(booleans);
    static_assert(std::is_same_v<decltype((*booleans)(0)), bool &>);
    (*booleans)(0) = true;
    EXPECT_TRUE(flags[0]);
    aligned_element values[2]{};
    auto aligned = np::try_make_tensor_view<shape>(values, 2, std::array{2});
    ASSERT_TRUE(aligned);
    EXPECT_EQ(aligned->data_handle(), values);
}

#if defined(__SIZEOF_INT128__) && defined(__GLIBCXX__) && !defined(__STRICT_ANSI__)
TEST(np_checked_view, input_wider_than_size_t_is_not_narrowed) {
    using wide = unsigned __int128;
    static_assert(sizeof(wide) > sizeof(size_t));
    auto result = np::try_make_tensor_view<np::dextents<size_t, 1>>(
        static_cast<int *>(nullptr), 0,
        std::array<wide, 1>{wide{1} << std::numeric_limits<size_t>::digits});
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), np::shape_error::extent_out_of_range);
    auto wider_index = np::try_make_tensor_view<np::dextents<wide, 1>>(
        static_cast<int *>(nullptr), 0,
        std::array<wide, 1>{wide{1} << std::numeric_limits<size_t>::digits});
    ASSERT_FALSE(wider_index);
    EXPECT_EQ(wider_index.error(), np::shape_error::size_overflow);
}
#endif
