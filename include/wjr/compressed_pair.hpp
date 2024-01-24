#ifndef WJR_COMPRESSED_PAIR_HPP__
#define WJR_COMPRESSED_PAIR_HPP__

#include <array>
#include <tuple>

#include <wjr/type_traits.hpp>

namespace wjr {

template <size_t index, typename T>
struct comp_pair_wrapper1 {

    template <typename Ty = T,
              std::enable_if_t<std::is_default_constructible_v<Ty>, int> = 0>
    constexpr comp_pair_wrapper1() noexcept(std::is_nothrow_default_constructible_v<Ty>)
        : val() {}

    template <typename Ty = T,
              std::enable_if_t<std::is_copy_constructible_v<Ty>, int> = 0>
    constexpr explicit comp_pair_wrapper1(const Ty &other) noexcept(
        std::is_nothrow_copy_constructible_v<Ty>)
        : val(other) {}

    template <typename Ty = T,
              std::enable_if_t<std::is_move_constructible_v<Ty>, int> = 0>
    constexpr explicit comp_pair_wrapper1(Ty &&other) noexcept(
        std::is_nothrow_move_constructible_v<Ty>)
        : val(std::forward<Ty>(other)) {}

    template <typename... Args,
              std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
    constexpr comp_pair_wrapper1(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : val(std::forward<Args>(args)...) {}

    constexpr comp_pair_wrapper1(const comp_pair_wrapper1 &) = default;
    constexpr comp_pair_wrapper1(comp_pair_wrapper1 &&) noexcept = default;
    constexpr comp_pair_wrapper1 &operator=(const comp_pair_wrapper1 &) = default;
    constexpr comp_pair_wrapper1 &operator=(comp_pair_wrapper1 &&) noexcept = default;

    constexpr T &value() noexcept { return val; }
    constexpr const T &value() const noexcept { return val; }

private:
    T val;
};

template <size_t index, typename T>
struct comp_pair_wrapper2 : private T {
    using Mybase = T;

    template <typename Ty = T,
              std::enable_if_t<std::is_default_constructible_v<Ty>, int> = 0>
    constexpr comp_pair_wrapper2() noexcept(std::is_nothrow_default_constructible_v<Ty>)
        : Mybase() {}

    template <typename Ty = T,
              std::enable_if_t<std::is_copy_constructible_v<Ty>, int> = 0>
    constexpr explicit comp_pair_wrapper2(const Ty &other) noexcept(
        std::is_nothrow_copy_constructible_v<Ty>)
        : Mybase(other) {}

    template <typename Ty = T,
              std::enable_if_t<std::is_move_constructible_v<Ty>, int> = 0>
    constexpr explicit comp_pair_wrapper2(Ty &&other) noexcept(
        std::is_nothrow_move_constructible_v<Ty>)
        : Mybase(std::forward<Ty>(other)) {}

    template <typename... Args,
              std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
    constexpr comp_pair_wrapper2(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : Mybase(std::forward<Args>(args)...) {}

    constexpr comp_pair_wrapper2(const comp_pair_wrapper2 &) = default;
    constexpr comp_pair_wrapper2(comp_pair_wrapper2 &&) noexcept = default;
    constexpr comp_pair_wrapper2 &operator=(const comp_pair_wrapper2 &) = default;
    constexpr comp_pair_wrapper2 &operator=(comp_pair_wrapper2 &&) noexcept = default;

    constexpr T &value() noexcept { return *this; }
    constexpr const T &value() const noexcept { return *this; }
};

template <typename T>
using comp_pair_wrapper_helper =
    std::conjunction<std::is_class<T>, std::is_empty<T>, std::negation<std::is_final<T>>>;

template <size_t index, typename T, typename U>
using comp_pair_wrapper =
    std::conditional_t<comp_pair_wrapper_helper<T>::value &&
                           (index == 0 || !comp_pair_wrapper_helper<U>::value),
                       comp_pair_wrapper2<index, T>, comp_pair_wrapper1<index, T>>;

template <typename T, typename U>
class compressed_pair : private comp_pair_wrapper<0, T, U>,
                        private comp_pair_wrapper<1, U, T> {

    template <typename Ty, typename Uy>
    using __is_all_default_constructible =
        std::conjunction<std::is_default_constructible<Ty>,
                         std::is_default_constructible<Uy>>;

    template <typename Ty, typename Uy>
    using __is_all_default_convertible =
        std::conjunction<is_default_convertible<Ty>, is_default_convertible<Uy>>;

    template <typename Ty, typename Uy>
    using __is_all_copy_constructible =
        std::conjunction<std::is_copy_constructible<Ty>, std::is_copy_constructible<Uy>>;

    template <typename Ty, typename Uy, typename Vty, typename Wuy>
    using __is_all_convertible =
        std::conjunction<std::is_convertible<Vty, Ty>, std::is_convertible<Wuy, Uy>>;

    template <typename Ty, typename Uy, typename Vty, typename Wuy>
    using __is_all_constructible =
        std::conjunction<std::is_constructible<Ty, Vty>, std::is_constructible<Uy, Wuy>>;

public:
    using Mybase1 = comp_pair_wrapper<0, T, U>;
    using Mybase2 = comp_pair_wrapper<1, U, T>;

    using first_type = T;
    using second_type = U;

    template <typename Ty = T, typename Uy = U,
              std::enable_if_t<std::conjunction_v<__is_all_default_constructible<Ty, Uy>,
                                                  __is_all_default_convertible<Ty, Uy>>,
                               bool> = true>
    constexpr compressed_pair() noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<Ty>,
                           std::is_nothrow_default_constructible<Uy>>)
        : Mybase1(), Mybase2() {}

    template <typename Ty = T, typename Uy = U,
              std::enable_if_t<
                  std::conjunction_v<__is_all_default_constructible<Ty, Uy>,
                                     std::negation<__is_all_default_convertible<Ty, Uy>>>,
                  bool> = false>
    constexpr explicit compressed_pair() noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<Ty>,
                           std::is_nothrow_default_constructible<Uy>>)
        : Mybase1(), Mybase2() {}

    template <typename Ty = T, typename Uy = U,
              std::enable_if_t<std::conjunction_v<
                                   __is_all_copy_constructible<Ty, Uy>,
                                   __is_all_convertible<Ty, Uy, const Ty &, const Uy &>>,
                               bool> = true>
    constexpr compressed_pair(const T &_First, const U &_Second) noexcept(
        std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                           std::is_nothrow_copy_constructible<Uy>>)
        : Mybase1(_First), Mybase2(_Second) {}

    template <typename Ty = T, typename Uy = U,
              std::enable_if_t<std::conjunction_v<__is_all_copy_constructible<Ty, Uy>,
                                                  std::negation<__is_all_convertible<
                                                      Ty, Uy, const Ty &, const Uy &>>>,
                               bool> = false>
    constexpr explicit compressed_pair(const T &_First, const U &_Second) noexcept(
        std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                           std::is_nothrow_copy_constructible<Uy>>)
        : Mybase1(_First), Mybase2(_Second) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<std::conjunction_v<__is_all_constructible<T, U, Other1, Other2>,
                                            __is_all_convertible<T, U, Other1, Other2>>,
                         bool> = true>
    constexpr compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Other1>,
                           std::is_nothrow_constructible<U, Other2>>)
        : Mybase1(std::forward<Other1>(_First)), Mybase2(std::forward<Other2>(_Second)) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<
            std::conjunction_v<__is_all_constructible<T, U, Other1, Other2>,
                               std::negation<__is_all_convertible<T, U, Other1, Other2>>>,
            bool> = false>
    constexpr explicit compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Other1>,
                           std::is_nothrow_constructible<U, Other2>>)
        : Mybase1(std::forward<Other1>(_First)), Mybase2(std::forward<Other2>(_Second)) {}

    constexpr compressed_pair(const compressed_pair &) = default;
    constexpr compressed_pair(compressed_pair &&) noexcept = default;

    template <
        typename Other1, typename Other2,
        std::enable_if_t<std::conjunction_v<
                             __is_all_constructible<T, U, const Other1 &, const Other2 &>,
                             __is_all_convertible<T, U, const Other1 &, const Other2 &>>,
                         bool> = true>
    constexpr compressed_pair(const compressed_pair<Other1, Other2> &other) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, const Other1 &>,
                           std::is_nothrow_constructible<U, const Other2 &>>)
        : Mybase1(other.first()), Mybase2(other.second()) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<std::conjunction_v<
                             __is_all_constructible<T, U, const Other1 &, const Other2 &>,
                             std::negation<__is_all_convertible<T, U, const Other1 &,
                                                                const Other2 &>>>,
                         bool> = false>
    constexpr explicit compressed_pair(
        const compressed_pair<Other1, Other2>
            &other) noexcept(std::
                                 conjunction_v<
                                     std::is_nothrow_constructible<T, const Other1 &>,
                                     std::is_nothrow_constructible<U, const Other2 &>>)
        : Mybase1(other.first()), Mybase2(other.second()) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<std::conjunction_v<__is_all_constructible<T, U, Other1, Other2>,
                                            __is_all_convertible<T, U, Other1, Other2>>,
                         bool> = true>
    constexpr compressed_pair(compressed_pair<Other1, Other2> &&other) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Other1>,
                           std::is_nothrow_constructible<U, Other2>>)
        : Mybase1(std::forward<Other1>(other.first())),
          Mybase2(std::forward<Other2>(other.second())) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<
            std::conjunction_v<__is_all_constructible<T, U, Other1, Other2>,
                               std::negation<__is_all_convertible<T, U, Other1, Other2>>>,
            bool> = false>
    constexpr explicit compressed_pair(compressed_pair<Other1, Other2> &&other) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Other1>,
                           std::is_nothrow_constructible<U, Other2>>)
        : Mybase1(std::forward<Other1>(other.first())),
          Mybase2(std::forward<Other2>(other.second())) {}

    template <typename Tuple1, typename Tuple2, size_t... N1, size_t... N2>
    constexpr compressed_pair(Tuple1 &tp1, Tuple2 &tp2, std::index_sequence<N1...>,
                              std::index_sequence<N2...>)
        : Mybase1(std::get<N1>(std::move(tp1))...),
          Mybase2(std::get<N2>(std::move(tp2))...) {}

    template <typename... Args1, typename... Args2>
    constexpr compressed_pair(std::piecewise_construct_t, std::tuple<Args1...> tp1,
                              std::tuple<Args2...> tp2)
        : compressed_pair(tp1, tp2, std::index_sequence_for<Args1...>{},
                          std::index_sequence_for<Args2...>{}) {}

    template <typename Myself = compressed_pair,
              std::enable_if_t<std::conjunction_v<std::is_copy_assignable<T>,
                                                  std::is_copy_assignable<U>>,
                               int> = 0>
    constexpr compressed_pair &operator=(type_identity_t<const Myself &> other) noexcept(
        std::conjunction_v<std::is_nothrow_copy_assignable<T>,
                           std::is_nothrow_copy_assignable<U>>) {
        first() = other.first();
        second() = other.second();
        return *this;
    }

    template <typename Myself = compressed_pair,
              std::enable_if_t<std::conjunction_v<std::is_copy_assignable<T>,
                                                  std::is_copy_assignable<U>>,
                               int> = 0>
    constexpr compressed_pair &operator=(type_identity_t<Myself &&> other) noexcept(
        std::conjunction_v<std::is_nothrow_move_assignable<T>,
                           std::is_nothrow_move_assignable<U>>) {
        first() = std::forward<T>(other.first());
        second() = std::forward<U>(other.second());
        return *this;
    }

    template <typename Other1, typename Other2,
              std::enable_if_t<std::conjunction_v<
                                   std::negation<std::is_same<
                                       compressed_pair, compressed_pair<Other1, Other2>>>,
                                   std::is_assignable<T &, const Other1 &>,
                                   std::is_assignable<U &, const Other2 &>>,
                               int> = 0>
    constexpr compressed_pair &
    operator=(const compressed_pair<Other1, Other2> &other) noexcept(
        std::conjunction_v<std::is_nothrow_assignable<T &, const Other1 &>,
                           std::is_nothrow_assignable<U &, const Other2 &>>) {
        first() = other.first();
        second() = other.second();
        return *this;
    }

    template <typename Other1, typename Other2,
              std::enable_if_t<
                  std::conjunction_v<
                      std::negation<
                          std::is_same<compressed_pair, compressed_pair<Other1, Other2>>>,
                      std::is_assignable<T &, Other1>, std::is_assignable<U &, Other2>>,
                  int> = 0>
    constexpr compressed_pair &
    operator=(compressed_pair<Other1, Other2> &&other) noexcept(
        std::conjunction_v<std::is_nothrow_assignable<T &, Other1>,
                           std::is_nothrow_assignable<U &, Other2>>) {
        first() = std::forward<Other1>(other.first());
        second() = std::forward<Other2>(other.second());
        return *this;
    }

    template <
        typename Myself = compressed_pair,
        std::enable_if_t<std::conjunction_v<is_swappable<T>, is_swappable<U>>, int> = 0>
    constexpr void swap(type_identity_t<compressed_pair &> other) noexcept(
        std::conjunction_v<is_nothrow_swappable<T>, is_nothrow_swappable<U>>) {
        using std::swap;
        swap(first(), other.first());
        swap(second(), other.second());
    }

    constexpr T &first() noexcept { return Mybase1::value(); }
    constexpr const T &first() const noexcept { return Mybase1::value(); }
    constexpr U &second() noexcept { return Mybase2::value(); }
    constexpr const U &second() const noexcept { return Mybase2::value(); }
};

template <typename T, typename U>
compressed_pair(T, U) -> compressed_pair<T, U>;

template <typename T, typename U>
constexpr bool operator==(const compressed_pair<T, U> &lhs,
                          const compressed_pair<T, U> &rhs) {
    return lhs.first() == rhs.first() && lhs.second() == rhs.second();
}

template <typename T, typename U>
constexpr bool operator!=(const compressed_pair<T, U> &lhs,
                          const compressed_pair<T, U> &rhs) {
    return !(lhs == rhs);
}

template <typename T, typename U>
constexpr bool operator<(const compressed_pair<T, U> &lhs,
                         const compressed_pair<T, U> &rhs) {
    return lhs.first() < rhs.first() ||
           (!(rhs.first() < lhs.first()) && lhs.second() < rhs.second());
}

template <typename T, typename U>
constexpr bool operator>(const compressed_pair<T, U> &lhs,
                         const compressed_pair<T, U> &rhs) {
    return rhs < lhs;
}

template <typename T, typename U>
constexpr bool operator<=(const compressed_pair<T, U> &lhs,
                          const compressed_pair<T, U> &rhs) {
    return !(rhs < lhs);
}

template <typename T, typename U>
constexpr bool operator>=(const compressed_pair<T, U> &lhs,
                          const compressed_pair<T, U> &rhs) {
    return !(lhs < rhs);
}

template <typename T, typename U>
constexpr compressed_pair<unrefwrap_t<T>, unrefwrap_t<U>>
make_pair(T &&t, U &&u) noexcept(
    std::conjunction_v<std::is_nothrow_constructible<unrefwrap_t<T>, T>,
                       std::is_nothrow_constructible<unrefwrap_t<U>, U>>) {
    return compressed_pair<unrefwrap_t<T>, unrefwrap_t<U>>(std::forward<T>(t),
                                                           std::forward<U>(u));
}

} // namespace wjr

namespace std {

template <typename T, typename U,
          std::enable_if_t<std::conjunction_v<wjr::is_swappable<T>, wjr::is_swappable<U>>,
                           int> = 0>
constexpr void swap(wjr::compressed_pair<T, U> &lhs,
                    wjr::compressed_pair<T, U> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr std::tuple_element_t<I, wjr::compressed_pair<T, U>> &
get(wjr::compressed_pair<T, U> &pr) noexcept {
    if constexpr (I == 0) {
        return pr.first();
    } else {
        return pr.second();
    }
}

template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr const std::tuple_element_t<I, wjr::compressed_pair<T, U>> &
get(const wjr::compressed_pair<T, U> &pr) noexcept {
    if constexpr (I == 0) {
        return pr.first();
    } else {
        return pr.second();
    }
}

template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr std::tuple_element_t<I, wjr::compressed_pair<T, U>> &&
get(wjr::compressed_pair<T, U> &&pr) noexcept {
    if constexpr (I == 0) {
        return std::forward<T>(pr.first());
    } else {
        return std::forward<U>(pr.second());
    }
}

template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr const std::tuple_element_t<I, wjr::compressed_pair<T, U>> &&
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
    return std::get<0>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &&get(wjr::compressed_pair<T, U> &&pr) noexcept {
    return std::get<0>(std::move(pr));
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &&get(const wjr::compressed_pair<T, U> &&pr) noexcept {
    return std::get<0>(std::move(pr));
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &get(wjr::compressed_pair<U, T> &pr) noexcept {
    return std::get<1>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &get(const wjr::compressed_pair<U, T> &pr) noexcept {
    return std::get<1>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &&get(wjr::compressed_pair<U, T> &&pr) noexcept {
    return std::get<1>(std::move(pr));
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &&get(const wjr::compressed_pair<U, T> &&pr) noexcept {
    return std::get<1>(std::move(pr));
}

template <typename T, typename U>
struct tuple_size<wjr::compressed_pair<T, U>> : std::integral_constant<size_t, 2> {};

template <size_t I, typename T, typename U>
struct tuple_element<I, wjr::compressed_pair<T, U>> {
    static_assert(I < 2, "wjr::compressed_pair has only 2 elements!");
};

template <typename T, typename U>
struct tuple_element<0, wjr::compressed_pair<T, U>> {
    using type = T;
};

template <typename T, typename U>
struct tuple_element<1, wjr::compressed_pair<T, U>> {
    using type = U;
};

} // namespace std

#endif // WJR_COMPRESSED_PAIR_HPP__