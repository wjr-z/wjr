
#include <wjr/arch/x86/simd/simd.hpp>
#include <wjr/format/utf8/utf8.hpp>
#include <wjr/math/ctz.hpp>

namespace wjr::utf8 {

#if WJR_HAS_BUILTIN(UTF8_CHECK_UNICODE)
optional<void> builtin_check_unicode(const char *first, const char *last) noexcept {
    using namespace detail;
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
                        WJR_OPTIONAL_SET(first,
                                         check_unicode_codepoint(first + u8_width + 1, last));

                        if (first + u8_width <= last) {
                            goto LOOP;
                        }

                        goto SMALL;
                    }

                    const uint8_t code = escape_table[ch];

                    if (WJR_UNLIKELY(code == 0)) {
                        return nullopt;
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
                    WJR_OPTIONAL_SET(__first, check_unicode_codepoint(first + pos + 2, last));

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
                        return nullopt;
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
                return voidopt;
            }

            break;
        }
    #endif

        if (WJR_UNLIKELY(n <= 8)) {
            if (WJR_UNLIKELY(n <= 4)) {
                if (WJR_UNLIKELY(n < 2)) {
                    if (WJR_UNLIKELY(first == last)) {
                        return voidopt;
                    }

                    const uint8_t ch = *first++;
                    if (WJR_UNLIKELY(ch == '\\')) {
                        return nullopt;
                    }

                    return voidopt;
                }

                const auto m = n - 2;
                const auto x0 = sse::loadu_si16(first);
                const auto x1 = sse::loadu_si16(first + m);
                B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
                    sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

                if (WJR_LIKELY(B == 0)) {
                    return voidopt;
                }

                break;
            }

            const auto m = n - 4;
            const auto x0 = sse::loadu_si32(first);
            const auto x1 = sse::loadu_si32(first + m);
            B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
                sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

            if (WJR_LIKELY(B == 0)) {
                return voidopt;
            }

            break;
        }

        const auto m = n - 8;
        const auto x0 = sse::loadu_si64(first);
        const auto x1 = sse::loadu_si64(first + m);
        B = sse::movemask_epi8(sse::cmpeq_epi8(x0, sse::set1_epi8('\\'))) |
            sse::movemask_epi8(sse::cmpeq_epi8(x1, sse::set1_epi8('\\'))) << m;

        if (WJR_LIKELY(B == 0)) {
            return voidopt;
        }
    } while (0);

    do {
        const int pos = ctz(B);

        if (WJR_UNLIKELY(pos == n - 1)) {
            return nullopt;
        }

        const uint8_t ch = first[pos + 1];

        if (WJR_UNLIKELY(ch == 'u')) {
            const char *__first = first;
            WJR_OPTIONAL_SET(__first, check_unicode_codepoint(first + pos + 2, last));

            if (__first == last) {
                return voidopt;
            }

            // two backslash
            if ((__first - first) - pos != 6) {
                B &= B - 1;
            }
        } else {
            const uint8_t code = escape_table[ch];

            if (WJR_UNLIKELY(code == 0)) {
                return nullopt;
            }

            // backslash
            if (WJR_UNLIKELY(code == 0x5c)) {
                B &= B - 1;
            }
        }

        B &= B - 1;
    } while (B);

    return voidopt;
}
#endif

namespace {
#if WJR_HAS_BUILTIN(UTF8_UNICODE_TO_UTF8)
WJR_INTRINSIC_INLINE char *small_copy(char *dst, const char *src, unsigned int n) noexcept {
    if (n >= 8) {
    #if WJR_HAS_SIMD(AVX2)
        if (n >= 16) {
            builtin_memcpy(dst, src, 16);
            builtin_memcpy(dst + n - 16, src + n - 16, 16);
            return dst + n;
        }
    #endif
        builtin_memcpy(dst, src, 8);
        builtin_memcpy(dst + n - 8, src + n - 8, 8);
        return dst + n;
    }

    if (WJR_UNLIKELY(n == 0)) {
        return dst;
    }

    if (n >= 4) {
        builtin_memcpy(dst, src, 4);
        builtin_memcpy(dst + n - 4, src + n - 4, 4);
        return dst + n;
    }

    if (n >= 2) {
        builtin_memcpy(dst, src, 2);
        builtin_memcpy(dst + n - 2, src + n - 2, 2);
        return dst + n;
    }

    *dst = *src;
    return dst + 1;
}
#endif
} // namespace

#if WJR_HAS_BUILTIN(UTF8_UNICODE_TO_UTF8)
compressed_optional<char *> builtin_unicode_to_utf8(char *dst, const char *first,
                                                    const char *last) noexcept {
    using namespace detail;
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
                        WJR_OPTIONAL_SET(
                            first, unicode_codepoint_to_utf8(dst, first + u8_width + 1, last));

                        if (first + u8_width <= last) {
                            goto LOOP;
                        }

                        goto SMALL;
                    }

                    const uint8_t code = escape_table[ch];

                    if (WJR_UNLIKELY(code == 0)) {
                        return nullopt;
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
                    WJR_OPTIONAL_SET(__first,
                                     unicode_codepoint_to_utf8(dst, first + pos + 2, last));

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
                        return nullopt;
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
                        return nullopt;
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
                    builtin_memcpy(dst, first, 2);
                    builtin_memcpy(dst + m, first + m, 2);
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
                builtin_memcpy(dst, first, 4);
                builtin_memcpy(dst + m, first + m, 4);
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
            builtin_memcpy(dst, first, 8);
            builtin_memcpy(dst + m, first + m, 8);
            return dst + n;
        }
    } while (0);

    int last_pos = 0;

    do {
        const int pos = ctz(B);
        dst = small_copy(dst, first + last_pos, pos - last_pos);

        if (WJR_UNLIKELY(pos == n - 1)) {
            return nullopt;
        }

        const uint8_t ch = first[pos + 1];

        if (WJR_UNLIKELY(ch == 'u')) {
            const char *__first = first;
            WJR_OPTIONAL_SET(__first, unicode_codepoint_to_utf8(dst, first + pos + 2, last));

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
                return nullopt;
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
#endif

} // namespace wjr::utf8