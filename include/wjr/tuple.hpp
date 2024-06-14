#ifndef WJR_TUPLE_HPP__
#define WJR_TUPLE_HPP__

#include <tuple>

#include <wjr/capture_leaf.hpp>
#include <wjr/math/integral_constant.hpp>

namespace wjr {

template <typename... Args>
class tuple;

} // namespace wjr

namespace std {

template <typename... Args>
struct tuple_size<wjr::tuple<Args...>> : std::integral_constant<size_t, sizeof...(Args)> {
};

template <size_t I, typename... Args>
struct tuple_element<I, wjr::tuple<Args...>> {
    using type = wjr::tp_at_t<wjr::tuple<Args...>, I>;
};

template <typename... Args, WJR_REQUIRES(std::conjunction_v<wjr::is_swappable<Args>...>)>
constexpr void swap(wjr::tuple<Args...> &lhs,
                    wjr::tuple<Args...> &rhs) noexcept(noexcept(lhs.swap(rhs)));

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &get(wjr::tuple<Args...> &t) noexcept;

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &
get(const wjr::tuple<Args...> &t) noexcept;

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &&get(wjr::tuple<Args...> &&t) noexcept;

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &&
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
    : capture_leaf<Args,
                   enable_base_identity_t<
                       Indexs, tuple_impl<std::index_sequence<Indexs...>, Args...>>>... {
    using Sequence = std::index_sequence<Indexs...>;

    template <size_t Idx>
    using Mybase = capture_leaf<std::tuple_element_t<Idx, tuple<Args...>>,
                                enable_base_identity_t<Idx, tuple_impl>>;

    constexpr static size_t Size = sizeof...(Args);

    using Tuple = tuple<Args...>;

public:
    template <typename S = Sequence,
              WJR_REQUIRES(
                  std::conjunction_v<std::is_same<S, Sequence>,
                                     std::is_default_constructible<Mybase<Indexs>>...>)>
    constexpr tuple_impl() noexcept(
        std::conjunction_v<std::is_nothrow_constructible<Args>...>) {}

    template <size_t... _Indexs, typename... _Args,
              WJR_REQUIRES(
                  std::conjunction_v<std::is_constructible<Mybase<_Indexs>, _Args>...>)>
    constexpr tuple_impl(std::index_sequence<_Indexs...>, _Args &&...args) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<Args, _Args &&>...>)
        : Mybase<_Indexs>(std::forward<_Args>(args))... {}

    template <size_t I>
    constexpr std::tuple_element_t<I, Tuple> &get() & noexcept {
        return Mybase<I>::get();
    }

    template <size_t I>
    constexpr const std::tuple_element_t<I, Tuple> &get() const & noexcept {
        return Mybase<I>::get();
    }

    template <size_t I>
    constexpr std::tuple_element_t<I, Tuple> &&get() && noexcept {
        return static_cast<std::tuple_element_t<I, Tuple> &&>(get<I>());
    }

    template <size_t I>
    constexpr const std::tuple_element_t<I, Tuple> &&get() const && noexcept {
        return static_cast<const std::tuple_element_t<I, Tuple> &&>(get<I>());
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
class tuple<This, Args...> {
    using Sequence = std::index_sequence_for<This, Args...>;
    using Impl = tuple_impl<Sequence, This, Args...>;

    constexpr static size_t Size = sizeof...(Args) + 1;

public:
#if defined(__cpp_conditional_explicit)
    template <typename T = This,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<T>,
                                              std::is_default_constructible<Args>...>)>
    constexpr explicit(
        !std::conjunction_v<is_default_convertible<T>, is_default_convertible<Args>...>)
        tuple() noexcept(std::is_nothrow_constructible_v<Impl>)
        : m_impl() {}
#else
    template <typename T = This,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<T>,
                                              std::is_default_constructible<Args>...>
                               &&std::conjunction_v<is_default_convertible<T>,
                                                    is_default_convertible<Args>...>)>
    constexpr tuple() noexcept(std::is_nothrow_constructible_v<Impl>) : m_impl() {}

    template <typename T = This,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<T>,
                                              std::is_default_constructible<Args>...> &&
                           !std::conjunction_v<is_default_convertible<T>,
                                               is_default_convertible<Args>...>)>
    constexpr explicit tuple() noexcept(std::is_nothrow_constructible_v<Impl>)
        : m_impl() {}
#endif

    template <typename Other = This,
              WJR_REQUIRES(std::is_constructible_v<Impl, Sequence, const Other &,
                                                   const Args &...>)>
    constexpr tuple(const Other &first, const Args &...rest) noexcept(
        std::is_nothrow_constructible_v<Impl, Sequence, const Other &, const Args &...>)
        : m_impl(Sequence(), first, rest...) {}

    template <
        typename Other, typename... _Args,
        WJR_REQUIRES(sizeof...(_Args) + 1 == Size &&
                     std::conjunction_v<
                         std::negation<std::conjunction<
                             std::is_same<This, std::remove_reference_t<Other>>,
                             std::is_same<Args, std::remove_reference_t<_Args>>...>>,
                         std::is_constructible<Impl, Sequence, Other &&, _Args &&...>>)>
    constexpr tuple(Other &&other, _Args &&...args) noexcept(
        std::is_nothrow_constructible_v<Impl, Sequence, Other &&, _Args &&...>)
        : m_impl(Sequence(), std::forward<Other>(other), std::forward<_Args>(args)...) {}

private:
    template <size_t... _Indexs, typename TupleLike>
    constexpr tuple(
        std::index_sequence<_Indexs...>, TupleLike &&other,
        in_place_empty_t) noexcept(std::
                                       is_nothrow_constructible_v<
                                           Impl, Sequence,
                                           decltype(std::get<_Indexs>(
                                               std::forward<TupleLike>(other)))...>)
        : m_impl(Sequence(), std::get<_Indexs>(std::forward<TupleLike>(other))...) {}

public:
    template <typename TupleLike,
              WJR_REQUIRES(__is_tuple_test_v<std::is_constructible, tuple, TupleLike &&>)>
    constexpr tuple(TupleLike &&other) noexcept(
        noexcept(tuple(Sequence(), std::forward<TupleLike>(other), in_place_empty)))
        : tuple(Sequence(), std::forward<TupleLike>(other), in_place_empty) {}

private:
    template <size_t... _Indexs, typename Container>
    constexpr void __assign(std::index_sequence<_Indexs...>, Container &&other) noexcept(
        noexcept(((this->template get<_Indexs>() =
                       std::get<_Indexs>(std::forward<Container>(other))),
                  ...))) {
        ((this->template get<_Indexs>() =
              std::get<_Indexs>(std::forward<Container>(other))),
         ...);
    }

public:
    template <typename TupleLike,
              WJR_REQUIRES(__is_tuple_test_v<__is_tuple_assignable, tuple, TupleLike &&>)>
    constexpr tuple &operator=(TupleLike &&other) noexcept(
        noexcept(__assign(Sequence(), std::forward<TupleLike>(other)))) {
        __assign(Sequence(), std::forward<TupleLike>(other));
        return *this;
    }

private:
    template <size_t... _Indexs>
    constexpr void
    __swap(std::index_sequence<_Indexs...>, tuple &other) noexcept(noexcept(((
        std::swap(this->template get<_Indexs>(), other.template get<_Indexs>()), ...)))) {
        ((std::swap(this->template get<_Indexs>(), other.template get<_Indexs>()), ...));
    }

public:
    constexpr void swap(tuple &other) noexcept(noexcept(__swap(Sequence(), other))) {
        __swap(Sequence(), other);
    }

    template <size_t I, WJR_REQUIRES(I < Size)>
    constexpr std::tuple_element_t<I, tuple> &get() & noexcept {
        return m_impl.template get<I>();
    }

    template <size_t I, WJR_REQUIRES(I < Size)>
    constexpr const std::tuple_element_t<I, tuple> &get() const & noexcept {
        return m_impl.template get<I>();
    }

    template <size_t I, WJR_REQUIRES(I < Size)>
    constexpr std::tuple_element_t<I, tuple> &&get() && noexcept {
        return std::move(m_impl).template get<I>();
    }

    template <size_t I, WJR_REQUIRES(I < Size)>
    constexpr const std::tuple_element_t<I, tuple> &&get() const && noexcept {
        return std::move(m_impl).template get<I>();
    }

    template <size_t I, WJR_REQUIRES(I >= 0 && I < Size)>
    constexpr std::tuple_element_t<I, tuple> &
    operator[](integral_constant<size_t, I>) & noexcept {
        return get<I>();
    }

    template <size_t I, WJR_REQUIRES(I >= 0 && I < Size)>
    constexpr const std::tuple_element_t<I, tuple> &
    operator[](integral_constant<size_t, I>) const & noexcept {
        return get<I>();
    }

    template <size_t I, WJR_REQUIRES(I >= 0 && I < Size)>
    constexpr std::tuple_element_t<I, tuple> &&
    operator[](integral_constant<size_t, I>) && noexcept {
        return static_cast<std::tuple_element_t<I, tuple> &&>(get<I>());
    }

    template <size_t I, WJR_REQUIRES(I >= 0 && I < Size)>
    constexpr const std::tuple_element_t<I, tuple> &&
    operator[](integral_constant<size_t, I>) const && noexcept {
        return static_cast<const std::tuple_element_t<I, tuple> &&>(get<I>());
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
constexpr tuple<unref_wrapper_t<Args>...> make_tuple(Args &&...args) noexcept(
    std::conjunction_v<
        std::is_nothrow_constructible<unref_wrapper_t<Args>, Args &&>...>) {
    return tuple<unref_wrapper_t<Args>...>(std::forward<Args>(args)...);
}

template <typename... Args>
constexpr tuple<Args &...> tie(Args &...args) noexcept {
    return tuple<Args &...>(args...);
}

template <typename... Args>
constexpr tuple<Args &&...> forward_as_tuple(Args &&...args) noexcept(
    std::conjunction_v<std::is_nothrow_constructible<Args &&, Args &&>...>) {
    return tuple<Args &&...>(std::forward<Args>(args)...);
}

/// @private
template <typename Func, typename Tuple, size_t... Indexs>
constexpr decltype(auto)
apply_impl(Func &&fn, Tuple &&tp, std::index_sequence<Indexs...>) noexcept(noexcept(
    std::invoke(std::forward<Func>(fn), std::get<Indexs>(std::forward<Tuple>(tp))...))) {
    return std::invoke(std::forward<Func>(fn),
                       std::get<Indexs>(std::forward<Tuple>(tp))...);
}

template <typename Func, typename Tuple>
constexpr decltype(auto) apply(Func &&fn, Tuple &&tp) noexcept(noexcept(
    apply_impl(std::forward<Func>(fn), std::forward<Tuple>(tp),
               std::make_index_sequence<std::tuple_size_v<remove_cvref_t<Tuple>>>{}))) {
    return apply_impl(
        std::forward<Func>(fn), std::forward<Tuple>(tp),
        std::make_index_sequence<std::tuple_size_v<remove_cvref_t<Tuple>>>{});
}

/// @private
template <size_t I, typename Tuple>
struct __tuple_cat_single_helper {
    static constexpr size_t Size = std::tuple_size_v<Tuple>;
    using type0 = tp_repeat_t<tp_list<std::integral_constant<size_t, I>>, Size>;
    using type1 = tp_make_index_sequence<Size>;
};

/// @private
template <typename S, typename... Tuples>
struct __tuple_cat_helper_impl;

/// @private
template <size_t... Indexs, typename... Tuples>
struct __tuple_cat_helper_impl<std::index_sequence<Indexs...>, Tuples...> {
    using type0 =
        tp_concat_t<typename __tuple_cat_single_helper<Indexs, Tuples>::type0...>;
    using type1 =
        tp_concat_t<typename __tuple_cat_single_helper<Indexs, Tuples>::type1...>;
};

/// @private
template <typename... Tuples>
struct __tuple_cat_helper {
    using Sequence = std::index_sequence_for<Tuples...>;
    using Impl = __tuple_cat_helper_impl<Sequence, Tuples...>;
    using type0 = tp_make_std_index_sequence<typename Impl::type0>;
    using type1 = tp_make_std_index_sequence<typename Impl::type1>;
};

/// @private
template <size_t... I0, size_t... I1, typename... Tuples>
constexpr decltype(auto) __tuple_cat_impl(std::index_sequence<I0...>,
                                          std::index_sequence<I1...>,
                                          tuple<Tuples...> &&tuples) {
    return tuple(std::get<I1>(std::get<I0>(std::move(tuples)))...);
}

template <typename... Tuples>
constexpr decltype(auto) tuple_cat(Tuples &&...tuples) {
    using Helper = __tuple_cat_helper<remove_cvref_t<Tuples>...>;
    return __tuple_cat_impl(typename Helper::type0{}, typename Helper::type1{},
                            wjr::forward_as_tuple(std::forward<Tuples>(tuples)...));
}

template <typename... TArgs, typename... UArgs>
constexpr bool
operator==(const tuple<TArgs...> &lhs, const tuple<UArgs...> &rhs) noexcept(
    std::conjunction_v<has_noexcept_equal_to<const TArgs &, const UArgs &>...>) {
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
constexpr bool operator!=(const tuple<TArgs...> &lhs,
                          const tuple<UArgs...> &rhs) noexcept(noexcept(lhs == rhs)) {
    return !(lhs == rhs);
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator<(const tuple<TArgs...> &lhs, const tuple<UArgs...> &rhs) noexcept(
    std::conjunction_v<
        std::conjunction<has_noexcept_less<const TArgs &, const UArgs &>,
                         has_noexcept_less<const UArgs &, const TArgs &>>...>) {
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
constexpr bool operator<=(const tuple<TArgs...> &lhs,
                          const tuple<UArgs...> &rhs) noexcept(noexcept(rhs < lhs)) {
    return !(rhs < lhs);
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator>(const tuple<TArgs...> &lhs,
                         const tuple<UArgs...> &rhs) noexcept(noexcept(rhs < lhs)) {
    return rhs < lhs;
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator>=(const tuple<TArgs...> &lhs,
                          const tuple<UArgs...> &rhs) noexcept(noexcept(lhs < rhs)) {
    return !(lhs < rhs);
}

template <size_t I, typename... Args>
struct __in_place_index_tuple_t_tag {};

template <size_t I, typename... Args>
using in_place_index_tuple_t =
    capture_leaf<tuple<Args...>, __in_place_index_tuple_t_tag<I, Args...>>;

template <size_t I, typename... Args>
constexpr in_place_index_tuple_t<I, Args &&...>
in_place_index_tuple(Args &&...args) noexcept(
    std::conjunction_v<std::is_nothrow_constructible<Args &&, Args &&>...>) {
    return in_place_index_tuple_t<I, Args &&...>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
struct __in_place_type_tuple_t_tag {};

template <typename T, typename... Args>
using in_place_type_tuple_t =
    capture_leaf<tuple<Args...>, __in_place_type_tuple_t_tag<T, Args...>>;

template <typename T, typename... Args>
constexpr in_place_type_tuple_t<T, Args &&...>
in_place_type_tuple(Args &&...args) noexcept(
    std::conjunction_v<std::is_nothrow_constructible<Args &&, Args &&>...>) {
    return in_place_type_tuple_t<T, Args &&...>(std::forward<Args>(args)...);
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
constexpr tuple_element_t<I, wjr::tuple<Args...>> &get(wjr::tuple<Args...> &t) noexcept {
    return t.template get<I>();
}

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &
get(const wjr::tuple<Args...> &t) noexcept {
    return t.template get<I>();
}

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &&
get(wjr::tuple<Args...> &&t) noexcept {
    return std::move(t).template get<I>();
}

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &&
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