#ifndef WJR_MATH_CMP_HPP__
#define WJR_MATH_CMP_HPP__

#include <wjr/simd/simd.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR int fallback_rcmp_n(const T *src0, const T *src1, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        auto a = src0[n - i - 1];
        auto b = src1[n - i - 1];

        if (a != b) {
            return a < b ? -1 : 1;
        }
    }

    return 0;
}

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_RCMP_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(RCMP_N)

template <size_t unroll, typename T>
WJR_NODISCARD WJR_INTRINSIC_INLINE int builtin_unroll_rcmp_n(const T *src0, const T *src1,
                                                             size_t n) {
    auto idx = unroll_call<unroll>([src0, src1, n](auto ic) -> std::optional<int> {
        constexpr auto idx = decltype(ic)::value;
        if (idx == n) {
            return 0;
        }

        auto a = src0[-idx - 1];
        auto b = src1[-idx - 1];

        if (a != b) {
            return a < b ? -1 : 1;
        }

        return std::nullopt;
    });

    if (idx.has_value()) {
        return idx.value();
    }

    return 0;
}

template <typename T>
WJR_NOINLINE int builtin_simd_rcmp_n(const T *src0, const T *src1, size_t n) {
    WJR_ASSUME(n % 2 == 0);
    WJR_ASSUME(n != 0);

#define WJR_REGISTER_RCMP_N_IMPL(index)                                                  \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 - (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 - (index)));                            \
                                                                                         \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
        auto mask = sse::movemask_epi8(r);                                               \
                                                                                         \
        if (mask != sse::mask()) {                                                       \
            if (!(mask & 0x00FF)) {                                                      \
                return src0[-1 - (index)] < src1[-1 - (index)] ? -1 : 1;                 \
            }                                                                            \
            return src0[-2 - (index)] < src1[-2 - (index)] ? -1 : 1;                     \
        }                                                                                \
    } while (0)

    if (n & 2) {
        WJR_REGISTER_RCMP_N_IMPL(0);

        src0 -= 2;
        src1 -= 2;
        n -= 2;
    }

    if (n & 4) {
        WJR_REGISTER_RCMP_N_IMPL(0);
        WJR_REGISTER_RCMP_N_IMPL(2);

        src0 -= 4;
        src1 -= 4;
        n -= 4;
    }

    if (!n) {
        return 0;
    }

    WJR_ASSUME(n % 8 == 0);

    size_t idx = 0;

    do {
        WJR_REGISTER_RCMP_N_IMPL(idx);
        WJR_REGISTER_RCMP_N_IMPL(idx + 2);
        WJR_REGISTER_RCMP_N_IMPL(idx + 4);
        WJR_REGISTER_RCMP_N_IMPL(idx + 6);

        idx += 8;
    } while (idx != n);

    return 0;

#undef WJR_REGISTER_RCMP_N_IMPL
}

template <typename T>
WJR_INTRINSIC_INLINE int builtin_rcmp_n(const T *src0, const T *src1, size_t n) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    if (WJR_UNLIKELY(n < 4)) {
        return builtin_unroll_rcmp_n<3>(src0, src1, n);
    }

    {
        auto idx = builtin_unroll_rcmp_n<1>(src0, src1, 1);

        if (idx != 0) {
            return idx;
        }

        --n;
    }

    src0 += n;
    src1 += n;

    {
        size_t k = n % 2;
        auto idx = builtin_unroll_rcmp_n<1>(src0, src1, k);

        if (idx != 0) {
            return idx;
        }

        src0 -= k;
        src1 -= k;
        n -= k;
    }

    return builtin_simd_rcmp_n(src0, src1, n);
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR int rcmp_n(const T *src0, const T *src1, size_t n) {
#if WJR_HAS_BUILTIN(RCMP_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_rcmp_n(src0, src1, n);
        }

        return builtin_rcmp_n(src0, src1, n);
    } else {
        return fallback_rcmp_n(src0, src1, n);
    }
#else
    return fallback_rcmp_n(src0, src1, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_CMP_HPP__