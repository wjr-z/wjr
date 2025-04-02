#include <wjr/biginteger.hpp>

namespace wjr {

namespace biginteger_detail {

template void
__addsub_impl<false, default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                                 const biginteger_data *lhs,
                                                 const biginteger_data *rhs) noexcept;

template void
__addsub_impl<true, default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                                const biginteger_data *lhs,
                                                const biginteger_data *rhs) noexcept;

template void
__mul_impl<default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                       const biginteger_data *lhs,
                                       const biginteger_data *rhs) noexcept;

template void
__sqr_impl<default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                       const biginteger_data *src) noexcept;

template <bool Check>
from_chars_result<const char *> __from_chars_impl(const char *first, const char *last,
                                                  biginteger_dispatcher dst,
                                                  unsigned int base) noexcept {
    const auto *__first = first;

    do {
        if (WJR_UNLIKELY(first == last)) {
            break;
        }

        uint8_t ch;
        ch = *first;

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
                const auto __try_match = [base](uint8_t &__ch) {
                    __ch -= '0';
                    return __ch < base;
                };

                if (WJR_UNLIKELY(!__try_match(ch))) {
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

                    if (!__try_match(ch)) {
                        dst.set_ssize(0);
                        return {first, std::errc{}};
                    }
                }

                __first = first;

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
                    if (__try_match(ch)) {
                        do {
                            ++first;
                            ch = *first;
                        } while (__try_match(ch));
                    }
                }
            } else {
                const auto __try_match = [base](uint8_t &__ch) {
                    __ch = char_converter.from(__ch);
                    return __ch < base;
                };

                if (WJR_UNLIKELY(!__try_match(ch))) {
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

                    if (!__try_match(ch)) {
                        dst.clear();
                        return {first, std::errc{}};
                    }
                }

                __first = first;

                do {
                    ++first;
                    if (first == last) {
                        break;
                    }

                    ch = *first;
                } while (__try_match(ch));
            }
#if WJR_DEBUG_LEVEL < 3
        } else {
            __first = first;
            first = last;
        }
#endif

        const uint32_t str_size = static_cast<uint32_t>(first - __first);
        uint32_t capacity;

        switch (base) {
        case 2: {
            capacity = __ceil_div(str_size, 64);
            break;
        }
        case 8: {
            capacity = __ceil_div(str_size * 3, 64);
            break;
        }
        case 16: {
            capacity = __ceil_div(str_size, 16);
            break;
        }
        case 4:
        case 32: {
            const int bits = base == 4 ? 2 : 5;
            capacity = __ceil_div(str_size * bits, 64);
            break;
        }
        case 10: {
            // capacity = (str_size * log2(10) + 63) / 64;
            capacity = __ceil_div(str_size * 10 / 3, 64);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        dst.clear_if_reserved(capacity);
        auto *const ptr = dst.data();
        int32_t dssize = biginteger_from_chars(__first, first, ptr, base) - ptr;
        dssize = __fast_conditional_negate<int32_t>(sign, dssize);
        dst.set_ssize(dssize);
        return {first, std::errc{}};
    } while (0);

    dst.clear();
    return {__first, std::errc::invalid_argument};
}

template from_chars_result<const char *> __from_chars_impl<false>(const char *first,
                                                                  const char *last,
                                                                  biginteger_dispatcher dst,
                                                                  unsigned int base) noexcept;

template from_chars_result<const char *> __from_chars_impl<true>(const char *first,
                                                                 const char *last,
                                                                 biginteger_dispatcher dst,
                                                                 unsigned int base) noexcept;

void __addsubmul_impl(biginteger_dispatcher dst, const biginteger_data *lhs, uint64_t rhs,
                      int32_t xmask) noexcept {
    const int32_t lssize = lhs->get_ssize();

    if (lssize == 0 || rhs == 0) {
        return;
    }

    const uint32_t lusize = __fast_abs(lssize);
    int32_t dssize = dst.get_ssize();

    if (dssize == 0) {
        dst.reserve(lusize + 1);
        const auto dp = dst.data();
        const auto cf = mul_1(dp, lhs->data(), lusize, rhs);
        dssize = lssize + (cf != 0);
        if (cf != 0) {
            dp[lusize] = cf;
        }

        dst.set_ssize(__fast_negate_with(xmask, dssize));
        return;
    }

    xmask ^= lssize;
    xmask ^= dssize;

    const uint32_t dusize = __fast_abs(dssize);

    uint32_t new_dusize = std::max(lusize, dusize);
    const uint32_t min_size = std::min(lusize, dusize);
    dst.reserve(new_dusize + 1);

    auto *dp = dst.data();
    const auto *lp = lhs->data();

    uint64_t cf;

    // dst += abs(lhs) * abs(rhs)
    if (xmask >= 0) {
        cf = addmul_1(dp, lp, min_size, rhs);

        dp += min_size;
        lp += min_size;

        int32_t sdelta = lusize - dusize;

        if (sdelta != 0) {
            uint64_t cf2;
            if (sdelta > 0) {
                cf2 = mul_1(dp, lp, sdelta, rhs);
            } else {
                sdelta = -sdelta;
                cf2 = 0;
            }

            cf = cf2 + addc_1(dp, dp, sdelta, cf);
        }

        dp[sdelta] = cf;
        new_dusize += (cf != 0);
    }
    // dst -= abs(lhs) * abs(rhs)
    else {
        cf = submul_1(dp, lp, min_size, rhs);

        do {
            if (dusize >= lusize) {
                if (dusize != lusize) {
                    cf = subc_1(dp + lusize, dp + lusize, dusize - lusize, cf);
                }

                if (cf != 0) {
                    cf += math::bi_negate_n(dp, dp, new_dusize) - 1;
                    dp[new_dusize] = cf;
                    ++new_dusize;
                    dssize = __fast_negate(dssize);
                }
            } else {
                cf += math::bi_negate_n(dp, dp, dusize) - 1;

                const auto cf2 = cf == UINT64_MAX;
                cf += cf2;

                const auto cf3 = mul_1(dp + dusize, lp + dusize, lusize - dusize, rhs);
                cf = cf3 + addc_1(dp + dusize, dp + dusize, lusize - dusize, cf);

                dp[new_dusize] = cf;
                new_dusize += (cf != 0);

                if (cf2) {
                    (void)subc_1(dp + dusize, dp + dusize, new_dusize - dusize, 1);
                }

                dssize = __fast_negate(dssize);
            }

            new_dusize = normalize(dp, new_dusize);
        } while (0);
    }

    dst.set_ssize(__fast_conditional_negate<int32_t>(dssize < 0, new_dusize));
}

void __addsubmul_impl(biginteger_dispatcher dst, const biginteger_data *lhs,
                      const biginteger_data *rhs, int32_t xmask) noexcept {
    int32_t lssize = lhs->get_ssize();
    int32_t rssize = rhs->get_ssize();

    if (lssize == 0 || rssize == 0) {
        return;
    }

    uint32_t lusize = __fast_abs(lssize);
    uint32_t rusize = __fast_abs(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lssize, rssize);
        std::swap(lusize, rusize);
    }

    xmask ^= rssize;

    if (rusize == 1) {
        __addsubmul_impl(dst, lhs, rhs->data()[0], xmask);
        return;
    }

    xmask ^= lssize;

    int32_t dssize = dst.get_ssize();
    xmask ^= dssize;
    uint32_t dusize = __fast_abs(dssize);

    uint32_t tusize = lusize + rusize;
    dst.reserve(std::max(tusize, dusize) + 1);
    auto *const dp = dst.data();

    if (dssize == 0) {
        mul_s(dp, lhs->data(), lusize, rhs->data(), rusize);
        tusize -= dp[tusize - 1] == 0;
        dst.set_ssize(__fast_conditional_negate<int32_t>(xmask < 0, tusize));
        return;
    }

    unique_stack_allocator stkal;
    auto *tp = stkal.template allocate<uint64_t>(tusize);

    mul_s(tp, lhs->data(), lusize, rhs->data(), rusize);
    tusize -= tp[tusize - 1] == 0;

    auto *up = dp;
    uint32_t uusize = dusize;

    if (xmask >= 0) {
        if (uusize < tusize) {
            up = tp;
            uusize = tusize;
            tp = dp;
            tusize = dusize;

            dusize = uusize;
        }

        const auto cf = addc_s(dp, up, uusize, tp, tusize);
        dp[uusize] = cf;
        dusize = uusize + (cf != 0);
    } else {
        if (uusize < tusize || (uusize == tusize && reverse_compare_n(up, tp, uusize) < 0)) {
            up = tp;
            uusize = tusize;
            tp = dp;
            tusize = dusize;

            dusize = uusize;

            dssize = __fast_negate(dssize);
        }

        (void)subc_s(dp, up, uusize, tp, tusize);
        dssize = normalize(dp, dusize);
    }

    dst.set_ssize(__fast_conditional_negate<int32_t>(dssize < 0, dusize));
}

} // namespace biginteger_detail

std::ostream &operator<<(std::ostream &os, const biginteger_data &src) noexcept {
    if (const std::ostream::sentry ok(os); ok) {
        unique_stack_allocator stkal;
        vector<char, weak_stack_allocator<char>> buffer(stkal);
        buffer.clear_if_reserved(256);

        const std::ios_base::fmtflags flags = os.flags();

        if ((flags & std::ios::showpos) && !src.is_negate()) {
            buffer.push_back('+');
        }

        int base = 10;

        if (const auto basefield = flags & std::ios::basefield; basefield != 0) {
            if (basefield == std::ios::oct) {
                base = 8;
                if (flags & std::ios::showbase) {
                    buffer.append('0');
                }
            } else if (basefield == std::ios::hex) {
                base = 16;
                if (flags & std::ios::showbase) {
                    buffer.append({'0', 'x'});
                }
            }
        }

        (void)to_chars_unchecked(std::back_inserter(buffer), src, base);
        // seems won't be empty
        WJR_ASSERT(!buffer.empty());
        __ostream_insert_unchecked(os, buffer.data(), buffer.size());
    } else {
        os.setstate(std::ios::badbit);
    }

    return os;
}

} // namespace wjr

namespace fmt {

void formatter<wjr::biginteger_data>::do_format(
    const wjr::biginteger_data &value,
    wjr::vector<char, wjr::weak_stack_allocator<char>> &buffer) const {
    buffer.clear_if_reserved(256);

    if (m_specs.sign() != sign::none && !value.is_negate()) {
        buffer.push_back('+');
    }

    int base = 10;
    switch (m_specs.type()) {
    case presentation_type::bin:
        base = 2;
        if (m_specs.alt())
            buffer.append({'0', m_specs.upper() ? 'B' : 'b'});
        break;
    case presentation_type::oct:
        base = 8;
        if (m_specs.alt())
            buffer.append('0');
        break;
    case presentation_type::hex:
        base = 16;
        if (m_specs.alt())
            buffer.append({'0', m_specs.upper() ? 'X' : 'x'});
        break;
    case presentation_type::dec:
        WJR_FALLTHROUGH;
    case presentation_type::none:
        break;
    default:
        WJR_UNREACHABLE();
    }

    (void)to_chars_unchecked(std::back_inserter(buffer), value, base);
}
} // namespace fmt