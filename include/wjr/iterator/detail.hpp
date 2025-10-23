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

/// Extract difference_type from iterator traits
template <typename Iter>
using iterator_difference_t = typename std::iterator_traits<Iter>::difference_type;

/// Extract value_type from iterator traits
template <typename Iter>
using iterator_value_t = typename std::iterator_traits<Iter>::value_type;

/// Extract reference type from iterator traits
template <typename Iter>
using iterator_reference_t = typename std::iterator_traits<Iter>::reference;

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
struct __is_iterator_impl : std::false_type {};

template <typename Iter>
struct __is_iterator_impl<Iter, std::void_t<typename std::iterator_traits<Iter>::iterator_category>>
    : std::true_type {};

/**
 * @brief Check if type is an iterator
 * @tparam Iter Type to check
 */
template <typename Iter>
struct is_iterator : __is_iterator_impl<Iter> {};

template <typename Iter>
inline constexpr bool is_iterator_v = is_iterator<Iter>::value;

/// @private
template <typename Iter, typename Category, typename = void>
struct __is_category_iterator_impl : std::false_type {};

/// @private
template <typename Iter, typename Category>
struct __is_category_iterator_impl<
    Iter, Category, std::void_t<typename std::iterator_traits<Iter>::iterator_category>>
    : std::is_base_of<Category, iterator_category_t<Iter>> {};

/**
 * @brief Check if iterator is an input iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_input_iterator : __is_category_iterator_impl<Iter, std::input_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_input_iterator_v = is_input_iterator<Iter>::value;

/**
 * @brief Check if iterator is an output iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_output_iterator : __is_category_iterator_impl<Iter, std::output_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_output_iterator_v = is_output_iterator<Iter>::value;

/**
 * @brief Check if iterator is a forward iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_forward_iterator : __is_category_iterator_impl<Iter, std::forward_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_forward_iterator_v = is_forward_iterator<Iter>::value;

/**
 * @brief Check if iterator is a bidirectional iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_bidirectional_iterator
    : __is_category_iterator_impl<Iter, std::bidirectional_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_bidirectional_iterator_v = is_bidirectional_iterator<Iter>::value;

/**
 * @brief Check if iterator is a random access iterator
 * @tparam Iter Iterator type to check
 */
template <typename Iter>
struct is_random_access_iterator
    : __is_category_iterator_impl<Iter, std::random_access_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_random_access_iterator_v = is_random_access_iterator<Iter>::value;

namespace detail {
/// @private
template <typename Iter>
struct __is_contiguous_iterator_impl
    : std::disjunction<std::is_pointer<Iter>, std::is_array<Iter>> {};

/// @private
template <typename Iter>
struct __is_contiguous_iterator_impl<std::move_iterator<Iter>>
    : std::conjunction<__is_contiguous_iterator_impl<Iter>,
                       std::is_trivially_copyable<iterator_value_t<Iter>>> {};

/// @todo Is std::contiguous_iterator needed here?
template <typename Iter>
inline constexpr bool __is_contiguous_iterator_impl_v =
    __is_contiguous_iterator_impl<Iter>::value || ranges::contiguous_iterator<Iter>;
} // namespace detail

template <typename Iter>
struct is_contiguous_iterator : std::bool_constant<detail::__is_contiguous_iterator_impl_v<Iter>> {
};

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
 * @details Inherits from \c ranges::contiguous_iterator_tag and
 * \c std::contiguous_iterator_tag (if C++20 is available).
 * range-v3's contiguous iterator tag does not inherit from std's tag, so this
 * tag bridges that gap.
 */
struct contiguous_iterator_tag : ranges::contiguous_iterator_tag
#if defined(WJR_CPP_20)
    ,
                                 std::contiguous_iterator_tag
#endif
{
};

} // namespace wjr

#endif // WJR_ITERATOR_DETAIL_HPP__