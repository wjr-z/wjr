#ifndef WJR_SIMD_DETAIL_HPP__
#define WJR_SIMD_DETAIL_HPP__

#include <wjr/simd/simd_cast.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {
template <typename T>
concept simd_integral =
    is_any_of_v<T, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t>;
}

#endif // WJR_SIMD_DETAIL_HPP__