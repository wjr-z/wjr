#include <wjr/json/document.hpp>

namespace wjr::json::visitor_detail {

template result<void>
parse<detail::basic_document_parser<document> &>(detail::basic_document_parser<document> &par,
                                                 ondemand_reader &rd) noexcept;

template result<void> parse<detail::check_parser>(detail::check_parser &&par,
                                                  ondemand_reader &rd) noexcept;

} // namespace wjr::json::visitor_detail