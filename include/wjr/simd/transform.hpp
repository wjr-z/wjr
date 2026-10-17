#ifndef WJR_SIMD_TRANSFORM_HPP__
#define WJR_SIMD_TRANSFORM_HPP__

#include <concepts>

#include <wjr/config/attribute.hpp>
#include <wjr/simd/xsimd.hpp>

namespace wjr::simd::xsimd_backend {

// Nonempty buffers contain count valid T objects. Each input is either disjoint
// from dst or exactly equal to it; other overlap is a caller precondition.
// The operation is used as an lvalue for both complete batches and scalar tails.
template <typename T, typename Arch = xsimd::default_arch, typename Operation>
requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool> &&
         xsimd::has_simd_register<T, Arch>::value &&
         requires(Operation &operation, const T *src) {
             { operation(batch<T, Arch>::load_unaligned(src)) } -> std::same_as<batch<T, Arch>>;
             { operation(*src) } -> std::convertible_to<T>;
         })
WJR_INTRINSIC_INLINE void
transform_to(T *dst, const T *src, std::size_t count, Operation &&operation) noexcept(
    noexcept(operation(batch<T, Arch>::load_unaligned(src)).store_unaligned(dst)) &&
    noexcept(*dst = operation(*src))) {
    using vector = batch<T, Arch>;
    std::size_t offset = 0;
    for (; count - offset >= vector::size; offset += vector::size) {
        const auto value = operation(vector::load_unaligned(src + offset));
        value.store_unaligned(dst + offset);
    }
    for (; offset < count; ++offset) {
        dst[offset] = operation(src[offset]);
    }
}

template <typename T, typename Arch = xsimd::default_arch, typename Operation>
requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool> &&
         xsimd::has_simd_register<T, Arch>::value &&
         requires(Operation &operation, const T *lhs, const T *rhs) {
             {
                 operation(batch<T, Arch>::load_unaligned(lhs), batch<T, Arch>::load_unaligned(rhs))
             } -> std::same_as<batch<T, Arch>>;
             { operation(*lhs, *rhs) } -> std::convertible_to<T>;
         })
WJR_INTRINSIC_INLINE void
transform_to(T *dst, const T *lhs, const T *rhs, std::size_t count,
             Operation &&operation) noexcept(noexcept(operation(batch<T, Arch>::load_unaligned(lhs),
                                                                batch<T, Arch>::load_unaligned(rhs))
                                                          .store_unaligned(dst)) &&
                                             noexcept(*dst = operation(*lhs, *rhs))) {
    using vector = batch<T, Arch>;
    std::size_t offset = 0;
    for (; count - offset >= vector::size; offset += vector::size) {
        const auto value =
            operation(vector::load_unaligned(lhs + offset), vector::load_unaligned(rhs + offset));
        value.store_unaligned(dst + offset);
    }
    for (; offset < count; ++offset) {
        dst[offset] = operation(lhs[offset], rhs[offset]);
    }
}

} // namespace wjr::simd::xsimd_backend

#endif // WJR_SIMD_TRANSFORM_HPP__
