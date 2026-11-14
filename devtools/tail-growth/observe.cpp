#include <cstddef>
#include <cstdint>
extern "C" uint64_t observe_bytes(const char *bytes, size_t size) noexcept {
    return size + static_cast<unsigned char>(bytes[0]) +
           static_cast<unsigned char>(bytes[size - 1]);
}
