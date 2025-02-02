#ifndef WJR_FORMAT_UTF8_HPP__
#define WJR_FORMAT_UTF8_HPP__

#include <array>

#include <wjr/optional.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/format/utf8/utf8.hpp>
#endif

namespace wjr::utf8 {

namespace detail {
inline constexpr std::array<uint32_t, 886> digit_to_val32 = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
    0x8,        0x9,        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xa,        0xb,        0xc,        0xd,        0xe,        0xf,        0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xa,        0xb,        0xc,        0xd,        0xe,        0xf,        0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0x0,        0x10,       0x20,       0x30,       0x40,       0x50,
    0x60,       0x70,       0x80,       0x90,       0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa0,       0xb0,       0xc0,       0xd0,       0xe0,
    0xf0,       0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa0,       0xb0,       0xc0,       0xd0,       0xe0,
    0xf0,       0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0,        0x100,      0x200,      0x300,
    0x400,      0x500,      0x600,      0x700,      0x800,      0x900,      0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa00,      0xb00,      0xc00,
    0xd00,      0xe00,      0xf00,      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa00,      0xb00,      0xc00,
    0xd00,      0xe00,      0xf00,      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0,        0x1000,
    0x2000,     0x3000,     0x4000,     0x5000,     0x6000,     0x7000,     0x8000,     0x9000,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa000,
    0xb000,     0xc000,     0xd000,     0xe000,     0xf000,     0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa000,
    0xb000,     0xc000,     0xd000,     0xe000,     0xf000,     0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

WJR_PURE WJR_INTRINSIC_CONSTEXPR uint32_t hex_to_u32_unchecked(const char *src) noexcept {
    const uint32_t v1 = digit_to_val32[630u + to_u8(src[0])];
    const uint32_t v2 = digit_to_val32[420u + to_u8(src[1])];
    const uint32_t v3 = digit_to_val32[210u + to_u8(src[2])];
    const uint32_t v4 = digit_to_val32[0u + to_u8(src[3])];
    return v1 | v2 | v3 | v4;
}

WJR_INTRINSIC_CONSTEXPR int unicode_codepoint_to_utf8(char *dst, uint32_t code_point) noexcept {
    if (code_point <= 0x7F) {
        dst[0] = uint8_t(code_point);
        return 1; // ascii
    }

    if (code_point <= 0x7FF) {
        dst[0] = uint8_t((code_point >> 6) + 192);
        dst[1] = uint8_t((code_point & 63) + 128);
        return 2; // universal plane
                  //  Surrogates are treated elsewhere...
                  //} //else if (0xd800 <= code_point && code_point <= 0xdfff) {
                  //  return 0; // surrogates // could put assert here
    } else if (code_point <= 0xFFFF) {
        dst[0] = uint8_t((code_point >> 12) + 224);
        dst[1] = uint8_t(((code_point >> 6) & 63) + 128);
        dst[2] = uint8_t((code_point & 63) + 128);
        return 3;
    } else if (code_point <= 0x10FFFF) { // if you know you have a valid code
                                         // point, this is not needed
        dst[0] = uint8_t((code_point >> 18) + 240);
        dst[1] = uint8_t(((code_point >> 12) & 63) + 128);
        dst[2] = uint8_t(((code_point >> 6) & 63) + 128);
        dst[3] = uint8_t((code_point & 63) + 128);
        return 4;
    }

    // will return 0 when the code point was too large.
    return 0; // bad r
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR bool check_unicode_codepoint(uint32_t code_point) noexcept {
    return code_point <= 0x10FFFF;
}

inline constexpr std::array<uint8_t, 256> escape_table = {
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, // 0x0.
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0x22, 0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0x2f, 0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,

    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, // 0x4.
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0x5c, 0, 0,    0, // 0x5.
    0, 0, 0x08, 0, 0,    0, 0x0c, 0,    0, 0, 0, 0, 0,    0, 0x0a, 0, // 0x6.
    0, 0, 0x0d, 0, 0x09, 0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, // 0x7.

    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0,

    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0,
};

} // namespace detail

/**
 * @brief Check if starts with `\u'
 */
WJR_PURE WJR_INTRINSIC_INLINE bool check_unicode_escape(const char *first) noexcept {
    constexpr uint16_t escape = (static_cast<uint8_t>('\\') << 8) | static_cast<uint8_t>('u');
    return read_memory<uint16_t>(first) == escape;
}

WJR_PURE WJR_INTRINSIC_INLINE compressed_optional<const char *>
check_unicode_codepoint(const char *first) noexcept {
    uint32_t code_point = detail::hex_to_u32_unchecked(first);
    first += 4;

    if (WJR_UNLIKELY(code_point >= 0xd800 && code_point < 0xdc00)) {
        if (WJR_UNLIKELY(!check_unicode_escape(first))) {
            return nullopt;
        }

        const uint32_t code_point_2 = detail::hex_to_u32_unchecked(first + 2);
        const uint32_t low_bit = code_point_2 - 0xdc00;
        if (WJR_UNLIKELY(low_bit >> 10)) {
            return nullopt;
        }

        code_point = (((code_point - 0xd800) << 10) | low_bit) + 0x10000;
        first += 6;
    } else if (code_point >= 0xdc00 && code_point <= 0xdfff) {
        return nullopt;
    }

    if (const bool success = detail::check_unicode_codepoint(code_point); WJR_UNLIKELY(!success)) {
        return nullopt;
    }

    return first;
}

WJR_PURE WJR_INTRINSIC_INLINE compressed_optional<const char *>
check_unicode_codepoint(const char *first, const char *last) noexcept {
    if (WJR_UNLIKELY(last - first < 4)) {
        return nullopt;
    }

    uint32_t code_point = detail::hex_to_u32_unchecked(first);
    first += 4;

    if (WJR_UNLIKELY(code_point >= 0xd800 && code_point < 0xdc00)) {
        if (WJR_UNLIKELY(last - first < 6)) {
            return nullopt;
        }

        if (WJR_UNLIKELY(!check_unicode_escape(first))) {
            return nullopt;
        }

        const uint32_t code_point_2 = detail::hex_to_u32_unchecked(first + 2);
        const uint32_t low_bit = code_point_2 - 0xdc00;
        if (low_bit >> 10) {
            return nullopt;
        }

        code_point = (((code_point - 0xd800) << 10) | low_bit) + 0x10000;
        first += 6;
    } else if (code_point >= 0xdc00 && code_point <= 0xdfff) {
        return nullopt;
    }

    if (const bool success = detail::check_unicode_codepoint(code_point); WJR_UNLIKELY(!success)) {
        return nullopt;
    }

    return first;
}

WJR_INTRINSIC_INLINE compressed_optional<const char *>
unicode_codepoint_to_utf8(char *&dst, const char *first) noexcept {
    uint32_t code_point = detail::hex_to_u32_unchecked(first);
    first += 4;

    if (WJR_UNLIKELY(code_point >= 0xd800 && code_point < 0xdc00)) {
        constexpr uint16_t escaped = (static_cast<uint8_t>('\\') << 8) | static_cast<uint8_t>('u');

        if (WJR_UNLIKELY(read_memory<uint16_t>(first) != escaped)) {
            return nullopt;
        }

        const uint32_t code_point_2 = detail::hex_to_u32_unchecked(first + 2);
        const uint32_t low_bit = code_point_2 - 0xdc00;
        if (low_bit >> 10) {
            return nullopt;
        }

        code_point = (((code_point - 0xd800) << 10) | low_bit) + 0x10000;
        first += 6;
    } else if (code_point >= 0xdc00 && code_point <= 0xdfff) {
        return nullopt;
    }

    const int offset = detail::unicode_codepoint_to_utf8(dst, code_point);
    dst += offset;

    if (WJR_UNLIKELY(offset == 0)) {
        return nullopt;
    }

    return first;
}

WJR_INTRINSIC_INLINE compressed_optional<const char *>
unicode_codepoint_to_utf8(char *&dst, const char *first, const char *last) noexcept {
    if (WJR_UNLIKELY(last - first < 4)) {
        return nullopt;
    }

    uint32_t code_point = detail::hex_to_u32_unchecked(first);
    first += 4;

    if (WJR_UNLIKELY(code_point >= 0xd800 && code_point < 0xdc00)) {
        if (WJR_UNLIKELY(last - first < 6)) {
            return nullopt;
        }

        constexpr uint16_t escaped = (static_cast<uint8_t>('\\') << 8) | static_cast<uint8_t>('u');

        if (WJR_UNLIKELY(read_memory<uint16_t>(first) != escaped)) {
            return nullopt;
        }

        const uint32_t code_point_2 = detail::hex_to_u32_unchecked(first + 2);
        const uint32_t low_bit = code_point_2 - 0xdc00;
        if (low_bit >> 10) {
            return nullopt;
        }

        code_point = (((code_point - 0xd800) << 10) | low_bit) + 0x10000;
        first += 6;
    } else if (code_point >= 0xdc00 && code_point <= 0xdfff) {
        return nullopt;
    }

    const int offset = detail::unicode_codepoint_to_utf8(dst, code_point);
    dst += offset;

    if (WJR_UNLIKELY(offset == 0)) {
        return nullopt;
    }

    return first;
}

inline optional<void> fallback_check_unicode(const char *first, const char *last) noexcept {
    if (WJR_UNLIKELY(first == last)) {
        return voidopt;
    }

    do {
        uint8_t ch = *first++;

        if (WJR_UNLIKELY(ch == '\\')) {
            WJR_ASSERT(first != last);
            ch = *first++;

            if (WJR_UNLIKELY(ch == 'u')) {
                WJR_OPTIONAL_SET(first, check_unicode_codepoint(first, last));
            } else {
                const uint8_t code = detail::escape_table[ch];

                if (WJR_UNLIKELY(code == 0)) {
                    return nullopt;
                }
            }
        }
    } while (first != last);

    return voidopt;
}

#if WJR_HAS_BUILTIN(UTF8_CHECK_UNICODE)
extern WJR_ALL_NONNULL optional<void> builtin_check_unicode(const char *first,
                                                            const char *last) noexcept;
#endif

WJR_ALL_NONNULL inline optional<void> check_unicode(const char *first, const char *last) noexcept {
#if WJR_HAS_BUILTIN(UTF8_CHECK_UNICODE)
    return builtin_check_unicode(first, last);
#else
    return fallback_check_unicode(first, last);
#endif
}

inline compressed_optional<char *> fallback_unicode_to_utf8(char *dst, const char *first,
                                                            const char *last) noexcept {
    if (WJR_UNLIKELY(first == last)) {
        return dst;
    }

    do {
        uint8_t ch = to_u8(*first++);

        if (WJR_UNLIKELY(ch == '\\')) {
            WJR_ASSERT(first != last);
            ch = *first++;

            if (WJR_UNLIKELY(ch == 'u')) {
                WJR_OPTIONAL_SET(first, unicode_codepoint_to_utf8(dst, first, last));
            } else {
                const uint8_t code = detail::escape_table[ch];

                if (WJR_UNLIKELY(code == 0)) {
                    return nullopt;
                }

                *dst++ = code;
            }
        } else {
            *dst++ = ch;
        }
    } while (first != last);

    return dst;
}

#if WJR_HAS_BUILTIN(UTF8_UNICODE_TO_UTF8)
extern WJR_ALL_NONNULL compressed_optional<char *>
builtin_unicode_to_utf8(char *dst, const char *first, const char *last) noexcept;
#endif

WJR_ALL_NONNULL inline compressed_optional<char *> unicode_to_utf8(char *dst, const char *first,
                                                                   const char *last) noexcept {
#if WJR_HAS_BUILTIN(UTF8_UNICODE_TO_UTF8)
    return builtin_unicode_to_utf8(dst, first, last);
#else
    return fallback_unicode_to_utf8(dst, first, last);
#endif
}

} // namespace wjr::utf8

#endif // WJR_FORMAT_UTF8_HPP__