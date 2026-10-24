#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/bincount.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;
template <class T>
using strided = np::tensor_view<T, np::dextents<size_t, 1>, np::layout_stride>;

template <class T>
constexpr auto make_strided(T *p, size_t count, size_t step) {
    using shape = np::dextents<size_t, 1>;
    return strided<T>(p,
                      np::layout_stride::mapping<shape>(shape(count), std::array<size_t, 1>{step}));
}

template <class D, class I>
concept countable = requires(D dst, I indices) { np::bincount_to(dst, indices); };
template <class D, class I, class W>
concept weightable =
    requires(D dst, I indices, W weights) { np::bincount_to(dst, indices, weights); };
static_assert(countable<view<uint64_t>, view<const char>>);
static_assert(!countable<view<bool>, view<int>>);
static_assert(!countable<view<const int>, view<int>>);
static_assert(!countable<view<volatile int>, view<int>>);
static_assert(!countable<view<double>, view<int>>);
static_assert(!countable<view<int>, view<bool>>);
static_assert(!countable<view<int>, view<float>>);
static_assert(!countable<view<int>, view<volatile int>>);
static_assert(!countable<view<int>, np::tensor_view<int, np::extents<size_t, 2, 2>>>);
static_assert(weightable<view<float>, view<const uint64_t>, view<const bool>>);
static_assert(!weightable<view<int>, view<int>, view<float>>);
static_assert(!weightable<view<const float>, view<int>, view<float>>);
static_assert(!weightable<view<float>, view<int>, view<volatile float>>);
static_assert(!weightable<view<float>, np::tensor_view<int, np::extents<size_t, 2>>,
                          np::tensor_view<int, np::extents<size_t, 3>>>);

constexpr bool constant_counts() {
    const unsigned char indices[] = {2, 0, 2, 3, 2};
    const int weights[] = {1, 2, 3, -4, 5};
    unsigned counts[4] = {99, 99, 99, 99};
    float sums[4];
    np::bincount_to(view<unsigned>(counts, 4), view<const unsigned char>(indices, 5));
    np::bincount_to(view<float>(sums, 4), view<const unsigned char>(indices, 5),
                    view<const int>(weights, 5));
    return counts[0] == 1 && counts[1] == 0 && counts[2] == 3 && counts[3] == 1 && sums[0] == 2 &&
           sums[1] == 0 && sums[2] == 9 && sums[3] == -4;
}
static_assert(constant_counts());

TEST(NpBincount, ConstantEvaluationAndEmptyInputsResetBins) {
    EXPECT_TRUE(constant_counts());
    np::bincount_to(view<int>(nullptr, 0), view<const int>(nullptr, 0));
    np::bincount_to(view<double>(nullptr, 0), view<const int>(nullptr, 0),
                    view<const float>(nullptr, 0));
    int counts[] = {99, 99, 99};
    np::bincount_to(view<int>(counts, 3), view<const char>(nullptr, 0));
    EXPECT_EQ((std::array{counts[0], counts[1], counts[2]}), (std::array{0, 0, 0}));
    double sums[] = {99, 99};
    np::bincount_to(view<double>(sums, 2), view<const size_t>(nullptr, 0),
                    view<const int>(nullptr, 0));
    EXPECT_EQ(sums[0], 0);
    EXPECT_EQ(sums[1], 0);
    EXPECT_FALSE(std::signbit(sums[0]));
}

TEST(NpBincount, FullWidthIndicesAndNarrowCounts) {
    const uint64_t indices[] = {255, 0, 255};
    std::array<uint8_t, 256> output;
    np::bincount_to(view<uint8_t>(output.data(), output.size()), view<const uint64_t>(indices, 3));
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[255], 2);
    const int all_same[] = {0, 0, 0, 0};
    uint64_t count = 0;
    np::bincount_to(view<uint64_t>(&count, 1), view<const int>(all_same, 4));
    EXPECT_EQ(count, 4);
    const bool weights[] = {true, false, true};
    double sums[256];
    np::bincount_to(view<double>(sums, 256), view<const uint64_t>(indices, 3),
                    view<const bool>(weights, 3));
    EXPECT_EQ(sums[0], 0);
    EXPECT_EQ(sums[255], 2);
}

TEST(NpBincount, StridesPreserveHolesAndAllowDifferentMappings) {
    const int indices[] = {2, -99, 0, -99, 2, -99, 1};
    const int weights[] = {3, -99, -99, 4, -99, -99, 5, -99, -99, 6};
    double output[] = {99, -99, 99, -99, 99, -99};
    const auto input = make_strided(indices, 4, 2);
    const auto values = make_strided(weights, 4, 3);
    const auto dst = make_strided(output, 3, 2);
    np::bincount_to(dst, input, values);
    EXPECT_EQ(output[0], 4);
    EXPECT_EQ(output[2], 6);
    EXPECT_EQ(output[4], 8);
    for (size_t i : {1, 3, 5})
        EXPECT_EQ(output[i], -99);
    int counts[] = {99, -99, 99, -99, 99};
    np::bincount_to(make_strided(counts, 3, 2), input);
    EXPECT_EQ(counts[0], 1);
    EXPECT_EQ(counts[2], 1);
    EXPECT_EQ(counts[4], 2);
    EXPECT_EQ(counts[1], -99);
    EXPECT_EQ(counts[3], -99);
}

TEST(NpBincount, OrderedConversionAndExceptionalValues) {
    const int indices[] = {0, 0, 0, 1, 1, 2, 2, 3};
    const double inf = std::numeric_limits<double>::infinity();
    const double weights[] = {16777216., 1.,  -16777216., inf,
                              -inf,      inf, 2.,         std::numeric_limits<double>::quiet_NaN()};
    float sums[5];
    np::bincount_to(view<float>(sums, 5), view<const int>(indices, 8),
                    view<const double>(weights, 8));
    EXPECT_EQ(sums[0], 0); // Conversion and addition both take place in float.
    EXPECT_TRUE(std::isnan(sums[1]));
    EXPECT_EQ(sums[2], std::numeric_limits<float>::infinity());
    EXPECT_TRUE(std::isnan(sums[3]));
    EXPECT_EQ(sums[4], 0);
}

TEST(NpBincount, PaddedMappingAndLargestRepresentableNarrowCount) {
    using shape = np::extents<size_t, 3>;
    using layout = Kokkos::Experimental::layout_right_padded<8>;
    const char indices[] = {2, 0, 2};
    const int weights[] = {4, 2, 6};
    unsigned counts[3];
    double sums[3];
    const np::tensor_view<const char, shape, layout> input(indices);
    np::bincount_to(np::tensor_view<unsigned, shape, layout>(counts), input);
    np::bincount_to(np::tensor_view<double, shape, layout>(sums), input,
                    np::tensor_view<const int, shape, layout>(weights));
    EXPECT_EQ(counts[0], 1);
    EXPECT_EQ(counts[1], 0);
    EXPECT_EQ(counts[2], 2);
    EXPECT_EQ(sums[0], 2);
    EXPECT_EQ(sums[1], 0);
    EXPECT_EQ(sums[2], 10);
    std::array<unsigned char, 255> repeated{};
    uint8_t count;
    np::bincount_to(view<uint8_t>(&count, 1),
                    view<const unsigned char>(repeated.data(), repeated.size()));
    EXPECT_EQ(count, 255);
}

TEST(NpBincount, RandomInputsMatchIndependentScalarOracle) {
    std::mt19937 engine(4027);
    for (size_t bins : {1, 2, 31, 256}) {
        for (size_t size : {0, 1, 17, 1024}) {
            std::vector<uint32_t> indices(size);
            std::vector<int> weights(size);
            std::vector<uint64_t> expected(bins), counts(bins, 99);
            std::vector<double> sums(bins, 99), reference(bins);
            for (size_t i = 0; i != size; ++i) {
                indices[i] = engine() % bins;
                weights[i] = static_cast<int>(engine() % 101) - 50;
                ++expected[indices[i]];
                reference[indices[i]] += weights[i];
            }
            np::bincount_to(view<uint64_t>(counts.data(), bins),
                            view<const uint32_t>(indices.data(), size));
            np::bincount_to(view<double>(sums.data(), bins),
                            view<const uint32_t>(indices.data(), size),
                            view<const int>(weights.data(), size));
            EXPECT_EQ(counts, expected);
            EXPECT_EQ(sums, reference);
        }
    }
}
} // namespace
