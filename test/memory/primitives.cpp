#include <array>
#include <bit>
#include <memory>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/memory/uninitialized.hpp>

namespace {

constexpr bool constexpr_construction() {
    std::allocator<int> allocator;
    int *ptr = allocator.allocate(1);
    wjr::construct_at(ptr, 42);
    const bool result = *ptr == 42;
    std::destroy_at(ptr);
    allocator.deallocate(ptr, 1);
    return result;
}

static_assert(constexpr_construction());
static_assert(std::is_same_v<decltype(wjr::construct_at(static_cast<int *>(nullptr), 0)), void>);
static_assert(noexcept(wjr::construct_at(static_cast<int *>(nullptr), 0)));
static_assert(static_cast<int>(wjr::endian::little) == 0);
static_assert(static_cast<int>(wjr::endian::big) == 1);
static_assert(wjr::is_little_endian == (std::endian::native == std::endian::little));
static_assert(wjr::is_big_endian == (std::endian::native == std::endian::big));
static_assert(std::bit_cast<std::array<unsigned char, 4>>(std::uint32_t{0x01020304})[0] == 4);

struct tag_value {
    int value;

    tag_value() noexcept : value(7) {}
    explicit tag_value(wjr::default_construct_t) noexcept : value(42) {}
};

struct default_value {
    int value;

    default_value() noexcept : value(17) {}
};

class friend_value {
    template <typename T, typename... Args>
    friend constexpr void
    wjr::construct_at(T *, Args &&...) noexcept(std::is_nothrow_constructible_v<T, Args...>);

public:
    int value() const noexcept { return m_value; }

private:
    explicit friend_value(int value) noexcept : m_value(value) {}
    int m_value;
};

static_assert(!std::is_constructible_v<friend_value, int>);

} // namespace

TEST(memory_primitives, forwards_move_only_construction) {
    std::allocator<std::unique_ptr<int>> allocator;
    auto *ptr = allocator.allocate(1);
    auto value = std::make_unique<int>(42);
    wjr::construct_at(ptr, std::move(value));
    EXPECT_EQ(value, nullptr);
    EXPECT_EQ(**ptr, 42);
    std::destroy_at(ptr);
    allocator.deallocate(ptr, 1);
}

TEST(memory_primitives, preserves_default_construct_tag_dispatch) {
    tag_value tagged;
    wjr::construct_at(&tagged, wjr::default_construct);
    EXPECT_EQ(tagged.value, 42);
    default_value plain;
    wjr::construct_at(&plain, wjr::default_construct);
    EXPECT_EQ(plain.value, 17);
}

TEST(memory_primitives, preserves_access_for_wjr_friend_constructors) {
    std::allocator<friend_value> allocator;
    auto *ptr = allocator.allocate(1);
    wjr::construct_at(ptr, 42);
    EXPECT_EQ(ptr->value(), 42);
    std::destroy_at(ptr);
    allocator.deallocate(ptr, 1);
}

TEST(memory_primitives, bounded_array_value_initialization) {
    struct {
        int before = 7;
        int values[3] = {1, 2, 3};
        int after = 9;
    } storage;
    wjr::construct_at(std::addressof(storage.values));
    EXPECT_EQ(storage.before, 7);
    for (int value : storage.values) {
        EXPECT_EQ(value, 0);
    }
    EXPECT_EQ(storage.after, 9);
}
