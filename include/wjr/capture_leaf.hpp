#ifndef WJR_CAPTURE_LEAF_HPP__
#define WJR_CAPTURE_LEAF_HPP__

#include <array>
#include <tuple>

#include <wjr/crtp/class_base.hpp>
#include <wjr/tp.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T, typename Tag = void>
class capture_leaf : enable_special_members_of_args_base<Tag, T> {
    using Mybase = enable_special_members_of_args_base<Tag, T>;

public:
    template <typename Ty = T, WJR_REQUIRES(std::is_default_constructible_v<Ty>)>
    constexpr capture_leaf() : Mybase(enable_default_constructor), m_value() {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr capture_leaf(Args &&...args)
        : Mybase(enable_default_constructor), m_value(std::forward<Args>(args)...) {}

    template <typename Ty = T, WJR_REQUIRES(std::is_default_constructible_v<Ty>)>
    constexpr explicit capture_leaf(dctor_t) : Mybase(enable_default_constructor) {}

    constexpr T &get() noexcept { return m_value; }
    constexpr const T &get() const noexcept { return m_value; }

private:
    T m_value;
};

template <typename T, typename Tag = void>
class compressed_capture_leaf : T {
    using Mybase = T;

public:
    template <typename Ty = T, WJR_REQUIRES(std::is_default_constructible_v<Ty>)>
    constexpr compressed_capture_leaf() : Mybase() {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr compressed_capture_leaf(Args &&...args)
        : Mybase(std::forward<Args>(args)...) {}

    template <typename Ty = T, WJR_REQUIRES(std::is_default_constructible_v<Ty>)>
    constexpr explicit compressed_capture_leaf(dctor_t) {}

    constexpr T &get() noexcept { return *this; }
    constexpr const T &get() const noexcept { return *this; }
};

template <typename C>
struct tuple_size {
    static constexpr size_t value = tp_size_v<tp_rename_t<C, tp_list>>;
};

template <typename T, size_t N>
struct tuple_size<std::array<T, N>> {
    static constexpr size_t value = N;
};

template <typename C>
inline constexpr size_t tuple_size_v = tuple_size<C>::value;

template <size_t I, typename C>
struct tuple_element {
    using type = tp_at_t<tp_rename_t<C, tp_list>, I>;
};

template <size_t I, typename C>
using tuple_element_t = typename tuple_element<I, C>::type;

template <typename T>
struct tuple_like : std::false_type {};

template <typename T>
inline constexpr bool tuple_like_v = tuple_like<T>::value;

template <typename... Ts>
struct tuple_like<std::tuple<Ts...>> : std::true_type {};

template <typename T, size_t N>
struct tuple_like<std::array<T, N>> : std::true_type {};

template <typename T, typename U>
struct tuple_like<std::pair<T, U>> : std::true_type {};

template <typename T, typename = void>
struct __pair_like_impl : std::false_type {};

template <typename T>
struct __pair_like_impl<T, std::enable_if_t<tuple_like_v<T> && tuple_size_v<T> == 2>>
    : std::true_type {};

template <typename T>
struct pair_like : __pair_like_impl<T> {};

template <typename T>
inline constexpr bool pair_like_v = pair_like<T>::value;

} // namespace wjr

#endif // WJR_CAPTURE_LEAF_HPP__