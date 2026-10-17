#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/np/subview.hpp>
#include <wjr/np/tensor.hpp>

namespace {
namespace np = wjr::np;
using shape = np::extents<size_t, 2, 3>;
using right_view = np::tensor_view<int, shape>;
using left_view = np::tensor_view<int, shape, np::layout_left>;

template <class View, class Index>
concept can_select = requires(View view, Index index) {
    np::try_row_view(view, index);
    np::try_column_view(view, index);
};
template <class Owner>
concept selects_owner_view = requires(Owner &&owner) {
    np::try_row_view(std::forward<Owner>(owner).view(), 0);
    np::try_column_view(std::forward<Owner>(owner).view(), 0);
};
using owner = np::tensor<int, shape>;
struct custom_accessor : np::default_accessor<int> {};
static_assert(can_select<right_view, int> && can_select<left_view, uint64_t>);
static_assert(!can_select<right_view, bool> && !can_select<right_view, double>);
static_assert(!can_select<np::tensor_view<int, np::extents<size_t>>, int>);
static_assert(!can_select<np::tensor_view<int, np::extents<size_t, 6>>, int>);
static_assert(!can_select<np::tensor_view<int, np::extents<size_t, 1, 2, 3>>, int>);
static_assert(!can_select<np::tensor_view<int, shape, np::layout_stride>, int>);
static_assert(!can_select<np::tensor_view<int, np::extents<uint8_t, 2, 3>>, int>);
static_assert(!can_select<np::tensor_view<int, np::extents<uint16_t, 2, 3>>, int>);
static_assert(!can_select<np::tensor_view<int, shape, np::layout_right, custom_accessor>, int>);
static_assert(!can_select<owner, int>);
static_assert(selects_owner_view<owner &> && selects_owner_view<const owner &>);
static_assert(!selects_owner_view<owner> && !selects_owner_view<const owner>);
static_assert(noexcept(np::try_row_view(std::declval<right_view>(), 0)));
static_assert(noexcept(np::try_column_view(std::declval<left_view>(), 0)));

constexpr bool constant_subviews() {
    int values[] = {0, 1, 2, 3, 4, 5};
    auto rr = np::try_row_view(right_view(values), 1);
    auto rc = np::try_column_view(right_view(values), 1);
    auto lr = np::try_row_view(left_view(values), 1);
    auto lc = np::try_column_view(left_view(values), 1);
    static_assert(decltype(rr)::value_type::static_extent(0) == 3);
    static_assert(decltype(rc)::value_type::static_extent(0) == 2);
    static_assert(std::same_as<typename decltype(rr)::value_type::layout_type, np::layout_right>);
    static_assert(std::same_as<typename decltype(rc)::value_type::layout_type, np::layout_stride>);
    static_assert(std::same_as<typename decltype(lr)::value_type::layout_type, np::layout_stride>);
    static_assert(std::same_as<typename decltype(lc)::value_type::layout_type, np::layout_left>);
    if (!rr || !rc || !lr || !lc)
        return false;
    if ((*rr)(2) != 5 || (*rc)(1) != 4 || (*lr)(2) != 5 || (*lc)(1) != 3)
        return false;
    (*rc)(1) = 7;
    return values[4] == 7 && !np::try_row_view(right_view(values), -1) &&
           !np::try_column_view(right_view(values), 3);
}
static_assert(constant_subviews());

constexpr bool constant_empty_subviews() {
    using zero_columns = np::extents<size_t, 2, 0>;
    using zero_rows = np::extents<size_t, 0, 3>;
    auto rr = np::try_row_view(np::tensor_view<int, zero_columns>(nullptr), 1);
    auto rc = np::try_column_view(np::tensor_view<int, zero_rows>(nullptr), 1);
    auto lr = np::try_row_view(np::tensor_view<int, zero_columns, np::layout_left>(nullptr), 1);
    auto lc = np::try_column_view(np::tensor_view<int, zero_rows, np::layout_left>(nullptr), 1);
    return rr && rc && lr && lc && rr->empty() && rc->empty() && lr->empty() && lc->empty() &&
           rr->data_handle() == nullptr && rc->data_handle() == nullptr &&
           lr->data_handle() == nullptr && lc->data_handle() == nullptr;
}
static_assert(constant_empty_subviews());
} // namespace

TEST(np_subview, layouts_and_static_extents_share_storage) {
    EXPECT_TRUE(constant_subviews());
    int values[] = {0, 1, 2, 3, 4, 5};
    const right_view source(values);
    auto row = np::try_row_view(source, 1);
    auto column = np::try_column_view(source, 1);
    ASSERT_TRUE(row);
    ASSERT_TRUE(column);
    static_assert(std::same_as<decltype((*row)(0)), int &>);
    EXPECT_EQ(row->data_handle(), values + 3);
    EXPECT_EQ(column->data_handle(), values + 1);
    EXPECT_EQ(row->stride(0), 1u);
    EXPECT_EQ(column->stride(0), 3u);
    (*row)(1) = 9;
    EXPECT_EQ((*column)(1), 9);
    auto read_only = np::try_column_view(np::tensor_view<const int, shape>(values), 1);
    ASSERT_TRUE(read_only);
    static_assert(std::same_as<decltype((*read_only)(0)), const int &>);
    EXPECT_EQ((*read_only)(1), 9);
}

TEST(np_subview, mixed_dynamic_and_non_narrowed_indices) {
    int values[6]{};
    using mixed_shape = np::extents<int, np::dynamic_extent, 3>;
    np::tensor_view<int, mixed_shape> source(values, 2);
    auto row = np::try_row_view(source, 1);
    auto column = np::try_column_view(source, 2);
    ASSERT_TRUE(row);
    ASSERT_TRUE(column);
    static_assert(decltype(row)::value_type::static_extent(0) == 3);
    static_assert(decltype(column)::value_type::static_extent(0) == np::dynamic_extent);
    EXPECT_EQ(column->extent(0), 2);
    EXPECT_FALSE(np::try_row_view(source, uint64_t{256}));
    EXPECT_FALSE(np::try_row_view(source, int64_t{-256}));
    EXPECT_FALSE(np::try_row_view(source, 2));
    EXPECT_FALSE(np::try_column_view(source, 3));
    EXPECT_FALSE(np::try_column_view(source, std::numeric_limits<uint64_t>::max()));
    if constexpr (std::numeric_limits<unsigned int>::digits <
                  std::numeric_limits<uint64_t>::digits) {
        const auto too_wide = static_cast<uint64_t>(std::numeric_limits<unsigned int>::max()) + 1;
        EXPECT_FALSE(np::try_row_view(source, too_wide));
    }
}

TEST(np_subview, empty_optional_differs_from_empty_result) {
    EXPECT_TRUE(constant_empty_subviews());
    using dynamic_shape = np::dextents<size_t, 2>;
    np::tensor_view<int, dynamic_shape> no_rows(nullptr, 0, 3);
    EXPECT_FALSE(np::try_row_view(no_rows, 0));
    auto empty_column = np::try_column_view(no_rows, 2);
    ASSERT_TRUE(empty_column);
    EXPECT_TRUE(empty_column->empty());
    np::tensor_view<int, dynamic_shape> no_columns(nullptr, 2, 0);
    EXPECT_FALSE(np::try_column_view(no_columns, 0));
    auto empty_row = np::try_row_view(no_columns, 1);
    ASSERT_TRUE(empty_row);
    EXPECT_TRUE(empty_row->empty());
}

TEST(np_subview, owner_borrow_real_bool_and_alignment) {
    owner storage;
    auto row = np::try_row_view(storage.view(), 1);
    ASSERT_TRUE(row);
    (*row)(2) = 7;
    EXPECT_EQ(storage.view()(1, 2), 7);
    auto column = np::try_column_view(std::as_const(storage).view(), 2);
    ASSERT_TRUE(column);
    static_assert(std::same_as<decltype((*column)(0)), const int &>);
    EXPECT_EQ((*column)(1), 7);
    bool flags[4]{};
    auto booleans = np::try_row_view(np::tensor_view<bool, np::extents<size_t, 2, 2>>(flags), 1);
    ASSERT_TRUE(booleans);
    (*booleans)(0) = true;
    EXPECT_TRUE(flags[2]);
    struct alignas(64) aligned {
        int value;
    };
    aligned values[4]{};
    auto aligned_row =
        np::try_row_view(np::tensor_view<aligned, np::extents<size_t, 2, 2>>(values), 1);
    ASSERT_TRUE(aligned_row);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(aligned_row->data_handle()) % alignof(aligned), 0u);
}
