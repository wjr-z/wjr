#include "detail.hpp"

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
} // namespace

TEST(memory, uninitialized) {
    {
        verify_uninitialized<int>();
        verify_uninitialized<std::string>();
        verify_uninitialized<std::pair<int, std::string>>();
    }
}
