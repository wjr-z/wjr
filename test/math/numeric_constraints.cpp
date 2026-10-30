#include <wjr/math/int128_t.hpp>
#include <wjr/math/prefix_xor.hpp>

namespace {

template <typename T>
constexpr bool integral_trait_domain =
    wjr::nonbool_integral<T> == wjr::is_nonbool_integral_v<T> &&
    wjr::nonbool_unsigned_integral<T> == wjr::is_nonbool_unsigned_integral_v<T> &&
    wjr::nonbool_signed_integral<T> == wjr::is_nonbool_signed_integral_v<T>;

static_assert(integral_trait_domain<bool> && integral_trait_domain<const bool> &&
              integral_trait_domain<volatile bool> && integral_trait_domain<const volatile bool> &&
              integral_trait_domain<int &> && integral_trait_domain<const unsigned &> &&
              integral_trait_domain<int> && integral_trait_domain<unsigned> &&
              integral_trait_domain<const int> && integral_trait_domain<volatile unsigned> &&
              integral_trait_domain<float> && integral_trait_domain<void *>);
static_assert(!wjr::nonbool_integral<bool> && wjr::nonbool_integral<const bool>);

template <typename Wide, typename T>
concept wide_scalar = requires(Wide value, T scalar) {
    value.template operator= <T>(scalar);
    value.template operator+= <T>(scalar);
    value.template operator-= <T>(scalar);
    value.template operator*= <T>(scalar);
    value.operator T();
};

template <typename Wide>
constexpr bool wide_domain() {
    return wide_scalar<Wide, int8_t> && wide_scalar<Wide, uint8_t> && wide_scalar<Wide, int16_t> &&
           wide_scalar<Wide, uint16_t> && wide_scalar<Wide, int32_t> &&
           wide_scalar<Wide, uint32_t> && wide_scalar<Wide, int64_t> &&
           wide_scalar<Wide, uint64_t> && !wide_scalar<Wide, bool> && !wide_scalar<Wide, float> &&
           !wide_scalar<Wide, void *>;
}

static_assert(wide_domain<wjr::uint128_t>());
static_assert(wide_domain<wjr::int128_t>());

template <typename T>
concept unsigned_primitives = requires(T x, T y, T high) {
    wjr::mul(x, y, high);
    wjr::mulhi(x, y);
    wjr::mullo(x, y);
    wjr::mul_overflow(x, y, high);
    wjr::prefix_xor(x);
};

static_assert(unsigned_primitives<uint8_t> && unsigned_primitives<uint16_t> &&
              unsigned_primitives<uint32_t> && unsigned_primitives<uint64_t>);
static_assert(!unsigned_primitives<bool> && !unsigned_primitives<int> &&
              !unsigned_primitives<double>);

constexpr bool scalar_arithmetic() {
    wjr::uint128_t value(7u);
    value += 2;
    value *= 3u;
    value -= 6;
    wjr::int128_t signed_value(-7);
    signed_value *= -3;
    return value.low == 21 && value.high == 0 && signed_value == wjr::int128_t(21);
}

static_assert(scalar_arithmetic());

#if WJR_HAS_BUILTIN(ASM_DIV2BY1_ADJUST)
template <typename T>
concept divider_adjust =
    requires(T value, T high) { wjr::asm_div2by1_adjust(value, value, high, value); };
static_assert(divider_adjust<uint64_t> && !divider_adjust<uint32_t>);
#endif

} // namespace
