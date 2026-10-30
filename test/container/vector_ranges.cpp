#include <array>
#include <forward_list>
#include <limits>
#include <memory>
#include <ranges>
#include <sstream>
#include <stdexcept>

#include <gtest/gtest.h>

#include <wjr/vector.hpp>

namespace {

struct range_allocations {
    size_t allocated = 0;
    size_t released = 0;
    bool fail = false;
};

template <typename T>
struct modifier_allocator {
    using value_type = T;
    range_allocations *state;
    explicit modifier_allocator(range_allocations &counts) noexcept : state(&counts) {}
    template <typename U>
    modifier_allocator(const modifier_allocator<U> &other) noexcept : state(other.state) {}
    T *allocate(size_t count) {
        if (state->fail) {
            throw std::bad_alloc();
        }
        auto *result = std::allocator<T>{}.allocate(count);
        ++state->allocated;
        return result;
    }
    void deallocate(T *ptr, size_t count) noexcept {
        ++state->released;
        std::allocator<T>{}.deallocate(ptr, count);
    }
    template <typename U>
    bool operator==(const modifier_allocator<U> &other) const noexcept {
        return state == other.state;
    }
};

struct sized_input {
    size_t count;
    size_t position = 0;
    size_t begins = 0;
    struct iterator {
        using iterator_concept = std::input_iterator_tag;
        using value_type = int;
        using difference_type = ptrdiff_t;
        sized_input *owner;
        iterator(const iterator &) = delete;
        iterator(iterator &&) = default;
        iterator &operator=(iterator &&) = default;
        explicit iterator(sized_input &input) noexcept : owner(&input) {}
        int operator*() const noexcept { return static_cast<int>(owner->position); }
        iterator &operator++() noexcept {
            ++owner->position;
            return *this;
        }
        void operator++(int) noexcept { ++*this; }
        bool operator==(std::default_sentinel_t) const noexcept {
            return owner->position == owner->count;
        }
    };
    iterator begin() noexcept {
        ++begins;
        return iterator(*this);
    }
    std::default_sentinel_t end() const noexcept { return {}; }
    size_t size() const noexcept { return count; }
};

struct modifier_value {
    inline static int live = 0;
    int value;
    modifier_value(int n) : value(n) { ++live; }
    modifier_value(const modifier_value &other) : value(other.value) { ++live; }
    modifier_value(modifier_value &&other) noexcept : value(other.value) { ++live; }
    modifier_value &operator=(const modifier_value &) = default;
    modifier_value &operator=(modifier_value &&) = default;
    ~modifier_value() { --live; }
};

template <typename Range>
concept vector_assignable_range = requires(wjr::vector<int> &values, Range &&source) {
    values.assign_range(std::forward<Range>(source));
};

static_assert(std::ranges::input_range<sized_input>);
static_assert(std::ranges::sized_range<sized_input>);
static_assert(!std::ranges::forward_range<sized_input>);
static_assert(!wjr::is_iterator_v<sized_input::iterator>);
static_assert(vector_assignable_range<std::array<int, 3> &>);
static_assert(!vector_assignable_range<std::array<void *, 3> &>);

TEST(vector_ranges, counted_append_and_assignment_reuse_allocator_and_capacity) {
    range_allocations counts;
    {
        wjr::vector<int, modifier_allocator<int>> values{modifier_allocator<int>(counts)};
        int input[] = {2, 3, 5, 7, 11};
        auto range = std::ranges::subrange(std::counted_iterator(input, 5), std::default_sentinel);
        values.append_range(range);
        EXPECT_TRUE(std::ranges::equal(values, input));
        EXPECT_EQ(counts.allocated, 1);
        EXPECT_EQ(values.get_allocator().state, &counts);
        auto *storage = values.data();
        auto smaller =
            std::ranges::subrange(std::counted_iterator(input, 3), std::default_sentinel);
        values.assign_range(smaller);
        EXPECT_EQ(values.data(), storage);
        EXPECT_TRUE(std::ranges::equal(values, std::array{2, 3, 5}));
        EXPECT_EQ(counts.allocated, 1);
        values.assign_range(std::array<int, 0>{});
        EXPECT_TRUE(values.empty());
        EXPECT_EQ(values.data(), storage);
    }
    EXPECT_EQ(counts.allocated, counts.released);
}

TEST(vector_ranges, sized_input_is_consumed_once_with_one_allocation) {
    range_allocations counts;
    {
        wjr::vector<int, modifier_allocator<int>> values{modifier_allocator<int>(counts)};
        sized_input first{25};
        values.append_range(first);
        EXPECT_EQ(first.begins, 1);
        EXPECT_EQ(first.position, 25);
        EXPECT_EQ(counts.allocated, 1);
        EXPECT_TRUE(std::ranges::equal(values, std::views::iota(0, 25)));
        sized_input second{100};
        values.assign_range(second);
        EXPECT_EQ(second.begins, 1);
        EXPECT_EQ(second.position, 100);
        EXPECT_EQ(counts.allocated, 2);
        EXPECT_TRUE(std::ranges::equal(values, std::views::iota(0, 100)));
    }
    EXPECT_EQ(counts.allocated, counts.released);
}

TEST(vector_ranges, forward_and_stream_sources) {
    range_allocations counts;
    wjr::vector<int, modifier_allocator<int>> values{modifier_allocator<int>(counts)};
    std::forward_list<int> input{2, 3, 5, 7, 11};
    values.append_range(input);
    EXPECT_EQ(counts.allocated, 1);
    EXPECT_TRUE(std::ranges::equal(values, input));
    std::istringstream replacement("13 17 19");
    values.assign_range(std::ranges::istream_view<int>(replacement));
    EXPECT_TRUE(std::ranges::equal(values, std::array{13, 17, 19}));
    EXPECT_TRUE(replacement.eof());
    std::istringstream suffix("23 29 31");
    values.append_range(std::ranges::istream_view<int>(suffix));
    EXPECT_TRUE(std::ranges::equal(values, std::array{13, 17, 19, 23, 29, 31}));
}

TEST(vector_ranges, moving_ranges_and_small_storage) {
    auto make_source = [] {
        return std::array{std::make_unique<int>(3), std::make_unique<int>(7)};
    };
    auto source = make_source();
    auto moving = std::ranges::subrange(std::make_move_iterator(source.begin()),
                                        std::make_move_iterator(source.end()));
    wjr::small_vector<std::unique_ptr<int>, 4> values;
    values.append_range(moving);
    EXPECT_EQ(*values[0], 3);
    EXPECT_EQ(*values[1], 7);
    EXPECT_TRUE(std::ranges::all_of(source, [](const auto &ptr) { return !ptr; }));
    auto replacement = make_source();
    auto assigning = std::ranges::subrange(std::make_move_iterator(replacement.begin()),
                                           std::make_move_iterator(replacement.end()));
    values.assign_range(assigning);
    EXPECT_EQ(*values[0], 3);
    EXPECT_EQ(*values[1], 7);
    EXPECT_TRUE(std::ranges::all_of(replacement, [](const auto &ptr) { return !ptr; }));
    wjr::small_vector<int, 4> small;
    small.append_range(std::views::iota(0, 12));
    EXPECT_TRUE(std::ranges::equal(small, std::views::iota(0, 12)));
}

TEST(vector_ranges, throwing_bulk_source_rolls_back_new_lifetimes) {
    EXPECT_EQ(modifier_value::live, 0);
    range_allocations counts;
    {
        wjr::vector<modifier_value, modifier_allocator<modifier_value>> values{
            modifier_allocator<modifier_value>(counts)};
        values.emplace_back(42);
        auto source = std::views::iota(0, 6) | std::views::transform([](int n) -> modifier_value {
                          if (n == 3) {
                              throw std::runtime_error("source failed");
                          }
                          return n;
                      });
        EXPECT_THROW(values.append_range(source), std::runtime_error);
        EXPECT_EQ(values.size(), 1);
        EXPECT_EQ(values.front().value, 42);
        EXPECT_EQ(modifier_value::live, 1);
        EXPECT_THROW(values.assign_range(source), std::runtime_error);
        EXPECT_EQ(modifier_value::live, static_cast<int>(values.size()));
    }
    EXPECT_EQ(modifier_value::live, 0);
    EXPECT_EQ(counts.allocated, counts.released);
}

TEST(vector_ranges, unsized_failure_retains_owned_prefix) {
    wjr::vector<int> values{42};
    std::istringstream input("0 1 2 3 4");
    auto source = std::ranges::istream_view<int>(input) | std::views::transform([](int n) {
                      if (n == 3) {
                          throw std::runtime_error("stream failed");
                      }
                      return n;
                  });
    EXPECT_THROW(values.append_range(source), std::runtime_error);
    EXPECT_TRUE(std::ranges::equal(values, std::array{42, 0, 1, 2}));
}

TEST(vector_ranges, allocation_failure_and_size_overflow_preserve_values) {
    range_allocations counts;
    wjr::vector<int, modifier_allocator<int>> values{modifier_allocator<int>(counts)};
    values.push_back(42);
    counts.fail = true;
    sized_input source{100};
    EXPECT_THROW(values.assign_range(source), std::bad_alloc);
    EXPECT_EQ(source.begins, 0);
    EXPECT_TRUE(std::ranges::equal(values, std::array{42}));
    auto huge = std::views::iota(size_t(0), std::numeric_limits<size_t>::max()) |
                std::views::transform([](size_t n) { return static_cast<int>(n); });
    EXPECT_THROW(values.append_range(huge), std::length_error);
    EXPECT_TRUE(std::ranges::equal(values, std::array{42}));
}

} // namespace
