#include <array>
#include <cstdint>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/np/copy.hpp>
#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_reduce.hpp>
#include <wjr/np/scan.hpp>
#include <wjr/np/slice.hpp>
#include <wjr/np/transform.hpp>

namespace {
namespace np = wjr::np;
using shape = np::extents<size_t, 4, 6>;
template <size_t N>
using constant = std::integral_constant<size_t, N>;

template <class View, class... Slices>
concept can_slice = requires(View view, Slices... slices) { np::slice_view(view, slices...); };
struct custom_accessor : np::default_accessor<int> {};
static_assert(can_slice<np::tensor_view<int, shape>, int, np::full_extent_t>);
static_assert(!can_slice<np::tensor_view<int, shape>, int>);
static_assert(!can_slice<np::tensor_view<int, shape>, int, int, int>);
static_assert(!can_slice<np::tensor_view<int, np::extents<uint16_t, 4, 6>>, int, int>);
static_assert(!can_slice<np::tensor_view<int, shape, np::layout_right, custom_accessor>, int, int>);
static_assert(!can_slice<np::tensor<int, shape>, int, int>);
using stepped = np::strided_slice<int, int, int>;
static_assert(!can_slice<np::tensor_view<int, np::extents<size_t, 0, 6>, np::layout_left>,
                         np::full_extent_t, stepped>);
static_assert(
    !can_slice<np::tensor_view<int, np::extents<size_t, 4, 0>>, stepped, np::full_extent_t>);
static_assert(
    can_slice<np::tensor_view<int, np::extents<size_t, 0, 6>>, np::full_extent_t, stepped>);
using nondivisible_static = np::strided_slice<constant<0>, constant<5>, constant<2>>;
static_assert(!can_slice<np::tensor_view<int, shape>, np::full_extent_t, nondivisible_static>);

constexpr bool static_slices() {
    int values[24]{};
    np::tensor_view<int, shape> source(values);
    auto row = np::slice_view(source, 1, np::full_extent);
    static_assert(std::same_as<typename decltype(row)::layout_type, np::layout_right>);
    static_assert(decltype(row)::static_extent(0) == 6);
    auto window = np::slice_view(source, std::pair{constant<1>{}, constant<3>{}},
                                 np::strided_slice{constant<0>{}, constant<6>{}, constant<2>{}});
    static_assert(decltype(window)::static_extent(0) == 2);
    static_assert(decltype(window)::static_extent(1) == 3);
    static_assert(std::same_as<typename decltype(window)::layout_type, np::layout_stride>);
    auto padded = np::slice_view(source, np::full_extent, std::pair{1, 5});
    static_assert(std::same_as<typename decltype(padded)::layout_type,
                               Kokkos::Experimental::layout_right_padded<6>>);
    auto nested = np::slice_view(padded, 2, np::strided_slice{0, 4, 2});
    auto scalar = np::slice_view(source, 3, 4);
    static_assert(decltype(scalar)::rank() == 0);
    np::tensor_view<int, np::dextents<size_t, 2>> dynamic_source(values, 4, 6);
    auto mixed = np::slice_view(dynamic_source, np::full_extent,
                                np::strided_slice{constant<0>{}, constant<6>{}, constant<2>{}});
    static_assert(decltype(mixed)::static_extent(0) == np::dynamic_extent);
    static_assert(decltype(mixed)::static_extent(1) == 3);
    auto nondivisible = np::slice_view(source, np::full_extent, np::strided_slice{0, 5, 2});
    np::tensor_view<int, np::extents<size_t>> scalar_source(values);
    auto same_scalar = np::slice_view(scalar_source);
    return &row(2) == values + 8 && &window(1, 2) == values + 16 && &nested(1) == values + 15 &&
           &scalar() == values + 22 && &same_scalar() == values && &mixed(2, 1) == values + 14 &&
           nondivisible.extent(1) == 3 && &nondivisible(2, 2) == values + 16;
}
static_assert(static_slices());

constexpr bool empty_slices() {
    np::tensor_view<int, np::extents<size_t, 0, 6>> source(nullptr);
    auto full = np::slice_view(source, np::full_extent, np::full_extent);
    auto stepped = np::slice_view(source, np::full_extent, np::strided_slice{0, 6, 2});
    int data[24]{};
    np::tensor_view<int, shape> valid(data);
    auto end = np::slice_view(valid, std::pair{4, 4}, std::pair{6, 6});
    auto interior = np::slice_view(valid, std::pair{1, 1}, np::full_extent);
    return full.empty() && stepped.empty() && full.data_handle() == nullptr &&
           stepped.data_handle() == nullptr && end.empty() && end.data_handle() == data + 24 &&
           interior.empty() && interior.data_handle() == data + 6;
}
static_assert(empty_slices());

template <class Layout>
void padded_kernels() {
    std::array<double, 24> data;
    data.fill(-99);
    np::tensor_view<double, shape, Layout> source(data.data());
    auto window = np::slice_view(source, std::pair{1, 3}, std::pair{1, 5});
    static_assert(np::detail::padded_layout<typename decltype(window)::layout_type>);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 4; ++col)
            window(row, col) = static_cast<double>(row * 4 + col + 1);
    double packed_values[8]{};
    np::tensor_view<double, np::extents<size_t, 2, 4>> packed(packed_values);
    np::copy_to(packed, window);
    for (size_t i = 0; i != 8; ++i)
        EXPECT_EQ(packed_values[i], i + 1);
    np::fill_to(window, 3.0);
    np::transform_to(window, window, [](double x) noexcept { return x * 2; });
    np::copy_to(window, packed);
    auto expression = np::as_expr(window) * 2.0 + 1.0;
    auto owner = np::eval(expression);
    ASSERT_TRUE(owner);
    using expected_layout = std::conditional_t<std::same_as<Layout, np::layout_left>,
                                               np::layout_left, np::layout_right>;
    static_assert(std::same_as<typename decltype(owner)::value_type::layout_type, expected_layout>);
    np::eval_to(window, expression);
    EXPECT_EQ(np::sum(window), 80);
    EXPECT_EQ(np::sum(np::as_expr(window)), 80);
    for (size_t row = 0; row != 4; ++row)
        for (size_t col = 0; col != 6; ++col)
            if (row < 1 || row >= 3 || col < 1 || col >= 5)
                EXPECT_EQ(source(row, col), -99);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 4; ++col)
            EXPECT_EQ(owner->view()(row, col), window(row, col));

    double totals[2]{};
    np::tensor_view<double, np::extents<size_t, 2>> out(totals);
    np::sum_to(out, window, np::axes<1>);
    EXPECT_EQ(totals[0], 24);
    EXPECT_EQ(totals[1], 56);
    np::sum_to(out, np::as_expr(window) + 1.0, np::axes<1>);
    EXPECT_EQ(totals[0], 28);
    EXPECT_EQ(totals[1], 60);
    np::cumsum_to(packed, window, np::axes<0>);
    EXPECT_EQ(packed(0, 0), 3);
    EXPECT_EQ(packed(1, 0), 14);
    EXPECT_EQ(packed(1, 3), 26);
    np::cumsum_to(window, window, np::axes<1>);
    EXPECT_EQ(window(0, 3), 24);
    EXPECT_EQ(window(1, 3), 56);
    // A sliced padded destination also needs the generic zero-length-axis path.
    auto empty = np::slice_view(window, np::full_extent, std::pair{0, 0});
    np::sum_to(out, empty, np::axes<1>);
    EXPECT_EQ(totals[0], 0);
    EXPECT_EQ(totals[1], 0);
    np::tensor_view<const double, np::extents<size_t, 2, 0, 4>> zero_axis(nullptr);
    np::sum_to(window, zero_axis, np::axes<1>);
    for (size_t row = 0; row != 4; ++row)
        for (size_t col = 0; col != 6; ++col)
            EXPECT_EQ(source(row, col), row >= 1 && row < 3 && col >= 1 && col < 5 ? 0 : -99);
}
} // namespace

TEST(np_slice, static_metadata_and_empty_boundaries) {
    EXPECT_TRUE(static_slices());
    EXPECT_TRUE(empty_slices());
}

TEST(np_slice, padded_right_kernels_preserve_holes) { padded_kernels<np::layout_right>(); }
TEST(np_slice, padded_left_kernels_preserve_holes) { padded_kernels<np::layout_left>(); }

TEST(np_slice, strided_source_and_rank_reduction) {
    std::array<int, 40> values{};
    using extents = np::extents<size_t, 3, 4>;
    np::tensor_view<int, extents, np::layout_stride> source(
        values.data(),
        np::layout_stride::mapping<extents>(extents{}, std::array<size_t, 2>{14, 2}));
    auto window = np::slice_view(source, std::pair{1, 3}, np::strided_slice{1, 3, 2});
    EXPECT_EQ(window.extent(0), 2u);
    EXPECT_EQ(window.extent(1), 2u);
    EXPECT_EQ(window.stride(0), 14u);
    EXPECT_EQ(window.stride(1), 4u);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 2; ++col) {
            EXPECT_EQ(&window(row, col), values.data() + (row + 1) * 14 + 2 + col * 4);
            window(row, col) = 5;
        }
    EXPECT_EQ(np::sum(window), 20);
    const auto backend = Kokkos::submdspan(source, std::pair{1, 3}, Kokkos::strided_slice{1, 3, 2});
    EXPECT_EQ(window.data_handle(), backend.data_handle());
    EXPECT_EQ(window.mapping(), backend.mapping());
    np::tensor_view<const int, extents, np::layout_stride> read_only(source);
    auto selected = np::slice_view(read_only, 2, 3);
    static_assert(std::same_as<decltype(selected()), const int &>);
    EXPECT_EQ(selected(), 5);
}

TEST(np_slice, higher_rank_static_full_extents) {
    int data[120]{};
    using extents = np::extents<size_t, 2, 3, 4, 5>;
    auto view = np::slice_view(np::tensor_view<int, extents>(data), 1, np::full_extent,
                               np::full_extent, std::pair{1, 4});
    static_assert(decltype(view)::rank() == 3);
    static_assert(decltype(view)::static_extent(0) == 3);
    static_assert(decltype(view)::static_extent(1) == 4);
    static_assert(decltype(view)::static_extent(2) == np::dynamic_extent);
    EXPECT_EQ(view.extent(2), 3u);
    EXPECT_EQ(&view(2, 3, 2), data + 118);
}
