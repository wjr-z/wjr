#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif 

#if defined(_WJR_FAST_MEMMIS)

#define __WJR_MEMRMIS_ONE(st, _s)														    \
	{	                                                                                    \
		auto r = st::cmp(x, y, pred, T());	                                                \
		st::mask_type z = st::movemask_epi8(r);		                                        \
		if(z != st::mask()) {	                                                            \
			return (_s) - wjr::countl_one(z) / _Mysize;		                                \
		}	                                                                                \
	}

#define __WJR_MEMRMIS_FOUR(st, _s0, _s1, _s2, _s3)										    \
	{	                                                                                    \
		auto r0 = st::cmp(x0, y0, pred, T());	                                            \
		auto r1 = st::cmp(x1, y1, pred, T());	                                            \
		auto r2 = st::cmp(x2, y2, pred, T());	                                            \
		auto r3 = st::cmp(x3, y3, pred, T());	                                            \
																							\
		r0 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
		st::mask_type z = st::movemask_epi8(r0);		                                    \
		if(z != st::mask()) {	                                                            \
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
		}	                                                                                \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memrmis(const T* s0, const T* s1, size_t n, _Pred pred) {
	using namespace wjr::literals;
	constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // WJR_AVX2
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	if (is_unlikely(n == 0)) return s0;

	s0 += n;
	s1 += n;

	if (n >= 16 / _Mysize) {

		if (n <= 32 / _Mysize) {
			// solve first 16 bytes
			auto x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (16 / _Mysize)));
			auto y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (16 / _Mysize)));

			__WJR_MEMRMIS_ONE(simd::sse, s0);

			// solve last 16 bytes

			s0 -= n;
			s1 -= n;

			x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
			y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));

			__WJR_MEMRMIS_ONE(simd::sse, s0 + (16 / _Mysize));

			return s0;
		}

		// solve first min(n, 128 / _Mysize) bytes
		// no branch algorithm

		{
			const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
			const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;

#if WJR_AVX2
			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - m));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - m + delta));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - (32 / _Mysize) - delta));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - (32 / _Mysize)));

			auto y0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - m));
			auto y1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - m + delta));
			auto y2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - (32 / _Mysize) - delta));
			auto y3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - (32 / _Mysize)));

			__WJR_MEMRMIS_FOUR(simd::avx, 
				s0 - m + 32 / _Mysize, 
				s0 - m + delta + 32 / _Mysize, 
				s0 - delta, 
				s0);
#else
			{
				auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (32 / _Mysize) - delta));
				auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (16 / _Mysize) - delta));
				auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (32 / _Mysize)));
				auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (16 / _Mysize)));

				auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (32 / _Mysize) - delta));
				auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (16 / _Mysize) - delta));
				auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (32 / _Mysize)));
				auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (16 / _Mysize)));

				__WJR_MEMRMIS_FOUR(simd::sse,
					s0 - (16 / _Mysize) - delta,
					s0 - delta,
					s0 - (16 / _Mysize),
					s0);
			}
			
			{
				auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - m));
				auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - m + 16 / _Mysize));
				auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - m + delta));
				auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - m + delta + 16 / _Mysize));

				auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - m));
				auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - m + 16 / _Mysize));
				auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - m + delta));
				auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - m + delta + 16 / _Mysize));

				__WJR_MEMRMIS_FOUR(simd::sse, 
					s0 - m + 16 / _Mysize, 
					s0 - m + 32 / _Mysize, 
					s0 - m + delta + 16 / _Mysize, 
					s0 - m + delta + 32 / _Mysize);
			}
#endif // WJR_AVX2

			// m = std::min(n, 128 / _Mysize) 
			// m == n -> n <= 128 / _Mysize
			if (m == n) {
				return s0 - n;
			}
		}

		if (n <= 128 / _Mysize + width * 4) {

			WJR_MACRO_LABEL(unaligned_last_4vec) :

			s0 -= n;
			s1 -= n;

			auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
			auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
			auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
			auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

			auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
			auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
			auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
			auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

			__WJR_MEMRMIS_FOUR(simd_t, s0 + width, s0 + width * 2, s0 + width * 3, s0 + width * 4);

			return s0;
		}

		s0 -= 128 / _Mysize;
		s1 -= 128 / _Mysize;
		n -= 128 / _Mysize;

		// align

		if (is_likely(reinterpret_cast<uintptr_t>(s0) % _Mysize == 0)) {

			const auto off0 = reinterpret_cast<uintptr_t>(s0) % bound;
			const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;

			if (off0 != off1) {
				// only align first pointer

				if (is_constant_p(off0) && off0 == 0) {
					// do nothing
				}
				else {
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_ONE(simd_t, s0);

					const auto __align_s = off0;
					s0 -= __align_s / _Mysize;
					s1 -= __align_s / _Mysize;
					n -= __align_s / _Mysize;
					
					if (is_unlikely(n < width * 4)) {
						goto WJR_MACRO_LABEL(last_solve_align_0);
					}
				}

				do {
					auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 4));
					auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 3));
					auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 2));
					auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 - width));

					auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 4));
					auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 3));
					auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 2));
					auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

					s0 -= width * 4;
					s1 -= width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				if (n != 0) {

					WJR_MACRO_LABEL(last_solve_align_0) :

					s0 -= n;
					s1 -= n;

					const auto ptr0 = reinterpret_cast<T*>(
						(reinterpret_cast<uintptr_t>(s0 + width * 4)) & (~(bound - 1)));
					const auto dif = ptr0 - s0;
					const auto ptr1 = s1 + dif;

					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
					auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 3));
					auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 2));
					auto x3 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width));

					auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
					auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 - width * 3));
					auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 - width * 2));
					auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 - width));

					__WJR_MEMRMIS_FOUR(simd_t, s0 + width, ptr0 - width * 2, ptr0 - width, ptr0);

				}

				return s0;
			}

			// align two pointers
			if (is_constant_p(off0) && off0 == 0) {
				// do nothing
			}
			else {
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
				auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

				__WJR_MEMRMIS_ONE(simd_t, s0);

				const auto __align_s = off0;
				s0 -= __align_s / _Mysize;
				s1 -= __align_s / _Mysize;
				n -= __align_s / _Mysize;
				
				if (is_unlikely(n < width * 4)) {
					goto WJR_MACRO_LABEL(last_solve_align_0_1);
				}
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

			if (n != 0) {

				WJR_MACRO_LABEL(last_solve_align_0_1) :

				s0 -= n;
				s1 -= n;

				const auto ptr0 = reinterpret_cast<T*>(
					(reinterpret_cast<uintptr_t>(s0 + width * 4)) & (~(bound - 1)));
				const auto dif = ptr0 - s0;
				const auto ptr1 = s1 + dif;

				auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 3));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width));

				auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width * 3));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width * 2));
				auto y3 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width));

				__WJR_MEMRMIS_FOUR(simd_t, s0 + width, ptr0 - width * 2, ptr0 - width, ptr0);

			}

			return s0;
		}

		if (is_likely(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {
			const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;
			// only align second pointer
			if (is_constant_p(off1) && off1 == 0) {
				// do nothing
			}
			else {
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
				auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

				__WJR_MEMRMIS_ONE(simd_t, s0);

				const auto __align_s = off1;
				s0 -= __align_s / _Mysize;
				s1 -= __align_s / _Mysize;
				n -= __align_s / _Mysize;
				
				if (is_unlikely(n < width * 4)) {
					goto WJR_MACRO_LABEL(last_solve_align_1);
				}
			}

			do {
				auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 4));
				auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 3));
				auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 2));
				auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

				auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 4));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 3));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 2));
				auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

				__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

				s0 -= width * 4;
				s1 -= width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			if (n != 0) {

				WJR_MACRO_LABEL(last_solve_align_1) :

				s0 -= n;
				s1 -= n;

				const auto ptr1 = reinterpret_cast<T*>(
					(reinterpret_cast<uintptr_t>(s1 + width * 4)) & (~(bound - 1)));
				const auto dif = ptr1 - s1;
				const auto ptr0 = s0 + dif;

				auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
				auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 - width * 3));
				auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 - width * 2));
				auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 - width));

				auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width * 3));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width * 2));
				auto y3 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width));

				__WJR_MEMRMIS_FOUR(simd_t, s0 + width, ptr0 - width * 2, ptr0 - width, ptr0);

			}

			return s0;
		}
		
		// unaligned algorithm
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

		if (n == 0) {
			return s0;
		}
		
		goto WJR_MACRO_LABEL(unaligned_last_4vec);
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
			auto q = i >= 4 ? i + (n - 8) : i;
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