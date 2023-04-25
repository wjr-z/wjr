// @WJR_MULTIPLE

// __WJR_MEMMIS_ONE
// __WJR_MEMMIS_FOUR
// __WJR_MEMMIS_NAME

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T*
WJR_MACRO_CONCAT(__large, __WJR_MEMMIS_NAME)(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
	constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // WJR_AVX2

	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	{
		auto m = n <= 32 / _Mysize ? (n - 16 / _Mysize) : (16 / _Mysize);

		{
			WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s0, m));
			WJR_SIMD_LOADU(simd::sse, y, WJR_SIMD_ADD_PTR(s1, m));

			__WJR_MEMMIS_ONE(simd::sse, WJR_SIMD_ADD_PTR(s0, m));
		}

		if (n <= 32 / _Mysize) {
			return WJR_SIMD_ADD_PTR(s0, n);
		}
	}

	// solve first min(n, 128 / _Mysize) bytes
	// no branch algorithm

	{
		const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
		const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
		const auto negdelta = m - 32 / _Mysize;

#if WJR_AVX2
		{
			{
				WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s0, delta));
				WJR_SIMD_LOADU(simd::avx, y, WJR_SIMD_ADD_PTR(s1, delta));
				__WJR_MEMMIS_ONE(simd::avx, WJR_SIMD_ADD_PTR(s0, delta));
			}
			{
				WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s0, negdelta - delta));
				WJR_SIMD_LOADU(simd::avx, y, WJR_SIMD_ADD_PTR(s1, negdelta - delta));
				__WJR_MEMMIS_ONE(simd::avx, WJR_SIMD_ADD_PTR(s0, negdelta - delta));
			}
			{
				WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s0, negdelta));
				WJR_SIMD_LOADU(simd::avx, y, WJR_SIMD_ADD_PTR(s1, negdelta));
				__WJR_MEMMIS_ONE(simd::avx, WJR_SIMD_ADD_PTR(s0, negdelta));
			}
		}
#else
		{
			{
				WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s0, delta));
				WJR_SIMD_LOADU(simd::sse, y, WJR_SIMD_ADD_PTR(s1, delta));
				__WJR_MEMMIS_ONE(simd::sse, WJR_SIMD_ADD_PTR(s0, delta));
			}
			{
				WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s0, delta + 16 / _Mysize));
				WJR_SIMD_LOADU(simd::sse, y, WJR_SIMD_ADD_PTR(s1, delta + 16 / _Mysize));
				__WJR_MEMMIS_ONE(simd::sse, WJR_SIMD_ADD_PTR(s0, delta + 16 / _Mysize));
			}
		}

		{
			WJR_SIMD_LOADU4(simd::sse, x,
				WJR_SIMD_ADD_PTR(s0, negdelta - delta),
				WJR_SIMD_ADD_PTR(s0, negdelta - delta + 16 / _Mysize),
				WJR_SIMD_ADD_PTR(s0, negdelta),
				WJR_SIMD_ADD_PTR(s0, negdelta + 16 / _Mysize));
			WJR_SIMD_LOADU4(simd::sse, y,
				WJR_SIMD_ADD_PTR(s1, negdelta - delta),
				WJR_SIMD_ADD_PTR(s1, negdelta - delta + 16 / _Mysize),
				WJR_SIMD_ADD_PTR(s1, negdelta),
				WJR_SIMD_ADD_PTR(s1, negdelta + 16 / _Mysize));

			__WJR_MEMMIS_FOUR(simd::sse,
				WJR_SIMD_ADD_PTR(s0, negdelta - delta),
				WJR_SIMD_ADD_PTR(s0, negdelta - delta + 16 / _Mysize),
				WJR_SIMD_ADD_PTR(s0, negdelta),
				WJR_SIMD_ADD_PTR(s0, negdelta + 16 / _Mysize));
		}
#endif // WJR_AVX2

		// m = std::min(n, 128 / _Mysize) 
		// m == n -> n <= 128 / _Mysize
		if (m == n) {
			return WJR_SIMD_ADD_PTR(s0, n);
		}
	}

	if (n <= 128 / _Mysize + width * 4) {

		WJR_MACRO_LABEL(unaligned_last_4vec) :

			WJR_SIMD_INC_PTR(s0, n);
		WJR_SIMD_INC_PTR(s1, n);

		WJR_SIMD_LOADU4(simd_t, x,
			WJR_SIMD_SUB_PTR(s0, width * 4),
			WJR_SIMD_SUB_PTR(s0, width * 3),
			WJR_SIMD_SUB_PTR(s0, width * 2),
			WJR_SIMD_SUB_PTR(s0, width));

		WJR_SIMD_LOADU4(simd_t, y,
			WJR_SIMD_SUB_PTR(s1, width * 4),
			WJR_SIMD_SUB_PTR(s1, width * 3),
			WJR_SIMD_SUB_PTR(s1, width * 2),
			WJR_SIMD_SUB_PTR(s1, width));

		__WJR_MEMMIS_FOUR(simd_t,
			WJR_SIMD_SUB_PTR(s0, width * 4),
			WJR_SIMD_SUB_PTR(s0, width * 3),
			WJR_SIMD_SUB_PTR(s0, width * 2),
			WJR_SIMD_SUB_PTR(s0, width));

		return s0;
	}

	WJR_SIMD_INC_PTR(s0, 128 / _Mysize);
	WJR_SIMD_INC_PTR(s1, 128 / _Mysize);
	n -= 128 / _Mysize;

	// align

	if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s0) % _Mysize == 0)) {

		const auto __align_s0 = WJR_SIMD_CEIL_ALIGN_OFFSET(s0, bound);
		const auto __align_s1 = WJR_SIMD_CEIL_ALIGN_OFFSET(s1, bound);

		if (__align_s0 != __align_s1) {
			// only align first pointer

			if (is_constant_p(__align_s0) && __align_s0 == 0) {
				// do nothing
			}
			else {

				WJR_SIMD_LOADU(simd_t, x, s0);
				WJR_SIMD_LOADU(simd_t, y, s1);

				__WJR_MEMMIS_ONE(simd_t, s0);

				WJR_SIMD_INC_PTR(s0, __align_s0 / _Mysize);
				WJR_SIMD_INC_PTR(s1, __align_s0 / _Mysize);
				n -= __align_s0 / _Mysize;

				if (WJR_UNLIKELY(n < width * 4)) {
					goto WJR_MACRO_LABEL(last_solve_align_0);
				}
			}

			do {
				WJR_SIMD_LOAD4(simd_t, x,
					s0,
					WJR_SIMD_ADD_PTR(s0, width),
					WJR_SIMD_ADD_PTR(s0, width * 2),
					WJR_SIMD_ADD_PTR(s0, width * 3));

				WJR_SIMD_LOADU4(simd_t, y,
					s1,
					WJR_SIMD_ADD_PTR(s1, width),
					WJR_SIMD_ADD_PTR(s1, width * 2),
					WJR_SIMD_ADD_PTR(s1, width * 3));

				__WJR_MEMMIS_FOUR(simd_t,
					s0,
					WJR_SIMD_ADD_PTR(s0, width),
					WJR_SIMD_ADD_PTR(s0, width * 2),
					WJR_SIMD_ADD_PTR(s0, width * 3));

				WJR_SIMD_INC_PTR(s0, width * 4);
				WJR_SIMD_INC_PTR(s1, width * 4);
				n -= width * 4;
			} while (n >= width * 4);

			if (n != 0) {

				WJR_MACRO_LABEL(last_solve_align_0) :

					WJR_SIMD_INC_PTR(s0, n);
				WJR_SIMD_INC_PTR(s1, n);

				const auto ptr0 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s0, bound), width * 4);
				const auto dif = s0 - ptr0;
				const auto ptr1 = s1 - dif;

				WJR_SIMD_LOAD(simd_t, x0, ptr0);
				WJR_SIMD_LOAD(simd_t, x1, WJR_SIMD_ADD_PTR(ptr0, width));
				WJR_SIMD_LOAD(simd_t, x2, WJR_SIMD_ADD_PTR(ptr0, width * 2));
				WJR_SIMD_LOADU(simd_t, x3, WJR_SIMD_SUB_PTR(s0, width));

				WJR_SIMD_LOADU4(simd_t, y,
					ptr1,
					WJR_SIMD_ADD_PTR(ptr1, width),
					WJR_SIMD_ADD_PTR(ptr1, width * 2),
					WJR_SIMD_SUB_PTR(s1, width));

				__WJR_MEMMIS_FOUR(simd_t,
					ptr0,
					WJR_SIMD_ADD_PTR(ptr0, width),
					WJR_SIMD_ADD_PTR(ptr0, width * 2),
					WJR_SIMD_SUB_PTR(s0, width));
			}

			return s0;
		}

		// align two pointers
		if (is_constant_p(__align_s0) && __align_s0 == 0) {
			// do nothing
		}
		else {
			WJR_SIMD_LOADU(simd_t, x, s0);
			WJR_SIMD_LOADU(simd_t, y, s1);

			__WJR_MEMMIS_ONE(simd_t, s0);

			WJR_SIMD_INC_PTR(s0, __align_s0 / _Mysize);
			WJR_SIMD_INC_PTR(s1, __align_s0 / _Mysize);
			n -= __align_s0 / _Mysize;

			if (WJR_UNLIKELY(n < width * 4)) {
				goto WJR_MACRO_LABEL(last_solve_align_0_1);
			}
		}

		do {
			WJR_SIMD_LOAD4(simd_t, x,
				s0,
				WJR_SIMD_ADD_PTR(s0, width),
				WJR_SIMD_ADD_PTR(s0, width * 2),
				WJR_SIMD_ADD_PTR(s0, width * 3));

			WJR_SIMD_LOAD4(simd_t, y,
				s1,
				WJR_SIMD_ADD_PTR(s1, width),
				WJR_SIMD_ADD_PTR(s1, width * 2),
				WJR_SIMD_ADD_PTR(s1, width * 3));

			__WJR_MEMMIS_FOUR(simd_t,
				s0,
				WJR_SIMD_ADD_PTR(s0, width),
				WJR_SIMD_ADD_PTR(s0, width * 2),
				WJR_SIMD_ADD_PTR(s0, width * 3));

			WJR_SIMD_INC_PTR(s0, width * 4);
			WJR_SIMD_INC_PTR(s1, width * 4);
			n -= width * 4;
		} while (n >= width * 4);

		if (n != 0) {

			WJR_MACRO_LABEL(last_solve_align_0_1) :

				WJR_SIMD_INC_PTR(s0, n);
			WJR_SIMD_INC_PTR(s1, n);

			const auto ptr0 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s0, bound), width * 4);
			const auto dif = s0 - ptr0;
			const auto ptr1 = s1 - dif;

			WJR_SIMD_LOAD(simd_t, x0, ptr0);
			WJR_SIMD_LOAD(simd_t, x1, WJR_SIMD_ADD_PTR(ptr0, width));
			WJR_SIMD_LOAD(simd_t, x2, WJR_SIMD_ADD_PTR(ptr0, width * 2));
			WJR_SIMD_LOADU(simd_t, x3, WJR_SIMD_SUB_PTR(s0, width));

			WJR_SIMD_LOAD(simd_t, y0, ptr1);
			WJR_SIMD_LOAD(simd_t, y1, WJR_SIMD_ADD_PTR(ptr1, width));
			WJR_SIMD_LOAD(simd_t, y2, WJR_SIMD_ADD_PTR(ptr1, width * 2));
			WJR_SIMD_LOADU(simd_t, y3, WJR_SIMD_SUB_PTR(s1, width));

			__WJR_MEMMIS_FOUR(simd_t,
				ptr0,
				WJR_SIMD_ADD_PTR(ptr0, width),
				WJR_SIMD_ADD_PTR(ptr0, width * 2),
				WJR_SIMD_SUB_PTR(s0, width));
		}

		return s0;
	}

	if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {

		const auto __align_s1 = WJR_SIMD_CEIL_ALIGN_OFFSET(s1, bound);
		// only align second pointer
		if (is_constant_p(__align_s1) && __align_s1 == 0) {
			// do nothing
		}
		else {
			WJR_SIMD_LOADU(simd_t, x, s0);
			WJR_SIMD_LOADU(simd_t, y, s1);

			__WJR_MEMMIS_ONE(simd_t, s0);

			WJR_SIMD_INC_PTR(s0, __align_s1 / _Mysize);
			WJR_SIMD_INC_PTR(s1, __align_s1 / _Mysize);
			n -= __align_s1 / _Mysize;

			if (WJR_UNLIKELY(n < width * 4)) {
				goto WJR_MACRO_LABEL(last_solve_align_1);
			}
		}

		do {
			WJR_SIMD_LOADU4(simd_t, x,
				s0,
				WJR_SIMD_ADD_PTR(s0, width),
				WJR_SIMD_ADD_PTR(s0, width * 2),
				WJR_SIMD_ADD_PTR(s0, width * 3));

			WJR_SIMD_LOAD4(simd_t, y,
				s1,
				WJR_SIMD_ADD_PTR(s1, width),
				WJR_SIMD_ADD_PTR(s1, width * 2),
				WJR_SIMD_ADD_PTR(s1, width * 3));

			__WJR_MEMMIS_FOUR(simd_t,
				s0,
				WJR_SIMD_ADD_PTR(s0, width),
				WJR_SIMD_ADD_PTR(s0, width * 2),
				WJR_SIMD_ADD_PTR(s0, width * 3));

			WJR_SIMD_INC_PTR(s0, width * 4);
			WJR_SIMD_INC_PTR(s1, width * 4);
			n -= width * 4;
		} while (n >= width * 4);

		if (n != 0) {

			WJR_MACRO_LABEL(last_solve_align_1) :

				WJR_SIMD_INC_PTR(s0, n);
			WJR_SIMD_INC_PTR(s1, n);

			const auto ptr1 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s1, bound), width * 4);
			const auto dif = s1 - ptr1;
			const auto ptr0 = s0 - dif;

			WJR_SIMD_LOADU4(simd_t, x,
				ptr0,
				WJR_SIMD_ADD_PTR(ptr0, width),
				WJR_SIMD_ADD_PTR(ptr0, width * 2),
				WJR_SIMD_SUB_PTR(s0, width));

			WJR_SIMD_LOAD(simd_t, y0, ptr1);
			WJR_SIMD_LOAD(simd_t, y1, WJR_SIMD_ADD_PTR(ptr1, width));
			WJR_SIMD_LOAD(simd_t, y2, WJR_SIMD_ADD_PTR(ptr1, width * 2));
			WJR_SIMD_LOADU(simd_t, y3, WJR_SIMD_SUB_PTR(s1, width));

			__WJR_MEMMIS_FOUR(simd_t,
				ptr0,
				WJR_SIMD_ADD_PTR(ptr0, width),
				WJR_SIMD_ADD_PTR(ptr0, width * 2),
				WJR_SIMD_SUB_PTR(s0, width));
		}

		return s0;
	}

	// unaligned algorithm
	do {
		WJR_SIMD_LOADU4(simd_t, x,
			s0,
			WJR_SIMD_ADD_PTR(s0, width),
			WJR_SIMD_ADD_PTR(s0, width * 2),
			WJR_SIMD_ADD_PTR(s0, width * 3));

		WJR_SIMD_LOADU4(simd_t, y,
			s1,
			WJR_SIMD_ADD_PTR(s1, width),
			WJR_SIMD_ADD_PTR(s1, width * 2),
			WJR_SIMD_ADD_PTR(s1, width * 3));

		__WJR_MEMMIS_FOUR(simd_t,
			s0,
			WJR_SIMD_ADD_PTR(s0, width),
			WJR_SIMD_ADD_PTR(s0, width * 2),
			WJR_SIMD_ADD_PTR(s0, width * 3));

		WJR_SIMD_INC_PTR(s0, width * 4);
		WJR_SIMD_INC_PTR(s1, width * 4);
		n -= width * 4;
	} while (n >= width * 4);

	if (n == 0) {
		return s0;
	}

	goto WJR_MACRO_LABEL(unaligned_last_4vec);

}

template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T*
WJR_MACRO_CONCAT(__small, __WJR_MEMMIS_NAME)(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
	constexpr auto _Mysize = sizeof(T);
#if !WJR_SIMD_IS_BACKWARD
	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(s0[0], s1[0]) ? s0 + 1 : s0;
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
			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == simd::sse::mask())return s0 + n;
			auto i = wjr::countr_one(z) / _Mysize;
			auto q = i >= 4 ? i + (n - 8) : i;
			return s0 + q;
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

			auto r = simd::sse::cmp(x, y, pred, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == simd::sse::mask()) return s0 + n;
			auto i = wjr::countr_one(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s0 + q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (!pred(s0[0], s1[0])) return s0;
		if (n == 1) return s0 + 1;
		const bool f = pred(s0[n - 2], s1[n - 2]);
		const bool g = pred(s0[n - 1], s1[n - 1]);
		const size_t i1 = g ? n : n - 1;
		const size_t i2 = f ? i1 : n - 2;
		return s0 + i2;
	}
#else
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
			const auto delta = (n & 8) >> 1;

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
		const bool f = pred(s0[static_cast<size_t>(1 - n)], s1[static_cast<size_t>(1 - n)]);
		const bool g = pred(s0[static_cast<size_t>(0 - n)], s1[static_cast<size_t>(0 - n)]);
		const size_t i1 = g ? -n : 1 - n;
		const size_t i2 = f ? i1 : 2 - n;
		return s0 + i2;
	}
#endif // WJR_SIMD_IS_BACKWARD
}

template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T*
__WJR_MEMMIS_NAME(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
	constexpr size_t _Mysize = sizeof(T);

	if (WJR_UNLIKELY(n == 0)) return s0;

	WJR_SIMD_INIT_PTR(s0, n);
	WJR_SIMD_INIT_PTR(s1, n);

	if (n >= 16 / _Mysize) {

		// solve first 16 bytes
		{
			WJR_SIMD_LOADU(simd::sse, x, s0);
			WJR_SIMD_LOADU(simd::sse, y, s1);

			__WJR_MEMMIS_ONE(simd::sse, s0);
		}

		return WJR_MACRO_CONCAT(__large, __WJR_MEMMIS_NAME)(s0, s1, n, pred);
	}

	return WJR_MACRO_CONCAT(__small, __WJR_MEMMIS_NAME)(s0, s1, n, pred);
}

_WJR_ALGO_END