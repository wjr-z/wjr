#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/merge.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;
template <class D, class L, class R>
concept merges = requires(D d, L l, R r) { np::merge_sorted_to(d, l, r); };
static_assert(merges<view<int>, view<const int>, view<int>>);
static_assert(!merges<view<const int>, view<int>, view<int>>);
static_assert(!merges<view<volatile int>, view<int>, view<int>>);
static_assert(!merges<view<int>, view<volatile int>, view<int>>);
static_assert(!merges<view<int>, view<int>, view<unsigned>>);
static_assert(!merges<view<int>, np::tensor_view<int, np::extents<size_t, 2, 2>>, view<int>>);

constexpr bool constant_merge() {
    const int a[] = {1, 3, 3}, b[] = {2, 3, 9};
    int out[6]{};
    np::merge_sorted_to(view<int>(out, 6), view<const int>(a, 3), view<const int>(b, 3));
    return std::equal(out, out + 6, std::array{1, 2, 3, 3, 3, 9}.begin());
}
static_assert(constant_merge());

struct counting_less {
    int calls = 0;
    counting_less() = default;
    counting_less(const counting_less &) = delete;
    bool operator()(const int &a, const int &b) noexcept {
        ++calls;
        return a < b;
    }
};
struct bool_proxy {
    bool value;
    explicit operator bool() const noexcept(false) { return value; }
};
struct proxy_less {
    bool_proxy operator()(const int &a, const int &b) noexcept { return {a < b}; }
};
static_assert(noexcept(np::merge_sorted_to(std::declval<view<int>>(), std::declval<view<int>>(),
                                           std::declval<view<int>>())));
static_assert(!noexcept(np::merge_sorted_to(std::declval<view<int>>(), std::declval<view<int>>(),
                                            std::declval<view<int>>(), proxy_less{})));

TEST(NpMerge, ConstantEvaluationAndEmptyRanges) {
    EXPECT_TRUE(constant_merge());
    np::merge_sorted_to(view<int>(nullptr, 0), view<const int>(nullptr, 0),
                        view<const int>(nullptr, 0));
    const int source[] = {1, 3, 5};
    int output[3];
    for (bool empty_left : {true, false}) {
        const view<const int> empty(nullptr, 0), full(source, 3);
        np::merge_sorted_to(view<int>(output, 3), empty_left ? empty : full,
                            empty_left ? full : empty);
        EXPECT_TRUE(std::equal(output, output + 3, source));
    }
}

TEST(NpMerge, RandomInputsMatchIndependentStandardMerge) {
    std::mt19937 engine(98);
    for (size_t n : {0, 1, 7, 32, 513}) {
        for (size_t m : {0, 1, 9, 64}) {
            std::vector<int> a(n), b(m), expected(n + m), actual(n + m);
            for (int &x : a)
                x = static_cast<int>(engine() % 31);
            for (int &x : b)
                x = static_cast<int>(engine() % 31);
            for (bool descending : {false, true}) {
                const auto compare = [descending](const int &x, const int &y) noexcept {
                    return descending ? x > y : x < y;
                };
                std::sort(a.begin(), a.end(), compare);
                std::sort(b.begin(), b.end(), compare);
                std::merge(a.begin(), a.end(), b.begin(), b.end(), expected.begin(), compare);
                np::merge_sorted_to(view<int>(actual.data(), actual.size()),
                                    view<const int>(a.data(), n), view<const int>(b.data(), m),
                                    compare);
                EXPECT_EQ(actual, expected);
            }
        }
    }
}

TEST(NpMerge, StableZerosNaNsAndInfinities) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    const double a[] = {-inf, -0., -0., inf, nan}, b[] = {0., 0., inf, nan};
    double output[9];
    np::merge_sorted_to(view<double>(output, 9), view<const double>(a, 5),
                        view<const double>(b, 4));
    EXPECT_EQ(output[0], -inf);
    EXPECT_TRUE(std::signbit(output[1]));
    EXPECT_TRUE(std::signbit(output[2]));
    EXPECT_FALSE(std::signbit(output[3]));
    EXPECT_FALSE(std::signbit(output[4]));
    EXPECT_EQ(output[5], inf);
    EXPECT_EQ(output[6], inf);
    EXPECT_TRUE(std::isnan(output[7]));
    EXPECT_TRUE(std::isnan(output[8]));
}

TEST(NpMerge, UnsignedPrecisionAndRealBoolStorage) {
    constexpr auto maximum = std::numeric_limits<uint64_t>::max();
    const uint64_t a[] = {0, maximum - 2, maximum}, b[] = {maximum - 1};
    uint64_t output[4];
    np::merge_sorted_to(view<uint64_t>(output, 4), view<const uint64_t>(a, 3),
                        view<const uint64_t>(b, 1));
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3]}),
              (std::array<uint64_t, 4>{0, maximum - 2, maximum - 1, maximum}));
    const bool x[] = {false, true}, y[] = {false};
    bool result[3];
    np::merge_sorted_to(view<bool>(result, 3), view<const bool>(x, 2), view<const bool>(y, 1));
    EXPECT_FALSE(result[0]);
    EXPECT_FALSE(result[1]);
    EXPECT_TRUE(result[2]);
}

TEST(NpMerge, StridesAndMixedLayoutsPreserveHoles) {
    using shape = np::dextents<size_t, 1>;
    const int a[] = {1, -9, 3, -9, 5}, b[] = {2, 4};
    int output[14];
    std::fill_n(output, 14, -99);
    const np::layout_stride::mapping<shape> lm(shape(3), std::array<size_t, 1>{2});
    const np::layout_stride::mapping<shape> dm(shape(5), std::array<size_t, 1>{3});
    np::merge_sorted_to(np::tensor_view<int, shape, np::layout_stride>(output, dm),
                        np::tensor_view<const int, shape, np::layout_stride>(a, lm),
                        np::tensor_view<const int, shape, np::layout_left>(b, 2));
    for (size_t i = 0; i != 14; ++i)
        EXPECT_EQ(output[i], i % 3 == 0 && i / 3 < 5 ? static_cast<int>(i / 3 + 1) : -99);
    // Rank-one padded layouts are physically packed, but remain valid in the fallback.
    using padded = Kokkos::Experimental::layout_right_padded<8>;
    const padded::mapping<shape> pm(shape(3));
    const int padded_source[] = {1, 3, 5};
    int packed[5];
    np::merge_sorted_to(view<int>(packed, 5),
                        np::tensor_view<const int, shape, padded>(padded_source, pm),
                        view<const int>(b, 2));
    EXPECT_EQ((std::array{packed[0], packed[1], packed[2], packed[3], packed[4]}),
              (std::array{1, 2, 3, 4, 5}));
}

TEST(NpMerge, InputsMayOverlapEachOther) {
    const int source[] = {1, 2, 3};
    int output[5];
    np::merge_sorted_to(view<int>(output, 5), view<const int>(source, 3),
                        view<const int>(source + 1, 2));
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3], output[4]}),
              (std::array{1, 2, 2, 3, 3}));
}

TEST(NpMerge, BorrowedNoncopyableAndExplicitBoolComparators) {
    const int a[] = {1, 3}, b[] = {2, 4};
    int output[4];
    counting_less compare;
    np::merge_sorted_to(view<int>(output, 4), view<const int>(a, 2), view<const int>(b, 2),
                        compare);
    EXPECT_EQ(compare.calls, 3);
    np::merge_sorted_to(view<int>(output, 4), view<const int>(a, 2), view<const int>(b, 2),
                        proxy_less{});
    EXPECT_EQ(output[3], 4);
    int calls = 0;
    const auto throwing = [&calls](const int &x, const int &y) {
        if (++calls == 2)
            throw std::runtime_error("comparison");
        return x < y;
    };
    std::fill_n(output, 4, -1);
    EXPECT_THROW(np::merge_sorted_to(view<int>(output, 4), view<const int>(a, 2),
                                     view<const int>(b, 2), throwing),
                 std::runtime_error);
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], -1);
}

TEST(NpMerge, StridedCallableUsesConstReferencesAndPropagatesBoolConversion) {
    using shape = np::dextents<size_t, 1>;
    using stride_view = np::tensor_view<int, shape, np::layout_stride>;
    const np::layout_stride::mapping<shape> mapping(shape(2), std::array<size_t, 1>{2});
    const np::layout_stride::mapping<shape> output_mapping(shape(4), std::array<size_t, 1>{2});
    int a[] = {1, -9, 3}, b[] = {2, -9, 4}, output[8];
    std::fill_n(output, 8, -99);
    counting_less compare;
    np::merge_sorted_to(stride_view(output, output_mapping), stride_view(a, mapping),
                        stride_view(b, mapping), compare);
    EXPECT_EQ(compare.calls, 3);
    for (size_t i = 0; i != 4; ++i) {
        EXPECT_EQ(output[2 * i], i + 1);
        EXPECT_EQ(output[2 * i + 1], -99);
    }
    struct throwing_proxy {
        explicit operator bool() const { throw std::runtime_error("conversion"); }
    };
    const auto throwing = [](const int &, const int &) noexcept { return throwing_proxy{}; };
    EXPECT_THROW(np::merge_sorted_to(stride_view(output, output_mapping), stride_view(a, mapping),
                                     stride_view(b, mapping), throwing),
                 std::runtime_error);
    EXPECT_THROW(
        np::merge_sorted_to(view<int>(output, 4), view<int>(a, 2), view<int>(b, 2), throwing),
        std::runtime_error);
}
} // namespace
