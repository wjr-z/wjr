#ifndef WJR_JSON_NUMBER_HPP__
#define WJR_JSON_NUMBER_HPP__

#include <wjr/format/fastfloat.hpp>
#include <wjr/json/detail.hpp>

namespace wjr::json::number_detail {
struct number_writer {
    using float_type = double;
    using support_integral = std::true_type;

    number_writer() = delete;
    number_writer(const number_writer &) = default;
    number_writer(number_writer &&) = default;
    number_writer &operator=(const number_writer &) = delete;
    number_writer &operator=(number_writer &&) = delete;
    ~number_writer() = default;

    WJR_INTRINSIC_CONSTEXPR number_writer(basic_value &_value) noexcept : value(_value) {}

    WJR_INTRINSIC_CONSTEXPR uint64_t &get_u64() noexcept {
        value.m_type = value_t::number_unsigned;
        return value.m_number_unsigned;
    }

    WJR_INTRINSIC_CONSTEXPR int64_t &get_i64() noexcept {
        value.m_type = value_t::number_signed;
        return value.m_number_signed;
    }

    WJR_INTRINSIC_CONSTEXPR double &get_float() noexcept {
        value.m_type = value_t::number_float;
        return value.m_number_float;
    }

    basic_value &value;
};
} // namespace wjr::json::number_detail

namespace wjr::fastfloat {
extern template from_chars_result<>
_from_chars_impl<json::number_detail::number_writer,
                 integral_constant<chars_format, chars_format::json>>(
    const char *first, const char *last, json::number_detail::number_writer wr,
    integral_constant<chars_format, chars_format::json> options) noexcept;
}

namespace wjr::json {

namespace number_detail {

WJR_INTRINSIC_INLINE from_chars_result<> from_chars_json(const char *first, const char *last,
                                                         basic_value &value) noexcept {
    return fastfloat::_from_chars_impl(first, last, number_writer(value),
                                       integral_constant<chars_format, chars_format::json>());
}

} // namespace number_detail

/// @private Number parsing implementation
namespace detail {
WJR_PURE WJR_INTRINSIC_INLINE result<void> parse_number(const char *first, const char *last,
                                                        basic_value &value) noexcept {
    const auto ret = number_detail::from_chars_json(first, last, value);
    if (WJR_LIKELY(ret)) {
        if (WJR_UNLIKELY(ret.ptr != last && !charconv_detail::isspace(*ret.ptr))) {
            return unexpected(error_code::TAPE_ERROR);
        }

        return {};
    }

    error_code err = ret.ec == std::errc::result_out_of_range ? error_code::BIGINT_ERROR
                                                              : error_code::TAPE_ERROR;
    return unexpected(err);
}

/**
 * @todo This can be optimize.
 */
WJR_PURE WJR_INTRINSIC_INLINE result<void> check_number(const char *first,
                                                        const char *last) noexcept {
    basic_value value(default_construct);
    return parse_number(first, last, value);
}

} // namespace detail

} // namespace wjr::json

#endif // WJR_JSON_NUMBER_HPP__