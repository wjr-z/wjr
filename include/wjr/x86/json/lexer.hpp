#ifndef WJR_X86_JSON_LEXER_HPP__
#define WJR_X86_JSON_LEXER_HPP__

#include <bitset>

#include <wjr/json/lexer-impl.hpp>
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
    // constexpr uint64_t E = 0x5555555555555555;
    // constexpr uint64_t O = ~E;

    // auto S = B & ~(B << 1);

    // auto ES = S & E;
    // auto EC = B + ES;
    // auto ECE = EC & ~B;
    // auto OD1 = ECE & O;

    // auto OS = S & O;
    // auto OC = B + OS;
    // auto OCE = OC & ~B;
    // auto OD2 = OCE & E;

    // return (OD1 | OD2);

    uint64_t maybe_escaped = B << 1;

    // To distinguish odd from even escape sequences, therefore, we turn on any *starting*
    // escapes that are on an odd byte. (We actually bring in all odd bits, for speed.)
    // - Odd runs of backslashes are 0000, and the code at the end ("n" in \n or \\n)
    // is 1.
    // - Odd runs of backslashes are 1111, and the code at the end ("n" in \n or \\n) is
    // 0.
    // - All other odd bytes are 1, and even bytes are 0.
    uint64_t maybe_escaped_and_odd_bits = maybe_escaped | 0xAAAAAAAAAAAAAAAAULL;
    uint64_t even_series_codes_and_odd_bits = maybe_escaped_and_odd_bits - B;

    // Now we flip all odd bytes back with xor. This:
    // - Makes odd runs of backslashes go from 0000 to 1010
    // - Makes even runs of backslashes go from 1111 to 1010
    // - Sets actually-escaped codes to 1 (the n in \n and \\n: \n = 11, \\n = 100)
    // - Resets all other bytes to 0
    return even_series_codes_and_odd_bits ^ 0xAAAAAAAAAAAAAAAAULL;
}

inline void builtin_read_token_mask(basic_lexer &lex) {
    constexpr bool is_avx = WJR_HAS_SIMD(AVX2);
    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto u8_width = simd_width / 8;
    constexpr auto u8_loop = 64 / u8_width;

    auto &first = lex.first;
    const auto &last = lex.last;
    auto &idx = lex.idx;
    unsigned diff = 64;
    unsigned __tmp_diff = std::distance(first, last);

    simd_int x[u8_loop];

    if (WJR_LIKELY(__tmp_diff >= 64)) {
        load_simd<simd>(first, x);
        first += 64;
    } else {
        diff = __tmp_diff;
        load_end_simd<simd>(first, diff, x);
        first = last;
    }

    uint64_t B = 0; // backslash
    uint64_t Q = 0; // quote
    uint64_t R;
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
        auto lo8_mask = simd::And(x[i], lh8_mask);
        auto hi8_mask = simd::And(simd::srli_epi16(x[i], 4), lh8_mask);

        auto shuf_lo8 = simd::shuffle_epi8(lo8_lookup, lo8_mask);
        auto shuf_hi8 = simd::shuffle_epi8(hi8_lookup, hi8_mask);

        auto result = simd::And(shuf_lo8, shuf_hi8);
        // comma : 1
        // colon : 2
        // brackets : 4
        // whitespace : 8, 16

        auto comma = equal<simd>(result, 1);
        auto colon = equal<simd>(result, 2);
        auto brackets = equal<simd>(result, 4);
        auto whitespace = simd::Or(equal<simd>(result, 8), equal<simd>(result, 16));

        S |= (uint64_t)simd::movemask_epi8(simd::Or(comma, simd::Or(colon, brackets)))
             << (i * u8_width);
        W |= (uint64_t)simd::movemask_epi8(whitespace) << (i * u8_width);
    }

    uint64_t codeB = calc_backslash(B & ~lex.prev_is_escape);
    lex.prev_is_escape = (codeB & B) >> 63;
    B = codeB ^ (B | lex.prev_is_escape);

    Q &= ~B;

    R = prefix_xor(Q) ^ lex.prev_in_string;
    lex.prev_in_string = (int64_t)R >> 63;

    S &= ~R;
    S |= Q;
    auto P = S | W;
    P <<= 1;
    P &= ~W & ~R;
    S |= P;
    S &= ~(Q & ~R);

    auto &token_last = lex.token_last;

    while (S) {
        *token_last++ = idx + ctz(S);
        S = clear_lowbit(S);
    }

    idx += diff;
}

} // namespace lexer_details

#endif

} // namespace wjr

#endif // WJR_X86_JSON_LEXER_HPP__