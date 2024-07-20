#include <wjr/json/lexer.hpp>
#include <wjr/math.hpp>

namespace wjr::json {

#if !WJR_HAS_BUILTIN(JSON_LEXER_READER_READ_BUF)

namespace lexer_detail {

const static std::array<uint8_t, 256> code_table = {
    4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 3, 4, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 2, 0, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

template <typename Lexer>
uint32_t read(Lexer lexer) noexcept {
    auto &storage = lexer.storage;

    auto first = storage.first;
    const auto last = storage.last;

    if (WJR_UNLIKELY(first == last)) {
        return 0;
    }

    using namespace lexer_detail;

    constexpr bool read_once = lexer.read_once;
    constexpr uint32_t token_buf_mask = static_cast<uint32_t>(1) << 31;
    auto token_buf = lexer.token_buf;

    uint64_t prev_is_escape;
    uint64_t prev_in_string;
    uint64_t prev_is_ws;
    uint32_t idx;

    if constexpr (read_once) {
        prev_is_escape = prev_in_string = 0;
        prev_is_ws = ~0ull;
        idx = 0;
    } else {
        prev_is_escape = storage.prev_is_escape;
        prev_in_string = storage.prev_in_string;
        prev_is_ws = storage.prev_is_ws;
        idx = storage.idx;
    }

    WJR_ASSERT_ASSUME_L2(first < last);

    uint32_t count = 0;
    char stk[64];

    bool loop;

    do {
        const char *ptr;
        const size_t diff = last - first;

        if (WJR_LIKELY(diff > 64)) {
            ptr = first;
            first += 64;
        } else {
            if (diff == 64) {
                ptr = first;
            } else {
                std::memcpy(stk, first, diff);
                std::memset(stk + diff, 0, 64 - diff);
                ptr = stk;
            }

            first = last;
            if constexpr (!read_once) {
                count |= token_buf_mask;
            }
        }

        uint64_t MASK[5] = {0, 0, 0, 0, 0};
        auto &[B, Q, S, W, UNUSED] = MASK;

        for (int i = 0; i < 8; i += 8) {
            MASK[code_table[static_cast<uint8_t>(ptr[i])]] |= 1ull << i;
            MASK[code_table[static_cast<uint8_t>(ptr[i + 1])]] |= 1ull << (i + 1);
            MASK[code_table[static_cast<uint8_t>(ptr[i + 2])]] |= 1ull << (i + 2);
            MASK[code_table[static_cast<uint8_t>(ptr[i + 3])]] |= 1ull << (i + 3);
            MASK[code_table[static_cast<uint8_t>(ptr[i + 4])]] |= 1ull << (i + 4);
            MASK[code_table[static_cast<uint8_t>(ptr[i + 5])]] |= 1ull << (i + 5);
            MASK[code_table[static_cast<uint8_t>(ptr[i + 6])]] |= 1ull << (i + 6);
            MASK[code_table[static_cast<uint8_t>(ptr[i + 7])]] |= 1ull << (i + 7);
        }

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
        S &= ~R;
        prev_in_string = static_cast<uint64_t>(static_cast<int64_t>(R) >> 63);

        S |= Q;
        const auto WS = S | W;
        const auto P = shld(WS, prev_is_ws, 1);
        prev_is_ws = WS;

        S |= (P ^ W) & ~R;
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

        if constexpr (!read_once) {
            loop = count <= lexer.token_buf_size;
        } else {
            loop = first != last;
        }

    } while (WJR_LIKELY(loop));

    if constexpr (!read_once) {
        storage.first = first;
        storage.prev_is_escape = prev_is_escape;
        storage.prev_in_string = prev_in_string;
        storage.prev_is_ws = prev_is_ws;
        storage.idx = idx;
    }

    if constexpr (!read_once) {
        return count & (token_buf_mask - 1);
    } else {
        return count;
    }
}

uint32_t read(dynamic_lexer_storage &storage, uint32_t *token_buf) noexcept {
    return read(__dynamic_lexer_struct{storage, token_buf});
}

uint32_t read(forward_lexer_storage &storage, uint32_t *token_buf,
              uint32_t token_buf_size) noexcept {
    return read(__forward_lexer_struct{storage, token_buf, token_buf_size});
}

} // namespace lexer_detail

#endif

} // namespace wjr::json
