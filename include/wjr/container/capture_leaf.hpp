#ifndef WJR_CONTAINER_CAPTURE_LEAF_HPP__
#define WJR_CONTAINER_CAPTURE_LEAF_HPP__

#include <tuple>

#include <wjr/container/container_of.hpp>
#include <wjr/crtp/class_base.hpp>
#include <wjr/tp/algorithm.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

/**
 * @class capture_leaf
 *
 * @brief Capture any type as a new type. Can be used as a class base.
 *
 */
template <typename T, typename Tag = void>
class capture_leaf : enable_special_members_of_args_base<Tag, T> {
    using Mybase = enable_special_members_of_args_base<Tag, T>;

    template <typename _Mybase, typename _Tag, typename _Enable>
    friend struct container_of_fn;

public:
    using value_type = T;

    template <typename Ty = T, WJR_REQUIRES(std::is_default_constructible_v<Ty>)>
    constexpr capture_leaf() noexcept(std::is_nothrow_constructible_v<T>)
        : Mybase(enable_default_constructor), m_value() {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr capture_leaf(Args &&...args) noexcept(std::is_constructible_v<T, Args...>)
        : Mybase(enable_default_constructor), m_value(std::forward<Args>(args)...) {}

    template <typename Ty = T, WJR_REQUIRES(std::is_default_constructible_v<Ty>)>
    constexpr explicit capture_leaf(default_construct_t) noexcept(
        std::is_nothrow_default_constructible_v<T>)
        : Mybase(enable_default_constructor) {}

    constexpr T &get() noexcept { return m_value; }
    constexpr const T &get() const noexcept { return m_value; }

private:
    T m_value;
};

template <typename T, typename TagT>
struct container_of_fn<capture_leaf<T, TagT>, void,
                       std::enable_if_t<std::is_standard_layout_v<T>>> {
    using base_type = capture_leaf<T, TagT>;
    using value_type = T;

    base_type &operator()(value_type &ref) const noexcept {
        return *reinterpret_cast<base_type *>(reinterpret_cast<std::byte *>(std::addressof(ref)) -
                                              offsetof(base_type, m_value));
    }

    const base_type &operator()(const value_type &ref) const noexcept {
        return *reinterpret_cast<const base_type *>(
            reinterpret_cast<const std::byte *>(std::addressof(ref)) -
            offsetof(base_type, m_value));
    }
};

/**
 * @class compressed_capture_leaf
 *
 * @brief Compressed capture any type as a new type.
 *
 * @details Use `EBO`(empty base optimization) to compress the size of the
 * object.
 *
 */
template <typename T, typename Tag = void>
class compressed_capture_leaf : T {
    using Mybase = T;

public:
    using value_type = T;

    template <typename Ty = T, WJR_REQUIRES(std::is_default_constructible_v<Ty>)>
    constexpr compressed_capture_leaf() noexcept(std::is_nothrow_constructible_v<T>) : Mybase() {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr compressed_capture_leaf(Args &&...args) noexcept(std::is_constructible_v<T, Args...>)
        : Mybase(std::forward<Args>(args)...) {}

    template <typename Ty = T, WJR_REQUIRES(std::is_default_constructible_v<Ty>)>
    constexpr explicit compressed_capture_leaf(default_construct_t) noexcept(
        std::is_nothrow_default_constructible_v<T>) {}

    constexpr T &get() noexcept { return *this; }
    constexpr const T &get() const noexcept { return *this; }
};

template <typename T, typename TagT>
struct container_of_fn<compressed_capture_leaf<T, TagT>, void> {
    using base_type = compressed_capture_leaf<T, TagT>;
    using value_type = T;

    constexpr base_type &operator()(value_type &ref) const noexcept {
        return static_cast<base_type &>(ref);
    }

    constexpr const base_type &operator()(const value_type &ref) const noexcept {
        return static_cast<const base_type &>(ref);
    }
};

/**
 * @struct is_compressed
 *
 * @brief Check if a class can be compressed.
 *
 * @details A class can be compressed if it is a `empty` `class`, and it is not
 * `final`.
 *
 */
template <typename T>
struct is_compressed
    : std::conjunction<std::is_class<T>, std::is_empty<T>, std::negation<std::is_final<T>>> {};

/**
 * @brief Value of @ref is_compressed.
 *
 */
template <typename T>
inline constexpr bool is_compressed_v = is_compressed<T>::value;

/// @private
template <typename LP, typename RP, typename = void>
struct __is_tuple_like_impl : std::false_type {};

/// @private
template <typename LP, typename RP>
struct __is_tuple_like_impl<
    LP, RP,
    std::enable_if_t<!std::is_same_v<LP, RP> &&
                     std::tuple_size_v<LP> == tp_defer_t<std::tuple_size, RP>::value>>
    : std::true_type {};

/**
 * @brief Use template<...>typename like to like all element of LP and RP.
 *
 * @details For example, like is std::is_assignable, LP is std::tuple<T0, U0>,
 * RP is std::tuple<T1, U1>. \n Then __is_tuple_like =
 * std::conjunction<std::is_assignable<T0, T1>, std::is_assignable<U0, U1>>.
 *
 */
template <typename LP, typename RP>
struct __is_tuple_like : __is_tuple_like_impl<LP, remove_cvref_t<RP>> {};

/**
 * @brief Value of @ref __is_tuple_like.
 *
 */
template <typename LP, typename RP>
inline constexpr bool __is_tuple_like_v = __is_tuple_like<LP, RP>::value;

/// @private
template <template <typename...> typename Test, typename Seq, typename LP, typename RP,
          typename = void>
struct __is_tuple_test_impl : std::false_type {};

/// @private
template <template <typename...> typename Test, size_t... Idxs, typename LP, typename RP>
struct __is_tuple_test_impl<Test, std::index_sequence<Idxs...>, LP, RP,
                            std::enable_if_t<__is_tuple_like_v<LP, RP>>>
    : std::conjunction<
          Test<std::tuple_element_t<Idxs, LP>, decltype(std::get<Idxs>(std::declval<RP>()))>...> {};

/**
 * @brief Use template<...>typename Test to test all element of LP and RP.
 *
 * @details For example, Test is std::is_assignable, LP is std::tuple<T0, U0>,
 * RP is std::tuple<T1, U1>. \n Then __is_tuple_test =
 * std::conjunction<std::is_assignable<T0, T1>, std::is_assignable<U0, U1>>.
 *
 */
template <template <typename...> typename Test, typename LP, typename RP>
struct __is_tuple_test
    : __is_tuple_test_impl<Test, std::make_index_sequence<std::tuple_size_v<LP>>, LP, RP> {};

/**
 * @brief Value of @ref __is_tuple_test.
 *
 */
template <template <typename...> typename Test, typename LP, typename RP>
inline constexpr bool __is_tuple_test_v = __is_tuple_test<Test, LP, RP>::value;

/// @private
template <typename T, typename U>
struct __is_tuple_assignable : std::is_assignable<T &, U> {};

} // namespace wjr

#endif // WJR_CONTAINER_CAPTURE_LEAF_HPP__