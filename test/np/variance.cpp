#include <array>
#include <cmath>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/variance.hpp>

namespace {
namespace np = wjr::np;
using shape = np::extents<size_t, 4>;
using input_view = np::tensor_view<const int, shape>;
template <class V, class A = void>
concept has_variance = requires(V v) { np::variance<A>(v); };
static_assert(has_variance<input_view> && has_variance<input_view, float>);
static_assert(!has_variance<input_view, int> && !has_variance<input_view, const double>);
static_assert(!has_variance<np::tensor_view<volatile int, shape>>);
static_assert(std::same_as<decltype(np::variance(std::declval<input_view>())), double>);
static_assert(std::same_as<
              decltype(np::variance(std::declval<np::tensor_view<const float, shape>>())), float>);
static_assert(noexcept(np::variance(std::declval<input_view>())));

constexpr bool constant_variance() {
    const int input[] = {1, 2, 3, 4};
    input_view src(input);
    return np::variance(src) == 1.25 && np::variance(src, 1) == 5.0 / 3.0;
}
static_assert(constant_variance());

template <class Acc, class View>
Acc ordered_reference(View src, size_t ddof = 0) {
    const auto count = static_cast<size_t>(src.size());
    Acc center = 0;
    for (size_t row = 0; row != static_cast<size_t>(src.extent(0)); ++row)
        for (size_t col = 0; col != static_cast<size_t>(src.extent(1)); ++col)
            center += static_cast<Acc>(src(row, col));
    center /= static_cast<Acc>(count);
    Acc squares = 0;
    for (size_t row = 0; row != static_cast<size_t>(src.extent(0)); ++row)
        for (size_t col = 0; col != static_cast<size_t>(src.extent(1)); ++col) {
            const Acc delta = static_cast<Acc>(src(row, col)) - center;
            squares += delta * delta;
        }
    return squares / static_cast<Acc>(count - ddof);
}

TEST(np_variance, PopulationAndSample) {
    EXPECT_TRUE(constant_variance());
    const bool flags[] = {true, false, true, false};
    EXPECT_EQ(np::variance(np::tensor_view<const bool, shape>(flags)), 0.25);
    const double shifted[] = {1000000001, 1000000002, 1000000003, 1000000004};
    EXPECT_EQ(np::variance(np::tensor_view<const double, shape>(shifted)), 1.25);
}

TEST(np_variance, EmptyScalarAndCorrection) {
    const np::tensor_view<const double, np::dextents<size_t, 1>> empty(nullptr, 0);
    EXPECT_TRUE(std::isnan(np::variance(empty)));
    EXPECT_TRUE(std::isnan(np::variance(empty, 1)));
    const double value = 7;
    const np::tensor_view<const double, np::extents<size_t>> scalar(&value);
    EXPECT_EQ(np::variance(scalar), 0);
    EXPECT_TRUE(std::isnan(np::variance(scalar, 1)));
    const int input[] = {1, 2, 3, 4};
    EXPECT_TRUE(std::isnan(np::variance(input_view(input), 4)));
    EXPECT_TRUE(std::isnan(np::variance(input_view(input), std::numeric_limits<size_t>::max())));
}

TEST(np_variance, PackedLayoutsAndAccumulator) {
    const float input[] = {16777216, 1, -16777216, 2, 3, 4};
    using extents = np::extents<size_t, 2, 3>;
    const np::tensor_view<const float, extents> right(input);
    const np::tensor_view<const float, extents, np::layout_left> left(input);
    EXPECT_EQ(np::variance(right), ordered_reference<float>(right));
    EXPECT_EQ(np::variance(left), ordered_reference<float>(left));
    EXPECT_EQ(np::variance<double>(right), ordered_reference<double>(right));
    EXPECT_EQ(np::variance<double>(left, 1), ordered_reference<double>(left, 1));
    const int large[] = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
    EXPECT_EQ(np::variance(np::tensor_view<const int, np::extents<size_t, 2>>(large)), 0);
}

TEST(np_variance, StridesAndPadding) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double input[] = {1, nan, 2, nan, 3, nan, nan, nan, 4, nan, 5, nan, 6};
    using extents = np::extents<size_t, 2, 3>;
    const np::layout_stride::mapping<extents> mapping(extents{}, std::array<size_t, 2>{8, 2});
    const np::tensor_view<const double, extents, np::layout_stride> view(input, mapping);
    EXPECT_EQ(np::variance(view), 35.0 / 12.0);
    EXPECT_EQ(np::variance(view, 1), 3.5);
    using vector_extents = np::extents<size_t, 3>;
    const np::layout_stride::mapping<vector_extents> stride(vector_extents{},
                                                            std::array<size_t, 1>{2});
    const np::tensor_view<const double, vector_extents, np::layout_stride> vector(input, stride);
    EXPECT_EQ(np::variance(vector), 2.0 / 3.0);
}

TEST(np_variance, NonfiniteAndSignedZero) {
    double input[] = {0, -0.0, 0, -0.0};
    const np::tensor_view<double, shape> view(input);
    EXPECT_EQ(np::variance(view), 0);
    EXPECT_FALSE(std::signbit(np::variance(view)));
    input[0] = std::numeric_limits<double>::infinity();
    EXPECT_TRUE(std::isnan(np::variance(view)));
    input[0] = std::numeric_limits<double>::quiet_NaN();
    EXPECT_TRUE(std::isnan(np::variance(view)));
}
} // namespace
