#ifndef WJR_SIMD_SIMD_MASK_HPP__
#define WJR_SIMD_SIMD_MASK_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/clz.hpp>
#include <wjr/math/ctz.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::simd_detail {

template <typename T, size_t Size, size_t BitWidth>
class basic_simd_mask {
    using mask_type = uint_t<BitWidth>;
    constexpr static size_t __mask_bits = BitWidth / Size;
    constexpr static mask_type __half_mask =
        static_cast<uint_t<BitWidth / 2>>(in_place_max);
    constexpr static mask_type __full_mask = in_place_max;

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(basic_simd_mask);

    constexpr basic_simd_mask(mask_type mask) noexcept : m_mask(mask) {}

    WJR_PURE WJR_CONSTEXPR20 int clz() const noexcept {
        WJR_ASSERT_ASSUME(m_mask != 0);

        if constexpr (Size == 2) {
            constexpr auto high_mask = __half_mask << (BitWidth / 2);

            return (m_mask & high_mask) ? 0 : 1;
        } else {
            return ::wjr::clz(m_mask) / __mask_bits;
        }
    }

    WJR_PURE WJR_CONSTEXPR20 int ctz() const noexcept {
        WJR_ASSERT_ASSUME(m_mask != 0);

        if constexpr (Size == 2) {
            constexpr auto low_mask = __half_mask;

            return (m_mask & low_mask) ? 0 : 1;
        } else {
            return ::wjr::ctz(m_mask) / __mask_bits;
        }
    }

    WJR_PURE constexpr bool all() const noexcept { return m_mask == __full_mask; }

private:
    mask_type m_mask;
};

} // namespace wjr::simd_detail

#endif // WJR_SIMD_SIMD_MASK_HPP__