#include <cstdint>
#include <iterator>
#include <map>
#include <memory>
#include <memory_resource>
#include <optional>
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

template <class Tree, bool Duplicates = false>
void transfer_fill(Tree &tree, int size) {
    for (int i = 0; i != size; ++i) {
        if constexpr (std::is_void_v<typename Tree::mapped_type>) {
            const auto text =
                std::to_string(100000 + (Duplicates ? i / 2 : i)) + std::string(120, 'k');
            tree.emplace(text.c_str());
        } else if constexpr (std::same_as<typename Tree::mapped_type, std::pmr::string>) {
            tree.try_emplace(i, 128, char('a' + i % 26));
        } else
            tree.try_emplace(i, i);
    }
}
template <class Tree>
void transfer_check(const Tree &tree, std::pmr::memory_resource *resource, size_t size) {
    ASSERT_EQ(tree.size(), size);
    EXPECT_EQ(tree.get_allocator().resource(), resource);
    for (const auto &entry : tree) {
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(&entry) % alignof(typename Tree::value_type),
                  0u);
        if constexpr (std::is_void_v<typename Tree::mapped_type>) {
            EXPECT_EQ(entry.get_allocator().resource(), resource);
        } else if constexpr (std::same_as<typename Tree::mapped_type, std::pmr::string>) {
            EXPECT_EQ(entry.second.get_allocator().resource(), resource);
            EXPECT_EQ(entry.second.size(), 128u);
            EXPECT_EQ(entry.second.front(), char('a' + entry.first % 26));
        } else
            EXPECT_EQ(entry.second.value, entry.first);
    }
}
template <class Tree, bool Compact = false, bool Duplicates = false>
void transfer_matrix() {
    using allocator = typename Tree::allocator_type;
    for (int size : {0, 1, 2, 3, 80}) {
        for (bool compatible : {false, true}) {
            tracking_resource first, second;
            auto *destination = compatible ? &first : &second;
            {
                Tree source(typename Tree::key_compare{}, allocator(&first));
                transfer_fill<Tree, Duplicates>(source, size);
                {
                    Tree copied(source, allocator(destination));
                    transfer_check(copied, destination, size);
                    transfer_check(source, &first, size);
                    if (size != 0)
                        EXPECT_NE(std::addressof(*copied.begin()), std::addressof(*source.begin()));
                }
                const auto *previous = size ? std::addressof(*source.begin()) : nullptr;
                const auto requests = first.allocations + second.allocations;
                Tree moved(std::move(source), allocator(destination));
                transfer_check(moved, destination, size);
                EXPECT_TRUE(source.empty());
                EXPECT_EQ(source.get_allocator().resource(), &first);
                if (compatible) {
                    EXPECT_EQ(first.allocations + second.allocations, requests);
                    if (size != 0 && (!Compact || size > 2))
                        EXPECT_EQ(std::addressof(*moved.begin()), previous);
                } else
                    EXPECT_TRUE(first.empty());
                transfer_fill<Tree, Duplicates>(source, 2);
                transfer_check(source, &first, 2);
                transfer_check(moved, destination, size);
            }
            EXPECT_TRUE(first.empty());
            EXPECT_TRUE(second.empty());
            EXPECT_EQ(first.allocations, first.deallocations);
            EXPECT_EQ(second.allocations, second.deallocations);
        }
    }
}
TEST(BtreeAllocatorTransfer, CopyAndMoveAcrossSameAndDifferentResources) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    transfer_matrix<wjr::btree_map<int, std::pmr::string, std::less<>, allocator>>();
    transfer_matrix<
        wjr::experimental::compact_btree_map<int, std::pmr::string, std::less<>, allocator>,
        true>();
    transfer_matrix<wjr::btree_set<std::pmr::string, std::less<>, allocator>>();
    transfer_matrix<wjr::btree_multiset<std::pmr::string, std::less<>, allocator>, false, true>();
    transfer_matrix<wjr::btree_map<int, payload, std::less<>, allocator>>();
    transfer_matrix<wjr::experimental::compact_btree_map<int, payload, std::less<>, allocator>,
                    true>();
}
TEST(BtreeAllocatorTransfer, DestinationOutlivesTemporaryMonotonicSource) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    using tree =
        wjr::experimental::compact_btree_map<int, std::pmr::string, std::less<>, allocator>;
    tracking_resource destination;
    std::optional<tree> moved;
    {
        std::pmr::monotonic_buffer_resource temporary;
        tree source(std::less<>{}, allocator(&temporary));
        transfer_fill(source, 80);
        moved.emplace(std::move(source), allocator(&destination));
        EXPECT_TRUE(source.empty());
    }
    transfer_check(*moved, &destination, 80);
    moved.reset();
    EXPECT_TRUE(destination.empty());
}
struct immovable_mapped {
    static inline int live = 0;
    int value;
    explicit immovable_mapped(int n) noexcept : value(n) { ++live; }
    immovable_mapped(const immovable_mapped &) = delete;
    immovable_mapped(immovable_mapped &&) = delete;
    ~immovable_mapped() noexcept { --live; }
};
TEST(BtreeAllocatorTransfer, AlwaysEqualStorageTransferDoesNotRequireMovingMappedValues) {
    using tree = wjr::btree_map<int, immovable_mapped>;
    static_assert(std::is_nothrow_constructible_v<tree, tree &&, const tree::allocator_type &>);
    for (int size : {0, 1, 2, 3, 80}) {
        {
            tree source;
            for (int i = 0; i != size; ++i)
                source.try_emplace(i, i);
            const auto *before = size ? std::addressof(*source.begin()) : nullptr;
            tree target(std::move(source), tree::allocator_type{});
            EXPECT_TRUE(source.empty());
            EXPECT_EQ(target.size(), size);
            EXPECT_EQ(immovable_mapped::live, size);
            if (size)
                EXPECT_EQ(std::addressof(*target.begin()), before);
        }
        EXPECT_EQ(immovable_mapped::live, 0);
    }
}

struct allocator_only_mapped {
    using allocator_type = std::pmr::polymorphic_allocator<char>;
    std::pmr::string text;
    allocator_only_mapped(std::allocator_arg_t, allocator_type alloc, int size)
        : text(size, 'x', alloc) {}
    allocator_only_mapped(std::allocator_arg_t, allocator_type alloc,
                          const allocator_only_mapped &other)
        : text(other.text, alloc) {}
    allocator_only_mapped(std::allocator_arg_t, allocator_type alloc, allocator_only_mapped &&other)
        : text(std::move(other.text), alloc) {}
    allocator_only_mapped(const allocator_only_mapped &) = delete;
    allocator_only_mapped(allocator_only_mapped &&) = delete;
};
TEST(BtreeAllocatorTransfer, UsesActualAllocatorConstructionRatherThanOrdinaryValueTraits) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    using tree = wjr::btree_map<int, allocator_only_mapped, std::less<>, allocator>;
    static_assert(!std::constructible_from<tree::value_type, const tree::value_type &>);
    tracking_resource first, second;
    {
        tree source(std::less<>{}, allocator(&first));
        source.try_emplace(1, 128);
        tree copied(source, allocator(&second));
        EXPECT_EQ(copied.begin()->second.text.get_allocator().resource(), &second);
        tree moved(std::move(source), allocator(&second));
        EXPECT_TRUE(source.empty());
        EXPECT_EQ(moved.begin()->second.text.get_allocator().resource(), &second);
        EXPECT_EQ(moved.begin()->second.text.size(), 128u);
    }
    EXPECT_TRUE(first.empty());
    EXPECT_TRUE(second.empty());
}
TEST(BtreeAllocatorTransfer, UnequalAllocatorMoveAssignmentClearsMovedKeys) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    using tree = wjr::btree_set<std::string, std::less<>, allocator>;
    tracking_resource first, second;
    {
        tree source({"alpha", "beta", "gamma"}, std::less<>{}, allocator(&first));
        tree target(std::less<>{}, allocator(&second));
        target = std::move(source);
        EXPECT_TRUE(source.empty());
        EXPECT_TRUE(first.empty());
        EXPECT_EQ(target.size(), 3u);
        EXPECT_EQ(target.count("alpha"), 1u);
        source.emplace("delta");
        EXPECT_EQ(source.size(), 1u);
        EXPECT_EQ(source.count("delta"), 1u);
    }
    EXPECT_TRUE(first.empty());
    EXPECT_TRUE(second.empty());
}

TEST(BtreeAllocatorTransfer, ConstPmrMapKeysAndValuesSurviveSourceResourceDestruction) {
    using allocator = std::pmr::polymorphic_allocator<char>;
    using tree = wjr::btree_map<std::pmr::string, std::pmr::string, std::less<>, allocator>;
    tracking_resource destination;
    std::optional<tree> copied, moved;
    {
        std::pmr::monotonic_buffer_resource temporary;
        tree source(std::less<>{}, allocator(&temporary));
        const std::string key(128, 'k'), value(128, 'v');
        source.emplace(key.c_str(), value.c_str());
        copied.emplace(source, allocator(&destination));
        moved.emplace(std::move(source), allocator(&destination));
        EXPECT_TRUE(source.empty());
    }
    for (const tree *values : {&*copied, &*moved}) {
        ASSERT_EQ(values->size(), 1u);
        EXPECT_EQ(values->begin()->first.get_allocator().resource(), &destination);
        EXPECT_EQ(values->begin()->second.get_allocator().resource(), &destination);
        EXPECT_EQ(values->begin()->first.size(), 128u);
        EXPECT_EQ(values->begin()->second.size(), 128u);
    }
    copied.reset();
    moved.reset();
    EXPECT_TRUE(destination.empty());
}
} // namespace
