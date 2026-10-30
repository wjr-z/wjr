#include "../detail.hpp"

#include <concepts>

#include <wjr/math/bit.hpp>
#include <wjr/math/count_digits.hpp>

using namespace wjr;

namespace {
struct convertible_unsigned {
    constexpr operator unsigned int() const noexcept { return 1; }
};

struct nontrivial {
    ~nontrivial() {}
    unsigned int value;
};

template <typename T>
concept unsigned_bit_operations = requires(T value) {
    { lowbit(value) } -> std::same_as<T>;
    { clear_lowbit(value) } -> std::same_as<T>;
    { is_zero_or_single_bit(value) } -> std::same_as<bool>;
    { has_single_bit(value) } -> std::same_as<bool>;
    { popcount(value) } -> std::same_as<int>;
    { clz(value) } -> std::same_as<int>;
    { ctz(value) } -> std::same_as<int>;
    { countl_zero(value) } -> std::same_as<int>;
    { countr_zero(value) } -> std::same_as<int>;
    { countl_one(value) } -> std::same_as<int>;
    { countr_one(value) } -> std::same_as<int>;
    { bit_width(value) } -> std::same_as<int>;
    { bit_floor(value) } -> std::same_as<T>;
    { bit_ceil(value) } -> std::same_as<T>;
    { rotl(value, 1) } -> std::same_as<T>;
    { rotr(value, 1) } -> std::same_as<T>;
    { count_digits<2>(value) } -> std::same_as<int>;
    { count_digits<8>(value) } -> std::same_as<int>;
    { count_digits<10>(value) } -> std::same_as<int>;
    { count_digits<16>(value) } -> std::same_as<int>;
    { count_digits<1>(value, 4) } -> std::same_as<int>;
};

template <typename To, typename From>
concept can_bitwise_cast = requires(const From &value) { bitwise_cast<To>(value); };

template <typename T>
concept signed_helpers = requires(T value) {
    { _fast_negate(value) } -> std::same_as<T>;
    { _fast_abs(value) } -> std::same_as<std::make_unsigned_t<T>>;
};

static_assert(unsigned_bit_operations<uint8_t>);
static_assert(unsigned_bit_operations<uint16_t>);
static_assert(unsigned_bit_operations<uint32_t>);
static_assert(unsigned_bit_operations<uint64_t>);
static_assert(!unsigned_bit_operations<bool>);
static_assert(!unsigned_bit_operations<int>);
static_assert(!unsigned_bit_operations<float>);
static_assert(!unsigned_bit_operations<convertible_unsigned>);
static_assert(signed_helpers<int>);
static_assert(!signed_helpers<unsigned int>);
static_assert(!signed_helpers<bool>);
static_assert(!signed_helpers<float>);
static_assert(can_bitwise_cast<uint32_t, float>);
static_assert(can_bitwise_cast<uint64_t, uint32_t>);
static_assert(!can_bitwise_cast<nontrivial, uint32_t>);
static_assert(!can_bitwise_cast<uint32_t, nontrivial>);

static_assert(ctz(uint16_t{8}) == 3);
static_assert(clz(uint16_t{8}) == 12);
static_assert(countr_zero(uint16_t{0}) == 16);
static_assert(countl_zero(uint16_t{0}) == 16);
static_assert(popcount(uint16_t{0xa5}) == 4);
static_assert(rotl(uint8_t{0x81}, 1) == uint8_t{0x03});
static_assert(rotr(uint8_t{0x81}, 1) == uint8_t{0xc0});
static_assert(count_digits<10>(uint64_t{1000}) == 4);
} // namespace
