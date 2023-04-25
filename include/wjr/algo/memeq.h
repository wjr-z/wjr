#ifndef __WJR_ALGO_MEM_ALL_H
#error "This file should not be included directly. Include <wjr/algo/mem-all.h> instead."
#endif // __WJR_ALGO_MEM_ALL_H

#if defined(_WJR_FAST_MEMEQ)

#define __WJR_MEMEQ_ONE_NORMAL(st)														    \
	{	                                                                                    \
		auto r = st::cmp(x, y, pred, T());	                                                \
		st::mask_type z = st::movemask_epi8(r);		                                        \
		if(z != st::mask()){	                                                            \
			return false;	                                                                \
		}	                                                                                \
	}

#define __WJR_MEMEQ_FOUR_NORMAL(st)													    \
	{	                                                                                    \
		auto r0 = st::cmp(x0, y0, pred, T());	                                            \
		auto r1 = st::cmp(x1, y1, pred, T());	                                            \
		auto r2 = st::cmp(x2, y2, pred, T());	                                            \
		auto r3 = st::cmp(x3, y3, pred, T());	                                            \
																							\
		r3 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
		st::mask_type z = st::movemask_epi8(r3);		                                    \
		if(z != st::mask()){	                                                            \
			return false;	                                                                \
		}	                                                                                \
	}

#define __WJR_MEMEQ_ONE(st) __WJR_MEMEQ_ONE_NORMAL(st)
#define __WJR_MEMEQ_FOUR(st) __WJR_MEMEQ_FOUR_NORMAL(st)

#if WJR_SSE4_1
#undef __WJR_MEMEQ_ONE
#undef __WJR_MEMEQ_FOUR

#define __WJR_MEMEQ_ONE(st)															\
	if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                            \
		auto r = st::Xor(x, y);	                                                        \
		auto z = st::test_all_zeros(r);	                                                \
		if(!z){	                                                                        \
			return false;	                                                            \
		}	                                                                            \
	}	                                                                                \
	else{	                                                                            \
		__WJR_MEMEQ_ONE_NORMAL(st)														\
	}

#define __WJR_MEMEQ_FOUR(st)															\
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
		__WJR_MEMEQ_FOUR_NORMAL(st)													\
	}
#endif // WJR_SSE4_1

_WJR_ALGO_BEGIN

#define WJR_SIMD_IS_BACKWARD 0

template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) bool __large__memeq(const T * s0, const T * s1, size_t n, _Pred pred) noexcept {
	constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // WJR_AVX2
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;
	// solve first min(n, 128 / _Mysize) bytes
		// no branch algorithm

	{
		const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
		const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
		const auto negdelta = m - 32 / _Mysize;

#if WJR_AVX2
		{
			WJR_SIMD_LOADU(simd::avx, x, s0 + delta);
			WJR_SIMD_LOADU(simd::avx, y, s1 + delta);

			__WJR_MEMEQ_ONE(simd::avx);
		}
		{
			WJR_SIMD_LOADU(simd::avx, x, s0 + negdelta - delta);
			WJR_SIMD_LOADU(simd::avx, y, s1 + negdelta - delta);

			__WJR_MEMEQ_ONE(simd::avx);
		}
		{
			WJR_SIMD_LOADU(simd::avx, x, s0 + negdelta);
			WJR_SIMD_LOADU(simd::avx, y, s1 + negdelta);

			__WJR_MEMEQ_ONE(simd::avx);
		}
#else
		{
			WJR_SIMD_LOADU(simd::sse, x, s0 + delta);
			WJR_SIMD_LOADU(simd::sse, y, s1 + delta);

			__WJR_MEMEQ_ONE(simd::sse);
		}
		{
			WJR_SIMD_LOADU(simd::sse, x, s0 + delta + 16 / _Mysize);
			WJR_SIMD_LOADU(simd::sse, y, s1 + delta + 16 / _Mysize);

			__WJR_MEMEQ_ONE(simd::sse);
		}

		{
			WJR_SIMD_LOADU4(simd::sse, x,
				s0 + negdelta - delta,
				s0 + negdelta - delta + 16 / _Mysize,
				s0 + negdelta,
				s0 + negdelta + 16 / _Mysize);

			WJR_SIMD_LOADU4(simd::sse, y,
				s1 + negdelta - delta,
				s1 + negdelta - delta + 16 / _Mysize,
				s1 + negdelta,
				s1 + negdelta + 16 / _Mysize);

			__WJR_MEMEQ_FOUR(simd::sse);
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

		WJR_SIMD_LOADU4(simd_t, x,
			s0 - width * 4,
			s0 - width * 3,
			s0 - width * 2,
			s0 - width);

		WJR_SIMD_LOADU4(simd_t, y,
			s1 - width * 4,
			s1 - width * 3,
			s1 - width * 2,
			s1 - width);

		__WJR_MEMEQ_FOUR(simd_t);

		return true;
	}

	s0 += 128 / _Mysize;
	s1 += 128 / _Mysize;
	n -= 128 / _Mysize;

	// align

	if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s0) % _Mysize == 0)) {

		const auto off0 = reinterpret_cast<uintptr_t>(s0) % bound;
		const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;

		if (off0 != off1) {
			// only align first pointer

			if (is_constant_p(off0) && off0 == 0) {
				// do nothing
			}
			else {
				WJR_SIMD_LOADU(simd_t, x, s0);
				WJR_SIMD_LOADU(simd_t, y, s1);

				__WJR_MEMEQ_ONE(simd_t);

				const auto __align_s = bound - off0;
				s0 += __align_s / _Mysize;
				s1 += __align_s / _Mysize;
				n -= __align_s / _Mysize;

				if (WJR_UNLIKELY(n < width * 4)) {
					goto WJR_MACRO_LABEL(last_solve_align_0);
				}
			}

			do {
				WJR_SIMD_LOAD4(simd_t, x, s0, s0 + width, s0 + width * 2, s0 + width * 3);
				WJR_SIMD_LOADU4(simd_t, y, s1, s1 + width, s1 + width * 2, s1 + width * 3);

				__WJR_MEMEQ_FOUR(simd_t);

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

				WJR_SIMD_LOADU4(simd_t, y, ptr1, ptr1 + width, ptr1 + width * 2, s1 - width);

				__WJR_MEMEQ_FOUR(simd_t);
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

			__WJR_MEMEQ_ONE(simd_t);

			const auto __align_s = bound - off0;
			s0 += __align_s / _Mysize;
			s1 += __align_s / _Mysize;
			n -= __align_s / _Mysize;

			if (WJR_UNLIKELY(n < width * 4)) {
				goto WJR_MACRO_LABEL(last_solve_align_0_1);
			}
		}

		do {
			WJR_SIMD_LOAD4(simd_t, x, s0, s0 + width, s0 + width * 2, s0 + width * 3);
			WJR_SIMD_LOAD4(simd_t, y, s1, s1 + width, s1 + width * 2, s1 + width * 3);

			__WJR_MEMEQ_FOUR(simd_t);

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

			__WJR_MEMEQ_FOUR(simd_t);
		}

		return true;
	}

	if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {

		const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;
		// only align second pointer
		if (is_constant_p(off1) && off1 == 0) {
			// do nothing
		}
		else {
			auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
			auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

			__WJR_MEMEQ_ONE(simd_t);

			const auto __align_s = bound - off1;
			s0 += __align_s / _Mysize;
			s1 += __align_s / _Mysize;
			n -= __align_s / _Mysize;

			if (WJR_UNLIKELY(n < width * 4)) {
				goto WJR_MACRO_LABEL(last_solve_align_1);
			}
		}

		do {
			WJR_SIMD_LOADU4(simd_t, x, s0, s0 + width, s0 + width * 2, s0 + width * 3);
			WJR_SIMD_LOAD4(simd_t, y, s1, s1 + width, s1 + width * 2, s1 + width * 3);

			__WJR_MEMEQ_FOUR(simd_t);

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

			__WJR_MEMEQ_FOUR(simd_t);
		}

		return true;
	}

	// unaligned algorithm
	do {
		WJR_SIMD_LOADU4(simd_t, x, s0, s0 + width, s0 + width * 2, s0 + width * 3);
		WJR_SIMD_LOADU4(simd_t, y, s1, s1 + width, s1 + width * 2, s1 + width * 3);

		__WJR_MEMEQ_FOUR(simd_t);

		s0 += width * 4;
		s1 += width * 4;
		n -= width * 4;
	} while (n >= width * 4);

	if (n == 0) {
		return true;
	}

	goto WJR_MACRO_LABEL(unaligned_last_4vec);
}

extern template bool __large__memeq(const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
extern template bool __large__memeq(const uint8_t*, const uint8_t*, size_t, std::not_equal_to<>) noexcept;

template<typename T, typename _Pred>
WJR_NODISCARD WJR_PURE inline bool __memeq(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {

	constexpr size_t _Mysize = sizeof(T);

	if (is_constant_p(n) && n <= 64 / _Mysize) {
		return ::memcmp(s0, s1, n * _Mysize) == 0;
	}

	if (WJR_UNLIKELY(n == 0)) return true;

	if (n >= 16 / _Mysize) {

		// solve first 16 bytes
		{
			WJR_SIMD_LOADU(simd::sse, x, s0);
			WJR_SIMD_LOADU(simd::sse, y, s1);

			__WJR_MEMEQ_ONE(simd::sse);
		}

		{
			auto m = n <= 32 / _Mysize ? (n - 16 / _Mysize) : (16 / _Mysize);

			{
				WJR_SIMD_LOADU(simd::sse, x, s0 + m);
				WJR_SIMD_LOADU(simd::sse, y, s1 + m);

				__WJR_MEMEQ_ONE(simd::sse);
			}

			if (n <= 32 / _Mysize) {
				return WJR_SIMD_ADD_PTR(s0, n);
			}
		}

		return __large__memeq(s0, s1, n, pred);
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s0[0], s1[0]);
	}

	constexpr auto __need_byteswap = !wjr::is_any_of_v<_Pred,
		std::equal_to<>,
		std::not_equal_to<>
	>;

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A0 = *reinterpret_cast<const uint64_t*>(s0);
			auto B0 = *reinterpret_cast<const uint64_t*>(s0 + n - 4);

			auto A1 = *reinterpret_cast<const uint64_t*>(s1);
			auto B1 = *reinterpret_cast<const uint64_t*>(s1 + n - 4);

			if constexpr (__need_byteswap) {
				A0 = wjr::byteswap(A0);
				B0 = wjr::byteswap(B0);
				A1 = wjr::byteswap(A1);
				B1 = wjr::byteswap(B1);
			}

			return pred(A0, A1) && pred(B0, B1);
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

			if constexpr (__need_byteswap) {
				A0 = wjr::byteswap(A0);
				B0 = wjr::byteswap(B0);
				C0 = wjr::byteswap(C0);
				D0 = wjr::byteswap(D0);
				A1 = wjr::byteswap(A1);
				B1 = wjr::byteswap(B1);
				C1 = wjr::byteswap(C1);
				D1 = wjr::byteswap(D1);
			}

			return (pred(A0, A1) && pred(B0, B1)) && (pred(C0, C1) && pred(D0, D1));
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (!pred(s0[0], s1[0])) return false;
		if (n == 1) return true;
		auto A = *reinterpret_cast<const uint16_t*>(s0 + n - 2);
		auto B = *reinterpret_cast<const uint16_t*>(s1 + n - 2);

		if constexpr (__need_byteswap) {
			A = wjr::byteswap(A);
			B = wjr::byteswap(B);
		}

		return pred(A, B);
	}

}

#undef WJR_SIMD_IS_BACKWARD

_WJR_ALGO_END

#undef __WJR_MEMEQ_ONE
#undef __WJR_MEMEQ_FOUR
#undef __WJR_MEMEQ_ONE_NORMAL
#undef __WJR_MEMEQ_FOUR_NORMAL

#endif // _WJR_FAST_MEMEQ