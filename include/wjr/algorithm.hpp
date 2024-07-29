#ifndef WJR_FORMAT_ALGORITHM_HPP__
#define WJR_FORMAT_ALGORITHM_HPP__

#include <string_view>

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename CharT, typename Traits>
WJR_PURE WJR_INTRINSIC_INLINE bool
starts_with(std::basic_string_view<CharT, Traits> str,
            std::basic_string_view<CharT, Traits> sv) noexcept {
    const auto length = sv.size();
    if (str.size() < length) {
        return false;
    }

    return Traits::compare(str.data(), sv.data(), length) == 0;
}

template <typename CharT, typename Traits>
WJR_PURE WJR_INTRINSIC_INLINE bool
ends_with(std::basic_string_view<CharT, Traits> str,
          std::basic_string_view<CharT, Traits> sv) noexcept {
    const auto n = str.size();
    const auto length = sv.size();
    if (n < length) {
        return false;
    }

    return Traits::compare(str.data() + n - length, sv.data(), length) == 0;
}

} // namespace wjr

#endif // WJR_FORMAT_ALGORITHM_HPP__