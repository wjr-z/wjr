#ifndef WJR_ACTION_UNROLL_HPP__
#define WJR_ACTION_UNROLL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

// preview :
// The GCC lower version of options lacks optimization for trial types and cannot achieve
// zero overhead.

template <size_t unroll, typename Fn, typename Ret, size_t idx>
WJR_INTRINSIC_CONSTEXPR Ret unroll_call_impl(Fn &&fn) {
#define WJR_REGISTER_UNROLL_CALL_IMPL(idxs)                                              \
    WJR_PP_TRANSFORM_PUT(idxs, WJR_REGISTER_UNROLL_CALL_IMPL_CALLER)
#define WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(offset)                                     \
    do {                                                                                 \
        if constexpr (std::is_same_v<Ret, void>) {                                       \
            fn(std::integral_constant<size_t, idx + offset>{});                          \
        } else {                                                                         \
            auto ret = fn(std::integral_constant<size_t, idx + offset>{});               \
                                                                                         \
            if (ret.has_value()) {                                                       \
                return ret;                                                              \
            }                                                                            \
        }                                                                                \
    } while (0);

    constexpr auto delta = unroll - idx;

    if constexpr (delta >= 8) {
        WJR_REGISTER_UNROLL_CALL_IMPL((0, 1, 2, 3, 4, 5, 6, 7));
        return unroll_call_impl<unroll, Fn, Ret, idx + 8>(std::forward<Fn>(fn));
    } else {
        if constexpr (delta >= 1) {
            WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(0);
        }
        if constexpr (delta >= 2) {
            WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(1);
        }
        if constexpr (delta >= 3) {
            WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(2);
        }
        if constexpr (delta >= 4) {
            WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(3);
        }
        if constexpr (delta >= 5) {
            WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(4);
        }
        if constexpr (delta >= 6) {
            WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(5);
        }
        if constexpr (delta >= 7) {
            WJR_REGISTER_UNROLL_CALL_IMPL_CALLER(6);
        }

        if constexpr (std::is_same_v<Ret, void>) {
            return;
        } else {
            return std::nullopt;
        }
    }
#undef WJR_REGISTER_UNROLL_CALL_IMPL_CALLER
#undef WJR_REGISTER_UNROLL_CALL_IMPL
}

template <size_t unroll, typename Fn,
          typename Ret = std::invoke_result_t<Fn, std::integral_constant<size_t, 0>>>
WJR_INTRINSIC_CONSTEXPR Ret unroll_call(Fn &&fn) {
    return unroll_call_impl<unroll, Fn, Ret, 0>(std::forward<Fn>(fn));
}

} // namespace wjr

#endif // WJR_ACTION_UNROLL_HPP__