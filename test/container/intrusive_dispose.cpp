#include <gtest/gtest.h>

#include <array>
#include <concepts>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include <wjr/container/intrusive_algorithms.hpp>

namespace {

struct primary_tag {};
struct secondary_tag {};
struct value;
using ring = wjr::intrusive::list_node<value, primary_tag>;
using other_ring = wjr::intrusive::list_node<value, secondary_tag>;
using chain = wjr::intrusive::hlist_node<value, primary_tag>;

struct prefix {
    uint64_t padding[3] = {};
};

struct value : prefix, ring, other_ring, chain {
    explicit value(int id_, int *destroyed_ = nullptr) : id(id_), destroyed(destroyed_) {}
    ~value() {
        if (destroyed != nullptr) {
            ++*destroyed;
        }
    }
    int id;
    int *destroyed;
};

struct nothrow_disposer {
    void operator()(value *) const noexcept {}
};
struct throwing_disposer {
    void operator()(value *) const {}
};
struct wrong_disposer {
    void operator()(int *) const noexcept {}
};

template <class Head, class Disposer>
concept can_dispose = requires(Head &head, Disposer dispose) {
    wjr::intrusive::clear_and_dispose(head, std::move(dispose));
};

static_assert(can_dispose<ring, nothrow_disposer>);
static_assert(can_dispose<chain, nothrow_disposer>);
static_assert(!can_dispose<const ring, nothrow_disposer>);
static_assert(!can_dispose<ring, throwing_disposer>);
static_assert(!can_dispose<chain, wrong_disposer>);
static_assert(noexcept(wjr::intrusive::clear_and_dispose(std::declval<ring &>(),
                                                         nothrow_disposer{})));
static_assert(sizeof(ring) == 2 * sizeof(void *));
static_assert(sizeof(chain) == sizeof(void *));
static_assert(std::is_trivially_destructible_v<ring>);

constexpr bool constant_empty_disposal() {
    wjr::intrusive::list_node<> r;
    wjr::intrusive::hlist_node<> c;
    size_t calls = 0;
    auto dispose = [&calls](void *) noexcept { ++calls; };
    wjr::intrusive::clear_and_dispose(r, dispose);
    wjr::intrusive::clear_and_dispose(c, dispose);
    return calls == 0 && r.empty() && c.empty();
}
static_assert(constant_empty_disposal());

struct move_only_disposer {
    int *calls;
    explicit move_only_disposer(int &count) : calls(&count) {}
    move_only_disposer(const move_only_disposer &) = delete;
    move_only_disposer(move_only_disposer &&) = default;
    void operator()(value *) & noexcept { ++*calls; }
};
static_assert(can_dispose<ring, move_only_disposer>);

template <class Head>
void dispose_owned_nodes() {
    for (size_t count = 0; count <= 16; ++count) {
        Head head;
        std::array<value *, 16> nodes{};
        std::array<int, 16> order{};
        int destroyed = 0;
        size_t visited = 0;
        for (size_t i = 0; i < count; ++i) {
            nodes[i] = new value(static_cast<int>(i), &destroyed);
        }
        if constexpr (std::same_as<Head, ring>) {
            for (size_t i = 0; i < count; ++i) {
                head.push_back(static_cast<ring *>(nodes[i]));
            }
        } else {
            for (size_t i = count; i != 0; --i) {
                head.push_front(static_cast<chain *>(nodes[i - 1]));
            }
        }

        wjr::intrusive::clear_and_dispose(head, [&](value *node) noexcept {
            EXPECT_TRUE(head.empty());
            EXPECT_EQ(node, nodes[visited]);
            order[visited++] = node->id;
            std::default_delete<value>{}(node);
        });
        EXPECT_TRUE(head.empty());
        EXPECT_EQ(visited, count);
        EXPECT_EQ(destroyed, count);
        for (size_t i = 0; i < count; ++i) {
            EXPECT_EQ(order[i], i);
        }

        value reused(42);
        if constexpr (std::same_as<Head, ring>) {
            head.push_back(static_cast<ring *>(&reused));
        } else {
            head.push_front(static_cast<chain *>(&reused));
        }
        int calls = 0;
        wjr::intrusive::clear_and_dispose(head, move_only_disposer(calls));
        EXPECT_EQ(calls, 1);
        EXPECT_TRUE(head.empty());
    }
}

} // namespace

TEST(intrusive_dispose, destroy_ring_elements_and_reuse_head) { dispose_owned_nodes<ring>(); }

TEST(intrusive_dispose, destroy_chain_elements_and_reuse_head) { dispose_owned_nodes<chain>(); }

TEST(intrusive_dispose, relink_current_hook_and_preserve_other_hooks) {
    ring source, destination;
    other_ring independent;
    value first(1), second(2), third(3);
    for (auto *node : {&first, &second, &third}) {
        source.push_back(static_cast<ring *>(node));
        independent.push_back(static_cast<other_ring *>(node));
    }

    wjr::intrusive::clear_and_dispose(
        source, [&](value *node) noexcept { destination.push_back(static_cast<ring *>(node)); });
    EXPECT_TRUE(source.empty());
    auto *current = destination.next();
    auto *other = independent.next();
    for (auto *node : {&first, &second, &third}) {
        EXPECT_EQ(current->self(), node);
        EXPECT_EQ(other->self(), node);
        current = current->next();
        other = other->next();
    }
    EXPECT_EQ(current, &destination);
    EXPECT_EQ(other, &independent);
    EXPECT_EQ(destination.prev()->self(), &third);
    EXPECT_EQ(independent.prev()->self(), &third);
}

TEST(intrusive_dispose, raw_hooks_are_not_implicitly_destroyed) {
    wjr::intrusive::list_node<> head, first, second;
    head.push_back(&first);
    head.push_back(&second);
    std::array<void *, 2> visited{};
    size_t count = 0;
    wjr::intrusive::clear_and_dispose(head, [&](void *node) noexcept { visited[count++] = node; });
    EXPECT_EQ(count, 2u);
    EXPECT_EQ(visited[0], &first);
    EXPECT_EQ(visited[1], &second);
    EXPECT_TRUE(head.empty());
    head.push_front(&second);
    EXPECT_EQ(head.pop_front(), &second);
    EXPECT_TRUE(head.empty());
}
