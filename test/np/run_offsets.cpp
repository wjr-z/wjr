#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/run_offsets.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;
template <class T>
constexpr auto strided(T *data, size_t count, size_t step) {
    using shape = np::dextents<size_t, 1>;
    return np::tensor_view<T, shape, np::layout_stride>(
        data, np::layout_stride::mapping<shape>(shape(count), std::array<size_t, 1>{step}));
}
template <class D, class S>
concept runs_available = requires(D dst, S src) { np::run_offsets_to(dst, src); };
static_assert(runs_available<view<unsigned>, view<const bool>>);
static_assert(!runs_available<view<bool>, view<int>>);
static_assert(!runs_available<view<float>, view<int>>);
static_assert(!runs_available<view<const int>, view<int>>);
static_assert(!runs_available<view<volatile int>, view<int>>);
static_assert(!runs_available<view<int>, view<volatile int>>);
static_assert(!runs_available<view<int>, np::tensor_view<int, np::extents<size_t, 2, 2>>>);
static_assert(!runs_available<decltype(strided(static_cast<int *>(nullptr), 0, 2)), view<int>>);

constexpr bool constants() {
    const int keys[] = {1, 1, 2, 1};
    unsigned output[] = {99, 99, 99, 99, 99};
    return np::run_offsets_to(view<unsigned>(output, 5), view<const int>(keys, 4)) == 3 &&
           output[0] == 0 && output[1] == 2 && output[2] == 3 && output[3] == 4 && output[4] == 99;
}
static_assert(constants());

TEST(NpRunOffsets, ConstantsEmptyAndSingleton) {
    EXPECT_TRUE(constants());
    unsigned output[] = {99, 99, 99};
    EXPECT_EQ(np::run_offsets_to(view<unsigned>(output, 1), view<const int>(nullptr, 0)), 0);
    EXPECT_EQ(output[0], 0);
    EXPECT_EQ(output[1], 99);
    const int key = 7;
    EXPECT_EQ(np::run_offsets_to(view<unsigned>(output, 2), view<const int>(&key, 1)), 1);
    EXPECT_EQ((std::array{output[0], output[1], output[2]}), (std::array{0u, 1u, 99u}));
}

TEST(NpRunOffsets, EqualDifferentAndReappearingKeys) {
    for (const auto &keys : {std::vector<int>{4, 4, 4, 4}, std::vector<int>{0, 1, 2, 3},
                             std::vector<int>{0, 0, 1, 0}}) {
        std::vector<size_t> expected{0};
        for (size_t i = 1; i != keys.size(); ++i)
            if (keys[i - 1] != keys[i])
                expected.push_back(i);
        expected.push_back(keys.size());
        std::vector<size_t> output(expected.size() + 1, 99);
        EXPECT_EQ(np::run_offsets_to(view<size_t>(output.data(), expected.size()),
                                     view<const int>(keys.data(), keys.size())),
                  expected.size() - 1);
        EXPECT_TRUE(std::equal(expected.begin(), expected.end(), output.begin()));
        EXPECT_EQ(output.back(), 99);
    }
}

TEST(NpRunOffsets, FullWidthBoolAndNarrowOffsets) {
    const uint64_t keys[] = {UINT64_MAX, UINT64_MAX - 1, UINT64_MAX - 1};
    unsigned output[4];
    EXPECT_EQ(np::run_offsets_to(view<unsigned>(output, 4), view<const uint64_t>(keys, 3)), 2);
    EXPECT_EQ((std::array{output[0], output[1], output[2]}), (std::array{0u, 1u, 3u}));
    const bool bools[] = {false, false, true, true, false};
    EXPECT_EQ(np::run_offsets_to(view<unsigned>(output, 4), view<const bool>(bools, 5)), 3);
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3]}),
              (std::array{0u, 2u, 4u, 5u}));
    std::array<int, 255> long_keys{};
    uint8_t narrow[2];
    EXPECT_EQ(np::run_offsets_to(view<uint8_t>(narrow, 2), view<const int>(long_keys.data(), 255)),
              1);
    EXPECT_EQ(narrow[1], 255);
}

TEST(NpRunOffsets, FloatingEqualityAndCustomNaNGrouping) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const double keys[] = {0., -0., nan, nan, 2.};
    size_t output[6];
    EXPECT_EQ(np::run_offsets_to(view<size_t>(output, 6), view<const double>(keys, 5)), 4);
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3], output[4]}),
              (std::array<size_t, 5>{0, 2, 3, 4, 5}));
    auto equal = [](double a, double b) noexcept { return a == b || (a != a && b != b); };
    EXPECT_EQ(np::run_offsets_to(view<size_t>(output, 6), view<const double>(keys, 5), equal), 3);
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3]}),
              (std::array<size_t, 4>{0, 2, 4, 5}));
}

TEST(NpRunOffsets, StridedAndPaddedKeysStayUnmodified) {
    const int keys[] = {1, -99, 1, -99, 2, -99, 2, -99, 1, -99};
    int output[5] = {-99, -99, -99, -99, -99};
    EXPECT_EQ(np::run_offsets_to(view<int>(output, 5), strided(keys, 5, 2)), 3);
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3], output[4]}),
              (std::array{0, 2, 4, 5, -99}));
    using shape = np::dextents<size_t, 1>;
    using layout = Kokkos::Experimental::layout_right_padded<8>;
    const np::tensor_view<const int, shape, layout> padded(
        keys, typename layout::mapping<shape>(shape(2)));
    EXPECT_EQ(np::run_offsets_to(view<int>(output, 5), padded), 2);
    EXPECT_EQ(keys[1], -99);
}

struct counting_equal {
    size_t calls = 0;
    counting_equal() = default;
    counting_equal(const counting_equal &) = delete;
    bool operator()(const int &a, const int &b) noexcept {
        ++calls;
        return a == b;
    }
};
struct throwing_bool {
    explicit operator bool() const noexcept(false) { return true; }
};
struct throwing_conversion_equal {
    throwing_bool operator()(int, int) const noexcept { return {}; }
};
static_assert(noexcept(np::run_offsets_to(std::declval<view<int>>(), std::declval<view<int>>(),
                                          std::declval<counting_equal &>())));
static_assert(!noexcept(np::run_offsets_to(std::declval<view<int>>(), std::declval<view<int>>(),
                                           throwing_conversion_equal{})));

TEST(NpRunOffsets, CallableIsBorrowedAndCalledInOrder) {
    const int keys[] = {1, 1, 2, 2, 3};
    int output[6];
    counting_equal equal;
    EXPECT_EQ(np::run_offsets_to(view<int>(output, 6), view<const int>(keys, 5), equal), 3);
    EXPECT_EQ(equal.calls, 4);
    EXPECT_EQ(np::run_offsets_to(view<int>(output, 6), view<const int>(nullptr, 0), equal), 0);
    EXPECT_EQ(equal.calls, 4);
    size_t calls = 0;
    auto throwing = [&](int a, int b) {
        if (++calls == 3)
            throw std::runtime_error("stop");
        return a == b;
    };
    std::fill(std::begin(output), std::end(output), -99);
    EXPECT_THROW(np::run_offsets_to(view<int>(output, 6), view<const int>(keys, 5), throwing),
                 std::runtime_error);
    EXPECT_EQ(calls, 3);
    EXPECT_EQ(output[0], 0);
    EXPECT_EQ(output[1], 2);
    EXPECT_EQ(output[2], -99);
}

TEST(NpRunOffsets, RandomOracle) {
    std::mt19937 random(6381);
    for (size_t count : {size_t{0}, size_t{1}, size_t{32}, size_t{1024}}) {
        std::vector<int> keys(count);
        for (auto &key : keys)
            key = static_cast<int>(random() % 7);
        const auto original = keys;
        std::vector<size_t> expected{0};
        for (size_t i = 1; i < count; ++i)
            if (keys[i - 1] != keys[i])
                expected.push_back(i);
        if (count != 0)
            expected.push_back(count);
        std::vector<size_t> output(count + 2, SIZE_MAX);
        EXPECT_EQ(np::run_offsets_to(view<size_t>(output.data(), count + 1),
                                     view<const int>(keys.data(), count)),
                  expected.size() - 1);
        EXPECT_TRUE(std::equal(expected.begin(), expected.end(), output.begin()));
        EXPECT_EQ(output[expected.size()], SIZE_MAX);
        EXPECT_EQ(keys, original);
    }
}

TEST(NpRunOffsets, ExactPhysicalSpanAndOriginalAdjacentReferences) {
    for (size_t step : {size_t{1}, size_t{2}, size_t{3}, size_t{5}}) {
        constexpr size_t count = 4;
        std::vector<int> keys((count - 1) * step + 1, -99);
        for (size_t i = 0; i != count; ++i)
            keys[i * step] = static_cast<int>(i / 2);
        size_t calls = 0;
        auto equal = [&](const int &left, const int &right) {
            EXPECT_EQ(&left, keys.data() + calls * step);
            EXPECT_EQ(&right, keys.data() + (calls + 1) * step);
            ++calls;
            return left == right;
        };
        unsigned output[3];
        EXPECT_EQ(
            np::run_offsets_to(view<unsigned>(output, 3), strided(keys.data(), count, step), equal),
            2);
        EXPECT_EQ(calls, count - 1);
        EXPECT_EQ((std::array{output[0], output[1], output[2]}), (std::array{0u, 2u, 4u}));
    }
}
} // namespace
