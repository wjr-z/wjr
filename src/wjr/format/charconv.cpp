#include <wjr/format/charconv.hpp>

namespace wjr {

template from_chars_result<const uint8_t *>
__unsigned_from_chars_of_10_impl<uint8_t, char_converter_t>(
    const uint8_t *first, const uint8_t *last, uint8_t &value,
    char_converter_t conv) noexcept;

template from_chars_result<const uint8_t *>
__unsigned_from_chars_of_10_impl<uint16_t, char_converter_t>(
    const uint8_t *first, const uint8_t *last, uint16_t &value,
    char_converter_t conv) noexcept;

template from_chars_result<const uint8_t *>
__unsigned_from_chars_of_10_impl<uint32_t, char_converter_t>(
    const uint8_t *first, const uint8_t *last, uint32_t &value,
    char_converter_t conv) noexcept;

template from_chars_result<const uint8_t *>
__unsigned_from_chars_of_10_impl<uint64_t, char_converter_t>(
    const uint8_t *first, const uint8_t *last, uint64_t &value,
    char_converter_t conv) noexcept;

} // namespace wjr