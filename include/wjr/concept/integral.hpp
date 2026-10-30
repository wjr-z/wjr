#ifndef WJR_CONCEPT_INTEGRAL_HPP__
#define WJR_CONCEPT_INTEGRAL_HPP__

#include <concepts>

namespace wjr {

/// @brief Match is_nonbool_integral, including its existing cv-qualified domain.
template <typename T>
concept nonbool_integral = std::integral<T> && !std::same_as<T, bool>;

/// @brief Match is_nonbool_unsigned_integral without changing bool or cv admission.
template <typename T>
concept nonbool_unsigned_integral = nonbool_integral<T> && std::unsigned_integral<T>;

/// @brief Match is_nonbool_signed_integral without changing its admitted types.
template <typename T>
concept nonbool_signed_integral = nonbool_integral<T> && std::signed_integral<T>;

} // namespace wjr

#endif // WJR_CONCEPT_INTEGRAL_HPP__
