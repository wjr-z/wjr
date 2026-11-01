#include <array>
#include <exception>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include <wjr/vector.hpp>

namespace {

template <typename Vector>
void check_erasure(Vector &values) {
    values.assign({1, 2, 1, 3, 4, 1, 5});
    auto *const storage = values.data();
    const auto capacity = values.capacity();
    EXPECT_EQ(wjr::erase(values, 1), 3);
    EXPECT_TRUE(std::ranges::equal(values, std::array{2, 3, 4, 5}));
    int calls = 0;
    EXPECT_EQ(wjr::erase_if(values,
                            [&calls](int value) {
                                ++calls;
                                return value % 2 == 0;
                            }),
              2);
    EXPECT_EQ(calls, 4);
    EXPECT_TRUE(std::ranges::equal(values, std::array{3, 5}));
    EXPECT_EQ(wjr::erase(values, 100), 0);
    EXPECT_EQ(wjr::erase_if(values, [](int) { return true; }), 2);
    EXPECT_TRUE(values.empty());
    EXPECT_EQ(wjr::erase(values, 1), 0);
    EXPECT_EQ(wjr::erase_if(values, [](int) { return true; }), 0);
    EXPECT_EQ(values.data(), storage);
    EXPECT_EQ(values.capacity(), capacity);
}

struct throwing_assignment {
    inline static int live = 0;
    inline static int budget = -1;
    int value;
    explicit throwing_assignment(int number) noexcept : value(number) { ++live; }
    throwing_assignment(const throwing_assignment &other) noexcept : value(other.value) { ++live; }
    throwing_assignment(throwing_assignment &&other) noexcept : value(other.value) { ++live; }
    throwing_assignment &operator=(throwing_assignment &&other) {
        if (budget == 0)
            throw std::runtime_error("assignment failed");
        if (budget > 0)
            --budget;
        value = other.value;
        return *this;
    }
    ~throwing_assignment() { --live; }
};

template <typename Function>
void check_assignment_failure(Function erase) {
    throwing_assignment::budget = -1;
    {
        wjr::vector<throwing_assignment> values;
        values.reserve(8);
        for (int i = 0; i != 5; ++i)
            values.emplace_back(i);
        auto *const storage = values.data();
        throwing_assignment::budget = 1;
        EXPECT_THROW(erase(values), std::runtime_error);
        EXPECT_EQ(values.size(), 5);
        EXPECT_EQ(values.data(), storage);
        EXPECT_EQ(throwing_assignment::live, 5);
        throwing_assignment::budget = -1;
        values.erase(values.begin());
        EXPECT_EQ(values.size(), 4);
    }
    EXPECT_EQ(throwing_assignment::live, 0);
}

static_assert(noexcept(
    std::declval<wjr::vector<int> &>().erase(std::declval<wjr::vector<int>::const_iterator>())));
static_assert(!noexcept(std::declval<wjr::vector<throwing_assignment> &>().erase(
    std::declval<wjr::vector<throwing_assignment>::const_iterator>())));

template <typename T>
concept erasable_value =
    requires(wjr::vector<int> &values, const T &value) { wjr::erase(values, value); };

static_assert(erasable_value<long> && !erasable_value<void *>);

TEST(vector_erasure, all_storage_policies_retain_capacity_and_order) {
    wjr::vector<int> dynamic;
    check_erasure(dynamic);
    wjr::small_vector<int, 4> small;
    check_erasure(small);
    wjr::inplace_vector<int, 8> inplace;
    check_erasure(inplace);
    wjr::fixed_vector<int> fixed(8, wjr::in_place_reserve);
    check_erasure(fixed);
}

TEST(vector_erasure, heterogeneous_value_and_move_only_elements) {
    wjr::vector<std::string> names{"same", "keep", "same"};
    EXPECT_EQ(wjr::erase(names, std::string_view("same")), 2);
    EXPECT_EQ(names.front(), "keep");
    wjr::vector<std::unique_ptr<int>> pointers;
    for (int i = 0; i != 6; ++i)
        pointers.push_back(std::make_unique<int>(i));
    EXPECT_EQ(wjr::erase_if(pointers, [](const auto &value) { return *value % 2 == 0; }), 3);
    EXPECT_EQ(*pointers[0], 1);
    EXPECT_EQ(*pointers[1], 3);
    EXPECT_EQ(*pointers[2], 5);
}

TEST(vector_erasure, member_assignment_failures_propagate_and_preserve_lifetimes) {
    check_assignment_failure([](auto &values) { values.erase(values.begin()); });
    check_assignment_failure(
        [](auto &values) { values.erase(values.begin(), values.begin() + 2); });
}

TEST(vector_erasure, filtering_assignment_failure_preserves_lifetimes) {
    check_assignment_failure([](auto &values) {
        wjr::erase_if(values, [](const auto &value) { return value.value == 0; });
    });
}

TEST(vector_erasure, predicate_failure_keeps_all_live_elements_owned) {
    wjr::vector<int> values{1, 2, 3, 4, 5};
    const auto capacity = values.capacity();
    int calls = 0;
    EXPECT_THROW(wjr::erase_if(values,
                               [&calls](int value) {
                                   if (++calls == 4)
                                       throw std::runtime_error("predicate failed");
                                   return value == 1;
                               }),
                 std::runtime_error);
    EXPECT_EQ(values.size(), 5);
    EXPECT_EQ(values.capacity(), capacity);
    values.clear();
    EXPECT_TRUE(values.empty());
}

} // namespace
