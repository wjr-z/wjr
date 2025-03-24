#include <wjr/arch/x86/simd/simd.hpp>
#include <wjr/json/lexer.hpp>
#include <wjr/math.hpp>

namespace wjr::json {

#if WJR_HAS_BUILTIN(JSON_LEXER_READER_READ_BUF) || WJR_HAS_BUILTIN(JSON_MINIFY_BUF)

namespace lexer_detail {

namespace {
const std::array<uint64_t, 256> thintable_epi8 = {
    0x0706050403020100, 0x0007060504030201, 0x0007060504030200, 0x0000070605040302,
    0x0007060504030100, 0x0000070605040301, 0x0000070605040300, 0x0000000706050403,
    0x0007060504020100, 0x0000070605040201, 0x0000070605040200, 0x0000000706050402,
    0x0000070605040100, 0x0000000706050401, 0x0000000706050400, 0x0000000007060504,
    0x0007060503020100, 0x0000070605030201, 0x0000070605030200, 0x0000000706050302,
    0x0000070605030100, 0x0000000706050301, 0x0000000706050300, 0x0000000007060503,
    0x0000070605020100, 0x0000000706050201, 0x0000000706050200, 0x0000000007060502,
    0x0000000706050100, 0x0000000007060501, 0x0000000007060500, 0x0000000000070605,
    0x0007060403020100, 0x0000070604030201, 0x0000070604030200, 0x0000000706040302,
    0x0000070604030100, 0x0000000706040301, 0x0000000706040300, 0x0000000007060403,
    0x0000070604020100, 0x0000000706040201, 0x0000000706040200, 0x0000000007060402,
    0x0000000706040100, 0x0000000007060401, 0x0000000007060400, 0x0000000000070604,
    0x0000070603020100, 0x0000000706030201, 0x0000000706030200, 0x0000000007060302,
    0x0000000706030100, 0x0000000007060301, 0x0000000007060300, 0x0000000000070603,
    0x0000000706020100, 0x0000000007060201, 0x0000000007060200, 0x0000000000070602,
    0x0000000007060100, 0x0000000000070601, 0x0000000000070600, 0x0000000000000706,
    0x0007050403020100, 0x0000070504030201, 0x0000070504030200, 0x0000000705040302,
    0x0000070504030100, 0x0000000705040301, 0x0000000705040300, 0x0000000007050403,
    0x0000070504020100, 0x0000000705040201, 0x0000000705040200, 0x0000000007050402,
    0x0000000705040100, 0x0000000007050401, 0x0000000007050400, 0x0000000000070504,
    0x0000070503020100, 0x0000000705030201, 0x0000000705030200, 0x0000000007050302,
    0x0000000705030100, 0x0000000007050301, 0x0000000007050300, 0x0000000000070503,
    0x0000000705020100, 0x0000000007050201, 0x0000000007050200, 0x0000000000070502,
    0x0000000007050100, 0x0000000000070501, 0x0000000000070500, 0x0000000000000705,
    0x0000070403020100, 0x0000000704030201, 0x0000000704030200, 0x0000000007040302,
    0x0000000704030100, 0x0000000007040301, 0x0000000007040300, 0x0000000000070403,
    0x0000000704020100, 0x0000000007040201, 0x0000000007040200, 0x0000000000070402,
    0x0000000007040100, 0x0000000000070401, 0x0000000000070400, 0x0000000000000704,
    0x0000000703020100, 0x0000000007030201, 0x0000000007030200, 0x0000000000070302,
    0x0000000007030100, 0x0000000000070301, 0x0000000000070300, 0x0000000000000703,
    0x0000000007020100, 0x0000000000070201, 0x0000000000070200, 0x0000000000000702,
    0x0000000000070100, 0x0000000000000701, 0x0000000000000700, 0x0000000000000007,
    0x0006050403020100, 0x0000060504030201, 0x0000060504030200, 0x0000000605040302,
    0x0000060504030100, 0x0000000605040301, 0x0000000605040300, 0x0000000006050403,
    0x0000060504020100, 0x0000000605040201, 0x0000000605040200, 0x0000000006050402,
    0x0000000605040100, 0x0000000006050401, 0x0000000006050400, 0x0000000000060504,
    0x0000060503020100, 0x0000000605030201, 0x0000000605030200, 0x0000000006050302,
    0x0000000605030100, 0x0000000006050301, 0x0000000006050300, 0x0000000000060503,
    0x0000000605020100, 0x0000000006050201, 0x0000000006050200, 0x0000000000060502,
    0x0000000006050100, 0x0000000000060501, 0x0000000000060500, 0x0000000000000605,
    0x0000060403020100, 0x0000000604030201, 0x0000000604030200, 0x0000000006040302,
    0x0000000604030100, 0x0000000006040301, 0x0000000006040300, 0x0000000000060403,
    0x0000000604020100, 0x0000000006040201, 0x0000000006040200, 0x0000000000060402,
    0x0000000006040100, 0x0000000000060401, 0x0000000000060400, 0x0000000000000604,
    0x0000000603020100, 0x0000000006030201, 0x0000000006030200, 0x0000000000060302,
    0x0000000006030100, 0x0000000000060301, 0x0000000000060300, 0x0000000000000603,
    0x0000000006020100, 0x0000000000060201, 0x0000000000060200, 0x0000000000000602,
    0x0000000000060100, 0x0000000000000601, 0x0000000000000600, 0x0000000000000006,
    0x0000050403020100, 0x0000000504030201, 0x0000000504030200, 0x0000000005040302,
    0x0000000504030100, 0x0000000005040301, 0x0000000005040300, 0x0000000000050403,
    0x0000000504020100, 0x0000000005040201, 0x0000000005040200, 0x0000000000050402,
    0x0000000005040100, 0x0000000000050401, 0x0000000000050400, 0x0000000000000504,
    0x0000000503020100, 0x0000000005030201, 0x0000000005030200, 0x0000000000050302,
    0x0000000005030100, 0x0000000000050301, 0x0000000000050300, 0x0000000000000503,
    0x0000000005020100, 0x0000000000050201, 0x0000000000050200, 0x0000000000000502,
    0x0000000000050100, 0x0000000000000501, 0x0000000000000500, 0x0000000000000005,
    0x0000000403020100, 0x0000000004030201, 0x0000000004030200, 0x0000000000040302,
    0x0000000004030100, 0x0000000000040301, 0x0000000000040300, 0x0000000000000403,
    0x0000000004020100, 0x0000000000040201, 0x0000000000040200, 0x0000000000000402,
    0x0000000000040100, 0x0000000000000401, 0x0000000000000400, 0x0000000000000004,
    0x0000000003020100, 0x0000000000030201, 0x0000000000030200, 0x0000000000000302,
    0x0000000000030100, 0x0000000000000301, 0x0000000000000300, 0x0000000000000003,
    0x0000000000020100, 0x0000000000000201, 0x0000000000000200, 0x0000000000000002,
    0x0000000000000100, 0x0000000000000001, 0x0000000000000000, 0x0000000000000000,
}; // static uint64_t thintable_epi8[256]

const std::array<uint8_t, 256> popcount_mul_2 = {
    0, 2,  2,  4,  2,  4,  4,  6,  2,  4,  4,  6,  4,  6,  6,  8,  2, 4,  4,  6,  4,  6,  6,  8,
    4, 6,  6,  8,  6,  8,  8,  10, 2,  4,  4,  6,  4,  6,  6,  8,  4, 6,  6,  8,  6,  8,  8,  10,
    4, 6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10, 12, 2, 4,  4,  6,  4,  6,  6,  8,
    4, 6,  6,  8,  6,  8,  8,  10, 4,  6,  6,  8,  6,  8,  8,  10, 6, 8,  8,  10, 8,  10, 10, 12,
    4, 6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10, 12, 6, 8,  8,  10, 8,  10, 10, 12,
    8, 10, 10, 12, 10, 12, 12, 14, 2,  4,  4,  6,  4,  6,  6,  8,  4, 6,  6,  8,  6,  8,  8,  10,
    4, 6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10, 12, 4, 6,  6,  8,  6,  8,  8,  10,
    6, 8,  8,  10, 8,  10, 10, 12, 6,  8,  8,  10, 8,  10, 10, 12, 8, 10, 10, 12, 10, 12, 12, 14,
    4, 6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10, 12, 6, 8,  8,  10, 8,  10, 10, 12,
    8, 10, 10, 12, 10, 12, 12, 14, 6,  8,  8,  10, 8,  10, 10, 12, 8, 10, 10, 12, 10, 12, 12, 14,
    8, 10, 10, 12, 10, 12, 12, 14, 10, 12, 12, 14, 12, 14, 14, 16};

const std::array<uint8_t, 272> pshufb_combine_table = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff,
    0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x01, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

    #if !WJR_HAS_SIMD(AVX2)
const __m128i lh8_mask = sse::set1_epi8(0x0f);

const __m128i lo8_lookup = sse::set_epi8(0, 0, 12, 1, 4, 10, 8, 0, 0, 0, 0, 0, 0, 0, 0, 16);
const __m128i hi8_lookup = sse::set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 0, 2, 17, 0, 8);

WJR_INTRINSIC_INLINE void compress(char *dst, __m128i x, uint16_t mask) noexcept {
    const uint8_t mask0 = uint8_t(mask);
    const uint8_t mask1 = uint8_t(mask >> 8);

    __m128i shufmask = sse::set_epi64x(thintable_epi8[mask1], thintable_epi8[mask0]);
    shufmask = sse::add_epi8(shufmask, sse::set_epi64x(0x08080808'08080808, 0));
    const __m128i pruned = sse::shuffle_epi8(x, shufmask);
    const int pop0 = popcount_mul_2[mask0];

    const __m128i compactmask = sse::loadu(&pshufb_combine_table[pop0 * 8]);
    const __m128i almostthere = sse::shuffle_epi8(pruned, compactmask);

    sse::storeu(dst, almostthere);
}

    #else
const __m256i lh8_mask = avx::set1_epi8(0x0f);
const __m256i lo8_lookup = avx::set_epi8(0, 0, 12, 1, 4, 10, 8, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0,
                                         12, 1, 4, 10, 8, 0, 0, 0, 0, 0, 0, 0, 0, 16);
const __m256i hi8_lookup = avx::set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 0, 2, 17, 0, 8, 0, 0, 0,
                                         0, 0, 0, 0, 0, 4, 0, 4, 0, 2, 17, 0, 8);

WJR_INTRINSIC_INLINE void compress(char *dst, __m256i x, uint32_t mask) noexcept {
    const uint8_t mask0 = uint8_t(mask);
    const uint8_t mask1 = uint8_t(mask >> 8);
    const uint8_t mask2 = uint8_t(mask >> 16);
    const uint8_t mask3 = uint8_t(mask >> 24);

    __m256i shufmask = avx::set_epi64x(thintable_epi8[mask3], thintable_epi8[mask2],
                                       thintable_epi8[mask1], thintable_epi8[mask0]);
    shufmask = avx::add_epi8(shufmask, avx::set_epi64x(0x18181818'18181818, 0x10101010'10101010,
                                                       0x08080808'08080808, 0));
    const __m256i pruned = avx::shuffle_epi8(x, shufmask);

    const int pop0 = popcount_mul_2[mask0];
    const int pop2 = popcount_mul_2[mask2];

    const __m256i compactmask = avx::concat(sse::loadu(&pshufb_combine_table[pop0 * 8]),
                                            sse::loadu(&pshufb_combine_table[pop2 * 8]));
    const __m256i almostthere = avx::shuffle_epi8(pruned, compactmask);

    sse::storeu(dst, avx::getlow(almostthere));
    sse::storeu(dst + 16 - popcount<uint16_t>(mask & 0xFFFF), avx::gethigh(almostthere));
}

    #endif

} // namespace
} // namespace lexer_detail

#endif

#if WJR_HAS_BUILTIN(JSON_LEXER_READER_READ_BUF)

/// @todo Unroll two times maybe faster on some platforms.
typename lexer::result_type lexer::read(uint32_t *token_buf, size_type token_buf_size) noexcept {
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
        if (const size_t diff = last - first; WJR_LIKELY(diff > 64)) {
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
                char buf[64];
                std::memset(buf, ' ', 64);
                std::memcpy(buf, first, diff);

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
            const auto shuf_hi8 =
                simd::shuffle_epi8(hi8_lookup, simd::And(simd::srli_epi16(stk[i], 4), lh8_mask));

            const auto result = simd::And(shuf_lo8, shuf_hi8);
            // comma : 1
            // colon : 2
            // brackets : 4
            // whitespace : 8, 16
            // others : 0

            const uint32_t stu = simd::movemask_epi8(simd::cmpgt_epi8(result, simd::zeros()));
            const uint32_t wsp = simd::movemask_epi8(simd::cmpgt_epi8(result, simd::set1_epi8(7)));

            S |= (uint64_t)(stu) << (i * u8_width);
            W |= (uint64_t)(wsp) << (i * u8_width);
        }

        const auto WS = S;
        S ^= W;

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

        const auto WT = shld(WS, prev_is_ws, 1);
        prev_is_ws = WS;

        S |= WT & ~W;
        S &= ~R;
        S |= Q;

        if (S) {
            const auto num = popcount(S);

            do {
                const auto __idx = idx;
                WJR_ASSUME(!(__idx & 0x3F));

                token_buf[0] = __idx + ctz(S);
                S &= S - 1;
                token_buf[1] = __idx + ctz(S);
                S &= S - 1;
                token_buf[2] = __idx + ctz(S);
                S &= S - 1;
                token_buf[3] = __idx + ctz(S);

                if (WJR_UNLIKELY(num <= 4)) {
                    break;
                }

                S &= S - 1;

                token_buf[4] = __idx + ctz(S);
                S &= S - 1;
                token_buf[5] = __idx + ctz(S);
                S &= S - 1;
                token_buf[6] = __idx + ctz(S);
                S &= S - 1;
                token_buf[7] = __idx + ctz(S);

                if (WJR_LIKELY(num <= 8)) {
                    break;
                }

                S &= S - 1;

                for (int i = 8; i < 12; ++i) {
                    token_buf[i] = __idx + ctz(S);
                    S &= S - 1;
                }

                if (WJR_LIKELY(num <= 12)) {
                    break;
                }

                for (int i = 12; i < 16; ++i) {
                    token_buf[i] = __idx + ctz(S);
                    S &= S - 1;
                }

                if (WJR_LIKELY(num <= 16)) {
                    break;
                }

                for (int i = 16; i < num; ++i) {
                    token_buf[i] = __idx + ctz(S);
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

#if WJR_HAS_BUILTIN(JSON_MINIFY_BUF)

char *minify(char *dst, const char *first, const char *last) noexcept {
    if (WJR_UNLIKELY(first == last)) {
        return dst;
    }

    using namespace lexer_detail;

    using simd = std::conditional_t<WJR_HAS_SIMD(AVX2), avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;
    constexpr auto u8_loop = 64 / u8_width;

    WJR_ASSERT_ASSUME_L2(first < last);

    uint64_t prev_in_string = 0;
    uint64_t prev_is_escape = 0;
    simd_int stk[u8_loop];
    char buf[64];

    do {
        if (const size_t diff = last - first; WJR_LIKELY(diff > 64)) {
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
                std::memset(buf, ' ', 64);
                std::memcpy(buf, first, diff);

                for (size_t i = 0; i < u8_loop; ++i) {
                    stk[i] = simd::loadu(buf + i * u8_width);
                }
            }

            first = last;
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

        uint64_t W = 0; // whitespace

        for (unsigned i = 0; i < u8_loop; ++i) {
            const auto shuf_lo8 = simd::shuffle_epi8(lo8_lookup, stk[i]);
            const auto shuf_hi8 =
                simd::shuffle_epi8(hi8_lookup, simd::And(simd::srli_epi16(stk[i], 4), lh8_mask));

            const auto result = simd::And(shuf_lo8, shuf_hi8);
            // whitespace : 8, 16
            // others : 0, 1, 2, 4

            const uint32_t wsp = simd::movemask_epi8(simd::cmpgt_epi8(result, simd::set1_epi8(7)));

            W |= (uint64_t)(wsp) << (i * u8_width);
        }

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
        W &= ~(R | Q);

        // this is last block
        if (WJR_UNLIKELY(first == last)) {
            auto buf_end = buf;
            for (unsigned i = 0; i < u8_loop; ++i) {
                const typename simd::mask_type mask = (W >> (i * u8_width)) & simd::mask();
                compress(buf_end, stk[i], mask);
                buf_end += u8_width - popcount(mask);
            }

            std::memcpy(dst, buf, buf_end - buf);
            break;
        }

        for (unsigned i = 0; i < u8_loop; ++i) {
            const typename simd::mask_type mask = (W >> (i * u8_width)) & simd::mask();
            compress(dst, stk[i], mask);
            dst += u8_width - popcount(mask);
        }
    } while (true);

    return dst;
}

#endif

} // namespace wjr::json
