#ifndef WJR_X86_NOT_HPP__
#define WJR_X86_NOT_HPP__

#include <wjr/gen/nofast_1_2_8.hpp>
#include <wjr/simd/simd.hpp>

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

    WJR_ASSUME(n > 32);

    uintptr_t ptr = reinterpret_cast<uintptr_t>(dst);
    WJR_ASSUME(ptr % sizeof(T) == 0);
    size_t offset = (((ptr + 32 - 1) & (-32)) - ptr) / sizeof(T);

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
        WJR_UNREACHABLE;
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

    n -= m;
    if (WJR_UNLIKELY(n == 0)) {
        return;
    }

    dst += m;
    src += m;

    do {
        if (n < simd_loop) {
            break;
        }

        simd::store((simd_int *)(dst), simd::Xor(simd::loadu((simd_int *)(src)), z));

        if (n < simd_loop * 2) {
            dst += simd_loop;
            src += simd_loop;
            n -= simd_loop;
            break;
        }

        simd::store((simd_int *)(dst + simd_loop),
                    simd::Xor(simd::loadu((simd_int *)(src + simd_loop)), z));

        if (n < simd_loop * 3) {
            dst += simd_loop * 2;
            src += simd_loop * 2;
            n -= simd_loop * 2;
            break;
        }

        simd::store((simd_int *)(dst + simd_loop * 2),
                    simd::Xor(simd::loadu((simd_int *)(src + simd_loop * 2)), z));

        dst += simd_loop * 3;
        src += simd_loop * 3;
        n -= simd_loop * 3;
    } while (0);

    if (WJR_UNLIKELY(n == 0)) {
        return;
    }

    switch (n) {
    case 0: {
        break;
    }
    case 1: {
        dst[0] = ~src[0];
        break;
    }
    case 2: {
        sse::storeu((__m128i *)(dst), sse::Xor(sse::loadu((__m128i *)(src)), y));
        break;
    }

    case 3: {
        sse::storeu((__m128i *)(dst), sse::Xor(sse::loadu((__m128i *)(src)), y));
        dst[2] = ~src[2];
        break;
    }

    default: {
        WJR_UNREACHABLE;
    }
    }
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_not_n(T *dst, const T *src, size_t n) {
    static_assert(sizeof(T) == 8, "");

    if (WJR_UNLIKELY(n < 4)) {
        do {
            if (n == 0) {
                break;
            }

            dst[0] = ~src[0];

            if (n == 1) {
                break;
            }

            dst[1] = ~src[1];

            if (n == 2) {
                break;
            }

            dst[2] = ~src[2];
        } while (0);

        return;
    }

    if (WJR_UNLIKELY(n >= 35)) {
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