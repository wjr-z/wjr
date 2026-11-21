#include <wjr/math/shift.hpp>

extern "C" uint64_t left64(uint64_t *dst, const uint64_t *src, size_t n, unsigned int c) {
    return wjr::lshift_n(dst, src, n, c, 0);
}

extern "C" uint64_t right64(uint64_t *dst, const uint64_t *src, size_t n, unsigned int c) {
    return wjr::rshift_n(dst, src, n, c, 0);
}

extern "C" uint32_t leftc32(uint32_t *dst, const uint32_t *src, size_t n, unsigned int c) {
    return wjr::lshiftc_n(dst, src, n, c, 0);
}

extern "C" uint32_t rightc32(uint32_t *dst, const uint32_t *src, size_t n, unsigned int c) {
    return wjr::rshiftc_n(dst, src, n, c, 0);
}
