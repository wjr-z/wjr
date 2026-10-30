#include "../detail.hpp"

#include <array>
#include <ranges>

#include <wjr/container/intrusive_algorithms.hpp>

namespace {
struct item : wjr::intrusive::list_node<item>, wjr::intrusive::hlist_node<item> {
    constexpr item(int v = 0) : key(v) {}
    int key;
    friend constexpr bool operator==(const item &a, const item &b) { return a.key == b.key; }
};
using ring = wjr::intrusive::list_node<item>;
using chain = wjr::intrusive::hlist_node<item>;
constexpr auto same_key = [](const item &a, const item &b) noexcept { return a.key == b.key; };
constexpr auto is_two = [](const item &a) noexcept { return a.key == 2; };

template <class H, size_t N>
void link(H &head, std::array<item, N> &values) {
    if constexpr (std::same_as<H, ring>) {
        for (auto &value : values)
            head.push_back(static_cast<H *>(&value));
    } else {
        for (auto it = values.rbegin(); it != values.rend(); ++it)
            head.push_front(static_cast<H *>(&*it));
    }
}

template <class H>
void expect_keys(H &head, std::initializer_list<int> keys) {
    EXPECT_TRUE(std::ranges::equal(head, keys, {}, &item::key));
    if constexpr (std::same_as<H, ring>) {
        auto *previous = &head;
        for (auto *current = head.next(); current != &head; current = current->next()) {
            EXPECT_EQ(current->prev(), previous);
            previous = current;
        }
        EXPECT_EQ(head.prev(), previous);
    }
}

constexpr bool constant_filter() {
    ring r;
    chain c;
    item first(1), second(2), third(2);
    r.push_back(static_cast<ring *>(&first));
    r.push_back(static_cast<ring *>(&second));
    r.push_back(static_cast<ring *>(&third));
    c.push_front(static_cast<chain *>(&third));
    c.push_front(static_cast<chain *>(&second));
    c.push_front(static_cast<chain *>(&first));
    return wjr::intrusive::unique(r, same_key) == 1 && wjr::intrusive::remove_if(c, is_two) == 2 &&
           r.prev()->self() == &second && c.front()->self() == &first &&
           c.front()->next() == nullptr;
}
static_assert(constant_filter());
static_assert(noexcept(wjr::intrusive::remove_if(std::declval<ring &>(), is_two)));
static_assert(noexcept(wjr::intrusive::unique(std::declval<chain &>(), same_key)));

template <class H>
concept can_filter = requires(H &head) { wjr::intrusive::remove_if(head, is_two); };
static_assert(!can_filter<wjr::intrusive::list_node<>>);
static_assert(!can_filter<wjr::intrusive::hlist_node<>>);

template <class H>
void filtering() {
    H head;
    EXPECT_EQ(wjr::intrusive::remove_if(head, is_two), 0);
    EXPECT_EQ(wjr::intrusive::unique(head, same_key), 0);
    EXPECT_EQ(wjr::intrusive::unique(head), 0);
    std::array<item, 6> values{1, 1, 2, 2, 1, 3};
    link(head, values);
    EXPECT_EQ(wjr::intrusive::unique(head, same_key), 2);
    expect_keys(head, {1, 2, 1, 3});
    EXPECT_EQ(wjr::intrusive::remove_if(head, [](item &v) { return v.key == 1; }), 2);
    expect_keys(head, {2, 3});
    EXPECT_EQ(wjr::intrusive::remove_if(head, [](item &) { return false; }), 0);
    EXPECT_EQ(wjr::intrusive::remove_if(head, [](item &) { return true; }), 2);
    EXPECT_TRUE(head.empty());
    head.push_front(static_cast<H *>(&values[1]));
    expect_keys(head, {1});
    EXPECT_EQ(wjr::intrusive::unique(head, same_key), 0);
}
} // namespace

TEST(intrusive_algorithms, filter_and_unique_rings_and_chains) {
    filtering<ring>();
    filtering<chain>();
}

TEST(intrusive_algorithms, modifying_one_hook_preserves_the_other) {
    ring r;
    chain c;
    std::array<item, 4> values{1, 1, 2, 3};
    link(r, values);
    link(c, values);
    EXPECT_EQ(wjr::intrusive::unique(r, same_key), 1);
    expect_keys(c, {1, 1, 2, 3});
    EXPECT_EQ(wjr::intrusive::remove_if(c, is_two), 1);
    expect_keys(r, {1, 2, 3});
    expect_keys(c, {1, 1, 3});
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
namespace {
struct predicate_error {};
template <class H>
void exception_contract() {
    H head;
    std::array<item, 4> values{1, 1, 2, 3};
    link(head, values);
    EXPECT_THROW(wjr::intrusive::unique(head,
                                        [](const item &a, const item &b) {
                                            if (b.key == 2)
                                                throw predicate_error{};
                                            return a.key == b.key;
                                        }),
                 predicate_error);
    expect_keys(head, {1, 2, 3});
    EXPECT_THROW(wjr::intrusive::remove_if(head,
                                           [](const item &v) {
                                               if (v.key == 3)
                                                   throw predicate_error{};
                                               return v.key == 1;
                                           }),
                 predicate_error);
    expect_keys(head, {2, 3});
}
} // namespace
TEST(intrusive_algorithms, exceptions_keep_prior_removals_and_valid_links) {
    exception_contract<ring>();
    exception_contract<chain>();
}
#endif
