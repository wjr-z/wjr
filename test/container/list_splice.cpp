#include "../detail.hpp"

#include <array>
#include <type_traits>
#include <vector>

#include <wjr/container/list.hpp>

namespace {
using raw_node = wjr::intrusive::list_node<>;

constexpr bool constant_splice() {
    raw_node source, destination, first, second;
    source.push_back(&first);
    source.push_back(&second);
    destination.splice_before(&first, &source);
    if (!source.empty() || destination.next() != &first || destination.prev() != &second) {
        return false;
    }
    first.splice_before(&first);
    second.splice_before(&first);
    source.splice_all_before(&destination);
    source.splice_all_before(&source);
    return destination.empty() && source.next() == &first && source.prev() == &second &&
           first.next() == &second && second.prev() == &first && first.prev() == &source &&
           second.next() == &source;
}

static_assert(constant_splice());
static_assert(sizeof(raw_node) == 2 * sizeof(void *));
static_assert(std::is_trivially_destructible_v<raw_node>);
static_assert(noexcept(std::declval<raw_node &>().splice_before(nullptr, nullptr)));
static_assert(noexcept(std::declval<raw_node &>().splice_before(nullptr)));
static_assert(noexcept(std::declval<raw_node &>().splice_all_before(nullptr)));

struct value : wjr::intrusive::list_node<value> {
    int payload = 42;
};
using node = wjr::intrusive::list_node<value>;
static_assert(!std::is_copy_constructible_v<value>);
static_assert(!std::is_move_constructible_v<value>);
static_assert(sizeof(node) == 2 * sizeof(void *));

template <size_t N>
void link(node &head, std::array<value, N> &nodes, size_t count = N) {
    for (size_t i = 0; i < count; ++i) {
        head.push_back(&nodes[i]);
    }
}

void expect_ring(node &head, const std::vector<node *> &expected) {
    auto *current = head.next();
    auto *previous = &head;
    for (auto *element : expected) {
        ASSERT_EQ(current, element);
        EXPECT_EQ(current->prev(), previous);
        EXPECT_EQ(current->self()->payload, 42);
        previous = current;
        current = current->next();
    }
    EXPECT_EQ(current, &head);
    EXPECT_EQ(head.prev(), previous);
    current = head.prev();
    auto *next = &head;
    for (auto it = expected.rbegin(); it != expected.rend(); ++it) {
        ASSERT_EQ(current, *it);
        EXPECT_EQ(current->next(), next);
        next = current;
        current = current->prev();
    }
    EXPECT_EQ(current, &head);
    EXPECT_EQ(head.next(), next);
    EXPECT_EQ(head.empty(), expected.empty());
}
} // namespace

TEST(intrusive_list, splice_ranges_between_rings) {
    for (size_t first = 0; first <= 5; ++first) {
        for (size_t last = first; last <= 5; ++last) {
            for (size_t pos = 0; pos <= 3; ++pos) {
                node source, destination;
                std::array<value, 5> src;
                std::array<value, 3> dst;
                link(source, src);
                link(destination, dst);
                std::vector<node *> remaining, transferred;
                for (size_t i = 0; i < 5; ++i) {
                    if (i < first || i >= last) {
                        remaining.push_back(&src[i]);
                    }
                }
                for (size_t i = 0; i <= 3; ++i) {
                    if (i == pos) {
                        for (size_t j = first; j < last; ++j) {
                            transferred.push_back(&src[j]);
                        }
                    }
                    if (i < 3) {
                        transferred.push_back(&dst[i]);
                    }
                }
                auto *position = pos == 3 ? &destination : &dst[pos];
                position->splice_before(first == 5 ? &source : &src[first],
                                        last == 5 ? &source : &src[last]);
                expect_ring(source, remaining);
                expect_ring(destination, transferred);
            }
        }
    }
}

TEST(intrusive_list, splice_ranges_within_ring) {
    for (size_t first = 0; first <= 5; ++first) {
        for (size_t last = first; last <= 5; ++last) {
            for (size_t pos = 0; pos <= 5; ++pos) {
                if (pos > first && pos < last) {
                    continue;
                }
                node head;
                std::array<value, 5> nodes;
                link(head, nodes);
                std::vector<node *> expected;
                for (size_t i = 0; i <= 5; ++i) {
                    if (pos == first || pos == last || first == last) {
                        if (i < 5) {
                            expected.push_back(&nodes[i]);
                        }
                        continue;
                    }
                    if (i == pos) {
                        for (size_t j = first; j < last; ++j) {
                            expected.push_back(&nodes[j]);
                        }
                    }
                    if (i < 5 && (i < first || i >= last)) {
                        expected.push_back(&nodes[i]);
                    }
                }
                auto *position = pos == 5 ? &head : &nodes[pos];
                position->splice_before(first == 5 ? &head : &nodes[first],
                                        last == 5 ? &head : &nodes[last]);
                expect_ring(head, expected);
            }
        }
    }
}

TEST(intrusive_list, splice_whole_ring) {
    for (size_t src_size = 0; src_size <= 4; ++src_size) {
        for (size_t dst_size = 0; dst_size <= 3; ++dst_size) {
            for (size_t pos = 0; pos <= dst_size; ++pos) {
                node source, destination;
                std::array<value, 4> src;
                std::array<value, 3> dst;
                link(source, src, src_size);
                link(destination, dst, dst_size);
                std::vector<node *> expected;
                for (size_t i = 0; i <= dst_size; ++i) {
                    if (i == pos) {
                        for (size_t j = 0; j < src_size; ++j) {
                            expected.push_back(&src[j]);
                        }
                    }
                    if (i < dst_size) {
                        expected.push_back(&dst[i]);
                    }
                }
                auto *position = pos == dst_size ? &destination : &dst[pos];
                position->splice_all_before(&source);
                expect_ring(source, {});
                expect_ring(destination, expected);
                destination.splice_all_before(&destination);
                expect_ring(destination, expected);
                source.splice_before(source.next(), &source);
                expect_ring(source, {});
            }
        }
    }
}

TEST(intrusive_list, splice_single_preserves_iterator_and_source_reuse) {
    node ready, deferred;
    value first, second;
    ready.push_back(&first);
    ready.push_back(&second);
    auto saved = ready.begin();
    deferred.splice_before(&first);
    EXPECT_EQ(&*saved, &first);
    EXPECT_EQ(++saved, deferred.end());
    expect_ring(ready, {&second});
    expect_ring(deferred, {&first});
    ready.splice_all_before(&deferred);
    expect_ring(ready, {&second, &first});
    expect_ring(deferred, {});
    deferred.push_back(ready.pop_front());
    expect_ring(ready, {&first});
    expect_ring(deferred, {&second});
}
