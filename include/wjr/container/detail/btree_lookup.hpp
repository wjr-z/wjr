#ifndef WJR_CONTAINER_DETAIL_BTREE_LOOKUP_HPP__
#define WJR_CONTAINER_DETAIL_BTREE_LOOKUP_HPP__

#include <concepts>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>

namespace wjr::btree_detail {
template <typename Key>
inline constexpr bool standard_char_string_v = false;

template <typename Alloc>
inline constexpr bool
    standard_char_string_v<std::basic_string<char, std::char_traits<char>, Alloc>> = true;

/// @brief Cache only standard string comparisons with ordinary, nonvolatile C-string queries.
/// @note Custom comparators/traits retain their original query overload and visible behavior.
template <typename Compare, typename Key, typename Query>
concept cached_c_string_lookup =
    std::same_as<Compare, std::less<>> && standard_char_string_v<Key> &&
    !std::is_volatile_v<Query> &&
    (std::same_as<std::decay_t<Query>, char *> || std::same_as<std::decay_t<Query>, const char *>);
} // namespace wjr::btree_detail

#endif // WJR_CONTAINER_DETAIL_BTREE_LOOKUP_HPP__
