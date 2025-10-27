/**
 * @file tuple.hpp
 * @brief Extended tuple implementation
 * @author wjr
 *
 * Provides an optimized tuple implementation with better compilation performance
 * and additional functionality beyond std::tuple.
 */

#ifndef WJR_TUPLE_HPP__
#define WJR_TUPLE_HPP__

#include <tuple>

#include <wjr/container/capture_leaf.hpp>
#include <wjr/math/integral_constant.hpp>

namespace wjr {

/**
 * @brief Extended tuple implementation
 *
 * A tuple implementation that provides better compilation performance and
 * additional features compared to std::tuple.
 *
 * @tparam Args Types of elements in the tuple
 */
template <typename... Args>
class tuple;

} // namespace wjr

namespace std {

template <typename... Args>
struct tuple_size<wjr::tuple<Args...>> : integral_constant<size_t, sizeof...(Args)> {};

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
constexpr tuple_element_t<I, wjr::tuple<Args...>> &get(const wjr::tuple<Args...> &t) noexcept;

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &&get(wjr::tuple<Args...> &&t) noexcept;

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &&get(const wjr::tuple<Args...> &&t) noexcept;

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
    : capture_leaf<Args, enable_base_identity_t<
                             Indexs, tuple_impl<std::index_sequence<Indexs...>, Args...>>>... {
    using Tuple = tuple<Args...>;

    template <size_t Idx>
    using Mybase =
        capture_leaf<std::tuple_element_t<Idx, Tuple>, enable_base_identity_t<Idx, tuple_impl>>;

    constexpr static size_t Size = sizeof...(Args);

public:
    template <typename S = void,
              WJR_REQUIRES(std::conjunction_v<std::is_same<S, void>,
                                              std::is_default_constructible<Args>...>)>
    constexpr tuple_impl() noexcept(std::conjunction_v<std::is_nothrow_constructible<Args>...>) {}

    template <typename... _Args,
              WJR_REQUIRES(std::conjunction_v<std::is_constructible<Mybase<Indexs>, _Args>...>)>
    constexpr tuple_impl(_Args &&...args) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<Args, _Args &&>...>)
        : Mybase<Indexs>(std::forward<_Args>(args))... {}

    template <typename TupleLike>
    constexpr tuple_impl(in_place_empty_t, TupleLike &&other) noexcept(
        std::is_nothrow_constructible_v<
            tuple_impl, decltype(std::get<Indexs>(std::forward<TupleLike>(other)))...>)
        : tuple_impl(std::get<Indexs>(std::forward<TupleLike>(other))...) {}

    template <size_t I>
    constexpr std::tuple_element_t<I, Tuple> &get() noexcept {
        return Mybase<I>::get();
    }

    template <size_t I>
    constexpr const std::tuple_element_t<I, Tuple> &get() const noexcept {
        return Mybase<I>::get();
    }
};

template <>
class tuple<> {
public:
    constexpr void swap(tuple &) noexcept {}
};

template <typename This, typename... Args>
class tuple<This, Args...> {
    using Sequence = std::index_sequence_for<This, Args...>;
    using Impl = tuple_impl<Sequence, This, Args...>;

    constexpr static size_t Size = sizeof...(Args) + 1;

    // MSVC always has strange bugs...
    template <typename Other, typename... _Args>
#if !WJR_HAS_MSVC(19, 37)
    using _is_all_convertible =
#else
    struct _is_all_convertible :
#endif
        std::conjunction<std::is_convertible<Other, This>, std::is_convertible<_Args, Args>...>
#if !WJR_HAS_MSVC(19, 37)
        ;
#else
    {
    };
#endif

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
              WJR_REQUIRES(
                  std::conjunction_v<std::is_default_constructible<T>,
                                     std::is_default_constructible<Args>...> &&
                  !std::conjunction_v<is_default_convertible<T>, is_default_convertible<Args>...>)>
    constexpr explicit tuple() noexcept(std::is_nothrow_constructible_v<Impl>) : m_impl() {}
#endif

#if defined(__cpp_conditional_explicit)
    template <typename Other = This,
              WJR_REQUIRES(std::is_constructible_v<Impl, const Other &, const Args &...>)>
    constexpr explicit(!_is_all_convertible<Other, Args...>::value)
        tuple(const Other &first, const Args &...rest) noexcept(
            std::is_nothrow_constructible_v<Impl, const Other &, const Args &...>)
        : m_impl(first, rest...) {}
#else
    template <typename Other = This,
              WJR_REQUIRES(std::is_constructible_v<Impl, const Other &, const Args &...>
                               &&_is_all_convertible<const Other &, const Args &...>::value)>
    constexpr tuple(const Other &first, const Args &...rest) noexcept(
        std::is_nothrow_constructible_v<Impl, const Other &, const Args &...>)
        : m_impl(first, rest...) {}

    template <typename Other = This,
              WJR_REQUIRES(std::is_constructible_v<Impl, const Other &, const Args &...> &&
                           !_is_all_convertible<const Other &, const Args &...>::value)>
    constexpr explicit tuple(const Other &first, const Args &...rest) noexcept(
        std::is_nothrow_constructible_v<Impl, const Other &, const Args &...>)
        : m_impl(first, rest...) {}
#endif

    template <
        typename Other, typename... _Args,
        WJR_REQUIRES(sizeof...(_Args) + 1 == Size &&
                     std::conjunction_v<std::negation<std::conjunction<
                                            std::is_same<This, std::remove_reference_t<Other>>,
                                            std::is_same<Args, std::remove_reference_t<_Args>>...>>,
                                        std::is_constructible<Impl, Other &&, _Args...>>)>
    constexpr tuple(Other &&other, _Args &&...args) noexcept(
        std::is_nothrow_constructible_v<Impl, Other &&, _Args...>)
        : m_impl(std::forward<Other>(other), std::forward<_Args>(args)...) {}

    template <typename TupleLike,
              WJR_REQUIRES(_is_tuple_test_v<std::is_constructible, tuple, TupleLike &&>)>
    constexpr tuple(TupleLike &&other) noexcept(
        std::is_nothrow_constructible_v<Impl, in_place_empty_t, TupleLike &&>)
        : m_impl(in_place_empty, std::forward<TupleLike>(other)) {}

private:
    template <size_t... _Indexs, typename Container>
    constexpr void _assign(std::index_sequence<_Indexs...>, Container &&other) noexcept(noexcept(
        ((this->template get<_Indexs>() = std::get<_Indexs>(std::forward<Container>(other))),
         ...))) {
        ((this->template get<_Indexs>() = std::get<_Indexs>(std::forward<Container>(other))), ...);
    }

public:
    template <typename TupleLike,
              WJR_REQUIRES(_is_tuple_test_v<_is_tuple_assignable, tuple, TupleLike &&>)>
    constexpr tuple &operator=(TupleLike &&other) noexcept(
        noexcept(_assign(Sequence(), std::forward<TupleLike>(other)))) {
        _assign(Sequence(), std::forward<TupleLike>(other));
        return *this;
    }

private:
    template <size_t... _Indexs>
    constexpr void _swap(std::index_sequence<_Indexs...>, tuple &other) noexcept(noexcept(
        ((std::swap(this->template get<_Indexs>(), other.template get<_Indexs>()), ...)))) {
        ((std::swap(this->template get<_Indexs>(), other.template get<_Indexs>()), ...));
    }

public:
    constexpr void swap(tuple &other) noexcept(noexcept(_swap(Sequence(), other))) {
        _swap(Sequence(), other);
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
        return static_cast<std::tuple_element_t<I, tuple> &&>(get<I>());
    }

    template <size_t I, WJR_REQUIRES(I < Size)>
    constexpr const std::tuple_element_t<I, tuple> &&get() const && noexcept {
        return static_cast<const std::tuple_element_t<I, tuple> &&>(get<I>());
    }

    template <typename Z, Z I>
    constexpr std::tuple_element_t<I, tuple> &operator[](integral_constant<Z, I>) & noexcept {
        return get<I>();
    }

    template <typename Z, Z I>
    constexpr const std::tuple_element_t<I, tuple> &
    operator[](integral_constant<Z, I>) const & noexcept {
        return get<I>();
    }

    template <typename Z, Z I>
    constexpr std::tuple_element_t<I, tuple> &&operator[](integral_constant<Z, I>) && noexcept {
        return std::move(*this).template get<I>();
    }

    template <typename Z, Z I>
    constexpr const std::tuple_element_t<I, tuple> &&
    operator[](integral_constant<Z, I>) const && noexcept {
        return std::move(*this).template get<I>();
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
struct get_relocate_mode<tuple<Args...>> {
    static constexpr relocate_t value = get_common_relocate_mode_v<Args...>;
};

template <typename... Args>
struct get_relocate_mode<std::tuple<Args...>> {
    static constexpr relocate_t value = get_common_relocate_mode_v<Args...>;
};

template <typename... Args>
constexpr tuple<unref_wrapper_t<Args>...> make_tuple(Args &&...args) noexcept(
    std::conjunction_v<std::is_nothrow_constructible<unref_wrapper_t<Args>, Args &&>...>) {
    return tuple<unref_wrapper_t<Args>...>(std::forward<Args>(args)...);
}

template <typename... Args>
constexpr tuple<Args &...> tie(Args &...args) noexcept {
    return tuple<Args &...>(args...);
}

template <typename... Args>
constexpr tuple<Args...> forward_as_tuple(Args &&...args) noexcept(
    std::conjunction_v<std::is_nothrow_constructible<Args &&, Args &&>...>) {
    return tuple<Args...>(std::forward<Args>(args)...);
}

/// @private
template <typename Func, typename Tuple, size_t... Indexs>
constexpr decltype(auto) apply_impl(Func &&fn, Tuple &&tp, std::index_sequence<Indexs...>) noexcept(
    noexcept(std::invoke(std::forward<Func>(fn), std::get<Indexs>(std::forward<Tuple>(tp))...))) {
    return std::invoke(std::forward<Func>(fn), std::get<Indexs>(std::forward<Tuple>(tp))...);
}

template <typename Func, typename Tuple>
constexpr decltype(auto) apply(Func &&fn, Tuple &&tp) noexcept(
    noexcept(apply_impl(std::forward<Func>(fn), std::forward<Tuple>(tp),
                        std::make_index_sequence<std::tuple_size_v<remove_cvref_t<Tuple>>>{}))) {
    return apply_impl(std::forward<Func>(fn), std::forward<Tuple>(tp),
                      std::make_index_sequence<std::tuple_size_v<remove_cvref_t<Tuple>>>{});
}

/// @private
template <size_t I, typename Tuple>
struct _tuple_cat_single_helper {
    static constexpr size_t Size = std::tuple_size_v<Tuple>;
    using type0 = tp_repeat_t<tp_list<integral_constant<size_t, I>>, Size>;
    using type1 = tp_make_indexs_list_t<Size>;
};

/// @private
template <typename S, typename... Tuples>
struct _tuple_cat_helper_impl;

/// @private
template <size_t... Indexs, typename... Tuples>
struct _tuple_cat_helper_impl<std::index_sequence<Indexs...>, Tuples...> {
    using type0 = tp_concat_t<typename _tuple_cat_single_helper<Indexs, Tuples>::type0...>;
    using type1 = tp_concat_t<typename _tuple_cat_single_helper<Indexs, Tuples>::type1...>;
};

/// @private
template <typename... Tuples>
struct _tuple_cat_helper {
    using Sequence = std::index_sequence_for<Tuples...>;
    using Impl = _tuple_cat_helper_impl<Sequence, Tuples...>;
    using type0 = typename Impl::type0;
    using type1 = typename Impl::type1;
};

/// @private
template <size_t... I0, size_t... I1, typename... Tuples>
constexpr decltype(auto) _tuple_cat_impl(tp_indexs_list_t<I0...>, tp_indexs_list_t<I1...>,
                                         tuple<Tuples...> &&tuples) {
    return tuple(std::get<I1>(std::get<I0>(std::move(tuples)))...);
}

template <typename... Tuples>
constexpr decltype(auto) tuple_cat(Tuples &&...tuples) {
    using Helper = _tuple_cat_helper<remove_cvref_t<Tuples>...>;
    return _tuple_cat_impl(typename Helper::type0{}, typename Helper::type1{},
                           wjr::forward_as_tuple(std::forward<Tuples>(tuples)...));
}

template <typename T, typename Tuple>
constexpr T make_from_tuple(Tuple &&tuple) {
    return apply([](auto &&...args) { return T(std::forward<decltype(args)>(args)...); },
                 std::forward<Tuple>(tuple));
}

template <typename... TArgs, typename... UArgs>
constexpr bool operator==(const tuple<TArgs...> &lhs, const tuple<UArgs...> &rhs) noexcept(
    std::conjunction_v<has_noexcept_equal_to<const TArgs &, const UArgs &>...>) {
    return apply(
        [&rhs](const auto &...lhs_args) {
            return apply(
                [&lhs_args...](const auto &...rhs_args) { return ((lhs_args == rhs_args) && ...); },
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
    std::conjunction_v<std::conjunction<has_noexcept_less<const TArgs &, const UArgs &>,
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

/// @private
struct _ignore {
    template <typename Ty>
    constexpr const _ignore &operator=(const Ty &) const noexcept {
        return *this;
    }
};

inline constexpr _ignore ignore{};

} // namespace wjr

namespace std {

template <typename... Args, WJR_REQUIRES_I(std::conjunction_v<wjr::is_swappable<Args>...>)>
constexpr void swap(wjr::tuple<Args...> &lhs,
                    wjr::tuple<Args...> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &get(wjr::tuple<Args...> &t) noexcept {
    return t.template get<I>();
}

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &get(const wjr::tuple<Args...> &t) noexcept {
    return t.template get<I>();
}

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &&get(wjr::tuple<Args...> &&t) noexcept {
    return std::move(t).template get<I>();
}

template <size_t I, typename... Args>
constexpr tuple_element_t<I, wjr::tuple<Args...>> &&get(const wjr::tuple<Args...> &&t) noexcept {
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