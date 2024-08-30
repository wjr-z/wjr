#ifndef WJR_CONTAINER_INTRUSIVE_DETAIL_HPP__
#define WJR_CONTAINER_INTRUSIVE_DETAIL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename Hook = void, typename Tag = void>
struct intrusive_tag {
    using hook_type = Hook;
    static constexpr bool use_hook = !std::is_same_v<Hook, void>;
};

template <typename Tag>
struct is_intrusive_tag : std::false_type {};

template <typename Hook, typename Tag>
struct is_intrusive_tag<intrusive_tag<Hook, Tag>> : std::true_type {};

template <typename Tag>
inline constexpr bool is_intrusive_tag_v = intrusive_tag<Tag>::value;

template <typename Tag>
using intrusive_hook_t = typename Tag::hook_type;

template <typename Tag>
inline constexpr bool intrusive_use_hook_v = Tag::use_hook;

} // namespace wjr

#endif // WJR_CONTAINER_INTRUSIVE_DETAIL_HPP__