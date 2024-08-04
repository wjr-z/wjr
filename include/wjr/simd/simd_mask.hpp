#ifndef WJR_SIMD_SIMD_MASK_HPP__
#define WJR_SIMD_SIMD_MASK_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::simd_detail {

template <typename T, size_t Size, size_t BitWidth>
class basic_simd_mask {
    using mask_type = uint_t<BitWidth>;
    constexpr static size_t __mask_bits = BitWidth / Size;

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(basic_simd_mask);

    constexpr basic_simd_mask(mask_type mask) noexcept : m_mask(mask) {}

    constexpr int clz() const noexcept {
        WJR_ASSERT_ASSUME(m_mask != 0);

        if constexpr (Size == 2) {
            constexpr auto high_mask =
                static_cast<mask_type>(static_cast<uint_t<BitWidth / 2>>(in_place_max))
                << (BitWidth / 2);

            return (m_mask & high_mask) ? 0 : 1;
        } else {
            return clz(m_mask) / __mask_bits;
        }
    }

    template <typename U, WJR_REQUIRES(is_nonbool_integral_v<U> && sizeof(U) < sizeof(T))>
    constexpr int ctz(U) const noexcept {
        WJR_ASSERT_ASSUME(m_mask != 0);

        constexpr size_t elements = sizeof(T) / sizeof(U);
        if constexpr (elements == 2) {
            constexpr auto low_mask =
                static_cast<mask_type>(static_cast<uint_t<BitWidth / 2>>(in_place_max));

            return (m_mask & low_mask) ? 0 : 1;
        } else {
            return ctz(m_mask) / __mask_bits;
        }
    }

    constexpr bool any() const noexcept { return m_mask != 0; }

private:
    mask_type m_mask;
};

} // namespace wjr::simd_detail

#endif // WJR_SIMD_SIMD_MASK_HPP__