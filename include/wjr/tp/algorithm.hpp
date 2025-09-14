#ifndef WJR_TP_ALGORITHM_HPP__
#define WJR_TP_ALGORITHM_HPP__

#include <wjr/tp/base.hpp>

namespace wjr {

template <typename T, typename U>
struct tp_equal_to : std::is_same<T, U> {};

template <typename T, typename U>
struct tp_not_equal_to : std::negation<tp_equal_to<T, U>> {};

template <typename T, typename U>
struct tp_less : std::bool_constant<(T::value < U::value)> {};

template <typename T, typename U>
struct tp_greater : tp_less<U, T> {};

template <typename T, typename U>
struct tp_less_equal : std::negation<tp_greater<T, U>> {};

template <typename T, typename U>
struct tp_greater_equal : std::negation<tp_less<T, U>> {};

template <typename T, template <typename...> typename F>
struct tp_transform;

template <template <typename...> typename C, typename... Args, template <typename...> typename F>
struct tp_transform<C<Args...>, F> {
    using type = C<F<Args>...>;
};

/**
 * @brief f(L<Args...>, Fn) -> L<Fn(Args)...>
 * @details use with apply, bind, bind_front, bind_back...  \
 * For example: \
 * tp_transform_f<tp_bind_front<tp_apply_f, tp_bind_front<std::is_same>>,   \
 * tp_list<tp_list<int, float>, tp_list<float, float>, tp_list<int, double>>> \
 * -> tp_list<std::is_same<int, float>, std::is_same<float, float>,
 * std::is_same<int,   \ double>>
 *
 */
template <typename T, template <typename...> typename F>
using tp_transform_t = typename tp_transform<T, F>::type;

template <typename T, typename F>
using tp_transform_f = typename tp_transform<T, F::template fn>::type;

template <typename T, template <typename...> typename P, typename U>
struct tp_replace_if;

template <template <typename...> typename C, typename... Args, template <typename...> typename P,
          typename U>
struct tp_replace_if<C<Args...>, P, U> {
    using type = C<tp_conditional_t<P<Args>, U, Args>...>;
};

/// @brief f(L<Args...>, P, U) -> L<if P(Args)::value then U else Args...>
template <typename T, template <typename...> typename P, typename U>
using tp_replace_if_t = typename tp_replace_if<T, P, U>::type;

template <typename T, typename P, typename U>
using tp_replace_if_f = tp_replace_if_t<T, P::template fn, U>;

template <typename T, typename O, typename N>
struct tp_replace {
    using type = tp_replace_if_f<T, tp_bind_front<tp_equal_to, O>, N>;
};

template <typename T, typename O, typename N>
using tp_replace_t = typename tp_replace<T, O, N>::type;

template <typename T, typename U>
struct tp_fill {
    using type = tp_repeat_t<U, tp_size_v<T>>;
};

/// @brief f(L<Args...>, U) -> L<U, U, ..., U>
template <typename T, typename U>
using tp_fill_t = typename tp_fill<T, U>::type;

template <typename T, template <typename...> typename P>
struct tp_count_if;

template <template <typename...> typename C, template <typename...> typename P>
struct tp_count_if<C<>, P> {
    static constexpr size_t value = 0;
};

template <template <typename...> typename C, typename... Args, template <typename...> typename P>
struct tp_count_if<C<Args...>, P> {
    static constexpr size_t value = (P<Args>::value + ...);
};

/// @brief f(L<Args...>, P) -> count(P(Args)::value)
template <typename T, template <typename...> typename P>
constexpr size_t tp_count_if_v = tp_count_if<T, P>::value;

template <typename T, typename P>
constexpr size_t tp_count_if_f_v = tp_count_if_v<T, P::template fn>;

template <typename T, typename V>
struct tp_count {
    static constexpr size_t value = tp_count_if_f_v<T, tp_bind_front<tp_equal_to, V>>;
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

template <template <typename...> typename C, typename... Args, template <typename...> typename P>
struct tp_remove_if<C<Args...>, P> {
    using type = tp_concat_t<C<>, tp_conditional_t<P<Args>, C<>, C<Args>>...>;
};

/// @brief f(L<Args...>, P) -> L<if P(Args)::value then L<> else L<Args>...>
template <typename T, template <typename...> typename P>
using tp_remove_if_t = typename tp_remove_if<T, P>::type;

template <typename T, typename P>
using tp_remove_if_f = tp_remove_if_t<T, P::template fn>;

template <typename T, typename V>
struct tp_remove {
    using type = tp_remove_if_f<T, tp_bind_front<tp_equal_to, V>>;
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

template <template <typename...> typename C, typename... Args>
struct tp_product;

/// @private
template <typename Enable, template <typename...> typename C, typename... Args>
struct __tp_product_helper {
    using type = tp_list<>;
};

/// @private
template <typename Enable, template <typename...> typename C, typename T>
struct __tp_product_helper<Enable, C, T> {
    using type = tp_list<tp_rename_t<T, C>>;
};

/// @private
template <template <typename...> typename C, typename T, template <typename...> typename C1,
          typename... Args1, typename... Args>
struct __tp_product_helper<std::enable_if_t<sizeof...(Args1) != 0, void>, C, T, C1<Args1...>,
                           Args...> {
    using type = tp_concat_t<
        typename __tp_product_helper<void, C, tp_push_back_t<T, Args1>, Args...>::type...>;
};

template <template <typename...> typename C, typename... Args>
struct tp_product {
    using type = typename __tp_product_helper<void, C, tp_list<>, Args...>::type;
};

/**
 * @brief f(C, L<A1, A2>, L<B1, B2, B3>) -> \
 *  L<C<A1, B1>, C<A1, B2>, C<A1, B3>, C<A2, B1>, C<A2, B2>, C<A2, B3>>
 *
 */
template <template <typename...> typename C, typename... Args>
using tp_product_t = typename tp_product<C, Args...>::type;

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
template <typename Enable, size_t idx, typename C, template <typename...> typename P>
struct __tp_find_if_helper;

/// @private
template <typename Enable, size_t idx, template <typename...> typename C, typename T,
          typename... Args, template <typename...> typename P>
struct __tp_find_if_helper<Enable, idx, C<T, Args...>, P> {
    constexpr static size_t value = __tp_find_if_helper<void, idx + 1, C<Args...>, P>::value;
};

/// @private
template <typename Enable, size_t idx, template <typename...> typename C,
          template <typename...> typename P>
struct __tp_find_if_helper<Enable, idx, C<>, P> {
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

template <typename C, typename V>
struct tp_find {
    constexpr static size_t value = tp_find_if_f<C, tp_bind_front<tp_equal_to, V>>;
};

template <typename C, typename V>
inline constexpr size_t tp_find_v = tp_find<C, V>::value;

template <typename C, template <typename...> typename P>
struct __tp_unique_helper;

template <template <typename...> typename C, template <typename...> typename P, typename T,
          typename U, typename... Args>
struct __tp_unique_helper<C<T, U, Args...>, P> {
    using Next = typename __tp_unique_helper<C<U, Args...>, P>::type;
    using Now = std::conditional_t<P<T, U>::value, C<>, C<T>>;
    using type = tp_concat_t<Now, Next>;
};

template <template <typename...> typename C, template <typename...> typename P, typename T>
struct __tp_unique_helper<C<T>, P> {
    using type = C<T>;
};

template <template <typename...> typename C, template <typename...> typename P>
struct __tp_unique_helper<C<>, P> {
    using type = C<>;
};

template <typename C, template <typename...> typename P>
struct tp_unique {
    using type = typename __tp_unique_helper<C, P>::type;
};

template <typename C, template <typename...> typename P = tp_equal_to>
using tp_unique_t = typename tp_unique<C, P>::type;

template <typename C, typename P = tp_make_fn<tp_equal_to>>
using tp_unique_f = typename tp_unique<C, P::template fn>::type;

/// @private
template <typename Enable, typename C, typename C1, typename C2, template <typename...> typename P>
struct __tp_merge_helper;

/// @private
template <typename Enable, template <typename...> typename C, typename... Args,
          template <typename...> typename C1, template <typename...> typename C2, typename... Args2,
          template <typename...> typename P>
struct __tp_merge_helper<Enable, C<Args...>, C1<>, C2<Args2...>, P> {
    using type = tp_list<Args..., Args2...>;
};

/// @private
template <typename Enable, template <typename...> typename C, typename... Args,
          template <typename...> typename C1, typename... Args1, template <typename...> typename C2,
          template <typename...> typename P>
struct __tp_merge_helper<Enable, C<Args...>, C1<Args1...>, C2<>, P> {
    using type = tp_list<Args..., Args1...>;
};

/// @private
template <typename Enable, template <typename...> typename C, typename... Args,
          template <typename...> typename C1, template <typename...> typename C2,
          template <typename...> typename P>
struct __tp_merge_helper<Enable, C<Args...>, C1<>, C2<>, P> {
    using type = tp_list<Args...>;
};

/// @private
template <template <typename...> typename C, typename... Args, template <typename...> typename C1,
          typename T1, typename... Args1, template <typename...> typename C2, typename T2,
          typename... Args2, template <typename...> typename P>
struct __tp_merge_helper<std::enable_if_t<P<T1, T2>::value, void>, C<Args...>, C1<T1, Args1...>,
                         C2<T2, Args2...>, P> {
    using type =
        typename __tp_merge_helper<void, C<Args..., T1>, C1<Args1...>, C2<T2, Args2...>, P>::type;
};

/// @private
template <template <typename...> typename C, typename... Args, template <typename...> typename C1,
          typename T1, typename... Args1, template <typename...> typename C2, typename T2,
          typename... Args2, template <typename...> typename P>
struct __tp_merge_helper<std::enable_if_t<!P<T1, T2>::value, void>, C<Args...>, C1<T1, Args1...>,
                         C2<T2, Args2...>, P> {
    using type =
        typename __tp_merge_helper<void, C<Args..., T2>, C1<T1, Args1...>, C2<Args2...>, P>::type;
};

template <typename C1, typename C2, template <typename...> typename P>
struct tp_merge {
    using type = typename __tp_merge_helper<void, tp_list<>, C1, C2, P>::type;
};

// like std::merge
// merge two list with P
template <typename C1, typename C2, template <typename...> typename P = tp_less>
using tp_merge_t = typename tp_merge<C1, C2, P>::type;

template <typename C1, typename C2, typename P = tp_make_fn<tp_less>>
using tp_merge_f = typename tp_merge<C1, C2, P::template fn>::type;

template <typename C, template <typename...> typename P>
struct tp_sort;

/// @private
template <typename C, template <typename...> typename P>
struct __tp_sort_helper;

/// @private
template <template <typename...> typename C, typename... Args, template <typename...> typename P>
struct __tp_sort_helper<C<Args...>, P> {
    using _Container = C<Args...>;
    constexpr static size_t size = tp_size_v<_Container>;
    constexpr static size_t mid = size / 2;
    using type1 = typename __tp_sort_helper<tp_prefix_t<_Container, mid>, P>::type;
    using type2 = typename __tp_sort_helper<tp_suffix_t<_Container, size - mid>, P>::type;
    using type = tp_merge_t<type1, type2, P>;
};

/// @private
template <template <typename...> typename C, typename T, template <typename...> typename P>
struct __tp_sort_helper<C<T>, P> {
    using type = C<T>;
};

/// @private
template <template <typename...> typename C, template <typename...> typename P>
struct __tp_sort_helper<C<>, P> {
    using type = C<>;
};

template <template <typename...> typename C, typename... Args, template <typename...> typename P>
struct tp_sort<C<Args...>, P> {
    using type = tp_rename_t<typename __tp_sort_helper<C<Args...>, P>::type, C>;
};

// list std::sort
template <typename C, template <typename...> typename P = tp_less>
using tp_sort_t = typename tp_sort<C, P>::type;

template <typename C, typename P = tp_make_fn<tp_less>>
using tp_sort_f = typename tp_sort<C, P::template fn>::type;

template <typename C0, typename C1, typename P>
struct __tp_lexicographical_compare__helper;

template <template <typename...> typename C0, template <typename...> typename C1, typename P,
          typename T, typename... Args0, typename U, typename... Args1>
struct __tp_lexicographical_compare__helper<C0<T, Args0...>, C1<U, Args1...>, P> {
    using Less = std::bool_constant<P::template fn<T, U>::value>;
    using Next = typename __tp_lexicographical_compare__helper<C0<Args0...>, C1<Args1...>, P>::type;
    using type = std::conditional_t<Less::value, Less,
                                    typename tp_defer_t<__tp_lexicographical_compare__helper,
                                                        C0<Args0...>, C1<Args1...>, P>::type>;
};

template <template <typename...> typename C0, template <typename...> typename C1, typename P,
          typename... Args0>
struct __tp_lexicographical_compare__helper<C0<Args0...>, C1<>, P> {
    using type = std::false_type;
};

template <template <typename...> typename C0, template <typename...> typename C1, typename P,
          typename... Args1>
struct __tp_lexicographical_compare__helper<C0<>, C1<Args1...>, P> {
    using type = std::true_type;
};

template <template <typename...> typename C0, template <typename...> typename C1, typename P>
struct __tp_lexicographical_compare__helper<C0<>, C1<>, P> {
    using type = std::false_type;
};

template <typename C0, typename C1, template <typename...> typename P>
struct tp_lexicographical_compare {
    using type = typename __tp_lexicographical_compare__helper<C0, C1, tp_make_fn<P>>::type;
};

template <typename C0, typename C1, template <typename...> typename P = tp_less>
using tp_lexicographical_compare_t = typename tp_lexicographical_compare<C0, C1, P>::type;

template <typename C0, typename C1, typename P = tp_make_fn<tp_less>>
using tp_lexicographical_compare_f =
    typename tp_lexicographical_compare<C0, C1, P::template fn>::type;

template <typename T, typename U, typename P>
struct __tp_max_element_transform {
    using type = std::conditional_t<P::template fn<T, U>::value, U, T>;
};

template <typename T, typename U, typename P>
using __tp_max_element_transform_t = typename __tp_max_element_transform<T, U, P>::type;

template <typename C, template <typename...> typename P>
struct tp_max_element {
    using Front = tp_front_t<C>;
    using Rest = tp_pop_front_t<C>;

    using type =
        tp_left_fold_f<Rest, Front, tp_bind_back<__tp_max_element_transform_t, tp_make_fn<P>>>;
};

template <typename C, template <typename...> typename P = tp_less>
using tp_max_element_t = typename tp_max_element<C, P>::type;

template <typename C, typename P = tp_make_fn<tp_less>>
using tp_max_element_f = typename tp_max_element<C, P::template fn>::type;

template <typename T, typename U, typename P>
struct __tp_min_element_transform {
    using type = std::conditional_t<P::template fn<T, U>::value, T, U>;
};

template <typename T, typename U, typename P>
using __tp_min_element_transform_t = typename __tp_min_element_transform<T, U, P>::type;

template <typename C, template <typename...> typename P>
struct tp_min_element {
    using Front = tp_front_t<C>;
    using Rest = tp_pop_front_t<C>;

    using type =
        tp_left_fold_f<Rest, Front, tp_bind_back<__tp_min_element_transform_t, tp_make_fn<P>>>;
};

template <typename C, template <typename...> typename P = tp_less>
using tp_min_element_t = typename tp_min_element<C, P>::type;

template <typename C, typename P = tp_make_fn<tp_less>>
using tp_min_element_f = typename tp_min_element<C, P::template fn>::type;

template <typename Enable, typename C0, typename C1, template <typename...> typename P>
struct __tp_exclude_helper;

template <typename Enable, template <typename...> typename C0, template <typename...> typename C1,
          template <typename...> typename P, typename... Args1>
struct __tp_exclude_helper<Enable, C0<>, C1<Args1...>, P> {
    using type = C0<>;
};

template <typename Enable, template <typename...> typename C0, template <typename...> typename C1,
          template <typename...> typename P, typename... Args0>
struct __tp_exclude_helper<Enable, C0<Args0...>, C1<>, P> {
    using type = C0<Args0...>;
};

template <typename Enable, template <typename...> typename C0, template <typename...> typename C1,
          template <typename...> typename P>
struct __tp_exclude_helper<Enable, C0<>, C1<>, P> {
    using type = C0<>;
};

template <template <typename...> typename C0, template <typename...> typename C1,
          template <typename...> typename P, typename T, typename... Args0, typename U,
          typename... Args1>
struct __tp_exclude_helper<std::enable_if_t<P<T, U>::value>, C0<T, Args0...>, C1<U, Args1...>, P> {
    using type =
        tp_push_front_t<typename __tp_exclude_helper<void, C0<Args0...>, C1<U, Args1...>, P>::type,
                        T>;
};

template <template <typename...> typename C0, template <typename...> typename C1,
          template <typename...> typename P, typename T, typename... Args0, typename U,
          typename... Args1>
struct __tp_exclude_helper<std::enable_if_t<!P<T, U>::value && P<U, T>::value>, C0<T, Args0...>,
                           C1<U, Args1...>, P> {
    using type = typename __tp_exclude_helper<void, C0<T, Args0...>, C1<Args1...>, P>::type;
};

template <template <typename...> typename C0, template <typename...> typename C1,
          template <typename...> typename P, typename T, typename... Args0, typename U,
          typename... Args1>
struct __tp_exclude_helper<std::enable_if_t<!P<T, U>::value && !P<U, T>::value>, C0<T, Args0...>,
                           C1<U, Args1...>, P> {
    using type = typename __tp_exclude_helper<void, C0<Args0...>, C1<Args1...>, P>::type;
};

template <typename C0, typename C1, template <typename...> typename P>
struct tp_exclude {
    using type = typename __tp_exclude_helper<void, C0, C1, P>::type;
};

template <typename C0, typename C1, template <typename...> typename P = tp_less>
using tp_exclude_t = typename tp_exclude<C0, C1, P>::type;

template <typename C0, typename C1, typename P = tp_make_fn<tp_less>>
using tp_exclude_f = typename tp_exclude<C0, C1, P::template fn>::type;

} // namespace wjr

#endif // WJR_TP_ALGORITHM_HPP__