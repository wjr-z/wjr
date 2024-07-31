#ifndef WJR_SIMD_SIMD_CAST_HPP__
#define WJR_SIMD_SIMD_CAST_HPP__

namespace wjr {

template <typename From, typename To>
struct simd_cast_fn;

template <typename From, typename To>
inline constexpr simd_cast_fn<From, To> simd_cast;

} // namespace wjr

#endif // WJR_SIMD_SIMD_CAST_HPP__