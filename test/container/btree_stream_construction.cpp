#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <algorithm>
#include <iterator>
#include <map>
#include <memory_resource>
#include <ranges>
#include <set>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace {
class resource : public std::pmr::memory_resource {
public:
    std::map<void *, std::pair<size_t, size_t>> blocks;

private:
    void *do_allocate(size_t bytes, size_t alignment) override {
        auto *p = std::pmr::new_delete_resource()->allocate(bytes, alignment);
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
using allocator = std::pmr::polymorphic_allocator<char>;

template <int Mode, typename Tree, typename Range>
Tree construct(Range &input, allocator alloc) {
    static_assert(std::input_iterator<std::ranges::iterator_t<Range>>);
    static_assert(!std::copy_constructible<std::ranges::iterator_t<Range>>);
    if constexpr (Mode == 0)
        return Tree(input.begin(), input.end());
    else if constexpr (Mode == 1)
        return Tree(input.begin(), input.end(), typename Tree::key_compare{});
    else if constexpr (Mode == 2)
        return Tree(input.begin(), input.end(), typename Tree::key_compare{}, alloc);
    else
        return Tree(input.begin(), input.end(), alloc);
}

template <int Mode, bool Multi, typename Compare>
void check_set() {
    using tree = std::conditional_t<Multi, wjr::btree_multiset<int, Compare, allocator>,
                                    wjr::btree_set<int, Compare, allocator>>;
    using reference =
        std::conditional_t<Multi, std::multiset<int, Compare>, std::set<int, Compare>>;
    resource state;
    {
        std::istringstream input("8 3 1 9 3 7 8 4 2 6 5 10 11 20 19 18 17 16 15 14 13 12");
        std::istringstream oracle(input.str());
        reference expected(std::istream_iterator<int>{oracle}, std::istream_iterator<int>{});
        auto view = std::ranges::istream_view<int>(input);
        auto actual = construct<Mode, tree>(view, allocator(&state));
        EXPECT_EQ(actual.get_allocator().resource(),
                  Mode >= 2 ? &state : std::pmr::get_default_resource());
        EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
        EXPECT_TRUE(input.eof());
        EXPECT_EQ(actual.erase(8), expected.erase(8));
        actual.insert(21);
        expected.insert(21);
        EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }
    EXPECT_TRUE(state.blocks.empty());
}

template <int Mode, bool Compact>
void check_map() {
    using tree =
        std::conditional_t<Compact,
                           wjr::experimental::compact_btree_map<int, int, std::less<>, allocator>,
                           wjr::btree_map<int, int, std::less<>, allocator>>;
    resource state;
    {
        std::istringstream input("8 3 1 9 3 7 8 4 2 6 5 10 11 20 19 18 17 16 15 14 13 12");
        std::istringstream oracle(input.str());
        std::map<int, int> expected;
        for (std::istream_iterator<int> it{oracle}, end; it != end; ++it)
            expected.emplace(*it, *it * 3);
        auto view = std::ranges::istream_view<int>(input) |
                    std::views::transform([](int key) { return std::pair{key, key * 3}; });
        auto actual = construct<Mode, tree>(view, allocator(&state));
        EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
        EXPECT_EQ(actual.get_allocator().resource(),
                  Mode >= 2 ? &state : std::pmr::get_default_resource());
        EXPECT_TRUE(input.eof());
        actual.insert_or_assign(3, 100);
        expected.insert_or_assign(3, 100);
        EXPECT_EQ(actual.erase(8), expected.erase(8));
        EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }
    EXPECT_TRUE(state.blocks.empty());
}

TEST(BtreeStreamConstruction, AllConstructorFamiliesConsumeMoveOnlyViews) {
    []<int... Mode>(std::integer_sequence<int, Mode...>) {
        (check_set<Mode, false, std::less<>>(), ...);
        (check_set<Mode, true, std::greater<>>(), ...);
        (check_map<Mode, false>(), ...);
        (check_map<Mode, true>(), ...);
    }(std::make_integer_sequence<int, 4>{});
}

struct allocated_value {
    using allocator_type = allocator;
    std::pmr::string text;
    allocated_value(std::allocator_arg_t, allocator a, int key) : text(64 + key, 'x', a) {}
    allocated_value(std::allocator_arg_t, allocator a, const allocated_value &other)
        : text(other.text, a) {}
    allocated_value(std::allocator_arg_t, allocator a, allocated_value &&other)
        : text(std::move(other.text), a) {}
    allocated_value(const allocated_value &) = delete;
    allocated_value(allocated_value &&) = delete;
};

TEST(BtreeStreamConstruction, AllocatorOnlyMappedValuesStreamIntoTheDestinationResource) {
    resource source, target;
    {
        using tree = wjr::btree_map<int, allocated_value, std::less<>, allocator>;
        using pair = tree::value_type;
        auto transform = [&](int key) {
            return pair(std::piecewise_construct, std::forward_as_tuple(key),
                        std::forward_as_tuple(std::allocator_arg, allocator(&source), key));
        };
        std::istringstream input("7 1 3 7 9 2 4 8 6 5 10 11 12 13 14 15 16 17");
        std::istringstream oracle(input.str());
        auto values = std::ranges::istream_view<int>(input) | std::views::transform(transform);
        auto expected_values =
            std::ranges::istream_view<int>(oracle) | std::views::transform(transform);
        std::pmr::map<int, allocated_value> expected(&target);
        std::ranges::copy(expected_values, std::inserter(expected, expected.end()));
        tree actual(values.begin(), values.end(), allocator(&target));
        EXPECT_EQ(actual.size(), expected.size());
        for (const auto &[key, value] : actual) {
            EXPECT_EQ(value.text, expected.at(key).text);
            EXPECT_EQ(value.text.get_allocator().resource(), &target);
        }
        EXPECT_TRUE(source.blocks.empty());
    }
    EXPECT_TRUE(source.blocks.empty());
    EXPECT_TRUE(target.blocks.empty());
}

TEST(BtreeStreamConstruction, EmptyInputAndThrowingStreamReleaseAllocatorStorage) {
    resource state;
    using tree = wjr::btree_set<int, std::less<>, allocator>;
    {
        std::istringstream empty;
        auto view = std::ranges::istream_view<int>(empty);
        tree data(view.begin(), view.end(), allocator(&state));
        EXPECT_TRUE(data.empty());
        EXPECT_TRUE(state.blocks.empty());
    }
    std::istringstream input("1 2 3");
    input.exceptions(std::ios::failbit | std::ios::badbit);
    auto values = std::ranges::istream_view<int>(input);
    EXPECT_THROW((tree(values.begin(), values.end(), allocator(&state))), std::ios_base::failure);
    EXPECT_TRUE(state.blocks.empty());
}
} // namespace
