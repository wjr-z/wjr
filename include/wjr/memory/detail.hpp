#ifndef WJR_MEMORY_DETAIL_HPP__
#define WJR_MEMORY_DETAIL_HPP__

#include <array>
#include <cstring>
#include <memory>

#include <wjr/iterator/detail.hpp>

namespace wjr {

template <typename T, typename D>
struct get_relocate_mode<std::unique_ptr<T, D>> {
    static constexpr relocate_t value = relocate_t::maybe_trivial;
};

#if defined(WJR_CPP_20)
using std::to_address;
#else

WJR_REGISTER_HAS_TYPE(pointer_traits_to_address,
                      std::pointer_traits<Ptr>::to_address(std::declval<const Ptr &>()), Ptr);

template <typename T>
constexpr T *to_address(T *p) noexcept {
    static_assert(!std::is_function_v<T>, "T cannot be a function.");
    return p;
}

/**
 * @details If std::pointer_traits<remove_cvref_t<Ptr>>::to_address(p) is valid,
 * return std::pointer_traits<remove_cvref_t<Ptr>>::to_address(p), otherwise
 * return to_address(p.operator->()).
 */
template <typename Ptr>
constexpr auto to_address(const Ptr &p) noexcept {
    if constexpr (has_pointer_traits_to_address_v<remove_cvref_t<Ptr>>) {
        return std::pointer_traits<remove_cvref_t<Ptr>>::to_address(p);
    } else {
        return wjr::to_address(p.operator->());
    }
}
#endif

/**
 * @return to_address(p.base()).
 *
 */
template <typename Iter>
constexpr auto to_address(const std::move_iterator<Iter> &p) noexcept {
    return wjr::to_address(p.base());
}

/**
 * @brief Return to_address(p) if p is a contiguous iterator and contiguouse
 * iterato check is disabled, otherwise return p.
 *
 */
template <typename Iter>
constexpr decltype(auto) to_contiguous_address(Iter &&t) noexcept {
#if !WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
    if constexpr (is_contiguous_iterator_v<remove_cvref_t<Iter>>) {
        return wjr::to_address(std::forward<Iter>(t));
    } else {
#endif
        return std::forward<Iter>(t);
#if !WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
    }
#endif
}

template <typename T>
constexpr T *to_contiguous_address(T *ptr) noexcept {
    return ptr;
}

template <typename Iter>
constexpr auto __unwrap_iter(Iter iter) {
    // todo: ...
    return to_contiguous_address(iter);
}

template <typename Iter>
constexpr auto __niter_base(Iter iter) {
    return __unwrap_iter(iter);
}

template <typename Iter>
constexpr auto __niter_wrap(const Iter &, Iter res) {
    return res;
}

template <typename From, typename To>
constexpr auto __niter_wrap(const From &from, To res) {
    return from + (res - __niter_base(from));
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
    native = __is_little_endian_helper::value ? little : big,
};

inline constexpr bool is_little_endian = endian::native == endian::little;
inline constexpr bool is_big_endian = !is_little_endian;

static_assert(
    is_little_endian,
    "Don't support big endian. There are currently many optimizations that rely on small end sequences,\
    which may gradually become compatible in the future");

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T fallback_byteswap(T x) noexcept {
    constexpr auto digits = std::numeric_limits<T>::digits;
    const auto val = static_cast<uint_t<digits>>(x);
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
               ((val << 24) & 0xff0000000000) | ((val << 40) & 0xff000000000000) | ((val << 56));
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
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 T byteswap(T x) noexcept {
#if WJR_HAS_BUILTIN(BYTESWAP)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_byteswap(x);
    }

    return builtin_byteswap(x);
#else
    return fallback_byteswap(x);
#endif
}

WJR_INTRINSIC_INLINE void builtin_memcpy(void *dst, void const *src, size_t size) noexcept {
#if WJR_HAS_BUILTIN(MEMCPY)
    (void)__builtin_memcpy(dst, src, size);
#else
    (void)std::memcpy(dst, src, size);
#endif
}

WJR_INTRINSIC_INLINE void maybe_null_memcpy(void *dst, void const *src, size_t size) noexcept {
    if (WJR_UNLIKELY(size == 0)) {
        return;
    }

    std::memcpy(dst, src, size);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_PURE WJR_INTRINSIC_INLINE T read_memory(const void *ptr) noexcept {
    T x;
    builtin_memcpy(std::addressof(x), ptr, sizeof(T));
    return x;
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_INLINE void write_memory(void *ptr, T x) noexcept {
    builtin_memcpy(ptr, std::addressof(x), sizeof(T));
}

} // namespace wjr

#endif // WJR_MEMORY_DETAIL_HPP__