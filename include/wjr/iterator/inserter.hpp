/**
 * @file iterator/inserter.hpp
 * @brief Insert iterator type traits and utilities
 * @author wjr
 *
 * Provides type traits to detect insert iterators and functions
 * to extract container and position from insert iterators.
 */

#ifndef WJR_ITERATOR_INSERTER_HPP__
#define WJR_ITERATOR_INSERTER_HPP__

#include <ranges>

#include <wjr/iterator/detail.hpp>

namespace wjr {

template <typename T>
struct is_insert_iterator : std::false_type {};

template <typename Container>
struct is_insert_iterator<std::insert_iterator<Container>> : std::true_type {};

template <typename T>
inline constexpr bool is_insert_iterator_v = is_insert_iterator<T>::value;

template <typename T>
struct is_back_insert_iterator : std::false_type {};

template <typename Container>
struct is_back_insert_iterator<std::back_insert_iterator<Container>> : std::true_type {};

template <typename T>
inline constexpr bool is_back_insert_iterator_v = is_back_insert_iterator<T>::value;

template <typename T>
struct is_front_insert_iterator : std::false_type {};

template <typename Container>
struct is_front_insert_iterator<std::front_insert_iterator<Container>> : std::true_type {};

template <typename T>
inline constexpr bool is_front_insert_iterator_v = is_front_insert_iterator<T>::value;

template <typename T>
struct is_any_insert_iterator
    : std::bool_constant<is_insert_iterator_v<T> || is_back_insert_iterator_v<T> ||
                         is_front_insert_iterator_v<T>> {};

template <typename T>
inline constexpr bool is_any_insert_iterator_v = is_any_insert_iterator<T>::value;

/// @private
// Copy into a real derived object to access the standard's protected members.
template <typename Iter>
struct _inserter_container_accessor : Iter {
    constexpr explicit _inserter_container_accessor(const Iter &it) noexcept(
        std::is_nothrow_copy_constructible_v<Iter>)
        : Iter(it) {}
    using Iter::container;
};

/// @private
template <typename Iter>
_inserter_container_accessor(Iter) -> _inserter_container_accessor<Iter>;

/// @private
template <typename Iter>
struct _inserter_iterator_accessor : Iter {
    constexpr explicit _inserter_iterator_accessor(const Iter &it) noexcept(
        std::is_nothrow_copy_constructible_v<Iter>)
        : Iter(it) {}
    using Iter::iter;
};

template <typename Iter>
_inserter_iterator_accessor(Iter) -> _inserter_iterator_accessor<Iter>;

template <typename Container>
constexpr Container &get_inserter_container(std::insert_iterator<Container> it) noexcept(
    std::is_nothrow_copy_constructible_v<std::insert_iterator<Container>>) {
    _inserter_container_accessor tmp(it);
    return *(tmp.container);
}

template <typename Container>
constexpr Container &get_inserter_container(std::back_insert_iterator<Container> it) noexcept(
    std::is_nothrow_copy_constructible_v<std::back_insert_iterator<Container>>) {
    _inserter_container_accessor tmp(it);
    return *(tmp.container);
}

template <typename Container>
constexpr Container &get_inserter_container(std::front_insert_iterator<Container> it) noexcept(
    std::is_nothrow_copy_constructible_v<std::front_insert_iterator<Container>>) {
    _inserter_container_accessor tmp(it);
    return *(tmp.container);
}

template <typename Container>
constexpr std::ranges::iterator_t<Container>
get_inserter_iterator(std::insert_iterator<Container> it) noexcept(
    std::is_nothrow_copy_constructible_v<std::insert_iterator<Container>> &&
    std::is_nothrow_copy_constructible_v<std::ranges::iterator_t<Container>>) {
    _inserter_iterator_accessor tmp(it);
    return tmp.iter;
}

} // namespace wjr

#endif // WJR_ITERATOR_INSERTER_HPP__