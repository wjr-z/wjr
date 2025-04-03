#include <wjr/biginteger.hpp>

namespace wjr {

namespace biginteger_detail {

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

void __addsub_impl(biginteger_dispatcher dst, const biginteger_data *lhs,
                   const biginteger_data *rhs, bool xsign) noexcept {
    auto lssize = lhs->get_ssize();
    int32_t rssize = __fast_conditional_negate<int32_t>(xsign, rhs->get_ssize());
    uint32_t lusize = __fast_abs(lssize);
    uint32_t rusize = __fast_abs(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lssize, rssize);
        std::swap(lusize, rusize);
    }

    dst.reserve(lusize + 1);

    auto *const dp = dst.data();
    const auto *const lp = lhs->data();
    const auto *const rp = rhs->data();
    int32_t dssize;

    if (rusize == 0) {
        if (lp != dp) {
            std::copy_n(lp, lusize, dp);
            dst.set_ssize(lssize);
        }

        return;
    }

    // different sign
    if ((lssize ^ rssize) < 0) {
        const auto ans = static_cast<int32_t>(abs_subc_s_pos(dp, lp, lusize, rp, rusize));
        dssize = __fast_negate_with<int32_t>(lssize, ans);
    } else {
        const auto cf = addc_s(dp, lp, lusize, rp, rusize);
        dssize = __fast_negate_with<int32_t>(lssize, lusize + cf);
        if (cf) {
            dp[lusize] = 1;
        }
    }

    dst.set_ssize(dssize);
}

void __mul_impl(biginteger_dispatcher dst, const biginteger_data *lhs,
                const biginteger_data *rhs) noexcept {
    int32_t lssize = lhs->get_ssize();
    int32_t rssize = rhs->get_ssize();
    uint32_t lusize = __fast_abs(lssize);
    uint32_t rusize = __fast_abs(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lusize, rusize);
    }

    if (WJR_UNLIKELY(rusize == 0)) {
        dst.set_ssize(0);
        return;
    }

    const int32_t mask = lssize ^ rssize;

    int32_t dssize;
    uint32_t dusize;

    if (rusize == 1) {
        dst.reserve(lusize + 1);
        const auto cf = mul_1(dst.data(), lhs->data(), lusize, rhs->data()[0]);
        dssize = __fast_negate_with<int32_t>(mask, lusize + (cf != 0));
        if (cf != 0) {
            dst.data()[lusize] = cf;
        }
        dst.set_ssize(dssize);
        return;
    }

    dusize = lusize + rusize;

    using pointer = uint64_t *;

    auto *dp = dst.data();
    auto *lp = const_cast<pointer>(lhs->data());
    auto *rp = const_cast<pointer>(rhs->data());

    unique_stack_allocator stkal;
    biginteger_dispatcher tmp(enable_default_constructor);

    if (dst.capacity() < dusize) {
        tmp = dst.construct_reserve(dusize, &stkal);
        dp = tmp.data();
    } else {
        if (dp == lp) {
            lp = stkal.template allocate<uint64_t>(lusize);
            if (dp == rp) {
                rp = lp;
            }
            copy_n_restrict(dp, lusize, lp);
        } else if (dp == rp) {
            rp = stkal.template allocate<uint64_t>(rusize);
            copy_n_restrict(dp, rusize, rp);
        }
    }

    if (WJR_UNLIKELY(lp == rp)) {
        sqr(dp, lp, lusize);
    } else {
        mul_s(dp, lp, lusize, rp, rusize);
    }

    const bool cf = dp[dusize - 1] == 0;
    dssize = __fast_negate_with<int32_t>(mask, dusize - cf);

    if (tmp.has_value()) {
        dst.move_assign(tmp.raw());
        tmp.destroy();
    }

    dst.set_ssize(dssize);
}

void __sqr_impl(biginteger_dispatcher dst, const biginteger_data *src) noexcept {
    int32_t sssize = src->get_ssize();
    uint32_t susize = __fast_abs(sssize);

    if (WJR_UNLIKELY(susize == 0)) {
        dst.set_ssize(0);
        return;
    }

    int32_t dssize;
    uint32_t dusize;

    if (susize == 1) {
        dst.reserve(susize + 1);
        const uint64_t num = src->data()[0];
        uint64_t cf;
        dst.data()[0] = mul(num, num, cf);
        dssize = 1 + (cf != 0);
        if (cf != 0) {
            dst.data()[1] = cf;
        }
        dst.set_ssize(dssize);
        return;
    }

    dusize = susize * 2;

    using pointer = uint64_t *;

    auto *dp = dst.data();
    auto *sp = const_cast<pointer>(src->data());

    unique_stack_allocator stkal;
    biginteger_dispatcher tmp(enable_default_constructor);

    if (dst.capacity() < dusize) {
        tmp = dst.construct_reserve(dusize, &stkal);
        dp = tmp.data();
    } else {
        if (dp == sp) {
            sp = stkal.template allocate<uint64_t>(susize);
            copy_n_restrict(dp, susize, sp);
        }
    }

    sqr(dp, sp, susize);

    const bool cf = dp[dusize - 1] == 0;
    dssize = dusize - cf;

    if (tmp.has_value()) {
        dst.move_assign(tmp.raw());
        tmp.destroy();
    }

    dst.set_ssize(dssize);
}

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

void __tdiv_qr_impl(biginteger_dispatcher quot, biginteger_dispatcher rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept {
    WJR_ASSERT_ASSUME(!__equal_pointer(quot.raw(), rem.raw()),
                      "quot should not be the same as rem");

    const auto nssize = num->get_ssize();
    const auto dssize = div->get_ssize();
    const auto nusize = __fast_abs(nssize);
    auto dusize = __fast_abs(dssize);
    int32_t qssize = nusize - dusize + 1;

    WJR_ASSERT(dusize != 0, "division by zero");

    rem.reserve(dusize);
    auto *rp = rem.data();

    // num < div
    if (qssize <= 0) {
        auto np = num->data();
        if (np != rp) {
            std::copy_n(np, nusize, rp);
            rem.set_ssize(nssize);
        }

        quot.set_ssize(0);
        return;
    }

    using pointer = uint64_t *;

    quot.reserve(qssize);
    auto *qp = quot.data();

    auto *np = const_cast<pointer>(num->data());
    auto *dp = const_cast<pointer>(div->data());

    unique_stack_allocator stkal;

    if (dp == rp || dp == qp) {
        auto *tp = stkal.template allocate<uint64_t>(dusize);
        copy_n_restrict(dp, dusize, tp);
        dp = tp;
    }

    if (np == rp || np == qp) {
        auto *tp = stkal.template allocate<uint64_t>(nusize);
        copy_n_restrict(np, nusize, tp);
        np = tp;
    }

    div_qr_s(qp, rp, np, nusize, dp, dusize);

    qssize -= qp[qssize - 1] == 0;
    dusize = normalize(rp, dusize);

    quot.set_ssize(__fast_conditional_negate<int32_t>((nssize ^ dssize) < 0, qssize));
    rem.set_ssize(__fast_conditional_negate<int32_t>(nssize < 0, dusize));
}

void __cfdiv_r_2exp_impl(biginteger_dispatcher rem, const biginteger_data *num, uint32_t shift,
                         int32_t xdir) noexcept {
    int32_t nssize = num->get_ssize();

    if (nssize == 0) {
        rem.set_ssize(0);
        return;
    }

    uint32_t nusize = __fast_abs(nssize);
    uint32_t offset = shift / 64;
    shift %= 64;

    uint64_t *rp;
    auto *np = const_cast<uint64_t *>(num->data());

    if ((nssize ^ xdir) < 0) {
        if (__equal_pointer(rem.raw(), num)) {
            if (nusize <= offset) {
                return;
            }

            rp = np;
        } else {
            const auto size = std::min<uint32_t>(nusize, offset + 1);
            rem.reserve(size);
            rp = rem.data();
            std::copy_n(np, size, rp);

            if (nusize <= offset) {
                rem.set_ssize(nssize);
                return;
            }
        }
    } else {
        do {
            if (nusize <= offset) {
                break;
            }

            if (find_not_n(np, 0, offset) != offset) {
                break;
            }

            if ((np[offset] & (((uint64_t)(1) << shift) - 1)) != 0) {
                break;
            }

            rem.set_ssize(0);
            return;
        } while (0);

        rem.reserve(offset + 1);
        rp = rem.data();
        np = const_cast<uint64_t *>(num->data());

        const auto size = std::min<uint32_t>(nusize, offset + 1);
        (void)math::bi_negate_n(rp, np, size);
        for (uint32_t i = size; i <= offset; ++i) {
            rp[i] = UINT64_MAX;
        }

        nssize = -nssize;
    }

    uint64_t hi = rp[offset] & (((uint64_t)(1) << shift) - 1);
    rp[offset] = hi;

    if (hi == 0) {
        offset = normalize(rp, offset);
    } else {
        ++offset;
    }

    rem.set_ssize(__fast_conditional_negate<int32_t>(nssize < 0, offset));
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