#include "old_api.hpp"
#include "old_checked.hpp"
namespace wjr::biginteger_detail {
template from_chars_result<const char *> _probe_old_from_chars_impl<true>(const char *,
                                                                          const char *,
                                                                          biginteger_dispatcher,
                                                                          unsigned int) noexcept;
}
