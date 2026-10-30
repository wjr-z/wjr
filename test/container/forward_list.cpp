#include "../detail.hpp"

#include <algorithm>
#include <array>
#include <ranges>

#include <wjr/container/forward_list.hpp>

using namespace wjr;

namespace {
struct hlist_value : intrusive::hlist_node<hlist_value> {
    explicit hlist_value(int value_) : value(value_) {}
    int value;
};

using hlist_iterator = hlist_value::iterator;
using hlist_const_iterator = hlist_value::const_iterator;

static_assert(std::forward_iterator<hlist_iterator>);
static_assert(std::forward_iterator<hlist_const_iterator>);
static_assert(std::convertible_to<hlist_iterator, hlist_const_iterator>);
static_assert(!std::constructible_from<hlist_iterator, hlist_const_iterator>);
static_assert(
    !std::constructible_from<hlist_iterator, const intrusive::hlist_node_base<hlist_value> *>);
static_assert(std::sentinel_for<hlist_const_iterator, hlist_iterator>);
static_assert(sizeof(hlist_iterator) == sizeof(void *));
static_assert(sizeof(hlist_const_iterator) == sizeof(void *));
static_assert(sizeof(intrusive::hlist_node<hlist_value>) == sizeof(void *));
static_assert(std::same_as<decltype(std::declval<intrusive::hlist_node<hlist_value> &>().front()),
                           intrusive::hlist_node<hlist_value> *>);
static_assert(
    std::same_as<decltype(std::declval<const intrusive::hlist_node<hlist_value> &>().front()),
                 const intrusive::hlist_node<hlist_value> *>);
} // namespace

TEST(intrusive_hlist, standard_iteration_and_front) {
    intrusive::hlist_node<hlist_value> head;
    hlist_value first(1), second(2), third(3);
    head.push_front(&third);
    head.push_front(&second);
    head.push_front(&first);

    EXPECT_EQ(std::ranges::distance(head), 3);
    EXPECT_TRUE(std::ranges::equal(head, std::array{1, 2, 3}, {}, &hlist_value::value));

    const auto &const_head = head;
    EXPECT_TRUE(std::ranges::equal(const_head, std::array{1, 2, 3}, {}, &hlist_value::value));
    EXPECT_EQ(head.front()->self(), &first);
    EXPECT_EQ(const_head.front()->self(), &first);

    hlist_const_iterator const_it = head.begin();
    EXPECT_TRUE(const_it == head.begin());
    EXPECT_TRUE(head.begin() == const_it);
    for (auto &node : head) {
        ++node.value;
    }
    EXPECT_TRUE(std::ranges::equal(const_head, std::array{2, 3, 4}, {}, &hlist_value::value));
}

TEST(intrusive_hlist, empty_iteration_and_front) {
    intrusive::hlist_node<hlist_value> head;
    EXPECT_EQ(head.begin(), head.end());
    EXPECT_EQ(std::ranges::distance(std::as_const(head)), 0);
    EXPECT_EQ(head.front(), nullptr);
    EXPECT_EQ(std::as_const(head).front(), nullptr);
}
