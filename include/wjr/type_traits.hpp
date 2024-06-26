#ifndef WJR_TYPE_TRAITS_HPP__
#define WJR_TYPE_TRAITS_HPP__

#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

#include <wjr/preprocessor.hpp>

namespace wjr {

struct in_place_empty_t {};

inline constexpr in_place_empty_t in_place_empty = {};

/**
 * @brief Tag of default constructor.
 *
 * @details Use dctor to indicate default constructor. \n
 * Used to avoid value initialization.  \n
 * For example : \n
 * @code
 * wjr::vector<int> vec(10, dctor); // default construct with 10 elements.
 * wjr::vector<int> vec2(10); // value construct with 10 elements.
 * wjr::vector<int> vec3(10, 0); // value construct with 10 elements.
 * wjr::vector<int> vec4(10, vctor); // value construct with 10 elements.
 * @endcode
 * elements of vec are not initialized. \n
 * elements of vec2, vec3, vec4 are initialized with 0.
 */
struct dctor_t {};

/**
 * @see dctor_t
 */
inline constexpr dctor_t dctor = {};

/**
 * @brief Tag of value constructor.
 *
 */
struct vctor_t {};

inline constexpr vctor_t vctor = {};

struct in_place_reallocate_t {};

inline constexpr in_place_reallocate_t in_place_reallocate = {};

struct in_place_reserve_t {};

inline constexpr in_place_reserve_t in_place_reserve = {};

struct in_place_move_t {};

inline constexpr in_place_move_t in_place_move = {};

struct in_place_max_t {
    template <typename T>
    WJR_CONST constexpr operator T() const noexcept {
        return std::numeric_limits<T>::max();
    }
};

inline constexpr in_place_max_t in_place_max = {};

struct in_place_min_t {
    template <typename T>
    WJR_CONST constexpr operator T() const noexcept {
        return std::numeric_limits<T>::min();
    }
};

inline constexpr in_place_min_t in_place_min = {};

struct self_init_t {};

inline constexpr self_init_t self_init = {};

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

/// @private
template <size_t n>
struct __uint_selector {};

/// @private
template <>
struct __uint_selector<8> {
    using type = std::uint8_t;
};

/// @private
template <>
struct __uint_selector<16> {
    using type = std::uint16_t;
};

/// @private
template <>
struct __uint_selector<32> {
    using type = std::uint32_t;
};

/// @private
template <>
struct __uint_selector<64> {
    using type = std::uint64_t;
};

/// @private
template <size_t n>
struct __int_selector {
    using type = std::make_signed_t<typename __uint_selector<n>::type>;
};

#if WJR_HAS_FEATURE(INT128)
/// @private
template <>
struct __uint_selector<128> {
    using type = __uint128_t;
};

/// @private
template <>
struct __int_selector<128> {
    using type = __int128_t;
};
#endif

template <size_t n>
using uint_t = typename __uint_selector<n>::type;

template <size_t n>
using int_t = typename __int_selector<n>::type;

template <size_t n, bool __s>
using usint_t = std::conditional_t<__s, int_t<n>, uint_t<n>>;

using ssize_t = std::make_signed_t<size_t>;

template <typename T>
struct is_nonbool_integral
    : std::conjunction<std::is_integral<T>, std::negation<std::is_same<T, bool>>> {};

template <typename T>
inline constexpr bool is_nonbool_integral_v = is_nonbool_integral<T>::value;

template <typename T>
struct is_unsigned_integral : std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {
};

template <typename T>
inline constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

template <typename T>
struct is_signed_integral : std::conjunction<std::is_integral<T>, std::is_signed<T>> {};

template <typename T>
inline constexpr bool is_signed_integral_v = is_signed_integral<T>::value;

template <typename T>
struct is_nonbool_unsigned_integral
    : std::conjunction<is_unsigned_integral<T>, std::negation<std::is_same<T, bool>>> {};

template <typename T>
inline constexpr bool is_nonbool_unsigned_integral_v =
    is_nonbool_unsigned_integral<T>::value;

template <typename T>
struct is_nonbool_signed_integral
    : std::conjunction<is_signed_integral<T>, std::negation<std::is_same<T, bool>>> {};

template <typename T>
inline constexpr bool is_nonbool_signed_integral_v = is_nonbool_signed_integral<T>::value;

// type identity
template <typename T>
struct type_identity {
    using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

template <typename T>
struct add_restrict {
    using type = T;
};

template <typename T>
struct add_restrict<T *> {
    using type = T *WJR_RESTRICT;
};

template <typename T>
using add_restrict_t = typename add_restrict<T>::type;

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

/// @private
template <typename T>
struct __unref_wrapper_helper {
    using type = T;
};

/// @private
template <typename T>
struct __unref_wrapper_helper<std::reference_wrapper<T>> {
    using type = T &;
};

template <typename T>
struct unref_wrapper {
    using type = typename __unref_wrapper_helper<std::decay_t<T>>::type;
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
WJR_INTRINSIC_CONSTEXPR20 size_t container_of_offset(const M P::*member) noexcept {
    return reinterpret_cast<size_t>(&(reinterpret_cast<P *>(nullptr)->*member));
}

template <class P, class M>
WJR_INTRINSIC_CONSTEXPR20 P *container_of_offset_impl(M *ptr,
                                                      const M P::*member) noexcept {
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

/// @private
template <typename From, typename To, typename = void>
struct __is_convertible_to_helper : std::false_type {};

/// @private
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

template <typename Value, WJR_REQUIRES(is_nonbool_integral_v<Value>)>
WJR_CONST constexpr std::make_signed_t<Value> to_signed(Value value) noexcept {
    return static_cast<std::make_signed_t<Value>>(value);
}

template <typename Value, WJR_REQUIRES(is_nonbool_integral_v<Value>)>
WJR_CONST constexpr std::make_unsigned_t<Value> to_unsigned(Value value) noexcept {
    return static_cast<std::make_unsigned_t<Value>>(value);
}

template <class T, class U>
WJR_CONST constexpr bool cmp_equal(T t, U u) noexcept {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
        return t == u;
    } else if constexpr (std::is_signed_v<T>) {
        return t >= 0 && to_unsigned(t) == u;
    } else {
        return u >= 0 && to_unsigned(u) == t;
    }
}

template <class T, class U>
WJR_CONST constexpr bool cmp_not_equal(T t, U u) noexcept {
    return !cmp_equal(t, u);
}

template <class T, class U>
WJR_CONST constexpr bool cmp_less(T t, U u) noexcept {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
        return t < u;
    } else if constexpr (std::is_signed_v<T>) {
        return t < 0 || to_unsigned(t) < u;
    } else {
        return u >= 0 && t < to_unsigned(u);
    }
}

template <class T, class U>
WJR_CONST constexpr bool cmp_greater(T t, U u) noexcept {
    return cmp_less(u, t);
}

template <class T, class U>
WJR_CONST constexpr bool cmp_less_equal(T t, U u) noexcept {
    return !cmp_less(u, t);
}

template <class T, class U>
WJR_CONST constexpr bool cmp_greater_equal(T t, U u) noexcept {
    return !cmp_less(t, u);
}

template <typename T, typename U>
WJR_CONST constexpr bool in_range(U value) noexcept {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
        if constexpr (std::is_signed_v<T>) {
            return value >= std::numeric_limits<T>::min() &&
                   value <= std::numeric_limits<T>::max();
        } else {
            return value <= std::numeric_limits<T>::max();
        }
    } else if constexpr (std::is_signed_v<T>) {
        return value <= to_unsigned(std::numeric_limits<T>::max());
    } else {
        return value >= 0 && to_unsigned(value) <= std::numeric_limits<T>::max();
    }
}

template <typename T, typename U,
          WJR_REQUIRES(std::is_integral_v<T> &&std::is_integral_v<U>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T fast_cast(U value) noexcept {
    WJR_ASSERT_ASSUME_L2(in_range<T>(value));
    return static_cast<T>(value);
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

WJR_REGISTER_HAS_TYPE(compare, std::declval<Comp>()(std::declval<T>(), std::declval<U>()),
                      Comp, T, U);
WJR_REGISTER_HAS_TYPE(
    noexcept_compare,
    allow_true_type(std::declval<std::bool_constant<noexcept(
                        std::declval<Comp>()(std::declval<T>(), std::declval<U>()))>>()),
    Comp, T, U);

#define WJR_REGISTER_HAS_COMPARE(NAME, STD)                                              \
    template <typename T, typename U>                                                    \
    struct has_##NAME : has_compare<STD, T, U> {};                                       \
    template <typename T, typename U>                                                    \
    inline constexpr bool has_##NAME##_v = has_##NAME<T, U>::value;                      \
    template <typename T, typename U>                                                    \
    struct has_noexcept_##NAME : has_noexcept_compare<STD, T, U> {};                     \
    template <typename T, typename U>                                                    \
    inline constexpr bool has_noexcept_##NAME##_v = has_noexcept_##NAME<T, U>::value;

WJR_REGISTER_HAS_COMPARE(equal_to, std::equal_to<>);
WJR_REGISTER_HAS_COMPARE(not_equal_to, std::not_equal_to<>);
WJR_REGISTER_HAS_COMPARE(less, std::less<>);
WJR_REGISTER_HAS_COMPARE(less_equal, std::less_equal<>);
WJR_REGISTER_HAS_COMPARE(greater, std::greater<>);
WJR_REGISTER_HAS_COMPARE(greater_equal, std::greater_equal<>);

#undef WJR_REGISTER_HAS_COMPARE

WJR_REGISTER_HAS_TYPE(invocable,
                      std::invoke(std::declval<Func>(), std::declval<Args>()...), Func);

} // namespace wjr

#endif // ! WJR_TYPE_TRAITS_HPP__