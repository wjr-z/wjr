#include <array>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/vector.hpp>
#include <wjr/np/tensor.hpp>

namespace {
namespace np = wjr::np;
using fixed_shape = np::extents<size_t, 2, 3>;
using dynamic_shape = np::dextents<size_t, 2>;
using fixed_owner = np::tensor<int, fixed_shape>;
using dynamic_owner = np::tensor<int, dynamic_shape>;
using array_owner = np::tensor<int, fixed_shape, np::layout_left, std::array<int, 6>>;

template <class T, class E, class L, class C>
concept owner_type = requires { typename np::tensor<T, E, L, C>; };
template <class Owner>
concept borrows = requires(Owner &&owner) { std::forward<Owner>(owner).view(); };
template <class Owner>
concept tries_borrow = requires(Owner &&owner) { std::forward<Owner>(owner).try_view(); };
template <class Owner, class Integer, size_t Rank>
concept makes = requires(std::array<Integer, Rank> shape) { np::make_tensor<Owner>(shape); };

template <class Owner>
concept makes_default_initialized = requires(std::array<int, Owner::rank()> shape) {
    np::make_tensor<Owner>(shape, wjr::default_construct);
};

using standard_owner = np::tensor<int, dynamic_shape, np::layout_right, std::vector<int>>;
static_assert(std::same_as<fixed_owner::container_type, np::tensor_vector<int>>);
static_assert(sizeof(np::tensor_vector<int>) == sizeof(wjr::vector<int>));
static_assert(wjr::get_relocate_mode_v<np::tensor_vector<int>> ==
              wjr::get_relocate_mode_v<wjr::vector<int>>);
static_assert(makes_default_initialized<dynamic_owner>);
static_assert(!makes_default_initialized<standard_owner>);
static_assert(!makes_default_initialized<array_owner>);

struct accepts_initialization_tag {
    accepts_initialization_tag() = default;
    // Deliberately implicit: std::vector could treat the tag as a fill value.
    accepts_initialization_tag(wjr::default_construct_t) {}
};
using convertible_tag_owner = np::tensor<accepts_initialization_tag, dynamic_shape,
                                         np::layout_right, std::vector<accepts_initialization_tag>>;
static_assert(!makes_default_initialized<convertible_tag_owner>);
template <class Owner>
concept bypasses_factory =
    requires(typename Owner::mapping_type mapping, typename Owner::container_type storage) {
        Owner({}, mapping, std::move(storage));
    };
template <class Owner>
concept borrows_metadata = requires(Owner &&owner) {
    std::forward<Owner>(owner).mapping();
    std::forward<Owner>(owner).extents();
};

static_assert(!owner_type<bool, fixed_shape, np::layout_right, std::vector<bool>>);
static_assert(!owner_type<const int, fixed_shape, np::layout_right, std::vector<int>>);
static_assert(owner_type<int, fixed_shape, np::layout_stride, std::vector<int>>);
static_assert(!owner_type<int, fixed_shape, np::layout_stride, std::array<int, 6>>);
static_assert(!owner_type<int, fixed_shape, np::layout_right, std::vector<float>>);
static_assert(!owner_type<int, fixed_shape, np::layout_right, wjr::vector<int>>);
static_assert(!owner_type<int, fixed_shape, np::layout_right, wjr::fixed_vector<int>>);
static_assert(!owner_type<int, fixed_shape, np::layout_right, wjr::small_vector<int, 0>>);
static_assert(borrows<fixed_owner &> && borrows<const fixed_owner &>);
static_assert(!borrows<fixed_owner> && !borrows<const fixed_owner>);
static_assert(!tries_borrow<fixed_owner> && !tries_borrow<const fixed_owner>);
static_assert(!borrows_metadata<fixed_owner> && !borrows_metadata<const fixed_owner>);
static_assert(!bypasses_factory<fixed_owner> && !bypasses_factory<array_owner>);
static_assert(!makes<dynamic_owner, bool, 2> && !makes<dynamic_owner, int, 1>);
static_assert(
    std::same_as<decltype(std::declval<const fixed_owner &>().view()(0, 0)), const int &>);
static_assert(std::is_nothrow_move_constructible_v<fixed_owner>);
static_assert(std::is_nothrow_move_assignable_v<fixed_owner> ==
              std::is_nothrow_move_assignable_v<fixed_owner::container_type>);

constexpr bool constant_array() {
    array_owner owner;
    if (owner.size() != 6 || !owner.has_storage() || owner.view()(1, 2) != 0)
        return false;
    owner.view()(1, 2) = 7;
    auto copy = owner;
    return copy.view()(1, 2) == 7 && copy.view().stride(0) == 1;
}
static_assert(constant_array());

struct no_default {
    int value;
    explicit no_default(int v) : value(v) {}
};
using no_default_owner = np::tensor<no_default, np::extents<size_t, 1>>;
static_assert(!std::is_default_constructible_v<no_default_owner>);
static_assert(!makes<no_default_owner, int, 1>);

struct move_counter {
    static inline int moves = 0;
    move_counter() = default;
    move_counter(move_counter &&) noexcept { ++moves; }
    move_counter &operator=(move_counter &&) = default;
};

struct immovable {
    int value = 7;
    immovable() = default;
    immovable(const immovable &) = delete;
    immovable(immovable &&) = delete;
};

struct narrow_container : std::vector<int> {
    using std::vector<int>::vector;
    using size_type = uint8_t;
    size_type size() const noexcept { return static_cast<size_type>(std::vector<int>::size()); }
    size_type max_size() const noexcept { return 255; }
};

struct allocation_state {
    size_t limit = 100;
    int allocations = 0;
    int live_blocks = 0;
    int last_identity = 0;
    bool fail = false;
};

template <class T>
struct state_allocator {
    using value_type = T;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::false_type;
    using is_always_equal = std::false_type;
    allocation_state *state = nullptr;
    int identity = 0;
    state_allocator() = default;
    state_allocator(allocation_state &s, int i) : state(&s), identity(i) {}
    template <class U>
    explicit state_allocator(const state_allocator<U> &other)
        : state(other.state), identity(other.identity) {}
    state_allocator select_on_container_copy_construction() const {
        auto result = *this;
        ++result.identity;
        return result;
    }
    size_t max_size() const noexcept { return state ? state->limit : 100; }
    T *allocate(size_t n) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        if (state && state->fail)
            throw std::bad_alloc();
#endif
        auto *result = std::allocator<T>{}.allocate(n);
        if (state) {
            ++state->allocations;
            ++state->live_blocks;
            state->last_identity = identity;
        }
        return result;
    }
    void deallocate(T *p, size_t n) noexcept {
        if (state)
            --state->live_blocks;
        std::allocator<T>{}.deallocate(p, n);
    }
    bool operator==(const state_allocator &) const = default;
};

template <class T>
using state_owner =
    np::tensor<T, dynamic_shape, np::layout_right, np::tensor_vector<T, state_allocator<T>>>;

#if !defined(WJR_DISABLE_EXCEPTIONS)
struct throwing_element {
    static inline int live = 0;
    static inline int budget = -1;
    static void may_fail() {
        if (budget == 0)
            throw std::runtime_error("element construction");
        if (budget > 0)
            --budget;
    }
    throwing_element() {
        may_fail();
        ++live;
    }
    throwing_element(const throwing_element &) {
        may_fail();
        ++live;
    }
    throwing_element(throwing_element &&) {
        may_fail();
        ++live;
    }
    throwing_element &operator=(const throwing_element &) {
        may_fail();
        return *this;
    }
    throwing_element &operator=(throwing_element &&) {
        may_fail();
        return *this;
    }
    ~throwing_element() { --live; }
};

WJR_NOINLINE void construct_throwing_owner(state_allocator<throwing_element> allocator) {
    (void)np::make_tensor<state_owner<throwing_element>>(std::array{2, 3}, std::allocator_arg,
                                                         allocator);
}
WJR_NOINLINE void move_throwing_owner(state_owner<throwing_element> &target,
                                      state_owner<throwing_element> &source) {
    target = std::move(source);
}

// A valid container that deliberately loses its objects before a failed copy.
// This makes the documented incomplete state deterministic across STL versions.
struct failing_container : std::vector<int> {
    using std::vector<int>::vector;
    static inline bool fail = false;
    failing_container() = default;
    failing_container(const failing_container &) = default;
    failing_container(failing_container &&) = default;
    failing_container &operator=(const failing_container &other) {
        if (fail) {
            clear();
            throw std::runtime_error("container copy");
        }
        std::vector<int>::operator=(other);
        return *this;
    }
    failing_container &operator=(failing_container &&) = default;
};
#endif
} // namespace

TEST(np_tensor, default_static_dynamic_scalar_and_empty) {
    fixed_owner fixed;
    EXPECT_EQ(fixed.size(), 6u);
    EXPECT_EQ(fixed.storage_size(), 6u);
    EXPECT_EQ(fixed.view()(1, 2), 0);
    dynamic_owner dynamic;
    EXPECT_EQ(dynamic.extent(0), 0u);
    EXPECT_EQ(dynamic.extent(1), 0u);
    ASSERT_TRUE(dynamic.try_view());
    EXPECT_TRUE(dynamic.try_view()->empty());
    np::tensor<int, np::extents<size_t>> scalar;
    EXPECT_EQ(scalar.size(), 1u);
    EXPECT_EQ(scalar.view()(), 0);
    np::tensor<int, np::extents<size_t, 3, 0>> empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_TRUE(empty.has_storage());
    EXPECT_TRUE(empty.try_view().has_value());
}

TEST(np_tensor, compact_storage_layout_const_and_alignment) {
    struct direct_dynamic_storage {
        dynamic_owner::mapping_type mapping;
        np::tensor_vector<int> storage;
    };
    EXPECT_EQ(sizeof(fixed_owner), sizeof(np::tensor_vector<int>));
    EXPECT_EQ(sizeof(array_owner), sizeof(std::array<int, 6>));
    EXPECT_EQ(sizeof(dynamic_owner), sizeof(direct_dynamic_storage));
    EXPECT_EQ(alignof(array_owner), alignof(std::array<int, 6>));
    array_owner column;
    column.view()(0, 1) = 9;
    EXPECT_EQ(column.view().data_handle()[2], 9);
    EXPECT_EQ(std::as_const(column).view()(0, 1), 9);
    struct alignas(64) aligned {
        int value;
    };
    np::tensor<aligned, np::extents<size_t, 2>> values;
    EXPECT_EQ(reinterpret_cast<uintptr_t>(values.view().data_handle()) % alignof(aligned), 0u);
    np::tensor<bool, np::extents<size_t, 2>, np::layout_right, std::array<bool, 2>> flags;
    flags.view()(1) = true;
    EXPECT_TRUE(std::as_const(flags).view()(1));
}

TEST(np_tensor, checked_creation_and_adoption) {
    auto standard = np::make_tensor<standard_owner>(std::array{2, 3});
    ASSERT_TRUE(standard);
    EXPECT_EQ(standard->view()(1, 2), 0);
    auto made = np::make_tensor<dynamic_owner>(std::array{2, 3});
    ASSERT_TRUE(made);
    EXPECT_EQ(made->view()(1, 2), 0);
    np::tensor_vector<int> storage{0, 1, 2, 3, 4, 5};
    auto *data = storage.data();
    auto wrong = np::adopt_tensor<dynamic_owner>(std::array{2, 2}, std::move(storage));
    ASSERT_FALSE(wrong);
    EXPECT_EQ(wrong.error(), np::shape_error::storage_size_mismatch);
    EXPECT_EQ(storage.data(), data);
    EXPECT_EQ(storage.size(), 6u);
    auto adopted = np::adopt_tensor<dynamic_owner>(std::array{2, 3}, std::move(storage));
    ASSERT_TRUE(adopted);
    EXPECT_EQ(adopted->view().data_handle(), data);
    EXPECT_EQ(adopted->view()(1, 2), 5);
    auto array = np::make_tensor<array_owner>(std::array{2, 3});
    ASSERT_TRUE(array);
    EXPECT_EQ(array->view()(1, 2), 0);
    auto mismatch = np::make_tensor<fixed_owner>(std::array{3, 2});
    ASSERT_FALSE(mismatch);
    EXPECT_EQ(mismatch.error(), np::shape_error::static_extent_mismatch);
    np::tensor_vector<no_default> objects;
    objects.emplace_back(7);
    auto nondefault = np::adopt_tensor<no_default_owner>(std::array{1}, std::move(objects));
    ASSERT_TRUE(nondefault);
    EXPECT_EQ(nondefault->view()(0).value, 7);
}

TEST(np_tensor, shape_validation_precedes_allocation) {
    allocation_state state;
    using owner = state_owner<int>;
    state_allocator<int> alloc(state, 8);
    auto negative = np::make_tensor<owner>(std::array{0, -1}, std::allocator_arg, alloc);
    ASSERT_FALSE(negative);
    EXPECT_EQ(negative.error(), np::shape_error::negative_extent);
    state.limit = 5;
    auto too_large = np::make_tensor<owner>(std::array{2, 3}, std::allocator_arg, alloc);
    ASSERT_FALSE(too_large);
    EXPECT_EQ(too_large.error(), np::shape_error::container_size_limit);
    EXPECT_EQ(state.allocations, 0);
    using shape = np::dextents<int, 3>;
    using narrow_owner = np::tensor<int, shape>;
    for (auto dimensions : {std::array{0, 46341, 46341}, std::array{46341, 46341, 0}}) {
        auto invalid = np::make_tensor<narrow_owner>(dimensions);
        ASSERT_FALSE(invalid);
        EXPECT_EQ(invalid.error(), np::shape_error::index_overflow);
    }
    auto empty = np::make_tensor<narrow_owner>(std::array{46341, 0, 46341});
    ASSERT_TRUE(empty);
    EXPECT_TRUE(empty->try_view().has_value());
    auto wide =
        np::make_tensor<np::tensor<int, np::dextents<uint8_t, 1>>>(std::array<uint64_t, 1>{256});
    ASSERT_FALSE(wide);
    EXPECT_EQ(wide.error(), np::shape_error::extent_out_of_range);
    using narrow_storage_owner = np::tensor<int, dynamic_shape, np::layout_right, narrow_container>;
    auto narrow_storage = np::make_tensor<narrow_storage_owner>(std::array{16, 16});
    ASSERT_FALSE(narrow_storage);
    EXPECT_EQ(narrow_storage.error(), np::shape_error::size_overflow);
}

TEST(np_tensor, in_place_factories_do_not_move_new_array_elements) {
    using owner =
        np::tensor<move_counter, fixed_shape, np::layout_right, std::array<move_counter, 6>>;
    move_counter::moves = 0;
    auto made = np::make_tensor<owner>(std::array{2, 3});
    ASSERT_TRUE(made);
    EXPECT_EQ(move_counter::moves, 0);
    std::array<move_counter, 6> storage;
    auto adopted = np::adopt_tensor<owner>(std::array{2, 3}, std::move(storage));
    ASSERT_TRUE(adopted);
    EXPECT_EQ(move_counter::moves, 6);
    using stationary =
        np::tensor<immovable, fixed_shape, np::layout_right, std::array<immovable, 6>>;
    auto in_place = np::make_tensor<stationary>(std::array{2, 3});
    ASSERT_TRUE(in_place);
    EXPECT_EQ(in_place->view()(1, 2).value, 7);
}

TEST(np_tensor, explicit_wjr_vector_int_and_real_bool_storage) {
    using integers = np::tensor<int, dynamic_shape, np::layout_right, np::tensor_vector<int>>;
    using booleans = np::tensor<bool, dynamic_shape, np::layout_right, np::tensor_vector<bool>>;
    auto values = np::make_tensor<integers>(std::array{2, 3});
    auto flags = np::make_tensor<booleans>(std::array{2, 3});
    ASSERT_TRUE(values);
    ASSERT_TRUE(flags);
    values->view()(1, 2) = 7;
    flags->view()(1, 2) = true;
    EXPECT_EQ(std::as_const(*values).view()(1, 2), 7);
    EXPECT_TRUE(std::as_const(*flags).view()(1, 2));
    static_assert(std::same_as<decltype(flags->view()(0, 0)), bool &>);
}

TEST(np_tensor, container_allocator_copy_selection_and_single_allocation) {
    allocation_state state;
    {
        auto made = np::make_tensor<state_owner<int>>(std::array{2, 3}, std::allocator_arg,
                                                      state_allocator<int>(state, 8));
        ASSERT_TRUE(made);
        EXPECT_EQ(state.allocations, 1);
        EXPECT_EQ(state.last_identity, 8);
        auto copied = *made;
        EXPECT_EQ(state.allocations, 2);
        EXPECT_EQ(state.last_identity, 9);
        copied.view()(0, 0) = 4;
        EXPECT_EQ(made->view()(0, 0), 0);
        auto moved = std::move(copied);
        EXPECT_EQ(moved.size(), 6u);
        EXPECT_EQ(copied.size(), 6u);
        EXPECT_EQ(copied.has_storage(), copied.storage_size() == copied.size());
        EXPECT_EQ(copied.try_view().has_value(), copied.has_storage());
        auto again = std::move(copied);
        EXPECT_EQ(again.has_storage(), again.storage_size() == again.size());
        auto incomplete_copy = again;
        EXPECT_EQ(incomplete_copy.storage_size(), again.storage_size());
        moved = moved;
        moved = std::move(moved);
        EXPECT_EQ(moved.view()(0, 0), 4);
    }
    EXPECT_EQ(state.live_blocks, 0);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(np_tensor, failed_container_assignment_retains_mapping_and_recovers) {
    using owner = np::tensor<int, dynamic_shape, np::layout_right, failing_container>;
    auto source = np::make_tensor<owner>(std::array{2, 3});
    auto target = np::make_tensor<owner>(std::array{1, 2});
    ASSERT_TRUE(source);
    ASSERT_TRUE(target);
    failing_container::fail = true;
    EXPECT_THROW(*target = *source, std::runtime_error);
    failing_container::fail = false;
    EXPECT_EQ(target->extent(0), 1u);
    EXPECT_EQ(target->extent(1), 2u);
    EXPECT_EQ(target->storage_size(), 0u);
    EXPECT_FALSE(target->has_storage());
    EXPECT_FALSE(target->try_view());
    auto representation = *target;
    auto moved_representation = std::move(representation);
    EXPECT_EQ(moved_representation.size(), 2u);
    EXPECT_EQ(moved_representation.storage_size(), 0u);
    *target = *source;
    EXPECT_EQ(target->extent(0), 2u);
    EXPECT_TRUE(target->has_storage());
}

TEST(np_tensor, allocator_failure_does_not_publish_new_metadata) {
    allocation_state state;
    {
        auto source = np::make_tensor<state_owner<int>>(std::array{2, 3}, std::allocator_arg,
                                                        state_allocator<int>(state, 1));
        auto target = np::make_tensor<state_owner<int>>(std::array{1, 2}, std::allocator_arg,
                                                        state_allocator<int>(state, 2));
        ASSERT_TRUE(source);
        ASSERT_TRUE(target);
        state.fail = true;
        EXPECT_THROW(*target = *source, std::bad_alloc);
        state.fail = false;
        EXPECT_EQ(target->extent(0), 1u);
        EXPECT_EQ(target->extent(1), 2u);
        EXPECT_EQ(target->try_view().has_value(), target->has_storage());
        *target = *source;
        EXPECT_TRUE(target->has_storage());
        auto unequal = np::make_tensor<state_owner<int>>(std::array{1, 1}, std::allocator_arg,
                                                         state_allocator<int>(state, 3));
        state.fail = true;
        EXPECT_THROW(*unequal = std::move(*source), std::bad_alloc);
        state.fail = false;
        EXPECT_EQ(unequal->extent(0), 1u);
        EXPECT_EQ(unequal->extent(1), 1u);
        EXPECT_EQ(source->extent(0), 2u);
        EXPECT_EQ(source->extent(1), 3u);
        *unequal = std::move(*source);
        EXPECT_TRUE(unequal->has_storage());
    }
    EXPECT_EQ(state.live_blocks, 0);
}

TEST(np_tensor, partial_construction_and_throwing_element_move) {
    allocation_state state;
    throwing_element::budget = 2;
    EXPECT_THROW(construct_throwing_owner(state_allocator<throwing_element>(state, 1)),
                 std::runtime_error);
    EXPECT_EQ(throwing_element::live, 0);
    EXPECT_EQ(state.live_blocks, 0);
    throwing_element::budget = -1;
    {
        auto source = np::make_tensor<state_owner<throwing_element>>(
            std::array{2, 3}, std::allocator_arg, state_allocator<throwing_element>(state, 1));
        auto target = np::make_tensor<state_owner<throwing_element>>(
            std::array{1, 1}, std::allocator_arg, state_allocator<throwing_element>(state, 2));
        ASSERT_TRUE(source);
        ASSERT_TRUE(target);
        throwing_element::budget = 1;
        EXPECT_THROW(move_throwing_owner(*target, *source), std::runtime_error);
        throwing_element::budget = -1;
        EXPECT_EQ(target->extent(0), 1u);
        EXPECT_EQ(source->extent(0), 2u);
        EXPECT_EQ(target->try_view().has_value(), target->has_storage());
        *target = *source;
        EXPECT_TRUE(target->has_storage());
    }
    EXPECT_EQ(throwing_element::live, 0);
    EXPECT_EQ(state.live_blocks, 0);
}
#endif
