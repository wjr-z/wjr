#include <wjr/arch/x86/simd/avx.hpp>

namespace {

template <typename Simd, typename Vector, typename T>
concept logical_lane = requires(Vector a, Vector b, T tag) {
    Simd::logical_and(a, b, tag);
    Simd::logical_not(a, tag);
    Simd::logical_or(a, b, tag);
};

template <typename Simd, typename Vector>
constexpr bool logical_domain() {
    return logical_lane<Simd, Vector, int8_t> && logical_lane<Simd, Vector, uint8_t> &&
           logical_lane<Simd, Vector, int16_t> && logical_lane<Simd, Vector, uint16_t> &&
           logical_lane<Simd, Vector, int32_t> && logical_lane<Simd, Vector, uint32_t> &&
           logical_lane<Simd, Vector, int64_t> && logical_lane<Simd, Vector, uint64_t> &&
           !logical_lane<Simd, Vector, bool> && !logical_lane<Simd, Vector, char> &&
           !logical_lane<Simd, Vector, float> && !logical_lane<Simd, Vector, double> &&
           !logical_lane<Simd, Vector, void *>;
}

static_assert(logical_domain<wjr::sse, __m128i>());

#if WJR_HAS_SIMD(AVX2)
static_assert(logical_domain<wjr::avx, __m256i>());
#endif

} // namespace
