#include <wjr/json/lexer.hpp>
#include <wjr/math.hpp>

namespace wjr::json {

#if WJR_HAS_BUILTIN(JSON_LEXER_READER_READ_BUF)

namespace lexer_detail {

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

} // namespace lexer_detail

typename lexer::result_type lexer::read(uint32_t *token_buf,
                                        size_type token_buf_size) noexcept {
    if (WJR_UNLIKELY(first == last)) {
        return result_type::mask;
    }

    using namespace lexer_detail;

    using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;
    constexpr auto u8_loop = 64 / u8_width;

    WJR_ASSERT_ASSUME_L2(first < last);

    uint32_t count = 0;
    simd_int stk[u8_loop];

    do {
        const size_t diff = last - first;

        if (WJR_LIKELY(diff > 64)) {
            for (size_t i = 0; i < u8_loop; ++i) {
                stk[i] = simd::loadu(first + i * u8_width);
            }

            first += 64;
        } else {
            if (diff == 64) {
                for (size_t i = 0; i < u8_loop; ++i) {
                    stk[i] = simd::loadu(first + i * u8_width);
                }
            } else {
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

                char buf[64];
                std::memcpy(buf, first, diff);
                std::memset(buf + diff, ch, 64 - diff);

                for (size_t i = 0; i < u8_loop; ++i) {
                    stk[i] = simd::loadu(buf + i * u8_width);
                }
            }

            first = last;
            count |= result_type::mask;
        }

        uint64_t B = 0; // backslash

        for (unsigned i = 0; i < u8_loop; ++i) {
            const auto backslash = simd::cmpeq_epi8(stk[i], simd::set1_epi8('\\'));
            B |= (uint64_t)simd::movemask_epi8(backslash) << (i * u8_width);
        }

        uint64_t Q = 0; // quote

        for (unsigned i = 0; i < u8_loop; ++i) {
            const auto quote = simd::cmpeq_epi8(stk[i], simd::set1_epi8('"'));
            Q |= (uint64_t)simd::movemask_epi8(quote) << (i * u8_width);
        }

        uint64_t S = 0; // brackets, comma , colon
        uint64_t W = 0; // whitespace

        for (unsigned i = 0; i < u8_loop; ++i) {
            const auto shuf_lo8 = simd::shuffle_epi8(lo8_lookup, stk[i]);
            const auto shuf_hi8 = simd::shuffle_epi8(
                hi8_lookup, simd::And(simd::srli_epi16(stk[i], 4), lh8_mask));

            const auto result = simd::And(shuf_lo8, shuf_hi8);
            // comma : 1
            // colon : 2
            // brackets : 4
            // whitespace : 8, 16

            const uint32_t stu = simd::movemask_epi8(
                simd::cmpeq_epi8(simd::And(result, simd::set1_epi8(7)), simd::zeros()));
            const uint32_t wsp = simd::movemask_epi8(
                simd::cmpeq_epi8(simd::And(result, simd::set1_epi8(24)), simd::zeros()));

            S |= (uint64_t)(stu) << (i * u8_width);
            W |= (uint64_t)(wsp) << (i * u8_width);
        }

        S = ~S;
        W = ~W;

        if (WJR_LIKELY(!B)) {
            B = prev_is_escape;
            prev_is_escape = 0;
        } else {
            const uint64_t codeB = calc_backslash(B & ~prev_is_escape);
            const auto escape = (codeB & B) >> 63;
            B = codeB ^ (B | prev_is_escape);
            prev_is_escape = escape;
        }

        Q &= ~B;
        const uint64_t R = prefix_xor(Q) ^ prev_in_string;
        prev_in_string = static_cast<uint64_t>(static_cast<int64_t>(R) >> 63);

        const auto WS = S | W;
        const auto WT = shld(WS, prev_is_ws, 1);
        prev_is_ws = WS;

        S |= (WT & ~W);
        S &= ~R;
        S |= Q;

        if (S) {
            const auto num = popcount(S);

            do {
                token_buf[0] = idx + ctz(S);
                S &= S - 1;
                token_buf[1] = idx + ctz(S);
                S &= S - 1;
                token_buf[2] = idx + ctz(S);
                S &= S - 1;
                token_buf[3] = idx + ctz(S);

                if (WJR_UNLIKELY(num <= 4)) {
                    break;
                }

                S &= S - 1;

                token_buf[4] = idx + ctz(S);
                S &= S - 1;
                token_buf[5] = idx + ctz(S);
                S &= S - 1;
                token_buf[6] = idx + ctz(S);
                S &= S - 1;
                token_buf[7] = idx + ctz(S);

                if (WJR_LIKELY(num <= 8)) {
                    break;
                }

                S &= S - 1;

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
    } while (WJR_LIKELY(count <= token_buf_size));

    return count;
}

#endif

} // namespace wjr::json
