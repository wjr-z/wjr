#ifndef WJR_JSON_DETAIL_UTF8_HPP__
#define WJR_JSON_DETAIL_UTF8_HPP__

#include <cstddef>
#include <string_view>

namespace wjr::json::detail {

/// @brief Return the first malformed UTF-8 sequence offset, or string_view::npos.
std::size_t find_invalid_utf8(std::string_view input) noexcept;

} // namespace wjr::json::detail

#endif // WJR_JSON_DETAIL_UTF8_HPP__
