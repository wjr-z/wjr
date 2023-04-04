#pragma once
#ifndef __WJR_CRTP_H
#define __WJR_CRTP_H

#include <wjr/type_traits.h>

_WJR_BEGIN

// testing

// TODO ...

template<typename Derived>
class explicit_operator_complement {
public:
	static_assert(has_global_binary_operator_equal_to_v<const Derived&, const Derived&>
		&& has_global_binary_operator_less_v<const Derived&, const Derived&>, 
		"must have operator== and operator <");
	Derived& derived() { return static_cast<Derived&>(*this); }
	const Derived& derived() const { return static_cast<const Derived&>(*this); }
private:
	WJR_CONSTEXPR20 explicit_operator_complement() {}
	friend Derived;
};

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator==(
	const explicit_operator_complement<Derived>& lhs,
	const explicit_operator_complement<Derived>& rhs) {
	return (lhs.derived() == rhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator!=(
	const explicit_operator_complement<Derived>& lhs,
	const explicit_operator_complement<Derived>& rhs) {
	return !(lhs.derived() == rhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator<(
	const explicit_operator_complement<Derived>& lhs,
	const explicit_operator_complement<Derived>& rhs) {
	return (lhs.derived() < rhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator<=(
	const explicit_operator_complement<Derived>& lhs,
	const explicit_operator_complement<Derived>& rhs) {
	return !(rhs.derived() < lhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator>(
	const explicit_operator_complement<Derived>& lhs,
	const explicit_operator_complement<Derived>& rhs) {
	return (rhs.derived() < lhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator>=(
	const explicit_operator_complement<Derived>& lhs,
	const explicit_operator_complement<Derived>& rhs) {
	return !(lhs.derived() < rhs.derived());
}

_WJR_END

#endif // __WJR_CRTP_H