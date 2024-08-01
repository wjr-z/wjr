#ifndef WJR_JSON_NUMBER_HPP__
#define WJR_JSON_NUMBER_HPP__

#include <wjr/format/charconv.hpp>
#include <wjr/json/detail.hpp>

namespace wjr::json {

namespace number_detail {
struct __floating_point_struct {
    int32_t exponent;
    uint32_t mantissa_count;
    uint8_t mantissa[768];
};
} // namespace number_detail

namespace detail {

WJR_PURE WJR_INTRINSIC_INLINE result<void> check_number(const char *first,
                                                        const char *last) noexcept {
    WJR_ASSERT_ASSUME(first != last);
    return {};
}

WJR_PURE WJR_INTRINSIC_INLINE basic_value parse_number(const char *first,
                                                       const char *last) noexcept {
    using namespace number_detail;

    WJR_ASSERT_ASSUME(first != last);

    bool negative;

    if (WJR_UNLIKELY(*first == '-')) {
        negative = true;

        if (WJR_UNLIKELY(++first == last)) {
            return basic_value(null_v);
        }

    } else {
        negative = false;
    }

    if (*first == '0') {
        if (++first == last) {
            return basic_value(number_unsigned_v, 0);
        }

        if (WJR_UNLIKELY(*first == '0')) {
            return basic_value(null_v);
        }
    }

    __floating_point_struct fp;

    uint8_t *const mantissa_first = fp.mantissa;
    uint8_t *const mantissa_last = std::end(fp.mantissa);
    uint8_t *mantissa_iter = mantissa_first;

    const auto *const integer_first = first;
    bool has_zero_tail = true;

    do {
        uint8_t ch = *first - '0';

        if (ch >= 10) {
            break;
        }

        if (WJR_UNLIKELY(mantissa_iter == mantissa_last)) {
            goto HEAD0;

            do {
                ch = *first - '0';

                if (ch >= 10) {
                    break;
                }

            HEAD0:
                has_zero_tail = has_zero_tail && ch == 0;
            } while (++first != last);

            goto END0;
        }

        *mantissa_iter++ = ch;
    } while (++first != last);

END0:

    const auto *const integer_last = first;
    ptrdiff_t exponent_adjustment = integer_last - integer_first;

    if (WJR_UNLIKELY(first == last)) {
        const auto n = integer_last - integer_first;

        if (WJR_LIKELY(n <= 19 - negative)) {
            uint64_t uvalue;
            __unsigned_from_chars_unchecked<10>(
                reinterpret_cast<const uint8_t *>(mantissa_first),
                reinterpret_cast<const uint8_t *>(mantissa_iter), uvalue,
                origin_converter);

            if (!negative) {
                return basic_value(number_unsigned_v, uvalue);
            }

            return basic_value(number_signed_v, static_cast<int64_t>(-uvalue));
        }
    }
}

} // namespace detail

} // namespace wjr::json

#endif // WJR_JSON_NUMBER_HPP__