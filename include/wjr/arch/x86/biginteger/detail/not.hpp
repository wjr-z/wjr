/**
 * @file not.hpp
 * @author wjr
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 * @todo Optimize... and move large builtin to .cpp.
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef WJR_ARCH_X86_BIGINTEGER_DETAIL_NOT_HPP__
#define WJR_ARCH_X86_BIGINTEGER_DETAIL_NOT_HPP__

#include <wjr/arch/x86/simd/simd.hpp>
#include <wjr/math/detail.hpp>
#include <wjr/memory/align.hpp>

namespace wjr::math {

#if WJR_HAS_SIMD(SSE2)
    #define WJR_HAS_BUILTIN_BI_NOT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(BI_NOT_N)

template <typename T>
void large_builtin_bi_not_n(T *dst, const T *src, size_t n) noexcept {
    constexpr auto is_avx = WJR_HAS_SIMD(AVX2);

    using simd = std::conditional_t<is_avx, avx, sse>;
    constexpr auto simd_width = simd::width();
    constexpr auto type_width = simd_width / 64;

    const uintptr_t ptr = reinterpret_cast<uintptr_t>(dst);
    WJR_ASSUME(ptr % sizeof(T) == 0);
    const size_t offset = align_up_offset(ptr, 32) / sizeof(T);

    WJR_ASSUME(offset < 4);

    const auto y = sse::ones();

    do {
        if (offset == 0) {
            break;
        }

        if (offset == 1) {
            dst[0] = ~src[0];
            break;
        }

        sse::storeu(dst, sse::Xor(sse::loadu(src), y));

        if (offset == 3) {
            dst[2] = ~src[2];
        }
    } while (false);

    dst += offset;
    src += offset;
    n -= offset;

    const auto z = broadcast<__m128i_t, typename simd::int_tag_type>(y);

    size_t m = n & (-type_width * 4);
    n &= (type_width * 4) - 1;
    WJR_ASSUME(m != 0);

    do {
        auto x0 = simd::loadu(src);
        auto x1 = simd::loadu(src + type_width);
        auto x2 = simd::loadu(src + type_width * 2);
        auto x3 = simd::loadu(src + type_width * 3);

        simd::store(dst, simd::Xor(x0, z));
        simd::store(dst + type_width, simd::Xor(x1, z));
        simd::store(dst + type_width * 2, simd::Xor(x2, z));
        simd::store(dst + type_width * 3, simd::Xor(x3, z));

        dst += type_width * 4;
        src += type_width * 4;
        m -= type_width * 4;
    } while (m != 0);

    if (WJR_UNLIKELY(n == 0)) {
        return;
    }

    m = n / type_width;
    WJR_ASSUME(m < 4);

    switch (m) {
    case 3: {
        simd::store(dst, simd::Xor(simd::loadu(src), z));
        WJR_FALLTHROUGH;
    }
    case 2: {
        simd::store(dst + type_width * (m - 2),
                    simd::Xor(simd::loadu(src + type_width * (m - 2)), z));
        WJR_FALLTHROUGH;
    }
    case 1: {
        simd::store(dst + type_width * (m - 1),
                    simd::Xor(simd::loadu(src + type_width * (m - 1)), z));
        WJR_FALLTHROUGH;
    }
    case 0: {
        break;
    }
    default: {
        WJR_UNREACHABLE();
    }
    }

    m = n & (-type_width);

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
        sse::store(dst + m, sse::Xor(sse::loadu(src + m), y));
        break;
    }

    case 3: {
        sse::store(dst + m, sse::Xor(sse::loadu(src + m), y));
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
WJR_INTRINSIC_INLINE void unaligned_large_builtin_bi_not_n(T *dst, const T *src,
                                                           size_t n) noexcept {
    size_t idx = 0;
    const auto y = sse::ones();

    if (n & 4) {
        sse::storeu(dst + idx, sse::Xor(sse::loadu(src + idx), y));
        sse::storeu(dst + idx + 2, sse::Xor(sse::loadu(src + idx + 2), y));

        idx += 4;
    }

    if (n & 2) {
        sse::storeu(dst + idx, sse::Xor(sse::loadu(src + idx), y));

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
        auto x0 = sse::loadu(src + idx);
        auto x1 = sse::loadu(src + idx + 2);
        auto x2 = sse::loadu(src + idx + 4);
        auto x3 = sse::loadu(src + idx + 6);

        sse::storeu(dst + idx, sse::Xor(x0, y));
        sse::storeu(dst + idx + 2, sse::Xor(x1, y));
        sse::storeu(dst + idx + 4, sse::Xor(x2, y));
        sse::storeu(dst + idx + 6, sse::Xor(x3, y));

        idx += 8;
    } while (idx != n);
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_bi_not_n(T *dst, const T *src, size_t n) noexcept {
    static_assert(sizeof(T) == 8);

    if (WJR_UNLIKELY(n < 4)) {
        if (WJR_UNLIKELY(n == 0)) {
            return;
        }

        if (WJR_UNLIKELY(n == 1)) {
            dst[0] = ~src[0];
            return;
        }

        sse::storeu(dst, sse::Xor(sse::loadu(src), sse::ones()));

        if (n == 3) {
            dst[2] = ~src[2];
        }

        return;
    }

    // Can be aligned
    // todo : Align those that cannot be aligned with T through uint8_t
    if (WJR_LIKELY(n >= 35 && reinterpret_cast<uintptr_t>(dst) % sizeof(T) == 0)) {
        return large_builtin_bi_not_n(dst, src, n);
    }

    return unaligned_large_builtin_bi_not_n(dst, src, n);
}

#endif

} // namespace wjr::math

#endif // WJR_ARCH_X86_BIGINTEGER_DETAIL_NOT_HPP__