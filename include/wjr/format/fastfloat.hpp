// MIT License

// Copyright (c) 2021 The fast_float authors

// Permission is hereby granted, free of charge, to any
// person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the
// Software without restriction, including without
// limitation the rights to use, copy, modify, merge,
// publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software
// is furnished to do so, subject to the following
// conditions:

// The above copyright notice and this permission notice
// shall be included in all copies or substantial portions
// of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef WJR_FORMAT_FASTFLOAT_HPP__
#define WJR_FORMAT_FASTFLOAT_HPP__

#include <cfloat>

#include <wjr/format/charconv.hpp>
#include <wjr/format/fastfloat/fast_table.hpp>
#include <wjr/math/div.hpp>
#include <wjr/span.hpp>
#include <wjr/string.hpp>

namespace wjr::fastfloat {

template <typename T>
struct default_writer {
    using float_type = T;
    using support_integral = std::false_type;

    default_writer() = delete;
    default_writer(const default_writer &) = default;
    default_writer(default_writer &&) = default;
    default_writer &operator=(const default_writer &) = delete;
    default_writer &operator=(default_writer &&) = delete;
    ~default_writer() = default;

    WJR_INTRINSIC_CONSTEXPR default_writer(T &_value) noexcept : value(_value) {}

    WJR_INTRINSIC_CONSTEXPR T &get_float() noexcept { return value; }

    T &value;
};

template <typename Writer, typename Op>
WJR_ALL_NONNULL WJR_NOINLINE from_chars_result<>
__from_chars_impl(const char *first, const char *last, Writer wr, Op fmt) noexcept;

extern template from_chars_result<>
__from_chars_impl<default_writer<float>, integral_constant<chars_format, chars_format::general>>(
    const char *first, const char *last, default_writer<float> wr,
    integral_constant<chars_format, chars_format::general> fmt) noexcept;

extern template from_chars_result<>
__from_chars_impl<default_writer<double>, integral_constant<chars_format, chars_format::general>>(
    const char *first, const char *last, default_writer<double> wr,
    integral_constant<chars_format, chars_format::general> fmt) noexcept;

extern template from_chars_result<> __from_chars_impl<default_writer<float>, chars_format>(
    const char *first, const char *last, default_writer<float> wr, chars_format fmt) noexcept;

extern template from_chars_result<> __from_chars_impl<default_writer<double>, chars_format>(
    const char *first, const char *last, default_writer<double> wr, chars_format fmt) noexcept;

/**
 * This function parses the character sequence [first,last) for a number. It
 * parses floating-point numbers expecting a locale-indepent format equivalent
 * to what is used by std::strtod in the default ("C") locale. The resulting
 * floating-point value is the closest floating-point values (using either float
 * or double), using the "round to even" convention for values that would
 * otherwise fall right in-between two values. That is, we provide exact parsing
 * according to the IEEE standard.
 *
 * Given a successful parse, the pointer (`ptr`) in the returned value is set to
 * point right after the parsed number, and the `value` referenced is set to the
 * parsed value. In case of error, the returned `ec` contains a representative
 * error, otherwise the default (`std::errc()`) value is stored.
 *
 * The implementation does not throw and does not allocate memory (e.g., with
 * `new` or `malloc`).
 *
 * Like the C++17 standard, the `fast_float::from_chars` functions take an
 * optional last argument of the type `fast_float::chars_format`. It is a bitset
 * value: we check whether `fmt & fast_float::chars_format::fixed` and `fmt &
 * fast_float::chars_format::scientific` are set to determine whether we allow
 * the fixed point and scientific notation respectively. The default is
 * `fast_float::chars_format::general` which allows both `fixed` and
 * `scientific`.
 */
template <chars_format Fmt = chars_format::general>
from_chars_result<> from_chars(const char *first, const char *last, float &value,
                               integral_constant<chars_format, Fmt> fmt = {}) noexcept {
    return __from_chars_impl(first, last, default_writer<float>(value), fmt);
}

template <chars_format Fmt = chars_format::general>
from_chars_result<> from_chars(const char *first, const char *last, double &value,
                               integral_constant<chars_format, Fmt> fmt = {}) noexcept {
    return __from_chars_impl(first, last, default_writer<double>(value), fmt);
}

template <typename T, WJR_REQUIRES(is_any_of_v<T, float, double>)>
from_chars_result<> from_chars(const char *first, const char *last, T &value,
                               chars_format fmt) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P(fmt)) {
        if (fmt == chars_format::general) {
            return from_chars(first, last, value);
        }
    }

    WJR_ASSERT(!(fmt & chars_format::__json_format));
    return __from_chars_impl(first, last, default_writer<T>(value), fmt);
}

struct adjusted_mantissa {
    uint64_t mantissa{0};
    int32_t power2{0}; // a negative value indicates an invalid result
    WJR_INTRINSIC_CONSTEXPR bool operator==(const adjusted_mantissa &o) const noexcept {
        return mantissa == o.mantissa && power2 == o.power2;
    }
    WJR_INTRINSIC_CONSTEXPR bool operator!=(const adjusted_mantissa &o) const noexcept {
        return mantissa != o.mantissa || power2 != o.power2;
    }
};

// Bias so we can get the real exponent with an invalid adjusted_mantissa.
inline constexpr int32_t invalid_am_bias = -0x8000;

template <typename T>
WJR_INTRINSIC_INLINE void to_float(bool negative, adjusted_mantissa am, T &value) {
    using fastfloat_uint = typename binary_format<T>::equiv_uint;
    fastfloat_uint word = (fastfloat_uint)am.mantissa;
    word |= fastfloat_uint(am.power2) << binary_format<T>::mantissa_explicit_bits();
    word |= fastfloat_uint(negative) << binary_format<T>::sign_index();
    value = bit_cast<T>(word);
}

// Next function can be micro-optimized, but compilers are entirely
// able to optimize it well.
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool is_integer(char c) noexcept { return c >= '0' && c <= '9'; }

// This will compute or rather approximate w * 5**q and return a pair of 64-bit
// words approximating the result, with the high" part corresponding to the most
// significant bits and the low part corresponding to the least significant
// bits.
//
template <int bit_precision>
WJR_INTRINSIC_INLINE uint128_t compute_product_approximation(int64_t q, uint64_t w) {
    const int index = 2 * int(q - powers::smallest_power_of_five);
    // For small values of q, e.g., q in [0,27], the answer is always exact
    // because The line uint128_t firstproduct = mul64x64to128(w,
    // power_of_five_128[index]); gives the exact answer.
    uint128_t firstproduct = mul64x64to128(w, powers::power_of_five_128[index]);
    static_assert((bit_precision >= 0) && (bit_precision <= 64), " precision should  be in (0,64]");
    constexpr uint64_t precision_mask = (bit_precision < 64)
                                            ? (uint64_t(0xFFFFFFFFFFFFFFFF) >> bit_precision)
                                            : uint64_t(0xFFFFFFFFFFFFFFFF);
    if ((firstproduct.high & precision_mask) ==
        precision_mask) { // could further guard with (lower + w < lower)
        // regarding the second product, we only need secondproduct.high, but
        // our expectation is that the compiler will optimize this extra work
        // away if needed.
        const uint128_t secondproduct = mul64x64to128(w, powers::power_of_five_128[index + 1]);
        firstproduct.low += secondproduct.high;
        if (secondproduct.high > firstproduct.low) {
            firstproduct.high++;
        }
    }
    return firstproduct;
}

namespace detail {
/**
 * For q in (0,350), we have that
 *  f = (((152170 + 65536) * q ) >> 16);
 * is equal to
 *   floor(p) + q
 * where
 *   p = log(5**q)/log(2) = q * log(5)/log(2)
 *
 * For negative values of q in (-400,0), we have that
 *  f = (((152170 + 65536) * q ) >> 16);
 * is equal to
 *   -ceil(p) + q
 * where
 *   p = log(5**-q)/log(2) = -q * log(5)/log(2)
 */
constexpr WJR_INTRINSIC_INLINE int32_t power(int32_t q) noexcept {
    return (((152170 + 65536) * q) >> 16) + 63;
}
} // namespace detail

// create an adjusted mantissa, biased by the invalid power2
// for significant digits already multiplied by 10 ** q.
template <typename binary>
WJR_INTRINSIC_INLINE adjusted_mantissa compute_error_scaled(int64_t q, uint64_t w,
                                                            int lz) noexcept {
    const int hilz = int(w >> 63) ^ 1;
    adjusted_mantissa answer;
    answer.mantissa = w << hilz;
    constexpr int bias = binary::mantissa_explicit_bits() - binary::minimum_exponent();
    answer.power2 = int32_t(detail::power(int32_t(q)) + bias - hilz - lz - 62 + invalid_am_bias);
    return answer;
}

// w * 10 ** q, without rounding the representation up.
// the power2 in the exponent will be adjusted by invalid_am_bias.
template <typename binary>
WJR_INTRINSIC_INLINE adjusted_mantissa compute_error(int64_t q, uint64_t w) noexcept {
    const int lz = clz(w);
    w <<= lz;
    const uint128_t product =
        compute_product_approximation<binary::mantissa_explicit_bits() + 3>(q, w);
    return compute_error_scaled<binary>(q, product.high, lz);
}

// w * 10 ** q
// The returned value should be a valid ieee64 number that simply need to be
// packed. However, in some very rare cases, the computation will fail. In such
// cases, we return an adjusted_mantissa with a negative power of 2: the caller
// should recompute in such cases.
template <typename binary>
WJR_CONST WJR_INTRINSIC_INLINE adjusted_mantissa compute_float(int64_t q, uint64_t w) noexcept {
    adjusted_mantissa answer;
    if ((w == 0) || (q < binary::smallest_power_of_ten())) {
        answer.power2 = 0;
        answer.mantissa = 0;
        // result should be zero
        return answer;
    }
    if (q > binary::largest_power_of_ten()) {
        // we want to get infinity:
        answer.power2 = binary::infinite_power();
        answer.mantissa = 0;
        return answer;
    }
    // At this point in time q is in [powers::smallest_power_of_five,
    // powers::largest_power_of_five].

    // We want the most significant bit of i to be 1. Shift if needed.
    const int lz = clz(w);
    w <<= lz;

    // The required precision is binary::mantissa_explicit_bits() + 3 because
    // 1. We need the implicit bit
    // 2. We need an extra bit for rounding purposes
    // 3. We might lose a bit due to the "upperbit" routine (result too small,
    // requiring a shift)

    const uint128_t product =
        compute_product_approximation<binary::mantissa_explicit_bits() + 3>(q, w);
    if (product.low == 0xFFFFFFFFFFFFFFFF) { //  could guard it further
        // In some very rare cases, this could happen, in which case we might
        // need a more accurate computation that what we can provide cheaply.
        // This is very, very unlikely.
        //
        const bool inside_safe_exponent =
            (q >= -27) && (q <= 55); // always good because 5**q <2**128 when q>=0,
        // and otherwise, for q<0, we have 5**-q<2**64 and the 128-bit
        // reciprocal allows for exact computation.
        if (!inside_safe_exponent) {
            return compute_error_scaled<binary>(q, product.high, lz);
        }
    }
    // The "compute_product_approximation" function can be slightly slower than
    // a branchless approach: uint128_t product = compute_product(q, w); but in
    // practice, we can win big with the compute_product_approximation if its
    // additional branch is easily predicted. Which is best is data specific.
    const int upperbit = int(product.high >> 63);

    answer.mantissa = product.high >> (upperbit + 64 - binary::mantissa_explicit_bits() - 3);

    answer.power2 = int32_t(detail::power(int32_t(q)) + upperbit - lz - binary::minimum_exponent());
    if (answer.power2 <= 0) { // we have a subnormal?
        // Here have that answer.power2 <= 0 so -answer.power2 >= 0
        if (-answer.power2 + 1 >= 64) { // if we have more than 64 bits below the minimum
                                        // exponent, you have a zero for sure.
            answer.power2 = 0;
            answer.mantissa = 0;
            // result should be zero
            return answer;
        }
        // next line is safe because -answer.power2 + 1 < 64
        answer.mantissa >>= -answer.power2 + 1;
        // Thankfully, we can't have both "round-to-even" and subnormals because
        // "round-to-even" only occurs for powers close to 0.
        answer.mantissa += (answer.mantissa & 1); // round up
        answer.mantissa >>= 1;
        // There is a weird scenario where we don't have a subnormal but just.
        // Suppose we start with 2.2250738585072013e-308, we end up
        // with 0x3fffffffffffff x 2^-1023-53 which is technically subnormal
        // whereas 0x40000000000000 x 2^-1023-53  is normal. Now, we need to
        // round up 0x3fffffffffffff x 2^-1023-53  and once we do, we are no
        // longer subnormal, but we can only know this after rounding. So we
        // only declare a subnormal if we are smaller than the threshold.
        answer.power2 =
            (answer.mantissa < (uint64_t(1) << binary::mantissa_explicit_bits())) ? 0 : 1;
        return answer;
    }

    // usually, we round *up*, but if we fall right in between and and we have
    // an even basis, we need to round down We are only concerned with the cases
    // where 5**q fits in single 64-bit word.
    if ((product.low <= 1) && (q >= binary::min_exponent_round_to_even()) &&
        (q <= binary::max_exponent_round_to_even()) &&
        ((answer.mantissa & 3) == 1)) { // we may fall between two floats!
        // To be in-between two floats we need that in doing
        //   answer.mantissa = product.high >> (upperbit + 64 -
        //   binary::mantissa_explicit_bits() - 3);
        // ... we dropped out only zeroes. But if this happened, then we can go
        // back!!!
        if ((answer.mantissa << (upperbit + 64 - binary::mantissa_explicit_bits() - 3)) ==
            product.high) {
            answer.mantissa &= ~uint64_t(1); // flip it so that we do not round up
        }
    }

    answer.mantissa += (answer.mantissa & 1); // round up
    answer.mantissa >>= 1;
    if (answer.mantissa >= (uint64_t(2) << binary::mantissa_explicit_bits())) {
        answer.mantissa = (uint64_t(1) << binary::mantissa_explicit_bits());
        answer.power2++; // undo previous addition
    }

    answer.mantissa &= ~(uint64_t(1) << binary::mantissa_explicit_bits());
    if (answer.power2 >= binary::infinite_power()) { // infinity
        answer.power2 = binary::infinite_power();
        answer.mantissa = 0;
    }
    return answer;
}

/// @brief special case of compute_float when q = 0.
template <typename binary>
WJR_CONST WJR_INTRINSIC_INLINE adjusted_mantissa compute_integer(uint64_t w) noexcept {
    adjusted_mantissa answer;
    // We want the most significant bit of i to be 1. Shift if needed.
    const int lz = clz(w);
    w <<= lz;

    // The required precision is binary::mantissa_explicit_bits() + 3 because
    // 1. We need the implicit bit
    // 2. We need an extra bit for rounding purposes
    // 3. We might lose a bit due to the "upperbit" routine (result too small,
    // requiring a shift)

    const uint128_t product =
        compute_product_approximation<binary::mantissa_explicit_bits() + 3>(0, w);
    // The "compute_product_approximation" function can be slightly slower than
    // a branchless approach: uint128_t product = compute_product(q, w); but in
    // practice, we can win big with the compute_product_approximation if its
    // additional branch is easily predicted. Which is best is data specific.
    const int upperbit = int(product.high >> 63);

    answer.mantissa = product.high >> (upperbit + 64 - binary::mantissa_explicit_bits() - 3);

    answer.power2 = int32_t(63 + upperbit - lz - binary::minimum_exponent());
    if (answer.power2 <= 0) { // we have a subnormal?
        // Here have that answer.power2 <= 0 so -answer.power2 >= 0
        if (-answer.power2 + 1 >= 64) { // if we have more than 64 bits below the minimum
                                        // exponent, you have a zero for sure.
            answer.power2 = 0;
            answer.mantissa = 0;
            // result should be zero
            return answer;
        }
        // next line is safe because -answer.power2 + 1 < 64
        answer.mantissa >>= -answer.power2 + 1;
        // Thankfully, we can't have both "round-to-even" and subnormals because
        // "round-to-even" only occurs for powers close to 0.
        answer.mantissa += (answer.mantissa & 1); // round up
        answer.mantissa >>= 1;
        // There is a weird scenario where we don't have a subnormal but just.
        // Suppose we start with 2.2250738585072013e-308, we end up
        // with 0x3fffffffffffff x 2^-1023-53 which is technically subnormal
        // whereas 0x40000000000000 x 2^-1023-53  is normal. Now, we need to
        // round up 0x3fffffffffffff x 2^-1023-53  and once we do, we are no
        // longer subnormal, but we can only know this after rounding. So we
        // only declare a subnormal if we are smaller than the threshold.
        answer.power2 =
            (answer.mantissa < (uint64_t(1) << binary::mantissa_explicit_bits())) ? 0 : 1;
        return answer;
    }

    // usually, we round *up*, but if we fall right in between and and we have
    // an even basis, we need to round down We are only concerned with the cases
    // where 5**q fits in single 64-bit word.
    if (product.low <= 1 && (answer.mantissa & 3) == 1) { // we may fall between two floats!
        // To be in-between two floats we need that in doing
        //   answer.mantissa = product.high >> (upperbit + 64 -
        //   binary::mantissa_explicit_bits() - 3);
        // ... we dropped out only zeroes. But if this happened, then we can go
        // back!!!
        if ((answer.mantissa << (upperbit + 64 - binary::mantissa_explicit_bits() - 3)) ==
            product.high) {
            answer.mantissa &= ~uint64_t(1); // flip it so that we do not round up
        }
    }

    answer.mantissa += (answer.mantissa & 1); // round up
    answer.mantissa >>= 1;
    if (answer.mantissa >= (uint64_t(2) << binary::mantissa_explicit_bits())) {
        answer.mantissa = (uint64_t(1) << binary::mantissa_explicit_bits());
        answer.power2++; // undo previous addition
    }

    answer.mantissa &= ~(uint64_t(1) << binary::mantissa_explicit_bits());
    if (answer.power2 >= binary::infinite_power()) { // infinity
        answer.power2 = binary::infinite_power();
        answer.mantissa = 0;
    }
    return answer;
}

// calculate the exponent, in scientific notation, of the number.
// this algorithm is not even close to optimized, but it has no practical
// effect on performance: in order to have a faster algorithm, we'd need
// to slow down performance for faster algorithms, and this is still fast.
WJR_INTRINSIC_INLINE int32_t scientific_exponent(int64_t exponent, uint64_t mantissa) noexcept {
    return int32_t(exponent) + count_digits<10>(mantissa) - 1;
}

template <typename T>
adjusted_mantissa digit_comp(adjusted_mantissa am, span<const char> integer,
                             span<const char> fraction, int32_t sci_exp) noexcept;

extern template adjusted_mantissa digit_comp<float>(adjusted_mantissa am, span<const char> integer,
                                                    span<const char> fraction,
                                                    int32_t sci_exp) noexcept;
extern template adjusted_mantissa digit_comp<double>(adjusted_mantissa am, span<const char> integer,
                                                     span<const char> fraction,
                                                     int32_t sci_exp) noexcept;

namespace detail {
/**
 * Special case +inf, -inf, nan, infinity, -infinity.
 * The case comparisons could be made much faster given that we know that the
 * strings a null-free and fixed.
 **/
template <typename T>
from_chars_result<> parse_infnan(const char *first, const char *last, T &value) noexcept {
    from_chars_result<> answer;
    answer.ptr = first;
    answer.ec = std::errc(); // be optimistic
    bool minusSign = false;
    if (*first == '-') { // assume first < last, so dereference without checks;
                         // C++17 20.19.3.(7.1) explicitly forbids '+' here
        minusSign = true;
        ++first;
    }
    if (last - first >= 3) {
        if (constant_length_strncaseequal(first, "nan", 3_u)) {
            answer.ptr = (first += 3);
            value = minusSign ? -std::numeric_limits<T>::quiet_NaN()
                              : std::numeric_limits<T>::quiet_NaN();
            // Check for possible nan(n-char-seq-opt), C++17 20.19.3.7,
            // C11 7.20.1.3.3. At least MSVC produces nan(ind) and nan(snan).
            if (first != last && *first == '(') {
                for (const char *ptr = first + 1; ptr != last; ++ptr) {
                    if (*ptr == ')') {
                        answer.ptr = ptr + 1; // valid nan(n-char-seq-opt)
                        break;
                    } else if (!(('a' <= *ptr && *ptr <= 'z') || ('A' <= *ptr && *ptr <= 'Z') ||
                                 ('0' <= *ptr && *ptr <= '9') || *ptr == '_'))
                        break; // forbidden char, not nan(n-char-seq-opt)
                }
            }
            return answer;
        }
        if (constant_length_strncaseequal(first, "inf", 3_u)) {
            if ((last - first >= 8) && constant_length_strncaseequal(first, "infinity", 8_u)) {
                answer.ptr = first + 8;
            } else {
                answer.ptr = first + 3;
            }
            value = minusSign ? -std::numeric_limits<T>::infinity()
                              : std::numeric_limits<T>::infinity();
            return answer;
        }
    }
    answer.ec = std::errc::invalid_argument;
    return answer;
}

/**
 * Returns true if the floating-pointing rounding mode is to 'nearest'.
 * It is the default on most system. This function is meant to be inexpensive.
 * Credit : @mwalcott3
 */
WJR_INTRINSIC_INLINE bool rounds_to_nearest() noexcept {
    // https://lemire.me/blog/2020/06/26/gcc-not-nearest/
#if (FLT_EVAL_METHOD != 1) && (FLT_EVAL_METHOD != 0)
    return false;
#endif
    // See
    // A fast function to check your floating-point rounding mode
    // https://lemire.me/blog/2022/11/16/a-fast-function-to-check-your-floating-point-rounding-mode/
    //
    // This function is meant to be equivalent to :
    // prior: #include <cfenv>
    //  return fegetround() == FE_TONEAREST;
    // However, it is expected to be much faster than the fegetround()
    // function call.
    //
    // The volatile keywoard prevents the compiler from computing the function
    // at compile-time.
    // There might be other ways to prevent compile-time optimizations (e.g.,
    // asm). The value does not need to be std::numeric_limits<float>::min(),
    // any small value so that 1 + x should round to 1 would do (after
    // accounting for excess precision, as in 387 instructions).
    static volatile float fmin = std::numeric_limits<float>::min();
    float fmini = fmin; // we copy it so that it gets loaded at most once.
//
// Explanation:
// Only when fegetround() == FE_TONEAREST do we have that
// fmin + 1.0f == 1.0f - fmin.
//
// FE_UPWARD:
//  fmin + 1.0f > 1
//  1.0f - fmin == 1
//
// FE_DOWNWARD or  FE_TOWARDZERO:
//  fmin + 1.0f == 1
//  1.0f - fmin < 1
//
// Note: This may fail to be accurate if fast-math has been
// enabled, as rounding conventions may not apply.
#ifdef WJR_COMPILER_MSVC
    #pragma warning(push)
//  todo: is there a VS warning?
//  see
//  https://stackoverflow.com/questions/46079446/is-there-a-warning-for-floating-point-equality-checking-in-visual-studio-2013
#elif defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
    return (fmini + 1.0f == 1.0f - fmini);
#ifdef WJR_COMPILER_MSVC
    #pragma warning(pop)
#elif defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
}

} // namespace detail

struct parsed_number_string {
    int64_t exponent{0};
    bool negative{false};
    // contains the range of the significant digits
    span<const char> integer{};  // non-nullable
    span<const char> fraction{}; // nullable
};

template <typename Writer, typename Op>
from_chars_result<> __from_chars_impl(const char *first, const char *last, Writer wr,
                                      Op options) noexcept {
    static_assert(!std::is_reference_v<Writer>);

    using T = typename Writer::float_type;
    constexpr bool is_support_integral = Writer::support_integral::value;
    constexpr bool is_constant_options = is_integral_constant_v<Op>;

    if (WJR_UNLIKELY(first == last)) {
        return {first, std::errc::invalid_argument};
    }

    const char *p = first;
    const auto fmt = static_cast<chars_format>(options);

    parsed_number_string pns;
    pns.negative = (*p == '-');

    if (*p == '-') { // C++17 20.19.3.(7.1) explicitly forbids '+' sign here
        if (++p == last) {
            return {first, std::errc::invalid_argument};
        }
    }

    const char *const start_digits = p;
    uint64_t uval = 0; // an unsigned int avoids signed overflows (which are bad)

    const char *end_of_integer_part;
    int64_t digit_count;
    int64_t exponent;
    int64_t exp_number; // explicit exponential part

    do {
        uint32_t ch = *p - '0';
        if (ch >= 10) { // This situation rarely occurs
            if constexpr (is_constant_options) {
                // digit_count is zero, this is an error in json.
                if (any(fmt & chars_format::__json_format)) {
                    return {first, std::errc::invalid_argument};
                }
            }

            break;
        }

        do {
            // a multiplication by 10 is cheaper than an arbitrary integer
            // multiplication
            uval = 10 * uval + ch; // might overflow, we will handle the overflow later

            if (++p == last) {
                goto INTEGER_AT_END;
            }

            ch = *p - '0';
        } while (WJR_LIKELY(ch < 10));
    } while (0);

    do {
        end_of_integer_part = p;
        digit_count = static_cast<int64_t>(p - start_digits);
        pns.integer = span<const char>(start_digits, static_cast<size_t>(digit_count));

        if constexpr (is_constant_options) {
            if (any(fmt & chars_format::__json_format)) {
                // at least 1 digit in integer part, without leading zeros
                if (digit_count == 0 || (start_digits[0] == '0' && digit_count > 1)) {
                    return {first, std::errc::invalid_argument};
                }
            }
        }

        if (*p != '.') {
            exponent = 0;
            if (*p == 'e' || *p == 'E') {
                break;
            }

            goto INTEGER;
        }

        ++p;
        const char *before = p;
        // can occur at most twice without overflowing, but let it occur more,
        // since for integers with many digits, digit parsing is the primary
        // bottleneck.
        if (last - p >= 8) {
            if (is_made_of_eight_digits_fast(p)) {
                do {
                    uval = uval * 100000000 + parse_eight_digits_unrolled(p);
                    p += 8;

                    if (last - p < 8) {
                        while ((p != last) && is_integer(*p)) {
                            uval = uval * 10 + uint32_t(*p - '0');
                            ++p;
                        }

                        goto POINT_DONE;
                    }

                } while (is_made_of_eight_digits_fast(p));
            }

            while (is_integer(*p)) {
                uval = uval * 10 + uint32_t(*p - '0');
                ++p;
            }
        } else {
            while ((p != last) && is_integer(*p)) {
                uval = uval * 10 + uint32_t(*p - '0');
                ++p;
            }
        }

    POINT_DONE:

        exponent = before - p;
        pns.fraction = span<const char>(before, size_t(p - before));
        digit_count -= exponent;

        auto &float_v = wr.get_float();
        if constexpr (is_constant_options) {
            if (any(fmt & chars_format::__json_format)) {
                if (WJR_UNLIKELY(exponent == 0)) {
                    return detail::parse_infnan(first, last, float_v);
                }
            } else {
                if (WJR_UNLIKELY(digit_count == 0)) {
                    return detail::parse_infnan(first, last, float_v);
                }
            }
        } else {
            if (WJR_UNLIKELY(digit_count == 0)) {
                return detail::parse_infnan(first, last, float_v);
            }
        }
    } while (0);

    exp_number = 0;

    if (any(fmt & chars_format::scientific) && (p != last) && (('e' == *p) || ('E' == *p))) {
        const char *location_of_e = p;
        ++p;
        bool neg_exp = false;
        if ((p != last) && ('-' == *p)) {
            neg_exp = true;
            ++p;
        } else if ((p != last) &&
                   ('+' == *p)) { // '+' on exponent is allowed by C++17 20.19.3.(7.1)
            ++p;
        }
        if ((p == last) || !is_integer(*p)) {
            if (!(fmt & chars_format::fixed)) {
                // We are in error.
                return detail::parse_infnan(first, last, wr.get_float());
            }
            // Otherwise, we will be ignoring the 'e'.
            p = location_of_e;
        } else {
            while ((p != last) && is_integer(*p)) {
                const auto digit = uint32_t(*p - '0');
                if (exp_number < 0x10000000) {
                    exp_number = 10 * exp_number + digit;
                }
                ++p;
            }
            if (neg_exp) {
                exp_number = -exp_number;
            }
            exponent += exp_number;
        }
    } else {
        // If it scientific and not fixed, we have to bail out.
        if (any(fmt & chars_format::scientific) && !(fmt & chars_format::fixed)) {
            return detail::parse_infnan(first, last, wr.get_float());
        }
    }

    do {
        from_chars_result<> answer;
        answer.ptr = p;
        answer.ec = std::errc(); // be optimistic
        bool too_many_digits = false;

        // If we frequently had to deal with long strings of digits,
        // we could extend our code by using a 128-bit integer instead
        // of a 64-bit integer. However, this is uncommon.
        //
        // We can deal with up to 19 digits.
        if (digit_count > 19) { // this is uncommon
            // It is possible that the integer had an overflow.
            // We have to handle the case where we have 0.0000somenumber.
            // We need to be mindful of the case where we only have zeroes...
            // E.g., 0.000000000...000.
            const char *start = start_digits;
            while ((start != last) && (*start == '0' || *start == '.')) {
                if (*start == '0') {
                    digit_count--;
                }
                start++;
            }

            if (digit_count > 19) {
                too_many_digits = true;
                // Let us start again, this time, avoiding overflows.
                // We don't need to check if is_integer, since we use the
                // pre-tokenized spans from above.
                uval = 0;
                p = pns.integer.data();
                const char *int_end = p + pns.integer.size();
                constexpr uint64_t minimal_nineteen_digit_integer = 1000000000000000000ull;
                while ((uval < minimal_nineteen_digit_integer) && (p != int_end)) {
                    uval = uval * 10 + uint64_t(*p - '0');
                    ++p;
                }
                if (uval >= minimal_nineteen_digit_integer) { // We have a big integers
                    exponent = end_of_integer_part - p + exp_number;
                } else { // We have a value with a fractional component.
                    p = pns.fraction.data();
                    const char *frac_end = p + pns.fraction.size();
                    while ((uval < minimal_nineteen_digit_integer) && (p != frac_end)) {
                        uval = uval * 10 + uint64_t(*p - '0');
                        ++p;
                    }
                    exponent = pns.fraction.data() - p + exp_number;
                }
                // We have now corrected both exponent and uval, to a truncated
                // value
            }
        }

        pns.exponent = exponent;

        T &float_v = wr.get_float();

        // The implementation of the Clinger's fast path is convoluted because
        // we want round-to-nearest in all cases, irrespective of the rounding
        // mode selected on the thread. We proceed optimistically, assuming that
        // detail::rounds_to_nearest() returns true.
        if (binary_format<T>::min_exponent_fast_path() <= pns.exponent &&
            pns.exponent <= binary_format<T>::max_exponent_fast_path() && !too_many_digits) {
            // Unfortunately, the conventional Clinger's fast path is only
            // possible when the system rounds to the nearest float.
            //
            // We expect the next branch to almost always be selected.
            // We could check it first (before the previous branch), but
            // there might be performance advantages at having the check
            // be last.
            if (detail::rounds_to_nearest()) {
                // We have that fegetround() == FE_TONEAREST.
                // Next is Clinger's fast path.
                if (uval <= binary_format<T>::max_mantissa_fast_path()) {
                    float_v = T(uval);
                    if (pns.exponent < 0) {
                        float_v = float_v / binary_format<T>::exact_power_of_ten(-pns.exponent);
                    } else {
                        float_v = float_v * binary_format<T>::exact_power_of_ten(pns.exponent);
                    }
                    if (pns.negative) {
                        float_v = -float_v;
                    }
                    return answer;
                }
            } else {
                // We do not have that fegetround() == FE_TONEAREST.
                // Next is a modified Clinger's fast path, inspired by Jakub
                // Jelínek's proposal
                if (pns.exponent >= 0 &&
                    uval <= binary_format<T>::max_mantissa_fast_path(pns.exponent)) {
#if defined(__clang__)
                    // ClangCL may map 0 to -0.0 when fegetround() ==
                    // FE_DOWNWARD
                    if (uval == 0) {
                        float_v = pns.negative ? T(-0.) : T(0.);
                        return answer;
                    }
#endif
                    float_v = T(uval) * binary_format<T>::exact_power_of_ten(pns.exponent);
                    if (pns.negative) {
                        float_v = -float_v;
                    }
                    return answer;
                }
            }
        }

        adjusted_mantissa am = compute_float<binary_format<T>>(pns.exponent, uval);
        if (too_many_digits && am.power2 >= 0) {
            if (am != compute_float<binary_format<T>>(pns.exponent, uval + 1)) {
                am = compute_error<binary_format<T>>(pns.exponent, uval);
            }
        }

        // If we called compute_float<binary_format<T>>(pns.exponent, uval)
        // and we have an invalid power (am.power2 < 0), then we need to go the
        // long way around again. This is very uncommon.
        if (am.power2 < 0) {
            am.power2 -= invalid_am_bias;

            const int32_t sci_exp = scientific_exponent(pns.exponent, uval);
            am = digit_comp<T>(am, pns.integer, pns.fraction, sci_exp);
        }

        to_float(pns.negative, am, float_v);
        // Test for over/underflow.
        if ((uval != 0 && am.mantissa == 0 && am.power2 == 0) ||
            am.power2 == binary_format<T>::infinite_power()) {
            answer.ec = std::errc::result_out_of_range;
        }

        return answer;
    } while (0);

INTEGER_AT_END:
    end_of_integer_part = p;
    digit_count = static_cast<int64_t>(p - start_digits);
    pns.integer = span<const char>(start_digits, static_cast<size_t>(digit_count));

    if constexpr (is_constant_options) {
        if (any(fmt & chars_format::__json_format)) {
            // at least 1 digit in integer part, without leading zeros
            if (digit_count == 0 || (start_digits[0] == '0' && digit_count > 1)) {
                return {first, std::errc::invalid_argument};
            }
        }
    }

INTEGER:
    from_chars_result<> answer;
    answer.ptr = p;
    answer.ec = std::errc(); // be optimistic

    // If we frequently had to deal with long strings of digits,
    // we could extend our code by using a 128-bit integer instead
    // of a 64-bit integer. However, this is uncommon.
    //
    // We can deal with up to 19 digits.
    // todo : Maybe need to make this branch unlikely.
    if (digit_count > 19) { // this is uncommon
        // It is possible that the integer had an overflow.
        // We have to handle the case where we have 0.0000somenumber.
        // We need to be mindful of the case where we only have zeroes...
        // E.g., 0000000000...000.
        const char *start = start_digits;
        while ((start != last) && *start == '0') {
            --digit_count;
            start++;
        }

        if (digit_count > 19) {
            p = start;

            uval = __from_chars_unroll_16<10>(reinterpret_cast<const uint8_t *>(p), char_converter);
            p += 16;
            uval = uval * 10 + char_converter.template from<10>(*p++);
            uval = uval * 10 + char_converter.template from<10>(*p++);
            uval = uval * 10 + char_converter.template from<10>(*p++);

            exponent = end_of_integer_part - p;
            pns.exponent = exponent;

            WJR_ASSUME(exponent >= 0);

            if constexpr (is_support_integral) {
                constexpr uint64_t max_quot = std::numeric_limits<uint64_t>::max() / 10;
                constexpr uint32_t max_rem = std::numeric_limits<uint64_t>::max() % 10;

                if (!pns.negative && digit_count == 20 &&
                    (uval < max_quot ||
                     (uval == max_quot && static_cast<uint32_t>(*p - '0') <= max_rem))) {
                    uint64_t &u64_v = wr.get_u64();
                    u64_v = uval;
                    return answer;
                }
            }

            T &float_v = wr.get_float();

            adjusted_mantissa am = compute_float<binary_format<T>>(pns.exponent, uval);
            if (am.power2 >= 0) {
                if (am != compute_float<binary_format<T>>(pns.exponent, uval + 1)) {
                    am = compute_error<binary_format<T>>(pns.exponent, uval);
                }
            }

            // If we called compute_float<binary_format<T>>(pns.exponent,
            // uval) and we have an invalid power (am.power2 < 0), then we
            // need to go the long way around again. This is very uncommon.
            if (am.power2 < 0) {
                am.power2 -= invalid_am_bias;

                const int32_t sci_exp = scientific_exponent(pns.exponent, uval);
                am = digit_comp<T>(am, pns.integer, pns.fraction, sci_exp);
            }

            to_float(pns.negative, am, float_v);
            // Test for over/underflow.
            if ((uval != 0 && am.mantissa == 0 && am.power2 == 0) ||
                am.power2 == binary_format<T>::infinite_power()) {
                answer.ec = std::errc::result_out_of_range;
            }

            return answer;
        }
    }

    if constexpr (is_support_integral) {
        constexpr auto fast_int64_maxn =
            static_cast<uint64_t>(0) - static_cast<uint64_t>(std::numeric_limits<int64_t>::min());

        if (!pns.negative) {
            uint64_t &u64_v = wr.get_u64();
            u64_v = uval;
            return answer;
        } else if (uval <= fast_int64_maxn) {
            int64_t &i64_v = wr.get_i64();
            i64_v = static_cast<int64_t>(-uval);
            return answer;
        }

        static_assert(fast_int64_maxn >= binary_format<T>::max_mantissa_fast_path());
    }

    auto &float_v = wr.get_float();

    if constexpr (!is_support_integral) {
        if (WJR_LIKELY(uval <= binary_format<T>::max_mantissa_fast_path())) {
#if defined(__clang__)
            // ClangCL may map 0 to -0.0 when fegetround() == FE_DOWNWARD
            if (uval == 0) {
                float_v = pns.negative ? T(-0.) : T(0.);
                return answer;
            }
#endif

            float_v = T(uval);
            if (pns.negative) {
                float_v = -float_v;
            }

            return answer;
        }
    }

    // Can be optimized when is_support_integral is true.
    // In that case, 2^63 <= uval < 2^64.
    adjusted_mantissa am = compute_integer<binary_format<T>>(uval);
    WJR_ASSERT_ASSUME(am.power2 >= 0);

    to_float(pns.negative, am, float_v);
    // Test for over/underflow.
    if ((uval != 0 && am.mantissa == 0 && am.power2 == 0) ||
        am.power2 == binary_format<T>::infinite_power()) {
        answer.ec = std::errc::result_out_of_range;
    }

    return answer;
}

} // namespace wjr::fastfloat

namespace wjr {
using fastfloat::from_chars;
}

#endif // WJR_FORMAT_FASTFLOAT_HPP__