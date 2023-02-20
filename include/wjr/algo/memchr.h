#pragma once
#ifndef __WJR_ALGO_MEMCHR_H__
#define __WJR_ALGO_MEMCHR_H__

#include <wjr/algo/macro.h>
#if defined(__HAS_FAST_MEMCHR)

#define __WJR_MEMCHR_ONE(st, _s)	                        \
	auto r = st::cmpeq(x, y, T());	                        \
	st::mask_type z = st::movemask_epi8(r);	                \
	if(z != 0){	                                            \
		return (_s) + wjr::countr_zero(z) / _Mysize;	    \
	}

#define __WJR_MEMCHR_FOUR(st, _s0, _s1, _s2, _s3)	        \
	auto r0 = st::cmpeq(x0, y, T());	                    \
	auto r1 = st::cmpeq(x1, y, T());	                    \
	auto r2 = st::cmpeq(x2, y, T());	                    \
	auto r3 = st::cmpeq(x3, y, T());	                    \
	                                                        \
	r3 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	        \
	st::mask_type z = st::movemask_epi8(r3);	            \
	if(z != 0){	                                            \
		st::mask_type tmp = st::movemask_epi8(r0);	        \
		if(tmp != 0){	                                    \
			return (_s0) + wjr::countr_zero(tmp) / _Mysize;	\
		}	                                                \
		tmp = st::movemask_epi8(r1);	                    \
		if(tmp != 0){	                                    \
			return (_s1) + wjr::countr_zero(tmp) / _Mysize;	\
		}	                                                \
		tmp = st::movemask_epi8(r2);	                    \
		if(tmp != 0){	                                    \
			return (_s2) + wjr::countr_zero(tmp) / _Mysize;	\
		}	                                                \
		tmp = z;	                                        \
		return (_s3) + wjr::countr_zero(tmp) / _Mysize;	    \
	}

_WJR_ALGO_BEGIN

template<typename T>
const T* __memchr(const T* s, T val, size_t n) {
	using namespace wjr::literals;
	constexpr size_t _Mysize = sizeof(T);

#if defined(__AVX2__)
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // __AVX2__
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	if (is_constant_p(n) && n <= 4_KiB) {
		for (size_t i = 0; i < n; ++i) {
			if (s[i] == val) {
				return s + i;
			}
		}
		return s + n;
	}

	if (is_unlikely(n == 0)) return s;

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			const T* _lst;

			auto y = simd_t::set1(val, T());

			if (is_constant_p(reinterpret_cast<uintptr_t>(s) % bound) &&
				reinterpret_cast<uintptr_t>(s) % bound == 0) {
				// do nothing
			}
			else if (_Mysize == 1 ||
					reinterpret_cast<uintptr_t>(s) % _Mysize == 0) {
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

				__WJR_MEMCHR_ONE(simd_t, s);

				auto __align_s = bound - reinterpret_cast<uintptr_t>(s) % bound;
				s += __align_s / _Mysize;
				n -= __align_s / _Mysize;
				if (is_unlikely(n < width * 4)) {
					_lst = s + n;
					goto WJR_MACRO_LABEL(aft_align);
				}
			}
			else {
				// unalign algorithm
				do {
					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s));
					auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s + width));
					auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 3));

					__WJR_MEMCHR_FOUR(simd_t, s, s + width, s + width * 2, s + width * 3);

					s += width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				_lst = s + n;
				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMCHR_ONE(simd_t, s);
						s += width;
					}
					case 3: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMCHR_ONE(simd_t, s);
						s += width;
					}
					case 2: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMCHR_ONE(simd_t, s);
					}
					case 1: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst - width));

						__WJR_MEMCHR_ONE(simd_t, _lst - width);
					}
					}
				}
				return _lst;
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s + width));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s + width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s + width * 3));

				__WJR_MEMCHR_FOUR(simd_t, s, s + width, s + width * 2, s + width * 3);
				
				s += width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			_lst = s + n;
			if (n != 0) {
				switch ((n + width - 1) / width) {
				default: unreachable(); break;
				case 4: {
					WJR_MACRO_LABEL(aft_align) :
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));
					
					__WJR_MEMCHR_ONE(simd_t, s);
					s += width;
				}
				case 3: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMCHR_ONE(simd_t, s);
					s += width;
				}
				case 2: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMCHR_ONE(simd_t, s);
				}
				case 1: {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst - width));

					__WJR_MEMCHR_ONE(simd_t, _lst - width);
				}
				}
			}
			return _lst;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto y = simd::avx::set1(val, T());

			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + 32 / _Mysize));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + n - 64 / _Mysize));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + n - 32 / _Mysize));

			__WJR_MEMCHR_FOUR(simd::avx, s, s + 32 / _Mysize, s + n - 64 / _Mysize, s + n - 32 / _Mysize);

			return s + n;
		}
#endif // __AVX2__

		auto y = simd::sse::set1(val, T());
		auto delta = (n & (32 / _Mysize)) >> 1;

		auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s));
		auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + delta));
		auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + n - 16 / _Mysize - delta));
		auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + n - 16 / _Mysize));

		__WJR_MEMCHR_FOUR(simd::sse, s, s + delta, s + n - 16 / _Mysize - delta, s + n - 16 / _Mysize);

		return s + n;
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return *s == val ? s : s + 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A = *reinterpret_cast<const uint64_t*>(s);
			auto B = *reinterpret_cast<const uint64_t*>(s + n - 4);

			auto x = simd::sse::set_epi64x(B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmpeq(x, y, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0)return s + n;
			auto i = wjr::countr_zero(z) / _Mysize;
			auto q = i >= 4 ? i + n - 8 : i;
			return s + q;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A = *reinterpret_cast<const uint32_t*>(s);
			auto B = *reinterpret_cast<const uint32_t*>(s + delta);
			auto C = *reinterpret_cast<const uint32_t*>(s + n - 4 - delta);
			auto D = *reinterpret_cast<const uint32_t*>(s + n - 4);

			auto x = simd::sse::set_epi32(D, C, B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmpeq(x, y, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0) return s + n;
			auto i = wjr::countr_zero(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s + q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (*s == val) return s;
		if (n == 1) return s + 1;
		if (s[1] == val) return s + 1;
		if (n == 2) return s + 2;
		return s[2] == val ? s + 2 : s + 3;
	}
}

_WJR_ALGO_END

#undef __WJR_MEMCHR_FOUR
#undef __WJR_MEMCHR_ONE

#endif // __HAS_FAST_MEMCHR

#endif // WJR_ALGO_MEMCHR_H