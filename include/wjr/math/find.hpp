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
WJR_NODISCARD WJR_INTRINSIC_INLINE size_t builtin_unroll_find_not(const T *src, size_t n,
                                                                  T val) {
    auto idx = unroll_call<unroll>([src, n, val](auto ic) -> std::optional<size_t> {
        constexpr size_t idx = decltype(ic)::value;

        if (idx == n || WJR_LIKELY(src[idx] != val)) {
            return idx;
        }

        return std::nullopt;
    });

    if (idx.has_value()) {
        return idx.value();
    }

    WJR_ASSUME(n == unroll);
    return n;
}

template <typename T>
WJR_INLINE size_t builtin_simd_find_not(const T *src, const size_t n, T val) {
    WJR_ASSUME(n % 2 == 0);
    WJR_ASSUME(n != 0);

    size_t m = n;
    __m128i y = sse::set1_epi64(val);

#define WJR_REGISTER_FIND_NOT_IMPL(index)                                                \
    do {                                                                                 \
        __m128i x = sse::loadu((__m128i *)(src + (index)));                              \
        __m128i r = sse::cmpeq_epi64(x, y);                                              \
        auto mask = sse::movemask_epi8(r);                                               \
        if (mask != sse::mask()) {                                                       \
            return (n - m) + (index) + (mask == 0x00FF);                                 \
        }                                                                                \
    } while (0)

    if (m & 2) {
        WJR_REGISTER_FIND_NOT_IMPL(0);

        src += 2;
        m -= 2;
    }

    if (m & 4) {
        WJR_REGISTER_FIND_NOT_IMPL(0);
        WJR_REGISTER_FIND_NOT_IMPL(2);

        src += 4;
        m -= 4;
    }

    if (!m) {
        return n;
    }

    WJR_ASSUME(m % 8 == 0);

    size_t idx = 0;

    do {
        WJR_REGISTER_FIND_NOT_IMPL(idx);
        WJR_REGISTER_FIND_NOT_IMPL(idx + 2);
        WJR_REGISTER_FIND_NOT_IMPL(idx + 4);
        WJR_REGISTER_FIND_NOT_IMPL(idx + 6);

        idx += 8;
    } while (idx != m);

    return n;

#undef WJR_REGISTER_FIND_NOT_IMPL
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_not(const T *src, size_t n, T val) {
    static_assert(sizeof(T) == 8, "Currently only support uint64_t.");

    if (WJR_UNLIKELY(n < 4)) {
        return builtin_unroll_find_not<3>(src, n, val);
    }

    size_t m = n;

    {
        auto idx = builtin_unroll_find_not<1>(src, 1, val);

        if (idx != 1) {
            return idx;
        }

        ++src;
        --m;
    }

    {
        size_t k = m % 2;
        size_t idx = builtin_unroll_find_not<1>(src, k, val);

        if (idx != k) {
            return 1 + idx;
        }

        src += k;
        m -= k;
    }

    return n - m + builtin_simd_find_not(src, m, val);
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