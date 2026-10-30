#include <cstdint>
#include <map>
#include <memory_resource>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

namespace {
class tracking_resource : public std::pmr::memory_resource {
    struct block {
        size_t bytes;
        size_t alignment;
    };
    std::map<void *, block> blocks;
    void *do_allocate(size_t bytes, size_t alignment) override {
        void *ptr = std::pmr::new_delete_resource()->allocate(bytes, alignment);
        blocks.emplace(ptr, block{bytes, alignment});
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % alignment, 0u);
        if (alignment >= 64)
            ++overaligned_allocations;
        ++allocations;
        return ptr;
    }
    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
        auto iter = blocks.find(ptr);
        ASSERT_NE(iter, blocks.end());
        EXPECT_EQ(iter->second.bytes, bytes);
        EXPECT_EQ(iter->second.alignment, alignment);
        blocks.erase(iter);
        ++deallocations;
        std::pmr::new_delete_resource()->deallocate(ptr, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }

public:
    size_t allocations = 0;
    size_t deallocations = 0;
    size_t overaligned_allocations = 0;
    bool empty() const noexcept { return blocks.empty(); }
};

struct alignas(64) payload {
    int value;
    explicit payload(int v = 0) noexcept : value(v) {}
};

TEST(BtreeAllocatorConstruction, ExplicitResourceOwnsValuesAndNodes) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    using tree = wjr::btree_map<int, payload, std::less<int>, allocator>;
    static_assert(std::is_nothrow_constructible_v<tree, const std::less<int> &, const allocator &>);
    static_assert(
        std::is_same_v<decltype(std::declval<const tree &>().get_allocator()), allocator>);
    tracking_resource first, second;
    {
        tree lhs(std::less<int>{}, allocator{&first});
        tree rhs(std::less<int>{}, allocator{&second});
        EXPECT_EQ(lhs.get_allocator().resource(), &first);
        EXPECT_EQ(rhs.get_allocator().resource(), &second);
        for (int i = 0; i != 80; ++i) {
            lhs.try_emplace(i, i);
            rhs.try_emplace(i, i + 100);
        }
        EXPECT_GT(first.overaligned_allocations, 0u);
        EXPECT_GT(second.overaligned_allocations, 0u);
        for (const auto &[key, value] : lhs) {
            EXPECT_EQ(value.value, key);
            EXPECT_EQ(reinterpret_cast<std::uintptr_t>(&value) % 64, 0u);
        }
        tree moved(std::move(lhs));
        EXPECT_EQ(moved.get_allocator().resource(), &first);
        while (!moved.empty())
            moved.erase(moved.begin());
        EXPECT_TRUE(first.empty());
        rhs.clear();
        EXPECT_TRUE(second.empty());
    }
    EXPECT_TRUE(first.empty());
    EXPECT_TRUE(second.empty());
    EXPECT_EQ(first.allocations, first.deallocations);
    EXPECT_EQ(second.allocations, second.deallocations);
}

TEST(BtreeAllocatorConstruction, PmrStringsAndSetUseTheProvidedResource) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    tracking_resource resource;
    {
        wjr::btree_map<std::pmr::string, std::pmr::string, std::less<>, allocator> map(
            std::less<>{}, allocator{&resource});
        const std::string long_text(128, 'x');
        map.emplace(long_text.c_str(), long_text.c_str());
        EXPECT_EQ(map.begin()->first.get_allocator().resource(), &resource);
        EXPECT_EQ(map.begin()->second.get_allocator().resource(), &resource);
        wjr::btree_set<int, std::less<int>, allocator> set(std::less<int>{}, allocator{&resource});
        EXPECT_EQ(set.get_allocator().resource(), &resource);
        for (int i = 0; i != 80; ++i)
            set.emplace(i);
        set.clear();
        EXPECT_EQ(map.begin()->second.size(), 128u);
    }
    EXPECT_TRUE(resource.empty());
}

struct construction_counts {
    static inline int defaults = 0;
    static inline int copies = 0;
    static inline int moves = 0;
    static void reset() noexcept { defaults = copies = moves = 0; }
};
template <typename T>
struct counted_allocator {
    using value_type = T;
    counted_allocator() noexcept { ++construction_counts::defaults; }
    counted_allocator(const counted_allocator &) noexcept { ++construction_counts::copies; }
    counted_allocator(counted_allocator &&) noexcept { ++construction_counts::moves; }
    template <typename U>
    counted_allocator(const counted_allocator<U> &) noexcept {}
    T *allocate(size_t n) { return std::allocator<T>{}.allocate(n); }
    void deallocate(T *p, size_t n) noexcept { std::allocator<T>{}.deallocate(p, n); }
    bool operator==(const counted_allocator &) const noexcept { return true; }
};

TEST(BtreeAllocatorConstruction, ExistingConstructorsKeepDirectAllocatorConstruction) {
    using allocator = counted_allocator<char>;
    using tree = wjr::btree_set<int, std::less<int>, allocator>;
    construction_counts::reset();
    { tree values; }
    EXPECT_EQ(construction_counts::defaults, 1);
    EXPECT_EQ(construction_counts::copies, 0);
    EXPECT_EQ(construction_counts::moves, 0);
    construction_counts::reset();
    { tree values(std::less<int>{}); }
    EXPECT_EQ(construction_counts::defaults, 1);
    EXPECT_EQ(construction_counts::copies, 0);
    EXPECT_EQ(construction_counts::moves, 0);
    allocator supplied;
    construction_counts::reset();
    { tree values(std::less<int>{}, supplied); }
    EXPECT_EQ(construction_counts::defaults, 0);
    EXPECT_EQ(construction_counts::copies, 1);
    EXPECT_EQ(construction_counts::moves, 0);
    using normal = wjr::btree_map<int, int>;
    using traits = wjr::btree_traits<int, int, false>;
    static_assert(sizeof(normal) == sizeof(wjr::btree_root_node<traits>));
}
} // namespace
