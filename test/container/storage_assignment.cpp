#include <memory>
#include <set>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/ring_buffer.hpp>
#include <wjr/vector.hpp>

namespace {
struct assignment_state {
    std::set<void *> allocations;
    int live = 0;
    int constructed = 0;
    int destroyed = 0;
    int invalid_deallocations = 0;
};

struct assignment_value {
    assignment_state *owner;
    int number;
    assignment_value(assignment_state &counts, int n) noexcept : owner(&counts), number(n) {
        ++owner->live;
    }
    assignment_value(const assignment_value &) = delete;
    assignment_value(assignment_value &&other) noexcept : owner(other.owner), number(other.number) {
        ++owner->live;
    }
    assignment_value &operator=(assignment_value &&) = default;
    ~assignment_value() { --owner->live; }
};

template <typename T>
struct assignment_allocator {
    using value_type = T;
    assignment_state *owner;
    explicit assignment_allocator(assignment_state &counts) noexcept : owner(&counts) {}
    template <typename U>
    assignment_allocator(const assignment_allocator<U> &other) noexcept : owner(other.owner) {}
    T *allocate(std::size_t n) {
        auto *ptr = std::allocator<T>{}.allocate(n);
        owner->allocations.insert(ptr);
        return ptr;
    }
    void deallocate(T *ptr, std::size_t n) noexcept {
        if (owner->allocations.erase(ptr) != 1)
            ++owner->invalid_deallocations;
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
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

template <typename Container>
void check_assignment(assignment_state &counts, const typename Container::allocator_type &alloc,
                      int destination_size, int source_size, bool reserve_destination) {
    {
        Container destination(alloc), source(alloc);
        if (reserve_destination)
            destination.reserve(8);
        for (int i = 0; i != destination_size; ++i)
            destination.emplace_back(counts, 100 + i);
        for (int i = 0; i != source_size; ++i)
            source.emplace_back(counts, 200 + i);
        auto *const original_data = destination.data();
        auto &self_result = destination = std::move(destination.get_storage());
        EXPECT_EQ(&self_result, &destination);
        EXPECT_EQ(destination.data(), original_data);
        EXPECT_EQ(destination.size(), destination_size);
        EXPECT_EQ(counts.live, destination_size + source_size);

        auto &result = destination = std::move(source.get_storage());
        EXPECT_EQ(&result, &destination);
        ASSERT_EQ(destination.size(), source_size);
        int i = 0;
        for (const auto &element : destination)
            EXPECT_EQ(element.number, 200 + i++);
        EXPECT_EQ(counts.live, source_size);
    }
    EXPECT_EQ(counts.live, 0);
    EXPECT_TRUE(counts.allocations.empty());
    EXPECT_EQ(counts.constructed, counts.destroyed);
    EXPECT_EQ(counts.invalid_deallocations, 0);
}

template <typename Container>
void check_matrix() {
    for (int destination_size : {0, 2, 6}) {
        for (int source_size : {0, 2, 6}) {
            for (bool reserve_destination : {false, true}) {
                SCOPED_TRACE(testing::Message() << destination_size << ',' << source_size << ','
                                                << reserve_destination);
                assignment_state counts;
                typename Container::allocator_type alloc(counts);
                check_assignment<Container>(counts, alloc, destination_size, source_size,
                                            reserve_destination);
            }
        }
    }
}

TEST(storage_assignment, vector_releases_old_elements_and_allocation) {
    check_matrix<wjr::vector<assignment_value, assignment_allocator<assignment_value>>>();
}

TEST(storage_assignment, small_vector_releases_inline_and_heap_destinations) {
    check_matrix<wjr::small_vector<assignment_value, 4, assignment_allocator<assignment_value>>>();
}

TEST(storage_assignment, inplace_vector_ends_old_lifetimes_before_take) {
    assignment_state counts;
    for (int destination_size : {0, 2, 6}) {
        for (int source_size : {0, 2, 6}) {
            check_assignment<wjr::inplace_vector<assignment_value, 8>>(counts, {}, destination_size,
                                                                       source_size, false);
        }
    }
}

TEST(storage_assignment, ring_buffer_releases_old_elements_and_allocation) {
    check_matrix<wjr::ring_buffer<assignment_value, assignment_allocator<assignment_value>>>();
}

TEST(storage_assignment, wrapped_ring_buffer_destroys_both_old_spans) {
    assignment_state counts;
    {
        using container =
            wjr::ring_buffer<assignment_value, assignment_allocator<assignment_value>>;
        container destination{assignment_allocator<assignment_value>(counts)};
        container source{assignment_allocator<assignment_value>(counts)};
        destination.reserve(8);
        source.reserve(8);
        for (int i = 0; i != 8; ++i) {
            destination.emplace_back(counts, 100 + i);
            source.emplace_back(counts, 200 + i);
        }
        for (int i = 0; i != 5; ++i) {
            destination.pop_front();
            source.pop_front();
        }
        for (int i = 8; i != 12; ++i) {
            destination.emplace_back(counts, 100 + i);
            source.emplace_back(counts, 200 + i);
        }
        ASSERT_FALSE(destination.array_two().empty());
        ASSERT_FALSE(source.array_two().empty());
        destination = std::move(source.get_storage());
        EXPECT_EQ(counts.live, 7);
        ASSERT_EQ(destination.size(), 7);
        int expected = 205;
        for (const auto &element : destination)
            EXPECT_EQ(element.number, expected++);
    }
    EXPECT_EQ(counts.live, 0);
    EXPECT_TRUE(counts.allocations.empty());
    EXPECT_EQ(counts.constructed, counts.destroyed);
    EXPECT_EQ(counts.invalid_deallocations, 0);
}
} // namespace
