#include <wjr/iterator/inserter.hpp>

#include <array>
#include <deque>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace {

// A C++20 range can provide its position type without a nested iterator typedef.
struct fixed_buffer {
    using value_type = int;
    std::array<int, 8> data{};
    std::size_t size = 0;

    constexpr int *begin() noexcept { return data.data(); }
    constexpr int *end() noexcept { return begin() + size; }

    constexpr int *insert(int *position, int value) {
        const auto offset = static_cast<std::size_t>(position - begin());
        for (auto index = size; index > offset; --index) {
            data[index] = data[index - 1];
        }
        data[offset] = value;
        ++size;
        return begin() + offset;
    }

    constexpr void push_back(int value) { (void)insert(end(), value); }
    constexpr void push_front(int value) { (void)insert(begin(), value); }
};

constexpr bool check_constexpr_accessors() {
    fixed_buffer buffer;
    const auto back = std::back_inserter(buffer);
    const auto front = std::front_inserter(buffer);
    auto position = std::inserter(buffer, buffer.end());
    if (std::addressof(wjr::get_inserter_container(back)) != std::addressof(buffer) ||
        std::addressof(wjr::get_inserter_container(front)) != std::addressof(buffer) ||
        std::addressof(wjr::get_inserter_container(position)) != std::addressof(buffer) ||
        wjr::get_inserter_iterator(position) != buffer.end()) {
        return false;
    }
    *position = 2;
    *std::front_inserter(buffer) = 1;
    *std::back_inserter(buffer) = 4;
    position = std::inserter(buffer, buffer.begin() + 2);
    *position = 3;
    return buffer.size == 4 && buffer.data[0] == 1 && buffer.data[1] == 2 && buffer.data[2] == 3 &&
           buffer.data[3] == 4 && wjr::get_inserter_iterator(position) == buffer.begin() + 3;
}

static_assert(check_constexpr_accessors());
static_assert(std::is_same_v<decltype(wjr::get_inserter_iterator(
                                 std::declval<std::insert_iterator<fixed_buffer>>())),
                             int *>);
static_assert(
    noexcept(wjr::get_inserter_iterator(std::declval<std::insert_iterator<fixed_buffer>>())));

TEST(inserter, container_identity_and_mutation) {
    std::vector<int> back_values{1};
    auto back = std::back_inserter(back_values);
    EXPECT_EQ(std::addressof(wjr::get_inserter_container(back)), std::addressof(back_values));
    wjr::get_inserter_container(back).push_back(2);
    *back = 3;
    EXPECT_EQ(back_values, (std::vector<int>{1, 2, 3}));

    std::deque<int> front_values{3};
    const auto front = std::front_inserter(front_values);
    EXPECT_EQ(std::addressof(wjr::get_inserter_container(front)), std::addressof(front_values));
    wjr::get_inserter_container(front).push_front(2);
    *std::front_inserter(front_values) = 1;
    EXPECT_EQ(front_values, (std::deque<int>{1, 2, 3}));
}

TEST(inserter, position_is_an_iterator_including_end) {
    std::vector<int> values;
    EXPECT_EQ(wjr::get_inserter_iterator(std::inserter(values, values.end())), values.end());
    values.reserve(8);
    values = {1, 4};
    auto position = std::inserter(values, values.begin() + 1);
    EXPECT_EQ(std::addressof(wjr::get_inserter_container(position)), std::addressof(values));
    EXPECT_EQ(wjr::get_inserter_iterator(position), values.begin() + 1);
    *position = 2;
    *position = 3;
    EXPECT_EQ(wjr::get_inserter_iterator(position), values.begin() + 3);
    EXPECT_EQ(values, (std::vector<int>{1, 2, 3, 4}));
    EXPECT_EQ(wjr::get_inserter_iterator(std::inserter(values, values.end())), values.end());
}

TEST(inserter, range_without_legacy_iterator_typedef) { EXPECT_TRUE(check_constexpr_accessors()); }

#if !defined(WJR_DISABLE_EXCEPTIONS)
struct copy_error {};

struct throwing_cursor {
    using value_type = int;
    using difference_type = std::ptrdiff_t;

    int *ptr = nullptr;
    const bool *throw_on_copy = nullptr;

    throwing_cursor() = default;
    throwing_cursor(int *data, const bool *fail) noexcept : ptr(data), throw_on_copy(fail) {}
    throwing_cursor(const throwing_cursor &other)
        : ptr(other.ptr), throw_on_copy(other.throw_on_copy) {
        if (throw_on_copy != nullptr && *throw_on_copy) {
            throw copy_error{};
        }
    }
    throwing_cursor &operator=(const throwing_cursor &) = default;

    int &operator*() const noexcept { return *ptr; }
    throwing_cursor &operator++() noexcept {
        ++ptr;
        return *this;
    }
    throwing_cursor operator++(int) {
        auto previous = *this;
        ++*this;
        return previous;
    }

    friend bool operator==(const throwing_cursor &left, const throwing_cursor &right) noexcept {
        return left.ptr == right.ptr;
    }
};

struct throwing_buffer {
    using value_type = int;
    int data = 0;
    bool throw_on_copy = false;

    throwing_cursor begin() noexcept { return {std::addressof(data), &throw_on_copy}; }
    throwing_cursor end() noexcept { return {std::addressof(data) + 1, &throw_on_copy}; }
};

static_assert(
    !noexcept(wjr::get_inserter_iterator(std::declval<std::insert_iterator<throwing_buffer>>())));
static_assert(
    !noexcept(wjr::get_inserter_container(std::declval<std::insert_iterator<throwing_buffer>>())));

WJR_NOINLINE void check_throwing_copy() {
    throwing_buffer buffer;
    auto position = std::inserter(buffer, buffer.begin());
    buffer.throw_on_copy = true;
    EXPECT_THROW((void)wjr::get_inserter_iterator(position), copy_error);
    EXPECT_THROW((void)wjr::get_inserter_container(position), copy_error);
}

TEST(inserter, throwing_iterator_copy_propagates) { check_throwing_copy(); }
#endif

} // namespace
