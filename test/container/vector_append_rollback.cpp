#include <wjr/vector.hpp>

#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#if !defined(WJR_DISABLE_EXCEPTIONS)
namespace {

struct append_state {
    int live = 0, tokens = 0;
    int allocations = 0, deallocations = 0;
    int created = 0, destroyed = 0;
    int copy_budget = -1, move_budget = -1, default_budget = -1, hook_budget = -1;
    int assignment_budget = -1;
    bool fail_allocate = false;
};

append_state *active_state = nullptr;

struct active_scope {
    explicit active_scope(append_state &state) : previous(std::exchange(active_state, &state)) {}
    ~active_scope() { active_state = previous; }
    append_state *previous;
};

void consume(int &budget) {
    if (budget == 0) {
        throw std::runtime_error("append failed");
    }
    if (budget > 0) {
        --budget;
    }
}

template <bool ThrowingMove>
struct append_value {
    append_state *state;
    int value;
    append_value() : state(active_state), value(17) {
        consume(state->default_budget);
        ++state->live;
    }
    append_value(append_state &s, int v) noexcept : state(&s), value(v) { ++state->live; }
    append_value(const append_value &other) : state(other.state), value(other.value) {
        consume(state->copy_budget);
        ++state->live;
    }
    append_value(append_value &&other) noexcept(!ThrowingMove) : state(other.state) {
        if constexpr (ThrowingMove) {
            consume(state->move_budget);
        }
        value = std::exchange(other.value, -1);
        ++state->live;
    }
    append_value &operator=(const append_value &) = delete;
    append_value &operator=(append_value &&) = delete;
    ~append_value() { --state->live; }
};

template <typename T>
struct append_allocator {
    using value_type = T;
    append_state *state;
    explicit append_allocator(append_state &s) noexcept : state(&s) {}
    template <typename U>
    append_allocator(const append_allocator<U> &other) noexcept : state(other.state) {}
    T *allocate(std::size_t n) {
        if (state->fail_allocate) {
            throw std::bad_alloc{};
        }
        auto *ptr = std::allocator<T>{}.allocate(n);
        ++state->allocations;
        return ptr;
    }
    void deallocate(T *ptr, std::size_t n) noexcept {
        ++state->deallocations;
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        consume(state->hook_budget);
        std::construct_at(ptr, std::forward<Args>(args)...);
        ++state->created;
    }
    void destroy(T *ptr) noexcept {
        ++state->destroyed;
        std::destroy_at(ptr);
    }
    friend bool operator==(const append_allocator &, const append_allocator &) = default;
};

enum class operation { range, fill, resize_fill, resize_default, append_default };
enum class failure { allocation, copy, move, default_constructor, hook };

template <typename Vector>
void verify_failure(operation op, failure mode, int budget, std::size_t initial = 3) {
    append_state state;
    active_scope scope(state);
    {
        using value_type = typename Vector::value_type;
        std::array<value_type, 8> source;
        value_type fill_source(state, 99);
        {
            Vector target{typename Vector::allocator_type(state)};
            for (std::size_t i = 0; i != initial; ++i) {
                target.emplace_back(state, static_cast<int>(i));
            }
            auto *const original_data = target.data();
            const auto original_capacity = target.capacity();
            const auto original_blocks = state.allocations - state.deallocations;
            state.fail_allocate = mode == failure::allocation;
            state.copy_budget = mode == failure::copy ? budget : -1;
            state.move_budget = mode == failure::move ? budget : -1;
            state.default_budget = mode == failure::default_constructor ? budget : -1;
            state.hook_budget = mode == failure::hook ? budget : -1;
            const auto append = [&] {
                switch (op) {
                case operation::range:
                    target.append(source.begin(), source.end());
                    break;
                case operation::fill:
                    target.append(8, fill_source);
                    break;
                case operation::resize_fill:
                    target.resize(initial + 8, fill_source);
                    break;
                case operation::resize_default:
                    target.resize(initial + 8);
                    break;
                case operation::append_default:
                    target.append(8, wjr::default_construct);
                    break;
                }
            };
            if (mode == failure::allocation) {
                EXPECT_THROW(append(), std::bad_alloc);
            } else {
                EXPECT_THROW(append(), std::runtime_error);
            }
            state.fail_allocate = false;
            state.copy_budget = state.move_budget = state.default_budget = state.hook_budget = -1;
            EXPECT_EQ(target.size(), initial);
            EXPECT_EQ(target.data(), original_data);
            EXPECT_EQ(target.capacity(), original_capacity);
            EXPECT_EQ(state.allocations - state.deallocations, original_blocks);
            EXPECT_EQ(state.live, static_cast<int>(initial) + 9);
            if (mode != failure::move && !(mode == failure::hook && budget >= 8)) {
                for (std::size_t i = 0; i != initial; ++i) {
                    EXPECT_EQ(target[i].value, static_cast<int>(i));
                }
            }
            append();
            ASSERT_EQ(target.size(), initial + 8);
            const int expected = op == operation::fill || op == operation::resize_fill ? 99 : 17;
            for (std::size_t i = initial; i != target.size(); ++i) {
                EXPECT_EQ(target[i].value, expected);
            }
        }
        EXPECT_EQ(state.live, 9);
    }
    EXPECT_EQ(state.live, 0);
    EXPECT_EQ(state.allocations, state.deallocations);
    EXPECT_EQ(state.created, state.destroyed);
}

using value_type = append_value<false>;
using vector_type = wjr::vector<value_type, append_allocator<value_type>>;
using small_type = wjr::small_vector<value_type, 4, append_allocator<value_type>>;
static_assert(!std::is_move_assignable_v<value_type>);

TEST(vector_append_rollback, allocation_failure_keeps_original_storage) {
    for (auto op : {operation::range, operation::fill, operation::resize_fill,
                    operation::resize_default, operation::append_default}) {
        verify_failure<vector_type>(op, failure::allocation, 0);
        verify_failure<small_type>(op, failure::allocation, 0);
    }
}

TEST(vector_append_rollback, every_incoming_copy_failure_releases_storage) {
    for (auto op : {operation::range, operation::fill, operation::resize_fill}) {
        for (int budget = 0; budget != 8; ++budget) {
            verify_failure<vector_type>(op, failure::copy, budget);
            verify_failure<small_type>(op, failure::copy, budget);
        }
    }
}

TEST(vector_append_rollback, every_default_constructor_failure_releases_storage) {
    for (auto op : {operation::resize_default, operation::append_default}) {
        for (int budget = 0; budget != 8; ++budget) {
            verify_failure<vector_type>(op, failure::default_constructor, budget);
            verify_failure<small_type>(op, failure::default_constructor, budget);
        }
    }
}

TEST(vector_append_rollback, old_element_move_failure_releases_both_new_regions) {
    using throwing = append_value<true>;
    using vector = wjr::vector<throwing, append_allocator<throwing>>;
    for (auto op : {operation::range, operation::fill, operation::resize_fill,
                    operation::resize_default, operation::append_default}) {
        for (int budget = 0; budget != 3; ++budget) {
            verify_failure<vector>(op, failure::move, budget);
        }
    }
}

TEST(vector_append_rollback, every_allocator_hook_failure_keeps_old_lifetimes) {
    for (auto op : {operation::range, operation::fill, operation::resize_fill,
                    operation::resize_default, operation::append_default}) {
        for (int budget = 0; budget != 11; ++budget) {
            verify_failure<vector_type>(op, failure::hook, budget);
            verify_failure<small_type>(op, failure::hook, budget);
        }
    }
}

struct token {
    append_state *state;
    explicit token(append_state &s) : state(&s) { ++state->tokens; }
    ~token() { --state->tokens; }
};

/** @brief Keep the bulk append model construct-only while testing middle insertion separately. */
template <bool ThrowingMove>
struct emplace_value : append_value<ThrowingMove> {
    using base = append_value<ThrowingMove>;
    using base::base;
    emplace_value() = default;
    emplace_value(const emplace_value &) = default;
    emplace_value(emplace_value &&) noexcept(!ThrowingMove) = default;
    emplace_value &operator=(emplace_value &&other) {
        consume(this->state->assignment_budget);
        this->value = std::exchange(other.value, -1);
        return *this;
    }
};

enum class emplace_site { back, middle, end };

template <typename Vector>
void verify_emplace_failure(emplace_site site, failure mode, int budget, bool spare = false) {
    append_state state;
    active_scope scope(state);
    {
        using value = typename Vector::value_type;
        value source(state, 99);
        {
            Vector target(typename Vector::size_type(spare ? 8 : 3), wjr::in_place_reserve,
                          typename Vector::allocator_type(state));
            const auto initial = spare ? 3u : target.capacity();
            for (unsigned int i = 0; i != initial; ++i) {
                target.emplace_back(state, static_cast<int>(i));
            }
            auto *const original_data = target.data();
            const auto capacity = target.capacity();
            const auto blocks = state.allocations - state.deallocations;
            state.fail_allocate = mode == failure::allocation;
            state.copy_budget = mode == failure::copy ? budget : -1;
            state.move_budget = mode == failure::move ? budget : -1;
            state.default_budget = mode == failure::default_constructor ? budget : -1;
            state.hook_budget = mode == failure::hook ? budget : -1;
            const auto insert = [&] {
                if (site == emplace_site::back) {
                    if (mode == failure::default_constructor) {
                        target.emplace_back();
                    } else {
                        target.emplace_back(source);
                    }
                } else {
                    const auto position =
                        site == emplace_site::middle ? target.cbegin() + 1 : target.cend();
                    if (mode == failure::default_constructor) {
                        target.emplace(position);
                    } else {
                        target.emplace(position, source);
                    }
                }
            };
            if (mode == failure::allocation) {
                EXPECT_THROW(insert(), std::bad_alloc);
            } else {
                EXPECT_THROW(insert(), std::runtime_error);
            }
            state.fail_allocate = false;
            state.copy_budget = state.move_budget = state.default_budget = state.hook_budget = -1;
            EXPECT_EQ(target.data(), original_data);
            EXPECT_EQ(target.capacity(), capacity);
            EXPECT_EQ(target.size(), initial);
            EXPECT_EQ(state.allocations - state.deallocations, blocks);
            EXPECT_EQ(state.live, static_cast<int>(initial) + 1);
            insert();
            EXPECT_EQ(target.size(), initial + 1);
        }
        EXPECT_EQ(state.live, 1);
    }
    EXPECT_EQ(state.live, 0);
    EXPECT_EQ(state.allocations, state.deallocations);
    EXPECT_EQ(state.created, state.destroyed);
}

TEST(vector_emplace_rollback, allocation_and_incoming_construction_failures) {
    using value = emplace_value<false>;
    using vector = wjr::vector<value, append_allocator<value>>;
    using small = wjr::small_vector<value, 4, append_allocator<value>>;
    for (const auto site : {emplace_site::back, emplace_site::middle, emplace_site::end}) {
        for (const auto mode : {failure::allocation, failure::copy, failure::default_constructor}) {
            verify_emplace_failure<vector>(site, mode, 0);
            verify_emplace_failure<small>(site, mode, 0);
        }
    }
}

TEST(vector_emplace_rollback, old_element_moves_keep_every_live_object_owned) {
    using value = emplace_value<true>;
    using vector = wjr::vector<value, append_allocator<value>>;
    for (const auto site : {emplace_site::back, emplace_site::middle, emplace_site::end}) {
        for (int budget = 0; budget != 3; ++budget) {
            verify_emplace_failure<vector>(site, failure::move, budget);
        }
    }
}

TEST(vector_emplace_rollback, spare_capacity_incoming_failures_leave_no_extra_objects) {
    using value = emplace_value<false>;
    using vector = wjr::vector<value, append_allocator<value>>;
    using small = wjr::small_vector<value, 4, append_allocator<value>>;
    for (const auto site : {emplace_site::back, emplace_site::middle, emplace_site::end}) {
        for (const auto mode : {failure::copy, failure::default_constructor, failure::hook}) {
            verify_emplace_failure<vector>(site, mode, 0, true);
            verify_emplace_failure<small>(site, mode, 0, true);
        }
    }
    verify_emplace_failure<vector>(emplace_site::middle, failure::hook, 1, true);
    verify_emplace_failure<small>(emplace_site::middle, failure::hook, 1, true);
}

TEST(vector_emplace_rollback,
     throwing_incoming_with_trusted_relocation_needs_only_storage_cleanup) {
    append_state state;
    {
        append_value<false> source(state, 99);
        {
            wjr::vector<append_value<false>> target(2, wjr::in_place_reserve);
            target.emplace_back(state, 1);
            target.emplace_back(state, 2);
            auto *const original = target.data();
            state.copy_budget = 0;
            EXPECT_THROW(target.emplace_back(source), std::runtime_error);
            EXPECT_EQ(target.size(), 2);
            EXPECT_EQ(target.data(), original);
            EXPECT_EQ(state.live, 3);
            EXPECT_EQ(target[0].value, 1);
            EXPECT_EQ(target[1].value, 2);
            state.copy_budget = -1;
            target.emplace_back(source);
            EXPECT_EQ(target.back().value, 99);
        }
        EXPECT_EQ(state.live, 1);
    }
    EXPECT_EQ(state.live, 0);
}

TEST(vector_emplace_rollback, allocator_hooks_cover_incoming_prefix_and_suffix) {
    using value = emplace_value<false>;
    using vector = wjr::vector<value, append_allocator<value>>;
    using small = wjr::small_vector<value, 4, append_allocator<value>>;
    for (const auto site : {emplace_site::back, emplace_site::middle, emplace_site::end}) {
        for (int budget = 0; budget != 4; ++budget) {
            verify_emplace_failure<vector>(site, failure::hook, budget);
        }
        for (int budget = 0; budget != 5; ++budget) {
            verify_emplace_failure<small>(site, failure::hook, budget);
        }
    }
}

template <typename Vector>
void verify_spare_assignment_failure(int budget) {
    append_state state;
    {
        const auto allocator = [&] {
            using allocator_type = typename Vector::allocator_type;
            if constexpr (std::is_constructible_v<allocator_type, append_state &>) {
                return allocator_type(state);
            } else {
                return allocator_type();
            }
        }();
        Vector target(typename Vector::size_type(8), wjr::in_place_reserve, allocator);
        for (int i = 0; i != 3; ++i) {
            target.emplace_back(state, i);
        }
        state.assignment_budget = budget;
        EXPECT_THROW(target.emplace(target.cbegin() + 1, state, 9), std::runtime_error);
        state.assignment_budget = -1;
        EXPECT_EQ(target.size(), 3);
        EXPECT_EQ(state.live, 3);
        target.clear();
        EXPECT_EQ(state.live, 0);
        target.emplace_back(state, 10);
        EXPECT_EQ(target.front().value, 10);
    }
    EXPECT_EQ(state.live, 0);
    EXPECT_EQ(state.allocations, state.deallocations);
    EXPECT_EQ(state.created, state.destroyed);
}

TEST(vector_emplace_rollback, spare_capacity_assignment_failure_destroys_uncommitted_tail) {
    using value = emplace_value<false>;
    using vector = wjr::vector<value, append_allocator<value>>;
    using small = wjr::small_vector<value, 4, append_allocator<value>>;
    using inplace = wjr::inplace_vector<value, 8>;
    for (int budget = 0; budget != 2; ++budget) {
        verify_spare_assignment_failure<vector>(budget);
        verify_spare_assignment_failure<small>(budget);
        verify_spare_assignment_failure<inplace>(budget);
    }
}

TEST(vector_emplace_rollback, move_only_hook_failure_preserves_old_object_lifetimes) {
    using owner = std::unique_ptr<token>;
    for (const auto site : {emplace_site::back, emplace_site::middle, emplace_site::end}) {
        for (int budget = 0; budget != 4; ++budget) {
            append_state state;
            {
                owner source = std::make_unique<token>(state);
                {
                    wjr::vector<owner, append_allocator<owner>> target(
                        3, wjr::in_place_reserve, append_allocator<owner>(state));
                    for (int i = 0; i != 3; ++i) {
                        target.emplace_back(std::make_unique<token>(state));
                    }
                    state.hook_budget = budget;
                    if (site == emplace_site::back) {
                        EXPECT_THROW(target.emplace_back(std::move(source)), std::runtime_error);
                    } else if (site == emplace_site::middle) {
                        EXPECT_THROW(target.emplace(target.cbegin() + 1, std::move(source)),
                                     std::runtime_error);
                    } else {
                        EXPECT_THROW(target.emplace(target.cend(), std::move(source)),
                                     std::runtime_error);
                    }
                    state.hook_budget = -1;
                    EXPECT_EQ(target.size(), 3);
                    EXPECT_EQ(source != nullptr, budget == 0);
                    EXPECT_EQ(state.tokens, budget == 0 ? 4 : 4 - budget);
                    for (int i = 0; i != 3; ++i) {
                        EXPECT_EQ(target[i] == nullptr, i < budget - 1);
                    }
                }
                EXPECT_EQ(state.tokens, budget == 0 ? 1 : 0);
            }
            EXPECT_EQ(state.tokens, 0);
            EXPECT_EQ(state.allocations, state.deallocations);
            EXPECT_EQ(state.created, state.destroyed);
        }
    }
}

TEST(vector_emplace_rollback, incoming_alias_is_copied_before_source_relocation) {
    for (const bool spare : {false, true}) {
        wjr::vector<std::string> target(2, wjr::in_place_reserve);
        target.emplace_back(100, 'x');
        target.emplace_back(100, 'y');
        if (spare) {
            target.reserve(8);
        }
        target.emplace(target.cbegin() + 1, target[0].c_str());
        ASSERT_EQ(target.size(), 3);
        EXPECT_EQ(target[0], std::string(100, 'x'));
        EXPECT_EQ(target[1], std::string(100, 'x'));
        EXPECT_EQ(target[2], std::string(100, 'y'));
    }
    wjr::vector<std::string> target(1, wjr::in_place_reserve);
    target.emplace_back(100, 'z');
    target.emplace_back(target[0]);
    EXPECT_EQ(target[0], target[1]);
}

TEST(vector_append_rollback, move_only_relocation_does_not_destroy_source_prefix_on_hook_failure) {
    using owner = std::unique_ptr<token>;
    for (int old_moves = 0; old_moves != 3; ++old_moves) {
        append_state state;
        {
            std::array<owner, 8> source;
            for (auto &value : source) {
                value = std::make_unique<token>(state);
            }
            wjr::vector<owner, append_allocator<owner>> target{append_allocator<owner>(state)};
            for (int i = 0; i != 3; ++i) {
                target.emplace_back(std::make_unique<token>(state));
            }
            auto *const data = target.data();
            state.hook_budget = 8 + old_moves;
            EXPECT_THROW(target.append(std::make_move_iterator(source.begin()),
                                       std::make_move_iterator(source.end())),
                         std::runtime_error);
            state.hook_budget = -1;
            EXPECT_EQ(target.size(), 3);
            EXPECT_EQ(target.data(), data);
            EXPECT_EQ(state.tokens, 3 - old_moves);
            for (int i = 0; i != 3; ++i) {
                EXPECT_EQ(target[i] == nullptr, i < old_moves);
            }
        }
        EXPECT_EQ(state.tokens, 0);
        EXPECT_EQ(state.allocations, state.deallocations);
        EXPECT_EQ(state.created, state.destroyed);
    }
}

} // namespace
#endif
