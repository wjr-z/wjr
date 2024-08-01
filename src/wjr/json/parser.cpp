#include <wjr/json/parser.hpp>

namespace wjr::json {

template result<void> parser_visitor::parse<check_parser>(check_parser &&,
                                                          const reader &) noexcept;

}