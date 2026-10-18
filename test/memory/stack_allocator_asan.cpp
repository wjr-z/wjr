#include <wjr/memory/asan.hpp>

#if defined(WJR_HAS_ADDRESS_SANITIZER)

    #include <array>
    #include <cstddef>

    #include <gtest/gtest.h>

    #include <wjr/memory/stack_allocator.hpp>

namespace {
bool poisoned(const void *address) { return WJR_ASAN_ADDRESS_IS_POISONED(address) != 0; }
} // namespace

TEST(stack_allocator_asan, rewind_poisons_inner_payload_and_preserves_outer) {
    std::byte *outer = nullptr;
    {
        wjr::unique_stack_allocator parent;
        outer = static_cast<std::byte *>(parent.allocate(64));
        for (const size_t size : {1u, 7u, 16u, 31u, 32u}) {
            std::byte *inner = nullptr;
            {
                wjr::unique_stack_allocator child;
                inner = static_cast<std::byte *>(child.allocate(size));
                EXPECT_FALSE(poisoned(inner));
                EXPECT_FALSE(poisoned(inner + size - 1));
                EXPECT_TRUE(poisoned(inner + size));
            }
            EXPECT_TRUE(poisoned(inner));
            EXPECT_TRUE(poisoned(inner + size - 1));
            EXPECT_TRUE(poisoned(inner + size));
            EXPECT_FALSE(poisoned(outer));
            EXPECT_FALSE(poisoned(outer + 63));
            EXPECT_TRUE(poisoned(outer + 64));
        }
    }
    EXPECT_TRUE(poisoned(outer));
    EXPECT_TRUE(poisoned(outer + 63));
}

TEST(stack_allocator_asan, smaller_reuse_unpoisons_only_the_new_payload) {
    wjr::unique_stack_allocator parent;
    (void)parent.allocate(32);
    std::byte *previous = nullptr;
    {
        wjr::unique_stack_allocator child;
        previous = static_cast<std::byte *>(child.allocate(64));
    }
    EXPECT_TRUE(poisoned(previous));
    {
        wjr::unique_stack_allocator child;
        auto *const reused = static_cast<std::byte *>(child.allocate(7));
        ASSERT_EQ(reused, previous);
        EXPECT_FALSE(poisoned(reused));
        EXPECT_FALSE(poisoned(reused + 6));
        EXPECT_TRUE(poisoned(reused + 7));
        EXPECT_TRUE(poisoned(reused + 31));
        EXPECT_TRUE(poisoned(reused + 63));
    }
    EXPECT_TRUE(poisoned(previous));
}

TEST(stack_allocator_asan, cross_chunk_rewind_and_cached_chunk_reuse) {
    constexpr size_t size = 10 * 1024;
    wjr::unique_stack_allocator parent;
    auto *const outer = static_cast<std::byte *>(parent.allocate(64));
    std::array<std::byte *, 5> starts{}, lasts{};
    {
        wjr::unique_stack_allocator child;
        // Span the first, intermediate and last chunks. Rewind retains cached
        // chunks as well as releasing excess chunks; both must be inaccessible.
        for (size_t i = 0; i != starts.size(); ++i) {
            starts[i] = static_cast<std::byte *>(child.allocate(size));
            lasts[i] = starts[i] + size - 1;
            EXPECT_FALSE(poisoned(starts[i]));
            EXPECT_FALSE(poisoned(lasts[i]));
            EXPECT_TRUE(poisoned(starts[i] + size));
        }
    }
    for (size_t i = 0; i != starts.size(); ++i) {
        EXPECT_TRUE(poisoned(starts[i]));
        EXPECT_TRUE(poisoned(lasts[i]));
    }
    EXPECT_FALSE(poisoned(outer));
    EXPECT_FALSE(poisoned(outer + 63));
    {
        wjr::unique_stack_allocator child;
        for (size_t i = 0; i != 2; ++i) {
            auto *const reused = static_cast<std::byte *>(child.allocate(size));
            EXPECT_EQ(reused, starts[i]);
            EXPECT_FALSE(poisoned(reused));
            EXPECT_FALSE(poisoned(reused + size - 1));
            EXPECT_TRUE(poisoned(reused + size));
        }
    }
    EXPECT_TRUE(poisoned(starts[0]));
    EXPECT_TRUE(poisoned(starts[1]));
    EXPECT_FALSE(poisoned(outer));
}

#endif // WJR_HAS_ADDRESS_SANITIZER
