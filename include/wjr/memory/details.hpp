#ifndef WJR_MEMORY_TO_ADDRESS_HPP__
#define WJR_MEMORY_TO_ADDRESS_HPP__

#include <cstring>

#include <wjr/type_traits.hpp>

namespace wjr {

namespace to_address_details {

template <typename Enable, typename Ptr, typename... Args>
struct __has_to_address : std::false_type {};
template <typename Ptr, typename... Args>
struct __has_to_address<
    std::void_t<decltype(typename std::pointer_traits<Ptr>::to_address(
        std::declval<const Ptr &>()))>,
    Ptr, Args...> : std::true_type {};
template <typename Ptr, typename... Args>
struct has_to_address : __has_to_address<void, Ptr, Args...> {};
template <typename Ptr, typename... Args>
constexpr bool has_to_address_v = has_to_address<Ptr, Args...>::value;

} // namespace to_address_details

template <typename T>
constexpr T *to_address(T *p) noexcept {
    static_assert(!std::is_function_v<T>, "T cannot be a function.");
    return p;
}

template <typename Ptr>
constexpr auto to_address(const Ptr &p) noexcept {
    if constexpr (to_address_details::has_to_address_v<Ptr>) {
        return std::pointer_traits<Ptr>::to_address(p);
    } else {
        return to_address(p.operator->());
    }
}

class __is_little_endian_helper {
    constexpr static std::uint32_t u4 = 1;
    constexpr static std::uint8_t u1 = static_cast<const std::uint8_t &>(u4);

public:
    constexpr static bool value = u1 != 0;
};

// constexpr endian
enum class endian {
    little = 0,
    big = 1,
    native = __is_little_endian_helper::value ? little : big
};

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T fallback_byteswap(T x) noexcept {
    constexpr auto digits = std::numeric_limits<T>::digits;
    auto val = static_cast<uint_t<digits>>(x);
    if constexpr (digits == 8) {
        return val;
    } else if constexpr (digits == 16) {
        return (val >> 8) | (val << 8);
    } else if constexpr (digits == 32) {
        return ((val >> 24) & 0xff) | ((val >> 8) & 0xff00) | ((val << 8) & 0xff0000) |
               ((val << 24));
    } else if constexpr (digits == 64) {
        return ((val >> 56) & 0xff) | ((val >> 40) & 0xff00) | ((val >> 24) & 0xff0000) |
               ((val >> 8) & 0xff000000) | ((val << 8) & 0xff00000000) |
               ((val << 24) & 0xff0000000000) | ((val << 40) & 0xff000000000000) |
               ((val << 56));
    } else {
        static_assert(digits <= 64, "Unsupported bit width");
    }
}

#if WJR_HAS_BUILTIN(__builtin_bswap16)
#define WJR_HAS_BUILTIN_BYTESWAP WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(BYTESWAP)

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T builtin_byteswap(T x) noexcept {
    constexpr auto digits = std::numeric_limits<T>::digits;
    auto val = static_cast<uint_t<digits>>(x);
    if constexpr (digits == 8) {
        return val;
    } else if constexpr (digits == 16) {
        return __builtin_bswap16(val);
    } else if constexpr (digits == 32) {
        return __builtin_bswap32(val);
    } else if constexpr (digits == 64) {
        return __builtin_bswap64(val);
    } else {
        static_assert(digits <= 64, "Unsupported bit width");
    }
}

#endif

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T byteswap(T x, endian to = endian::little) noexcept {
    if (to == endian::native) {
        return x;
    }

#if WJR_HAS_BUILTIN(BYTESWAP)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_byteswap(x);
    }

    return builtin_byteswap(x);
#else
    return fallback_byteswap(x);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_INLINE T read_memory(const void *ptr,
                                            endian to = endian::little) noexcept {
    T x;
    std::memcpy(&x, ptr, sizeof(T));

    if (to != endian::native) {
        x = byteswap(x);
    }

    return x;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_INLINE void write_memory(void *ptr, T x,
                                       endian to = endian::little) noexcept {
    if (to != endian::native) {
        x = byteswap(x);
    }

    std::memcpy(ptr, &x, sizeof(T));
}

template <class Pointer, class SizeType = std::size_t>
struct allocation_result {
    Pointer ptr;
    SizeType count;
};

template <typename Enable, typename Allocator, typename SizeType, typename... Args>
struct __has_allocate_at_least : std::false_type {};
template <typename Allocator, typename SizeType, typename... Args>
struct __has_allocate_at_least<
    std::void_t<decltype(std::declval<Allocator>().allocate_at_least(
        std::declval<SizeType>()))>,
    Allocator, SizeType, Args...> : std::true_type {};
template <typename Allocator, typename SizeType, typename... Args>
struct has_allocate_at_least
    : __has_allocate_at_least<void, Allocator, SizeType, Args...> {};
template <typename Allocator, typename SizeType, typename... Args>
constexpr bool has_allocate_at_least_v =
    has_allocate_at_least<Allocator, SizeType, Args...>::value;

template <typename Allocator, typename SizeType,
          typename Pointer = typename std::allocator_traits<Allocator>::pointer>
WJR_NODISCARD allocation_result<Pointer, SizeType> allocate_at_least(Allocator &alloc,
                                                                     SizeType count) {
    if constexpr (has_allocate_at_least_v<Allocator, SizeType>) {
        auto result = alloc.allocate_at_least(count);
        WJR_ASSUME(result.count >= count);
        return result;
    } else {
        auto ptr = std::allocator_traits<Allocator>::allocate(alloc, count);
        return {ptr, count};
    }
}

} // namespace wjr

#endif // WJR_MEMORY_TO_ADDRESS_HPP__