#include <array>
#include <iterator>
#include <memory>

#include <gtest/gtest.h>

#include <wjr/memory/uninitialized.hpp>

#if !defined(WJR_DISABLE_EXCEPTIONS)
namespace {

struct construction_error {};

struct counted_value {
    inline static int live = 0;
    inline static int attempts = 0;
    inline static int throw_at = 0;

    counted_value() { create(); }
    counted_value(const counted_value &) { create(); }
    ~counted_value() { --live; }

private:
    static void create() {
        if (++attempts == throw_at) {
            throw construction_error{};
        }
        ++live;
    }
};

template <typename T>
struct destroying_allocator {
    using value_type = T;
    int *destroyed;

    T *allocate(std::size_t n) { return std::allocator<T>{}.allocate(n); }
    void deallocate(T *ptr, std::size_t n) { std::allocator<T>{}.deallocate(ptr, n); }
    void destroy(T *ptr) noexcept {
        ++*destroyed;
        std::destroy_at(ptr);
    }
};

static_assert(wjr::is_trivially_allocator_construct_v<destroying_allocator<counted_value>,
                                                      counted_value, const counted_value &>);
static_assert(
    !wjr::is_trivially_allocator_destroy_v<destroying_allocator<counted_value>, counted_value>);

template <typename Alloc, typename Construct>
// Clang 18/Windows ASan also traps in a standard-only equivalent when GTest's
// exception machinery is inlined into TestBody. Keep this catch in the helper.
WJR_NOINLINE void check_prefix_cleanup(Alloc &alloc, Construct construct) {
    ASSERT_EQ(counted_value::live, 0);
    counted_value::throw_at = 0;
    {
        std::array<counted_value, 3> source;
        auto *dest = alloc.allocate(3);
        counted_value::attempts = 0;
        counted_value::throw_at = 2;
        EXPECT_THROW(construct(source, dest), construction_error);
        EXPECT_EQ(counted_value::live, 3);
        // Leave a failing baseline test with the constructed prefix cleaned up too.
        if (counted_value::live > 3) {
            std::destroy_at(dest);
        }
        counted_value::throw_at = 0;
        alloc.deallocate(dest, 3);
    }
    EXPECT_EQ(counted_value::live, 0);
}

} // namespace

TEST(construction_rollback, noncommon_default_value_and_fill) {
    std::allocator<counted_value> alloc;
    for (int operation = 0; operation < 3; ++operation) {
        SCOPED_TRACE(operation);
        check_prefix_cleanup(alloc, [&](const auto &source, auto *dest) {
            auto first = std::counted_iterator(dest, 3);
            if (operation == 0) {
                wjr::uninitialized_default_construct(first, std::default_sentinel);
            } else if (operation == 1) {
                wjr::uninitialized_value_construct(first, std::default_sentinel);
            } else {
                wjr::uninitialized_fill(first, std::default_sentinel, source[0]);
            }
        });
    }
}

TEST(construction_rollback, noncommon_restrict_copy_preserves_sentinel_pair) {
    std::allocator<counted_value> alloc;
    check_prefix_cleanup(alloc, [&](const auto &source, auto *dest) {
        wjr::uninitialized_copy_restrict(std::counted_iterator(source.data(), 3),
                                         std::default_sentinel, dest);
    });
}

TEST(construction_rollback, allocator_destroy_hook_runs_on_failed_construction) {
    for (int operation = 0; operation < 8; ++operation) {
        SCOPED_TRACE(operation);
        int destroyed = 0;
        destroying_allocator<counted_value> alloc{&destroyed};
        check_prefix_cleanup(alloc, [&](const auto &source, auto *dest) {
            const auto end = std::default_sentinel;
            auto first = std::counted_iterator(dest, 3);
            switch (operation) {
            case 0:
                wjr::uninitialized_copy_restrict_using_allocator(
                    std::counted_iterator(source.data(), 3), end, dest, alloc);
                break;
            case 1:
                wjr::uninitialized_copy_n_restrict_using_allocator(source.data(), 3, dest, alloc);
                break;
            case 2:
                wjr::uninitialized_default_construct_using_allocator(first, end, alloc);
                break;
            case 3:
                wjr::uninitialized_default_construct_n_using_allocator(dest, 3, alloc);
                break;
            case 4:
                wjr::uninitialized_value_construct_using_allocator(first, end, alloc);
                break;
            case 5:
                wjr::uninitialized_value_construct_n_using_allocator(dest, 3, alloc);
                break;
            case 6:
                wjr::uninitialized_fill_using_allocator(first, end, alloc, source[0]);
                break;
            case 7:
                wjr::uninitialized_fill_n_using_allocator(dest, 3, alloc, source[0]);
                break;
            }
        });
        EXPECT_EQ(destroyed, 1);
    }
}
#endif
