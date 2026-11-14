#include <map>
#include <memory>
#include <memory_resource>
#include <set>
#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/ring_buffer.hpp>
#include <wjr/vector.hpp>

namespace {
struct move_state {
    std::map<void *, int> blocks;
    std::map<void *, int> objects;
    std::set<void *> live;
    int wrong_deallocations = 0;
    int wrong_destroys = 0;
    int comparisons = 0;
    int move_budget = -1;
};

struct move_domain {
    move_state *state;
    int id;
    int allocation_budget = -1;
    int hook_budget = -1;
};

template <typename T>
struct move_allocator {
    using value_type = T;
    move_domain *domain;
    explicit move_allocator(move_domain &owner) noexcept : domain(&owner) {}
    template <typename U>
    move_allocator(const move_allocator<U> &other) noexcept : domain(other.domain) {}
    T *allocate(std::size_t n) {
        if (domain->allocation_budget == 0)
            throw std::bad_alloc();
        if (domain->allocation_budget > 0)
            --domain->allocation_budget;
        auto *ptr = std::allocator<T>{}.allocate(n);
        domain->state->blocks.emplace(ptr, domain->id);
        return ptr;
    }
    void deallocate(T *ptr, std::size_t n) noexcept {
        auto &blocks = domain->state->blocks;
        const auto it = blocks.find(ptr);
        if (it == blocks.end() || it->second != domain->id)
            ++domain->state->wrong_deallocations;
        if (it != blocks.end())
            blocks.erase(it);
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        if (domain->hook_budget == 0)
            throw std::runtime_error("construct hook failed");
        if (domain->hook_budget > 0)
            --domain->hook_budget;
        std::construct_at(ptr, std::forward<Args>(args)...);
        EXPECT_TRUE(domain->state->objects.emplace(ptr, domain->id).second);
    }
    template <typename U>
    void destroy(U *ptr) noexcept {
        auto &objects = domain->state->objects;
        const auto it = objects.find(ptr);
        if (it == objects.end() || it->second != domain->id)
            ++domain->state->wrong_destroys;
        if (it != objects.end())
            objects.erase(it);
        std::destroy_at(ptr);
    }
    friend bool operator==(const move_allocator &left, const move_allocator &right) noexcept {
        ++left.domain->state->comparisons;
        return left.domain == right.domain;
    }
};

template <bool Nothrow>
struct move_value {
    move_state *owner;
    int number;
    move_value(move_state &state, int n) noexcept : owner(&state), number(n) {
        EXPECT_TRUE(owner->live.insert(this).second);
    }
    move_value(const move_value &) = delete;
    move_value &operator=(const move_value &) = delete;
    move_value &operator=(move_value &&) = delete;
    move_value(move_value &&other) noexcept(Nothrow) : owner(other.owner), number(other.number) {
        if constexpr (!Nothrow) {
            if (owner->move_budget == 0)
                throw std::runtime_error("move failed");
            if (owner->move_budget > 0)
                --owner->move_budget;
        }
        EXPECT_TRUE(owner->live.insert(this).second);
        other.number = -1;
    }
    ~move_value() { EXPECT_EQ(owner->live.erase(this), 1); }
};

template <typename Container>
void fill_source(Container &source, move_state &state, bool wrapped, int count,
                 bool reserve = true) {
    if (reserve)
        source.reserve(8);
    for (int i = 0; i != count; ++i)
        source.emplace_back(state, i);
    if constexpr (requires { source.pop_front(); }) {
        if (wrapped) {
            for (int i = 0; i != 3; ++i)
                source.pop_front();
            for (int i = count; i != count + 3; ++i)
                source.emplace_back(state, i);
            ASSERT_FALSE(source.array_two().empty());
        }
    }
}

void expect_released(const move_state &state) {
    EXPECT_TRUE(state.blocks.empty());
    EXPECT_TRUE(state.objects.empty());
    EXPECT_TRUE(state.live.empty());
    EXPECT_EQ(state.wrong_deallocations, 0);
    EXPECT_EQ(state.wrong_destroys, 0);
}

template <typename Container>
void check_ownership(bool equal, bool wrapped, int count = 7, bool inline_source = false) {
    move_state state;
    move_domain first{&state, 1}, second{&state, 2};
    using allocator = typename Container::allocator_type;
    {
        Container source{allocator(first)};
        fill_source(source, state, wrapped, count, !inline_source);
        const auto *original_data = source.data();
        auto &destination_domain = equal ? first : second;
        if (equal && !inline_source)
            destination_domain.allocation_budget = destination_domain.hook_budget = 0;
        Container destination(std::move(source), allocator(destination_domain));
        EXPECT_EQ(destination.get_allocator().domain, &destination_domain);
        EXPECT_EQ(destination.size(), count);
        EXPECT_EQ(source.get_allocator().domain, &first);
        if (equal && !inline_source)
            EXPECT_EQ(destination.data(), original_data);
        else
            EXPECT_NE(destination.data(), original_data);
        int expected = wrapped ? 3 : 0;
        for (const auto &element : destination)
            EXPECT_EQ(element.number, expected++);
    }
    expect_released(state);
}

TEST(allocator_move_construction, vector_respects_explicit_unequal_allocator) {
    using value = move_value<true>;
    using vector = wjr::vector<value, move_allocator<value>>;
    EXPECT_FALSE(
        (std::is_nothrow_constructible_v<vector, vector &&, const move_allocator<value> &>));
    check_ownership<vector>(false, false);
    check_ownership<vector>(true, false);
}

TEST(allocator_move_construction, ring_respects_allocator_and_logical_wrapped_order) {
    using value = move_value<true>;
    using ring = wjr::ring_buffer<value, move_allocator<value>>;
    EXPECT_FALSE((std::is_nothrow_constructible_v<ring, ring &&, const move_allocator<value> &>));
    for (bool wrapped : {false, true}) {
        check_ownership<ring>(false, wrapped);
        check_ownership<ring>(true, wrapped);
    }
}

template <typename Container>
void check_failure(bool fail_allocation, bool fail_hook, bool wrapped = false) {
    const int count = wrapped ? 7 : 4;
    for (int budget = 0; budget != count; ++budget) {
        move_state state;
        move_domain first{&state, 1}, second{&state, 2};
        using allocator = typename Container::allocator_type;
        {
            Container source{allocator(first)};
            fill_source(source, state, wrapped, count);
            const auto original_live = state.live;
            const auto original_objects = state.objects;
            const auto original_blocks = state.blocks;
            if (fail_allocation)
                second.allocation_budget = 0;
            else
                (fail_hook ? second.hook_budget : state.move_budget) = budget;
            bool caught = false;
            try {
                Container destination(std::move(source), allocator(second));
            } catch (const std::exception &) {
                caught = true;
                EXPECT_EQ(source.size(), count);
                EXPECT_EQ(state.live, original_live);
                EXPECT_EQ(state.objects, original_objects);
                EXPECT_EQ(state.blocks, original_blocks);
            }
            EXPECT_TRUE(caught);
            second.allocation_budget = second.hook_budget = state.move_budget = -1;
        }
        expect_released(state);
    }
}

TEST(allocator_move_construction, allocation_failure_keeps_source_ownership) {
    using value = move_value<true>;
    check_failure<wjr::vector<value, move_allocator<value>>>(true, false);
    check_failure<wjr::ring_buffer<value, move_allocator<value>>>(true, false);
}

TEST(allocator_move_construction, throwing_moves_keep_source_lifetimes) {
    using value = move_value<false>;
    check_failure<wjr::vector<value, move_allocator<value>>>(false, false);
    for (bool wrapped : {false, true})
        check_failure<wjr::ring_buffer<value, move_allocator<value>>>(false, false, wrapped);
}

TEST(allocator_move_construction, throwing_hooks_keep_source_lifetimes) {
    using value = move_value<true>;
    check_failure<wjr::vector<value, move_allocator<value>>>(false, true);
    for (bool wrapped : {false, true})
        check_failure<wjr::ring_buffer<value, move_allocator<value>>>(false, true, wrapped);
}

TEST(allocator_move_construction, default_move_preserves_constant_time_heap_take) {
    using value = move_value<true>;
    move_state state;
    move_domain domain{&state, 1};
    {
        using vector = wjr::vector<value, move_allocator<value>>;
        vector source{move_allocator<value>(domain)};
        fill_source(source, state, false, 4);
        const auto *original_data = source.data();
        domain.allocation_budget = domain.hook_budget = 0;
        state.comparisons = 0;
        vector destination(std::move(source));
        EXPECT_EQ(state.comparisons, 0);
        EXPECT_EQ(destination.data(), original_data);
    }
    expect_released(state);
}
} // namespace

namespace allocator_move_test {
struct relocation_value {
    inline static int live = 0;
    int number = 42;
    relocation_value() noexcept { ++live; }
    relocation_value(const relocation_value &) = delete;
    relocation_value(relocation_value &&) = delete;
    ~relocation_value() noexcept(false) { --live; }
};

template <typename T>
struct plain_allocator {
    using value_type = T;
    move_domain *domain;
    explicit plain_allocator(move_domain &owner) noexcept : domain(&owner) {}
    template <typename U>
    plain_allocator(const plain_allocator<U> &other) noexcept : domain(other.domain) {}
    T *allocate(std::size_t n) { return move_allocator<T>(*domain).allocate(n); }
    void deallocate(T *ptr, std::size_t n) noexcept {
        move_allocator<T>(*domain).deallocate(ptr, n);
    }
    friend bool operator==(const plain_allocator &left, const plain_allocator &right) noexcept {
        return left.domain == right.domain;
    }
};

struct factory_value {
    move_state *owner;
    int number;
    factory_value(move_state &state, int n) noexcept : owner(&state), number(n) {
        EXPECT_TRUE(owner->live.insert(this).second);
    }
    factory_value(const factory_value &) = delete;
    factory_value(factory_value &&) = delete;
    ~factory_value() { EXPECT_EQ(owner->live.erase(this), 1); }
};

template <typename T>
struct factory_allocator : plain_allocator<T> {
    using is_trivially_allocator = std::false_type;
    using plain_allocator<T>::plain_allocator;
    template <typename U>
    factory_allocator(const factory_allocator<U> &other) noexcept
        : plain_allocator<T>(*other.domain) {}
    void construct(T *ptr, T &&other) {
        move_allocator<T>(*this->domain).construct(ptr, *other.owner, other.number);
    }
    template <typename U, typename... Args>
    void construct(U *ptr, Args &&...args) {
        move_allocator<T>(*this->domain).construct(ptr, std::forward<Args>(args)...);
    }
    template <typename U>
    void destroy(U *ptr) noexcept {
        move_allocator<T>(*this->domain).destroy(ptr);
    }
};
} // namespace allocator_move_test

namespace wjr {
template <>
struct get_relocate_mode<allocator_move_test::relocation_value> {
    static constexpr relocate_t value = relocate_t::trivial;
};
} // namespace wjr

namespace {
TEST(allocator_move_construction, small_vector_uses_allocator_in_both_storage_modes) {
    using value = move_value<true>;
    using vector = wjr::small_vector<value, 4, move_allocator<value>>;
    for (bool equal : {false, true}) {
        check_ownership<vector>(equal, false, 3, true);
        check_ownership<vector>(equal, false, 7);
    }
}

TEST(allocator_move_construction, empty_source_keeps_reserved_allocation_with_its_owner) {
    using value = move_value<true>;
    check_ownership<wjr::vector<value, move_allocator<value>>>(false, false, 0);
    check_ownership<wjr::ring_buffer<value, move_allocator<value>>>(false, false, 0);
}

template <typename Container>
void check_compatible_take(const typename Container::allocator_type &allocator) {
    static_assert(std::is_nothrow_constructible_v<Container, Container &&,
                                                  const typename Container::allocator_type &>);
    Container source(3, allocator);
    auto *const original_data = source.data();
    Container destination(std::move(source), allocator);
    EXPECT_EQ(destination.data(), original_data);
    ASSERT_EQ(destination.size(), 3);
    EXPECT_EQ(destination.front().number, 42);
}

TEST(allocator_move_construction, registered_deleted_moves_keep_compatible_stateful_take) {
    using namespace allocator_move_test;
    using allocator = plain_allocator<relocation_value>;
    move_state state;
    move_domain domain{&state, 1};
    check_compatible_take<wjr::vector<relocation_value, allocator>>(allocator(domain));
    check_compatible_take<wjr::ring_buffer<relocation_value, allocator>>(allocator(domain));
    EXPECT_EQ(relocation_value::live, 0);
    expect_released(state);
}

TEST(allocator_move_construction, registered_deleted_moves_keep_compatible_pmr_generic_hook) {
    using allocator_move_test::relocation_value;
    using allocator = std::pmr::polymorphic_allocator<relocation_value>;
    std::pmr::monotonic_buffer_resource resource;
    check_compatible_take<wjr::vector<relocation_value, allocator>>(allocator(&resource));
    check_compatible_take<wjr::ring_buffer<relocation_value, allocator>>(allocator(&resource));
    EXPECT_EQ(relocation_value::live, 0);
}

TEST(allocator_move_construction, custom_factory_hook_can_migrate_deleted_move_values) {
    using namespace allocator_move_test;
    using allocator = factory_allocator<factory_value>;
    static_assert(!std::is_move_constructible_v<factory_value>);
    static_assert(
        !wjr::is_trivially_allocator_construct_v<allocator, factory_value, factory_value &&>);
    check_ownership<wjr::vector<factory_value, allocator>>(false, false);
    check_ownership<wjr::ring_buffer<factory_value, allocator>>(false, false);
}
} // namespace
