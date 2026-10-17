#include <wjr/expected.hpp>
#include <wjr/optional.hpp>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace {

struct copied_value {
    int value = 0;
    copied_value() = default;
    constexpr explicit copied_value(int input) noexcept : value(input) {}
    constexpr copied_value(const copied_value &other) noexcept : value(other.value) {}
    copied_value(copied_value &&) = default;
    copied_value &operator=(const copied_value &) = default;
    copied_value &operator=(copied_value &&) = default;
};

struct copy_only {
    copy_only() = default;
    copy_only(const copy_only &) noexcept {}
};

struct deleted_move {
    deleted_move() = default;
    deleted_move(const deleted_move &) = default;
    deleted_move(deleted_move &&) = delete;
};

template <typename T>
constexpr bool construction_traits() {
    return std::is_copy_constructible_v<wjr::optional<T>> &&
           std::is_copy_constructible_v<wjr::expected<T, int>> &&
           std::is_trivially_destructible_v<wjr::optional<T>> &&
           std::is_trivially_destructible_v<wjr::expected<T, int>>;
}

static_assert(construction_traits<copied_value>());
static_assert(construction_traits<copy_only>());
static_assert(construction_traits<deleted_move>());
static_assert(!std::is_trivially_copy_constructible_v<wjr::optional<copied_value>>);
static_assert(!std::is_trivially_copy_constructible_v<wjr::expected<copied_value, int>>);
static_assert(std::is_trivially_move_constructible_v<wjr::optional<copied_value>>);
static_assert(std::is_trivially_move_constructible_v<wjr::expected<copied_value, int>>);
static_assert(!std::is_copy_constructible_v<wjr::optional<std::unique_ptr<int>>>);
static_assert(!std::is_copy_constructible_v<wjr::expected<std::unique_ptr<int>, int>>);
static_assert(std::is_nothrow_move_constructible_v<wjr::optional<std::unique_ptr<int>>>);
static_assert(std::is_nothrow_move_constructible_v<wjr::expected<std::unique_ptr<int>, int>>);
static_assert(sizeof(wjr::optional<copied_value>) == sizeof(wjr::optional<int>));
static_assert(alignof(wjr::optional<copied_value>) == alignof(wjr::optional<int>));
static_assert(sizeof(wjr::expected<copied_value, int>) == sizeof(wjr::expected<int, int>));
static_assert(alignof(wjr::expected<copied_value, int>) == alignof(wjr::expected<int, int>));

constexpr bool constant_construction() {
    const wjr::optional<int> source(7);
    const wjr::optional<int> empty;
    const wjr::optional<copied_value> value(source);
    const wjr::optional<copied_value> copied(value);
    wjr::optional<copied_value> movable(std::in_place, 11);
    const wjr::optional<copied_value> moved(std::move(movable));
    const wjr::optional<copied_value> absent(empty);
    const wjr::expected<int, short> success(8);
    const wjr::expected<int, short> failure(wjr::unexpect, 9);
    const wjr::expected<copied_value, int> result(success);
    const wjr::expected<copied_value, int> copied_result(result);
    wjr::expected<copied_value, int> movable_result(std::in_place, 12);
    const wjr::expected<copied_value, int> moved_result(std::move(movable_result));
    const wjr::expected<copied_value, int> error(failure);
    const wjr::expected<void, short> void_error(wjr::unexpect, 10);
    const wjr::expected<void, int> converted_void(void_error);
    const wjr::optional<wjr::compressed_value<int, 0>> compact(source);
    const wjr::expected<copied_value, wjr::compressed_value<int, 0>> compact_result(success);
    const wjr::expected<copied_value, wjr::compressed_value<int, 0>> compact_error(failure);
    const wjr::expected<void, wjr::compressed_value<int, 0>> compact_void(void_error);
    return copied->value == 7 && moved->value == 11 && !absent && copied_result->value == 8 &&
           moved_result->value == 12 && !error && error.error() == 9 && !converted_void &&
           converted_void.error() == 10 && *compact == 7 && compact_result->value == 8 &&
           !compact_error && compact_error.error() == 9 && !compact_void &&
           compact_void.error() == 10;
}
static_assert(constant_construction());

#if !defined(WJR_DISABLE_EXCEPTIONS)

struct construction_error {};

template <int Id>
struct tracked_payload {
    static inline int constructed = 0;
    static inline int destroyed = 0;
    static inline bool fail = false;

    int value;

    explicit tracked_payload(int input) : value(input) { construct(); }
    tracked_payload(const tracked_payload &other) : value(other.value) { construct(); }
    tracked_payload(tracked_payload &&other) : value(other.value) { construct(); }
    tracked_payload &operator=(const tracked_payload &) = default;
    tracked_payload &operator=(tracked_payload &&) = default;

    // Do not read members: an incorrect destruction of inactive storage must be observable.
    ~tracked_payload() { ++destroyed; }

private:
    static void construct() {
        if (fail) {
            throw construction_error{};
        }
        ++constructed;
    }
};

using tracked_value = tracked_payload<0>;
using tracked_error = tracked_payload<1>;

template <typename Target, typename Source>
WJR_NOINLINE void construct_target(void *storage, Source &&source) {
    ::new (storage) Target(std::forward<Source>(source));
}

template <typename Target, typename Source>
void check_conversion(Source &&source, bool has_value, int values, int errors) {
    if (values + errors != 0) {
        tracked_value::fail = tracked_error::fail = true;
        const int value_destroys = tracked_value::destroyed;
        const int error_destroys = tracked_error::destroyed;
        for (const unsigned char poison : {0, 1}) {
            alignas(Target) unsigned char storage[sizeof(Target)];
            std::fill_n(storage, sizeof(storage), poison);
            EXPECT_THROW(construct_target<Target>(storage, std::forward<Source>(source)),
                         construction_error);
            EXPECT_EQ(tracked_value::destroyed, value_destroys);
            EXPECT_EQ(tracked_error::destroyed, error_destroys);
        }
        tracked_value::fail = tracked_error::fail = false;
    }

    const int value_constructs = tracked_value::constructed;
    const int error_constructs = tracked_error::constructed;
    const int value_destroys = tracked_value::destroyed;
    const int error_destroys = tracked_error::destroyed;
    {
        const Target converted(std::forward<Source>(source));
        EXPECT_EQ(converted.has_value(), has_value);
        EXPECT_EQ(tracked_value::constructed - value_constructs, values);
        EXPECT_EQ(tracked_error::constructed - error_constructs, errors);
    }
    EXPECT_EQ(tracked_value::destroyed - value_destroys, values);
    EXPECT_EQ(tracked_error::destroyed - error_destroys, errors);
}

template <typename Target, typename Source>
void check_copy_and_move(Source &source, int values, int errors) {
    const bool has_value = source.has_value();
    check_conversion<Target>(std::as_const(source), has_value, values, errors);
    check_conversion<Target>(std::move(source), has_value, values, errors);
    EXPECT_EQ(source.has_value(), has_value);
}

struct changing_error {
    int value = 9;
    void (*change)(void *) = nullptr;
    void *context = nullptr;

    operator int() const {
        const int result = value;
        const auto callback = change;
        void *const object = context;
        callback(object);
        return result;
    }
};

#endif

} // namespace

TEST(value_construction, constexpr_and_move_only) {
    EXPECT_TRUE(constant_construction());
    wjr::optional<std::unique_ptr<int>> source(std::in_place, std::make_unique<int>(11));
    wjr::optional<std::unique_ptr<int>> moved(std::move(source));
    EXPECT_EQ(**moved, 11);
    wjr::expected<std::unique_ptr<int>, int> result(std::in_place, std::make_unique<int>(12));
    wjr::expected<std::unique_ptr<int>, int> moved_result(std::move(result));
    EXPECT_EQ(**moved_result, 12);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)

TEST(value_construction, optional_lifetime) {
    wjr::optional<int> source(7);
    wjr::optional<int> empty;
    check_copy_and_move<wjr::optional<tracked_value>>(source, 1, 0);
    check_copy_and_move<wjr::optional<tracked_value>>(empty, 0, 0);
    wjr::optional<tracked_value> same(std::in_place, 8);
    wjr::optional<tracked_value> same_empty;
    check_copy_and_move<wjr::optional<tracked_value>>(same, 1, 0);
    check_copy_and_move<wjr::optional<tracked_value>>(same_empty, 0, 0);
    check_copy_and_move<wjr::optional<wjr::compressed_value<int, 0>>>(source, 0, 0);
    check_copy_and_move<wjr::optional<wjr::compressed_value<int, 0>>>(empty, 0, 0);
}

TEST(value_construction, expected_lifetime) {
    using result = wjr::expected<tracked_value, tracked_error>;
    wjr::expected<int, short> source(7);
    wjr::expected<int, short> error(wjr::unexpect, 8);
    check_copy_and_move<result>(source, 1, 0);
    check_copy_and_move<result>(error, 0, 1);
    result same(std::in_place, 9);
    result same_error(wjr::unexpect, 10);
    check_copy_and_move<result>(same, 1, 0);
    check_copy_and_move<result>(same_error, 0, 1);
    using compact = wjr::expected<tracked_value, wjr::compressed_value<int, 0>>;
    check_copy_and_move<compact>(source, 1, 0);
    check_copy_and_move<compact>(error, 0, 0);
    compact compact_same(std::in_place, 11);
    compact compact_error(wjr::unexpect, 12);
    check_copy_and_move<compact>(compact_same, 1, 0);
    check_copy_and_move<compact>(compact_error, 0, 0);
}

TEST(value_construction, void_expected_lifetime) {
    using result = wjr::expected<void, tracked_error>;
    wjr::expected<void, short> source;
    wjr::expected<void, short> error(wjr::unexpect, 7);
    check_copy_and_move<result>(source, 0, 0);
    check_copy_and_move<result>(error, 0, 1);
    result same;
    result same_error(wjr::unexpect, 8);
    check_copy_and_move<result>(same, 0, 0);
    check_copy_and_move<result>(same_error, 0, 1);
    using compact = wjr::expected<void, wjr::compressed_value<int, 0>>;
    check_copy_and_move<compact>(source, 0, 0);
    check_copy_and_move<compact>(error, 0, 0);
}

TEST(value_construction, conversion_keeps_selected_branch) {
    using source_type = wjr::expected<int, changing_error>;
    source_type source(wjr::unexpect);
    source.error().context = &source;
    source.error().change = [](void *object) { static_cast<source_type *>(object)->emplace(42); };
    const int before = tracked_value::constructed;
    const wjr::expected<tracked_value, wjr::compressed_value<int, 0>> result(source);
    ASSERT_TRUE(source.has_value());
    EXPECT_EQ(*source, 42);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), 9);
    EXPECT_EQ(tracked_value::constructed, before);
}

#endif
