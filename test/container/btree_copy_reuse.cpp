#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <functional>
#include <map>
#include <memory_resource>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {
class resource : public std::pmr::memory_resource {
    void *do_allocate(size_t bytes, size_t alignment) override {
        ++allocations;
        live_bytes += bytes;
        return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    }
    void do_deallocate(void *p, size_t bytes, size_t alignment) override {
        ++deallocations;
        live_bytes -= bytes;
        std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }
    bool do_is_equal(const memory_resource &other) const noexcept override {
        return this == &other;
    }

public:
    size_t allocations = 0, deallocations = 0, live_bytes = 0;
};
using allocator = std::pmr::polymorphic_allocator<char>;

template <typename Tree, bool Reuse = true>
void check_sizes(bool duplicates = false) {
    for (int count : {0, 1, 2, 3, 17, 80, 1024}) {
        resource first, second;
        {
            Tree source(std::less<>{}, allocator(&first)),
                target(std::less<>{}, allocator(&second));
            for (int i = 0; i < count; ++i) {
                if constexpr (std::is_void_v<typename Tree::mapped_type>) {
                    source.emplace(duplicates ? i / 2 : i);
                    target.emplace(10000 + (duplicates ? i / 2 : i));
                } else {
                    source.try_emplace(i, i * 3);
                    target.try_emplace(10000 + i, -i);
                }
            }
            const auto allocations = second.allocations;
            const auto deallocations = second.deallocations;
            const auto bytes = second.live_bytes;
            target = source;
            if constexpr (Reuse) {
                EXPECT_EQ(second.allocations, allocations);
                EXPECT_EQ(second.deallocations, deallocations);
            } else if (count > 2) {
                EXPECT_GT(second.allocations, allocations);
            }
            EXPECT_EQ(second.live_bytes, bytes);
            EXPECT_EQ(target.get_allocator().resource(), &second);
            EXPECT_EQ(source.get_allocator().resource(), &first);
            ASSERT_EQ(target.size(), source.size());
            EXPECT_TRUE(std::equal(target.begin(), target.end(), source.begin()));
            for (int i = 0; i < count; ++i) {
                EXPECT_NE(target.find(duplicates ? i / 2 : i), target.end());
                if constexpr (!std::is_void_v<typename Tree::mapped_type>)
                    EXPECT_EQ(target.at(i), i * 3);
            }
            auto &alias = target;
            const auto before_self = second.allocations;
            target = alias;
            EXPECT_EQ(second.allocations, before_self);
            target.clear();
            EXPECT_TRUE(target.empty());
            EXPECT_EQ(second.live_bytes, 0u);
            if (count != 0)
                target.emplace(*source.begin());
        }
        EXPECT_EQ(first.live_bytes, 0u);
        EXPECT_EQ(second.live_bytes, 0u);
    }
}

TEST(BtreeCopyReuse, MatchedShapesReuseAllocationsForAllFamilies) {
    check_sizes<wjr::btree_map<int, int, std::less<>, allocator, 5>>();
    check_sizes<wjr::experimental::compact_btree_map<int, int, std::less<>, allocator, 5>, false>();
    check_sizes<wjr::btree_set<int, std::less<>, allocator, 5>, false>();
    check_sizes<wjr::btree_multiset<int, std::less<>, allocator, 5>, false>(true);
}

template <typename Tree>
void check_mismatch() {
    resource first, second;
    {
        Tree source(std::less<>{}, allocator(&first)), target(std::less<>{}, allocator(&second));
        for (int i = 0; i < 80; ++i) {
            source.try_emplace(i, i);
            target.try_emplace(i + 10000, i);
        }
        target.erase(target.begin());
        target.try_emplace(20000, 1);
        const auto allocations = second.allocations;
        target = source;
        EXPECT_LE(second.allocations - allocations, first.allocations);
        EXPECT_EQ(second.live_bytes, first.live_bytes);
        for (int i = 0; i < 80; ++i)
            EXPECT_EQ(target.at(i), i);
        target.clear();
        target.try_emplace(7, 7);
        target = source;
        EXPECT_EQ(target.size(), source.size());
        source.clear();
        source.try_emplace(1, 1);
        source.try_emplace(2, 2);
        target.clear();
        target.try_emplace(5, 5);
        target.try_emplace(6, 6);
        target.try_emplace(7, 7);
        target.erase(7);
        target = source;
        EXPECT_EQ(second.live_bytes, first.live_bytes);
        EXPECT_EQ(target.at(1), 1);
        EXPECT_EQ(target.at(2), 2);
    }
    EXPECT_EQ(first.live_bytes, 0u);
    EXPECT_EQ(second.live_bytes, 0u);
}
TEST(BtreeCopyReuse, ShapeAndSizeMismatchesRetainFreshCopyAndSsoFootprint) {
    check_mismatch<wjr::btree_map<int, int, std::less<>, allocator, 5>>();
    check_mismatch<wjr::experimental::compact_btree_map<int, int, std::less<>, allocator, 5>>();
}

struct ordering {
    bool reverse;
    bool operator()(int a, int b) const noexcept { return reverse ? a > b : a < b; }
};
TEST(BtreeCopyReuse, ComparatorStateAndSeparatorsAreReplacedTogether) {
    using tree = wjr::btree_map<int, int, ordering, allocator, 5>;
    resource first, second;
    {
        tree source(ordering{true}, allocator(&first)), target(ordering{false}, allocator(&second));
        for (int i = 0; i < 80; ++i) {
            source.try_emplace(79 - i, i);
            target.try_emplace(10000 + i, i);
        }
        const auto allocations = second.allocations;
        target = source;
        EXPECT_EQ(second.allocations, allocations);
        EXPECT_TRUE(target.key_comp().reverse);
        EXPECT_EQ(target.begin()->first, 79);
        for (int i = 0; i < 80; ++i)
            EXPECT_EQ(target.at(i), 79 - i);
        target.try_emplace(100, 7);
        target.erase(40);
        EXPECT_EQ(target.begin()->first, 100);
        EXPECT_FALSE(target.contains(40));
    }
    EXPECT_EQ(first.live_bytes, 0u);
    EXPECT_EQ(second.live_bytes, 0u);
}

TEST(BtreeCopyReuse, IndirectPmrKeysKeepOwnedResourcesAndValidSeparatorPointers) {
    using tree = wjr::btree_map<std::pmr::string, std::pmr::string, std::less<>, allocator, 5>;
    resource first, second;
    {
        tree source(std::less<>{}, allocator(&first)), target(std::less<>{}, allocator(&second));
        for (int i = 0; i < 80; ++i) {
            const auto key = std::string(50, 'x') + std::to_string(i);
            source.try_emplace(key.c_str(), std::string(80, 'a').c_str());
            target.try_emplace(key.c_str(), std::string(80, 'b').c_str());
        }
        const auto bytes = second.live_bytes;
        target = source;
        EXPECT_EQ(second.live_bytes, bytes);
        for (const auto &[key, value] : target) {
            EXPECT_EQ(key.get_allocator().resource(), &second);
            EXPECT_EQ(value.get_allocator().resource(), &second);
            EXPECT_EQ(value.front(), 'a');
            EXPECT_EQ(target.find(std::string_view(key))->first, key);
        }
        source.clear();
        for (int i = 0; i < 80; ++i) {
            const auto key = std::string(50, 'x') + std::to_string(i);
            EXPECT_EQ(target.at(std::string_view(key)).size(), 80u);
        }
    }
    EXPECT_EQ(first.live_bytes, 0u);
    EXPECT_EQ(second.live_bytes, 0u);
}

struct alignas(64) counted {
    int number;
    static inline int live = 0, copies = 0;
    explicit counted(int n) noexcept : number(n) { ++live; }
    counted(const counted &other) noexcept : number(other.number) {
        ++live;
        ++copies;
    }
    counted(counted &&other) noexcept : number(other.number) { ++live; }
    ~counted() noexcept { --live; }
};
struct hooks {
    struct node_record {
        std::function<int()> size;
        std::function<void *()> parent;
        std::function<unsigned()> position;
        std::function<std::vector<void *>()> children;
        std::function<std::vector<int>()> separators;
        std::function<int()> minimum;
        void *list = nullptr;
        std::function<void *()> next;
        std::function<void *()> previous;
    };
    size_t allocations = 0, blocks = 0, constructs = 0, destroys = 0;
    size_t bytes = 0, peak = 0;
    std::map<void *, node_record> nodes;
    std::function<void()> observe;
};
template <typename T>
struct hooked_allocator {
    using value_type = T;
    hooks *state = nullptr;
    hooked_allocator() = default;
    explicit hooked_allocator(hooks &s) noexcept : state(&s) {}
    template <typename U>
    hooked_allocator(const hooked_allocator<U> &other) noexcept : state(other.state) {}
    T *allocate(size_t n) {
        ++state->allocations;
        ++state->blocks;
        state->bytes += n * sizeof(T);
        state->peak = std::max(state->peak, state->bytes);
        return std::allocator<T>{}.allocate(n);
    }
    void deallocate(T *p, size_t n) noexcept {
        --state->blocks;
        state->bytes -= n * sizeof(T);
        std::allocator<T>{}.deallocate(p, n);
    }
    template <typename U, typename... Args>
    void construct(U *p, Args &&...args) {
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(p) % alignof(U), 0u);
        if constexpr (std::same_as<U, std::pair<const int, counted>>) {
            ++state->constructs;
            if (state->observe)
                state->observe();
        }
        std::construct_at(p, std::forward<Args>(args)...);
        if constexpr (requires {
                          p->size();
                          p->m_parent;
                          p->pos();
                      }) {
            using base_pointer = decltype(p->m_parent);
            base_pointer node = p;
            hooks::node_record record;
            record.size = [p] { return p->size(); };
            record.parent = [p] { return static_cast<void *>(p->m_parent); };
            record.position = [p] { return p->pos(); };
            if constexpr (requires { p->m_sons[0]; }) {
                record.children = [p] {
                    std::vector<void *> result;
                    for (int i = 0; i <= p->size(); ++i)
                        result.push_back(p->m_sons[i]);
                    return result;
                };
                record.separators = [p] {
                    std::vector<int> result;
                    for (int i = 0; i < p->size(); ++i)
                        result.push_back(p->m_keys[i]);
                    return result;
                };
            } else {
                using list_pointer = decltype(p->next());
                list_pointer list = p;
                record.list = list;
                record.minimum = [p] { return p->get_key(0); };
                record.next = [p] { return static_cast<void *>(p->next()); };
                record.previous = [p] { return static_cast<void *>(p->prev()); };
            }
            state->nodes.emplace(node, std::move(record));
        }
    }
    template <typename U>
    void destroy(U *p) noexcept {
        if constexpr (requires {
                          p->size();
                          p->m_parent;
                          p->pos();
                      }) {
            using base_pointer = decltype(p->m_parent);
            base_pointer node = p;
            EXPECT_EQ(state->nodes.erase(node), 1u);
        }
        if constexpr (std::same_as<U, std::pair<const int, counted>>) {
            ++state->destroys;
            if (state->observe)
                state->observe();
        }
        std::destroy_at(p);
    }
    bool operator==(const hooked_allocator &other) const noexcept { return state == other.state; }
};
template <typename Tree, bool Reuse = true>
void check_hooks() {
    for (int count : {1, 2, 3, 80}) {
        hooks first, second;
        {
            Tree source(std::less<>{}, hooked_allocator<char>(first)),
                target(std::less<>{}, hooked_allocator<char>(second));
            for (int i = 0; i < count; ++i) {
                source.try_emplace(i, i);
                target.try_emplace(10000 + i, -i);
            }
            const auto allocations = second.allocations, constructs = second.constructs,
                       destroys = second.destroys;
            const auto copies = counted::copies;
            if constexpr (Reuse) {
                second.observe = [&] {
                    EXPECT_TRUE(target.empty());
                    EXPECT_EQ(target.begin(), target.end());
                    EXPECT_EQ(source.size(), count);
                    EXPECT_EQ(source.at(0).number, 0);
                };
            }
            target = source;
            second.observe = {};
            if constexpr (Reuse)
                EXPECT_EQ(second.allocations, allocations);
            EXPECT_EQ(second.constructs - constructs, count);
            EXPECT_EQ(second.destroys - destroys, count);
            EXPECT_EQ(counted::copies - copies, count);
            for (int i = 0; i < count; ++i)
                EXPECT_EQ(target.at(i).number, i);
        }
        EXPECT_EQ(first.blocks, 0u);
        EXPECT_EQ(second.blocks, 0u);
        EXPECT_EQ(counted::live, 0);
    }
}
TEST(BtreeCopyReuse, TypedHooksObserveEmptyDestinationAndBalancedOveralignedLifetimes) {
    check_hooks<wjr::btree_map<int, counted, std::less<>, hooked_allocator<char>, 5>>();
    check_hooks<
        wjr::experimental::compact_btree_map<int, counted, std::less<>, hooked_allocator<char>, 5>,
        false>();
}

struct owned_key {
    int number;
    explicit owned_key(int n) noexcept : number(n) {}
    ~owned_key() noexcept {}
    friend bool operator<(const owned_key &a, const owned_key &b) noexcept {
        return a.number < b.number;
    }
    friend bool operator==(const owned_key &, const owned_key &) = default;
};
TEST(BtreeCopyReuse, IndirectSetAndMultisetKeysRetainTheirValueAllocations) {
    resource first, second;
    {
        using set = wjr::btree_set<owned_key, std::less<>, allocator, 5>;
        using multi = wjr::btree_multiset<owned_key, std::less<>, allocator, 5>;
        set source(std::less<>{}, allocator(&first)), target(std::less<>{}, allocator(&second));
        multi source_multi(std::less<>{}, allocator(&first)),
            target_multi(std::less<>{}, allocator(&second));
        for (int i = 0; i < 80; ++i) {
            source.emplace(i);
            target.emplace(i + 1000);
            source_multi.emplace(i / 2);
            target_multi.emplace(i / 2 + 1000);
        }
        const auto allocations = second.allocations;
        target = source;
        target_multi = source_multi;
        EXPECT_EQ(second.allocations, allocations);
        EXPECT_TRUE(std::equal(target.begin(), target.end(), source.begin()));
        EXPECT_TRUE(std::equal(target_multi.begin(), target_multi.end(), source_multi.begin()));
        for (int i = 0; i < 40; ++i)
            EXPECT_EQ(target_multi.count(owned_key(i)), 2u);
    }
    EXPECT_EQ(first.live_bytes, 0u);
    EXPECT_EQ(second.live_bytes, 0u);
}

struct graph_snapshot {
    std::vector<int> slots;
    std::vector<void *> leaves;
};
int inspect_branch(hooks &state, void *node, graph_snapshot &out) {
    auto &record = state.nodes.at(node);
    if (record.size() < 0) {
        out.slots.push_back(-record.size());
        out.leaves.push_back(node);
        return record.minimum();
    }
    const auto children = record.children();
    const auto separators = record.separators();
    int first = 0;
    for (size_t i = 0; i < children.size(); ++i) {
        const auto &child = state.nodes.at(children[i]);
        EXPECT_EQ(child.parent(), node);
        EXPECT_EQ(child.position(), i);
        const auto minimum = inspect_branch(state, children[i], out);
        if (i == 0)
            first = minimum;
        else
            EXPECT_EQ(separators[i - 1], minimum);
    }
    return first;
}
graph_snapshot inspect_graph(hooks &state) {
    graph_snapshot result;
    void *root = nullptr;
    for (const auto &[node, record] : state.nodes) {
        if (record.parent() == nullptr) {
            EXPECT_EQ(root, nullptr);
            root = node;
        }
    }
    if (root == nullptr)
        return result;
    inspect_branch(state, root, result);
    for (size_t i = 0; i < result.leaves.size(); ++i) {
        const auto &leaf = state.nodes.at(result.leaves[i]);
        if (i != 0)
            EXPECT_EQ(leaf.previous(), state.nodes.at(result.leaves[i - 1]).list);
        if (i + 1 != result.leaves.size())
            EXPECT_EQ(leaf.next(), state.nodes.at(result.leaves[i + 1]).list);
    }
    EXPECT_EQ(state.nodes.at(result.leaves.front()).previous(),
              state.nodes.at(result.leaves.back()).next());
    return result;
}

template <typename Tree>
void redistribute_leaves(Tree &tree, int matched_percent, bool late) {
    constexpr size_t leaves = 455;
    const size_t changed = (leaves * (100 - matched_percent) + 50) / 100;
    const size_t start = late ? leaves - changed : 0;
    const size_t paired = changed % 2 ? changed - 3 : changed;
    for (size_t i = 0; i < paired; i += 2) {
        tree.erase(static_cast<int>(4 * ((start + i) * 9 + 1)));
        tree.try_emplace(static_cast<int>(4 * ((start + i + 1) * 9 + 1) + 1), -1);
    }
    if (paired != changed) {
        const size_t first = start + paired;
        tree.erase(static_cast<int>(4 * (first * 9 + 1)));
        tree.erase(static_cast<int>(4 * ((first + 1) * 9 + 1)));
        tree.try_emplace(static_cast<int>(4 * ((first + 2) * 9 + 1) + 1), -1);
        tree.try_emplace(static_cast<int>(4 * ((first + 2) * 9 + 1) + 2), -1);
    }
}

TEST(BtreeCopyReuse, RecursiveLeafOccupancyMatchesNeverSkipValueCopiesOrRetainWrongShape) {
    using tree = wjr::btree_map<int, counted, std::less<>, hooked_allocator<char>, 16>;
    for (int percent : {0, 25, 50, 75, 100}) {
        for (bool late : {false, true}) {
            hooks first, second;
            {
                tree source(std::less<>{}, hooked_allocator<char>(first)),
                    target(std::less<>{}, hooked_allocator<char>(second));
                for (int i = 0; i < 4096; ++i) {
                    source.try_emplace(4 * i, i);
                    target.try_emplace(4 * i, i);
                }
                redistribute_leaves(target, percent, late);
                const auto source_graph = inspect_graph(first),
                           target_graph = inspect_graph(second);
                ASSERT_EQ(source_graph.slots.size(), 455u);
                ASSERT_EQ(source_graph.slots.size(), target_graph.slots.size());
                size_t matched = 0;
                for (size_t i = 0; i < 455; ++i)
                    matched += source_graph.slots[i] == target_graph.slots[i];
                const size_t changed = (455 * (100 - percent) + 50) / 100;
                EXPECT_EQ(matched, 455 - changed);
                const auto allocations = second.allocations, constructs = second.constructs,
                           destroys = second.destroys, bytes = second.bytes;
                const auto copies = counted::copies;
                second.peak = bytes;
                second.observe = [&] {
                    EXPECT_TRUE(target.empty());
                    EXPECT_EQ(target.begin(), target.end());
                    EXPECT_EQ(source.size(), 4096u);
                    EXPECT_EQ(source.at(0).number, 0);
                };
                target = source;
                second.observe = {};
                EXPECT_EQ(second.allocations - allocations, (changed + 1) / 2);
                EXPECT_EQ(second.constructs - constructs, 4096u);
                EXPECT_EQ(second.destroys - destroys, 4096u);
                EXPECT_EQ(counted::copies - copies, 4096);
                EXPECT_EQ(second.bytes, first.bytes);
                EXPECT_LE(second.peak, bytes + 2 * sizeof(tree::value_type));
                EXPECT_EQ(inspect_graph(second).slots, source_graph.slots);
                for (int i = 0; i < 4096; ++i)
                    EXPECT_EQ(target.at(4 * i).number, i);
            }
            EXPECT_TRUE(first.nodes.empty());
            EXPECT_TRUE(second.nodes.empty());
            EXPECT_EQ(first.blocks, 0u);
            EXPECT_EQ(second.blocks, 0u);
            EXPECT_EQ(counted::live, 0);
        }
    }
}

TEST(BtreeCopyReuse, RecursiveDifferentSizesKindsAndSsoPublishACompleteOwnedGraph) {
    using tree = wjr::btree_map<int, counted, std::less<>, hooked_allocator<char>, 5>;
    for (int source_size : {0, 1, 2, 3, 8, 17, 80, 257}) {
        for (int target_size : {0, 1, 2, 3, 8, 17, 80, 257}) {
            hooks first, second;
            {
                tree source(std::less<>{}, hooked_allocator<char>(first)),
                    target(std::less<>{}, hooked_allocator<char>(second));
                for (int i = 0; i < source_size; ++i)
                    source.try_emplace(i * 4, i);
                for (int i = 0; i < target_size; ++i)
                    target.try_emplace(10000 + i * 3, -i);
                const auto constructs = second.constructs, destroys = second.destroys;
                second.observe = [&] {
                    EXPECT_TRUE(target.empty());
                    EXPECT_EQ(target.begin(), target.end());
                    EXPECT_EQ(source.size(), source_size);
                };
                target = source;
                second.observe = {};
                EXPECT_EQ(second.constructs - constructs, source_size);
                EXPECT_EQ(second.destroys - destroys, target_size);
                EXPECT_EQ(second.bytes, first.bytes);
                EXPECT_EQ(inspect_graph(second).slots, inspect_graph(first).slots);
                ASSERT_EQ(target.size(), source_size);
                for (int i = 0; i < source_size; ++i)
                    EXPECT_EQ(target.at(i * 4).number, i);
                target.try_emplace(-1, 7);
                target.erase(-1);
            }
            EXPECT_EQ(first.blocks, 0u);
            EXPECT_EQ(second.blocks, 0u);
            EXPECT_EQ(counted::live, 0);
        }
    }
}
} // namespace
