/**
 * @file result.hpp
 * @author wjr
 * @details 
 * @version 0.1
 * @date 2024-07-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WJR_RESULT_HPP__
#define WJR_RESULT_HPP__

#include <wjr/tuple.hpp>
#include <wjr/type_traits.hpp>
#include <wjr/variant.hpp>

namespace wjr {

template <typename... Args>
class result_ok : public tuple<Args...> {
    using Mybase = tuple<Args...>;

public:
    using Mybase::Mybase;
};

template <typename... Args>
result_ok(Args...) -> result_ok<Args...>;

template <typename... Args>
class result_error : public tuple<Args...> {
    using Mybase = tuple<Args...>;

public:
    using Mybase::Mybase;
};

template <typename... Args>
result_error(Args...) -> result_error<Args...>;

template <typename T, typename Error>
class result;

template <typename T, typename Error>
class result : capture_leaf<std::variant<T, Error>, result<T, Error>> {
    using Variant = std::variant<T, Error>;
    using Mybase = capture_leaf<Variant, result<T, Error>>;

public:
    using Mybase::Mybase;

    template <typename U, WJR_REQUIRES(std::is_convertible_v<U &&, T>)>
    constexpr result(U &&other) noexcept(
        std::is_nothrow_constructible_v<Mybase, std::in_place_index_t<0>, U &&>)
        : Mybase(std::in_place_index<0>, std::forward<U>(other)) {}

private:
    template <size_t idx, typename Tp, size_t... Idxs>
    constexpr result(
        std::in_place_index_t<idx> ii, Tp &tp,
        std::index_sequence<Idxs...>) noexcept(std::
                                                   is_nothrow_constructible_v<
                                                       Mybase, std::in_place_index_t<idx>,
                                                       decltype(std::get<Idxs>(tp))...>)
        : Mybase(ii, std::get<Idxs>(tp)...) {}

public:
    template <typename... Args>
    constexpr result(result_ok<Args...> other) noexcept(noexcept(
        result(std::in_place_index<0>, other, std::index_sequence_for<Args...>())))
        : result(std::in_place_index<0>, other, std::index_sequence_for<Args...>()) {}

    template <typename... Args>
    constexpr result(result_error<Args...> other) noexcept(noexcept(
        result(std::in_place_index<1>, other, std::index_sequence_for<Args...>())))
        : result(std::in_place_index<1>, other, std::index_sequence_for<Args...>()) {}

    constexpr bool is_ok() const noexcept { return Mybase::get().index() == 0; }
    constexpr bool is_error() const noexcept { return Mybase::get().index() == 1; }
};

} // namespace wjr

#endif // WJR_RESULT_HPP__