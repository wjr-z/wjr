#ifndef WJR_TP_SWITCH_HPP__
#define WJR_TP_SWITCH_HPP__

#include <ratio>

#include <wjr/tp/algorithm.hpp>

namespace wjr {

static constexpr std::ratio<1, 6> switch_jump_table_threshold;

struct switch_default_t {};
inline constexpr switch_default_t switch_default;

template <typename T>
WJR_INTRINSIC_CONSTEXPR bool is_switch_default(T) {
    return std::is_same_v<remove_cvref_t<T>, switch_default_t>;
}

#define WJR_REGISTER_SWITCH_CASE(Case)                                                             \
case tp_at_t<CaseList, (Case)>::value: {                                                           \
    if constexpr ((Case) < Size) {                                                                 \
        return std::forward<Func>(func)(integral_constant<size_t, (Case)>(),                       \
                                        std::forward<Args>(args)...);                              \
    } else {                                                                                       \
        WJR_FALLTHROUGH;                                                                           \
    }                                                                                              \
}

#define WJR_REGISTER_SWITCH_CASE_2(Idx)                                                            \
    WJR_REGISTER_SWITCH_CASE(Idx) WJR_REGISTER_SWITCH_CASE(Idx + 1)

#define WJR_REGISTER_SWITCH_CASE_4(Idx)                                                            \
    WJR_REGISTER_SWITCH_CASE_2(Idx)                                                                \
    WJR_REGISTER_SWITCH_CASE_2(Idx + 2)

#define WJR_REGISTER_SWITCH_CASE_8(Idx)                                                            \
    WJR_REGISTER_SWITCH_CASE_4(Idx)                                                                \
    WJR_REGISTER_SWITCH_CASE_4(Idx + 4)

#define WJR_REGISTER_SWITCH_CASE_16(Idx)                                                           \
    WJR_REGISTER_SWITCH_CASE_8(Idx)                                                                \
    WJR_REGISTER_SWITCH_CASE_8(Idx + 8)

#define WJR_REGISTER_SWITCH_CASE_64(Idx)                                                           \
    WJR_REGISTER_SWITCH_CASE_16(Idx)                                                               \
    WJR_REGISTER_SWITCH_CASE_16(Idx + 16)                                                          \
    WJR_REGISTER_SWITCH_CASE_16(Idx + 32) WJR_REGISTER_SWITCH_CASE_16(Idx + 48)

#define WJR_REGISTER_SWITCH_CASE_256(Idx)                                                          \
    WJR_REGISTER_SWITCH_CASE_64(Idx)                                                               \
    WJR_REGISTER_SWITCH_CASE_64(Idx + 64)                                                          \
    WJR_REGISTER_SWITCH_CASE_64(Idx + 128) WJR_REGISTER_SWITCH_CASE_64(Idx + 192)

#define WJR_REGISTER_SWITCH_DEFAULT                                                                \
default: {                                                                                         \
    if constexpr (std::is_invocable_v<Func &&, switch_default_t, Args &&...>) {                    \
        using Ret = std::invoke_result_t<Func &&, integral_constant<size_t, 0>, Args &&...>;       \
        using DRet = std::invoke_result_t<Func &&, switch_default_t, Args &&...>;                  \
        if constexpr (std::is_same_v<Ret, DRet>) {                                                 \
            return std::forward<Func>(func)(switch_default, std::forward<Args>(args)...);          \
        } else {                                                                                   \
            static_assert(std::is_void_v<DRet>);                                                   \
            std::forward<Func>(func)(switch_default, std::forward<Args>(args)...);                 \
            WJR_UNREACHABLE();                                                                     \
        }                                                                                          \
    } else {                                                                                       \
        WJR_UNREACHABLE();                                                                         \
    }                                                                                              \
}

#define WJR_REGISTER_SWITCH_VISITOR(SZ)                                                            \
    template <typename C, size_t Size>                                                             \
    struct siwtch_visitor_helper<SZ, C, Size> {                                                    \
        using list_type = C;                                                                       \
        using value_type = typename tp_front_t<list_type>::value_type;                             \
        static constexpr value_type MinValue = tp_min_element_t<list_type>::value;                 \
        using CaseList = tp_concat_t<C, tp_exclude_t<tp_iota_t<value_type, MinValue, SZ>, C>>;     \
                                                                                                   \
        template <typename Val, typename Func, typename... Args>                                   \
        WJR_INTRINSIC_CONSTEXPR static decltype(auto) visit(Val ms, Func &&func, Args &&...args) { \
            switch (ms) {                                                                          \
                WJR_PP_CONCAT(WJR_REGISTER_SWITCH_CASE_, SZ)                                       \
                (0) WJR_REGISTER_SWITCH_DEFAULT                                                    \
            }                                                                                      \
        }                                                                                          \
    }

template <size_t SZ, typename C, size_t Size>
struct siwtch_visitor_helper;

WJR_REGISTER_SWITCH_VISITOR(2);
WJR_REGISTER_SWITCH_VISITOR(4);
WJR_REGISTER_SWITCH_VISITOR(8);
WJR_REGISTER_SWITCH_VISITOR(16);
WJR_REGISTER_SWITCH_VISITOR(64);
WJR_REGISTER_SWITCH_VISITOR(256);

#undef WJR_REGISTER_SWITCH_VISITOR
#undef WJR_REGISTER_SWITCH_DEFAULT
#undef WJR_REGISTER_SWITCH_CASE_256
#undef WJR_REGISTER_SWITCH_CASE_64
#undef WJR_REGISTER_SWITCH_CASE_16
#undef WJR_REGISTER_SWITCH_CASE_8
#undef WJR_REGISTER_SWITCH_CASE_4
#undef WJR_REGISTER_SWITCH_CASE_2
#undef WJR_REGISTER_SWITCH_CASE

namespace {
constexpr size_t __switch_visitor_SZ(size_t size) {
    return size <= 2 ? 2 : size <= 4 ? 4 : size <= 8 ? 8 : size <= 16 ? 16 ? size <= 64 : 64 : 256;
}
} // namespace

template <typename C>
struct siwtch_visitor_sorted_helper {
    static constexpr size_t Size = tp_size_v<C>;
    using type = siwtch_visitor_helper<__switch_visitor_SZ(Size), C, Size>;
};

template <typename C>
struct switch_visitor {
    using type = typename siwtch_visitor_sorted_helper<tp_sort_t<C>>::type;
};

template <typename C>
using switch_visitor_t = typename switch_visitor<C>::type;

template <typename C, typename Val, typename Func, typename... Args>
WJR_INTRINSIC_CONSTEXPR decltype(auto) vswitch(Val ms, Func &&func, Args &&...args) {
    return switch_visitor_t<C>::visit(ms, std::forward<Func>(func), std::forward<Args>(args)...);
}

template <typename... Args>
struct __tp_string_list {};

template <char... Chars>
using __tp_string_list_t = tp_rename_t<tp_integers_list_t<char, Chars...>, __tp_string_list>;

template <typename... Args0, typename... Args1>
struct tp_less<__tp_string_list<Args0...>, __tp_string_list<Args1...>>
    : tp_lexicographical_compare_t<__tp_string_list<Args0...>, __tp_string_list<Args1...>> {};

} // namespace wjr

#endif // WJR_TP_SWITCH_HPP__