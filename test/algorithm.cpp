#include <wjr/algorithm.hpp>

#include <array>
#include <iterator>
#include <memory>
#include <sstream>

#include <gtest/gtest.h>

namespace {

struct move_only_value {
    int value;

    constexpr explicit move_only_value(int v) : value(v) {}
    move_only_value(move_only_value &&) = default;
    move_only_value &operator=(move_only_value &&) = default;
    move_only_value(const move_only_value &) = delete;
    move_only_value &operator=(const move_only_value &) = delete;
};

constexpr bool check_constant_evaluation() {
    std::array<int, 5> values{3, 1, 4, 1, 2};
    wjr::constant::sort(values.begin(), values.end());
    if (values != std::array{1, 1, 2, 3, 4}) {
        return false;
    }

    std::array<int, 5> copied{};
    if (wjr::constant::copy(values.begin(), values.end(), copied.begin()) != copied.end() ||
        copied != values) {
        return false;
    }
    if (wjr::constant::move_backward(copied.begin(), copied.begin() + 3, copied.begin() + 4) !=
            copied.begin() + 1 ||
        copied != std::array{1, 1, 1, 2, 4}) {
        return false;
    }

    std::array<int, 2> other{0, 5};
    std::array<int, 7> merged{};
    if (wjr::constant::merge(values.begin(), values.end(), other.begin(), other.end(),
                             merged.begin(), std::less<>{}) != merged.end() ||
        merged != std::array{0, 1, 1, 2, 3, 4, 5}) {
        return false;
    }

    std::array<move_only_value, 3> objects{move_only_value{3}, move_only_value{1},
                                           move_only_value{2}};
    wjr::constant::sort(objects.begin(), objects.end(),
                        [](const auto &lhs, const auto &rhs) { return lhs.value < rhs.value; });
    return objects[0].value == 1 && objects[1].value == 2 && objects[2].value == 3;
}

static_assert(check_constant_evaluation());

struct record {
    int key;
    int order;

    bool operator==(const record &) const = default;
};

constexpr auto compare_records = [](const record &lhs, const record &rhs) {
    return lhs.key < rhs.key;
};

} // namespace

TEST(algorithm, constexpr_algorithms_are_also_callable_at_runtime) {
    EXPECT_TRUE(check_constant_evaluation());
}

TEST(algorithm, merge_reads_single_pass_inputs_once) {
    std::istringstream left("1 3 5"), right("2 4 6");
    std::array<int, 6> result{};
    using input_iterator = std::istream_iterator<int>;
    EXPECT_EQ(wjr::constant::merge(input_iterator(left), input_iterator(), input_iterator(right),
                                   input_iterator(), result.begin(), std::less<>{}),
              result.end());
    EXPECT_EQ(result, (std::array{1, 2, 3, 4, 5, 6}));
}

TEST(algorithm, merge_preserves_equivalent_input_order) {
    const std::array left{record{1, 10}, record{2, 11}, record{2, 12}};
    const std::array right{record{1, 20}, record{2, 21}, record{3, 22}};
    std::array<record, 6> result{};
    EXPECT_EQ(wjr::constant::merge(left.begin(), left.end(), right.begin(), right.end(),
                                   result.begin(), compare_records),
              result.end());
    EXPECT_EQ(result, (std::array{record{1, 10}, record{1, 20}, record{2, 11}, record{2, 12},
                                  record{2, 21}, record{3, 22}}));
}

TEST(algorithm, sort_and_move_backward_accept_move_only_values) {
    std::array<std::unique_ptr<int>, 4> values;
    values[0] = std::make_unique<int>(3);
    values[1] = std::make_unique<int>(1);
    values[2] = std::make_unique<int>(2);
    wjr::constant::sort(values.begin(), values.begin() + 3,
                        [](const auto &lhs, const auto &rhs) { return *lhs < *rhs; });
    EXPECT_EQ(*values[0], 1);
    EXPECT_EQ(*values[1], 2);
    EXPECT_EQ(*values[2], 3);
    EXPECT_EQ(wjr::constant::move_backward(values.begin(), values.begin() + 3, values.end()),
              values.begin() + 1);
    EXPECT_EQ(values[0], nullptr);
    EXPECT_EQ(*values[1], 1);
    EXPECT_EQ(*values[2], 2);
    EXPECT_EQ(*values[3], 3);
}

TEST(algorithm, sort_handles_empty_repeated_and_custom_order_ranges) {
    std::array<int, 6> values{2, 1, 2, 3, 1, 3};
    wjr::constant::sort(values.begin(), values.begin());
    EXPECT_EQ(values, (std::array{2, 1, 2, 3, 1, 3}));
    wjr::constant::sort(values.begin(), values.end(), std::greater<>{});
    EXPECT_EQ(values, (std::array{3, 3, 2, 2, 1, 1}));
}

TEST(algorithm, sort_avoids_quadratic_comparison_growth) {
    constexpr std::size_t count = 4096;
    std::array<int, count> values{};
    for (std::size_t i = 0; i < count; ++i) {
        values[i] = static_cast<int>((i * 2053) % count);
    }
    std::size_t comparisons = 0;
    wjr::constant::sort(values.begin(), values.end(), [&](int lhs, int rhs) {
        ++comparisons;
        return lhs < rhs;
    });
    for (std::size_t i = 0; i < count; ++i) {
        ASSERT_EQ(values[i], static_cast<int>(i));
    }
    // A deliberately broad N log2(N) budget rejects the old insertion-sort path.
    EXPECT_LT(comparisons, 32 * count * 12);
}

TEST(algorithm, inplace_merge_is_stable_and_returns_last) {
    std::array values{record{1, 10}, record{2, 11}, record{2, 12},
                      record{1, 20}, record{2, 21}, record{3, 22}};
    EXPECT_EQ(wjr::constant::inplace_merge(values.begin(), values.begin() + 3, values.end(),
                                           compare_records),
              values.end());
    EXPECT_EQ(values, (std::array{record{1, 10}, record{1, 20}, record{2, 11}, record{2, 12},
                                  record{2, 21}, record{3, 22}}));
    EXPECT_EQ(
        wjr::constant::inplace_merge(values.begin(), values.begin(), values.end(), compare_records),
        values.end());
}
