#ifndef WJR_TP_LIST_HPP__
#define WJR_TP_LIST_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename... Args>
struct tp_list {};

template <typename T>
struct tp_is_list : std::false_type {};

template <typename... Args>
struct tp_is_list<tp_list<Args...>> : std::true_type {};

// check if is tp_list
template <typename T>
inline constexpr bool tp_is_list_v = tp_is_list<T>::value;

template <typename T>
struct tp_is_container : std::false_type {};

template <template <typename...> typename C, typename... Args>
struct tp_is_container<C<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool tp_is_container_v = tp_is_container<T>::value;

template <typename T>
struct tp_size;

template <template <typename...> typename C, typename... Args>
struct tp_size<C<Args...>> : std::integral_constant<size_t, sizeof...(Args)> {};

// get size of C<Args...>
template <typename T>
inline constexpr size_t tp_size_v = tp_size<T>::value;

template <typename T>
struct tp_is_fn : std::false_type {};

template <typename T>
inline constexpr bool tp_is_fn_v = tp_is_fn<T>::value;

/// @private
template <typename _Enable, template <typename...> typename F, typename... Args>
struct __tp_is_valid_helper : std::false_type {};

/// @private
template <template <typename...> typename F, typename... Args>
struct __tp_is_valid_helper<std::void_t<F<Args...>>, F, Args...> : std::true_type {};

template <template <typename...> typename F, typename... Args>
struct tp_is_valid : __tp_is_valid_helper<void, F, Args...> {};

template <template <typename...> typename F, typename... Args>
inline constexpr bool tp_is_valid_v = tp_is_valid<F, Args...>::value;

template <typename F, typename... Args>
inline constexpr bool tp_is_valid_f = tp_is_valid_v<F::template fn, Args...>;

/// @private
template <template <typename...> typename F, typename... Args>
struct __tp_defer_helper {
    using type = F<Args...>;
};

template <template <typename...> typename F, typename... Args>
struct tp_defer {
    using type = std::enable_if_t<tp_is_valid_v<F, Args...>,
                                  typename __tp_defer_helper<F, Args...>::type>;
};

// use std::enable_if_t to defer the instantiation of F<Args...>
template <template <typename...> typename F, typename... Args>
using tp_defer_t = typename tp_defer<F, Args...>::type;

template <typename F, typename... Args>
using tp_defer_f = tp_defer_t<F::template fn, Args...>;

template <typename T>
struct tp_type_identity {
    using type = T;
};

// tp_type_identity_t<T> is T
template <typename T>
using tp_type_identity_t = typename tp_type_identity<T>::type;

// F1<F2<Args...>>
template <template <typename...> typename F1, template <typename...> typename F2>
struct tp_bind_fn {
    template <typename... Args>
    using fn = tp_defer_t<F1, tp_defer_t<F2, Args...>>;
};

// make F can be used as fn
template <template <typename...> typename F>
struct tp_make_fn {
    template <typename... Args>
    using fn = tp_defer_t<F, Args...>;
};

// std::negation<F<Args...>>
template <template <typename...> typename F>
struct tp_not_fn {
    template <typename... Args>
    using fn = typename tp_bind_fn<std::negation, F>::template fn<Args...>;
};

template <typename... Args>
using tp_true_type = std::true_type;

template <typename... Args>
using tp_false_type = std::false_type;

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

// f(L1<Args1...>, L2<Args2...>) -> L1<Args2...>
template <typename T, typename U>
using tp_assign_t = typename tp_assign<T, U>::type;

template <typename T>
struct tp_clear;

template <template <typename...> typename T, typename... Args>
struct tp_clear<T<Args...>> {
    using type = T<>;
};

// f(L<Args...>) -> L<>
template <typename T>
using tp_clear_t = typename tp_clear<T>::type;

template <typename T, typename... Args>
struct tp_push_front;

template <template <typename...> typename C, typename... Args1, typename... Args2>
struct tp_push_front<C<Args1...>, Args2...> {
    using type = C<Args2..., Args1...>;
};

// f(L<Args1...>, Args2...) -> L<Args1..., Args2...)
template <typename T, typename... Args>
using tp_push_front_t = typename tp_push_front<T, Args...>::type;

template <typename T, typename... Args>
struct tp_push_back;

template <template <typename...> typename C, typename... Args1, typename... Args2>
struct tp_push_back<C<Args1...>, Args2...> {
    using type = C<Args1..., Args2...>;
};

// f(L<Args1...>, Args2...) -> L<Args2..., Args1...)
template <typename T, typename... Args>
using tp_push_back_t = typename tp_push_back<T, Args...>::type;

/// @private
template <typename _Enable, size_t I, size_t N, typename... Args>
struct __tp_cut_helper;

/// @private
template <size_t I, size_t N, typename T, typename... Args>
struct __tp_cut_helper<std::enable_if_t<N != 0, void>, I, N, T, Args...> {
    using type = typename __tp_cut_helper<void, I - 1, N, Args...>::type;
};

/// @private
template <size_t I, size_t N, typename T, typename... Args>
struct __tp_cut_helper<std::enable_if_t<N == 0, void>, I, N, T, Args...> {
    using type = tp_list<>;
};

/// @private
template <size_t N, typename... Args2>
struct __tp_cut_helper2;

/// @private
template <size_t N, typename T, typename... Args>
struct __tp_cut_helper2<N, T, Args...> {
    using type = tp_push_front_t<typename __tp_cut_helper2<N - 1, Args...>::type, T>;
};

/// @private
template <typename... Args>
struct __tp_cut_helper2<0, Args...> {
    using type = tp_list<>;
};

/// @private
template <typename T, typename... Args>
struct __tp_cut_helper2<0, T, Args...> {
    using type = tp_list<>;
};

/// @private
template <size_t N, typename... Args>
struct __tp_cut_helper<std::enable_if_t<N != 0>, 0, N, Args...> {
    using type = typename __tp_cut_helper2<N, Args...>::type;
};

/// @private
template <size_t N, typename T, typename... Args>
struct __tp_cut_helper<std::enable_if_t<N != 0>, 0, N, T, Args...> {
    using type = typename __tp_cut_helper2<N, T, Args...>::type;
};

template <typename T, template <typename...> typename U>
struct tp_rename;

template <template <typename...> typename C, typename... Args,
          template <typename...> typename U>
struct tp_rename<C<Args...>, U> {
    using type = U<Args...>;
};

// f(L1<Args1...>, L2) -> L2<Args1...>
template <typename T, template <typename...> typename U>
using tp_rename_t = typename tp_rename<T, U>::type;

template <typename T, size_t I, size_t N>
struct tp_cut;

template <template <typename...> typename C, typename... Args, size_t I, size_t N>
struct tp_cut<C<Args...>, I, N> {
    static_assert(N <= sizeof...(Args) && I <= (sizeof...(Args) - N),
                  "tp_cut: invalid index");
    using type = tp_rename_t<typename __tp_cut_helper<void, I, N, Args...>::type, C>;
};

// f(L<Args...>, I, N) -> L<Args[I ~ I + N - 1]>
template <typename T, size_t I, size_t N>
using tp_cut_t = typename tp_cut<T, I, N>::type;

template <typename T>
struct tp_pop_front : tp_cut<T, 1, tp_size_v<T> - 1> {};

// f(L<T, Args...>) -> L<Args...>
template <typename T>
using tp_pop_front_t = typename tp_pop_front<T>::type;

template <typename T>
struct tp_pop_back : tp_cut<T, 0, tp_size_v<T> - 1> {};

// f(L<Args..., T>) -> L<Args...>
template <typename T>
using tp_pop_back_t = typename tp_pop_back<T>::type;

/// @private
template <size_t index, typename... Args>
struct __tp_at_helper;

/// @private
template <size_t index, typename T, typename... Args>
struct __tp_at_helper<index, T, Args...> {
    using type = typename __tp_at_helper<index - 1, Args...>::type;
};

/// @private
template <typename T, typename... Args>
struct __tp_at_helper<0, T, Args...> {
    using type = T;
};

//
template <typename T, size_t index>
struct tp_at;

template <template <typename... Args> typename C, typename... Args, size_t index>
struct tp_at<C<Args...>, index> {
    static_assert(index < sizeof...(Args), "tp_at: invalid index");
    using type = typename __tp_at_helper<index, Args...>::type;
};

// f(L<Args...>, index) - > Args(index)
template <typename T, size_t index>
using tp_at_t = typename tp_at<T, index>::type;

template <typename T>
struct tp_front {
    using type = tp_at_t<T, 0>;
};

// tp_at_t(T, 0)
template <typename T>
using tp_front_t = typename tp_front<T>::type;

template <typename T>
struct tp_back {
    using type = tp_at_t<T, tp_size_v<T> - 1>;
};

// tp_at_t(T, tp_size_v<T> - 1)
template <typename T>
using tp_back_t = typename tp_back<T>::type;

template <typename T, size_t idx>
struct tp_prefix {
    using type = tp_cut_t<T, 0, idx>;
};

// f(L<Args...>, idx) -> L<Args[0 ~ idx - 1]>
template <typename T, size_t idx>
using tp_prefix_t = typename tp_prefix<T, idx>::type;

template <typename T, size_t idx>
struct tp_suffix {
    using type = tp_cut_t<T, tp_size_v<T> - idx, idx>;
};

// f(L<Args...>, idx) -> L<Args[tp_size_v<T> - idx ~ tp_size_v<T> - 1]>
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

template <template <typename...> typename C1, typename... Args1,
          template <typename...> typename C2, typename... Args2>
struct tp_concat<C1<Args1...>, C2<Args2...>> {
    using type = C1<Args1..., Args2...>;
};

template <typename T, typename U, typename... Args3>
struct tp_concat<T, U, Args3...> {
    using type = typename tp_concat<typename tp_concat<T, U>::type, Args3...>::type;
};

// f(L1<Args...>, L2<Args2...>, ... Ln<Argsn...>) -> L1<Args..., Args2..., Argsn...>
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

// f(L<Args...>, idx, U) -> L<Args[0 ~ idx - 1], U, Args[idx + 1 ~ tp_size_v<T> - 1]>
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

// f(V, T, U) -> std::conditional_t<V::value, T, U>
// f(V, T, Args...) -> std::conditional_t<V::value, T, f(Args...)>
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

// f(F, L<Args...>) -> F<Args...>
// same as tp_rename_t(L<Args...>, F)
template <template <typename...> typename F, typename T>
using tp_apply_t = typename tp_apply<F, T>::type;

template <typename F, typename T>
using tp_apply_f = tp_apply_t<F::template fn, T>;

/// @private
template <typename _Enable, typename T, typename... Args>
struct __tp_bind_helper {
    using type = T;
};

/// @private
template <typename F, typename... Args>
struct __tp_bind_helper<std::enable_if_t<tp_is_fn_v<F>, void>, F, Args...> {
    using type = typename F::template fn<Args...>;
};

template <template <typename...> typename F, typename... Args>
struct tp_bind {
    template <typename... Args2>
    using fn = F<typename __tp_bind_helper<void, Args, Args2...>::type...>;
};

template <typename F, typename... Args>
using tp_bind_f = tp_bind<F::template fn, Args...>;

template <template <typename...> typename F, typename... Args>
struct tp_bind_front {
    template <typename... Args2>
    using fn = tp_defer_t<F, Args..., Args2...>;
};

template <typename F, typename... Args>
using tp_bind_front_f = tp_bind_front<F::template fn, Args...>;

template <template <typename...> typename F, typename... Args>
struct tp_bind_back {
    template <typename... Args2>
    using fn = tp_defer_t<F, Args2..., Args...>;
};

template <typename F, typename... Args>
using tp_bind_back_f = tp_bind_back<F::template fn, Args...>;

template <typename T, template <typename...> typename F>
struct tp_transform;

template <template <typename...> typename C, typename... Args,
          template <typename...> typename F>
struct tp_transform<C<Args...>, F> {
    using type = C<F<Args>...>;
};

// f(L<Args...>, Fn) -> L<Fn(Args)...>
// use with apply, bind, bind_front, bind_back...
// for example:
// tp_transform_f<tp_bind_front<tp_apply_f, tp_bind_front<std::is_same>>,
// tp_list<tp_list<int, float>, tp_list<float, float>, tp_list<int, double>>>
// -> tp_list<std::is_same<int, float>, std::is_same<float, float>, std::is_same<int,
// double>>
template <typename T, template <typename...> typename F>
using tp_transform_t = typename tp_transform<T, F>::type;

template <typename T, typename F>
using tp_transform_f = typename tp_transform<T, F::template fn>::type;

template <template <typename...> typename C, typename... Args>
struct tp_zip;

/// @private
template <template <typename...> typename C, typename T>
struct __tp_zip_helper;

/// @private
template <template <typename...> typename C, size_t... Idxs>
struct __tp_zip_helper<C, std::index_sequence<Idxs...>> {
    template <size_t I, typename... Args>
    using __type = C<tp_at_t<Args, I>...>;
    template <typename... Args>
    using type = tp_list<__type<Idxs, Args...>...>;
};

template <template <typename...> typename C>
struct tp_zip<C> {
    using type = tp_list<>;
};

template <template <typename...> typename C, typename T>
struct tp_zip<C, T> {
    using type = typename __tp_zip_helper<
        C, std::make_index_sequence<tp_size_v<T>>>::template type<T>;
};

template <template <typename...> typename C, typename T, typename... Args>
struct tp_zip<C, T, Args...> {
    constexpr static size_t size = tp_size_v<T>;
    static_assert(((size == tp_size_v<Args>)&&...),
                  "tp_zip arguments must have same size, \
		you can make all arguments have same size by tp_");
    using type = typename __tp_zip_helper<
        C, std::make_index_sequence<tp_size_v<T>>>::template type<T, Args...>;
};

// f(C, L<A1, A2, ... An>, L<B1, B2, ..., Bn> ...)
// -> L<C<A1, B1, ...>, C<A2, B2, ...>, ..., C<An, Bn, ...>>
template <template <typename...> typename C, typename... Args>
using tp_zip_t = typename tp_zip<C, Args...>::type;

/// @private
template <typename... Args>
struct __tp_max_size_helper;

/// @private
template <typename T>
struct __tp_max_size_helper<T> {
    constexpr static size_t value = tp_size_v<T>;
};

/// @private
template <typename T, typename... Args>
struct __tp_max_size_helper<T, Args...> {
    constexpr static size_t value =
        std::max(tp_size_v<T>, __tp_max_size_helper<Args...>::value);
};

template <typename T, typename... Args>
struct tp_max_size {
    constexpr static size_t value = __tp_max_size_helper<T, Args...>::value;
};

// tp_max_size_v<T, Args...> -> size_t
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

// f(C<T>) -> T
template <typename T>
using tp_unwrap_t = typename tp_unwrap<T>::type;

template <typename T, template <typename...> typename P, typename U>
struct tp_replace_if;

template <template <typename...> typename C, typename... Args,
          template <typename...> typename P, typename U>
struct tp_replace_if<C<Args...>, P, U> {
    using type = C<tp_conditional_t<P<Args>, U, Args>...>;
};

// f(L<Args...>, P, U) -> L<if P(Args)::value then U else Args...>
template <typename T, template <typename...> typename P, typename U>
using tp_replace_if_t = typename tp_replace_if<T, P, U>::type;

template <typename T, typename P, typename U>
using tp_replace_if_f = tp_replace_if_t<T, P::template fn, U>;

template <typename T, typename U>
struct tp_replace_if_true {
    using type = tp_replace_if_t<T, tp_type_identity_t, U>;
};

template <typename T, typename U>
using tp_replace_if_true_t = typename tp_replace_if_true<T, U>::type;

template <typename T, typename U>
struct tp_replace_if_false {
    using type = tp_replace_if_f<T, tp_not_fn<tp_type_identity_t>, U>;
};

template <typename T, typename U>
using tp_replace_if_false_t = typename tp_replace_if_false<T, U>::type;

template <typename T, typename O, typename N>
struct tp_replace {
    using type = tp_replace_if_f<T, tp_bind_front<std::is_same, O>, N>;
};

template <typename T, typename O, typename N>
using tp_replace_t = typename tp_replace<T, O, N>::type;

template <typename T, typename U>
struct tp_fill {
    using type = tp_replace_if_t<T, tp_true_type, U>;
};

// f(L<Args...>, U) -> L<U, U, ..., U>
template <typename T, typename U>
using tp_fill_t = typename tp_fill<T, U>::type;

template <typename T, template <typename...> typename P>
struct tp_count_if;

template <template <typename...> typename C, template <typename...> typename P>
struct tp_count_if<C<>, P> {
    static constexpr size_t value = 0;
};

template <template <typename...> typename C, typename... Args,
          template <typename...> typename P>
struct tp_count_if<C<Args...>, P> {
    static constexpr size_t value = (P<Args>::value + ...);
};

// f(L<Args...>, P) -> count(P(Args)::value)
template <typename T, template <typename...> typename P>
constexpr size_t tp_count_if_v = tp_count_if<T, P>::value;

template <typename T, typename P>
constexpr size_t tp_count_if_f_v = tp_count_if_v<T, P::template fn>;

template <typename T, typename V>
struct tp_count {
    static constexpr size_t value = tp_count_if_f_v<T, tp_bind_front<std::is_same, V>>;
};

template <typename T, typename V>
constexpr size_t tp_count_v = tp_count<T, V>::value;

template <typename T, typename V>
struct tp_contains {
    static constexpr bool value = tp_count_v<T, V> != 0;
};

template <typename T, typename V>
constexpr bool tp_contains_v = tp_contains<T, V>::value;

template <typename T, template <typename...> typename P>
struct tp_remove_if;

template <template <typename...> typename C, typename... Args,
          template <typename...> typename P>
struct tp_remove_if<C<Args...>, P> {
    using type = tp_concat_t<C<>, tp_conditional_t<P<Args>, C<>, C<Args>>...>;
};

// f(L<Args...>, P) -> L<if P(Args)::value then L<> else L<Args>...>
template <typename T, template <typename...> typename P>
using tp_remove_if_t = typename tp_remove_if<T, P>::type;

template <typename T, typename P>
using tp_remove_if_f = tp_remove_if_t<T, P::template fn>;

template <typename T, typename V>
struct tp_remove {
    using type = tp_remove_if_f<T, tp_bind_front<std::is_same, V>>;
};

template <typename T, typename V>
using tp_remove_t = typename tp_remove<T, V>::type;

template <typename T, template <typename...> typename P>
struct tp_filter {
    using type = tp_remove_if_f<T, tp_not_fn<P>>;
};

template <typename T, template <typename...> typename P>
using tp_filter_t = typename tp_filter<T, P>::type;

template <typename T, typename P>
using tp_filter_f = tp_filter_t<T, P::template fn>;

template <typename T, typename U>
struct tp_equal;

/// @private
template <typename _Enable, typename T, typename U>
struct __tp_equal_helper : std::false_type {};

/// @private
template <template <typename...> typename C, typename... Args,
          template <typename...> typename D, typename... Args2>
struct __tp_equal_helper<std::enable_if_t<sizeof...(Args) == sizeof...(Args2), void>,
                         C<Args...>, D<Args2...>>
    : std::conjunction<std::is_same<Args, Args2>...> {};

template <typename T, typename U>
struct tp_equal : __tp_equal_helper<void, T, U> {};

template <typename T, typename U>
inline constexpr bool tp_equal_v = tp_equal<T, U>::value;

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
struct __tp_resize_helper {
    using type = tp_cut_t<C, 0, N>;
};

/// @private
template <typename C, size_t N, typename V>
struct __tp_resize_helper<std::enable_if_t<N >= tp_size_v<C>, void>, C, N, V> {
    using type = tp_concat_t<C, tp_repeat_t<V, N - tp_size_v<C>>>;
};

template <typename C, size_t N, typename V>
struct tp_resize {
    using tyep = typename __tp_resize_helper<void, C, N, V>::type;
};

template <typename C, size_t N, typename V>
using tp_resize_t = typename tp_resize<C, N, V>::type;

template <template <typename...> typename C, typename... Args>
struct tp_product;

/// @private
template <typename _Enable, template <typename...> typename C, typename... Args>
struct __tp_product_helper {
    using type = tp_list<>;
};

/// @private
template <typename _Enable, template <typename...> typename C, typename T>
struct __tp_product_helper<_Enable, C, T> {
    using type = tp_list<tp_rename_t<T, C>>;
};

/// @private
template <template <typename...> typename C, typename T,
          template <typename...> typename C1, typename... Args1, typename... Args>
struct __tp_product_helper<std::enable_if_t<sizeof...(Args1) != 0, void>, C, T,
                           C1<Args1...>, Args...> {
    using type =
        tp_concat_t<typename __tp_product_helper<void, C, tp_push_back_t<T, Args1>,
                                                 Args...>::type...>;
};

template <template <typename...> typename C, typename... Args>
struct tp_product {
    using type = typename __tp_product_helper<void, C, tp_list<>, Args...>::type;
};

// for example
// f(C, L<A1, A2>, L<B1, B2, B3>) -> L<C<A1, B1>, C<A1, B2>, C<A1, B3>, C<A2, B1>, C<A2,
// B2>, C<A2, B3>>
template <template <typename...> typename C, typename... Args>
using tp_product_t = typename tp_product<C, Args...>::type;

template <typename C, size_t I, typename... Args>
struct tp_insert {
    static_assert(I <= tp_size_v<C>, "tp insert index out of range");
    using type = tp_concat_t<tp_push_back_t<tp_prefix_t<C, I>, Args...>,
                             tp_suffix_t<C, tp_size_v<C> - I>>;
};

template <typename C, size_t I, typename... Args>
using tp_insert_t = typename tp_insert<C, I, Args...>::type;

template <typename C, size_t I, size_t N>
struct tp_erase {
    static_assert(N <= tp_size_v<C> && I <= tp_size_v<C> - N,
                  "tp erase index out of range");
    using type = tp_concat_t<tp_prefix_t<C, I>, tp_suffix_t<C, tp_size_v<C> - I - N>>;
};

template <typename C, size_t I, size_t N>
using tp_erase_t = typename tp_erase<C, I, N>::type;

template <typename C>
struct tp_reverse;

template <template <typename...> typename C>
struct tp_reverse<C<>> {
    using type = C<>;
};

template <template <typename...> typename C, typename T, typename... Args>
struct tp_reverse<C<T, Args...>> {
    using type = tp_push_back_t<typename tp_reverse<C<Args...>>::type, T>;
};

template <typename C>
using tp_reverse_t = typename tp_reverse<C>::type;

/// @private
template <typename _Enable, size_t idx, typename C, template <typename...> typename P>
struct __tp_find_if_helper;

/// @private
template <typename _Enable, size_t idx, template <typename...> typename C, typename T,
          typename... Args, template <typename...> typename P>
struct __tp_find_if_helper<_Enable, idx, C<T, Args...>, P> {
    constexpr static size_t value =
        __tp_find_if_helper<void, idx + 1, C<Args...>, P>::value;
};

/// @private
template <typename _Enable, size_t idx, template <typename...> typename C,
          template <typename...> typename P>
struct __tp_find_if_helper<_Enable, idx, C<>, P> {
    constexpr static size_t value = -1;
};

/// @private
template <size_t idx, template <typename...> typename C, typename T, typename... Args,
          template <typename...> typename P>
struct __tp_find_if_helper<std::enable_if_t<P<T>::value, void>, idx, C<T, Args...>, P> {
    constexpr static size_t value = idx;
};

template <typename C, template <typename...> typename P>
struct tp_find_if {
    constexpr static size_t value = __tp_find_if_helper<void, 0, C, P>::value;
};

template <typename C, template <typename...> typename P>
inline constexpr size_t tp_find_if_v = tp_find_if<C, P>::value;

template <typename C, typename P>
inline constexpr size_t tp_find_if_f = tp_find_if<C, P::template fn>::value;

template <typename C, template <typename...> typename P>
struct tp_find_if_not {
    constexpr static size_t value = tp_find_if_f<C, tp_not_fn<P>>;
};

template <typename C, template <typename...> typename P>
inline constexpr size_t tp_find_if_not_v = tp_find_if_not<C, P>::value;

template <typename C, typename P>
inline constexpr size_t tp_find_if_not_f = tp_find_if_not<C, P::template fn>::value;

template <typename C, typename V>
struct tp_find {
    constexpr static size_t value = tp_find_if_f<C, tp_bind_front<std::is_same, V>>;
};

template <typename C, typename V>
inline constexpr size_t tp_find_v = tp_find<C, V>::value;

template <typename C, typename E, template <typename...> typename F>
struct tp_left_fold;

template <template <typename...> typename C, typename E,
          template <typename...> typename F>
struct tp_left_fold<C<>, E, F> {
    using type = E;
};

template <template <typename...> typename C, typename T, typename... Args, typename E,
          template <typename...> typename F>
struct tp_left_fold<C<T, Args...>, E, F> {
    using type = typename tp_left_fold<C<Args...>, F<E, T>, F>::type;
};

// f(L<A1, A2, ... An>, E, F) -> F<F<F...<F<E, A1>, A2>, ...>, An>
template <typename C, typename E, template <typename...> typename F>
using tp_left_fold_t = typename tp_left_fold<C, E, F>::type;

template <typename C, typename E, typename F>
using tp_left_fold_f = typename tp_left_fold<C, E, F::template fn>::type;

template <typename C, typename E, template <typename...> typename F>
struct tp_right_fold;

template <template <typename...> typename C, typename E,
          template <typename...> typename F>
struct tp_right_fold<C<>, E, F> {
    using type = E;
};

template <template <typename...> typename C, typename T, typename... Args, typename E,
          template <typename...> typename F>
struct tp_right_fold<C<T, Args...>, E, F> {
    using next_type = typename tp_right_fold<C<Args...>, E, F>::type;
    using type = F<T, next_type>;
};

// f(L<A1, A2, ... An>, E, F) -> F<A1, F<A2, ... F<An, E>...>>
template <typename C, typename E, template <typename...> typename F>
using tp_right_fold_t = typename tp_right_fold<C, E, F>::type;

template <typename C, typename E, typename F>
using tp_right_fold_f = typename tp_right_fold<C, E, F::template fn>::type;

template <typename C, template <typename...> typename P>
struct tp_unique_if {
    using type = tp_left_fold_f<C, tp_clear_t<C>,
                                tp_bind<tp_conditional_t, tp_bind_front<P>, tp_arg<0>,
                                        tp_bind_front<tp_push_back_t>>>;
};

// using NOW_LIST = tp_prefix_t<C, I + 1>;
// using PRE_LIST = tp_prefix_t<C, I>;
// using PRE_UNIQUE_IF_LIST = tp_unique_if_t<PRE_LIST>;
// then :
// tp_unique_if_t<NOW_LIST, P>
// = tp_conditonal_t<
// P<PRE_UNIQUE_IF_LIST, tp_at_t<C, I>>,
// PRE_UNIQUE_IF_LIST,
// tp_push_back_t<PRE_UNIQUE_IF_LIST, tp_at_t<C, I>>>
//
// It is equivalent to calling P every time on the results
// of the previous processing and the new value.
// If P is false, the new value is added
template <typename C, template <typename...> typename P>
using tp_unique_if_t = typename tp_unique_if<C, P>::type;

template <typename C, typename P>
using tp_unique_if_f = typename tp_unique_if<C, P::template fn>::type;

template <typename C>
struct tp_unique {
    using type = tp_unique_if_t<C, tp_contains>;
};

// same as tp_unique_if_t<C, tp_contains>
// remove the same type
template <typename C>
using tp_unique_t = typename tp_unique<C>::type;

/// @private
template <typename _Enable, typename C, typename C1, typename C2,
          template <typename...> typename P>
struct __tp_merge_helper;

/// @private
template <typename _Enable, template <typename...> typename C, typename... Args,
          template <typename...> typename C1, template <typename...> typename C2,
          typename... Args2, template <typename...> typename P>
struct __tp_merge_helper<_Enable, C<Args...>, C1<>, C2<Args2...>, P> {
    using type = tp_list<Args..., Args2...>;
};

/// @private
template <typename _Enable, template <typename...> typename C, typename... Args,
          template <typename...> typename C1, typename... Args1,
          template <typename...> typename C2, template <typename...> typename P>
struct __tp_merge_helper<_Enable, C<Args...>, C1<Args1...>, C2<>, P> {
    using type = tp_list<Args..., Args1...>;
};

/// @private
template <typename _Enable, template <typename...> typename C, typename... Args,
          template <typename...> typename C1, template <typename...> typename C2,
          template <typename...> typename P>
struct __tp_merge_helper<_Enable, C<Args...>, C1<>, C2<>, P> {
    using type = tp_list<Args...>;
};

/// @private
template <template <typename...> typename C, typename... Args,
          template <typename...> typename C1, typename T1, typename... Args1,
          template <typename...> typename C2, typename T2, typename... Args2,
          template <typename...> typename P>
struct __tp_merge_helper<std::enable_if_t<P<T1, T2>::value, void>, C<Args...>,
                         C1<T1, Args1...>, C2<T2, Args2...>, P> {
    using type = typename __tp_merge_helper<void, C<Args..., T1>, C1<Args1...>,
                                            C2<T2, Args2...>, P>::type;
};

/// @private
template <template <typename...> typename C, typename... Args,
          template <typename...> typename C1, typename T1, typename... Args1,
          template <typename...> typename C2, typename T2, typename... Args2,
          template <typename...> typename P>
struct __tp_merge_helper<std::enable_if_t<!P<T1, T2>::value, void>, C<Args...>,
                         C1<T1, Args1...>, C2<T2, Args2...>, P> {
    using type = typename __tp_merge_helper<void, C<Args..., T2>, C1<T1, Args1...>,
                                            C2<Args2...>, P>::type;
};

template <typename C1, typename C2, template <typename...> typename P>
struct tp_merge {
    using type = typename __tp_merge_helper<void, tp_list<>, C1, C2, P>::type;
};

// like std::merge
// merge two list with P
template <typename C1, typename C2, template <typename...> typename P>
using tp_merge_t = typename tp_merge<C1, C2, P>::type;

template <typename C1, typename C2, typename P>
using tp_merge_f = typename tp_merge<C1, C2, P::template fn>::type;

template <typename C, template <typename...> typename P>
struct tp_sort;

/// @private
template <typename C, template <typename...> typename P>
struct __tp_sort_helper;

/// @private
template <template <typename...> typename C, typename... Args,
          template <typename...> typename P>
struct __tp_sort_helper<C<Args...>, P> {
    using _Container = C<Args...>;
    constexpr static size_t size = tp_size_v<_Container>;
    constexpr static size_t mid = size / 2;
    using type1 = typename __tp_sort_helper<tp_prefix_t<_Container, mid>, P>::type;
    using type2 = typename __tp_sort_helper<tp_suffix_t<_Container, size - mid>, P>::type;
    using type = tp_merge_t<type1, type2, P>;
};

/// @private
template <template <typename...> typename C, typename T,
          template <typename...> typename P>
struct __tp_sort_helper<C<T>, P> {
    using type = C<T>;
};

/// @private
template <template <typename...> typename C, template <typename...> typename P>
struct __tp_sort_helper<C<>, P> {
    using type = C<>;
};

template <template <typename...> typename C, typename... Args,
          template <typename...> typename P>
struct tp_sort<C<Args...>, P> {
    using type = tp_rename_t<typename __tp_sort_helper<C<Args...>, P>::type, C>;
};

// list std::sort
template <typename C, template <typename...> typename P>
using tp_sort_t = typename tp_sort<C, P>::type;

template <typename C, typename P>
using tp_sort_f = typename tp_sort<C, P::template fn>::type;

/// @private
template <typename T, typename S>
struct __tp_make_integer_sequence_helper;

/// @private
template <typename T, T... Idxs>
struct __tp_make_integer_sequence_helper<T, std::integer_sequence<T, Idxs...>> {
    using type = tp_list<std::integral_constant<T, Idxs>...>;
};

template <typename T, T N>
using tp_make_integer_sequence =
    typename __tp_make_integer_sequence_helper<T, std::make_integer_sequence<T, N>>::type;

template <size_t N>
using tp_make_index_sequence = tp_make_integer_sequence<size_t, N>;

template <typename... Args>
using tp_index_sequence_for = tp_make_index_sequence<sizeof...(Args)>;

/// @private
template <typename T, typename S>
struct __tp_make_std_integer_sequence_helper;

/// @private
template <typename T, T... Idxs>
struct __tp_make_std_integer_sequence_helper<
    T, tp_list<std::integral_constant<T, Idxs>...>> {
    using type = std::integer_sequence<T, Idxs...>;
};

template <typename S>
using tp_make_std_index_sequence =
    typename __tp_make_std_integer_sequence_helper<size_t, S>::type;

} // namespace wjr

#endif // WJR_TP_LIST_HPP__