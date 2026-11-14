#include <gtest/gtest.h>

#include <wjr/expected.hpp>

namespace {
struct move_error {};

struct throwing_move {
    int *live;
    bool *fail;
    int value;

    throwing_move(int &count, bool &should_fail, int initial) noexcept
        : live(&count), fail(&should_fail), value(initial) {
        ++*live;
    }

    throwing_move(throwing_move &&other) : live(other.live), fail(other.fail), value(other.value) {
#ifndef WJR_DISABLE_EXCEPTIONS
        if (*fail) {
            throw move_error{};
        }
#endif
        ++*live;
    }

    throwing_move &operator=(throwing_move &&other) noexcept {
        value = other.value;
        return *this;
    }

    ~throwing_move() { --*live; }
};

using error_result = wjr::expected<int, throwing_move>;
using value_result = wjr::expected<throwing_move, int>;
using void_result = wjr::expected<void, throwing_move>;
using compressed_result = wjr::expected<throwing_move, wjr::compressed_value<int, 0>>;

static_assert(std::is_nothrow_move_assignable_v<throwing_move>);
static_assert(!std::is_nothrow_move_constructible_v<throwing_move>);
static_assert(std::is_move_assignable_v<error_result>);
static_assert(std::is_move_assignable_v<value_result>);
static_assert(std::is_move_assignable_v<void_result>);
static_assert(std::is_move_assignable_v<compressed_result>);
static_assert(!std::is_nothrow_move_assignable_v<error_result>);
static_assert(!std::is_nothrow_move_assignable_v<value_result>);
static_assert(!std::is_nothrow_move_assignable_v<void_result>);
static_assert(!std::is_nothrow_move_assignable_v<compressed_result>);
static_assert(std::is_nothrow_move_assignable_v<wjr::expected<int, int>>);
static_assert(std::is_nothrow_move_assignable_v<wjr::expected<void, int>>);
static_assert(std::is_trivially_copyable_v<wjr::expected<int, int>>);
static_assert(std::is_trivially_move_assignable_v<wjr::expected<int, int>>);

#ifndef WJR_DISABLE_EXCEPTIONS
TEST(expected_assignment, error_construction_exception_preserves_value) {
    int live = 0;
    bool fail = true;
    {
        error_result target(42);
        error_result source(wjr::unexpect, live, fail, 7);
        EXPECT_THROW(target = std::move(source), move_error);
        ASSERT_TRUE(target);
        EXPECT_EQ(*target, 42);
        ASSERT_FALSE(source);
        EXPECT_EQ(source.error().value, 7);
        EXPECT_EQ(live, 1);

        fail = false;
        target = std::move(source);
        ASSERT_FALSE(target);
        EXPECT_EQ(target.error().value, 7);
        EXPECT_EQ(live, 2);
        fail = true;
        EXPECT_NO_THROW(target = std::move(source));
        EXPECT_EQ(live, 2);
    }
    EXPECT_EQ(live, 0);
}

template <typename Result>
void check_value_construction_exception() {
    int live = 0;
    bool fail = true;
    {
        Result target(wjr::unexpect, 42);
        Result source(std::in_place, live, fail, 7);
        EXPECT_THROW(target = std::move(source), move_error);
        ASSERT_FALSE(target);
        EXPECT_EQ(target.error(), 42);
        ASSERT_TRUE(source);
        EXPECT_EQ(source->value, 7);
        EXPECT_EQ(live, 1);

        fail = false;
        target = std::move(source);
        ASSERT_TRUE(target);
        EXPECT_EQ(target->value, 7);
        EXPECT_EQ(live, 2);
    }
    EXPECT_EQ(live, 0);
}

TEST(expected_assignment, value_construction_exception_restores_error) {
    check_value_construction_exception<value_result>();
    check_value_construction_exception<compressed_result>();
}

TEST(expected_assignment, void_error_construction_exception_preserves_success) {
    int live = 0;
    bool fail = true;
    {
        void_result target;
        void_result source(wjr::unexpect, live, fail, 7);
        EXPECT_THROW(target = std::move(source), move_error);
        EXPECT_TRUE(target);
        ASSERT_FALSE(source);
        EXPECT_EQ(source.error().value, 7);
        EXPECT_EQ(live, 1);

        fail = false;
        target = std::move(source);
        ASSERT_FALSE(target);
        EXPECT_EQ(target.error().value, 7);
        EXPECT_EQ(live, 2);
    }
    EXPECT_EQ(live, 0);
}
#endif
} // namespace
