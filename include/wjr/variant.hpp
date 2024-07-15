#ifndef WJR_VARIANT_HPP__
#define WJR_VARIANT_HPP__

#include <variant>

#include <wjr/preprocessor.hpp>

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
std::variant<Args...> &__is_as_variant_impl(std::variant<Args...> &);
template <typename... Args>
const std::variant<Args...> &__is_as_variant_impl(const std::variant<Args...> &);
template <typename... Args>
std::variant<Args...> &&__is_as_variant_impl(std::variant<Args...> &&);
template <typename... Args>
const std::variant<Args...> &&__is_as_variant_impl(const std::variant<Args...> &&);

template <typename T, typename Enable>
struct __is_as_variant_helper : std::false_type {};

template <typename T>
struct __is_as_variant_helper<
    T, std::void_t<decltype(__is_as_variant_impl(std::declval<T>()))>> : std::true_type {
};

template <typename T>
struct is_as_variant : __is_as_variant_helper<T, void> {};

template <typename T>
inline constexpr bool is_as_variant_v = is_as_variant<T>::value;

template <typename Callable, typename... Variants,
          WJR_REQUIRES(std::conjunction_v<is_as_variant<Variants>...>)>
constexpr decltype(auto) visit(Callable &&obj, Variants &&...vars) {
    return std::visit(std::forward<Callable>(obj), std::forward<Variants>(vars)...);
}

template <typename Var, typename... Args, WJR_REQUIRES(is_as_variant_v<Var>)>
constexpr decltype(auto) match(Var &&var, Args &&...args) {
    return std::visit(variant_detail::overloaded{std::forward<Args>(args)...},
                      std::forward<Var>(var));
}

} // namespace wjr

#endif // WJR_VARIANT_HPP__