#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <wjr/crtp/trivially_allocator_base.hpp>
#include <wjr/memory/object_pool.hpp>
#include <wjr/memory/small_copy.hpp>
#include <wjr/memory/stack_allocator.hpp>

using namespace wjr;

WJR_NOINLINE static bool pointer_slot_changed(int **dest, int **source, int **observed) {
    const auto *before = *observed;
    wjr::small_copy<1, 1>(source, source + 1, dest);
    return *observed != before;
}

TEST(small_copy, pointer_slot_remains_observable_through_its_type) {
    int first = 0, second = 0;
    int *source = &first;
    int *dest = &second;
    EXPECT_TRUE(pointer_slot_changed(&dest, &source, &dest));
    EXPECT_EQ(dest, source);
}

TEST(small_copy, overlapping_pointer_slots) {
    int values[18] = {};
    std::array<int *, 18> slots;
    for (size_t count = 0; count < slots.size(); ++count) {
        SCOPED_TRACE(count);
        for (size_t i = 0; i < slots.size(); ++i) {
            slots[i] = values + i;
        }
        wjr::small_copy(slots.data(), slots.data() + count, slots.data() + 1);
        for (size_t i = 0; i < count; ++i) {
            ASSERT_EQ(slots[i + 1], values + i);
        }
        wjr::small_copy(slots.data() + 1, slots.data() + count + 1, slots.data());
        for (size_t i = 0; i < count; ++i) {
            ASSERT_EQ(slots[i], values + i);
        }
    }
}

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

namespace {

struct alignas(4 * wjr::mem::default_new_alignment) aligned_pool_value {
    int value;
};

struct padded_aligned_pool_traits
    : wjr::mem::detail::single_object_pool_traits<aligned_pool_value> {
    static constexpr std::pair<size_t, size_t> get(size_t size) noexcept {
        return {0, size + wjr::mem::default_new_alignment};
    }
};

} // namespace

TEST(object_pool, padded_size_class_preserves_value_alignment) {
    dynamic_object_pool<padded_aligned_pool_traits> pool;
    auto *first = pool.allocate(1);
    auto *second = pool.allocate(1);
    ASSERT_EQ(reinterpret_cast<uintptr_t>(first) % alignof(aligned_pool_value), 0);
    ASSERT_EQ(reinterpret_cast<uintptr_t>(second) % alignof(aligned_pool_value), 0);
    std::construct_at(first, 17);
    std::construct_at(second, 29);
    EXPECT_EQ(first->value, 17);
    EXPECT_EQ(second->value, 29);
    std::destroy_at(first);
    std::destroy_at(second);
    pool.deallocate(first, 1);
    pool.deallocate(second, 1);
}

TEST(object_pool, overaligned_slots_survive_move_and_reuse) {
    object_pool<aligned_pool_value> source;
    auto *initial = source.allocate(1);
    ASSERT_EQ(reinterpret_cast<uintptr_t>(initial) % alignof(aligned_pool_value), 0);
    source.deallocate(initial, 1);

    object_pool<aligned_pool_value> target(std::move(source));
    auto *reused = target.allocate(1);
    EXPECT_EQ(reused, initial);
    auto *fresh = source.allocate(1);
    EXPECT_NE(fresh, reused);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(fresh) % alignof(aligned_pool_value), 0);
    target.deallocate(reused, 1);
    source.deallocate(fresh, 1);
}
