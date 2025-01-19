#include <wjr/biginteger/detail/mul.hpp>
#include <wjr/memory/stack_allocator.hpp>

namespace wjr {
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