#include <array>
#include <concepts>
#include <cstddef>
#include <forward_list>
#include <istream>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <ranges>
#include <set>
#include <span>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/vector.hpp>

namespace {
struct insertion_state {
    std::set<const void *> live;
    int allocations = 0, releases = 0, constructed = 0, destroyed = 0;
    int copies = -1, moves = -1, assignments = -1, hooks = -1;
    bool fail_allocation = false;
};

void consume_budget(int &budget) {
    if (budget == 0)
        throw std::runtime_error("insertion operation");
    if (budget > 0)
        --budget;
}

struct insertion_value {
    static inline insertion_state *state = nullptr;
    int value;
    insertion_value(int n = 0) : value(n) { state->live.insert(this); }
    insertion_value(const insertion_value &other) : value(other.value) {
        consume_budget(state->copies);
        state->live.insert(this);
    }
    insertion_value(insertion_value &&other) : value(other.value) {
        consume_budget(state->moves);
        other.value = -1;
        state->live.insert(this);
    }
    insertion_value &operator=(const insertion_value &other) {
        consume_budget(state->assignments);
        value = other.value;
        return *this;
    }
    insertion_value &operator=(insertion_value &&other) {
        consume_budget(state->assignments);
        value = other.value;
        other.value = -1;
        return *this;
    }
    ~insertion_value() { EXPECT_EQ(state->live.erase(this), 1); }
};

template <typename T>
struct insertion_allocator {
    using value_type = T;
    insertion_state *state;
    explicit insertion_allocator(insertion_state &s) noexcept : state(&s) {}
    template <typename U>
    insertion_allocator(const insertion_allocator<U> &other) noexcept : state(other.state) {}
    T *allocate(std::size_t count) {
        if (state->fail_allocation)
            throw std::bad_alloc();
        auto *pointer = std::allocator<T>{}.allocate(count);
        ++state->allocations;
        return pointer;
    }
    void deallocate(T *pointer, std::size_t count) noexcept {
        ++state->releases;
        std::allocator<T>{}.deallocate(pointer, count);
    }
    template <typename U, typename... Args>
    void construct(U *pointer, Args &&...args) {
        consume_budget(state->hooks);
        std::construct_at(pointer, std::forward<Args>(args)...);
        ++state->constructed;
    }
    template <typename U>
    void destroy(U *pointer) noexcept {
        ++state->destroyed;
        std::destroy_at(pointer);
    }
    template <typename U>
    bool operator==(const insertion_allocator<U> &other) const noexcept {
        return state == other.state;
    }
};

struct sized_input {
    std::size_t count, position = 0, begins = 0;
    std::size_t increment_failure = std::numeric_limits<std::size_t>::max();
    struct iterator {
        using iterator_concept = std::input_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        sized_input *owner;
        explicit iterator(sized_input &input) noexcept : owner(&input) {}
        iterator(const iterator &) = delete;
        iterator(iterator &&) = default;
        iterator &operator=(iterator &&) = default;
        int operator*() const { return static_cast<int>(owner->position); }
        iterator &operator++() {
            if (owner->position == owner->increment_failure)
                throw std::runtime_error("input increment");
            ++owner->position;
            return *this;
        }
        void operator++(int) { ++*this; }
        bool operator==(std::default_sentinel_t) const { return owner->position == owner->count; }
    };
    iterator begin() {
        ++begins;
        return iterator(*this);
    }
    std::default_sentinel_t end() const { return {}; }
    std::size_t size() const { return count; }
};

template <typename T>
struct narrow_allocator : insertion_allocator<T> {
    using value_type = T;
    using size_type = unsigned char;
    explicit narrow_allocator(insertion_state &state) noexcept : insertion_allocator<T>(state) {}
    template <typename U>
    narrow_allocator(const narrow_allocator<U> &other) noexcept
        : insertion_allocator<T>(*other.state) {}
};

template <typename T, typename Range>
concept insertable = requires(wjr::vector<T> &values, Range &&range) {
    {
        values.insert_range(values.cbegin(), std::forward<Range>(range))
    } -> std::same_as<typename wjr::vector<T>::iterator>;
};
static_assert(insertable<int, std::array<int, 3> &>);
static_assert(!insertable<int, std::array<void *, 3> &>);
static_assert(!insertable<std::unique_ptr<int>, std::array<std::unique_ptr<int>, 3> &>);
static_assert(std::ranges::input_range<sized_input> && std::ranges::sized_range<sized_input>);
static_assert(!std::copy_constructible<sized_input::iterator>);

using tracked_vector = wjr::vector<insertion_value, insertion_allocator<insertion_value>>;

WJR_NOINLINE void check_failure(unsigned scenario, int point) {
    insertion_state state;
    insertion_value::state = &state;
    {
        std::array<insertion_value, 3> input{1, 2, 3};
        tracked_vector values{insertion_allocator<insertion_value>(state)};
        values.reserve(scenario == 0 || scenario == 4 || scenario == 5 ? 4 : 16);
        for (int value : {10, 20, 30, 40})
            values.emplace_back(value);
        const auto original_blocks = state.allocations - state.releases;
        if (scenario == 0)
            state.moves = point;
        else if (scenario == 1 || scenario == 6)
            state.assignments = point;
        else if (scenario == 2 || scenario == 5)
            state.copies = point;
        else
            state.hooks = point;
        const auto position = scenario == 2 ? 3 : scenario == 6 ? 0 : 1;
        EXPECT_THROW(values.insert_range(values.cbegin() + position, input), std::runtime_error);
        state.copies = state.moves = state.assignments = state.hooks = -1;
        EXPECT_EQ(values.size(), 4);
        EXPECT_EQ(state.live.size(), input.size() + values.size());
        EXPECT_EQ(state.allocations - state.releases, original_blocks);
    }
    EXPECT_TRUE(state.live.empty());
    EXPECT_EQ(state.allocations, state.releases);
    EXPECT_EQ(state.constructed, state.destroyed);
    insertion_value::state = nullptr;
}
} // namespace

TEST(vector_insert_range, positions_empty_and_return_value) {
    for (std::size_t position = 0; position != 4; ++position) {
        wjr::vector<int> values{10, 20, 30};
        std::array input{1, 2};
        auto result = values.insert_range(values.cbegin() + position, std::span(input));
        std::vector<int> expected{10, 20, 30};
        expected.insert(expected.begin() + position, input.begin(), input.end());
        EXPECT_TRUE(std::ranges::equal(values, expected));
        EXPECT_EQ(result, values.begin() + position);
        EXPECT_EQ(*result, 1);
        auto *storage = values.data();
        auto empty = std::span(values.data(), std::size_t(0));
        EXPECT_EQ(values.insert_range(values.cbegin() + position, empty),
                  values.begin() + position);
        EXPECT_EQ(values.data(), storage);
    }
    wjr::vector<int> empty;
    EXPECT_EQ(empty.insert_range(empty.cend(), std::array<int, 0>{}), empty.end());
}

TEST(vector_insert_range, noncommon_sized_input_and_forward_ranges) {
    insertion_state state;
    {
        wjr::vector<int, insertion_allocator<int>> values{insertion_allocator<int>(state)};
        values.append_range(std::array{100, 200});
        const auto before = state.allocations;
        sized_input source{25};
        auto result = values.insert_range(values.cbegin() + 1, source);
        EXPECT_EQ(source.begins, 1);
        EXPECT_EQ(source.position, 25);
        EXPECT_EQ(state.allocations - before, 1);
        EXPECT_EQ(result, values.begin() + 1);
        EXPECT_EQ(values.front(), 100);
        EXPECT_EQ(values.back(), 200);
        EXPECT_TRUE(std::ranges::equal(std::span(values.data() + 1, 25), std::views::iota(0, 25)));
        values.reserve(64);
        auto *storage = values.data();
        std::forward_list<int> list{3, 5, 7};
        auto forward = list | std::views::take_while([](int) { return true; });
        static_assert(std::ranges::forward_range<decltype(forward)>);
        static_assert(!std::ranges::sized_range<decltype(forward)>);
        values.insert_range(values.cbegin() + 2, forward);
        EXPECT_EQ(values.data(), storage);
        EXPECT_TRUE(std::ranges::equal(std::span(values.data() + 2, 3), list));
    }
    EXPECT_EQ(state.allocations, state.releases);
    EXPECT_EQ(state.constructed, state.destroyed);
}

TEST(vector_insert_range, single_pass_streams_and_proxy_values) {
    wjr::vector<int> values{10, 20, 30};
    std::istringstream stream("1 2 3 4");
    auto result = values.insert_range(values.cbegin() + 1, std::ranges::istream_view<int>(stream));
    EXPECT_EQ(result, values.begin() + 1);
    EXPECT_TRUE(std::ranges::equal(values, std::array{10, 1, 2, 3, 4, 20, 30}));
    EXPECT_TRUE(stream.eof());
    std::vector<bool> proxy{true, false, true};
    values.insert_range(values.cend(), proxy);
    EXPECT_TRUE(std::ranges::equal(std::span(values.data() + 7, 3), std::array{1, 0, 1}));
}

TEST(vector_insert_range, move_only_values_and_storage_variants) {
    std::array source{std::make_unique<int>(3), std::make_unique<int>(5)};
    auto moving = std::ranges::subrange(std::make_move_iterator(source.begin()),
                                        std::make_move_iterator(source.end()));
    wjr::small_vector<std::unique_ptr<int>, 2> values;
    values.emplace_back(std::make_unique<int>(1));
    values.emplace_back(std::make_unique<int>(7));
    auto result = values.insert_range(values.cbegin() + 1, moving);
    EXPECT_EQ(result, values.begin() + 1);
    EXPECT_EQ(*values[0], 1);
    EXPECT_EQ(*values[1], 3);
    EXPECT_EQ(*values[2], 5);
    EXPECT_EQ(*values[3], 7);
    EXPECT_FALSE(source[0]);
    EXPECT_FALSE(source[1]);
    wjr::inplace_vector<int, 8> fixed{10, 20};
    auto *storage = fixed.data();
    fixed.insert_range(fixed.cbegin() + 1, std::array{1, 2, 3});
    EXPECT_EQ(fixed.data(), storage);
    EXPECT_TRUE(std::ranges::equal(fixed, std::array{10, 1, 2, 3, 20}));

    std::istringstream stream("11 13");
    auto single_pass =
        std::ranges::istream_view<int>(stream) |
        std::views::transform([](int value) { return std::make_unique<int>(value); });
    values.insert_range(values.cbegin() + 2, single_pass);
    EXPECT_EQ(values.size(), 6);
    EXPECT_EQ(*values[2], 11);
    EXPECT_EQ(*values[3], 13);
    EXPECT_EQ(*values[4], 5);
}

TEST(vector_insert_range, failures_keep_all_lifetimes_and_allocations_owned) {
    for (int point = 0; point != 4; ++point)
        check_failure(0, point);
    for (int point = 0; point != 3; ++point)
        check_failure(1, point);
    for (int point = 0; point != 2; ++point)
        check_failure(2, point);
    for (int point = 0; point != 3; ++point)
        check_failure(3, point);
    for (int point = 0; point != 7; ++point)
        check_failure(4, point);
    for (int point = 0; point != 3; ++point)
        check_failure(5, point);
    for (int point = 0; point != 4; ++point)
        check_failure(6, point);
}

TEST(vector_insert_range, rotation_failure_keeps_every_completed_element_owned) {
    insertion_state state;
    insertion_value::state = &state;
    {
        tracked_vector values{insertion_allocator<insertion_value>(state)};
        values.emplace_back(10);
        values.emplace_back(20);
        std::istringstream stream("1 2 3");
        auto input = std::ranges::istream_view<int>(stream) | std::views::transform([&](int value) {
                         if (value == 3)
                             state.assignments = 0;
                         return insertion_value(value);
                     });
        EXPECT_THROW(values.insert_range(values.cbegin() + 1, input), std::runtime_error);
        state.assignments = -1;
        EXPECT_EQ(values.size(), 5);
        EXPECT_EQ(state.live.size(), values.size());
    }
    EXPECT_TRUE(state.live.empty());
    EXPECT_EQ(state.allocations, state.releases);
    EXPECT_EQ(state.constructed, state.destroyed);
    insertion_value::state = nullptr;
}

TEST(vector_insert_range, source_failure_retains_owned_appended_prefix) {
    wjr::vector<int> values{10, 20};
    std::istringstream stream("1 2 3 4");
    auto input = std::ranges::istream_view<int>(stream) | std::views::transform([](int value) {
                     if (value == 3)
                         throw std::runtime_error("stream");
                     return value;
                 });
    EXPECT_THROW(values.insert_range(values.cbegin() + 1, input), std::runtime_error);
    EXPECT_TRUE(std::ranges::equal(values, std::array{10, 20, 1, 2}));
}

TEST(vector_insert_range, allocation_failure_and_oversized_input) {
    insertion_state state;
    wjr::vector<int, insertion_allocator<int>> values{insertion_allocator<int>(state)};
    values.append_range(std::array{10, 20});
    auto *storage = values.data();
    state.fail_allocation = true;
    sized_input source{100};
    EXPECT_THROW(values.insert_range(values.cbegin() + 1, source), std::bad_alloc);
    EXPECT_EQ(source.position, 0);
    EXPECT_EQ(values.data(), storage);
    EXPECT_TRUE(std::ranges::equal(values, std::array{10, 20}));
    sized_input huge{std::numeric_limits<std::size_t>::max()};
    EXPECT_THROW(values.insert_range(values.cbegin(), huge), std::length_error);
    EXPECT_EQ(huge.begins, 0);
    EXPECT_EQ(values.data(), storage);
}

TEST(vector_insert_range, throwing_increment_rolls_back_constructed_input) {
    insertion_state state;
    {
        wjr::vector<int, insertion_allocator<int>> values{insertion_allocator<int>(state)};
        values.append_range(std::array{10, 20});
        auto *storage = values.data();
        sized_input input{5};
        input.increment_failure = 2;
        EXPECT_THROW(values.insert_range(values.cbegin() + 1, input), std::runtime_error);
        EXPECT_EQ(input.position, 2);
        EXPECT_EQ(values.data(), storage);
        EXPECT_TRUE(std::ranges::equal(values, std::array{10, 20}));
        EXPECT_EQ(state.constructed - state.destroyed, 2);
        EXPECT_EQ(state.allocations - state.releases, 1);
    }
    EXPECT_EQ(state.constructed, state.destroyed);
    EXPECT_EQ(state.allocations, state.releases);
}

TEST(vector_insert_range, count_is_checked_before_narrowing_storage_size) {
    insertion_state state;
    wjr::vector<int, narrow_allocator<int>> values{narrow_allocator<int>(state)};
    values.append_range(std::array{10, 20});
    const auto allocations = state.allocations;
    sized_input source{300};
    EXPECT_THROW(values.insert_range(values.cbegin() + 1, source), std::length_error);
    EXPECT_EQ(source.begins, 0);
    EXPECT_EQ(state.allocations, allocations);
    EXPECT_TRUE(std::ranges::equal(values, std::array{10, 20}));
}

namespace {

enum class replacement_failure { allocation, copy, move, assignment, hook };

WJR_NOINLINE void check_replacement_failure(bool grow, replacement_failure failure, int point) {
    insertion_state state;
    insertion_value::state = &state;
    {
        std::array<insertion_value, 3> input{10, 11, 12};
        tracked_vector values{insertion_allocator<insertion_value>(state)};
        values.reserve(grow ? 4 : 16);
        for (int value : {0, 1, 2, 3})
            values.emplace_back(value);
        auto *const storage = values.data();
        const auto capacity = values.capacity();
        state.fail_allocation = failure == replacement_failure::allocation;
        state.copies = failure == replacement_failure::copy ? point : -1;
        state.moves = failure == replacement_failure::move ? point : -1;
        state.assignments = failure == replacement_failure::assignment ? point : -1;
        state.hooks = failure == replacement_failure::hook ? point : -1;
        if (state.fail_allocation) {
            EXPECT_THROW(values.replace_range(values.cbegin() + 1, values.cbegin() + 2, input),
                         std::bad_alloc);
        } else {
            EXPECT_THROW(values.replace_range(values.cbegin() + 1, values.cbegin() + 2, input),
                         std::runtime_error);
        }
        state.fail_allocation = false;
        state.copies = state.moves = state.assignments = state.hooks = -1;
        EXPECT_EQ(values.data(), storage);
        EXPECT_EQ(values.capacity(), capacity);
        EXPECT_EQ(values.size(), 4);
        EXPECT_EQ(state.live.size(), 7);
        EXPECT_EQ(state.constructed - state.destroyed, 4);
        EXPECT_EQ(state.allocations - state.releases, 1);
        std::vector<int> expected;
        for (const auto &value : values)
            expected.push_back(value.value);
        expected.erase(expected.begin() + 1);
        expected.insert(expected.begin() + 1, {10, 11, 12});
        values.replace_range(values.cbegin() + 1, values.cbegin() + 2, input);
        ASSERT_EQ(values.size(), expected.size());
        for (std::size_t i = 0; i != expected.size(); ++i)
            EXPECT_EQ(values[i].value, expected[i]);
    }
    EXPECT_TRUE(state.live.empty());
    EXPECT_EQ(state.constructed, state.destroyed);
    EXPECT_EQ(state.allocations, state.releases);
}

struct unsized_input {
    sized_input input;
    auto begin() { return input.begin(); }
    auto end() const { return input.end(); }
};

std::istream &operator>>(std::istream &stream, insertion_value &value) {
    return stream >> value.value;
}

} // namespace

TEST(vector_replace_range, construction_assignment_and_allocation_failures_keep_ownership) {
    check_replacement_failure(true, replacement_failure::allocation, 0);
    for (int point = 0; point != 2; ++point) {
        check_replacement_failure(true, replacement_failure::copy, point);
        check_replacement_failure(false, replacement_failure::move, point);
        check_replacement_failure(false, replacement_failure::hook, point);
    }
    for (int point = 0; point != 4; ++point)
        check_replacement_failure(true, replacement_failure::move, point);
    for (int point = 0; point != 6; ++point)
        check_replacement_failure(true, replacement_failure::hook, point);
    for (int point = 0; point != 3; ++point)
        check_replacement_failure(false, replacement_failure::assignment, point);
}

TEST(vector_replace_range, sized_move_only_input_is_not_preconsumed) {
    insertion_state state;
    {
        wjr::vector<int, insertion_allocator<int>> values{insertion_allocator<int>(state)};
        values.append_range(std::array{10, 20, 30});
        const auto allocations = state.allocations;
        sized_input source{5};
        values.replace_range(values.cbegin() + 1, values.cbegin() + 2, source);
        EXPECT_EQ(source.begins, 1);
        EXPECT_EQ(source.position, 5);
        EXPECT_EQ(state.allocations, allocations + 1);
        EXPECT_TRUE(std::ranges::equal(values, std::array{10, 0, 1, 2, 3, 4, 30}));
        values.reserve(32);
        const auto reuse_allocations = state.allocations;
        unsized_input stream{{5}};
        values.replace_range(values.cbegin() + 1, values.cbegin() + 2, stream);
        EXPECT_EQ(stream.input.position, 5);
        EXPECT_EQ(stream.input.begins, 1);
        EXPECT_EQ(state.allocations, reuse_allocations);
    }
    EXPECT_EQ(state.constructed, state.destroyed);
    EXPECT_EQ(state.allocations, state.releases);
}

TEST(vector_replace_range, source_increment_failure_cleans_uncommitted_tail) {
    for (std::size_t point = 0; point != 5; ++point) {
        insertion_state state;
        {
            wjr::vector<int, insertion_allocator<int>> values{insertion_allocator<int>(state)};
            values.append_range(std::array{10, 20, 30});
            auto *const storage = values.data();
            sized_input input{5};
            input.increment_failure = point;
            EXPECT_THROW(values.replace_range(values.cbegin() + 1, values.cbegin() + 2, input),
                         std::runtime_error);
            EXPECT_EQ(values.size(), 3);
            EXPECT_EQ(values.data(), storage);
            EXPECT_EQ(state.constructed - state.destroyed, 3);
            EXPECT_EQ(state.allocations - state.releases, 1);
        }
        EXPECT_EQ(state.constructed, state.destroyed);
        EXPECT_EQ(state.allocations, state.releases);
    }
}

TEST(vector_replace_range, throwing_input_rotation_keeps_completed_tail_owned) {
    insertion_state state;
    insertion_value::state = &state;
    {
        tracked_vector values{insertion_allocator<insertion_value>(state)};
        values.reserve(16);
        for (int value : {0, 1, 2, 3})
            values.emplace_back(value);
        std::istringstream stream("10 11 12");
        auto input = std::ranges::istream_view<insertion_value>(stream);
        state.assignments = 1;
        EXPECT_THROW(values.replace_range(values.cbegin() + 1, values.cbegin() + 2, input),
                     std::runtime_error);
        state.assignments = -1;
        EXPECT_EQ(values.size(), 6);
        EXPECT_EQ(state.live.size(), values.size() + 1);
    }
    EXPECT_TRUE(state.live.empty());
    EXPECT_EQ(state.constructed, state.destroyed);
    EXPECT_EQ(state.allocations, state.releases);
}

TEST(vector_replace_range, bounded_size_checks_and_growth_do_not_wrap) {
    insertion_state state;
    using vector = wjr::vector<int, narrow_allocator<int>>;
    static_assert(vector::get_growth_capacity(128, 129) == 255);
    {
        vector values{narrow_allocator<int>(state)};
        values.append_range(std::array{10, 20});
        const auto allocations = state.allocations;
        sized_input too_large{300};
        EXPECT_THROW(values.replace_range(values.cbegin(), values.cbegin() + 1, too_large),
                     std::length_error);
        EXPECT_EQ(too_large.begins, 0);
        EXPECT_EQ(state.allocations, allocations);
        unsized_input stream{{256}};
        EXPECT_THROW(values.replace_range(values.cbegin(), values.cbegin(), stream),
                     std::length_error);
        EXPECT_EQ(values.size(), 255);
        EXPECT_EQ(stream.input.position, 253);
        EXPECT_EQ(state.constructed - state.destroyed, 255);
    }
    EXPECT_EQ(state.constructed, state.destroyed);
    EXPECT_EQ(state.allocations, state.releases);
}
