#ifndef WJR_TP_BASE_HPP__
#define WJR_TP_BASE_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename... Args>
struct tp_list {};

template <typename... Args>
constexpr tp_list<Args...> tp_make_list(Args...) {
    return {};
}

template <typename T>
struct tp_is_container : std::false_type {};

template <template <typename...> typename C, typename... Args>
struct tp_is_container<C<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool tp_is_container_v = tp_is_container<T>::value;

template <typename T>
struct tp_size;

template <template <typename...> typename C, typename... Args>
struct tp_size<C<Args...>> : integral_constant<size_t, sizeof...(Args)> {};

/// @brief get size of C<Args...>
template <typename T>
inline constexpr size_t tp_size_v = tp_size<T>::value;

template <typename T>
struct tp_is_fn : std::false_type {};

template <typename T>
inline constexpr bool tp_is_fn_v = tp_is_fn<T>::value;

/// @private
template <typename _Enable, template <typename...> typename F, typename... Args>
struct _tp_is_valid_helper : std::false_type {};

/// @private
template <template <typename...> typename F, typename... Args>
struct _tp_is_valid_helper<std::void_t<F<Args...>>, F, Args...> : std::true_type {};

template <template <typename...> typename F, typename... Args>
struct tp_is_valid : _tp_is_valid_helper<void, F, Args...> {};

template <template <typename...> typename F, typename... Args>
inline constexpr bool tp_is_valid_v = tp_is_valid<F, Args...>::value;

template <typename F, typename... Args>
inline constexpr bool tp_is_valid_f = tp_is_valid_v<F::template fn, Args...>;

/// @private
template <template <typename...> typename F, typename... Args>
struct _tp_defer_helper {
    using type = F<Args...>;
};

template <template <typename...> typename F, typename... Args>
struct tp_defer {
    using type =
        std::enable_if_t<tp_is_valid_v<F, Args...>, typename _tp_defer_helper<F, Args...>::type>;
};

/// @brief use std::enable_if_t to defer the instantiation of F<Args...>
template <template <typename...> typename F, typename... Args>
using tp_defer_t = typename tp_defer<F, Args...>::type;

template <typename F, typename... Args>
using tp_defer_f = tp_defer_t<F::template fn, Args...>;

/// @brief F1<F2<Args...>>
template <template <typename...> typename F1, template <typename...> typename F2>
struct tp_bind_fn {
    template <typename... Args>
    using fn = tp_defer_t<F1, tp_defer_t<F2, Args...>>;
};

/// @brief make F can be used as fn
template <template <typename...> typename F>
struct tp_make_fn {
    template <typename... Args>
    using fn = tp_defer_t<F, Args...>;
};

/// @brief std::negation<F<Args...>>
template <template <typename...> typename F>
struct tp_not_fn {
    template <typename... Args>
    using fn = typename tp_bind_fn<std::negation, F>::template fn<Args...>;
};

template <typename T>
struct tp_is_empty : std::bool_constant<tp_size_v<T> == 0> {};

template <typename T>
inline constexpr bool tp_is_empty_v = tp_is_empty<T>::value;

template <typename T, typename U>
struct tp_assign;

template <typename... Args1, template <typename...> typename T1, typename... Args2,
          template <typename...> typename T2>
struct tp_assign<T1<Args1...>, T2<Args2...>> {
    using type = T1<Args2...>;
};

/// @brief f(L1<Args1...>, L2<Args2...>) -> L1<Args2...>
template <typename T, typename U>
using tp_assign_t = typename tp_assign<T, U>::type;

template <typename T>
struct tp_clear;

template <template <typename...> typename T, typename... Args>
struct tp_clear<T<Args...>> {
    using type = T<>;
};

/// @brief f(L<Args...>) -> L<>
template <typename T>
using tp_clear_t = typename tp_clear<T>::type;

template <typename T, typename... Args>
struct tp_push_front;

template <template <typename...> typename C, typename... Args1, typename... Args2>
struct tp_push_front<C<Args1...>, Args2...> {
    using type = C<Args2..., Args1...>;
};

/// @brief f(L<Args1...>, Args2...) -> L<Args1..., Args2...)
template <typename T, typename... Args>
using tp_push_front_t = typename tp_push_front<T, Args...>::type;

template <typename T, typename... Args>
struct tp_push_back;

template <template <typename...> typename C, typename... Args1, typename... Args2>
struct tp_push_back<C<Args1...>, Args2...> {
    using type = C<Args1..., Args2...>;
};

/// @brief f(L<Args1...>, Args2...) -> L<Args2..., Args1...)
template <typename T, typename... Args>
using tp_push_back_t = typename tp_push_back<T, Args...>::type;

/// @private
template <typename _Enable, size_t I, size_t N, typename... Args>
struct _tp_cut_helper;

/// @private
template <size_t I, size_t N, typename T, typename... Args>
struct _tp_cut_helper<std::enable_if_t<N != 0, void>, I, N, T, Args...> {
    using type = typename _tp_cut_helper<void, I - 1, N, Args...>::type;
};

/// @private
template <size_t I, size_t N, typename T, typename... Args>
struct _tp_cut_helper<std::enable_if_t<N == 0, void>, I, N, T, Args...> {
    using type = tp_list<>;
};

/// @private
template <size_t N, typename... Args2>
struct _tp_cut_helper2;

/// @private
template <size_t N, typename T, typename... Args>
struct _tp_cut_helper2<N, T, Args...> {
    using type = tp_push_front_t<typename _tp_cut_helper2<N - 1, Args...>::type, T>;
};

/// @private
template <typename... Args>
struct _tp_cut_helper2<0, Args...> {
    using type = tp_list<>;
};

/// @private
template <typename T, typename... Args>
struct _tp_cut_helper2<0, T, Args...> {
    using type = tp_list<>;
};

/// @private
template <size_t N, typename... Args>
struct _tp_cut_helper<std::enable_if_t<N != 0>, 0, N, Args...> {
    using type = typename _tp_cut_helper2<N, Args...>::type;
};

/// @private
template <size_t N, typename T, typename... Args>
struct _tp_cut_helper<std::enable_if_t<N != 0>, 0, N, T, Args...> {
    using type = typename _tp_cut_helper2<N, T, Args...>::type;
};

template <typename T, template <typename...> typename U>
struct tp_rename;

template <template <typename...> typename C, typename... Args, template <typename...> typename U>
struct tp_rename<C<Args...>, U> {
    using type = U<Args...>;
};

/// @brief f(L1<Args1...>, L2) -> L2<Args1...>
template <typename T, template <typename...> typename U>
using tp_rename_t = typename tp_rename<T, U>::type;

template <typename T, size_t I, size_t N>
struct tp_cut;

template <template <typename...> typename C, typename... Args, size_t I, size_t N>
struct tp_cut<C<Args...>, I, N> {
    static_assert(N <= sizeof...(Args) && I <= (sizeof...(Args) - N), "tp_cut: invalid index");
    using type = tp_rename_t<typename _tp_cut_helper<void, I, N, Args...>::type, C>;
};

/// @brief f(L<Args...>, I, N) -> L<Args[I ~ I + N - 1]>
template <typename T, size_t I, size_t N>
using tp_cut_t = typename tp_cut<T, I, N>::type;

template <typename T>
struct tp_pop_front : tp_cut<T, 1, tp_size_v<T> - 1> {};

/// @brief f(L<T, Args...>) -> L<Args...>
template <typename T>
using tp_pop_front_t = typename tp_pop_front<T>::type;

template <typename T>
struct tp_pop_back : tp_cut<T, 0, tp_size_v<T> - 1> {};

/// @brief f(L<Args..., T>) -> L<Args...>
template <typename T>
using tp_pop_back_t = typename tp_pop_back<T>::type;

/// @private
template <size_t index, typename... Args>
struct _tp_at_helper;

/// @private
template <size_t index, typename T, typename... Args>
struct _tp_at_helper<index, T, Args...> {
    using type = typename _tp_at_helper<index - 1, Args...>::type;
};

/// @private
template <typename T, typename... Args>
struct _tp_at_helper<0, T, Args...> {
    using type = T;
};

//
template <typename T, size_t index>
struct tp_at;

template <template <typename... Args> typename C, typename... Args, size_t index>
struct tp_at<C<Args...>, index> {
    static_assert(index < sizeof...(Args), "tp_at: invalid index");
    using type = typename _tp_at_helper<index, Args...>::type;
};

/// @brief f(L<Args...>, index) - > Args(index)
template <typename T, size_t index>
using tp_at_t = typename tp_at<T, index>::type;

template <typename T>
struct tp_front {
    using type = tp_at_t<T, 0>;
};

/// @brief tp_at_t(T, 0)
template <typename T>
using tp_front_t = typename tp_front<T>::type;

template <typename T>
struct tp_back {
    using type = tp_at_t<T, tp_size_v<T> - 1>;
};

/// @brief tp_at_t(T, tp_size_v<T> - 1)
template <typename T>
using tp_back_t = typename tp_back<T>::type;

template <typename T, size_t idx>
struct tp_prefix {
    using type = tp_cut_t<T, 0, idx>;
};

/// @brief f(L<Args...>, idx) -> L<Args[0 ~ idx - 1]>
template <typename T, size_t idx>
using tp_prefix_t = typename tp_prefix<T, idx>::type;

template <typename T, size_t idx>
struct tp_suffix {
    using type = tp_cut_t<T, tp_size_v<T> - idx, idx>;
};

/// @brief f(L<Args...>, idx) -> L<Args[tp_size_v<T> - idx ~ tp_size_v<T> - 1]>
template <typename T, size_t idx>
using tp_suffix_t = typename tp_suffix<T, idx>::type;

template <typename T, size_t idx>
struct tp_remove_prefix {
    using type = tp_suffix_t<T, tp_size_v<T> - idx>;
};

template <typename T, size_t idx>
using tp_remove_prefix_t = typename tp_remove_prefix<T, idx>::type;

template <typename T, size_t idx>
struct tp_remove_suffix {
    using type = tp_prefix_t<T, tp_size_v<T> - idx>;
};

template <typename T, size_t idx>
using tp_remove_suffix_t = typename tp_remove_suffix<T, idx>::type;

template <typename... Args>
struct tp_concat;

template <typename T>
struct tp_concat<T> {
    using type = T;
};

template <template <typename...> typename C1, typename... Args1, template <typename...> typename C2,
          typename... Args2>
struct tp_concat<C1<Args1...>, C2<Args2...>> {
    using type = C1<Args1..., Args2...>;
};

template <typename T, typename U, typename... Args3>
struct tp_concat<T, U, Args3...> {
    using type = typename tp_concat<typename tp_concat<T, U>::type, Args3...>::type;
};

/// @brief f(L1<Args...>, L2<Args2...>, ... Ln<Argsn...>) -> L1<Args...,
/// Args2..., Argsn...>
template <typename... Args>
using tp_concat_t = typename tp_concat<Args...>::type;

template <typename T, size_t idx, typename U>
struct tp_replace_at {
    using type = tp_concat_t<tp_push_back_t<tp_cut_t<T, 0, idx>, U>,
                             tp_cut_t<T, idx + 1, tp_size_v<T> - idx - 1>>;
};

template <typename T, typename U>
struct tp_replace_at<T, 0, U> {
    using type = tp_push_front_t<tp_pop_front_t<T>, U>;
};

/// @brief f(L<Args...>, idx, U) -> L<Args[0 ~ idx - 1], U, Args[idx + 1 ~
/// tp_size_v<T> - 1]>
template <typename T, size_t idx, typename U>
using tp_replace_at_t = typename tp_replace_at<T, idx, U>::type;

template <typename T, typename U>
struct tp_replace_front_at {
    using type = tp_replace_at_t<T, 0, U>;
};

template <typename T, typename U>
using tp_replace_front_at_t = typename tp_replace_front_at<T, U>::type;

template <typename T, typename U>
struct tp_replace_back_at {
    using type = tp_replace_at_t<T, tp_size_v<T> - 1, U>;
};

template <typename T, typename U>
using tp_replace_back_at_t = typename tp_replace_back_at<T, U>::type;

template <typename V, typename T, typename... Args>
struct tp_conditional {
    using type = std::conditional_t<V::value, T, typename tp_conditional<Args...>::type>;
};

template <typename V, typename T1, typename T2>
struct tp_conditional<V, T1, T2> {
    using type = std::conditional_t<V::value, T1, T2>;
};

/// @brief f(V, T, Args...) -> std::conditional_t<V::value, T, f(Args...)>
template <typename V, typename T, typename... Args>
using tp_conditional_t = typename tp_conditional<V, T, Args...>::type;

template <size_t idx>
struct tp_arg;

template <template <typename...> typename F, typename... Args>
struct tp_bind;

template <template <typename...> typename F, typename... Args>
struct tp_bind_front;

template <template <typename...> typename F, typename... Args>
struct tp_bind_back;

template <size_t idx>
struct tp_is_fn<tp_arg<idx>> : std::true_type {};

template <template <typename...> typename F, typename... Args>
struct tp_is_fn<tp_bind<F, Args...>> : std::true_type {};

template <template <typename...> typename F, typename... Args>
struct tp_is_fn<tp_bind_front<F, Args...>> : std::true_type {};

template <template <typename...> typename F, typename... Args>
struct tp_is_fn<tp_bind_back<F, Args...>> : std::true_type {};

template <size_t idx>
struct tp_arg {
    template <typename... Args>
    using fn = tp_at_t<tp_list<Args...>, idx>;
};

template <template <typename...> typename F, typename T>
struct tp_apply {
    using type = tp_rename_t<T, F>;
};

/**
 * @brief f(F, L<Args...>) -> F<Args...>. same as tp_rename_t(L<Args...>, F)
 */
template <template <typename...> typename F, typename T>
using tp_apply_t = typename tp_apply<F, T>::type;

template <typename F, typename T>
using tp_apply_f = tp_apply_t<F::template fn, T>;

/// @private
template <typename _Enable, typename T, typename... Args>
struct _tp_bind_helper {
    using type = T;
};

/// @private
template <typename F, typename... Args>
struct _tp_bind_helper<std::enable_if_t<tp_is_fn_v<F>, void>, F, Args...> {
    using type = typename F::template fn<Args...>;
};

template <template <typename...> typename F, typename... Args>
struct tp_bind {
    template <typename... Args2>
    using fn = F<typename _tp_bind_helper<void, Args, Args2...>::type...>;
};

template <template <typename...> typename F, typename... Args>
struct tp_bind_front {
    template <typename... Args2>
    using fn = tp_defer_t<F, Args..., Args2...>;
};

template <template <typename...> typename F, typename... Args>
struct tp_bind_back {
    template <typename... Args2>
    using fn = tp_defer_t<F, Args2..., Args...>;
};

template <template <typename...> typename C, typename... Args>
struct tp_zip;

/// @private
template <template <typename...> typename C, typename T>
struct _tp_zip_helper;

/// @private
template <template <typename...> typename C, size_t... Idxs>
struct _tp_zip_helper<C, std::index_sequence<Idxs...>> {
    template <size_t I, typename... Args>
    using _type = C<tp_at_t<Args, I>...>;
    template <typename... Args>
    using type = tp_list<_type<Idxs, Args...>...>;
};

template <template <typename...> typename C>
struct tp_zip<C> {
    using type = tp_list<>;
};

template <template <typename...> typename C, typename T>
struct tp_zip<C, T> {
    using type =
        typename _tp_zip_helper<C, std::make_index_sequence<tp_size_v<T>>>::template type<T>;
};

template <template <typename...> typename C, typename T, typename... Args>
struct tp_zip<C, T, Args...> {
    constexpr static size_t size = tp_size_v<T>;
    static_assert(((size == tp_size_v<Args>) && ...),
                  "tp_zip arguments must have same size, \
		you can make all arguments have same size by tp_");
    using type =
        typename _tp_zip_helper<C, std::make_index_sequence<tp_size_v<T>>>::template type<T,
                                                                                          Args...>;
};

/**
 * @brief f(C, L<A1, A2, ... An>, L<B1, B2, ..., Bn> ...) \
 * -> L<C<A1, B1, ...>, C<A2, B2, ...>, ..., C<An, Bn, ...>>
 */
template <template <typename...> typename C, typename... Args>
using tp_zip_t = typename tp_zip<C, Args...>::type;

/// @private
template <typename... Args>
struct _tp_max_size_helper;

/// @private
template <typename T>
struct _tp_max_size_helper<T> {
    constexpr static size_t value = tp_size_v<T>;
};

/// @private
template <typename T, typename... Args>
struct _tp_max_size_helper<T, Args...> {
    constexpr static size_t value = std::max(tp_size_v<T>, _tp_max_size_helper<Args...>::value);
};

template <typename T, typename... Args>
struct tp_max_size {
    constexpr static size_t value = _tp_max_size_helper<T, Args...>::value;
};

/// @brief tp_max_size_v<T, Args...> -> size_t
template <typename T, typename... Args>
inline constexpr size_t tp_max_size_v = tp_max_size<T, Args...>::value;

template <typename T>
struct tp_unwrap {
    static_assert(tp_size_v<T> == 1, "only container that size = 1 can use unwrap");
};

template <template <typename...> typename C, typename T>
struct tp_unwrap<C<T>> {
    using type = T;
};

/// @brief f(C<T>) -> T
template <typename T>
using tp_unwrap_t = typename tp_unwrap<T>::type;

template <typename T, size_t N>
struct tp_repeat {
    using type = tp_concat_t<T, typename tp_repeat<T, N - 1>::type>;
};

template <typename T>
struct tp_repeat<T, 0> {
    using type = tp_clear_t<T>;
};

template <typename C, size_t N>
using tp_repeat_t = typename tp_repeat<C, N>::type;

/// @private
template <typename _Enable, typename C, size_t N, typename V>
struct _tp_resize_helper {
    using type = tp_cut_t<C, 0, N>;
};

/// @private
template <typename C, size_t N, typename V>
struct _tp_resize_helper<std::enable_if_t<N >= tp_size_v<C>, void>, C, N, V> {
    using type = tp_concat_t<C, tp_repeat_t<V, N - tp_size_v<C>>>;
};

template <typename C, size_t N, typename V>
struct tp_resize {
    using tyep = typename _tp_resize_helper<void, C, N, V>::type;
};

template <typename C, size_t N, typename V>
using tp_resize_t = typename tp_resize<C, N, V>::type;

template <typename C, size_t I, typename... Args>
struct tp_insert {
    static_assert(I <= tp_size_v<C>, "tp insert index out of range");
    using type =
        tp_concat_t<tp_push_back_t<tp_prefix_t<C, I>, Args...>, tp_suffix_t<C, tp_size_v<C> - I>>;
};

template <typename C, size_t I, typename... Args>
using tp_insert_t = typename tp_insert<C, I, Args...>::type;

template <typename C, size_t I, size_t N>
struct tp_erase {
    static_assert(N <= tp_size_v<C> && I <= tp_size_v<C> - N, "tp erase index out of range");
    using type = tp_concat_t<tp_prefix_t<C, I>, tp_suffix_t<C, tp_size_v<C> - I - N>>;
};

template <typename C, size_t I, size_t N>
using tp_erase_t = typename tp_erase<C, I, N>::type;

template <typename C, typename E, template <typename...> typename F>
struct tp_left_fold;

template <template <typename...> typename C, typename E, template <typename...> typename F>
struct tp_left_fold<C<>, E, F> {
    using type = E;
};

template <template <typename...> typename C, typename T, typename... Args, typename E,
          template <typename...> typename F>
struct tp_left_fold<C<T, Args...>, E, F> {
    using type = typename tp_left_fold<C<Args...>, F<E, T>, F>::type;
};

/// @brief f(L<A1, A2, ... An>, E, F) -> F<F<F...<F<E, A1>, A2>, ...>, An>
template <typename C, typename E, template <typename...> typename F>
using tp_left_fold_t = typename tp_left_fold<C, E, F>::type;

template <typename C, typename E, typename F>
using tp_left_fold_f = typename tp_left_fold<C, E, F::template fn>::type;

template <typename C, typename E, template <typename...> typename F>
struct tp_right_fold;

template <template <typename...> typename C, typename E, template <typename...> typename F>
struct tp_right_fold<C<>, E, F> {
    using type = E;
};

template <template <typename...> typename C, typename T, typename... Args, typename E,
          template <typename...> typename F>
struct tp_right_fold<C<T, Args...>, E, F> {
    using next_type = typename tp_right_fold<C<Args...>, E, F>::type;
    using type = F<T, next_type>;
};

/// @brief f(L<A1, A2, ... An>, E, F) -> F<A1, F<A2, ... F<An, E>...>>
template <typename C, typename E, template <typename...> typename F>
using tp_right_fold_t = typename tp_right_fold<C, E, F>::type;

template <typename C, typename E, typename F>
using tp_right_fold_f = typename tp_right_fold<C, E, F::template fn>::type;

template <typename Type, Type Min, size_t Count>
struct _tp_iota_helper;

template <typename Type, Type Min, size_t Count>
struct _tp_iota_helper {
    using type = tp_push_front_t<typename _tp_iota_helper<Type, Min + 1, Count - 1>::type,
                                 integral_constant<Type, Min>>;
};

template <typename Type, Type Min>
struct _tp_iota_helper<Type, Min, 0> {
    using type = tp_list<>;
};

template <typename Type, Type Min, size_t Count>
struct tp_iota {
    using type = typename _tp_iota_helper<Type, Min, Count>::type;
};

template <typename Type, Type Min, size_t Count>
using tp_iota_t = typename tp_iota<Type, Min, Count>::type;

template <typename Type, Type... Vals>
using tp_integers_list_t = tp_list<integral_constant<Type, Vals>...>;

template <size_t... Vals>
using tp_indexs_list_t = tp_integers_list_t<size_t, Vals...>;

template <typename T, size_t N>
using tp_make_integers_list_t = tp_iota_t<T, 0, N>;

template <size_t N>
using tp_make_indexs_list_t = tp_make_integers_list_t<size_t, N>;

template <typename... Args>
using tp_index_sequence_for = tp_make_indexs_list_t<sizeof...(Args)>;

} // namespace wjr

#endif // WJR_TP_BASE_HPP__