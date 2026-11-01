#include <wjr/biginteger.hpp>

namespace wjr {

namespace biginteger_detail {

template <bool Check>
from_chars_result<const char *> _probe_old_from_chars_impl(const char *first, const char *last,
                                                           biginteger_dispatcher dst,
                                                           unsigned int base) noexcept {
    const auto *_first = first;

    do {
        if (WJR_UNLIKELY(first == last)) {
            break;
        }

        uint8_t ch = *first;

        if constexpr (Check) {
            first = skip_whitespace(first, last);
        } else {
            WJR_ASSERT(!charconv_detail::isspace(ch));
        }

        int sign = 0;
        if (ch == '-') {
            sign = 1;
            if (++first == last) {
                break;
            }

            ch = *first;
        }

        if constexpr (Check) {
            if (base == 0) {
                base = 10;
                if (ch == '0') {
                    base = 8;
                    if (++first == last) {
                        break;
                    }

                    ch = *first;
                    if (ch == 'x' || ch == 'X') {
                        base = 16;
                        if (++first == last) {
                            break;
                        }

                        ch = *first;
                    } else {
                        if (ch == 'b' || ch == 'B') {
                            base = 2;
                            if (++first == last) {
                                break;
                            }

                            ch = *first;
                        }
                    }
                }
            }
        } else {
            WJR_ASSERT(base != 0);
        }

#if WJR_DEBUG_LEVEL < 3
        if constexpr (Check) {
#endif
            if (base <= 10) {
                const auto _try_match = [base](uint8_t &_ch) {
                    _ch -= '0';
                    return _ch < base;
                };

                if (WJR_UNLIKELY(!_try_match(ch))) {
                    break;
                }

                if (WJR_UNLIKELY(ch == 0)) {
                    goto LOOP_HEAD_0;

                    do {
                        ch = *first;
                        if (ch != '0') {
                            goto LOOP_END_0;
                        }

                    LOOP_HEAD_0:
                        ++first;
                    } while (first != last);

                    dst.set_ssize(0);
                    return {first, std::errc{}};
                LOOP_END_0:

                    if (!_try_match(ch)) {
                        dst.set_ssize(0);
                        return {first, std::errc{}};
                    }
                }

                _first = first;

                if (++first != last) {
                    if (last - first >= 8) {
                        do {
                            if (!check_eight_digits(first, base)) {
                                break;
                            }

                            first += 8;
                        } while (last - first >= 8);
                    }

                    ch = *first;
                    if (_try_match(ch)) {
                        do {
                            ++first;
                            ch = *first;
                        } while (_try_match(ch));
                    }
                }
            } else {
                const auto _try_match = [base](uint8_t &_ch) {
                    _ch = char_converter.from(_ch);
                    return _ch < base;
                };

                if (WJR_UNLIKELY(!_try_match(ch))) {
                    break;
                }

                if (WJR_UNLIKELY(ch == 0)) {
                    goto LOOP_HEAD_1;

                    do {
                        ch = *first;
                        if (ch != '0') {
                            goto LOOP_END_1;
                        }

                    LOOP_HEAD_1:
                        ++first;
                    } while (first != last);

                    dst.clear();
                    return {first, std::errc{}};
                LOOP_END_1:

                    if (!_try_match(ch)) {
                        dst.clear();
                        return {first, std::errc{}};
                    }
                }

                _first = first;

                do {
                    ++first;
                    if (first == last) {
                        break;
                    }

                    ch = *first;
                } while (_try_match(ch));
            }
#if WJR_DEBUG_LEVEL < 3
        } else {
            _first = first;
            first = last;
        }
#endif

        const uint32_t str_size = static_cast<uint32_t>(first - _first);
        uint32_t capacity;

        switch (base) {
        case 2: {
            capacity = _ceil_div(str_size, 64);
            break;
        }
        case 8: {
            capacity = _ceil_div(str_size * 3, 64);
            break;
        }
        case 16: {
            capacity = _ceil_div(str_size, 16);
            break;
        }
        case 4:
        case 32: {
            const int bits = base == 4 ? 2 : 5;
            capacity = _ceil_div(str_size * bits, 64);
            break;
        }
        case 10: {
            // capacity = (str_size * log2(10) + 63) / 64;
            capacity = _ceil_div(str_size * 10 / 3, 64);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        dst.clear_if_reserved(capacity);
        auto *const ptr = dst.data();
        int32_t dssize = biginteger_from_chars(_first, first, ptr, base) - ptr;
        dssize = _fast_conditional_negate<int32_t>(sign, dssize);
        dst.set_ssize(dssize);
        return {first, std::errc{}};
    } while (0);

    dst.clear();
    return {_first, std::errc::invalid_argument};
}

} // namespace biginteger_detail
} // namespace wjr
