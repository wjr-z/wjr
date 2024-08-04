#ifndef WJR_SIMD_SIMD_HPP__
#define WJR_SIMD_SIMD_HPP__

#include <wjr/math/broadcast.hpp>
#include <wjr/memory/detail.hpp>
#include <wjr/simd/detail.hpp>
#include <wjr/simd/simd_mask.hpp>

#if defined(WJR_X86)
#include <wjr/x86/simd/simd.hpp>
#endif

namespace wjr {

template <typename T, size_t N>
class simd<T, simd_abi::fixed_size<N>> {
    static constexpr size_t BitWidth = sizeof(T) * 8 * N;
    using int_type = uint_t<BitWidth>;

public:
    using mask_type = simd_detail::basic_simd_mask<T, N, BitWidth>;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(simd);

    template <typename U, WJR_REQUIRES(is_value_preserving_or_int_v<U, T>)>
    constexpr simd(U value) noexcept : m_data(broadcast<U, int_type>(value)) {}

    template <typename Flags = element_aligned_t>
    simd(const T *mem, Flags flags = {}) noexcept {
        copy_from(mem, flags);
    }

    template <typename Flags = element_aligned_t>
    void copy_from(const T *mem, Flags flags = {}) noexcept {
        m_data = read_memory<int_type>(mem);
    }

    template <typename Flags = element_aligned_t>
    void copy_to(T *mem, Flags flags = {}) noexcept {
        write_memory<int_type>(mem, m_data);
    }

    friend constexpr mask_type operator==(const simd &lhs, const simd &rhs) noexcept {
        return lhs.m_data ^ rhs.m_dat;
    }

private:
    int_type m_data;
};

} // namespace wjr

#endif // WJR_SIMD_SIMD_HPP__