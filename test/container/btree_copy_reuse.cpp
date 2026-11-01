#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <functional>
#include <map>
#include <memory_resource>
#include <string>

#include <gtest/gtest.h>

namespace {
class resource : public std::pmr::memory_resource {
    void *do_allocate(size_t bytes, size_t alignment) override {
        ++allocations;
        live_bytes += bytes;
        return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    }
    void do_deallocate(void *p, size_t bytes, size_t alignment) override {
        ++deallocations;
        live_bytes -= bytes;
        std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }
    bool do_is_equal(const memory_resource &other) const noexcept override {
        return this == &other;
    }

public:
    size_t allocations = 0, deallocations = 0, live_bytes = 0;
};
using allocator = std::pmr::polymorphic_allocator<char>;

template <typename Tree>
void check_sizes(bool duplicates = false) {
    for (int count : {0, 1, 2, 3, 17, 80, 1024}) {
        resource first, second;
        {
            Tree source(std::less<>{}, allocator(&first)),
                target(std::less<>{}, allocator(&second));
            for (int i = 0; i < count; ++i) {
                if constexpr (std::is_void_v<typename Tree::mapped_type>) {
                    source.emplace(duplicates ? i / 2 : i);
                    target.emplace(10000 + (duplicates ? i / 2 : i));
                } else {
                    source.try_emplace(i, i * 3);
                    target.try_emplace(10000 + i, -i);
                }
            }
            const auto allocations = second.allocations;
            const auto deallocations = second.deallocations;
            const auto bytes = second.live_bytes;
            target = source;
            EXPECT_EQ(second.allocations, allocations);
            EXPECT_EQ(second.deallocations, deallocations);
            EXPECT_EQ(second.live_bytes, bytes);
            EXPECT_EQ(target.get_allocator().resource(), &second);
            EXPECT_EQ(source.get_allocator().resource(), &first);
            ASSERT_EQ(target.size(), source.size());
            EXPECT_TRUE(std::equal(target.begin(), target.end(), source.begin()));
            for (int i = 0; i < count; ++i) {
                EXPECT_NE(target.find(duplicates ? i / 2 : i), target.end());
                if constexpr (!std::is_void_v<typename Tree::mapped_type>)
                    EXPECT_EQ(target.at(i), i * 3);
            }
            auto &alias = target;
            target = alias;
            EXPECT_EQ(second.allocations, allocations);
            target.clear();
            EXPECT_TRUE(target.empty());
            EXPECT_EQ(second.live_bytes, 0u);
            if (count != 0)
                target.emplace(*source.begin());
        }
        EXPECT_EQ(first.live_bytes, 0u);
        EXPECT_EQ(second.live_bytes, 0u);
    }
}

TEST(BtreeCopyReuse, MatchedShapesReuseAllocationsForAllFamilies) {
    check_sizes<wjr::btree_map<int, int, std::less<>, allocator, 5>>();
    check_sizes<wjr::experimental::compact_btree_map<int, int, std::less<>, allocator, 5>>();
    check_sizes<wjr::btree_set<int, std::less<>, allocator, 5>>();
    check_sizes<wjr::btree_multiset<int, std::less<>, allocator, 5>>(true);
}

template <typename Tree>
void check_mismatch() {
    resource first, second;
    {
        Tree source(std::less<>{}, allocator(&first)), target(std::less<>{}, allocator(&second));
        for (int i = 0; i < 80; ++i) {
            source.try_emplace(i, i);
            target.try_emplace(i + 10000, i);
        }
        target.erase(target.begin());
        target.try_emplace(20000, 1);
        const auto allocations = second.allocations;
        target = source;
        EXPECT_GT(second.allocations, allocations);
        EXPECT_EQ(second.live_bytes, first.live_bytes);
        for (int i = 0; i < 80; ++i)
            EXPECT_EQ(target.at(i), i);
        target.clear();
        target.try_emplace(7, 7);
        target = source;
        EXPECT_EQ(target.size(), source.size());
        source.clear();
        source.try_emplace(1, 1);
        source.try_emplace(2, 2);
        target.clear();
        target.try_emplace(5, 5);
        target.try_emplace(6, 6);
        target.try_emplace(7, 7);
        target.erase(7);
        target = source;
        EXPECT_EQ(second.live_bytes, first.live_bytes);
        EXPECT_EQ(target.at(1), 1);
        EXPECT_EQ(target.at(2), 2);
    }
    EXPECT_EQ(first.live_bytes, 0u);
    EXPECT_EQ(second.live_bytes, 0u);
}
TEST(BtreeCopyReuse, ShapeAndSizeMismatchesRetainFreshCopyAndSsoFootprint) {
    check_mismatch<wjr::btree_map<int, int, std::less<>, allocator, 5>>();
    check_mismatch<wjr::experimental::compact_btree_map<int, int, std::less<>, allocator, 5>>();
}

struct ordering {
    bool reverse;
    bool operator()(int a, int b) const noexcept { return reverse ? a > b : a < b; }
};
TEST(BtreeCopyReuse, ComparatorStateAndSeparatorsAreReplacedTogether) {
    using tree = wjr::btree_map<int, int, ordering, allocator, 5>;
    resource first, second;
    {
        tree source(ordering{true}, allocator(&first)), target(ordering{false}, allocator(&second));
        for (int i = 0; i < 80; ++i) {
            source.try_emplace(79 - i, i);
            target.try_emplace(10000 + i, i);
        }
        const auto allocations = second.allocations;
        target = source;
        EXPECT_EQ(second.allocations, allocations);
        EXPECT_TRUE(target.key_comp().reverse);
        EXPECT_EQ(target.begin()->first, 79);
        for (int i = 0; i < 80; ++i)
            EXPECT_EQ(target.at(i), 79 - i);
        target.try_emplace(100, 7);
        target.erase(40);
        EXPECT_EQ(target.begin()->first, 100);
        EXPECT_FALSE(target.contains(40));
    }
    EXPECT_EQ(first.live_bytes, 0u);
    EXPECT_EQ(second.live_bytes, 0u);
}

TEST(BtreeCopyReuse, IndirectPmrKeysKeepOwnedResourcesAndValidSeparatorPointers) {
    using tree = wjr::btree_map<std::pmr::string, std::pmr::string, std::less<>, allocator, 5>;
    resource first, second;
    {
        tree source(std::less<>{}, allocator(&first)), target(std::less<>{}, allocator(&second));
        for (int i = 0; i < 80; ++i) {
            const auto key = std::string(50, 'x') + std::to_string(i);
            source.try_emplace(key.c_str(), std::string(80, 'a').c_str());
            target.try_emplace(key.c_str(), std::string(80, 'b').c_str());
        }
        const auto bytes = second.live_bytes;
        target = source;
        EXPECT_EQ(second.live_bytes, bytes);
        for (const auto &[key, value] : target) {
            EXPECT_EQ(key.get_allocator().resource(), &second);
            EXPECT_EQ(value.get_allocator().resource(), &second);
            EXPECT_EQ(value.front(), 'a');
            EXPECT_EQ(target.find(std::string_view(key))->first, key);
        }
        source.clear();
        for (int i = 0; i < 80; ++i) {
            const auto key = std::string(50, 'x') + std::to_string(i);
            EXPECT_EQ(target.at(std::string_view(key)).size(), 80u);
        }
    }
    EXPECT_EQ(first.live_bytes, 0u);
    EXPECT_EQ(second.live_bytes, 0u);
}

struct alignas(64) counted {
    int number;
    static inline int live = 0, copies = 0;
    explicit counted(int n) noexcept : number(n) { ++live; }
    counted(const counted &other) noexcept : number(other.number) {
        ++live;
        ++copies;
    }
    counted(counted &&other) noexcept : number(other.number) { ++live; }
    ~counted() noexcept { --live; }
};
struct hooks {
    size_t allocations = 0, blocks = 0, constructs = 0, destroys = 0;
    std::function<void()> observe;
};
template <typename T>
struct hooked_allocator {
    using value_type = T;
    hooks *state = nullptr;
    hooked_allocator() = default;
    explicit hooked_allocator(hooks &s) noexcept : state(&s) {}
    template <typename U>
    hooked_allocator(const hooked_allocator<U> &other) noexcept : state(other.state) {}
    T *allocate(size_t n) {
        ++state->allocations;
        ++state->blocks;
        return std::allocator<T>{}.allocate(n);
    }
    void deallocate(T *p, size_t n) noexcept {
        --state->blocks;
        std::allocator<T>{}.deallocate(p, n);
    }
    template <typename U, typename... Args>
    void construct(U *p, Args &&...args) {
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(p) % alignof(U), 0u);
        if constexpr (std::same_as<U, std::pair<const int, counted>>) {
            ++state->constructs;
            if (state->observe)
                state->observe();
        }
        std::construct_at(p, std::forward<Args>(args)...);
    }
    template <typename U>
    void destroy(U *p) noexcept {
        if constexpr (std::same_as<U, std::pair<const int, counted>>) {
            ++state->destroys;
            if (state->observe)
                state->observe();
        }
        std::destroy_at(p);
    }
    bool operator==(const hooked_allocator &other) const noexcept { return state == other.state; }
};
template <typename Tree>
void check_hooks() {
    for (int count : {1, 2, 3, 80}) {
        hooks first, second;
        {
            Tree source(std::less<>{}, hooked_allocator<char>(first)),
                target(std::less<>{}, hooked_allocator<char>(second));
            for (int i = 0; i < count; ++i) {
                source.try_emplace(i, i);
                target.try_emplace(10000 + i, -i);
            }
            const auto allocations = second.allocations, constructs = second.constructs,
                       destroys = second.destroys;
            const auto copies = counted::copies;
            second.observe = [&] {
                EXPECT_TRUE(target.empty());
                EXPECT_EQ(target.begin(), target.end());
                EXPECT_EQ(source.size(), count);
                EXPECT_EQ(source.at(0).number, 0);
            };
            target = source;
            second.observe = {};
            EXPECT_EQ(second.allocations, allocations);
            EXPECT_EQ(second.constructs - constructs, count);
            EXPECT_EQ(second.destroys - destroys, count);
            EXPECT_EQ(counted::copies - copies, count);
            for (int i = 0; i < count; ++i)
                EXPECT_EQ(target.at(i).number, i);
        }
        EXPECT_EQ(first.blocks, 0u);
        EXPECT_EQ(second.blocks, 0u);
        EXPECT_EQ(counted::live, 0);
    }
}
TEST(BtreeCopyReuse, TypedHooksObserveEmptyDestinationAndBalancedOveralignedLifetimes) {
    check_hooks<wjr::btree_map<int, counted, std::less<>, hooked_allocator<char>, 5>>();
    check_hooks<wjr::experimental::compact_btree_map<int, counted, std::less<>,
                                                     hooked_allocator<char>, 5>>();
}
} // namespace
