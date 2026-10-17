#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/view.hpp>

namespace {

namespace np = wjr::np;
using static_shape = np::extents<size_t, 2, 3>;
using mixed_shape = np::extents<size_t, np::dynamic_extent, 3>;
using dynamic_shape = np::dextents<size_t, 2>;
using static_view = np::tensor_view<int, static_shape>;
using mixed_view = np::tensor_view<int, mixed_shape>;
using dynamic_view = np::tensor_view<int, dynamic_shape>;
using const_view = np::tensor_view<const int, static_shape>;

static_assert(static_view::rank() == 2);
static_assert(mixed_view::rank_dynamic() == 1);
static_assert(std::is_same_v<static_view::index_type, size_t>);
static_assert(std::is_trivially_copyable_v<static_view>);
static_assert(std::is_trivially_copyable_v<dynamic_view>);
static_assert(std::is_default_constructible_v<dynamic_view>);
static_assert(!std::is_default_constructible_v<static_view>);
static_assert(!std::is_default_constructible_v<np::tensor_view<int, np::extents<int>>>);
static_assert(std::is_convertible_v<static_view, const_view>);
static_assert(!std::is_constructible_v<static_view, const_view>);
static_assert(std::is_convertible_v<static_view, dynamic_view>);
static_assert(std::is_constructible_v<static_view, dynamic_view>);
static_assert(!std::is_convertible_v<dynamic_view, static_view>);
static_assert(
    !std::is_constructible_v<static_view, np::tensor_view<int, np::extents<size_t, 2, 4>>>);
static_assert(!std::is_convertible_v<dynamic_shape, np::dextents<uint8_t, 2>>);
static_assert(std::is_same_v<decltype(std::declval<const static_view &>()(0, 0)), int &>);
static_assert(std::is_same_v<decltype(std::declval<const_view>()(0, 0)), const int &>);

struct base {
    int value;
};
struct derived : base {
    int extra;
};
using base_view = np::tensor_view<base, static_shape>;
using derived_view = np::tensor_view<derived, static_shape>;
static_assert(!std::is_constructible_v<base_view, derived_view>);

template <class View, class... Indices>
constexpr bool can_index = requires(View view, Indices... indices) { view(indices...); };
static_assert(can_index<static_view, int, int>);
static_assert(!can_index<static_view, int>);
static_assert(!can_index<static_view, int, int, int>);

constexpr bool constexpr_indexing() {
    int values[] = {0, 1, 2, 3, 4, 5};
    static_view view(values);
    view(1, 2) = 7;
    return view.size() == 6 && view.extent(0) == 2 && view(1, 1) == 4 && values[5] == 7;
}
static_assert(constexpr_indexing());

// These describe the expected representation on the supported x64 ABIs. They
// deliberately test the pinned backend, rather than promising a serialized ABI.
#if defined(_MSC_VER)
static_assert(sizeof(static_view) == 2 * sizeof(int *));
#else
static_assert(sizeof(static_view) == sizeof(int *));
#endif
static_assert(sizeof(mixed_view) == sizeof(int *) + sizeof(size_t));
static_assert(sizeof(dynamic_view) == sizeof(int *) + 2 * sizeof(size_t));
static_assert(alignof(static_view) == alignof(int *));

struct shifted_accessor {
    using offset_policy = shifted_accessor;
    using element_type = int;
    using reference = int &;
    using data_handle_type = int *;

    size_t shift;

    constexpr reference access(data_handle_type ptr, size_t offset) const noexcept {
        return ptr[shift + offset];
    }
    constexpr data_handle_type offset(data_handle_type ptr, size_t offset) const noexcept {
        return ptr + offset;
    }
};

struct alignas(64) aligned_element {
    int value;
};

} // namespace

TEST(np_view, row_major_and_const_elements) {
    std::array<int, 6> data = {0, 1, 2, 3, 4, 5};
    const static_view view(data.data());
    EXPECT_EQ(view.data_handle(), data.data());
    EXPECT_TRUE(view.is_unique());
    EXPECT_TRUE(view.is_exhaustive());
    EXPECT_TRUE(view.is_strided());
    EXPECT_EQ(view.stride(0), 3u);
    EXPECT_EQ(view.stride(1), 1u);

    view(1, 2) = 42;
    const_view read_only = view;
    EXPECT_EQ(read_only(1, 2), 42);
    EXPECT_EQ(data[5], 42);

    auto copy = view;
    EXPECT_EQ(copy.data_handle(), view.data_handle());
    copy(0, 1) = 7;
    EXPECT_EQ(view(0, 1), 7);
}

TEST(np_view, dynamic_and_mixed_extents) {
    std::array<int, 6> data = {0, 1, 2, 3, 4, 5};
    mixed_view mixed(data.data(), 2);
    dynamic_view dynamic(data.data(), 2, 3);
    EXPECT_EQ(mixed.extent(0), 2u);
    EXPECT_EQ(mixed.extent(1), 3u);
    EXPECT_EQ(mixed(1, 2), dynamic(1, 2));

    // Values match the static extents before this explicit conversion.
    static_view fixed(dynamic);
    EXPECT_EQ(fixed(1, 2), 5);
}

TEST(np_view, column_major) {
    std::array<int, 6> data = {0, 1, 2, 3, 4, 5};
    np::tensor_view<int, static_shape, np::layout_left> view(data.data());
    EXPECT_EQ(view.stride(0), 1u);
    EXPECT_EQ(view.stride(1), 2u);
    EXPECT_EQ(view(1, 2), 5);
    view(0, 1) = 17;
    EXPECT_EQ(data[2], 17);
}

TEST(np_view, non_exhaustive_positive_strides) {
    std::array<int, 9> data = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    using mapping = np::layout_stride::mapping<static_shape>;
    const mapping map(static_shape{}, std::array<size_t, 2>{6, 1});
    np::tensor_view<int, static_shape, np::layout_stride> view(data.data(), map);
    EXPECT_EQ(view.size(), 6u);
    EXPECT_EQ(map.required_span_size(), 9u);
    EXPECT_TRUE(view.is_unique());
    EXPECT_FALSE(view.is_exhaustive());
    view(1, 2) = 42;
    EXPECT_EQ(data[8], 42);
    EXPECT_EQ(data[3], 3);
    EXPECT_EQ(data[4], 4);
    EXPECT_EQ(data[5], 5);
}

TEST(np_view, empty_and_rank_zero) {
    dynamic_view empty;
    EXPECT_EQ(empty.data_handle(), nullptr);
    EXPECT_EQ(empty.size(), 0u);
    EXPECT_TRUE(empty.empty());

    dynamic_view zero_dimension(nullptr, 3, 0);
    EXPECT_EQ(zero_dimension.mapping().required_span_size(), 0u);
    EXPECT_TRUE(zero_dimension.empty());

    int value = 8;
    np::tensor_view<int, np::extents<size_t>> scalar(&value);
    EXPECT_EQ(scalar.size(), 1u);
    EXPECT_FALSE(scalar.empty());
    scalar() = 9;
    EXPECT_EQ(value, 9);
}

TEST(np_view, stateful_accessor) {
    std::array<int, 8> data = {0, 1, 2, 3, 4, 5, 6, 7};
    using view_type = np::tensor_view<int, static_shape, np::layout_right, shifted_accessor>;
    const view_type view(data.data(), view_type::mapping_type{}, shifted_accessor{2});
    EXPECT_EQ(view.accessor().shift, 2u);
    EXPECT_EQ(view(0, 0), 2);
    auto copy = view;
    copy(1, 2) = 42;
    EXPECT_EQ(data[7], 42);
    EXPECT_EQ(view(1, 2), 42);
}

TEST(np_view, bool_and_overaligned_objects) {
    bool flags[2] = {false, true};
    np::tensor_view<bool, np::extents<size_t, 2>> flag_view(flags);
    static_assert(std::is_same_v<decltype(flag_view(0)), bool &>);
    flag_view(0) = true;
    EXPECT_TRUE(flags[0]);

    aligned_element data[2] = {{1}, {2}};
    np::tensor_view<aligned_element, np::extents<size_t, 2>> view(data);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&view(1)) % alignof(aligned_element), 0u);
    view(1).value = 7;
    EXPECT_EQ(data[1].value, 7);
}
