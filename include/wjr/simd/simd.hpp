#ifndef WJR_SIMD_SIMD_HPP__
#define WJR_SIMD_SIMD_HPP__

#include <wjr/simd/simd_cast.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

#define WJR_REGISTER_SIMD_CAST(From, To)                                                 \
    template <>                                                                          \
    struct simd_cast_fn<From, To> {                                                      \
        WJR_CONST constexpr To operator()(From v) const noexcept {                       \
            return static_cast<To>(v);                                                   \
        }                                                                                \
    }

WJR_REGISTER_SIMD_CAST(uint8_t, uint8_t);

WJR_REGISTER_SIMD_CAST(uint8_t, uint16_t);

WJR_REGISTER_SIMD_CAST(uint8_t, uint32_t);

WJR_REGISTER_SIMD_CAST(uint8_t, uint64_t);

WJR_REGISTER_SIMD_CAST(uint16_t, uint16_t);

WJR_REGISTER_SIMD_CAST(uint16_t, uint32_t);

WJR_REGISTER_SIMD_CAST(uint16_t, uint64_t);

WJR_REGISTER_SIMD_CAST(uint32_t, uint32_t);

WJR_REGISTER_SIMD_CAST(uint32_t, uint64_t);

WJR_REGISTER_SIMD_CAST(uint64_t, uint64_t);

#undef WJR_REGISTER_SIMD_CAST

namespace simd_abi {

template <size_t N>
struct fixed_size {};

} // namespace simd_abi

struct element_aligned_t {};
inline constexpr element_aligned_t element_aligned{};

struct vector_aligned_t {};
inline constexpr vector_aligned_t vector_aligned{};

} // namespace wjr

#endif // WJR_SIMD_SIMD_HPP__