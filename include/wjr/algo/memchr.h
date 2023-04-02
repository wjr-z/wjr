#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif 

#if defined(_WJR_FAST_MEMCHR)

#define __WJR_MEMCHR_ONE(st, _s, _q)	                            \
	{	                                                            \
		auto r = st::cmp(x, _q, pred, T());	                        \
		st::mask_type z = st::movemask_epi8(r);	                    \
		if(z != 0){	                                                \
			return (_s) + wjr::countr_zero(z) / _Mysize;	        \
		}	                                                        \
	}

#define __WJR_MEMCHR_FOUR(st, _s0, _s1, _s2, _s3, _q)	            \
	{	                                                            \
		auto r0 = st::cmp(x0, _q, pred, T());	                    \
		auto r1 = st::cmp(x1, _q, pred, T());	                    \
		auto r2 = st::cmp(x2, _q, pred, T());	                    \
		auto r3 = st::cmp(x3, _q, pred, T());	                    \
																	\
		r3 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	            \
		st::mask_type z = st::movemask_epi8(r3);	                \
		if(z != 0){	                                                \
			st::mask_type tmp = st::movemask_epi8(r0);	            \
			if(tmp != 0){	                                        \
				return (_s0) + wjr::countr_zero(tmp) / _Mysize;	    \
			}	                                                    \
			tmp = st::movemask_epi8(r1);	                        \
			if(tmp != 0){	                                        \
				return (_s1) + wjr::countr_zero(tmp) / _Mysize;	    \
			}	                                                    \
			tmp = st::movemask_epi8(r2);	                        \
			if(tmp != 0){	                                        \
				return (_s2) + wjr::countr_zero(tmp) / _Mysize;	    \
			}	                                                    \
			tmp = z;	                                            \
			return (_s3) + wjr::countr_zero(tmp) / _Mysize;	        \
		}	                                                        \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memchr(const T* s, T val, size_t n, _Pred pred) {
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

	constexpr size_t __constant_threshold = 8 / _Mysize;

	if (is_unlikely(n == 0)) return s;

	if (n >= 16 / _Mysize) {
		auto qx = simd::sse::set1(val, T());

		if (n <= 32 / _Mysize) {
			// solve first 16 bytes
			auto x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s));

			__WJR_MEMCHR_ONE(simd::sse, s, qx);

			// solve last 16 bytes

			s += n - 16 / _Mysize;

			x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s));

			__WJR_MEMCHR_ONE(simd::sse, s, qx);

			return s + 16 / _Mysize;
		}

		// solve first min(n, 128 / _Mysize) bytes
		// no branch algorithm

		{
			const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
			const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
			const auto negdelta = m - 32 / _Mysize;

#if WJR_AVX2
			{
				auto qy = broadcast<simd::__m256i_tag, simd::__m128i_tag>(qx);
				auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s));
				auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + delta));
				auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + negdelta - delta));
				auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + negdelta));

				__WJR_MEMCHR_FOUR(simd::avx, 
					s, 
					s + delta, 
					s + negdelta - delta, 
					s + negdelta, 
					qy);
			}
#else
			{
				auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s));
				auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + 16 / _Mysize));
				auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + delta));
				auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + delta + 16 / _Mysize));

				__WJR_MEMCHR_FOUR(simd::sse, 
					s, 
					s + 16 / _Mysize, 
					s + delta, 
					s + delta + 16 / _Mysize, 
					qx);
			}

			{
				auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + negdelta - delta));
				auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + negdelta - delta + 16 / _Mysize));
				auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + negdelta));
				auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + negdelta + 16 / _Mysize));

				__WJR_MEMCHR_FOUR(simd::sse,
					s + negdelta - delta,
					s + negdelta - delta + 16 / _Mysize,
					s + negdelta,
					s + negdelta + 16 / _Mysize,
					qx);
			}
#endif // WJR_AVX2

			// m = std::min(n, 128 / _Mysize) 
			// m == n -> n <= 128 / _Mysize
			if (m == n) {
				return s + n;
			}
		}

		auto q = broadcast<simd::__simd_wrapper_t<sint>, simd::__m128i_tag>(qx);

		if (n <= 128 / _Mysize + width * 4) {

			WJR_MACRO_LABEL(unaligned_last_4vec) :

			s += n;

			auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 4));
			auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 3));
			auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 2));
			auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s - width));

			__WJR_MEMCHR_FOUR(simd_t, 
				s - width * 4, 
				s - width * 3, 
				s - width * 2, 
				s - width, 
				q);

			return s;
		}

		s += 128 / _Mysize;
		n -= 128 / _Mysize;

		// align

		if (is_likely(reinterpret_cast<uintptr_t>(s) % _Mysize == 0)) {

			const auto off0 = reinterpret_cast<uintptr_t>(s) % bound;

			// align two pointers
			if (is_constant_p(off0) && off0 == 0) {
				// do nothing
			}
			else {
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

				__WJR_MEMCHR_ONE(simd_t, s, q);

				const auto __align_s = bound - off0;
				s += __align_s / _Mysize;
				n -= __align_s / _Mysize;

				if (is_unlikely(n < width * 4)) {
					goto WJR_MACRO_LABEL(last_solve_align);
				}
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s + width));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s + width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s + width * 3));

				__WJR_MEMCHR_FOUR(simd_t, 
					s, 
					s + width, 
					s + width * 2, 
					s + width * 3, 
					q);

				s += width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			if (n != 0) {

				WJR_MACRO_LABEL(last_solve_align) :

				s += n;

				auto ptr0 = reinterpret_cast<T*>(
					(reinterpret_cast<uintptr_t>(s - width * 3)) & (~(bound - 1)));

				auto x0 = simd_t::load(reinterpret_cast<const sint*>(ptr0));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width * 2));
				auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s - width));

				__WJR_MEMCHR_FOUR(simd_t, 
					ptr0, 
					ptr0 + width, 
					ptr0 + width * 2, 
					s - width, 
					q);
			}

			return s;
		}

		// unaligned algorithm
		do {
			auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s));
			auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s + width));
			auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 2));
			auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 3));

			__WJR_MEMCHR_FOUR(simd_t, 
				s, 
				s + width, 
				s + width * 2, 
				s + width * 3, 
				q);

			s += width * 4;
			n -= width * 4;
		} while (n >= width * 4);

		if (n == 0) {
			return s;
		}

		goto WJR_MACRO_LABEL(unaligned_last_4vec);
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(*s, val) ? s : s + 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A = *reinterpret_cast<const uint64_t*>(s);
			auto B = *reinterpret_cast<const uint64_t*>(s + n - 4);

			auto x = simd::sse::set_epi64x(B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmp(x, y, pred, T());
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
			
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0) return s + n;
			auto i = wjr::countr_zero(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s + q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (pred(s[0], val)) return s;
		if (n == 1) return s + 1;
		const bool f = pred(s[n - 2], val);
		const bool g = pred(s[n - 1], val);
		const size_t i1 = g ? n - 1 : n;
		const size_t i2 = f ? n - 2 : i1;
		return s + i2;
	}

}

_WJR_ALGO_END

#undef __WJR_MEMCHR_FOUR
#undef __WJR_MEMCHR_ONE

#endif // _WJR_FAST_MEMCHR