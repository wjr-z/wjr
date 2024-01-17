#ifndef WJR_TYPE_TRAITS_HPP__
#define WJR_TYPE_TRAITS_HPP__

#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <type_traits>

#include <wjr/preprocessor.hpp>

namespace wjr {

struct empty {};

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
    : std::bool_constant<std::contiguous_iterator<iter> ||
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

using int8_t = int_t<8>;
using int16_t = int_t<16>;
using int32_t = int_t<32>;
using int64_t = int_t<64>;
#if WJR_HAS_FEATURE(INT128)
using int128_t = int_t<128>;
#endif

using uint8_t = uint_t<8>;
using uint16_t = uint_t<16>;
using uint32_t = uint_t<32>;
using uint64_t = uint_t<64>;
#if WJR_HAS_FEATURE(INT128)
using uint128_t = uint_t<128>;
#endif

using intptr_t = std::intptr_t;
using uintptr_t = std::uintptr_t;

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
struct is_unsigned_integral : std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {
};

template <typename T>
inline constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

template <typename T>
struct is_signed_integral : std::conjunction<std::is_integral<T>, std::is_signed<T>> {};

template <typename T>
inline constexpr bool is_signed_integral_v = is_signed_integral<T>::value;

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

WJR_INTRINSIC_CONSTEXPR bool is_constant_evaluated() noexcept {
    return WJR_IS_CONSTANT_EVALUATED();
}

template <typename T, typename U, typename = void>
struct __is_swappable_with : std::false_type {};

template <typename T, typename U>
struct __is_swappable_with<
    T, U, std::void_t<decltype(std::swap(std::declval<T>(), std::declval<U>()))>>
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
    : std::bool_constant<noexcept(std::swap(std::declval<T>(), std::declval<U>())) &&
                         noexcept(std::swap(std::declval<U>(), std::declval<T>()))> {};

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
struct unrefwrap {
    using type = T;
};

template <typename T>
struct unrefwrap<std::reference_wrapper<T>> {
    using type = T &;
};

template <typename T>
using unrefwrap_t = typename unrefwrap<T>::type;

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

WJR_INTRINSIC_CONSTEXPR size_t abs_cast(size_t n) {
    constexpr auto nd = std::numeric_limits<size_t>::digits;
    WJR_ASSUME((n >> (nd - 1)) == 0);
    return n;
}

#if WJR_CPP_20
template <typename T>
using make_template_constant_t = T;
#else
template <typename T>
using make_template_constant_t = std::add_lvalue_reference_t<std::add_const_t<T>>;
#endif

} // namespace wjr

#endif // ! WJR_TYPE_TRAITS_HPP__