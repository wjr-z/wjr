#include <wjr/json/yyjson.hpp>

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace json = wjr::json::yyjson;

extern "C" WJR_NOINLINE bool raw_uint(const yyjson_val *value, uint64_t *output) noexcept {
    if (!yyjson_is_uint(value))
        return false;
    *output = yyjson_get_uint(value);
    return true;
}
extern "C" WJR_NOINLINE bool wrapped_uint(const yyjson_val *value, uint64_t *output) noexcept {
    return json::value_view(value).try_get_uint(*output);
}
extern "C" WJR_NOINLINE bool raw_string(const yyjson_val *value,
                                        std::string_view *output) noexcept {
    if (!yyjson_is_str(value))
        return false;
    *output = {yyjson_get_str(value), yyjson_get_len(value)};
    return true;
}
extern "C" WJR_NOINLINE bool wrapped_string(const yyjson_val *value,
                                            std::string_view *output) noexcept {
    return json::value_view(value).try_get_string(*output);
}
extern "C" WJR_NOINLINE bool raw_number(const yyjson_val *value, double *output) noexcept {
    if (!yyjson_is_num(value))
        return false;
    *output = yyjson_get_num(value);
    return true;
}
extern "C" WJR_NOINLINE bool wrapped_number(const yyjson_val *value, double *output) noexcept {
    return json::value_view(value).try_get_number(*output);
}
extern "C" WJR_NOINLINE uint64_t raw_unchecked(const yyjson_val *value) noexcept {
    const bool matches = yyjson_is_uint(value);
    WJR_ASSERT_ASSUME_L2(matches);
    return yyjson_get_uint(value);
}
extern "C" WJR_NOINLINE uint64_t wrapped_unchecked(const yyjson_val *value) noexcept {
    return json::value_view(value).get_uint_unchecked();
}
extern "C" WJR_NOINLINE uint64_t raw_array(const yyjson_val *value) noexcept {
    const bool matches = yyjson_is_arr(value);
    WJR_ASSERT_ASSUME_L2(matches);
    auto *child = unsafe_yyjson_get_first(value);
    const auto *end = unsafe_yyjson_get_next(value);
    uint64_t sum = 0;
    for (; child != end; child = unsafe_yyjson_get_next(child)) {
        const bool numeric = yyjson_is_uint(child);
        WJR_ASSERT_ASSUME_L2(numeric);
        sum += yyjson_get_uint(child);
    }
    return sum;
}
extern "C" WJR_NOINLINE uint64_t wrapped_array(const yyjson_val *value) noexcept {
    uint64_t sum = 0;
    for (auto child : json::value_view(value).get_array_unchecked())
        sum += child.get_uint_unchecked();
    return sum;
}
extern "C" WJR_NOINLINE const yyjson_val *raw_find(const yyjson_val *value, const char *key,
                                                   size_t length) noexcept {
    const bool matches = yyjson_is_obj(value);
    WJR_ASSERT_ASSUME_L2(matches);
    return yyjson_obj_getn(value, key ? key : "", length);
}
extern "C" WJR_NOINLINE const yyjson_val *wrapped_find(const yyjson_val *value, const char *key,
                                                       size_t length) noexcept {
    return json::value_view(value).get_object_unchecked().find_first({key, length}).native_handle();
}
extern "C" WJR_NOINLINE bool raw_parse(const char *input, size_t length, uint64_t *output,
                                       yyjson_read_err *error) noexcept {
    auto *document =
        yyjson_read_opts(const_cast<char *>(input), length, YYJSON_READ_NOFLAG, nullptr, error);
    if (!document)
        return false;
    auto *root = yyjson_doc_get_root(document);
    const bool valid = yyjson_is_uint(root);
    if (valid)
        *output = yyjson_get_uint(root);
    yyjson_doc_free(document);
    return valid;
}
extern "C" WJR_NOINLINE bool wrapped_parse(const char *input, size_t length, uint64_t *output,
                                           yyjson_read_err *error) noexcept {
    auto document = json::document::parse({input, length}, error);
    if (!document)
        return false;
    return document.root().try_get_uint(*output);
}
