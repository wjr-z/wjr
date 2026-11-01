#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <compare>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {
template <typename T>
concept has_order = requires(const T &a, const T &b) { a <=> b; };

template <typename Left, typename Right, typename StandardLeft, typename StandardRight>
void check_relations(const Left &lhs, const Right &rhs, const StandardLeft &a,
                     const StandardRight &b) {
    EXPECT_EQ(lhs == rhs, a == b);
    EXPECT_EQ(lhs != rhs, a != b);
    EXPECT_EQ(lhs < rhs, a < b);
    EXPECT_EQ(lhs > rhs, a > b);
    EXPECT_EQ(lhs <= rhs, a <= b);
    EXPECT_EQ(lhs >= rhs, a >= b);
    EXPECT_EQ(lhs <=> rhs, a <=> b);
}

template <typename Tree, typename Standard>
void check_sequences() {
    static_assert(std::three_way_comparable<Tree>);
    static_assert(std::same_as<decltype(std::declval<Tree>() <=> std::declval<Tree>()),
                               decltype(std::declval<Standard>() <=> std::declval<Standard>())>);
    for (int left : {0, 1, 2, 3, 80}) {
        for (int right : {0, 1, 2, 3, 80}) {
            for (int variant = 0; variant < 3; ++variant) {
                Tree lhs, rhs;
                Standard a, b;
                for (int i = 0; i < left; ++i) {
                    if constexpr (std::is_void_v<typename Tree::mapped_type>) {
                        lhs.insert(i / 2);
                        a.insert(i / 2);
                    } else {
                        lhs.emplace(i * 2, i);
                        a.emplace(i * 2, i);
                    }
                }
                for (int i = 0; i < right; ++i) {
                    if constexpr (std::is_void_v<typename Tree::mapped_type>) {
                        rhs.insert(i / 2 + variant);
                        b.insert(i / 2 + variant);
                    } else {
                        rhs.emplace(i * 2 + (variant == 1), i + (variant == 2));
                        b.emplace(i * 2 + (variant == 1), i + (variant == 2));
                    }
                }
                check_relations(lhs, rhs, a, b);
            }
        }
    }
}

TEST(BtreeValueComparison, AllFamiliesUseStoredValueSequences) {
    check_sequences<wjr::btree_map<int, int>, std::map<int, int>>();
    check_sequences<wjr::experimental::compact_btree_map<int, int>, std::map<int, int>>();
    check_sequences<wjr::btree_set<int>, std::set<int>>();
    check_sequences<wjr::btree_multiset<int>, std::multiset<int>>();
    check_sequences<wjr::btree_set<int, std::greater<>, std::allocator<char>, 5>,
                    std::set<int, std::greater<>>>();
    check_sequences<wjr::btree_multiset<int, std::less<>, std::allocator<char>, 32>,
                    std::multiset<int, std::less<>>>();
}

struct less_only {
    int value;
    friend bool operator<(const less_only &a, const less_only &b) noexcept {
        return a.value < b.value;
    }
};
struct equal_only {
    int value;
    friend bool operator==(const equal_only &, const equal_only &) = default;
};
struct equal_key_order {
    bool operator()(const equal_only &a, const equal_only &b) const noexcept {
        return a.value < b.value;
    }
};

TEST(BtreeValueComparison, LessOnlyAndEqualityOnlyDomainsRemainIndependent) {
    using ordered = wjr::btree_set<less_only>;
    static_assert(!std::equality_comparable<less_only>);
    static_assert(std::same_as<decltype(std::declval<ordered>() <=> std::declval<ordered>()),
                               std::weak_ordering>);
    ordered a{{1}, {3}}, b{{1}, {5}};
    std::set<less_only> expected_a{{1}, {3}}, expected_b{{1}, {5}};
    EXPECT_EQ(a <=> b, expected_a <=> expected_b);
    EXPECT_EQ(a < b, expected_a < expected_b);
    wjr::btree_map<int, less_only> map_a{{1, {3}}}, map_b{{1, {5}}};
    std::map<int, less_only> std_a{{1, {3}}}, std_b{{1, {5}}};
    EXPECT_EQ(map_a <=> map_b, std_a <=> std_b);
    EXPECT_EQ(map_a < map_b, std_a < std_b);
    wjr::experimental::compact_btree_map<int, less_only> compact_less_a{{1, {3}}},
        compact_less_b{{1, {5}}};
    EXPECT_EQ(compact_less_a <=> compact_less_b, std_a <=> std_b);
    EXPECT_EQ(compact_less_a < compact_less_b, std_a < std_b);
    using equal_set = wjr::btree_set<equal_only, equal_key_order>;
    using equal_map = wjr::btree_map<int, equal_only>;
    using equal_compact = wjr::experimental::compact_btree_map<int, equal_only>;
    static_assert(!has_order<equal_set>);
    static_assert(!has_order<equal_map>);
    static_assert(!has_order<equal_compact>);
    equal_set set_a{{1}, {3}}, set_b{{1}, {3}};
    EXPECT_TRUE(set_a == set_b);
    EXPECT_FALSE(set_a != set_b);
    equal_map equal_a{{1, {3}}}, equal_b{{1, {3}}};
    EXPECT_TRUE(equal_a == equal_b);
    equal_compact compact_a{{1, {3}}}, compact_b{{1, {3}}};
    EXPECT_TRUE(compact_a == compact_b);
}

struct weak_value {
    int value;
    friend std::weak_ordering operator<=>(const weak_value &a, const weak_value &b) noexcept {
        return (a.value / 10) <=> (b.value / 10);
    }
    friend bool operator==(const weak_value &a, const weak_value &b) noexcept {
        return a.value / 10 == b.value / 10;
    }
};

TEST(BtreeValueComparison, WeakCategoryAndDefaultedCompositeComparison) {
    using weak_map = wjr::btree_map<int, weak_value>;
    static_assert(std::same_as<decltype(std::declval<weak_map>() <=> std::declval<weak_map>()),
                               std::weak_ordering>);
    weak_map a{{1, {1}}}, b{{1, {9}}};
    std::map<int, weak_value> expected_a{{1, {1}}}, expected_b{{1, {9}}};
    check_relations(a, b, expected_a, expected_b);
    struct snapshot {
        wjr::btree_set<int> keys;
        wjr::btree_map<int, int> values;
        auto operator<=>(const snapshot &) const = default;
    };
    static_assert(std::three_way_comparable<snapshot>);
    snapshot first{{1, 2}, {{3, 4}}}, second{{1, 2}, {{3, 5}}};
    EXPECT_LT(first, second);
    EXPECT_FALSE(first == second);
    EXPECT_EQ(first <=> first, std::strong_ordering::equal);
}

template <typename Tree>
void check_nan() {
    static_assert(std::same_as<decltype(std::declval<Tree>() <=> std::declval<Tree>()),
                               std::partial_ordering>);
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const std::vector<std::map<int, double>> cases{
        {},         {{1, 1.0}}, {{1, nan}}, {{1, nan}, {2, 0.0}}, {{1, nan}, {3, 0.0}},
        {{2, nan}}, {{1, 2.0}}};
    for (const auto &a : cases) {
        for (const auto &b : cases) {
            Tree lhs(a.begin(), a.end()), rhs(b.begin(), b.end());
            check_relations(lhs, rhs, a, b);
            EXPECT_EQ(lhs == lhs, a == a);
        }
    }
}

TEST(BtreeValueComparison, MappedNaNPropagatesUnorderedToLegacyRelations) {
    check_nan<wjr::btree_map<int, double>>();
    check_nan<wjr::experimental::compact_btree_map<int, double>>();
}

struct key_order {
    bool reverse;
    int *calls;
    key_order() = delete;
    key_order(bool reverse, int &calls) : reverse(reverse), calls(&calls) {}
    bool operator()(int a, int b) const noexcept {
        ++*calls;
        return reverse ? a > b : a < b;
    }
};

TEST(BtreeValueComparison, ComparisonDoesNotConsultStatefulKeyOrdering) {
    int calls = 0;
    wjr::btree_map<int, int, key_order> lhs(key_order(false, calls)), rhs(key_order(true, calls));
    std::map<int, int, key_order> a(key_order(false, calls)), b(key_order(true, calls));
    for (int i = 0; i < 20; ++i) {
        lhs.emplace(i, i);
        rhs.emplace(i, i);
        a.emplace(i, i);
        b.emplace(i, i);
    }
    calls = 0;
    check_relations(lhs, rhs, a, b);
    EXPECT_EQ(calls, 0);
}

TEST(BtreeValueComparison, TransparentStringOrderingUsesValueSemantics) {
    wjr::btree_map<std::string, int, std::less<>> lhs{{"alpha", 1}, {"beta", 2}},
        rhs{{"alpha", 1}, {"gamma", 2}};
    std::map<std::string, int, std::less<>> a{{"alpha", 1}, {"beta", 2}},
        b{{"alpha", 1}, {"gamma", 2}};
    check_relations(lhs, rhs, a, b);
    EXPECT_EQ(lhs.find(std::string_view("beta"))->second, 2);
}

struct throwing_value {
    int value;
    friend bool operator==(const throwing_value &, const throwing_value &) {
        throw std::runtime_error("equal");
    }
    friend std::strong_ordering operator<=>(const throwing_value &, const throwing_value &) {
        throw std::runtime_error("order");
    }
};
TEST(BtreeValueComparison, ValueExceptionsPropagateWithoutChangingTheTrees) {
    using tree = wjr::btree_map<int, throwing_value>;
    static_assert(!noexcept(std::declval<const tree &>() <=> std::declval<const tree &>()));
    tree lhs{{1, {2}}}, rhs{{1, {3}}};
    EXPECT_THROW((void)(lhs <=> rhs), std::runtime_error);
    EXPECT_THROW((void)(lhs == rhs), std::runtime_error);
    EXPECT_THROW((void)(lhs == lhs), std::runtime_error);
    EXPECT_THROW((void)(lhs < rhs), std::runtime_error);
    EXPECT_EQ(lhs.at(1).value, 2);
    EXPECT_EQ(rhs.at(1).value, 3);
}

TEST(BtreeValueComparison, ExistingExplicitMapFunctionTemplatesRemainUsable) {
    using tree = wjr::btree_map<int, int, std::less<int>, std::allocator<char>, 16>;
    auto equal = &wjr::operator== <int, int, std::less<int>, std::allocator<char>, 16>;
    auto less = &wjr::operator< <int, int, std::less<int>, std::allocator<char>, 16>;
    tree lhs{{1, 2}}, rhs{{1, 3}};
    EXPECT_FALSE(equal(lhs, rhs));
    EXPECT_TRUE(less(lhs, rhs));
}
} // namespace
