#include <wjr/json/string.hpp>
#include <wjr/math/ctz.hpp>

namespace wjr::json {

namespace {
#if WJR_HAS_BUILTIN(JSON_PARSE_STRING)
WJR_INTRINSIC_INLINE char *small_copy(char *dst, const char *src,
                                      unsigned int n) noexcept {
    if (n >= 8) {
#if WJR_HAS_SIMD(AVX2)
        if (n >= 16) {
            std::memcpy(dst, src, 16);
            std::memcpy(dst + n - 16, src + n - 16, 16);
            return dst + n;
        }
#endif
        std::memcpy(dst, src, 8);
        std::memcpy(dst + n - 8, src + n - 8, 8);
        return dst + n;
    }

    if (WJR_UNLIKELY(n == 0)) {
        return dst;
    }

    if (n >= 4) {
        std::memcpy(dst, src, 4);
        std::memcpy(dst + n - 4, src + n - 4, 4);
        return dst + n;
    }

    if (n >= 2) {
        std::memcpy(dst, src, 2);
        std::memcpy(dst + n - 2, src + n - 2, 2);
        return dst + n;
    }

    *dst = *src;
    return dst + 1;
}
}

namespace detail {

result<char *> parse_string(char *dst, const char *first, const char *last) noexcept {
    using namespace string_detail;

    constexpr auto is_avx = WJR_HAS_SIMD(AVX2);
    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_mask_type = typename simd::mask_type;

    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;

    if (first + u8_width <= last) {
    LOOP:
        const auto x = simd::loadu(first);
        auto B = static_cast<simd_mask_type>(
            simd::movemask_epi8(simd::cmpeq_epi8(x, simd::set1_epi8('\\'))));

        if (WJR_UNLIKELY(B != 0)) {
            int last_pos = 0;

            do {
                const int pos = ctz(B);
                dst = small_copy(dst, first + last_pos, pos - last_pos);

                // last backslash, special handling
                if (WJR_UNLIKELY(pos == u8_width - 1)) {
                    WJR_ASSERT(first + u8_width + 1 <= last);

                    const uint8_t ch = first[u8_width];

                    if (WJR_UNLIKELY(ch == 'u')) {
                        WJR_EXPECTED_SET(first, parse_unicode_codepoint(
                                                    dst, first + u8_width + 1, last));

                        if (first + u8_width <= last) {
                            goto LOOP;
                        }

                        goto SMALL;
                    }

                    const uint8_t code = escape_table[ch];

                    if (WJR_UNLIKELY(code == 0)) {
                        return unexpected(error_code::STRING_ERROR);
                    }

                    *dst++ = code;
                    first += u8_width + 1;

                    if (first + u8_width <= last) {
                        goto LOOP;
                    }

                    goto SMALL;
                }

                const uint8_t ch = first[pos + 1];

                if (WJR_UNLIKELY(ch == 'u')) {
                    const char *__first;
                    WJR_EXPECTED_SET(__first,
                                     parse_unicode_codepoint(dst, first + pos + 2, last));

                    if (to_unsigned(__first - first) >= u8_width) {
                        first = __first;
                        if (first + u8_width <= last) {
                            goto LOOP;
                        }

                        goto SMALL;
                    }

                    last_pos = __first - first;
                    // two backslash
                    if (last_pos - pos != 6) {
                        B &= B - 1;
                    }
                } else {
                    const uint8_t code = escape_table[ch];

                    if (WJR_UNLIKELY(code == 0)) {
                        return unexpected(error_code::STRING_ERROR);
                    }

                    *dst++ = code;

                    // backslash
                    if (WJR_UNLIKELY(code == 0x5c)) {
                        B &= B - 1;
                    }

                    last_pos = pos + 2;
                }

                B &= B - 1;
            } while (B);

            dst = small_copy(dst, first + last_pos, u8_width - last_pos);
            first += u8_width;

            if (first + u8_width <= last) {
                goto LOOP;
            }

            goto SMALL;
        }

        simd::storeu(dst, x);
        dst += u8_width;
        first += u8_width;

        if (first + u8_width <= last) {
            goto LOOP;
        }
    }

SMALL:

    const auto n = last - first;
    simd_mask_type B;

    do {
#if WJR_HAS_SIMD(AVX2)
        if (WJR_UNLIKELY(n > 16)) {
            const auto m = n - 16;
            const auto x0 = sse::loadu(first);
            const auto x1 = sse::loadu(first + m);
            B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
                sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

            if (WJR_LIKELY(B == 0)) {
                sse::storeu(dst, x0);
                sse::storeu(dst + m, x1);
                return dst + n;
            }

            break;
        }
#endif

        if (WJR_UNLIKELY(n <= 8)) {
            if (WJR_UNLIKELY(n <= 4)) {
                if (WJR_UNLIKELY(n < 2)) {
                    if (WJR_UNLIKELY(first == last)) {
                        return dst;
                    }

                    const uint8_t ch = *first++;
                    if (WJR_UNLIKELY(ch == '\\')) {
                        return unexpected(error_code::STRING_ERROR);
                    }

                    *dst++ = ch;
                    return dst;
                }

                const auto m = n - 2;
                const auto x0 = sse::loadu_si16(first);
                const auto x1 = sse::loadu_si16(first + m);
                B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
                    sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

                if (WJR_LIKELY(B == 0)) {
                    std::memcpy(dst, first, 2);
                    std::memcpy(dst + m, first + m, 2);
                    return dst + n;
                }

                break;
            }

            const auto m = n - 4;
            const auto x0 = sse::loadu_si32(first);
            const auto x1 = sse::loadu_si32(first + m);
            B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
                sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

            if (WJR_LIKELY(B == 0)) {
                std::memcpy(dst, first, 4);
                std::memcpy(dst + m, first + m, 4);
                return dst + n;
            }

            break;
        }

        const auto m = n - 8;
        const auto x0 = sse::loadu_si64(first);
        const auto x1 = sse::loadu_si64(first + m);
        B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
            sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

        if (WJR_LIKELY(B == 0)) {
            std::memcpy(dst, first, 8);
            std::memcpy(dst + m, first + m, 8);
            return dst + n;
        }
    } while (false);

    int last_pos = 0;

    do {
        const int pos = ctz(B);
        dst = small_copy(dst, first + last_pos, pos - last_pos);

        if (WJR_UNLIKELY(pos == n - 1)) {
            return unexpected(error_code::STRING_ERROR);
        }

        const uint8_t ch = first[pos + 1];

        if (WJR_UNLIKELY(ch == 'u')) {
            const char *__first = first;
            WJR_EXPECTED_SET(__first,
                             parse_unicode_codepoint(dst, first + pos + 2, last));

            if (__first == last) {
                return dst;
            }

            last_pos = __first - first;
            // two backslash
            if (last_pos - pos != 6) {
                B &= B - 1;
            }
        } else {
            const uint8_t code = escape_table[ch];

            if (WJR_UNLIKELY(code == 0)) {
                return unexpected(error_code::STRING_ERROR);
            }

            *dst++ = code;

            // backslash
            if (WJR_UNLIKELY(code == 0x5c)) {
                B &= B - 1;
            }

            last_pos = pos + 2;
        }

        B &= B - 1;
    } while (B);

    dst = small_copy(dst, first + last_pos, n - last_pos);
    return dst;
}

} // namespace detail
#endif

#if WJR_HAS_BUILTIN(JSON_CHECK_STRING)

namespace detail {

result<void> check_string(const char *first, const char *last) noexcept {
    using namespace string_detail;

    constexpr auto is_avx = WJR_HAS_SIMD(AVX2);
    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_mask_type = typename simd::mask_type;

    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;

    if (first + u8_width <= last) {
    LOOP:
        const auto x = simd::loadu(first);
        auto B = static_cast<simd_mask_type>(
            simd::movemask_epi8(simd::cmpeq_epi8(x, simd::set1_epi8('\\'))));

        if (WJR_UNLIKELY(B != 0)) {
            do {
                const int pos = ctz(B);

                // last backslash, special handling
                if (WJR_UNLIKELY(pos == u8_width - 1)) {
                    WJR_ASSERT(first + u8_width + 1 <= last);

                    const uint8_t ch = first[u8_width];

                    if (WJR_UNLIKELY(ch == 'u')) {
                        WJR_EXPECTED_SET(
                            first, check_unicode_codepoint(first + u8_width + 1, last));

                        if (first + u8_width <= last) {
                            goto LOOP;
                        }

                        goto SMALL;
                    }

                    const uint8_t code = escape_table[ch];

                    if (WJR_UNLIKELY(code == 0)) {
                        return unexpected(error_code::STRING_ERROR);
                    }

                    first += u8_width + 1;

                    if (first + u8_width <= last) {
                        goto LOOP;
                    }

                    goto SMALL;
                }

                const uint8_t ch = first[pos + 1];

                if (WJR_UNLIKELY(ch == 'u')) {
                    const char *__first;
                    WJR_EXPECTED_SET(__first,
                                     check_unicode_codepoint(first + pos + 2, last));

                    if (to_unsigned(__first - first) >= u8_width) {
                        first = __first;
                        if (first + u8_width <= last) {
                            goto LOOP;
                        }

                        goto SMALL;
                    }

                    // two backslash
                    if ((__first - first) - pos != 6) {
                        B &= B - 1;
                    }
                } else {
                    const uint8_t code = escape_table[ch];

                    if (WJR_UNLIKELY(code == 0)) {
                        return unexpected(error_code::STRING_ERROR);
                    }

                    // backslash
                    if (WJR_UNLIKELY(code == 0x5c)) {
                        B &= B - 1;
                    }
                }

                B &= B - 1;
            } while (B);

            first += u8_width;

            if (first + u8_width <= last) {
                goto LOOP;
            }

            goto SMALL;
        }

        first += u8_width;

        if (first + u8_width <= last) {
            goto LOOP;
        }
    }

SMALL:

    const auto n = last - first;
    simd_mask_type B;

    do {
#if WJR_HAS_SIMD(AVX2)
        if (WJR_UNLIKELY(n > 16)) {
            const auto m = n - 16;
            const auto x0 = sse::loadu(first);
            const auto x1 = sse::loadu(first + m);
            B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
                sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

            if (WJR_LIKELY(B == 0)) {
                return {};
            }

            break;
        }
#endif

        if (WJR_UNLIKELY(n <= 8)) {
            if (WJR_UNLIKELY(n <= 4)) {
                if (WJR_UNLIKELY(n < 2)) {
                    if (WJR_UNLIKELY(first == last)) {
                        return {};
                    }

                    const uint8_t ch = *first++;
                    if (WJR_UNLIKELY(ch == '\\')) {
                        return unexpected(error_code::STRING_ERROR);
                    }

                    return {};
                }

                const auto m = n - 2;
                const auto x0 = sse::loadu_si16(first);
                const auto x1 = sse::loadu_si16(first + m);
                B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
                    sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

                if (WJR_LIKELY(B == 0)) {
                    return {};
                }

                break;
            }

            const auto m = n - 4;
            const auto x0 = sse::loadu_si32(first);
            const auto x1 = sse::loadu_si32(first + m);
            B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
                sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

            if (WJR_LIKELY(B == 0)) {
                return {};
            }

            break;
        }

        const auto m = n - 8;
        const auto x0 = sse::loadu_si64(first);
        const auto x1 = sse::loadu_si64(first + m);
        B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
            sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

        if (WJR_LIKELY(B == 0)) {
            return {};
        }
    } while (false);

    do {
        const int pos = ctz(B);

        if (WJR_UNLIKELY(pos == n - 1)) {
            return unexpected(error_code::STRING_ERROR);
        }

        const uint8_t ch = first[pos + 1];

        if (WJR_UNLIKELY(ch == 'u')) {
            const char *__first = first;
            WJR_EXPECTED_SET(__first, check_unicode_codepoint(first + pos + 2, last));

            if (__first == last) {
                return {};
            }

            // two backslash
            if ((__first - first) - pos != 6) {
                B &= B - 1;
            }
        } else {
            const uint8_t code = escape_table[ch];

            if (WJR_UNLIKELY(code == 0)) {
                return unexpected(error_code::STRING_ERROR);
            }

            // backslash
            if (WJR_UNLIKELY(code == 0x5c)) {
                B &= B - 1;
            }
        }

        B &= B - 1;
    } while (B);

    return {};
}

} // namespace detail

#endif

} // namespace wjr::json
