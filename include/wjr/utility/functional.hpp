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

#include <functional>

#include <wjr/type_traits.hpp>

namespace wjr {

#ifdef __cpp_lib_invoke_r
using std::invoke_r;
#else
template <class R, class F, class... Args>
requires(std::is_invocable_r_v<R, F, Args...>)
constexpr R invoke_r(F &&f, Args &&...args) noexcept(std::is_nothrow_invocable_r_v<R, F, Args...>) {
    if constexpr (std::is_void_v<R>)
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    else
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}
#endif

namespace func_detail {
union Ptr {
    const volatile void *obj;
    void (*func)();
};

/// @brief Keep trivial values in the call ABI and forward other existing parameters.
template <typename Arg>
using thunk_arg_t = std::conditional_t<std::is_trivially_move_constructible_v<Arg> &&
                                           std::is_trivially_destructible_v<Arg>,
                                       Arg, Arg &&>;

/// @details Keep the thunk outside the class template for Clang issue #90760.
template <typename Target, bool Noex, typename Ret, typename... Args>
WJR_INTRINSIC_INLINE static Ret _S_call_ptrs(Ptr ptr, thunk_arg_t<Args>... args) noexcept(Noex) {
    if constexpr (std::is_function_v<std::remove_pointer_t<Target>>) {
        return invoke_r<Ret>(reinterpret_cast<Target>(ptr.func), std::forward<Args>(args)...);
    } else {
        using Object = std::remove_reference_t<Target>;
        auto &object = *static_cast<Object *>(const_cast<void *>(ptr.obj));
        return invoke_r<Ret>(object, std::forward<Args>(args)...);
    }
}

template <auto Fn, bool Noex, typename Ret, typename... Args>
static Ret _S_nt_ptrs(Ptr, thunk_arg_t<Args>... args) noexcept(Noex) {
    return invoke_r<Ret>(Fn, std::forward<Args>(args)...);
}

template <bool Noex, typename Ret, typename... Args>
struct invoker {
    // Only inspect parameter traits when constructing or calling a reference, so
    // declarations with incomplete by-value parameter types retain a complete wrapper.
    using func_ptr_t = void (*)();

    template <typename... Parameters>
    using thunk_ptr_t = Ret (*)(Ptr, thunk_arg_t<Parameters>...) noexcept(Noex);

    template <typename Tp>
    static func_ptr_t _s_ptr() noexcept {
        return reinterpret_cast<func_ptr_t>(&_S_call_ptrs<Tp, Noex, Ret, Args...>);
    }

    template <auto Fn>
    static func_ptr_t _S_nt_ptr() noexcept {
        return reinterpret_cast<func_ptr_t>(&_S_nt_ptrs<Fn, Noex, Ret, Args...>);
    }
};

template <typename Tp>
constexpr void init_ptr(Ptr &ptr, Tp *p) noexcept {
    if constexpr (std::is_function_v<Tp>) {
        ptr.func = reinterpret_cast<void (*)()>(p);
    } else {
        ptr.obj = static_cast<const volatile void *>(p);
    }
}

} // namespace func_detail

template <auto V>
struct nontype_t {
    explicit nontype_t() = default;
};

template <auto V>
constexpr nontype_t<V> nontype{};

/**
 * @brief A non-owning callable reference.
 * @details Callable objects are borrowed and must outlive every invocation.
 * Construction does not extend temporary callable lifetimes.
 */
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