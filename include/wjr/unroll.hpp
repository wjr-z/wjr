#ifndef WJR_UNROLL_HPP__
#define WJR_UNROLL_HPP__

#include <optional>
#include <wjr/type_traits.hpp>

namespace wjr {

template <typename Ret, size_t unroll, typename Fn, size_t idx>
WJR_INTRINSIC_CONSTEXPR Ret unroll_call_impl(Fn &fn) {
#define WJR_REGISTER_UNROLL_CALL_IMPL(idxs)                                              \
    WJR_PP_TRANSFORM_PUT(idxs, WJR_REGISTER_UNROLL_CALL_IMPL_CALLER)
#define WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(offset)                                     \
    do {                                                                                 \
        auto ret = fn(std::integral_constant<size_t, idx + offset>{});                   \
                                                                                         \
        if (ret.has_value()) {                                                           \
            return ret.value();                                                          \
        }                                                                                \
                                                                                         \
    } while (0);

    if constexpr (idx == unroll) {
        return std::nullopt;
    } else if constexpr (unroll - idx >= 8) {
        WJR_REGISTER_UNROLL_CALL_IMPL((0, 1, 2, 3, 4, 5, 6, 7));
        return unroll_call_impl<Ret, unroll, Fn, idx + 8>(fn);
    } else if constexpr (unroll - idx >= 4) {
        WJR_REGISTER_UNROLL_CALL_IMPL((0, 1, 2, 3));
        return unroll_call_impl<Ret, unroll, Fn, idx + 4>(fn);
    } else {
        WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(0);
        return unroll_call_impl<Ret, unroll, Fn, idx + 1>(fn);
    }

#undef WJR_REGISTER_UNROLL_CALL_IMPL_CALLER
#undef WJR_REGISTER_UNROLL_CALL_IMPL
}

template <typename Ret, size_t unroll, typename Fn>
WJR_INTRINSIC_CONSTEXPR Ret unroll_call(Fn fn) {
    return unroll_call_impl<Ret, unroll, Fn, 0>(fn);
}

} // namespace wjr

#endif // WJR_UNROLL_HPP__