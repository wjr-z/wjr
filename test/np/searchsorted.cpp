#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/searchsorted.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using vector_view = np::tensor_view<T, np::dextents<size_t, 1>>;

template <class D, class S, class Q>
concept searchable =
    requires(D dst, S sorted, Q queries) { np::searchsorted_to(dst, sorted, queries); };
template <class D, class S, class Q>
concept batch_searchable =
    requires(D dst, S sorted, Q queries) { np::searchsorted_batch_to(dst, sorted, queries); };
static_assert(!batch_searchable<np::tensor_view<int, np::extents<size_t>>, vector_view<int>,
                                np::tensor_view<int, np::extents<size_t>>>);
static_assert(!batch_searchable<np::tensor_view<int, np::extents<size_t, 2, 3>>, vector_view<int>,
                                np::tensor_view<int, np::extents<size_t, 2, 3>>>);
static_assert(
    batch_searchable<vector_view<int8_t>, vector_view<const double>, vector_view<double>>);
static_assert(!batch_searchable<vector_view<bool>, vector_view<int>, vector_view<int>>);
static_assert(!batch_searchable<vector_view<const int>, vector_view<int>, vector_view<int>>);
static_assert(!batch_searchable<vector_view<volatile int>, vector_view<int>, vector_view<int>>);
static_assert(!batch_searchable<vector_view<int>, vector_view<volatile int>, vector_view<int>>);
static_assert(!batch_searchable<vector_view<int>, vector_view<int>, vector_view<unsigned>>);
static_assert(!batch_searchable<np::tensor_view<int, np::extents<size_t, 2>>, vector_view<int>,
                                np::tensor_view<int, np::extents<size_t, 3>>>);
static_assert(searchable<vector_view<int>, vector_view<const double>, vector_view<double>>);
static_assert(!searchable<vector_view<bool>, vector_view<int>, vector_view<int>>);
static_assert(!searchable<vector_view<const int>, vector_view<int>, vector_view<int>>);
static_assert(!searchable<vector_view<volatile int>, vector_view<int>, vector_view<int>>);
static_assert(!searchable<vector_view<int>, vector_view<volatile int>, vector_view<int>>);
static_assert(!searchable<vector_view<int>, vector_view<int>, vector_view<volatile int>>);
static_assert(!searchable<vector_view<int>, vector_view<int>, vector_view<unsigned>>);
static_assert(!searchable<vector_view<float>, vector_view<int>, vector_view<int>>);
static_assert(!searchable<vector_view<int>, np::tensor_view<int, np::extents<size_t, 2, 2>>,
                          vector_view<int>>);
static_assert(!searchable<np::tensor_view<int, np::extents<size_t, 2>>, vector_view<int>,
                          np::tensor_view<int, np::extents<size_t, 3>>>);

constexpr bool constant_search() {
    const int sorted[] = {1, 3, 3, 7};
    const int queries[] = {-1, 3, 5, 9};
    int left[4], right[4];
    const vector_view<const int> source(sorted, 4), values(queries, 4);
    np::searchsorted_to(vector_view<int>(left, 4), source, values);
    np::searchsorted_to<np::search_side::right>(vector_view<int>(right, 4), source, values);
    return left[0] == 0 && left[1] == 1 && left[2] == 3 && left[3] == 4 && right[0] == 0 &&
           right[1] == 3 && right[2] == 3 && right[3] == 4;
}
static_assert(constant_search());

constexpr bool constant_batch_search() {
    const int source[] = {1, 3, 3, 7};
    const int queries[] = {-1, 3, 5, 9};
    int left[4], right[4];
    np::searchsorted_batch_to(vector_view<int>(left, 4), vector_view<const int>(source, 4),
                              vector_view<const int>(queries, 4));
    np::searchsorted_batch_to<np::search_side::right>(vector_view<int>(right, 4),
                                                      vector_view<const int>(source, 4),
                                                      vector_view<const int>(queries, 4));
    return left[0] == 0 && left[1] == 1 && left[2] == 3 && left[3] == 4 && right[0] == 0 &&
           right[1] == 3 && right[2] == 3 && right[3] == 4;
}
static_assert(constant_batch_search());

TEST(NpSearchsortedBatch, EverySmallLengthAndSpecialValues) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    const double queries[] = {-inf, -1, nan, inf, -0., 0., 1, 2, 3, 99};
    std::array<int16_t, 10> expected, actual;
    std::vector<double> sorted;
    for (size_t count = 0; count != 258; ++count) {
        sorted.resize(count);
        for (size_t i = 0; i != count; ++i)
            sorted[i] = i + 2 >= count   ? nan
                        : i == 0         ? -inf
                        : i <= 2         ? 0.
                        : i + 3 == count ? inf
                                         : static_cast<double>(i / 2);
        const vector_view<const double> source(sorted.data(), count), values(queries, 10);
        const vector_view<int16_t> dst(actual.data(), 10), reference(expected.data(), 10);
        np::searchsorted_to(reference, source, values);
        np::searchsorted_batch_to(dst, source, values);
        EXPECT_EQ(actual, expected) << count;
        np::searchsorted_to<np::search_side::right>(reference, source, values);
        np::searchsorted_batch_to<np::search_side::right>(dst, source, values);
        EXPECT_EQ(actual, expected) << count;
    }
    EXPECT_TRUE(constant_batch_search());
}

TEST(NpSearchsortedBatch, StridedInputsAndNarrowOutputPreserveHoles) {
    const int sorted[] = {1, -999, 3, -999, 3, -999, 7};
    std::array<int, 49> values;
    values.fill(-99);
    for (size_t i = 0; i != 17; ++i)
        values[i * 3] = static_cast<int>(i % 9) - 1;
    std::array<int8_t, 33> output;
    output.fill(-99);
    using source_shape = np::extents<size_t, 4>;
    using query_shape = np::extents<size_t, 17>;
    const np::layout_stride::mapping<source_shape> imap(source_shape{}, std::array<size_t, 1>{2});
    const np::layout_stride::mapping<query_shape> qmap(query_shape{}, std::array<size_t, 1>{3}),
        omap(query_shape{}, std::array<size_t, 1>{2});
    const np::tensor_view<const int, source_shape, np::layout_stride> source(sorted, imap);
    const np::tensor_view<const int, query_shape, np::layout_stride> queries(values.data(), qmap);
    const np::tensor_view<int8_t, query_shape, np::layout_stride> dst(output.data(), omap);
    np::searchsorted_batch_to(dst, source, queries);
    for (size_t i = 0; i != 17; ++i) {
        const auto value = values[i * 3];
        EXPECT_EQ(output[i * 2], (value > 1) + (value > 3) * 2 + (value > 7));
    }
    for (size_t i = 1; i < output.size(); i += 2)
        EXPECT_EQ(output[i], -99);
}

TEST(NpSearchsortedBatch, EmptyAndBoolean) {
    np::searchsorted_batch_to(vector_view<int>(nullptr, 0), vector_view<const int>(nullptr, 0),
                              vector_view<const int>(nullptr, 0));
    const bool sorted[] = {false, false, true};
    const bool queries[] = {true, false};
    uint8_t output[2];
    const vector_view<const bool> values(queries, 2);
    const vector_view<uint8_t> dst(output, 2);
    np::searchsorted_batch_to<np::search_side::right>(dst, vector_view<const bool>(sorted, 3),
                                                      values);
    EXPECT_EQ(output[0], 3);
    EXPECT_EQ(output[1], 2);
    np::searchsorted_batch_to(dst, vector_view<const bool>(nullptr, 0), values);
    EXPECT_EQ(output[0], 0);
    EXPECT_EQ(output[1], 0);
    bool many_queries[17];
    uint8_t many_output[17];
    for (size_t i = 0; i != 17; ++i)
        many_queries[i] = i % 2 != 0;
    np::searchsorted_batch_to<np::search_side::right>(vector_view<uint8_t>(many_output, 17),
                                                      vector_view<const bool>(sorted, 3),
                                                      vector_view<const bool>(many_queries, 17));
    for (size_t i = 0; i != 17; ++i)
        EXPECT_EQ(many_output[i], many_queries[i] ? 3 : 2);
}

TEST(NpSearchsorted, BoundariesDuplicatesAndConstantEvaluation) {
    EXPECT_TRUE(constant_search());
    const uint64_t sorted[] = {0, UINT64_MAX - 1, UINT64_MAX};
    const uint64_t queries[] = {0, 1, UINT64_MAX};
    int32_t output[3];
    np::searchsorted_to(vector_view<int32_t>(output, 3), vector_view<const uint64_t>(sorted, 3),
                        vector_view<const uint64_t>(queries, 3));
    EXPECT_EQ(output[0], 0);
    EXPECT_EQ(output[1], 1);
    EXPECT_EQ(output[2], 2);
}

TEST(NpSearchsorted, NaNsInfinitiesAndSignedZeros) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    const double sorted[] = {-inf, -0., 0., 2, inf, nan, nan};
    const double queries[] = {-inf, -0., 0., 1, inf, nan};
    std::array<size_t, 6> output;
    const vector_view<const double> source(sorted, 7), values(queries, 6);
    const vector_view<size_t> dst(output.data(), output.size());
    np::searchsorted_to(dst, source, values);
    EXPECT_EQ(output, (std::array<size_t, 6>{0, 1, 1, 3, 4, 5}));
    np::searchsorted_to<np::search_side::right>(dst, source, values);
    EXPECT_EQ(output, (std::array<size_t, 6>{1, 3, 3, 3, 5, 7}));
    const double finite[] = {1, 2};
    np::searchsorted_to(dst, vector_view<const double>(finite, 2), values);
    EXPECT_EQ(output.back(), 2);
    const double missing[] = {nan, nan};
    np::searchsorted_to(dst, vector_view<const double>(missing, 2), values);
    EXPECT_EQ(output, (std::array<size_t, 6>{0, 0, 0, 0, 0, 0}));
    np::searchsorted_to<np::search_side::right>(dst, vector_view<const double>(missing, 2), values);
    EXPECT_EQ(output.back(), 2);
    EXPECT_EQ(output[4], 0);
}

TEST(NpSearchsorted, EmptySourceEmptyOutputAndScalarQuery) {
    const int values[] = {-1, 0, 5};
    int output[] = {-1, -1, -1};
    np::searchsorted_to(vector_view<int>(output, 3), vector_view<const int>(nullptr, 0),
                        vector_view<const int>(values, 3));
    for (int value : output)
        EXPECT_EQ(value, 0);
    np::searchsorted_to(vector_view<int>(nullptr, 0), vector_view<const int>(nullptr, 0),
                        vector_view<const int>(nullptr, 0));
    int result = -1;
    const int query = 0;
    using scalar = np::extents<size_t>;
    np::searchsorted_to(np::tensor_view<int, scalar>(&result), vector_view<const int>(values, 3),
                        np::tensor_view<const int, scalar>(&query));
    EXPECT_EQ(result, 1);
}

TEST(NpSearchsorted, StridedSourceAndMixedQueryLayoutsPreserveHoles) {
    const int sorted[] = {1, -999, 3, -999, 3, -999, 7};
    const int values[] = {0, 3, 8, 3, 7, 2};
    std::array<int, 11> output;
    output.fill(-999);
    using source_shape = np::extents<size_t, 4>;
    using matrix_shape = np::extents<size_t, 2, 3>;
    const np::layout_stride::mapping<source_shape> imap(source_shape{}, std::array<size_t, 1>{2});
    const np::layout_stride::mapping<matrix_shape> omap(matrix_shape{},
                                                        std::array<size_t, 2>{6, 2});
    const np::tensor_view<const int, source_shape, np::layout_stride> source(sorted, imap);
    const np::tensor_view<const int, matrix_shape, np::layout_left> queries(values);
    const np::tensor_view<int, matrix_shape, np::layout_stride> dst(output.data(), omap);
    np::searchsorted_to(dst, source, queries);
    for (size_t row = 0; row != 2; ++row)
        for (size_t column = 0; column != 3; ++column) {
            const int value = values[row + column * 2];
            const int expected = (value > 1) + (value > 3) * 2 + (value > 7);
            EXPECT_EQ(output[row * 6 + column * 2], expected);
        }
    for (size_t i = 1; i < output.size(); i += 2)
        EXPECT_EQ(output[i], -999);
}

TEST(NpSearchsorted, BooleanSourceUsesActualBooleanStorage) {
    const bool sorted[] = {false, false, true};
    const bool queries[] = {true, false};
    size_t output[2];
    np::searchsorted_to<np::search_side::right>(vector_view<size_t>(output, 2),
                                                vector_view<const bool>(sorted, 3),
                                                vector_view<const bool>(queries, 2));
    EXPECT_EQ(output[0], 3);
    EXPECT_EQ(output[1], 2);
}

TEST(NpSearchsorted, RandomDataAgainstStandardBounds) {
    std::mt19937 engine(741);
    std::vector<int> sorted(1024), values(300);
    for (auto &value : sorted)
        value = static_cast<int>(engine() % 300) - 150;
    for (auto &value : values)
        value = static_cast<int>(engine() % 400) - 200;
    std::sort(sorted.begin(), sorted.end());
    std::vector<size_t> output(values.size());
    const vector_view<const int> source(sorted.data(), sorted.size()),
        queries(values.data(), values.size());
    const vector_view<size_t> dst(output.data(), output.size());
    np::searchsorted_to(dst, source, queries);
    for (size_t i = 0; i != values.size(); ++i)
        EXPECT_EQ(output[i],
                  std::lower_bound(sorted.begin(), sorted.end(), values[i]) - sorted.begin());
    np::searchsorted_to<np::search_side::right>(dst, source, queries);
    for (size_t i = 0; i != values.size(); ++i)
        EXPECT_EQ(output[i],
                  std::upper_bound(sorted.begin(), sorted.end(), values[i]) - sorted.begin());
    np::searchsorted_batch_to(dst, source, queries);
    for (size_t i = 0; i != values.size(); ++i)
        EXPECT_EQ(output[i],
                  std::lower_bound(sorted.begin(), sorted.end(), values[i]) - sorted.begin());
    np::searchsorted_batch_to<np::search_side::right>(dst, source, queries);
    for (size_t i = 0; i != values.size(); ++i)
        EXPECT_EQ(output[i],
                  std::upper_bound(sorted.begin(), sorted.end(), values[i]) - sorted.begin());
}
} // namespace
