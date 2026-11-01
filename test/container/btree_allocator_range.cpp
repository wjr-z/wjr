#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <map>
#include <memory_resource>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace {
class resource : public std::pmr::memory_resource {
public:
    std::map<void *, std::pair<size_t, size_t>> blocks;

private:
    void *do_allocate(size_t bytes, size_t alignment) override {
        void *p = std::pmr::new_delete_resource()->allocate(bytes, alignment);
        EXPECT_TRUE(blocks.emplace(p, std::pair{bytes, alignment}).second);
        return p;
    }
    void do_deallocate(void *p, size_t bytes, size_t alignment) override {
        auto found = blocks.find(p);
        ASSERT_NE(found, blocks.end());
        EXPECT_EQ(found->second, (std::pair{bytes, alignment}));
        blocks.erase(found);
        std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
};

template <bool Movable>
struct allocator_value {
    using allocator_type = std::pmr::polymorphic_allocator<char>;
    static inline int live = 0;
    static inline int copies = 0;
    std::pmr::string text;
    allocator_value(std::allocator_arg_t, allocator_type alloc, int count)
        : text(count, 'x', alloc) {
        ++live;
    }
    allocator_value(std::allocator_arg_t, allocator_type alloc, const allocator_value &other)
        : text(other.text, alloc) {
        ++live;
        ++copies;
    }
    allocator_value(std::allocator_arg_t, allocator_type alloc, allocator_value &&other)
        : text(std::move(other.text), alloc) {
        ++live;
    }
    allocator_value(const allocator_value &) = delete;
    allocator_value(allocator_value &&other) noexcept
    requires Movable
        : text(std::move(other.text)) {
        ++live;
    }
    allocator_value(allocator_value &&)
    requires(!Movable)
    = delete;
    ~allocator_value() { --live; }
    friend bool operator<(const allocator_value &lhs, const allocator_value &rhs) noexcept {
        return lhs.text < rhs.text;
    }
};
using allocated_value = allocator_value<false>;
using movable_value = allocator_value<true>;
using pmr_allocator = std::pmr::polymorphic_allocator<char>;
using pair = std::pair<const int, allocated_value>;
using tree = wjr::btree_map<int, allocated_value, std::less<>, pmr_allocator>;

template <typename Tree, typename Iter>
concept accepts_range = requires(Tree &data, Iter first, Iter last) { data.insert(first, last); };

template <typename Tree, typename Iter>
concept accepts_constructor = requires(Iter first, Iter last, typename Tree::allocator_type alloc) {
    Tree(first, last, typename Tree::key_compare{}, alloc);
};

static_assert(!std::constructible_from<pair, const pair &>);
static_assert(accepts_range<tree, const pair *>);
static_assert(accepts_constructor<tree, const pair *>);
static_assert(!accepts_range<wjr::btree_map<int, allocated_value>, const pair *>);
static_assert(!accepts_range<tree, const int *>);
static_assert(!accepts_constructor<tree, const int *>);
static_assert(!accepts_range<tree, const std::pair<std::string, allocated_value> *>);
static_assert(!accepts_range<tree, const std::pair<int, std::string> *>);
static_assert(!accepts_constructor<tree, const std::pair<int, std::string> *>);

struct throwing_destructor {
    ~throwing_destructor() noexcept(false) {}
};
using throwing_tree = wjr::btree_map<int, throwing_destructor, std::less<>, pmr_allocator>;
static_assert(!accepts_range<throwing_tree, const throwing_tree::value_type *>);
struct declared_trivial_allocator : pmr_allocator {
    using is_trivially_allocator = std::true_type;
};
static_assert(
    !wjr::btree_detail::allocator_insert_iterator<const pair *, pair, declared_trivial_allocator>);

template <typename Tree, typename Source>
void check_values(const Tree &data, const Source &source, std::pmr::memory_resource *expected) {
    EXPECT_EQ(data.size(), source.size());
    for (const auto &[key, value] : data) {
        EXPECT_EQ(value.text.get_allocator().resource(), expected);
        EXPECT_EQ(value.text, source.at(key).text);
    }
}

template <typename Tree, typename Value>
void check_map_ranges() {
    resource first, second;
    {
        using standard = std::pmr::map<int, Value, std::less<>>;
        standard source(std::less<>{}, typename standard::allocator_type(&first));
        for (int i = 0; i < 48; ++i)
            source.try_emplace(i, 64 + i);
        standard control(source.cbegin(), source.cend(), std::less<>{},
                         typename standard::allocator_type(&second));
        const auto copies = Value::copies;
        Tree data(source.cbegin(), source.cend(), std::less<>{},
                  typename Tree::allocator_type(&second));
        EXPECT_EQ(Value::copies - copies, 48);
        check_values(data, control, &second);
        data.clear();
        auto first_value = data.try_emplace(0, 64);
        EXPECT_TRUE(first_value.second);
        const auto before = Value::copies;
        data.insert(std::counted_iterator(source.cbegin(), source.size()), std::default_sentinel);
        EXPECT_EQ(Value::copies - before, 47);
        check_values(data, source, &second);
        data.clear();
        data.insert(source.cbegin(), source.cbegin());
        EXPECT_TRUE(data.empty());
        data.insert(std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()));
        EXPECT_EQ(source.size(), 48);
        check_values(data, control, &second);
        for (int i = 0; i < 48; ++i)
            EXPECT_NE(source.find(i), source.end());
    }
    EXPECT_TRUE(first.blocks.empty());
    EXPECT_TRUE(second.blocks.empty());
    EXPECT_EQ(Value::live, 0);
}

TEST(BtreeAllocatorRange, MapConstructionInsertionDuplicatesAndAllocatorExtendedMoves) {
    check_map_ranges<tree, allocated_value>();
    using compact =
        wjr::experimental::compact_btree_map<int, movable_value, std::less<>, pmr_allocator>;
    check_map_ranges<compact, movable_value>();
}

TEST(BtreeAllocatorRange, AllocatorAwareKeysAndNestedMappedPairs) {
    resource first, second;
    {
        using source_tree = std::pmr::map<std::pmr::string, allocated_value, std::less<>>;
        source_tree source(std::less<>{}, source_tree::allocator_type(&first));
        for (int i = 0; i < 40; ++i)
            source.try_emplace(std::pmr::string(64 + i, 'k', &first), 96);
        using target =
            wjr::btree_map<std::pmr::string, allocated_value, std::less<>, pmr_allocator>;
        target data(source.cbegin(), source.cend(), pmr_allocator(&second));
        EXPECT_EQ(data.size(), source.size());
        for (const auto &[key, value] : data) {
            EXPECT_EQ(key.get_allocator().resource(), &second);
            EXPECT_EQ(value.text.get_allocator().resource(), &second);
        }
        using mapped = std::pair<allocated_value, std::pmr::string>;
        using nested_source = std::pmr::map<int, mapped>;
        nested_source nested(&first);
        nested.try_emplace(1, std::piecewise_construct, std::forward_as_tuple(96),
                           std::forward_as_tuple(128, 'a'));
        using nested_target = wjr::btree_map<int, mapped, std::less<>, pmr_allocator>;
        nested_target copied(nested.cbegin(), nested.cend(), pmr_allocator(&second));
        EXPECT_EQ(copied.at(1).first.text.get_allocator().resource(), &second);
        EXPECT_EQ(copied.at(1).second.get_allocator().resource(), &second);
    }
    EXPECT_TRUE(first.blocks.empty());
    EXPECT_TRUE(second.blocks.empty());
    EXPECT_EQ(allocated_value::live, 0);
}

TEST(BtreeAllocatorRange, PairKeyConversionUsesAllocatorExtendedMappedCopy) {
    resource first, second;
    {
        std::pair<short, allocated_value> source(
            std::piecewise_construct, std::forward_as_tuple(short{7}),
            std::forward_as_tuple(std::allocator_arg, pmr_allocator(&first), 96));
        static_assert(!std::constructible_from<pair, decltype(source) &>);
        tree copied(&source, &source + 1, pmr_allocator(&second));
        EXPECT_EQ(copied.size(), 1);
        EXPECT_EQ(copied.at(7).text.size(), 96);
        EXPECT_EQ(copied.at(7).text.get_allocator().resource(), &second);
    }
    EXPECT_TRUE(first.blocks.empty());
    EXPECT_TRUE(second.blocks.empty());
    EXPECT_EQ(allocated_value::live, 0);
}

TEST(BtreeAllocatorRange, ListFormsAndSourceResourceCanBeReleased) {
    resource second;
    std::optional<tree> retained;
    {
        resource first;
        std::pmr::map<int, allocated_value> source(&first);
        source.try_emplace(7, 96);
        retained.emplace(source.cbegin(), source.cend(), pmr_allocator(&second));
        tree listed({pair(std::piecewise_construct, std::forward_as_tuple(3),
                          std::forward_as_tuple(std::allocator_arg, pmr_allocator(&first), 80))},
                    pmr_allocator(&second));
        EXPECT_EQ(listed.at(3).text.get_allocator().resource(), &second);
        listed = {pair(std::piecewise_construct, std::forward_as_tuple(5),
                       std::forward_as_tuple(std::allocator_arg, pmr_allocator(&first), 88))};
        EXPECT_FALSE(listed.contains(3));
        EXPECT_EQ(listed.at(5).text.get_allocator().resource(), &second);
    }
    EXPECT_EQ(retained->at(7).text.get_allocator().resource(), &second);
    EXPECT_EQ(retained->at(7).text, std::pmr::string(96, 'x', &second));
    retained.reset();
    EXPECT_TRUE(second.blocks.empty());
    EXPECT_EQ(allocated_value::live, 0);
}

TEST(BtreeAllocatorRange, SetAndMultisetCopyAllocatorOnlyKeys) {
    resource first, second;
    {
        std::pmr::multiset<allocated_value> source(&first);
        for (int i = 0; i < 48; ++i)
            source.emplace(64 + i % 9);
        using unique = wjr::btree_set<allocated_value, std::less<>, pmr_allocator>;
        using multiple = wjr::btree_multiset<allocated_value, std::less<>, pmr_allocator>;
        unique one(source.begin(), source.end(), pmr_allocator(&second));
        multiple many(source.begin(), source.end(), pmr_allocator(&second));
        EXPECT_EQ(one.size(), 9);
        EXPECT_EQ(many.size(), source.size());
        for (const auto &key : one)
            EXPECT_EQ(key.text.get_allocator().resource(), &second);
        for (const auto &key : many)
            EXPECT_EQ(key.text.get_allocator().resource(), &second);
    }
    EXPECT_TRUE(first.blocks.empty());
    EXPECT_TRUE(second.blocks.empty());
    EXPECT_EQ(allocated_value::live, 0);
}

template <typename T>
struct constrained_allocator : std::pmr::polymorphic_allocator<T> {
    using base = std::pmr::polymorphic_allocator<T>;
    using value_type = T;
    template <typename U>
    struct rebind {
        using other = constrained_allocator<U>;
    };
    using base::base;
    template <typename U>
    constrained_allocator(const constrained_allocator<U> &other) noexcept
        : base(other.resource()) {}
    constrained_allocator select_on_container_copy_construction() const noexcept {
        return constrained_allocator(this->resource());
    }
    template <typename U, typename... Args>
    requires std::constructible_from<U, Args...>
    void construct(U *p, Args &&...args) {
        base::construct(p, std::forward<Args>(args)...);
    }
    void construct(pair *p, const pair &source) { base::construct(p, source); }
};
using hook_tree = wjr::btree_map<int, allocated_value, std::less<>, constrained_allocator<char>>;
static_assert(accepts_range<hook_tree, const pair *>);
static_assert(!accepts_range<hook_tree, const int *>);
static_assert(!accepts_range<hook_tree, const std::pair<int, std::string> *>);

TEST(BtreeAllocatorRange, ConstrainedAllocatorHookRetainsItsResource) {
    check_map_ranges<hook_tree, allocated_value>();
}

template <typename Iter>
struct input_cursor {
    Iter current, end;
    int increments = 0;
    int fail = -1;
};
template <typename Iter>
struct input_iterator {
    using value_type = std::iter_value_t<Iter>;
    using difference_type = ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;
    input_cursor<Iter> *cursor = nullptr;
    decltype(auto) operator*() const { return *cursor->current; }
    input_iterator &operator++() {
        if (++cursor->increments == cursor->fail)
            throw std::runtime_error("iterator");
        ++cursor->current;
        return *this;
    }
    void operator++(int) { ++*this; }
    friend bool operator==(input_iterator it, std::default_sentinel_t) {
        return it.cursor->current == it.cursor->end;
    }
};

TEST(BtreeAllocatorRange, SinglePassAndIteratorFailureKeepResourceOwnership) {
    resource first, second;
    {
        std::pmr::map<int, allocated_value> source(&first);
        for (int i = 0; i < 40; ++i)
            source.try_emplace(i, 96);
        using iterator = decltype(source.cbegin());
        input_cursor<iterator> cursor{source.cbegin(), source.cend()};
        using input = input_iterator<iterator>;
        static_assert(std::input_iterator<input>);
        tree data(input{&cursor}, std::default_sentinel, pmr_allocator(&second));
        check_values(data, source, &second);
        data.clear();
        EXPECT_TRUE(second.blocks.empty());
        cursor = {source.cbegin(), source.cend(), 0, 3};
        EXPECT_THROW((tree(input{&cursor}, std::default_sentinel, pmr_allocator(&second))),
                     std::runtime_error);
        EXPECT_TRUE(second.blocks.empty());
        cursor = {source.cbegin(), source.cend(), 0, 3};
        EXPECT_THROW(data.insert(input{&cursor}, std::default_sentinel), std::runtime_error);
        EXPECT_EQ(data.size(), 3);
        data.clear();
    }
    EXPECT_TRUE(first.blocks.empty());
    EXPECT_TRUE(second.blocks.empty());
    EXPECT_EQ(allocated_value::live, 0);
}
} // namespace
