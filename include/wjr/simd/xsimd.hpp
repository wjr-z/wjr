#ifndef WJR_SIMD_XSIMD_HPP__
#define WJR_SIMD_XSIMD_HPP__

#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <xsimd/xsimd.hpp>

#if XSIMD_VERSION_MAJOR != 14 || XSIMD_VERSION_MINOR != 3 || XSIMD_VERSION_PATCH != 0
    #error "The experimental WJR backend requires xsimd 14.3.0"
#endif

// Experimental backend-local types. Keep them out of compiled kernel ABIs.
namespace wjr::simd::xsimd_backend {

template <typename T, typename Arch = xsimd::default_arch>
using batch = xsimd::batch<T, Arch>;

template <typename T, typename Arch = xsimd::default_arch>
using mask = xsimd::batch_bool<T, Arch>;

template <typename T, typename Arch>
inline bool any(const mask<T, Arch> &value) noexcept {
    return xsimd::any(value);
}

template <typename T, typename Arch>
inline bool all(const mask<T, Arch> &value) noexcept {
    return xsimd::all(value);
}

// Lane order, not byte-sign movemask order. No match returns the lane count.
template <typename T, typename Arch>
inline std::size_t first_true(const mask<T, Arch> &value) noexcept {
    const std::uint64_t bits = value.mask();
    return bits == 0 ? batch<T, Arch>::size : static_cast<std::size_t>(std::countr_zero(bits));
}

// The scalar tail never loads outside [src, src + count). Empty input may be null.
// This reference kernel is opt-in; existing WJR scans retain their own contracts.
template <typename T, typename Arch = xsimd::default_arch>
requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool> &&
         xsimd::has_simd_register<T, Arch>::value)
inline std::size_t find_equal(const T *src, T value, std::size_t count) noexcept {
    using vector = batch<T, Arch>;
    const vector needle(value);
    std::size_t offset = 0;
    for (; count - offset >= vector::size; offset += vector::size) {
        const auto matches = vector::load_unaligned(src + offset) == needle;
        if (xsimd::any(matches)) {
            return offset + first_true(matches);
        }
    }
    for (; offset < count; ++offset) {
        if (src[offset] == value) {
            return offset;
        }
    }
    return count;
}

} // namespace wjr::simd::xsimd_backend

#endif // WJR_SIMD_XSIMD_HPP__
