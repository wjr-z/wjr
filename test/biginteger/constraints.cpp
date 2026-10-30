#include <wjr/biginteger/biginteger.hpp>

namespace {

template <typename T>
concept integral_interface = requires(wjr::biginteger &dst, const wjr::biginteger &src, T value) {
    wjr::biginteger(value);
    dst = value;
    dst.operator T();
    wjr::add(dst, src, value);
    wjr::sub(dst, value, src);
    wjr::mul(dst, src, value);
    wjr::tdiv_q(dst, src, value);
    wjr::compare(src, value);
    wjr::compare(value, src);
};

static_assert(integral_interface<int8_t> && integral_interface<uint8_t> &&
              integral_interface<int16_t> && integral_interface<uint16_t> &&
              integral_interface<int32_t> && integral_interface<uint32_t> &&
              integral_interface<int64_t> && integral_interface<uint64_t>);
static_assert(!integral_interface<bool> && !integral_interface<float> &&
              !integral_interface<void *>);

template <typename Engine>
concept random_interface = requires(wjr::biginteger &dst, Engine &engine) {
    wjr::urandom_bit(dst, 128, engine);
    wjr::urandom_exact_bit(dst, 128, engine);
    wjr::urandom(dst, dst, engine);
    wjr::urandom_exact(dst, dst, engine);
};

static_assert(random_interface<std::mt19937_64>);
static_assert(!random_interface<std::mt19937>);

using expression = decltype(std::declval<wjr::biginteger &>() + 1);
static_assert(std::is_constructible_v<wjr::biginteger, expression>);
static_assert(std::is_assignable_v<wjr::biginteger &, expression>);
static_assert(!std::is_constructible_v<wjr::biginteger_detail::ref_expr<int>, const char *>);
static_assert(std::is_same_v<wjr::biginteger_detail::expr_value_category_t<wjr::biginteger &>,
                             const wjr::biginteger &>);
static_assert(std::is_same_v<wjr::biginteger_detail::expr_value_category_t<wjr::biginteger &&>,
                             wjr::biginteger>);

constexpr bool owned_expression_value() {
    wjr::biginteger_detail::ref_expr<int> leaf(4);
    leaf.value() = 9;
    return leaf.value() == 9;
}

static_assert(owned_expression_value());
static_assert(std::is_standard_layout_v<wjr::biginteger>);

} // namespace
