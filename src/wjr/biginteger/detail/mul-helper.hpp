#include <wjr/biginteger/detail/mul.hpp>

namespace wjr {

/// helper macros

#define WJR_SUBMUL_1_S(A, n, B, m, cfA, cfB, ml, ret)                                              \
    do {                                                                                           \
        WJR_ASSERT_ASSUME(n >= m);                                                                 \
                                                                                                   \
        uint64_t __cf = submul_1(A, B, m, ml) + cfB * ml;                                          \
        if (n != m) {                                                                              \
            __cf = subc_1(A + m, A + m, n - m, __cf);                                              \
        }                                                                                          \
                                                                                                   \
        ret = cfA - __cf;                                                                          \
    } while (0)

#define WJR_ADDLSH_S(dst, A, n, B, m, cfA, cfB, cl, ret)                                           \
    do {                                                                                           \
        WJR_ASSERT_ASSUME(n >= m);                                                                 \
                                                                                                   \
        uint64_t __cf = addlsh_n(dst, A, B, m, cl) + (cfB << (cl));                                \
        if (n != m) {                                                                              \
            __cf = addc_1(dst + m, A + m, n - m, __cf);                                            \
        }                                                                                          \
                                                                                                   \
        ret = cfA + __cf;                                                                          \
    } while (0)

#define WJR_ADDLSH_NS(dst, A, m, B, n, cfA, cfB, cl, ret)                                          \
    do {                                                                                           \
        WJR_ASSERT_ASSUME(n >= m);                                                                 \
                                                                                                   \
        uint64_t __cf = addlsh_n(dst, A, B, m, cl) + cfA;                                          \
        if (n != m) {                                                                              \
            const uint64_t __tmp = lshift_n(dst + m, B + m, n - m, cl);                            \
            __cf = __tmp + addc_1(dst + m, dst + m, n - m, __cf);                                  \
        }                                                                                          \
                                                                                                   \
        ret = (cfB << (cl)) + __cf;                                                                \
    } while (0)

/// helper functions

template <uint64_t maxn = UINT64_MAX>
WJR_INTRINSIC_INLINE uint64_t try_addmul_1(WJR_MAYBE_UNUSED uint64_t *dst,
                                           WJR_MAYBE_UNUSED const uint64_t *src, size_t n,
                                           uint64_t ml) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    WJR_ASSERT_ASSUME(ml <= maxn);

    if constexpr (maxn == 0) {
        return 0;
    } else {
        if constexpr (maxn <= 3) {
            if (ml == 0) {
                return 0;
            }

            if constexpr (maxn == 1) {
                return addc_n(dst, dst, src, n);
            } else {
                if (ml == 1) {
                    return addc_n(dst, dst, src, n);
                }

                if constexpr (maxn == 2) {
                    return addlsh_n(dst, dst, src, n, 1);
                } else {
                    if (ml == 2) {
                        return addlsh_n(dst, dst, src, n, 1);
                    }

                    return addmul_1(dst, src, n, ml);
                }
            }
        } else {
            if (WJR_UNLIKELY(ml <= 2)) {
                switch (ml) {
                case 0: {
                    return 0;
                }
                case 1: {
                    return addc_n(dst, dst, src, n);
                }
                default: {
                    return addlsh_n(dst, dst, src, n, 1);
                }
                }
            }

            return addmul_1(dst, src, n, ml);
        }
    }
}

WJR_INTRINSIC_INLINE void __mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                  const uint64_t *src1, size_t n,
                                  WJR_MAYBE_UNUSED uint64_t *stk) noexcept {
    mul_n(dst, src0, src1, n);
}

template <uint64_t m0 = UINT64_MAX, uint64_t m1 = UINT64_MAX>
WJR_INTRINSIC_INLINE void __mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                  const uint64_t *src1, size_t n, uint64_t *stk, uint64_t &c_out,
                                  uint64_t cf0, uint64_t cf1) noexcept {
    WJR_ASSERT_ASSUME(cf0 <= m0);
    WJR_ASSERT_ASSUME(cf1 <= m1);

    __mul_n(dst, src0, src1, n, stk);

    if constexpr (m0 == 0 || m1 == 0) {
        c_out = 0;
    } else if constexpr (m0 == 1 || m1 == 1) {
        if constexpr (m0 == 1 && m1 == 1) {
            c_out = cf0 && cf1;
        } else if constexpr (m0 == 1) {
            c_out = cf0 ? cf1 : 0;
        } else {
            c_out = cf1 ? cf0 : 0;
        }
    } else {
        c_out = cf0 * cf1;
    }

    c_out += try_addmul_1<m0>(dst + n, src1, n, cf0);
    c_out += try_addmul_1<m1>(dst + n, src0, n, cf1);
}

} // namespace wjr
