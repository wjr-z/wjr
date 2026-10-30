#include <cstdint>
#include <iterator>
#include <map>
#include <memory>
#include <memory_resource>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

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

struct selected_order {
    bool descending;
    selected_order() = delete;
    explicit selected_order(bool value) noexcept : descending(value) {}
    bool operator()(int lhs, int rhs) const noexcept { return descending ? lhs > rhs : lhs < rhs; }
};

template <class Tree>
void set_constructors(size_t expected) {
    using allocator = typename Tree::allocator_type;
    tracking_resource resource;
    const int input[] = {3, 1, 3, 2};
    {
        Tree range(std::counted_iterator(input, 4), std::default_sentinel, selected_order(true),
                   allocator(&resource));
        EXPECT_EQ(range.size(), expected);
        EXPECT_EQ(*range.begin(), 3);
        EXPECT_EQ(range.get_allocator().resource(), &resource);
        Tree list({3, 1, 3, 2}, selected_order(true), allocator(&resource));
        EXPECT_EQ(list.size(), expected);
        list = {8, 2, 8, 3};
        EXPECT_EQ(list.size(), expected);
        EXPECT_EQ(*list.begin(), 8);
        EXPECT_TRUE(list.key_comp().descending);
        EXPECT_EQ(list.get_allocator().resource(), &resource);
        static_assert(!std::constructible_from<Tree, allocator>);
    }
    EXPECT_TRUE(resource.empty());
    EXPECT_EQ(resource.allocations, resource.deallocations);
}
TEST(BtreeAllocatorConstruction, RangeAndListSetConstructionRetainNondefaultOrderingAndResource) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    set_constructors<wjr::btree_set<int, selected_order, allocator>>(3);
    set_constructors<wjr::btree_multiset<int, selected_order, allocator>>(4);
    std::istringstream stream("3 1 3 2");
    wjr::btree_set<int> values(std::istream_iterator<int>(stream), std::istream_iterator<int>{});
    EXPECT_EQ(values.size(), 3u);
    wjr::btree_multiset<int> duplicates{3, 1, 3, 2};
    EXPECT_EQ(duplicates.size(), 4u);
}

template <class Tree>
void allocated_maps() {
    tracking_resource resource;
    using allocator = typename Tree::allocator_type;
    std::vector<std::pair<int, payload>> input;
    for (int i = 0; i != 100; ++i)
        input.emplace_back(i, payload(i));
    {
        Tree explicit_state(input.begin(), input.end(), typename Tree::key_compare{},
                            allocator(&resource));
        Tree default_order(input.begin(), input.end(), allocator(&resource));
        Tree list({{2, payload(2)}, {1, payload(1)}}, allocator(&resource));
        Tree full_list({{2, payload(2)}, {1, payload(1)}}, typename Tree::key_compare{},
                       allocator(&resource));
        Tree empty{allocator(&resource)};
        for (const Tree *tree : {&explicit_state, &default_order, &list, &full_list, &empty}) {
            EXPECT_EQ(tree->get_allocator().resource(), &resource);
            for (const auto &[key, value] : *tree) {
                EXPECT_EQ(value.value, key);
                EXPECT_EQ(reinterpret_cast<std::uintptr_t>(&value) % 64, 0u);
            }
        }
        EXPECT_EQ(explicit_state.size(), 100u);
        EXPECT_EQ(default_order.size(), 100u);
        EXPECT_EQ(list.size(), 2u);
        EXPECT_EQ(full_list.size(), 2u);
        EXPECT_TRUE(empty.empty());
        EXPECT_GT(resource.overaligned_allocations, 0u);
    }
    EXPECT_TRUE(resource.empty());
    EXPECT_EQ(resource.allocations, resource.deallocations);
}
TEST(BtreeAllocatorConstruction, AllocatorRangeListAndEmptyMapsUseAlignedStorage) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    allocated_maps<wjr::btree_map<int, payload, std::less<int>, allocator>>();
    allocated_maps<wjr::experimental::compact_btree_map<int, payload, std::less<int>, allocator>>();
}

template <class Tree>
void pmr_mapped_range() {
    using allocator = typename Tree::allocator_type;
    const std::pair<int, std::pmr::string> input[] = {{1, std::pmr::string(128, 'a')},
                                                      {2, std::pmr::string(128, 'b')}};
    tracking_resource resource;
    {
        Tree values(std::counted_iterator(input, 2), std::default_sentinel, allocator(&resource));
        for (const auto &[key, text] : values) {
            EXPECT_EQ(text.get_allocator().resource(), &resource);
            EXPECT_EQ(text.size(), 128u);
            EXPECT_EQ(text.front(), char('a' + key - 1));
        }
    }
    EXPECT_TRUE(resource.empty());
}
TEST(BtreeAllocatorConstruction, RangeMappedAllocatorAwareValuesUseTargetResource) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    pmr_mapped_range<wjr::btree_map<int, std::pmr::string, std::less<int>, allocator>>();
    pmr_mapped_range<
        wjr::experimental::compact_btree_map<int, std::pmr::string, std::less<int>, allocator>>();
}

template <class Value>
struct failing_input {
    using value_type = Value;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;
    const Value *position = nullptr;
    const Value *stop = nullptr;
    const Value &operator*() const { return *position; }
    failing_input &operator++() {
        if (position == stop)
            throw std::runtime_error("iterator increment");
        ++position;
        return *this;
    }
    void operator++(int) { ++*this; }
    friend bool operator==(const failing_input &lhs, const failing_input &rhs) {
        return lhs.position == rhs.position;
    }
};

template <class Tree, class Value>
void failed_input_cleanup(const std::vector<Value> &input) {
    tracking_resource resource;
    using allocator = typename Tree::allocator_type;
    const failing_input<Value> first{input.data(), input.data() + 20};
    const failing_input<Value> last{input.data() + input.size(), input.data() + 20};
    auto construct = [&] {
        Tree values(first, last, typename Tree::key_compare{}, allocator(&resource));
    };
    EXPECT_THROW(construct(), std::runtime_error);
    EXPECT_GT(resource.allocations, 0u);
    EXPECT_TRUE(resource.empty());
    EXPECT_EQ(resource.allocations, resource.deallocations);
}
TEST(BtreeAllocatorConstruction, IteratorFailureReleasesCompletedBaseStorage) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    std::vector<int> keys;
    std::vector<std::pair<int, int>> pairs;
    for (int i = 0; i != 80; ++i) {
        keys.push_back(i);
        pairs.emplace_back(i, i);
    }
    failed_input_cleanup<wjr::btree_set<int, std::less<int>, allocator>>(keys);
    failed_input_cleanup<wjr::btree_multiset<int, std::less<int>, allocator>>(keys);
    failed_input_cleanup<wjr::btree_map<int, int, std::less<int>, allocator>>(pairs);
    failed_input_cleanup<wjr::experimental::compact_btree_map<int, int, std::less<int>, allocator>>(
        pairs);
}

template <class Tree>
void direct_default_allocator() {
    construction_counts::reset();
    { Tree values; }
    EXPECT_EQ(construction_counts::defaults, 1);
    EXPECT_EQ(construction_counts::copies, 0);
    EXPECT_EQ(construction_counts::moves, 0);
    construction_counts::reset();
    { Tree values(typename Tree::key_compare{}); }
    EXPECT_EQ(construction_counts::defaults, 1);
    EXPECT_EQ(construction_counts::copies, 0);
    EXPECT_EQ(construction_counts::moves, 0);
}
TEST(BtreeAllocatorConstruction, ExpandedAdaptersKeepDefaultAndComparatorDirectPaths) {
    using allocator = counted_allocator<char>;
    direct_default_allocator<wjr::btree_set<int, std::less<int>, allocator>>();
    direct_default_allocator<wjr::btree_multiset<int, std::less<int>, allocator>>();
    direct_default_allocator<wjr::btree_map<int, int, std::less<int>, allocator>>();
    direct_default_allocator<
        wjr::experimental::compact_btree_map<int, int, std::less<int>, allocator>>();
}

template <class T>
struct dual_allocator : counted_allocator<T> {
    template <class U>
    struct rebind {
        using other = dual_allocator<U>;
    };
    dual_allocator() = default;
    template <class U>
    dual_allocator(const dual_allocator<U> &) noexcept {}
    bool operator()(int lhs, int rhs) const noexcept { return lhs < rhs; }
};
using dual_tree = wjr::btree_map<int, int, dual_allocator<char>, dual_allocator<char>>;
static_assert(std::constructible_from<dual_tree, const dual_allocator<char> &>);
static_assert(!std::constructible_from<wjr::btree_map<int, int>, int, int>);
TEST(BtreeAllocatorConstruction, AllocatorConvenienceDoesNotHideSameTypeComparator) {
    dual_allocator<char> comparator;
    dual_tree values(comparator);
    values.emplace(1, 2);
    EXPECT_EQ(values.at(1), 2);
}

template <class Tree>
void moved_constructor_range() {
    using allocator = typename Tree::allocator_type;
    tracking_resource resource;
    std::vector<std::pair<int, std::unique_ptr<int>>> input;
    for (int i = 0; i != 40; ++i)
        input.emplace_back(i, std::make_unique<int>(i));
    {
        Tree values(std::make_move_iterator(input.begin()), std::make_move_iterator(input.end()),
                    allocator(&resource));
        ASSERT_EQ(values.size(), 40u);
        for (const auto &[key, value] : values) {
            ASSERT_TRUE(value);
            EXPECT_EQ(*value, key);
        }
        for (const auto &[key, value] : input)
            EXPECT_FALSE(value);
    }
    EXPECT_TRUE(resource.empty());
}
TEST(BtreeAllocatorConstruction, RangeConstructorsConsumeMoveOnlyValues) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    moved_constructor_range<wjr::btree_map<int, std::unique_ptr<int>, std::less<int>, allocator>>();
    moved_constructor_range<wjr::experimental::compact_btree_map<int, std::unique_ptr<int>,
                                                                 std::less<int>, allocator>>();
    using tree = wjr::btree_map<int, int>;
    using source = const std::pair<int, int> *;
    static_assert(std::is_nothrow_constructible_v<tree, source, source>);
}
} // namespace
