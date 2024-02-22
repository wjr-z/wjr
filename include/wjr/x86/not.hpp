#ifndef WJR_X86_NOT_HPP__
#define WJR_X86_NOT_HPP__

#include <wjr/simd/simd.hpp>
#include <wjr/math/details.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_NOT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(NOT_N)

template <typename T>
WJR_COLD void large_builtin_not_n(T *dst, const T *src, size_t n) {
    constexpr auto is_avx = WJR_PP_BOOL(WJR_HAS_SIMD(AVX2));

    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto simd_loop = simd_width / 64;

    uintptr_t ptr = reinterpret_cast<uintptr_t>(dst);
    WJR_ASSUME(ptr % sizeof(T) == 0);
    size_t offset = __align_up_offset(ptr, 32) / sizeof(T);

    WJR_ASSUME(offset < 4);

    auto y = sse::ones();

    switch (offset) {
    case 0: {
        break;
    }
    case 1: {
        dst[0] = ~src[0];

        ++dst;
        ++src;
        --n;
        break;
    }
    case 2: {
        sse::storeu((__m128i *)(dst), sse::Xor(sse::loadu((__m128i *)(src)), y));

        dst += 2;
        src += 2;
        n -= 2;
        break;
    }

    case 3: {
        sse::storeu((__m128i *)(dst), sse::Xor(sse::loadu((__m128i *)(src)), y));
        dst[2] = ~src[2];

        dst += 3;
        src += 3;
        n -= 3;
        break;
    }

    default: {
        WJR_UNREACHABLE();
    }
    }

    auto z = broadcast<__m128i_t, typename simd::int_tag_type>(y);

    size_t idx = 0;
    size_t m = n & (-simd_loop * 4);

    WJR_ASSUME(idx != m);

    do {
        auto x0 = simd::loadu((simd_int *)(src + idx));
        auto x1 = simd::loadu((simd_int *)(src + idx + simd_loop));
        auto x2 = simd::loadu((simd_int *)(src + idx + simd_loop * 2));
        auto x3 = simd::loadu((simd_int *)(src + idx + simd_loop * 3));

        simd::store((simd_int *)(dst + idx), simd::Xor(x0, z));
        simd::store((simd_int *)(dst + idx + simd_loop), simd::Xor(x1, z));
        simd::store((simd_int *)(dst + idx + simd_loop * 2), simd::Xor(x2, z));
        simd::store((simd_int *)(dst + idx + simd_loop * 3), simd::Xor(x3, z));

        idx += simd_loop * 4;
    } while (idx != m);

    if (WJR_UNLIKELY(n == m)) {
        return;
    }

    dst += m;
    src += m;
    n -= m;

    m = n / simd_loop;
    WJR_ASSUME(m < 4);

    switch (m) {
    case 3: {
        simd::store((simd_int *)(dst), simd::Xor(simd::loadu((simd_int *)(src)), z));
        WJR_FALLTHROUGH;
    }
    case 2: {
        simd::store((simd_int *)(dst + simd_loop * (m - 2)),
                    simd::Xor(simd::loadu((simd_int *)(src + simd_loop * (m - 2))), z));
        WJR_FALLTHROUGH;
    }
    case 1: {
        simd::store((simd_int *)(dst + simd_loop * (m - 1)),
                    simd::Xor(simd::loadu((simd_int *)(src + simd_loop * (m - 1))), z));
        WJR_FALLTHROUGH;
    }
    case 0: {
        break;
    }
    default: {
        WJR_UNREACHABLE();
    }
    }

    m = n & (-simd_loop);

    if (WJR_UNLIKELY(n == m)) {
        return;
    }

    WJR_ASSUME(n - m < 4);

    switch (n - m) {
    case 1: {
        dst[m] = ~src[m];
        break;
    }
    case 2: {
        sse::store((__m128i *)(dst + m), sse::Xor(sse::loadu((__m128i *)(src + m)), y));
        break;
    }

    case 3: {
        sse::store((__m128i *)(dst + m), sse::Xor(sse::loadu((__m128i *)(src + m)), y));
        dst[m + 2] = ~src[m + 2];
        break;
    }

    default: {
        WJR_UNREACHABLE();
    }
    }

    return;
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_not_n(T *dst, const T *src, size_t n) {
    static_assert(sizeof(T) == 8, "");

    if (WJR_UNLIKELY(n < 4)) {
        switch (n) {
        case 3: {
            dst[0] = ~src[0];
            WJR_FALLTHROUGH;
        }
        case 2: {
            dst[n - 2] = ~src[n - 2];
            WJR_FALLTHROUGH;
        }
        case 1: {
            dst[n - 1] = ~src[n - 1];
            WJR_FALLTHROUGH;
        }
        case 0: {
            break;
        }
        }

        return;
    }

    if (WJR_UNLIKELY(n >= 35)) {
        // Can be aligned
        // TODO : Align those that cannot be aligned with T through uint8_t
        if (WJR_LIKELY(reinterpret_cast<uintptr_t>(dst) % sizeof(T) == 0)) {
            return large_builtin_not_n(dst, src, n);
        }
    }

    size_t idx = 0;

    auto y = sse::ones();

    if (n & 4) {
        auto x0 = sse::loadu((__m128i *)(src + idx));
        auto x1 = sse::loadu((__m128i *)(src + idx + 2));

        sse::storeu((__m128i *)(dst + idx), sse::Xor(x0, y));
        sse::storeu((__m128i *)(dst + idx + 2), sse::Xor(x1, y));

        idx += 4;
    }

    if (n & 2) {
        sse::storeu((__m128i *)(dst + idx),
                    sse::Xor(sse::loadu((__m128i *)(src + idx)), y));

        idx += 2;
    }

    if (n & 1) {
        dst[idx] = ~src[idx];

        ++idx;
    }

    if (WJR_UNLIKELY(idx == n)) {
        return;
    }

    WJR_ASSUME((n - idx) % 8 == 0);

    do {
        auto x0 = sse::loadu((__m128i *)(src + idx));
        auto x1 = sse::loadu((__m128i *)(src + idx + 2));
        auto x2 = sse::loadu((__m128i *)(src + idx + 4));
        auto x3 = sse::loadu((__m128i *)(src + idx + 6));

        sse::storeu((__m128i *)(dst + idx), sse::Xor(x0, y));
        sse::storeu((__m128i *)(dst + idx + 2), sse::Xor(x1, y));
        sse::storeu((__m128i *)(dst + idx + 4), sse::Xor(x2, y));
        sse::storeu((__m128i *)(dst + idx + 6), sse::Xor(x3, y));

        idx += 8;
    } while (idx != n);
}

#endif //

} // namespace wjr

#endif // WJR_X86_NOT_HPP__