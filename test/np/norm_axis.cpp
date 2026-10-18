#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/norm_axis.hpp>
#include <wjr/np/slice.hpp>

namespace {
namespace np = wjr::np;
using source_shape = np::extents<size_t, 2, 3>;
using source_view = np::tensor_view<const float, source_shape>;
using row_output = np::tensor_view<double, np::extents<size_t, 2>>;

template <class D, class S, size_t Axis = 1, class Acc = void>
concept has_axis_norm = requires(D dst, S src) { np::norm_l2_to<Acc>(dst, src, np::axes<Axis>); };
template <class D, class S, size_t Axis = 1>
concept has_kept_norm =
    requires(D dst, S src) { np::norm_l2_to(dst, src, np::axes<Axis>, np::keepdims); };

struct custom_accessor : np::default_accessor<float> {};
static_assert(has_axis_norm<row_output, source_view>);
static_assert(has_axis_norm<row_output, source_view, 1, long double>);
static_assert(!has_axis_norm<row_output, source_view, 2>);
static_assert(!has_axis_norm<row_output, source_view, 1, int>);
static_assert(!has_axis_norm<row_output, source_view, 1, const double>);
static_assert(!has_axis_norm<np::tensor_view<int, np::extents<size_t, 2>>, source_view>);
static_assert(!has_axis_norm<np::tensor_view<const double, np::extents<size_t, 2>>, source_view>);
static_assert(
    !has_axis_norm<np::tensor_view<volatile double, np::extents<size_t, 2>>, source_view>);
static_assert(!has_axis_norm<row_output, np::tensor_view<volatile float, source_shape>>);
static_assert(!has_axis_norm<
              row_output, np::tensor_view<float, source_shape, np::layout_right, custom_accessor>>);
static_assert(!has_axis_norm<np::tensor_view<double, np::extents<size_t, 3>>, source_view>);
static_assert(!has_kept_norm<np::tensor_view<double, np::extents<size_t, 2, 2>>, source_view>);
static_assert(has_kept_norm<np::tensor_view<double, np::extents<size_t, 2, 1>>, source_view>);
static_assert(!has_axis_norm<np::tensor_view<double, np::extents<size_t>>,
                             np::tensor_view<float, np::extents<size_t>>, 0>);
static_assert(noexcept(np::norm_l2_to(std::declval<row_output>(), std::declval<source_view>(),
                                      np::axes<1>)));

template <size_t Axis, class Layout>
void check_rank_three_axis() {
    using shape = np::extents<size_t, 2, 3, 4>;
    std::array<double, 24> storage{};
    np::tensor_view<double, shape, Layout> src(storage.data());
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 3; ++j)
            for (size_t k = 0; k != 4; ++k)
                src(i, j, k) = std::ldexp(double(1 + i + j + k), int(k) * 500 - 750);
    std::array<size_t, 2> remaining{};
    for (size_t axis = 0, out = 0; axis != 3; ++axis)
        if (axis != Axis)
            remaining[out++] = src.extent(axis);
    std::array<double, 12> result{}, kept_result{};
    // Deliberately use a different output layout from the source.
    using output_layout = std::conditional_t<std::same_as<Layout, np::layout_right>,
                                             np::layout_left, np::layout_right>;
    np::tensor_view<double, np::dextents<size_t, 2>, output_layout> dst(result.data(), remaining[0],
                                                                        remaining[1]);
    std::array<size_t, 3> kept_shape{2, 3, 4};
    kept_shape[Axis] = 1;
    np::tensor_view<double, np::dextents<size_t, 3>, output_layout> kept(kept_result.data(),
                                                                         kept_shape);
    np::norm_l2_to(dst, src, np::axes<Axis>);
    np::norm_l2_to(kept, src, np::axes<Axis>, np::keepdims);
    for (size_t a = 0; a != remaining[0]; ++a) {
        for (size_t b = 0; b != remaining[1]; ++b) {
            std::array<size_t, 3> coordinate{};
            for (size_t axis = 0, out = 0; axis != 3; ++axis)
                if (axis != Axis)
                    coordinate[axis] = out++ ? b : a;
            using line_shape = np::dextents<size_t, 1>;
            const np::layout_stride::mapping<line_shape> mapping(line_shape(src.extent(Axis)),
                                                                 std::array{src.stride(Axis)});
            const auto offset = src.mapping()(coordinate[0], coordinate[1], coordinate[2]);
            const auto expected =
                np::norm_l2(np::tensor_view<const double, line_shape, np::layout_stride>(
                    storage.data() + offset, mapping));
            EXPECT_EQ(dst(a, b), expected);
            EXPECT_EQ(kept(coordinate[0], coordinate[1], coordinate[2]), expected);
            long double oracle = 0;
            for (size_t i = 0; i != src.extent(Axis); ++i) {
                coordinate[Axis] = i;
                oracle = std::hypot(oracle, static_cast<long double>(
                                                src(coordinate[0], coordinate[1], coordinate[2])));
            }
            EXPECT_NEAR(dst(a, b), double(oracle), double(oracle) * 1e-14);
        }
    }
}

TEST(np_norm_axis, PackedLayoutsEveryAxisAndKeepdims) {
    check_rank_three_axis<0, np::layout_right>();
    check_rank_three_axis<1, np::layout_right>();
    check_rank_three_axis<2, np::layout_right>();
    check_rank_three_axis<0, np::layout_left>();
    check_rank_three_axis<1, np::layout_left>();
    check_rank_three_axis<2, np::layout_left>();
}

TEST(np_norm_axis, AccumulatorIndependentOfDestination) {
    const float data[] = {1, 1};
    np::tensor_view<const float, np::extents<size_t, 1, 2>> src(data);
    double result = 0;
    np::tensor_view<double, np::extents<size_t, 1>> dst(&result);
    np::norm_l2_to(dst, src, np::axes<1>);
    EXPECT_EQ(result,
              double(np::norm_l2(np::tensor_view<const float, np::extents<size_t, 2>>(data))));
    EXPECT_NE(result, std::sqrt(2.0));
    np::norm_l2_to<double>(dst, src, np::axes<1>);
    EXPECT_EQ(result, std::sqrt(2.0));
    const std::int64_t minimum = std::numeric_limits<std::int64_t>::min();
    np::tensor_view<double, np::extents<size_t>> scalar(&result);
    np::norm_l2_to(scalar, np::tensor_view<const std::int64_t, np::extents<size_t, 1>>(&minimum),
                   np::axes<0>);
    EXPECT_EQ(result, std::ldexp(1.0, 63));
    const bool flags[] = {true, false, true, true, true};
    np::norm_l2_to(scalar, np::tensor_view<const bool, np::extents<size_t, 5>>(flags), np::axes<0>);
    EXPECT_EQ(result, 2);
}

TEST(np_norm_axis, StridedInputOutputAndPadding) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double data[] = {3, nan, 4, nan, 0, nan, nan, nan, 5, nan, 12, nan, 0};
    const np::layout_stride::mapping<source_shape> mapping(source_shape{},
                                                           std::array<size_t, 2>{8, 2});
    np::tensor_view<const double, source_shape, np::layout_stride> src(data, mapping);
    double result[] = {-9, -9, -9, -9};
    using shape = np::extents<size_t, 2>;
    const np::layout_stride::mapping<shape> out_mapping(shape{}, std::array<size_t, 1>{3});
    np::tensor_view<double, shape, np::layout_stride> dst(result, out_mapping);
    np::norm_l2_to(dst, src, np::axes<1>);
    EXPECT_EQ(result[0], 5);
    EXPECT_EQ(result[3], 13);
    EXPECT_EQ(result[1], -9);
    EXPECT_EQ(result[2], -9);
    using kept_shape = np::extents<size_t, 2, 1>;
    const np::layout_stride::mapping<kept_shape> kept_mapping(kept_shape{},
                                                              std::array<size_t, 2>{3, 1});
    np::tensor_view<double, kept_shape, np::layout_stride> kept(result, kept_mapping);
    np::norm_l2_to(kept, src, np::axes<1>, np::keepdims);
    EXPECT_EQ(result[0], 5);
    EXPECT_EQ(result[3], 13);
    EXPECT_EQ(result[1], -9);
    EXPECT_EQ(result[2], -9);
}

template <class Layout>
void check_padded_slice() {
    std::array<double, 24> storage;
    storage.fill(std::numeric_limits<double>::quiet_NaN());
    np::tensor_view<double, np::extents<size_t, 4, 6>, Layout> full(storage.data());
    auto src = np::slice_view(full, std::pair{1, 3}, std::pair{1, 4});
    static_assert(np::detail::padded_layout<typename decltype(src)::layout_type>);
    src(0, 0) = 3;
    src(0, 1) = 4;
    src(0, 2) = 0;
    src(1, 0) = 5;
    src(1, 1) = 12;
    src(1, 2) = 0;
    double result[2]{};
    np::tensor_view<double, np::extents<size_t, 2>> dst(result);
    np::norm_l2_to(dst, src, np::axes<1>);
    EXPECT_EQ(result[0], 5);
    EXPECT_EQ(result[1], 13);
}

TEST(np_norm_axis, PaddedSlicesPreservePhysicalPitch) {
    check_padded_slice<np::layout_right>();
    check_padded_slice<np::layout_left>();
}

TEST(np_norm_axis, EmptyLinesAndEmptyOutputAvoidNullArithmetic) {
    np::tensor_view<const double, np::dextents<size_t, 2>> src(nullptr, 3, 0);
    double result[] = {-1, -1, -1};
    np::tensor_view<double, np::extents<size_t, 3>> dst(result);
    np::norm_l2_to(dst, src, np::axes<1>);
    np::tensor_view<double, np::extents<size_t, 3, 1>> kept(result);
    np::norm_l2_to(kept, src, np::axes<1>, np::keepdims);
    for (const auto value : result) {
        EXPECT_EQ(value, 0);
        EXPECT_FALSE(std::signbit(value));
    }
    np::tensor_view<const double, np::extents<size_t, 0, 7>> no_rows(nullptr);
    np::tensor_view<double, np::extents<size_t, 0>> no_output(nullptr);
    np::norm_l2_to(no_output, no_rows, np::axes<1>);
    double scalar = -1;
    np::norm_l2_to(np::tensor_view<double, np::extents<size_t>>(&scalar), no_output, np::axes<0>);
    EXPECT_EQ(scalar, 0);
    EXPECT_FALSE(std::signbit(scalar));
}

TEST(np_norm_axis, ExtremeValuesAndNonfinitePriorityPerLine) {
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double data[] = {3e200, 4e200, 0,   3e-200, 4e-200, 0, inf,  -inf, 0,
                           inf,   0,     nan, nan,    -inf,   0, -0.0, 0.0,  -0.0};
    double result[6]{};
    np::norm_l2_to(np::tensor_view<double, np::extents<size_t, 6>>(result),
                   np::tensor_view<const double, np::extents<size_t, 6, 3>>(data), np::axes<1>);
    EXPECT_NEAR(result[0] / 5e200, 1, 1e-14);
    EXPECT_NEAR(result[1] / 5e-200, 1, 1e-14);
    EXPECT_EQ(result[2], inf);
    EXPECT_TRUE(std::isnan(result[3]));
    EXPECT_TRUE(std::isnan(result[4]));
    EXPECT_EQ(result[5], 0);
    EXPECT_FALSE(std::signbit(result[5]));
    const auto tiny = std::numeric_limits<double>::denorm_min();
    double subnormal = -1;
    np::norm_l2_to(np::tensor_view<double, np::extents<size_t>>(&subnormal),
                   np::tensor_view<const double, np::extents<size_t, 1>>(&tiny), np::axes<0>);
    EXPECT_EQ(subnormal, tiny);
}

TEST(np_norm_axis, FiniteBucketOverflowRetainsScaledRecovery) {
    // The medium bucket overflows, while the final float norm is representable.
    const std::vector<float> data(size_t{1} << 24, 0x1p52f);
    float result = -1;
    np::norm_l2_to(np::tensor_view<float, np::extents<size_t>>(&result),
                   np::tensor_view<const float, np::dextents<size_t, 1>>(data.data(), data.size()),
                   np::axes<0>);
    EXPECT_EQ(result, 0x1p64f);
}
} // namespace
