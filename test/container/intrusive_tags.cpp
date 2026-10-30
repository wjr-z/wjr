#include "../detail.hpp"

#include <array>
#include <ranges>

#include <wjr/container/forward_list.hpp>
#include <wjr/container/list.hpp>

namespace {
struct ready_tag {};
struct timer_tag {};
struct item;
using ready_ring = wjr::intrusive::list_node<item, ready_tag>;
using timer_ring = wjr::intrusive::list_node<item, timer_tag>;
using ready_chain = wjr::intrusive::hlist_node<item, ready_tag>;
using timer_chain = wjr::intrusive::hlist_node<item, timer_tag>;
struct padding {
    int unused = 0;
};
struct item : padding, ready_ring, timer_ring, ready_chain, timer_chain {
    explicit item(int v) : value(v) {}
    int value;
};

template <class A, class B>
concept can_link_ring = requires(A &a, B &b) { a.push_back(&b); };
template <class A, class B>
concept can_link_chain = requires(A &a, B &b) { a.push_front(&b); };
template <class I>
concept dereferenceable = requires(I it) {
    *it;
    it.operator->();
};
template <class Hook, class Tag>
concept can_get_node = requires(Hook &hook) { hook.template get_node<item, Tag>(); };

static_assert(!can_link_ring<ready_ring, timer_ring>);
static_assert(!can_link_chain<ready_chain, timer_chain>);
static_assert(can_get_node<ready_ring, ready_tag>);
static_assert(!can_get_node<ready_ring, timer_tag>);
static_assert(!std::convertible_to<ready_ring::iterator, timer_ring::iterator>);
static_assert(std::bidirectional_iterator<ready_ring::iterator>);
static_assert(std::forward_iterator<timer_chain::const_iterator>);
static_assert(sizeof(ready_ring) == 2 * sizeof(void *));
static_assert(sizeof(ready_chain) == sizeof(void *));
static_assert(sizeof(ready_ring::iterator) == sizeof(void *));
static_assert(!dereferenceable<wjr::intrusive::list_node_iterator<void>>);
static_assert(!dereferenceable<wjr::intrusive::hlist_node_iterator<void>>);
static_assert(
    std::same_as<wjr::intrusive::list_node_base<item>, wjr::intrusive::list_node_base<item, void>>);
} // namespace

TEST(intrusive_tags, simultaneous_rings_adjust_self_and_iterators) {
    ready_ring ready;
    timer_ring timer;
    item first(1), second(2), third(3);
    ready.push_back(static_cast<ready_ring *>(&first));
    ready.push_back(static_cast<ready_ring *>(&second));
    ready.push_back(static_cast<ready_ring *>(&third));
    timer.push_back(static_cast<timer_ring *>(&third));
    timer.push_back(static_cast<timer_ring *>(&first));
    EXPECT_EQ(ready.front()->self(), &first);
    EXPECT_EQ(timer.front()->self(), &third);
    EXPECT_TRUE(std::ranges::equal(ready, std::array{1, 2, 3}, {}, &item::value));
    EXPECT_TRUE(std::ranges::equal(std::as_const(timer), std::array{3, 1}, {}, &item::value));
    auto saved = timer.begin();
    static_cast<ready_ring &>(first).remove();
    EXPECT_TRUE(std::ranges::equal(ready, std::array{2, 3}, {}, &item::value));
    EXPECT_EQ(&*saved, &third);
    EXPECT_EQ(&*++saved, &first);
    static_cast<timer_ring &>(third).remove();
    EXPECT_TRUE(std::ranges::equal(timer, std::array{1}, {}, &item::value));
    EXPECT_TRUE(std::ranges::equal(ready, std::array{2, 3}, {}, &item::value));
}

TEST(intrusive_tags, simultaneous_chains_and_ring_are_independent) {
    ready_chain ready;
    timer_chain timer;
    ready_ring ring;
    item first(1), second(2);
    ready.push_front(static_cast<ready_chain *>(&second));
    ready.push_front(static_cast<ready_chain *>(&first));
    timer.push_front(static_cast<timer_chain *>(&first));
    timer.push_front(static_cast<timer_chain *>(&second));
    ring.push_back(static_cast<ready_ring *>(&first));
    ring.push_back(static_cast<ready_ring *>(&second));
    EXPECT_EQ(ready.front()->self(), &first);
    EXPECT_EQ(timer.front()->self(), &second);
    EXPECT_TRUE(std::ranges::equal(ready, std::array{1, 2}, {}, &item::value));
    EXPECT_TRUE(std::ranges::equal(std::as_const(timer), std::array{2, 1}, {}, &item::value));
    EXPECT_EQ(ready.pop_front()->self(), &first);
    EXPECT_TRUE(std::ranges::equal(ready, std::array{2}, {}, &item::value));
    EXPECT_TRUE(std::ranges::equal(timer, std::array{2, 1}, {}, &item::value));
    EXPECT_TRUE(std::ranges::equal(ring, std::array{1, 2}, {}, &item::value));
}
