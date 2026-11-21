#ifndef WJR_CONTAINER_CAPTURE_LEAF_HPP__
#define WJR_CONTAINER_CAPTURE_LEAF_HPP__

#include <concepts>
#include <tuple>

#include <wjr/container/container_of.hpp>
#include <wjr/crtp/class_base.hpp>
#include <wjr/tp/algorithm.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

namespace product_detail {

/** @brief Preserve product storage construction admission without requiring noexcept destruction.
 */
template <typename T, typename... Args>
concept constructible_from = std::is_constructible_v<T, Args...>;

/** @brief Admit the same default initialization forms as the storage traits. */
template <typename T>
concept default_constructible = std::is_default_constructible_v<T>;

/** @brief Preserve the library swap trait used by product types. */
template <typename T>
concept swappable = is_swappable_v<T>;

} // namespace product_detail

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

    template <product_detail::default_constructible Ty = T>
    constexpr capture_leaf() noexcept(std::is_nothrow_constructible_v<T>)
        : Mybase(enable_default_constructor), m_value() {}

    template <typename... Args>
    requires product_detail::constructible_from<T, Args...>
    constexpr capture_leaf(Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : Mybase(enable_default_constructor), m_value(std::forward<Args>(args)...) {}

    template <product_detail::default_constructible Ty = T>
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

    template <product_detail::default_constructible Ty = T>
    constexpr compressed_capture_leaf() noexcept(std::is_nothrow_constructible_v<T>) : Mybase() {}

    template <typename... Args>
    requires product_detail::constructible_from<T, Args...>
    constexpr compressed_capture_leaf(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : Mybase(std::forward<Args>(args)...) {}

    template <product_detail::default_constructible Ty = T>
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

namespace product_detail {

/// @private Require distinct products with equal arity before probing element access.
template <typename Source, typename Destination>
concept same_tuple_arity = !std::same_as<remove_cvref_t<Source>, Destination> && requires {
    std::tuple_size<Destination>::value;
    std::tuple_size<remove_cvref_t<Source>>::value;
} && (std::tuple_size_v<Destination> == std::tuple_size_v<remove_cvref_t<Source>>);

/// @private Test qualified element access and the requested operation together.
template <template <typename...> typename Test, typename Source, typename Destination, size_t I>
concept tuple_element_satisfies = requires {
    requires Test<std::tuple_element_t<I, Destination>,
                  decltype(std::get<I>(std::declval<Source>()))>::value;
};

template <template <typename...> typename Test, typename Source, typename Destination, size_t... I>
constexpr bool tuple_elements_satisfy(std::index_sequence<I...>) {
    return (tuple_element_satisfies<Test, Source, Destination, I> && ...);
}

template <template <typename...> typename Test, typename Source, typename Destination>
concept tuple_satisfies = same_tuple_arity<Source, Destination> &&
                          tuple_elements_satisfy<Test, Source, Destination>(
                              std::make_index_sequence<std::tuple_size_v<Destination>>{});

template <typename T, typename U>
using element_assignable = std::is_assignable<T &, U>;

/// @brief Test tuple construction lazily, including tuple size and get availability.
template <typename Source, typename Destination>
concept tuple_constructible_to = tuple_satisfies<std::is_constructible, Source &&, Destination>;

/// @brief Test assignment through each destination element reference.
template <typename Source, typename Destination>
concept tuple_assignable_to = tuple_satisfies<element_assignable, Source &&, Destination>;

} // namespace product_detail

} // namespace wjr

#endif // WJR_CONTAINER_CAPTURE_LEAF_HPP__