#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/extrema.hpp>
#include <wjr/np/extrema_axis.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using vector_view = np::tensor_view<T, np::dextents<size_t, 1>>;
using matrix_shape = np::extents<size_t, 2, 3>;
using source_view = np::tensor_view<const float, matrix_shape>;
template <class D, class S, size_t Axis = 1>
concept available = requires(D dst, S src) {
    np::argmin_to(dst, src, np::axes<Axis>);
    np::argmax_to(dst, src, np::axes<Axis>);
};
template <class D, class S, size_t Axis = 1>
concept kept_available =
    requires(D dst, S src) { np::argmin_to(dst, src, np::axes<Axis>, np::keepdims); };
static_assert(available<vector_view<char>, source_view>);
static_assert(available<vector_view<wchar_t>, source_view>);
static_assert(available<vector_view<uint64_t>, source_view>);
static_assert(!available<vector_view<bool>, source_view>);
static_assert(!available<vector_view<float>, source_view>);
static_assert(!available<vector_view<const int>, source_view>);
static_assert(!available<vector_view<volatile int>, source_view>);
static_assert(!available<vector_view<int>, np::tensor_view<volatile float, matrix_shape>>);
static_assert(!available<vector_view<int>, source_view, 2>);
static_assert(!available<np::tensor_view<int, np::extents<size_t, 3>>, source_view>);
static_assert(!available<vector_view<int>, np::tensor_view<int, np::extents<size_t>>, 0>);
static_assert(kept_available<np::tensor_view<int, np::extents<size_t, 2, 1>>, source_view>);
static_assert(!kept_available<np::tensor_view<int, np::extents<size_t, 2, 2>>, source_view>);
static_assert(noexcept(np::argmin_to(std::declval<vector_view<int>>(), std::declval<source_view>(),
                                     np::axes<1>)));

constexpr bool constants() {
    const int input[] = {3, 1, 1, 0, 4, 2};
    unsigned rows[2], columns[3], kept[2];
    const np::tensor_view<const int, matrix_shape> src(input);
    np::argmin_to(np::tensor_view<unsigned, np::extents<size_t, 2>>(rows), src, np::axes<1>);
    np::argmax_to(np::tensor_view<unsigned, np::extents<size_t, 3>>(columns), src, np::axes<0>);
    np::argmin_to(np::tensor_view<unsigned, np::extents<size_t, 2, 1>>(kept), src, np::axes<1>,
                  np::keepdims);
    return rows[0] == 1 && rows[1] == 0 && columns[0] == 0 && columns[1] == 1 && columns[2] == 1 &&
           kept[0] == 1 && kept[1] == 0;
}
static_assert(constants());

TEST(NpAxisArgextrema, ConstantsAndScalarOutput) {
    EXPECT_TRUE(constants());
    const int keys[] = {4, 2, 2, 8};
    unsigned result = 99;
    np::tensor_view<unsigned, np::extents<size_t>> scalar(&result);
    np::argmin_to(scalar, vector_view<const int>(keys, 4), np::axes<0>);
    EXPECT_EQ(result, 1);
    np::argmax_to(scalar, vector_view<const int>(keys, 4), np::axes<0>);
    EXPECT_EQ(result, 3);
    unsigned kept = 99;
    np::argmax_to(vector_view<unsigned>(&kept, 1), vector_view<const int>(keys, 4), np::axes<0>,
                  np::keepdims);
    EXPECT_EQ(kept, 3);
}

TEST(NpAxisArgextrema, EmptyOutputIgnoresSourceAndEmptySelectedAxis) {
    using shape = np::dextents<size_t, 2>;
    np::argmin_to(vector_view<int>(nullptr, 0), np::tensor_view<const float, shape>(nullptr, 0, 0),
                  np::axes<1>);
    np::argmax_to(vector_view<int>(nullptr, 0), np::tensor_view<const int, shape>(nullptr, 2, 0),
                  np::axes<0>);
    np::argmin_to(np::tensor_view<int, shape>(nullptr, 0, 1),
                  np::tensor_view<const float, shape>(nullptr, 0, 0), np::axes<1>, np::keepdims);
}

TEST(NpAxisArgextrema, FirstNaNAndEqualValuesIncludingSignedZeros) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    const double input[] = {0.,  -0.,  0.,  -0., 7.,   nan, 8.,  nan,
                            nan, -inf, inf, nan, -inf, inf, inf, -inf};
    const np::tensor_view<const double, np::extents<size_t, 4, 4>> src(input);
    size_t minimum[4], maximum[4];
    np::argmin_to(vector_view<size_t>(minimum, 4), src, np::axes<1>);
    np::argmax_to(vector_view<size_t>(maximum, 4), src, np::axes<1>);
    EXPECT_EQ((std::array{minimum[0], minimum[1], minimum[2], minimum[3]}),
              (std::array<size_t, 4>{0, 1, 0, 0}));
    EXPECT_EQ((std::array{maximum[0], maximum[1], maximum[2], maximum[3]}),
              (std::array<size_t, 4>{0, 1, 0, 1}));
}

TEST(NpAxisArgextrema, NativeIntegerPrecisionBooleanAndNarrowIndex) {
    const uint64_t values[] = {UINT64_MAX, UINT64_MAX - 1, UINT64_MAX - 2, UINT64_MAX - 1};
    int minimum, maximum;
    np::argmin_to(np::tensor_view<int, np::extents<size_t>>(&minimum),
                  vector_view<const uint64_t>(values, 4), np::axes<0>);
    np::argmax_to(np::tensor_view<int, np::extents<size_t>>(&maximum),
                  vector_view<const uint64_t>(values, 4), np::axes<0>);
    EXPECT_EQ(minimum, 2);
    EXPECT_EQ(maximum, 0);
    const bool flags[] = {true, true, false, false};
    np::argmin_to(np::tensor_view<int, np::extents<size_t>>(&minimum),
                  vector_view<const bool>(flags, 4), np::axes<0>);
    np::argmax_to(np::tensor_view<int, np::extents<size_t>>(&maximum),
                  vector_view<const bool>(flags, 4), np::axes<0>);
    EXPECT_EQ(minimum, 2);
    EXPECT_EQ(maximum, 0);
    std::array<unsigned, 256> wide_axis{};
    wide_axis.back() = 1;
    uint8_t result;
    np::argmax_to(np::tensor_view<uint8_t, np::extents<size_t>>(&result),
                  vector_view<const unsigned>(wide_axis.data(), wide_axis.size()), np::axes<0>);
    EXPECT_EQ(result, 255);
    char char_result;
    np::argmax_to(np::tensor_view<char, np::extents<size_t>>(&char_result),
                  vector_view<const bool>(flags, 4), np::axes<0>);
    EXPECT_EQ(char_result, 0);
}

template <size_t Axis, class Layout>
void check_rank_three() {
    using shape = np::extents<size_t, 2, 3, 4>;
    std::array<double, 96> storage;
    storage.fill(-99.);
    const auto mapping = [] {
        if constexpr (std::same_as<Layout, np::layout_stride>)
            return
                typename Layout::template mapping<shape>(shape{}, std::array<size_t, 3>{50, 13, 2});
        else
            return typename Layout::template mapping<shape>{};
    }();
    const np::tensor_view<double, shape, Layout> src(storage.data(), mapping);
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 3; ++j)
            for (size_t k = 0; k != 4; ++k)
                src(i, j, k) = static_cast<double>((i * 17 + j * 7 + k * 13) % 11) - 5.;
    src(0, 1, 2) = std::numeric_limits<double>::quiet_NaN();
    src(1, 1, 1) = std::numeric_limits<double>::quiet_NaN();
    const auto original = storage;
    std::array<size_t, 2> dimensions;
    for (size_t i = 0, out = 0; i != 3; ++i)
        if (i != Axis)
            dimensions[out++] = src.extent(i);
    using out_shape = np::dextents<size_t, 2>;
    const np::layout_stride::mapping<out_shape> output_mapping(out_shape(dimensions),
                                                               std::array<size_t, 2>{11, 2});
    std::array<unsigned, 64> minimum, maximum;
    minimum.fill(99);
    maximum.fill(99);
    np::tensor_view<unsigned, out_shape, np::layout_stride> mins(minimum.data(), output_mapping),
        maxs(maximum.data(), output_mapping);
    np::argmin_to(mins, src, np::axes<Axis>);
    np::argmax_to(maxs, src, np::axes<Axis>);
    std::array<size_t, 3> kept_dimensions{2, 3, 4};
    kept_dimensions[Axis] = 1;
    std::array<unsigned, 12> kept;
    np::tensor_view<unsigned, np::dextents<size_t, 3>, np::layout_left> kept_view(kept.data(),
                                                                                  kept_dimensions);
    np::argmax_to(kept_view, src, np::axes<Axis>, np::keepdims);
    std::array<bool, 64> written{};
    for (size_t a = 0; a != dimensions[0]; ++a)
        for (size_t b = 0; b != dimensions[1]; ++b) {
            std::array<size_t, 3> c{};
            for (size_t axis = 0, out = 0; axis != 3; ++axis)
                if (axis != Axis)
                    c[axis] = out++ == 0 ? a : b;
            size_t lo = 0, hi = 0;
            double low = src(c[0], c[1], c[2]), high = low;
            for (size_t i = 1; i != src.extent(Axis); ++i) {
                if (low != low)
                    break;
                c[Axis] = i;
                const auto value = src(c[0], c[1], c[2]);
                if (value != value) {
                    lo = hi = i;
                    break;
                }
                if (value < low) {
                    low = value;
                    lo = i;
                }
                if (high < value) {
                    high = value;
                    hi = i;
                }
            }
            EXPECT_EQ(mins(a, b), lo);
            EXPECT_EQ(maxs(a, b), hi);
            c[Axis] = 0;
            EXPECT_EQ(kept_view(c[0], c[1], c[2]), hi);
            written[output_mapping(a, b)] = true;
        }
    for (size_t i = 0; i != written.size(); ++i)
        if (!written[i]) {
            EXPECT_EQ(minimum[i], 99);
            EXPECT_EQ(maximum[i], 99);
        }
    for (size_t i = 0; i != storage.size(); ++i)
        EXPECT_TRUE(storage[i] == original[i] ||
                    (storage[i] != storage[i] && original[i] != original[i]));
}

TEST(NpAxisArgextrema, EveryAxisAndMixedPhysicalLayouts) {
    check_rank_three<0, np::layout_right>();
    check_rank_three<1, np::layout_right>();
    check_rank_three<2, np::layout_right>();
    check_rank_three<0, np::layout_left>();
    check_rank_three<1, np::layout_left>();
    check_rank_three<2, np::layout_left>();
    check_rank_three<0, np::layout_stride>();
    check_rank_three<1, np::layout_stride>();
    check_rank_three<2, np::layout_stride>();
    check_rank_three<0, Kokkos::Experimental::layout_right_padded<8>>();
    check_rank_three<1, Kokkos::Experimental::layout_right_padded<8>>();
    check_rank_three<2, Kokkos::Experimental::layout_right_padded<8>>();
    check_rank_three<0, Kokkos::Experimental::layout_left_padded<8>>();
    check_rank_three<1, Kokkos::Experimental::layout_left_padded<8>>();
    check_rank_three<2, Kokkos::Experimental::layout_left_padded<8>>();
}

TEST(NpAxisArgextrema, RandomLinesMatchScalarAndIndependentOracle) {
    std::mt19937 random(8211);
    constexpr size_t rows = 13, columns = 37;
    std::array<int64_t, rows * columns> data;
    for (auto &value : data)
        value = static_cast<int64_t>(random() % 17) - 8;
    const np::tensor_view<const int64_t, np::extents<size_t, rows, columns>> src(data.data());
    std::array<unsigned, rows> output;
    np::argmin_to(vector_view<unsigned>(output.data(), rows), src, np::axes<1>);
    for (size_t row = 0; row != rows; ++row) {
        const auto *begin = data.data() + row * columns;
        EXPECT_EQ(output[row],
                  static_cast<unsigned>(std::min_element(begin, begin + columns) - begin));
        EXPECT_EQ(output[row], np::argmin(vector_view<const int64_t>(begin, columns)));
    }
}
} // namespace
