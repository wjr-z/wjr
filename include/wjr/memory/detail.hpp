#ifndef WJR_MEMORY_DETAIL_HPP__
#define WJR_MEMORY_DETAIL_HPP__

#include <cstring>

#include <wjr/iterator/detail.hpp>

namespace wjr {

WJR_REGISTER_HAS_TYPE(pointer_traits_to_address,
                      std::pointer_traits<Ptr>::to_address(std::declval<const Ptr &>()),
                      Ptr);

WJR_REGISTER_HAS_TYPE(pointer_access, std::declval<const Ptr &>().operator->(), Ptr);

template <typename T>
constexpr T *to_address(T *p) noexcept {
    static_assert(!std::is_function_v<T>, "T cannot be a function.");
    return p;
}

/**
 * @detail If std::pointer_traits<remove_cvref_t<Ptr>>::to_address(p) is valid, return
 * std::pointer_traits<remove_cvref_t<Ptr>>::to_address(p), otherwise return
 * to_address(p.operator->()).
 */
template <typename Ptr>
constexpr auto to_address(const Ptr &p) noexcept {
    if constexpr (has_pointer_traits_to_address_v<remove_cvref_t<Ptr>>) {
        return std::pointer_traits<remove_cvref_t<Ptr>>::to_address(p);
    } else {
        return wjr::to_address(p.operator->());
    }
}

/**
 * @return to_address(p.base()).
 *
 */
template <typename Iter>
constexpr auto to_address(const std::move_iterator<Iter> &p) noexcept {
    return wjr::to_address(p.base());
}

/**
 * @brief Return to_address(p) if p is a contiguous iterator and contiguouse iterato check
 * is disabled, otherwise return p.
 *
 */
template <typename T>
constexpr decltype(auto) to_contiguous_address(T &&t) noexcept {
#if !WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
    if constexpr (is_contiguous_iterator_v<remove_cvref_t<T>>) {
        return wjr::to_address(std::forward<T>(t));
    } else {
#endif
        return std::forward<T>(t);
#if !WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
    }
#endif
}

/// @private
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
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 T builtin_byteswap(T x) noexcept {
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

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 T byteswap(T x, endian to = endian::little) noexcept {
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

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_PURE WJR_INTRINSIC_INLINE T read_memory(const void *ptr,
                                            endian to = endian::little) noexcept {
    T x;
    std::memcpy(&x, ptr, sizeof(T));

    if (to != endian::native) {
        x = byteswap(x);
    }

    return x;
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
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

WJR_REGISTER_HAS_TYPE(
    allocate_at_least,
    std::declval<Allocator>().allocate_at_least(std::declval<SizeType>()), Allocator,
    SizeType);

template <typename Allocator, typename SizeType,
          typename Pointer = typename std::allocator_traits<Allocator>::pointer>
WJR_NODISCARD auto allocate_at_least(Allocator &alloc, SizeType count) {
    if constexpr (has_allocate_at_least_v<Allocator, SizeType>) {
        return alloc.allocate_at_least(count);
    } else {
        const auto ptr = std::allocator_traits<Allocator>::allocate(alloc, count);
        return allocation_result<decltype(ptr), SizeType>{ptr, count};
    }
}

} // namespace wjr

#endif // WJR_MEMORY_DETAIL_HPP__