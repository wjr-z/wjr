#ifndef WJR_CONTAINER_DETAIL_BTREE_RANGE_HPP__
#define WJR_CONTAINER_DETAIL_BTREE_RANGE_HPP__

#include <concepts>
#include <iterator>
#include <utility>

#include <wjr/crtp/trivially_allocator_base.hpp>

namespace wjr::btree_detail {
template <typename T>
inline constexpr bool range_pair_v = false;
template <typename First, typename Second>
inline constexpr bool range_pair_v<std::pair<First, Second>> = true;

/// @brief Check the uses-allocator constructor forms, including pair component references.
template <typename Value, typename Alloc, typename Reference>
constexpr bool range_allocator_arguments() {
    if constexpr (range_pair_v<std::remove_cv_t<Value>>) {
        if constexpr (range_pair_v<remove_cvref_t<Reference>>) {
            using pair = std::remove_cv_t<Value>;
            return range_allocator_arguments<typename pair::first_type, Alloc,
                                             decltype((std::declval<Reference>().first))>() &&
                   range_allocator_arguments<typename pair::second_type, Alloc,
                                             decltype((std::declval<Reference>().second))>();
        } else {
            return false;
        }
    } else if constexpr (std::uses_allocator_v<Value, Alloc>) {
        return std::constructible_from<Value, std::allocator_arg_t, const Alloc &, Reference> ||
               std::constructible_from<Value, Reference, const Alloc &>;
    } else {
        return std::constructible_from<Value, Reference>;
    }
}

/// @brief Retain ordinary insertion and admit allocator-extended component construction.
/// @note A construct declaration alone cannot validate an unconstrained allocator template body.
template <typename Iter, typename Value, typename Alloc>
concept allocator_insert_iterator =
    std::input_iterator<Iter> && std::destructible<Value> &&
    (std::constructible_from<Value, std::iter_reference_t<Iter>> ||
     (!is_trivially_allocator_construct_v<Alloc, Value, std::iter_reference_t<Iter>> &&
      range_allocator_arguments<Value, Alloc, std::iter_reference_t<Iter>>()));
} // namespace wjr::btree_detail

#endif // WJR_CONTAINER_DETAIL_BTREE_RANGE_HPP__
