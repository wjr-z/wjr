#ifndef WJR_MATH_REPLACE_HPP__
#define WJR_MATH_REPLACE_HPP__

#include <cstring>

#include <wjr/math/broadcast.hpp>
#include <wjr/math/find.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_replace_val_nomemset(T *dst, const T *src,
                                                             size_t n, T from, T to) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != from) {
            break;
        }

        dst[idx] = to;
    }

    return idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_replace_val_memset(T *dst, const T *src, size_t n,
                                                           T from, uint8_t to) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != from) {
            break;
        }
    }

    memset(dst, to, sizeof(T) * idx);
    return idx;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_replace_val(T *dst, const T *src, size_t n,
                                                    T from, T to) {
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(n <= 8) && n <= 8)) {
        return fallback_replace_val_nomemset(dst, src, n, from, to);
    }

    uint8_t set = to;

    if (broadcast<uint8_t, T>(set) == to) {
        return fallback_replace_val_memset(dst, src, n, from, set);
    }

    return fallback_replace_val_nomemset(dst, src, n, from, to);
}

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_REPLACE_VAL WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(REPLACE_VAL)

template <size_t unroll, typename T>
WJR_NODISCARD WJR_INTRINSIC_INLINE size_t builtin_unroll_replace_val(T *dst, const T *src,
                                                                     size_t n, T from,
                                                                     T to) {
    auto idx =
        unroll_call<unroll>([dst, src, n, from, to](auto ic) -> std::optional<size_t> {
            constexpr size_t idx = decltype(ic)::value;

            if (idx == n || WJR_LIKELY(src[idx] != from)) {
                return idx;
            }

            dst[idx] = to;
            return std::nullopt;
        });

    if (idx.has_value()) {
        return idx.value();
    }

    WJR_ASSUME(n == unroll);
    return n;
}

template <typename T>
WJR_INLINE size_t builtin_simd_replace_val(T *dst, const T *src, const size_t n, T from,
                                           T to) {
    WJR_ASSUME(n >= 2);

    size_t m = n;

    {
        size_t k = m % 2;
        size_t idx = builtin_unroll_replace_val<1>(dst, src, k, from, to);
        if (idx != k) {
            return idx;
        }

        dst += k;
        src += k;
        m -= k;
    }

    WJR_ASSUME(m % 2 == 0);
    WJR_ASSUME(m != 0);

    __m128i y = sse::set1_epi64(from);
    __m128i z = sse::set1_epi64(to);

#define WJR_REGISTER_REPLACE_VAL_IMPL(index)                                             \
    do {                                                                                 \
        __m128i x = sse::loadu((__m128i *)(src + (index)));                              \
        __m128i r = sse::cmpeq_epi64(x, y);                                              \
        auto mask = sse::movemask_epi8(r);                                               \
        if (mask != sse::mask()) {                                                       \
            if (!(mask & 0xFF)) {                                                        \
                return (n - m) + (index);                                                \
            }                                                                            \
            dst[(index)] = to;                                                           \
            return (n - m) + (index) + 1;                                                \
        }                                                                                \
        sse::storeu((__m128i *)(dst + (index)), z);                                      \
    } while (0)

    if (m & 2) {
        WJR_REGISTER_REPLACE_VAL_IMPL(0);

        dst += 2;
        src += 2;
        m -= 2;
    }

    if (m & 4) {
        WJR_REGISTER_REPLACE_VAL_IMPL(0);
        WJR_REGISTER_REPLACE_VAL_IMPL(2);

        dst += 4;
        src += 4;
        m -= 4;
    }

    if (!m) {
        return n;
    }

    WJR_ASSUME(m % 8 == 0);

    size_t idx = 0;

    do {
        WJR_REGISTER_REPLACE_VAL_IMPL(idx);
        WJR_REGISTER_REPLACE_VAL_IMPL(idx + 2);
        WJR_REGISTER_REPLACE_VAL_IMPL(idx + 4);
        WJR_REGISTER_REPLACE_VAL_IMPL(idx + 6);

        idx += 8;
    } while (idx != m);

    return n;

#undef WJR_REGISTER_REPLACE_VAL_IMPL
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_replace_val(T *dst, const T *src, size_t n, T from,
                                                T to) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    if (WJR_UNLIKELY(n < 4)) {
        return builtin_unroll_replace_val<3>(dst, src, n, from, to);
    }

    return builtin_simd_replace_val(dst, src, n, from, to);
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t replace_val(T *dst, const T *src, size_t n,
                                           type_identity_t<T> from,
                                           type_identity_t<T> to) {
    if (from == to && src == dst) {
        return find_not(src, n, from);
    }

#if WJR_HAS_BUILTIN(REPLACE_VAL)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_replace_val(dst, src, n, from, to);
        }

        return builtin_replace_val(dst, src, n, from, to);
    } else {
        return fallback_replace_val(dst, src, n, from, to);
    }
#else
    return fallback_replace_val(dst, src, n, from, to);
#endif
}

} // namespace wjr

#endif // WJR_MATH_REPLACE_HPP__