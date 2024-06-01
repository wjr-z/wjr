#ifndef WJR_X86_JSON_LEXER_HPP__
#define WJR_X86_JSON_LEXER_HPP__

#include <wjr/json/lexer-impl.hpp>
#include <wjr/math.hpp>
#include <wjr/x86/simd/simd.hpp>

namespace wjr::json {

#if WJR_HAS_SIMD(SSSE3) && WJR_HAS_SIMD(X86_SIMD)
#define WJR_HAS_BUILTIN_JSON_LEXER_READER_READ_BUF WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(JSON_LEXER_READER_READ_BUF)

namespace lexer_details {

#if !WJR_HAS_SIMD(AVX2)
const static __m128i lh8_mask = sse::set1_epi8(0x0f);

const static __m128i lo8_lookup =
    sse::set_epi8(0, 0, 12, 1, 4, 10, 8, 0, 0, 0, 0, 0, 0, 0, 0, 16);
const static __m128i hi8_lookup =
    sse::set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 0, 2, 17, 0, 8);
#else
const static __m256i lh8_mask = avx::set1_epi8(0x0f);
const static __m256i lo8_lookup =
    avx::set_epi8(0, 0, 12, 1, 4, 10, 8, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 12, 1, 4, 10,
                  8, 0, 0, 0, 0, 0, 0, 0, 0, 16);
const static __m256i hi8_lookup =
    avx::set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 0, 2, 17, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0,
                  4, 0, 4, 0, 2, 17, 0, 8);
#endif

template <typename simd>
inline typename simd::int_type equal(typename simd::int_type x, uint8_t ch) {
    return simd::cmpeq_epi8(x, simd::set1_epi8(ch));
}

template <typename simd>
void load_simd(const char *first, typename simd::int_type *arr) {
    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;
    constexpr auto u8_loop = 64 / u8_width;
    for (unsigned i = 0; i < u8_loop; ++i) {
        arr[i] = simd::loadu((typename simd::int_type *)(first + i * u8_width));
    }
}

} // namespace lexer_details

template <uint32_t token_buf_size>
uint32_t basic_lexer<token_buf_size>::read(uint32_t *token_buf) noexcept {
    using namespace lexer_details;

    constexpr bool is_avx = WJR_HAS_SIMD(AVX2);
    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;
    constexpr auto u8_loop = 64 / u8_width;

    constexpr uint32_t token_buf_mask = token_buf_size * 2;

    auto first = m_storage.first;
    const auto last = m_storage.last;

    if (WJR_UNLIKELY(first == last)) {
        return 0;
    }

    uint64_t prev_is_escape;
    uint64_t prev_in_string;
    uint64_t prev_is_ws;
    uint32_t idx;

    if constexpr (__is_dynamic) {
        prev_is_escape = prev_in_string = 0;
        prev_is_ws = ~0ull;
        idx = 0;
    } else {
        prev_is_escape = m_storage.prev_is_escape;
        prev_in_string = m_storage.prev_in_string;
        prev_is_ws = m_storage.prev_is_ws;
        idx = m_storage.idx;
    }

    WJR_ASSERT_ASSUME_L2(first < last);

    uint32_t count = 0;
    bool loop;

    do {
        simd_int x[u8_loop];

        const size_t diff = last - first;

        if (WJR_LIKELY(diff > 64)) {
            load_simd<simd>(first, x);
            first += 64;
        } else {
            if (diff == 64) {
                load_simd<simd>(first, x);
            } else {
                char buf[64];
                char ch;
                switch (last[-1]) {
                case ' ':
                case '\n':
                case '\r':
                case '\t':
                case '[':
                case ']':
                case '{':
                case '}': {
                    ch = ' ';
                    break;
                }
                default: {
                    ch = '\0';
                    break;
                }
                }

                std::memcpy(buf, first, diff);
                std::memset(buf + diff, ch, 64 - diff);
                load_simd<simd>(buf, x);
            }

            first = last;
            if constexpr (!__is_dynamic) {
                count |= token_buf_mask;
            }
        }

        uint64_t B = 0; // backslash

        for (unsigned i = 0; i < u8_loop; ++i) {
            const auto backslash = equal<simd>(x[i], '\\');
            B |= (uint64_t)simd::movemask_epi8(backslash) << (i * u8_width);
        }

        uint64_t Q = 0; // quote

        for (unsigned i = 0; i < u8_loop; ++i) {
            const auto quote = equal<simd>(x[i], '\"');
            Q |= (uint64_t)simd::movemask_epi8(quote) << (i * u8_width);
        }

        uint64_t S = 0; // brackets, comma , colon
        uint64_t W = 0; // whitespace

        for (unsigned i = 0; i < u8_loop; ++i) {
            const auto shuf_lo8 = simd::shuffle_epi8(lo8_lookup, x[i]);
            const auto shuf_hi8 = simd::shuffle_epi8(
                hi8_lookup, simd::And(simd::srli_epi16(x[i], 4), lh8_mask));

            const auto result = simd::And(shuf_lo8, shuf_hi8);
            // comma : 1
            // colon : 2
            // brackets : 4
            // whitespace : 8, 16

            uint32_t stu = simd::movemask_epi8(
                simd::cmpeq_epi8(simd::And(result, simd::set1_epi8(7)), simd::zeros()));
            uint32_t wsp = simd::movemask_epi8(
                simd::cmpeq_epi8(simd::And(result, simd::set1_epi8(24)), simd::zeros()));

            S |= (uint64_t)(stu) << (i * u8_width);
            W |= (uint64_t)(wsp) << (i * u8_width);
        }

        S = ~S;
        W = ~W;

        {
            if (!B) {
                B = prev_is_escape;
                Q &= ~B;
                prev_is_escape = 0;
            } else {
                const uint64_t codeB = calc_backslash(B & ~prev_is_escape);
                const auto escape = (codeB & B) >> 63;
                B = codeB ^ (B | prev_is_escape);
                Q &= ~B;
                prev_is_escape = escape;
            }
        }

        const uint64_t R = prefix_xor(Q) ^ prev_in_string;

        const auto WS = S | W;
        const auto WT = shld(WS, prev_is_ws, 1);
        const auto TW = shld(~WS, ~prev_is_ws, 1);
        prev_is_ws = WS;

        S &= ~R;
        prev_in_string = static_cast<uint64_t>(static_cast<int64_t>(R) >> 63);

        S |= Q;
        S |= ((TW & W) | (WT & ~W)) & ~R;
        S &= ~(Q & ~R);

        if (S) {
            const auto num = popcount(S);

            do {
                for (int i = 0; i < 4; ++i) {
                    token_buf[i] = idx + ctz(S);
                    S &= S - 1;
                }

                if (WJR_UNLIKELY(num <= 4)) {
                    break;
                }

                for (int i = 4; i < 8; ++i) {
                    token_buf[i] = idx + ctz(S);
                    S &= S - 1;
                }

                if (WJR_LIKELY(num <= 8)) {
                    break;
                }

                for (int i = 8; i < 12; ++i) {
                    token_buf[i] = idx + ctz(S);
                    S &= S - 1;
                }

                if (WJR_LIKELY(num <= 12)) {
                    break;
                }

                for (int i = 12; i < 16; ++i) {
                    token_buf[i] = idx + ctz(S);
                    S &= S - 1;
                }

                if (WJR_LIKELY(num <= 16)) {
                    break;
                }

                for (int i = 16; i < num; ++i) {
                    token_buf[i] = idx + ctz(S);
                    S &= S - 1;
                }
            } while (0);

            token_buf += num;
            count += num;
        }

        idx += 64;

        if constexpr (!__is_dynamic) {
            loop = (count <= token_buf_size);
        } else {
            loop = first != last;
        }

    } while (WJR_LIKELY(loop));

    if constexpr (!__is_dynamic) {
        m_storage.first = first;
        m_storage.prev_is_escape = prev_is_escape;
        m_storage.prev_in_string = prev_in_string;
        m_storage.prev_is_ws = prev_is_ws;
        m_storage.idx = idx;
    }

    if constexpr (!__is_dynamic) {
        return count & (token_buf_mask - 1);
    } else {
        return count;
    }
}

#endif

} // namespace wjr::json

#endif // WJR_X86_JSON_LEXER_HPP__