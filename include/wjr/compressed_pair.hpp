/**
 * @file compressed_pair.hpp
 * @brief Space-optimized pair using empty base optimization
 * @author wjr
 *
 * Provides compressed_pair, a pair-like container that uses empty base
 * optimization to reduce memory footprint when one or both types are empty.
 */

#ifndef WJR_COMPRESSED_PAIR_HPP__
#define WJR_COMPRESSED_PAIR_HPP__

#include <wjr/tuple.hpp>

namespace wjr {

/**
 * @brief Space-optimized pair container
 *
 * Similar to std::pair but uses empty base optimization to avoid wasting
 * space when storing empty types (e.g., stateless allocators).
 *
 * @tparam T First element type
 * @tparam U Second element type
 */
template <typename T, typename U>
class compressed_pair;

template <typename T, typename U>
using cpair = compressed_pair<T, U>;

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

/// @private
template <typename T, typename U>
struct _compressed_pair1 {};

/// @private
template <typename T, typename U>
struct _compressed_pair2 {};

template <typename T, typename U>
using _compressed_pair_base_tag1 =
    enable_special_members_of_args_base<enable_base_identity_t<0, _compressed_pair1<T, U>>, T>;

template <typename T, typename U>
using _compressed_pair_base_tag2 =
    enable_special_members_of_args_base<enable_base_identity_t<1, _compressed_pair2<T, U>>, U>;

template <typename T, typename U, typename Enable = void>
struct _compressed_pair_base;

template <typename T, typename U>
struct WJR_EMPTY_BASES
    _compressed_pair_base<T, U, std::enable_if_t<!is_compressed_v<T> && !is_compressed_v<U>>>
    : _compressed_pair_base_tag1<T, U>, _compressed_pair_base_tag2<T, U> {
    using Mybase1 = _compressed_pair_base_tag1<T, U>;
    using Mybase2 = _compressed_pair_base_tag2<T, U>;

    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr _compressed_pair_base() noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T>, std::is_nothrow_constructible<U>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor), m_first(),
          m_second() {}

    template <typename Ty, typename Uy,
              WJR_REQUIRES(
                  std::conjunction_v<std::is_constructible<T, Ty>, std::is_constructible<U, Uy>>)>
    constexpr explicit _compressed_pair_base(Ty &&t, Uy &&u) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Ty>,
                           std::is_nothrow_constructible<U, Uy>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          m_first(std::forward<Ty>(t)), m_second(std::forward<Uy>(u)) {}

    template <typename Tuple1, typename Tuple2, size_t... N1, size_t... N2>
    constexpr _compressed_pair_base(
        Tuple1 &tp1, Tuple2 &tp2, std::index_sequence<N1...>,
        std::index_sequence<N2...>) noexcept(std::
                                                 conjunction_v<
                                                     std::is_nothrow_constructible<
                                                         T, std::tuple_element_t<N1, Tuple1>...>,
                                                     std::is_nothrow_constructible<
                                                         U, std::tuple_element_t<N2, Tuple2>...>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          m_first(std::get<N1>(std::move(tp1))...), m_second(std::get<N2>(std::move(tp2))...) {}

    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr explicit _compressed_pair_base(default_construct_t, default_construct_t) noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<T>,
                           std::is_nothrow_default_constructible<U>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor) {}

    template <typename Ty = T, typename Uy,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_constructible<U, Uy>>)>
    constexpr explicit _compressed_pair_base(default_construct_t, Uy &&u) noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<T>,
                           std::is_nothrow_constructible<U, Uy>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          m_second(std::forward<Uy>(u)) {}

    template <typename Ty, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_constructible<T, Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr explicit _compressed_pair_base(T &&t, default_construct_t) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Ty>,
                           std::is_nothrow_default_constructible<U>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          m_first(std::forward<Ty>(t)) {}

    constexpr T &first() noexcept { return m_first; }
    constexpr const T &first() const noexcept { return m_first; }

    constexpr U &second() noexcept { return m_second; }
    constexpr const U &second() const noexcept { return m_second; }

    T m_first;
    U m_second;
};

template <typename T, typename U>
struct container_of_fn<_compressed_pair_base<T, U>, std::in_place_index_t<0>,
                       std::enable_if_t<!is_compressed_v<T> && !is_compressed_v<U> &&
                                        std::is_standard_layout_v<_compressed_pair_base<T, U>>>> {
    using base_type = _compressed_pair_base<T, U>;
    using value_type = T;

    base_type &operator()(value_type &ref) const noexcept {
        return *reinterpret_cast<base_type *>(reinterpret_cast<std::byte *>(std::addressof(ref)) -
                                              offsetof(base_type, m_first));
    }

    const base_type &operator()(const value_type &ref) const noexcept {
        return *reinterpret_cast<const base_type *>(
            reinterpret_cast<const std::byte *>(std::addressof(ref)) -
            offsetof(base_type, m_first));
    }
};

template <typename T, typename U>
struct container_of_fn<_compressed_pair_base<T, U>, std::in_place_index_t<1>,
                       std::enable_if_t<!is_compressed_v<T> && !is_compressed_v<U> &&
                                        std::is_standard_layout_v<_compressed_pair_base<T, U>>>> {
    using base_type = _compressed_pair_base<T, U>;
    using value_type = U;

    base_type &operator()(value_type &ref) const noexcept {
        return *reinterpret_cast<base_type *>(reinterpret_cast<std::byte *>(std::addressof(ref)) -
                                              offsetof(base_type, m_second));
    }

    const base_type &operator()(const value_type &ref) const noexcept {
        return *reinterpret_cast<const base_type *>(
            reinterpret_cast<const std::byte *>(std::addressof(ref)) -
            offsetof(base_type, m_second));
    }
};

template <typename T, typename U>
struct WJR_EMPTY_BASES _compressed_pair_base<T, U, std::enable_if_t<is_compressed_v<T>>>
    : _compressed_pair_base_tag1<T, U>, _compressed_pair_base_tag2<T, U>, T {
    using Mybase1 = _compressed_pair_base_tag1<T, U>;
    using Mybase2 = _compressed_pair_base_tag2<T, U>;

    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr _compressed_pair_base() noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T>, std::is_nothrow_constructible<U>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor), T(),
          m_second() {}

    template <typename Ty, typename Uy,
              WJR_REQUIRES(
                  std::conjunction_v<std::is_constructible<T, Ty>, std::is_constructible<U, Uy>>)>
    constexpr explicit _compressed_pair_base(Ty &&t, Uy &&u) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Ty>,
                           std::is_nothrow_constructible<U, Uy>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          T(std::forward<Ty>(t)), m_second(std::forward<Uy>(u)) {}

    template <typename Tuple1, typename Tuple2, size_t... N1, size_t... N2>
    constexpr _compressed_pair_base(
        Tuple1 &tp1, Tuple2 &tp2, std::index_sequence<N1...>,
        std::index_sequence<N2...>) noexcept(std::
                                                 conjunction_v<
                                                     std::is_nothrow_constructible<
                                                         T, std::tuple_element_t<N1, Tuple1>...>,
                                                     std::is_nothrow_constructible<
                                                         U, std::tuple_element_t<N2, Tuple2>...>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          T(std::get<N1>(std::move(tp1))...), m_second(std::get<N2>(std::move(tp2))...) {}

    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr explicit _compressed_pair_base(default_construct_t, default_construct_t) noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<T>,
                           std::is_nothrow_default_constructible<U>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor) {}

    template <typename Ty = T, typename Uy,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_constructible<U, Uy>>)>
    constexpr explicit _compressed_pair_base(default_construct_t, Uy &&u) noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<T>,
                           std::is_nothrow_constructible<U, Uy>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          m_second(std::forward<Uy>(u)) {}

    template <typename Ty, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_constructible<T, Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr explicit _compressed_pair_base(T &&t, default_construct_t) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Ty>,
                           std::is_nothrow_default_constructible<U>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          T(std::forward<Ty>(t)) {}

    constexpr T &first() noexcept { return *this; }
    constexpr const T &first() const noexcept { return *this; }

    constexpr U &second() noexcept { return m_second; }
    constexpr const U &second() const noexcept { return m_second; }

    U m_second;
};

template <typename T, typename U>
struct container_of_fn<_compressed_pair_base<T, U>, std::in_place_index_t<0>,
                       std::enable_if_t<is_compressed_v<T>>> {
    using base_type = _compressed_pair_base<T, U>;
    using value_type = T;

    base_type &operator()(value_type &ref) const noexcept { return static_cast<base_type &>(ref); }

    const base_type &operator()(const value_type &ref) const noexcept {
        return static_cast<const base_type &>(ref);
    }
};

template <typename T, typename U>
struct container_of_fn<_compressed_pair_base<T, U>, std::in_place_index_t<1>,
                       std::enable_if_t<is_compressed_v<T> &&
                                        std::is_standard_layout_v<_compressed_pair_base<T, U>>>> {
    using base_type = _compressed_pair_base<T, U>;
    using value_type = U;

    base_type &operator()(value_type &ref) const noexcept {
        return *reinterpret_cast<base_type *>(reinterpret_cast<std::byte *>(std::addressof(ref)) -
                                              offsetof(base_type, m_second));
    }

    const base_type &operator()(const value_type &ref) const noexcept {
        return *reinterpret_cast<const base_type *>(
            reinterpret_cast<const std::byte *>(std::addressof(ref)) -
            offsetof(base_type, m_second));
    }
};

template <typename T, typename U>
struct WJR_EMPTY_BASES
    _compressed_pair_base<T, U, std::enable_if_t<!is_compressed_v<T> && is_compressed_v<U>>>
    : _compressed_pair_base_tag1<T, U>, _compressed_pair_base_tag2<T, U>, U {
    using Mybase1 = _compressed_pair_base_tag1<T, U>;
    using Mybase2 = _compressed_pair_base_tag2<T, U>;

    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr _compressed_pair_base() noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T>, std::is_nothrow_constructible<U>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor), U(), m_first() {
    }

    template <typename Ty, typename Uy,
              WJR_REQUIRES(
                  std::conjunction_v<std::is_constructible<T, Ty>, std::is_constructible<U, Uy>>)>
    constexpr explicit _compressed_pair_base(Ty &&t, Uy &&u) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Ty>,
                           std::is_nothrow_constructible<U, Uy>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          U(std::forward<Uy>(u)), m_first(std::forward<Ty>(t)) {}

    template <typename Tuple1, typename Tuple2, size_t... N1, size_t... N2>
    constexpr _compressed_pair_base(
        Tuple1 &tp1, Tuple2 &tp2, std::index_sequence<N1...>,
        std::index_sequence<N2...>) noexcept(std::
                                                 conjunction_v<
                                                     std::is_nothrow_constructible<
                                                         T, std::tuple_element_t<N1, Tuple1>...>,
                                                     std::is_nothrow_constructible<
                                                         U, std::tuple_element_t<N2, Tuple2>...>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          U(std::get<N2>(std::move(tp2))...), m_first(std::get<N1>(std::move(tp1))...) {}

    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr explicit _compressed_pair_base(default_construct_t, default_construct_t) noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<T>,
                           std::is_nothrow_default_constructible<U>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor) {}

    template <typename Ty = T, typename Uy,
              WJR_REQUIRES(std::conjunction_v<std::is_default_constructible<Ty>,
                                              std::is_constructible<U, Uy>>)>
    constexpr explicit _compressed_pair_base(default_construct_t, Uy &&u) noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<T>,
                           std::is_nothrow_constructible<U, Uy>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          U(std::forward<Uy>(u)) {}

    template <typename Ty, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<std::is_constructible<T, Ty>,
                                              std::is_default_constructible<Uy>>)>
    constexpr explicit _compressed_pair_base(T &&t, default_construct_t) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Ty>,
                           std::is_nothrow_default_constructible<U>>)
        : Mybase1(enable_default_constructor), Mybase2(enable_default_constructor),
          m_first(std::forward<Ty>(t)) {}

    constexpr T &first() noexcept { return m_first; }
    constexpr const T &first() const noexcept { return m_first; }

    constexpr U &second() noexcept { return *this; }
    constexpr const U &second() const noexcept { return *this; }

    T m_first;
};

template <typename T, typename U>
struct container_of_fn<
    _compressed_pair_base<T, U>, std::in_place_index_t<0>,
    std::enable_if_t<!is_compressed_v<T> && is_compressed_v<U> && std::is_standard_layout_v<T>>> {
    using base_type = _compressed_pair_base<T, U>;
    using value_type = T;

    base_type &operator()(value_type &ref) const noexcept {
        return *reinterpret_cast<base_type *>(reinterpret_cast<std::byte *>(std::addressof(ref)) -
                                              offsetof(base_type, m_first));
    }

    const base_type &operator()(const value_type &ref) const noexcept {
        return *reinterpret_cast<const base_type *>(
            reinterpret_cast<const std::byte *>(std::addressof(ref)) -
            offsetof(base_type, m_first));
    }
};

template <typename T, typename U>
struct container_of_fn<_compressed_pair_base<T, U>, std::in_place_index_t<1>,
                       std::enable_if_t<!is_compressed_v<T> && is_compressed_v<U>>> {
    using base_type = _compressed_pair_base<T, U>;
    using value_type = U;

    base_type &operator()(value_type &ref) const noexcept { return static_cast<base_type &>(ref); }

    const base_type &operator()(const value_type &ref) const noexcept {
        return static_cast<const base_type &>(ref);
    }
};

/**
 * @class compressed_pair
 *
 * @brief A pair used empty base optimization to reduce the size of the pair.
 *
 * @details Guaranteed to be standard-layout if `T' and `U' both are standard-layout.
 */
template <typename T, typename U>
class WJR_EMPTY_BASES compressed_pair final : private _compressed_pair_base<T, U> {
    using Mybase = _compressed_pair_base<T, U>;

    template <typename Ty, typename Uy>
    using _is_all_copy_constructible =
        std::conjunction<std::is_copy_constructible<Ty>, std::is_copy_constructible<Uy>>;

    template <typename Vty, typename Wuy>
    using _is_all_convertible =
        std::conjunction<std::is_convertible<Vty, T>, std::is_convertible<Wuy, U>>;

    template <typename Vty, typename Wuy>
    using _is_all_constructible = std::is_constructible<Mybase, Vty, Wuy>;

    template <typename _Mybase, typename _Tag, typename _Enable>
    friend struct container_of_fn;

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
              WJR_REQUIRES(_is_all_copy_constructible<Ty, Uy>::value)>
    constexpr explicit(!_is_all_convertible<const Ty &, const Uy &>::value)
        compressed_pair(const T &_First, const U &_Second) noexcept(
            std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                               std::is_nothrow_copy_constructible<Uy>>)
        : Mybase(_First, _Second) {}
#else
    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(std::conjunction_v<_is_all_copy_constructible<Ty, Uy>,
                                              _is_all_convertible<const Ty &, const Uy &>>)>
    constexpr compressed_pair(const T &_First, const U &_Second) noexcept(
        std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                           std::is_nothrow_copy_constructible<Uy>>)
        : Mybase(_First, _Second) {}

    template <typename Ty = T, typename Uy = U,
              WJR_REQUIRES(
                  std::conjunction_v<_is_all_copy_constructible<Ty, Uy>,
                                     std::negation<_is_all_convertible<const Ty &, const Uy &>>>)>
    constexpr explicit compressed_pair(const T &_First, const U &_Second) noexcept(
        std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                           std::is_nothrow_copy_constructible<Uy>>)
        : Mybase(_First, _Second) {}
#endif

#if defined(__cpp_conditional_explicit)
    template <typename Other1, typename Other2,
              WJR_REQUIRES(_is_all_constructible<Other1 &&, Other2 &&>::value)>
    constexpr explicit(!_is_all_convertible<Other1 &&, Other2 &&>::value)
        compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
            std::is_nothrow_constructible_v<Mybase, Other1 &&, Other2 &&>)
        : Mybase(std::forward<Other1>(_First), std::forward<Other2>(_Second)) {}
#else
    template <typename Other1, typename Other2,
              WJR_REQUIRES(std::conjunction_v<_is_all_constructible<Other1 &&, Other2 &&>,
                                              _is_all_convertible<Other1 &&, Other2 &&>>)>
    constexpr compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
        std::is_nothrow_constructible_v<Mybase, Other1 &&, Other2 &&>)
        : Mybase(std::forward<Other1>(_First), std::forward<Other2>(_Second)) {}

    template <
        typename Other1, typename Other2,
        WJR_REQUIRES(std::conjunction_v<_is_all_constructible<Other1 &&, Other2 &&>,
                                        std::negation<_is_all_convertible<Other1 &&, Other2 &&>>>)>
    constexpr explicit compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
        std::is_nothrow_constructible_v<Mybase, Other1 &&, Other2 &&>)
        : Mybase(std::forward<Other1>(_First), std::forward<Other2>(_Second)) {}
#endif

private:
    template <typename Tuple1, typename Tuple2, size_t... N1, size_t... N2>
    constexpr compressed_pair(
        Tuple1 &tp1, Tuple2 &tp2, std::index_sequence<N1...> i1,
        std::index_sequence<N2...>
            i2) noexcept(std::is_nothrow_constructible_v<Mybase, Tuple1 &, Tuple2 &,
                                                         std::index_sequence<N1...>,
                                                         std::index_sequence<N2...>>)
        : Mybase(tp1, tp2, i1, i2) {}

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
              WJR_REQUIRES(_is_tuple_test_v<std::is_constructible, compressed_pair, PairLike &&>)>
    constexpr explicit(!_is_tuple_test_v<std::is_convertible, compressed_pair, PairLike &&>)
        compressed_pair(PairLike &&pr) noexcept(
            std::conjunction_v<
                std::is_nothrow_constructible<T, decltype(std::get<0>(std::forward<PairLike>(pr)))>,
                std::is_nothrow_constructible<U,
                                              decltype(std::get<1>(std::forward<PairLike>(pr)))>>)
        : Mybase(std::get<0>(std::forward<PairLike>(pr)), std::get<1>(std::forward<PairLike>(pr))) {
    }
#else
    template <typename PairLike,
              WJR_REQUIRES(_is_tuple_test_v<std::is_constructible, compressed_pair, PairLike &&> &&
                               _is_tuple_test_v<std::is_convertible, compressed_pair, PairLike &&>)>
    constexpr compressed_pair(PairLike &&pr) noexcept(
        std::conjunction_v<
            std::is_nothrow_constructible<T, decltype(std::get<0>(std::forward<PairLike>(pr)))>,
            std::is_nothrow_constructible<U, decltype(std::get<1>(std::forward<PairLike>(pr)))>>)
        : Mybase(std::get<0>(std::forward<PairLike>(pr)), std::get<1>(std::forward<PairLike>(pr))) {
    }

    template <typename PairLike,
              WJR_REQUIRES(_is_tuple_test_v<std::is_constructible, compressed_pair, PairLike &&> &&
                           !_is_tuple_test_v<std::is_convertible, compressed_pair, PairLike &&>)>
    constexpr explicit compressed_pair(PairLike &&pr) noexcept(
        std::conjunction_v<
            std::is_nothrow_constructible<T, decltype(std::get<0>(std::forward<PairLike>(pr)))>,
            std::is_nothrow_constructible<U, decltype(std::get<1>(std::forward<PairLike>(pr)))>>)
        : Mybase(std::get<0>(std::forward<PairLike>(pr)), std::get<1>(std::forward<PairLike>(pr))) {
    }
#endif

    template <typename PairLike,
              WJR_REQUIRES(_is_tuple_test_v<_is_tuple_assignable, compressed_pair, PairLike &&>)>
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

    constexpr T &first() noexcept { return Mybase::first(); }
    constexpr const T &first() const noexcept { return Mybase::first(); }
    constexpr U &second() noexcept { return Mybase::second(); }
    constexpr const U &second() const noexcept { return Mybase::second(); }

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

    template <typename Z, Z I>
    constexpr std::tuple_element_t<I, compressed_pair> &
    operator[](integral_constant<Z, I>) & noexcept {
        return get<I>();
    }

    template <typename Z, Z I>
    constexpr const std::tuple_element_t<I, compressed_pair> &
    operator[](integral_constant<Z, I>) const & noexcept {
        return get<I>();
    }

    template <typename Z, Z I>
    constexpr std::tuple_element_t<I, compressed_pair> &&
    operator[](integral_constant<Z, I>) && noexcept {
        return std::move(*this).template get<I>();
    }

    template <typename Z, Z I>
    constexpr const std::tuple_element_t<I, compressed_pair> &&
    operator[](integral_constant<Z, I>) const && noexcept {
        return std::move(*this).template get<I>();
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

template <typename T, typename U>
struct container_of_fn<compressed_pair<T, U>, std::in_place_index_t<0>> {
    using base_type = compressed_pair<T, U>;
    using value_type = T;

    base_type &operator()(value_type &ref) const noexcept {
        return static_cast<base_type &>(
            container_of<typename base_type::Mybase, std::in_place_index_t<0>>(ref));
    }

    const base_type &operator()(const value_type &ref) const noexcept {
        return static_cast<const base_type &>(
            container_of<typename base_type::Mybase, std::in_place_index_t<0>>(ref));
    }
};

template <typename T, typename U>
struct container_of_fn<compressed_pair<T, U>, std::in_place_index_t<1>> {
    using base_type = compressed_pair<T, U>;
    using value_type = U;

    base_type &operator()(value_type &ref) const noexcept {
        return static_cast<base_type &>(
            container_of<typename base_type::Mybase, std::in_place_index_t<1>>(ref));
    }

    const base_type &operator()(const value_type &ref) const noexcept {
        return static_cast<const base_type &>(
            container_of<typename base_type::Mybase, std::in_place_index_t<1>>(ref));
    }
};

template <typename T1, typename U1, typename T2, typename U2>
WJR_PURE constexpr bool
operator==(const compressed_pair<T1, U1> &lhs, const compressed_pair<T2, U2> &rhs) noexcept(
    std::conjunction_v<has_noexcept_equal_to<const T1 &, const T2 &>,
                       has_noexcept_equal_to<const U1 &, const U2 &>>) {
    return lhs.first() == rhs.first() && lhs.second() == rhs.second();
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_PURE constexpr bool
operator!=(const compressed_pair<T1, U1> &lhs,
           const compressed_pair<T2, U2> &rhs) noexcept(noexcept(lhs == rhs)) {
    return !(lhs == rhs);
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_PURE constexpr bool
operator<(const compressed_pair<T1, U1> &lhs, const compressed_pair<T2, U2> &rhs) noexcept(
    std::conjunction_v<has_noexcept_less<const T1 &, const T2 &>,
                       has_noexcept_less<const T2 &, const T1 &>,
                       has_noexcept_less<const U1 &, const U2 &>>) {
    return lhs.first() < rhs.first() ||
           (!(rhs.first() < lhs.first()) && lhs.second() < rhs.second());
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_PURE constexpr bool
operator>(const compressed_pair<T1, U1> &lhs,
          const compressed_pair<T2, U2> &rhs) noexcept(noexcept(rhs < lhs)) {
    return rhs < lhs;
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_PURE constexpr bool
operator<=(const compressed_pair<T1, U1> &lhs,
           const compressed_pair<T2, U2> &rhs) noexcept(noexcept(rhs < lhs)) {
    return !(rhs < lhs);
}

template <typename T1, typename U1, typename T2, typename U2>
WJR_PURE constexpr bool
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
