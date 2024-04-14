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

template <template <typename...> typename Test, typename Seq, typename LP, typename RP,
          typename = void>
struct __is_tuple_test_impl : std::false_type {};

template <template <typename...> typename Test, size_t... Idxs, typename LP, typename RP>
struct __is_tuple_test_impl<
    Test, std::index_sequence<Idxs...>, LP, RP,
    std::enable_if_t<!std::is_same_v<LP, remove_cvref_t<RP>> &&
                     std::tuple_size_v<LP> ==
                         tp_defer_t<std::tuple_size, remove_cvref_t<RP>>::value>>
    : std::conjunction<Test<std::tuple_element_t<Idxs, LP>,
                            decltype(std::get<Idxs>(std::declval<RP>()))>...> {};

template <template <typename...> typename Test, typename LP, typename RP>
struct __is_tuple_test
    : __is_tuple_test_impl<Test, std::make_index_sequence<std::tuple_size_v<LP>>, LP,
                           RP> {};

template <template <typename...> typename Test, typename LP, typename RP>
inline constexpr bool __is_tuple_test_v = __is_tuple_test<Test, LP, RP>::value;

template <typename T, typename U>
struct __is_tuple_assignable : std::is_assignable<T &, U> {};

} // namespace wjr

#endif // WJR_CAPTURE_LEAF_HPP__