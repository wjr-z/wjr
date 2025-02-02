#ifndef WJR_SIMD_SIMD_HPP__
#define WJR_SIMD_SIMD_HPP__

#include <wjr/math/broadcast.hpp>
#include <wjr/memory/detail.hpp>
#include <wjr/simd/detail.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/simd/simd.hpp>
#endif

namespace wjr {

template <typename T, size_t N>
class simd<T, simd_abi::fixed_size<N>> {
    static constexpr size_t BitWidth = sizeof(T) * 8 * N;
    using int_type = uint_t<BitWidth>;

    static_assert(std::is_unsigned_v<T>);
    static_assert(N >= 2);

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(simd);

    template <typename U, WJR_REQUIRES(is_value_preserving_or_int_v<U, T>)>
    constexpr simd(U value) noexcept : m_data(broadcast<U, int_type>(value)) {}

    template <typename Flags = element_aligned_t>
    simd(const T *mem, Flags flags = {}) noexcept {
        copy_from(mem, flags);
    }

    template <typename Flags = element_aligned_t>
    void copy_from(const T *mem, Flags = {}) noexcept {
        m_data = read_memory<int_type>(mem);
    }

    template <typename Flags = element_aligned_t>
    void copy_to(T *mem, Flags = {}) noexcept {
        write_memory<int_type>(mem, m_data);
    }

    constexpr simd &operator&=(const simd &other) noexcept {
        m_data &= other.m_data;
        return *this;
    }

    friend constexpr simd operator&(const simd &lhs, const simd &rhs) noexcept {
        simd ret(lhs);
        ret &= rhs;
        return ret;
    }

    constexpr simd &operator|=(const simd &other) noexcept {
        m_data |= other.m_data;
        return *this;
    }

    friend constexpr simd operator|(const simd &lhs, const simd &rhs) noexcept {
        simd ret(lhs);
        ret |= rhs;
        return ret;
    }

    constexpr simd &operator^=(const simd &other) noexcept {
        m_data ^= other.m_data;
        return *this;
    }

    friend constexpr simd operator^(const simd &lhs, const simd &rhs) noexcept {
        simd ret(lhs);
        ret ^= rhs;
        return ret;
    }

private:
    int_type m_data;
};

template <size_t N>
struct is_native_simd_bit : std::false_type {};

template <>
struct is_native_simd_bit<8> : std::true_type {};
template <>
struct is_native_simd_bit<16> : std::true_type {};
template <>
struct is_native_simd_bit<32> : std::true_type {};
template <>
struct is_native_simd_bit<64> : std::true_type {};

#if WJR_HAS_SIMD(NATIVE_128BIT)
template <>
struct is_native_simd_bit<128> : std::true_type {};
#endif

#if WJR_HAS_SIMD(NATIVE_256BIT)
template <>
struct is_native_simd_bit<256> : std::true_type {};
#endif

} // namespace wjr

#endif // WJR_SIMD_SIMD_HPP__