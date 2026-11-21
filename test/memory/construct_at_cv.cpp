#include <gtest/gtest.h>

#include <cstddef>

#include <wjr/memory/uninitialized.hpp>

namespace {
template <typename T>
void check_scalar_construction() {
    alignas(T) std::byte storage[sizeof(T)];
    T *const pointer = reinterpret_cast<T *>(storage);
    wjr::construct_at(pointer, 42);
    T *const value = std::launder(pointer);
    EXPECT_EQ(static_cast<int>(*value), 42);
    std::destroy_at(value);
}
} // namespace

TEST(construct_at, cv_qualified_scalar_storage) {
    check_scalar_construction<const int>();
    check_scalar_construction<volatile int>();
    check_scalar_construction<const volatile int>();
}

TEST(construct_at, default_construct_volatile_scalar_storage) {
    alignas(int) std::byte storage[sizeof(int)];
    volatile int *const pointer = reinterpret_cast<volatile int *>(storage);
    wjr::construct_at(pointer, wjr::default_construct);
    volatile int *const value = std::launder(pointer);
    *value = 42;
    EXPECT_EQ(static_cast<int>(*value), 42);
    std::destroy_at(value);
}
