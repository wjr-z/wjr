#ifndef WJR_MATH_DETAIL_BIT_CAST_HPP__
#define WJR_MATH_DETAIL_BIT_CAST_HPP__

#include <type_traits>

#include <wjr/config/attribute.hpp>
#include <wjr/config/builtin.hpp>
#include <wjr/config/compiler.hpp>
#include <wjr/config/has.hpp>
#include <wjr/macros/requires.hpp>
#include <wjr/memory/detail.hpp>

#if WJR_HAS_BUILTIN(__builtin_bit_cast) || WJR_HAS_MSVC(19, 27)
    #define WJR_HAS_BUILTIN_BIT_CAST WJR_HAS_DEF
#endif

namespace wjr {
template <typename To, typename From,
          WJR_REQUIRES(sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> &&
                       std::is_trivially_copyable_v<To>)>
WJR_PURE WJR_INTRINSIC_INLINE To bit_cast(const From &src) noexcept {
    static_assert(std::is_trivially_constructible_v<To>);
    #if WJR_HAS_BUILTIN(BIT_CAST)
    return __builtin_bit_cast(To, src);
    #else
    To dst;
    builtin_memcpy(std::addressof(dst), std::addressof(src), sizeof(To));
    return dst;
    #endif
}
} // namespace wjr

#endif // WJR_MATH_DETAIL_BIT_CAST_HPP__