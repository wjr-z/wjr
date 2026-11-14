#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <memory>
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

namespace {
template <class Hook>
void entry_macros_select_tagged_hook() {
    for (int count : {0, 1, 3}) {
        Hook head;
        item first(1), second(2), third(3);
        item *values[] = {&first, &second, &third};
        if constexpr (requires { head.prev(); }) {
            for (int i = 0; i < count; ++i)
                head.push_back(static_cast<Hook *>(values[i]));
        } else {
            for (int i = count; i > 0; --i)
                head.push_front(static_cast<Hook *>(values[i - 1]));
        }
        item *entry = nullptr, *next = nullptr;
        int visited = 0;
        if constexpr (requires { head.prev(); }) {
            WJR_LIST_FOR_EACH_ENTRY(entry, &head) { EXPECT_EQ(entry, values[visited++]); }
        } else {
            WJR_HLIST_FOR_EACH_ENTRY(entry, &head) { EXPECT_EQ(entry, values[visited++]); }
        }
        EXPECT_EQ(visited, count);
        EXPECT_EQ(entry, nullptr);
        visited = 0;
        const auto &const_head = head;
        const item *const_entry = nullptr, *const_next = nullptr;
        if constexpr (requires { head.prev(); }) {
            WJR_LIST_FOR_EACH_ENTRY_SAFE(const_entry, const_next, &const_head) {
                EXPECT_EQ(const_entry, values[visited++]);
                EXPECT_EQ(const_next, visited == count ? nullptr : values[visited]);
            }
        } else {
            WJR_HLIST_FOR_EACH_ENTRY_SAFE(const_entry, const_next, &const_head) {
                EXPECT_EQ(const_entry, values[visited++]);
                EXPECT_EQ(const_next, visited == count ? nullptr : values[visited]);
            }
        }
        EXPECT_EQ(visited, count);
        EXPECT_EQ(const_entry, nullptr);
        EXPECT_EQ(const_next, nullptr);
        visited = 0;
        if constexpr (requires { head.prev(); }) {
            WJR_LIST_FOR_EACH_ENTRY_SAFE(entry, next, &head) {
                EXPECT_EQ(entry, values[visited++]);
                static_cast<Hook *>(entry)->remove();
            }
        } else {
            WJR_HLIST_FOR_EACH_ENTRY_SAFE(entry, next, &head) {
                EXPECT_EQ(entry, values[visited++]);
                EXPECT_EQ(head.pop_front()->self(), entry);
            }
        }
        EXPECT_EQ(visited, count);
        EXPECT_TRUE(head.empty());
    }
}

template <class Hook>
void safe_entry_macro_destroys_current_payload() {
    Hook head;
    for (int i = 3; i > 0; --i)
        head.push_front(static_cast<Hook *>(new item(i)));
    item *entry = nullptr, *next = nullptr;
    int visited = 0;
    if constexpr (requires { head.prev(); }) {
        WJR_LIST_FOR_EACH_ENTRY_SAFE(entry, next, &head) {
            std::unique_ptr<item> current(entry);
            EXPECT_EQ(entry->value, ++visited);
            static_cast<Hook *>(entry)->remove();
            EXPECT_EQ(next == nullptr, visited == 3);
        }
    } else {
        WJR_HLIST_FOR_EACH_ENTRY_SAFE(entry, next, &head) {
            std::unique_ptr<item> current(entry);
            EXPECT_EQ(entry->value, ++visited);
            EXPECT_EQ(head.pop_front()->self(), entry);
            EXPECT_EQ(next == nullptr, visited == 3);
        }
    }
    EXPECT_EQ(visited, 3);
    EXPECT_TRUE(head.empty());
    EXPECT_EQ(entry, nullptr);
    EXPECT_EQ(next, nullptr);
}
} // namespace

TEST(intrusive_tags, entry_macros_follow_selected_ring_and_chain_hooks) {
    entry_macros_select_tagged_hook<ready_ring>();
    entry_macros_select_tagged_hook<timer_ring>();
    entry_macros_select_tagged_hook<ready_chain>();
    entry_macros_select_tagged_hook<timer_chain>();
}

TEST(intrusive_tags, safe_entry_macros_allow_destroying_tagged_payloads) {
    safe_entry_macro_destroys_current_payload<ready_ring>();
    safe_entry_macro_destroys_current_payload<timer_ring>();
    safe_entry_macro_destroys_current_payload<ready_chain>();
    safe_entry_macro_destroys_current_payload<timer_chain>();
}

TEST(intrusive_tags, nested_entry_macros_preserve_break_and_continue) {
    ready_ring ring;
    timer_chain chain;
    item first(1), second(2), third(3);
    for (auto *value : {&first, &second, &third})
        ring.push_back(static_cast<ready_ring *>(value));
    for (auto *value : {&third, &second, &first})
        chain.push_front(static_cast<timer_chain *>(value));
    item *outer = nullptr, *entry = nullptr, *next = nullptr;
    int sum = 0;
    WJR_LIST_FOR_EACH_ENTRY(outer, &ring) {
        WJR_HLIST_FOR_EACH_ENTRY_SAFE(entry, next, &chain) {
            if (entry->value == 2)
                continue;
            sum += outer->value * entry->value;
        }
    }
    EXPECT_EQ(sum, 24);
    EXPECT_EQ(outer, nullptr);
    EXPECT_EQ(entry, nullptr);
    EXPECT_EQ(next, nullptr);
    WJR_LIST_FOR_EACH_ENTRY_SAFE(entry, next, &ring) { break; }
    EXPECT_EQ(entry, &first);
    EXPECT_EQ(next, &second);
}
