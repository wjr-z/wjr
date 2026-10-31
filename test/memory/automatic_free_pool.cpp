#include <wjr/memory/automatic_free_pool.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace {

using pool_type = wjr::automatic_free_pool;

static_assert(!std::is_copy_constructible_v<pool_type>);
static_assert(!std::is_copy_assignable_v<pool_type>);
static_assert(std::is_nothrow_move_constructible_v<pool_type>);
static_assert(std::is_nothrow_move_assignable_v<pool_type>);
static_assert(std::is_nothrow_destructible_v<pool_type>);
static_assert(noexcept(std::declval<pool_type &>().clear()));
static_assert(sizeof(pool_type) == 2 * sizeof(void *));

std::byte *allocate_bytes(pool_type &pool, std::size_t size, std::byte value) {
    auto *ptr = static_cast<std::byte *>(pool.allocate(size));
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % wjr::mem::default_new_alignment, 0u);
    for (std::size_t i = 0; i < size; ++i) {
        ptr[i] = value;
    }
    return ptr;
}

void check_bytes(const std::byte *ptr, std::size_t size, std::byte value) {
    for (std::size_t i = 0; i < size; ++i) {
        EXPECT_EQ(ptr[i], value);
    }
}

} // namespace

TEST(automatic_free_pool, allocation_alignment_and_unlinking) {
    pool_type pool;
    constexpr std::array<std::size_t, 7> sizes = {0, 1, 15, 16, 17, 64, 4096};
    std::array<std::byte *, sizes.size()> blocks;
    for (std::size_t i = 0; i < sizes.size(); ++i) {
        blocks[i] = allocate_bytes(pool, sizes[i], static_cast<std::byte>(i));
    }
    for (const auto i : {3u, 0u, 6u, 1u, 5u, 2u, 4u}) {
        pool_type::deallocate(blocks[i]);
        blocks[i] = nullptr;
        for (std::size_t j = 0; j < sizes.size(); ++j) {
            if (blocks[j] != nullptr) {
                check_bytes(blocks[j], sizes[j], static_cast<std::byte>(j));
            }
        }
    }
    (void)allocate_bytes(pool, 32, std::byte{7});
}

TEST(automatic_free_pool, self_move_preserves_empty_and_nonempty_pool) {
    for (const bool nonempty : {false, true}) {
        pool_type pool;
        auto *original = nonempty ? allocate_bytes(pool, 32, std::byte{7}) : nullptr;
        auto &alias = pool;
        auto &result = (pool = std::move(alias));
        EXPECT_EQ(std::addressof(result), std::addressof(pool));
        if (original != nullptr) {
            check_bytes(original, 32, std::byte{7});
            pool_type::deallocate(original);
        }
        (void)allocate_bytes(pool, 64, std::byte{9});
    }
}

TEST(automatic_free_pool, clear_releases_all_blocks_and_allows_reuse) {
    pool_type pool;
    pool.clear();
    for (int cycle = 0; cycle < 3; ++cycle) {
        (void)allocate_bytes(pool, 0, std::byte{1});
        (void)allocate_bytes(pool, 32, std::byte{2});
        auto *removed = allocate_bytes(pool, 64, std::byte{3});
        pool_type::deallocate(removed);
        pool.clear();
        pool.clear();
        auto *fresh = allocate_bytes(pool, 48, std::byte{4});
        check_bytes(fresh, 48, std::byte{4});
        pool_type::deallocate(fresh);
    }
    (void)allocate_bytes(pool, 96, std::byte{5});
    pool_type destination(std::move(pool));
    pool.clear();
    destination.clear();
    (void)allocate_bytes(destination, 64, std::byte{6});
}

TEST(automatic_free_pool, move_construction_detaches_source_and_preserves_blocks) {
    for (const bool nonempty : {false, true}) {
        auto source = std::make_unique<pool_type>();
        auto *first = nonempty ? allocate_bytes(*source, 32, std::byte{7}) : nullptr;
        auto *second = nonempty ? allocate_bytes(*source, 48, std::byte{9}) : nullptr;
        pool_type destination(std::move(*source));
        auto *independent = allocate_bytes(*source, 32, std::byte{1});
        EXPECT_NE(independent, first);
        EXPECT_NE(independent, second);
        source.reset();
        if (nonempty) {
            check_bytes(first, 32, std::byte{7});
            check_bytes(second, 48, std::byte{9});
            pool_type::deallocate(first);
            pool_type::deallocate(second);
        }
        (void)allocate_bytes(destination, 64, std::byte{3});
    }
}

TEST(automatic_free_pool, move_assignment_replaces_ownership_for_empty_and_nonempty_pools) {
    for (const bool source_nonempty : {false, true}) {
        for (const bool destination_nonempty : {false, true}) {
            auto source = std::make_unique<pool_type>();
            auto *first = source_nonempty ? allocate_bytes(*source, 32, std::byte{7}) : nullptr;
            auto *second = source_nonempty ? allocate_bytes(*source, 48, std::byte{9}) : nullptr;
            pool_type destination;
            if (destination_nonempty) {
                (void)allocate_bytes(destination, 64, std::byte{2});
                (void)allocate_bytes(destination, 96, std::byte{3});
            }
            destination = std::move(*source);
            auto *independent = allocate_bytes(*source, 32, std::byte{1});
            EXPECT_NE(independent, first);
            EXPECT_NE(independent, second);
            source.reset();
            if (source_nonempty) {
                check_bytes(first, 32, std::byte{7});
                check_bytes(second, 48, std::byte{9});
                pool_type::deallocate(second);
                pool_type::deallocate(first);
            }
            (void)allocate_bytes(destination, 64, std::byte{4});
        }
    }
}
