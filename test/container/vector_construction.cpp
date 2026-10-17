#include <wjr/vector.hpp>

#include <cstddef>
#include <iterator>
#include <memory>
#include <set>
#include <span>
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
    int attempts = 0, throw_at = 0, created = 0, destroyed = 0, invalid_destroys = 0;
    int allocations = 0, deallocations = 0;
    int hook_attempts = 0, hook_throw_at = 0, hook_created = 0, hook_destroyed = 0;
    bool fail_allocation = false;

private:
    construction_state *m_previous;
};

struct tracked_value {
    int value = 17;

    tracked_value() { construct(); }
    tracked_value(const tracked_value &other) : value(other.value) { construct(); }
    tracked_value(tracked_value &&other) noexcept : value(other.value) { register_live(); }
    ~tracked_value() {
        if (active_state->live.erase(this) == 1) {
            ++active_state->destroyed;
        } else {
            ++active_state->invalid_destroys;
        }
    }

private:
    void construct() {
        auto &state = *active_state;
        if (++state.attempts == state.throw_at) {
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
        if (state->fail_allocation) {
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

struct noncommon_range {
    struct sentinel {
        const tracked_value *ptr = nullptr;
    };
    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = tracked_value;
        using difference_type = std::ptrdiff_t;
        using pointer = const tracked_value *;
        using reference = const tracked_value &;
        pointer ptr = nullptr;

        reference operator*() const { return *ptr; }
        iterator &operator++() {
            ++ptr;
            return *this;
        }
        iterator operator++(int) {
            auto old = *this;
            ++*this;
            return old;
        }
        bool operator==(const iterator &) const = default;
        friend bool operator==(iterator lhs, sentinel rhs) { return lhs.ptr == rhs.ptr; }
    };
    const tracked_value *first, *last;
    iterator begin() const { return {first}; }
    sentinel end() const { return {last}; }
    std::size_t size() const { return static_cast<std::size_t>(last - first); }
};

enum class constructor_kind {
    value,
    defaulted,
    fill,
    copy,
    copy_allocator,
    range,
    from_range,
    sentinel
};

template <typename Vec>
Vec construct_vector(constructor_kind kind, const Vec &source,
                     const typename Vec::allocator_type &alloc) {
    const auto n = source.size();
    switch (kind) {
    case constructor_kind::value:
        return Vec(n, alloc);
    case constructor_kind::defaulted:
        return Vec(n, wjr::default_construct, alloc);
    case constructor_kind::fill:
        return Vec(n, source.front(), alloc);
    case constructor_kind::copy:
        return Vec(source);
    case constructor_kind::copy_allocator:
        return Vec(source, alloc);
    case constructor_kind::range:
        return Vec(source.begin(), source.end(), alloc);
    case constructor_kind::from_range:
        return Vec(wjr::from_range, std::span<const tracked_value>(source.data(), n), alloc);
    case constructor_kind::sentinel:
        return Vec(wjr::from_range, noncommon_range{source.data(), source.data() + n}, alloc);
    }
    throw construction_error{};
}

template <typename Vec>
void check_construction(constructor_kind kind, std::size_t n, int throw_at, bool hook,
                        bool allocation_failure = false) {
    SCOPED_TRACE(::testing::Message() << "constructor=" << static_cast<int>(kind) << " n=" << n
                                      << " throw_at=" << throw_at << " hook=" << hook
                                      << " allocation_failure=" << allocation_failure);
    construction_state state;
    {
        const typename Vec::allocator_type alloc;
        const Vec source(n, alloc);
        const int created = state.created, destroyed = state.destroyed;
        const int allocations = state.allocations, deallocations = state.deallocations;
        const int hook_created = state.hook_created, hook_destroyed = state.hook_destroyed;
        state.attempts = state.hook_attempts = 0;
        state.throw_at = hook ? 0 : throw_at;
        state.hook_throw_at = hook ? throw_at : 0;
        state.fail_allocation = allocation_failure;
        if (allocation_failure) {
            EXPECT_THROW((void)construct_vector(kind, source, alloc), std::bad_alloc);
        } else if (throw_at != 0) {
            EXPECT_THROW((void)construct_vector(kind, source, alloc), construction_error);
        } else {
            const Vec result = construct_vector(kind, source, alloc);
            EXPECT_EQ(result.size(), n);
            for (const auto &value : result) {
                EXPECT_EQ(value.value, 17);
            }
        }
        state.throw_at = state.hook_throw_at = 0;
        state.fail_allocation = false;
        EXPECT_EQ(state.invalid_destroys, 0);
        EXPECT_EQ(state.live.size(), n);
        EXPECT_EQ(state.created - created, state.destroyed - destroyed);
        EXPECT_EQ(state.allocations - allocations, state.deallocations - deallocations);
        EXPECT_EQ(state.hook_created - hook_created, state.hook_destroyed - hook_destroyed);
        for (std::size_t i = 0; i < n; ++i) {
            ASSERT_TRUE(state.live.contains(source.data() + i));
            EXPECT_EQ(source[i].value, 17);
        }
    }
    EXPECT_TRUE(state.live.empty());
    EXPECT_EQ(state.created, state.destroyed);
    EXPECT_EQ(state.allocations, state.deallocations);
    EXPECT_EQ(state.hook_created, state.hook_destroyed);
}

template <typename Vec>
void check_constructor_matrix(std::size_t n, bool custom_allocator, bool heap) {
    for (const auto kind :
         {constructor_kind::value, constructor_kind::defaulted, constructor_kind::fill,
          constructor_kind::copy, constructor_kind::copy_allocator, constructor_kind::range,
          constructor_kind::from_range, constructor_kind::sentinel}) {
        check_construction<Vec>(kind, n, 0, false);
        for (const int position : {1, static_cast<int>((n + 1) / 2), static_cast<int>(n)}) {
            check_construction<Vec>(kind, n, position, false);
            if (custom_allocator) {
                check_construction<Vec>(kind, n, position, true);
            }
        }
        if (custom_allocator && heap) {
            check_construction<Vec>(kind, n, 0, false, true);
        }
    }
}

} // namespace

TEST(vector_construction, standard_allocator_rolls_back_elements) {
    check_constructor_matrix<wjr::vector<tracked_value>>(7, false, true);
    check_constructor_matrix<wjr::small_vector<tracked_value, 4>>(3, false, false);
    check_constructor_matrix<wjr::small_vector<tracked_value, 4>>(7, false, true);
    check_constructor_matrix<wjr::inplace_vector<tracked_value, 8>>(7, false, false);
}

TEST(vector_construction, stateful_allocator_rolls_back_elements_and_hooks) {
    using allocator = tracking_allocator<tracked_value>;
    check_constructor_matrix<wjr::vector<tracked_value, allocator>>(7, true, true);
    check_constructor_matrix<wjr::small_vector<tracked_value, 4, allocator>>(3, true, false);
    check_constructor_matrix<wjr::small_vector<tracked_value, 4, allocator>>(7, true, true);
}
#endif
