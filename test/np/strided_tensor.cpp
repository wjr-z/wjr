#include <array>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/reduce.hpp>
#include <wjr/np/strided_tensor.hpp>

namespace {
namespace np = wjr::np;
using shape = np::dextents<size_t, 2>;
using owner = np::tensor<int, shape, np::layout_stride>;
using fixed_owner = np::tensor<int, np::extents<size_t, 2, 3>, np::layout_stride>;

template <class Owner>
concept shape_only_factory = requires { np::make_tensor<Owner>(std::array{2, 3}); };
static_assert(!std::default_initializable<owner> && !std::default_initializable<fixed_owner>);
static_assert(!shape_only_factory<owner>);
struct direct_representation {
    owner::mapping_type mapping;
    owner::container_type storage;
};
static_assert(sizeof(owner) == sizeof(direct_representation));
static_assert(sizeof(fixed_owner) ==
              sizeof(fixed_owner::mapping_type) + sizeof(fixed_owner::container_type));

struct allocation_state {
    size_t allocated = 0;
    size_t limit = 100;
    int allocations = 0;
    int deallocations = 0;
    int constructed = 0;
    int destroyed = 0;
    int budget = -1;
};
template <class T>
struct hook_allocator {
    using value_type = T;
    allocation_state *state;
    explicit hook_allocator(allocation_state &s) noexcept : state(&s) {}
    template <class U>
    explicit hook_allocator(const hook_allocator<U> &other) noexcept : state(other.state) {}
    size_t max_size() const noexcept { return state->limit; }
    T *allocate(size_t n) {
        state->allocated = n;
        ++state->allocations;
        return std::allocator<T>{}.allocate(n);
    }
    void deallocate(T *p, size_t n) noexcept {
        ++state->deallocations;
        std::allocator<T>{}.deallocate(p, n);
    }
    void construct(T *p) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        if (state->budget == 0)
            throw std::runtime_error("construction failed");
#endif
        if (state->budget > 0)
            --state->budget;
        std::construct_at(p, -7);
        ++state->constructed;
    }
    void destroy(T *p) noexcept {
        ++state->destroyed;
        std::destroy_at(p);
    }
    bool operator==(const hook_allocator &) const = default;
};
using hooked_owner =
    np::tensor<int, shape, np::layout_stride, np::tensor_vector<int, hook_allocator<int>>>;

TEST(NpStridedOwner, LogicalSizeAndAddressedSpan) {
    auto result = np::make_tensor<owner>(std::array{2, 3}, std::array{8, 1});
    ASSERT_TRUE(result);
    EXPECT_EQ(result->size(), 6u);
    EXPECT_EQ(result->required_span_size(), 11u);
    EXPECT_EQ(result->storage_size(), 11u);
    EXPECT_TRUE(result->has_storage());
    auto view = result->view();
    for (size_t i = 0; i != 11; ++i)
        EXPECT_EQ(view.data_handle()[i], 0);
    view(1, 2) = 42;
    EXPECT_EQ(view.data_handle()[10], 42);
    EXPECT_EQ(std::as_const(*result).view()(1, 2), 42);
    for (size_t i = 3; i != 8; ++i)
        view.data_handle()[i] = -9;
    np::eval_to(view, np::as_expr(view) + 1);
    EXPECT_EQ(view(1, 2), 43);
    EXPECT_EQ(np::sum(view), 48);
    for (size_t i = 3; i != 8; ++i)
        EXPECT_EQ(view.data_handle()[i], -9);
    const auto packed = np::eval(np::as_expr(view) * 2);
    ASSERT_TRUE(packed);
    EXPECT_EQ(packed->storage_size(), 6u);
    EXPECT_EQ(packed->view()(1, 2), 86);
}

TEST(NpStridedOwner, StaticShapeRetainsEachRuntimeMapping) {
    auto row = np::make_tensor<fixed_owner>(std::array{2, 3}, std::array{8, 1});
    auto column = np::make_tensor<fixed_owner>(std::array{2, 3}, std::array{1, 4});
    ASSERT_TRUE(row);
    ASSERT_TRUE(column);
    EXPECT_EQ(row->view().stride(0), 8u);
    EXPECT_EQ(column->view().stride(1), 4u);
    EXPECT_EQ(column->required_span_size(), 10u);
    column->view()(1, 2) = 5;
    auto copied = *column;
    EXPECT_EQ(copied.view()(1, 2), 5);
    EXPECT_NE(copied.view().data_handle(), column->view().data_handle());
    *row = copied;
    EXPECT_EQ(row->view().stride(1), 4u);
    EXPECT_EQ(row->storage_size(), 10u);
}

TEST(NpStridedOwner, MovePreservesMappingAndQueryableSource) {
    auto result = np::make_tensor<owner>(std::array{2, 3}, std::array{8, 1});
    ASSERT_TRUE(result);
    result->view()(1, 2) = 9;
    auto *data = result->view().data_handle();
    auto moved = std::move(*result);
    EXPECT_EQ(moved.view().data_handle(), data);
    EXPECT_EQ(moved.view()(1, 2), 9);
    EXPECT_EQ(result->size(), 6u);
    EXPECT_EQ(result->required_span_size(), 11u);
    EXPECT_EQ(result->storage_size(), 0u);
    EXPECT_FALSE(result->has_storage());
    EXPECT_FALSE(result->try_view());
    const auto copied_source = *result;
    EXPECT_FALSE(copied_source.has_storage());
    auto target = np::make_tensor<owner>(std::array{1, 1}, std::array{1, 1});
    ASSERT_TRUE(target);
    *target = std::move(moved);
    EXPECT_EQ(target->view().data_handle(), data);
    EXPECT_EQ(target->view().stride(0), 8u);
    EXPECT_FALSE(moved.has_storage());
}

TEST(NpStridedOwner, EmptyAndRankZero) {
    auto empty = np::make_tensor<owner>(std::array{2, 0}, std::array{4, 1}, wjr::default_construct);
    ASSERT_TRUE(empty);
    EXPECT_TRUE(empty->empty());
    EXPECT_EQ(empty->required_span_size(), 0u);
    EXPECT_TRUE(empty->has_storage());
    EXPECT_TRUE(empty->try_view());
    using scalar = np::tensor<int, np::extents<size_t>, np::layout_stride>;
    auto single = np::make_tensor<scalar>(std::array<int, 0>{}, std::array<int, 0>{});
    ASSERT_TRUE(single);
    EXPECT_EQ(single->size(), 1u);
    EXPECT_EQ(single->required_span_size(), 1u);
    single->view()() = 17;
    EXPECT_EQ(np::sum(single->view()), 17);
}

TEST(NpStridedOwner, ExactSpanAdoptionDoesNotConsumeOnFailure) {
    owner::container_type storage(11, wjr::default_construct);
    auto *data = storage.data();
    data[10] = 7;
    auto rejected = np::adopt_tensor<owner>(std::array{2, 3}, std::array{7, 1}, std::move(storage));
    ASSERT_FALSE(rejected);
    EXPECT_EQ(rejected.error(), np::shape_error::storage_size_mismatch);
    EXPECT_EQ(storage.data(), data);
    EXPECT_EQ(storage.size(), 11u);
    const auto accepted =
        np::adopt_tensor<owner>(std::array{2, 3}, std::array{8, 1}, std::move(storage));
    ASSERT_TRUE(accepted);
    EXPECT_EQ(accepted->view().data_handle(), data);
    EXPECT_EQ(accepted->view()(1, 2), 7);
    EXPECT_TRUE(storage.empty());
}

TEST(NpStridedOwner, ValidationAndAllocatorLimitBeforeAllocation) {
    allocation_state state;
    const hook_allocator<int> allocator(state);
    const auto negative = np::make_tensor<hooked_owner>(std::array{-1, 3}, std::array{8, 1},
                                                        std::allocator_arg, allocator);
    ASSERT_FALSE(negative);
    EXPECT_EQ(negative.error(), np::shape_error::negative_extent);
    const auto zero_stride = np::make_tensor<hooked_owner>(std::array{2, 3}, std::array{0, 1},
                                                           std::allocator_arg, allocator);
    ASSERT_FALSE(zero_stride);
    EXPECT_EQ(zero_stride.error(), np::shape_error::nonpositive_stride);
    const auto order = np::make_tensor<hooked_owner>(std::array{2, 2}, std::array{2, 3},
                                                     std::allocator_arg, allocator);
    ASSERT_FALSE(order);
    EXPECT_EQ(order.error(), np::shape_error::unsupported_stride_order);
    state.limit = 10;
    const auto limit = np::make_tensor<hooked_owner>(std::array{2, 3}, std::array{8, 1},
                                                     std::allocator_arg, allocator);
    ASSERT_FALSE(limit);
    EXPECT_EQ(limit.error(), np::shape_error::container_size_limit);
    using narrow_owner = np::tensor<int, np::dextents<int, 2>, np::layout_stride>;
    const auto overflow = np::make_tensor<narrow_owner>(
        std::array{2, 3}, std::array{std::numeric_limits<int>::max(), 1});
    ASSERT_FALSE(overflow);
    EXPECT_EQ(overflow.error(), np::shape_error::index_overflow);
    EXPECT_EQ(state.allocations, 0);
}

TEST(NpStridedOwner, EntireSpanUsesAllocatorLifetimeHooks) {
    allocation_state state;
    const hook_allocator<int> allocator(state);
    {
        auto result =
            np::make_tensor<hooked_owner>(std::array{2, 3}, std::array{8, 1},
                                          wjr::default_construct, std::allocator_arg, allocator);
        ASSERT_TRUE(result);
        EXPECT_EQ(state.allocations, 1);
        EXPECT_EQ(state.allocated, 11u);
        EXPECT_EQ(state.constructed, 11);
        EXPECT_EQ(result->view().data_handle()[5], -7);
        EXPECT_EQ(result->view()(1, 2), -7);
    }
    EXPECT_EQ(state.destroyed, 11);
    EXPECT_EQ(state.deallocations, 1);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(NpStridedOwner, ConstructionFailureDestroysCompletedSpanPrefix) {
    allocation_state state;
    state.budget = 4;
    const hook_allocator<int> allocator(state);
    EXPECT_THROW(
        (np::make_tensor<hooked_owner>(std::array{2, 3}, std::array{8, 1}, wjr::default_construct,
                                       std::allocator_arg, allocator)),
        std::runtime_error);
    EXPECT_EQ(state.constructed, 4);
    EXPECT_EQ(state.destroyed, 4);
    EXPECT_EQ(state.allocations, state.deallocations);
}
#endif

} // namespace
