#include <wjr/math/set.hpp>

extern "C" void fill16(uint16_t *dst, uint16_t value, size_t n) { wjr::set_n(dst, value, n); }
extern "C" void fill32(uint32_t *dst, uint32_t value, size_t n) { wjr::set_n(dst, value, n); }
extern "C" void fill64(uint64_t *dst, uint64_t value, size_t n) { wjr::set_n(dst, value, n); }

extern "C" void fill16_short(uint16_t *dst, uint16_t value) { wjr::set_n(dst, value, 3); }
extern "C" void fill32_short(uint32_t *dst, uint32_t value) { wjr::set_n(dst, value, 5); }
extern "C" void fill64_short(uint64_t *dst, uint64_t value) { wjr::set_n(dst, value, 2); }
