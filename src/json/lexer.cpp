#include <wjr/json/lexer.hpp>
#include <wjr/math.hpp>

namespace wjr::json {

#if !WJR_HAS_BUILTIN(JSON_LEXER_READER_READ_BUF) || !WJR_HAS_BUILTIN(JSON_MINIFY_BUF)

namespace lexer_detail {

constexpr static std::array<uint8_t, 256> code_table = {
    4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 4, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 0, 2, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 4, 2, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

constexpr static std::array<uint8_t, 16 * 8> diff_table = {
    1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 1, 1, 2, 3, 4, 5, 6, 7, 0, 0, 1, 2, 3, 4, 5, 6,
    1, 2, 2, 3, 4, 5, 6, 7, 0, 1, 1, 2, 3, 4, 5, 6, 1, 1, 1, 2, 3, 4, 5, 6, 0, 0, 0, 1, 2, 3, 4, 5,
    1, 2, 3, 3, 4, 5, 6, 7, 0, 1, 2, 2, 3, 4, 5, 6, 1, 1, 2, 2, 3, 4, 5, 6, 0, 0, 1, 1, 2, 3, 4, 5,
    1, 2, 2, 2, 3, 4, 5, 6, 0, 1, 1, 1, 2, 3, 4, 5, 1, 1, 1, 1, 2, 3, 4, 5, 0, 0, 0, 0, 1, 2, 3, 4};

} // namespace lexer_detail

#endif

#if !WJR_HAS_BUILTIN(JSON_LEXER_READER_READ_BUF)

uint32_t lexer::read(uint32_t *token_buf, size_type token_buf_size) noexcept {
    if (WJR_UNLIKELY(first == last)) {
        return 0;
    }

    using namespace lexer_detail;

    WJR_ASSERT_ASSUME_L2(first < last);

    uint32_t count = 0;
    char stk[64];

    do {
        const char *ptr;

        if (const size_t diff = last - first; WJR_LIKELY(diff > 64)) {
            ptr = first;
            first += 64;
        } else {
            if (diff == 64) {
                ptr = first;
            } else {
                std::memcpy(stk, first, diff);
                std::memset(stk + diff, ' ', 64 - diff);
                ptr = stk;
            }

            first = last;
            count |= big_mask;
        }

        uint64_t MASK[4][5] = {{0}};

        for (int i = 0; i < 64; i += 4) {
            MASK[0][code_table[static_cast<uint8_t>(ptr[i])]] |= 1ull << i;
            MASK[1][code_table[static_cast<uint8_t>(ptr[i + 1])]] |= 1ull << (i + 1);
            MASK[2][code_table[static_cast<uint8_t>(ptr[i + 2])]] |= 1ull << (i + 2);
            MASK[3][code_table[static_cast<uint8_t>(ptr[i + 3])]] |= 1ull << (i + 3);
        }

        uint64_t B = MASK[0][0] | MASK[1][0] | MASK[2][0] | MASK[3][0];
        uint64_t Q = MASK[0][1] | MASK[1][1] | MASK[2][1] | MASK[3][1];
        uint64_t S = MASK[0][2] | MASK[1][2] | MASK[2][2] | MASK[3][2];
        uint64_t W = MASK[0][3] | MASK[1][3] | MASK[2][3] | MASK[3][3];

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
                WJR_ASSUME(!(idx & 0x3F));

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

    return count & ~big_mask;
}

#endif

#if !WJR_HAS_BUILTIN(JSON_MINIFY_BUF)

char *minify(char *dst, const char *first, const char *last) noexcept {
    if (WJR_UNLIKELY(first == last)) {
        return dst;
    }

    using namespace lexer_detail;

    WJR_ASSERT_ASSUME_L2(first < last);

    uint64_t prev_in_string = 0;
    uint64_t prev_is_escape = 0;

    char stk[64];

    do {
        const char *ptr;

        if (const size_t diff = last - first; WJR_LIKELY(diff > 64)) {
            ptr = first;
            first += 64;
        } else {
            if (diff == 64) {
                ptr = first;
            } else {
                std::memset(stk, ' ', 64);
                std::memcpy(stk, first, diff);
                ptr = stk;
            }

            first = last;
        }

        uint64_t MASK[4][5] = {{0}};

        for (int i = 0; i < 64; i += 4) {
            MASK[0][code_table[static_cast<uint8_t>(ptr[i])]] |= 1ull << i;
            MASK[1][code_table[static_cast<uint8_t>(ptr[i + 1])]] |= 1ull << (i + 1);
            MASK[2][code_table[static_cast<uint8_t>(ptr[i + 2])]] |= 1ull << (i + 2);
            MASK[3][code_table[static_cast<uint8_t>(ptr[i + 3])]] |= 1ull << (i + 3);
        }

        uint64_t B = MASK[0][0] | MASK[1][0] | MASK[2][0] | MASK[3][0];
        uint64_t Q = MASK[0][1] | MASK[1][1] | MASK[2][1] | MASK[3][1];
        uint64_t W = MASK[0][3] | MASK[1][3] | MASK[2][3] | MASK[3][3];

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

        for (int i = 0; i < 64; i += 4) {
            const uint8_t X = (W >> i) & 0x0F;

            dst[0] = ptr[i];
            dst[diff_table[X * 8]] = ptr[i + 1];
            dst[diff_table[X * 8 + 1]] = ptr[i + 2];
            dst[diff_table[X * 8 + 2]] = ptr[i + 3];

            dst += diff_table[X * 8 + 3];
        }

    } while (WJR_LIKELY(first != last));

    return dst;
}

#endif

} // namespace wjr::json
