#ifndef WJR_COMPRESSED_PAIR_HPP__
#define WJR_COMPRESSED_PAIR_HPP__

#include <wjr/tuple.hpp>

namespace wjr {

template <typename T, typename U>
class compressed_pair;

} // namespace wjr

namespace std {

template <typename T, typename U>
struct tuple_size<wjr::compressed_pair<T, U>> : integral_constant<size_t, 2> {};

template <size_t I, typename T, typename U>
struct tuple_element<I, wjr::compressed_pair<T, U>> {
    using type = wjr::tp_at_t<wjr::compressed_pair<T, U>, I>;
};

} // namespace std

namespace wjr {

/**
 * @brief Select the base class of compressed_pair.
 *
 * @details For compressed_pair<T, U> : \n
 * If `T` is @ref is_compressed_v "compressed" and `U` is not ref is_compressed_v
 * "compressed", then the base class is
 * @ref compressed_capture_leaf \<T> and @ref capture_leaf \<U>. \n
 * If `T` is not ref is_compressed_v "compressed" and `U` is ref is_compressed_v
 * "compressed", then the base class is
 * @ref capture_leaf \<T> and @ref compressed_capture_leaf \<U>. \n
 * If `T` and `U` are both ref is_compressed_v "compressed", then the base class is
 * @ref compressed_capture_leaf \<T> and @ref capture_leaf \<U>. \n
 * Otherwise, the base class is @ref capture_leaf \<T> and @ref capture_leaf \<U>. \n
 * Notice that both `T` and `U` are ref is_compressed_v "compressed" is not allowed.
 *
 */
template <size_t index, typename T, typename U, typename Tag = void>
using compressed_pair_wrapper =
    std::conditional_t<is_compressed_v<T> && (index == 0 || !is_compressed_v<U>),
                       compressed_capture_leaf<T, enable_base_identity_t<index, Tag>>,
                       capture_leaf<T, enable_base_identity_t<index, Tag>>>;

/// @private
template <typename T, typename U>
struct __compressed_pair1 {};

/// @private
template <typename T, typename U>
struct __compressed_pair2 {};

/// @private
template <typename T, typename U>
struct __compressed_pair3 {};

/// @private
template <typename T, typename U>
using __compressed_pair_base1 = compressed_pair_wrapper<0, T, U, __compressed_pair1<T, U>>;

/// @private
template <typename T, typename U>
using __compressed_pair_base2 = compressed_pair_wrapper<1, U, T, __compressed_pair2<T, U>>;

/**
 * @class compressed_pair
 *
 * @brief A pair used empty base optimization to reduce the size of the pair.
 *
 * @details See @ref compressed_pair_wrapper for the base class of compressed_pair. \n
 * compressed_pair is final, so it can't be derived from. \n
 * For example : \n
 * @code
 * static_assert(sizeof(compressed_pair<int, double>) == sizeof(int) + sizeof(double));
 * static_assert(sizeof(compressed_pair<std::allocator<int>, int>) == sizeof(int));
 * static_assert(sizeof(compressed_pair<int, std::allocator<int>>) == sizeof(int));
 * @endcode
 */
template <typename T, typename U>
class WJR_EMPTY_BASES compressed_pair final : __compressed_pair_base1<T, U>,
                                              __compressed_pair_base2<T, U> {

    using Mybase1 = __compressed_pair_base1<T, U>;
    using Mybase2 = __compressed_pair_base2<T, U>;

    template <typename Ty, typename Uy>
    using __is_all_copy_constructible =
        std::conjunction<std::is_copy_constructible<Ty>, std::is_copy_constructible<Uy>>;

    template <typename Vty, typename Wuy>
    using __is_all_convertible =
        std::conjunction<std::is_convertible<Vty, T>, std::is_convertible<Wuy, U>>;

    template <typename Vty, typename Wuy>
    using __is_all_constructible =
        std::conjunction<std::is_constructible<Mybase1, Vty>, std::is_constructible<Mybase2, Wuy>>;

public:
    using first_type = T;
    using second_type = U;

#if defined(__cpp_conditional_explicit)
    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr explicit(!std::conjunction_v<is_default_convertible<Ty>, is_default_convertible<Uy>>)
        compressed_pair() noexcept(std::conjunction_v<std::is_nothrow_constructible<Ty>,
                                                      std::is_nothrow_constructible<Uy>>) {}
#else
    template <
        typename Ty = T, typename Uy = U,
        WJR_REQUIRES(
            std::conjunction_v<std::is_default_constructible<Ty>, std::is_default_constructible<Uy>>
                &&std::conjunction_v<is_default_convertible<Ty>, is_default_convertible<Uy>>)>
    constexpr compressed_pair() noexcept(
        std::conjunction_v<std::is_nothrow_constructible<Ty>, std::is_nothrow_constructible<Uy>>) {}

    template <
        typename Ty = T, typename Uy = U,
        WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                        std::is_default_constructible<Uy>> &&
                     !std::conjunction_v<is_default_convertible<Ty>, is_default_convertible<Uy>>)>
    constexpr explicit compressed_pair() noexcept(
        std::conjunction_v<std::is_nothrow_constructible<Ty>, std::is_nothrow_constructible<Uy>>) {}
#endif

#if defined(__cpp_conditional_explicit)
    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(__is_all_copy_constructible<Ty, Uy>::value)>
    constexpr explicit(!__is_all_convertible<const Ty &, const Uy &>::value)
        compressed_pair(const T &_First, const U &_Second) noexcept(
            std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                               std::is_nothrow_copy_constructible<Uy>>)
        : Mybase1(_First), Mybase2(_Second) {}
#else
    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<__is_all_copy_constructible<Ty, Uy>,
                                              __is_all_convertible<const Ty &, const Uy &>>)>
    constexpr compressed_pair(const T &_First, const U &_Second) noexcept(
        std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                           std::is_nothrow_copy_constructible<Uy>>)
        : Mybase1(_First), Mybase2(_Second) {}

    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(
                  std::conjunction_v<__is_all_copy_constructible<Ty, Uy>,
                                     std::negation<__is_all_convertible<const Ty &, const Uy &>>>)>
    constexpr explicit compressed_pair(const T &_First, const U &_Second) noexcept(
        std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                           std::is_nothrow_copy_constructible<Uy>>)
        : Mybase1(_First), Mybase2(_Second) {}
#endif

#if defined(__cpp_conditional_explicit)
    template <typename Other1, typename Other2,
              WJR_REQUIRES(__is_all_constructible<Other1 &&, Other2 &&>::value)>
    constexpr explicit(!__is_all_convertible<Other1 &&, Other2 &&>::value)
        compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
            std::conjunction_v<std::is_nothrow_constructible<Mybase1, Other1 &&>,
                               std::is_nothrow_constructible<Mybase2, Other2 &&>>)
        : Mybase1(std::forward<Other1>(_First)), Mybase2(std::forward<Other2>(_Second)) {}
#else
    template <typename Other1, typename Other2,
              WJR_REQUIRES(std::conjunction_v<__is_all_constructible<Other1 &&, Other2 &&>,
                                              __is_all_convertible<Other1 &&, Other2 &&>>)>
    constexpr compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<Mybase1, Other1 &&>,
                           std::is_nothrow_constructible<Mybase2, Other2 &&>>)
        : Mybase1(std::forward<Other1>(_First)), Mybase2(std::forward<Other2>(_Second)) {}

    template <
        typename Other1, typename Other2,
        WJR_REQUIRES(std::conjunction_v<__is_all_constructible<Other1 &&, Other2 &&>,
                                        std::negation<__is_all_convertible<Other1 &&, Other2 &&>>>)>
    constexpr explicit compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<Mybase1, Other1 &&>,
                           std::is_nothrow_constructible<Mybase2, Other2 &&>>)
        : Mybase1(std::forward<Other1>(_First)), Mybase2(std::forward<Other2>(_Second)) {}
#endif

private:
    template <typename Tuple1, typename Tuple2, size_t... N1, size_t... N2>
    constexpr compressed_pair(
        Tuple1 &tp1, Tuple2 &tp2, std::index_sequence<N1...>,
        std::index_sequence<N2...>) noexcept(noexcept(Mybase1(std::get<N1>(std::move(tp1))...))
                                                 && noexcept(
                                                     Mybase2(std::get<N2>(std::move(tp2))...)))
        : Mybase1(std::get<N1>(std::move(tp1))...), Mybase2(std::get<N2>(std::move(tp2))...) {}

public:
    template <typename... Args1, typename... Args2>
    constexpr compressed_pair(
        std::piecewise_construct_t, tuple<Args1...> tp1,
        tuple<Args2...>
            tp2) noexcept(noexcept(compressed_pair(tp1, tp2, std::index_sequence_for<Args1...>{},
                                                   std::index_sequence_for<Args2...>{})))
        : compressed_pair(tp1, tp2, std::index_sequence_for<Args1...>{},
                          std::index_sequence_for<Args2...>{}) {}

#if defined(__cpp_conditional_explicit)
    template <typename PairLike,
              WJR_REQUIRES(__is_tuple_test_v<std::is_constructible, compressed_pair, PairLike &&>)>
    constexpr explicit(!__is_tuple_test_v<std::is_convertible, compressed_pair, PairLike &&>)
        compressed_pair(PairLike &&pr) noexcept(
            std::conjunction_v<
                std::is_nothrow_constructible<T, decltype(std::get<0>(std::forward<PairLike>(pr)))>,
                std::is_nothrow_constructible<U,
                                              decltype(std::get<1>(std::forward<PairLike>(pr)))>>)
        : Mybase1(std::get<0>(std::forward<PairLike>(pr))),
          Mybase2(std::get<1>(std::forward<PairLike>(pr))) {}
#else
    template <
        typename PairLike,
        WJR_REQUIRES(__is_tuple_test_v<std::is_constructible, compressed_pair, PairLike &&>
                         &&__is_tuple_test_v<std::is_convertible, compressed_pair, PairLike &&>)>
    constexpr compressed_pair(PairLike &&pr) noexcept(
        std::conjunction_v<
            std::is_nothrow_constructible<T, decltype(std::get<0>(std::forward<PairLike>(pr)))>,
            std::is_nothrow_constructible<U, decltype(std::get<1>(std::forward<PairLike>(pr)))>>)
        : Mybase1(std::get<0>(std::forward<PairLike>(pr))),
          Mybase2(std::get<1>(std::forward<PairLike>(pr))) {}

    template <typename PairLike,
              WJR_REQUIRES(__is_tuple_test_v<std::is_constructible, compressed_pair, PairLike &&> &&
                           !__is_tuple_test_v<std::is_convertible, compressed_pair, PairLike &&>)>
    constexpr explicit compressed_pair(PairLike &&pr) noexcept(
        std::conjunction_v<
            std::is_nothrow_constructible<T, decltype(std::get<0>(std::forward<PairLike>(pr)))>,
            std::is_nothrow_constructible<U, decltype(std::get<1>(std::forward<PairLike>(pr)))>>)
        : Mybase1(std::get<0>(std::forward<PairLike>(pr))),
          Mybase2(std::get<1>(std::forward<PairLike>(pr))) {}
#endif

    template <typename PairLike,
              WJR_REQUIRES(__is_tuple_test_v<__is_tuple_assignable, compressed_pair, PairLike &&>)>
    constexpr compressed_pair &operator=(PairLike &&pr) noexcept(
        std::conjunction_v<std::is_nothrow_assignable<T, decltype(std::get<0>(pr))>,
                           std::is_nothrow_assignable<U, decltype(std::get<1>(pr))>>) {
        first() = std::get<0>(std::forward<PairLike>(pr));
        second() = std::get<1>(std::forward<PairLike>(pr));
        return *this;
    }

    template <typename Myself = compressed_pair, typename _T = T,
              WJR_REQUIRES(std::conjunction_v<is_swappable<_T>, is_swappable<U>>)>
    constexpr void swap(type_identity_t<compressed_pair &> other) noexcept(
        std::conjunction_v<std::is_nothrow_swappable<T>, std::is_nothrow_swappable<U>>) {
        std::swap(first(), other.first());
        std::swap(second(), other.second());
    }

    constexpr T &first() noexcept { return Mybase1::get(); }
    constexpr const T &first() const noexcept { return Mybase1::get(); }
    constexpr U &second() noexcept { return Mybase2::get(); }
    constexpr const U &second() const noexcept { return Mybase2::get(); }

    // extension

    template <size_t I, WJR_REQUIRES(I < 2)>
    constexpr std::tuple_element_t<I, compressed_pair> &get() & noexcept {
        if constexpr (I == 0) {
            return first();
        } else {
            return second();
        }
    }

    template <size_t I, WJR_REQUIRES(I < 2)>
    constexpr const std::tuple_element_t<I, compressed_pair> &get() const & noexcept {
        if constexpr (I == 0) {
            return first();
        } else {
            return second();
        }
    }

    template <size_t I, WJR_REQUIRES(I < 2)>
    constexpr std::tuple_element_t<I, compressed_pair> &&get() && noexcept {
        return static_cast<std::tuple_element_t<I, compressed_pair> &&>(get());
    }

    template <size_t I, WJR_REQUIRES(I < 2)>
    constexpr const std::tuple_element_t<I, compressed_pair> &&get() const && noexcept {
        return static_cast<const std::tuple_element_t<I, compressed_pair> &&>(get());
    }

    template <size_t I, WJR_REQUIRES(I >= 0 && I < 2)>
    constexpr std::tuple_element_t<I, compressed_pair> &
    operator[](integral_constant<size_t, I>) & noexcept {
        return get<I>();
    }

    template <size_t I, WJR_REQUIRES(I >= 0 && I < 2)>
    constexpr const std::tuple_element_t<I, compressed_pair> &
    operator[](integral_constant<size_t, I>) const & noexcept {
        return get<I>();
    }

    template <size_t I, WJR_REQUIRES(I >= 0 && I < 2)>
    constexpr std::tuple_element_t<I, compressed_pair> &&
    operator[](integral_constant<size_t, I>) && noexcept {
        return static_cast<std::tuple_element_t<I, compressed_pair> &&>(get<I>());
    }

    template <size_t I, WJR_REQUIRES(I >= 0 && I < 2)>
    constexpr const std::tuple_element_t<I, compressed_pair> &&
    operator[](integral_constant<size_t, I>) const && noexcept {
        return static_cast<const std::tuple_element_t<I, compressed_pair> &&>(get<I>());
    }
};

template <typename T, typename U>
compressed_pair(T, U) -> compressed_pair<T, U>;

template <typename T, typename U>
struct get_relocate_mode<compressed_pair<T, U>> {
    static constexpr relocate_t value = get_common_relocate_mode_v<T, U>;
};

template <typename T, typename U>
struct get_relocate_mode<std::pair<T, U>> {
    static constexpr relocate_t value = get_common_relocate_mode_v<T, U>;
};

template <typename T1, typename U1, typename T2, typename U2>
WJR_CONST constexpr bool
operator==(const compressed_pair<T1, U1> &lhs, const compressed_pair<T2, U2> &rhs) noexcept(
    std::conjunction_v<has_noexcept_equal_to<const T1 &, const T2 &>,
                       has_noexcept_equal_to<const U1 &, const U2 &>>) {
    return lhs.first() == rhs.first() && lhs.second() == rhs.second();
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_CONST constexpr bool
operator!=(const compressed_pair<T1, U1> &lhs,
           const compressed_pair<T2, U2> &rhs) noexcept(noexcept(lhs == rhs)) {
    return !(lhs == rhs);
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_CONST constexpr bool
operator<(const compressed_pair<T1, U1> &lhs, const compressed_pair<T2, U2> &rhs) noexcept(
    std::conjunction_v<has_noexcept_less<const T1 &, const T2 &>,
                       has_noexcept_less<const T2 &, const T1 &>,
                       has_noexcept_less<const U1 &, const U2 &>>) {
    return lhs.first() < rhs.first() ||
           (!(rhs.first() < lhs.first()) && lhs.second() < rhs.second());
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_CONST constexpr bool
operator>(const compressed_pair<T1, U1> &lhs,
          const compressed_pair<T2, U2> &rhs) noexcept(noexcept(rhs < lhs)) {
    return rhs < lhs;
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_CONST constexpr bool
operator<=(const compressed_pair<T1, U1> &lhs,
           const compressed_pair<T2, U2> &rhs) noexcept(noexcept(rhs < lhs)) {
    return !(rhs < lhs);
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_CONST constexpr bool
operator>=(const compressed_pair<T1, U1> &lhs,
           const compressed_pair<T2, U2> &rhs) noexcept(noexcept(lhs < rhs)) {
    return !(lhs < rhs);
}

template <typename T, typename U>
constexpr compressed_pair<unref_wrapper_t<T>, unref_wrapper_t<U>> make_compressed_pair(
    T &&t,
    U &&u) noexcept(std::conjunction_v<std::is_nothrow_constructible<unref_wrapper_t<T>, T &&>,
                                       std::is_nothrow_constructible<unref_wrapper_t<U>, U &&>>) {
    return compressed_pair<unref_wrapper_t<T>, unref_wrapper_t<U>>(std::forward<T>(t),
                                                                   std::forward<U>(u));
}

} // namespace wjr

namespace std {

template <typename T, typename U,
          WJR_REQUIRES(std::conjunction_v<wjr::is_swappable<T>, wjr::is_swappable<U>>)>
constexpr void swap(wjr::compressed_pair<T, U> &lhs,
                    wjr::compressed_pair<T, U> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <size_t I, typename T, typename U, WJR_REQUIRES(I < 2)>
WJR_NODISCARD constexpr tuple_element_t<I, wjr::compressed_pair<T, U>> &
get(wjr::compressed_pair<T, U> &pr) noexcept {
    if constexpr (I == 0) {
        return pr.first();
    } else {
        return pr.second();
    }
}

template <size_t I, typename T, typename U, WJR_REQUIRES(I < 2)>
WJR_NODISCARD constexpr const tuple_element_t<I, wjr::compressed_pair<T, U>> &
get(const wjr::compressed_pair<T, U> &pr) noexcept {
    if constexpr (I == 0) {
        return pr.first();
    } else {
        return pr.second();
    }
}

template <size_t I, typename T, typename U, WJR_REQUIRES(I < 2)>
WJR_NODISCARD constexpr tuple_element_t<I, wjr::compressed_pair<T, U>> &&
get(wjr::compressed_pair<T, U> &&pr) noexcept {
    if constexpr (I == 0) {
        return std::forward<T>(pr.first());
    } else {
        return std::forward<U>(pr.second());
    }
}

template <size_t I, typename T, typename U, WJR_REQUIRES(I < 2)>
WJR_NODISCARD constexpr const tuple_element_t<I, wjr::compressed_pair<T, U>> &&
get(const wjr::compressed_pair<T, U> &&pr) noexcept {
    if constexpr (I == 0) {
        return std::forward<T>(pr.first());
    } else {
        return std::forward<U>(pr.second());
    }
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &get(wjr::compressed_pair<T, U> &pr) noexcept {
    return std::get<0>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &get(const wjr::compressed_pair<T, U> &pr) noexcept {
    return get<0>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &&get(wjr::compressed_pair<T, U> &&pr) noexcept {
    return get<0>(std::move(pr));
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &&get(const wjr::compressed_pair<T, U> &&pr) noexcept {
    return get<0>(std::move(pr));
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &get(wjr::compressed_pair<U, T> &pr) noexcept {
    return get<1>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &get(const wjr::compressed_pair<U, T> &pr) noexcept {
    return get<1>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &&get(wjr::compressed_pair<U, T> &&pr) noexcept {
    return get<1>(std::move(pr));
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &&get(const wjr::compressed_pair<U, T> &&pr) noexcept {
    return get<1>(std::move(pr));
}

} // namespace std

#endif // WJR_COMPRESSED_PAIR_HPP__