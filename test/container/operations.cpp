#include <gtest/gtest.h>

#include <concepts>
#include <memory>
#include <string>
#include <vector>

#include <wjr/container/detail.hpp>
#include <wjr/memory/copy.hpp>
#include <wjr/vector.hpp>

namespace container_operations_test {

struct lvalue_members {
    unsigned count = 0;
    constexpr void resize(unsigned n) & noexcept { count = n; }
    constexpr void append(unsigned n) & noexcept { count += n; }
    constexpr unsigned size() const noexcept { return count; }
    void reserve(unsigned) &;
    const int *cbegin() const &;
    const int *cend() const &;
    void insert(const int *, int) &;
};

struct rvalue_members {
    void resize(unsigned) &&;
    void append(unsigned) &&;
    void reserve(unsigned) &&;
    const int *cbegin() const &&;
    const int *cend() const &&;
    void insert(const int *, int) &&;
};

struct append_only {
    unsigned count = 0;
    constexpr void append(unsigned n) & noexcept { count += n; }
};

struct tagged_only {
    unsigned count = 0;
    constexpr void append(unsigned &n, const wjr::default_construct_t &) & noexcept { count += n; }
};

struct tagged_resize_only {
    unsigned count = 0;
    constexpr void resize(unsigned &n, const wjr::default_construct_t &) & noexcept { count = n; }
};

struct local_size_only {
    unsigned count = 0;
    constexpr void resize(unsigned &n) & noexcept { count = n; }
    constexpr unsigned size() const noexcept { return count; }
};

struct resize_without_size {
    void resize(unsigned) &;
};

struct priority_container {
    unsigned count = 2;
    unsigned called = 0;
    constexpr void append(unsigned n) noexcept {
        count += n;
        called = 1;
    }
    constexpr void resize(unsigned n) noexcept {
        count = n;
        called = 2;
    }
    constexpr void resize(unsigned n, wjr::default_construct_t) noexcept {
        count = n;
        called = 3;
    }
    constexpr unsigned size() const noexcept { return count; }
};

struct fast_priority_container : priority_container {
    using priority_container::append;
    constexpr void append(unsigned n, wjr::default_construct_t) noexcept {
        count += n;
        called = 4;
    }
};

struct custom_hooks {
    unsigned count = 0;
};

struct forwarding_receiver {
    std::unique_ptr<int> value;
    void append(std::unique_ptr<int> &&ptr) & noexcept { value = std::move(ptr); }
    void resize(unsigned) & { throw 42; }
};

struct bulk_receiver {
    using value_type = int;
    std::vector<int> data;
    unsigned scalar_calls = 0;
    unsigned bulk_calls = 0;

    void push_back(int value) {
        ++scalar_calls;
        data.push_back(value);
    }
    void append(const int *first, const int *last) & {
        ++bulk_calls;
        data.insert(data.end(), first, last);
    }
};

} // namespace container_operations_test

namespace wjr {

template <>
struct resize_fn_impl<container_operations_test::custom_hooks> {
    static constexpr int resize(container_operations_test::custom_hooks &cont,
                                unsigned n) noexcept {
        cont.count = n;
        return 0;
    }
};

template <>
struct append_fn_impl<container_operations_test::custom_hooks> {
    static constexpr void append(container_operations_test::custom_hooks &cont,
                                 unsigned n) noexcept {
        cont.count += n;
    }
};

} // namespace wjr

namespace {
using namespace container_operations_test;

template <typename Container>
concept uninitialized_resizable =
    requires(Container &cont) { wjr::try_uninitialized_resize(cont, 3u); };

template <typename Container>
concept uninitialized_appendable =
    requires(Container &cont) { wjr::try_uninitialized_append(cont, 3u); };

static_assert(wjr::resizable_container<lvalue_members, unsigned>);
static_assert(wjr::appendable_container<lvalue_members, unsigned>);
static_assert(!wjr::resizable_container<rvalue_members, unsigned>);
static_assert(!wjr::appendable_container<rvalue_members, unsigned>);
static_assert(!wjr::resizable_container<const lvalue_members, unsigned>);
static_assert(!wjr::appendable_container<const lvalue_members, unsigned>);
static_assert(wjr::has__container_append_v<lvalue_members, unsigned>);
static_assert(wjr::has_container_append_v<lvalue_members, unsigned>);
static_assert(wjr::has_container_reserve_v<lvalue_members, unsigned>);
static_assert(wjr::has_container_insert_v<lvalue_members, int>);
static_assert(!wjr::has_container_append_v<rvalue_members, unsigned>);
static_assert(!wjr::has_container_reserve_v<rvalue_members, unsigned>);
static_assert(!wjr::has_container_insert_v<rvalue_members, int>);
static_assert(!std::invocable<decltype(wjr::resize), int &, unsigned>);
static_assert(!std::invocable<decltype(wjr::append), int &, unsigned>);
static_assert(!std::invocable<decltype(wjr::resize), lvalue_members &&, unsigned>);
static_assert(!std::invocable<decltype(wjr::append), lvalue_members &&, unsigned>);
static_assert(!wjr::appendable_container<void, unsigned>);
static_assert(!wjr::resizable_container<void, unsigned>);
static_assert(wjr::resizable_container<local_size_only, unsigned &>);
static_assert(!wjr::resizable_container<local_size_only, unsigned>);
static_assert(uninitialized_resizable<local_size_only>);
static_assert(uninitialized_appendable<local_size_only>);
static_assert(uninitialized_appendable<append_only>);
static_assert(uninitialized_appendable<tagged_only>);
static_assert(uninitialized_resizable<tagged_resize_only>);
static_assert(!uninitialized_appendable<resize_without_size>);
static_assert(!uninitialized_appendable<int>);
static_assert(!uninitialized_resizable<int>);
static_assert(wjr::resizable_container<custom_hooks, unsigned>);
static_assert(wjr::appendable_container<custom_hooks, unsigned>);
static_assert(!wjr::has__container_resize_v<custom_hooks, unsigned>);
static_assert(!wjr::has__container_append_v<custom_hooks, unsigned>);
static_assert(noexcept(wjr::resize(std::declval<custom_hooks &>(), 1u)));
static_assert(std::same_as<decltype(wjr::resize(std::declval<custom_hooks &>(), 1u)), void>);
static_assert(noexcept(wjr::append(std::declval<forwarding_receiver &>(),
                                   std::declval<std::unique_ptr<int>>())));
static_assert(!noexcept(wjr::resize(std::declval<forwarding_receiver &>(), 1u)));
static_assert(
    !std::invocable<decltype(wjr::append), forwarding_receiver &, std::unique_ptr<int> &>);

constexpr bool constant_evaluation() {
    lvalue_members member;
    wjr::resize(member, 2u);
    wjr::append(member, 3u);
    wjr::try_uninitialized_append(member, 2u);
    custom_hooks custom;
    wjr::resize(custom, 3u);
    wjr::append(custom, 2u);
    tagged_only tagged;
    wjr::try_uninitialized_append(tagged, 3u);
    append_only plain;
    wjr::try_uninitialized_append(plain, 4u);
    local_size_only local;
    wjr::try_uninitialized_resize(local, 3u);
    wjr::try_uninitialized_append(local, 4u);
    return member.count == 7 && custom.count == 5 && tagged.count == 3 && plain.count == 4 &&
           local.count == 7;
}

static_assert(constant_evaluation());
} // namespace

TEST(container_operations, ref_qualified_members_and_customization_hooks) {
    lvalue_members member;
    wjr::resize(member, 7u);
    wjr::append(member, 3u);
    EXPECT_EQ(member.count, 10u);
    custom_hooks custom;
    wjr::resize(custom, 7u);
    wjr::append(custom, 3u);
    EXPECT_EQ(custom.count, 10u);
}

TEST(container_operations, preserve_tag_and_resize_priority) {
    priority_container resized;
    wjr::try_uninitialized_append(resized, 3u);
    EXPECT_EQ(resized.count, 5u);
    EXPECT_EQ(resized.called, 3u);
    fast_priority_container appended;
    wjr::try_uninitialized_append(appended, 3u);
    EXPECT_EQ(appended.count, 5u);
    EXPECT_EQ(appended.called, 4u);
}

TEST(container_operations, append_only_and_local_size_categories) {
    append_only plain;
    wjr::try_uninitialized_append(plain, 4u);
    EXPECT_EQ(plain.count, 4u);
    tagged_only tagged;
    wjr::try_uninitialized_append(tagged, 4u);
    EXPECT_EQ(tagged.count, 4u);
    local_size_only local;
    wjr::try_uninitialized_resize(local, 4u);
    wjr::try_uninitialized_append(local, 3u);
    EXPECT_EQ(local.count, 7u);
    tagged_resize_only resize_tag;
    wjr::try_uninitialized_resize(resize_tag, 9u);
    EXPECT_EQ(resize_tag.count, 9u);
}

TEST(container_operations, forward_ownership_and_exceptions) {
    forwarding_receiver receiver;
    auto value = std::make_unique<int>(42);
    auto *ptr = value.get();
    wjr::append(receiver, std::move(value));
    EXPECT_FALSE(value);
    EXPECT_EQ(receiver.value.get(), ptr);
    EXPECT_THROW(wjr::resize(receiver, 1u), int);
}

TEST(container_operations, real_containers_and_bulk_copy) {
    std::vector<int> ordinary{1, 2};
    wjr::try_uninitialized_append(ordinary, 3u);
    EXPECT_EQ(ordinary, (std::vector<int>{1, 2, 0, 0, 0}));
    wjr::vector<int> optimized{1, 2};
    wjr::try_uninitialized_append(optimized, 3u);
    EXPECT_EQ(optimized.size(), 5u);
    std::fill(optimized.begin() + 2, optimized.end(), 7);
    EXPECT_EQ(optimized[4], 7);
    std::string text("a");
    wjr::append(text, 3u, 'x');
    EXPECT_EQ(text, "axxx");
    bulk_receiver bulk;
    const int source[]{1, 2, 3, 4};
    wjr::copy(std::begin(source), std::end(source), std::back_inserter(bulk));
    EXPECT_EQ(bulk.data, (std::vector<int>{1, 2, 3, 4}));
    EXPECT_EQ(bulk.bulk_calls, 1u);
    EXPECT_EQ(bulk.scalar_calls, 0u);
}
