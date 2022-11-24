#include <wjr/simd.h>
#include <wjr/math.h>

_WJR_BEGIN

#if defined(__AVX2__)

template<typename T>
const T* __AVX2_memchr(const T* s, T val, size_t n) {

	using _Mytraits = simd_traits<__simd_type::AVX2, T>;
	using _Myint = typename _Mytraits::simd_int_type;

	constexpr int width = _Mytraits::width;
	constexpr int cor = sizeof(T) / sizeof(uint8_t);

	_Myint q = _Mytraits::set1_epi(val);

	constexpr auto __one_memchr = [](auto s, auto&q) -> int{
		auto x = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s));
		auto r = _Mytraits::cmpeq_epi(x, q);
		auto z = _Mytraits::movemask_epi(r);
		if (z) {
			return wjr::countr_zero(static_cast<uint32_t>(z)) / cor;
		}
		return -1;
	};

	constexpr auto __four_memchr = [](auto s, auto& q) -> int {
		auto x0 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s));
		auto x1 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width));
		auto x2 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width * 2));
		auto x3 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width * 3));

		auto r0 = _Mytraits::cmpeq_epi(x0, q);
		auto r1 = _Mytraits::cmpeq_epi(x1, q);
		auto r2 = _Mytraits::cmpeq_epi(x2, q);
		auto r3 = _Mytraits::cmpeq_epi(x3, q);

		auto z = _Mytraits::movemask_epi(_Mytraits::or_si(_Mytraits::or_si(r0, r1), _Mytraits::or_si(r2, r3)));

		if (z) {
			auto z0 = _Mytraits::movemask_epi(r0);
			if (z0) {
				return wjr::countr_zero(static_cast<uint32_t>(z0)) / cor;
			}

			auto z1 = _Mytraits::movemask_epi(r1);
			if (z1) {
				return width + wjr::countr_zero(static_cast<uint32_t>(z1)) / cor;
			}

			auto z2 = _Mytraits::movemask_epi(r2);
			if (z2) {
				return width * 2 + wjr::countr_zero(static_cast<uint32_t>(z2)) / cor;
			}

			auto z3 = _Mytraits::movemask_epi(r3);
			return width * 3 + wjr::countr_zero(static_cast<uint32_t>(z3)) / cor;
		}
		return -1;
	};

	switch (n / width) {
	case 0: {
		constexpr int threshold = sizeof(uint32_t) / sizeof(T);
		if (n >= threshold) {

			int k = n / threshold;
			int l = k * threshold;

			auto mask = _Mytraits::mask32_si256_pre(k);
			auto x = _Mytraits::maskload_epi32(reinterpret_cast<const int*>(s), mask);
			auto r = _Mytraits::cmpeq_epi(x, q);
			int z = _Mytraits::movemask_epi(r) & ((1u << (l * cor)) - 1);

			if (z) {
				return s + wjr::countr_zero(static_cast<uint32_t>(z)) / cor;
			}

			s += l;
			n -= l;
		}

		switch (n) {
		case 3:
			if (*s == val) {
				return s;
			}
			++s;
			[[fallthrough]];
		case 2:
			if (*s == val) {
				return s;
			}
			++s;
			[[fallthrough]];
		case 1:
			if (*s == val) {
				return s;
			}
			++s;
			break;
		default:break;
		}
		return s;
	}
	case 3: {
		auto p = __one_memchr(s, q);
		if (p != -1) {
			return s + p;
		}
		s += width;
		n -= width;
		[[fallthrough]];
	}
	case 2: {
		auto p = __one_memchr(s, q);
		if (p != -1) {
			return s + p;
		}
		s += width;
		n -= width;
		[[fallthrough]];
	}
	case 1: {
		auto p = __one_memchr(s, q);
		if (p != -1) {
			return s + p;
		}
		s += width;
		n -= width;

		s += n;
		if(n){
			p = __one_memchr(s - width, q);
			if (p != -1) {
				return s - width + p;
			}
		}
		return s;
	}
	default: {
		do {
			auto p = __four_memchr(s, q);
			if (p != -1) {
				return s + p;
			}

			s += width * 4;
			n -= width * 4;
		} while (n >= 4 * width);

		if (n) {
			s += n;
			auto p = __four_memchr(s - 4 * width, q);
			if (p != -1) {
				return s - 4 * width + p;
			}
		}

		return s;
	}
	}
}

#endif

#if defined(__SSE2__)

template<typename T>
const T* __SSE2_memchr(const T* s, T val, size_t n) {
	using _Mytraits = simd_traits<__simd_type::SSE2, T>;
	using _Myint = typename _Mytraits::simd_int_type;

	constexpr int width = _Mytraits::width;
	constexpr int cor = sizeof(T) / sizeof(uint8_t);

	_Myint q = _Mytraits::set1_epi(val);

	constexpr auto __one_memchr = [](auto s, auto& q) -> int {
		auto x = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s));
		auto r = _Mytraits::cmpeq_epi(x, q);
		auto z = _Mytraits::movemask_epi(r);
		if (z) {
			return wjr::countr_zero(static_cast<uint32_t>(z)) / cor;
		}
		return -1;
	};

	constexpr auto __four_memchr = [](auto s, auto& q) -> int {
		auto x0 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s));
		auto x1 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width));
		auto x2 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width * 2));
		auto x3 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width * 3));

		auto r0 = _Mytraits::cmpeq_epi(x0, q);
		auto r1 = _Mytraits::cmpeq_epi(x1, q);
		auto r2 = _Mytraits::cmpeq_epi(x2, q);
		auto r3 = _Mytraits::cmpeq_epi(x3, q);

		auto z = _Mytraits::movemask_epi(_Mytraits::or_si(_Mytraits::or_si(r0, r1), _Mytraits::or_si(r2, r3)));

		if (z) {
			auto z0 = _Mytraits::movemask_epi(r0);
			if (z0) {
				return wjr::countr_zero(static_cast<uint32_t>(z0)) / cor;
			}

			auto z1 = _Mytraits::movemask_epi(r1);
			if (z1) {
				return width + wjr::countr_zero(static_cast<uint32_t>(z1)) / cor;
			}

			auto z2 = _Mytraits::movemask_epi(r2);
			if (z2) {
				return width * 2 + wjr::countr_zero(static_cast<uint32_t>(z2)) / cor;
			}

			auto z3 = _Mytraits::movemask_epi(r3);
			return width * 3 + wjr::countr_zero(static_cast<uint32_t>(z3)) / cor;
		}
		return -1;

	};

	switch (n / width) {
	case 0: {
		for (; n; ++s, --n) {
			if (*s == val) {
				return s;
			}
		}
		return s;
	}
	case 3: {
		auto p = __one_memchr(s, q);
		if (p != -1) {
			return s + p;
		}
		s += width;
		n -= width;
		[[fallthrough]];
	}
	case 2: {
		auto p = __one_memchr(s, q);
		if (p != -1) {
			return s + p;
		}
		s += width;
		n -= width;
		[[fallthrough]];
	}
	case 1: {
		auto p = __one_memchr(s, q);
		if (p != -1) {
			return s + p;
		}
		s += width;
		n -= width;

		s += n;
		if (n) {
			p = __one_memchr(s - width, q);
			if (p != -1) {
				return s - width + p;
			}
		}
		return s;
	}
	default: {
		do {
			auto p = __four_memchr(s, q);
			if (p != -1) {
				return s + p;
			}

			s += width * 4;
			n -= width * 4;
		} while (n >= 4 * width);

		if (n) {
			s += n;
			auto p = __four_memchr(s - 4 * width, q);
			if (p != -1) {
				return s - 4 * width + p;
			}
		}

		return s;
	}
	}
}

#endif

#if defined(__AVX2__)

template<typename T>
const T* __AVX2_memrchr(const T* s, T val, size_t n) {

	using _Mytraits = simd_traits<__simd_type::AVX2, T>;
	using _Myint = typename _Mytraits::simd_int_type;

	constexpr int width = _Mytraits::width;
	constexpr int cor = sizeof(T) / sizeof(uint8_t);

	_Myint q = _Mytraits::set1_epi(val);

	s += n;

	constexpr auto __one_memrchr = [](auto s, auto& q) -> int {
		auto x = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s));
		auto r = _Mytraits::cmpeq_epi(x, q);
		auto z = _Mytraits::movemask_epi(r);
		if (z) {
			return width - wjr::countl_zero(static_cast<uint32_t>(z)) / cor;
		}
		return -1;
	};

	constexpr auto __four_memrchr = [](auto s, auto& q) -> int {
		auto x0 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s));
		auto x1 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width));
		auto x2 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width * 2));
		auto x3 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width * 3));

		auto r0 = _Mytraits::cmpeq_epi(x0, q);
		auto r1 = _Mytraits::cmpeq_epi(x1, q);
		auto r2 = _Mytraits::cmpeq_epi(x2, q);
		auto r3 = _Mytraits::cmpeq_epi(x3, q);

		auto z = _Mytraits::movemask_epi(_Mytraits::or_si(_Mytraits::or_si(r0, r1), _Mytraits::or_si(r2, r3)));

		if (z) {
			auto z3 = _Mytraits::movemask_epi(r3);
			if (z3) {
				return 4 * width - wjr::countl_zero(static_cast<uint32_t>(z3)) / cor;
			}

			auto z2 = _Mytraits::movemask_epi(r2);
			if (z2) {
				return 3 * width - wjr::countl_zero(static_cast<uint32_t>(z2)) / cor;
			}

			auto z1 = _Mytraits::movemask_epi(r1);
			if (z1) {
				return 2 * width - wjr::countl_zero(static_cast<uint32_t>(z1)) / cor;
			}

			auto z0 = _Mytraits::movemask_epi(r0);
			return width - wjr::countl_zero(static_cast<uint32_t>(z0)) / cor;
		}
		return -1;
	};

	switch (n / width) {
	case 0: {
		constexpr int threshold = sizeof(uint32_t) / sizeof(T);
		if (n >= threshold) {

			int k = n / threshold;
			int l = k * threshold;

			s -= l;

			auto mask = _Mytraits::mask32_si256_pre(k);
			auto x = _Mytraits::maskload_epi32(reinterpret_cast<const int*>(s), mask);
			auto r = _Mytraits::cmpeq_epi(x, q);
			int z = _Mytraits::movemask_epi(r) & ((1u << (l * cor)) - 1);

			if (z) {
				return s + width - wjr::countl_zero(static_cast<uint32_t>(z)) / cor;
			}
			
			n -= l;
		}

		switch (n) {
		case 3:
			--s;
			if (*s == val) {
				return 1 + s;
			}
			[[fallthrough]];
		case 2:
			--s;
			if (*s == val) {
				return 1 + s;
			}
			[[fallthrough]];
		case 1:
			--s;
			if (*s == val) {
				return 1 + s;
			}
			break;
		}
		return s;
	}
	
	case 3: {
		s -= width;
		auto p = __one_memrchr(s, q);
		if (p != -1) {
			return s + p;
		}
		n -= width;
		[[fallthrough]];
	}
	case 2: {
		s -= width;
		auto p = __one_memrchr(s, q);
		if (p != -1) {
			return s + p;
		}
		n -= width;
		[[fallthrough]];
	}
	case 1: {
		s -= width;
		auto p = __one_memrchr(s, q);
		if (p != -1) {
			return s + p;
		}
		n -= width;

		s -= n;
		if (n) {
			p = __one_memrchr(s, q);
			if (p != -1) {
				return s + p;
			}
		}
		return s;
	}
	default: {
		do {
			s -= width * 4;
			auto p = __four_memrchr(s, q);
			if (p != -1) {
				return s + p;
			}
			n -= width * 4;
		} while (n >= 4 * width);

		if (n) {
			s -= n;
			auto p = __four_memrchr(s, q);
			if (p != -1) {
				return s + p;
			}
		}

		return s;
	}
	}
}

#endif

#if defined(__SSE2__)

template<typename T>
const T* __SSE2_memrchr(const T* s, T val, size_t n) {

	using _Mytraits = simd_traits<__simd_type::SSE2, T>;
	using _Myint = typename _Mytraits::simd_int_type;

	constexpr int width = _Mytraits::width;
	constexpr int cor = sizeof(T) / sizeof(uint8_t);

	_Myint q = _Mytraits::set1_epi(val);

	s += n;

	constexpr auto __one_memrchr = [](auto s, auto& q) -> int {
		auto x = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s));
		auto r = _Mytraits::cmpeq_epi(x, q);
		auto z = _Mytraits::movemask_epi(r);
		if (z) {
			return width - wjr::countl_zero(static_cast<uint16_t>(z)) / cor;
		}
		return -1;
	};

	constexpr auto __four_memrchr = [](auto s, auto& q) -> int {
		auto x0 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s));
		auto x1 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width));
		auto x2 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width * 2));
		auto x3 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(s + width * 3));

		auto r0 = _Mytraits::cmpeq_epi(x0, q);
		auto r1 = _Mytraits::cmpeq_epi(x1, q);
		auto r2 = _Mytraits::cmpeq_epi(x2, q);
		auto r3 = _Mytraits::cmpeq_epi(x3, q);

		auto z = _Mytraits::movemask_epi(_Mytraits::or_si(_Mytraits::or_si(r0, r1), _Mytraits::or_si(r2, r3)));

		if (z) {

			auto z3 = _Mytraits::movemask_epi(r3);
			if(z3){
				return width * 4 - wjr::countl_zero(static_cast<uint16_t>(z3)) / cor;
			}

			auto z2 = _Mytraits::movemask_epi(r2);
			if (z2) {
				return width * 3 - wjr::countl_zero(static_cast<uint16_t>(z2)) / cor;
			}

			auto z1 = _Mytraits::movemask_epi(r1);
			if (z1) {
				return width * 2 - wjr::countl_zero(static_cast<uint16_t>(z1)) / cor;
			}

			auto z0 = _Mytraits::movemask_epi(r0);
			return width - wjr::countl_zero(static_cast<uint16_t>(z0)) / cor;
		}
		return -1;
	};

	switch (n / width) {
	case 0: {
		for (; n; --n) {
			--s;
			if (*s == val) {
				return 1 + s;
			}
		}
		return s;
	}

	case 3: {
		s -= width;
		auto p = __one_memrchr(s, q);
		if (p != -1) {
			return s + p;
		}
		n -= width;
		[[fallthrough]];
	}
	case 2: {
		s -= width;
		auto p = __one_memrchr(s, q);
		if (p != -1) {
			return s + p;
		}
		n -= width;
		[[fallthrough]];
	}
	case 1: {
		s -= width;
		auto p = __one_memrchr(s, q);
		if (p != -1) {
			return s + p;
		}
		n -= width;

		s -= n;
		if (n) {
			p = __one_memrchr(s, q);
			if (p != -1) {
				return s + p;
			}
		}
		return s;
	}
	default: {
		do {
			s -= width * 4;
			auto p = __four_memrchr(s, q);
			if (p != -1) {
				return s + p;
			}
			n -= width * 4;
		} while (n >= 4 * width);

		if (n) {
			s -= n;
			auto p = __four_memrchr(s, q);
			if (p != -1) {
				return s + p;
			}
		}

		return s;
	}
	}
}

#endif

#if defined(__HAS_SIMD_MEMCHR)

const uint8_t* simd_memchr(const uint8_t* s, const uint8_t* e, uint8_t val) {
#if defined(__AVX2__)
	return __AVX2_memchr<uint8_t>(s, val, e - s);
#else
	return __SSE2_memchr<uint8_t>(s, val, e - s);
#endif
}

const uint16_t* simd_memchr(const uint16_t* s, const uint16_t* e, uint16_t val) {
#if defined(__AVX2__)
	return __AVX2_memchr<uint16_t>(s, val, e - s);
#else
	return __SSE2_memchr<uint16_t>(s, val, e - s);
#endif
}

const uint32_t* simd_memchr(const uint32_t* s, const uint32_t* e, uint32_t val) {
#if defined(__AVX2__)
	return __AVX2_memchr<uint32_t>(s, val, e - s);
#else
	return __SSE2_memchr<uint32_t>(s, val, e - s);
#endif
}

const uint8_t* simd_memrchr(const uint8_t* s, const uint8_t* e, uint8_t val) {
#if defined(__AVX2__)
	return __AVX2_memrchr<uint8_t>(s, val, e - s);
#else
	return __SSE2_memrchr<uint8_t>(s, val, e - s);
#endif
}

const uint16_t* simd_memrchr(const uint16_t* s, const uint16_t* e, uint16_t val) {
#if defined(__AVX2__)
	return __AVX2_memrchr<uint16_t>(s, val, e - s);
#else
	return __SSE2_memrchr<uint16_t>(s, val, e - s);
#endif
}

const uint32_t* simd_memrchr(const uint32_t* s, const uint32_t* e, uint32_t val) {
#if defined(__AVX2__)
	return __AVX2_memrchr<uint32_t>(s, val, e - s);
#else
	return __SSE2_memrchr<uint32_t>(s, val, e - s);
#endif
}

#endif

#if defined(__HAS_SIMD_MEMCMP)

template<__simd_type S>
const uint8_t* __SIMD_memcmp(const uint8_t* s1, const uint8_t* e1, const uint8_t* s2) {
	using _Mytraits = simd_traits<S, uint8_t>;
	using _Myint = typename _Mytraits::simd_int_type;

	constexpr int width = _Mytraits::width;
	auto n = e1 - s1;
	const uint8_t* i1 = s1;
	const uint8_t* i2 = s2;
	if (n >= width) {
		do {
			auto x1 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(i1));
			auto x2 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(i2));
			auto r = _Mytraits::cmpeq_epi(x1, x2);
			int z = _Mytraits::movemask_epi(r);
			constexpr int __full = width < 32 ? (1 << width) - 1 : -1;
			if (z != __full) {
				return i1 + wjr::countr_one(static_cast<uint32_t>(z));
			}
			i1 += width;
			i2 += width;
		} while (i1 < e1);
		if (i1 < e1) {
			i1 = e1 - width;
			i2 = s2 + n - width;
			auto x1 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(i1));
			auto x2 = _Mytraits::loadu_si(reinterpret_cast<const _Myint*>(i2));
			auto r = _Mytraits::cmpeq_epi(x1, x2);
			int z = _Mytraits::movemask_epi(r);
			constexpr int __full = width < 32 ? (1 << width) - 1 : -1;
			if (z != __full) {
				return i1 + wjr::countr_one(static_cast<uint32_t>(z));
			}
		}
	}
	else {
		if constexpr (S == __simd_type::AVX2) {
			if (n >= 4) {

				int k = n / 4;
				int l = k * 4;

				auto mask = _Mytraits::mask32_si256_pre(k);
				auto x = _Mytraits::maskload_epi32(reinterpret_cast<const int*>(i1), mask);
				auto y = _Mytraits::maskload_epi32(reinterpret_cast<const int*>(i2), mask);
				auto r = _Mytraits::cmpeq_epi(x, y);
				int z = _Mytraits::movemask_epi(r);
				int cl = wjr::countr_one(static_cast<uint32_t>(z));
				if (cl < l) {
					return i1 + cl;
				}
				i1 += l;
				i2 += l;
				n -= l;
			}

			switch (n) {
			case 3:
				if (*i1 != *i2) {
					return i1;
				}
				++i1;
				++i2;
			case 2:
				if (*i1 != *i2) {
					return i1;
				}
				++i1;
				++i2;
			case 1:
				if (*i1 != *i2) {
					return i1;
				}
			}
		}
		else {
			for (; i1 < e1; ++i1, ++i2) {
				if (*i1 != *i2) {
					return i1;
				}
			}
		}
	}
	return e1;
}

const uint8_t* simd_memcmp(const uint8_t* s1, const uint8_t* e1, const uint8_t* s2) {
#if defined(__AVX2__)
	return __SIMD_memcmp<__simd_type::AVX2>(s1, e1, s2);
#else
	return __SIMD_memcmp<__simd_type::SSE2>(s1, e1, s2);
#endif
}

#endif

_WJR_END