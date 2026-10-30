#include <cstdint>
#include <map>
#include <memory>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

namespace {
template <typename T>
inline constexpr char allocation_type = 0;

struct allocation_record {
    const void *type;
    size_t count;
    int owner;
};

struct aligned_allocation_state {
    static inline std::map<void *, allocation_record> allocations;
    static inline int selected = 0;
    static inline int byte_allocations = 0;
    static inline int value_constructions = 0;
    static inline int value_destructions = 0;
};

template <typename T>
struct aligned_allocator {
    using value_type = T;
    int owner = aligned_allocation_state::selected;

    aligned_allocator() noexcept = default;
    template <typename U>
    aligned_allocator(const aligned_allocator<U> &other) noexcept : owner(other.owner) {}

    T *allocate(size_t count) {
        if constexpr (std::is_same_v<T, char>) {
            ++aligned_allocation_state::byte_allocations;
        }
        auto *ptr = std::allocator<T>{}.allocate(count);
        aligned_allocation_state::allocations.emplace(
            ptr, allocation_record{&allocation_type<T>, count, owner});
        return ptr;
    }

    void deallocate(T *ptr, size_t count) noexcept {
        auto iter = aligned_allocation_state::allocations.find(ptr);
        ASSERT_NE(iter, aligned_allocation_state::allocations.end());
        EXPECT_EQ(iter->second.type, &allocation_type<T>);
        EXPECT_EQ(iter->second.count, count);
        EXPECT_EQ(iter->second.owner, owner);
        aligned_allocation_state::allocations.erase(iter);
        std::allocator<T>{}.deallocate(ptr, count);
    }

    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        EXPECT_TRUE((std::is_same_v<T, char>));
        if constexpr (alignof(U) >= 64) {
            ++aligned_allocation_state::value_constructions;
        }
        std::construct_at(ptr, std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U *ptr) {
        EXPECT_TRUE((std::is_same_v<T, char>));
        if constexpr (alignof(U) >= 64) {
            ++aligned_allocation_state::value_destructions;
        }
        std::destroy_at(ptr);
    }

    bool operator==(const aligned_allocator &) const = default;
};

struct alignas(64) aligned_value {
    int key;
    explicit aligned_value(int k = 0) noexcept : key(k) {}
    bool operator<(const aligned_value &other) const noexcept { return key < other.key; }
};

template <typename Tree>
void fill_aligned_tree(Tree &tree, int count) {
    for (int i = 0; i != count; ++i) {
        if constexpr (std::is_void_v<typename Tree::mapped_type>) {
            tree.emplace(i);
        } else {
            tree.try_emplace(i, i);
        }
    }
    EXPECT_EQ(tree.size(), static_cast<size_t>(count));
    for (const auto &value : tree) {
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(std::addressof(value)) %
                      alignof(typename Tree::value_type),
                  0u);
    }
}

template <typename Tree>
void check_typed_allocations() {
    aligned_allocation_state::byte_allocations = 0;
    aligned_allocation_state::value_constructions = 0;
    aligned_allocation_state::value_destructions = 0;
    aligned_allocation_state::selected = 42;
    {
        Tree tree;
        fill_aligned_tree(tree, 2);
        constexpr bool is_inline = wjr::btree_detail::is_inline_key_v<typename Tree::value_type>;
        EXPECT_EQ(aligned_allocation_state::allocations.size(), is_inline ? 0u : 2u);
        fill_aligned_tree(tree, 160);
        EXPECT_EQ(aligned_allocation_state::byte_allocations, 0);
        aligned_allocation_state::selected = 99;
        Tree copy(tree);
        Tree moved(std::move(copy));
        EXPECT_TRUE(copy.empty());
        fill_aligned_tree(moved, 160);
        while (tree.size() > 1) {
            tree.erase(tree.begin());
        }
        tree.clear();
        fill_aligned_tree(tree, 80);
        moved.clear();
    }
    EXPECT_TRUE(aligned_allocation_state::allocations.empty());
    EXPECT_EQ(aligned_allocation_state::value_constructions,
              aligned_allocation_state::value_destructions);
    aligned_allocation_state::selected = 0;
}

TEST(BtreeAllocation, DefaultAllocatorSupportsOveralignedValues) {
    wjr::btree_map<int, aligned_value> map;
    fill_aligned_tree(map, 160);
    wjr::btree_set<aligned_value> set;
    fill_aligned_tree(set, 160);
}

TEST(BtreeAllocation, RebindsNodesAndValuesAndPreservesHooks) {
    using allocator = aligned_allocator<char>;
    check_typed_allocations<wjr::btree_set<int, std::less<int>, allocator>>();
    check_typed_allocations<wjr::btree_map<int, int, std::less<int>, allocator>>();
    check_typed_allocations<wjr::btree_set<aligned_value, std::less<aligned_value>, allocator>>();
    check_typed_allocations<wjr::btree_map<int, aligned_value, std::less<int>, allocator>>();
    check_typed_allocations<
        wjr::btree_set<unsigned char, std::less<unsigned char>, allocator, 5>>();
}
} // namespace
