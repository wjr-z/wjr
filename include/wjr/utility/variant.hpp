#ifndef WJR_VARIANT_HPP__
#define WJR_VARIANT_HPP__

#include <variant>

#include <wjr/tp/algorithm.hpp>

namespace wjr {

namespace variant_detail {

template <typename... Ts>
struct WJR_EMPTY_BASES overloaded : Ts... {
    using Ts::operator()...;
};

template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace variant_detail

template <typename... Args>
std::variant<Args...> &_is_as_variant_impl(std::variant<Args...> &);
template <typename... Args>
const std::variant<Args...> &_is_as_variant_impl(const std::variant<Args...> &);
template <typename... Args>
std::variant<Args...> &&_is_as_variant_impl(std::variant<Args...> &&);
template <typename... Args>
const std::variant<Args...> &&_is_as_variant_impl(const std::variant<Args...> &&);

/// @private
template <typename T, typename Enable>
struct _is_as_variant_helper : std::false_type {};

/// @private
template <typename T>
struct _is_as_variant_helper<T, std::void_t<decltype(_is_as_variant_impl(std::declval<T>()))>>
    : std::true_type {};

template <typename T>
struct is_as_variant : _is_as_variant_helper<T, void> {};

template <typename T>
inline constexpr bool is_as_variant_v = is_as_variant<T>::value;

template <typename Callable, typename... Variants,
          WJR_REQUIRES(std::conjunction_v<is_as_variant<Variants>...>)>
constexpr decltype(auto) visit(Callable &&func, Variants &&...vars) {
    return std::visit(std::forward<Callable>(func), std::forward<Variants>(vars)...);
}

template <typename Var, typename... Args, WJR_REQUIRES(is_as_variant_v<Var>)>
constexpr decltype(auto) match(Var &&var, Args &&...args) {
    return std::visit(variant_detail::overloaded{std::forward<Args>(args)...},
                      std::forward<Var>(var));
}

/// @private Internal implementation details for variant visitation
namespace visit_detail {
/// @private Helper for computing total index from multi-dimensional variant access
template <typename Sizes, typename T>
struct _tp_list_total;

/// @private Type alias for computed total
template <typename Sizes, typename T>
inline constexpr size_t _tp_list_multiply_v = _tp_list_total<Sizes, T>::value;

/// @private Base case for single-dimensional access
template <size_t Size, size_t Idx>
struct _tp_list_total<tp_list<integral_constant<size_t, Size>>, tp_indexs_list_t<Idx>> {
    static constexpr size_t value = Idx;
    static constexpr size_t total = Size;
};

/// @private Recursive case for multi-dimensional access
template <size_t Size, typename... Sizes, size_t Idx, size_t... Rest>
struct _tp_list_total<tp_list<integral_constant<size_t, Size>, Sizes...>,
                      tp_indexs_list_t<Idx, Rest...>> {
    using __Rest = _tp_list_total<tp_list<Sizes...>, tp_indexs_list_t<Rest...>>;
    static constexpr size_t value = __Rest::value + Idx * __Rest::total;
    static constexpr size_t total = Size * __Rest::total;
};

/// @private Access a specific index in the variant index pack
template <typename PIndexs, size_t Idx, typename = void>
struct _tp_indexs_access {
    static constexpr size_t value =
        _tp_list_total<tp_front_t<PIndexs>, tp_at_t<tp_back_t<PIndexs>, Idx>>::value;
};

/// @private Specialization for out-of-bounds index access
template <typename PIndexs, size_t Idx>
struct _tp_indexs_access<PIndexs, Idx, std::enable_if_t<(Idx >= tp_size_v<PIndexs>)>> {
    static constexpr size_t value = _tp_indexs_access<PIndexs, Idx - 1>::value + 1;
};

/// @private Execute visitor with expanded index pack
template <size_t... Idxs, typename Callable, typename... Getter>
constexpr decltype(auto) _execute(tp_indexs_list_t<Idxs...>, Callable &&func, Getter &&...getter) {
    return std::forward<Callable>(func)(
        std::forward<Getter>(getter)(integral_constant<size_t, Idxs>{})...);
}

#define WJR_REGISTER_SWITCH_CASE(Idx)                                                              \
case _tp_indexs_access<PIndexs, Idx>::value: {                                                     \
    if constexpr ((Idx) < Size) {                                                                  \
        return _execute(tp_at_t<tp_back_t<PIndexs>, Idx>{}, std::forward<Callable>(func),          \
                        std::forward<Getter>(getter)...);                                          \
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
    WJR_REGISTER_SWITCH_CASE_64(Idx + 128)                                                         \
    WJR_REGISTER_SWITCH_CASE_64(Idx + 192)

#define WJR_REGISTER_SWITCH_DEFAULT                                                                \
default: {                                                                                         \
    WJR_UNREACHABLE();                                                                             \
}

#define WJR_REGISTER_SWITCH_VISITOR(SZ)                                                            \
    template <>                                                                                    \
    struct _siwtch_visitor_helper<SZ> {                                                            \
        template <typename PIndexs, typename Callable, typename... Getter>                         \
        WJR_INTRINSIC_CONSTEXPR static decltype(auto) visit(size_t index, PIndexs,                 \
                                                            Callable &&func, Getter &&...getter) { \
            constexpr size_t Size = tp_size_v<tp_back_t<PIndexs>>;                                 \
            switch (index) {                                                                       \
                WJR_PP_CONCAT(WJR_REGISTER_SWITCH_CASE_, SZ)                                       \
                (0) WJR_REGISTER_SWITCH_DEFAULT                                                    \
            }                                                                                      \
        }                                                                                          \
    }

/// @private Switch-based visitor helper for efficient variant visitation
template <size_t SZ>
struct _siwtch_visitor_helper;

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

/// @private Determine appropriate switch case size based on variant size
constexpr size_t _switch_visitor_SZ(size_t size) {
    return size <= 2 ? 2 : size <= 4 ? 4 : size <= 8 ? 8 : size <= 16 ? 16 ? size <= 64 : 64 : 256;
}

/// @private Result structure for multi-variant index calculation
struct __Index_result {
    size_t acc;
    size_t total;
};

/// @private Base case for index computation
template <template <typename> typename Accessor>
constexpr __Index_result _get_index() {
    return {0, 1};
}

/// @private Recursive index computation for multiple variants
template <template <typename> typename Accessor, typename T, typename... Getter>
constexpr decltype(auto) _get_index(const T &first, const Getter &...getter) {
    auto _result = _get_index<Accessor>(getter...);
    _result.acc += Accessor<T>::get_index(first) * _result.total;
    _result.total *= Accessor<T>::value;
    return _result;
}

/// @private Wrap accessor for delayed execution
template <template <typename> typename Accessor, typename T>
constexpr decltype(auto) _accessor_wrapper(T &&t) {
    return [_t = std::forward<T>(t)](auto ic) mutable {
        return Accessor<remove_cvref_t<T>>::run(std::forward<T>(_t), ic);
    };
}

/// @private Main visit implementation dispatcher
template <typename PIndexs, typename Callable, typename... Getter>
constexpr decltype(auto) _visit_impl(size_t index, PIndexs, Callable &&func, Getter &&...getter) {
    constexpr size_t SZ = visit_detail::_switch_visitor_SZ(tp_size_v<tp_back_t<PIndexs>>);
    return visit_detail::_siwtch_visitor_helper<SZ>::visit(
        index, PIndexs{}, std::forward<Callable>(func), std::forward<Getter>(getter)...);
}

} // namespace visit_detail

template <template <typename> typename Accessor, typename Callable, typename... Args>
constexpr decltype(auto) full_visit(Callable &&func, Args &&...args) {
    using Sizes = tp_list<integral_constant<size_t, Accessor<remove_cvref_t<Args>>::value>...>;
    using PIndexs = tp_list<
        Sizes,
        tp_product_t<tp_list,
                     tp_make_integers_list_t<size_t, Accessor<remove_cvref_t<Args>>::value>...>>;
    return visit_detail::_visit_impl(
        visit_detail::_get_index<Accessor>(args...).acc, PIndexs{}, std::forward<Callable>(func),
        visit_detail::_accessor_wrapper<Accessor>(std::forward<Args>(args))...);
}

template <template <typename> typename Accessor, typename Indexs, typename Callable,
          typename... Args>
constexpr decltype(auto) discrete_visit(Indexs, Callable &&func, Args &&...args) {
    using Sizes = tp_list<integral_constant<size_t, Accessor<remove_cvref_t<Args>>::value>...>;
    using PIndexs = tp_list<Sizes, Indexs>;
    return visit_detail::_visit_impl(
        visit_detail::_get_index<Accessor>(args...).acc, PIndexs{}, std::forward<Callable>(func),
        visit_detail::_accessor_wrapper<Accessor>(std::forward<Args>(args))...);
}

} // namespace wjr

#endif // WJR_VARIANT_HPP__