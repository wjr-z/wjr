#pragma once
#ifndef __WJR_PAIR_H
#define __WJR_PAIR_H

#include <wjr/type_traits.h>

_WJR_BEGIN

// using empty base optimization

template<size_t index, typename T>
struct _Pair_wrapper1 {
    template<typename _Ty = T, std::enable_if_t<std::is_default_constructible_v<_Ty>, int> = 0>
    constexpr _Pair_wrapper1() noexcept(std::is_nothrow_default_constructible_v<_Ty>)
        : val() {}
    template<typename _Ty = T, std::enable_if_t<std::is_copy_constructible_v<_Ty>, int> = 0>
    constexpr _Pair_wrapper1(const _Ty& other) noexcept(std::is_nothrow_copy_constructible_v<_Ty>)
        : val(other) {}
    template<typename _Ty = T, std::enable_if_t<std::is_move_constructible_v<_Ty>, int> = 0>
    constexpr _Pair_wrapper1(_Ty&& other) noexcept(std::is_nothrow_move_constructible_v<_Ty>)
        : val(std::forward<_Ty>(other)) {}
    template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
    constexpr _Pair_wrapper1(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : val(std::forward<Args>(args)...) {}
    _Pair_wrapper1(const _Pair_wrapper1&) = default;
    _Pair_wrapper1(_Pair_wrapper1&&) noexcept = default;
    _Pair_wrapper1& operator=(const _Pair_wrapper1&) = default;
	_Pair_wrapper1& operator=(_Pair_wrapper1&&) noexcept = default;
    constexpr T& value() noexcept { return val; }
    constexpr const T& value() const noexcept { return val; }
private:
    T val;
};

template<size_t index, typename T>
struct _Pair_wrapper2 : private T{
    using _Mybase = T;
    template<typename _Ty = T, std::enable_if_t<std::is_default_constructible_v<_Ty>, int> = 0>
    constexpr _Pair_wrapper2() noexcept(std::is_nothrow_default_constructible_v<_Ty>)
        : _Mybase() {}
    template<typename _Ty = T, std::enable_if_t<std::is_copy_constructible_v<_Ty>, int> = 0>
    constexpr _Pair_wrapper2(const _Ty& other) noexcept(std::is_nothrow_copy_constructible_v<_Ty>)
        : _Mybase(other) {}
    template<typename _Ty = T, std::enable_if_t<std::is_move_constructible_v<_Ty>, int> = 0>
    constexpr _Pair_wrapper2(_Ty&& other) noexcept(std::is_nothrow_move_constructible_v<_Ty>)
        : _Mybase(std::forward<_Ty>(other)) {}
    template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
    constexpr _Pair_wrapper2(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : _Mybase(std::forward<Args>(args)...) {}
    _Pair_wrapper2(const _Pair_wrapper2&) = default;
    _Pair_wrapper2(_Pair_wrapper2&&) noexcept= default;
	_Pair_wrapper2& operator=(const _Pair_wrapper2&) = default;
	_Pair_wrapper2& operator=(_Pair_wrapper2&&) noexcept = default;
    constexpr T& value() noexcept { return *this; }
    constexpr const T& value() const noexcept { return *this; }
};

template<size_t index, typename T>
using _Pair_wrapper = std::conditional_t<
    std::conjunction_v<std::is_class<T>, std::is_empty<T>, std::negation<std::is_final<T>>>,
    _Pair_wrapper2<index, T>,
    _Pair_wrapper1<index, T>
>;

template<typename T, typename U>
class pair : private _Pair_wrapper<0, T>, private _Pair_wrapper<1, U> {

    template<typename _Ty, typename _Uy>
    using _Is_default_constructible = std::conjunction <
        std::is_default_constructible<_Ty>,
        std::is_default_constructible<_Uy>>;
    template<typename _Ty, typename _Uy>
	using _Is_default_convertible = std::conjunction <
		is_default_convertible<_Ty>,
		is_default_convertible<_Uy>>;

    template<typename _Ty, typename _Uy>
	using _Is_copy_constructible = std::conjunction <
		std::is_copy_constructible<_Ty>,
		std::is_copy_constructible<_Uy>>;

	template<typename _Ty, typename _Uy, typename _Vty, typename _Wuy>
	using _Is_convertible = std::conjunction <
        std::is_convertible<_Vty, _Ty>,
		std::is_convertible<_Wuy, _Uy>>;
		
    template<typename _Ty, typename _Uy, typename _Vty, typename _Wuy>
    using _Is_constructible = std::conjunction<
        std::is_constructible<_Ty, _Vty>,
        std::is_constructible<_Uy, _Wuy>>;
    
public:
    using _Mybase1 = _Pair_wrapper<0, T>;
    using _Mybase2 = _Pair_wrapper<1, U>;

    using first_type = T;
	using second_type = U;

    template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
		std::conjunction_v<_Is_default_constructible<_Ty, _Uy>,
        _Is_default_convertible<_Ty, _Uy>>, bool> = true>
	constexpr pair() noexcept(std::conjunction_v<std::is_nothrow_default_constructible<_Ty>,
		std::is_nothrow_default_constructible<_Uy>>)
		: _Mybase1(), _Mybase2() {}

	template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
        std::conjunction_v<_Is_default_constructible<_Ty, _Uy>,
        std::negation<_Is_default_convertible<_Ty, _Uy>>>, bool> = false>
    constexpr explicit pair() noexcept(std::conjunction_v<std::is_nothrow_default_constructible<_Ty>,
        std::is_nothrow_default_constructible<_Uy>>)
        : _Mybase1(), _Mybase2() {}

    template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
        std::conjunction_v<_Is_copy_constructible<_Ty, _Uy>,
        _Is_convertible<_Ty, _Uy, const _Ty&, const _Uy&>>, bool> = true>
    constexpr pair(const T& _First, const U& _Second)
        noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<_Ty>, std::is_nothrow_copy_constructible<_Uy>>)
        : _Mybase1(_First), _Mybase2(_Second) {}

    template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
        std::conjunction_v<_Is_copy_constructible<_Ty, _Uy>,
        std::negation<_Is_convertible<_Ty, _Uy, const _Ty&, const _Uy&>>>, bool> = false>
    constexpr explicit pair(const T& _First, const U& _Second)
        noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<_Ty>, std::is_nothrow_copy_constructible<_Uy>>)
        : _Mybase1(_First), _Mybase2(_Second) {}

    template<typename _Other1, typename _Other2, std::enable_if_t<
        std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
		_Is_convertible<T, U, _Other1, _Other2>>, bool> = true>
	constexpr pair(_Other1&& _First, _Other2&& _Second)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
        : _Mybase1(std::forward<_Other1>(_First)), _Mybase2(std::forward<_Other2>(_Second)) {}

    template<typename _Other1, typename _Other2, std::enable_if_t<
        std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
        std::negation<_Is_convertible<T, U, _Other1, _Other2>>>, bool> = false>
    constexpr explicit pair(_Other1&& _First, _Other2&& _Second)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
        : _Mybase1(std::forward<_Other1>(_First)), _Mybase2(std::forward<_Other2>(_Second)) {}

    pair(const pair&) = default;
    pair(pair&&) noexcept = default;

    template<typename _Other1, typename _Other2, std::enable_if_t<
        std::conjunction_v<_Is_constructible<T, U, const _Other1&, const _Other2&>,
		_Is_convertible<T, U, const _Other1&, const _Other2&>>, bool> = true>
    constexpr pair(const pair<_Other1, _Other2>& other)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<T, const _Other1&>, std::is_nothrow_constructible<U, const _Other2&>>)
        : _Mybase1(other.first()), _Mybase2(other.second()) {}

    template<typename _Other1, typename _Other2, std::enable_if_t<
        std::conjunction_v<_Is_constructible<T, U, const _Other1&, const _Other2&>,
        std::negation<_Is_convertible<T, U, const _Other1&, const _Other2&>>>, bool> = false>
    constexpr explicit pair(const pair<_Other1, _Other2>& other)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<T, const _Other1&>, std::is_nothrow_constructible<U, const _Other2&>>)
        : _Mybase1(other.first()), _Mybase2(other.second()) {}

    template<typename _Other1, typename _Other2, std::enable_if_t<
        std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
		_Is_convertible<T, U, _Other1, _Other2>>, bool> = true>
    constexpr pair(pair<_Other1, _Other2>&& other)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
        : _Mybase1(std::forward<_Other1>(other.first())), _Mybase2(std::forward<_Other2>(other.second())) {}

    template<typename _Other1, typename _Other2, std::enable_if_t<
        std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
		std::negation<_Is_convertible<T, U, _Other1, _Other2>>>, bool> = false>
    constexpr explicit pair(pair<_Other1, _Other2>&& other)
        noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
        : _Mybase1(std::forward<_Other1>(other.first())), _Mybase2(std::forward<_Other2>(other.second())) {}

    template<typename _Tuple1, typename _Tuple2, size_t...N1, size_t...N2>
    constexpr pair(_Tuple1& tp1, _Tuple2& tp2, std::index_sequence<N1...>, std::index_sequence<N2...>)
        : _Mybase1(std::get<N1>(std::move(tp1))...), _Mybase2(std::get<N2>(std::move(tp2))...) {}

    template<typename...Args1, typename...Args2>
    constexpr pair(std::piecewise_construct_t, std::tuple<Args1...> tp1, std::tuple<Args2...> tp2)
        : pair(tp1, tp2, std::index_sequence_for<Args1...>{}, std::index_sequence_for<Args2...>{}) {}

    constexpr pair& 
        operator=(std::conditional_t<std::conjunction_v<std::is_copy_assignable<T>,
            std::is_copy_assignable<U>>,const pair&, const wjr::disable_tag&> other)
        noexcept(std::conjunction_v<std::is_nothrow_copy_assignable<T>, std::is_nothrow_copy_assignable<U>>) {
        first() = other.first();
        second() = other.second();
        return *this;
    }

    constexpr pair& 
        operator=(std::conditional_t<std::conjunction_v<std::is_move_assignable<T>,
            std::is_move_assignable<U>>, pair&&, wjr::disable_tag&&> other)
        noexcept(std::conjunction_v<std::is_nothrow_move_assignable<T>, std::is_nothrow_move_assignable<U>>) {
        first() = std::forward<T>(other.first());
        second() = std::forward<U>(other.second());
        return *this;
    }

    template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<std::negation<std::is_same<pair, pair<_Other1, _Other2>>>, 
        std::is_assignable<T&, const _Other1&>,
		std::is_assignable<U&, const _Other2&>>, int> = 0>
	constexpr pair& operator=(const pair<_Other1, _Other2>& other)
		noexcept(std::conjunction_v<std::is_nothrow_assignable<T&, const _Other1&>, 
            std::is_nothrow_assignable<U&, const _Other2&>>) {
		first() = other.first();
		second() = other.second();
		return *this;
	}

    template<typename _Other1, typename _Other2, std::enable_if_t<
		std::conjunction_v<std::negation<std::is_same<pair, pair<_Other1, _Other2>>>, 
		std::is_assignable<T&, _Other1>,
		std::is_assignable<U&, _Other2>>, int> = 0>
    constexpr pair& operator=(pair<_Other1, _Other2>&& other)
		noexcept(std::conjunction_v<std::is_nothrow_assignable<T&, _Other1>,
			std::is_nothrow_assignable<U&, _Other2>>) {
		first() = std::forward<_Other1>(other.first());
		second() = std::forward<_Other2>(other.second());
		return *this;
	}

	constexpr void swap(pair& other)
		noexcept(std::conjunction_v<is_nothrow_swappable<T>, is_nothrow_swappable<U>>) {
		using std::swap;
		swap(first(), other.first());
		swap(second(), other.second());
	}

    constexpr T& first() noexcept { return _Mybase1::value(); }
    constexpr const T& first() const noexcept { return _Mybase1::value(); }
    constexpr U& second() noexcept { return _Mybase2::value(); }
    constexpr const U& second() const noexcept { return _Mybase2::value(); }
};

#ifdef WJR_CPP17
template<typename T, typename U>
pair(T, U)->pair<T, U>;
#endif

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator==(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return lhs.first() == rhs.first() && lhs.second() == rhs.second();
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator!=(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return !(lhs == rhs);
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator<(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return lhs.first() < rhs.first() || (!(rhs.first() < lhs.first()) && lhs.second() < rhs.second());
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator>(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return rhs < lhs;
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator<=(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return !(rhs < lhs);
}

template<typename T, typename U>
WJR_NODISCARD constexpr bool operator>=(const pair<T, U>& lhs, const pair<T, U>& rhs) {
	return !(lhs < rhs);
}

template<typename T, typename U>
WJR_NODISCARD constexpr pair<unrefwrap_t<T>, unrefwrap_t<U>> make_pair(T&& t, U&& u) 
noexcept(std::conjunction_v<std::is_nothrow_constructible<unrefwrap_t<T>, T>,
	std::is_nothrow_constructible<unrefwrap_t<U>, U>>) {
	return pair<unrefwrap_t<T>, unrefwrap_t<U>>(std::forward<T>(t), std::forward<U>(u));
}

_WJR_END

namespace std {

    template<typename T, typename U>
    using _Wjr_pair = wjr::pair<T, U>;
	
	template<typename T, typename U, enable_if_t<conjunction_v<wjr::is_swappable<T>, wjr::is_swappable<U>>, int> = 0>
	void swap(_Wjr_pair<T, U>& lhs, _Wjr_pair<T, U>& rhs)
		noexcept(noexcept(lhs.swap(rhs))) {
		lhs.swap(rhs);
	}
	template<typename T, typename U>
	struct tuple_size<_Wjr_pair<T, U>> : integral_constant<size_t, 2> {};

	template<size_t I, typename T, typename U>
	struct tuple_element<I, _Wjr_pair<T, U>> {
		static_assert(I < 2, "tuple_element index out of bounds");
		using type = conditional_t<I == 0, T, U>;
	};

    template<typename _Ret, typename _Pair>
	WJR_NODISCARD constexpr _Ret& _Wjr_pair_get(_Pair& p, integral_constant<size_t, 0>) noexcept {
		return p.first();
	}

	template<typename _Ret, typename _Pair>
    WJR_NODISCARD constexpr _Ret& _Wjr_pair_get(_Pair& p, integral_constant<size_t, 1>) noexcept {
        return p.second();
    }

    template<size_t I, typename T, typename U>
	WJR_NODISCARD constexpr tuple_element_t<I, _Wjr_pair<T, U>>& get(_Wjr_pair<T, U>& p) noexcept {
		using _Ret = tuple_element_t<I, _Wjr_pair<T, U>>&;
        return _Wjr_pair_get<_Ret>(p, integral_constant<size_t, I>{});
	}

	template<typename T, typename U>
	WJR_NODISCARD constexpr T& get(_Wjr_pair<T, U>& p) noexcept {
        return get<0>(p);
	}

	template<typename U, typename T>
	WJR_NODISCARD constexpr U& get(_Wjr_pair<T, U>& p) noexcept {
        return get<1>(p);
	}

    template<size_t I, typename T, typename U>
    WJR_NODISCARD constexpr const tuple_element_t<I, _Wjr_pair<T, U>>& get(const _Wjr_pair<T, U>& p) noexcept {
        using _Ret = const tuple_element_t<I, _Wjr_pair<T, U>>&;
        return _Wjr_pair_get<_Ret>(p, integral_constant<size_t, I>{});
    }

    template<typename T, typename U>
    WJR_NODISCARD constexpr const T& get(const _Wjr_pair<T, U>& p) noexcept {
        return get<0>(p);
    }

    template<typename U, typename T>
    WJR_NODISCARD constexpr const U& get(const _Wjr_pair<T, U>& p) noexcept {
        return get<1>(p);
    }


    template <size_t I, typename T, typename U>
    WJR_NODISCARD constexpr tuple_element_t<I, _Wjr_pair<T, U>>&& get(
        _Wjr_pair<T, U>&& p) noexcept {
        using _RRtype = tuple_element_t<I, _Wjr_pair<T, U>>&&;
        return forward<_RRtype>(get<I>(p));
    }

    template <typename T, typename U>
    WJR_NODISCARD constexpr T&& get(_Wjr_pair<T, U>&& p) noexcept {
        return get<0>(move(p));
    }

    template <typename U, typename T>
    WJR_NODISCARD constexpr U&& get(_Wjr_pair<T, U>&& p) noexcept {
        return get<1>(move(p));
    }

    template <size_t I, typename T, typename U>
    WJR_NODISCARD constexpr const tuple_element_t<I, _Wjr_pair<T, U>>&& get(
        const _Wjr_pair<T, U>&& p) noexcept { 
        using _RRtype = const tuple_element_t<I, _Wjr_pair<T, U>>&&;
        return forward<_RRtype>(get<I>(p));
    }

    template <typename T, typename U>
    WJR_NODISCARD constexpr const T&& get(
        const _Wjr_pair<T, U>&& p) noexcept {
        return get<0>(move(p));
    }

    template <typename U, typename T>
    WJR_NODISCARD constexpr const U&& get(
        const _Wjr_pair<T, U>&& p) noexcept {
        return get<1>(move(p));
    }

}

#endif // __WJR_COMPRESSED_PAIR_H