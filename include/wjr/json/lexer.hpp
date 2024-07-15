#ifndef WJR_JSON_LEXER_HPP__
#define WJR_JSON_LEXER_HPP__

#include <wjr/json/lexer-impl.hpp>
#include <wjr/span.hpp>

#if defined(WJR_X86)
#include <wjr/x86/json/lexer.hpp>
#endif

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

}

template <uint32_t token_buf_size>
uint32_t basic_lexer<token_buf_size>::read(uint32_t *token_buf) noexcept {
    WJR_UNREACHABLE();
    if (WJR_UNLIKELY(m_storage.first == m_storage.last)) {
        return 0;
    }

    using namespace lexer_detail;

    auto first = m_storage.first;
    const auto last = m_storage.last;
    uint64_t prev_is_escape;
    uint64_t prev_in_string;
    uint64_t prev_is_ws;
    uint32_t idx;

    if constexpr (__is_dynamic) {
        prev_is_escape = prev_in_string = prev_is_ws = idx = 0;
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
        const char *ptr;
        char stk[64];
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
            if constexpr (!__is_dynamic) {
                count |= token_buf_mask;
            }
        }

        uint64_t MASK[5] = {0, 0, 0, 0, 0};
        auto &[B, Q, S, W, UNUSED] = MASK;

        WJR_UNROLL(8)
        for (int i = 0; i < 64; ++i) {
            MASK[code_table[(uint8_t)ptr[i]]] |= 1ull << i;
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

#endif // WJR_JSON_LEXER_HPP__