#include <wjr/json/detail/utf8.hpp>

#include <cstdint>
#include <cstring>

namespace wjr::json::detail {

std::size_t find_invalid_utf8(std::string_view input) noexcept {
    std::size_t offset = 0;
    while (offset != input.size()) {
        while (input.size() - offset >= sizeof(uint64_t)) {
            uint64_t bytes;
            std::memcpy(&bytes, input.data() + offset, sizeof(bytes));
            if ((bytes & UINT64_C(0x8080808080808080)) != 0) {
                break;
            }
            offset += sizeof(bytes);
        }
        if (offset == input.size()) {
            break;
        }

        const auto first = static_cast<unsigned char>(input[offset]);
        if (first < 0x80) {
            ++offset;
            continue;
        }

        unsigned int length;
        unsigned char second_min = 0x80;
        unsigned char second_max = 0xbf;
        if (first >= 0xc2 && first <= 0xdf) {
            length = 2;
        } else if (first >= 0xe0 && first <= 0xef) {
            length = 3;
            if (first == 0xe0) {
                second_min = 0xa0;
            } else if (first == 0xed) {
                second_max = 0x9f;
            }
        } else if (first >= 0xf0 && first <= 0xf4) {
            length = 4;
            if (first == 0xf0) {
                second_min = 0x90;
            } else if (first == 0xf4) {
                second_max = 0x8f;
            }
        } else {
            return offset;
        }

        if (input.size() - offset < length) {
            return offset;
        }
        const auto second = static_cast<unsigned char>(input[offset + 1]);
        if (second < second_min || second > second_max) {
            return offset;
        }
        for (unsigned int index = 2; index < length; ++index) {
            const auto next = static_cast<unsigned char>(input[offset + index]);
            if ((next & 0xc0) != 0x80) {
                return offset;
            }
        }
        offset += length;
    }
    return std::string_view::npos;
}

} // namespace wjr::json::detail
