#include "detail.hpp"

#include <wjr/crtp/trivially_allocator_base.hpp>
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
