#ifndef WJR_MATH_FIND_HPP__
#define WJR_MATH_FIND_HPP__

#include <wjr/simd/simd.hpp>
#include <wjr/unroll.hpp>

namespace wjr {

    // TODO : find

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_not(const T *src, size_t n, T val) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != val) {
            break;
        }
    }

    return idx;
}

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_FIND_NOT_VAL WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(FIND_NOT_VAL)

template <size_t unroll, typename T>
WJR_INTRINSIC_INLINE size_t builtin_unroll_find_not(const T *src, size_t n, T val) {
    auto fn = [src, n, val](auto ic) -> std::optional<size_t> {
        constexpr size_t idx = decltype(ic)::value;

        if (idx == n || WJR_LIKELY(src[idx] != val)) {
            return idx;
        }

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
WJR_INLINE size_t builtin_simd_find_not(const T *src, const size_t n, T val) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;
    using simd_int = typename simd::int_type;

    WJR_ASSUME(n >= simd_width);

    size_t m = n;

    {
        size_t k = m % simd_width;
        size_t idx = builtin_unroll_find_not<simd_width - 1>(src, k, val);
        if (idx != k) {
            return idx;
        }

        src += k;
        m -= k;
    }

    WJR_ASSUME(m % simd_width == 0);
    WJR_ASSUME(m != 0);

    simd_int y = simd::set1_epi64(val);

#define WJR_REGISTER_FIND_NOT_IMPL(index)                                                \
    do {                                                                                 \
        simd_int x = simd::loadu((simd_int *)(src + (index)));                           \
        simd_int r = simd::cmpeq_epi64(x, y);                                            \
        auto mask = simd::movemask_epi8(r);                                              \
        if (mask != simd::mask()) {                                                      \
            if constexpr (std::is_same_v<simd, sse>) {                                   \
                return (n - m) + (index) + (mask == 0x00FF);                             \
            } else {                                                                     \
                auto k = ctz(~mask) / 8;                                                 \
                return (n - m) + (index) + k;                                            \
            }                                                                            \
        }                                                                                \
    } while (0)

    {
        WJR_REGISTER_FIND_NOT_IMPL(0);

        src += simd_width;
        m -= simd_width;
    }

    if (!m) {
        return n;
    }

    if ((m / simd_width) & 1) {
        WJR_REGISTER_FIND_NOT_IMPL(0);

        src += simd_width;
        m -= simd_width;
    }

    if ((m / (simd_width * 2)) & 1) {
        WJR_REGISTER_FIND_NOT_IMPL(0);
        WJR_REGISTER_FIND_NOT_IMPL(simd_width);

        src += simd_width * 2;
        m -= simd_width * 2;
    }

    if (!m) {
        return n;
    }

    WJR_ASSUME(m % (simd_width * 4) == 0);

    WJR_REGISTER_FIND_NOT_IMPL(0);
    WJR_REGISTER_FIND_NOT_IMPL(simd_width);
    WJR_REGISTER_FIND_NOT_IMPL(simd_width * 2);
    WJR_REGISTER_FIND_NOT_IMPL(simd_width * 3);

    m -= simd_width * 4;

    if (WJR_UNLIKELY(!m)) {
        return n;
    }

    src += simd_width * 4;

    size_t idx = 0;

    do {
        WJR_REGISTER_FIND_NOT_IMPL(idx);
        WJR_REGISTER_FIND_NOT_IMPL(idx + simd_width);
        WJR_REGISTER_FIND_NOT_IMPL(idx + simd_width * 2);
        WJR_REGISTER_FIND_NOT_IMPL(idx + simd_width * 3);

        idx += simd_width * 4;
    } while (idx != m);

    return n;

#undef WJR_REGISTER_FIND_NOT_IMPL
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_not(const T *src, size_t n, T val) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    using simd = sse;

    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width() / nd;
    constexpr auto threshold = std::max<size_t>(simd_width + 2, 4);

    if (WJR_UNLIKELY(n < threshold)) {
        return builtin_unroll_find_not<threshold - 1>(src, n, val);
    }

    WJR_ASSUME(n >= threshold);
    return builtin_simd_find_not<simd>(src, n, val);
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t find_not(const T *src, size_t n, type_identity_t<T> val) {
#if WJR_HAS_BUILTIN(FIND_NOT_VAL)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_not(src, n, val);
        }

        return builtin_find_not(src, n, val);
    } else {
        return fallback_find_not(src, n, val);
    }
#else
    return fallback_find_not(src, n, val);
#endif
}

} // namespace wjr

#endif // WJR_MATH_FIND_HPP__