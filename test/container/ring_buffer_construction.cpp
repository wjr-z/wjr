#include <wjr/container/ring_buffer.hpp>

#include <array>
#include <initializer_list>
#include <memory>
#include <set>
#include <utility>

#include <gtest/gtest.h>

#if !defined(WJR_DISABLE_EXCEPTIONS)
namespace {

struct construction_error {};
struct construction_state;
construction_state *active_state = nullptr;

struct construction_state {
    construction_state() noexcept : m_previous(std::exchange(active_state, this)) {}
    construction_state(const construction_state &) = delete;
    construction_state &operator=(const construction_state &) = delete;
    ~construction_state() { active_state = m_previous; }

    std::set<const void *> live;
    int attempts = 0, throw_at = 0, move_attempts = 0, move_throw_at = 0;
    int created = 0, destroyed = 0, invalid_destroys = 0;
    int allocations = 0, deallocations = 0, allocation_attempts = 0, allocation_throw_at = 0;
    int hook_attempts = 0, hook_throw_at = 0, hook_created = 0, hook_destroyed = 0;

private:
    construction_state *m_previous;
};

template <bool NothrowMove>
struct tracked_value {
    int value;

    explicit tracked_value(int v = 17) : value(v) { construct(); }
    tracked_value(const tracked_value &other) : value(other.value) { construct(); }
    tracked_value(tracked_value &&other) noexcept(NothrowMove) : value(other.value) {
        other.value = -1;
        if constexpr (!NothrowMove) {
            if (++active_state->move_attempts == active_state->move_throw_at) {
                throw construction_error{};
            }
        }
        register_live();
    }
    ~tracked_value() {
        if (active_state->live.erase(this) == 1) {
            ++active_state->destroyed;
        } else {
            ++active_state->invalid_destroys;
        }
    }

private:
    void construct() {
        if (++active_state->attempts == active_state->throw_at) {
            throw construction_error{};
        }
        register_live();
    }
    void register_live() {
        active_state->live.insert(this);
        ++active_state->created;
    }
};

template <typename T>
struct tracking_allocator {
    using value_type = T;
    construction_state *state = active_state;

    tracking_allocator() = default;
    template <typename U>
    explicit tracking_allocator(const tracking_allocator<U> &other) noexcept : state(other.state) {}

    T *allocate(std::size_t n) {
        if (++state->allocation_attempts == state->allocation_throw_at) {
            throw std::bad_alloc{};
        }
        auto *ptr = std::allocator<T>{}.allocate(n);
        ++state->allocations;
        return ptr;
    }
    void deallocate(T *ptr, std::size_t n) noexcept {
        ++state->deallocations;
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        if (++state->hook_attempts == state->hook_throw_at) {
            throw construction_error{};
        }
        std::construct_at(ptr, std::forward<Args>(args)...);
        ++state->hook_created;
    }
    template <typename U>
    void destroy(U *ptr) noexcept {
        ++state->hook_destroyed;
        std::destroy_at(ptr);
    }
    template <typename U>
    bool operator==(const tracking_allocator<U> &other) const noexcept {
        return state == other.state;
    }
};

template <typename T>
struct input_iterator {
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T *;
    using reference = const T &;
    using iterator_category = std::input_iterator_tag;
    pointer ptr = nullptr;

    reference operator*() const { return *ptr; }
    input_iterator &operator++() {
        ++ptr;
        return *this;
    }
    input_iterator operator++(int) {
        auto old = *this;
        ++*this;
        return old;
    }
    bool operator==(const input_iterator &) const = default;
};

enum class constructor_kind { value, defaulted, fill, copy, copy_allocator, forward, input, list };

template <typename Ring>
Ring construct_ring(constructor_kind kind, const Ring &source,
                    const std::array<typename Ring::value_type, 7> &seed,
                    std::initializer_list<typename Ring::value_type> list,
                    const typename Ring::allocator_type &alloc) {
    const auto n = source.size();
    switch (kind) {
    case constructor_kind::value:
        return Ring(n, alloc);
    case constructor_kind::defaulted:
        return Ring(n, wjr::default_construct, alloc);
    case constructor_kind::fill:
        return Ring(n, seed.front(), alloc);
    case constructor_kind::copy:
        return Ring(source);
    case constructor_kind::copy_allocator:
        return Ring(source, alloc);
    case constructor_kind::forward:
        return Ring(seed.data(), seed.data() + n, alloc);
    case constructor_kind::input:
        return Ring(input_iterator<typename Ring::value_type>{seed.data()},
                    input_iterator<typename Ring::value_type>{seed.data() + n}, alloc);
    case constructor_kind::list:
        return Ring(n == 0 ? std::initializer_list<typename Ring::value_type>{} : list, alloc);
    }
    throw construction_error{};
}

template <typename Ring>
void check_construction(constructor_kind kind, std::size_t n, int throw_at, bool hook,
                        int allocation_throw_at = 0) {
    SCOPED_TRACE(::testing::Message()
                 << "kind=" << static_cast<int>(kind) << " n=" << n << " throw_at=" << throw_at
                 << " hook=" << hook << " allocation_throw_at=" << allocation_throw_at);
    construction_state state;
    {
        const typename Ring::allocator_type alloc;
        const std::array<typename Ring::value_type, 7> seed;
        const auto list = {seed[0], seed[1], seed[2], seed[3], seed[4], seed[5], seed[6]};
        Ring source(8, wjr::in_place_reserve, alloc);
        if (n != 0) {
            for (int i = 0; i < 6; ++i)
                source.emplace_back();
            for (int i = 0; i < 3; ++i)
                source.pop_front();
            for (int i = 0; i < 4; ++i)
                source.emplace_back();
        }
        const auto live = state.live;
        const int created = state.created, destroyed = state.destroyed;
        const int allocations = state.allocations, deallocations = state.deallocations;
        const int hook_created = state.hook_created, hook_destroyed = state.hook_destroyed;
        state.attempts = state.hook_attempts = state.allocation_attempts = 0;
        state.throw_at = hook ? 0 : throw_at;
        state.hook_throw_at = hook ? throw_at : 0;
        state.allocation_throw_at = allocation_throw_at;
        if (allocation_throw_at != 0) {
            EXPECT_THROW((void)construct_ring(kind, source, seed, list, alloc), std::bad_alloc);
        } else if (throw_at != 0) {
            EXPECT_THROW((void)construct_ring(kind, source, seed, list, alloc), construction_error);
        } else {
            const Ring result = construct_ring(kind, source, seed, list, alloc);
            EXPECT_EQ(result.size(), n);
            for (const auto &value : result)
                EXPECT_EQ(value.value, 17);
        }
        state.throw_at = state.hook_throw_at = state.allocation_throw_at = 0;
        EXPECT_EQ(state.live, live);
        EXPECT_EQ(state.invalid_destroys, 0);
        EXPECT_EQ(state.created - created, state.destroyed - destroyed);
        EXPECT_EQ(state.allocations - allocations, state.deallocations - deallocations);
        EXPECT_EQ(state.hook_created - hook_created, state.hook_destroyed - hook_destroyed);
        for (const auto &value : source) {
            ASSERT_TRUE(state.live.contains(std::addressof(value)));
            EXPECT_EQ(value.value, 17);
        }
    }
    EXPECT_TRUE(state.live.empty());
    EXPECT_EQ(state.created, state.destroyed);
    EXPECT_EQ(state.allocations, state.deallocations);
    EXPECT_EQ(state.hook_created, state.hook_destroyed);
}

template <typename Ring>
void check_matrix(bool hooks) {
    for (auto kind : {constructor_kind::value, constructor_kind::defaulted, constructor_kind::fill,
                      constructor_kind::copy, constructor_kind::copy_allocator,
                      constructor_kind::forward, constructor_kind::input, constructor_kind::list}) {
        check_construction<Ring>(kind, 0, 0, false);
        check_construction<Ring>(kind, 7, 0, false);
        for (int position : {1, 4, 5, 6, 7}) {
            check_construction<Ring>(kind, 7, position, false);
            if (hooks)
                check_construction<Ring>(kind, 7, position, true);
        }
        if (hooks)
            check_construction<Ring>(kind, 7, 0, false, 1);
    }
    if (hooks) {
        check_construction<Ring>(constructor_kind::input, 7, 0, false, 2);
        check_construction<Ring>(constructor_kind::input, 7, 0, false, 3);
    }
}

template <typename Ring>
void check_growth_failure(int position, bool hook) {
    SCOPED_TRACE(::testing::Message() << "position=" << position << " hook=" << hook);
    construction_state state;
    {
        Ring ring(8, wjr::in_place_reserve);
        for (int i = 0; i < 6; ++i)
            ring.emplace_back();
        for (int i = 0; i < 3; ++i)
            ring.pop_front();
        for (int i = 0; i < 5; ++i)
            ring.emplace_back();
        ASSERT_TRUE(ring.full());
        ASSERT_EQ(ring.array_one().size(), 5);
        ASSERT_EQ(ring.array_two().size(), 3);
        const auto live = state.live;
        const auto data = ring.data();
        const int created = state.created, destroyed = state.destroyed;
        const int allocations = state.allocations, deallocations = state.deallocations;
        state.move_attempts = state.hook_attempts = 0;
        state.move_throw_at = hook ? 0 : position;
        state.hook_throw_at = hook ? position : 0;
        EXPECT_THROW(ring.emplace_back(99), construction_error);
        state.move_throw_at = state.hook_throw_at = 0;
        EXPECT_EQ(ring.data(), data);
        EXPECT_EQ(ring.size(), 8);
        EXPECT_EQ(ring.capacity(), 8);
        EXPECT_EQ(state.live, live);
        EXPECT_EQ(state.invalid_destroys, 0);
        EXPECT_EQ(state.created - created, state.destroyed - destroyed);
        EXPECT_EQ(state.allocations - allocations, state.deallocations - deallocations);
    }
    EXPECT_TRUE(state.live.empty());
    EXPECT_EQ(state.created, state.destroyed);
    EXPECT_EQ(state.allocations, state.deallocations);
    EXPECT_EQ(state.hook_created, state.hook_destroyed);
}

} // namespace

TEST(ring_buffer_construction, standard_allocator_cleans_all_constructor_paths) {
    check_matrix<wjr::ring_buffer<tracked_value<false>>>(false);
    check_matrix<wjr::ring_buffer<tracked_value<true>>>(false);
}

TEST(ring_buffer_construction, allocator_hooks_and_allocation_failures_are_balanced) {
    using value = tracked_value<false>;
    check_matrix<wjr::ring_buffer<value, tracking_allocator<value>>>(true);
}

TEST(ring_buffer_construction, wrapped_growth_preserves_old_object_lifetimes_on_failure) {
    for (int position : {1, 5, 6, 8}) {
        check_growth_failure<wjr::ring_buffer<tracked_value<false>>>(position, false);
    }
    using value = tracked_value<true>;
    for (int position : {1, 2, 6, 7, 9}) {
        check_growth_failure<wjr::ring_buffer<value, tracking_allocator<value>>>(position, true);
    }
}
#endif
