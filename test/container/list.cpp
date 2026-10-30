#include "../detail.hpp"

#include <algorithm>
#include <array>
#include <ranges>

#include <wjr/container/list.hpp>

using namespace wjr;

namespace {
struct list_value : intrusive::list_node<list_value> {
    explicit list_value(int value_) : value(value_) {}
    int value;
};

using list_iterator = list_value::iterator;
using list_const_iterator = list_value::const_iterator;

static_assert(std::bidirectional_iterator<list_iterator>);
static_assert(std::bidirectional_iterator<list_const_iterator>);
static_assert(std::bidirectional_iterator<list_value::reverse_iterator>);
static_assert(std::bidirectional_iterator<list_value::const_reverse_iterator>);
static_assert(std::convertible_to<list_iterator, list_const_iterator>);
static_assert(!std::constructible_from<list_iterator, list_const_iterator>);
static_assert(
    !std::constructible_from<list_iterator, const intrusive::list_node_base<list_value> *>);
static_assert(std::sentinel_for<list_const_iterator, list_iterator>);
static_assert(sizeof(list_iterator) == sizeof(void *));
static_assert(sizeof(list_const_iterator) == sizeof(void *));
static_assert(sizeof(intrusive::list_node<list_value>) == 2 * sizeof(void *));
} // namespace

TEST(intrusive_list, standard_iteration) {
    intrusive::list_node<list_value> head;
    list_value first(1), second(2), third(3);
    head.push_back(&first);
    head.push_back(&second);
    head.push_back(&third);

    EXPECT_EQ(std::ranges::distance(head), 3);
    EXPECT_TRUE(std::ranges::equal(head, std::array{1, 2, 3}, {}, &list_value::value));
    EXPECT_TRUE(std::ranges::equal(head | std::views::reverse, std::array{3, 2, 1}, {},
                                   &list_value::value));

    const auto &const_head = head;
    EXPECT_TRUE(std::ranges::equal(const_head, std::array{1, 2, 3}, {}, &list_value::value));
    EXPECT_TRUE(std::ranges::equal(std::ranges::subrange(const_head.crbegin(), const_head.crend()),
                                   std::array{3, 2, 1}, {}, &list_value::value));
    EXPECT_EQ(head.front()->self(), &first);
    EXPECT_EQ(const_head.front()->self(), &first);

    list_const_iterator const_it = head.begin();
    EXPECT_TRUE(const_it == head.begin());
    EXPECT_TRUE(head.begin() == const_it);
    for (auto &node : head) {
        ++node.value;
    }
    EXPECT_TRUE(std::ranges::equal(const_head, std::array{2, 3, 4}, {}, &list_value::value));
}

TEST(intrusive_list, empty_iteration) {
    intrusive::list_node<list_value> head;
    EXPECT_EQ(head.begin(), head.end());
    EXPECT_EQ(head.rbegin(), head.rend());
    EXPECT_EQ(std::ranges::distance(std::as_const(head)), 0);
}