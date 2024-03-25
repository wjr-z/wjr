#ifndef WJR_TYPE_TRAITS_HPP__
#define WJR_TYPE_TRAITS_HPP__

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

#include <wjr/preprocessor.hpp>

namespace wjr {

struct in_place_empty_t {};

inline constexpr in_place_empty_t in_place_empty = {};

struct in_place_default_construct_t {};

inline constexpr in_place_default_construct_t in_place_default_construct = {};

struct in_place_value_construct_t {};

inline constexpr in_place_value_construct_t in_place_value_construct = {};

struct in_place_reallocate_t {};

inline constexpr in_place_reallocate_t in_place_reallocate = {};

struct in_place_max_t {
    template <typename T>
    WJR_CONST constexpr operator T() const {
        return std::numeric_limits<T>::max();
    }
};

inline constexpr in_place_max_t in_place_max = {};

struct in_place_min_t {
    template <typename T>
    WJR_CONST constexpr operator T() const {
        return std::numeric_limits<T>::min();
    }
};

inline constexpr in_place_min_t in_place_min = {};

inline constexpr std::size_t dynamic_extent = in_place_max;

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
    using type = std::conditional_t<F::value, T, U>;
};

template <typename F, typename T, typename U, typename V, typename... Args>
struct multi_conditional<F, T, U, V, Args...> {
    using type = typename multi_conditional_impl<F::value, T, U, V, Args...>::type;
};

template <typename T, typename... Args>
struct is_any_of : std::disjunction<std::is_same<T, Args>...> {};

template <typename T, typename... Args>
inline constexpr bool is_any_of_v = is_any_of<T, Args...>::value;

template <typename T>
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

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
struct __int_selector {
    using type = std::make_signed_t<typename __uint_selector<n>::type>;
};

#if WJR_HAS_FEATURE(INT128)
template <>
struct __uint_selector<128> {
    using type = __uint128_t;
};

template <>
struct __int_selector<128> {
    using type = __int128_t;
};
#endif

template <size_t n>
using uint_t = typename __uint_selector<n>::type;

template <size_t n>
using int_t = typename __int_selector<n>::type;

#if WJR_HAS_FEATURE(INT128)
using int128_t = int_t<128>;
#endif

#if WJR_HAS_FEATURE(INT128)
using uint128_t = uint_t<128>;
#endif

template <size_t n, bool __s>
using usint_t = std::conditional_t<__s, int_t<n>, uint_t<n>>;

using ssize_t = std::make_signed_t<size_t>;

template <typename T>
struct is_unsigned_integral : std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {
};

template <typename T>
inline constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

template <typename T>
struct is_signed_integral : std::conjunction<std::is_integral<T>, std::is_signed<T>> {};

template <typename T>
inline constexpr bool is_signed_integral_v = is_signed_integral<T>::value;

// type identity
template <typename T>
struct type_identity {
    using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

/**
 * @brief Return if is constant evaluated.
 *
 * @details Use macro WJR_IS_CONSTANT_EVALUATED(). \n
 * Use std::is_constant_evaluated() if C++ 20 is supported. \n
 * Otherwise, use __builtin_constant_evaluated() if
 * WJR_HAS_BUILTIN(__builtin_is_constant_evaluated). Otherwise, return false.
 *
 */
WJR_INTRINSIC_CONSTEXPR bool is_constant_evaluated() noexcept {
    return WJR_IS_CONSTANT_EVALUATED();
}

template <typename T, typename U, typename = void>
struct __is_swappable_with : std::false_type {};

template <typename T, typename U>
struct __is_swappable_with<
    T, U, std::void_t<decltype(std::swap(std::declval<T &>(), std::declval<U &>()))>>
    : std::true_type {};

template <typename T, typename U>
struct is_swappable_with
    : std::conjunction<__is_swappable_with<T, U>, __is_swappable_with<U, T>> {};

template <typename T, typename U>
inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

template <typename T>
struct is_swappable
    : is_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>> {
};

template <typename T>
inline constexpr bool is_swappable_v = is_swappable<T>::value;

template <typename T, typename U>
struct __is_nothrow_swappable_with
    : std::bool_constant<noexcept(std::swap(std::declval<T &>(), std::declval<U &>())) &&
                         noexcept(std::swap(std::declval<U &>(), std::declval<T &>()))> {
};

template <typename T, typename U>
struct is_nothrow_swappable_with
    : std::conjunction<is_swappable_with<T, U>, __is_nothrow_swappable_with<T, U>> {};

template <typename T, typename U>
inline constexpr bool is_nothrow_swappable_with_v =
    is_nothrow_swappable_with<T, U>::value;

template <typename T>
struct is_nothrow_swappable : is_nothrow_swappable_with<std::add_lvalue_reference_t<T>,
                                                        std::add_lvalue_reference_t<T>> {
};

template <typename T>
inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

template <typename T>
struct unref_wrapper {
    using type = T;
};

template <typename T>
struct unref_wrapper<std::reference_wrapper<T>> {
    using type = T &;
};

template <typename T>
using unref_wrapper_t = typename unref_wrapper<T>::type;

template <typename T, typename = void>
struct __is_default_convertible : std::false_type {};

template <typename T>
void __test_default_convertible(const T &);

template <typename T>
struct __is_default_convertible<T,
                                std::void_t<decltype(__test_default_convertible<T>({}))>>
    : std::true_type {};

template <typename T>
using is_default_convertible = __is_default_convertible<T>;

template <typename T>
inline constexpr bool is_default_convertible_v = is_default_convertible<T>::value;

template <typename T>
struct get_place_index {};

template <size_t idx>
struct get_place_index<std::in_place_index_t<idx>> {
    static constexpr size_t value = idx;
};

template <typename T>
inline constexpr size_t get_place_index_v = get_place_index<T>::value;

// ....

template <class P, class M>
WJR_INTRINSIC_CONSTEXPR20 size_t container_of_offset(const M P::*member) {
    return reinterpret_cast<size_t>(&(reinterpret_cast<P *>(nullptr)->*member));
}

template <class P, class M>
WJR_INTRINSIC_CONSTEXPR20 P *container_of_offset_impl(M *ptr, const M P::*member) {
    return reinterpret_cast<P *>(reinterpret_cast<char *>(ptr) -
                                 container_of_offset(member));
}

#define WJR_CONTAINER_OF(ptr, type, member) container_of_offset_impl(ptr, &type::member)

// C++ 17 concept adapt

template <typename Derived, typename Base>
struct is_derived_from
    : std::conjunction<
          std::is_base_of<Base, Derived>,
          std::is_convertible<const volatile Derived *, const volatile Base *>> {};

template <typename Derived, typename Base>
inline constexpr bool is_derived_from_v = is_derived_from<Derived, Base>::Value;

template <typename From, typename To, typename = void>
struct __is_convertible_to_helper : std::false_type {};

template <typename From, typename To>
struct __is_convertible_to_helper<
    From, To, std::void_t<decltype(static_cast<To>(std::declval<From>()))>>
    : std::true_type {};

template <typename From, typename To>
struct is_convertible_to : std::conjunction<std::is_convertible<From, To>,
                                            __is_convertible_to_helper<From, To, void>> {
};

template <typename From, typename To>
inline constexpr bool is_convertible_to_v = is_convertible_to<From, To>::value;

template <typename T>
using iter_reference_t = decltype(*std::declval<T &>());

template <typename iter, typename = void>
struct is_contiguous_iterator_impl
    : std::disjunction<std::is_pointer<iter>, std::is_array<iter>> {};

template <typename iter>
struct is_contiguous_iterator_impl<iter, typename iter::is_contiguous_iterator>
    : std::true_type {};

#if defined(WJR_CPP_20)
template <typename iter>
struct is_contiguous_iterator
    : std::bool_constant<std::contiguous_iterator<iter> ||
                         is_contiguous_iterator_impl<iter>::value> {};
#else
template <typename iter>
struct is_contiguous_iterator : is_contiguous_iterator_impl<iter> {};
#endif

template <typename iter>
inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<iter>::value;

template <typename T, typename = void>
struct __is_iterator_helper : std::false_type {};

template <typename T>
struct __is_iterator_helper<
    T, std::void_t<typename std::iterator_traits<T>::iterator_category>>
    : std::true_type {};

template <typename T>
struct is_iterator : __is_iterator_helper<T> {};

template <typename T>
inline constexpr bool is_iterator_v = is_iterator<T>::value;

// TODO : move __is_in_i32_range to other header.
WJR_INTRINSIC_CONSTEXPR bool __is_in_i32_range(int64_t value) noexcept {
    return value >= (int32_t)in_place_min && value <= (int32_t)in_place_max;
}

#define __WJR_REGISTER_TYPENAMES_EXPAND(x) __WJR_REGISTER_TYPENAMES_EXPAND_I x
#define __WJR_REGISTER_TYPENAMES_EXPAND_I(...) __VA_ARGS__

#define __WJR_REGISTER_TYPENAMES(...)                                                    \
    __WJR_REGISTER_TYPENAMES_EXPAND(                                                     \
        WJR_PP_QUEUE_TRANSFORM((__VA_ARGS__), __WJR_REGISTER_TYPENAMES_CALLER))
#define __WJR_REGISTER_TYPENAMES_CALLER(x) typename x

#define WJR_REGISTER_HAS_TYPE_0(NAME, HAS_EXPR)                                          \
    template <typename Enable, typename... Args>                                         \
    struct __has_##NAME : std::false_type {};                                            \
    template <typename... Args>                                                          \
    struct __has_##NAME<std::void_t<decltype(HAS_EXPR)>, Args...> : std::true_type {};   \
    template <typename... Args>                                                          \
    struct has_##NAME : __has_##NAME<void, Args...> {};                                  \
    template <typename... Args>                                                          \
    constexpr bool has_##NAME##_v = has_##NAME<Args...>::value

#define WJR_REGISTER_HAS_TYPE_MORE(NAME, HAS_EXPR, ...)                                  \
    template <typename Enable, __WJR_REGISTER_TYPENAMES(__VA_ARGS__), typename... Args>  \
    struct __has_##NAME : std::false_type {};                                            \
    template <__WJR_REGISTER_TYPENAMES(__VA_ARGS__), typename... Args>                   \
    struct __has_##NAME<std::void_t<decltype(HAS_EXPR)>, __VA_ARGS__, Args...>           \
        : std::true_type {};                                                             \
    template <__WJR_REGISTER_TYPENAMES(__VA_ARGS__), typename... Args>                   \
    struct has_##NAME : __has_##NAME<void, __VA_ARGS__, Args...> {};                     \
    template <__WJR_REGISTER_TYPENAMES(__VA_ARGS__), typename... Args>                   \
    constexpr bool has_##NAME##_v = has_##NAME<__VA_ARGS__, Args...>::value

#define WJR_REGISTER_HAS_TYPE(NAME, ...)                                                 \
    WJR_REGISTER_HAS_TYPE_N(WJR_PP_ARGS_LEN(__VA_ARGS__), NAME, __VA_ARGS__)
#define WJR_REGISTER_HAS_TYPE_N(N, ...)                                                  \
    WJR_PP_BOOL_IF(WJR_PP_EQ(N, 1), WJR_REGISTER_HAS_TYPE_0, WJR_REGISTER_HAS_TYPE_MORE) \
    (__VA_ARGS__)

// used for SFINAE
constexpr static void allow_true_type(std::true_type) noexcept {}
constexpr static void allow_false_type(std::false_type) noexcept {}

} // namespace wjr

#endif // ! WJR_TYPE_TRAITS_HPP__