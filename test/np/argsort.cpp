#include <array>
#include <cmath>
#include <numeric>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/argsort.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;

template <class D, class S>
concept sortable = requires(D dst, S src) { np::argsort_to(dst, src); };
static_assert(sortable<view<int>, view<const double>>);
static_assert(!sortable<view<const int>, view<double>>);
static_assert(!sortable<view<volatile int>, view<double>>);
static_assert(!sortable<view<bool>, view<double>>);
static_assert(!sortable<view<double>, view<int>>);
static_assert(!sortable<view<int>, view<volatile int>>);
static_assert(!sortable<np::tensor_view<int, np::extents<size_t, 3>>,
                        np::tensor_view<int, np::extents<size_t, 4>>>);

constexpr bool constant_sort() {
    const int source[] = {4, 1, 1, 3};
    unsigned output[4]{};
    np::argsort_to<np::sort_order::ascending, np::sort_stability::stable>(
        view<unsigned>(output, 4), view<const int>(source, 4));
    return output[0] == 1 && output[1] == 2 && output[2] == 3 && output[3] == 0;
}
static_assert(constant_sort());

TEST(NpArgsort, EmptySingletonAndConstantEvaluation) {
    EXPECT_TRUE(constant_sort());
    np::argsort_to(view<int>(nullptr, 0), view<const double>(nullptr, 0));
    int output = -1;
    const bool source = true;
    np::argsort_to(view<int>(&output, 1), view<const bool>(&source, 1));
    EXPECT_EQ(output, 0);
}

template <np::sort_order Order, np::sort_stability Stability, class T>
void compare_reference(const std::vector<T> &source) {
    std::vector<size_t> expected(source.size()), actual(source.size());
    std::iota(expected.begin(), expected.end(), size_t{0});
    const auto less = [&](size_t a, size_t b) {
        const auto left = source[a], right = source[b];
        if constexpr (std::is_floating_point_v<T>) {
            if (std::isnan(left))
                return false;
            if (std::isnan(right))
                return true;
        }
        if constexpr (Order == np::sort_order::ascending)
            return left < right;
        else
            return left > right;
    };
    std::stable_sort(expected.begin(), expected.end(), less);
    np::argsort_to<Order, Stability>(view<size_t>(actual.data(), actual.size()),
                                     view<const T>(source.data(), source.size()));
    if constexpr (Stability == np::sort_stability::stable)
        EXPECT_EQ(actual, expected);
    else {
        EXPECT_TRUE(std::is_sorted(actual.begin(), actual.end(), less));
        std::sort(actual.begin(), actual.end());
        for (size_t i = 0; i != actual.size(); ++i)
            EXPECT_EQ(actual[i], i);
    }
}

TEST(NpArgsort, NaNsInfinitiesZerosAndDuplicateTies) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    const std::vector<double> source{nan, 2, -0., inf, nan, 2, 0., -inf, nan};
    compare_reference<np::sort_order::ascending, np::sort_stability::stable>(source);
    compare_reference<np::sort_order::descending, np::sort_stability::stable>(source);
    compare_reference<np::sort_order::ascending, np::sort_stability::unstable>(source);
    compare_reference<np::sort_order::descending, np::sort_stability::unstable>(source);
}

TEST(NpArgsort, IntegersKeepFullPrecision) {
    constexpr auto max = std::numeric_limits<uint64_t>::max();
    const std::vector<uint64_t> source{max, max - 1, max - 2, 0, max - 1};
    compare_reference<np::sort_order::ascending, np::sort_stability::stable>(source);
    compare_reference<np::sort_order::descending, np::sort_stability::stable>(source);
    const bool bits[] = {true, false, true, false};
    unsigned indices[4];
    np::argsort_to<np::sort_order::ascending, np::sort_stability::stable>(
        view<unsigned>(indices, 4), view<const bool>(bits, 4));
    EXPECT_EQ((std::array{indices[0], indices[1], indices[2], indices[3]}),
              (std::array<unsigned, 4>{1, 3, 0, 2}));
}

TEST(NpArgsort, NarrowIndicesIncludeTheirMaximumValue) {
    std::array<int, 256> source;
    std::array<uint8_t, 256> output;
    for (size_t i = 0; i != source.size(); ++i)
        source[i] = static_cast<int>(source.size() - i);
    np::argsort_to(view<uint8_t>(output.data(), output.size()),
                   view<const int>(source.data(), source.size()));
    for (size_t i = 0; i != output.size(); ++i)
        EXPECT_EQ(output[i], 255 - i);
}

TEST(NpArgsort, StridedSourceStaysUntouched) {
    const int source[] = {7, -99, 1, -99, 7, -99, 2};
    const np::dextents<size_t, 1> shape(4);
    const np::layout_stride::mapping mapping(shape, std::array<size_t, 1>{2});
    np::tensor_view<const int, np::dextents<size_t, 1>, np::layout_stride> input(source, mapping);
    int output[4];
    np::argsort_to<np::sort_order::descending, np::sort_stability::stable>(view<int>(output, 4),
                                                                           input);
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3]}), (std::array{0, 2, 3, 1}));
    EXPECT_EQ(source[1], -99);
}

template <class Layout>
void padded_source() {
    const int source[] = {3, 1, 2};
    using shape = np::extents<size_t, 3>;
    int output[3];
    np::argsort_to(view<int>(output, 3), np::tensor_view<const int, shape, Layout>(source));
    EXPECT_EQ((std::array{output[0], output[1], output[2]}), (std::array{1, 2, 0}));
}

TEST(NpArgsort, RankOnePaddedLayoutsArePacked) {
    padded_source<Kokkos::Experimental::layout_right_padded<8>>();
    padded_source<Kokkos::Experimental::layout_left_padded<8>>();
}

TEST(NpArgsort, RandomAndAdversarialInputs) {
    std::mt19937 engine(713);
    for (size_t count : {0, 1, 2, 17, 256, 1024}) {
        for (int distribution = 0; distribution != 4; ++distribution) {
            std::vector<int> values(count);
            for (size_t i = 0; i != count; ++i)
                values[i] = distribution == 0   ? static_cast<int>(engine() % 31)
                            : distribution == 1 ? static_cast<int>(i)
                            : distribution == 2 ? -static_cast<int>(i)
                                                : 1;
            compare_reference<np::sort_order::ascending, np::sort_stability::stable>(values);
            compare_reference<np::sort_order::descending, np::sort_stability::stable>(values);
            compare_reference<np::sort_order::ascending, np::sort_stability::unstable>(values);
            compare_reference<np::sort_order::descending, np::sort_stability::unstable>(values);
        }
    }
}
} // namespace
