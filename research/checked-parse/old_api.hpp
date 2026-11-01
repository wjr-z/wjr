#include <wjr/biginteger.hpp>
namespace wjr::biginteger_detail {
template <bool Check>
WJR_ALL_NONNULL from_chars_result<const char *>
_probe_old_from_chars_impl(const char *, const char *, biginteger_dispatcher,
                           unsigned int) noexcept;
extern template from_chars_result<const char *>
_probe_old_from_chars_impl<true>(const char *, const char *, biginteger_dispatcher,
                                 unsigned int) noexcept;
} // namespace wjr::biginteger_detail
