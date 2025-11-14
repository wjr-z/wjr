#ifndef WJR_ARCH_X86_SIMD_SSE_HPP__
#define WJR_ARCH_X86_SIMD_SSE_HPP__

#include <cstring>

#include <wjr/arch/x86/simd/simd_cast.hpp>
#include <wjr/math/broadcast.hpp>
#include <wjr/math/clz.hpp>
#include <wjr/math/ctz.hpp>
#include <wjr/memory/detail.hpp>

namespace wjr {

// LCOV_EXCL_START

struct sse {
    using mask_type = uint16_t;

#if WJR_HAS_SIMD(SSE)

    using float_type = __m128;
    using float_tag_type = _m128_t;

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

    using int_type = __m128i;
    using int_tag_type = _m128i_t;
    using double_type = __m128d;
    using double_tag_type = _m128d_t;

#endif // SSE2

    constexpr static size_t width() noexcept;
    constexpr static mask_type mask() noexcept;

    WJR_INTRINSIC_INLINE static int clz_nz_epu64(mask_type x) noexcept;
    WJR_INTRINSIC_INLINE static int clz_nz(mask_type x, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static int ctz_nz_epu64(mask_type x) noexcept;
    WJR_INTRINSIC_INLINE static int ctz_nz(mask_type x, uint64_t) noexcept;

#if WJR_HAS_SIMD(SSE)

    WJR_INTRINSIC_INLINE static mask_type movemask_ps(__m128 v) noexcept;
    WJR_INTRINSIC_INLINE static void sfence() noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128 shuffle_ps(__m128 a, __m128 b) noexcept;

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

    WJR_INTRINSIC_INLINE static __m128i add_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add_epi32(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add_epi64(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static int8_t add_epi8(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static int16_t add_epi16(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static int32_t add_epi32(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static int64_t add_epi64(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static int8_t add(__m128i a, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static int16_t add(__m128i a, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static int32_t add(__m128i a, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static int64_t add(__m128i a, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static uint8_t add(__m128i a, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t add(__m128i a, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t add(__m128i a, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static uint64_t add(__m128i a, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i adds_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i adds_epi16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i adds_epu8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i adds_epu16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint16_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i alignr_epi16(__m128i a, __m128i b, int c) noexcept;
    WJR_INTRINSIC_INLINE static __m128i alignr_epi32(__m128i a, __m128i b, int c) noexcept;
    WJR_INTRINSIC_INLINE static __m128i alignr_epi64(__m128i a, __m128i b, int c) noexcept;

    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i And(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i AndNot(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i avg_epu8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i avg_epu16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint16_t) noexcept;

    // notice that mask must be 0 or 255(every byte)
    WJR_INTRINSIC_INLINE static __m128i blendv_epi8(__m128i a, __m128i b, __m128i mask) noexcept;
    WJR_INTRINSIC_INLINE static __m128i blendv_epi16(__m128i a, __m128i b, __m128i mask) noexcept;
    WJR_INTRINSIC_INLINE static __m128i blendv_epi32(__m128i a, __m128i b, __m128i mask) noexcept;

    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               uint32_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i bslli(__m128i val) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i bsrli(__m128i val) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpeq_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpeq_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpeq_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpge_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpge_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpge_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpge_epu8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpge_epu16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpge_epu32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpgt_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpgt_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpgt_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpgt_epu8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpgt_epu16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpgt_epu32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmple_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmple_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmple_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmple_epu8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmple_epu16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmple_epu32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmplt_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmplt_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmplt_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmplt_epu8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmplt_epu16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmplt_epu32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpne_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpne_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpne_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint32_t) noexcept;

    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::equal_to<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::not_equal_to<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater_equal<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less_equal<>, T) noexcept;

    WJR_INTRINSIC_INLINE static __m128i concat(uint64_t lo, uint64_t hi) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi8(__m128i a) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi16(__m128i a) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi32(__m128i a) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m128i a) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, int8_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, int16_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, int32_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, int64_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, uint8_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, uint16_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, uint32_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static uint64_t getlow(__m128i v) noexcept;
    WJR_INTRINSIC_INLINE static uint64_t gethigh(__m128i v) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert_epi16(__m128i a, int i) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int16_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint16_t) noexcept;

    WJR_INTRINSIC_INLINE static void lfence() noexcept;

    WJR_INTRINSIC_INLINE static __m128i load(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m128i loadu(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m128i loadu_si16(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m128i loadu_si32(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m128i loadu_si64(const void *ptr) noexcept;

    template <simd_integral T>
    WJR_INTRINSIC_INLINE static __m128i logical_and(__m128i a, __m128i b, T) noexcept;

    template <simd_integral T>
    WJR_INTRINSIC_INLINE static __m128i logical_not(__m128i v, T) noexcept;

    template <simd_integral T>
    WJR_INTRINSIC_INLINE static __m128i logical_or(__m128i a, __m128i b, T) noexcept;

    WJR_INTRINSIC_INLINE static __m128i madd_epi16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static void maskmoveu(__m128i a, __m128i mask, char *mem_addr) noexcept;

    WJR_INTRINSIC_INLINE static __m128i max_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i max_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i max_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i max_epu8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i max_epu16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i max_epu32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static int8_t max_epi8(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static int16_t max_epi16(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static int32_t max_epi32(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static int8_t max(__m128i a, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static int16_t max(__m128i a, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static int32_t max(__m128i a, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static uint8_t max(__m128i a, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t max(__m128i a, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t max(__m128i a, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static void mfence() noexcept;

    WJR_INTRINSIC_INLINE static __m128i min_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i min_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i min_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i min_epu8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i min_epu16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i min_epu32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static int8_t min_epi8(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static int16_t min_epi16(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static int32_t min_epi32(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static int8_t min(__m128i a, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static int16_t min(__m128i a, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static int32_t min(__m128i a, int32_t) noexcept;

    WJR_INTRINSIC_INLINE static uint8_t min(__m128i a, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t min(__m128i a, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t min(__m128i a, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i move_epi64(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static mask_type movemask_pd(__m128d v) noexcept;

    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int64_t) noexcept;

    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i mul_epu32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i mulhi_epi16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i mulhi_epu16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i mullo_epi16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i negate_epi8(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate_epi16(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate_epi32(__m128i a) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate_epi64(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i Not(__m128i v) noexcept;

    WJR_INTRINSIC_INLINE static __m128i Or(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i packs_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i packs_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i packus_epi16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i loadu_si48(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m128i loadu_si80(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m128i loadu_si96(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m128i loadu_si112(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m128i loadu_si128(const void *ptr) noexcept;

    WJR_INTRINSIC_INLINE static __m128i loadu_si16x(const void *ptr, int n) noexcept;

    WJR_INTRINSIC_INLINE static __m128i sad_epu8(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i zeros() noexcept;
    WJR_INTRINSIC_INLINE static __m128i ones() noexcept;

    WJR_INTRINSIC_INLINE static __m128i set_epi8(char e15, char e14, char e13, char e12, char e11,
                                                 char e10, char e9, char e8, char e7, char e6,
                                                 char e5, char e4, char e3, char e2, char e1,
                                                 char e0) noexcept;

    WJR_INTRINSIC_INLINE static __m128i set_epi16(short e7, short e6, short e5, short e4, short e3,
                                                  short e2, short e1, short e0) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set_epi32(int e3, int e2, int e1, int e0) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set_epi64x(long long e1, long long e0) noexcept;

    WJR_INTRINSIC_INLINE static __m128i setr_epi8(char e15, char e14, char e13, char e12, char e11,
                                                  char e10, char e9, char e8, char e7, char e6,
                                                  char e5, char e4, char e3, char e2, char e1,
                                                  char e0) noexcept;

    WJR_INTRINSIC_INLINE static __m128i setr_epi16(short e7, short e6, short e5, short e4, short e3,
                                                   short e2, short e1, short e0) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setr_epi32(int e3, int e2, int e1, int e0) noexcept;

    WJR_INTRINSIC_INLINE static __m128i set1_epi8(int8_t val) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1_epi16(int16_t val) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1_epi32(int32_t val) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1_epi64(int64_t val) noexcept;

    WJR_INTRINSIC_INLINE static __m128i set1(int8_t val, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1(int16_t val, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1(int32_t val, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1(int64_t val, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1(uint8_t val, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1(uint16_t val, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1(uint32_t val, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i set1(uint64_t val, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i setmin_epi8() noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmin_epi16() noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmin_epi32() noexcept;

    WJR_INTRINSIC_INLINE static __m128i setmin(int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmin(int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmin(int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmin(uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmin(uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmin(uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i setmax_epi8() noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmax_epi16() noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmax_epi32() noexcept;

    WJR_INTRINSIC_INLINE static __m128i setmax(int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmax(int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmax(int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmax(uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmax(uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i setmax(uint32_t) noexcept;

    template <int imm>
    WJR_INTRINSIC_INLINE static __m128i shl(__m128i a) noexcept;

    template <int imm>
    WJR_INTRINSIC_INLINE static __m128i shr(__m128i b) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i shuffle_epi32(__m128i v) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i shufflehi_epi16(__m128i v) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i shufflelo_epi16(__m128i v) noexcept;

    WJR_INTRINSIC_INLINE static __m128i sll_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sll_epi32(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sll_epi64(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint64_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i v) noexcept;
    WJR_INTRINSIC_INLINE static __m128i slli_epi16(__m128i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m128i slli_epi32(__m128i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m128i slli_epi64(__m128i a, int imm8) noexcept;

    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i sra_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sra_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i srai_epi16(__m128i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srai_epi32(__m128i a, int imm8) noexcept;

    WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i srl_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srl_epi32(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srl_epi64(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint64_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i v) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli_epi8(__m128i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli_epi16(__m128i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli_epi32(__m128i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli_epi64(__m128i a, int imm8) noexcept;

    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static void stream(void *p, __m128i a) noexcept;
    WJR_INTRINSIC_INLINE static void store(void *p, __m128i a) noexcept;
    WJR_INTRINSIC_INLINE static void storeu(void *p, __m128i a) noexcept;

    WJR_INTRINSIC_INLINE static __m128i sub_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub_epi32(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub_epi64(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i subs_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i subs_epi16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i subs_epu8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i subs_epu16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint16_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i unpackhi_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi_epi32(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi_epi64(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i unpacklo_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpacklo_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpacklo_epi32(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpacklo_epi64(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i Xor(__m128i a, __m128i b) noexcept;

#endif // SSE2

#if WJR_HAS_SIMD(SSE3)

    WJR_INTRINSIC_INLINE static __m128i lddqu(const __m128i *ptr) noexcept;

#endif // SSE3

#if WJR_HAS_SIMD(SSSE3)

    WJR_INTRINSIC_INLINE static __m128i abs_epi8(__m128i val) noexcept;
    WJR_INTRINSIC_INLINE static __m128i abs_epi16(__m128i val) noexcept;
    WJR_INTRINSIC_INLINE static __m128i abs_epi32(__m128i val) noexcept;

    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i shuffle_epi8(__m128i v, __m128i imm8) noexcept;

    WJR_INTRINSIC_INLINE static __m128i sign_epi8(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sign_epi16(__m128i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sign_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint32_t) noexcept;

#endif // SSSE3

#if WJR_HAS_SIMD(SSE4_1)

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i blend_epi16(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpeq_epi64(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpgt_epi64(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int64_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert_epi8(__m128i a, int i) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert_epi32(__m128i a, int i) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert_epi64(__m128i a, int64_t i) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int8_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int32_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, int64_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint8_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint32_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m128i minpos_epu16(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static __m128i mul_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i mullo_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i packus_epi32(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m128i stream_load(void *p) noexcept;

    WJR_INTRINSIC_INLINE static int test_all_ones(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a) noexcept;

    WJR_INTRINSIC_INLINE static int test_mix_ones_zeros(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static int testc(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static int testnzc(__m128i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static int testz(__m128i a, __m128i b) noexcept;

#endif // SSE4_1
};

namespace sse_detail {
namespace {
#if WJR_HAS_SIMD(SSE2)

static const __m128i srli_epi8_mask[8] = {
    sse::set1_epi16(0xFFFF), sse::set1_epi16(0x7F7F), sse::set1_epi16(0x3F3F),
    sse::set1_epi16(0x1F1F), sse::set1_epi16(0xF0F),  sse::set1_epi16(0x707),
    sse::set1_epi16(0x303),  sse::set1_epi16(0x101),
};

static const __m128i ascii_base_10 = sse::set1_epi8('0');

#endif
} // namespace
} // namespace sse_detail

#if WJR_HAS_SIMD(SSE2)

template <>
struct broadcast_fn<uint8_t, _m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(uint8_t v) const noexcept {
        return _mm_set1_epi8(v);
    }
};

template <>
struct broadcast_fn<uint16_t, _m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(uint16_t v) const noexcept {
        return _mm_set1_epi16(v);
    }
};

template <>
struct broadcast_fn<uint32_t, _m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(uint32_t v) const noexcept {
        return _mm_set1_epi32(v);
    }
};

template <>
struct broadcast_fn<uint64_t, _m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(uint64_t v) const noexcept {
        return _mm_set1_epi64x(v);
    }
};

template <>
struct broadcast_fn<_m128i_t, _m128i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m128i operator()(__m128i v) const noexcept { return v; }
};

#endif // SSE2

/*------------------------sse------------------------*/

constexpr size_t sse::width() noexcept { return 128; }
constexpr sse::mask_type sse::mask() noexcept { return 0xFFFF; }

int sse::clz_nz_epu64(mask_type x) noexcept {
    // x == (0xFFFF || 0xFF00 || 0x00FF)
    return x == 0x00FF ? 8 : 0;
}

int sse::clz_nz(mask_type x, uint64_t) noexcept { return clz_nz_epu64(x); }

int sse::ctz_nz_epu64(mask_type x) noexcept {
    // x == (0xFFFF || 0xFF00 || 0x00FF)
    return x == 0xFF00 ? 8 : 0;
}

int sse::ctz_nz(mask_type x, uint64_t) noexcept { return ctz_nz_epu64(x); }

#if WJR_HAS_SIMD(SSE)

sse::mask_type sse::movemask_ps(__m128 v) noexcept {
    return static_cast<sse::mask_type>(_mm_movemask_ps(v));
}

void sse::sfence() noexcept { return _mm_sfence(); }

template <int imm8>
__m128 sse::shuffle_ps(__m128 a, __m128 b) noexcept {
    static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
    return _mm_shuffle_ps(a, b, imm8);
}

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

__m128i sse::add_epi8(__m128i a, __m128i b) noexcept { return _mm_add_epi8(a, b); }
__m128i sse::add_epi16(__m128i a, __m128i b) noexcept { return _mm_add_epi16(a, b); }
__m128i sse::add_epi32(__m128i a, __m128i b) noexcept { return _mm_add_epi32(a, b); }
__m128i sse::add_epi64(__m128i a, __m128i b) noexcept { return _mm_add_epi64(a, b); }

__m128i sse::add(__m128i a, __m128i b, int8_t) noexcept { return add_epi8(a, b); }
__m128i sse::add(__m128i a, __m128i b, int16_t) noexcept { return add_epi16(a, b); }
__m128i sse::add(__m128i a, __m128i b, int32_t) noexcept { return add_epi32(a, b); }
__m128i sse::add(__m128i a, __m128i b, int64_t) noexcept { return add_epi64(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint8_t) noexcept { return add_epi8(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint16_t) noexcept { return add_epi16(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint32_t) noexcept { return add_epi32(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint64_t) noexcept { return add_epi64(a, b); }

int8_t sse::add_epi8(__m128i a) noexcept { return static_cast<int8_t>(add_epu8(a)); }
int16_t sse::add_epi16(__m128i a) noexcept { return static_cast<int16_t>(add_epu16(a)); }
int32_t sse::add_epi32(__m128i a) noexcept { return static_cast<int32_t>(add_epu32(a)); }
int64_t sse::add_epi64(__m128i a) noexcept { return static_cast<int64_t>(add_epu64(a)); }

uint8_t sse::add_epu8(__m128i a) noexcept {
    auto b = shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a);
    a = add(a, b, uint8_t());
    b = zeros();
    a = sad_epu8(a, b);
    return simd_cast<_m128i_t, uint8_t>(a);
}

uint16_t sse::add_epu16(__m128i a) noexcept {
    a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint16_t());
    a = add(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a), uint16_t());
    a = add(a, srli<2>(a), uint16_t());
    return simd_cast<_m128i_t, uint16_t>(a);
}

uint32_t sse::add_epu32(__m128i a) noexcept {
    a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint32_t());
    a = add(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a), uint32_t());
    return simd_cast<_m128i_t, uint32_t>(a);
}

uint64_t sse::add_epu64(__m128i a) noexcept {
    a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint64_t());
    return simd_cast<_m128i_t, uint64_t>(a);
}

int8_t sse::add(__m128i a, int8_t) noexcept { return add_epi8(a); }
int16_t sse::add(__m128i a, int16_t) noexcept { return add_epi16(a); }
int32_t sse::add(__m128i a, int32_t) noexcept { return add_epi32(a); }
int64_t sse::add(__m128i a, int64_t) noexcept { return add_epi64(a); }
uint8_t sse::add(__m128i a, uint8_t) noexcept { return add_epu8(a); }
uint16_t sse::add(__m128i a, uint16_t) noexcept { return add_epu16(a); }
uint32_t sse::add(__m128i a, uint32_t) noexcept { return add_epu32(a); }
uint64_t sse::add(__m128i a, uint64_t) noexcept { return add_epu64(a); }

__m128i sse::adds_epi8(__m128i a, __m128i b) noexcept { return _mm_adds_epi8(a, b); }
__m128i sse::adds_epi16(__m128i a, __m128i b) noexcept { return _mm_adds_epi16(a, b); }

__m128i sse::adds_epu8(__m128i a, __m128i b) noexcept { return _mm_adds_epu8(a, b); }
__m128i sse::adds_epu16(__m128i a, __m128i b) noexcept { return _mm_adds_epu16(a, b); }

__m128i sse::adds(__m128i a, __m128i b, int8_t) noexcept { return adds_epi8(a, b); }
__m128i sse::adds(__m128i a, __m128i b, int16_t) noexcept { return adds_epi16(a, b); }
__m128i sse::adds(__m128i a, __m128i b, uint8_t) noexcept { return adds_epu8(a, b); }
__m128i sse::adds(__m128i a, __m128i b, uint16_t) noexcept { return adds_epu16(a, b); }

template <int imm8>
__m128i sse::alignr(__m128i a, __m128i b) noexcept {
    static_assert(imm8 >= 0 && imm8 <= 16, "imm8 must be in range [0, 16]");
    constexpr int s = imm8 & 0x1F;
    #if WJR_HAS_SIMD(SSSE3)
    return _mm_alignr_epi8(a, b, s);
    #else
    if constexpr (s == 0) {
        return b;
    }
    if constexpr (s == 16) {
        return a;
    }
    if constexpr (s < 16) {
        if constexpr (s == 8) {
            return _mm_castps_si128(
                shuffle_ps<_MM_SHUFFLE(1, 0, 3, 2)>(_mm_castsi128_ps(b), _mm_castsi128_ps(a)));
        } else {
            return Or(slli<16 - s>(a), srli<s>(b));
        }
    }
    WJR_UNREACHABLE();
    #endif // SSSE3
}

__m128i sse::alignr_epi16(__m128i a, __m128i b, int c) noexcept {
    return Or(slli(a, 16 - c, uint16_t()), srli(b, c, uint16_t()));
}

__m128i sse::alignr_epi32(__m128i a, __m128i b, int c) noexcept {
    return Or(slli(a, 32 - c, uint32_t()), srli(b, c, uint32_t()));
}

__m128i sse::alignr_epi64(__m128i a, __m128i b, int c) noexcept {
    return Or(slli(a, 64 - c, uint64_t()), srli(b, c, uint64_t()));
}

__m128i sse::alignr(__m128i a, __m128i b, int c, int16_t) noexcept { return alignr_epi16(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, int32_t) noexcept { return alignr_epi32(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, int64_t) noexcept { return alignr_epi64(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, uint16_t) noexcept {
    return alignr_epi16(a, b, c);
}
__m128i sse::alignr(__m128i a, __m128i b, int c, uint32_t) noexcept {
    return alignr_epi32(a, b, c);
}
__m128i sse::alignr(__m128i a, __m128i b, int c, uint64_t) noexcept {
    return alignr_epi64(a, b, c);
}

__m128i sse::And(__m128i a, __m128i b) noexcept { return _mm_and_si128(a, b); }

__m128i sse::AndNot(__m128i a, __m128i b) noexcept { return _mm_andnot_si128(a, b); }

__m128i sse::avg_epu8(__m128i a, __m128i b) noexcept { return _mm_avg_epu8(a, b); }
__m128i sse::avg_epu16(__m128i a, __m128i b) noexcept { return _mm_avg_epu16(a, b); }

__m128i sse::avg(__m128i a, __m128i b, int8_t) noexcept { return avg_epu8(a, b); }
__m128i sse::avg(__m128i a, __m128i b, int16_t) noexcept { return avg_epu16(a, b); }
__m128i sse::avg(__m128i a, __m128i b, uint8_t) noexcept { return avg_epu8(a, b); }
__m128i sse::avg(__m128i a, __m128i b, uint16_t) noexcept { return avg_epu16(a, b); }

// notice that mask must be 0 or 255(every byte)
__m128i sse::blendv_epi8(__m128i a, __m128i b, __m128i mask) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_blendv_epi8(a, b, mask);
    #elif defined(WJR_COMPILER_GCC)
    return ((~mask) & a) | (mask & b);
    #else
    return Or(AndNot(mask, a), And(mask, b));
    #endif
}

__m128i sse::blendv_epi16(__m128i a, __m128i b, __m128i mask) noexcept {
    return blendv_epi8(b, a, logical_not(mask, uint16_t()));
}

__m128i sse::blendv_epi32(__m128i a, __m128i b, __m128i mask) noexcept {
    return blendv_epi8(b, a, logical_not(mask, uint32_t()));
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int8_t) noexcept {
    return blendv_epi8(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int16_t) noexcept {
    return blendv_epi16(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int32_t) noexcept {
    return blendv_epi32(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint8_t) noexcept {
    return blendv_epi8(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint16_t) noexcept {
    return blendv_epi16(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint32_t) noexcept {
    return blendv_epi32(a, b, mask);
}

template <int imm8>
__m128i sse::bslli(__m128i val) noexcept {
    return _mm_bslli_si128(val, imm8);
}

template <int imm8>
__m128i sse::bsrli(__m128i val) noexcept {
    return _mm_bsrli_si128(val, imm8);
}

__m128i sse::cmpeq_epi8(__m128i a, __m128i b) noexcept { return _mm_cmpeq_epi8(a, b); }
__m128i sse::cmpeq_epi16(__m128i a, __m128i b) noexcept { return _mm_cmpeq_epi16(a, b); }
__m128i sse::cmpeq_epi32(__m128i a, __m128i b) noexcept { return _mm_cmpeq_epi32(a, b); }

__m128i sse::cmpeq(__m128i a, __m128i b, int8_t) noexcept { return cmpeq_epi8(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int16_t) noexcept { return cmpeq_epi16(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int32_t) noexcept { return cmpeq_epi32(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint8_t) noexcept { return cmpeq_epi8(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint16_t) noexcept { return cmpeq_epi16(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint32_t) noexcept { return cmpeq_epi32(a, b); }

__m128i sse::cmpge_epi8(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comge_epi8(a, b);
    #elif WJR_HAS_SIMD(SSE4_1)
    return cmpeq(min(a, b, int8_t()), b, uint8_t());
    #else
    return Not(cmpgt(b, a, int8_t()));
    #endif
}

__m128i sse::cmpge_epi16(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comge_epi16(a, b);
    #else
    return cmpeq(min(a, b, int16_t()), b, uint16_t());
    #endif
}

__m128i sse::cmpge_epi32(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comge_epi32(a, b);
    #elif WJR_HAS_SIMD(SSE4_1)
    return cmpeq(min(a, b, int32_t()), b, uint32_t());
    #else
    return Not(cmpgt(b, a, int32_t()));
    #endif
}

__m128i sse::cmpge_epu8(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comge_epu8(a, b);
    #else
    return cmpeq(min(a, b, uint8_t()), b, uint8_t());
    #endif
}

__m128i sse::cmpge_epu16(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comge_epu16(a, b);
    #elif WJR_HAS_SIMD(SSE4_1)
    return cmpeq(min(a, b, uint16_t()), b, uint16_t());
    #else
    return logical_not(subs(b, a, uint16_t()), uint16_t());
    #endif
}

__m128i sse::cmpge_epu32(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comge_epu32(a, b);
    #elif WJR_HAS_SIMD(SSE4_1)
    return cmpeq(min(a, b, uint32_t()), b, uint32_t());
    #else
    return Not(cmpgt(b, a, uint32_t()));
    #endif
}

__m128i sse::cmpge(__m128i a, __m128i b, int8_t) noexcept { return cmpge_epi8(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, int16_t) noexcept { return cmpge_epi16(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, int32_t) noexcept { return cmpge_epi32(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint8_t) noexcept { return cmpge_epu8(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint16_t) noexcept { return cmpge_epu16(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint32_t) noexcept { return cmpge_epu32(a, b); }

__m128i sse::cmpgt_epi8(__m128i a, __m128i b) noexcept { return _mm_cmpgt_epi8(a, b); }
__m128i sse::cmpgt_epi16(__m128i a, __m128i b) noexcept { return _mm_cmpgt_epi16(a, b); }
__m128i sse::cmpgt_epi32(__m128i a, __m128i b) noexcept { return _mm_cmpgt_epi32(a, b); }

__m128i sse::cmpgt_epu8(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comgt_epu8(a, b);
    #else
    return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
    #endif
}

__m128i sse::cmpgt_epu16(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comgt_epu16(a, b);
    #else
    return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
    #endif
}

__m128i sse::cmpgt_epu32(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comgt_epu32(a, b);
    #else
    return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
    #endif
}

__m128i sse::cmpgt(__m128i a, __m128i b, int8_t) noexcept { return cmpgt_epi8(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, int16_t) noexcept { return cmpgt_epi16(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, int32_t) noexcept { return cmpgt_epi32(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint8_t) noexcept { return cmpgt_epu8(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint16_t) noexcept { return cmpgt_epu16(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint32_t) noexcept { return cmpgt_epu32(a, b); }

__m128i sse::cmple_epi8(__m128i a, __m128i b) noexcept { return cmpge_epi8(b, a); }
__m128i sse::cmple_epi16(__m128i a, __m128i b) noexcept { return cmpge_epi16(b, a); }
__m128i sse::cmple_epi32(__m128i a, __m128i b) noexcept { return cmpge_epi32(b, a); }

__m128i sse::cmple_epu8(__m128i a, __m128i b) noexcept { return cmpge_epu8(b, a); }
__m128i sse::cmple_epu16(__m128i a, __m128i b) noexcept { return cmpge_epu16(b, a); }
__m128i sse::cmple_epu32(__m128i a, __m128i b) noexcept { return cmpge_epu32(b, a); }

__m128i sse::cmple(__m128i a, __m128i b, int8_t) noexcept { return cmple_epi8(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, int16_t) noexcept { return cmple_epi16(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, int32_t) noexcept { return cmple_epi32(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint8_t) noexcept { return cmple_epu8(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint16_t) noexcept { return cmple_epu16(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint32_t) noexcept { return cmple_epu32(a, b); }

__m128i sse::cmplt_epi8(__m128i a, __m128i b) noexcept { return _mm_cmplt_epi8(a, b); }
__m128i sse::cmplt_epi16(__m128i a, __m128i b) noexcept { return _mm_cmplt_epi16(a, b); }
__m128i sse::cmplt_epi32(__m128i a, __m128i b) noexcept { return _mm_cmplt_epi32(a, b); }

__m128i sse::cmplt_epu8(__m128i a, __m128i b) noexcept { return cmpgt_epu8(b, a); }
__m128i sse::cmplt_epu16(__m128i a, __m128i b) noexcept { return cmpgt_epu16(b, a); }
__m128i sse::cmplt_epu32(__m128i a, __m128i b) noexcept { return cmpgt_epu32(b, a); }

__m128i sse::cmplt(__m128i a, __m128i b, int8_t) noexcept { return cmplt_epi8(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, int16_t) noexcept { return cmplt_epi16(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, int32_t) noexcept { return cmplt_epi32(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint8_t) noexcept { return cmplt_epu8(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint16_t) noexcept { return cmplt_epu16(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint32_t) noexcept { return cmplt_epu32(a, b); }

__m128i sse::cmpne_epi8(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comneq_epi8(a, b);
    #else
    return Not(cmpeq_epi8(a, b));
    #endif
}

__m128i sse::cmpne_epi16(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comneq_epi16(a, b);
    #else
    return Not(cmpeq_epi16(a, b));
    #endif
}

__m128i sse::cmpne_epi32(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(XOP)
    return _mm_comneq_epi32(a, b);
    #else
    return Not(cmpeq_epi32(a, b));
    #endif
}

__m128i sse::cmpne(__m128i a, __m128i b, int8_t) noexcept { return cmpne_epi8(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, int16_t) noexcept { return cmpne_epi16(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, int32_t) noexcept { return cmpne_epi32(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint8_t) noexcept { return cmpne_epi8(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint16_t) noexcept { return cmpne_epi16(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint32_t) noexcept { return cmpne_epi32(a, b); }

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::equal_to<>, T) noexcept {
    return cmpeq(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::not_equal_to<>, T) noexcept {
    return cmpne(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::greater<>, T) noexcept {
    return cmpgt(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::greater_equal<>, T) noexcept {
    return cmpge(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::less<>, T) noexcept {
    return cmplt(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::less_equal<>, T) noexcept {
    return cmple(a, b, T());
}

__m128i sse::concat(uint64_t lo, uint64_t hi) noexcept { return set_epi64x(hi, lo); }

template <int imm8>
int sse::extract_epi8(__m128i a) noexcept {
    static_assert(imm8 >= 0 && imm8 < 16, "imm8 must be in range [0, 15]");
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_extract_epi8(a, imm8);
    #else
    if constexpr (imm8 & 1) {
        return extract_epi16<(imm8 >> 1)>(a) >> 8;
    } else {
        return extract_epi16<(imm8 >> 1)>(a) & 0xff;
    }
    #endif
}

template <int imm8>
int sse::extract_epi16(__m128i a) noexcept {
    static_assert(imm8 >= 0 && imm8 < 8, "imm8 must be in range [0, 7]");
    return _mm_extract_epi16(a, imm8);
}

template <int imm8>
int sse::extract_epi32(__m128i a) noexcept {
    static_assert(imm8 >= 0 && imm8 < 4, "imm8 must be in range [0, 3]");
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_extract_epi32(a, imm8);
    #else
    if constexpr (imm8 == 0) {
        return simd_cast<_m128i_t, uint32_t>(a);
    } else if constexpr (imm8 == 1) {
        return static_cast<uint32_t>(simd_cast<_m128i_t, uint64_t>(a) >> 32);
    } else if constexpr (imm8 == 2) {
        return simd_cast<_m128i_t, uint32_t>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    } else {
        return simd_cast<_m128i_t, uint32_t>(shuffle_epi32<_MM_SHUFFLE(3, 3, 3, 3)>(a));
    }
    #endif
}

template <int imm8>
int64_t sse::extract_epi64(__m128i a) noexcept {
    static_assert(imm8 >= 0 && imm8 < 2, "imm8 must be in range [0, 1]");
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_extract_epi64(a, imm8);
    #else
    if constexpr (imm8 == 0) {
        return simd_cast<_m128i_t, uint64_t>(a);
    } else {
        return simd_cast<_m128i_t, uint64_t>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    }
    #endif
}

template <int imm8>
int sse::extract(__m128i a, int8_t) noexcept {
    return extract_epi8<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, int16_t) noexcept {
    return extract_epi16<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, int32_t) noexcept {
    return extract_epi32<imm8>(a);
}

template <int imm8>
int64_t sse::extract(__m128i a, int64_t) noexcept {
    return extract_epi64<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, uint8_t) noexcept {
    return extract_epi8<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, uint16_t) noexcept {
    return extract_epi16<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, uint32_t) noexcept {
    return extract_epi32<imm8>(a);
}

template <int imm8>
int64_t sse::extract(__m128i a, uint64_t) noexcept {
    return extract_epi64<imm8>(a);
}

uint64_t sse::getlow(__m128i v) noexcept { return simd_cast<_m128i_t, uint64_t>(v); }
uint64_t sse::gethigh(__m128i v) noexcept { return extract_epi64<1>(v); }

template <int imm8>
__m128i sse::insert_epi16(__m128i a, int i) noexcept {
    return _mm_insert_epi16(a, i, imm8);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, int16_t) noexcept {
    return insert_epi16<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, uint16_t) noexcept {
    return insert_epi16<imm8>(a, i);
}

void sse::lfence() noexcept { _mm_lfence(); }

__m128i sse::load(const void *ptr) noexcept {
    return _mm_load_si128(static_cast<const __m128i *>(ptr));
}
__m128i sse::loadu(const void *ptr) noexcept {
    return _mm_loadu_si128(static_cast<const __m128i *>(ptr));
}
__m128i sse::loadu_si16(const void *ptr) noexcept {
    return simd_cast<uint16_t, _m128i_t>(read_memory<uint16_t>(ptr));
}

__m128i sse::loadu_si32(const void *ptr) noexcept {
    return simd_cast<uint32_t, _m128i_t>(read_memory<uint32_t>(ptr));
}

__m128i sse::loadu_si64(const void *ptr) noexcept {
    return simd_cast<uint64_t, _m128i_t>(read_memory<uint64_t>(ptr));
}

template <simd_integral T>
__m128i sse::logical_and(__m128i a, __m128i b, T) noexcept {
    return Not(Or(logical_not(a, T()), logical_not(b, T())));
}

template <simd_integral T>
__m128i sse::logical_not(__m128i v, T) noexcept {
    auto Zero = zeros();
    return cmpeq(v, Zero, T());
}

template <simd_integral T>
__m128i sse::logical_or(__m128i a, __m128i b, T) noexcept {
    return Not(logical_not(Or(a, b), T()));
}

__m128i sse::madd_epi16(__m128i a, __m128i b) noexcept { return _mm_madd_epi16(a, b); }

void sse::maskmoveu(__m128i a, __m128i mask, char *mem_addr) noexcept {
    return _mm_maskmoveu_si128(a, mask, mem_addr);
}

__m128i sse::max_epi8(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_max_epi8(a, b);
    #else
    return blendv_epi8(b, a, cmpgt_epi8(a, b));
    #endif
}

__m128i sse::max_epi16(__m128i a, __m128i b) noexcept { return _mm_max_epi16(a, b); }

__m128i sse::max_epi32(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_max_epi32(a, b);
    #else
    return blendv_epi8(b, a, cmpgt_epi32(a, b));
    #endif
}

__m128i sse::max_epu8(__m128i a, __m128i b) noexcept { return _mm_max_epu8(a, b); }

__m128i sse::max_epu16(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_max_epu16(a, b);
    #else
    return add(subs_epu16(b, a), a, uint16_t());
    #endif
}

__m128i sse::max_epu32(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_max_epu32(a, b);
    #else
    return blendv_epi8(b, a, cmpgt_epu32(a, b));
    #endif
}

__m128i sse::max(__m128i a, __m128i b, int8_t) noexcept { return max_epi8(a, b); }
__m128i sse::max(__m128i a, __m128i b, int16_t) noexcept { return max_epi16(a, b); }
__m128i sse::max(__m128i a, __m128i b, int32_t) noexcept { return max_epi32(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint8_t) noexcept { return max_epu8(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint16_t) noexcept { return max_epu16(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint32_t) noexcept { return max_epu32(a, b); }

int8_t sse::max_epi8(__m128i a) noexcept { return 0x7fu ^ min_epu8(Xor(a, set1_epi8(0x7fu))); }

int16_t sse::max_epi16(__m128i a) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return 0x7fffu ^ min_epu16(Xor(a, set1_epi16(0x7fffu)));
    #else
    a = max_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    return simd_cast<_m128i_t, int16_t>(a);
    #endif
}

int32_t sse::max_epi32(__m128i a) noexcept {
    a = max_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    return simd_cast<_m128i_t, int32_t>(a);
}

uint8_t sse::max_epu8(__m128i a) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return 0xffu ^ min_epu8(Xor(a, ones()));
    #else
    a = max_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    auto X = simd_cast<_m128i_t, uint32_t>(a);
    return std::max((uint8_t)X, (uint8_t)(X >> 8));
    #endif
}

uint16_t sse::max_epu16(__m128i a) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return 0xffffu ^ min_epu16(Xor(a, ones()));
    #else
    a = max_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    return simd_cast<_m128i_t, uint16_t>(a);
    #endif
}

uint32_t sse::max_epu32(__m128i a) noexcept {
    a = max_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    return simd_cast<_m128i_t, uint32_t>(a);
}

int8_t sse::max(__m128i a, int8_t) noexcept { return max_epi8(a); }
int16_t sse::max(__m128i a, int16_t) noexcept { return max_epi16(a); }
int32_t sse::max(__m128i a, int32_t) noexcept { return max_epi32(a); }
uint8_t sse::max(__m128i a, uint8_t) noexcept { return max_epu8(a); }
uint16_t sse::max(__m128i a, uint16_t) noexcept { return max_epu16(a); }
uint32_t sse::max(__m128i a, uint32_t) noexcept { return max_epu32(a); }

void sse::mfence() noexcept { _mm_mfence(); }

__m128i sse::min_epi8(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_min_epi8(a, b);
    #else
    return blendv_epi8(a, b, cmpgt_epi8(a, b));
    #endif
}

__m128i sse::min_epi16(__m128i a, __m128i b) noexcept { return _mm_min_epi16(a, b); }

__m128i sse::min_epi32(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_min_epi32(a, b);
    #else
    return blendv_epi8(a, b, cmpgt_epi32(a, b));
    #endif
}

__m128i sse::min_epu8(__m128i a, __m128i b) noexcept { return _mm_min_epu8(a, b); }

__m128i sse::min_epu16(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_min_epu16(a, b);
    #else
    return blendv_epi8(a, b, cmpgt_epu16(a, b));
    #endif
}

__m128i sse::min_epu32(__m128i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return _mm_min_epu32(a, b);
    #else
    return blendv_epi8(a, b, cmpgt_epu32(a, b));
    #endif
}

__m128i sse::min(__m128i a, __m128i b, int8_t) noexcept { return min_epi8(a, b); }
__m128i sse::min(__m128i a, __m128i b, int16_t) noexcept { return min_epi16(a, b); }
__m128i sse::min(__m128i a, __m128i b, int32_t) noexcept { return min_epi32(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint8_t) noexcept { return min_epu8(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint16_t) noexcept { return min_epu16(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint32_t) noexcept { return min_epu32(a, b); }

int8_t sse::min_epi8(__m128i a) noexcept { return 0x80u ^ min_epu8(Xor(a, setmin_epi8())); }

int16_t sse::min_epi16(__m128i a) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return 0x8000u ^ min_epu16(Xor(a, setmin_epi16()));
    #else
    a = min_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    return simd_cast<_m128i_t, int16_t>(a);
    #endif
}

int32_t sse::min_epi32(__m128i a) noexcept {
    a = min_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    return simd_cast<_m128i_t, int32_t>(a);
}

uint8_t sse::min_epu8(__m128i a) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    a = min_epu8(a, srli_epi16(a, 8));
    a = _mm_minpos_epu16(a);
    return simd_cast<_m128i_t, uint8_t>(a);
    #else
    a = min_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    auto X = simd_cast<_m128i_t, uint32_t>(a);
    return std::min((uint8_t)X, (uint8_t)(X >> 8));
    #endif
}

uint16_t sse::min_epu16(__m128i a) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return simd_cast<_m128i_t, uint16_t>(_mm_minpos_epu16(a));
    #else
    a = min_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    return simd_cast<_m128i_t, uint16_t>(a);
    #endif
}

uint32_t sse::min_epu32(__m128i a) noexcept {
    a = min_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    return simd_cast<_m128i_t, uint32_t>(a);
}

int8_t sse::min(__m128i a, int8_t) noexcept { return min_epi8(a); }
int16_t sse::min(__m128i a, int16_t) noexcept { return min_epi16(a); }
int32_t sse::min(__m128i a, int32_t) noexcept { return min_epi32(a); }
uint8_t sse::min(__m128i a, uint8_t) noexcept { return min_epu8(a); }
uint16_t sse::min(__m128i a, uint16_t) noexcept { return min_epu16(a); }
uint32_t sse::min(__m128i a, uint32_t) noexcept { return min_epu32(a); }

__m128i sse::move_epi64(__m128i a) noexcept { return _mm_move_epi64(a); }

sse::mask_type sse::movemask_epi8(__m128i a) noexcept {
    return static_cast<mask_type>(_mm_movemask_epi8(a));
}
sse::mask_type sse::movemask_pd(__m128d v) noexcept {
    return static_cast<mask_type>(_mm_movemask_pd(v));
}

sse::mask_type sse::movemask(__m128i v, int8_t) noexcept { return movemask_epi8(v); }
sse::mask_type sse::movemask(__m128i v, int32_t) noexcept {
    return movemask_ps(simd_cast<_m128i_t, _m128_t>(v));
}
sse::mask_type sse::movemask(__m128i v, int64_t) noexcept {
    return movemask_pd(simd_cast<_m128i_t, _m128d_t>(v));
}
sse::mask_type sse::movemask(__m128i v, uint8_t) noexcept { return movemask(v, int8_t()); }
sse::mask_type sse::movemask(__m128i v, uint32_t) noexcept { return movemask(v, int32_t()); }
sse::mask_type sse::movemask(__m128i v, uint64_t) noexcept { return movemask(v, int64_t()); }

__m128i sse::mul_epu32(__m128i a, __m128i b) noexcept { return _mm_mul_epu32(a, b); }

__m128i sse::mulhi_epi16(__m128i a, __m128i b) noexcept { return _mm_mulhi_epi16(a, b); }

__m128i sse::mulhi_epu16(__m128i a, __m128i b) noexcept { return _mm_mulhi_epu16(a, b); }

__m128i sse::mullo_epi16(__m128i a, __m128i b) noexcept { return _mm_mullo_epi16(a, b); }

__m128i sse::negate_epi8(__m128i a) noexcept {
    #if WJR_HAS_SIMD(SSSE3)
    return sign_epi8(a, ones());
    #else
    return sub_epi8(zeros(), a);
    #endif
}

__m128i sse::negate_epi16(__m128i a) noexcept {
    #if WJR_HAS_SIMD(SSSE3)
    return sign_epi16(a, ones());
    #else
    return sub_epi16(zeros(), a);
    #endif
}

__m128i sse::negate_epi32(__m128i a) noexcept {
    #if WJR_HAS_SIMD(SSSE3)
    return sign_epi32(a, ones());
    #else
    return sub_epi32(zeros(), a);
    #endif
}

__m128i sse::negate_epi64(__m128i a) noexcept { return sub_epi64(zeros(), a); }

__m128i sse::negate(__m128i a, int8_t) noexcept { return negate_epi8(a); }
__m128i sse::negate(__m128i a, int16_t) noexcept { return negate_epi16(a); }
__m128i sse::negate(__m128i a, int32_t) noexcept { return negate_epi32(a); }
__m128i sse::negate(__m128i a, int64_t) noexcept { return negate_epi64(a); }
__m128i sse::negate(__m128i a, uint8_t) noexcept { return negate_epi8(a); }
__m128i sse::negate(__m128i a, uint16_t) noexcept { return negate_epi16(a); }
__m128i sse::negate(__m128i a, uint32_t) noexcept { return negate_epi32(a); }
__m128i sse::negate(__m128i a, uint64_t) noexcept { return negate_epi64(a); }

__m128i sse::Not(__m128i v) noexcept { return Xor(v, ones()); }

__m128i sse::Or(__m128i a, __m128i b) noexcept { return _mm_or_si128(a, b); }

__m128i sse::packs_epi16(__m128i a, __m128i b) noexcept { return _mm_packs_epi16(a, b); }
__m128i sse::packs_epi32(__m128i a, __m128i b) noexcept { return _mm_packs_epi32(a, b); }

__m128i sse::packus_epi16(__m128i a, __m128i b) noexcept { return _mm_packus_epi16(a, b); }

__m128i sse::loadu_si48(const void *ptr) noexcept {
    return insert_epi16<2>(loadu_si32(ptr), static_cast<const uint16_t *>(ptr)[2]);
}

__m128i sse::loadu_si80(const void *ptr) noexcept {
    return insert_epi16<4>(loadu_si64(ptr), static_cast<const uint16_t *>(ptr)[4]);
}

__m128i sse::loadu_si96(const void *ptr) noexcept {
    #if WJR_HAS_SIMD(SSE4_1)
    return insert_epi32<2>(loadu_si64(ptr), static_cast<const uint32_t *>(ptr)[2]);
    #else
    return insert_epi16<5>(loadu_si80(ptr), static_cast<const uint16_t *>(ptr)[5]);
    #endif
}

__m128i sse::loadu_si112(const void *ptr) noexcept {
    return insert_epi16<6>(loadu_si96(ptr), static_cast<const uint16_t *>(ptr)[6]);
}

__m128i sse::loadu_si128(const void *ptr) noexcept { return loadu(ptr); }

__m128i sse::loadu_si16x(const void *ptr, int n) noexcept {
    switch (n) {
    case 0:
        return zeros();
    case 1:
        return loadu_si16(ptr);
    case 2:
        return loadu_si32(ptr);
    case 3:
        return loadu_si48(ptr);
    case 4:
        return loadu_si64(ptr);
    case 5:
        return loadu_si80(ptr);
    case 6:
        return loadu_si96(ptr);
    case 7:
        return loadu_si112(ptr);
    default:
        return loadu_si128(ptr);
    }
}

__m128i sse::sad_epu8(__m128i a, __m128i b) noexcept { return _mm_sad_epu8(a, b); }

__m128i sse::zeros() noexcept { return _mm_setzero_si128(); }
__m128i sse::ones() noexcept { return _mm_set1_epi32(-1); }

__m128i sse::set_epi8(char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
                      char e7, char e6, char e5, char e4, char e3, char e2, char e1,
                      char e0) noexcept {
    return _mm_set_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}

__m128i sse::set_epi16(short e7, short e6, short e5, short e4, short e3, short e2, short e1,
                       short e0) noexcept {
    return _mm_set_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::set_epi32(int e3, int e2, int e1, int e0) noexcept {
    return _mm_set_epi32(e3, e2, e1, e0);
}
__m128i sse::set_epi64x(long long e1, long long e0) noexcept { return _mm_set_epi64x(e1, e0); }

__m128i sse::setr_epi8(char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
                       char e7, char e6, char e5, char e4, char e3, char e2, char e1,
                       char e0) noexcept {
    return _mm_setr_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}

__m128i sse::setr_epi16(short e7, short e6, short e5, short e4, short e3, short e2, short e1,
                        short e0) noexcept {
    return _mm_setr_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::setr_epi32(int e3, int e2, int e1, int e0) noexcept {
    return _mm_setr_epi32(e3, e2, e1, e0);
}

__m128i sse::set1_epi8(int8_t val) noexcept { return _mm_set1_epi8(val); }
__m128i sse::set1_epi16(int16_t val) noexcept { return _mm_set1_epi16(val); }
__m128i sse::set1_epi32(int32_t val) noexcept { return _mm_set1_epi32(val); }
__m128i sse::set1_epi64(int64_t val) noexcept { return _mm_set1_epi64x(val); }

__m128i sse::set1(int8_t val, int8_t) noexcept { return set1_epi8(val); }
__m128i sse::set1(int16_t val, int16_t) noexcept { return set1_epi16(val); }
__m128i sse::set1(int32_t val, int32_t) noexcept { return set1_epi32(val); }
__m128i sse::set1(int64_t val, int64_t) noexcept { return set1_epi64(val); }
__m128i sse::set1(uint8_t val, uint8_t) noexcept { return set1_epi8(val); }
__m128i sse::set1(uint16_t val, uint16_t) noexcept { return set1_epi16(val); }
__m128i sse::set1(uint32_t val, uint32_t) noexcept { return set1_epi32(val); }
__m128i sse::set1(uint64_t val, uint64_t) noexcept { return set1_epi64(val); }

__m128i sse::setmin_epi8() noexcept { return set1_epi8(0x80u); }
__m128i sse::setmin_epi16() noexcept { return set1_epi16(0x8000u); }
__m128i sse::setmin_epi32() noexcept { return set1_epi32(0x80000000u); }

__m128i sse::setmin(int8_t) noexcept { return setmin_epi8(); }
__m128i sse::setmin(int16_t) noexcept { return setmin_epi16(); }
__m128i sse::setmin(int32_t) noexcept { return setmin_epi32(); }
__m128i sse::setmin(uint8_t) noexcept { return set1_epi32(0); }
__m128i sse::setmin(uint16_t) noexcept { return set1_epi32(0); }
__m128i sse::setmin(uint32_t) noexcept { return set1_epi32(0); }

__m128i sse::setmax_epi8() noexcept { return set1_epi8(0x7F); }
__m128i sse::setmax_epi16() noexcept { return set1_epi16(0x7FFF); }
__m128i sse::setmax_epi32() noexcept { return set1_epi32(0x7FFFFFFF); }

__m128i sse::setmax(int8_t) noexcept { return setmax_epi8(); }
__m128i sse::setmax(int16_t) noexcept { return setmax_epi16(); }
__m128i sse::setmax(int32_t) noexcept { return setmax_epi32(); }
__m128i sse::setmax(uint8_t) noexcept { return set1_epi32(0xFFFFFFFF); }
__m128i sse::setmax(uint16_t) noexcept { return set1_epi32(0xFFFFFFFF); }
__m128i sse::setmax(uint32_t) noexcept { return set1_epi32(0xFFFFFFFF); }

template <int imm>
__m128i sse::shl(__m128i a) noexcept {
    if constexpr (imm >= 64) {
        a = slli<8>(a);
        a = slli_epi64(a, imm - 64);
        return a;
    } else {
        auto b = slli_epi64(a, imm);
        auto c = slli<8>(a);
        c = srli_epi64(c, 64 - imm);
        return Or(b, c);
    }
}

template <int imm>
__m128i sse::shr(__m128i a) noexcept {
    if constexpr (imm >= 64) {
        a = srli<8>(a);
        a = srli_epi64(a, imm - 64);
        return a;
    } else {
        auto b = srli_epi64(a, imm);
        auto c = srli<8>(a);
        c = slli_epi64(c, 64 - imm);
        return Or(b, c);
    }
}

template <int imm8>
__m128i sse::shuffle_epi32(__m128i v) noexcept {
    static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
    return _mm_shuffle_epi32(v, imm8);
}

template <int imm8>
__m128i sse::shufflehi_epi16(__m128i v) noexcept {
    return _mm_shufflehi_epi16(v, imm8);
}

template <int imm8>
__m128i sse::shufflelo_epi16(__m128i v) noexcept {
    return _mm_shufflelo_epi16(v, imm8);
}

__m128i sse::sll_epi16(__m128i a, __m128i b) noexcept { return _mm_sll_epi16(a, b); }
__m128i sse::sll_epi32(__m128i a, __m128i b) noexcept { return _mm_sll_epi32(a, b); }
__m128i sse::sll_epi64(__m128i a, __m128i b) noexcept { return _mm_sll_epi64(a, b); }

__m128i sse::sll(__m128i a, __m128i b, int16_t) noexcept { return sll_epi16(a, b); }
__m128i sse::sll(__m128i a, __m128i b, int32_t) noexcept { return sll_epi32(a, b); }
__m128i sse::sll(__m128i a, __m128i b, int64_t) noexcept { return sll_epi64(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint16_t) noexcept { return sll_epi16(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint32_t) noexcept { return sll_epi32(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint64_t) noexcept { return sll_epi64(a, b); }

template <int imm8>
__m128i sse::slli(__m128i v) noexcept {
    return _mm_slli_si128(v, imm8);
}
__m128i sse::slli_epi16(__m128i a, int imm8) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(imm8 == 1)) {
        return sse::add_epi16(a, a);
    }

    return _mm_slli_epi16(a, imm8);
}
__m128i sse::slli_epi32(__m128i a, int imm8) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(imm8 == 1)) {
        return sse::add_epi32(a, a);
    }

    return _mm_slli_epi32(a, imm8);
}
__m128i sse::slli_epi64(__m128i a, int imm8) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(imm8 == 1)) {
        return sse::add_epi64(a, a);
    }

    return _mm_slli_epi64(a, imm8);
}

__m128i sse::slli(__m128i a, int imm8, int16_t) noexcept { return slli_epi16(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, int32_t) noexcept { return slli_epi32(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, int64_t) noexcept { return slli_epi64(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint16_t) noexcept { return slli_epi16(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint32_t) noexcept { return slli_epi32(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint64_t) noexcept { return slli_epi64(a, imm8); }

__m128i sse::sra_epi16(__m128i a, __m128i b) noexcept { return _mm_sra_epi16(a, b); }
__m128i sse::sra_epi32(__m128i a, __m128i b) noexcept { return _mm_sra_epi32(a, b); }

__m128i sse::sra(__m128i a, __m128i b, int16_t) noexcept { return sra_epi16(a, b); }
__m128i sse::sra(__m128i a, __m128i b, int32_t) noexcept { return sra_epi32(a, b); }

__m128i sse::srai_epi16(__m128i a, int imm8) noexcept { return _mm_srai_epi16(a, imm8); }
__m128i sse::srai_epi32(__m128i a, int imm8) noexcept { return _mm_srai_epi32(a, imm8); }

__m128i sse::srai(__m128i a, int imm8, int16_t) noexcept { return srai_epi16(a, imm8); }
__m128i sse::srai(__m128i a, int imm8, int32_t) noexcept { return srai_epi32(a, imm8); }

__m128i sse::srl_epi16(__m128i a, __m128i b) noexcept { return _mm_srl_epi16(a, b); }
__m128i sse::srl_epi32(__m128i a, __m128i b) noexcept { return _mm_srl_epi32(a, b); }
__m128i sse::srl_epi64(__m128i a, __m128i b) noexcept { return _mm_srl_epi64(a, b); }

__m128i sse::srl(__m128i a, __m128i b, int16_t) noexcept { return srl_epi16(a, b); }
__m128i sse::srl(__m128i a, __m128i b, int32_t) noexcept { return srl_epi32(a, b); }
__m128i sse::srl(__m128i a, __m128i b, int64_t) noexcept { return srl_epi64(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint16_t) noexcept { return srl_epi16(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint32_t) noexcept { return srl_epi32(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint64_t) noexcept { return srl_epi64(a, b); }

template <int imm8>
__m128i sse::srli(__m128i v) noexcept {
    return _mm_srli_si128(v, imm8);
}
__m128i sse::srli_epi8(__m128i a, int imm8) noexcept {
    return And(srli_epi16(a, imm8), sse_detail::srli_epi8_mask[imm8]);
}
__m128i sse::srli_epi16(__m128i a, int imm8) noexcept { return _mm_srli_epi16(a, imm8); }
__m128i sse::srli_epi32(__m128i a, int imm8) noexcept { return _mm_srli_epi32(a, imm8); }
__m128i sse::srli_epi64(__m128i a, int imm8) noexcept { return _mm_srli_epi64(a, imm8); }

__m128i sse::srli(__m128i a, int imm8, int8_t) noexcept { return srli_epi8(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, int16_t) noexcept { return srli_epi16(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, int32_t) noexcept { return srli_epi32(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, int64_t) noexcept { return srli_epi64(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint8_t) noexcept { return srli_epi8(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint16_t) noexcept { return srli_epi16(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint32_t) noexcept { return srli_epi32(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint64_t) noexcept { return srli_epi64(a, imm8); }

void sse::stream(void *p, __m128i a) noexcept { _mm_stream_si128(static_cast<__m128i *>(p), a); }
void sse::store(void *p, __m128i a) noexcept { _mm_store_si128(static_cast<__m128i *>(p), a); }
void sse::storeu(void *p, __m128i a) noexcept { _mm_storeu_si128(static_cast<__m128i *>(p), a); }

__m128i sse::sub_epi8(__m128i a, __m128i b) noexcept { return _mm_sub_epi8(a, b); }
__m128i sse::sub_epi16(__m128i a, __m128i b) noexcept { return _mm_sub_epi16(a, b); }
__m128i sse::sub_epi32(__m128i a, __m128i b) noexcept { return _mm_sub_epi32(a, b); }
__m128i sse::sub_epi64(__m128i a, __m128i b) noexcept { return _mm_sub_epi64(a, b); }

__m128i sse::sub(__m128i a, __m128i b, int8_t) noexcept { return sub_epi8(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int16_t) noexcept { return sub_epi16(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int32_t) noexcept { return sub_epi32(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int64_t) noexcept { return sub_epi64(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint8_t) noexcept { return sub_epi8(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint16_t) noexcept { return sub_epi16(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint32_t) noexcept { return sub_epi32(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint64_t) noexcept { return sub_epi64(a, b); }

__m128i sse::subs_epi8(__m128i a, __m128i b) noexcept { return _mm_subs_epi8(a, b); }
__m128i sse::subs_epi16(__m128i a, __m128i b) noexcept { return _mm_subs_epi16(a, b); }

__m128i sse::subs_epu8(__m128i a, __m128i b) noexcept { return _mm_subs_epu8(a, b); }
__m128i sse::subs_epu16(__m128i a, __m128i b) noexcept { return _mm_subs_epu16(a, b); }

__m128i sse::subs(__m128i a, __m128i b, int8_t) noexcept { return subs_epi8(a, b); }
__m128i sse::subs(__m128i a, __m128i b, int16_t) noexcept { return subs_epi16(a, b); }
__m128i sse::subs(__m128i a, __m128i b, uint8_t) noexcept { return subs_epu8(a, b); }
__m128i sse::subs(__m128i a, __m128i b, uint16_t) noexcept { return subs_epu16(a, b); }

__m128i sse::unpackhi_epi8(__m128i a, __m128i b) noexcept { return _mm_unpackhi_epi8(a, b); }
__m128i sse::unpackhi_epi16(__m128i a, __m128i b) noexcept { return _mm_unpackhi_epi16(a, b); }
__m128i sse::unpackhi_epi32(__m128i a, __m128i b) noexcept { return _mm_unpackhi_epi32(a, b); }
__m128i sse::unpackhi_epi64(__m128i a, __m128i b) noexcept { return _mm_unpackhi_epi64(a, b); }

__m128i sse::unpackhi(__m128i a, __m128i b, int8_t) noexcept { return unpackhi_epi8(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int16_t) noexcept { return unpackhi_epi16(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int32_t) noexcept { return unpackhi_epi32(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int64_t) noexcept { return unpackhi_epi64(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint8_t) noexcept { return unpackhi_epi8(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint16_t) noexcept { return unpackhi_epi16(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint32_t) noexcept { return unpackhi_epi32(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint64_t) noexcept { return unpackhi_epi64(a, b); }

__m128i sse::unpacklo_epi8(__m128i a, __m128i b) noexcept { return _mm_unpacklo_epi8(a, b); }
__m128i sse::unpacklo_epi16(__m128i a, __m128i b) noexcept { return _mm_unpacklo_epi16(a, b); }
__m128i sse::unpacklo_epi32(__m128i a, __m128i b) noexcept { return _mm_unpacklo_epi32(a, b); }
__m128i sse::unpacklo_epi64(__m128i a, __m128i b) noexcept { return _mm_unpacklo_epi64(a, b); }

__m128i sse::unpacklo(__m128i a, __m128i b, int8_t) noexcept { return unpacklo_epi8(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int16_t) noexcept { return unpacklo_epi16(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int32_t) noexcept { return unpacklo_epi32(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int64_t) noexcept { return unpacklo_epi64(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint8_t) noexcept { return unpacklo_epi8(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint16_t) noexcept { return unpacklo_epi16(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint32_t) noexcept { return unpacklo_epi32(a, b); }

__m128i sse::Xor(__m128i a, __m128i b) noexcept { return _mm_xor_si128(a, b); }

#endif

#if WJR_HAS_SIMD(SSE3)

__m128i sse::lddqu(const __m128i *ptr) noexcept { return _mm_lddqu_si128(ptr); }

#endif

#if WJR_HAS_SIMD(SSSE3)

__m128i sse::abs_epi8(__m128i val) noexcept { return _mm_abs_epi8(val); }
__m128i sse::abs_epi16(__m128i val) noexcept { return _mm_abs_epi16(val); }
__m128i sse::abs_epi32(__m128i val) noexcept { return _mm_abs_epi32(val); }

__m128i sse::abs(__m128i val, int8_t) noexcept { return abs_epi8(val); }
__m128i sse::abs(__m128i val, int16_t) noexcept { return abs_epi16(val); }
__m128i sse::abs(__m128i val, int32_t) noexcept { return abs_epi32(val); }
__m128i sse::abs(__m128i val, uint8_t) noexcept { return val; }
__m128i sse::abs(__m128i val, uint16_t) noexcept { return val; }
__m128i sse::abs(__m128i val, uint32_t) noexcept { return val; }

__m128i sse::shuffle_epi8(__m128i v, __m128i imm8) noexcept { return _mm_shuffle_epi8(v, imm8); }

__m128i sse::sign_epi8(__m128i a, __m128i b) noexcept { return _mm_sign_epi8(a, b); }
__m128i sse::sign_epi16(__m128i a, __m128i b) noexcept { return _mm_sign_epi16(a, b); }
__m128i sse::sign_epi32(__m128i a, __m128i b) noexcept { return _mm_sign_epi32(a, b); }

__m128i sse::sign(__m128i a, __m128i b, int8_t) noexcept { return sign_epi8(a, b); }
__m128i sse::sign(__m128i a, __m128i b, int16_t) noexcept { return sign_epi16(a, b); }
__m128i sse::sign(__m128i a, __m128i b, int32_t) noexcept { return sign_epi32(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint8_t) noexcept { return sign_epi8(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint16_t) noexcept { return sign_epi16(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint32_t) noexcept { return sign_epi32(a, b); }

#endif

#if WJR_HAS_SIMD(SSE4_1)

template <int imm8>
__m128i sse::blend_epi16(__m128i a, __m128i b) noexcept {
    return _mm_blend_epi16(a, b, imm8);
}

__m128i sse::cmpeq_epi64(__m128i a, __m128i b) noexcept { return _mm_cmpeq_epi64(a, b); }

__m128i sse::cmpeq(__m128i a, __m128i b, int64_t) noexcept { return cmpeq_epi64(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint64_t) noexcept { return cmpeq_epi64(a, b); }

__m128i sse::cmpgt_epi64(__m128i a, __m128i b) noexcept { return _mm_cmpgt_epi64(a, b); }

__m128i sse::cmpgt(__m128i a, __m128i b, int64_t) noexcept { return cmpgt_epi64(a, b); }

template <int imm8>
__m128i sse::insert_epi8(__m128i a, int i) noexcept {
    return _mm_insert_epi8(a, i, imm8);
}

template <int imm8>
__m128i sse::insert_epi32(__m128i a, int i) noexcept {
    return _mm_insert_epi32(a, i, imm8);
}

template <int imm8>
__m128i sse::insert_epi64(__m128i a, int64_t i) noexcept {
    return _mm_insert_epi64(a, i, imm8);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, int8_t) noexcept {
    return insert_epi8<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, int32_t) noexcept {
    return insert_epi32<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int64_t i, int64_t) noexcept {
    return insert_epi64<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, uint8_t) noexcept {
    return insert_epi8<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, uint32_t) noexcept {
    return insert_epi32<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int64_t i, uint64_t) noexcept {
    return insert_epi64<imm8>(a, i);
}

__m128i sse::minpos_epu16(__m128i a) noexcept { return _mm_minpos_epu16(a); }

__m128i sse::mul_epi32(__m128i a, __m128i b) noexcept { return _mm_mul_epi32(a, b); }

__m128i sse::mullo_epi32(__m128i a, __m128i b) noexcept { return _mm_mullo_epi32(a, b); }

__m128i sse::packus_epi32(__m128i a, __m128i b) noexcept { return _mm_packus_epi32(a, b); }

__m128i sse::stream_load(void *p) noexcept {
    return _mm_stream_load_si128(static_cast<__m128i *>(p));
}

int sse::test_all_ones(__m128i a) noexcept { return _mm_test_all_ones(a); }

int sse::test_all_zeros(__m128i a, __m128i b) noexcept { return _mm_test_all_zeros(a, b); }

int sse::test_all_zeros(__m128i a) noexcept { return _mm_test_all_zeros(a, a); }

int sse::test_mix_ones_zeros(__m128i a, __m128i b) noexcept {
    return _mm_test_mix_ones_zeros(a, b);
}

int sse::testc(__m128i a, __m128i b) noexcept { return _mm_testc_si128(a, b); }

int sse::testnzc(__m128i a, __m128i b) noexcept { return _mm_testnzc_si128(a, b); }

int sse::testz(__m128i a, __m128i b) noexcept { return _mm_testz_si128(a, b); }

#endif

// LCOV_EXCL_STOP

} // namespace wjr

#endif // WJR_ARCH_X86_SIMD_SSE_HPP__
