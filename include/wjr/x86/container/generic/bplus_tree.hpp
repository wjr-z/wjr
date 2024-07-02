#ifndef WJR_X86_CONTAINER_GENERIC_BPLUS_TREE_HPP__
#define WJR_X86_CONTAINER_GENERIC_BPLUS_TREE_HPP__

#include <algorithm>

#include <wjr/x86/simd/simd.hpp>

namespace wjr {

#if WJR_HAS_SIMD(SSE2)
#define WJR_HAS_BUILTIN_BPLUS_TREE_COPY WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(BPLUS_TREE_COPY)

template <size_t Min, size_t Max, size_t size>
WJR_INTRINSIC_INLINE void __builtin_bplus_tree_copy_impl(const uint8_t *first,
                                                         const uint8_t *last,
                                                         uint8_t *dest) noexcept {
    const size_t n = last - first;
    WJR_ASSERT_ASSUME_L2(n >= Min * size && n <= Max * size);

    if (WJR_UNLIKELY(n == 0)) {
        return;
    }

    if (WJR_UNLIKELY(n == size)) {
        reinterpret_cast<uint_t<size * 8> *>(dest)[0] =
            reinterpret_cast<const uint_t<size * 8> *>(first)[0];
        return;
    }

    if constexpr (size <= 1) {
        do {
            if (n >= 4) {
                break;
            }

            const auto x0 = read_memory<uint16_t>(first, endian::native);
            const auto x1 = read_memory<uint16_t>(last - 2, endian::native);
            write_memory<uint16_t>(dest, x0, endian::native);
            write_memory<uint16_t>(dest + n - 2, x1, endian::native);
            return;
        } while (0);
    }

    if constexpr (size <= 2) {
        do {
            if (n >= 8) {
                break;
            }

            const auto x0 = read_memory<uint32_t>(first, endian::native);
            const auto x1 = read_memory<uint32_t>(last - 4, endian::native);
            write_memory<uint32_t>(dest, x0, endian::native);
            write_memory<uint32_t>(dest + n - 4, x1, endian::native);
            return;
        } while (0);
    }

    if constexpr (size <= 4) {
        do {
            if constexpr (size >= 2) {
                if (n >= 16) {
                    break;
                }
            }

            const auto x0 = read_memory<uint64_t>(first, endian::native);
            const auto x1 = read_memory<uint64_t>(last - 8, endian::native);
            write_memory<uint64_t>(dest, x0, endian::native);
            write_memory<uint64_t>(dest + n - 8, x1, endian::native);
            return;
        } while (0);
    }

    if constexpr (size >= 2) {
        do {
            if constexpr (size >= 4) {
                if (n >= 32) {
                    break;
                }
            }

            const auto x0 = sse::loadu((__m128i *)first);
            const auto x1 = sse::loadu((__m128i *)(last - 16));
            sse::storeu((__m128i *)(dest), x0);
            sse::storeu((__m128i *)(dest + n - 16), x1);
            return;
        } while (0);
    }

    if constexpr (size >= 4) {
        do {
            if constexpr (size >= 8) {
                if (n >= 64) {
                    break;
                }
            }

#if WJR_HAS_SIMD(AVX2)
            const auto x0 = avx::loadu((__m256i *)first);
            const auto x1 = avx::loadu((__m256i *)(last - 32));
            avx::storeu((__m256i *)(dest), x0);
            avx::storeu((__m256i *)(dest + n - 32), x1);
#else
            const auto x0 = sse::loadu((__m128i *)first);
            const auto x1 = sse::loadu((__m128i *)(first + 16));
            const auto x2 = sse::loadu((__m128i *)(last - 32));
            const auto x3 = sse::loadu((__m128i *)(last - 16));
            sse::storeu((__m128i *)(dest), x0);
            sse::storeu((__m128i *)(dest + 16), x1);
            sse::storeu((__m128i *)(dest + n - 32), x2);
            sse::storeu((__m128i *)(dest + n - 16), x3);
#endif
            return;
        } while (0);
    }

    if constexpr (size == 8) {
#if WJR_HAS_SIMD(AVX2)
        const auto x0 = avx::loadu((__m256i *)first);
        const auto x1 = avx::loadu((__m256i *)(first + 32));
        const auto x2 = avx::loadu((__m256i *)(last - 64));
        const auto x3 = avx::loadu((__m256i *)(last - 32));
        avx::storeu((__m256i *)(dest), x0);
        avx::storeu((__m256i *)(dest + 32), x1);
        avx::storeu((__m256i *)(dest + n - 64), x2);
        avx::storeu((__m256i *)(dest + n - 32), x3);
#else
        const auto x0 = sse::loadu((__m128i *)first);
        const auto x1 = sse::loadu((__m128i *)(first + 16));
        const auto x2 = sse::loadu((__m128i *)(first + 32));
        const auto x3 = sse::loadu((__m128i *)(first + 48));
        const auto x4 = sse::loadu((__m128i *)(last - 64));
        const auto x5 = sse::loadu((__m128i *)(last - 48));
        const auto x6 = sse::loadu((__m128i *)(last - 32));
        const auto x7 = sse::loadu((__m128i *)(last - 16));
        sse::storeu((__m128i *)(dest), x0);
        sse::storeu((__m128i *)(dest + 16), x1);
        sse::storeu((__m128i *)(dest + 32), x2);
        sse::storeu((__m128i *)(dest + 48), x3);
        sse::storeu((__m128i *)(dest + n - 64), x4);
        sse::storeu((__m128i *)(dest + n - 48), x5);
        sse::storeu((__m128i *)(dest + n - 32), x6);
        sse::storeu((__m128i *)(dest + n - 16), x7);
#endif
    }
}

template <size_t Min, size_t Max, typename Other>
WJR_INTRINSIC_INLINE void builtin_bplus_tree_copy(const Other *first, const Other *last,
                                                  Other *dest) noexcept {
    __builtin_bplus_tree_copy_impl<Min, Max, sizeof(Other)>(
        reinterpret_cast<const uint8_t *>(first), reinterpret_cast<const uint8_t *>(last),
        reinterpret_cast<uint8_t *>(dest));
}

template <size_t Min, size_t Max, size_t size>
WJR_INTRINSIC_INLINE void
__builtin_bplus_tree_copy_backward_impl(const uint8_t *first, const uint8_t *last,
                                        uint8_t *dest) noexcept {
    const size_t n = last - first;
    WJR_ASSERT_ASSUME_L2(n >= Min * size && n <= Max * size);

    if (WJR_UNLIKELY(n == 0)) {
        return;
    }

    if (WJR_UNLIKELY(n == size)) {
        reinterpret_cast<uint_t<size * 8> *>(dest)[-1] =
            reinterpret_cast<const uint_t<size * 8> *>(first)[0];
        return;
    }

    if constexpr (size <= 1) {
        do {
            if (n >= 4) {
                break;
            }

            const auto x0 = read_memory<uint16_t>(first, endian::native);
            const auto x1 = read_memory<uint16_t>(last - 2, endian::native);
            write_memory<uint16_t>(dest - n, x0, endian::native);
            write_memory<uint16_t>(dest - 2, x1, endian::native);
            return;
        } while (0);
    }

    if constexpr (size <= 2) {
        do {
            if (n >= 8) {
                break;
            }

            const auto x0 = read_memory<uint32_t>(first, endian::native);
            const auto x1 = read_memory<uint32_t>(last - 4, endian::native);
            write_memory<uint32_t>(dest - n, x0, endian::native);
            write_memory<uint32_t>(dest - 4, x1, endian::native);
            return;
        } while (0);
    }

    if constexpr (size <= 4) {
        do {
            if constexpr (size >= 2) {
                if (n >= 16) {
                    break;
                }
            }

            const auto x0 = read_memory<uint64_t>(first, endian::native);
            const auto x1 = read_memory<uint64_t>(last - 8, endian::native);
            write_memory<uint64_t>(dest - n, x0, endian::native);
            write_memory<uint64_t>(dest - 8, x1, endian::native);
            return;
        } while (0);
    }

    if constexpr (size >= 2) {
        do {
            if constexpr (size >= 4) {
                if (n >= 32) {
                    break;
                }
            }

            const auto x0 = sse::loadu((__m128i *)first);
            const auto x1 = sse::loadu((__m128i *)(last - 16));
            sse::storeu((__m128i *)(dest - n), x0);
            sse::storeu((__m128i *)(dest - 16), x1);
            return;
        } while (0);
    }

    if constexpr (size >= 4) {
        do {
            if constexpr (size >= 8) {
                if (n >= 64) {
                    break;
                }
            }

#if WJR_HAS_SIMD(AVX2)
            const auto x0 = avx::loadu((__m256i *)first);
            const auto x1 = avx::loadu((__m256i *)(last - 32));
            avx::storeu((__m256i *)(dest - n), x0);
            avx::storeu((__m256i *)(dest - 32), x1);
#else
            const auto x0 = sse::loadu((__m128i *)first);
            const auto x1 = sse::loadu((__m128i *)(first + 16));
            const auto x2 = sse::loadu((__m128i *)(last - 32));
            const auto x3 = sse::loadu((__m128i *)(last - 16));
            sse::storeu((__m128i *)(dest - n), x0);
            sse::storeu((__m128i *)(dest - n + 16), x1);
            sse::storeu((__m128i *)(dest - 32), x2);
            sse::storeu((__m128i *)(dest - 16), x3);
#endif
            return;
        } while (0);
    }

    if constexpr (size == 8) {
#if WJR_HAS_SIMD(AVX2)
        const auto x0 = avx::loadu((__m256i *)first);
        const auto x1 = avx::loadu((__m256i *)(first + 32));
        const auto x2 = avx::loadu((__m256i *)(last - 64));
        const auto x3 = avx::loadu((__m256i *)(last - 32));
        avx::storeu((__m256i *)(dest - n), x0);
        avx::storeu((__m256i *)(dest - n + 32), x1);
        avx::storeu((__m256i *)(dest - 64), x2);
        avx::storeu((__m256i *)(dest - 32), x3);
#else
        const auto x0 = sse::loadu((__m128i *)first);
        const auto x1 = sse::loadu((__m128i *)(first + 16));
        const auto x2 = sse::loadu((__m128i *)(first + 32));
        const auto x3 = sse::loadu((__m128i *)(first + 48));
        const auto x4 = sse::loadu((__m128i *)(last - 64));
        const auto x5 = sse::loadu((__m128i *)(last - 48));
        const auto x6 = sse::loadu((__m128i *)(last - 32));
        const auto x7 = sse::loadu((__m128i *)(last - 16));
        sse::storeu((__m128i *)(dest - n), x0);
        sse::storeu((__m128i *)(dest - n + 16), x1);
        sse::storeu((__m128i *)(dest - n + 32), x2);
        sse::storeu((__m128i *)(dest - n + 48), x3);
        sse::storeu((__m128i *)(dest - 64), x4);
        sse::storeu((__m128i *)(dest - 48), x5);
        sse::storeu((__m128i *)(dest - 32), x6);
        sse::storeu((__m128i *)(dest - 16), x7);
#endif
    }
}

template <size_t Min, size_t Max, typename Other>
WJR_INTRINSIC_INLINE void builtin_bplus_tree_copy_backward(const Other *first,
                                                           const Other *last,
                                                           Other *dest) noexcept {
    __builtin_bplus_tree_copy_backward_impl<Min, Max, sizeof(Other)>(
        reinterpret_cast<const uint8_t *>(first), reinterpret_cast<const uint8_t *>(last),
        reinterpret_cast<uint8_t *>(dest));
}

#endif

} // namespace wjr

#endif // WJR_X86_CONTAINER_GENERIC_BPLUS_TREE_HPP__
