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
#define WJR_HAS_BUILTIN_SSE4_1_REPLACE_VAL WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(REPLACE_VAL)

template <size_t unroll, typename T>
WJR_INTRINSIC_INLINE size_t builtin_unroll_replace_val(T *dst, const T *src, size_t n,
                                                       T from, T to) {
    auto fn = [dst, src, n, from, to](auto ic) -> std::optional<size_t> {
        constexpr size_t idx = decltype(ic)::value;

        if (idx == n || WJR_LIKELY(src[idx] != from)) {
            return idx;
        }

        dst[idx] = to;
        return std::nullopt;
    };

    auto idx = unroll_call<unroll>(fn);
    if (idx.has_value()) {
        return idx.value();
    }

    WJR_ASSUME(n == unroll);
    return n;
}

template <typename simd, typename T>
WJR_INLINE size_t builtin_simd_replace_val(T *dst, const T *src, const size_t n, T from,
                                           T to) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;
    using simd_int = typename simd::int_type;

    WJR_ASSUME(n >= simd_width);

    size_t m = n;

    {
        size_t k = m % simd_width;
        size_t idx = builtin_unroll_replace_val<simd_width - 1>(dst, src, k, from, to);
        if (idx != k) {
            return idx;
        }

        dst += k;
        src += k;
        m -= k;
    }

    WJR_ASSUME(m % simd_width == 0);
    WJR_ASSUME(m != 0);

    simd_int y = simd::set1_epi64(from);
    simd_int z = simd::set1_epi64(to);

#define WJR_REGISTER_REPLACE_VAL_IMPL(index)                                             \
    do {                                                                                 \
        simd_int x = simd::loadu((simd_int *)(src + (index)));                           \
        simd_int r = simd::cmpeq_epi64(x, y);                                            \
        auto mask = simd::movemask_epi8(r);                                              \
        if (mask != simd::mask()) {                                                      \
            if (!(mask & 0xFF)) {                                                        \
                return (n - m) + (index);                                                \
            }                                                                            \
            dst[(index)] = to;                                                           \
            if constexpr (std::is_same_v<simd, sse>) {                                   \
                return (n - m) + (index) + 1;                                            \
            } else {                                                                     \
                if (!(mask & 0xFF00)) {                                                  \
                    return (n - m) + (index) + 1;                                        \
                }                                                                        \
                dst[(index) + 1] = to;                                                   \
                if (!(mask & 0xFF00'00)) {                                               \
                    return (n - m) + (index) + 2;                                        \
                }                                                                        \
                dst[(index) + 2] = to;                                                   \
                return (n - m) + (index) + 3;                                            \
            }                                                                            \
        }                                                                                \
        simd::storeu((simd_int *)(dst + (index)), z);                                    \
    } while (0)

    {
        WJR_REGISTER_REPLACE_VAL_IMPL(0);

        dst += simd_width;
        src += simd_width;
        m -= simd_width;
    }

    if (!m) {
        return n;
    }

    if ((m / simd_width) & 1) {
        WJR_REGISTER_REPLACE_VAL_IMPL(0);

        dst += simd_width;
        src += simd_width;
        m -= simd_width;
    }

    if ((m / (simd_width * 2)) & 1) {
        WJR_REGISTER_REPLACE_VAL_IMPL(0);
        WJR_REGISTER_REPLACE_VAL_IMPL(simd_width);

        dst += simd_width * 2;
        src += simd_width * 2;
        m -= simd_width * 2;
    }

    if (!m) {
        return n;
    }

    WJR_ASSUME(m % (simd_width * 4) == 0);

    WJR_REGISTER_REPLACE_VAL_IMPL(0);
    WJR_REGISTER_REPLACE_VAL_IMPL(simd_width);
    WJR_REGISTER_REPLACE_VAL_IMPL(simd_width * 2);
    WJR_REGISTER_REPLACE_VAL_IMPL(simd_width * 3);

    m -= simd_width * 4;

    if (WJR_UNLIKELY(!m)) {
        return n;
    }

    dst += simd_width * 4;
    src += simd_width * 4;

    size_t idx = 0;

    do {
        WJR_REGISTER_REPLACE_VAL_IMPL(idx);
        WJR_REGISTER_REPLACE_VAL_IMPL(idx + simd_width);
        WJR_REGISTER_REPLACE_VAL_IMPL(idx + simd_width * 2);
        WJR_REGISTER_REPLACE_VAL_IMPL(idx + simd_width * 3);

        idx += simd_width * 4;
    } while (idx != m);

    return n;

#undef WJR_REGISTER_REPLACE_VAL_IMPL
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_replace_val(T *dst, const T *src, size_t n, T from,
                                                T to) {
    static_assert(std::is_same_v<T, uint64_t>, "Currently only support uint64_t.");

    using simd = sse;

    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;
    constexpr auto threshold = std::max<size_t>(simd_width + 2, 4);

    if (WJR_UNLIKELY(n < threshold)) {
        return builtin_unroll_replace_val<threshold - 1>(dst, src, n, from, to);
    }

    WJR_ASSUME(n >= threshold);
    return builtin_simd_replace_val<simd>(dst, src, n, from, to);
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