#ifndef WJR_TP_STRING_HPP__
#define WJR_TP_STRING_HPP__

#include <wjr/tp/algorithm.hpp>

namespace wjr {

template <typename... Chars>
struct tp_cstr {
    using value_type = char;
};

template <char... Chars>
using _tp_cstr_t = tp_rename_t<tp_integers_list_t<char, Chars...>, tp_cstr>;

template <typename... Args0, typename... Args1>
struct tp_less<tp_cstr<Args0...>, tp_cstr<Args1...>>
    : tp_lexicographical_compare_t<tp_cstr<Args0...>, tp_cstr<Args1...>> {};

template <typename Str, std::size_t... indices>
constexpr decltype(auto) _build_tp_cstr(std::index_sequence<indices...>) {
    return _tp_cstr_t<Str().chars[indices]...>();
}

#define WJR_MAKE_CSTR(str)                                                                         \
    [] {                                                                                           \
        struct WJR_VISIBILITY("hidden") Str {                                                      \
            const char *chars = str;                                                               \
        };                                                                                         \
        return _build_tp_cstr<Str>(std::make_index_sequence<sizeof(str)>());                       \
    }()

} // namespace wjr

#endif // WJR_TP_STRING_HPP__