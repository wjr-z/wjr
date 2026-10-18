#include <array>
#include <cmath>
#include <limits>

#include <gtest/gtest.h>

#include <wjr/np/cblas.hpp>
#include <wjr/np/slice.hpp>

namespace {
namespace np = wjr::np;
namespace blas = np::cblas;
using line = np::dextents<size_t, 1>;
using matrix = np::dextents<size_t, 2>;
template <class T, class E = matrix, class L = np::layout_right>
using view = np::tensor_view<T, E, L>;
template <class X, class Y>
concept can_dot = requires(X x, Y y) { blas::dot(x, y); };
template <class C, class A = C, class B = A>
concept can_gemm = requires(C c, A a, B b) { blas::gemm_to(c, a, b); };
template <class Y, class A, class X = Y>
concept can_gemv = requires(Y y, A a, X x) { blas::gemv_to(y, a, x); };
struct custom_accessor : np::default_accessor<float> {};
static_assert(can_dot<view<const float, line>, view<float, line>>);
static_assert(!can_dot<view<float, line>, view<double, line>>);
static_assert(!can_dot<view<int, line>, view<int, line>>);
static_assert(!can_dot<view<volatile float, line>, view<float, line>>);
static_assert(!can_dot<view<float>, view<float>>);
static_assert(can_gemm<view<float>, view<const float>>);
static_assert(can_gemm<view<double, matrix, np::layout_left>>);
static_assert(!can_gemm<view<const double>>);
static_assert(!can_gemm<view<float>, view<double>>);
static_assert(!can_gemm<view<float>, view<float, matrix, np::layout_left>>);
static_assert(!can_gemm<np::tensor_view<float, matrix, np::layout_right, custom_accessor>>);
static_assert(can_gemv<view<float, line>, view<const float>, view<const float, line>>);
static_assert(!can_gemv<view<const float, line>, view<float>>);
static_assert(!can_gemv<view<float, line>, view<double>>);
static_assert(!can_gemv<view<float, line>, view<float>, view<volatile float, line>>);
static_assert(!can_gemv<view<float>, view<float>>);
static_assert(std::same_as<decltype(blas::dot(std::declval<view<const float, line>>(),
                                              std::declval<view<const float, line>>())),
                           float>);
static_assert(noexcept(blas::gemm_to(std::declval<view<double>>(),
                                     std::declval<view<const double>>(),
                                     std::declval<view<const double>>())));

template <class T>
void check_dot() {
    const T x[]{1, 2, 3};
    const view<const T, line> source(x, 3);
    EXPECT_EQ(blas::dot(source, source), T{14});
    const T a[]{-99, 2, -99, 4, -99, 6};
    const T b[]{3, -99, -99, 5, -99, -99, 7};
    const np::layout_stride::mapping<line> ma(line(3), std::array<size_t, 1>{2});
    const np::layout_stride::mapping<line> mb(line(3), std::array<size_t, 1>{3});
    EXPECT_EQ(blas::dot(view<const T, line, np::layout_stride>(a + 1, ma),
                        view<const T, line, np::layout_stride>(b, mb)),
              T{68});
    const size_t large = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
    const np::layout_stride::mapping<line> empty_mapping(line(0), std::array{large});
    const auto empty = blas::dot(view<const T, line, np::layout_stride>(nullptr, empty_mapping),
                                 view<const T, line>(nullptr, 0));
    EXPECT_EQ(empty, T{0});
    EXPECT_FALSE(std::signbit(empty));
}

TEST(NpCblas, DotSupportsBothDtypesPositiveStridesAndEmpty) {
    check_dot<float>();
    check_dot<double>();
}

template <class T, class Layout>
void check_gemm() {
    for (bool ta : {false, true})
        for (bool tb : {false, true}) {
            std::array<T, 8> a_values;
            std::array<T, 12> b_values;
            std::array<T, 6> c_values;
            const view<T, matrix, Layout> a(a_values.data(), ta ? 4 : 2, ta ? 2 : 4);
            const view<T, matrix, Layout> b(b_values.data(), tb ? 3 : 4, tb ? 4 : 3);
            const view<T, matrix, Layout> c(c_values.data(), 2, 3);
            for (size_t row = 0; row != 2; ++row)
                for (size_t inner = 0; inner != 4; ++inner)
                    a(ta ? inner : row, ta ? row : inner) = static_cast<T>(row + inner + 1);
            for (size_t inner = 0; inner != 4; ++inner)
                for (size_t col = 0; col != 3; ++col)
                    b(tb ? col : inner, tb ? inner : col) = static_cast<T>(inner * 2 + col + 1);
            c_values.fill(T{4});
            blas::gemm_to(c, a, b, T{2}, T{.5},
                          ta ? blas::transpose::transposed : blas::transpose::none,
                          tb ? blas::transpose::transposed : blas::transpose::none);
            for (size_t row = 0; row != 2; ++row)
                for (size_t col = 0; col != 3; ++col) {
                    T expected = T{2};
                    for (size_t inner = 0; inner != 4; ++inner)
                        expected += T{2} * static_cast<T>(row + inner + 1) *
                                    static_cast<T>(inner * 2 + col + 1);
                    EXPECT_EQ(c(row, col), expected);
                }
        }
}

TEST(NpCblas, GemmBothOrdersAndAllTransposePairs) {
    check_gemm<float, np::layout_right>();
    check_gemm<double, np::layout_right>();
    check_gemm<float, np::layout_left>();
    check_gemm<double, np::layout_left>();
}

template <class Layout>
void check_padded_gemm() {
    using full = np::extents<size_t, 3, 5>;
    std::array<double, 15> a, b, c;
    a.fill(-99.);
    b.fill(-99.);
    c.fill(-99.);
    const auto window = [](auto &storage) {
        return np::slice_view(view<double, full, Layout>(storage.data()), std::pair{0, 2},
                              std::pair{1, 3});
    };
    const auto av = window(a), bv = window(b), cv = window(c);
    static_assert(np::detail::padded_layout<typename decltype(av)::layout_type>);
    av(0, 0) = 1.;
    av(0, 1) = 2.;
    av(1, 0) = 3.;
    av(1, 1) = 4.;
    bv(0, 0) = 5.;
    bv(0, 1) = 6.;
    bv(1, 0) = 7.;
    bv(1, 1) = 8.;
    // Normal beta-zero GEMM must not depend on the previous output values.
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 2; ++col)
            cv(row, col) = std::numeric_limits<double>::quiet_NaN();
    blas::gemm_to(cv, av, bv);
    EXPECT_EQ(cv(0, 0), 19.);
    EXPECT_EQ(cv(0, 1), 22.);
    EXPECT_EQ(cv(1, 0), 43.);
    EXPECT_EQ(cv(1, 1), 50.);
    for (size_t i = 0; i != c.size(); ++i) {
        bool logical = false;
        for (size_t row = 0; row != 2; ++row)
            for (size_t col = 0; col != 2; ++col)
                logical |= &c[i] == &cv(row, col);
        if (!logical)
            EXPECT_EQ(c[i], -99.);
    }
    // A layout_stride descriptor retains the same pitch without packing.
    const view<double, matrix, np::layout_stride> cs(cv);
    const view<const double, matrix, np::layout_stride> as(av), bs(bv);
    blas::gemm_to(cs, as, bs);
    EXPECT_EQ(cv(1, 1), 50.);
}

TEST(NpCblas, PaddedWindowsAndGeneralPitchPreservePadding) {
    check_padded_gemm<np::layout_right>();
    check_padded_gemm<np::layout_left>();
}

TEST(NpCblas, EmptyAndZeroProductScaleOnlyLogicalOutput) {
    using output_shape = np::extents<size_t, 2, 3>;
    double uninitialized[6]; // Live scalar objects; beta zero must only write.
    const view<double, output_shape> c(uninitialized);
    const view<const double, matrix> a(nullptr, 2, 0), b(nullptr, 0, 3);
    blas::gemm_to(c, a, b, std::numeric_limits<double>::quiet_NaN());
    for (double value : uninitialized) {
        EXPECT_EQ(value, 0.);
        EXPECT_FALSE(std::signbit(value));
    }
    np::fill_to(c, -0.);
    blas::gemm_to(c, a, b, 1., 1.);
    for (double value : uninitialized)
        EXPECT_TRUE(std::signbit(value));
    np::fill_to(c, 3.);
    blas::gemm_to(c, a, b, 1., 2.);
    for (double value : uninitialized)
        EXPECT_EQ(value, 6.);
    const double nan = std::numeric_limits<double>::quiet_NaN();
    double av[]{nan, nan, nan, nan}, bv[]{nan, nan, nan, nan, nan, nan};
    blas::gemm_to(c, view<const double, matrix>(av, 2, 2), view<const double, matrix>(bv, 2, 3), 0.,
                  2.);
    for (double value : uninitialized)
        EXPECT_EQ(value, 12.);
    const size_t large = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
    blas::gemm_to(view<double, matrix>(nullptr, 0, large),
                  view<const double, matrix>(nullptr, 0, 0),
                  view<const double, matrix>(nullptr, 0, large));
}

TEST(NpCblas, NormalBetaZeroAcceptsDefaultInitializedOutput) {
    const float a[]{1, 2, 3, 4}, b[]{5, 6, 7, 8};
    float c[4];
    using square = np::extents<size_t, 2, 2>;
    blas::gemm_to(view<float, square>(c), view<const float, square>(a),
                  view<const float, square>(b));
    EXPECT_EQ(c[0], 19.f);
    EXPECT_EQ(c[3], 50.f);
}

template <class T>
void check_floating_dot() {
    std::array<T, 67> a, b;
    long double expected = 0, magnitude = 0;
    for (size_t i = 0; i != a.size(); ++i) {
        a[i] = static_cast<T>(static_cast<int>(i % 17) - 8) / T{7};
        b[i] = static_cast<T>(static_cast<int>(i % 13) - 6) / T{11};
        const auto product = static_cast<long double>(a[i]) * b[i];
        expected += product;
        magnitude += std::abs(product);
    }
    const auto actual =
        blas::dot(view<const T, line>(a.data(), a.size()), view<const T, line>(b.data(), b.size()));
    EXPECT_TRUE(std::abs(static_cast<long double>(actual) - expected) <=
                std::numeric_limits<T>::epsilon() * 4 * a.size() * magnitude);
}

TEST(NpCblas, DotAllowsProviderReductionOrderWithinTolerance) {
    check_floating_dot<float>();
    check_floating_dot<double>();
}

template <class T, class Layout>
void check_gemv() {
    std::array<T, 6> storage;
    const view<T, matrix, Layout> a(storage.data(), 2, 3);
    for (size_t i = 0; i != 2; ++i)
        for (size_t j = 0; j != 3; ++j)
            a(i, j) = static_cast<T>(i * 3 + j + 1);
    for (bool transposed : {false, true}) {
        const size_t count = transposed ? 3 : 2;
        const size_t inner = transposed ? 2 : 3;
        T x[]{1, 2, 3};
        T y[]{4, 4, 4};
        blas::gemv_to(view<T, line>(y, count), a, view<const T, line>(x, inner), T{2}, T{.5},
                      transposed ? blas::transpose::transposed : blas::transpose::none);
        for (size_t i = 0; i != count; ++i) {
            T expected{2};
            for (size_t j = 0; j != inner; ++j)
                expected += T{2} * a(transposed ? j : i, transposed ? i : j) * x[j];
            EXPECT_EQ(y[i], expected);
        }
    }
}

TEST(NpCblas, GemvBothOrdersDtypesAndTranspose) {
    check_gemv<float, np::layout_right>();
    check_gemv<double, np::layout_right>();
    check_gemv<float, np::layout_left>();
    check_gemv<double, np::layout_left>();
}

TEST(NpCblas, GemvGeneralPitchAndVectorIncrementsPreservePadding) {
    double matrix_data[]{1, 2, 3, -99, -99, 4, 5, 6};
    const np::layout_stride::mapping<matrix> mapping(matrix(2, 3), std::array<size_t, 2>{5, 1});
    const view<const double, matrix, np::layout_stride> a(matrix_data, mapping);
    const double x_data[]{1, -99, -99, 2, -99, -99, 3};
    double y_data[]{std::numeric_limits<double>::quiet_NaN(), -99,
                    std::numeric_limits<double>::quiet_NaN()};
    const np::layout_stride::mapping<line> xm(line(3), std::array<size_t, 1>{3});
    const np::layout_stride::mapping<line> ym(line(2), std::array<size_t, 1>{2});
    blas::gemv_to(view<double, line, np::layout_stride>(y_data, ym), a,
                  view<const double, line, np::layout_stride>(x_data, xm));
    EXPECT_EQ(y_data[0], 14.);
    EXPECT_EQ(y_data[1], -99.);
    EXPECT_EQ(y_data[2], 32.);
    EXPECT_EQ(matrix_data[3], -99.);
    EXPECT_EQ(matrix_data[4], -99.);
    const double column_data[]{1, 4, -99, -99, 2, 5, -99, -99, 3, 6};
    const np::layout_stride::mapping<matrix> column_mapping(matrix(2, 3),
                                                            std::array<size_t, 2>{1, 4});
    blas::gemv_to(view<double, line, np::layout_stride>(y_data, ym),
                  view<const double, matrix, np::layout_stride>(column_data, column_mapping),
                  view<const double, line, np::layout_stride>(x_data, xm));
    EXPECT_EQ(y_data[0], 14.);
    EXPECT_EQ(y_data[1], -99.);
    EXPECT_EQ(y_data[2], 32.);
}

TEST(NpCblas, GemvBetaZeroAcceptsDefaultInitializedOutput) {
    const float a[]{1, 2, 3, 4, 5, 6}, x[]{1, 2, 3};
    float y[2];
    blas::gemv_to(view<float, line>(y, 2), view<const float>(a, 2, 3),
                  view<const float, line>(x, 3));
    EXPECT_EQ(y[0], 14.f);
    EXPECT_EQ(y[1], 32.f);
}

TEST(NpCblas, GemvEmptyAndZeroProductOnlyTouchLogicalOutput) {
    double y_data[]{1, -99, 2, -99, 3};
    const np::layout_stride::mapping<line> ym(line(3), std::array<size_t, 1>{2});
    const view<double, line, np::layout_stride> y(y_data, ym);
    const view<const double> empty_a(nullptr, 3, 0);
    const view<const double, line> empty_x(nullptr, 0);
    blas::gemv_to(y, empty_a, empty_x, std::numeric_limits<double>::quiet_NaN());
    for (size_t i = 0; i != 3; ++i) {
        EXPECT_EQ(y_data[i * 2], 0.);
        EXPECT_FALSE(std::signbit(y_data[i * 2]));
    }
    np::fill_to(y, -0.);
    blas::gemv_to(y, empty_a, empty_x, 1., 1.);
    for (size_t i = 0; i != 3; ++i)
        EXPECT_TRUE(std::signbit(y_data[i * 2]));
    np::fill_to(y, 3.);
    blas::gemv_to(y, empty_a, empty_x, 1., 2.);
    EXPECT_EQ(y_data[4], 6.);
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double a[]{nan, nan, nan}, x[]{nan};
    blas::gemv_to(y, view<const double>(a, 3, 1), view<const double, line>(x, 1), 0., 2.);
    EXPECT_EQ(y_data[0], 12.);
    EXPECT_EQ(y_data[4], 12.);
    EXPECT_EQ(y_data[1], -99.);
    EXPECT_EQ(y_data[3], -99.);
    const size_t large = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
    const np::layout_stride::mapping<line> empty_mapping(line(0), std::array{large});
    const double input[]{1, 2, 3};
    blas::gemv_to(view<double, line, np::layout_stride>(nullptr, empty_mapping),
                  view<const double>(nullptr, 0, 3), view<const double, line>(input, 3));
}
} // namespace
