#ifndef WJR_TUPLE_HPP__
#define WJR_TUPLE_HPP__

#include <wjr/compressed_pair.hpp>

namespace wjr {
template <typename... Args>
class tuple;
}

namespace std {

template <typename... Args, WJR_REQUIRES(std::conjunction_v<wjr::is_swappable<Args>...>)>
constexpr void swap(wjr::tuple<Args...> &lhs,
                    wjr::tuple<Args...> &rhs) noexcept(noexcept(lhs.swap(rhs)));

template <size_t I, typename... Args>
constexpr wjr::tuple_element_t<I, wjr::tuple<Args...>> &
get(wjr::tuple<Args...> &t) noexcept;

template <size_t I, typename... Args>
constexpr wjr::tuple_element_t<I, wjr::tuple<Args...>> &
get(const wjr::tuple<Args...> &t) noexcept;

template <size_t I, typename... Args>
constexpr wjr::tuple_element_t<I, wjr::tuple<Args...>> &&
get(wjr::tuple<Args...> &&t) noexcept;

template <size_t I, typename... Args>
constexpr wjr::tuple_element_t<I, wjr::tuple<Args...>> &&
get(const wjr::tuple<Args...> &&t) noexcept;

template <typename T, typename... Args>
constexpr T &get(wjr::tuple<Args...> &t) noexcept;

template <typename T, typename... Args>
constexpr T &get(const wjr::tuple<Args...> &t) noexcept;

template <typename T, typename... Args>
constexpr T &&get(wjr::tuple<Args...> &&t) noexcept;

template <typename T, typename... Args>
constexpr T &&get(const wjr::tuple<Args...> &&t) noexcept;

} // namespace std

namespace wjr {

template <typename Indexs, typename... Args>
class tuple_impl;

template <size_t... Indexs, typename... Args>
class WJR_EMPTY_BASES tuple_impl<std::index_sequence<Indexs...>, Args...>
    : capture_leaf<tuple_element_t<Indexs, tuple<Args...>>,
                   enable_base_identity_t<
                       Indexs, tuple_impl<std::index_sequence<Indexs...>, Args...>>>...,
      enable_special_members_of_args_base<
          void,
          capture_leaf<
              tuple_element_t<Indexs, tuple<Args...>>,
              enable_base_identity_t<
                  Indexs, tuple_impl<std::index_sequence<Indexs...>, Args...>>>...> {
    using Sequence = std::index_sequence<Indexs...>;

    template <size_t Idx>
    using Mybase = capture_leaf<tuple_element_t<Idx, tuple<Args...>>,
                                enable_base_identity_t<Idx, tuple_impl>>;

    using Mybase2 = enable_special_members_of_args_base<
        void, capture_leaf<tuple_element_t<Indexs, tuple<Args...>>,
                           enable_base_identity_t<Indexs, tuple_impl>>...>;

    constexpr static size_t Size = sizeof...(Args);

public:
    template <typename S = Sequence,
              WJR_REQUIRES(std::conjunction_v<std::is_same<S, Sequence>,
                                              std::is_constructible<Mybase<Indexs>>...>)>
    constexpr tuple_impl(Sequence) : Mybase2(enable_default_constructor) {}

    template <size_t... _Indexs, typename... _Args,
              WJR_REQUIRES(
                  std::conjunction_v<std::is_constructible<Mybase<_Indexs>, _Args>...>)>
    constexpr tuple_impl(std::index_sequence<_Indexs...>, _Args &&...args)
        : Mybase<_Indexs>(std::forward<_Args>(args))...,
          Mybase2(enable_default_constructor) {}

    template <size_t I>
    constexpr auto &get() & noexcept {
        return Mybase<I>::get();
    }

    template <size_t I>
    constexpr const auto &get() const & noexcept {
        return Mybase<I>::get();
    }

    template <size_t I>
    constexpr auto &&get() && noexcept {
        return std::move(Mybase<I>::get());
    }

    template <size_t I>
    constexpr const auto &&get() const && noexcept {
        return std::move(Mybase<I>::get());
    }
};

template <typename Tuple>
struct __tuple_like;

template <template <typename...> typename Tuple, typename... Args>
struct __tuple_like<Tuple<Args...>>
    : std::disjunction<std::is_same<Tuple<Args...>, std::tuple<Args...>>,
                       std::is_same<Tuple<Args...>, std::pair<Args...>>> {};

template <>
class tuple<> {
public:
    constexpr tuple() noexcept = default;
    constexpr tuple(const tuple &) noexcept = default;
    constexpr tuple(tuple &&) noexcept = default;
    constexpr tuple &operator=(const tuple &) noexcept = default;
    constexpr tuple &operator=(tuple &&) noexcept = default;
    ~tuple() noexcept = default;

    constexpr void swap(tuple &) noexcept {}
};

template <typename This, typename... Args>
class tuple<This, Args...>
    : enable_special_members_of_args_base<
          tuple<This, Args...>,
          tuple_impl<std::index_sequence_for<This, Args...>, This, Args...>> {
    using Sequence = std::index_sequence_for<This, Args...>;
    using Impl = tuple_impl<Sequence, This, Args...>;
    using Mybase = enable_special_members_of_args_base<tuple<This, Args...>, Impl>;

    constexpr static size_t Size = sizeof...(Args) + 1;

public:
    template <typename T = This,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<T>,
                                              std::is_constructible<Impl, Sequence>>)>
    constexpr tuple() : Mybase(enable_default_constructor), m_impl(Sequence()) {}

    template <typename Other = This,
              WJR_REQUIRES(std::is_constructible_v<Impl, Sequence, const Other &,
                                                   const Args &...>)>
    constexpr tuple(const Other &first, const Args &...rest)
        : Mybase(enable_default_constructor), m_impl(Sequence(), first, rest...) {}

    template <
        typename Other, typename... _Args,
        WJR_REQUIRES(sizeof...(_Args) + 1 == Size &&
                     std::conjunction_v<
                         std::negation<std::conjunction<
                             std::is_same<This, std::remove_reference_t<Other>>,
                             std::is_same<Args, std::remove_reference_t<_Args>>...>>,
                         std::is_constructible<Impl, Sequence, Other &&, _Args &&...>>)>
    constexpr tuple(Other &&other, _Args &&...args)
        : Mybase(enable_default_constructor),
          m_impl(Sequence(), std::forward<Other>(other), std::forward<_Args>(args)...) {}

private:
    template <size_t... _Indexs, typename TupleLike>
    constexpr tuple(std::index_sequence<_Indexs...>, TupleLike &&other, in_place_empty_t)
        : Mybase(enable_default_constructor),
          m_impl(Sequence(), std::get<_Indexs>(std::forward<TupleLike>(other))...) {}

public:
    template <typename TupleLike,
              WJR_REQUIRES(__is_tuple_test_v<std::is_constructible, tuple, TupleLike &&>)>
    constexpr tuple(TupleLike &&other)
        : tuple(Sequence(), std::forward<TupleLike>(other), in_place_empty) {}

private:
    template <size_t... _Indexs, typename Container>
    constexpr void __assign(std::index_sequence<_Indexs...>, Container &&other) {
        (void)((this->template get<_Indexs>() =
                    std::get<_Indexs>(std::forward<Container>(other))),
               ...);
    }

public:
    template <typename TupleLike,
              WJR_REQUIRES(__is_tuple_test_v<std::is_constructible, tuple, TupleLike &&>)>
    constexpr tuple &operator=(TupleLike &&other) {
        __assign(Sequence(), std::forward<TupleLike>(other));
        return *this;
    }

private:
    template <size_t... _Indexs>
    constexpr void __swap(std::index_sequence<_Indexs...>, tuple &other) {
        (void)((std::swap(this->template get<_Indexs>(), other.template get<_Indexs>()),
                ...));
    }

public:
    constexpr void swap(tuple &other) noexcept { __swap(Sequence(), other); }

    template <size_t I>
    constexpr tuple_element_t<I, tuple> &get() & noexcept {
        return m_impl.template get<I>();
    }

    template <size_t I>
    constexpr const tuple_element_t<I, tuple> &get() const & noexcept {
        return m_impl.template get<I>();
    }

    template <size_t I>
    constexpr tuple_element_t<I, tuple> &&get() && noexcept {
        return std::move(m_impl.template get<I>());
    }

    template <size_t I>
    constexpr const tuple_element_t<I, tuple> &&get() const && noexcept {
        return std::move(m_impl.template get<I>());
    }

private:
    Impl m_impl;
};

template <typename... Args>
tuple(Args...) -> tuple<Args...>;

template <typename T1, typename T2>
tuple(std::pair<T1, T2>) -> tuple<T1, T2>;

template <typename... Args>
tuple(std::tuple<Args...>) -> tuple<Args...>;

template <typename... Args>
constexpr tuple<unref_wrapper_t<Args>...> make_tuple(Args &&...args) {
    return tuple<unref_wrapper_t<Args>...>(std::forward<Args>(args)...);
}

template <typename... Args>
constexpr tuple<Args &...> tie(Args &...args) noexcept {
    return tuple<Args &...>(args...);
}

template <typename... Args>
constexpr tuple<Args &&...> forward_as_tuple(Args &&...args) noexcept {
    return tuple<Args &&...>(std::forward<Args>(args)...);
}

template <typename Func, typename Tuple, size_t... Indexs>
constexpr decltype(auto) apply_impl(Func &&fn, Tuple &&tp,
                                    std::index_sequence<Indexs...>) {
    return std::invoke(std::forward<Func>(fn),
                       std::get<Indexs>(std::forward<Tuple>(tp))...);
}

template <typename Func, typename Tuple>
constexpr decltype(auto) apply(Func &&fn, Tuple &&tp) {
    return apply_impl(std::forward<Func>(fn), std::forward<Tuple>(tp),
                      std::make_index_sequence<tuple_size_v<Tuple>>{});
}

template <size_t I, typename Tuple>
struct __tuple_cat_single_helper {
    static constexpr size_t Size = tuple_size_v<Tuple>;
    using type0 = tp_repeat_t<tp_list<std::integral_constant<size_t, I>>, Size>;
    using type1 = tp_make_index_sequence<Size>;
};

template <typename S, typename... Tuples>
struct __tuple_cat_helper_impl;

template <size_t... Indexs, typename... Tuples>
struct __tuple_cat_helper_impl<std::index_sequence<Indexs...>, Tuples...> {
    using type0 =
        tp_concat_t<typename __tuple_cat_single_helper<Indexs, Tuples>::type0...>;
    using type1 =
        tp_concat_t<typename __tuple_cat_single_helper<Indexs, Tuples>::type1...>;
};

template <typename... Tuples>
struct __tuple_cat_helper {
    using Sequence = std::index_sequence_for<Tuples...>;
    using Impl = __tuple_cat_helper_impl<Sequence, Tuples...>;
    using type0 = tp_make_std_index_sequence<typename Impl::type0>;
    using type1 = tp_make_std_index_sequence<typename Impl::type1>;
};

template <size_t... I0, size_t... I1, typename... Tuples>
constexpr decltype(auto) __tuple_cat_impl(std::index_sequence<I0...>,
                                          std::index_sequence<I1...>,
                                          tuple<Tuples...> &&tuples) {
    return tuple(std::get<I1>(std::get<I0>(std::move(tuples)))...);
}

template <typename... Tuples>
constexpr decltype(auto) tuple_cat(Tuples &&...tuples) {
    using Helper = __tuple_cat_helper<Tuples...>;
    return __tuple_cat_impl(typename Helper::type0{}, typename Helper::type1{},
                            forward_as_tuple(std::forward<Tuples>(tuples)...));
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator==(const tuple<TArgs...> &lhs, const tuple<UArgs...> &rhs) {
    return apply(
        [&rhs](const auto &...lhs_args) {
            return apply(
                [&lhs_args...](const auto &...rhs_args) {
                    return ((lhs_args == rhs_args) && ...);
                },
                rhs);
        },
        lhs);
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator!=(const tuple<TArgs...> &lhs, const tuple<UArgs...> &rhs) {
    return !(lhs == rhs);
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator<(const tuple<TArgs...> &lhs, const tuple<UArgs...> &rhs) {
    bool ret = false;
    apply(
        [&rhs, &ret](const auto &...lhs_args) {
            return apply(
                [&lhs_args..., &ret](const auto &...rhs_args) {
                    (void)((lhs_args < rhs_args ? (ret = true, false)
                                                : (rhs_args < lhs_args ? false : true)) &&
                           ...);
                },
                rhs);
        },
        lhs);
    return ret;
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator<=(const tuple<TArgs...> &lhs, const tuple<UArgs...> &rhs) {
    return !(rhs < lhs);
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator>(const tuple<TArgs...> &lhs, const tuple<UArgs...> &rhs) {
    return rhs < lhs;
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator>=(const tuple<TArgs...> &lhs, const tuple<UArgs...> &rhs) {
    return !(lhs < rhs);
}

} // namespace wjr

namespace std {

template <typename... Args,
          WJR_REQUIRES_I(std::conjunction_v<wjr::is_swappable<Args>...>)>
constexpr void swap(wjr::tuple<Args...> &lhs,
                    wjr::tuple<Args...> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <size_t I, typename... Args>
constexpr wjr::tuple_element_t<I, wjr::tuple<Args...>> &
get(wjr::tuple<Args...> &t) noexcept {
    return t.template get<I>();
}

template <size_t I, typename... Args>
constexpr wjr::tuple_element_t<I, wjr::tuple<Args...>> &
get(const wjr::tuple<Args...> &t) noexcept {
    return t.template get<I>();
}

template <size_t I, typename... Args>
constexpr wjr::tuple_element_t<I, wjr::tuple<Args...>> &&
get(wjr::tuple<Args...> &&t) noexcept {
    return std::move(t).template get<I>();
}

template <size_t I, typename... Args>
constexpr wjr::tuple_element_t<I, wjr::tuple<Args...>> &&
get(const wjr::tuple<Args...> &&t) noexcept {
    return std::move(t).template get<I>();
}

template <typename T, typename... Args>
constexpr T &get(wjr::tuple<Args...> &t) noexcept {
    return get<wjr::tp_find_v<wjr::tuple<Args...>, T>>(t);
}

template <typename T, typename... Args>
constexpr T &get(const wjr::tuple<Args...> &t) noexcept {
    return get<wjr::tp_find_v<wjr::tuple<Args...>, T>>(t);
}

template <typename T, typename... Args>
constexpr T &&get(wjr::tuple<Args...> &&t) noexcept {
    return get<wjr::tp_find_v<wjr::tuple<Args...>, T>>(std::move(t));
}

template <typename T, typename... Args>
constexpr T &&get(const wjr::tuple<Args...> &&t) noexcept {
    return get<wjr::tp_find_v<wjr::tuple<Args...>, T>>(std::move(t));
}

} // namespace std

#endif // WJR_TUPLE_HPP__