#include <array>
#include <cmath>
#include <limits>

#include <gtest/gtest.h>

#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_xsimd.hpp>

namespace {
namespace np = wjr::np;

using shape = np::extents<size_t, 4>;
using source_view = np::tensor_view<const float, shape>;
struct unknown_map {
    float operator()(float value) const noexcept { return value; }
};
using mapped_square = decltype(np::square(np::map(unknown_map{}, std::declval<source_view>())));
using math_square = decltype(np::square(np::sqrt(std::declval<source_view>())));
static_assert(!np::detail::homogeneous_xsimd_expression<mapped_square, float>);
static_assert(!np::detail::homogeneous_xsimd_expression<math_square, float>);

template <class T>
void check_square() {
    constexpr size_t capacity = 3 * xsimd::batch<T>::size + 3;
    std::array<T, capacity> input{};
    std::array<T, capacity + 2> output{};
    for (size_t i = 0; i != capacity; ++i)
        input[i] = static_cast<T>(static_cast<int>(i % 17) - 8) / T{4};
    input[0] = -T{0};
    input[1] = std::numeric_limits<T>::infinity();
    input[2] = std::numeric_limits<T>::quiet_NaN();
    using view = np::tensor_view<T, np::dextents<size_t, 1>>;
    using const_view = np::tensor_view<const T, np::dextents<size_t, 1>>;
    for (size_t count = 0; count != capacity; ++count) {
        output.fill(T{123});
        auto expression = np::square(const_view(input.data(), count));
        static_assert(np::detail::homogeneous_xsimd_expression<decltype(expression), T>);
        np::eval_to(view(output.data() + 1, count), expression, np::xsimd_policy<>{});
        EXPECT_EQ(output.front(), T{123});
        EXPECT_EQ(output[count + 1], T{123});
        for (size_t i = 0; i != count; ++i) {
            const T expected = input[i] * input[i];
            if (std::isnan(expected))
                EXPECT_TRUE(std::isnan(output[i + 1]));
            else {
                EXPECT_EQ(output[i + 1], expected);
                EXPECT_EQ(std::signbit(output[i + 1]), std::signbit(expected));
            }
        }
    }
}

TEST(NpSquareXsimd, FloatDoubleEmptyTailsAndExceptionalValues) {
    check_square<float>();
    check_square<double>();
}
} // namespace
