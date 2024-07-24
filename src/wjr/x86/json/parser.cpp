#include <wjr/json/parser.hpp>
#include <wjr/math/ctz.hpp>

#if WJR_HAS_BUILTIN(JSON_PARSE_STRING)

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

            if (WJR_LIKELY(B == 0)) {
                simd::storeu(dst, x);
                dst += u8_width;
            } else {
                int last_pos = 0;

                do {
                    const int pos = ctz(B);

                    dst = copy_small(dst, first + last_pos, pos - last_pos);
                    last_pos = pos;

                    // last backslash, special handling
                    if (WJR_UNLIKELY(pos == u8_width - 1)) {
                        WJR_ASSERT(first + u8_width + 1 <= last);

                        const uint8_t ch = first[u8_width];

                        if (ch == 'u') {
                            if (WJR_UNLIKELY(first + u8_width + 5 > last)) {
                                return unexpected(error_code::STRING_ERROR);
                            }

                            WJR_EXPECTED_SET(
                                dst, parse_unicode_codepoint(dst, first + u8_width + 1));

                            first += 5;
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
                        int offset = 0;
                        if (WJR_UNLIKELY(to_unsigned(pos) > u8_width - 6)) {
                            offset = pos - (u8_width - 6);
                            if (first + u8_width + offset > last) {
                                return unexpected(error_code::STRING_ERROR);
                            }
                        }

                        WJR_EXPECTED_SET(dst,
                                         parse_unicode_codepoint(dst, first + pos + 2));

                        last_pos = pos + 6;
                        first += offset;
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

        uint8_t ch = *first++;
        if (WJR_LIKELY(ch != '\\')) {
            *dst++ = ch;
        } else {
            return unexpected(error_code::STRING_ERROR);
        }

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

    do {
        int last_pos = 0;

        do {
            const int pos = ctz(B);

            copy_small(dst, first + last_pos, pos - last_pos);
            last_pos = pos;

            if (WJR_UNLIKELY(pos == floor - 1)) {
                WJR_ASSERT(floor != n);

                const uint8_t ch = first[floor];

                if (ch == 'u') {
                    return unexpected(error_code::STRING_ERROR);
                } else {
                    const uint8_t code = escape_table[ch];

                    if (WJR_UNLIKELY(code == 0)) {
                        return unexpected(error_code::STRING_ERROR);
                    }

                    *dst++ = code;
                }

                return dst;
            }

            const uint8_t ch = first[pos + 1];

            if (ch == 'u') {
                int offset = 0;
                if (WJR_UNLIKELY(pos > floor - 6)) {
                    offset = pos - (floor - 6);
                    if (floor + offset > n) {
                        return unexpected(error_code::STRING_ERROR);
                    }
                }

                WJR_EXPECTED_SET(dst, parse_unicode_codepoint(dst, first + pos + 2));

                last_pos = pos + 6;
                first += offset;
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

} // namespace wjr::json::parser_detail

#endif