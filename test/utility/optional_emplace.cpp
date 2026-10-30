#include <wjr/optional.hpp>

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace {

struct construction_error {};

struct tracked_value {
    static inline int alive = 0;
    static inline int destroyed = 0;
    int value;

    explicit tracked_value(int v, bool fail = false) : value(v) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        if (fail) {
            throw construction_error{};
        }
#endif
        ++alive;
    }

    tracked_value(std::initializer_list<int> values, bool fail = false)
        : tracked_value(static_cast<int>(values.size()), fail) {}

    ~tracked_value() {
        --alive;
        ++destroyed;
    }
};

struct nothrow_value {
    explicit nothrow_value(int) noexcept {}
    nothrow_value(std::initializer_list<int>, int) noexcept {}
};

struct throwing_destructor {
    throwing_destructor() noexcept = default;
    ~throwing_destructor() noexcept(false) {}
};

static_assert(noexcept(std::declval<wjr::optional<int> &>().emplace(1)));
static_assert(noexcept(std::declval<wjr::optional<nothrow_value> &>().emplace(1)));
static_assert(noexcept(std::declval<wjr::optional<nothrow_value> &>().emplace({1, 2}, 3)));
static_assert(!noexcept(std::declval<wjr::optional<tracked_value> &>().emplace(1)));
static_assert(!noexcept(std::declval<wjr::optional<tracked_value> &>().emplace({1, 2})));
static_assert(!noexcept(std::declval<wjr::optional<throwing_destructor> &>().emplace()));

constexpr bool scalar_replacement() {
    wjr::optional<int> ordinary;
    ordinary.emplace(3);
    ordinary.emplace(5);
    wjr::optional<wjr::compressed_value<int, 0>> compressed;
    compressed.emplace(7);
    compressed.emplace(11);
    return *ordinary == 5 && *compressed == 11;
}
static_assert(scalar_replacement());

} // namespace

TEST(optional_emplace, constructs_replaces_and_returns_the_live_value) {
    tracked_value::alive = 0;
    tracked_value::destroyed = 0;
    {
        wjr::optional<tracked_value> value;
        auto &first = value.emplace(17);
        EXPECT_TRUE(value.has_value());
        EXPECT_EQ(std::addressof(first), std::addressof(*value));
        EXPECT_EQ(first.value, 17);
        EXPECT_EQ(tracked_value::alive, 1);
        auto &second = value.emplace({1, 2, 3});
        EXPECT_TRUE(value.has_value());
        EXPECT_EQ(std::addressof(second), std::addressof(*value));
        EXPECT_EQ(second.value, 3);
        EXPECT_EQ(tracked_value::alive, 1);
        EXPECT_EQ(tracked_value::destroyed, 1);
    }
    EXPECT_EQ(tracked_value::alive, 0);
    EXPECT_EQ(tracked_value::destroyed, 2);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(optional_emplace, empty_value_stays_empty_after_construction_failure) {
    tracked_value::alive = 0;
    tracked_value::destroyed = 0;
    {
        wjr::optional<tracked_value> value;
        EXPECT_THROW(value.emplace(17, true), construction_error);
        EXPECT_FALSE(value.has_value());
        EXPECT_EQ(tracked_value::alive, 0);
        EXPECT_EQ(tracked_value::destroyed, 0);
        value.emplace(23);
        EXPECT_TRUE(value.has_value());
        EXPECT_EQ(value->value, 23);
    }
    EXPECT_EQ(tracked_value::alive, 0);
    EXPECT_EQ(tracked_value::destroyed, 1);
}

TEST(optional_emplace, replacement_failure_destroys_the_previous_value_once) {
    tracked_value::alive = 0;
    tracked_value::destroyed = 0;
    {
        wjr::optional<tracked_value> value(std::in_place, 17);
        EXPECT_THROW(value.emplace(23, true), construction_error);
        EXPECT_FALSE(value.has_value());
        EXPECT_EQ(tracked_value::alive, 0);
        EXPECT_EQ(tracked_value::destroyed, 1);
        value.emplace(31);
        EXPECT_EQ(value->value, 31);
        EXPECT_EQ(tracked_value::alive, 1);
    }
    EXPECT_EQ(tracked_value::alive, 0);
    EXPECT_EQ(tracked_value::destroyed, 2);
}

TEST(optional_emplace, initializer_list_failure_preserves_empty_and_engaged_lifetimes) {
    tracked_value::alive = 0;
    tracked_value::destroyed = 0;
    {
        wjr::optional<tracked_value> value;
        EXPECT_THROW(value.emplace({1, 2}, true), construction_error);
        EXPECT_FALSE(value.has_value());
        EXPECT_EQ(tracked_value::alive, 0);
        value.emplace({1, 2, 3});
        EXPECT_EQ(value->value, 3);
        EXPECT_THROW(value.emplace({4, 5}, true), construction_error);
        EXPECT_FALSE(value.has_value());
        EXPECT_EQ(tracked_value::alive, 0);
        EXPECT_EQ(tracked_value::destroyed, 1);
        value.emplace({7});
        EXPECT_EQ(value->value, 1);
    }
    EXPECT_EQ(tracked_value::alive, 0);
    EXPECT_EQ(tracked_value::destroyed, 2);
}
#endif
