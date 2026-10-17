#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/functional.hpp>

namespace {

struct qualified_callable {
    int operator()(int value) & noexcept { return value + 1; }
    int operator()(int value) const & noexcept { return value + 2; }
    int operator()(int value) volatile & noexcept { return value + 3; }
    int operator()(int value) const volatile & noexcept { return value + 4; }
};

struct mutable_only {
    int operator()() & noexcept { return 1; }
};

struct throwing_const {
    int operator()() & noexcept { return 1; }
    int operator()() const & { return 2; }
};

template <typename Signature>
constexpr bool accepts_cv_targets =
    std::is_constructible_v<wjr::function_ref<Signature>, qualified_callable &> &&
    std::is_constructible_v<wjr::function_ref<Signature>, const qualified_callable &> &&
    std::is_constructible_v<wjr::function_ref<Signature>, volatile qualified_callable &> &&
    std::is_constructible_v<wjr::function_ref<Signature>, const volatile qualified_callable &>;

static_assert(accepts_cv_targets<int(int)>);
static_assert(accepts_cv_targets<int(int) const>);
static_assert(accepts_cv_targets<int(int) noexcept>);
static_assert(accepts_cv_targets<int(int) const noexcept>);
static_assert(std::is_constructible_v<wjr::function_ref<int()>, mutable_only &>);
static_assert(!std::is_constructible_v<wjr::function_ref<int()>, const mutable_only &>);
static_assert(!std::is_constructible_v<wjr::function_ref<int() const>, mutable_only &>);
static_assert(std::is_constructible_v<wjr::function_ref<int() noexcept>, throwing_const &>);
static_assert(!std::is_constructible_v<wjr::function_ref<int() noexcept>, const throwing_const &>);
static_assert(!std::is_constructible_v<wjr::function_ref<int() const noexcept>, throwing_const &>);
static_assert(std::is_constructible_v<wjr::function_ref<int()>, const throwing_const &>);
static_assert(std::is_trivially_copyable_v<wjr::function_ref<int(int)>>);
static_assert(std::is_trivially_copyable_v<wjr::function_ref<int(int) const noexcept>>);
static_assert(!std::is_default_constructible_v<wjr::function_ref<int(int)>>);
static_assert(sizeof(wjr::function_ref<int(int)>) == 2 * sizeof(void *));
static_assert(sizeof(wjr::function_ref<int(int) const noexcept>) == 2 * sizeof(void *));

template <typename Signature>
void check_cv_dispatch(bool impose_const) {
    qualified_callable mutable_target;
    const qualified_callable const_target;
    volatile qualified_callable volatile_target;
    const volatile qualified_callable cv_target;

    wjr::function_ref<Signature> first = mutable_target;
    wjr::function_ref<Signature> second = const_target;
    wjr::function_ref<Signature> third = volatile_target;
    wjr::function_ref<Signature> fourth = cv_target;
    EXPECT_EQ(first(10), impose_const ? 12 : 11);
    EXPECT_EQ(second(10), 12);
    EXPECT_EQ(third(10), impose_const ? 14 : 13);
    EXPECT_EQ(fourth(10), 14);

    auto copy = fourth;
    EXPECT_EQ(copy(10), 14);
}

int square(int value) noexcept { return value * value; }

} // namespace

TEST(function_ref_contracts, preserves_target_cv_qualification) {
    check_cv_dispatch<int(int)>(false);
    check_cv_dispatch<int(int) noexcept>(false);
}

TEST(function_ref_contracts, const_signature_adds_const_without_losing_volatile) {
    check_cv_dispatch<int(int) const>(true);
    check_cv_dispatch<int(int) const noexcept>(true);
}

TEST(function_ref_contracts, retains_throwing_const_invocation) {
    const throwing_const target;
    wjr::function_ref<int()> reference = target;
    EXPECT_EQ(reference(), 2);
}

TEST(function_ref_contracts, retains_function_and_nontype_targets) {
    wjr::function_ref<int(int) noexcept> direct = &square;
    wjr::function_ref<int(int) noexcept> fixed = wjr::nontype<&square>;
    wjr::function_ref<int(int) const noexcept> constant = wjr::nontype<&square>;
    EXPECT_EQ(direct(4), 16);
    EXPECT_EQ(fixed(4), 16);
    EXPECT_EQ(constant(4), 16);
}
