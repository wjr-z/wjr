#include <wjr/json/parser.hpp>
#include <wjr/math/ctz.hpp>

namespace wjr::json ::parser_detail {

template <typename simd>
inline typename simd::int_type equal(typename simd::int_type stk, uint8_t ch) noexcept {
    return simd::cmpeq_epi8(stk, simd::set1_epi8(ch));
}

WJR_INTRINSIC_INLINE char *copy_small(char *dst, const char *src, size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0)) {
        return dst;
    }

#if WJR_HAS_SIMD(AVX2)
    if (n >= 16) {
        std::memcpy(dst, src, 16);
        std::memcpy(dst + n - 16, src + n - 16, 16);
        return dst + n;
    }
#endif

    if (n >= 8) {
        std::memcpy(dst, src, 8);
        std::memcpy(dst + n - 8, src + n - 8, 8);
        return dst + n;
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

#if WJR_HAS_BUILTIN(JSON_CHECK_STRING)

result<void> check_string(const char *first, const char *last) noexcept {
    constexpr auto is_avx = WJR_HAS_SIMD(AVX2);
    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_mask_type = typename simd::mask_type;

    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;

    if (first + u8_width <= last) {
        do {
            const auto x = simd::loadu(first);
            auto B =
                static_cast<simd_mask_type>(simd::movemask_epi8(equal<simd>(x, '\\')));

            if (WJR_UNLIKELY(B != 0)) {
                do {
                    const int pos = ctz(B);

                    // last backslash, special handling
                    if (WJR_UNLIKELY(pos == u8_width - 1)) {
                        WJR_ASSERT(first + u8_width + 1 <= last);

                        const uint8_t ch = first[u8_width];

                        if (ch == 'u') {
                            WJR_EXPECTED_SET(first, check_unicode_codepoint(
                                                        first + u8_width + 1, last));
                            first -= u8_width;
                        } else {
                            const uint8_t code = escape_table[ch];

                            if (WJR_UNLIKELY(code == 0)) {
                                return unexpected(error_code::STRING_ERROR);
                            }

                            first += 1;
                        }

                        break;
                    }

                    const uint8_t ch = first[pos + 1];

                    if (ch == 'u') {
                        const char *__first;
                        WJR_EXPECTED_SET(__first,
                                         check_unicode_codepoint(first + pos + 2, last));

                        if (to_unsigned(__first - first) >= u8_width) {
                            first = __first - u8_width;
                            break;
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
            }

            first += u8_width;
        } while (first + u8_width <= last);
    }

    const auto n = last - first;
    const auto floor = (n >> 1) << 1;
    simd_mask_type B;

#define WJR_REGISTER_PARSE_STRING_CASE(IDX, SUFFIX)                                      \
    case IDX: {                                                                          \
        const auto x = simd::loadu_si##SUFFIX(first);                                    \
        B = static_cast<simd_mask_type>(simd::movemask_epi8(equal<simd>(x, '\\')));      \
        if (WJR_LIKELY(B == 0)) {                                                        \
            goto LAST;                                                                   \
        }                                                                                \
        break;                                                                           \
    }

    switch (floor) {
    case 0: {
        if (WJR_UNLIKELY(first == last)) {
            return {};
        }

        const uint8_t ch = *first++;
        if (WJR_UNLIKELY(ch == '\\')) {
            return unexpected(error_code::STRING_ERROR);
        }

        return {};
    }
        WJR_REGISTER_PARSE_STRING_CASE(2, 16)
        WJR_REGISTER_PARSE_STRING_CASE(4, 32)
        WJR_REGISTER_PARSE_STRING_CASE(6, 48)
        WJR_REGISTER_PARSE_STRING_CASE(8, 64)
        WJR_REGISTER_PARSE_STRING_CASE(10, 80)
        WJR_REGISTER_PARSE_STRING_CASE(12, 96)
        WJR_REGISTER_PARSE_STRING_CASE(14, 112)
#if WJR_HAS_SIMD(AVX2)
        WJR_REGISTER_PARSE_STRING_CASE(16, 128)
        WJR_REGISTER_PARSE_STRING_CASE(18, 144)
        WJR_REGISTER_PARSE_STRING_CASE(20, 160)
        WJR_REGISTER_PARSE_STRING_CASE(22, 176)
        WJR_REGISTER_PARSE_STRING_CASE(24, 192)
        WJR_REGISTER_PARSE_STRING_CASE(26, 208)
        WJR_REGISTER_PARSE_STRING_CASE(28, 224)
        WJR_REGISTER_PARSE_STRING_CASE(30, 240)
#endif
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

#undef WJR_REGISTER_PARSE_STRING_CASE

    do {
        do {
            const int pos = ctz(B);

            if (WJR_UNLIKELY(pos == floor - 1)) {
                WJR_ASSERT(floor != n);

                const uint8_t ch = first[floor];

                if (ch == 'u') {
                    return unexpected(error_code::STRING_ERROR);
                }

                const uint8_t code = escape_table[ch];

                if (WJR_UNLIKELY(code == 0)) {
                    return unexpected(error_code::STRING_ERROR);
                }

                return {};
            }

            const uint8_t ch = first[pos + 1];

            if (ch == 'u') {
                const char *__first = first;
                WJR_EXPECTED_SET(__first, check_unicode_codepoint(first + pos + 2, last));

                if (__first == last) {
                    return {};
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
    } while (0);

LAST:
    if (floor != n) {
        WJR_ASSERT(first[floor] != '\\');
    }

    return {};
}

#endif

#if WJR_HAS_BUILTIN(JSON_PARSE_STRING)

result<char *> parse_string(char *dst, const char *first, const char *last) noexcept {
    constexpr auto is_avx = WJR_HAS_SIMD(AVX2);
    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_mask_type = typename simd::mask_type;

    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;

    if (first + u8_width <= last) {
        do {
            const auto x = simd::loadu(first);
            auto B =
                static_cast<simd_mask_type>(simd::movemask_epi8(equal<simd>(x, '\\')));

            if (WJR_UNLIKELY(B != 0)) {
                int last_pos = 0;

                do {
                    const int pos = ctz(B);

                    dst = copy_small(dst, first + last_pos, pos - last_pos);

                    // last backslash, special handling
                    if (WJR_UNLIKELY(pos == u8_width - 1)) {
                        WJR_ASSERT(first + u8_width + 1 <= last);

                        const uint8_t ch = first[u8_width];

                        if (ch == 'u') {
                            WJR_EXPECTED_SET(first, parse_unicode_codepoint(
                                                        dst, first + u8_width + 1, last));
                            first -= u8_width;
                        } else {
                            const uint8_t code = escape_table[ch];

                            if (WJR_UNLIKELY(code == 0)) {
                                return unexpected(error_code::STRING_ERROR);
                            }

                            *dst++ = code;
                            first += 1;
                        }

                        break;
                    }

                    const uint8_t ch = first[pos + 1];

                    if (ch == 'u') {
                        const char *__first;
                        WJR_EXPECTED_SET(
                            __first, parse_unicode_codepoint(dst, first + pos + 2, last));

                        if (to_unsigned(__first - first) >= u8_width) {
                            first = __first - u8_width;
                            break;
                        }

                        last_pos = pos + 6;
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
            } else {
                simd::storeu(dst, x);
                dst += u8_width;
            }

            first += u8_width;
        } while (first + u8_width <= last);
    }

    const auto n = last - first;
    const auto floor = (n >> 1) << 1;
    simd_mask_type B;

#define WJR_REGISTER_PARSE_STRING_CASE(IDX, SUFFIX)                                      \
    case IDX: {                                                                          \
        const auto x = simd::loadu_si##SUFFIX(first);                                    \
        B = static_cast<simd_mask_type>(simd::movemask_epi8(equal<simd>(x, '\\')));      \
        if (WJR_LIKELY(B == 0)) {                                                        \
            std::memcpy(dst, first, IDX);                                                \
            dst += IDX;                                                                  \
            goto LAST;                                                                   \
        }                                                                                \
        break;                                                                           \
    }

    switch (floor) {
    case 0: {
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
        WJR_REGISTER_PARSE_STRING_CASE(2, 16)
        WJR_REGISTER_PARSE_STRING_CASE(4, 32)
        WJR_REGISTER_PARSE_STRING_CASE(6, 48)
        WJR_REGISTER_PARSE_STRING_CASE(8, 64)
        WJR_REGISTER_PARSE_STRING_CASE(10, 80)
        WJR_REGISTER_PARSE_STRING_CASE(12, 96)
        WJR_REGISTER_PARSE_STRING_CASE(14, 112)
#if WJR_HAS_SIMD(AVX2)
        WJR_REGISTER_PARSE_STRING_CASE(16, 128)
        WJR_REGISTER_PARSE_STRING_CASE(18, 144)
        WJR_REGISTER_PARSE_STRING_CASE(20, 160)
        WJR_REGISTER_PARSE_STRING_CASE(22, 176)
        WJR_REGISTER_PARSE_STRING_CASE(24, 192)
        WJR_REGISTER_PARSE_STRING_CASE(26, 208)
        WJR_REGISTER_PARSE_STRING_CASE(28, 224)
        WJR_REGISTER_PARSE_STRING_CASE(30, 240)
#endif
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

#undef WJR_REGISTER_PARSE_STRING_CASE

    do {
        int last_pos = 0;

        do {
            const int pos = ctz(B);

            copy_small(dst, first + last_pos, pos - last_pos);

            if (WJR_UNLIKELY(pos == floor - 1)) {
                WJR_ASSERT(floor != n);

                const uint8_t ch = first[floor];

                if (ch == 'u') {
                    return unexpected(error_code::STRING_ERROR);
                }

                const uint8_t code = escape_table[ch];

                if (WJR_UNLIKELY(code == 0)) {
                    return unexpected(error_code::STRING_ERROR);
                }

                *dst++ = code;
                return dst;
            }

            const uint8_t ch = first[pos + 1];

            if (ch == 'u') {
                const char *__first = first;
                WJR_EXPECTED_SET(__first,
                                 parse_unicode_codepoint(dst, first + pos + 2, last));

                if (__first == last) {
                    return dst;
                }

                last_pos = pos + 6;
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
    } while (0);

LAST:
    if (floor != n) {
        WJR_ASSERT(first[floor] != '\\');
        *dst++ = first[floor];
    }

    return dst;
}

#endif

} // namespace wjr::json::parser_detail
