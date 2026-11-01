#include <wjr/container/btree_map.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <map>
#include <memory_resource>
#include <utility>

#include <gtest/gtest.h>

namespace {
class tracking_resource : public std::pmr::memory_resource {
    void *do_allocate(size_t bytes, size_t alignment) override {
        ++allocations;
        live_bytes += bytes;
        return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    }
    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
        live_bytes -= bytes;
        std::pmr::new_delete_resource()->deallocate(ptr, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }

public:
    size_t allocations = 0;
    size_t live_bytes = 0;
};

struct source {
    int number;
    bool consumed = false;
    explicit source(int n) : number(n) {}
    source(const source &) = delete;
};

struct mapped {
    using allocator_type = std::pmr::polymorphic_allocator<char>;
    std::pmr::memory_resource *resource;
    int number;
    static inline int input_constructions = 0;
    static inline int assignments = 0;

    mapped(std::allocator_arg_t, allocator_type alloc, int n) noexcept
        : resource(alloc.resource()), number(n) {
        ++input_constructions;
    }
    mapped(std::allocator_arg_t, allocator_type alloc, source &&input) noexcept
        : mapped(std::allocator_arg, alloc, input.number) {
        EXPECT_FALSE(std::exchange(input.consumed, true));
    }
    mapped(std::allocator_arg_t, allocator_type alloc, mapped &&other) noexcept
        : resource(alloc.resource()), number(other.number) {}
    mapped(mapped &&) noexcept = default;
    mapped &operator=(int n) noexcept {
        ++assignments;
        number = n;
        return *this;
    }
    mapped &operator=(source &&input) noexcept {
        EXPECT_FALSE(std::exchange(input.consumed, true));
        return *this = input.number;
    }
};

using allocator = mapped::allocator_type;
template <typename Value>
using legacy = wjr::btree_map<int, Value, std::less<int>, allocator, 5>;
template <typename Value>
using compact = wjr::experimental::compact_btree_map<int, Value, std::less<int>, allocator, 5>;

template <typename Tree, typename Source>
concept can_upsert = requires(Tree &tree, const int &key, Source &&input) {
    tree.insert_or_assign(key, std::forward<Source>(input));
    tree.insert_or_assign(int(key), std::forward<Source>(input));
    tree.insert_or_assign(tree.cend(), key, std::forward<Source>(input));
    tree.insert_or_assign(tree.cend(), int(key), std::forward<Source>(input));
};

template <typename Tree>
void check_all_overloads() {
    static_assert(!std::constructible_from<mapped, int>);
    static_assert(can_upsert<Tree, int>);
    static_assert(can_upsert<Tree, source>);
    for (int initial : {0, 1, 2, 3, 80}) {
        tracking_resource resource;
        {
            Tree tree(std::less<int>{}, allocator(&resource));
            std::pmr::map<int, mapped> expected;
            for (int i = 0; i < initial; ++i) {
                tree.try_emplace(i, i);
                expected.try_emplace(i, i);
            }
            for (int operation = 0; operation < 4; ++operation) {
                const int key = initial + operation;
                const int number = 100 + operation;
                if (operation == 0)
                    EXPECT_TRUE(tree.insert_or_assign(key, number).second);
                else if (operation == 1)
                    EXPECT_TRUE(tree.insert_or_assign(int(key), number).second);
                else if (operation == 2)
                    EXPECT_EQ(tree.insert_or_assign(tree.cend(), key, number)->first, key);
                else
                    EXPECT_EQ(tree.insert_or_assign(tree.cbegin(), int(key), number)->first, key);
                expected.insert_or_assign(key, number);
                EXPECT_EQ(tree.at(key).resource, &resource);

                const auto allocations = resource.allocations;
                const auto constructions = mapped::input_constructions;
                if (operation == 0)
                    EXPECT_FALSE(tree.insert_or_assign(key, number + 1).second);
                else if (operation == 1)
                    EXPECT_FALSE(tree.insert_or_assign(int(key), number + 1).second);
                else if (operation == 2)
                    EXPECT_EQ(tree.insert_or_assign(tree.cend(), key, number + 1)->first, key);
                else
                    EXPECT_EQ(tree.insert_or_assign(tree.cbegin(), int(key), number + 1)->first,
                              key);
                EXPECT_EQ(resource.allocations, allocations);
                EXPECT_EQ(mapped::input_constructions, constructions);
                expected.insert_or_assign(key, number + 1);
            }
            ASSERT_EQ(tree.size(), expected.size());
            auto reference = expected.begin();
            for (const auto &[key, value] : tree) {
                EXPECT_EQ(key, reference->first);
                EXPECT_EQ(value.number, reference->second.number);
                EXPECT_EQ(value.resource, &resource);
                ++reference;
            }
        }
        EXPECT_EQ(resource.live_bytes, 0u);
    }
}

TEST(BtreeAllocatorUpsert, AllKeyAndHintOverloadsUseTargetResource) {
    check_all_overloads<legacy<mapped>>();
    check_all_overloads<compact<mapped>>();
}

template <typename Tree>
void check_consumption() {
    tracking_resource resource;
    {
        Tree tree(std::less<int>{}, allocator(&resource));
        source fresh(17);
        EXPECT_TRUE(tree.insert_or_assign(1, std::move(fresh)).second);
        EXPECT_TRUE(fresh.consumed);
        source replacement(23);
        const auto allocations = resource.allocations;
        const auto constructions = mapped::input_constructions;
        const auto assignments = mapped::assignments;
        tree.insert_or_assign(tree.cend(), 1, std::move(replacement));
        EXPECT_TRUE(replacement.consumed);
        EXPECT_EQ(mapped::assignments, assignments + 1);
        EXPECT_EQ(mapped::input_constructions, constructions);
        EXPECT_EQ(resource.allocations, allocations);
        EXPECT_EQ(tree.at(1).number, 23);
    }
    EXPECT_EQ(resource.live_bytes, 0u);
}

TEST(BtreeAllocatorUpsert, MoveOnlySourceIsConsumedExactlyOnceOnEitherBranch) {
    check_consumption<legacy<mapped>>();
    check_consumption<compact<mapped>>();
}

template <typename Tree>
void check_pair() {
    static_assert(!std::constructible_from<typename Tree::mapped_type, std::pair<int, int>>);
    static_assert(can_upsert<Tree, std::pair<int, int>>);
    tracking_resource resource;
    {
        Tree tree(std::less<int>{}, allocator(&resource));
        tree.insert_or_assign(1, std::pair{7, 8});
        EXPECT_EQ(tree.at(1).first.resource, &resource);
        EXPECT_EQ(tree.at(1).first.number, 7);
        const auto allocations = resource.allocations;
        tree.insert_or_assign(tree.cbegin(), 1, std::pair{9, 10});
        EXPECT_EQ(tree.at(1).first.resource, &resource);
        EXPECT_EQ(tree.at(1).first.number, 9);
        EXPECT_EQ(tree.at(1).second, 10);
        EXPECT_EQ(resource.allocations, allocations);
    }
    EXPECT_EQ(resource.live_bytes, 0u);
}

TEST(BtreeAllocatorUpsert, PairMappedConstructionChecksAllocatorAwareComponents) {
    check_pair<legacy<std::pair<mapped, int>>>();
    check_pair<compact<std::pair<mapped, int>>>();
}

struct unconstructible {};
struct no_assignment {
    using allocator_type = allocator;
    no_assignment(std::allocator_arg_t, allocator_type, int) noexcept {}
    no_assignment(no_assignment &&) noexcept = default;
};

TEST(BtreeAllocatorUpsert, InvalidSourcesFailConstraintsWithoutInstantiatingPmrConstructBody) {
    static_assert(!can_upsert<legacy<mapped>, unconstructible>);
    static_assert(!can_upsert<compact<mapped>, unconstructible>);
    static_assert(!can_upsert<legacy<no_assignment>, int>);
    static_assert(!can_upsert<compact<no_assignment>, int>);
    static_assert(!can_upsert<wjr::btree_map<int, mapped>, int>);
    static_assert(!can_upsert<wjr::experimental::compact_btree_map<int, mapped>, int>);
    static_assert(can_upsert<wjr::btree_map<int, int>, int>);
    static_assert(can_upsert<wjr::experimental::compact_btree_map<int, int>, int>);
}
} // namespace
