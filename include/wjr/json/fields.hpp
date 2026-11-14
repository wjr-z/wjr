#ifndef WJR_JSON_FIELDS_HPP__
#define WJR_JSON_FIELDS_HPP__

#include <string_view>
#include <tuple>

#include <wjr/preprocessor/queue/algorithm.hpp>

namespace wjr::json {

/// @brief Name and member pointer used by the native typed parser.
template <auto Member>
struct field_descriptor {
    static constexpr auto member = Member;
    std::string_view name;
};

} // namespace wjr::json

#define __WJR_JSON_FIELD_DESCRIPTOR(var)                                                           \
    ::wjr::json::field_descriptor<&WjrJsonOwner::var> { #var }

/** @brief Register required writable instance members for native typed parsing. */
#define WJR_REGISTER_JSON_FIELDS(Type, ...)                                                        \
public:                                                                                            \
    template <typename WjrJsonOwner = Type>                                                        \
    static constexpr auto wjr_json_fields() noexcept {                                             \
        return std::tuple{                                                                         \
            WJR_PP_QUEUE_TRANSFORM_EXPAND((__VA_ARGS__), __WJR_JSON_FIELD_DESCRIPTOR)};            \
    }

#endif // WJR_JSON_FIELDS_HPP__
