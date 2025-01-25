#include <wjr/biginteger.hpp>
#include <wjr/format/fastfloat.hpp>

namespace wjr::fastfloat {

template from_chars_result<>
__from_chars_impl<default_writer<float>, integral_constant<chars_format, chars_format::general>>(
    const char *first, const char *last, default_writer<float> wr,
    integral_constant<chars_format, chars_format::general> options) noexcept;

template from_chars_result<>
__from_chars_impl<default_writer<double>, integral_constant<chars_format, chars_format::general>>(
    const char *first, const char *last, default_writer<double> wr,
    integral_constant<chars_format, chars_format::general> options) noexcept;

template from_chars_result<> __from_chars_impl<default_writer<float>, chars_format>(
    const char *first, const char *last, default_writer<float> wr, chars_format fmt) noexcept;

template from_chars_result<> __from_chars_impl<default_writer<double>, chars_format>(
    const char *first, const char *last, default_writer<double> wr, chars_format fmt) noexcept;

namespace {
// this converts a native floating-point number to an extended-precision float.
template <typename T>
WJR_INTRINSIC_INLINE adjusted_mantissa to_extended(T value) noexcept {
    using equiv_uint = typename binary_format<T>::equiv_uint;
    constexpr equiv_uint exponent_mask = binary_format<T>::exponent_mask();
    constexpr equiv_uint mantissa_mask = binary_format<T>::mantissa_mask();
    constexpr equiv_uint hidden_bit_mask = binary_format<T>::hidden_bit_mask();

    adjusted_mantissa am;
    int32_t bias =
        binary_format<T>::mantissa_explicit_bits() - binary_format<T>::minimum_exponent();
    auto bits = bit_cast<equiv_uint>(value);
    if ((bits & exponent_mask) == 0) {
        // denormal
        am.power2 = 1 - bias;
        am.mantissa = bits & mantissa_mask;
    } else {
        // normal
        am.power2 = int32_t((bits & exponent_mask) >> binary_format<T>::mantissa_explicit_bits());
        am.power2 -= bias;
        am.mantissa = (bits & mantissa_mask) | hidden_bit_mask;
    }

    return am;
}

// get the extended precision value of the halfway point between b and b+u.
// we are given a native float that represents b, so we need to adjust it
// halfway between b and b+u.
template <typename T>
WJR_INTRINSIC_INLINE adjusted_mantissa to_extended_halfway(T value) noexcept {
    adjusted_mantissa am = to_extended(value);
    am.mantissa <<= 1;
    am.mantissa += 1;
    am.power2 -= 1;
    return am;
}

// round an extended-precision float to the nearest machine float.
template <typename T, typename callback>
WJR_INTRINSIC_INLINE void round(adjusted_mantissa &am, callback cb) noexcept {
    int32_t mantissa_shift = 64 - binary_format<T>::mantissa_explicit_bits() - 1;
    if (-am.power2 >= mantissa_shift) {
        // have a denormal float
        int32_t shift = -am.power2 + 1;
        cb(am, std::min<int32_t>(shift, 64));
        // check for round-up: if rounding-nearest carried us to the hidden bit.
        am.power2 =
            (am.mantissa < (uint64_t(1) << binary_format<T>::mantissa_explicit_bits())) ? 0 : 1;
        return;
    }

    // have a normal float, use the default shift.
    cb(am, mantissa_shift);

    // check for carry
    if (am.mantissa >= (uint64_t(2) << binary_format<T>::mantissa_explicit_bits())) {
        am.mantissa = (uint64_t(1) << binary_format<T>::mantissa_explicit_bits());
        am.power2++;
    }

    // check for infinite: we could have carried to an infinite power
    am.mantissa &= ~(uint64_t(1) << binary_format<T>::mantissa_explicit_bits());
    if (am.power2 >= binary_format<T>::infinite_power()) {
        am.power2 = binary_format<T>::infinite_power();
        am.mantissa = 0;
    }
}

template <typename callback>
WJR_INTRINSIC_INLINE void round_nearest_tie_even(adjusted_mantissa &am, int32_t shift,
                                                 callback cb) noexcept {
    uint64_t mask;
    uint64_t halfway;
    if (shift == 64) {
        mask = UINT64_MAX;
    } else {
        mask = (uint64_t(1) << shift) - 1;
    }
    if (shift == 0) {
        halfway = 0;
    } else {
        halfway = uint64_t(1) << (shift - 1);
    }
    uint64_t truncated_bits = am.mantissa & mask;
    uint64_t is_above = truncated_bits > halfway;
    uint64_t is_halfway = truncated_bits == halfway;

    // shift digits into position
    if (shift == 64) {
        am.mantissa = 0;
    } else {
        am.mantissa >>= shift;
    }
    am.power2 += shift;

    const bool is_odd = (am.mantissa & 1) == 1;
    am.mantissa += uint64_t(cb(is_odd, is_halfway, is_above));
}

WJR_INTRINSIC_INLINE void round_down(adjusted_mantissa &am, int32_t shift) noexcept {
    if (shift == 64) {
        am.mantissa = 0;
    } else {
        am.mantissa >>= shift;
    }
    am.power2 += shift;
}

WJR_INTRINSIC_INLINE void skip_zeros(const char *&first, const char *last) noexcept {
    uint64_t val;
    while (std::distance(first, last) >= 8) {
        val = read_memory<uint64_t>(first);
        if (val != 0x3030303030303030) {
            break;
        }
        first += 8;
    }
    while (first != last) {
        if (*first != '0') {
            break;
        }
        first++;
    }
}

// determine if any non-zero digits were truncated.
// all characters must be valid digits.
WJR_INTRINSIC_INLINE bool is_truncated(const char *first, const char *last) noexcept {
    // do 8-bit optimizations, can just compare to 8 literal 0s.
    uint64_t val;
    while (std::distance(first, last) >= 8) {
        val = read_memory<uint64_t>(first);
        if (val != 0x3030303030303030) {
            return true;
        }
        first += 8;
    }
    while (first != last) {
        if (*first != '0') {
            return true;
        }
        first++;
    }
    return false;
}

WJR_INTRINSIC_INLINE bool is_truncated(span<const char> s) noexcept {
    return is_truncated(s.begin_unsafe(), s.end_unsafe());
}

WJR_INTRINSIC_INLINE void parse_eight_digits(const char *&p, uint64_t &value, size_t &counter,
                                             size_t &count) noexcept {
    value = value * 100000000 + parse_eight_digits_unrolled(p);
    p += 8;
    counter += 8;
    count += 8;
}

WJR_INTRINSIC_INLINE void parse_one_digit(const char *&p, uint64_t &value, size_t &counter,
                                          size_t &count) noexcept {
    value = value * 10 + uint64_t(*p - '0');
    p++;
    counter++;
    count++;
}

inline constexpr size_t max_limbs = 4000 / 64;
using bigint = fixed_stack_biginteger;

// 1e0 to 1e19
inline constexpr uint64_t powers_of_ten_uint64[] = {1UL,
                                                    10UL,
                                                    100UL,
                                                    1000UL,
                                                    10000UL,
                                                    100000UL,
                                                    1000000UL,
                                                    10000000UL,
                                                    100000000UL,
                                                    1000000000UL,
                                                    10000000000UL,
                                                    100000000000UL,
                                                    1000000000000UL,
                                                    10000000000000UL,
                                                    100000000000000UL,
                                                    1000000000000000UL,
                                                    10000000000000000UL,
                                                    100000000000000000UL,
                                                    1000000000000000000UL,
                                                    10000000000000000000UL};

WJR_INTRINSIC_INLINE void add_native(bigint &big, uint64_t power, uint64_t value) noexcept {
    big *= power;
    big += value;
}

WJR_INTRINSIC_INLINE void round_up_bigint(bigint &big, size_t &count) noexcept {
    // need to round-up the digits, but need to avoid rounding
    // ....9999 to ...10000, which could cause a false halfway point.
    add_native(big, 10, 1);
    count++;
}

WJR_INTRINSIC_INLINE void pow5(bigint &big, uint32_t exp) noexcept {
    static constexpr uint64_t small_power_of_5[] = {
        1UL,
        5UL,
        25UL,
        125UL,
        625UL,
        3125UL,
        15625UL,
        78125UL,
        390625UL,
        1953125UL,
        9765625UL,
        48828125UL,
        244140625UL,
        1220703125UL,
        6103515625UL,
        30517578125UL,
        152587890625UL,
        762939453125UL,
        3814697265625UL,
        19073486328125UL,
        95367431640625UL,
        476837158203125UL,
        2384185791015625UL,
        11920928955078125UL,
        59604644775390625UL,
        298023223876953125UL,
        1490116119384765625UL,
        7450580596923828125UL,
    };

    constexpr static uint64_t large_power_of_5[] = {1414648277510068013UL, 9180637584431281687UL,
                                                    4539964771860779200UL, 10482974169319127550UL,
                                                    198276706040285095UL};

    constexpr uint32_t large_step = 135;
    constexpr auto large = make_biginteger_data(span(large_power_of_5));

    while (exp >= large_step) {
        big *= large;
        exp -= large_step;
    }

    constexpr uint32_t small_step = 27;
    constexpr uint64_t max_native = 7450580596923828125UL;

    while (exp >= small_step) {
        big *= max_native;
        exp -= small_step;
    }

    if (exp != 0) {
        big *= small_power_of_5[exp];
    }
}

WJR_INTRINSIC_INLINE void pow2(bigint &big, uint32_t exp) noexcept { mul_2exp(big, big, exp); }
WJR_INTRINSIC_INLINE void pow10(bigint &big, uint32_t exp) noexcept {
    pow5(big, exp);
    pow2(big, exp);
}

WJR_INTRINSIC_INLINE uint64_t empty_hi64(bool &truncated) noexcept {
    truncated = false;
    return 0;
}

WJR_INTRINSIC_INLINE uint64_t uint64_hi64(uint64_t r0, bool &truncated) noexcept {
    truncated = false;
    int shl = clz(r0);
    return r0 << shl;
}

WJR_INTRINSIC_INLINE uint64_t uint64_hi64(uint64_t r0, uint64_t r1, bool &truncated) noexcept {
    int shl = clz(r0);
    if (shl == 0) {
        truncated = r1 != 0;
        return r0;
    } else {
        int shr = 64 - shl;
        truncated = (r1 << shl) != 0;
        return (r0 << shl) | (r1 >> shr);
    }
}

// get the high 64 bits from the vector, and if bits were truncated.
// this is to get the significant digits for the float.
WJR_INTRINSIC_INLINE uint64_t hi64(bigint &big, bool &truncated) noexcept {
    const auto n = big.size();
    if (n == 0) {
        return empty_hi64(truncated);
    }
    if (n == 1) {
        return uint64_hi64(big.back(), truncated);
    }

    uint64_t result = uint64_hi64(big.rbegin()[0], big.rbegin()[1], truncated);
    truncated |= reverse_find_not_n(big.data(), 0, n - 2) != 0;
    return result;
}

// parse the significant digits into a big integer
template <typename T>
void parse_mantissa(bigint &result, span<const char> integer, span<const char> fraction,
                    size_t &digits) noexcept {
    constexpr size_t max_digits = binary_format<T>::max_digits();

    // try to minimize the number of big integer and scalar multiplication.
    // therefore, try to parse 8 digits at a time, and multiply by the largest
    // scalar value (9 or 19 digits) for each step.
    size_t counter = 0;
    digits = 0;
    uint64_t value = 0;
    size_t step = 19;

    // process all integer digits.
    const char *p = integer.data();
    const char *pend = p + integer.size();
    skip_zeros(p, pend);
    // process all digits, in increments of step per loop
    while (p != pend) {
        while ((std::distance(p, pend) >= 8) && (step - counter >= 8) &&
               (max_digits - digits >= 8)) {
            parse_eight_digits(p, value, counter, digits);
        }

        while (counter < step && p != pend && digits < max_digits) {
            parse_one_digit(p, value, counter, digits);
        }

        if (digits == max_digits) {
            // add the temporary value, then check if we've truncated any digits
            add_native(result, uint64_t(powers_of_ten_uint64[counter]), value);
            bool truncated = is_truncated(p, pend);
            if (fraction.data() != nullptr) {
                truncated |= is_truncated(fraction);
            }

            if (truncated) {
                round_up_bigint(result, digits);
            }

            return;
        } else {
            add_native(result, uint64_t(powers_of_ten_uint64[counter]), value);
            counter = 0;
            value = 0;
        }
    }

    // add our fraction digits, if they're available.
    if (fraction.data() != nullptr) {
        p = fraction.data();
        pend = p + fraction.size();
        if (digits == 0) {
            skip_zeros(p, pend);
        }
        // process all digits, in increments of step per loop
        while (p != pend) {
            while ((std::distance(p, pend) >= 8) && (step - counter >= 8) &&
                   (max_digits - digits >= 8)) {
                parse_eight_digits(p, value, counter, digits);
            }

            while (counter < step && p != pend && digits < max_digits) {
                parse_one_digit(p, value, counter, digits);
            }

            if (digits == max_digits) {
                // add the temporary value, then check if we've truncated any
                // digits
                add_native(result, uint64_t(powers_of_ten_uint64[counter]), value);

                if (bool truncated = is_truncated(p, pend); truncated) {
                    round_up_bigint(result, digits);
                }

                return;
            } else {
                add_native(result, uint64_t(powers_of_ten_uint64[counter]), value);
                counter = 0;
                value = 0;
            }
        }
    }

    if (counter != 0) {
        add_native(result, uint64_t(powers_of_ten_uint64[counter]), value);
    }
}

template <typename T>
inline adjusted_mantissa positive_digit_comp(bigint &bigmant, int32_t exponent) noexcept {
    pow10(bigmant, static_cast<uint32_t>(exponent));
    adjusted_mantissa answer;
    bool truncated;
    answer.mantissa = hi64(bigmant, truncated);
    int bias = binary_format<T>::mantissa_explicit_bits() - binary_format<T>::minimum_exponent();
    answer.power2 = bit_width(bigmant) - 64 + bias;

    round<T>(answer, [truncated](adjusted_mantissa &a, int32_t shift) {
        round_nearest_tie_even(
            a, shift, [truncated](bool is_odd, bool is_halfway, bool is_above) -> bool {
                return is_above || (is_halfway && truncated) || (is_odd && is_halfway);
            });
    });

    return answer;
}

// the scaling here is quite simple: we have, for the real digits `m * 10^e`,
// and for the theoretical digits `n * 2^f`. Since `e` is always negative,
// to scale them identically, we do `n * 2^f * 5^-f`, so we now have `m * 2^e`.
// we then need to scale by `2^(f- e)`, and then the two significant digits
// are of the same magnitude.
template <typename T>
adjusted_mantissa negative_digit_comp(bigint &bigmant, adjusted_mantissa am,
                                      int32_t exponent) noexcept {
    bigint &real_digits = bigmant;
    int32_t real_exp = exponent;

    // get the value of `b`, rounded down, and get a fixed_stack_biginteger
    // representation of b+h
    adjusted_mantissa am_b = am;
    // gcc7 buf: use a lambda to remove the noexcept qualifier bug with
    // -Wnoexcept-type.
    round<T>(am_b, [](adjusted_mantissa &a, int32_t shift) { round_down(a, shift); });
    T b;
    to_float(false, am_b, b);
    adjusted_mantissa theor = to_extended_halfway(b);
    unique_stack_allocator stkal;
    bigint theor_digits(max_limbs, in_place_reserve, stkal);
    theor_digits = theor.mantissa;
    int32_t theor_exp = theor.power2;

    // scale real digits and theor digits to be same power.
    int32_t pow2_exp = theor_exp - real_exp;
    uint32_t pow5_exp = uint32_t(-real_exp);
    if (pow5_exp != 0) {
        pow5(theor_digits, pow5_exp);
    }
    if (pow2_exp > 0) {
        pow2(theor_digits, static_cast<uint32_t>(pow2_exp));
    } else if (pow2_exp < 0) {
        pow2(real_digits, static_cast<uint32_t>(-pow2_exp));
    }

    // compare digits, and use it to director rounding
    int ord = compare(real_digits, theor_digits);
    adjusted_mantissa answer = am;
    round<T>(answer, [ord](adjusted_mantissa &a, int32_t shift) {
        round_nearest_tie_even(a, shift, [ord](bool is_odd, bool _, bool __) -> bool {
            (void)_;  // not needed, since we've done our comparison
            (void)__; // not needed, since we've done our comparison
            if (ord > 0) {
                return true;
            } else if (ord < 0) {
                return false;
            } else {
                return is_odd;
            }
        });
    });

    return answer;
}
} // namespace

template <typename T>
adjusted_mantissa digit_comp(adjusted_mantissa am, span<const char> integer,
                             span<const char> fraction, int32_t sci_exp) noexcept {
    size_t digits = 0;
    unique_stack_allocator stkal;
    bigint bigmant(max_limbs, in_place_reserve, stkal);
    parse_mantissa<T>(bigmant, integer, fraction, digits);
    int32_t exponent = sci_exp + 1 - int32_t(digits);
    if (exponent >= 0) {
        return positive_digit_comp<T>(bigmant, exponent);
    } else {
        return negative_digit_comp<T>(bigmant, am, exponent);
    }
}

template adjusted_mantissa digit_comp<float>(adjusted_mantissa am, span<const char> integer,
                                             span<const char> fraction, int32_t sci_exp) noexcept;

template adjusted_mantissa digit_comp<double>(adjusted_mantissa am, span<const char> integer,
                                              span<const char> fraction, int32_t sci_exp) noexcept;

} // namespace wjr::fastfloat