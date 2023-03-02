#pragma once
#ifndef __WJR_ALGO_MEMRCHR_H
#define __WJR_ALGO_MEMRCHR_H

#include <wjr/literals.h>
#include <wjr/simd/simd_intrin.h>

#if defined(__HAS_FAST_MEMCHR)

#define __WJR_MEMRCHR_ONE(st, _s)	                            \
	auto r = st::cmp(x, y, pred, T());	                        \
	st::mask_type z = st::movemask_epi8(r);	                    \
	if(z != 0){	                                                \
		return (_s) - wjr::countl_zero(z) / _Mysize;	        \
	}

#define __WJR_MEMRCHR_FOUR(st, _s0, _s1, _s2, _s3)	            \
	auto r0 = st::cmp(x0, y, pred, T());	                    \
	auto r1 = st::cmp(x1, y, pred, T());	                    \
	auto r2 = st::cmp(x2, y, pred, T());	                    \
	auto r3 = st::cmp(x3, y, pred, T());	                    \
	                                                            \
	r0 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	            \
	st::mask_type z = st::movemask_epi8(r0);	                \
	if(z != 0){	                                                \
		st::mask_type tmp = st::movemask_epi8(r3);	            \
		if(tmp != 0){	                                        \
			return (_s3) - wjr::countl_zero(tmp) / _Mysize;	    \
		}	                                                    \
		tmp = st::movemask_epi8(r2);	                        \
		if(tmp != 0){	                                        \
			return (_s2) - wjr::countl_zero(tmp) / _Mysize;	    \
		}	                                                    \
		tmp = st::movemask_epi8(r1);	                        \
		if(tmp != 0){	                                        \
			return (_s1) - wjr::countl_zero(tmp) / _Mysize;	    \
		}	                                                    \
		tmp = z;	                                            \
		return (_s0) - wjr::countl_zero(tmp) / _Mysize;	        \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memrchr(const T* s, T val, size_t n, _Pred pred) {
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

	if (is_unlikely(n == 0)) return s;

	s += n;

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
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s - width));

				__WJR_MEMRCHR_ONE(simd_t, s);

				auto __align_s = reinterpret_cast<uintptr_t>(s) % bound;
				s -= __align_s / _Mysize;
				n -= __align_s / _Mysize;
				if (is_unlikely(n < width * 4)) {
					// n = [width * 3, width * 4)
					_lst = s - n;
					goto WJR_MACRO_LABEL(aft_align);
				}
			}
			else {
				// unalign algorithm
				do {
					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 4));
					auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 3));
					auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s - width));

					__WJR_MEMRCHR_FOUR(simd_t, s - width * 3, s - width * 2, s - width, s);

					s -= width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				_lst = s - n;
				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						s -= width;
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMRCHR_ONE(simd_t, s + width);
					}
					case 3: {
						s -= width;
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMRCHR_ONE(simd_t, s + width);
					}
					case 2: {
						s -= width;
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

						__WJR_MEMRCHR_ONE(simd_t, s + width);
					}
					case 1: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst));

						__WJR_MEMRCHR_ONE(simd_t, _lst + width);
					}
					}
				}
				return _lst;
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s - width * 4));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s - width * 3));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s - width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s - width));

				__WJR_MEMRCHR_FOUR(simd_t, s - width * 3, s - width * 2, s - width, s);

				s -= width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			_lst = s - n;
			if (n != 0) {
				switch ((n + width - 1) / width) {
				default: unreachable(); break;
				case 4: {
					WJR_MACRO_LABEL(aft_align) :
					s -= width;
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMRCHR_ONE(simd_t, s + width);
				}
				case 3: {
					s -= width;
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMRCHR_ONE(simd_t, s + width);
				}
				case 2: {
					s -= width;
					auto x = simd_t::load(reinterpret_cast<const sint*>(s));

					__WJR_MEMRCHR_ONE(simd_t, s + width);
				}
				case 1: {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst));

					__WJR_MEMRCHR_ONE(simd_t, _lst + width);
				}
				}
			}
			return _lst;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto y = simd::avx::set1(val, T());

			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - n));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - n + 32 / _Mysize));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - 64 / _Mysize));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - 32 / _Mysize));

			__WJR_MEMRCHR_FOUR(simd::avx, s - n + 32 / _Mysize, s - n + 64 / _Mysize, s - 32 / _Mysize, s);
			
			return s - n;
		}
#endif // __AVX2__

		auto y = simd::sse::set1(val, T());
		auto delta = (n & (32 / _Mysize)) >> 1;

		auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - n));
		auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - n + delta));
		auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - 16 / _Mysize - delta));
		auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - 16 / _Mysize));

		__WJR_MEMRCHR_FOUR(simd::sse, s - n + 16 / _Mysize, s - n + delta + 16 / _Mysize, s - delta, s);

		return s - n;
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s[-1], val) ? s: s - 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A = *reinterpret_cast<const uint64_t*>(s - n);
			auto B = *reinterpret_cast<const uint64_t*>(s - 4);

			auto x = simd::sse::set_epi64x(B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0)return s - n;
			auto i = wjr::countl_zero(z) / _Mysize;
			auto q = i >= 4 ? i + n - 4 : i;
			return s - q;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A = *reinterpret_cast<const uint32_t*>(s - n);
			auto B = *reinterpret_cast<const uint32_t*>(s - n + delta);
			auto C = *reinterpret_cast<const uint32_t*>(s - 4 - delta);
			auto D = *reinterpret_cast<const uint32_t*>(s - 4);

			auto x = simd::sse::set_epi32(D, C, B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0) return s - n;
			size_t i = wjr::countl_zero(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s - q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (pred(s[-1], val)) return s;
		if (n == 1) return s - 1;
		if (pred(s[-2], val)) return s - 1;
		if (n == 2) return s - 2;
		return pred(s[-3], val) ? s - 2 : s - 3;
	}
}

_WJR_ALGO_END

#undef __WJR_MEMRCHR_FOUR
#undef __WJR_MEMRCHR_ONE

#endif // __HAS_FAST_MEMCHR

#endif // __WJR_ALGO_MEMRCHR_H