#include <memory>
#include <set>
#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/vector.hpp>

namespace capacity_test {

struct state {
    std::set<void *> live;
    int allocations = 0;
    int deallocations = 0;
    int invalid_destroys = 0;
    int move_budget = -1;
    int hook_budget = -1;
};

/** @brief Relocation ends the old lifetime without invoking a move or destructor. */
struct relocation_only {
    inline static int live = 0;
    int number = 42;
    relocation_only() noexcept { ++live; }
    relocation_only(const relocation_only &) = delete;
    relocation_only(relocation_only &&) = delete;
    ~relocation_only() noexcept(false) { --live; }
};

template <wjr::relocate_t Mode, bool Nothrow = false>
struct value {
    state *owner;
    int number;
    value(state &counts, int n) noexcept : owner(&counts), number(n) {}
    value(const value &) = delete;
    value(value &&other) noexcept(Nothrow) : owner(other.owner), number(other.number) {
        if constexpr (!Nothrow) {
            if (owner->move_budget == 0)
                throw std::runtime_error("move failed");
            if (owner->move_budget > 0)
                --owner->move_budget;
        }
        other.number = -1;
    }
    value &operator=(value &&) = default;
};

template <typename T>
struct allocator {
    using value_type = T;
    state *owner;
    explicit allocator(state &counts) noexcept : owner(&counts) {}
    template <typename U>
    allocator(const allocator<U> &other) noexcept : owner(other.owner) {}
    T *allocate(size_t count) {
        auto *result = std::allocator<T>{}.allocate(count);
        ++owner->allocations;
        return result;
    }
    void deallocate(T *ptr, size_t count) noexcept {
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
        owner->live.insert(ptr);
    }
    template <typename U>
    void destroy(U *ptr) noexcept {
        if (owner->live.erase(ptr) != 1)
            ++owner->invalid_destroys;
        std::destroy_at(ptr);
    }
    bool operator==(const allocator &) const = default;
};

template <typename Vector>
void check_transaction(bool shrink, bool fail_move) {
    for (int budget = 0; budget != 5; ++budget) {
        state counts;
        {
            Vector values{typename Vector::allocator_type(counts)};
            values.reserve(16);
            for (int i = 0; i != 5; ++i)
                values.emplace_back(counts, i);
            auto *const original_data = values.data();
            const auto original_capacity = values.capacity();
            const auto original_live = counts.live;
            const int original_blocks = counts.allocations - counts.deallocations;
            (fail_move ? counts.move_budget : counts.hook_budget) = budget;
            EXPECT_THROW((shrink ? values.shrink_to_fit() : values.reserve(32)),
                         std::runtime_error);
            EXPECT_EQ(values.data(), original_data);
            EXPECT_EQ(values.capacity(), original_capacity);
            EXPECT_EQ(values.size(), 5);
            EXPECT_EQ(counts.live, original_live);
            EXPECT_EQ(counts.allocations - counts.deallocations, original_blocks);
            EXPECT_EQ(counts.invalid_destroys, 0);
            counts.move_budget = counts.hook_budget = -1;
            if (shrink)
                values.shrink_to_fit();
            else
                values.reserve(32);
            EXPECT_EQ(values.size(), 5);
            EXPECT_EQ(counts.live.size(), 5);
        }
        EXPECT_TRUE(counts.live.empty());
        EXPECT_EQ(counts.allocations, counts.deallocations);
        EXPECT_EQ(counts.invalid_destroys, 0);
    }
}

} // namespace capacity_test

namespace wjr {

template <>
struct get_relocate_mode<capacity_test::relocation_only> {
    static constexpr relocate_t value = relocate_t::trivial;
};

template <relocate_t Mode, bool Nothrow>
struct get_relocate_mode<capacity_test::value<Mode, Nothrow>> {
    static constexpr relocate_t value = Mode;
};

} // namespace wjr

namespace {

TEST(vector_capacity, explicit_trivial_relocation_does_not_require_move_or_nothrow_destruction) {
    using capacity_test::relocation_only;
    {
        wjr::vector<relocation_only> values(3);
        values[1].number = 17;
        values.reserve(16);
        EXPECT_EQ(values.size(), 3);
        EXPECT_EQ(relocation_only::live, 3);
        EXPECT_EQ(values[1].number, 17);
        values.shrink_to_fit();
        EXPECT_EQ(values.size(), 3);
        EXPECT_EQ(values.capacity(), 3);
        EXPECT_EQ(relocation_only::live, 3);
        EXPECT_EQ(values[0].number, 42);
        EXPECT_EQ(values[1].number, 17);
        EXPECT_EQ(values[2].number, 42);
    }
    EXPECT_EQ(relocation_only::live, 0);
}

TEST(vector_capacity, throwing_moves_preserve_source_lifetimes_and_release_new_storage) {
    using namespace capacity_test;
    for (bool shrink : {false, true}) {
        using normal = value<wjr::relocate_t::normal>;
        using interleaved = value<wjr::relocate_t::maybe_trivial>;
        check_transaction<wjr::vector<normal, allocator<normal>>>(shrink, true);
        check_transaction<wjr::vector<interleaved, allocator<interleaved>>>(shrink, true);
    }
}

TEST(vector_capacity, throwing_hooks_preserve_source_lifetimes_and_release_new_storage) {
    using namespace capacity_test;
    using element = value<wjr::relocate_t::maybe_trivial, true>;
    for (bool shrink : {false, true}) {
        check_transaction<wjr::vector<element, allocator<element>>>(shrink, false);
        check_transaction<wjr::small_vector<element, 2, allocator<element>>>(shrink, false);
    }
}

TEST(vector_capacity, potentially_throwing_inline_shrink_is_a_nonbinding_request) {
    using namespace capacity_test;
    using element = value<wjr::relocate_t::maybe_trivial, true>;
    state counts;
    {
        wjr::small_vector<element, 4, allocator<element>> values{allocator<element>(counts)};
        values.reserve(16);
        values.emplace_back(counts, 42);
        auto *const original_data = values.data();
        const auto original_capacity = values.capacity();
        counts.hook_budget = 0;
        EXPECT_NO_THROW(values.shrink_to_fit());
        EXPECT_EQ(values.data(), original_data);
        EXPECT_EQ(values.capacity(), original_capacity);
        EXPECT_EQ(values.front().number, 42);
        counts.hook_budget = -1;
    }
    EXPECT_EQ(counts.allocations, counts.deallocations);
    EXPECT_TRUE(counts.live.empty());
}

} // namespace
