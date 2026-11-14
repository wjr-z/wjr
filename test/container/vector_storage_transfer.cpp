#include <memory>
#include <set>
#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/vector.hpp>

namespace transfer_test {
struct state {
    std::set<void *> live;
    int move_budget = -1;
    int hook_budget = -1;
    int allocations = 0;
    int deallocations = 0;
    int invalid_destroys = 0;
    int constructed = 0;
    int destroyed = 0;
};

template <wjr::relocate_t Mode, bool Nothrow = false>
struct value {
    state *owner;
    int number;
    value(state &counts, int n) noexcept : owner(&counts), number(n) { owner->live.insert(this); }
    value(const value &other) : value(*other.owner, other.number) {}
    value(value &&other) noexcept(Nothrow) : owner(other.owner), number(other.number) {
        if constexpr (!Nothrow) {
            if (owner->move_budget == 0)
                throw std::runtime_error("move failed");
            if (owner->move_budget > 0)
                --owner->move_budget;
        }
        owner->live.insert(this);
        other.number = -1;
    }
    value &operator=(value &&) = default;
    ~value() {
        if (owner->live.erase(this) != 1)
            ++owner->invalid_destroys;
    }
};

template <typename T>
struct allocator {
    using value_type = T;
    state *owner;
    explicit allocator(state &counts) noexcept : owner(&counts) {}
    template <typename U>
    allocator(const allocator<U> &other) noexcept : owner(other.owner) {}
    T *allocate(std::size_t count) {
        auto *result = std::allocator<T>{}.allocate(count);
        ++owner->allocations;
        return result;
    }
    void deallocate(T *ptr, std::size_t count) noexcept {
        ++owner->deallocations;
        std::allocator<T>{}.deallocate(ptr, count);
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
    bool operator==(const allocator &) const = default;
};

struct relocation_only {
    inline static int live = 0;
    int number = 42;
    relocation_only() noexcept { ++live; }
    relocation_only(const relocation_only &) = delete;
    relocation_only(relocation_only &&) = delete;
    ~relocation_only() noexcept(false) { --live; }
};
} // namespace transfer_test

namespace wjr {
template <relocate_t Mode, bool Nothrow>
struct get_relocate_mode<transfer_test::value<Mode, Nothrow>> {
    static constexpr relocate_t value = Mode;
};
template <>
struct get_relocate_mode<transfer_test::relocation_only> {
    static constexpr relocate_t value = relocate_t::trivial;
};
} // namespace wjr

namespace {
using transfer_test::allocator;
using transfer_test::state;
using transfer_test::value;

template <typename Vector>
void fill(Vector &values, state &counts, int count) {
    for (int i = 0; i < count; ++i)
        values.emplace_back(counts, i);
}

template <typename T>
void check_move_failures(bool fail_hook) {
    using vector = wjr::small_vector<T, 4, allocator<T>>;
    for (int budget = 0; budget < 3; ++budget) {
        state counts;
        {
            vector source{allocator<T>(counts)};
            fill(source, counts, 3);
            const auto original_live = counts.live;
            (fail_hook ? counts.hook_budget : counts.move_budget) = budget;
            EXPECT_THROW(vector destination(std::move(source)), std::runtime_error);
            EXPECT_EQ(source.size(), 3);
            EXPECT_EQ(counts.live, original_live);
            counts.hook_budget = counts.move_budget = -1;
            vector destination{allocator<T>(counts)};
            fill(destination, counts, 6);
            (fail_hook ? counts.hook_budget : counts.move_budget) = budget;
            EXPECT_THROW(destination = std::move(source), std::runtime_error);
            EXPECT_TRUE(destination.empty());
            EXPECT_EQ(source.size(), 3);
            EXPECT_EQ(counts.live, original_live);
            counts.hook_budget = counts.move_budget = -1;
            destination.emplace_back(counts, 99);
            destination = std::move(source);
            EXPECT_TRUE(source.empty());
            EXPECT_EQ(destination.size(), 3);
        }
        EXPECT_TRUE(counts.live.empty());
        EXPECT_EQ(counts.invalid_destroys, 0);
        EXPECT_EQ(counts.allocations, counts.deallocations);
        EXPECT_EQ(counts.constructed, counts.destroyed);
    }
}

TEST(vector_storage_transfer, throwing_inline_moves_preserve_source_lifetimes) {
    check_move_failures<value<wjr::relocate_t::normal>>(false);
    check_move_failures<value<wjr::relocate_t::maybe_trivial>>(false);
}

TEST(vector_storage_transfer, throwing_allocator_hooks_propagate_from_nothrow_values) {
    check_move_failures<value<wjr::relocate_t::normal, true>>(true);
    check_move_failures<value<wjr::relocate_t::maybe_trivial, true>>(true);
    using vector = wjr::small_vector<value<wjr::relocate_t::normal, true>, 4,
                                     allocator<value<wjr::relocate_t::normal, true>>>;
    static_assert(!std::is_nothrow_move_constructible_v<vector>);
    static_assert(!std::is_nothrow_swappable_v<vector>);
    using grow_storage = typename vector::storage_type::grow_storage_type;
    static_assert(noexcept(std::declval<vector &>().take_storage(std::declval<grow_storage &>())));
}

TEST(vector_storage_transfer, heap_take_uses_pointer_transfer_for_throwing_values_and_hooks) {
    using element = value<wjr::relocate_t::maybe_trivial>;
    using vector = wjr::small_vector<element, 4, allocator<element>>;
    state counts;
    {
        vector source{allocator<element>(counts)};
        fill(source, counts, 6);
        auto *const original_data = source.data();
        counts.hook_budget = counts.move_budget = 0;
        vector destination(std::move(source));
        EXPECT_TRUE(source.empty());
        EXPECT_EQ(destination.data(), original_data);
        EXPECT_EQ(destination.size(), 6);
        source = std::move(destination);
        EXPECT_TRUE(destination.empty());
        EXPECT_EQ(source.data(), original_data);
        counts.hook_budget = counts.move_budget = -1;
        destination.emplace_back(counts, 9);
    }
    EXPECT_TRUE(counts.live.empty());
    EXPECT_EQ(counts.invalid_destroys, 0);
    EXPECT_EQ(counts.allocations, counts.deallocations);
    EXPECT_EQ(counts.constructed, counts.destroyed);
}

template <typename T>
void check_swaps(bool fail_hook) {
    using vector = wjr::small_vector<T, 4, allocator<T>>;
    for (int left_count : {0, 2, 3, 6}) {
        for (int right_count : {0, 2, 3, 6}) {
            for (int budget = 0; budget < 10; ++budget) {
                SCOPED_TRACE(testing::Message()
                             << left_count << ',' << right_count << ',' << budget);
                state counts;
                {
                    vector left{allocator<T>(counts)}, right{allocator<T>(counts)};
                    fill(left, counts, left_count);
                    fill(right, counts, right_count);
                    (fail_hook ? counts.hook_budget : counts.move_budget) = budget;
                    try {
                        if (budget % 2 == 0)
                            wjr::swap(left, right);
                        else
                            left.swap(right);
                        EXPECT_EQ(left.size(), right_count);
                        EXPECT_EQ(right.size(), left_count);
                    } catch (const std::runtime_error &) {
                    }
                    EXPECT_EQ(counts.live.size(), left.size() + right.size());
                    counts.hook_budget = counts.move_budget = -1;
                    left.clear();
                    right.clear();
                    left.emplace_back(counts, 7);
                    right.emplace_back(counts, 8);
                }
                EXPECT_TRUE(counts.live.empty());
                EXPECT_EQ(counts.invalid_destroys, 0);
                EXPECT_EQ(counts.allocations, counts.deallocations);
                EXPECT_EQ(counts.constructed, counts.destroyed);
            }
        }
    }
}

TEST(vector_storage_transfer, throwing_swaps_keep_all_live_ranges_valid) {
    check_swaps<value<wjr::relocate_t::normal>>(false);
    check_swaps<value<wjr::relocate_t::maybe_trivial>>(false);
    check_swaps<value<wjr::relocate_t::maybe_trivial, true>>(true);
}

TEST(vector_storage_transfer, explicit_trivial_inline_relocation_accepts_deleted_moves) {
    using element = transfer_test::relocation_only;
    using vector = wjr::small_vector<element, 4>;
    static_assert(std::is_nothrow_move_constructible_v<vector>);
    {
        vector source(3);
        source[1].number = 17;
        vector destination(std::move(source));
        EXPECT_TRUE(source.empty());
        EXPECT_EQ(element::live, 3);
        EXPECT_EQ(destination[1].number, 17);
        destination.reserve(12);
        destination.shrink_to_fit();
        EXPECT_EQ(destination[1].number, 17);
        EXPECT_EQ(element::live, 3);
        destination.emplace_back();
        destination.emplace_back();
        EXPECT_EQ(destination.size(), 5);
        EXPECT_EQ(destination[1].number, 17);
        EXPECT_EQ(element::live, 5);
    }
    EXPECT_EQ(element::live, 0);
}

TEST(vector_storage_transfer, nested_vector_growth_rebinds_inline_buffer_pointers) {
    using small = wjr::small_vector<int, 4>;
    static_assert(wjr::get_relocate_mode_v<small> != wjr::relocate_t::trivial);
    static_assert(wjr::get_relocate_mode_v<wjr::small_vector<int, 0>> == wjr::relocate_t::trivial);
    wjr::vector<small> values;
    values.reserve(1);
    values.emplace_back(3, 42);
    auto *const original_inline = values.front().data();
    values.reserve(8);
    EXPECT_NE(values.front().data(), original_inline);
    ASSERT_EQ(values.front().size(), 3);
    EXPECT_EQ(values.front()[1], 42);
    values.front().push_back(17);
    EXPECT_EQ(values.front().back(), 17);
}
} // namespace
