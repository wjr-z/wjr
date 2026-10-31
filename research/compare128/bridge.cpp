#include <wjr/math/div.hpp>
#define JOIN_IMPL(a,b) a##b
#define JOIN(a,b) JOIN_IMPL(a,b)
struct Item { uint64_t lo, hi, d0, d1; };
struct Result { uint64_t quotient, lo, hi; };
extern "C" void JOIN(WJR_VARIANT,_divide)(const Item *items, Result *results, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        const auto &x = items[i];
        const wjr::div3by2_divider_noshift<uint64_t> divider(x.d0, x.d1, 0);
        wjr::uint128_t rem;
        const auto q = wjr::div128by128to64_noshift(rem, x.lo, x.hi, divider);
        results[i] = {q, rem.low, rem.high};
    }
}
extern "C" unsigned JOIN(WJR_VARIANT,_compare)(uint64_t lo0, uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    return unsigned(wjr::_less_128(lo0,hi0,lo1,hi1)) | (unsigned(wjr::_less_equal_128(lo0,hi0,lo1,hi1)) << 1);
}
