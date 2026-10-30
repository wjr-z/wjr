#include "detail.hpp"

#include <wjr/memory/stack_allocator.hpp>
#include <wjr/memory/temporary_value_allocator.hpp>

namespace {

struct tracked {
    static inline int constructed = 0;
    static inline int destroyed = 0;
    int value;

    tracked() noexcept : tracked(7) {}
    explicit tracked(int n) noexcept : value(n) { ++constructed; }
    tracked(const tracked &other) noexcept : tracked(other.value) {}
    tracked(tracked &&other) noexcept : tracked(other.value) { other.value = -1; }
    tracked &operator=(const tracked &) = default;
    tracked &operator=(tracked &&) = default;
    ~tracked() { ++destroyed; }
};

struct no_default {
    no_default() = delete;
    explicit no_default(int) noexcept {}
};

struct move_only {
    move_only() = default;
    move_only(const move_only &) = delete;
    move_only(move_only &&) noexcept = default;
    move_only &operator=(const move_only &) = delete;
    move_only &operator=(move_only &&) noexcept = default;
};

struct alignas(64) aligned {
    int value;
};
struct throwing_destructor {
    throwing_destructor(int) noexcept {}
    ~throwing_destructor() noexcept(false) {}
};

template <typename T, typename... Args>
concept can_emplace =
    requires(T &storage, Args &&...args) { storage.emplace(std::forward<Args>(args)...); };

template <bool Constructor, bool Destructor>
constexpr bool union_traits() {
    using storage = wjr::_uninitialized_base<int, Constructor, Destructor>;
    return std::is_trivially_default_constructible_v<storage> == (Constructor && Destructor) &&
           std::is_trivially_destructible_v<storage> == Destructor &&
           std::is_trivially_copyable_v<storage> == Destructor &&
           std::is_nothrow_default_constructible_v<storage> && sizeof(storage) == sizeof(int) &&
           alignof(storage) == alignof(int);
}

static_assert(union_traits<false, false>());
static_assert(union_traits<false, true>());
static_assert(union_traits<true, false>());
static_assert(union_traits<true, true>());
static_assert(std::is_trivially_copyable_v<wjr::lazy_initialized<int>>);
static_assert(std::is_trivially_default_constructible_v<wjr::uninitialized<int>>);
static_assert(!std::is_copy_constructible_v<wjr::uninitialized<int>>);
static_assert(std::is_copy_constructible_v<wjr::lazy_initialized<tracked>>);
static_assert(std::is_move_constructible_v<wjr::lazy_initialized<tracked>>);
static_assert(!std::is_copy_constructible_v<wjr::lazy_initialized<move_only>>);
static_assert(std::is_move_constructible_v<wjr::lazy_initialized<move_only>>);
static_assert(!std::is_copy_assignable_v<wjr::lazy_initialized<move_only>>);
static_assert(std::is_move_assignable_v<wjr::lazy_initialized<move_only>>);
static_assert(std::is_default_constructible_v<wjr::uninitialized<no_default>>);
static_assert(std::is_constructible_v<wjr::uninitialized<no_default>, int>);
static_assert(!std::is_constructible_v<wjr::uninitialized<no_default>, const char *>);
static_assert(can_emplace<wjr::uninitialized<no_default>, int>);
static_assert(!can_emplace<wjr::uninitialized<no_default>, const char *>);
static_assert(std::is_constructible_v<wjr::uninitialized<throwing_destructor>, int>);
static_assert(!std::constructible_from<throwing_destructor, int>);
static_assert(sizeof(wjr::uninitialized<aligned>) == sizeof(aligned));
static_assert(alignof(wjr::uninitialized<aligned>) == alignof(aligned));
static_assert(
    std::same_as<decltype(*std::declval<const wjr::uninitialized<int> &&>()), const int &&>);

TEST(lifetime_storage, explicit_lifetime) {
    tracked::constructed = tracked::destroyed = 0;
    {
        wjr::uninitialized<tracked> storage;
        EXPECT_EQ(tracked::constructed, 0);
        EXPECT_EQ(storage.emplace(42).value, 42);
        EXPECT_EQ(tracked::constructed, 1);
        storage.reset();
        EXPECT_EQ(tracked::destroyed, 1);
        storage.emplace(8);
        EXPECT_EQ(storage->value, 8);
        storage.reset();
    }
    EXPECT_EQ(tracked::constructed, 2);
    EXPECT_EQ(tracked::destroyed, 2);
}

TEST(lifetime_storage, lazy_copy_move_and_destruction) {
    tracked::constructed = tracked::destroyed = 0;
    {
        wjr::lazy_initialized<tracked> first(42);
        wjr::lazy_initialized<tracked> second(first);
        wjr::lazy_initialized<tracked> third(std::move(first));
        EXPECT_EQ(second->value, 42);
        EXPECT_EQ(third->value, 42);
        EXPECT_EQ(first->value, 42);
        EXPECT_EQ(tracked::constructed, 3);
        EXPECT_EQ(tracked::destroyed, 0);
    }
    EXPECT_EQ(tracked::destroyed, 3);
}

struct hooked_allocator : std::allocator<tracked> {
    int constructs = 0;
    int destroys = 0;
    template <typename... Args>
    void construct(tracked *ptr, Args &&...args) {
        ++constructs;
        std::construct_at(ptr, std::forward<Args>(args)...);
    }
    void destroy(tracked *ptr) {
        ++destroys;
        std::destroy_at(ptr);
    }
};

TEST(lifetime_storage, temporary_value_uses_allocator_hooks) {
    hooked_allocator allocator;
    {
        wjr::temporary_value_allocator value(allocator, 19);
        EXPECT_EQ(value.get()->value, 19);
        EXPECT_EQ(allocator.constructs, 1);
        EXPECT_EQ(allocator.destroys, 0);
    }
    EXPECT_EQ(allocator.destroys, 1);
}

struct tagged_default {
    int value;
    tagged_default() : value(1) {}
    tagged_default(wjr::default_construct_t) : value(2) {}
};

TEST(lifetime_storage, default_construct_tag_dispatch) {
    wjr::uninitialized<tagged_default> storage;
    wjr::construct_at(storage.get(), wjr::default_construct);
    EXPECT_EQ(storage->value, 2);
    storage.reset();
    wjr::uninitialized<tracked> ordinary;
    wjr::construct_at(ordinary.get(), wjr::default_construct);
    EXPECT_EQ(ordinary->value, 7);
    ordinary.reset();
}

struct cold_owner {
    using allocator_type = std::allocator<int>;
};
struct warm_owner {
    using allocator_type = wjr::weak_stack_allocator<int>;
    allocator_type allocator;
    allocator_type get_allocator() const { return allocator; }
};
static_assert(wjr::lifetime_detail::allocator_owner<cold_owner>);
static_assert(!wjr::lifetime_detail::weak_stack_owner<cold_owner>);
static_assert(wjr::lifetime_detail::weak_stack_owner<warm_owner>);
static_assert(!wjr::lifetime_detail::allocator_owner<int>);
static_assert(!wjr::lifetime_detail::weak_stack_owner<int>);
static_assert(std::same_as<wjr::unique_stack_allocator_selector<cold_owner>,
                           wjr::unique_stack_allocator_wrapper>);
static_assert(std::same_as<wjr::unique_stack_allocator_selector<cold_owner, warm_owner>,
                           wjr::unique_stack_allocator_ref>);

TEST(lifetime_storage, first_borrowed_stack_allocator_keeps_identity) {
    wjr::unique_stack_allocator first;
    wjr::unique_stack_allocator second;
    cold_owner cold;
    warm_owner warm_first{first};
    warm_owner warm_second{second};
    wjr::unique_stack_allocator_ref borrowed(&cold, &warm_first, &warm_second);
    EXPECT_EQ(borrowed.operator->(), &first);
    EXPECT_EQ(&*borrowed, &first);
}

} // namespace
