#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/rank.hpp>
#include <wjr/np/slice.hpp>

namespace {
namespace np = wjr::np;
using vector_shape = np::dextents<size_t, 1>;
template <class T>
using vector_view = np::tensor_view<T, vector_shape>;

template <class D, class S>
concept can_rank =
    requires(D dst, S src, std::span<size_t> work) { np::rankdata_to(dst, src, work); };
static_assert(can_rank<vector_view<double>, vector_view<const int>>);
static_assert(!can_rank<vector_view<int>, vector_view<const int>>);
static_assert(!can_rank<vector_view<const double>, vector_view<double>>);
static_assert(!can_rank<vector_view<double>, vector_view<volatile double>>);
static_assert(!can_rank<np::tensor_view<double, np::extents<size_t, 3>>,
                        np::tensor_view<int, np::extents<size_t, 4>>>);
static_assert(noexcept(np::rankdata_to(std::declval<vector_view<double>>(),
                                       std::declval<vector_view<double>>(),
                                       std::declval<std::span<size_t>>())));

// Independent quadratic reference, deliberately not a sorting implementation.
template <np::rank_method Method, class T>
double reference(const std::vector<T> &values, size_t index) {
    if (values[index] != values[index])
        return std::numeric_limits<double>::quiet_NaN();
    size_t less = 0, equal = 0, earlier = 0, distinct = 0;
    for (size_t j = 0; j != values.size(); ++j) {
        if (values[j] < values[index]) {
            ++less;
            bool first = true;
            for (size_t k = 0; k != j; ++k)
                if (values[k] == values[j])
                    first = false;
            distinct += first;
        }
        if (values[j] == values[index]) {
            ++equal;
            earlier += j < index;
        }
    }
    if constexpr (Method == np::rank_method::average)
        return less + 1.0 + (equal - 1) / 2.0;
    else if constexpr (Method == np::rank_method::min)
        return less + 1;
    else if constexpr (Method == np::rank_method::max)
        return less + equal;
    else if constexpr (Method == np::rank_method::dense)
        return distinct + 1;
    else
        return less + earlier + 1;
}

template <np::rank_method Method, class T>
void check(const std::vector<T> &values) {
    std::vector<double> result(values.size());
    std::vector<size_t> work(values.size() + 1, 999);
    np::rankdata_to<Method, np::rank_nan_policy::omit>(
        vector_view<double>(result.data(), result.size()),
        vector_view<const T>(values.data(), values.size()),
        std::span<size_t>(work.data(), values.size()));
    for (size_t i = 0; i != values.size(); ++i) {
        const auto expected = reference<Method>(values, i);
        if (std::isnan(expected))
            EXPECT_TRUE(std::isnan(result[i])) << i;
        else
            EXPECT_EQ(result[i], expected) << i;
    }
    EXPECT_EQ(work.back(), 999);
}

template <class T>
void check_methods(const std::vector<T> &values) {
    check<np::rank_method::average>(values);
    check<np::rank_method::min>(values);
    check<np::rank_method::max>(values);
    check<np::rank_method::dense>(values);
    check<np::rank_method::ordinal>(values);
}

TEST(NpRank, TiesAndExactIntegerComparison) {
    check_methods(std::vector<int>{0, 2, 3, 2});
    check_methods(std::vector<int>{4, 4, 4, 4});
    check_methods(std::vector<int64_t>{INT64_MAX, INT64_MAX - 1, INT64_MIN, INT64_MIN + 1});
    check_methods(std::vector<uint64_t>{UINT64_MAX, UINT64_MAX - 1, 0, 1});
}

TEST(NpRank, NonfiniteAndSignedZero) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    check_methods(std::vector<double>{nan, 0., -0., inf, -inf, nan, 3., inf});
    check_methods(std::vector<double>{nan, nan});
    const double input[] = {2, nan, 1};
    double output[3];
    size_t work[3];
    np::rankdata_to(vector_view<double>(output, 3), vector_view<const double>(input, 3), work);
    for (const auto value : output)
        EXPECT_TRUE(std::isnan(value));
}

TEST(NpRank, EmptySingletonAndBool) {
    np::rankdata_to(vector_view<double>(nullptr, 0), vector_view<const double>(nullptr, 0), {});
    check_methods(std::vector<double>{7});
    const bool input[] = {true, false, true, false};
    double output[4];
    size_t work[4];
    np::rankdata_to(vector_view<double>(output, 4), vector_view<const bool>(input, 4), work);
    EXPECT_EQ(output[0], 3.5);
    EXPECT_EQ(output[1], 1.5);
    EXPECT_EQ(output[2], 3.5);
    EXPECT_EQ(output[3], 1.5);
}

TEST(NpRank, RandomAndOrderedInputs) {
    std::mt19937 engine(428);
    for (const size_t size : {0, 1, 7, 16, 17, 64, 129, 300}) {
        std::vector<double> values(size);
        for (auto &value : values)
            value = static_cast<int>(engine() % 31) - 15;
        check_methods(values);
        std::sort(values.begin(), values.end());
        check_methods(values);
        std::reverse(values.begin(), values.end());
        check_methods(values);
    }
}

TEST(NpRank, AxesAndMixedLayouts) {
    using shape = np::extents<size_t, 2, 3>;
    const int input[] = {0, 2, 2, 3, 2, 5};
    double output[6];
    size_t work[3];
    const np::tensor_view<const int, shape> source(input);
    const np::tensor_view<double, shape, np::layout_left> destination(output);
    np::rankdata_to(destination, source, work, np::axes<1>);
    EXPECT_EQ(destination(0, 0), 1);
    EXPECT_EQ(destination(0, 1), 2.5);
    EXPECT_EQ(destination(0, 2), 2.5);
    EXPECT_EQ(destination(1, 0), 2);
    EXPECT_EQ(destination(1, 1), 1);
    EXPECT_EQ(destination(1, 2), 3);
    np::rankdata_to(destination, source, work, np::axes<0>);
    EXPECT_EQ(destination(0, 0), 1);
    EXPECT_EQ(destination(1, 0), 2);
    EXPECT_EQ(destination(0, 1), 1.5);
    EXPECT_EQ(destination(1, 1), 1.5);
    EXPECT_EQ(destination(0, 2), 1);
    EXPECT_EQ(destination(1, 2), 2);
}

TEST(NpRank, StridedLinesPreservePaddingAndReuseWorkspace) {
    using shape = np::extents<size_t, 2, 3>;
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const double input[] = {nan, 999, 2, 999, 1, 999, 999, 999, 3, 999, 2, 999, 5};
    std::array<float, 17> output;
    output.fill(-999);
    size_t work[3];
    const np::layout_stride::mapping<shape> input_mapping(shape{}, std::array<size_t, 2>{8, 2});
    const np::layout_stride::mapping<shape> output_mapping(shape{}, std::array<size_t, 2>{10, 3});
    const np::tensor_view<const double, shape, np::layout_stride> source(input, input_mapping);
    const np::tensor_view<float, shape, np::layout_stride> destination(output.data(),
                                                                       output_mapping);
    np::rankdata_to<np::rank_method::average, np::rank_nan_policy::omit>(destination, source, work,
                                                                         np::axes<1>);
    EXPECT_TRUE(std::isnan(output[0]));
    EXPECT_EQ(output[3], 2);
    EXPECT_EQ(output[6], 1);
    EXPECT_EQ(output[10], 2);
    EXPECT_EQ(output[13], 1);
    EXPECT_EQ(output[16], 3);
    for (size_t i = 0; i != output.size(); ++i)
        if (i != 0 && i != 3 && i != 6 && i != 10 && i != 13 && i != 16)
            EXPECT_EQ(output[i], -999);
    np::rankdata_to(destination, source, work, np::axes<1>);
    EXPECT_TRUE(std::isnan(output[0]) && std::isnan(output[3]) && std::isnan(output[6]));
    EXPECT_EQ(output[10], 2);
}

TEST(NpRank, RankThreeAndZeroExtent) {
    using shape = np::extents<size_t, 2, 2, 2>;
    const int input[] = {4, 3, 2, 1, 1, 2, 3, 4};
    double output[8];
    size_t work[2];
    np::rankdata_to(np::tensor_view<double, shape>(output),
                    np::tensor_view<const int, shape>(input), work, np::axes<1>);
    const double expected[] = {2, 2, 1, 1, 1, 1, 2, 2};
    EXPECT_TRUE(std::equal(std::begin(output), std::end(output), std::begin(expected)));
    using empty_shape = np::extents<size_t, 0, 3>;
    np::rankdata_to(np::tensor_view<double, empty_shape>(nullptr),
                    np::tensor_view<const int, empty_shape>(nullptr), {}, np::axes<1>);
}

template <class Layout>
void check_padded_slice() {
    std::array<int, 24> input;
    input.fill(-999);
    std::array<double, 24> output;
    output.fill(-999);
    using shape = np::extents<size_t, 4, 6>;
    auto source = np::slice_view(np::tensor_view<int, shape, Layout>(input.data()), std::pair{1, 3},
                                 std::pair{1, 4});
    auto destination = np::slice_view(np::tensor_view<double, shape, Layout>(output.data()),
                                      std::pair{1, 3}, std::pair{1, 4});
    static_assert(np::detail::padded_layout<typename decltype(source)::layout_type>);
    source(0, 0) = 3;
    source(0, 1) = 1;
    source(0, 2) = 2;
    source(1, 0) = 2;
    source(1, 1) = 2;
    source(1, 2) = 1;
    size_t work[3];
    np::rankdata_to(destination, source, work, np::axes<1>);
    EXPECT_EQ(destination(0, 0), 3);
    EXPECT_EQ(destination(0, 1), 1);
    EXPECT_EQ(destination(0, 2), 2);
    EXPECT_EQ(destination(1, 0), 2.5);
    EXPECT_EQ(destination(1, 1), 2.5);
    EXPECT_EQ(destination(1, 2), 1);
    size_t changed = 0;
    for (const auto value : output)
        changed += value != -999;
    EXPECT_EQ(changed, 6);
}

TEST(NpRank, PaddedSlices) {
    check_padded_slice<np::layout_right>();
    check_padded_slice<np::layout_left>();
}
} // namespace
