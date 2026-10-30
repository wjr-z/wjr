#include <wjr/utility/variant.hpp>

#include <array>
#include <memory>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace {

template <typename T, size_t N>
struct selected_values {
    size_t index = 0;
    std::array<T, N> values{};
};

template <typename T>
struct selection_accessor;

template <typename T, size_t N>
struct selection_accessor<selected_values<T, N>> {
    static constexpr size_t value = N;

    static constexpr size_t get_index(const selected_values<T, N> &input) noexcept {
        return input.index;
    }

    template <typename Input, size_t I>
    static constexpr decltype(auto) run(Input &&input, wjr::integral_constant<size_t, I>) {
        return std::get<I>(std::forward<Input>(input).values);
    }
};

struct counted_value {
    static inline int copies = 0;
    static inline int moves = 0;
    int value = 17;

    counted_value() = default;
    counted_value(const counted_value &other) : value(other.value) { ++copies; }
    counted_value(counted_value &&other) noexcept : value(other.value) { ++moves; }
};

struct immovable {
    int value = 23;
    immovable() = default;
    immovable(const immovable &) = delete;
    immovable(immovable &&) = delete;
};

struct reference_category {
    constexpr int operator()(int &) const { return 1; }
    constexpr int operator()(const int &) const { return 2; }
    constexpr int operator()(int &&) const { return 3; }
    constexpr int operator()(const int &&) const { return 4; }
};

constexpr bool constexpr_borrowing() {
    selected_values<int, 2> input{1, {3, 5}};
    auto &result =
        wjr::full_visit<selection_accessor>([](int &value) -> int & { return value; }, input);
    result = 7;
    return input.values[1] == 7 && std::addressof(result) == std::addressof(input.values[1]);
}
static_assert(constexpr_borrowing());

using sparse_indices = wjr::tp_list<wjr::tp_indexs_list_t<0, 0>, wjr::tp_indexs_list_t<0, 1>,
                                    wjr::tp_indexs_list_t<1, 1>>;
using reordered_indices = wjr::tp_list<wjr::tp_indexs_list_t<1, 1>, wjr::tp_indexs_list_t<0, 1>,
                                       wjr::tp_indexs_list_t<0, 0>>;

} // namespace

TEST(variant_visitation, borrows_lvalues_and_preserves_reference_results) {
    selected_values<counted_value, 2> input;
    input.index = 1;
    counted_value::copies = 0;
    counted_value::moves = 0;
    auto &result = wjr::full_visit<selection_accessor>(
        [](counted_value &value) -> counted_value & { return value; }, input);
    EXPECT_EQ(std::addressof(result), std::addressof(input.values[1]));
    result.value = 31;
    EXPECT_EQ(input.values[1].value, 31);
    EXPECT_EQ(counted_value::copies, 0);
    EXPECT_EQ(counted_value::moves, 0);
}

TEST(variant_visitation, preserves_all_reference_categories) {
    selected_values<int, 2> input{1, {3, 5}};
    const selected_values<int, 2> constant{1, {3, 5}};
    EXPECT_EQ(wjr::full_visit<selection_accessor>(reference_category{}, input), 1);
    EXPECT_EQ(wjr::full_visit<selection_accessor>(reference_category{}, constant), 2);
    EXPECT_EQ(wjr::full_visit<selection_accessor>(reference_category{}, std::move(input)), 3);
    EXPECT_EQ(wjr::full_visit<selection_accessor>(reference_category{}, std::move(constant)), 4);
}

TEST(variant_visitation, supports_immovable_and_move_only_elements) {
    selected_values<immovable, 2> fixed;
    fixed.index = 1;
    EXPECT_EQ(
        wjr::full_visit<selection_accessor>([](immovable &value) { return value.value; }, fixed),
        23);

    selected_values<std::unique_ptr<int>, 2> owner;
    owner.index = 1;
    owner.values[1] = std::make_unique<int>(41);
    auto &borrowed =
        wjr::full_visit<selection_accessor>([](auto &value) -> auto & { return value; }, owner);
    EXPECT_EQ(std::addressof(borrowed), std::addressof(owner.values[1]));
    auto moved = wjr::full_visit<selection_accessor>([](auto &&value) { return std::move(value); },
                                                     std::move(owner));
    EXPECT_EQ(*moved, 41);
    EXPECT_EQ(owner.values[1], nullptr);

    EXPECT_EQ(wjr::full_visit<selection_accessor>(
                  [](auto &&value) { return *value; },
                  selected_values<std::unique_ptr<int>, 1>{0, {std::make_unique<int>(43)}}),
              43);
}

TEST(variant_visitation, forwards_rvalues_without_an_intermediate_move) {
    selected_values<counted_value, 1> input;
    counted_value::copies = 0;
    counted_value::moves = 0;
    auto result = wjr::full_visit<selection_accessor>(
        [](counted_value &&value) { return counted_value(std::move(value)); }, std::move(input));
    EXPECT_EQ(result.value, 17);
    EXPECT_EQ(counted_value::copies, 0);
    EXPECT_EQ(counted_value::moves, 1);
}

TEST(variant_visitation, full_dispatch_covers_the_cartesian_product) {
    selected_values<int, 2> first{0, {10, 20}};
    selected_values<int, 3> second{0, {1, 2, 3}};
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            first.index = i;
            second.index = j;
            EXPECT_EQ(wjr::full_visit<selection_accessor>(
                          [](int &lhs, int &rhs) { return lhs + rhs; }, first, second),
                      first.values[i] + second.values[j]);
        }
    }
}

TEST(variant_visitation, sparse_dispatch_preserves_selected_indices_and_references) {
    selected_values<int, 2> first{1, {10, 20}};
    selected_values<int, 2> second{1, {1, 2}};
    auto &result = wjr::discrete_visit<selection_accessor>(
        sparse_indices{}, [](int &, int &rhs) -> int & { return rhs; }, first, second);
    EXPECT_EQ(std::addressof(result), std::addressof(second.values[1]));
    result = 7;
    EXPECT_EQ(second.values[1], 7);
    EXPECT_EQ(wjr::discrete_visit<selection_accessor>(
                  sparse_indices{}, [](int lhs, int rhs) { return lhs + rhs; }, first, second),
              27);
}

TEST(variant_visitation, reordered_sparse_indices_do_not_collide_with_padding) {
    selected_values<int, 2> first{0, {10, 20}};
    selected_values<int, 2> second{0, {1, 2}};
    for (const auto &indices : std::array<std::array<size_t, 2>, 3>{{{1, 1}, {0, 1}, {0, 0}}}) {
        first.index = indices[0];
        second.index = indices[1];
        EXPECT_EQ(
            wjr::discrete_visit<selection_accessor>(
                reordered_indices{}, [](int lhs, int rhs) { return lhs + rhs; }, first, second),
            first.values[first.index] + second.values[second.index]);
    }
}

TEST(variant_visitation, single_sparse_combination_can_use_a_nonzero_index) {
    selected_values<int, 2> first{1, {10, 20}};
    selected_values<int, 2> second{1, {1, 2}};
    using only_last = wjr::tp_list<wjr::tp_indexs_list_t<1, 1>>;
    EXPECT_EQ(wjr::discrete_visit<selection_accessor>(
                  only_last{}, [](int lhs, int rhs) { return lhs + rhs; }, first, second),
              22);
}
