#pragma once
#ifndef __WJR_ALGO_MEMRMIS_H
#define __WJR_ALGO_MEMRMIS_H

#include <wjr/literals.h>
#include <wjr/simd/simd_intrin.h>

#if defined(_WJR_FAST_MEMMIS)

#define __WJR_MEMRMIS_ONE(st, _s)														\
	auto r = st::cmp(x, y, pred, T());	                                                \
	st::mask_type z = st::movemask_epi8(r);		                                        \
	if(z != st::mask()){	                                                            \
		return (_s) - wjr::countl_one(z) / _Mysize;		                                \
	}

#define __WJR_MEMRMIS_FOUR(st, _s0, _s1, _s2, _s3)										\
	auto r0 = st::cmp(x0, y0, pred, T());	                                            \
	auto r1 = st::cmp(x1, y1, pred, T());	                                            \
	auto r2 = st::cmp(x2, y2, pred, T());	                                            \
	auto r3 = st::cmp(x3, y3, pred, T());	                                            \
		                                                                                \
	r0 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
	st::mask_type z = st::movemask_epi8(r0);		                                    \
	if(z != st::mask()){	                                                            \
		st::mask_type tmp = st::movemask_epi8(r3);	                                    \
		if(tmp != st::mask()){	                                                        \
			return (_s3) - wjr::countl_one(tmp) / _Mysize;	                            \
		}	                                                                            \
		tmp = st::movemask_epi8(r2);	                                                \
		if(tmp != st::mask()){	                                                        \
			return (_s2) - wjr::countl_one(tmp) / _Mysize;	                            \
		}	                                                                            \
		tmp = st::movemask_epi8(r1);	                                                \
		if(tmp != st::mask()){	                                                        \
			return (_s1) - wjr::countl_one(tmp) / _Mysize;	                            \
		}	                                                                            \
		tmp = z;	                                                                    \
		return (_s0) - wjr::countl_one(tmp) / _Mysize;	                                \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memrmis(const T* s0, const T* s1, size_t n, _Pred pred) {
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

	if (is_unlikely(n == 0)) return s0;

	s0 += n;
	s1 += n;

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			const T* lst0;
			const T* lst1;

			auto _off0 = reinterpret_cast<uintptr_t>(s0) % bound;
			auto _off1 = reinterpret_cast<uintptr_t>(s1) % bound;

			// align 2 pointer
			if (_off0 == _off1 &&
				(_Mysize == 1 || _off0 % _Mysize == 0)) {
				if (is_constant_p(_off0) && _off0 == 0) {
					// do nothing
				}
				else {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_ONE(simd_t, s0);

					auto __align_s = _off0;
					s0 -= __align_s / _Mysize;
					s1 -= __align_s / _Mysize;
					n -= __align_s / _Mysize;
					if (is_unlikely(n < width * 4)) {
						lst0 = s0 - n;
						lst1 = s1 - n;
						goto WJR_MACRO_LABEL(aft_align);
					}
				}
			}
			// unalign algorithm
			else {
				do {
					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 4));
					auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 3));
					auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

					auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 4));
					auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 3));
					auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 2));
					auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

					s0 -= width * 4;
					s1 -= width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				lst0 = s0 - n;
				lst1 = s1 - n;

				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

						__WJR_MEMRMIS_ONE(simd_t, s0);
						s0 -= width;
						s1 -= width;
					}
					case 3: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

						__WJR_MEMRMIS_ONE(simd_t, s0);
						s0 -= width;
						s1 -= width;
					}
					case 2: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

						__WJR_MEMRMIS_ONE(simd_t, s0);
					}
					case 1: {
						auto x = simd_t::loadu(reinterpret_cast<const sint*>(lst0));
						auto y = simd_t::loadu(reinterpret_cast<const sint*>(lst1));

						__WJR_MEMRMIS_ONE(simd_t, lst0 + width);
					}
					}
				}
				return lst0;
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 4));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 3));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 - width));

				auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 4));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 3));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 2));
				auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

				__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

				s0 -= width * 4;
				s1 -= width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			lst0 = s0 - n;
			lst1 = s1 - n;

			if (n != 0) {
				switch ((n + width - 1) / width) {
				default: unreachable(); break;
				case 4: {
					WJR_MACRO_LABEL(aft_align) :
					auto x = simd_t::load(reinterpret_cast<const sint*>(s0 - width));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_ONE(simd_t, s0);
					s0 -= width;
					s1 -= width;
				}
				case 3: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s0 - width));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_ONE(simd_t, s0);
					s0 -= width;
					s1 -= width;
				}
				case 2: {
					auto x = simd_t::load(reinterpret_cast<const sint*>(s0 - width));
					auto y = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_ONE(simd_t, s0);
				}
				case 1: {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(lst0));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(lst1));

					__WJR_MEMRMIS_ONE(simd_t, lst0 + width);
				}
				}
			}

			return lst0;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - n));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - n + 32 / _Mysize));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - 64 / _Mysize));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - 32 / _Mysize));

			auto y0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - n));
			auto y1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - n + 32 / _Mysize));
			auto y2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - 64 / _Mysize));
			auto y3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - 32 / _Mysize));

			__WJR_MEMRMIS_FOUR(simd::avx, s0 - n + 32 / _Mysize, s0 - n + 64 / _Mysize, s0 - 32 / _Mysize, s0);

			return s0 - n;
		}
#endif // __AVX2__

		auto delta = (n & (32 / _Mysize)) >> 1;

		auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - n));
		auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - n + delta));
		auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - 16 / _Mysize - delta));
		auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - 16 / _Mysize));

		auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - n));
		auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - n + delta));
		auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - 16 / _Mysize - delta));
		auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - 16 / _Mysize));

		__WJR_MEMRMIS_FOUR(simd::sse, s0 - n + 16 / _Mysize, s0 - n + delta + 16 / _Mysize, s0 - delta, s0);

		return s0 - n;
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s0[-1], s1[-1]) ? s0 : s0 - 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A0 = *reinterpret_cast<const uint64_t*>(s0 - n);
			auto B0 = *reinterpret_cast<const uint64_t*>(s0 - 4);

			auto A1 = *reinterpret_cast<const uint64_t*>(s1 - n);
			auto B1 = *reinterpret_cast<const uint64_t*>(s1 - 4);

			auto x = simd::sse::set_epi64x(B0, A0);
			auto y = simd::sse::set_epi64x(B1, A1);
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == simd::sse::mask())return s0 - n;
			auto i = wjr::countl_one(z) / _Mysize;
			auto q = i >= 4 ? i + n - 4 : i;
			return s0 - q;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A0 = *reinterpret_cast<const uint32_t*>(s0 - n);
			auto B0 = *reinterpret_cast<const uint32_t*>(s0 - n + delta);
			auto C0 = *reinterpret_cast<const uint32_t*>(s0 - 4 - delta);
			auto D0 = *reinterpret_cast<const uint32_t*>(s0 - 4);

			auto A1 = *reinterpret_cast<const uint32_t*>(s1 - n);
			auto B1 = *reinterpret_cast<const uint32_t*>(s1 - n + delta);
			auto C1 = *reinterpret_cast<const uint32_t*>(s1 - 4 - delta);
			auto D1 = *reinterpret_cast<const uint32_t*>(s1 - 4);

			auto x = simd::sse::set_epi32(D0, C0, B0, A0);
			auto y = simd::sse::set_epi32(D1, C1, B1, A1);

			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == simd::sse::mask()) return s0 - n;
			auto i = wjr::countl_one(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s0 - q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (!pred(s0[-1], s1[-1])) return s0;
		if (n == 1) return s0 - 1;
		if (!pred(s0[-2], s1[-2])) return s0 - 1;
		if (n == 2) return s0 - 2;
		return pred(s0[-3], s1[-3]) ? s0 - 3 : s0 - 2;
	}
}

_WJR_ALGO_END

#undef __WJR_MEMRMIS_FOUR
#undef __WJR_MEMRMIS_ONE

#endif // _WJR_FAST_MEMMIS

#endif // __WJR_ALGO_MEMRMIS_H