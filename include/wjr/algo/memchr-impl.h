// @WJR_MULTIPLE

// __WJR_MEMCHR_ONE
// __WJR_MEMCHR_FOUR
// __WJR_MEMCHR_NAME

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
WJR_NODISCARD WJR_PURE const T*
	WJR_MACRO_CONCAT(__large, __WJR_MEMCHR_NAME)(const T* s, T val, size_t n, _Pred pred) noexcept {

	constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // WJR_AVX2

	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	auto q = simd_t::set1(val, T());

	{
		const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
		const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
		const auto negdelta = m - 32 / _Mysize;

#if WJR_AVX2
		static_assert(std::is_same_v<simd_t, simd::avx>, "");

		{
			{
				WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s, delta));
				__WJR_MEMCHR_ONE(simd::avx, q, WJR_SIMD_ADD_PTR(s, delta));
			}
			{
				WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s, negdelta - delta));
				__WJR_MEMCHR_ONE(simd::avx, q, WJR_SIMD_ADD_PTR(s, negdelta - delta));
			}
			{
				WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s, negdelta));
				__WJR_MEMCHR_ONE(simd::avx, q, WJR_SIMD_ADD_PTR(s, negdelta));
			}

		}
#else
		static_assert(std::is_same_v<simd_t, simd::sse>, "");

		{
			{
				WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, delta));
				__WJR_MEMCHR_ONE(simd::sse, q, WJR_SIMD_ADD_PTR(s, delta));
			}
			{
				WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, delta + 16 / _Mysize));
				__WJR_MEMCHR_ONE(simd::sse, q, WJR_SIMD_ADD_PTR(s, delta + 16 / _Mysize));
			}
		}

		{
			WJR_SIMD_LOADU4(simd::sse, x, 
				WJR_SIMD_ADD_PTR(s, negdelta - delta),
				WJR_SIMD_ADD_PTR(s, negdelta - delta + 16 / _Mysize),
				WJR_SIMD_ADD_PTR(s, negdelta),
				WJR_SIMD_ADD_PTR(s, negdelta + 16 / _Mysize));

			__WJR_MEMCHR_FOUR(simd::sse, q,
				WJR_SIMD_ADD_PTR(s, negdelta - delta),
				WJR_SIMD_ADD_PTR(s, negdelta - delta + 16 / _Mysize),
				WJR_SIMD_ADD_PTR(s, negdelta),
				WJR_SIMD_ADD_PTR(s, negdelta + 16 / _Mysize));
		}
#endif // WJR_AVX2

		// m = std::min(n, 128 / _Mysize) 
		// m == n -> n <= 128 / _Mysize
		if (m == n) {
			return WJR_SIMD_ADD_PTR(s, n);
		}
	}

	if (n <= 128 / _Mysize + width * 4) {

		WJR_MACRO_LABEL(unaligned_last_4vec) :

			WJR_SIMD_INC_PTR(s, n);

		WJR_SIMD_LOADU4(simd_t, x,
			WJR_SIMD_SUB_PTR(s, width * 4),
			WJR_SIMD_SUB_PTR(s, width * 3),
			WJR_SIMD_SUB_PTR(s, width * 2),
			WJR_SIMD_SUB_PTR(s, width));

		__WJR_MEMCHR_FOUR(simd_t, q,
			WJR_SIMD_SUB_PTR(s, width * 4),
			WJR_SIMD_SUB_PTR(s, width * 3),
			WJR_SIMD_SUB_PTR(s, width * 2),
			WJR_SIMD_SUB_PTR(s, width));

		return s;
	}

	WJR_SIMD_INC_PTR(s, 128 / _Mysize);
	n -= 128 / _Mysize;

	// align

	if (is_likely(WJR_SIMD_UINTPTR(s) % _Mysize == 0)) {

		const auto __align_s = WJR_SIMD_CEIL_ALIGN_OFFSET(s, bound);

		// align 
		if (is_constant_p(__align_s) && __align_s == 0) {
			// do nothing
		}
		else {
			WJR_SIMD_LOADU(simd_t, x, s);

			__WJR_MEMCHR_ONE(simd_t, q, s);

			WJR_SIMD_INC_PTR(s, __align_s / _Mysize);
			n -= __align_s / _Mysize;

			if (is_unlikely(n < width * 4)) {
				goto WJR_MACRO_LABEL(last_solve_align);
			}
		}

		do {
			WJR_SIMD_LOAD4(simd_t, x,
				s,
				WJR_SIMD_ADD_PTR(s, width),
				WJR_SIMD_ADD_PTR(s, width * 2),
				WJR_SIMD_ADD_PTR(s, width * 3));

			__WJR_MEMCHR_FOUR(simd_t, q,
				s,
				WJR_SIMD_ADD_PTR(s, width),
				WJR_SIMD_ADD_PTR(s, width * 2),
				WJR_SIMD_ADD_PTR(s, width * 3));

			WJR_SIMD_INC_PTR(s, width * 4);
			n -= width * 4;
		} while (n >= width * 4);

		if (n != 0) {

			WJR_MACRO_LABEL(last_solve_align) :

				WJR_SIMD_INC_PTR(s, n);

			auto ptr0 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s, bound), width * 4);

			WJR_SIMD_LOAD(simd_t, x0, ptr0);
			WJR_SIMD_LOAD(simd_t, x1, WJR_SIMD_ADD_PTR(ptr0, width));
			WJR_SIMD_LOAD(simd_t, x2, WJR_SIMD_ADD_PTR(ptr0, width * 2));
			WJR_SIMD_LOADU(simd_t, x3, WJR_SIMD_SUB_PTR(s, width));

			__WJR_MEMCHR_FOUR(simd_t, q,
				ptr0,
				WJR_SIMD_ADD_PTR(ptr0, width),
				WJR_SIMD_ADD_PTR(ptr0, width * 2),
				WJR_SIMD_SUB_PTR(s, width));
		}

		return s;
	}

	// unaligned algorithm
	do {

		WJR_SIMD_LOADU4(simd_t, x,
			s,
			WJR_SIMD_ADD_PTR(s, width),
			WJR_SIMD_ADD_PTR(s, width * 2),
			WJR_SIMD_ADD_PTR(s, width * 3));

		__WJR_MEMCHR_FOUR(simd_t, q,
			s,
			WJR_SIMD_ADD_PTR(s, width),
			WJR_SIMD_ADD_PTR(s, width * 2),
			WJR_SIMD_ADD_PTR(s, width * 3));

		WJR_SIMD_INC_PTR(s, width * 4);
		n -= width * 4;
	} while (n >= width * 4);

	if (n != 0) {
		goto WJR_MACRO_LABEL(unaligned_last_4vec);

	}

	return s;
}

// inline this function, and call __large_memchr for ans pos > 32
// Doing so does not cause the code to become overly bloated, while also balancing performance
// But what's awkward is that MSVC doesn't inline it
template<typename T, typename _Pred>
WJR_NODISCARD WJR_PURE inline const T* (__WJR_MEMCHR_NAME)(const T* s, T val, size_t n, _Pred pred) noexcept {
	constexpr size_t _Mysize = sizeof(T);

	if (is_unlikely(n == 0)) return s;

	WJR_SIMD_INIT_PTR(s, n);

	if (n >= 16 / _Mysize) {
		auto qx = simd::sse::set1(val, T());

		{
			// solve first 16 bytes
			WJR_SIMD_LOADU(simd::sse, x, s);
			__WJR_MEMCHR_ONE(simd::sse, qx, s);
		}

		if (n <= 32 / _Mysize) {
			// solve last 16 bytes

			WJR_SIMD_INC_PTR(s, n - 16 / _Mysize);

			WJR_SIMD_LOADU(simd::sse, x, s);
			__WJR_MEMCHR_ONE(simd::sse, qx, s);

			return WJR_SIMD_ADD_PTR(s, 16 / _Mysize);
		}

		{
			// solve first 16 bytes
			WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, 16 / _Mysize));
			__WJR_MEMCHR_ONE(simd::sse, qx, WJR_SIMD_ADD_PTR(s, 16 / _Mysize));
		}

		return WJR_MACRO_CONCAT(__large, __WJR_MEMCHR_NAME)(s, val, n, pred);
	}

#if !WJR_SIMD_IS_BACKWARD
	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return pred(*s, val) ? s : s + 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A = *reinterpret_cast<const uint64_t*>(s);
			auto B = *reinterpret_cast<const uint64_t*>(s + (n - 4u));

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
			auto C = *reinterpret_cast<const uint32_t*>(s + (n - 4u - delta));
			auto D = *reinterpret_cast<const uint32_t*>(s + (n - 4u));

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
#else
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
		const bool f = pred(s[static_cast<size_t>(1 - n)], val);
		const bool g = pred(s[static_cast<size_t>(0 - n)], val);
		const size_t i1 = g ? 1 - n : -n;
		const size_t i2 = f ? 2 - n : i1;
		return s + i2;
	}
#endif //

}

_WJR_ALGO_END