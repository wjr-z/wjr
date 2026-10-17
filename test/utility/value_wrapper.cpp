#include <wjr/container/capture_leaf.hpp>
#include <wjr/expected.hpp>

#include <initializer_list>
#include <memory>
#include <type_traits>

#include <gtest/gtest.h>

namespace {

struct construction_error {};

struct throwing_empty {
    explicit throwing_empty(int) noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw construction_error{};
#endif
    }
};

struct throwing_value {
    int value;

    explicit throwing_value(int x) noexcept(false) : value(x) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw construction_error{};
#endif
    }
};

struct nothrow_empty {
    constexpr explicit nothrow_empty(int) noexcept {}
};

struct nothrow_value {
    int value;

    constexpr explicit nothrow_value(int x) noexcept : value(x) {}
};

struct move_only_value {
    std::unique_ptr<int> value;

    explicit move_only_value(std::unique_ptr<int> ptr) noexcept : value(std::move(ptr)) {}
};

struct nothrow_list_error {
    std::size_t size;
    std::unique_ptr<int> value;

    nothrow_list_error(std::initializer_list<int> &items, std::unique_ptr<int> ptr) noexcept
        : size(items.size()), value(std::move(ptr)) {}
};

struct throwing_list_error {
    throwing_list_error(std::initializer_list<int> &, std::unique_ptr<int>) noexcept(false) {
#if !defined(WJR_DISABLE_EXCEPTIONS)
        throw construction_error{};
#endif
    }
};

template <template <typename> typename Leaf>
constexpr bool check_capture_traits() {
    static_assert(std::is_constructible_v<Leaf<throwing_empty>, int>);
    static_assert(std::is_constructible_v<Leaf<throwing_value>, int>);
    static_assert(!std::is_nothrow_constructible_v<Leaf<throwing_empty>, int>);
    static_assert(!std::is_nothrow_constructible_v<Leaf<throwing_value>, int>);
    static_assert(std::is_nothrow_constructible_v<Leaf<nothrow_empty>, int>);
    static_assert(std::is_nothrow_constructible_v<Leaf<nothrow_value>, int>);
    static_assert(std::is_nothrow_constructible_v<Leaf<move_only_value>, std::unique_ptr<int>>);
    static_assert(!std::is_constructible_v<Leaf<move_only_value>, std::unique_ptr<int> &>);
    static_assert(Leaf<nothrow_value>{42}.get().value == 42);
    return true;
}

template <typename T>
using leaf = wjr::capture_leaf<T>;

template <typename T>
using compressed_leaf = wjr::compressed_capture_leaf<T>;

static_assert(check_capture_traits<leaf>());
static_assert(check_capture_traits<compressed_leaf>());
static_assert(std::is_empty_v<throwing_empty>);
static_assert(!std::is_empty_v<throwing_value>);
static_assert(std::is_nothrow_constructible_v<wjr::unexpected<nothrow_list_error>, std::in_place_t,
                                              std::initializer_list<int>, std::unique_ptr<int>>);
static_assert(std::is_constructible_v<wjr::unexpected<throwing_list_error>, std::in_place_t,
                                      std::initializer_list<int>, std::unique_ptr<int>>);
static_assert(
    !std::is_nothrow_constructible_v<wjr::unexpected<throwing_list_error>, std::in_place_t,
                                     std::initializer_list<int>, std::unique_ptr<int>>);

template <template <typename> typename Leaf>
void check_capture_forwarding() {
    Leaf<nothrow_empty> empty(7);
    (void)empty;
    Leaf<nothrow_value> value(42);
    EXPECT_EQ(value.get().value, 42);

    auto ptr = std::make_unique<int>(17);
    Leaf<move_only_value> moved(std::move(ptr));
    EXPECT_EQ(ptr, nullptr);
    EXPECT_EQ(*moved.get().value, 17);
}

} // namespace

TEST(capture_leaf, nonthrowing_construction_and_forwarding) {
    check_capture_forwarding<leaf>();
    check_capture_forwarding<compressed_leaf>();
}

TEST(unexpected, initializer_list_forwards_move_only_argument) {
    auto ptr = std::make_unique<int>(42);
    wjr::unexpected<nothrow_list_error> value(std::in_place, {1, 2, 3}, std::move(ptr));
    EXPECT_EQ(value.error().size, 3u);
    EXPECT_EQ(*value.error().value, 42);
    EXPECT_EQ(ptr, nullptr);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(capture_leaf, constructor_exceptions_propagate) {
    EXPECT_THROW((leaf<throwing_empty>(7)), construction_error);
    EXPECT_THROW((leaf<throwing_value>(7)), construction_error);
    EXPECT_THROW((compressed_leaf<throwing_empty>(7)), construction_error);
    EXPECT_THROW((compressed_leaf<throwing_value>(7)), construction_error);
}

TEST(unexpected, initializer_list_constructor_exceptions_propagate) {
    EXPECT_THROW(
        (wjr::unexpected<throwing_list_error>(std::in_place, {1, 2}, std::make_unique<int>(7))),
        construction_error);
}
#endif
