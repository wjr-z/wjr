#ifndef WJR_FUNCREF_CV
    #error "WJR_FUNCREF_CV must be defined"
#endif

#ifndef WJR_FUNCREF_NOEXCEPT
    #error "WJR_FUNCREF_NOEXCEPT must be defined"
#endif

namespace wjr {
template <typename Ret, typename... Args>
class function_ref<Ret(Args...) WJR_FUNCREF_CV noexcept(WJR_FUNCREF_NOEXCEPT)> {
    using Invoker = func_detail::invoker<WJR_FUNCREF_NOEXCEPT, Ret, Args...>;
    using func_ptr_t = typename Invoker::func_ptr_t;

    template <typename... Tps>
    static constexpr bool _is_invocable_using =
        std::conditional_t<WJR_FUNCREF_NOEXCEPT, std::is_nothrow_invocable_r<Ret, Tps..., Args...>,
                           std::is_invocable_r<Ret, Tps..., Args...>>::value;

public:
    function_ref() = delete;
    function_ref(const function_ref &) = default;

    template <typename Fn>
    requires(std::is_function_v<Fn> && _is_invocable_using<Fn *>)
    function_ref(Fn *f) noexcept : m_func(Invoker::template _s_ptr<Fn *>()) {
        func_detail::init_ptr(m_ptr, f);
    }

    template <typename F, typename VT = remove_cvref_t<F>>
    requires(!std::is_same_v<VT, function_ref> && !std::is_member_pointer_v<VT> &&
             !std::is_function_v<VT> && _is_invocable_using<VT WJR_FUNCREF_CV &>)
    function_ref(F &&f) noexcept : m_func(Invoker::template _s_ptr<VT WJR_FUNCREF_CV &>()) {
        func_detail::init_ptr(m_ptr, std::addressof(f));
    }

    template <auto _fn>
    requires(_is_invocable_using<decltype(_fn)>)
    function_ref(nontype_t<_fn>) noexcept {
        using Fn = remove_cvref_t<decltype(_fn)>;
        if constexpr (std::is_pointer_v<Fn> || std::is_member_function_pointer_v<Fn>)
            static_assert(_fn != nullptr, "function pointer cannot be null");

        m_func = Invoker::template _S_nt_ptr<_fn>();
        // m_ptr is unused in this case
    }

    template <typename Tp>
    requires(!std::is_same_v<Tp, function_ref> && !std::is_pointer_v<Tp>)
    function_ref &operator=(Tp) = delete;

    Ret operator()(Args... args) const noexcept(WJR_FUNCREF_NOEXCEPT) {
        return m_func(m_ptr, std::forward<Args>(args)...);
    }

private:
    func_ptr_t m_func;
    func_detail::Ptr m_ptr;
};
} // namespace wjr

#undef WJR_FUNCREF_CV
#undef WJR_FUNCREF_NOEXCEPT