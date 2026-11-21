#include "type.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <iterator>
#include <memory>
#include <vector>
#include <wjr/memory/uninitialized.hpp>

using namespace wjr;

namespace {
template <typename T>
void verify_uninitialized() {
    using type = uninitialized<T>;
    static_assert(std::is_constructible_v<type>, "constructible test error");
    static_assert(!std::is_copy_constructible_v<type>, "copy constructible test error");
    static_assert(!std::is_move_constructible_v<type>, "move constructible test error");
    static_assert(!std::is_copy_assignable_v<type>, "copy assignable test error");
    static_assert(!std::is_move_assignable_v<type>, "move assignable test error");
    static_assert(std::is_destructible_v<type>, "destructible test error");

    static_assert(std::is_trivially_default_constructible_v<type> ==
                      std::is_trivially_default_constructible_v<T>,
                  "trivially default constructible test error");
    static_assert(std::is_trivially_destructible_v<type> == std::is_trivially_destructible_v<T>,
                  "trivially destructible test error");
}

template <typename T>
void verify_lazy_initialized() {
    using type = lazy_initialized<T>;
    static_assert(std::is_constructible_v<type>, "constructible test error");
    static_assert(std::is_copy_constructible_v<type> == std::is_copy_constructible_v<T>,
                  "copy constructible test error");
    static_assert(std::is_move_constructible_v<type> == std::is_move_constructible_v<T>,
                  "move constructible test error");
    static_assert(std::is_copy_assignable_v<type> == std::is_copy_assignable_v<T>,
                  "copy assignable test error");
    static_assert(std::is_move_assignable_v<type> == std::is_move_assignable_v<T>,
                  "move assignable test error");
    static_assert(std::is_destructible_v<type>, "destructible test error");

    static_assert(std::is_trivially_default_constructible_v<type> ==
                      std::is_trivially_default_constructible_v<T>,
                  "trivially default constructible test error");
    static_assert(std::is_trivially_copy_constructible_v<type> ==
                      std::is_trivially_copy_constructible_v<T>,
                  "trivially copy constructible test error");
    static_assert(std::is_trivially_move_constructible_v<type> ==
                      std::is_trivially_move_constructible_v<T>,
                  "trivially move constructible test error");
    static_assert(std::is_trivially_copy_assignable_v<type> ==
                      std::is_trivially_copy_assignable_v<T>,
                  "trivially copy assignable test error");
    static_assert(std::is_trivially_move_assignable_v<type> ==
                      std::is_trivially_move_assignable_v<T>,
                  "trivially move assignable test error");
    static_assert(std::is_trivially_destructible_v<type> == std::is_trivially_destructible_v<T>,
                  "trivially destructible test error");
}

} // namespace

TEST(memory, uninitialized) {
    verify_uninitialized<int>();
    verify_uninitialized<std::string>();
    verify_uninitialized<std::pair<int, std::string>>();

    verify_uninitialized<disable_copy>();
    verify_uninitialized<disable_move>();
    verify_uninitialized<disable_copy_construct>();
    verify_uninitialized<disable_move_construct>();
    verify_uninitialized<disable_copy_assign>();
    verify_uninitialized<disable_move_assign>();
}

TEST(memory, lazy_initialized) {
    verify_lazy_initialized<int>();
    verify_lazy_initialized<std::string>();
    verify_lazy_initialized<std::pair<int, std::string>>();

    verify_lazy_initialized<disable_copy>();
    verify_lazy_initialized<disable_move>();
    verify_lazy_initialized<disable_copy_construct>();
    verify_lazy_initialized<disable_move_construct>();
    verify_lazy_initialized<disable_copy_assign>();
    verify_lazy_initialized<disable_move_assign>();
}

TEST(memory, allocator_fill_n_constructs_each_element) {
    struct allocator {
        using value_type = int;
        std::vector<int *> constructed;
        void construct(int *ptr, const int &value) {
            constructed.push_back(ptr);
            wjr::construct_at(ptr, value);
        }
    } alloc;
    auto *ptr = std::allocator<int>().allocate(3);
    uninitialized_fill_n_using_allocator(ptr, 3, alloc, 42);
    ASSERT_EQ(alloc.constructed.size(), 3);
    EXPECT_EQ(alloc.constructed[0], ptr);
    EXPECT_EQ(alloc.constructed[1], ptr + 1);
    EXPECT_EQ(alloc.constructed[2], ptr + 2);
    uninitialized_fill_n_using_allocator(ptr, 0, alloc, 42);
    EXPECT_EQ(alloc.constructed.size(), 3);
    std::allocator<int>().deallocate(ptr, 3);
}

TEST(memory, trivial_relocation_returns_destination_end) {
    int source[] = {7, 11, 13};
    int destination[3] = {};
    std::allocator<int> alloc;
    EXPECT_EQ(uninitialized_relocate(source, source + 3, destination), destination + 3);
    EXPECT_TRUE(std::equal(source, source + 3, destination));
    EXPECT_EQ(uninitialized_relocate_n(source, 3, destination), destination + 3);
    EXPECT_EQ(uninitialized_relocate_restrict(source, source + 3, destination), destination + 3);
    EXPECT_EQ(uninitialized_relocate_n_restrict(source, 3, destination), destination + 3);
    EXPECT_EQ(uninitialized_relocate_using_allocator(source, source + 3, destination, alloc),
              destination + 3);
    EXPECT_EQ(uninitialized_relocate_n_using_allocator(source, 3, destination, alloc),
              destination + 3);
    EXPECT_EQ(uninitialized_relocate_n(source, 0, destination), destination);
    int *empty = nullptr;
    EXPECT_EQ(uninitialized_relocate(empty, empty, empty), empty);
    EXPECT_EQ(uninitialized_relocate_n(empty, 0, empty), empty);
}

TEST(memory, uninitialized_move_from_a_counted_range) {
    using pointer = std::unique_ptr<int>;
    std::allocator<pointer> alloc;
    auto *dest = alloc.allocate(2);
    for (int operation = 0; operation < 4; ++operation) {
        SCOPED_TRACE(operation);
        std::array source = {std::make_unique<int>(7), std::make_unique<int>(11)};
        auto first = std::counted_iterator(source.data(), 2);
        constexpr auto last = std::default_sentinel;
        pointer *end;
        switch (operation) {
        case 0:
            end = wjr::uninitialized_move(first, last, dest);
            break;
        case 1:
            end = wjr::uninitialized_move_restrict(first, last, dest);
            break;
        case 2:
            end = wjr::uninitialized_move_using_allocator(first, last, dest, alloc);
            break;
        default:
            end = wjr::uninitialized_move_restrict_using_allocator(first, last, dest, alloc);
            break;
        }
        EXPECT_EQ(end, dest + 2);
        EXPECT_EQ(source[0], nullptr);
        EXPECT_EQ(source[1], nullptr);
        EXPECT_EQ(*dest[0], 7);
        EXPECT_EQ(*dest[1], 11);
        wjr::destroy(std::counted_iterator(dest, 2), last);
    }
    alloc.deallocate(dest, 2);
}

TEST(memory, relocate_from_a_counted_range_destroys_the_original_objects) {
    int destroyed = 0;
    struct element {
        int *destroyed;
        ~element() { ++*destroyed; }
    };
    std::allocator<element> alloc;
    auto *source = alloc.allocate(2);
    auto *dest = alloc.allocate(2);
    std::construct_at(source, &destroyed);
    std::construct_at(source + 1, &destroyed);
    EXPECT_EQ(
        wjr::uninitialized_relocate(std::counted_iterator(source, 2), std::default_sentinel, dest),
        dest + 2);
    EXPECT_EQ(destroyed, 2);
    wjr::destroy(dest, dest + 2);
    EXPECT_EQ(destroyed, 4);
    alloc.deallocate(source, 2);
    alloc.deallocate(dest, 2);
}
