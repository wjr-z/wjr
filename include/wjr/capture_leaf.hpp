#ifndef WJR_CAPTURE_LEAF_HPP__
#define WJR_CAPTURE_LEAF_HPP__

#include <wjr/crtp/class_base.hpp>
#include <wjr/tp.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T, typename Tag = void>
class capture_leaf : enable_special_members_of_args_base<Tag, T> {
    using Mybase = enable_special_members_of_args_base<Tag, T>;

public:
    using Mybase::Mybase;

    template <typename... Args,
              std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
    constexpr capture_leaf(Args &&...args)
        : Mybase(enable_default_constructor), m_value(std::forward<Args>(args)...) {}

    template <typename Ty = T,
              std::enable_if_t<std::is_default_constructible_v<Ty>, int> = 0>
    constexpr capture_leaf(in_place_default_construct_t)
        : Mybase(enable_default_constructor) {}

    constexpr T &get() noexcept { return m_value; }
    constexpr const T &get() const noexcept { return m_value; }

private:
    T m_value = {};
};

template <typename T, typename Tag = void>
class compressed_capture_leaf : T {
    using Mybase = T;

public:
    using Mybase::Mybase;

    template <typename... Args,
              std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
    constexpr compressed_capture_leaf(Args &&...args)
        : Mybase(std::forward<Args>(args)...) {}

    template <typename Ty = T,
              std::enable_if_t<std::is_default_constructible_v<Ty>, int> = 0>
    constexpr compressed_capture_leaf(in_place_default_construct_t) {}

    constexpr T &get() noexcept { return *this; }
    constexpr const T &get() const noexcept { return *this; }
};

template <typename C>
struct tuple_size {
    static constexpr size_t value = tp_size_v<tp_rename_t<C, tp_list>>;
};

template <typename C>
inline constexpr size_t tuple_size_v = tuple_size<C>::value;

template <size_t I, typename C>
struct tuple_element {
    using type = tp_at_t<tp_rename_t<C, tp_list>, I>;
};

template <size_t I, typename C>
using tuple_element_t = typename tuple_element<I, C>::type;

} // namespace wjr

#endif // WJR_CAPTURE_LEAF_HPP__