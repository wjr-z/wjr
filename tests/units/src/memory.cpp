#include <vector>

#include "details.hpp"

#include <wjr/math.hpp>

namespace memory_tests {

TEST(memory, stack_allocator) {
    using namespace wjr;

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

} // namespace memory_tests