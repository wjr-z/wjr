#ifndef WJR_FUNCREF_CV
    #define WJR_FUNCREF_CV
#endif

namespace wjr {
template <typename Ret, typename... Args, bool Noex>
class function_ref<Ret(Args...) WJR_FUNCREF_CV noexcept(Noex)> {
    using Invoker = func_detail::invoker<Noex, Ret, Args...>;
    using func_ptr_t = typename Invoker::func_ptr_t;

    template <typename... Tps>
    static constexpr bool __is_invocable_using =
        std::conditional_t<Noex, std::is_nothrow_invocable_r<Ret, Tps..., Args...>,
                           std::is_invocable_r<Ret, Tps..., Args...>>::value;

public:
    function_ref() = delete;
    function_ref(const function_ref &) = default;

    template <typename Fn, WJR_REQUIRES(std::is_function_v<Fn> &&__is_invocable_using<Fn *>)>
    function_ref(Fn *f) noexcept : m_func(Invoker::template __s_ptr<Fn *>()) {
        func_detail::init_ptr(m_ptr, f);
    }

    template <typename F, typename VT = remove_cvref_t<F>,
              WJR_REQUIRES(!std::is_same_v<VT, function_ref> && !std::is_member_pointer_v<VT> &&
                           !std::is_function_v<VT> && __is_invocable_using<VT WJR_FUNCREF_CV &>)>
    function_ref(F &&f) noexcept : m_func(Invoker::template __s_ptr<VT WJR_FUNCREF_CV &>()) {
        func_detail::init_ptr(m_ptr, std::addressof(f));
    }

    template <typename Tp,
              WJR_REQUIRES(!std::is_same_v<Tp, function_ref> && !std::is_pointer_v<Tp>)>
    function_ref &operator=(Tp) = delete;

    Ret operator()(Args... args) const noexcept(Noex) {
        return m_func(m_ptr, std::forward<Args>(args)...);
    }

private:
    func_ptr_t m_func;
    func_detail::Ptr m_ptr;
};
} // namespace wjr

#undef WJR_FUNCREF_CV