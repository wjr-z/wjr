#ifndef WJR_TYPE_TRAITS_HPP__
#define WJR_TYPE_TRAITS_HPP__

#include <functional>
#include <limits>
#include <type_traits>
#include <wjr/preprocessor.hpp>

namespace wjr {

template <typename... Ts>
struct make_void {
    typedef void type;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

template <bool B>
using bool_constant = std::integral_constant<bool, B>;

#if !defined(WJR_CPP_17)
template <typename Fn, typename... Args>
struct invoke_result {
    using type = std::result_of_t<Fn(Args...)>;
};
#else
template <typename Fn, typename... Args>
struct invoke_result : std::invoke_result<Fn, Args...> {};
#endif

template <typename Fn, typename... Args>
using invoke_result_t = typename invoke_result<Fn, Args...>::type;

template <bool f, typename T, typename U>
struct conditional {
    using type = T;
};

template <typename T, typename U>
struct conditional<false, T, U> {
    using type = U;
};

template <bool f, typename T, typename U>
using conditional_t = typename conditional<f, T, U>::type;

template <typename... Args>
struct multi_conditional;

template <typename... Args>
using multi_conditional_t = typename multi_conditional<Args...>::type;

template <bool f, typename T, typename... Args>
struct multi_conditional_impl {
    using type = T;
};

template <typename T, typename... Args>
struct multi_conditional_impl<false, T, Args...> {
    using type = multi_conditional_t<Args...>;
};

template <typename F, typename T, typename U>
struct multi_conditional<F, T, U> {
    using type = conditional_t<F::value, T, U>;
};

template <typename F, typename T, typename U, typename V, typename... Args>
struct multi_conditional<F, T, U, V, Args...> {
    using type =
        typename multi_conditional_impl<F::value, T, U, V, Args...>::type;
};

template <typename... Args>
struct conjunction_impl;

template <>
struct conjunction_impl<> : std::true_type {};

template <typename T>
struct conjunction_impl<T> : T {};

template <typename T, typename U>
struct conjunction_impl<T, U> : conditional_t<T::value, U, T> {};

template <typename T, typename U, typename V, typename... Args>
struct conjunction_impl<T, U, V, Args...>
    : conditional_t<T::value, conjunction_impl<U, V, Args...>, U> {};

template <typename... Args>
struct conjunction {
    using type = typename conjunction_impl<Args...>::type;
};

template <typename... Args>
using conjunction_t = typename conjunction<Args...>::type;

template <typename... Args>
struct disjunction_impl;

template <>
struct disjunction_impl<> : std::false_type {};

template <typename T>
struct disjunction_impl<T> : T {};

template <typename T, typename U>
struct disjunction_impl<T, U> : conditional_t<T::value, T, U> {};

template <typename T, typename U, typename V, typename... Args>
struct disjunction_impl<T, U, V, Args...>
    : conditional_t<T::value, T, disjunction_impl<U, V, Args...>> {};

template <typename... Args>
struct disjunction {
    using type = typename disjunction_impl<Args...>::type;
};

template <typename... Args>
using disjunction_t = typename disjunction<Args...>::type;

template <typename F>
struct negation {
    using type = bool_constant<!F::value>;
};

template <typename F>
using negation_t = typename negation<F>::type;

template <typename T>
using aligned_storage_for_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

template <typename T, typename... Args>
struct is_any_of : disjunction<std::is_same<T, Args>...> {};

template <typename T, T idx, T... indexs>
struct is_any_integral_of : disjunction<bool_constant<idx == indexs>...> {};

template <typename T>
using remove_ref_t = std::remove_reference_t<T>;

template <typename T>
using remove_cref_t = std::remove_const_t<remove_ref_t<T>>;

template <typename T>
using remove_cvref_t = std::remove_cv_t<remove_ref_t<T>>;

template <typename iter, typename = void>
struct is_contiguous_iterator_impl : std::is_pointer<iter> {};

template <typename iter>
struct is_contiguous_iterator_impl<iter, typename iter::is_contiguous_iterator>
    : std::true_type {};

#if defined(WJR_CPP_20)
template <typename iter>
struct is_contiguous_iterator
    : bool_constant<std::contiguous_iterator<iter> ||
                    is_contiguous_iterator_impl<iter>::value> {};
#else
template <typename iter>
struct is_contiguous_iterator : is_contiguous_iterator_impl<iter> {};
#endif

template <typename iter>
struct is_contiguous_iterator<std::reverse_iterator<iter>>
    : is_contiguous_iterator<iter> {};

template <size_t n>
struct __uint_selector {};

template <>
struct __uint_selector<8> {
    using type = std::uint8_t;
};
template <>
struct __uint_selector<16> {
    using type = std::uint16_t;
};
template <>
struct __uint_selector<32> {
    using type = std::uint32_t;
};
template <>
struct __uint_selector<64> {
    using type = std::uint64_t;
};

template <size_t n>
using uint_t = typename __uint_selector<n>::type;

template <size_t n>
using int_t = std::make_signed_t<uint_t<n>>;

using int8_t = int_t<8>;
using int16_t = int_t<16>;
using int32_t = int_t<32>;
using int64_t = int_t<64>;

using uint8_t = uint_t<8>;
using uint16_t = uint_t<16>;
using uint32_t = uint_t<32>;
using uint64_t = uint_t<64>;

using intptr_t = int_t<sizeof(void *) * 8>;
using uintptr_t = uint_t<sizeof(void *) * 8>;

using std_int8_t = std::int8_t;
using std_int16_t = std::int16_t;
using std_int32_t = std::int32_t;
using std_int64_t = std::int64_t;

using std_uint8_t = std::uint8_t;
using std_uint16_t = std::uint16_t;
using std_uint32_t = std::uint32_t;
using std_uint64_t = std::uint64_t;

using std_intptr_t = std::intptr_t;
using std_uintptr_t = std::uintptr_t;

template <size_t n, bool __s>
using usint_t = std::conditional_t<__s, int_t<n>, uint_t<n>>;

template <typename T>
struct is_unsigned_integral
    : conjunction<std::is_integral<T>, std::is_unsigned<T>> {};

template <typename T>
struct is_signed_integral
    : conjunction<std::is_integral<T>, std::is_signed<T>> {};

class __is_little_endian_helper {
    constexpr static std::uint32_t u4 = 1;
    constexpr static std::uint8_t u1 = static_cast<const std::uint8_t &>(u4);

public:
    constexpr static bool value = u1 != 0;
};

// constexpr endian
enum class endian {
    little = 0,
    big = 1,
    native = __is_little_endian_helper::value ? little : big
};

// type identity
template <typename T>
struct type_identity {
    using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

} // namespace wjr

#endif // ! WJR_TYPE_TRAITS_HPP__