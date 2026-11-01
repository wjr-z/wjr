#include "../detail.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <memory_resource>
#include <random>
#include <scoped_allocator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <wjr/container/lru_cache.hpp>

namespace {
using basic_cache = wjr::lru_cache<int, int>;
static_assert(!std::is_copy_constructible_v<basic_cache>);
static_assert(!std::is_move_constructible_v<basic_cache>);
static_assert(!std::is_copy_assignable_v<basic_cache>);
static_assert(!std::is_move_assignable_v<basic_cache>);
static_assert(std::is_nothrow_destructible_v<basic_cache>);

struct throwing_hash {
    size_t operator()(int) const noexcept(false);
};
template <typename Hash>
concept accepted_hash = requires { typename wjr::lru_cache<int, int, Hash>; };
static_assert(!accepted_hash<throwing_hash>);

struct immovable {
    static inline int constructions = 0;
    explicit immovable(int v) : value(v) { ++constructions; }
    immovable(const immovable &) = delete;
    immovable(immovable &&) = delete;
    int value;
};

struct allocation_state {
    std::map<void *, size_t> blocks;
    size_t calls = 0, constructs = 0, destroys = 0;
    size_t remaining = std::numeric_limits<size_t>::max();
    size_t construction_remaining = std::numeric_limits<size_t>::max();
};

template <typename T>
struct tracked_allocator {
    using value_type = T;
    allocation_state *state;
    explicit tracked_allocator(allocation_state &s) noexcept : state(&s) {}
    template <typename U>
    tracked_allocator(const tracked_allocator<U> &other) noexcept : state(other.state) {}
    T *allocate(size_t n) {
        ++state->calls;
        if (state->remaining == 0)
            throw std::bad_alloc();
        if (state->remaining != std::numeric_limits<size_t>::max())
            --state->remaining;
        auto *pointer = std::allocator<T>{}.allocate(n);
        state->blocks.emplace(pointer, n * sizeof(T));
        return pointer;
    }
    void deallocate(T *pointer, size_t n) noexcept {
        const auto iterator = state->blocks.find(pointer);
        EXPECT_NE(iterator, state->blocks.end());
        if (iterator != state->blocks.end()) {
            EXPECT_EQ(iterator->second, n * sizeof(T));
            state->blocks.erase(iterator);
        }
        std::allocator<T>{}.deallocate(pointer, n);
    }
    template <typename U, typename... Args>
    void construct(U *pointer, Args &&...args) {
        if (state->construction_remaining == 0)
            throw std::runtime_error("allocator construct");
        if (state->construction_remaining != std::numeric_limits<size_t>::max())
            --state->construction_remaining;
        std::construct_at(pointer, std::forward<Args>(args)...);
        ++state->constructs;
    }
    template <typename U>
    void destroy(U *pointer) noexcept {
        ++state->destroys;
        std::destroy_at(pointer);
    }
    template <typename U>
    bool operator==(const tracked_allocator<U> &other) const noexcept {
        return state == other.state;
    }
};

struct controlled_value {
    static inline int live = 0, peak = 0;
    static inline bool fail_construct = false, fail_assign = false;
    explicit controlled_value(int v) : value(v) {
        if (fail_construct)
            throw std::runtime_error("value construct");
        peak = std::max(peak, ++live);
    }
    controlled_value(const controlled_value &) = delete;
    controlled_value(controlled_value &&) = delete;
    controlled_value &operator=(int v) {
        value = v;
        if (fail_assign)
            throw std::runtime_error("value assign");
        return *this;
    }
    ~controlled_value() { --live; }
    int value;
};

struct equal_state {
    size_t remaining = std::numeric_limits<size_t>::max(), calls = 0;
};
struct collision_hash {
    size_t operator()(int) const noexcept { return 0; }
};
struct salted_hash {
    explicit salted_hash(size_t v) noexcept : salt(v) {}
    size_t operator()(int value) const noexcept { return static_cast<size_t>(value) ^ salt; }
    size_t salt;
};
struct controlled_equal {
    equal_state *state;
    bool operator()(int a, int b) const {
        ++state->calls;
        if (state->remaining == 0)
            throw std::runtime_error("equality");
        if (state->remaining != std::numeric_limits<size_t>::max())
            --state->remaining;
        return a == b;
    }
};

struct tracked_key {
    static inline bool fail_copy = false;
    explicit tracked_key(int v) : value(v) {}
    tracked_key(const tracked_key &other) : value(other.value) {
        if (fail_copy)
            throw std::runtime_error("key copy");
    }
    tracked_key(tracked_key &&) = default;
    bool operator==(const tracked_key &) const = default;
    int value;
};
struct key_hash {
    size_t operator()(const tracked_key &key) const noexcept { return key.value; }
};

struct string_hash {
    using is_transparent = void;
    size_t operator()(std::string_view value) const noexcept {
        return std::hash<std::string_view>{}(value);
    }
};
struct alignas(128) aligned_value {
    explicit aligned_value(int n) : value(n) {}
    int value;
};

struct resource_value {
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;
    resource_value(std::allocator_arg_t, allocator_type allocator, int v)
        : resource(allocator.resource()), value(v) {}
    resource_value(const resource_value &) = delete;
    resource_value(resource_value &&) = delete;
    std::pmr::memory_resource *resource;
    int value;
};
struct resource_equal {
    std::pmr::memory_resource *expected;
    bool *correct;
    bool operator()(const std::pmr::string &left, const std::pmr::string &right) const noexcept {
        *correct = *correct && left.get_allocator().resource() == expected &&
                   right.get_allocator().resource() == expected;
        return left == right;
    }
};
} // namespace

TEST(lru_cache, exact_recency_zero_one_and_clear) {
    wjr::lru_cache<int, immovable> zero(0);
    immovable::constructions = 0;
    EXPECT_EQ(zero.try_emplace(1, 7), (std::pair<immovable *, bool>{nullptr, false}));
    EXPECT_EQ(immovable::constructions, 0);
    EXPECT_FALSE(zero.get(1));
    EXPECT_FALSE(zero.peek(1));
    EXPECT_FALSE(zero.erase(1));
    basic_cache one(1);
    one.try_emplace(1, 7);
    one.try_emplace(2, 8);
    EXPECT_FALSE(one.peek(1));
    ASSERT_TRUE(one.peek(2));
    EXPECT_EQ(*one.peek(2), 8);
    basic_cache cache(2);
    EXPECT_TRUE(cache.try_emplace(1, 10).second);
    EXPECT_TRUE(cache.try_emplace(2, 20).second);
    ASSERT_TRUE(cache.get(1));
    cache.try_emplace(3, 30);
    EXPECT_FALSE(cache.peek(2));
    EXPECT_TRUE(cache.peek(1));
    EXPECT_TRUE(cache.peek(3));
    cache.peek(1);
    cache.try_emplace(4, 40);
    EXPECT_FALSE(cache.peek(1));
    EXPECT_TRUE(cache.peek(3));
    EXPECT_TRUE(cache.peek(4));
    EXPECT_FALSE(cache.erase(99));
    EXPECT_TRUE(cache.erase(3));
    cache.clear();
    EXPECT_TRUE(cache.empty());
    EXPECT_EQ(cache.capacity(), 2);
    cache.try_emplace(5, 50);
    EXPECT_EQ(*cache.get(5), 50);
}

TEST(lru_cache, duplicate_construction_assignment_and_stable_borrowing) {
    wjr::lru_cache<int, immovable> cache(2);
    immovable::constructions = 0;
    auto *first = cache.try_emplace(1, 10).first;
    cache.try_emplace(2, 20);
    const auto duplicate = cache.try_emplace(1, 99);
    EXPECT_FALSE(duplicate.second);
    EXPECT_EQ(duplicate.first, first);
    EXPECT_EQ(first->value, 10);
    EXPECT_EQ(immovable::constructions, 2);
    cache.try_emplace(3, 30);
    EXPECT_EQ(cache.peek(1), first);
    EXPECT_FALSE(cache.peek(2));
    basic_cache assigned(2);
    assigned.try_emplace(1, 1);
    assigned.try_emplace(2, 2);
    EXPECT_FALSE(assigned.insert_or_assign(1, 11).second);
    assigned.try_emplace(3, 3);
    EXPECT_FALSE(assigned.peek(2));
    EXPECT_EQ(*assigned.peek(1), 11);
    EXPECT_TRUE(assigned.insert_or_assign(4, 4).second);
    EXPECT_EQ(assigned.size(), 2);
}

TEST(lru_cache, value_failure_preserves_victim_and_assignment_preserves_order) {
    controlled_value::live = controlled_value::peak = 0;
    {
        wjr::lru_cache<int, controlled_value> cache(2);
        cache.try_emplace(1, 10);
        cache.try_emplace(2, 20);
        controlled_value::fail_construct = true;
        EXPECT_THROW(cache.try_emplace(3, 30), std::runtime_error);
        controlled_value::fail_construct = false;
        EXPECT_EQ(cache.size(), 2);
        EXPECT_TRUE(cache.peek(1));
        EXPECT_TRUE(cache.peek(2));
        controlled_value::fail_assign = true;
        EXPECT_THROW(cache.insert_or_assign(1, 11), std::runtime_error);
        controlled_value::fail_assign = false;
        EXPECT_EQ(cache.peek(1)->value, 11);
        cache.try_emplace(3, 30);
        EXPECT_FALSE(cache.peek(1));
        EXPECT_TRUE(cache.peek(2));
        EXPECT_EQ(controlled_value::live, 2);
        EXPECT_EQ(controlled_value::peak, 3);
    }
    EXPECT_EQ(controlled_value::live, 0);
}

TEST(lru_cache, key_failure_does_not_evict) {
    wjr::lru_cache<tracked_key, int, key_hash> cache(2);
    tracked_key one(1), two(2), three(3), four(4);
    cache.try_emplace(one, 10);
    cache.try_emplace(two, 20);
    tracked_key::fail_copy = true;
    EXPECT_THROW(cache.try_emplace(three, 30), std::runtime_error);
    tracked_key::fail_copy = false;
    EXPECT_EQ(cache.size(), 2);
    EXPECT_TRUE(cache.peek(one));
    cache.try_emplace(four, 40);
    EXPECT_FALSE(cache.peek(one));
    EXPECT_TRUE(cache.peek(two));
    wjr::lru_cache<tracked_key, int, key_hash> zero(0);
    tracked_key::fail_copy = true;
    EXPECT_FALSE(zero.try_emplace(one, 1).second);
    tracked_key::fail_copy = false;
}

TEST(lru_cache, lookup_failures_and_stateful_hash_preserve_order) {
    equal_state state;
    wjr::lru_cache<int, int, salted_hash, controlled_equal> cache(2, salted_hash(17),
                                                                  controlled_equal{&state});
    cache.try_emplace(1, 10);
    cache.try_emplace(2, 20);
    state.remaining = 0;
    EXPECT_THROW(cache.get(1), std::runtime_error);
    state.remaining = std::numeric_limits<size_t>::max();
    cache.try_emplace(3, 30);
    EXPECT_FALSE(cache.peek(1));
    EXPECT_TRUE(cache.peek(2));
    state.remaining = 0;
    EXPECT_THROW(cache.erase(3), std::runtime_error);
    state.remaining = std::numeric_limits<size_t>::max();
    cache.try_emplace(4, 40);
    EXPECT_FALSE(cache.peek(2));
    EXPECT_TRUE(cache.peek(3));
    EXPECT_GT(state.calls, 0);
}

TEST(lru_cache, every_equality_failure_leaves_membership_and_recency) {
    using cache_type = wjr::lru_cache<int, int, collision_hash, controlled_equal>;
    equal_state baseline_state;
    size_t comparisons = 0;
    {
        cache_type baseline(2, {}, controlled_equal{&baseline_state});
        baseline.try_emplace(1, 10);
        baseline.try_emplace(2, 20);
        baseline_state.calls = 0;
        baseline.try_emplace(3, 30);
        comparisons = baseline_state.calls;
    }
    ASSERT_GT(comparisons, 0);
    for (size_t fail = 0; fail < comparisons; ++fail) {
        equal_state state;
        cache_type cache(2, {}, controlled_equal{&state});
        cache.try_emplace(1, 10);
        cache.try_emplace(2, 20);
        state.remaining = fail;
        EXPECT_THROW(cache.try_emplace(3, 30), std::runtime_error);
        state.remaining = std::numeric_limits<size_t>::max();
        EXPECT_EQ(cache.size(), 2);
        EXPECT_EQ(*cache.peek(1), 10);
        EXPECT_EQ(*cache.peek(2), 20);
        cache.try_emplace(4, 40);
        EXPECT_FALSE(cache.peek(1));
        EXPECT_TRUE(cache.peek(2));
    }
}

TEST(lru_cache, allocator_reserve_and_admission_failures_are_balanced) {
    using allocator = tracked_allocator<std::pair<const int, int>>;
    using cache_type = wjr::lru_cache<int, int, std::hash<int>, std::equal_to<int>, allocator>;
    size_t constructor_calls = 0;
    {
        allocation_state state;
        {
            cache_type cache(32, {}, {}, allocator(state));
            constructor_calls = state.calls;
        }
        EXPECT_TRUE(state.blocks.empty());
        EXPECT_EQ(state.constructs, state.destroys);
    }
    ASSERT_GT(constructor_calls, 0);
    for (size_t fail = 0; fail < constructor_calls; ++fail) {
        allocation_state state;
        state.remaining = fail;
        EXPECT_THROW((void)cache_type(32, {}, {}, allocator(state)), std::bad_alloc);
        EXPECT_TRUE(state.blocks.empty());
        EXPECT_EQ(state.constructs, state.destroys);
    }
    allocation_state state;
    {
        cache_type cache(2, {}, {}, allocator(state));
        cache.try_emplace(1, 10);
        cache.try_emplace(2, 20);
        const auto live = state.blocks.size();
        state.remaining = 0;
        EXPECT_THROW(cache.try_emplace(3, 30), std::bad_alloc);
        state.remaining = std::numeric_limits<size_t>::max();
        EXPECT_EQ(state.blocks.size(), live);
        state.construction_remaining = 0;
        EXPECT_THROW(cache.try_emplace(3, 30), std::runtime_error);
        state.construction_remaining = std::numeric_limits<size_t>::max();
        EXPECT_EQ(state.blocks.size(), live);
        cache.try_emplace(4, 40);
        EXPECT_FALSE(cache.peek(1));
        EXPECT_TRUE(cache.peek(2));
        EXPECT_EQ(cache.get_allocator().state, &state);
        cache.clear();
        EXPECT_TRUE(cache.empty());
    }
    EXPECT_TRUE(state.blocks.empty());
    EXPECT_EQ(state.constructs, state.destroys);
}

TEST(lru_cache, capacity_overflow_and_high_alignment) {
    EXPECT_THROW((void)basic_cache(std::numeric_limits<size_t>::max()), std::length_error);
    allocation_state state;
    using allocator = tracked_allocator<std::pair<const int, aligned_value>>;
    {
        wjr::lru_cache<int, aligned_value, std::hash<int>, std::equal_to<int>, allocator> cache(
            2, {}, {}, allocator(state));
        auto *value = cache.try_emplace(1, 7).first;
        EXPECT_EQ(reinterpret_cast<uintptr_t>(value) % alignof(aligned_value), 0);
        cache.try_emplace(2, 8);
        cache.get(1);
        cache.try_emplace(3, 9);
        EXPECT_EQ(cache.peek(1), value);
    }
    EXPECT_TRUE(state.blocks.empty());
}

TEST(lru_cache, transparent_lookup_and_borrowed_value_arguments) {
    wjr::lru_cache<std::string, std::string, string_hash, std::equal_to<>> cache(2);
    cache.try_emplace("one", "source");
    cache.try_emplace("two", "other");
    EXPECT_EQ(*cache.peek(std::string_view("one")), "source");
    const auto *victim = cache.peek("one");
    cache.try_emplace("three", *victim);
    EXPECT_FALSE(cache.peek("one"));
    EXPECT_EQ(*cache.peek("three"), "source");
    const auto *key = cache.peek("two");
    cache.try_emplace(*key, "borrowed key before eviction");
    EXPECT_FALSE(cache.peek("two"));
    EXPECT_TRUE(cache.get(std::string_view("other")));
    EXPECT_TRUE(cache.erase(std::string_view("three")));
    EXPECT_FALSE(cache.erase("absent"));
}

TEST(lru_cache, pmr_and_scoped_allocators_reach_payloads) {
    std::pmr::unsynchronized_pool_resource resource;
    using allocator = std::pmr::polymorphic_allocator<std::pair<const int, std::pmr::string>>;
    wjr::lru_cache<int, std::pmr::string, std::hash<int>, std::equal_to<int>, allocator> cache(
        2, {}, {}, allocator(&resource));
    auto *value = cache.try_emplace(1, 100, 'x').first;
    EXPECT_EQ(value->get_allocator().resource(), &resource);
    cache.try_emplace(2, 80, 'y');
    cache.try_emplace(3, 90, 'z');
    cache.clear();

    using payload_allocator = std::pmr::polymorphic_allocator<std::pair<const int, resource_value>>;
    wjr::lru_cache<int, resource_value, std::hash<int>, std::equal_to<int>, payload_allocator>
        aware(1, {}, {}, payload_allocator(&resource));
    EXPECT_EQ(aware.try_emplace(1, 7).first->resource, &resource);

    allocation_state state;
    using scoped =
        std::scoped_allocator_adaptor<tracked_allocator<std::pair<const int, std::pmr::string>>,
                                      std::pmr::polymorphic_allocator<std::byte>>;
    {
        wjr::lru_cache<int, std::pmr::string, std::hash<int>, std::equal_to<int>, scoped> inner(
            1, {}, {},
            scoped(tracked_allocator<std::pair<const int, std::pmr::string>>(state),
                   std::pmr::polymorphic_allocator<std::byte>(&resource)));
        EXPECT_EQ(inner.try_emplace(1, 100, 's').first->get_allocator().resource(), &resource);
        EXPECT_EQ(inner.get_allocator().outer_allocator().state, &state);
        EXPECT_EQ(inner.get_allocator().inner_allocator().outer_allocator().resource(), &resource);
    }
    EXPECT_TRUE(state.blocks.empty());
    EXPECT_EQ(state.constructs, state.destroys);
}

TEST(lru_cache, pmr_keys_and_values_own_resource_backed_storage) {
    std::pmr::unsynchronized_pool_resource resource;
    using key_type = std::pmr::string;
    using allocator = std::pmr::polymorphic_allocator<std::pair<const key_type, std::pmr::string>>;
    bool correct = true;
    wjr::lru_cache<key_type, std::pmr::string, std::hash<key_type>, resource_equal, allocator>
        cache(1, {}, resource_equal{&resource, &correct}, allocator(&resource));
    key_type input(100, 'k', &resource), query(input, &resource);
    cache.try_emplace(input, 100, 'v');
    input.clear();
    input.shrink_to_fit();
    const auto *value = cache.get(query);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->get_allocator().resource(), &resource);
    EXPECT_TRUE(correct);
    EXPECT_EQ(value->size(), 100);
    cache.clear();
}

TEST(lru_cache, randomized_model_including_recency_eviction) {
    std::mt19937 random(731);
    for (size_t capacity : {0, 1, 2, 7, 64}) {
        basic_cache cache(capacity);
        std::list<std::pair<int, int>> model;
        const auto locate = [&](int key) {
            return std::find_if(model.begin(), model.end(),
                                [&](auto value) { return value.first == key; });
        };
        const auto admit = [&](int key, int value, bool assign) {
            auto position = locate(key);
            if (capacity == 0)
                return;
            if (position != model.end()) {
                if (assign)
                    position->second = value;
                model.splice(model.begin(), model, position);
            } else {
                model.emplace_front(key, value);
                if (model.size() > capacity)
                    model.pop_back();
            }
        };
        for (size_t step = 0; step < 4000; ++step) {
            const int key = static_cast<int>(random() % 97), value = static_cast<int>(random());
            const auto position = locate(key);
            switch (random() % 6) {
            case 0:
                cache.try_emplace(key, value);
                admit(key, value, false);
                break;
            case 1:
                cache.insert_or_assign(key, value);
                admit(key, value, true);
                break;
            case 2: {
                auto *found = cache.get(key);
                EXPECT_EQ(found != nullptr, position != model.end());
                if (found) {
                    EXPECT_EQ(*found, position->second);
                    model.splice(model.begin(), model, position);
                }
                break;
            }
            case 3: {
                const auto *found = cache.peek(key);
                EXPECT_EQ(found != nullptr, position != model.end());
                if (found)
                    EXPECT_EQ(*found, position->second);
                break;
            }
            case 4:
                EXPECT_EQ(cache.erase(key), position != model.end());
                if (position != model.end())
                    model.erase(position);
                break;
            case 5:
                if (step % 127 == 0) {
                    cache.clear();
                    model.clear();
                }
                break;
            }
            EXPECT_EQ(cache.size(), model.size());
            EXPECT_LE(cache.size(), capacity);
            for (int probe = 0; probe < 97; ++probe) {
                const auto expected = locate(probe);
                const auto *found = cache.peek(probe);
                EXPECT_EQ(found != nullptr, expected != model.end());
                if (found)
                    EXPECT_EQ(*found, expected->second);
            }
        }
        for (int key = 100; key < 100 + static_cast<int>(capacity); ++key) {
            cache.try_emplace(key, key);
            admit(key, key, false);
            for (const auto &[present, value] : model) {
                ASSERT_TRUE(cache.peek(present));
                EXPECT_EQ(*cache.peek(present), value);
            }
        }
    }
}
