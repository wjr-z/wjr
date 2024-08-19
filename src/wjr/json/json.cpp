#include <wjr/json/json.hpp>

namespace wjr::json::visitor_detail {

template result<void>
parse<detail::basic_json_parser<json> &>(detail::basic_json_parser<json> &par,
                                         const reader &rd) noexcept;

template result<void> parse<detail::check_parser>(detail::check_parser &&par,
                                                  const reader &rd) noexcept;

} // namespace wjr::json::visitor_detail