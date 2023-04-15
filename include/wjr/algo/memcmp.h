#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif

#if defined(_WJR_FAST_MEMCMP)

#define __WJR_MEMCMP_ONE_NORMAL(st)														\
	{	\
		auto r = st::cmp(x, y, pred, T());	                                                \
		st::mask_type z = st::movemask_epi8(r);		                                        \
		if(z != st::mask()){	                                                            \
			return false;	                                                                \
		}	\
	}

#define __WJR_MEMCMP_FOUR_NORMAL(st)													\
	{	\
		auto r0 = st::cmp(x0, y0, pred, T());	                                            \
		auto r1 = st::cmp(x1, y1, pred, T());	                                            \
		auto r2 = st::cmp(x2, y2, pred, T());	                                            \
		auto r3 = st::cmp(x3, y3, pred, T());	                                            \
																							\
		r3 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
		st::mask_type z = st::movemask_epi8(r3);		                                    \
		if(z != st::mask()){	                                                            \
			return false;	                                                                \
		}	\
	}

#define __WJR_MEMCMP_ONE(st) __WJR_MEMCMP_ONE_NORMAL(st)
#define __WJR_MEMCMP_FOUR(st) __WJR_MEMCMP_FOUR_NORMAL(st)

#if WJR_SSE4_1
#undef __WJR_MEMCMP_ONE
#undef __WJR_MEMCMP_FOUR

#define __WJR_MEMCMP_ONE(st)															\
	if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                            \
		auto r = st::Xor(x, y);	                                                        \
		auto z = st::test_all_zeros(r);	                                                \
		if(!z){	                                                                        \
			return false;	                                                            \
		}	                                                                            \
	}	                                                                                \
	else{	                                                                            \
		__WJR_MEMCMP_ONE_NORMAL(st)														\
	}

#define __WJR_MEMCMP_FOUR(st)															\
	if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                            \
		auto r0 = st::Xor(x0, y0);	                                                    \
		auto r1 = st::Xor(x1, y1);	                                                    \
		auto r2 = st::Xor(x2, y2);	                                                    \
		auto r3 = st::Xor(x3, y3);	                                                    \
				                                                                        \
		r3 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	                                \
		auto z = st::test_all_zeros(r3);	                                            \
		if(!z){	                                                                        \
			return false;	                                                            \
		}	                                                                            \
	}	                                                                                \
	else{	                                                                            \
		__WJR_MEMCMP_FOUR_NORMAL(st)													\
	}
#endif // WJR_SSE4_1

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
bool __memcmp(const T* s0, const T* s1, size_t n, _Pred pred) {
	constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // WJR_AVX2
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	if (is_constant_p(n) && n <= 64 / _Mysize) {
		return ::memcmp(s0, s1, n * _Mysize) == 0;
	}

	if (is_unlikely(n == 0)) return true;

	if (n >= 16 / _Mysize) {

		if (n <= 32 / _Mysize) {
			// solve first 16 bytes
			auto x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
			auto y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));

			__WJR_MEMCMP_ONE(simd::sse);

			// solve last 16 bytes

			s0 += n - 16 / _Mysize;
			s1 += n - 16 / _Mysize;

			x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
			y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));

			__WJR_MEMCMP_ONE(simd::sse);

			return true;
		}

		// solve first min(n, 128 / _Mysize) bytes
		// no branch algorithm

		{
			const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
			const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
			const auto negdelta = m - 32 / _Mysize;

#if WJR_AVX2
			auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0));
			auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + delta));
			auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + negdelta - delta));
			auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + negdelta));

			auto y0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1));
			auto y1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + delta));
			auto y2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + negdelta - delta));
			auto y3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + negdelta));

			__WJR_MEMCMP_FOUR(simd::avx);
#else
			{
				auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
				auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + 16 / _Mysize));
				auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + delta));
				auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + delta + 16 / _Mysize));

				auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));
				auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + 16 / _Mysize));
				auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + delta));
				auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + delta + 16 / _Mysize));

				__WJR_MEMCMP_FOUR(simd::sse);
			}

			{
				auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta - delta));
				auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta - delta + 16 / _Mysize));
				auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta));
				auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta + 16 / _Mysize));

				auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta - delta));
				auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta - delta + 16 / _Mysize));
				auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta));
				auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta + 16 / _Mysize));

				__WJR_MEMCMP_FOUR(simd::sse);
			}
#endif // WJR_AVX2

			// m = std::min(n, 128 / _Mysize) 
			// m == n -> n <= 128 / _Mysize
			if (m == n) {
				return true;
			}
		}

		if (n <= 128 / _Mysize + width * 4) {

			WJR_MACRO_LABEL(unaligned_last_4vec) :

			s0 += n;
			s1 += n;

			auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 4));
			auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 3));
			auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 2));
			auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

			auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 4));
			auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 3));
			auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 2));
			auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

			__WJR_MEMCMP_FOUR(simd_t);

			return true;
		}

		s0 += 128 / _Mysize;
		s1 += 128 / _Mysize;
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
					auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
					auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

					__WJR_MEMCMP_ONE(simd_t);

					const auto __align_s = bound - off0;
					s0 += __align_s / _Mysize;
					s1 += __align_s / _Mysize;
					n -= __align_s / _Mysize;

					if (is_unlikely(n < width * 4)) {
						goto WJR_MACRO_LABEL(last_solve_align_0);
					}
				}

				do {
					auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0));
					auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 + width));
					auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 2));
					auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 3));

					auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
					auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
					auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
					auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

					__WJR_MEMCMP_FOUR(simd_t);

					s0 += width * 4;
					s1 += width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				if (n != 0) {

					WJR_MACRO_LABEL(last_solve_align_0) :

					s0 += n;
					s1 += n;

					const auto ptr0 = reinterpret_cast<T*>(
						(reinterpret_cast<uintptr_t>(s0 - width * 3)) & (~(bound - 1)));
					const auto dif = s0 - ptr0;
					const auto ptr1 = s1 - dif;

					auto x0 = simd_t::load(reinterpret_cast<const sint*>(ptr0));
					auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width));
					auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

					auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1));
					auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 + width));
					auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 + width * 2));
					auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

					__WJR_MEMCMP_FOUR(simd_t);
				}

				return true;
			}

			// align two pointers
			if (is_constant_p(off0) && off0 == 0) {
				// do nothing
			}
			else {
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
				auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

				__WJR_MEMCMP_ONE(simd_t);

				const auto __align_s = bound - off0;
				s0 += __align_s / _Mysize;
				s1 += __align_s / _Mysize;
				n -= __align_s / _Mysize;

				if (is_unlikely(n < width * 4)) {
					goto WJR_MACRO_LABEL(last_solve_align_0_1);
				}
			}

			do {
				auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 + width));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 2));
				auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 3));

				auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 + width));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 2));
				auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 3));

				__WJR_MEMCMP_FOUR(simd_t);

				s0 += width * 4;
				s1 += width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			if (n != 0) {

				WJR_MACRO_LABEL(last_solve_align_0_1) :

				s0 += n;
				s1 += n;

				auto ptr0 = reinterpret_cast<T*>(
					(reinterpret_cast<uintptr_t>(s0 - width * 3)) & (~(bound - 1)));
				const auto dif = s0 - ptr0;
				const auto ptr1 = s1 - dif;

				auto x0 = simd_t::load(reinterpret_cast<const sint*>(ptr0));
				auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width));
				auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width * 2));
				auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

				auto y0 = simd_t::load(reinterpret_cast<const sint*>(ptr1));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(ptr1 + width));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(ptr1 + width * 2));
				auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

				__WJR_MEMCMP_FOUR(simd_t);
			}

			return true;
		}

		if (is_likely(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {

			const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;
			// only align second pointer
			if (is_constant_p(off1) && off1 == 0) {
				// do nothing
			}
			else {
				auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
				auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

				__WJR_MEMCMP_ONE(simd_t);

				const auto __align_s = bound - off1;
				s0 += __align_s / _Mysize;
				s1 += __align_s / _Mysize;
				n -= __align_s / _Mysize;

				if (is_unlikely(n < width * 4)) {
					goto WJR_MACRO_LABEL(last_solve_align_1);
				}
			}

			do {
				auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
				auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
				auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
				auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

				auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 + width));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 2));
				auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 3));

				__WJR_MEMCMP_FOUR(simd_t);

				s0 += width * 4;
				s1 += width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			if (n != 0) {

				WJR_MACRO_LABEL(last_solve_align_1) :

				s0 += n;
				s1 += n;

				const auto ptr1 = reinterpret_cast<T*>(
					(reinterpret_cast<uintptr_t>(s1 - width * 3)) & (~(bound - 1)));
				const auto dif = s1 - ptr1;
				const auto ptr0 = s0 - dif;

				auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0));
				auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 + width));
				auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 + width * 2));
				auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

				auto y0 = simd_t::load(reinterpret_cast<const sint*>(ptr1));
				auto y1 = simd_t::load(reinterpret_cast<const sint*>(ptr1 + width));
				auto y2 = simd_t::load(reinterpret_cast<const sint*>(ptr1 + width * 2));
				auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

				__WJR_MEMCMP_FOUR(simd_t);
			}

			return true;
		}

		// unaligned algorithm
		do {
			auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
			auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
			auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
			auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

			auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
			auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
			auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
			auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

			__WJR_MEMCMP_FOUR(simd_t);

			s0 += width * 4;
			s1 += width * 4;
			n -= width * 4;
		} while (n >= width * 4);

		if (n == 0) {
			return true;
		}

		goto WJR_MACRO_LABEL(unaligned_last_4vec);
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s0[0], s1[0]);
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A0 = *reinterpret_cast<const uint64_t*>(s0);
			auto B0 = *reinterpret_cast<const uint64_t*>(s0 + n - 4);

			auto A1 = *reinterpret_cast<const uint64_t*>(s1);
			auto B1 = *reinterpret_cast<const uint64_t*>(s1 + n - 4);

			auto x = simd::sse::set_epi64x(B0, A0);
			auto y = simd::sse::set_epi64x(B1, A1);

			__WJR_MEMCMP_ONE(simd::sse);

			return true;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A0 = *reinterpret_cast<const uint32_t*>(s0);
			auto B0 = *reinterpret_cast<const uint32_t*>(s0 + delta);
			auto C0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4 - delta);
			auto D0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4);

			auto A1 = *reinterpret_cast<const uint32_t*>(s1);
			auto B1 = *reinterpret_cast<const uint32_t*>(s1 + delta);
			auto C1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4 - delta);
			auto D1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4);

			auto x = simd::sse::set_epi32(D0, C0, B0, A0);
			auto y = simd::sse::set_epi32(D1, C1, B1, A1);

			__WJR_MEMCMP_ONE(simd::sse);

			return true;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (!pred(s0[0], s1[0])) return false;
		if (n == 1) return true;
		const bool f = pred(s0[n - 2], s1[n - 2]);
		const bool g = pred(s0[n - 1], s1[n - 1]);
		return f && g;
	}

}

_WJR_ALGO_END

#undef __WJR_MEMCMP_ONE
#undef __WJR_MEMCMP_FOUR
#undef __WJR_MEMCMP_ONE_NORMAL
#undef __WJR_MEMCMP_FOUR_NORMAL

#endif // _WJR_FAST_MEMCMP