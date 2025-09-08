#include "detail.hpp"
#include "type.hpp"

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
    {
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
}

TEST(memory, lazy_initialized) {
    {
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
}
