#include "../detail.hpp"

#include <concepts>
#include <limits>

#include <wjr/math/add.hpp>
#include <wjr/math/compare.hpp>
#include <wjr/math/set.hpp>
#include <wjr/math/shift.hpp>
#include <wjr/math/sub.hpp>

using namespace wjr;

namespace {
template <typename T, typename Carry>
concept carry_arithmetic = requires(T value, Carry carry) {
    { addc(value, value, Carry{}, carry) } -> std::same_as<T>;
    { subc(value, value, Carry{}, carry) } -> std::same_as<T>;
};

template <typename T>
concept add_overflow_available = requires(T value) {
    { add_overflow(value, value, value) } -> std::same_as<bool>;
};

template <typename T>
concept sub_overflow_available = requires(T value) {
    { sub_overflow(value, value, value) } -> std::same_as<bool>;
};

template <typename T>
concept unsigned_array_arithmetic = requires(T *dst, const T *src, T value) {
    { set_n(dst, value, size_t{2}) } -> std::same_as<void>;
    { lshift_n(dst, src, size_t{2}, 1) } -> std::same_as<T>;
    { rshift_n(dst, src, size_t{2}, 1) } -> std::same_as<T>;
    { lshiftc_n(dst, src, size_t{2}, 1) } -> std::same_as<T>;
    { rshiftc_n(dst, src, size_t{2}, 1) } -> std::same_as<T>;
};

template <typename T>
concept integral_comparison = requires(const T *src) {
    { compare_n(src, src, size_t{2}) } -> std::same_as<int>;
    { reverse_compare_n(src, src, size_t{2}) } -> std::same_as<int>;
};

static_assert(carry_arithmetic<uint8_t, uint8_t>);
static_assert(carry_arithmetic<uint64_t, uint64_t>);
static_assert(carry_arithmetic<uint64_t, bool>);
static_assert(!carry_arithmetic<bool, bool>);
static_assert(!carry_arithmetic<int64_t, uint8_t>);
static_assert(!carry_arithmetic<uint64_t, int>);
static_assert(!carry_arithmetic<float, uint8_t>);
static_assert(add_overflow_available<int64_t>);
static_assert(add_overflow_available<uint64_t>);
static_assert(!add_overflow_available<bool>);
static_assert(!add_overflow_available<float>);
static_assert(sub_overflow_available<uint64_t>);
static_assert(!sub_overflow_available<int64_t>);
static_assert(!sub_overflow_available<bool>);
static_assert(unsigned_array_arithmetic<uint8_t>);
static_assert(unsigned_array_arithmetic<uint16_t>);
static_assert(unsigned_array_arithmetic<uint32_t>);
static_assert(unsigned_array_arithmetic<uint64_t>);
static_assert(!unsigned_array_arithmetic<int64_t>);
static_assert(!unsigned_array_arithmetic<bool>);
static_assert(!unsigned_array_arithmetic<float>);
static_assert(integral_comparison<int64_t>);
static_assert(integral_comparison<uint64_t>);
static_assert(!integral_comparison<bool>);
static_assert(!integral_comparison<float>);

constexpr bool valid_arithmetic() {
    uint64_t carry = 0;
    const auto max = std::numeric_limits<uint64_t>::max();
    if (addc(max, uint64_t{0}, uint64_t{1}, carry) != 0 || carry != 1) {
        return false;
    }
    if (subc(uint64_t{0}, uint64_t{0}, uint64_t{1}, carry) != max || carry != 1) {
        return false;
    }
    uint64_t output = 0;
    if (!add_overflow(max, uint64_t{1}, output) || output != 0) {
        return false;
    }
    uint64_t values[2] = {max, 0};
    (void)lshift_n(values, values, 2, 1);
    (void)rshift_n(values, values, 2, 1);
    return values[0] == max && values[1] == 0;
}

static_assert(valid_arithmetic());
} // namespace
