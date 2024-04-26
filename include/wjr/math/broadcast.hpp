#ifndef WJR_MATH_BROADCAST_HPP
#define WJR_MATH_BROADCAST_HPP

#include <cstdint>

#include <wjr/preprocessor.hpp>

namespace wjr {

template <typename From, typename To>
struct broadcast_fn {};

/**
 * @brief Broadcast a value to a wider type.
 * 
 * @note From must be a smaller type than To.
 */
template <typename From, typename To>
inline constexpr broadcast_fn<From, To> broadcast{};

template <>
struct broadcast_fn<uint8_t, uint8_t> {
    WJR_INTRINSIC_CONSTEXPR uint8_t operator()(uint8_t x) const { return x; }
};
template <>
struct broadcast_fn<uint16_t, uint16_t> {
    WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint16_t x) const { return x; }
};
template <>
struct broadcast_fn<uint32_t, uint32_t> {
    WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint32_t x) const { return x; }
};
template <>
struct broadcast_fn<uint64_t, uint64_t> {
    WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint64_t x) const { return x; }
};

template <>
struct broadcast_fn<uint8_t, uint16_t> {
    WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint8_t x) const {
        return static_cast<uint16_t>(static_cast<uint32_t>(x) |
                                     (static_cast<uint16_t>(x) << 8));
    }
};

template <>
struct broadcast_fn<uint16_t, uint32_t> {
    WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint16_t x) const {
        return x | (static_cast<uint32_t>(x) << 16);
    }
};
template <>
struct broadcast_fn<uint32_t, uint64_t> {
    WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint32_t x) const {
        return static_cast<uint64_t>(x) | (static_cast<uint64_t>(x) << 32);
    }
};

template <>
struct broadcast_fn<uint8_t, uint32_t> {
    WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint8_t x) const {
        return x * static_cast<uint32_t>(0x01010101u);
    }
};
template <>
struct broadcast_fn<uint16_t, uint64_t> {
    WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint16_t x) const {
        return x * static_cast<uint64_t>(0x0001000100010001ull);
    }
};

template <>
struct broadcast_fn<uint8_t, uint64_t> {
    WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint8_t x) const {
        return x * static_cast<uint64_t>(0x0101010101010101ull);
    }
};

} // namespace wjr

#endif // WJR_MATH_BROADCAST_HPP