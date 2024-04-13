#ifndef WJR_X86_JSON_LEXER_HPP__
#define WJR_X86_JSON_LEXER_HPP__

#include <bitset>

#include <wjr/json/lexer-impl.hpp>
#include <wjr/math.hpp>
#include <wjr/math/ctz.hpp>
#include <wjr/math/prefix_xor.hpp>
#include <wjr/x86/simd/simd.hpp>

namespace wjr {

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_LEXER_READ_TOKEN_MASK WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(LEXER_READ_TOKEN_MASK)

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

template <typename simd>
WJR_NOINLINE void load_end_simd(const char *first, unsigned n,
                                typename simd::int_type *arr) {
    WJR_ASSUME(n < 64);
    char buf[64];
    ::memcpy(buf, first, n);
    ::memset(buf + n, 0, 64 - n);
    load_simd<simd>(buf, arr);
}

inline uint64_t calc_backslash(uint64_t B) {
    uint64_t maybe_escaped = B << 1;

    uint64_t maybe_escaped_and_odd_bits = maybe_escaped | 0xAAAAAAAAAAAAAAAAULL;
    uint64_t even_series_codes_and_odd_bits = maybe_escaped_and_odd_bits - B;

    return even_series_codes_and_odd_bits ^ 0xAAAAAAAAAAAAAAAAULL;
}

inline void builtin_read_token_mask(basic_lexer &lex) {
    constexpr bool is_avx = WJR_HAS_SIMD(AVX2);
    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;
    constexpr auto u8_loop = 64 / u8_width;

    const auto first = lex.first;
    const auto last = lex.last;
    const auto idx = lex.idx;
    const uint32_t diff = std::distance(first, last);

    simd_int x[u8_loop];

    if (WJR_LIKELY(diff >= 64)) {
        load_simd<simd>(first, x);
        lex.first += 64;
        lex.idx += 64;
    } else {
        load_end_simd<simd>(first, diff, x);
        lex.first = last;
    }

    uint64_t B = 0; // backslash
    uint64_t Q = 0; // quote
    uint64_t S = 0; // brackets, comma , colon
    uint64_t W = 0; // whitespace

    for (unsigned i = 0; i < u8_loop; ++i) {
        auto backslash = equal<simd>(x[i], '\\');
        B |= (uint64_t)simd::movemask_epi8(backslash) << (i * u8_width);
    }

    for (unsigned i = 0; i < u8_loop; ++i) {
        auto quote = equal<simd>(x[i], '\"');
        Q |= (uint64_t)simd::movemask_epi8(quote) << (i * u8_width);
    }

    for (unsigned i = 0; i < u8_loop; ++i) {
        auto shuf_lo8 = simd::shuffle_epi8(lo8_lookup, x[i]);
        auto shuf_hi8 = simd::shuffle_epi8(hi8_lookup, simd::And(simd::srli_epi16(x[i], 4), lh8_mask));

        auto result = simd::And(shuf_lo8, shuf_hi8);
        // comma : 1
        // colon : 2
        // brackets : 4
        // whitespace : 8, 16

        uint32_t stu = ~simd::movemask_epi8(
            simd::cmpeq_epi8(simd::And(result, simd::set1_epi8(7)), simd::zeros()));
        uint32_t wsp = ~simd::movemask_epi8(
            simd::cmpeq_epi8(simd::And(result, simd::set1_epi8(24)), simd::zeros()));

        S |= (uint64_t)(stu) << (i * u8_width);
        W |= (uint64_t)(wsp) << (i * u8_width);
    }

    uint64_t codeB = calc_backslash(B & ~lex.prev_is_escape);
    auto escape = (codeB & B) >> 63;
    B = codeB ^ (B | lex.prev_is_escape);
    Q &= ~B;
    lex.prev_is_escape = escape;

    uint64_t R = prefix_xor(Q) ^ lex.prev_in_string;
    S &= ~R;
    lex.prev_in_string = static_cast<uint64_t>(static_cast<int64_t>(R) >> 63);

    S |= Q;
    auto WS = S | W;
    auto P = shld(WS, lex.prev_is_ws, 1);
    lex.prev_is_ws = WS;

    S |= (P ^ W) & ~R;
    S &= ~(Q & ~R);

    auto token_last = lex.token_last;

    const auto num = popcount(S);
    const auto next = token_last + num;

    while (S) {
        for (int i = 0; i < 8; ++i) {
            token_last[i] = idx + ctz(S);
            S &= S - 1;
        }

        token_last += 8;
    }

    lex.token_last = next;
}

} // namespace lexer_details

#endif

} // namespace wjr

#endif // WJR_X86_JSON_LEXER_HPP__