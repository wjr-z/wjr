#ifndef WJR_SIMD_DETAIL_HPP__
#define WJR_SIMD_DETAIL_HPP__

#include <wjr/simd/simd_cast.hpp>
#include <wjr/simd/simd_mask.hpp>

namespace wjr {

namespace simd_abi {

template <size_t N>
struct fixed_size {};

} // namespace simd_abi

struct element_aligned_t {};
inline constexpr element_aligned_t element_aligned{};

struct vector_aligned_t {};
inline constexpr vector_aligned_t vector_aligned{};

template <typename T, typename Abi>
class simd;

template <typename T, size_t N>
using fixed_size_simd = simd<T, simd_abi::fixed_size<N>>;

} // namespace wjr

#endif // WJR_SIMD_DETAIL_HPP__