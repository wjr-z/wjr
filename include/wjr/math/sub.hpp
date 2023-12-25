#ifndef WJR_MATH_SUB_HPP__
#define WJR_MATH_SUB_HPP__

#include <wjr/math/ctz.hpp>

namespace wjr {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_subc(T a, T b, U c_in, U &c_out) {
    T ret = a;
    U c = ret < b;
    ret -= b;
    c |= ret < c_in;
    ret -= c_in;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_subc)
#define WJR_HAS_BUILTIN_SUBC WJR_HAS_DEF
#endif

#if WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&                                   \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_SUBC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SUBC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T asm_subc_1(T a, T b, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ASM_SUBC(args)                                              \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_ASM_SUBC_I_CALLER)
#define WJR_REGISTER_BUILTIN_ASM_SUBC_I(suffix, type)                                    \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        unsigned char cf;                                                                \
        asm volatile("stc\n\t"                                                           \
                     "sbb{" #suffix " %2, %0| %0, %2}\n\t"                               \
                     "setb %b1"                                                          \
                     : "=r"(a), "=r"(cf)                                                 \
                     : "%r"(b), "0"(a)                                                   \
                     : "cc");                                                            \
        c_out = cf;                                                                      \
        return a;                                                                        \
    } else
#define WJR_REGISTER_BUILTIN_ASM_SUBC_I_CALLER(args) WJR_REGISTER_BUILTIN_ASM_SUBC_I args

    WJR_REGISTER_BUILTIN_ASM_SUBC(
        ((b, uint8_t), (w, uint16_t), (l, uint32_t), (q, uint64_t))) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ASM_SUBC_I_CALLER
#undef WJR_REGISTER_BUILTIN_ASM_SUBC_I
#undef WJR_REGISTER_BUILTIN_ASM_SUBC
}

#endif // WJR_HAS_BUILTIN(ASM_SUBC)

#if WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_subc(T a, T b, U c_in, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_SUBC(args)                                                  \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_SUBC_I_CALLER)
#define WJR_REGISTER_BUILTIN_SUBC_I(suffix, type)                                        \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        type __c_out;                                                                    \
        T ret = __builtin_subc##suffix(a, b, static_cast<type>(c_in), &__c_out);         \
        c_out = static_cast<U>(__c_out);                                                 \
        return ret;                                                                      \
    } else
#define WJR_REGISTER_BUILTIN_SUBC_I_CALLER(args) WJR_REGISTER_BUILTIN_SUBC_I args

    WJR_REGISTER_BUILTIN_SUBC(((b, unsigned char), (s, unsigned short), (, unsigned int),
                               (l, unsigned long), (ll, unsigned long long))) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_SUBC_I_CALLER
#undef WJR_REGISTER_BUILTIN_SUBC_I
#undef WJR_REGISTER_BUILTIN_SUBC
}

#endif // WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T subc(T a, T b, type_identity_t<U> c_in, U &c_out) {
    WJR_ASSERT_L(1, c_in == 0 || c_in == 1);
    WJR_ASSUME((c_in == 0 || c_in == 1));

#if !WJR_HAS_BUILTIN(SUBC) && !WJR_HAS_BUILTIN(ASM_SUBC)
    return fallback_subc(a, b, c_in, c_out);
#else
    constexpr auto is_constant_or_zero = [](const auto &x) -> int {
        return is_constant_p(x) ? (x == 0 ? 2 : 1) : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_subc(a, b, c_in, c_out);
    }

#if WJR_HAS_BUILTIN(ASM_SUBC)
    if (is_constant_p(c_in) && c_in == 1) {
        return asm_subc_1(a, b, c_out);
    }
#endif

#if WJR_HAS_BUILTIN(SUBC)
    return builtin_subc(a, b, c_in, c_out);
#else
    return fallback_subc(a, b, c_in, c_out);
#endif // WJR_HAS_BUILTIN(SUBC)

#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_subc_1_impl(T *dst, const T *src0, size_t n) {
    size_t idx = 1;

    // small size optimize
    if (is_constant_p(n <= 8) && n <= 8) {

        for (; idx < n; ++idx) {
            if (src0[idx] != static_cast<T>(0)) {
                break;
            }
            dst[idx] = static_cast<T>(-1);
        }

        return idx;
    }

    WJR_UNROLL(4)
    for (; idx < n && src0[idx] == static_cast<T>(0); ++idx)
        ;

    for (size_t i = 1; i < idx; ++i) {
        dst[i] = static_cast<T>(-1);
    }
    return idx;
}

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_SUBC_1_IMPL WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SUBC_1_IMPL)

template <typename T>
WJR_INLINE size_t builtin_subc_1_impl(T *dst, const T *src0, size_t n) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    static_assert(nd == 64, "Currently, only support uint64_t");

    if (n <= 8) {
        return fallback_subc_1_impl(dst, src0, n);
    }

    size_t idx = fallback_subc_1_impl(dst, src0, 5);

    if (WJR_LIKELY(idx != 5)) {
        return idx;
    }

    __m128i y = _mm_setzero_si128();

    do {
        __m128i x = _mm_loadu_si128((__m128i *)(src0 + idx));
        __m128i r = _mm_cmpeq_epi64(x, y);
        uint16_t z = _mm_movemask_epi8(r);
        if (z != static_cast<uint16_t>(-1)) {
            int k = ctz(z) / 8;
            if (!k) {
                return idx;
            }

            dst[idx] = static_cast<T>(-1);
            return idx + 1;
        }

        _mm_storeu_si128((__m128i *)(dst + idx), y);

        idx += 2;
    } while (idx + 2 <= n);

    if (idx != n && src0[idx] == 0ull) {
        dst[idx] = -1ull;
        ++idx;
    }

    return idx;
}

#endif

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U subc_1(T *dst, const T *src0, size_t n, type_identity_t<T> src1,
                                 U c_in) {
    WJR_ASSUME(n >= 1);
    WJR_ASSUME(dst != nullptr);
    WJR_ASSUME(src0 != nullptr);

    dst[0] = subc(src0[0], src1, c_in, c_in);

    if (c_in) {
        size_t idx = 0;

#if WJR_HAS_BUILTIN(SUBC_1_IMPL)
        if constexpr (std::numeric_limits<T>::digits != 64) {
            // not support builtin_subc_1_impl yet
            idx = fallback_subc_1_impl(dst, src0, n);
        } else {
            if (is_constant_evaluated()) {
                idx = fallback_subc_1_impl(dst, src0, n);
            } else {
                idx = builtin_subc_1_impl(dst, src0, n);
            }
        }
#else
        idx = fallback_subc_1_impl(dst, src0, 5);
        if (WJR_UNLIKELY(idx == 5)) {
            idx = 4 + fallback_subc_1_impl(dst + 4, src0 + 4, n - 4);
        }
#endif

        if (idx == n) {
            return static_cast<U>(1);
        }

        dst[idx] = src0[idx] - 1;

        dst += idx;
        src0 += idx;
        n -= idx;
    }

    if (src0 != dst) {
        for (size_t i = 1; i < n; ++i) {
            dst[i] = src0[i];
        }
    }

    return static_cast<U>(0);
}

template <size_t div, typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U subc_n_res(T *dst, const T *src0, const T *src1, size_t n,
                                     U c_in) {

    constexpr size_t mask = div - 1;

    n &= mask;

    if (WJR_UNLIKELY(n == 0)) {
        return c_in;
    }

    dst += n;
    src0 += n;
    src1 += n;

#define WJR_REGISTER_SUBC_RES_CASE_CALLER(idx)                                           \
    case idx: {                                                                          \
        dst[-idx] = subc(src0[-idx], src1[-idx], c_in, c_in);                            \
        WJR_FALLTHROUGH;                                                                 \
    }

#define WJR_REGISTER_SUBC_RES_SWITCH_CALLER(size)                                        \
    if constexpr (div == size) {                                                         \
        switch (n) {                                                                     \
            WJR_PP_TRANSFORM_PUT(                                                        \
                WJR_PP_QUEUE_TRANSFORM(                                                  \
                    WJR_PP_QUEUE_REVERSE((WJR_PP_IOTA(WJR_PP_DEC(size)))), WJR_PP_INC),  \
                WJR_REGISTER_SUBC_RES_CASE_CALLER)                                       \
        }                                                                                \
        return c_in;                                                                     \
    } else

    WJR_REGISTER_SUBC_RES_SWITCH_CALLER(2)
    WJR_REGISTER_SUBC_RES_SWITCH_CALLER(4) WJR_REGISTER_SUBC_RES_SWITCH_CALLER(8) {
        static_assert(div <= 8, "not support yet");
    }

#undef WJR_REGISTER_SUBC_RES_CASE_CALLER
#undef WJR_REGISTER_SUBC_RES_SWITCH_CALLER
}

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U fallback_subc_n(T *dst, const T *src0, const T *src1, size_t n,
                                          U c_in) {
    size_t m = n / 4;

    for (size_t i = 0; i < m; ++i) {
        dst[0] = subc(src0[0], src1[0], c_in, c_in);
        dst[1] = subc(src0[1], src1[1], c_in, c_in);
        dst[2] = subc(src0[2], src1[2], c_in, c_in);
        dst[3] = subc(src0[3], src1[3], c_in, c_in);

        dst += 4;
        src0 += 4;
        src1 += 4;
    }

    return subc_n_res<4>(dst, src0, src1, n, c_in);
}

#if WJR_HAS_BUILTIN(ASM_SUBC)
#define WJR_HAS_BUILTIN_ASM_SUBC_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SUBC_N)

template <typename T, typename U>
WJR_INLINE U asm_subc_n(T *dst, const T *src0, const T *src1, size_t n, U c_in) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    size_t m = n / 4;

#define WJR_REGISTER_ASM_SUBC_N(args)                                                    \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_ASM_SUBC_N_I_CALLER)

#define WJR_REGISTER_ASM_SUBC_N_I(type, suffix, offset0, offset1, offset2, offset3,      \
                                  offset4)                                               \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        if (WJR_LIKELY(m != 0)) {                                                        \
            T tmp;                                                                       \
            unsigned char cf = c_in;                                                     \
            asm volatile("dec %[cf]\n\t"                                                 \
                         "loop" #suffix "%=:\n\t"                                        \
                         "mov{" #suffix " " #offset0                                     \
                         "(%[src0]), %[tmp]| %[tmp], [%[src0] + " #offset0 "]}\n\t"      \
                         "sbb{" #suffix " " #offset0                                     \
                         "(%[src1]), %[tmp]| %[tmp], [%[src1] + " #offset0 "]}\n\t"      \
                         "mov{" #suffix " %[tmp], " #offset0                             \
                         "(%[dst])| [%[dst] + " #offset0 "], %[tmp]}\n\t"                \
                         "mov{" #suffix " " #offset1                                     \
                         "(%[src0]), %[tmp]| %[tmp], [%[src0] + " #offset1 "]}\n\t"      \
                         "sbb{" #suffix " " #offset1                                     \
                         "(%[src1]), %[tmp]| %[tmp], [%[src1] + " #offset1 "]}\n\t"      \
                         "mov{" #suffix " %[tmp], " #offset1                             \
                         "(%[dst])| [%[dst] + " #offset1 "], %[tmp]}\n\t"                \
                         "mov{" #suffix " " #offset2                                     \
                         "(%[src0]), %[tmp]| %[tmp], [%[src0] + " #offset2 "]}\n\t"      \
                         "sbb{" #suffix " " #offset2                                     \
                         "(%[src1]), %[tmp]| %[tmp], [%[src1] + " #offset2 "]}\n\t"      \
                         "mov{" #suffix " %[tmp], " #offset2                             \
                         "(%[dst])| [%[dst] + " #offset2 "], %[tmp]}\n\t"                \
                         "mov{" #suffix " " #offset3                                     \
                         "(%[src0]), %[tmp]| %[tmp], [%[src0] + " #offset3 "]}\n\t"      \
                         "sbb{" #suffix " " #offset3                                     \
                         "(%[src1]), %[tmp]| %[tmp], [%[src1] + " #offset3 "]}\n\t"      \
                         "mov{" #suffix " %[tmp], " #offset3                             \
                         "(%[dst])| [%[dst] + " #offset3 "], %[tmp]}\n\t"                \
                         "lea{" #suffix " " #offset4                                     \
                         "(%[src0]), %3| %3, [%[src0] + " #offset4 "]}\n\t"              \
                         "lea{" #suffix " " #offset4                                     \
                         "(%[src1]), %4| %4, [%[src1] + " #offset4 "]}\n\t"              \
                         "lea{" #suffix " " #offset4                                     \
                         "(%[dst]), %2| %2, [%[dst] + " #offset4 "]}\n\t"                \
                         "dec %[m]\n\t"                                                  \
                         "jne loop" #suffix "%=\n\t"                                     \
                         "setb %b0"                                                      \
                         : [cf] "+r"(cf), [tmp] "=r"(tmp), [dst] "+r"(dst),              \
                           [src0] "+r"(src0), [src1] "+r"(src1), [m] "+r"(m)             \
                         :                                                               \
                         : "cc", "memory");                                              \
            c_in = cf;                                                                   \
        }                                                                                \
    } else

#define WJR_REGISTER_ASM_SUBC_N_I_CALLER(args) WJR_REGISTER_ASM_SUBC_N_I args

    WJR_REGISTER_ASM_SUBC_N(((uint8_t, b, 0, 1, 2, 3, 4), (uint16_t, w, 0, 2, 4, 6, 8),
                             (uint32_t, l, 0, 4, 8, 12, 16),
                             (uint64_t, q, 0, 8, 16, 24, 32))) {
        static_assert(nd <= 64, "not support yet");
    }

#undef WJR_REGISTER_ASM_SUBC_N_I_CALLER
#undef WJR_REGISTER_ASM_SUBC_N_I
#undef WJR_REGISTER_ASM_SUBC_N

    return subc_n_res<4>(dst, src0, src1, n, c_in);
}

#endif

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U subc_n(T *dst, const T *src0, const T *src1, size_t n, U c_in) {
#if WJR_HAS_BUILTIN(ASM_SUBC_N)
    if (is_constant_evaluated()) {
        return fallback_subc_n(dst, src0, src1, n, c_in);
    }

    return asm_subc_n(dst, src0, src1, n, c_in);
#else
    return fallback_subc_n(dst, src0, src1, n, c_in);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SUB_HPP__