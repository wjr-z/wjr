#ifndef WJR_X86_MATH_COPY_HPP__
#define WJR_X86_MATH_COPY_HPP__

#include <algorithm>

#include <wjr/x86/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(X86_SIMD)
#define WJR_HAS_BUILTIN_COPY_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_COPY_C WJR_HAS_DEF
#define WJR_HAS_BUILTIN_COPY_BACKWARD_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_COPY_BACKWARD_C WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(COPY_C)

template <size_t N>
void builtin_copy_c_bytes_impl(const uint8_t *src, uint8_t *dst) {
    if constexpr (N == 2) {
        const auto x = read_memory<uint16_t>(src, endian::native);
        write_memory<uint16_t>(dst, x, endian::native);
        return;
    }

    if constexpr (N == 4) {
        const auto x = read_memory<uint32_t>(src, endian::native);
        write_memory<uint32_t>(dst, x, endian::native);
        return;
    }

    if constexpr (N == 8) {
        const auto x = read_memory<uint64_t>(src, endian::native);
        write_memory<uint64_t>(dst, x, endian::native);
        return;
    }

    if constexpr (N == 16) {
        const auto x = sse::loadu((__m128i *)(src));
        sse::storeu((__m128i *)(dst), x);
        return;
    }

    if constexpr (N == 32) {
#if WJR_HAS_SIMD(AVX2)
        const auto x = avx::loadu((__m256i *)(src));
        avx::storeu((__m256i *)(dst), x);
#else
        builtin_copy_c_bytes_impl<16>(dst, src);
        builtin_copy_c_bytes_impl<16>(dst + 16, src + 16);
#endif
        return;
    }

    WJR_UNREACHABLE();
}

template <size_t N, typename T>
void builtin_copy_c_bytes(const T *src, T *dst) {
    builtin_copy_c_bytes_impl<N * sizeof(T)>(reinterpret_cast<const uint8_t *>(src),
                                             reinterpret_cast<uint8_t *>(dst));
}

template <size_t N, typename T>
void builtin_copy_c(const T *src, T *dst) {
    if constexpr (N == 0) {
        return;
    }

    if constexpr (N == 1) {
        dst[0] = src[0];
        return;
    }

    if constexpr (N > 8) {
        std::copy(src, src + N, dst);
        return;
    }

    if constexpr (N == 2) {
        builtin_copy_c_bytes<2>(src, dst);
    } else if constexpr (N == 3) {
        builtin_copy_c_bytes<2>(src, dst);
        builtin_copy_c_bytes<1>(src + 2, dst + 2);
    } else if constexpr (N == 4) {
        builtin_copy_c_bytes<4>(src, dst);
    } else if constexpr (N == 5) {
        builtin_copy_c_bytes<4>(src, dst);
        builtin_copy_c_bytes<1>(src + 4, dst + 4);
    } else if constexpr (N == 6) {
        builtin_copy_c_bytes<4>(src, dst);
        builtin_copy_c_bytes<2>(src + 4, dst + 4);
    } else if constexpr (N == 7) {
        builtin_copy_c_bytes<4>(src, dst);
        builtin_copy_c_bytes<4>(src + 3, dst + 3);
    } else if constexpr (N == 8) {
        builtin_copy_c_bytes<8>(src, dst);
    }
}

#endif

#if WJR_HAS_BUILTIN(COPY_BACKWARD_C)

template <size_t N>
void builtin_copy_backward_c_bytes_impl(const uint8_t *s_last, uint8_t *d_last) {
    if constexpr (N == 2) {
        const auto x = read_memory<uint16_t>(s_last - 2, endian::native);
        write_memory<uint16_t>(d_last - 2, x, endian::native);
        return;
    }

    if constexpr (N == 4) {
        const auto x = read_memory<uint32_t>(s_last - 4, endian::native);
        write_memory<uint32_t>(d_last - 4, x, endian::native);
        return;
    }

    if constexpr (N == 8) {
        const auto x = read_memory<uint64_t>(s_last - 8, endian::native);
        write_memory<uint64_t>(d_last - 8, x, endian::native);
        return;
    }

    if constexpr (N == 16) {
        const auto x = sse::loadu((__m128i *)(s_last - 16));
        sse::storeu((__m128i *)(d_last - 16), x);
        return;
    }

    if constexpr (N == 32) {
#if WJR_HAS_SIMD(AVX2)
        const auto x = avx::loadu((__m256i *)(s_last - 32));
        avx::storeu((__m256i *)(d_last - 32), x);
#else
        builtin_copy_backward_c_bytes_impl<16>(s_last, d_last);
        builtin_copy_backward_c_bytes_impl<16>(s_last - 16, d_last - 16);
#endif
    }

    WJR_UNREACHABLE();
}

template <size_t N, typename T>
void builtin_copy_backward_c_bytes(const T *s_last, T *d_last) {
    builtin_copy_backward_c_bytes_impl<N * sizeof(T)>(
        reinterpret_cast<const uint8_t *>(s_last), reinterpret_cast<uint8_t *>(d_last));
}

template <size_t N, typename T>
void builtin_copy_backward_c(const T *s_last, T *d_last) {
    if constexpr (N == 0) {
        return;
    }

    if constexpr (N == 1) {
        d_last[-1] = s_last[-1];
        return;
    }

    if constexpr (N > 8) {
        std::copy_backward(s_last - N, s_last, d_last);
        return;
    }

    if constexpr (N == 2) {
        builtin_copy_backward_c_bytes<2>(s_last, d_last);
    } else if constexpr (N == 3) {
        builtin_copy_backward_c_bytes<2>(s_last, d_last);
        builtin_copy_backward_c_bytes<1>(s_last - 2, d_last - 2);
    } else if constexpr (N == 4) {
        builtin_copy_backward_c_bytes<4>(s_last, d_last);
    } else if constexpr (N == 5) {
        builtin_copy_backward_c_bytes<4>(s_last, d_last);
        builtin_copy_backward_c_bytes<1>(s_last - 4, d_last - 4);
    } else if constexpr (N == 6) {
        builtin_copy_backward_c_bytes<4>(s_last, d_last);
        builtin_copy_backward_c_bytes<2>(s_last - 4, d_last - 4);
    } else if constexpr (N == 7) {
        builtin_copy_backward_c_bytes<4>(s_last, d_last);
        builtin_copy_backward_c_bytes<4>(s_last - 3, d_last - 3);
    } else if constexpr (N == 8) {
        builtin_copy_backward_c_bytes<8>(s_last, d_last);
    }
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_COPY_HPP__