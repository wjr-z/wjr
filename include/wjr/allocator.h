#pragma once
#ifndef __WJR_ALLOCATOR_H
#define __WJR_ALLOCATOR_H

#include <new>
#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename _Ty>
struct __aligned_helper;

template<>
struct __aligned_helper<void> {
	constexpr static size_t value = std::max((size_t)(16), (size_t)(alignof(max_align_t)));
};

template<typename _Ty>
struct __aligned_helper {
	constexpr static size_t value = std::max((size_t)alignof(_Ty), __aligned_helper<void>::value);
};

#if defined(_DEBUG)
#define WJR_TEST_ALLOCATOR
#endif

#if defined(WJR_TEST_ALLOCATOR)
struct __test_allocator {
	~__test_allocator();
	long long _Count = 0;
};
extern __test_allocator __test_allocator_instance;
#endif

template<typename _Ty, size_t _Al = __aligned_helper<_Ty>::value, size_t _Off = 0>
class aligned_allocator {
public:
	static_assert(!std::is_const_v<_Ty>, "The C++ Standard forbids containers of const elements "
		"because allocator<const T> is ill-formed.");
	static_assert(_Al >= 8, "The alignment must be greater than or equal to 8.");
	static_assert(_Off >= 0 && _Off < _Al,
		"The offset must be greater than or equal to 0 and less than the alignment.");
	static_assert((_Al & (_Al - 1)) == 0, "The alignment must be a power of 2.");

	using value_type = _Ty;

#if !defined(WJR_CPP_20)
	using pointer = _Ty*;
	using const_pointer = const _Ty*;

	using reference = _Ty&;
	using const_reference = const _Ty&;
#endif // !WJR_CPP_20

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

	template <typename _Other>
	struct rebind {
		using other = aligned_allocator<_Other, _Al, _Off>;
	};

#if !defined(WJR_CPP_20)
	WJR_NODISCARD _Ty* address(_Ty& _Val) const noexcept {
		return std::addressof(_Val);
	}

	WJR_NODISCARD const _Ty* address(const _Ty& _Val) const noexcept {
		return std::addressof(_Val);
	}
#endif // !WJR_CPP_20

	constexpr aligned_allocator() noexcept {}

	constexpr aligned_allocator(const aligned_allocator&) noexcept = default;
	template <typename _Other>
	constexpr aligned_allocator(const aligned_allocator<_Other, _Al, _Off>&) noexcept {}
	WJR_CONSTEXPR20 ~aligned_allocator() = default;
	WJR_CONSTEXPR20 aligned_allocator& operator=(const aligned_allocator&) = default;

	WJR_CONSTEXPR20 void deallocate(_Ty* const _Ptr, const size_t _Count) {
#if defined(WJR_TEST_ALLOCATOR)
		__test_allocator_instance._Count -= _Count * sizeof(_Ty) + _Off;
#endif
		WJR_ASSUME(reinterpret_cast<uintptr_t>(_Ptr) % _Al == _Off);
		auto ptr = reinterpret_cast<char*>(_Ptr) - _Off;
		::operator delete(ptr, _Count * sizeof(_Ty) + _Off, static_cast<std::align_val_t>(_Al));
	}

	WJR_NODISCARD WJR_CONSTEXPR20 _Ty* allocate(const size_t _Count) {
#if defined(WJR_TEST_ALLOCATOR)
		__test_allocator_instance._Count += _Count * sizeof(_Ty) + _Off;
#endif
		auto ptr = static_cast<char*>(::operator new(_Count * sizeof(_Ty) + _Off, 
			static_cast<std::align_val_t>(_Al))) + _Off;
		WJR_ASSUME(reinterpret_cast<uintptr_t>(ptr) % _Al == _Off);
		return reinterpret_cast<_Ty*>(ptr);
	}

#if !defined(WJR_CPP_20)
	WJR_NODISCARD size_t max_size() const noexcept {
		return static_cast<size_t>(-1) / sizeof(_Ty);
	}
#endif // !WJR_CPP_20
};

template <size_t _Al, size_t _Off>
class aligned_allocator<void, _Al, _Off> {
public:
	using value_type = void;
#if !defined(WJR_CPP_20)
	using pointer = void*;
	using const_pointer = const void*;
#endif // !WJR_CPP_20

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

#if !defined(WJR_CPP_20)
	template <class _Other>
	struct rebind {
		using other = aligned_allocator<_Other>;
	};
#endif // !WJR_CPP_20
};

template <typename _Ty, typename _Other, size_t _Al, size_t _Off>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool operator==(
	const aligned_allocator<_Ty, _Al, _Off>&,
	const aligned_allocator<_Other, _Al, _Off>&) noexcept {
	return true;
}

template <typename _Ty, size_t _Al1, size_t _Off1, typename _Other, size_t _Al2, size_t _Off2>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool operator==(
	const aligned_allocator<_Ty, _Al1, _Off1>& lhs,
	const aligned_allocator<_Other, _Al2, _Off2>& rhs) noexcept {
	return false;
}

template <typename _Ty, size_t _Al1, size_t _Off1, typename _Other, size_t _Al2, size_t _Off2>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool operator!=(
	const aligned_allocator<_Ty, _Al1, _Off1>& lhs,
	const aligned_allocator<_Other, _Al2, _Off2>& rhs) noexcept {
	return !(lhs == rhs);
}

template<typename T, size_t _Al, size_t _Off>
struct is_default_allocator<aligned_allocator<T, _Al, _Off>> : std::true_type {};

_WJR_END

#endif // __WJR_ALLOCATOR_H