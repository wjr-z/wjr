#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif 

#if defined(_WJR_FAST_MEMCHR)

#define __WJR_MEMRCHR_ONE(st, _s, _q)	                            \
	{	                                                            \
		auto r = st::cmp(x, _q, pred, T());	                        \
		st::mask_type z = st::movemask_epi8(r);	                    \
		if(z != 0){	                                                \
			return (_s) - wjr::countl_zero(z) / _Mysize;	        \
		}	                                                        \
	}

#define __WJR_MEMRCHR_FOUR(st, _s0, _s1, _s2, _s3, _q)	            \
	{	                                                            \
		auto r0 = st::cmp(x0, _q, pred, T());	                    \
		auto r1 = st::cmp(x1, _q, pred, T());	                    \
		auto r2 = st::cmp(x2, _q, pred, T());	                    \
		auto r3 = st::cmp(x3, _q, pred, T());	                    \
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
		}	                                                        \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memrchr(const T* s, T val, size_t n, _Pred pred) {
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

	if (is_unlikely(n == 0)) return s;

	s += n;

	if (n >= 16 / _Mysize) {
		auto qx = simd::sse::set1(val, T());

		if (n <= 32 / _Mysize) {
			// solve first 16 bytes
			auto x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - (16 / _Mysize)));

			__WJR_MEMRCHR_ONE(simd::sse, s, qx);

			// solve last 16 bytes

			s -= n;

			x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s));

			__WJR_MEMRCHR_ONE(simd::sse, s + (16 / _Mysize), qx);

			return s;
		}

		// solve first min(n, 128 / _Mysize) bytes
		// no branch algorithm

		{
			const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
			const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;

#if WJR_AVX2
			{
				auto qy = broadcast<simd::__m256i_tag, simd::__m128i_tag>(qx);
				auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - m));
				auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - m + delta));
				auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - (32 / _Mysize) - delta));
				auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s - (32 / _Mysize)));

				__WJR_MEMRCHR_FOUR(simd::avx,
					s - m + 32 / _Mysize,
					s - m + delta + 32 / _Mysize,
					s - delta,
					s,
					qy);
			}
#else
			{
				auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - (32 / _Mysize) - delta));
				auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - (16 / _Mysize) - delta));
				auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - (32 / _Mysize)));
				auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - (16 / _Mysize)));

				__WJR_MEMRCHR_FOUR(simd::sse,
					s - (16 / _Mysize) - delta,
					s - delta,
					s - (16 / _Mysize),
					s,
					qx);
			}

			{
				auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - m));
				auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - m + 16 / _Mysize));
				auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - m + delta));
				auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s - m + delta + 16 / _Mysize));

				__WJR_MEMRCHR_FOUR(simd::sse,
					s - m + 16 / _Mysize,
					s - m + 32 / _Mysize,
					s - m + delta + 16 / _Mysize,
					s - m + delta + 32 / _Mysize,
					qx);
			}
#endif // WJR_AVX2

			// m = std::min(n, 128 / _Mysize) 
			// m == n -> n <= 128 / _Mysize
			if (m == n) {
				return s - n;
			}
		}

		auto q = broadcast<simd::__simd_wrapper_t<sint>, simd::__m128i_tag>(qx);

		if (n <= 128 / _Mysize + width * 4) {

			WJR_MACRO_LABEL(unaligned_last_4vec) :

			s -= n;

			auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s));
			auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s + width));
			auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 2));
			auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 3));

			__WJR_MEMRCHR_FOUR(simd_t, 
				s + width, 
				s + width * 2, 
				s + width * 3, 
				s + width * 4,
				q);

			return s;
		}

		s -= 128 / _Mysize;
		n -= 128 / _Mysize;

		// align

		if (is_likely(reinterpret_cast<uintptr_t>(s) % _Mysize == 0)) {

			const auto off0 = reinterpret_cast<uintptr_t>(s) % bound;

			// align two pointers
			if (is_constant_p(off0) && off0 == 0) {
				// do nothing
			}
			else {
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s - width));

				__WJR_MEMRCHR_ONE(simd_t, s, q);

				const auto __align_s = off0;
				s -= __align_s / _Mysize;
				n -= __align_s / _Mysize;

				if (is_unlikely(n < width * 4)) {
					goto WJR_MACRO_LABEL(last_solve_align);
				}
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s - width * 4));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s - width * 3));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s - width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s - width));

				__WJR_MEMRCHR_FOUR(simd_t, s - width * 3, s - width * 2, s - width, s, q);

				s -= width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			if (n != 0) {

				WJR_MACRO_LABEL(last_solve_align) :

				s -= n;

				const auto ptr0 = reinterpret_cast<T*>(
					(reinterpret_cast<uintptr_t>(s + width * 4)) & (~(bound - 1)));

				auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 3));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width));

				__WJR_MEMRCHR_FOUR(simd_t, 
					s + width, 
					ptr0 - width * 2, 
					ptr0 - width, 
					ptr0, 
					q);

			}

			return s;
		}

		// unaligned algorithm
		do {
			auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 4));
			auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 3));
			auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 2));
			auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s - width));

			__WJR_MEMRCHR_FOUR(simd_t, 
				s - width * 3, 
				s - width * 2, 
				s - width, 
				s,
				q);

			s -= width * 4;
			n -= width * 4;
		} while (n >= width * 4);

		if (n == 0) {
			return s;
		}

		goto WJR_MACRO_LABEL(unaligned_last_4vec);
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s[-1], val) ? s : s - 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A0 = *reinterpret_cast<const uint64_t*>(s - n);
			auto B0 = *reinterpret_cast<const uint64_t*>(s - 4);

			auto x = simd::sse::set_epi64x(B0, A0);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0)return s - n;
			auto i = wjr::countl_zero(z) / _Mysize;
			auto q = i >= 4 ? i + (n - 8) : i;
			return s - q;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A0 = *reinterpret_cast<const uint32_t*>(s - n);
			auto B0 = *reinterpret_cast<const uint32_t*>(s - n + delta);
			auto C0 = *reinterpret_cast<const uint32_t*>(s - 4 - delta);
			auto D0 = *reinterpret_cast<const uint32_t*>(s - 4);

			auto x = simd::sse::set_epi32(D0, C0, B0, A0);
			auto y = simd::sse::set1(val, T());

			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0) return s - n;
			auto i = wjr::countl_zero(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s - q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (pred(s[-1], val)) return s;
		if (n == 1) return s - 1;
		const bool f = pred(s[1 - n], val);
		const bool g = pred(s[-n], val);
		const size_t i1 = g ? 1 - n : -n;
		const size_t i2 = f ? 2 - n : i1;
		return s + i2;
	}
}

_WJR_ALGO_END

#undef __WJR_MEMRCHR_FOUR
#undef __WJR_MEMRCHR_ONE

#endif // _WJR_FAST_MEMCHR