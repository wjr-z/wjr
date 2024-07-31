#include <vector>

#include "detail.hpp"

#include <wjr/math.hpp>
#include <wjr/memory/uninitialized.hpp>

using namespace wjr;

TEST(memory, stack_allocator) {

    static_assert(is_trivially_allocator_v<math_detail::weak_stack_alloc<char>>, "error");

    do {
        math_detail::stack_alloc_object obj;
        (void)(obj);
    } while (0);

    do {
        math_detail::stack_alloc_object obj = {};
        unique_stack_allocator stkal(obj);

        for (int i = 0; i < 32 * 1024; ++i) {
            (void)stkal.allocate(16);
        }

    } while (0);

    do {
        math_detail::stack_alloc_object obj = {};
        unique_stack_allocator stkal(obj);
        (void)stkal.allocate(36 * 1024);

        unique_stack_allocator stkal2(obj);
        (void)stkal.allocate(16);

    } while (0);
}

#if WJR_HAS_DEBUG(UNINITIALIZED_CHECKER)
#else
TEST(memory, uninitialized) {
    do {
        using type = aligned_storage<int>;
        static_assert(std::is_trivially_default_constructible_v<type>,
                      "trivially default constructible error");
        static_assert(std::is_trivially_copy_constructible_v<type>,
                      "trivially copy constructible error");
        static_assert(std::is_trivially_move_constructible_v<type>,
                      "trivially move constructible error");
        static_assert(std::is_trivially_copy_assignable_v<type>,
                      "trivially copy assignable error");
        static_assert(std::is_trivially_move_assignable_v<type>,
                      "trivially move assignable error");
        static_assert(std::is_trivially_destructible_v<type>,
                      "trivially destructible error");
    } while (0);

    do {
        using type = uninitialized<std::string>;
        static_assert(std::is_default_constructible_v<type>,
                      "trivially default constructible error");
        static_assert(!std::is_copy_constructible_v<type>,
                      "trivially copy constructible error");
        static_assert(!std::is_move_constructible_v<type>,
                      "trivially move constructible error");
        static_assert(!std::is_copy_assignable_v<type>,
                      "trivially copy assignable error");
        static_assert(!std::is_move_assignable_v<type>,
                      "trivially move assignable error");
        static_assert(std::is_destructible_v<type>, "trivially destructible error");
    } while (0);
}

#endif