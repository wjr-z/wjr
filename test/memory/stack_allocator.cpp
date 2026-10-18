#include "detail.hpp"

#include <array>
#include <cstdint>
#include <memory>

#include <wjr/memory/stack_allocator.hpp>

using namespace wjr;

template <size_t Alignment>
struct alignas(Alignment) aligned_stack_value {
    std::array<std::byte, Alignment> bytes;
};

template <size_t Alignment>
static void check_overaligned_weak_stack_allocator() {
    using value_type = aligned_stack_value<Alignment>;
    static_assert(Alignment > mem::default_new_alignment);
    static_assert(sizeof(value_type) < stack_allocator_threshold);

    unique_stack_allocator owner;
    weak_stack_allocator<value_type> alloc(owner);

    auto *const small = alloc.allocate(1);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(small) % Alignment, 0);
    std::construct_at(small);
    small->bytes[0] = std::byte{0x2a};
    EXPECT_EQ(small->bytes[0], std::byte{0x2a});
    std::destroy_at(small);
    alloc.deallocate(small, 1);

    const size_t large_count = stack_allocator_threshold / sizeof(value_type) + 1;
    auto *const large = alloc.allocate(large_count);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(large) % Alignment, 0);
    for (size_t i = 0; i < large_count; ++i) {
        std::construct_at(large + i);
        large[i].bytes[0] = static_cast<std::byte>(i);
    }
    EXPECT_EQ(large[0].bytes[0], std::byte{0});
    EXPECT_EQ(large[large_count - 1].bytes[0], static_cast<std::byte>(large_count - 1));
    for (size_t i = 0; i < large_count; ++i) {
        std::destroy_at(large + i);
    }
    alloc.deallocate(large, large_count);
}

TEST(stack_allocator, weak_overaligned_allocations_are_aligned_and_paired) {
    check_overaligned_weak_stack_allocator<64>();
    check_overaligned_weak_stack_allocator<256>();
    check_overaligned_weak_stack_allocator<4096>();
}

TEST(stack_allocator, weak_small_allocations_keep_nested_rewind_behavior) {
    unique_stack_allocator owner;
    weak_stack_allocator<int> alloc(owner);

    auto *const outer = alloc.allocate(1);
    std::construct_at(outer, 17);

    int *inner = nullptr;
    {
        unique_stack_allocator scope;
        inner = alloc.allocate(1);
        std::construct_at(inner, 29);
        EXPECT_EQ(*outer, 17);
        EXPECT_EQ(*inner, 29);
        std::destroy_at(inner);
    }

    auto *const reused = alloc.allocate(1);
    EXPECT_EQ(reused, inner);
    std::construct_at(reused, 31);
    EXPECT_EQ(*outer, 17);
    EXPECT_EQ(*reused, 31);
    std::destroy_at(reused);
    std::destroy_at(outer);
}
