#include <wjr/biginteger/detail/mul.hpp>
#include <wjr/memory/stack_allocator.hpp>

namespace wjr {
namespace {
inline size_t sqrmod_bnm1_next_size(size_t n) noexcept {
    size_t nh;

    if (n < WJR_SQRMOD_BNM1_THRESHOLD)
        return n;
    if (n < 4 * (WJR_SQRMOD_BNM1_THRESHOLD - 1) + 1)
        return (n + (2 - 1)) & (-2);
    if (n < 8 * (WJR_SQRMOD_BNM1_THRESHOLD - 1) + 1)
        return (n + (4 - 1)) & (-4);

    nh = (n + 1) >> 1;

    if (nh < WJR_SQR_FFT_MODF_THRESHOLD)
        return (n + (8 - 1)) & (-8);

    return 2 * fft_next_size(nh, fft_best_k(nh, 1));
}

inline size_t mulmod_bnm1_next_size(size_t n) noexcept {
    size_t nh;

    if (n < WJR_MULMOD_BNM1_THRESHOLD)
        return n;
    if (n < 4 * (WJR_MULMOD_BNM1_THRESHOLD - 1) + 1)
        return (n + (2 - 1)) & (-2);
    if (n < 8 * (WJR_MULMOD_BNM1_THRESHOLD - 1) + 1)
        return (n + (4 - 1)) & (-4);

    nh = (n + 1) >> 1;

    if (nh < WJR_MUL_FFT_MODF_THRESHOLD)
        return (n + (8 - 1)) & (-8);

    return 2 * fft_next_size(nh, fft_best_k(nh, 0));
}

inline size_t mulmod_bnm1_itch(size_t rn, size_t an, size_t bn) {
    size_t n, itch;
    n = rn >> 1;
    itch = rn + 4 + (an > n ? (bn > n ? rn : n) : 0);
    return itch;
}

inline size_t sqrmod_bnm1_itch(size_t rn, size_t an) {
    size_t n, itch;
    n = rn >> 1;
    itch = rn + 3 + (an > n ? an : 0);
    return itch;
}

} // namespace

/* Multiply {ap,an} by {bp,bn}, and put the result in {pp, an+bn} */
void nussbaumer_mul(uint64_t *pp, const uint64_t *ap, size_t an, const uint64_t *bp,
                    size_t bn) noexcept {
    size_t rn;
    uint64_t *tp;
    unique_stack_allocator stkal;

    WJR_ASSERT(an >= bn);
    WJR_ASSERT(bn > 0);

    if ((ap == bp) && (an == bn)) {
        rn = sqrmod_bnm1_next_size(2 * an);
        tp = stkal.template allocate<uint64_t>(sqrmod_bnm1_itch(rn, an));
        sqrmod_bnm1(pp, rn, ap, an, tp);
    } else {
        rn = mulmod_bnm1_next_size(an + bn);
        tp = stkal.template allocate<uint64_t>(mulmod_bnm1_itch(rn, an, bn));
        mulmod_bnm1(pp, rn, ap, an, bp, bn, tp);
    }
}
} // namespace wjr