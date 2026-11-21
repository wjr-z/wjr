#include <gtest/gtest.h>

#include <memory>
#include <type_traits>

#include <wjr/crtp/trivially_allocator_base.hpp>
#include <wjr/memory/temporary_value_allocator.hpp>

namespace {

struct no_hooks {};
struct only_construct {
    void construct(int *, int) &;
};
struct only_destroy {
    void destroy(int *) &;
};
struct rvalue_hooks {
    void construct(int *, int) &&;
    void destroy(int *) &&;
};
struct both_hooks : only_construct, only_destroy {};
struct false_marker : both_hooks {
    using is_trivially_allocator = std::false_type;
};
struct true_marker : both_hooks {
    using is_trivially_allocator = std::true_type;
};
struct nontrivial_value {
    nontrivial_value();
    ~nontrivial_value();
};

static_assert(wjr::is_trivially_allocator_construct_v<no_hooks, nontrivial_value>);
static_assert(wjr::is_trivially_allocator_destroy_v<no_hooks, nontrivial_value>);
static_assert(!wjr::is_trivially_allocator_construct_v<only_construct, int, int>);
static_assert(wjr::is_trivially_allocator_destroy_v<only_construct, int>);
static_assert(wjr::is_trivially_allocator_construct_v<only_destroy, int, int>);
static_assert(!wjr::is_trivially_allocator_destroy_v<only_destroy, int>);
static_assert(!wjr::is_trivially_allocator_construct_v<false_marker, int, int>);
static_assert(!wjr::is_trivially_allocator_destroy_v<false_marker, int>);
static_assert(wjr::is_trivially_allocator_construct_v<true_marker, int, int>);
static_assert(wjr::is_trivially_allocator_destroy_v<true_marker, int>);
static_assert(wjr::is_trivially_allocator_construct_v<rvalue_hooks, int, int>);
static_assert(wjr::is_trivially_allocator_destroy_v<rvalue_hooks, int>);
static_assert(wjr::is_trivially_allocator_construct_v<std::allocator<int>, int, int>);
static_assert(wjr::is_trivially_allocator_destroy_v<std::allocator<int>, int>);

struct incomplete_marker;
struct lazy_incomplete {
    using is_trivially_allocator = incomplete_marker;
};
struct lazy_void {
    using is_trivially_allocator = void;
};
static_assert(wjr::is_trivially_allocator_construct_v<lazy_incomplete, int, int>);
static_assert(wjr::is_trivially_allocator_destroy_v<lazy_incomplete, int>);
static_assert(wjr::is_trivially_allocator_construct_v<lazy_void, int, int>);
static_assert(wjr::is_trivially_allocator_destroy_v<lazy_void, int>);

struct numeric_tag {
    using value_type = int;
    using type = numeric_tag;
    static constexpr int value = 2;
};
struct numeric_marker : both_hooks {
    using is_trivially_allocator = numeric_tag;
};
static_assert(wjr::is_trivially_allocator_construct<numeric_marker, int, int>::value == 2);
static_assert(wjr::is_trivially_allocator_destroy<numeric_marker, int>::value == 2);
static_assert(
    std::is_same_v<typename wjr::is_trivially_allocator_construct<numeric_marker, int, int>::type,
                   numeric_tag>);
static_assert(std::is_same_v<
              typename wjr::is_trivially_allocator_destroy<numeric_marker, int>::value_type, int>);

struct counted_value {
    int *destroyed;
    counted_value(int *constructed, int *destroyed_value) : destroyed(destroyed_value) {
        ++*constructed;
    }
    ~counted_value() { ++*destroyed; }
};

template <bool Bypass>
struct hook_allocator {
    using value_type = counted_value;
    using is_trivially_allocator = std::bool_constant<Bypass>;
    int constructed = 0;
    int destroyed = 0;

    template <typename T, typename... Args>
    void construct(T *ptr, Args &&...args) {
        ++constructed;
        std::construct_at(ptr, std::forward<Args>(args)...);
    }
    template <typename T>
    void destroy(T *ptr) {
        ++destroyed;
        std::destroy_at(ptr);
    }
};

template <bool Bypass>
void verify_hooks() {
    std::allocator<counted_value> storage;
    auto *ptr = storage.allocate(1);
    hook_allocator<Bypass> allocator;
    int constructed = 0;
    int destroyed = 0;
    wjr::uninitialized_construct_using_allocator(ptr, allocator, &constructed, &destroyed);
    EXPECT_EQ(constructed, 1);
    EXPECT_EQ(allocator.constructed, Bypass ? 0 : 1);
    wjr::destroy_at_using_allocator(ptr, allocator);
    EXPECT_EQ(destroyed, 1);
    EXPECT_EQ(allocator.destroyed, Bypass ? 0 : 1);
    storage.deallocate(ptr, 1);
}

} // namespace

namespace {

struct copy_assign_only {
    copy_assign_only() = default;
    copy_assign_only(const copy_assign_only &) = delete;
    copy_assign_only &operator=(const copy_assign_only &) = default;
};

static_assert(wjr::get_relocate_mode_v<copy_assign_only> == wjr::relocate_t::trivial);
static_assert(!std::is_move_constructible_v<copy_assign_only>);
static_assert(noexcept(wjr::relocate_at(std::declval<copy_assign_only *>(),
                                        std::declval<copy_assign_only *>())));

struct hook_failure {};

struct throwing_relocation_allocator : std::allocator<int> {
    bool fail_construct = false;
    bool fail_destroy = false;

    void construct(int *ptr, int &&value) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        if (fail_construct) {
            throw hook_failure{};
        }
#endif
        std::construct_at(ptr, value);
    }

    void destroy(int *ptr) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        if (fail_destroy) {
            throw hook_failure{};
        }
#endif
        std::destroy_at(ptr);
    }
};

static_assert(noexcept(wjr::destroy_at_using_allocator(std::declval<int *>(),
                                                       std::declval<std::allocator<int> &>())));
static_assert(std::is_nothrow_destructible_v<wjr::temporary_value_allocator<std::allocator<int>>>);
static_assert(
    !std::is_nothrow_destructible_v<wjr::temporary_value_allocator<throwing_relocation_allocator>>);
static_assert(noexcept(wjr::uninitialized_relocate_at_using_allocator(
    std::declval<int *>(), std::declval<int *>(), std::declval<std::allocator<int> &>())));
static_assert(!noexcept(wjr::uninitialized_relocate_at_using_allocator(
    std::declval<int *>(), std::declval<int *>(),
    std::declval<throwing_relocation_allocator &>())));

} // namespace

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(allocator_lifecycle_traits, relocation_construct_failure_preserves_source) {
    throwing_relocation_allocator alloc;
    auto *source = alloc.allocate(1);
    auto *dest = alloc.allocate(1);
    std::construct_at(source, 37);

    alloc.fail_construct = true;
    EXPECT_THROW(wjr::uninitialized_relocate_at_using_allocator(source, dest, alloc), hook_failure);
    EXPECT_EQ(*source, 37);

    alloc.fail_construct = false;
    EXPECT_EQ(wjr::uninitialized_relocate_at_using_allocator(source, dest, alloc), dest);
    EXPECT_EQ(*dest, 37);
    std::destroy_at(dest);
    alloc.deallocate(dest, 1);
    alloc.deallocate(source, 1);
}

TEST(allocator_lifecycle_traits, relocation_destroy_failure_leaves_destination_live) {
    throwing_relocation_allocator alloc;
    auto *source = alloc.allocate(1);
    auto *dest = alloc.allocate(1);
    std::construct_at(source, 53);

    alloc.fail_destroy = true;
    EXPECT_THROW(wjr::uninitialized_relocate_at_using_allocator(source, dest, alloc), hook_failure);
    EXPECT_EQ(*dest, 53);
    std::destroy_at(dest);
    std::destroy_at(source);
    alloc.deallocate(dest, 1);
    alloc.deallocate(source, 1);
}
#endif

namespace allocator_lifecycle_customization {
struct detected_hooks {};
struct lifecycle_override {
    using is_trivially_allocator = void;
    void construct(int *, int);
    void destroy(int *);
};
} // namespace allocator_lifecycle_customization

namespace wjr {
template <>
struct has_is_non_trivially_allocator_construct<allocator_lifecycle_customization::detected_hooks,
                                                int, int> : std::true_type {};
template <>
struct has_is_non_trivially_allocator_destroy<allocator_lifecycle_customization::detected_hooks,
                                              int> : std::true_type {};
template <>
struct is_trivially_allocator_construct<allocator_lifecycle_customization::lifecycle_override, int,
                                        int> : std::true_type {};
template <>
struct is_trivially_allocator_destroy<allocator_lifecycle_customization::lifecycle_override, int>
    : std::true_type {};
} // namespace wjr

static_assert(!wjr::is_trivially_allocator_construct_v<
              allocator_lifecycle_customization::detected_hooks, int, int>);
static_assert(
    !wjr::is_trivially_allocator_destroy_v<allocator_lifecycle_customization::detected_hooks, int>);
static_assert(wjr::is_trivially_allocator_construct_v<
              allocator_lifecycle_customization::lifecycle_override, int, int>);
static_assert(wjr::is_trivially_allocator_destroy_v<
              allocator_lifecycle_customization::lifecycle_override, int>);

TEST(allocator_lifecycle_traits, ordinary_hooks_are_observed) { verify_hooks<false>(); }

TEST(allocator_lifecycle_traits, marker_bypasses_hooks_without_bypassing_object_lifetime) {
    verify_hooks<true>();
}
