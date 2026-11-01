#include <wjr/vector.hpp>

#include <cstddef>
#include <memory>
#include <set>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace {

struct lifetime_state {
    std::set<const void *> objects;
    std::set<int *> resources;
    int invalid_destroys = 0;
    int created = 0;
    int hook_destroyed = 0;

    /** @brief Release C++ test allocations retained by a failing regression. */
    ~lifetime_state() {
        for (auto *resource : resources) {
            // clang-format off
            // nosemgrep: apex.lang.performance.ncino.operationsinloops.avoidnativedmlinloops.avoid-native-dml-in-loops
            delete resource;
            // clang-format on
        }
    }
};

struct resource_deleter {
    lifetime_state *state;
    void operator()(int *ptr) const noexcept {
        state->resources.erase(ptr);
        delete ptr;
    }
};

/** @brief A valid move leaves an independently owned resource in the source. */
template <bool Assignable>
struct retained_resource {
    using owner = std::unique_ptr<int, resource_deleter>;
    lifetime_state *state;
    owner payload;
    owner auxiliary;

    retained_resource(lifetime_state &s, int value)
        : state(&s), payload(make_resource(s, value)), auxiliary(make_resource(s, -1)) {
        EXPECT_TRUE(state->objects.insert(this).second);
        ++state->created;
    }
    retained_resource(retained_resource &&other) noexcept
        : state(other.state), payload(std::move(other.payload)),
          auxiliary(nullptr, resource_deleter{state}) {
        EXPECT_TRUE(state->objects.insert(this).second);
        ++state->created;
    }
    retained_resource &operator=(retained_resource &&other) noexcept
    requires Assignable
    {
        payload = std::move(other.payload);
        return *this;
    }
    ~retained_resource() {
        if (state->objects.erase(this) != 1) {
            ++state->invalid_destroys;
        }
    }

private:
    static owner make_resource(lifetime_state &state, int value) {
        auto *ptr = new int(value);
        state.resources.insert(ptr);
        return owner(ptr, resource_deleter{&state});
    }
};

template <typename T>
struct destroy_allocator {
    using value_type = T;
    lifetime_state *state;
    explicit destroy_allocator(lifetime_state &s) noexcept : state(&s) {}
    template <typename U>
    destroy_allocator(const destroy_allocator<U> &other) noexcept : state(other.state) {}
    T *allocate(std::size_t n) { return std::allocator<T>{}.allocate(n); }
    void deallocate(T *ptr, std::size_t n) noexcept { std::allocator<T>{}.deallocate(ptr, n); }
    void destroy(T *ptr) noexcept {
        ++state->hook_destroyed;
        std::destroy_at(ptr);
    }
    friend bool operator==(const destroy_allocator &, const destroy_allocator &) = default;
};

template <typename Vector>
void fill(Vector &vec, lifetime_state &state, int count, int offset) {
    for (int i = 0; i != count; ++i) {
        vec.emplace_back(state, offset + i);
    }
}

template <typename Vector>
void expect_values(const Vector &vec, int count, int offset) {
    ASSERT_EQ(vec.size(), static_cast<std::size_t>(count));
    for (int i = 0; i != count; ++i) {
        ASSERT_NE(vec[i].payload, nullptr);
        EXPECT_EQ(*vec[i].payload, offset + i);
    }
}

template <typename Vector>
void verify_swap(int left_size, int right_size) {
    lifetime_state state;
    {
        Vector left, right;
        fill(left, state, left_size, 10);
        fill(right, state, right_size, 20);
        left.swap(right);
        expect_values(left, right_size, 20);
        expect_values(right, left_size, 10);
        EXPECT_EQ(state.objects.size(), left.size() + right.size());
        left.swap(left);
        right.swap(right);
        expect_values(left, right_size, 20);
        expect_values(right, left_size, 10);
    }
    EXPECT_TRUE(state.objects.empty());
    EXPECT_TRUE(state.resources.empty());
    EXPECT_EQ(state.invalid_destroys, 0);
}

template <typename Vector>
void verify_move(int count) {
    lifetime_state state;
    {
        Vector source;
        fill(source, state, count, 30);
        Vector destination(std::move(source));
        EXPECT_TRUE(source.empty());
        expect_values(destination, count, 30);
        EXPECT_EQ(state.objects.size(), destination.size());

        Vector assigned;
        fill(assigned, state, 2, 40);
        assigned = std::move(destination);
        EXPECT_TRUE(destination.empty());
        expect_values(assigned, count, 30);
        EXPECT_EQ(state.objects.size(), assigned.size());
    }
    EXPECT_TRUE(state.objects.empty());
    EXPECT_TRUE(state.resources.empty());
    EXPECT_EQ(state.invalid_destroys, 0);
}

static_assert(!std::is_move_assignable_v<retained_resource<false>>);
static_assert(std::is_nothrow_move_assignable_v<retained_resource<true>>);
using value = retained_resource<false>;
using small_storage = wjr::small_vector_storage<value, 4, std::allocator<value>>;
using inplace_storage = wjr::inplace_vector_storage<value, 4>;
static_assert(noexcept(std::declval<small_storage &>().swap_storage(
    std::declval<small_storage &>(), std::declval<std::allocator<value> &>())));
static_assert(noexcept(std::declval<inplace_storage &>().swap_storage(
    std::declval<inplace_storage &>(), std::declval<std::allocator<value> &>())));

} // namespace

TEST(vector_inline_lifetime, small_swap_all_storage_combinations) {
    for (const int left : {0, 1, 3, 4, 6, 8}) {
        for (const int right : {0, 1, 3, 4, 6, 8}) {
            SCOPED_TRACE(testing::Message() << left << "," << right);
            verify_swap<wjr::small_vector<retained_resource<true>, 4>>(left, right);
            verify_swap<wjr::small_vector<retained_resource<false>, 4>>(left, right);
        }
    }
}

TEST(vector_inline_lifetime, inplace_swap_all_sizes) {
    for (int left = 0; left <= 4; ++left) {
        for (int right = 0; right <= 4; ++right) {
            SCOPED_TRACE(testing::Message() << left << "," << right);
            verify_swap<wjr::inplace_vector<retained_resource<true>, 4>>(left, right);
            verify_swap<wjr::inplace_vector<retained_resource<false>, 4>>(left, right);
        }
    }
}

TEST(vector_inline_lifetime, move_constructor_and_assignment_end_inline_source_lifetimes) {
    for (int count = 0; count <= 4; ++count) {
        verify_move<wjr::small_vector<retained_resource<true>, 4>>(count);
        verify_move<wjr::small_vector<retained_resource<false>, 4>>(count);
        verify_move<wjr::inplace_vector<retained_resource<true>, 4>>(count);
        verify_move<wjr::inplace_vector<retained_resource<false>, 4>>(count);
    }
    verify_move<wjr::small_vector<retained_resource<false>, 4>>(8);
}

TEST(vector_inline_lifetime, custom_destroy_observes_inline_transfers) {
    lifetime_state state;
    {
        using value_type = retained_resource<false>;
        using allocator = destroy_allocator<value_type>;
        using vector = wjr::small_vector<value_type, 4, allocator>;
        vector left{allocator(state)}, right{allocator(state)};
        fill(left, state, 2, 10);
        fill(right, state, 4, 20);
        left.swap(right);
        expect_values(left, 4, 20);
        expect_values(right, 2, 10);
        vector destination(std::move(left));
        expect_values(destination, 4, 20);
        right.reserve(8);
        right.swap(destination);
        expect_values(right, 4, 20);
        expect_values(destination, 2, 10);
    }
    EXPECT_EQ(state.hook_destroyed, state.created);
    EXPECT_TRUE(state.objects.empty());
    EXPECT_TRUE(state.resources.empty());
    EXPECT_EQ(state.invalid_destroys, 0);
}

TEST(vector_inline_lifetime, trivial_inline_move_preserves_typed_destroy_hook) {
    lifetime_state state;
    {
        using allocator = destroy_allocator<int>;
        wjr::small_vector<int, 4, allocator> source{allocator(state)};
        source.emplace_back(10);
        source.emplace_back(20);
        source.emplace_back(30);
        auto destination = std::move(source);
        EXPECT_TRUE(source.empty());
        ASSERT_EQ(destination.size(), 3);
        EXPECT_EQ(destination[0], 10);
        EXPECT_EQ(destination[1], 20);
        EXPECT_EQ(destination[2], 30);
        EXPECT_EQ(state.hook_destroyed, 3);
    }
    EXPECT_EQ(state.hook_destroyed, 6);
}
