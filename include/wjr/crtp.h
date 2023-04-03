#pragma once
#ifndef __WJR_CRTP_H
#define __WJR_CRTP_H

#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename Derived>
class operator_complement {
public:
	Derived& derived() { return static_cast<Derived&>(*this); }
	const Derived& derived() const { return static_cast<const Derived&>(*this); }
private:
	WJR_CONSTEXPR20 operator_complement() {}
	friend Derived;
};

template<typename Derived>
WJR_INLINE_CONSTEXPR20 bool operator==(
	const operator_complement<Derived>& lhs,
	const operator_complement<Derived>& rhs) {
	return (lhs.derived() == rhs.derived());
}

template<typename Derived>
WJR_INLINE_CONSTEXPR20 bool operator!=(
	const operator_complement<Derived>& lhs,
	const operator_complement<Derived>& rhs) {
	return !(lhs.derived() == rhs.derived());
}

template<typename Derived>
WJR_INLINE_CONSTEXPR20 bool operator<(
	const operator_complement<Derived>& lhs,
	const operator_complement<Derived>& rhs) {
	return (lhs.derived() < rhs.derived());
}

template<typename Derived>
WJR_INLINE_CONSTEXPR20 bool operator<=(
	const operator_complement<Derived>& lhs,
	const operator_complement<Derived>& rhs) {
	return !(rhs.derived() < lhs.derived());
}

template<typename Derived>
WJR_INLINE_CONSTEXPR20 bool operator>(
	const operator_complement<Derived>& lhs,
	const operator_complement<Derived>& rhs) {
	return (rhs.derived() < lhs.derived());
}

template<typename Derived>
WJR_INLINE_CONSTEXPR20 bool operator>=(
	const operator_complement<Derived>& lhs,
	const operator_complement<Derived>& rhs) {
	return !(lhs.derived() < rhs.derived());
}

_WJR_END

#endif // __WJR_CRTP_H