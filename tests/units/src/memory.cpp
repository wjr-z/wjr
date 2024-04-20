#include <vector>

#include "details.hpp"

#include <wjr/math.hpp>
#include <wjr/memory/uninitialized.hpp>

using namespace wjr;

TEST(memory, stack_allocator) {

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

#if WJR_DEBUG_LEVEL > 2

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
        try {
            uninitialized<std::string> a;
            a.emplace("hello world");
        } catch (...) {
            goto NEXT0;
        }
        goto ERROR;
    NEXT0 : {}
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

#endif