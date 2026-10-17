#include "detail.hpp"

#include <array>
#include <wjr/crtp/trivially_allocator_base.hpp>
#include <wjr/memory/object_pool.hpp>
#include <wjr/memory/stack_allocator.hpp>

using namespace wjr;

TEST(stack_allocator, simple_test) {
    static_assert(is_trivially_allocator_v<weak_stack_allocator<char>>, "error");

    do {
        unique_stack_allocator stkal;

        for (int i = 0; i < 32 * 1024; ++i) {
            (void)stkal.allocate(16);
        }

    } while (false);

    do {
        unique_stack_allocator stkal;
        (void)stkal.allocate(36 * 1024);

        unique_stack_allocator stkal2;
        (void)stkal.allocate(16);
    } while (false);
}

TEST(arena, unaligned_cache_size) {
    arena pool(static_cast<uint32_t>(mem::default_new_alignment + 1));
    std::array<std::byte *, 8> allocations;
    for (size_t i = 0; i < allocations.size(); ++i) {
        auto *ptr = static_cast<std::byte *>(pool.allocate(1));
        EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % mem::default_new_alignment, 0);
        *ptr = static_cast<std::byte>(i);
        allocations[i] = ptr;
    }
    for (size_t i = 0; i < allocations.size(); ++i) {
        EXPECT_EQ(*allocations[i], static_cast<std::byte>(i));
    }
}

TEST(arena, moved_from_arena_can_allocate_independently) {
    arena source;
    auto *original = static_cast<int *>(source.allocate(sizeof(int)));
    *original = 42;
    arena destination(std::move(source));
    auto *dst = static_cast<int *>(destination.allocate(sizeof(int)));
    auto *src = static_cast<int *>(source.allocate(sizeof(int)));
    EXPECT_NE(dst, src);
    *dst = 7;
    *src = 9;
    EXPECT_EQ(*original, 42);
    EXPECT_EQ(*dst, 7);

    arena assigned;
    (void)assigned.allocate(1);
    assigned = std::move(destination);
    dst = static_cast<int *>(assigned.allocate(sizeof(int)));
    src = static_cast<int *>(destination.allocate(sizeof(int)));
    EXPECT_NE(dst, src);
    EXPECT_EQ(*original, 42);
}

TEST(object_pool, small_objects_are_reused) {
    object_pool<char> pool;
    EXPECT_EQ(pool.allocate(0), nullptr);
    pool.deallocate(nullptr, 0);
    auto *first = pool.allocate(1);
    auto *second = pool.allocate(1);
    *first = 'a';
    *second = 'b';
    pool.deallocate(first, 1);
    EXPECT_EQ(*second, 'b');
    auto *reused = pool.allocate(1);
    EXPECT_EQ(reused, first);
    *reused = 'c';
    EXPECT_EQ(*second, 'b');
    pool.deallocate(reused, 1);
    pool.deallocate(second, 1);
}

TEST(object_pool, move_transfers_free_list_ownership) {
    object_pool<int> source;
    auto *freed = source.allocate(1);
    source.deallocate(freed, 1);
    object_pool<int> destination(std::move(source));
    auto *owned = destination.allocate(1);
    auto *independent = source.allocate(1);
    EXPECT_EQ(owned, freed);
    EXPECT_NE(owned, independent);
    *owned = 42;
    *independent = 7;
    EXPECT_EQ(*owned, 42);
    source.deallocate(independent, 1);
    destination.deallocate(owned, 1);

    object_pool<int> assigned;
    auto *old = assigned.allocate(1);
    assigned.deallocate(old, 1);
    assigned = std::move(destination);
    owned = assigned.allocate(1);
    independent = destination.allocate(1);
    EXPECT_EQ(owned, freed);
    EXPECT_NE(owned, independent);
    assigned.deallocate(owned, 1);
    destination.deallocate(independent, 1);
}
