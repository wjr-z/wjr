#include <wjr/memory/uninitialized.hpp>

#include <cstddef>
#include <iterator>
#include <memory>

#include <gtest/gtest.h>

namespace {

/** @brief Only destruction of the actual value type is customized. */
struct typed_destroy_allocator {
    using value_type = int;
    int destroyed = 0;
    void destroy(int *ptr) noexcept {
        ++destroyed;
        std::destroy_at(ptr);
    }
};

enum class range_kind { common, count, sentinel };

void verify_relocation(range_kind kind) {
    typed_destroy_allocator alloc;
    std::allocator<int> storage;
    auto *source = storage.allocate(3);
    auto *destination = storage.allocate(3);
    for (int i = 0; i != 3; ++i) {
        std::construct_at(source + i, 10 + i);
    }
    if (kind == range_kind::count) {
        EXPECT_EQ(wjr::uninitialized_relocate_n_using_allocator(source, 3, destination, alloc),
                  destination + 3);
    } else if (kind == range_kind::sentinel) {
        EXPECT_EQ(wjr::uninitialized_relocate_using_allocator(
                      std::counted_iterator(source, 3), std::default_sentinel, destination, alloc),
                  destination + 3);
    } else {
        EXPECT_EQ(
            wjr::uninitialized_relocate_using_allocator(source, source + 3, destination, alloc),
            destination + 3);
    }
    EXPECT_EQ(alloc.destroyed, 3);
    for (int i = 0; i != 3; ++i) {
        EXPECT_EQ(destination[i], 10 + i);
    }
    wjr::destroy_n_using_allocator(destination, 3, alloc);
    EXPECT_EQ(alloc.destroyed, 6);
    storage.deallocate(source, 3);
    storage.deallocate(destination, 3);
}

} // namespace

TEST(memory, relocation_calls_typed_allocator_destroy) { verify_relocation(range_kind::common); }
TEST(memory, counted_relocation_calls_typed_allocator_destroy) {
    verify_relocation(range_kind::count);
}

TEST(memory, sentinel_relocation_calls_typed_allocator_destroy) {
    verify_relocation(range_kind::sentinel);
}
