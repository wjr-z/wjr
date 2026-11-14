#include <gtest/gtest.h>

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
constexpr bool empty_entry_macros() {
    intrusive::list_node<list_value> head;
    list_value *entry = nullptr, *next = nullptr;
    WJR_LIST_FOR_EACH_ENTRY(entry, &head) { return false; }
    WJR_LIST_FOR_EACH_ENTRY_SAFE(entry, next, &head) { return false; }
    return entry == nullptr && next == nullptr;
}
static_assert(empty_entry_macros());
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
TEST(intrusive_list, entry_macros_handle_empty_final_and_const_links) {
    for (int count : {0, 1, 3}) {
        intrusive::list_node<list_value> head;
        list_value first(1), second(2), third(3);
        list_value *values[] = {&first, &second, &third};
        for (int i = 0; i < count; ++i)
            head.push_back(values[i]);
        int visited = 0;
        list_value *entry, *next;
        WJR_LIST_FOR_EACH_ENTRY(entry, &head) { EXPECT_EQ(entry, values[visited++]); }
        EXPECT_EQ(visited, count);
        EXPECT_EQ(entry, nullptr);
        visited = 0;
        WJR_LIST_FOR_EACH_ENTRY_SAFE(entry, next, &head) {
            EXPECT_EQ(entry, values[visited++]);
            EXPECT_EQ(next, visited == count ? nullptr : values[visited]);
        }
        EXPECT_EQ(visited, count);
        EXPECT_EQ(entry, nullptr);
        EXPECT_EQ(next, nullptr);
        const auto &const_head = head;
        const list_value *const_entry, *const_next;
        visited = 0;
        WJR_LIST_FOR_EACH_ENTRY(const_entry, &const_head) {
            EXPECT_EQ(const_entry, values[visited++]);
        }
        EXPECT_EQ(visited, count);
        visited = 0;
        WJR_LIST_FOR_EACH_ENTRY_SAFE(const_entry, const_next, &const_head) {
            EXPECT_EQ(const_entry, values[visited++]);
            EXPECT_EQ(const_next, visited == count ? nullptr : values[visited]);
        }
        EXPECT_EQ(visited, count);
        visited = 0;
        WJR_LIST_FOR_EACH_ENTRY(const_entry, &head) { EXPECT_EQ(const_entry, values[visited++]); }
        EXPECT_EQ(visited, count);
    }
}