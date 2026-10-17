#include <algorithm>
#include <array>
#include <forward_list>
#include <iterator>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/ring_buffer.hpp>
#include <wjr/container/vector.hpp>
#include <wjr/span.hpp>

namespace {

struct input_state {
    int *current;
    int *last;
    int increments = 0;
    int begins = 0;
};

// Copies share the cursor; taking a distance would consume this input range.
struct single_pass_iterator {
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;

    input_state *state = nullptr;

    int &operator*() const { return *state->current; }
    single_pass_iterator &operator++() {
        ++state->current;
        ++state->increments;
        return *this;
    }
    void operator++(int) { ++*this; }
    bool operator==(std::default_sentinel_t) const { return state->current == state->last; }
};

struct single_pass_range {
    input_state *state;

    auto begin() {
        ++state->begins;
        return single_pass_iterator{state};
    }
    auto end() const { return std::default_sentinel; }
};

struct sized_input_range : single_pass_range {
    auto size() const { return static_cast<std::size_t>(state->last - state->current); }
};

struct common_input_iterator {
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;

    int *ptr = nullptr;

    int &operator*() const { return *ptr; }
    common_input_iterator &operator++() {
        ++ptr;
        return *this;
    }
    void operator++(int) { ++*this; }
    bool operator==(const common_input_iterator &) const = default;
};

struct sized_common_input_range {
    int *first;
    int *last;

    auto begin() const { return common_input_iterator{first}; }
    auto end() const { return common_input_iterator{last}; }
    auto size() const { return static_cast<std::size_t>(last - first); }
};

struct move_only_iterator : single_pass_iterator {
    move_only_iterator() = default;
    explicit move_only_iterator(input_state *state) : single_pass_iterator{state} {}
    move_only_iterator(move_only_iterator &&) = default;
    move_only_iterator &operator=(move_only_iterator &&) = default;
    move_only_iterator(const move_only_iterator &) = delete;
    move_only_iterator &operator=(const move_only_iterator &) = delete;

    move_only_iterator &operator++() {
        single_pass_iterator::operator++();
        return *this;
    }
    void operator++(int) { ++*this; }
};

template <bool Sized>
struct move_only_input_range {
    input_state *state;

    auto begin() {
        ++state->begins;
        return move_only_iterator(state);
    }
    auto end() const { return std::default_sentinel; }
    auto size() const
    requires Sized
    {
        return static_cast<std::size_t>(state->last - state->current);
    }
};

static_assert(std::ranges::input_range<single_pass_range>);
static_assert(!std::ranges::forward_range<single_pass_range>);
static_assert(!std::ranges::sized_range<single_pass_range>);
static_assert(std::ranges::sized_range<sized_input_range>);
static_assert(!std::ranges::common_range<sized_input_range>);
static_assert(std::ranges::input_range<sized_common_input_range>);
static_assert(std::ranges::common_range<sized_common_input_range>);
static_assert(!wjr::is_iterator_v<common_input_iterator>);
static_assert(wjr::detail::_container_compatible_range<single_pass_range, int>);
static_assert(!wjr::detail::_container_compatible_range<std::array<std::string, 3>, int>);
static_assert(
    !std::is_constructible_v<wjr::vector<int>, wjr::from_range_t, std::array<std::string, 3> &>);
static_assert(std::ranges::input_range<move_only_input_range<false>>);
static_assert(std::ranges::input_range<move_only_input_range<true>>);
static_assert(!std::copy_constructible<move_only_iterator>);
static_assert(std::contiguous_iterator<wjr::vector<int>::iterator>);
static_assert(std::contiguous_iterator<wjr::vector<int>::const_iterator>);
static_assert(std::contiguous_iterator<wjr::span<volatile int>::iterator>);
static_assert(std::ranges::contiguous_range<wjr::vector<std::unique_ptr<int>>>);
static_assert(std::ranges::contiguous_range<wjr::span<int>>);
static_assert(!std::ranges::borrowed_range<wjr::span<int>>);
static_assert(!std::ranges::contiguous_range<wjr::ring_buffer<int>>);
static_assert(std::same_as<wjr::ring_buffer<int>::array_range, std::ranges::subrange<int *>>);
static_assert(
    std::same_as<wjr::ring_buffer<int>::const_array_range, std::ranges::subrange<const int *>>);
static_assert(std::ranges::borrowed_range<wjr::ring_buffer<int>::array_range>);

template <typename Range>
void expect_values(const Range &values) {
    EXPECT_EQ(values.size(), 3);
    EXPECT_EQ(values[0], 2);
    EXPECT_EQ(values[1], 4);
    EXPECT_EQ(values[2], 6);
}

template <bool Sized>
void check_move_only_input() {
    int source[] = {2, 4, 6};
    input_state state{source, source + 3};
    wjr::vector<int> values(wjr::from_range, move_only_input_range<Sized>{&state});
    expect_values(values);
    EXPECT_EQ(state.begins, 1);
    EXPECT_EQ(state.increments, 3);
    if constexpr (Sized) {
        EXPECT_EQ(values.capacity(), 3);
    }
}

} // namespace

TEST(ranges, standard_algorithms_use_wjr_iterators_directly) {
    wjr::vector<int> values{6, 2, 4};
    std::ranges::sort(values);
    expect_values(values);
    EXPECT_EQ(std::ranges::find(values, 4), values.begin() + 1);
    std::span<int> view(values);
    view[1] = 9;
    EXPECT_EQ(values[1], 9);

    wjr::span local(values.data(), values.size());
    std::ranges::reverse(local);
    EXPECT_EQ(values[0], 6);
    EXPECT_EQ(values[2], 2);
}

TEST(ranges, vector_preallocates_sized_and_forward_ranges) {
    const std::array source{2, 4, 6};
    wjr::vector<int> sized(wjr::from_range, source);
    expect_values(sized);
    EXPECT_EQ(sized.capacity(), 3);

    const std::forward_list<int> forward{2, 4, 6};
    wjr::vector<int> unsized_forward(wjr::from_range, forward);
    expect_values(unsized_forward);
    EXPECT_EQ(unsized_forward.capacity(), 3);

    auto projected = source | std::views::transform([](int value) { return value + 1; });
    wjr::vector<int> proxy(wjr::from_range, projected);
    EXPECT_EQ(proxy[0], 3);
    EXPECT_EQ(proxy[2], 7);
}

TEST(ranges, vector_accepts_noncommon_counted_ranges) {
    int source[] = {2, 4, 6};
    auto input = std::ranges::subrange(std::counted_iterator(source, 3), std::default_sentinel);
    static_assert(std::ranges::sized_range<decltype(input)>);
    static_assert(!std::ranges::common_range<decltype(input)>);
    wjr::vector<int> values(wjr::from_range, input);
    expect_values(values);
    EXPECT_EQ(values.capacity(), 3);

    auto forward = std::views::iota(1) | std::views::take(3) |
                   std::views::transform([](int value) { return value * 2; });
    static_assert(std::ranges::forward_range<decltype(forward)>);
    static_assert(!std::ranges::sized_range<decltype(forward)>);
    static_assert(!std::ranges::common_range<decltype(forward)>);
    wjr::vector<int> unsized(wjr::from_range, forward);
    expect_values(unsized);
    EXPECT_EQ(unsized.capacity(), 3);
}

TEST(ranges, vector_reads_single_pass_input_once) {
    int source[] = {2, 4, 6};
    input_state state{source, source + 3};
    single_pass_range input{&state};
    wjr::vector<int> values(wjr::from_range, input);
    expect_values(values);
    EXPECT_EQ(state.begins, 1);
    EXPECT_EQ(state.increments, 3);

    input_state empty_state{source, source};
    wjr::vector<int> empty(wjr::from_range, single_pass_range{&empty_state});
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty_state.increments, 0);
}

TEST(ranges, vector_accepts_sized_modern_input_iterators) {
    int source[] = {2, 4, 6};
    input_state state{source, source + 3};
    sized_input_range input{{&state}};
    wjr::vector<int> values(wjr::from_range, input);
    expect_values(values);
    EXPECT_EQ(values.capacity(), 3);
    EXPECT_EQ(state.begins, 1);
    EXPECT_EQ(state.increments, 3);

    wjr::vector<int> common(wjr::from_range, sized_common_input_range{source, source + 3});
    expect_values(common);
    EXPECT_EQ(common.capacity(), 3);
}

TEST(ranges, vector_accepts_move_only_input_iterators) {
    check_move_only_input<false>();
    check_move_only_input<true>();
}

TEST(ranges, ring_segments_are_standard_borrowed_subranges) {
    wjr::ring_buffer<int> empty;
    EXPECT_TRUE(std::ranges::empty(empty.array_one()));
    EXPECT_TRUE(std::ranges::empty(empty.array_two()));

    wjr::ring_buffer<int> ring{1, 2, 3, 4, 5};
    ring.pop_front();
    ring.pop_front();
    ring.push_back(6);
    ring.push_back(7);
    std::vector<int> combined;
    std::ranges::copy(ring.array_one(), std::back_inserter(combined));
    std::ranges::copy(ring.array_two(), std::back_inserter(combined));
    EXPECT_EQ(combined, (std::vector<int>{3, 4, 5, 6, 7}));
    const auto &constant = ring;
    EXPECT_EQ(constant.array_one().size() + constant.array_two().size(), ring.size());
    const auto first = ring.array_one();
    *first.begin() = 8;
    EXPECT_EQ(ring.front(), 8);
}
