#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/norm.hpp>

namespace {
namespace np = wjr::np;

template <class View, class Acc = void>
concept has_norm = requires(View view) { np::norm_l2<Acc>(view); };

template <class T>
using vector_view = np::tensor_view<T, np::dextents<size_t, 1>>;

struct custom_accessor : np::default_accessor<float> {};
struct object {};
static_assert(has_norm<vector_view<const float>>);
static_assert(has_norm<vector_view<int>, long double>);
static_assert(!has_norm<vector_view<float>, int>);
static_assert(!has_norm<vector_view<float>, const double>);
static_assert(!has_norm<vector_view<volatile double>>);
static_assert(!has_norm<vector_view<object>>);
static_assert(
    !has_norm<np::tensor_view<float, np::extents<size_t, 3>, np::layout_right, custom_accessor>>);
static_assert(!has_norm<np::tensor_view<float, np::extents<size_t, 2, 3>, np::layout_stride>>);
static_assert(std::same_as<decltype(np::norm_l2(std::declval<vector_view<float>>())), float>);
static_assert(std::same_as<decltype(np::norm_l2(std::declval<vector_view<double>>())), double>);
static_assert(
    std::same_as<decltype(np::norm_l2(std::declval<vector_view<long double>>())), long double>);
static_assert(std::same_as<decltype(np::norm_l2(std::declval<vector_view<int>>())), double>);
static_assert(std::same_as<decltype(np::norm_l2(std::declval<vector_view<bool>>())), double>);
static_assert(noexcept(np::norm_l2(std::declval<vector_view<float>>())));

template <class T>
void check_values(const std::vector<T> &values) {
    long double expected = 0;
    for (auto value : values)
        expected = std::hypot(expected, static_cast<long double>(value));
    const auto result = np::norm_l2(vector_view<const T>(values.data(), values.size()));
    const auto rounded = static_cast<T>(expected);
    if (std::isinf(rounded)) {
        EXPECT_EQ(result, rounded);
    } else {
        const auto tolerance = std::abs(rounded) * std::numeric_limits<T>::epsilon() * 16 +
                               std::numeric_limits<T>::denorm_min() * 2;
        EXPECT_NEAR(result, rounded, tolerance);
    }
}

TEST(np_norm, packed_layouts_and_scalar) {
    const double values[] = {0, 3, 0, -4, 0, 0};
    using shape = np::extents<size_t, 2, 3>;
    EXPECT_EQ(np::norm_l2(np::tensor_view<const double, shape>(values)), 5);
    EXPECT_EQ(np::norm_l2(np::tensor_view<const double, shape, np::layout_left>(values)), 5);
    const float scalar = -7;
    EXPECT_EQ(np::norm_l2(np::tensor_view<const float, np::extents<size_t>>(&scalar)), 7);
    EXPECT_EQ(
        np::norm_l2(np::tensor_view<const float, np::extents<size_t>, np::layout_left>(&scalar)),
        7);
    using scalar_shape = np::extents<size_t>;
    EXPECT_EQ(np::norm_l2(np::tensor_view<const float, scalar_shape, np::layout_stride>(
                  &scalar, np::layout_stride::mapping<scalar_shape>(scalar_shape{},
                                                                    std::array<size_t, 0>{}))),
              7);
}

TEST(np_norm, empty_and_signed_zeros_return_positive_zero) {
    np::tensor_view<const double, np::extents<size_t, 3, 0>> empty(nullptr);
    const auto zero = np::norm_l2(empty);
    EXPECT_EQ(zero, 0);
    EXPECT_FALSE(std::signbit(zero));
    const double values[] = {-0.0, 0.0, -0.0};
    const auto result = np::norm_l2(vector_view<const double>(values, 3));
    EXPECT_EQ(result, 0);
    EXPECT_FALSE(std::signbit(result));
    using shape = np::extents<size_t, 0>;
    const auto mapping = np::layout_stride::mapping<shape>(shape{}, std::array{size_t{17}});
    EXPECT_EQ(
        np::norm_l2(np::tensor_view<const double, shape, np::layout_stride>(nullptr, mapping)), 0);
}

TEST(np_norm, finite_large_values_do_not_overflow_when_squared) {
    check_values<float>({1e30f, -1e30f});
    check_values<double>({1e308, -1e308});
    check_values<long double>(
        {std::numeric_limits<long double>::max() / 4, std::numeric_limits<long double>::max() / 4});
    const double largest = std::numeric_limits<double>::max();
    EXPECT_EQ(np::norm_l2(vector_view<const double>(&largest, 1)), largest);
    const double overflow[] = {largest, largest};
    EXPECT_EQ(np::norm_l2(vector_view<const double>(overflow, 2)),
              std::numeric_limits<double>::infinity());
}

TEST(np_norm, small_and_subnormal_values_do_not_disappear_when_squared) {
    check_values<float>({1e-30f, -1e-30f});
    check_values<double>({1e-300, -1e-300});
    const auto tiny = std::numeric_limits<double>::denorm_min();
    check_values<double>({tiny, tiny, tiny, tiny});
    const auto long_tiny = std::numeric_limits<long double>::denorm_min();
    check_values<long double>({long_tiny, long_tiny, long_tiny, long_tiny});
    EXPECT_EQ(np::norm_l2(vector_view<const double>(&tiny, 1)), tiny);
}

TEST(np_norm, mixed_scaling_buckets_and_threshold_neighbors) {
    for (const int exponent : {-1022, -511, 0, 486, 1022}) {
        const auto value = std::ldexp(1.0, exponent);
        check_values<double>({std::nextafter(value, 0.0), value,
                              std::nextafter(value, std::numeric_limits<double>::infinity()),
                              -value});
    }
    check_values<double>({1e-300, 2e-300, 1e-154, -1e-154});
    check_values<double>({1e-300, 1, 1e300, -1e300});
    check_values<float>({1e-40f, 1e-20f, 1e20f, -1e20f});
}

TEST(np_norm, nan_takes_precedence_over_infinity_in_every_position) {
    const auto inf = std::numeric_limits<double>::infinity();
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    for (size_t first = 0; first != 4; ++first) {
        for (size_t second = 0; second != 4; ++second) {
            if (first == second)
                continue;
            std::array<double, 4> values{1e-300, 3, -4, 0};
            values[first] = first % 2 ? -inf : inf;
            values[second] = nan;
            EXPECT_TRUE(std::isnan(np::norm_l2(vector_view<const double>(values.data(), 4))));
        }
    }
    const double infinite[] = {inf, -inf, 3, 1e-300};
    EXPECT_EQ(np::norm_l2(vector_view<const double>(infinite, 4)), inf);
}

TEST(np_norm, strided_input_skips_padding_and_handles_short_tails) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    using shape = np::dextents<size_t, 1>;
    for (size_t count = 1; count != 34; ++count) {
        std::vector<double> storage(3 * (count - 1) + 1, nan);
        long double expected = 0;
        for (size_t i = 0; i != count; ++i) {
            storage[3 * i] = static_cast<double>(i) - 9;
            expected = std::hypot(expected, static_cast<long double>(storage[3 * i]));
        }
        const auto mapping = np::layout_stride::mapping<shape>(shape(count), std::array{3});
        const auto result = np::norm_l2(
            np::tensor_view<const double, shape, np::layout_stride>(storage.data(), mapping));
        EXPECT_NEAR(result, static_cast<double>(expected), static_cast<double>(expected) * 1e-14);
    }
}

TEST(np_norm, integer_conversion_precedes_absolute_value) {
    const std::int64_t minimum = std::numeric_limits<std::int64_t>::min();
    EXPECT_EQ(np::norm_l2(vector_view<const std::int64_t>(&minimum, 1)), std::ldexp(1.0, 63));
    const std::uint64_t maximum = std::numeric_limits<std::uint64_t>::max();
    EXPECT_EQ(np::norm_l2(vector_view<const std::uint64_t>(&maximum, 1)),
              static_cast<double>(maximum));
    const bool flags[] = {true, false, true, true, true};
    EXPECT_EQ(np::norm_l2(vector_view<const bool>(flags, 5)), 2);
    const float values[] = {3, 4};
    EXPECT_EQ(np::norm_l2<double>(vector_view<const float>(values, 2)), 5);
    EXPECT_EQ(np::norm_l2(np::tensor_view<const float, np::extents<std::uint8_t, 2>>(values)), 5);
}

TEST(np_norm, deterministic_exponent_sweep_matches_hypot_oracle) {
    std::vector<double> values;
    for (size_t count :
         {size_t{1}, size_t{2}, size_t{3}, size_t{4}, size_t{7}, size_t{31}, size_t{300}}) {
        values.clear();
        for (size_t i = 0; i != count; ++i)
            values.push_back(std::ldexp((i % 2 ? -1.0 : 1.0) * (1 + (i % 13) / 16.0),
                                        static_cast<int>((i * 137) % 2001) - 1000));
        check_values(values);
    }
}

TEST(np_norm, fixed_scale_bucket_overflow_uses_scaled_fallback) {
    // 64 MiB: the float medium bucket overflows although the norm is 2^64.
    const std::vector<float> values(size_t{1} << 24, 0x1p52f);
    EXPECT_EQ(np::norm_l2(vector_view<const float>(values.data(), values.size())), 0x1p64f);
}
} // namespace
