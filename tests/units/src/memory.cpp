#include <vector>

#include "details.hpp"

#include <wjr/math.hpp>
#include <wjr/memory/aligned_allocator.hpp>
#include <wjr/memory/uninitialized.hpp>

using namespace wjr;

TEST(memory, stack_allocator) {

    static_assert(is_trivially_allocator_v<math_details::weak_stack_alloc<char>>,
                  "error");

    do {
        math_details::stack_alloc_object obj;
        (void)(obj);
    } while (0);

    do {
        math_details::stack_alloc_object obj = {};
        unique_stack_allocator stkal(obj);

        for (int i = 0; i < 32 * 1024; ++i) {
            (void)stkal.allocate(16);
        }

    } while (0);

    do {
        math_details::stack_alloc_object obj = {};
        unique_stack_allocator stkal(obj);
        (void)stkal.allocate(36 * 1024);

        unique_stack_allocator stkal2(obj);
        (void)stkal.allocate(16);

    } while (0);
}

TEST(memory, algined_allocator) {
    do {
        using alloc = aligned_allocator<std::allocator<int>, 64>;
        static_assert(is_trivially_allocator_v<alloc>, "error");

        alloc al;
        auto ptr = al.allocate(1);
        WJR_ASSERT(reinterpret_cast<uintptr_t>(ptr) % 64 == 0, "alignment error");
        al.deallocate(ptr, 1);

        auto result = allocate_at_least(al, 32);
        WJR_ASSERT(reinterpret_cast<uintptr_t>(result.ptr) % 64 == 0, "alignment error");
        WJR_ASSERT(result.count >= 32, "count error");
        al.deallocate(result.ptr, result.count);
    } while (0);

    do {
        using weak_alloc = math_details::weak_stack_alloc<int>;
        using alloc = aligned_allocator<weak_alloc, 64>;
        static_assert(is_trivially_allocator_v<alloc>, "error");

        math_details::unique_stack_alloc stkal(math_details::stack_alloc);
        alloc al(stkal);

        auto ptr = al.allocate(1);
        WJR_ASSERT(reinterpret_cast<uintptr_t>(ptr) % 64 == 0, "alignment error");
        al.deallocate(ptr, 1);

        auto result = allocate_at_least(al, 32);
        WJR_ASSERT(reinterpret_cast<uintptr_t>(result.ptr) % 64 == 0, "alignment error");
        WJR_ASSERT(result.count >= 32, "count error");
        al.deallocate(result.ptr, result.count);
    } while (0);
}

#if WJR_HAS_BUILTIN(UNINITIALIZED_CHECKER)

TEST(memory, uninitialized) {
    do {
        uninitialized<std::string> a;

        try {
            auto &x = a.get();
            goto ERROR;
        } catch (...) {
        }

        try {
            a.reset();
            goto ERROR;
        } catch (...) {
        }

        a.emplace("hello world");

        try {
            auto &x = a.get();
        } catch (...) {
            goto ERROR;
        }

        a.reset();

    } while (0);

    do {
        uninitialized<uint32_t> a;

        try {
            auto &x = a.get();
            goto ERROR;
        } catch (...) {
        }

        a.reset();

    } while (0);

    return;
ERROR : { WJR_ASSERT(false, "uninitialized error"); }
}

#else

TEST(memory, uninitialized) {
    do {
        using type = uninitialized<int>;
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