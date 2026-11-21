#include <memory>
#include <ranges>
#include <set>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/ring_buffer.hpp>

namespace {
struct assignment_counts {
    std::set<void *> live;
    int allocations = 0;
    int deallocations = 0;
    int constructed = 0;
    int destroyed = 0;
    int invalid_destroys = 0;
    int copy_budget = -1;
    int move_budget = -1;
    int hook_budget = -1;
};

struct assignment_element {
    assignment_counts *owner;
    int number;
    assignment_element(assignment_counts &counts, int n) noexcept : owner(&counts), number(n) {
        EXPECT_TRUE(owner->live.insert(this).second);
    }
    assignment_element(const assignment_element &other) : owner(other.owner), number(other.number) {
        if (owner->copy_budget == 0)
            throw std::runtime_error("copy failed");
        if (owner->copy_budget > 0)
            --owner->copy_budget;
        EXPECT_TRUE(owner->live.insert(this).second);
    }
    assignment_element(assignment_element &&other) : owner(other.owner), number(other.number) {
        if (owner->move_budget == 0)
            throw std::runtime_error("move failed");
        if (owner->move_budget > 0)
            --owner->move_budget;
        EXPECT_TRUE(owner->live.insert(this).second);
        other.number = -1;
    }
    assignment_element &operator=(const assignment_element &) = default;
    assignment_element &operator=(assignment_element &&) = default;
    ~assignment_element() {
        if (owner->live.erase(this) != 1)
            ++owner->invalid_destroys;
    }
};

template <typename T>
struct assignment_allocator {
    using value_type = T;
    assignment_counts *owner;
    int domain;
    explicit assignment_allocator(assignment_counts &counts, int id = 0) noexcept
        : owner(&counts), domain(id) {}
    template <typename U>
    assignment_allocator(const assignment_allocator<U> &other) noexcept
        : owner(other.owner), domain(other.domain) {}
    T *allocate(std::size_t n) {
        auto *ptr = std::allocator<T>{}.allocate(n);
        ++owner->allocations;
        return ptr;
    }
    void deallocate(T *ptr, std::size_t n) noexcept {
        ++owner->deallocations;
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        if (owner->hook_budget == 0)
            throw std::runtime_error("hook failed");
        if (owner->hook_budget > 0)
            --owner->hook_budget;
        std::construct_at(ptr, std::forward<Args>(args)...);
        ++owner->constructed;
    }
    template <typename U>
    void destroy(U *ptr) noexcept {
        ++owner->destroyed;
        std::destroy_at(ptr);
    }
    bool operator==(const assignment_allocator &) const = default;
};

template <typename T>
struct plain_assignment_allocator {
    using value_type = T;
    assignment_counts *owner;
    explicit plain_assignment_allocator(assignment_counts &counts) noexcept : owner(&counts) {}
    template <typename U>
    plain_assignment_allocator(const plain_assignment_allocator<U> &other) noexcept
        : owner(other.owner) {}
    T *allocate(std::size_t n) { return assignment_allocator<T>(*owner).allocate(n); }
    void deallocate(T *ptr, std::size_t n) noexcept {
        assignment_allocator<T>(*owner).deallocate(ptr, n);
    }
    bool operator==(const plain_assignment_allocator &) const = default;
};

template <typename T>
struct counted_forward_iterator {
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    const T *ptr = nullptr;
    int *steps = nullptr;
    int throw_at = -1;

    const T &operator*() const { return *ptr; }
    counted_forward_iterator &operator++() {
        if (++*steps == throw_at) {
            throw std::runtime_error("increment failed");
        }
        ++ptr;
        return *this;
    }
    counted_forward_iterator operator++(int) {
        auto copy = *this;
        ++*this;
        return copy;
    }
    bool operator==(const counted_forward_iterator &) const = default;
};

static_assert(std::forward_iterator<counted_forward_iterator<int>>);

using allocator = assignment_allocator<assignment_element>;
using ring = wjr::ring_buffer<assignment_element, allocator>;

template <typename Ring>
void populate(Ring &destination, assignment_counts &counts, int head, int size) {
    for (int i = 0; i != head; ++i)
        destination.emplace_back(counts, -1);
    for (int i = 0; i != head; ++i)
        destination.pop_front();
    for (int i = 0; i != size; ++i)
        destination.emplace_back(counts, i);
}

void expect_released(const assignment_counts &counts) {
    EXPECT_TRUE(counts.live.empty());
    EXPECT_EQ(counts.allocations, counts.deallocations);
    EXPECT_EQ(counts.constructed, counts.destroyed);
    EXPECT_EQ(counts.invalid_destroys, 0);
}

TEST(ring_assignment, growth_releases_new_allocation_after_copy_or_hook_failure) {
    for (bool fail_hook : {false, true}) {
        for (int budget = 0; budget != 6; ++budget) {
            assignment_counts counts;
            {
                ring destination(4, wjr::in_place_reserve, allocator(counts));
                populate(destination, counts, 0, 2);
                std::vector<assignment_element> source;
                source.reserve(6);
                for (int i = 0; i != 6; ++i)
                    source.emplace_back(counts, 100 + i);
                auto *const original_data = destination.data();
                const auto original_live = counts.live;
                const auto original_blocks = counts.allocations - counts.deallocations;
                (fail_hook ? counts.hook_budget : counts.copy_budget) = budget;
                EXPECT_THROW(destination.assign(source.data(), source.data() + source.size()),
                             std::runtime_error);
                EXPECT_EQ(destination.data(), original_data);
                EXPECT_EQ(destination.size(), 2);
                EXPECT_EQ(destination.capacity(), 4);
                EXPECT_EQ(counts.live, original_live);
                EXPECT_EQ(counts.allocations - counts.deallocations, original_blocks);
                counts.hook_budget = counts.copy_budget = -1;
                destination.assign_range(std::span(source));
                EXPECT_EQ(destination.size(), 6);
                EXPECT_EQ(destination.front().number, 100);
                EXPECT_EQ(destination.back().number, 105);
            }
            expect_released(counts);
        }
    }
}

template <typename Ring>
void check_tail_rollback(int head, int old_size, int new_size, bool fail_hook) {
    for (int budget = 0; budget != new_size - old_size; ++budget) {
        SCOPED_TRACE(testing::Message()
                     << head << ',' << old_size << ',' << new_size << ',' << budget);
        assignment_counts counts;
        {
            Ring destination(8, wjr::in_place_reserve, allocator(counts));
            populate(destination, counts, head, old_size);
            std::vector<assignment_element> source;
            source.reserve(new_size);
            for (int i = 0; i != new_size; ++i)
                source.emplace_back(counts, 100 + i);
            auto *const original_data = destination.data();
            const auto original_live = counts.live;
            (fail_hook ? counts.hook_budget : counts.copy_budget) = budget;
            EXPECT_THROW(destination.assign_range(std::span(source)), std::runtime_error);
            EXPECT_EQ(destination.data(), original_data);
            EXPECT_EQ(destination.size(), old_size);
            EXPECT_EQ(counts.live, original_live);
            EXPECT_EQ(counts.invalid_destroys, 0);
            counts.hook_budget = counts.copy_budget = -1;
            destination.assign(source.data(), source.data() + source.size());
            EXPECT_EQ(destination.size(), new_size);
            for (int i = 0; i != new_size; ++i)
                EXPECT_EQ(destination[i].number, 100 + i);
        }
        expect_released(counts);
    }
}

TEST(ring_assignment, in_capacity_growth_rolls_back_all_unpublished_tail_segments) {
    using fixed = wjr::fixed_ring_buffer<assignment_element, allocator>;
    for (bool fail_hook : {false, true}) {
        for (const auto shape :
             {std::pair{3, 2}, std::pair{3, 0}, std::pair{0, 2}, std::pair{3, 6}}) {
            check_tail_rollback<ring>(shape.first, shape.second, 7, fail_hook);
            check_tail_rollback<fixed>(shape.first, shape.second, 7, fail_hook);
        }
    }
}

TEST(ring_assignment, copy_assignment_from_wrapped_source_releases_failed_growth) {
    for (int budget = 0; budget != 7; ++budget) {
        assignment_counts counts;
        {
            ring destination(4, wjr::in_place_reserve, allocator(counts));
            ring source(8, wjr::in_place_reserve, allocator(counts));
            populate(destination, counts, 0, 2);
            populate(source, counts, 3, 7);
            ASSERT_FALSE(source.array_two().empty());
            const auto original_live = counts.live;
            const auto original_blocks = counts.allocations - counts.deallocations;
            counts.copy_budget = budget;
            EXPECT_THROW(destination = source, std::runtime_error);
            EXPECT_EQ(destination.size(), 2);
            EXPECT_EQ(source.size(), 7);
            EXPECT_EQ(counts.live, original_live);
            EXPECT_EQ(counts.allocations - counts.deallocations, original_blocks);
            counts.copy_budget = -1;
        }
        expect_released(counts);
    }
}

TEST(ring_assignment, unequal_allocator_move_assignment_releases_failed_growth) {
    for (bool fail_hook : {false, true}) {
        for (int budget = 0; budget != 6; ++budget) {
            assignment_counts counts;
            {
                ring destination(4, wjr::in_place_reserve, allocator(counts, 1));
                ring source(8, wjr::in_place_reserve, allocator(counts, 2));
                populate(destination, counts, 0, 2);
                populate(source, counts, 3, 6);
                const auto original_blocks = counts.allocations - counts.deallocations;
                auto *const original_data = destination.data();
                (fail_hook ? counts.hook_budget : counts.move_budget) = budget;
                EXPECT_THROW(destination = std::move(source), std::runtime_error);
                EXPECT_TRUE(destination.empty());
                EXPECT_EQ(destination.data(), original_data);
                EXPECT_EQ(source.size(), 6);
                EXPECT_EQ(counts.allocations - counts.deallocations, original_blocks);
                EXPECT_EQ(counts.live.size(), 6);
                counts.hook_budget = counts.move_budget = -1;
                destination = std::move(source);
                EXPECT_EQ(destination.size(), 6);
            }
            expect_released(counts);
        }
    }
}
TEST(ring_assignment, throwing_forward_iterator_releases_growth_for_nothrow_values) {
    assignment_counts counts;
    {
        using int_ring = wjr::ring_buffer<int, plain_assignment_allocator<int>>;
        int_ring destination(4, wjr::in_place_reserve, plain_assignment_allocator<int>(counts));
        destination.push_back(1);
        destination.push_back(2);
        int values[] = {10, 11, 12, 13, 14, 15};
        bool fail = true;
        auto source = std::span(values) | std::views::transform([&](int &number) -> const int & {
                          if (fail && number == 13)
                              throw std::runtime_error("iterator failed");
                          return number;
                      });
        static_assert(
            std::is_base_of_v<std::forward_iterator_tag,
                              wjr::iterator_category_t<decltype(std::ranges::begin(source))>>);
        const auto original_blocks = counts.allocations - counts.deallocations;
        EXPECT_THROW(destination.assign_range(source), std::runtime_error);
        EXPECT_EQ(counts.allocations - counts.deallocations, original_blocks);
        EXPECT_EQ(destination.size(), 2);
        EXPECT_EQ(destination[0], 1);
        EXPECT_EQ(destination[1], 2);
        fail = false;
        destination.assign_range(source);
        EXPECT_TRUE(std::ranges::equal(destination, values));
    }
    expect_released(counts);
}
} // namespace

TEST(ring_assignment, forward_cursor_advances_once_per_element_after_counting) {
    const int source[] = {10, 11, 12, 13, 14, 15, 16};
    wjr::ring_buffer<int> queue(8, wjr::in_place_reserve);
    for (int i = 0; i != 4; ++i)
        queue.emplace_back(i);
    for (int i = 0; i != 3; ++i)
        queue.pop_front();
    queue.emplace_back(4);

    int steps = 0;
    using iter = counted_forward_iterator<int>;
    const auto *storage = queue.data();
    queue.assign(iter{source, &steps}, iter{source + 7, &steps});

    EXPECT_EQ(queue.data(), storage);
    EXPECT_TRUE(std::ranges::equal(queue, source));
    EXPECT_EQ(steps, 14);
}

TEST(ring_assignment, throwing_forward_advance_rolls_back_both_new_segments) {
    assignment_counts counts;
    {
        ring queue(8, wjr::in_place_reserve, allocator(counts));
        populate(queue, counts, 3, 2);
        std::vector<assignment_element> source;
        source.reserve(7);
        for (int i = 0; i != 7; ++i)
            source.emplace_back(counts, i + 10);

        int steps = 0;
        using iter = counted_forward_iterator<assignment_element>;
        const auto *storage = queue.data();
        EXPECT_THROW(
            queue.assign(iter{source.data(), &steps, 13}, iter{source.data() + 7, &steps, 13}),
            std::runtime_error);

        EXPECT_EQ(queue.data(), storage);
        EXPECT_EQ(queue.size(), 2);
        EXPECT_EQ(queue.front().number, 10);
        EXPECT_EQ(queue.back().number, 11);
        EXPECT_EQ(counts.live.size(), 9);
        EXPECT_EQ(counts.invalid_destroys, 0);
    }
    expect_released(counts);
}
