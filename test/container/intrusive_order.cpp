#include "../detail.hpp"

#include <algorithm>
#include <array>
#include <vector>

#include <wjr/container/intrusive_order.hpp>

namespace {
struct item : wjr::intrusive::list_node<item>, wjr::intrusive::hlist_node<item> {
    constexpr item(int k = 0, int i = 0) : key(k), id(i) {}
    int key;
    int id;
    friend constexpr bool operator<(const item &a, const item &b) { return a.key < b.key; }
};
using ring = wjr::intrusive::list_node<item>;
using chain = wjr::intrusive::hlist_node<item>;
constexpr auto less_key = [](const item &a, const item &b) noexcept { return a.key < b.key; };

template <class H, size_t N>
void link(H &head, std::array<item, N> &values, size_t count = N) {
    if constexpr (std::same_as<H, ring>) {
        for (size_t i = 0; i < count; ++i)
            head.push_back(static_cast<H *>(&values[i]));
    } else {
        for (size_t i = count; i != 0; --i)
            head.push_front(static_cast<H *>(&values[i - 1]));
    }
}

template <class H>
std::vector<int> ids(H &head, size_t limit) {
    std::vector<int> result;
    auto *current = head.next();
    H *previous = &head;
    const auto *end = [&]() -> H * {
        if constexpr (std::same_as<H, ring>)
            return &head;
        else
            return nullptr;
    }();
    while (current != end && result.size() <= limit) {
        if constexpr (std::same_as<H, ring>)
            EXPECT_EQ(current->prev(), previous);
        result.push_back(current->self()->id);
        previous = current;
        current = current->next();
    }
    EXPECT_EQ(current, end);
    if constexpr (std::same_as<H, ring>)
        EXPECT_EQ(head.prev(), previous);
    return result;
}

constexpr bool constant_order() {
    ring head;
    item a(3, 0), b(1, 1), c(1, 2), d(2, 3);
    head.push_back(static_cast<ring *>(&a));
    head.push_back(static_cast<ring *>(&b));
    head.push_back(static_cast<ring *>(&c));
    head.push_back(static_cast<ring *>(&d));
    wjr::intrusive::sort(head, less_key);
    if (head.next()->self() != &b || head.next()->next()->self() != &c)
        return false;
    wjr::intrusive::reverse(head);
    return head.next()->self() == &a && head.prev()->self() == &b;
}
static_assert(constant_order());
static_assert(noexcept(wjr::intrusive::sort(std::declval<ring &>(), less_key)));
static_assert(noexcept(wjr::intrusive::reverse(std::declval<chain &>())));

template <class H>
void merging() {
    H destination, source;
    std::array<item, 4> a{item{1, 0}, item{2, 1}, item{2, 2}, item{4, 3}};
    std::array<item, 3> b{item{0, 4}, item{2, 5}, item{3, 6}};
    link(destination, a);
    link(source, b);
    auto saved = source.begin();
    wjr::intrusive::merge(destination, source, less_key);
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(ids(destination, 7), (std::vector<int>{4, 0, 1, 2, 5, 6, 3}));
    EXPECT_EQ(&*saved, &b[0]);
    EXPECT_EQ(&*++saved, &a[0]);
    wjr::intrusive::merge(source, destination, less_key);
    EXPECT_TRUE(destination.empty());
    EXPECT_EQ(ids(source, 7), (std::vector<int>{4, 0, 1, 2, 5, 6, 3}));
    wjr::intrusive::merge(source, source, [](const item &, const item &) {
        ADD_FAILURE() << "self merge compared payloads";
        return false;
    });
    wjr::intrusive::reverse(source);
    EXPECT_EQ(ids(source, 7), (std::vector<int>{3, 6, 5, 2, 1, 0, 4}));
    wjr::intrusive::reverse(source);
    EXPECT_EQ(ids(source, 7), (std::vector<int>{4, 0, 1, 2, 5, 6, 3}));
}
} // namespace

TEST(intrusive_order, stable_merge_and_reverse_both_container_types) {
    merging<ring>();
    merging<chain>();
    wjr::intrusive::list_node<> raw_ring;
    wjr::intrusive::hlist_node<> raw_chain;
    wjr::intrusive::reverse(raw_ring);
    wjr::intrusive::reverse(raw_chain);
    EXPECT_TRUE(raw_ring.empty());
    EXPECT_TRUE(raw_chain.empty());
}

TEST(intrusive_order, stable_sort_retains_addresses_and_other_hook) {
    for (size_t count : {0, 1, 2, 3, 4, 17, 65, 128}) {
        ring head;
        chain other;
        std::array<item, 128> values;
        std::vector<int> expected;
        for (size_t i = 0; i < count; ++i) {
            values[i].key = static_cast<int>((i * 37 + 11) % 7);
            values[i].id = static_cast<int>(i);
            expected.push_back(static_cast<int>(i));
        }
        std::stable_sort(expected.begin(), expected.end(),
                         [&](int a, int b) { return values[a].key < values[b].key; });
        link(head, values, count);
        link(other, values, count);
        const auto other_before = ids(other, count);
        wjr::intrusive::sort(head, less_key);
        wjr::intrusive::sort(head);
        EXPECT_EQ(ids(head, count), expected);
        EXPECT_EQ(ids(other, count), other_before);
        for (auto &value : head)
            EXPECT_EQ(&value, &values[value.id]);
    }
}

TEST(intrusive_order, sort_borrows_its_single_move_only_comparator) {
    struct compare {
        int *calls;
        explicit compare(int &n) : calls(&n) {}
        compare(compare &&) = default;
        compare(const compare &) = delete;
        bool operator()(const item &a, const item &b) noexcept {
            ++*calls;
            return a.key < b.key;
        }
    };
    ring head;
    std::array<item, 4> values{item{4, 0}, item{2, 1}, item{3, 2}, item{1, 3}};
    link(head, values);
    int calls = 0;
    wjr::intrusive::sort(head, compare(calls));
    EXPECT_GT(calls, 0);
    EXPECT_EQ(ids(head, 4), (std::vector<int>{3, 1, 2, 0}));
}

TEST(intrusive_order, ordering_one_tag_preserves_another_ring) {
    struct order_tag {};
    struct watch_tag {};
    struct value;
    using order_hook = wjr::intrusive::list_node<value, order_tag>;
    using watch_hook = wjr::intrusive::list_node<value, watch_tag>;
    struct value : order_hook, watch_hook {
        int key = 0;
    };
    order_hook first, second;
    watch_hook watch;
    std::array<value, 4> values;
    for (size_t i = 0; i < values.size(); ++i) {
        values[i].key = static_cast<int>(3 - i);
        (i % 2 ? second : first).push_back(static_cast<order_hook *>(&values[i]));
        watch.push_back(static_cast<watch_hook *>(&values[i]));
    }
    auto compare = [](const value &a, const value &b) { return a.key < b.key; };
    wjr::intrusive::sort(first, compare);
    wjr::intrusive::sort(second, compare);
    wjr::intrusive::merge(first, second, compare);
    EXPECT_TRUE(second.empty());
    EXPECT_TRUE(std::ranges::equal(first, std::array{0, 1, 2, 3}, {}, &value::key));
    wjr::intrusive::reverse(first);
    EXPECT_TRUE(std::ranges::equal(watch, std::array{3, 2, 1, 0}, {}, &value::key));
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
namespace {
struct comparison_error {};

template <class H>
void throwing_merge() {
    H destination, source;
    std::array<item, 2> a{item{1, 0}, item{3, 1}};
    std::array<item, 3> b{item{0, 2}, item{2, 3}, item{4, 4}};
    link(destination, a);
    link(source, b);
    int calls = 0;
    EXPECT_THROW(wjr::intrusive::merge(destination, source,
                                       [&](const item &left, const item &right) {
                                           if (calls++ == 2)
                                               throw comparison_error{};
                                           return left.key < right.key;
                                       }),
                 comparison_error);
    EXPECT_EQ(ids(destination, 5), (std::vector<int>{2, 0, 1}));
    EXPECT_EQ(ids(source, 5), (std::vector<int>{3, 4}));
}
} // namespace
TEST(intrusive_order, throwing_merge_preserves_both_inputs) {
    throwing_merge<ring>();
    throwing_merge<chain>();
}
TEST(intrusive_order, throwing_sort_recovers_every_node_into_the_input_ring) {
    for (int throw_at : {0, 1, 2, 5, 10, 20, 40, 80, 200}) {
        ring head;
        std::array<item, 32> values;
        for (size_t i = 0; i < values.size(); ++i) {
            values[i].key = static_cast<int>((i * 37 + 11) % 7);
            values[i].id = static_cast<int>(i);
        }
        link(head, values);
        int calls = 0;
        try {
            wjr::intrusive::sort(head, [&](const item &a, const item &b) {
                if (calls++ == throw_at)
                    throw comparison_error{};
                return a.key < b.key;
            });
        } catch (const comparison_error &) {
        }
        auto recovered = ids(head, values.size());
        std::sort(recovered.begin(), recovered.end());
        ASSERT_EQ(recovered.size(), values.size());
        wjr::intrusive::reverse(head);
        head.push_back(head.pop_front());
        wjr::intrusive::sort(head, less_key);
        int previous = -1;
        for (const auto &value : head) {
            EXPECT_GE(value.key, previous);
            previous = value.key;
        }
        for (size_t i = 0; i < values.size(); ++i)
            EXPECT_EQ(recovered[i], i);
    }
}
#endif
