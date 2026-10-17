#include <array>
#include <cstdint>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/np/tensor.hpp>
#include <wjr/np/transpose.hpp>

namespace {
namespace np = wjr::np;

template <class View>
concept transposes = requires(View source) { np::transpose_view(source); };

using fixed_shape = np::extents<size_t, 2, 3>;
using mixed_shape = np::extents<int, np::dynamic_extent, 3>;
using dynamic_shape = np::dextents<size_t, 2>;
using fixed_view = np::tensor_view<int, fixed_shape>;
using mixed_view = np::tensor_view<const double, mixed_shape, np::layout_left>;
using fixed_result = decltype(np::transpose_view(std::declval<fixed_view>()));
using mixed_result = decltype(np::transpose_view(std::declval<mixed_view>()));

static_assert(
    std::same_as<fixed_result, np::tensor_view<int, np::extents<size_t, 3, 2>, np::layout_left>>);
static_assert(std::same_as<mixed_result,
                           np::tensor_view<const double, np::extents<int, 3, np::dynamic_extent>>>);
static_assert(std::same_as<decltype(np::transpose_view(std::declval<fixed_result>())), fixed_view>);
static_assert(std::is_trivially_copyable_v<fixed_result>);
static_assert(noexcept(np::transpose_view(std::declval<fixed_view>())));
static_assert(!transposes<np::tensor_view<int, np::extents<size_t>>>);
static_assert(!transposes<np::tensor_view<int, np::extents<size_t, 2>>>);
static_assert(!transposes<np::tensor_view<int, np::extents<size_t, 2, 3, 4>>>);
static_assert(!transposes<np::tensor_view<int, fixed_shape, np::layout_stride>>);

struct custom_accessor : np::default_accessor<int> {};
static_assert(!transposes<np::tensor_view<int, fixed_shape, np::layout_right, custom_accessor>>);

template <class Owner>
concept transposes_owner =
    requires(Owner &&owner) { np::transpose_view(std::forward<Owner>(owner)); };
using owner_type = np::tensor<int, fixed_shape>;
static_assert(!transposes_owner<owner_type &> && !transposes_owner<owner_type>);

template <class Layout, class Index>
constexpr bool constant_transpose() {
    int data[] = {0, 1, 2, 3, 4, 5};
    using shape = np::extents<Index, np::dynamic_extent, 3>;
    np::tensor_view<int, shape, Layout> source(data, 2);
    const auto result = np::transpose_view(source);
    if (result.data_handle() != source.data_handle() || result.extent(0) != 3 ||
        result.extent(1) != 2 || result.size() != 6 ||
        result.mapping().required_span_size() != source.mapping().required_span_size())
        return false;
    for (Index row = 0; row < 2; ++row) {
        for (Index column = 0; column < 3; ++column) {
            if (&result(column, row) != &source(row, column))
                return false;
        }
    }
    result(2, 1) = 17;
    const auto round_trip = np::transpose_view(result);
    return source(1, 2) == 17 && round_trip.extents() == source.extents() &&
           round_trip.mapping() == source.mapping() && round_trip.data_handle() == data;
}

static_assert(constant_transpose<np::layout_right, size_t>());
static_assert(constant_transpose<np::layout_left, int>());
static_assert(constant_transpose<np::layout_right, std::uint8_t>());
static_assert(constant_transpose<np::layout_left, std::uint16_t>());

TEST(np_transpose, preserves_addresses_and_mutability) {
    EXPECT_TRUE((constant_transpose<np::layout_right, size_t>()));
    EXPECT_TRUE((constant_transpose<np::layout_left, int>()));
}

TEST(np_transpose, dynamic_and_const_views) {
    const double data[] = {1, 2, 3, 4, 5, 6};
    np::tensor_view<const double, dynamic_shape> source(data, 2, 3);
    const auto result = np::transpose_view(source);
    static_assert(std::same_as<decltype(result(0, 0)), const double &>);
    EXPECT_EQ(result.extent(0), 3);
    EXPECT_EQ(result.extent(1), 2);
    EXPECT_EQ(result.stride(0), 1);
    EXPECT_EQ(result.stride(1), 3);
    EXPECT_EQ(result(1, 1), 5);
    EXPECT_EQ(result.data_handle(), data);
}

template <class Layout>
void check_empty_transpose() {
    for (auto dimensions : {std::array<size_t, 2>{0, 0}, {0, 3}, {2, 0}}) {
        np::tensor_view<int, dynamic_shape, Layout> source(nullptr, dimensions);
        const auto result = np::transpose_view(source);
        EXPECT_EQ(result.data_handle(), nullptr);
        EXPECT_EQ(result.extent(0), dimensions[1]);
        EXPECT_EQ(result.extent(1), dimensions[0]);
        EXPECT_TRUE(result.empty());
        EXPECT_EQ(result.size(), 0);
        EXPECT_EQ(result.mapping().required_span_size(), 0);
    }
    np::tensor_view<int, np::extents<size_t, 0, 3>, Layout> source(nullptr);
    const auto result = np::transpose_view(source);
    EXPECT_EQ(result.data_handle(), nullptr);
    EXPECT_TRUE(result.empty());
}

TEST(np_transpose, empty_views_do_not_offset_null) {
    check_empty_transpose<np::layout_left>();
    check_empty_transpose<np::layout_right>();
}

TEST(np_transpose, borrows_owner_without_moving_elements) {
    owner_type owner;
    owner.view()(1, 2) = 9;
    const auto result = np::transpose_view(owner.view());
    result(2, 1) = 11;
    EXPECT_EQ(owner.view()(1, 2), 11);
    EXPECT_EQ(result.data_handle(), owner.view().data_handle());
    const auto readonly = np::transpose_view(std::as_const(owner).view());
    static_assert(std::same_as<decltype(readonly(0, 0)), const int &>);
    EXPECT_EQ(readonly(2, 1), 11);
}

struct alignas(64) aligned_value {
    int value = 0;
    aligned_value() = default;
    aligned_value(const aligned_value &) = delete;
    aligned_value(aligned_value &&) = delete;
};

TEST(np_transpose, supports_real_bool_and_immovable_aligned_elements) {
    bool bits[] = {false, true, false, false, false, true};
    const auto transposed_bits = np::transpose_view(np::tensor_view<bool, fixed_shape>(bits));
    transposed_bits(0, 1) = true;
    EXPECT_TRUE(bits[3]);
    aligned_value values[6];
    const auto transposed = np::transpose_view(np::tensor_view<aligned_value, fixed_shape>(values));
    transposed(2, 1).value = 42;
    EXPECT_EQ(values[5].value, 42);
    EXPECT_EQ(&transposed(2, 1), values + 5);
}
} // namespace
