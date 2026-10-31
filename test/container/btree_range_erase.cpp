#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <algorithm>
#include <map>
#include <memory_resource>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {
template <typename Tree, typename Reference>
void check_ranges() {
    for (int count : {0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17, 31, 32, 33, 80, 257}) {
        std::vector<int> order(count);
        std::iota(order.begin(), order.end(), 0);
        std::mt19937 random(159106 + count);
        std::shuffle(order.begin(), order.end(), random);
        std::vector<int> cuts;
        if (count <= 80) {
            for (int i = 0; i <= count; ++i)
                cuts.push_back(i);
        } else {
            cuts = {0, 1, 2, 7, 8, 9, 63, 64, 65, 127, 128, 129, count - 1, count};
        }
        for (int start : cuts) {
            for (int finish : cuts) {
                if (finish < start)
                    continue;
                Tree actual;
                Reference expected;
                for (int key : order) {
                    if constexpr (std::same_as<typename Tree::key_type,
                                               typename Tree::value_type>) {
                        actual.insert(key);
                        expected.insert(key);
                    } else {
                        actual.emplace(key, key * 3);
                        expected.emplace(key, key * 3);
                    }
                }
                auto first = actual.cbegin(), last = actual.cbegin();
                auto ref_first = expected.cbegin(), ref_last = expected.cbegin();
                std::advance(first, start);
                std::advance(last, finish);
                std::advance(ref_first, start);
                std::advance(ref_last, finish);
                auto next = actual.erase(first, last);
                auto ref_next = expected.erase(ref_first, ref_last);
                ASSERT_EQ(next == actual.end(), ref_next == expected.end())
                    << count << ':' << start << ':' << finish;
                if (ref_next != expected.end())
                    EXPECT_EQ(*next, *ref_next);
                ASSERT_EQ(actual.size(), expected.size());
                ASSERT_TRUE(
                    std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()))
                    << count << ':' << start << ':' << finish;
                for (int key : {0, start - 1, start, finish - 1, finish, count}) {
                    auto lhs = actual.lower_bound(key);
                    auto rhs = expected.lower_bound(key);
                    ASSERT_EQ(lhs == actual.end(), rhs == expected.end());
                    if (rhs != expected.end())
                        EXPECT_EQ(*lhs, *rhs);
                }
                if constexpr (std::same_as<typename Tree::key_type, typename Tree::value_type>) {
                    actual.insert(count + 1);
                    expected.insert(count + 1);
                } else {
                    actual.emplace(count + 1, 42);
                    expected.emplace(count + 1, 42);
                }
                EXPECT_TRUE(
                    std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
            }
        }
    }
}

TEST(BtreeRangeErase, EverySmallRangeAndMultiLevelBoundary) {
    check_ranges<wjr::btree_set<int, std::less<>, std::allocator<char>, 4>, std::set<int>>();
    check_ranges<wjr::btree_set<int, std::greater<>, std::allocator<char>, 5>,
                 std::set<int, std::greater<>>>();
    check_ranges<wjr::btree_map<int, int, std::less<>, std::allocator<char>, 16>,
                 std::map<int, int>>();
    check_ranges<
        wjr::experimental::compact_btree_map<int, int, std::less<>, std::allocator<char>, 16>,
        std::map<int, int>>();
}

struct alignas(64) range_value {
    static inline unsigned moves = 0;
    static inline unsigned live = 0;
    const range_value *self = this;
    range_value() noexcept { ++live; }
    range_value(const range_value &) = delete;
    range_value(range_value &&other) noexcept {
        EXPECT_EQ(other.self, &other);
        ++moves;
        ++live;
    }
    ~range_value() noexcept {
        EXPECT_EQ(self, this);
        --live;
    }
};

TEST(BtreeRangeErase, CompactLifetimesAndBoundedSurvivorRelocation) {
    for (auto [first, last] : {std::pair{1024, 3072}, {1025, 3071}, {1, 3071}}) {
        {
            wjr::experimental::compact_btree_map<int, range_value> tree;
            for (int i = 0; i != 4096; ++i)
                tree.try_emplace(i);
            range_value::moves = 0;
            auto next = tree.erase(tree.lower_bound(first), tree.lower_bound(last));
            EXPECT_EQ(next->first, last);
            EXPECT_EQ(tree.size(), static_cast<size_t>(4096 - (last - first)));
            EXPECT_EQ(range_value::live, tree.size());
            if (first != 1)
                EXPECT_LT(range_value::moves, static_cast<unsigned>(last - first));
            for (const auto &[key, value] : tree) {
                EXPECT_EQ(value.self, &value);
                EXPECT_TRUE(key < first || key >= last);
            }
        }
        EXPECT_EQ(range_value::live, 0u);
    }
}

TEST(BtreeRangeErase, IndirectKeysAndSurvivingValueAddresses) {
    wjr::btree_map<std::string, std::string> tree;
    for (int i = 0; i != 4096; ++i) {
        const auto key = std::to_string(100000 + i);
        tree.emplace(key, std::string(80, 'a') + key);
    }
    const auto *first_value = &*tree.begin();
    const auto *last_value = &*std::prev(tree.end());
    auto next = tree.erase(tree.lower_bound("101024"), tree.lower_bound("103072"));
    EXPECT_EQ(next->first, "103072");
    EXPECT_EQ(&*tree.begin(), first_value);
    EXPECT_EQ(&*std::prev(tree.end()), last_value);
    for (int i = 0; i != 4096; ++i) {
        const auto key = std::to_string(100000 + i);
        EXPECT_EQ(tree.count(key), i < 1024 || i >= 3072 ? 1u : 0u);
    }
}

TEST(BtreeRangeErase, DuplicateKeyGroupsUseTheSameRangePath) {
    wjr::btree_multiset<std::string, std::less<>, std::allocator<char>, 5> actual;
    std::multiset<std::string> expected;
    for (int i = 0; i != 2048; ++i) {
        auto key = std::string(80, 'x') + std::to_string(i % 9);
        actual.insert(key);
        expected.insert(key);
    }
    for (int group : {4, 0, 8, 3, 5, 1, 7, 2, 6}) {
        auto key = std::string(80, 'x') + std::to_string(group);
        EXPECT_EQ(actual.erase(key), expected.erase(key));
        EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }
    EXPECT_TRUE(actual.empty());
}

class range_resource final : public std::pmr::memory_resource {
public:
    std::map<void *, std::pair<size_t, size_t>> blocks;

private:
    void *do_allocate(size_t bytes, size_t alignment) override {
        void *ptr = std::pmr::new_delete_resource()->allocate(bytes, alignment);
        blocks.emplace(ptr, std::pair{bytes, alignment});
        return ptr;
    }
    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
        const auto entry = blocks.find(ptr);
        ASSERT_NE(entry, blocks.end());
        EXPECT_EQ(entry->second, (std::pair{bytes, alignment}));
        blocks.erase(entry);
        std::pmr::new_delete_resource()->deallocate(ptr, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
};

template <typename Tree>
void check_allocator() {
    range_resource resource;
    {
        Tree tree(std::less<>{}, typename Tree::allocator_type(&resource));
        for (int i = 0; i != 2048; ++i)
            tree.try_emplace(i);
        tree.erase(tree.lower_bound(257), tree.lower_bound(1791));
        tree.erase(tree.begin(), tree.lower_bound(129));
        tree.erase(tree.lower_bound(1900), tree.end());
        for (const auto &[key, value] : tree) {
            EXPECT_EQ(value.self, &value);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(&value) % 64, 0u);
            EXPECT_TRUE((key >= 129 && key < 257) || (key >= 1791 && key < 1900));
        }
        EXPECT_EQ(range_value::live, tree.size());
    }
    EXPECT_TRUE(resource.blocks.empty());
    EXPECT_EQ(range_value::live, 0u);
}

TEST(BtreeRangeErase, TypedPmrAllocationAndOveralignedDestruction) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    check_allocator<wjr::btree_map<int, range_value, std::less<>, allocator, 16>>();
    check_allocator<
        wjr::experimental::compact_btree_map<int, range_value, std::less<>, allocator, 16>>();
}
} // namespace
