#include <memory>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/memory/temporary_value_allocator.hpp>

namespace {

struct tracked_value {
    static inline int live = 0;
    int value;

    explicit tracked_value(int number = 0) noexcept : value(number) { ++live; }
    ~tracked_value() noexcept { --live; }
};

struct hook_allocator : std::allocator<tracked_value> {
    int attempts = 0;
    int destroys = 0;
    bool fail = true;

    template <typename... Args>
    void construct(tracked_value *ptr, Args &&...args) {
        ++attempts;
        if (fail) {
            throw std::runtime_error("allocator construction failed");
        }
        std::construct_at(ptr, std::forward<Args>(args)...);
    }

    void destroy(tracked_value *ptr) noexcept {
        ++destroys;
        std::destroy_at(ptr);
    }
};

struct nothrow_hook_allocator : std::allocator<int> {
    void construct(int *ptr, int value) noexcept { std::construct_at(ptr, value); }
};

using int_allocator = std::allocator<int>;
using int_temporary = wjr::temporary_value_allocator<int_allocator>;
using hooked_temporary = wjr::temporary_value_allocator<hook_allocator>;
using nothrow_temporary = wjr::temporary_value_allocator<nothrow_hook_allocator>;

template <typename Alloc, typename... Args>
constexpr bool nothrow_construction =
    noexcept(::new (std::declval<void *>()) wjr::temporary_value_allocator<Alloc>(
        std::declval<Alloc &>(), std::declval<Args>()...));

static_assert(nothrow_construction<int_allocator, int>);
static_assert(nothrow_construction<int_allocator, wjr::default_construct_t>);
static_assert(!nothrow_construction<hook_allocator, int>);
static_assert(!nothrow_construction<hook_allocator, wjr::default_construct_t>);
static_assert(nothrow_construction<nothrow_hook_allocator, int> ==
              noexcept(std::allocator_traits<nothrow_hook_allocator>::construct(
                  std::declval<nothrow_hook_allocator &>(), std::declval<int *>(), 7)));

TEST(temporary_value_allocator, throwing_hook_preserves_lifetimes) {
    hook_allocator allocator;
    EXPECT_THROW((hooked_temporary(allocator, 19)), std::runtime_error);
    EXPECT_EQ(allocator.attempts, 1);
    EXPECT_EQ(allocator.destroys, 0);
    EXPECT_EQ(tracked_value::live, 0);

    allocator.fail = false;
    {
        hooked_temporary value(allocator, 23);
        EXPECT_EQ(value.get()->value, 23);
        EXPECT_EQ(tracked_value::live, 1);
    }
    EXPECT_EQ(allocator.attempts, 2);
    EXPECT_EQ(allocator.destroys, 1);
    EXPECT_EQ(tracked_value::live, 0);
}

TEST(temporary_value_allocator, default_construction_uses_hook_exception_contract) {
    hook_allocator allocator;
    EXPECT_THROW((hooked_temporary(allocator, wjr::default_construct)), std::runtime_error);
    EXPECT_EQ(allocator.attempts, 1);
    EXPECT_EQ(allocator.destroys, 0);
    EXPECT_EQ(tracked_value::live, 0);

    allocator.fail = false;
    {
        hooked_temporary value(allocator, wjr::default_construct);
        EXPECT_EQ(value.get()->value, 0);
    }
    EXPECT_EQ(allocator.attempts, 2);
    EXPECT_EQ(allocator.destroys, 1);
    EXPECT_EQ(tracked_value::live, 0);
}

TEST(temporary_value_allocator, direct_construction_and_nothrow_hook) {
    int_allocator allocator;
    int_temporary value(allocator, 31);
    EXPECT_EQ(*value.get(), 31);
    nothrow_hook_allocator hooked_allocator;
    nothrow_temporary hooked(hooked_allocator, 37);
    EXPECT_EQ(*hooked.get(), 37);
}

} // namespace
