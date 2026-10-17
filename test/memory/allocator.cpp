#include <wjr/memory/allocate_at_least.hpp>
#include <wjr/memory/uninitialized.hpp>

#include <gtest/gtest.h>

namespace {

template <typename T>
class fancy_pointer {
public:
    using element_type = T;
    using difference_type = std::ptrdiff_t;

    fancy_pointer() = default;
    fancy_pointer(std::nullptr_t) noexcept {}
    explicit fancy_pointer(T *ptr) noexcept : m_ptr(ptr) {}

    template <typename U, std::enable_if_t<std::is_convertible_v<U *, T *>, int> = 0>
    fancy_pointer(fancy_pointer<U> ptr) noexcept : m_ptr(ptr.operator->()) {}

    T *operator->() const noexcept { return m_ptr; }

    template <typename U = T, std::enable_if_t<!std::is_void_v<U>, int> = 0>
    U &operator*() const noexcept {
        return *m_ptr;
    }

    explicit operator bool() const noexcept { return m_ptr != nullptr; }

    friend bool operator==(fancy_pointer lhs, fancy_pointer rhs) noexcept {
        return lhs.m_ptr == rhs.m_ptr;
    }

    friend bool operator!=(fancy_pointer lhs, fancy_pointer rhs) noexcept { return !(lhs == rhs); }

private:
    T *m_ptr = nullptr;
};

struct allocation_state {
    std::size_t allocated = 0;
    std::size_t deallocated = 0;
    unsigned allocations = 0;
    unsigned constructions = 0;
    unsigned destructions = 0;
    void *ptr = nullptr;
};

template <typename T>
struct tracking_allocator {
    using value_type = T;

    allocation_state *state;

    T *allocate(std::size_t count) {
        auto *ptr = std::allocator<T>{}.allocate(count);
        state->allocated = count;
        ++state->allocations;
        state->ptr = ptr;
        return ptr;
    }

    void deallocate(T *ptr, std::size_t count) noexcept {
        EXPECT_EQ(ptr, state->ptr);
        EXPECT_EQ(count, state->allocated);
        state->deallocated = count;
        std::allocator<T>{}.deallocate(ptr, count);
    }
};

template <typename T>
struct fancy_allocator : tracking_allocator<T> {
    using pointer = fancy_pointer<T>;

    pointer allocate(std::size_t count) { return pointer(tracking_allocator<T>::allocate(count)); }

    void deallocate(pointer ptr, std::size_t count) noexcept {
        tracking_allocator<T>::deallocate(ptr.operator->(), count);
    }
};

template <typename T>
struct rounding_allocator : tracking_allocator<T> {
    struct result_type {
        T *ptr;
        std::size_t count;
        unsigned marker;
    };

    result_type allocate_at_least(std::size_t count) & {
        const auto capacity = (count + 7) / 8 * 8;
        return {this->allocate(capacity), capacity, 42};
    }
};

template <typename T>
struct rvalue_allocator : tracking_allocator<T> {
    wjr::allocation_result<T *> allocate_at_least(std::size_t) &&;
};

struct lvalue_count_allocator : tracking_allocator<int> {
    wjr::allocation_result<int *> allocate_at_least(std::size_t &count) & {
        return {allocate(count), count};
    }
};

template <typename T>
struct hooked_allocator : tracking_allocator<T> {
    template <typename Obj, typename... Args>
    void construct(Obj *ptr, Args &&...args) & {
        ++this->state->constructions;
        ::new (static_cast<void *>(ptr)) Obj(std::forward<Args>(args)...);
    }

    template <typename Obj>
    void destroy(Obj *ptr) & noexcept {
        ++this->state->destructions;
        ptr->~Obj();
    }
};

struct rvalue_hooks {
    void construct(int *, int) &&;
    void destroy(int *) &&;
};

struct explicit_trivial_hooks : hooked_allocator<int> {
    using is_trivially_allocator = std::true_type;
};

static_assert(wjr::has_allocate_at_least_v<rounding_allocator<int>, std::size_t>);
static_assert(!wjr::has_allocate_at_least_v<rvalue_allocator<int>, std::size_t>);
static_assert(wjr::has_allocate_at_least_v<lvalue_count_allocator, std::size_t>);
static_assert(!wjr::is_trivially_allocator_construct_v<hooked_allocator<int>, int, int>);
static_assert(!wjr::is_trivially_allocator_destroy_v<hooked_allocator<int>, int>);
static_assert(wjr::is_trivially_allocator_construct_v<rvalue_hooks, int, int>);
static_assert(wjr::is_trivially_allocator_destroy_v<rvalue_hooks, int>);
static_assert(wjr::is_trivially_allocator_construct_v<explicit_trivial_hooks, int, int>);
static_assert(wjr::is_trivially_allocator_destroy_v<explicit_trivial_hooks, int>);
static_assert(wjr::is_trivially_allocator_v<std::allocator<int>>);

} // namespace

TEST(allocate_at_least, fancy_pointer_fallback_preserves_result_types) {
    allocation_state state;
    fancy_allocator<int> alloc{{&state}};
    const auto result = wjr::allocate_at_least(alloc, 3u);
    static_assert(std::is_same_v<std::remove_cv_t<decltype(result)>,
                                 wjr::allocation_result<fancy_pointer<int>, unsigned>>);
    EXPECT_EQ(result.ptr.operator->(), state.ptr);
    EXPECT_EQ(result.count, 3u);
    EXPECT_EQ(state.allocations, 1u);
    wjr::uninitialized_construct_using_allocator(result.ptr.operator->(), alloc, 7);
    EXPECT_EQ(*result.ptr, 7);
    wjr::destroy_at_using_allocator(result.ptr.operator->(), alloc);
    std::allocator_traits<decltype(alloc)>::deallocate(alloc, result.ptr, result.count);
    EXPECT_EQ(state.deallocated, 3u);
}

TEST(allocate_at_least, lvalue_member_preserves_extra_capacity_and_state) {
    allocation_state first_state;
    allocation_state second_state;
    rounding_allocator<int> first{{&first_state}};
    rounding_allocator<int> second{{&second_state}};
    const auto result = wjr::allocate_at_least(first, std::size_t{9});
    static_assert(
        std::is_same_v<std::remove_cv_t<decltype(result)>, rounding_allocator<int>::result_type>);
    EXPECT_EQ(result.count, 16u);
    EXPECT_EQ(result.marker, 42u);
    EXPECT_EQ(first_state.allocations, 1u);
    EXPECT_EQ(second_state.allocations, 0u);
    std::uninitialized_fill_n(result.ptr, result.count, 7);
    EXPECT_EQ(result.ptr[result.count - 1], 7);
    std::destroy_n(result.ptr, result.count);
    std::allocator_traits<decltype(first)>::deallocate(first, result.ptr, result.count);
    EXPECT_EQ(first_state.deallocated, 16u);

    const auto other = wjr::allocate_at_least(second, std::size_t{1});
    EXPECT_EQ(other.count, 8u);
    std::allocator_traits<decltype(second)>::deallocate(second, other.ptr, other.count);
    EXPECT_EQ(second_state.deallocated, 8u);
}

TEST(allocate_at_least, zero_count_is_forwarded_to_allocator) {
    allocation_state state;
    tracking_allocator<int> alloc{&state};
    const auto result = wjr::allocate_at_least(alloc, std::size_t{0});
    EXPECT_EQ(result.count, 0u);
    EXPECT_EQ(state.allocations, 1u);
    std::allocator_traits<decltype(alloc)>::deallocate(alloc, result.ptr, result.count);
}

TEST(allocate_at_least, detection_matches_lvalue_allocator_and_count) {
    allocation_state state;
    rvalue_allocator<int> rvalue_only{{&state}};
    const auto fallback = wjr::allocate_at_least(rvalue_only, std::size_t{3});
    EXPECT_EQ(fallback.count, 3u);
    rvalue_only.deallocate(fallback.ptr, fallback.count);

    lvalue_count_allocator lvalue_only{{&state}};
    const auto member = wjr::allocate_at_least(lvalue_only, std::size_t{7});
    EXPECT_EQ(member.count, 7u);
    lvalue_only.deallocate(member.ptr, member.count);
}

TEST(allocator_hooks, lvalue_hooks_receive_forwarded_arguments_and_same_state) {
    allocation_state state;
    hooked_allocator<std::unique_ptr<int>> alloc{{&state}};
    auto *ptr = alloc.allocate(1);
    auto value = std::make_unique<int>(42);
    wjr::uninitialized_construct_using_allocator(ptr, alloc, std::move(value));
    EXPECT_EQ(value, nullptr);
    EXPECT_EQ(**ptr, 42);
    EXPECT_EQ(state.constructions, 1u);
    wjr::destroy_at_using_allocator(ptr, alloc);
    EXPECT_EQ(state.destructions, 1u);
    alloc.deallocate(ptr, 1);
}
