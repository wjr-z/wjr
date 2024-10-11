#ifndef WJR_TP_MAP_HPP__
#define WJR_TP_MAP_HPP__

#include <wjr/tp/detail.hpp>

namespace wjr {

template <typename Key, typename Value>
struct tp_map_construct {
    using type = tp_zip_t<tp_list, Key, Value>;
};

template <typename Key, typename Value>
using tp_map_construct_t = typename tp_map_construct<Key, Value>::type;

template <typename Map, typename Key, typename Value>
struct tp_map_emplace {};

} // namespace wjr

#endif // WJR_TP_MAP_HPP__