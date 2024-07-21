#include <wjr/json/parser.hpp>
#include <wjr/math/ctz.hpp>

#if WJR_HAS_BUILTIN(JSON_PARSE_STRING)

namespace wjr::json {

namespace parser_detail {

template <typename simd>
inline typename simd::int_type equal(typename simd::int_type stk, uint8_t ch) noexcept {
    return simd::cmpeq_epi8(stk, simd::set1_epi8(ch));
}

WJR_INTRINSIC_INLINE void copy2x(char *dst, const char *src, size_t n) noexcept {
#if WJR_HAS_SIMD(AVX2)
    if (n >= 16) {
        std::memcpy(dst, src, 16);
        std::memcpy(dst + n - 16, src + n - 16, 16);
        return;
    }
#endif

    if (n >= 8) {
        std::memcpy(dst, src, 8);
        std::memcpy(dst + n - 8, src + n - 8, 8);
        return;
    }

    std::memcpy(dst, src, 4);
    std::memcpy(dst + n - 4, src + n - 4, 4);
}

result<char *> parse_string(char *dst, const char *first, const char *last) noexcept {
    constexpr auto is_avx = WJR_HAS_SIMD(AVX2);
    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_int = typename simd::int_type;
    using simd_mask_type = typename simd::mask_type;

    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;
    constexpr auto simd_hi_mask = static_cast<simd_mask_type>(1) << (u8_width - 1);

    if (first + u8_width <= last) {
        do {
            simd_int x = simd::loadu(first);
            auto B =
                static_cast<simd_mask_type>(simd::movemask_epi8(equal<simd>(x, '\\')));

            if (WJR_LIKELY(B == 0)) {
                simd::storeu(dst, x);
                dst += u8_width;
            } else {
                simd_mask_type last_backslash = B & simd_hi_mask;

                B &= simd_hi_mask - 1;
                int last_pos = 0;

                if (B) {
                    do {
                        const int pos = ctz(B);

                        for (; last_pos != pos; ++last_pos) {
                            *dst++ = first[last_pos];
                        }

                        const uint8_t ch = first[pos + 1];

                        if (ch == 'u') {
                            int offset = 0;
                            if (WJR_UNLIKELY(pos > u8_width - 6)) {
                                offset = pos - (u8_width - 6);
                                if (first + u8_width + offset > last) {
                                    return unexpected(error_code::STRING_ERROR);
                                }
                            }

                            dst = parse_unicode_codepoint(dst, first + pos + 2);

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
                                last_backslash &= B - 1;
                            }

                            last_pos = pos + 2;
                        }

                        B &= B - 1;
                    } while (B);
                }

                for (; last_pos != u8_width - (last_backslash != 0); ++last_pos) {
                    *dst++ = first[last_pos];
                }

                // last charactor is backslash
                if (last_backslash) {
                    WJR_ASSERT(first + u8_width + 1 <= last);

                    const uint8_t ch = first[u8_width];

                    if (ch == 'u') {
                        if (WJR_UNLIKELY(first + u8_width + 5 > last)) {
                            return unexpected(error_code::STRING_ERROR);
                        }

                        dst = parse_unicode_codepoint(dst, first + u8_width + 1);
                        first += 5;
                    } else {
                        const uint8_t code = escape_table[ch];

                        if (WJR_UNLIKELY(code == 0)) {
                            return unexpected(error_code::STRING_ERROR);
                        }

                        *dst++ = code;
                        first += 1;
                    }
                }
            }

            first += u8_width;
        } while (first + u8_width <= last);
    }

    const auto n = last - first;

    if (n <= 4) {
        return generic_parse_string(dst, first, last);
    }

    const auto floor = (n / 2) * 2;

    simd_int x = simd::loadu_si16x(first, n / 2);
    auto B = static_cast<simd_mask_type>(simd::movemask_epi8(equal<simd>(x, '\\')));

    const auto __mask = static_cast<simd_mask_type>(1) << (floor - 1);

    if (WJR_LIKELY(B == 0)) {
        copy2x(dst, first, floor);
        dst += floor;
    } else {
        simd_mask_type last_backslash = B & __mask;

        B &= __mask - 1;
        int last_pos = 0;

        if (B) {
            do {
                const int pos = ctz(B);

                for (; last_pos != pos; ++last_pos) {
                    *dst++ = first[last_pos];
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

                    dst = parse_unicode_codepoint(dst, first + pos + 2);

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
                        last_backslash &= B - 1;
                    }

                    last_pos = pos + 2;
                }

                B &= B - 1;
            } while (B);
        }

        for (; last_pos != floor - (last_backslash != 0); ++last_pos) {
            *dst++ = first[last_pos];
        }

        // last charactor is backslash
        if (last_backslash) {
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
    }

    if (floor != n) {
        WJR_ASSERT(first[floor] != '\\');
        *dst++ = first[floor];
    }

    return dst;
}

} // namespace parser_detail

} // namespace wjr::json

#endif