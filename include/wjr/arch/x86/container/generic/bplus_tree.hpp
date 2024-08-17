#ifndef WJR_X86_CONTAINER_GENERIC_BPLUS_TREE_HPP__
#define WJR_X86_CONTAINER_GENERIC_BPLUS_TREE_HPP__

#include <algorithm>

#include <wjr/arch/x86/simd/simd.hpp>

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

            const auto x0 = read_memory<uint16_t>(first);
            const auto x1 = read_memory<uint16_t>(last - 2);
            write_memory<uint16_t>(dest, x0);
            write_memory<uint16_t>(dest + n - 2, x1);
            return;
        } while (false);
    }

    if constexpr (size <= 2) {
        do {
            if (n >= 8) {
                break;
            }

            const auto x0 = read_memory<uint32_t>(first);
            const auto x1 = read_memory<uint32_t>(last - 4);
            write_memory<uint32_t>(dest, x0);
            write_memory<uint32_t>(dest + n - 4, x1);
            return;
        } while (false);
    }

    if constexpr (size <= 4) {
        do {
            if constexpr (size >= 2) {
                if (n >= 16) {
                    break;
                }
            }

            const auto x0 = read_memory<uint64_t>(first);
            const auto x1 = read_memory<uint64_t>(last - 8);
            write_memory<uint64_t>(dest, x0);
            write_memory<uint64_t>(dest + n - 8, x1);
            return;
        } while (false);
    }

    if constexpr (size >= 2) {
        do {
            if constexpr (size >= 4) {
                if (n >= 32) {
                    break;
                }
            }

            const auto x0 = sse::loadu(first);
            const auto x1 = sse::loadu(last - 16);
            sse::storeu(dest, x0);
            sse::storeu(dest + n - 16, x1);
            return;
        } while (false);
    }

    if constexpr (size >= 4) {
        do {
            if constexpr (size >= 8) {
                if (n >= 64) {
                    break;
                }
            }

#if WJR_HAS_SIMD(AVX2)
            const auto x0 = avx::loadu(first);
            const auto x1 = avx::loadu(last - 32);
            avx::storeu(dest, x0);
            avx::storeu(dest + n - 32, x1);
#else
            const auto x0 = sse::loadu(first);
            const auto x1 = sse::loadu(first + 16);
            const auto x2 = sse::loadu(last - 32);
            const auto x3 = sse::loadu(last - 16);
            sse::storeu((dest), x0);
            sse::storeu((dest + 16), x1);
            sse::storeu((dest + n - 32), x2);
            sse::storeu((dest + n - 16), x3);
#endif
            return;
        } while (false);
    }

    if constexpr (size == 8) {
#if WJR_HAS_SIMD(AVX2)
        const auto x0 = avx::loadu(first);
        const auto x1 = avx::loadu(first + 32);
        const auto x2 = avx::loadu(last - 64);
        const auto x3 = avx::loadu(last - 32);
        avx::storeu((dest), x0);
        avx::storeu((dest + 32), x1);
        avx::storeu((dest + n - 64), x2);
        avx::storeu((dest + n - 32), x3);
#else
        const auto x0 = sse::loadu(first);
        const auto x1 = sse::loadu(first + 16);
        const auto x2 = sse::loadu(first + 32);
        const auto x3 = sse::loadu(first + 48);
        const auto x4 = sse::loadu(last - 64);
        const auto x5 = sse::loadu(last - 48);
        const auto x6 = sse::loadu(last - 32);
        const auto x7 = sse::loadu(last - 16);
        sse::storeu((dest), x0);
        sse::storeu((dest + 16), x1);
        sse::storeu((dest + 32), x2);
        sse::storeu((dest + 48), x3);
        sse::storeu((dest + n - 64), x4);
        sse::storeu((dest + n - 48), x5);
        sse::storeu((dest + n - 32), x6);
        sse::storeu((dest + n - 16), x7);
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

            const auto x0 = read_memory<uint16_t>(first);
            const auto x1 = read_memory<uint16_t>(last - 2);
            write_memory<uint16_t>(dest - n, x0);
            write_memory<uint16_t>(dest - 2, x1);
            return;
        } while (false);
    }

    if constexpr (size <= 2) {
        do {
            if (n >= 8) {
                break;
            }

            const auto x0 = read_memory<uint32_t>(first);
            const auto x1 = read_memory<uint32_t>(last - 4);
            write_memory<uint32_t>(dest - n, x0);
            write_memory<uint32_t>(dest - 4, x1);
            return;
        } while (false);
    }

    if constexpr (size <= 4) {
        do {
            if constexpr (size >= 2) {
                if (n >= 16) {
                    break;
                }
            }

            const auto x0 = read_memory<uint64_t>(first);
            const auto x1 = read_memory<uint64_t>(last - 8);
            write_memory<uint64_t>(dest - n, x0);
            write_memory<uint64_t>(dest - 8, x1);
            return;
        } while (false);
    }

    if constexpr (size >= 2) {
        do {
            if constexpr (size >= 4) {
                if (n >= 32) {
                    break;
                }
            }

            const auto x0 = sse::loadu(first);
            const auto x1 = sse::loadu(last - 16);
            sse::storeu((dest - n), x0);
            sse::storeu((dest - 16), x1);
            return;
        } while (false);
    }

    if constexpr (size >= 4) {
        do {
            if constexpr (size >= 8) {
                if (n >= 64) {
                    break;
                }
            }

#if WJR_HAS_SIMD(AVX2)
            const auto x0 = avx::loadu(first);
            const auto x1 = avx::loadu(last - 32);
            avx::storeu((dest - n), x0);
            avx::storeu((dest - 32), x1);
#else
            const auto x0 = sse::loadu(first);
            const auto x1 = sse::loadu(first + 16);
            const auto x2 = sse::loadu(last - 32);
            const auto x3 = sse::loadu(last - 16);
            sse::storeu((dest - n), x0);
            sse::storeu((dest - n + 16), x1);
            sse::storeu((dest - 32), x2);
            sse::storeu((dest - 16), x3);
#endif
            return;
        } while (false);
    }

    if constexpr (size == 8) {
#if WJR_HAS_SIMD(AVX2)
        const auto x0 = avx::loadu(first);
        const auto x1 = avx::loadu(first + 32);
        const auto x2 = avx::loadu(last - 64);
        const auto x3 = avx::loadu(last - 32);
        avx::storeu((dest - n), x0);
        avx::storeu((dest - n + 32), x1);
        avx::storeu((dest - 64), x2);
        avx::storeu((dest - 32), x3);
#else
        const auto x0 = sse::loadu(first);
        const auto x1 = sse::loadu(first + 16);
        const auto x2 = sse::loadu(first + 32);
        const auto x3 = sse::loadu(first + 48);
        const auto x4 = sse::loadu(last - 64);
        const auto x5 = sse::loadu(last - 48);
        const auto x6 = sse::loadu(last - 32);
        const auto x7 = sse::loadu(last - 16);
        sse::storeu((dest - n), x0);
        sse::storeu((dest - n + 16), x1);
        sse::storeu((dest - n + 32), x2);
        sse::storeu((dest - n + 48), x3);
        sse::storeu((dest - 64), x4);
        sse::storeu((dest - 48), x5);
        sse::storeu((dest - 32), x6);
        sse::storeu((dest - 16), x7);
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
