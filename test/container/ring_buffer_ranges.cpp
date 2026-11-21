#include <array>
#include <limits>
#include <memory>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/ring_buffer.hpp>

namespace {

struct allocation_counts {
    size_t allocations = 0;
    size_t deallocations = 0;
};

template <typename T>
struct range_allocator {
    using value_type = T;
    allocation_counts *counts;

    explicit range_allocator(allocation_counts &state) noexcept : counts(&state) {}
    template <typename U>
    range_allocator(const range_allocator<U> &other) noexcept : counts(other.counts) {}
    T *allocate(size_t n) {
        auto *result = std::allocator<T>{}.allocate(n);
        ++counts->allocations;
        return result;
    }
    void deallocate(T *ptr, size_t n) noexcept {
        ++counts->deallocations;
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U>
    bool operator==(const range_allocator<U> &other) const noexcept {
        return counts == other.counts;
    }
};

struct range_value {
    inline static int live = 0;
    int value;
    explicit range_value(int n) : value(n) { ++live; }
    range_value(const range_value &other) : value(other.value) { ++live; }
    range_value(range_value &&other) noexcept : value(other.value) { ++live; }
    range_value &operator=(const range_value &) = default;
    range_value &operator=(range_value &&) = default;
    ~range_value() { --live; }
};

using queue = wjr::ring_buffer<int>;
static_assert(std::ranges::random_access_range<queue>);
static_assert(std::ranges::random_access_range<const queue>);
static_assert(std::sortable<queue::iterator>);
static_assert(std::is_trivially_copyable_v<queue::iterator>);
static_assert(sizeof(queue::iterator) == 2 * sizeof(void *));
static_assert(!std::indirectly_writable<queue::const_iterator, int>);
static_assert(std::constructible_from<queue, wjr::from_range_t, std::array<int, 3> &>);
static_assert(!std::constructible_from<queue, wjr::from_range_t, std::array<void *, 3> &>);

TEST(ring_buffer_ranges, sized_noncommon_construction_allocates_once) {
    int values[] = {3, 4, 5, 6, 7};
    auto range = std::ranges::subrange(std::counted_iterator(values, 5), std::default_sentinel);
    static_assert(!std::ranges::common_range<decltype(range)>);
    allocation_counts counts;
    {
        wjr::ring_buffer<int, range_allocator<int>> result(wjr::from_range, range,
                                                           range_allocator<int>(counts));
        EXPECT_TRUE(std::ranges::equal(result, values));
        EXPECT_EQ(counts.allocations, 1);
        EXPECT_EQ(result.get_allocator().counts, &counts);
    }
    EXPECT_EQ(counts.allocations, counts.deallocations);
}

TEST(ring_buffer_ranges, single_pass_stream_and_move_only_values) {
    std::istringstream input("2 3 5 7 11");
    auto stream = std::ranges::istream_view<int>(input);
    queue result(wjr::from_range, stream);
    EXPECT_TRUE(std::ranges::equal(result, std::array{2, 3, 5, 7, 11}));
    EXPECT_TRUE(input.eof());

    std::array<std::unique_ptr<int>, 3> source;
    for (size_t i = 0; i < source.size(); ++i) {
        source[i] = std::make_unique<int>(static_cast<int>(i + 10));
    }
    auto moving = std::ranges::subrange(std::make_move_iterator(source.begin()),
                                        std::make_move_iterator(source.end()));
    wjr::ring_buffer<std::unique_ptr<int>> moved(wjr::from_range, moving);
    EXPECT_EQ(*moved[0], 10);
    EXPECT_EQ(*moved[2], 12);
    EXPECT_TRUE(std::ranges::all_of(source, [](const auto &value) { return !value; }));
}

TEST(ring_buffer_ranges, append_wrap_and_assign_reuse_storage) {
    queue result;
    result.reserve(8);
    result.append_range(std::views::iota(0, 6));
    for (int i = 0; i < 4; ++i) {
        result.pop_front();
    }
    auto *storage = result.data();
    result.append_range(std::array{6, 7, 8, 9, 10});
    EXPECT_EQ(result.data(), storage);
    EXPECT_TRUE(std::ranges::equal(result, std::array{4, 5, 6, 7, 8, 9, 10}));
    EXPECT_FALSE(result.array_two().empty());

    int values[] = {40, 41, 42};
    auto range = std::ranges::subrange(std::counted_iterator(values, 3), std::default_sentinel);
    result.assign_range(range);
    EXPECT_EQ(result.data(), storage);
    EXPECT_TRUE(std::ranges::equal(result, values));
    std::istringstream input("1 2 3 4 5 6 7 8 9");
    result.assign_range(std::ranges::istream_view<int>(input));
    EXPECT_TRUE(std::ranges::equal(result, std::views::iota(1, 10)));
    result.assign_range(std::array<int, 0>{});
    EXPECT_TRUE(result.empty());
}

TEST(ring_buffer_ranges, pointer_append_at_and_across_physical_end) {
    queue result(8, wjr::in_place_reserve);
    result.append_range(std::array{0, 1, 2, 3, 4, 5});
    for (int i = 0; i < 3; ++i) {
        result.pop_front();
    }

    result.append_range(std::array{6, 7});
    EXPECT_EQ(result.array_two().size(), 0);
    result.append_range(std::array{8, 9, 10});
    EXPECT_TRUE(result.full());
    EXPECT_TRUE(std::ranges::equal(result, std::views::iota(3, 11)));

    result.pop_front();
    result.pop_back();
    EXPECT_EQ(result.front(), 4);
    EXPECT_EQ(result.back(), 9);
    result.append_range(std::array<int, 0>{});
    EXPECT_EQ(result.size(), 6);

    result.append_range(std::array{10, 11, 12, 13});
    EXPECT_TRUE(std::ranges::equal(result, std::views::iota(4, 14)));
}

template <typename T>
struct throwing_int_allocator : range_allocator<T> {
    using range_allocator<T>::range_allocator;
    template <typename U>
    throwing_int_allocator(const throwing_int_allocator<U> &other)
        : range_allocator<T>(*other.counts) {}

    void construct(T *ptr, const T &value) {
        if (value == 3) {
            throw std::runtime_error("allocator construction failed");
        }
        std::construct_at(ptr, value);
    }
};

TEST(ring_buffer_ranges, pointer_append_preserves_allocator_hooks_and_prefix) {
    allocation_counts counts;
    using custom_queue = wjr::ring_buffer<int, throwing_int_allocator<int>>;
    custom_queue values{throwing_int_allocator<int>(counts)};
    const int input[] = {1, 2, 3, 4};
    EXPECT_THROW(values.append_range(input), std::runtime_error);
    EXPECT_TRUE(std::ranges::equal(values, std::array{1, 2}));
}

TEST(ring_buffer_ranges, partial_append_and_failed_construction_cleanup) {
    for (bool single_pass : {false, true}) {
        allocation_counts counts;
        auto convert = [](int value) -> range_value {
            if (value == 3) {
                throw std::runtime_error("source failed");
            }
            return range_value(value);
        };
        {
            using owned = wjr::ring_buffer<range_value, range_allocator<range_value>>;
            if (single_pass) {
                std::istringstream input("0 1 2 3 4");
                auto range = std::ranges::istream_view<int>(input) | std::views::transform(convert);
                EXPECT_THROW((owned(wjr::from_range, range, range_allocator<range_value>(counts))),
                             std::runtime_error);
            } else {
                auto range = std::views::iota(0, 5) | std::views::transform(convert);
                EXPECT_THROW((owned(wjr::from_range, range, range_allocator<range_value>(counts))),
                             std::runtime_error);
            }
        }
        EXPECT_EQ(range_value::live, 0);
        EXPECT_EQ(counts.allocations, counts.deallocations);
    }

    queue result;
    result.push_back(20);
    auto throwing = std::views::iota(0, 5) | std::views::transform([](int value) {
                        if (value == 3) {
                            throw std::runtime_error("append failed");
                        }
                        return value;
                    });
    EXPECT_THROW(result.append_range(throwing), std::runtime_error);
    EXPECT_TRUE(std::ranges::equal(result, std::array{20, 0, 1, 2}));
}

TEST(ring_buffer_ranges, sized_append_rejects_overflow_before_consuming_input) {
    wjr::ring_buffer<size_t> result;
    result.push_back(7);
    auto huge = std::views::iota(size_t(0), std::numeric_limits<size_t>::max());
    EXPECT_THROW(result.append_range(huge), std::length_error);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.front(), 7);
}

TEST(ring_buffer_ranges, random_access_ordering_and_algorithms_across_wrap) {
    queue::iterator singular{};
    queue::iterator other{};
    EXPECT_EQ(singular, other);
    queue result;
    EXPECT_EQ(result.end() - result.begin(), 0);
    EXPECT_EQ(result.begin() <=> result.end(), std::strong_ordering::equal);
    result.reserve(8);
    EXPECT_EQ(result.end() - result.begin(), 0);
    EXPECT_EQ(result.begin() <=> result.end(), std::strong_ordering::equal);
    result.append_range(std::array{0, 1, 2, 3, 8, 6});
    for (int i = 0; i < 4; ++i) {
        result.pop_front();
    }
    result.append_range(std::array{4, 2, 7, 5, 3});
    ASSERT_FALSE(result.array_two().empty());
    for (size_t i = 0; i <= result.size(); ++i) {
        auto position = result.begin() + static_cast<ptrdiff_t>(i);
        EXPECT_EQ(position - result.begin(), static_cast<ptrdiff_t>(i));
        EXPECT_EQ(position == result.end(), i == result.size());
        EXPECT_EQ(position < result.end(), i < result.size());
        EXPECT_EQ(result.cbegin() <= position, true);
    }
    std::ranges::sort(result);
    EXPECT_TRUE(std::ranges::equal(result, std::array{2, 3, 4, 5, 6, 7, 8}));
    std::ranges::reverse(result);
    EXPECT_TRUE(std::ranges::equal(result, std::array{8, 7, 6, 5, 4, 3, 2}));
}

} // namespace
