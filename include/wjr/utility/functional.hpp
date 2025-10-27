/**
 * @file functional.hpp
 * @brief Functional programming utilities
 * @author wjr
 *
 * Provides function wrappers and utilities including function_ref,
 * invoke_r, and related helpers for functional programming.
 */

#ifndef WJR_UTILITY_FUNCTIONAL_HPP__
#define WJR_UTILITY_FUNCTIONAL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

#ifdef __cpp_lib_invoke_r
using std::invoke_r;
#else
template <class R, class F, class... Args, WJR_REQUIRES(std::is_invocable_r_v<R, F, Args...>)>
constexpr R invoke_r(F &&f, Args &&...args) noexcept(std::is_nothrow_invocable_r_v<R, F, Args...>) {
    if constexpr (std::is_void_v<R>)
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    else
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}
#endif

namespace func_detail {
union Ptr {
    const void *obj;
    void (*func)();
};

template <typename Tp>
WJR_INTRINSIC_INLINE constexpr auto *_cast_to(Ptr ptr) noexcept {
    using Td = std::remove_reference_t<Tp>;
    if constexpr (std::is_function_v<Td>)
        return reinterpret_cast<Td *>(ptr.func);
    else if constexpr (std::is_const_v<Td>)
        return static_cast<Td *>(ptr.obj);
    else
        return static_cast<Td *>(const_cast<void *>(ptr.obj));
}

/// @details clang bug [issue 90760](https://github.com/llvm/llvm-project/pull/90760)
template <typename Tp, bool Noex, typename Ret, typename... Args>
WJR_INTRINSIC_INLINE static Ret _S_call_ptrs(Ptr ptr, Args... args) noexcept(Noex) {
    if constexpr (std::is_function_v<std::remove_pointer_t<Tp>>)
        return invoke_r<Ret>(reinterpret_cast<Tp>(ptr.func), std::forward<Args>(args)...);
    else {
        auto *_p = _cast_to<Tp>(ptr);
        return invoke_r<Ret>(static_cast<Tp>(*_p), std::forward<Args>(args)...);
    }
}

template <auto _fn, bool Noex, typename Ret, typename... Args>
static Ret _S_nt_ptrs(Ptr, Args... args) noexcept(Noex) {
    return invoke_r<Ret>(_fn, std::forward<Args>(args)...);
}

template <bool Noex, typename Ret, typename... Args>
struct invoker {
    using func_ptr_t = Ret (*)(Ptr, Args...) noexcept(Noex);

    template <typename Tp>
    static constexpr func_ptr_t _s_ptr() {
        return &_S_call_ptrs<Tp, Noex, Ret, Args...>;
    }

    template <auto _fn>
    static constexpr auto _S_nt_ptr() {
        return &_S_nt_ptrs<_fn, Noex, Ret, Args...>;
    }

private:
    template <typename Tp, typename Td = remove_cvref_t<Tp>>
    using _adjust_target =
        std::conditional_t<std::is_pointer_v<Td> || std::is_member_pointer_v<Td>, Td, Tp>;
};

template <typename Tp>
constexpr void init_ptr(Ptr &ptr, Tp *p) noexcept {
    if constexpr (std::is_function_v<Tp>) {
        ptr.func = reinterpret_cast<void (*)()>(p);
    } else {
        ptr.obj = static_cast<const void *>(p);
    }
}

} // namespace func_detail

#ifndef WJR_CPP_26
template <auto V>
struct nontype_t {
    explicit nontype_t() = default;
};

template <auto V>
constexpr nontype_t<V> nontype{};
#else
using std::nontype;
using std::nontype_t;
#endif

template <typename F>
class function_ref;

} // namespace wjr

#define WJR_FUNCREF_CV
#define WJR_FUNCREF_NOEXCEPT false
#include "funcref_impl.hpp"

#define WJR_FUNCREF_CV
#define WJR_FUNCREF_NOEXCEPT true
#include "funcref_impl.hpp"

#define WJR_FUNCREF_CV const
#define WJR_FUNCREF_NOEXCEPT false
#include "funcref_impl.hpp"

#define WJR_FUNCREF_CV const
#define WJR_FUNCREF_NOEXCEPT true
#include "funcref_impl.hpp"

#endif // WJR_UTILITY_FUNCTIONAL_HPP__