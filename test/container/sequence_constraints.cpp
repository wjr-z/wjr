#include <wjr/container/ring_buffer.hpp>
#include <wjr/vector.hpp>

#include <array>
#include <concepts>
#include <iterator>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

namespace {

/** @brief Legacy input iterator with deliberately non-const dereference. */
struct legacy_iterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = const int *;
    using reference = const int &;

    pointer ptr = nullptr;

    reference operator*() { return *ptr; }
    legacy_iterator &operator++() {
        ++ptr;
        return *this;
    }
    legacy_iterator operator++(int) {
        auto old = *this;
        ++*this;
        return old;
    }
    bool operator==(const legacy_iterator &) const = default;
};

struct non_iterator {};

template <typename Container, typename Iter>
concept can_assign =
    requires(Container &values, Iter first, Iter last) { values.assign(first, last); };

template <typename Iter>
concept can_modify_vector = requires(wjr::vector<int> &values, Iter first, Iter last) {
    values.insert(values.cbegin(), first, last);
    values.append(first, last);
    values.replace(values.cbegin(), values.cend(), first, last);
};

template <typename Iter>
concept can_deduce_vector = requires(Iter first, Iter last) { wjr::basic_vector(first, last); };

template <typename Iter>
concept can_deduce_ring = requires(Iter first, Iter last) { wjr::basic_ring_buffer(first, last); };

static_assert(wjr::is_iterator_v<legacy_iterator>);
static_assert(!std::input_iterator<legacy_iterator>);
static_assert(std::is_constructible_v<wjr::vector<int>, legacy_iterator, legacy_iterator>);
static_assert(std::is_constructible_v<wjr::ring_buffer<int>, legacy_iterator, legacy_iterator>);
static_assert(!std::is_constructible_v<wjr::vector<int>, non_iterator, non_iterator>);
static_assert(!std::is_constructible_v<wjr::ring_buffer<int>, non_iterator, non_iterator>);
static_assert(can_assign<wjr::vector<int>, legacy_iterator>);
static_assert(can_assign<wjr::ring_buffer<int>, legacy_iterator>);
static_assert(!can_assign<wjr::vector<int>, non_iterator>);
static_assert(!can_assign<wjr::ring_buffer<int>, non_iterator>);
static_assert(can_modify_vector<legacy_iterator>);
static_assert(!can_modify_vector<non_iterator>);
static_assert(can_deduce_vector<legacy_iterator> && can_deduce_vector<const int *>);
static_assert(can_deduce_ring<legacy_iterator> && can_deduce_ring<const int *>);
static_assert(!can_deduce_vector<non_iterator> && !can_deduce_vector<int>);
static_assert(!can_deduce_ring<non_iterator> && !can_deduce_ring<int>);
static_assert(std::is_constructible_v<wjr::vector<int>, wjr::from_range_t, std::span<const int>>);
static_assert(
    !std::is_constructible_v<wjr::vector<int>, wjr::from_range_t, std::span<std::string>>);
static_assert(!std::is_constructible_v<wjr::vector<int>, wjr::from_range_t, non_iterator>);

TEST(sequence_constraints, vector_count_and_legacy_iterator_overloads) {
    const std::array seed{2, 4, 6};
    wjr::basic_vector values(legacy_iterator{seed.data()},
                             legacy_iterator{seed.data() + seed.size()});
    static_assert(std::same_as<decltype(values), wjr::vector<int>>);
    EXPECT_EQ((std::vector<int>{values.begin(), values.end()}), (std::vector<int>{2, 4, 6}));

    values.assign(2, 9);
    values.insert(values.cbegin(), 2, 7);
    values.append(2, 5);
    values.replace(values.cbegin(), values.cbegin() + 2, 1, 3);
    EXPECT_EQ((std::vector<int>{values.begin(), values.end()}), (std::vector<int>{3, 9, 9, 5, 5}));

    const auto first = legacy_iterator{seed.data()};
    const auto last = legacy_iterator{seed.data() + seed.size()};
    values.assign(first, last);
    values.insert(values.cbegin(), first, last);
    values.append(first, last);
    EXPECT_EQ(values.size(), 9u);
    for (std::size_t i = 0; i != values.size(); ++i) {
        EXPECT_EQ(values[i], seed[i % seed.size()]);
    }

    const wjr::vector<int> counted(3, 8);
    EXPECT_EQ(counted.size(), 3u);
    EXPECT_EQ(counted.back(), 8);
    const wjr::vector<int> ranged(wjr::from_range, std::span<const int>(seed));
    EXPECT_EQ(ranged.size(), seed.size());
    EXPECT_EQ(ranged.back(), seed.back());
}

TEST(sequence_constraints, ring_count_and_legacy_iterator_overloads) {
    const std::array seed{2, 4, 6};
    wjr::basic_ring_buffer values(legacy_iterator{seed.data()},
                                  legacy_iterator{seed.data() + seed.size()});
    static_assert(std::same_as<decltype(values), wjr::ring_buffer<int>>);
    EXPECT_EQ(values.size(), seed.size());
    EXPECT_EQ(values.front(), seed.front());
    EXPECT_EQ(values.back(), seed.back());
    values.assign(legacy_iterator{seed.data() + 1}, legacy_iterator{seed.data() + seed.size()});
    EXPECT_EQ(values.size(), 2u);
    EXPECT_EQ(values.front(), 4);

    const wjr::ring_buffer<int> counted(3, 8);
    EXPECT_EQ(counted.size(), 3u);
    EXPECT_EQ(counted.front(), 8);
    EXPECT_EQ(counted.back(), 8);
}

} // namespace
