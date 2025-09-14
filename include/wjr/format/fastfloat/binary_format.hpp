#ifndef WJR_FORMAT_FASTFLOAT_BINARY_FORMAT_HPP__
#define WJR_FORMAT_FASTFLOAT_BINARY_FORMAT_HPP__

#include <cfloat>
#include <cstdint>

#include <wjr/type_traits.hpp>

namespace wjr::fastfloat {
// used for binary_format_lookup_tables<T>::max_mantissa
constexpr uint64_t constant_55555 = 5 * 5 * 5 * 5 * 5;

template <typename T, typename U = void>
struct binary_format_lookup_tables;

template <typename T>
struct binary_format : binary_format_lookup_tables<T> {
    using equiv_uint = uint_t<sizeof(T) * 8>;

    static constexpr int mantissa_explicit_bits();
    static constexpr int minimum_exponent();
    static constexpr int infinite_power();
    static constexpr int sign_index();
    static constexpr int min_exponent_fast_path(); // used when fegetround() == FE_TONEAREST
    static constexpr int max_exponent_fast_path();
    static constexpr int max_exponent_round_to_even();
    static constexpr int min_exponent_round_to_even();
    static constexpr uint64_t max_mantissa_fast_path(int64_t power);
    static constexpr uint64_t max_mantissa_fast_path(); // used when fegetround() == FE_TONEAREST
    static constexpr int largest_power_of_ten();
    static constexpr int smallest_power_of_ten();
    static constexpr T exact_power_of_ten(int64_t power);
    static constexpr size_t max_digits();
    static constexpr equiv_uint exponent_mask();
    static constexpr equiv_uint mantissa_mask();
    static constexpr equiv_uint hidden_bit_mask();
};

template <typename U>
struct binary_format_lookup_tables<double, U> {
    static constexpr double powers_of_ten[] = {1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
                                               1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
                                               1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

    // Largest integer value v so that (5**index * v) <= 1<<53.
    // 0x20000000000000 == 1 << 53
    static constexpr uint64_t max_mantissa[] = {
        0x20000000000000,
        0x20000000000000 / 5,
        0x20000000000000 / (5 * 5),
        0x20000000000000 / (5 * 5 * 5),
        0x20000000000000 / (5 * 5 * 5 * 5),
        0x20000000000000 / (constant_55555),
        0x20000000000000 / (constant_55555 * 5),
        0x20000000000000 / (constant_55555 * 5 * 5),
        0x20000000000000 / (constant_55555 * 5 * 5 * 5),
        0x20000000000000 / (constant_55555 * 5 * 5 * 5 * 5),
        0x20000000000000 / (constant_55555 * constant_55555),
        0x20000000000000 / (constant_55555 * constant_55555 * 5),
        0x20000000000000 / (constant_55555 * constant_55555 * 5 * 5),
        0x20000000000000 / (constant_55555 * constant_55555 * 5 * 5 * 5),
        0x20000000000000 / (constant_55555 * constant_55555 * constant_55555),
        0x20000000000000 / (constant_55555 * constant_55555 * constant_55555 * 5),
        0x20000000000000 / (constant_55555 * constant_55555 * constant_55555 * 5 * 5),
        0x20000000000000 / (constant_55555 * constant_55555 * constant_55555 * 5 * 5 * 5),
        0x20000000000000 / (constant_55555 * constant_55555 * constant_55555 * 5 * 5 * 5 * 5),
        0x20000000000000 / (constant_55555 * constant_55555 * constant_55555 * constant_55555),
        0x20000000000000 / (constant_55555 * constant_55555 * constant_55555 * constant_55555 * 5),
        0x20000000000000 /
            (constant_55555 * constant_55555 * constant_55555 * constant_55555 * 5 * 5),
        0x20000000000000 /
            (constant_55555 * constant_55555 * constant_55555 * constant_55555 * 5 * 5 * 5),
        0x20000000000000 /
            (constant_55555 * constant_55555 * constant_55555 * constant_55555 * 5 * 5 * 5 * 5)};
};

template <typename U>
struct binary_format_lookup_tables<float, U> {
    static constexpr float powers_of_ten[] = {1e0f, 1e1f, 1e2f, 1e3f, 1e4f, 1e5f,
                                              1e6f, 1e7f, 1e8f, 1e9f, 1e10f};

    // Largest integer value v so that (5**index * v) <= 1<<24.
    // 0x1000000 == 1<<24
    static constexpr uint64_t max_mantissa[] = {0x1000000,
                                                0x1000000 / 5,
                                                0x1000000 / (5 * 5),
                                                0x1000000 / (5 * 5 * 5),
                                                0x1000000 / (5 * 5 * 5 * 5),
                                                0x1000000 / (constant_55555),
                                                0x1000000 / (constant_55555 * 5),
                                                0x1000000 / (constant_55555 * 5 * 5),
                                                0x1000000 / (constant_55555 * 5 * 5 * 5),
                                                0x1000000 / (constant_55555 * 5 * 5 * 5 * 5),
                                                0x1000000 / (constant_55555 * constant_55555),
                                                0x1000000 / (constant_55555 * constant_55555 * 5)};
};

template <>
inline constexpr int binary_format<double>::min_exponent_fast_path() {
#if (FLT_EVAL_METHOD != 1) && (FLT_EVAL_METHOD != 0)
    return 0;
#else
    return -22;
#endif
}

template <>
inline constexpr int binary_format<float>::min_exponent_fast_path() {
#if (FLT_EVAL_METHOD != 1) && (FLT_EVAL_METHOD != 0)
    return 0;
#else
    return -10;
#endif
}

template <>
inline constexpr int binary_format<double>::mantissa_explicit_bits() {
    return 52;
}

template <>
inline constexpr int binary_format<float>::mantissa_explicit_bits() {
    return 23;
}

template <>
inline constexpr int binary_format<double>::max_exponent_round_to_even() {
    return 23;
}

template <>
inline constexpr int binary_format<float>::max_exponent_round_to_even() {
    return 10;
}

template <>
inline constexpr int binary_format<double>::min_exponent_round_to_even() {
    return -4;
}

template <>
inline constexpr int binary_format<float>::min_exponent_round_to_even() {
    return -17;
}

template <>
inline constexpr int binary_format<double>::minimum_exponent() {
    return -1023;
}

template <>
inline constexpr int binary_format<float>::minimum_exponent() {
    return -127;
}

template <>
inline constexpr int binary_format<double>::infinite_power() {
    return 0x7FF;
}

template <>
inline constexpr int binary_format<float>::infinite_power() {
    return 0xFF;
}

template <>
inline constexpr int binary_format<double>::sign_index() {
    return 63;
}

template <>
inline constexpr int binary_format<float>::sign_index() {
    return 31;
}

template <>
inline constexpr int binary_format<double>::max_exponent_fast_path() {
    return 22;
}

template <>
inline constexpr int binary_format<float>::max_exponent_fast_path() {
    return 10;
}

template <>
inline constexpr uint64_t binary_format<double>::max_mantissa_fast_path() {
    return uint64_t(2) << mantissa_explicit_bits();
}

template <>
inline constexpr uint64_t binary_format<float>::max_mantissa_fast_path() {
    return uint64_t(2) << mantissa_explicit_bits();
}

template <>
inline constexpr uint64_t binary_format<double>::max_mantissa_fast_path(int64_t power) {
    // caller is responsible to ensure that
    // power >= 0 && power <= 22
    //
    // Work around clang bug https://godbolt.org/z/zedh7rrhc
    return (void)max_mantissa[0], max_mantissa[power];
}

template <>
inline constexpr uint64_t binary_format<float>::max_mantissa_fast_path(int64_t power) {
    // caller is responsible to ensure that
    // power >= 0 && power <= 10
    //
    // Work around clang bug https://godbolt.org/z/zedh7rrhc
    return (void)max_mantissa[0], max_mantissa[power];
}

template <>
inline constexpr double binary_format<double>::exact_power_of_ten(int64_t power) {
    // Work around clang bug https://godbolt.org/z/zedh7rrhc
    return (void)powers_of_ten[0], powers_of_ten[power];
}

template <>
inline constexpr float binary_format<float>::exact_power_of_ten(int64_t power) {
    // Work around clang bug https://godbolt.org/z/zedh7rrhc
    return (void)powers_of_ten[0], powers_of_ten[power];
}

template <>
inline constexpr int binary_format<double>::largest_power_of_ten() {
    return 308;
}

template <>
inline constexpr int binary_format<float>::largest_power_of_ten() {
    return 38;
}

template <>
inline constexpr int binary_format<double>::smallest_power_of_ten() {
    return -342;
}

template <>
inline constexpr int binary_format<float>::smallest_power_of_ten() {
    return -64;
}

template <>
inline constexpr size_t binary_format<double>::max_digits() {
    return 769;
}

template <>
inline constexpr size_t binary_format<float>::max_digits() {
    return 114;
}

template <>
inline constexpr binary_format<float>::equiv_uint binary_format<float>::exponent_mask() {
    return 0x7F800000;
}

template <>
inline constexpr binary_format<double>::equiv_uint binary_format<double>::exponent_mask() {
    return 0x7FF0000000000000;
}

template <>
inline constexpr binary_format<float>::equiv_uint binary_format<float>::mantissa_mask() {
    return 0x007FFFFF;
}

template <>
inline constexpr binary_format<double>::equiv_uint binary_format<double>::mantissa_mask() {
    return 0x000FFFFFFFFFFFFF;
}

template <>
inline constexpr binary_format<float>::equiv_uint binary_format<float>::hidden_bit_mask() {
    return 0x00800000;
}

template <>
inline constexpr binary_format<double>::equiv_uint binary_format<double>::hidden_bit_mask() {
    return 0x0010000000000000;
}
} // namespace wjr::fastfloat

#endif // WJR_FORMAT_FASTFLOAT_BINARY_FORMAT_HPP__