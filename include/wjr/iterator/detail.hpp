/**
 * @file iterator/detail.hpp
 * @brief Iterator type traits and utilities
 * @author wjr
 *
 * Provides type traits for detecting and classifying iterators,
 * including category detection and contiguous iterator support.
 */

#ifndef WJR_ITERATOR_DETAIL_HPP__
#define WJR_ITERATOR_DETAIL_HPP__

#include <iterator>

#include <wjr/type_traits.hpp>

#include <range/v3/iterator/concepts.hpp>

namespace wjr {

/// Extract the C++20 iterator difference type
template <typename Iter>
using iterator_difference_t = std::iter_difference_t<Iter>;

/// @private Preserve void value types of legacy output iterators.
template <typename Iter, typename = void>
struct _iterator_value_impl : std::iterator_traits<Iter> {};

template <typename Iter>
struct _iterator_value_impl<Iter, std::void_t<std::iter_value_t<Iter>>> {
    using value_type = std::iter_value_t<Iter>;
};

/// Extract the C++20 value type, retaining the legacy output-iterator fallback
template <typename Iter>
using iterator_value_t = typename _iterator_value_impl<Iter>::value_type;

/// Extract the actual iterator dereference type
template <typename Iter>
using iterator_reference_t = std::iter_reference_t<Iter>;

/// Extract pointer type from iterator traits
template <typename Iter>
using iterator_pointer_t = typename std::iterator_traits<Iter>::pointer;

/// Extract iterator_category from iterator traits
template <typename Iter>
using iterator_category_t = typename std::iterator_traits<Iter>::iterator_category;

template <typename T>
using iterator_common_reference_t =
    common_reference_t<iterator_reference_t<T>, iterator_value_t<T> &>;

template <typename Iter, typename = void>
struct _is_iterator_impl : std::false_type {};

template <typename Iter>
struct _is_iterator_impl<Iter, std::void_t<typename std::iterator_traits<Iter>::iterator_category>>
    : std::true_type {};

/**
 * @brief Check if type is an iterator
 * @tparam Iter Type to check
 */
template <typename Iter>
struct is_iterator : _is_iterator_impl<Iter> {};

template <typename Iter>
inline constexpr bool is_iterator_v = is_iterator<Iter>::value;

/// @private
template <typename Iter, typename Category, typename = void>
struct _is_category_iterator_impl : std::false_type {};

/// @private
template <typename Iter, typename Category>
struct _is_category_iterator_impl<
    Iter, Category, std::void_t<typename std::iterator_traits<Iter>::iterator_category>>
    : std::is_base_of<Category, iterator_category_t<Iter>> {};

/**
 * @brief Check if iterator is an input iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_input_iterator : _is_category_iterator_impl<Iter, std::input_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_input_iterator_v = is_input_iterator<Iter>::value;

/**
 * @brief Check if iterator is an output iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_output_iterator : _is_category_iterator_impl<Iter, std::output_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_output_iterator_v = is_output_iterator<Iter>::value;

/**
 * @brief Check if iterator is a forward iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_forward_iterator : _is_category_iterator_impl<Iter, std::forward_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_forward_iterator_v = is_forward_iterator<Iter>::value;

/**
 * @brief Check if iterator is a bidirectional iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_bidirectional_iterator
    : _is_category_iterator_impl<Iter, std::bidirectional_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_bidirectional_iterator_v = is_bidirectional_iterator<Iter>::value;

/**
 * @brief Check if iterator is a random access iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_random_access_iterator
    : _is_category_iterator_impl<Iter, std::random_access_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_random_access_iterator_v = is_random_access_iterator<Iter>::value;

/// @private
template <typename Iter>
struct _is_contiguous_iterator_impl
    : std::bool_constant<(std::is_pointer_v<Iter> &&
                          std::is_object_v<std::remove_pointer_t<Iter>>) ||
                         std::is_array_v<Iter>> {};

/// @private
template <typename Iter>
struct _is_contiguous_iterator_impl<std::move_iterator<Iter>>
    : std::conjunction<_is_contiguous_iterator_impl<Iter>,
                       std::is_trivially_copyable<iterator_value_t<Iter>>> {};

// Retain range-v3 tags and the existing array/trivial move-iterator extensions.
template <typename Iter>
inline constexpr bool _is_contiguous_iterator_impl_v =
    _is_contiguous_iterator_impl<Iter>::value || std::contiguous_iterator<Iter> ||
    ranges::contiguous_iterator<Iter>;

template <typename Iter>
struct is_contiguous_iterator : std::bool_constant<_is_contiguous_iterator_impl_v<Iter>> {};

template <typename Iter>
inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<Iter>::value;

template <typename Iter, WJR_REQUIRES(is_contiguous_iterator_v<Iter>)>
using iterator_contiguous_value_t = std::remove_reference_t<iterator_reference_t<Iter>>;

template <typename Iter, WJR_REQUIRES(is_contiguous_iterator_v<Iter>)>
using iterator_contiguous_pointer_t = std::add_pointer_t<iterator_contiguous_value_t<Iter>>;

#if WJR_DEBUG_LEVEL >= 3
    #define WJR_HAS_DEBUG_CONTIGUOUS_ITERATOR_CHECKER WJR_HAS_DEF
#endif

/**
 * @brief Contiguous iterator tag
 * @details Kept for range-v3 compatibility; the trait also accepts standard tags.
 */
using contiguous_iterator_tag = ranges::contiguous_iterator_tag;

} // namespace wjr

#endif // WJR_ITERATOR_DETAIL_HPP__