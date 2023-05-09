#pragma once
#ifndef __WJR_INTN_H
#define __WJR_INTN_H

#include <wjr/math.h>

_WJR_BEGIN

// template<size_t N>
// uintn<N> is unsigned integer type with (8 * N) bytes
// N is power of 2 and N >= 2

// TODO :

template<size_t N>
class uintn {
	static_assert(has_single_bit(N) && N >= 2, "");
public:
	constexpr uintn() noexcept : data() {}
	constexpr uintn(const uintn&) noexcept = default;
	constexpr uintn& operator=(const uintn&) noexcept = default;
	template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
	constexpr uintn(T x) noexcept {
		data[0] = x;
		for (size_t i = 0; i < N; ++i) {
			data[i] = 0;
		}
	}
	template<typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
	constexpr uintn(T x) noexcept {
		data[0] = x;
		uint64_t c = x < 0 ? -1 : 0;
		for (size_t i = 1; i < N; ++i) {
			data[i] = c;
		}
	}
	template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
	constexpr uintn& operator=(T x) noexcept {
		data[0] = x;
		for (size_t i = 1; i < N; ++i) {
			data[i] = 0;
		}
		return *this;
	}
	template<typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
	constexpr uintn& operator=(T x) noexcept {
		data[0] = x;
		uint64_t c = x < 0 ? -1 : 0;
		for (size_t i = 1; i < N; ++i) {
			data[i] = c;
		}
		return *this;
	}

	WJR_CONSTEXPR20 uintn& operator++() noexcept {
		++data[0];
		uint64_t cf;
		data[1] = adc(data[1], 0, data[0] == 0, cf);
		if constexpr (N > 2) {
			if (WJR_UNLIKELY(cf)) {
				size_t i = 2;
				while (i < N) {
					if (++data[i])break;
				}
			}
		}
		return *this;
	}

	WJR_CONSTEXPR20 uintn& operator--() noexcept {
		--data[0];
		uint64_t cf;
		data[1] = sbb(data[1], 0, data[0] == -1, cf);
		if constexpr (N > 2) {
			if (WJR_UNLIKELY(cf)) {
				size_t i = 2;
				while (i < N) {
					if (--data[i] != -1)break;
				}
			}
		}
		return *this;
	}

	constexpr uintn operator++(int) noexcept {
		uintn tmp = *this;
		++*this;
		return tmp;
	}

	constexpr uintn operator--(int) noexcept {
		uintn tmp = *this;
		--*this;
		return tmp;
	}

	constexpr uintn& operator|= (const uintn& rhs) noexcept {
		for (size_t i = 0; i < N; ++i) {
			data[i] |= rhs.data[i];
		}
		return *this;
	}

	constexpr uintn& operator&= (const uintn& rhs) noexcept {
		for (size_t i = 0; i < N; ++i) {
			data[i] &= rhs.data[i];
		}
		return *this;
	}

	constexpr uintn& operator^= (const uintn& rhs) noexcept {
		for (size_t i = 0; i < N; ++i) {
			data[i] ^= rhs.data[i];
		}
		return *this;
	}

	friend constexpr uintn operator|(const uintn& lhs, const uintn& rhs) noexcept {
		uintn tmp = lhs;
		tmp |= rhs;
		return tmp;
	}

	friend constexpr uintn operator&(const uintn& lhs, const uintn& rhs) noexcept {
		uintn tmp = lhs;
		tmp &= rhs;
		return tmp;
	}

	friend constexpr uintn operator^(const uintn& lhs, const uintn& rhs) noexcept {
		uintn tmp = lhs;
		tmp ^= rhs;
		return tmp;
	}

	friend constexpr uintn operator~ (const uintn& rhs) noexcept {
		uintn tmp;
		for (size_t i = 0; i < N; ++i) {
			tmp.data[i] = ~rhs.data[i];
		}
		return tmp;
	}

private:
	uint64_t data[N];
};

_WJR_END

#endif // __WJR_INTN_H