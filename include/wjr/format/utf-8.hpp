#ifndef WJR_FORMAT_UTF_8_HPP__
#define WJR_FORMAT_UTF_8_HPP__

#include <wjr/memory/details.hpp>
#include <wjr/span.hpp>
#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
#include <wjr/x86/format/utf-8.hpp>
#endif

namespace wjr::utf8 {

WJR_PURE WJR_INTRINSIC_INLINE bool is_ascii_1(uint8_t *ptr) { return *ptr < 0x80; }

WJR_PURE WJR_INTRINSIC_INLINE bool is_ascii_2(uint8_t *ptr) {
    return read_memory<uint16_t>(ptr) < 0x8080;
}

WJR_PURE WJR_INTRINSIC_INLINE bool is_ascii_4(uint8_t *ptr) {
    return read_memory<uint32_t>(ptr) < 0x80808080;
}

WJR_PURE WJR_INTRINSIC_INLINE bool is_ascii_8(uint8_t *ptr) {
    return read_memory<uint64_t>(ptr) < 0x8080808080808080;
}

WJR_PURE WJR_INTRINSIC_INLINE bool fallback_is_ascii_16(uint8_t *ptr) {
    return is_ascii_8(ptr) && is_ascii_8(ptr + 8);
}

WJR_PURE WJR_INTRINSIC_INLINE bool is_ascii_16(uint8_t *ptr) {
#if WJR_HAS_BUILTIN(IS_ASCII_16)
    if (is_constant_evaluated()) {
        return fallback_is_ascii_16(ptr);
    }

    return builtin_is_ascii_16(ptr);
#else
    return fallback_is_ascii_16(ptr);
#endif
}

WJR_PURE WJR_INTRINSIC_INLINE bool fallback_is_ascii_32(uint8_t *ptr) {
    return is_ascii_16(ptr) && is_ascii_16(ptr + 16);
}

WJR_PURE WJR_INTRINSIC_INLINE bool is_ascii_32(uint8_t *ptr) {
#if WJR_HAS_BUILTIN(IS_ASCII_32)
    if (is_constant_evaluated()) {
        return fallback_is_ascii_32(ptr);
    }

    return builtin_is_ascii_32(ptr);
#else
    return fallback_is_ascii_32(ptr);
#endif
}

WJR_PURE WJR_INTRINSIC_INLINE bool is_bom(uint8_t *ptr) {
    return read_memory<uint32_t>(ptr) == 0xEFBBBF;
}

} // namespace wjr::utf8

#endif // WJR_FORMAT_UTF_8_HPP__