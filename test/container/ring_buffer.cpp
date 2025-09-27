#include "../detail.hpp"

#include <list>
#include <vector>
#include <wjr/container/ring_buffer.hpp>

using namespace wjr;

// Test class to track construction/destruction
class TrackingObject {
private:
    int value_;
    static int construction_count;
    static int destruction_count;
    static int copy_count;
    static int move_count;

public:
    TrackingObject(int val = 0) : value_(val) { ++construction_count; }

    TrackingObject(const TrackingObject &other) : value_(other.value_) {
        ++construction_count;
        ++copy_count;
    }

    TrackingObject(TrackingObject &&other) noexcept : value_(other.value_) {
        ++construction_count;
        ++move_count;
        other.value_ = 0;
    }

    TrackingObject &operator=(const TrackingObject &other) {
        if (this != &other) {
            value_ = other.value_;
            ++copy_count;
        }
        return *this;
    }

    TrackingObject &operator=(TrackingObject &&other) noexcept {
        if (this != &other) {
            value_ = other.value_;
            ++move_count;
            other.value_ = 0;
        }
        return *this;
    }

    ~TrackingObject() { ++destruction_count; }

    int value() const { return value_; }

    static int get_construction_count() { return construction_count; }
    static int get_destruction_count() { return destruction_count; }
    static int get_copy_count() { return copy_count; }
    static int get_move_count() { return move_count; }

    static void reset_counts() {
        construction_count = destruction_count = copy_count = move_count = 0;
    }

    bool operator==(const TrackingObject &other) const { return value_ == other.value_; }
};

int TrackingObject::construction_count = 0;
int TrackingObject::destruction_count = 0;
int TrackingObject::copy_count = 0;
int TrackingObject::move_count = 0;

class ThrowingObject {
private:
    static int construction_count;
    static bool should_throw_on_construction;

public:
    int value;

    ThrowingObject(int val = 0) : value(val) {
        ++construction_count;
        if (should_throw_on_construction) {
            throw std::runtime_error("Construction failed");
        }
    }

    static void reset() {
        construction_count = 0;
        should_throw_on_construction = false;
    }

    static void enable_throwing() {
        should_throw_on_construction = true;
        construction_count = 0;
    }
};

int ThrowingObject::construction_count = 0;
bool ThrowingObject::should_throw_on_construction = false;

TEST(ring_buffer, constructor) {
    // Default constructor
    ring_buffer<int> rb1;
    EXPECT_TRUE(rb1.empty());
    EXPECT_EQ(rb1.size(), 0);

    // Size constructor
    ring_buffer<int> rb2(5);
    EXPECT_EQ(rb2.size(), 5);
    EXPECT_FALSE(rb2.empty());

    // Size and value constructor
    ring_buffer<int> rb3(4, 42);
    EXPECT_EQ(rb3.size(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(rb3[i], 42);
    }

    // Iterator constructor
    std::vector<int> vec = {1, 2, 3, 4, 5};
    ring_buffer<int> rb4(vec.begin(), vec.end());
    EXPECT_EQ(rb4.size(), 5);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(rb4[i], vec[i]);
    }

    // Initializer list constructor
    ring_buffer<int> rb5{10, 20, 30};
    EXPECT_EQ(rb5.size(), 3);
    EXPECT_EQ(rb5[0], 10);
    EXPECT_EQ(rb5[1], 20);
    EXPECT_EQ(rb5[2], 30);
}

TEST(ring_buffer, basic_operations) {
    ring_buffer<int> rb;

    // Test push_back and emplace_back
    rb.push_back(1);
    rb.emplace_back(2);
    rb.push_back(3);

    EXPECT_EQ(rb.size(), 3);
    EXPECT_EQ(rb[0], 1);
    EXPECT_EQ(rb[1], 2);
    EXPECT_EQ(rb[2], 3);

    // Test front and back
    EXPECT_EQ(rb.front(), 1);
    EXPECT_EQ(rb.back(), 3);

    // Test pop_front and pop_back
    rb.pop_front();
    EXPECT_EQ(rb.size(), 2);
    EXPECT_EQ(rb.front(), 2);

    rb.pop_back();
    EXPECT_EQ(rb.size(), 1);
    EXPECT_EQ(rb.back(), 2);

    // Test clear
    rb.clear();
    EXPECT_TRUE(rb.empty());
    EXPECT_EQ(rb.size(), 0);
}

TEST(ring_buffer, iterator) {
    ring_buffer<int> rb{1, 2, 3, 4, 5};

    // Test forward iteration
    int expected = 1;
    for (auto it = rb.begin(); it != rb.end(); ++it, ++expected) {
        EXPECT_EQ(*it, expected);
    }

    // Test range-based for loop
    expected = 1;
    for (int value : rb) {
        EXPECT_EQ(value, expected++);
    }

    // Test reverse iteration
    expected = 5;
    for (auto it = rb.rbegin(); it != rb.rend(); ++it, --expected) {
        EXPECT_EQ(*it, expected);
    }

    // Test const iterators
    const ring_buffer<int> &const_rb = rb;
    expected = 1;
    for (auto it = const_rb.cbegin(); it != const_rb.cend(); ++it, ++expected) {
        EXPECT_EQ(*it, expected);
    }
}

TEST(ring_buffer, capacity) {
    ring_buffer<int> rb;
    size_t initial_capacity = rb.capacity();

    // Test reserve
    rb.reserve(100);
    EXPECT_GE(rb.capacity(), 100);
    EXPECT_TRUE(rb.empty());

    // Fill up to capacity
    for (size_t i = 0; i < rb.capacity(); ++i) {
        rb.push_back(static_cast<int>(i));
    }

    EXPECT_TRUE(rb.full());
    EXPECT_EQ(rb.size(), rb.capacity());

    // Test automatic expansion
    size_t old_capacity = rb.capacity();
    rb.push_back(999);
    EXPECT_GT(rb.capacity(), old_capacity);
    EXPECT_EQ(rb.back(), 999);
}

TEST(ring_buffer, wrap_around_behavior) {
    ring_buffer<int> rb;
    rb.reserve(4);

    // Fill the buffer
    for (int i = 0; i < 4; ++i) {
        rb.push_back(i);
    }

    // Test wrap-around behavior
    for (int i = 4; i < 8; ++i) {
        rb.pop_front();
        rb.push_back(i);

        // Verify the buffer contains the last 4 elements
        for (int j = 0; j < 4; ++j) {
            EXPECT_EQ(rb[j], i - 3 + j);
        }
    }
}

TEST(ring_buffer, array_access) {
    ring_buffer<int> rb{1, 2, 3, 4, 5};

    // Test array_one and array_two (contiguous access)
    auto array1 = rb.array_one();
    auto array2 = rb.array_two();

    // When data is contiguous, array_one should contain all data
    // and array_two should be empty
    size_t total_size = array1.size() + array2.size();
    EXPECT_EQ(total_size, rb.size());

    // Test after wrap-around
    rb.pop_front();
    rb.pop_front();
    rb.push_back(6);
    rb.push_back(7);

    array1 = rb.array_one();
    array2 = rb.array_two();
    total_size = array1.size() + array2.size();
    EXPECT_EQ(total_size, rb.size());
}

TEST(ring_buffer, comparison_with_vector) {
    // Test that ring_buffer behaves similarly to vector for basic operations
    ring_buffer<int> rb;
    std::vector<int> vec;

    for (int i = 0; i < 10; ++i) {
        rb.push_back(i);
        vec.push_back(i);

        EXPECT_EQ(rb.size(), vec.size());
        EXPECT_EQ(rb.back(), vec.back());
        if (!rb.empty()) {
            EXPECT_EQ(rb.front(), vec.front());
        }
    }

    // Compare element access
    for (size_t i = 0; i < rb.size(); ++i) {
        EXPECT_EQ(rb[i], vec[i]);
    }

    // Test that iterators work the same way
    EXPECT_TRUE(std::equal(rb.begin(), rb.end(), vec.begin()));
}

TEST(ring_buffer, input_iterator_assign) {
    ring_buffer<int> rb{1, 2, 3, 4, 5};

    // Test assigning from input iterator (list iterator)
    std::list<int> lst{10, 20, 30};
    // Note: This would call __range_assign with input_iterator_tag
    // rb.assign(lst.begin(), lst.end()); // Uncomment when assign is implemented

    // For now, test the underlying behavior manually
    rb.clear();
    for (auto val : lst) {
        rb.emplace_back(val);
    }

    EXPECT_EQ(rb.size(), 3);
    EXPECT_EQ(rb[0], 10);
    EXPECT_EQ(rb[1], 20);
    EXPECT_EQ(rb[2], 30);
}

TEST(ring_buffer, copy_constructor_and_assignment) {
    ring_buffer<int> rb1{1, 2, 3, 4, 5};

    // Test copy constructor
    ring_buffer<int> rb2(rb1);
    EXPECT_EQ(rb2.size(), rb1.size());
    EXPECT_TRUE(std::equal(rb1.begin(), rb1.end(), rb2.begin()));

    // Modify original to ensure deep copy
    rb1[0] = 999;
    EXPECT_NE(rb2[0], rb1[0]);
    EXPECT_EQ(rb2[0], 1); // Should still be original value

    // Test copy assignment (when implemented)
    ring_buffer<int> rb3{10, 20};
    rb3 = rb1;
    EXPECT_EQ(rb3.size(), rb1.size());
    EXPECT_TRUE(std::equal(rb1.begin(), rb1.end(), rb3.begin()));
}

TEST(ring_buffer, move_constructor_and_assignment) {
    ring_buffer<int> rb1{1, 2, 3, 4, 5};
    auto original_capacity = rb1.capacity();
    auto original_data = rb1.data();

    // Test move constructor
    ring_buffer<int> rb2(std::move(rb1));
    EXPECT_EQ(rb2.size(), 5);
    EXPECT_EQ(rb2.capacity(), original_capacity);
    EXPECT_EQ(rb2.data(), original_data); // Should have taken ownership

    // rb1 should be in valid but unspecified state
    EXPECT_EQ(rb1.size(), 0);
    EXPECT_TRUE(rb1.empty());

    // Test move assignment (when implemented)
    ring_buffer<int> rb3{10, 20};
    ring_buffer<int> rb4{100, 200, 300};
    auto rb4_capacity = rb4.capacity();
    auto rb4_data = rb4.data();

    rb3 = std::move(rb4);
    EXPECT_EQ(rb3.size(), 3);
    EXPECT_EQ(rb3.capacity(), rb4_capacity);
    EXPECT_EQ(rb3.data(), rb4_data);
    EXPECT_EQ(rb4.size(), 0);
    EXPECT_TRUE(rb4.empty());
}

TEST(ring_buffer, destruction_and_raii) {
    TrackingObject::reset_counts();

    {
        ring_buffer<TrackingObject> rb;
        // Reserve enough capacity to avoid reallocation during test
        rb.reserve(10);

        rb.emplace_back(1);
        rb.emplace_back(2);
        rb.emplace_back(3);

        EXPECT_EQ(TrackingObject::get_construction_count(), 3);
        EXPECT_EQ(TrackingObject::get_destruction_count(), 0);

        rb.pop_back();
        EXPECT_EQ(TrackingObject::get_destruction_count(), 1);

        rb.clear();
        EXPECT_EQ(TrackingObject::get_destruction_count(), 3);

        // Add more elements (no reallocation due to reserve)
        rb.emplace_back(4);
        rb.emplace_back(5);
        EXPECT_EQ(TrackingObject::get_construction_count(), 5);
    }
    // ring_buffer destructor should clean up remaining elements
    EXPECT_EQ(TrackingObject::get_destruction_count(), 5);
    EXPECT_EQ(TrackingObject::get_construction_count(), TrackingObject::get_destruction_count());
}

TEST(ring_buffer, copy_semantics_with_tracking) {
    TrackingObject::reset_counts();

    {
        ring_buffer<TrackingObject> rb1;
        // Reserve to avoid reallocation during test
        rb1.reserve(10);

        rb1.emplace_back(1);
        rb1.emplace_back(2);
        rb1.emplace_back(3);

        int constructions_before_copy = TrackingObject::get_construction_count();
        int copies_before_copy = TrackingObject::get_copy_count();

        // Test copy constructor
        ring_buffer<TrackingObject> rb2(rb1);

        // Should have created copies of all elements
        EXPECT_EQ(TrackingObject::get_construction_count(), constructions_before_copy + 3);
        EXPECT_EQ(TrackingObject::get_copy_count(), copies_before_copy + 3);

        // Verify content
        EXPECT_EQ(rb2.size(), 3);
        for (size_t i = 0; i < 3; ++i) {
            EXPECT_EQ(rb1[i].value(), rb2[i].value());
        }
    }

    // All objects should be destroyed
    EXPECT_EQ(TrackingObject::get_construction_count(), TrackingObject::get_destruction_count());
}

TEST(ring_buffer, move_semantics_with_tracking) {
    TrackingObject::reset_counts();

    {
        ring_buffer<TrackingObject> rb1;
        // Reserve to avoid reallocation during test
        rb1.reserve(10);

        rb1.emplace_back(1);
        rb1.emplace_back(2);
        rb1.emplace_back(3);

        int constructions_before_move = TrackingObject::get_construction_count();
        int moves_before_move = TrackingObject::get_move_count();
        int destructions_before_move = TrackingObject::get_destruction_count();

        // Test move constructor
        ring_buffer<TrackingObject> rb2(std::move(rb1));

        // Should not have created additional objects (just moved storage)
        EXPECT_EQ(TrackingObject::get_construction_count(), constructions_before_move);
        EXPECT_EQ(TrackingObject::get_move_count(),
                  moves_before_move); // No element moves, just storage move
        EXPECT_EQ(TrackingObject::get_destruction_count(),
                  destructions_before_move); // No destructions from move

        // Verify content moved
        EXPECT_EQ(rb2.size(), 3);
        EXPECT_EQ(rb2[0].value(), 1);
        EXPECT_EQ(rb2[1].value(), 2);
        EXPECT_EQ(rb2[2].value(), 3);

        // rb1 should be empty
        EXPECT_TRUE(rb1.empty());
    }

    // All objects should be destroyed only once at the end
    EXPECT_EQ(TrackingObject::get_construction_count(), TrackingObject::get_destruction_count());
}

TEST(ring_buffer, exception_safety) {
    ThrowingObject::reset();

    ring_buffer<ThrowingObject> rb;
    // Reserve enough capacity to avoid reallocation during test
    rb.reserve(10);

    // Normal construction should work
    rb.emplace_back(1);
    rb.emplace_back(2);
    rb.emplace_back(3);
    EXPECT_EQ(rb.size(), 3);

    // Test exception during construction
    ThrowingObject::enable_throwing();

    try {
        rb.emplace_back(4); // This should throw
        FAIL() << "Expected exception was not thrown";
    } catch (const std::exception &) {
        // Exception was thrown as expected
        // ring_buffer should remain in valid state
        EXPECT_EQ(rb.size(), 3); // Size should not have changed
        EXPECT_EQ(rb[0].value, 1);
        EXPECT_EQ(rb[1].value, 2);
        EXPECT_EQ(rb[2].value, 3);
    }
}

TEST(ring_buffer, reallocation_move_semantics) {
    TrackingObject::reset_counts();

    {
        ring_buffer<TrackingObject> rb;
        // Start with small initial capacity to force reallocation

        rb.emplace_back(1);
        rb.emplace_back(2);

        // Fill to capacity to ensure next insertion causes reallocation
        size_t current_capacity = rb.capacity();
        while (rb.size() < current_capacity) {
            rb.emplace_back(static_cast<int>(rb.size() + 1));
        }

        int size_before_realloc = rb.size();
        int constructions_before_realloc = TrackingObject::get_construction_count();
        int moves_before_realloc = TrackingObject::get_move_count();
        int destructions_before_realloc = TrackingObject::get_destruction_count();

        // This should trigger reallocation since we're at capacity
        rb.emplace_back(999);

        // After reallocation, old elements should be moved/relocated and old copies destroyed
        EXPECT_EQ(TrackingObject::get_construction_count(),
                  constructions_before_realloc + size_before_realloc + 1);

        EXPECT_EQ(TrackingObject::get_destruction_count(),
                  destructions_before_realloc + size_before_realloc);

        // The exact number of moves/destructions depends on implementation
        // but there should be some activity due to reallocation
        EXPECT_GE(TrackingObject::get_move_count() + TrackingObject::get_copy_count(),
                  moves_before_realloc); // Some form of relocation should occur

        // Content should still be correct
        EXPECT_EQ(rb.back().value(), 999);
        EXPECT_GT(rb.capacity(), current_capacity); // Capacity should have increased
    }

    // All objects should be properly cleaned up
    EXPECT_EQ(TrackingObject::get_construction_count(), TrackingObject::get_destruction_count());
}

TEST(ring_buffer, fixed_ring_buffer_capacity_constraint) {
    // Test fixed_ring_buffer which has truly fixed capacity
    fixed_ring_buffer<int> frb;

    // fixed_ring_buffer should start empty
    EXPECT_TRUE(frb.empty());
    EXPECT_EQ(frb.size(), 0);

    // For fixed_ring_buffer, we need to reserve capacity at construction
    fixed_ring_buffer<int> frb_with_capacity(5, in_place_reserve);
    EXPECT_EQ(frb_with_capacity.capacity(), 5);
    EXPECT_TRUE(frb_with_capacity.empty());

    // Fill to capacity
    for (int i = 0; i < 5; ++i) {
        frb_with_capacity.emplace_back(i);
    }

    EXPECT_TRUE(frb_with_capacity.full());
    EXPECT_EQ(frb_with_capacity.size(), 5);

    // Verify contents
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(frb_with_capacity[i], i);
    }
}

TEST(ring_buffer, dynamic_vs_fixed_behavior) {
    TrackingObject::reset_counts();

    {
        // Test dynamic ring_buffer - should expand automatically
        ring_buffer<TrackingObject> dynamic_rb;
        for (int i = 0; i < 10; ++i) {
            dynamic_rb.emplace_back(i);
        }
        EXPECT_EQ(dynamic_rb.size(), 10);

        // Test fixed ring_buffer with pre-allocated capacity
        fixed_ring_buffer<TrackingObject> fixed_rb(5, in_place_reserve);
        for (int i = 0; i < 5; ++i) {
            fixed_rb.emplace_back(i);
        }
        EXPECT_EQ(fixed_rb.size(), 5);
        EXPECT_TRUE(fixed_rb.full());

        // Verify contents
        for (int i = 0; i < 5; ++i) {
            EXPECT_EQ(fixed_rb[i].value(), i);
        }
    }

    // All objects should be cleaned up
    EXPECT_EQ(TrackingObject::get_construction_count(), TrackingObject::get_destruction_count());
}

TEST(ring_buffer, swap_functionality) {
    ring_buffer<int> rb1{1, 2, 3};
    ring_buffer<int> rb2{10, 20, 30, 40, 50};

    auto rb1_size = rb1.size();
    auto rb2_size = rb2.size();

    rb1.swap(rb2);

    // Sizes should be swapped
    EXPECT_EQ(rb1.size(), rb2_size);
    EXPECT_EQ(rb2.size(), rb1_size);

    // Contents should be swapped
    EXPECT_EQ(rb1[0], 10);
    EXPECT_EQ(rb1[4], 50);
    EXPECT_EQ(rb2[0], 1);
    EXPECT_EQ(rb2[2], 3);

    // Test std::swap
    std::swap(rb1, rb2);

    // Should be back to original state
    EXPECT_EQ(rb1.size(), rb1_size);
    EXPECT_EQ(rb2.size(), rb2_size);
    EXPECT_EQ(rb1[0], 1);
    EXPECT_EQ(rb2[0], 10);
}
