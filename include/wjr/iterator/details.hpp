#ifndef WJR_ITERATOR_DETAILS_HPP__
#define WJR_ITERATOR_DETAILS_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename Iter>
using iterator_difference_t = typename std::iterator_traits<Iter>::difference_type;

template <typename Iter>
using iterator_value_t = typename std::iterator_traits<Iter>::value_type;

template <typename Iter>
using iterator_reference_t = typename std::iterator_traits<Iter>::reference;

template <typename Iter>
using iterator_pointer_t = typename std::iterator_traits<Iter>::pointer;

template <typename Iter>
using iterator_category_t = typename std::iterator_traits<Iter>::iterator_category;

template <typename Iter, typename = void>
struct __is_iterator_impl : std::false_type {};

template <typename Iter>
struct __is_iterator_impl<
    Iter, std::void_t<typename std::iterator_traits<Iter>::iterator_category>>
    : std::true_type {};

template <typename Iter>
struct is_iterator : __is_iterator_impl<Iter> {};

template <typename Iter>
inline constexpr bool is_iterator_v = is_iterator<Iter>::value;

template <typename Iter, typename Category, typename = void>
struct __is_category_iterator_impl : std::false_type {};

template <typename Iter, typename Category>
struct __is_category_iterator_impl<
    Iter, Category, std::void_t<typename std::iterator_traits<Iter>::iterator_category>>
    : std::is_base_of<Category, iterator_category_t<Iter>> {};

template <typename Iter>
struct is_input_iterator : __is_category_iterator_impl<Iter, std::input_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_input_iterator_v = is_input_iterator<Iter>::value;

template <typename Iter>
struct is_output_iterator : __is_category_iterator_impl<Iter, std::output_iterator_tag> {
};

template <typename Iter>
inline constexpr bool is_output_iterator_v = is_output_iterator<Iter>::value;

template <typename Iter>
struct is_forward_iterator
    : __is_category_iterator_impl<Iter, std::forward_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_forward_iterator_v = is_forward_iterator<Iter>::value;

template <typename Iter>
struct is_bidirectional_iterator
    : __is_category_iterator_impl<Iter, std::bidirectional_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_bidirectional_iterator_v =
    is_bidirectional_iterator<Iter>::value;

template <typename Iter>
struct is_random_access_iterator
    : __is_category_iterator_impl<Iter, std::random_access_iterator_tag> {};

template <typename Iter>
inline constexpr bool is_random_access_iterator_v =
    is_random_access_iterator<Iter>::value;

template <typename Iter, typename = void>
struct __is_contiguous_iterator_impl
    : std::disjunction<std::is_pointer<Iter>, std::is_array<Iter>> {};

#if defined(WJR_CPP_20)
template <typename Iter>
struct is_contiguous_iterator : __is_contiguous_iterator_impl<Iter> {};

template <std::contiguous_iterator Iter>
struct is_contiguous_iterator<Iter> : std::true_type {};
#else
template <typename Iter>
struct is_contiguous_iterator : __is_contiguous_iterator_impl<Iter> {};
#endif

template <typename Iter>
inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<Iter>::value;

template <typename Iter, std::enable_if_t<is_contiguous_iterator_v<Iter>, int> = 0>
using iterator_contiguous_value_t = std::remove_reference_t<iterator_reference_t<Iter>>;

template <typename Iter, std::enable_if_t<is_contiguous_iterator_v<Iter>, int> = 0>
using iterator_contiguous_pointer_t =
    std::add_pointer_t<iterator_contiguous_value_t<Iter>>;

} // namespace wjr

#endif // WJR_ITERATOR_DETAILS_HPP__