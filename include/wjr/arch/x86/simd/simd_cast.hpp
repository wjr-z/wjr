#ifndef WJR_ARCH_X86_SIMD_SIMD_CAST_HPP__
#define WJR_ARCH_X86_SIMD_SIMD_CAST_HPP__

#include <cstdint>

#include <wjr/arch/x86/simd/intrin.hpp>
#include <wjr/simd/detail.hpp>

namespace wjr {

#if WJR_HAS_SIMD(SSE)

struct __m128_t {
    using type = __m128;
};

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

struct __m128i_t {
    using type = __m128i;
};

struct __m128d_t {
    using type = __m128d;
};

template <>
struct simd_cast_fn<__m128_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(__m128 v) const noexcept {
        return _mm_castps_si128(v);
    }
};

template <>
struct simd_cast_fn<__m128_t, __m128d_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128d operator()(__m128 v) const noexcept {
        return _mm_castps_pd(v);
    }
};

template <>
struct simd_cast_fn<__m128i_t, __m128_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128 operator()(__m128i v) const noexcept {
        return _mm_castsi128_ps(v);
    }
};

template <>
struct simd_cast_fn<__m128i_t, __m128d_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128d operator()(__m128i v) const noexcept {
        return _mm_castsi128_pd(v);
    }
};

template <>
struct simd_cast_fn<__m128d_t, __m128_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128 operator()(__m128d v) const noexcept {
        return _mm_castpd_ps(v);
    }
};

template <>
struct simd_cast_fn<__m128d_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(__m128d v) const noexcept {
        return _mm_castpd_si128(v);
    }
};

template <>
struct simd_cast_fn<int8_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(int8_t v) const noexcept {
        return _mm_cvtsi32_si128(v);
    }
};

template <>
struct simd_cast_fn<uint8_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(uint8_t v) const noexcept {
        return _mm_cvtsi32_si128(v);
    }
};

template <>
struct simd_cast_fn<__m128i_t, int8_t> {
    WJR_CONST WJR_INTRINSIC_INLINE int8_t operator()(__m128i v) const noexcept {
        return static_cast<int8_t>(_mm_cvtsi128_si32(v));
    }
};

template <>
struct simd_cast_fn<__m128i_t, uint8_t> {
    WJR_CONST WJR_INTRINSIC_INLINE uint8_t operator()(__m128i v) const noexcept {
        return static_cast<uint8_t>(_mm_cvtsi128_si32(v));
    }
};

template <>
struct simd_cast_fn<int16_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(int16_t v) const noexcept {
        return _mm_cvtsi32_si128(v);
    }
};

template <>
struct simd_cast_fn<uint16_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(uint16_t v) const noexcept {
        return _mm_cvtsi32_si128(v);
    }
};

template <>
struct simd_cast_fn<__m128i_t, int16_t> {
    WJR_CONST WJR_INTRINSIC_INLINE int16_t operator()(__m128i v) const noexcept {
        return static_cast<int16_t>(_mm_cvtsi128_si32(v));
    }
};

template <>
struct simd_cast_fn<__m128i_t, uint16_t> {
    WJR_CONST WJR_INTRINSIC_INLINE uint16_t operator()(__m128i v) const noexcept {
        return static_cast<uint16_t>(_mm_cvtsi128_si32(v));
    }
};

template <>
struct simd_cast_fn<int32_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(int32_t v) const noexcept {
        return _mm_cvtsi32_si128(v);
    }
};

template <>
struct simd_cast_fn<uint32_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(uint32_t v) const noexcept {
        return _mm_cvtsi32_si128(v);
    }
};

template <>
struct simd_cast_fn<__m128i_t, int32_t> {
    WJR_CONST WJR_INTRINSIC_INLINE int32_t operator()(__m128i v) const noexcept {
        return _mm_cvtsi128_si32(v);
    }
};

template <>
struct simd_cast_fn<__m128i_t, uint32_t> {
    WJR_CONST WJR_INTRINSIC_INLINE uint32_t operator()(__m128i v) const noexcept {
        return static_cast<uint32_t>(_mm_cvtsi128_si32(v));
    }
};

template <>
struct simd_cast_fn<int64_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(int64_t v) const noexcept {
        return _mm_cvtsi64_si128(v);
    }
};

template <>
struct simd_cast_fn<uint64_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(uint64_t v) const noexcept {
        return _mm_cvtsi64_si128(static_cast<int64_t>(v));
    }
};

template <>
struct simd_cast_fn<__m128i_t, int64_t> {
    WJR_CONST WJR_INTRINSIC_INLINE int64_t operator()(__m128i v) const noexcept {
        return _mm_cvtsi128_si64(v);
    }
};

template <>
struct simd_cast_fn<__m128i_t, uint64_t> {
    WJR_CONST WJR_INTRINSIC_INLINE uint64_t operator()(__m128i v) const noexcept {
        return static_cast<uint64_t>(_mm_cvtsi128_si64(v));
    }
};

#endif // SSE2

#if WJR_HAS_SIMD(AVX)

struct __m256_t {
    using type = __m256;
};

struct __m256i_t {
    using type = __m256i;
};

struct __m256d_t {
    using type = __m256d;
};

template <>
struct simd_cast_fn<__m256_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(__m256 v) const noexcept {
        return _mm256_castps_si256(v);
    }
};

template <>
struct simd_cast_fn<__m256_t, __m256d_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256d operator()(__m256 v) const noexcept {
        return _mm256_castps_pd(v);
    }
};

template <>
struct simd_cast_fn<__m256i_t, __m256_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256 operator()(__m256i v) const noexcept {
        return _mm256_castsi256_ps(v);
    }
};

template <>
struct simd_cast_fn<__m256i_t, __m256d_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256d operator()(__m256i v) const noexcept {
        return _mm256_castsi256_pd(v);
    }
};

template <>
struct simd_cast_fn<__m256d_t, __m256_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256 operator()(__m256d v) const noexcept {
        return _mm256_castpd_ps(v);
    }
};

template <>
struct simd_cast_fn<__m256d_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(__m256d v) const noexcept {
        return _mm256_castpd_si256(v);
    }
};

template <>
struct simd_cast_fn<__m128i_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(__m128i v) const noexcept {
        return _mm256_castsi128_si256(v);
    }
};

template <>
struct simd_cast_fn<__m256i_t, __m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(__m256i v) const noexcept {
        return _mm256_castsi256_si128(v);
    }
};

template <>
struct simd_cast_fn<int8_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(int8_t v) const noexcept {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<int8_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<uint8_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(uint8_t v) const noexcept {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint8_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<__m256i_t, int8_t> {
    WJR_CONST WJR_INTRINSIC_INLINE int8_t operator()(__m256i v) const noexcept {
        return simd_cast<__m128i_t, int8_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<__m256i_t, uint8_t> {
    WJR_CONST WJR_INTRINSIC_INLINE uint8_t operator()(__m256i v) const noexcept {
        return simd_cast<__m128i_t, uint8_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<int16_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(int16_t v) const noexcept {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<int16_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<uint16_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(uint16_t v) const noexcept {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint16_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<__m256i_t, int16_t> {
    WJR_CONST WJR_INTRINSIC_INLINE int16_t operator()(__m256i v) const noexcept {
        return simd_cast<__m128i_t, int16_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<__m256i_t, uint16_t> {
    WJR_CONST WJR_INTRINSIC_INLINE uint16_t operator()(__m256i v) const noexcept {
        return simd_cast<__m128i_t, uint16_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<int32_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(int32_t v) const noexcept {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<int32_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<uint32_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(uint32_t v) const noexcept {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<__m256i_t, int32_t> {
    WJR_CONST WJR_INTRINSIC_INLINE int32_t operator()(__m256i v) const noexcept {
        return simd_cast<__m128i_t, int32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<__m256i_t, uint32_t> {
    WJR_CONST WJR_INTRINSIC_INLINE uint32_t operator()(__m256i v) const noexcept {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<int64_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(int64_t v) const noexcept {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<int64_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<uint64_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(uint64_t v) const noexcept {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint64_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<__m256i_t, int64_t> {
    WJR_CONST WJR_INTRINSIC_INLINE int64_t operator()(__m256i v) const noexcept {
        return simd_cast<__m128i_t, int64_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<__m256i_t, uint64_t> {
    WJR_CONST WJR_INTRINSIC_INLINE uint64_t operator()(__m256i v) const noexcept {
        return simd_cast<__m128i_t, uint64_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

#endif // AVX

} // namespace wjr

#endif // WJR_ARCH_X86_SIMD_SIMD_CAST_HPP__