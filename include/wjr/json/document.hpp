#ifndef WJR_JSON_DOCUMENT_HPP__
#define WJR_JSON_DOCUMENT_HPP__

#include <map>

#include <wjr/json/visitor.hpp>

namespace wjr::json {

template <typename Traits>
class basic_json;

template <typename Traits>
struct get_relocate_mode<basic_json<Traits>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

template <template <typename Char, typename Traits, typename Alloc> typename String,
          template <typename Key, typename Ty, typename Pr, typename Alloc>
          typename Object,
          template <typename T, typename Alloc> typename Array,
          template <typename T> typename Allocator>
struct basic_json_traits {
private:
    using json_type = basic_json<basic_json_traits>;

public:
    using string_type = String<char, std::char_traits<char>, Allocator<char>>;
    using object_type = Object<string_type, json_type, std::less<string_type>,
                               Allocator<std::pair<const string_type, json_type>>>;
    using array_type = Array<json_type, Allocator<json_type>>;
};

using default_json_traits =
    basic_json_traits<std::basic_string, std::map, vector, memory_pool>;
using __default_json_string = typename json::default_json_traits::string_type;

template <typename Traits>
class basic_json {};

} // namespace wjr::json

namespace wjr {
WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(default_json_string,
                                         json::__default_json_string);
}

#endif // WJR_JSON_DOCUMENT_HPP__