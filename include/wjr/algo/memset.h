#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif 

#if defined(_WJR_FAST_MEMSET)

#if WJR_AVX2
#define __WJR_ALIGN64byte(ptr)	                                    \
	simd_t::storeu(reinterpret_cast<sint*>((ptr)), q);	            \
	simd_t::storeu(reinterpret_cast<sint*>((ptr) + width), q);
#else
#define __WJR_ALIGN64byte(ptr)	                                    \
	simd_t::storeu(reinterpret_cast<sint*>((ptr)), q);	            \
	simd_t::storeu(reinterpret_cast<sint*>((ptr) + width), q);	    \
	simd_t::storeu(reinterpret_cast<sint*>((ptr) + width * 2), q);	\
	simd_t::storeu(reinterpret_cast<sint*>((ptr) + width * 3), q);
#endif // __WJR_MEMSET64byte

_WJR_ALGO_BEGIN

template<typename T>
void __memset(T* s, T val, size_t n) {
	constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // WJR_AVX2

	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
	constexpr uintptr_t bound = width * _Mysize;

	static_assert(bound >= 16, "");
	static_assert(std::is_same_v<T, make_integral_t<T>>, "");

	if constexpr (_Mysize != 1) {
		if (
			is_constant_p(val) &&
			broadcast<T, uint8_t>(static_cast<uint8_t>(val)) == val) {
			return __memset(reinterpret_cast<uint8_t*>(s), static_cast<uint8_t>(val), n * _Mysize);
		}
	}

	constexpr size_t __constant_threshold = 2048 / _Mysize;
	// threshold for enhanced rep
	// enhanced rep will be faster when 64-byte memory is aligned
	// but it will be slower when 64-byte memory is not aligned
	// so we use a threshold to determine whether to use enhanced rep
	constexpr size_t __rep_align64byte_threshold = 1024 / _Mysize;

	const bool __use_rep = wjr::is_enhanced_rep();

#if defined(_WJR_NON_TEMPORARY)
	const size_t __nt_threshold = (wjr::max_cache_size() + wjr::max_cache_size() / 8) / _Mysize;
#endif // _WJR_NON_TEMPORARY

	if (is_constant_p(n) && n <= __constant_threshold) {
		for (size_t i = 0; i < n; ++i) {
			s[i] = val;
		}
		return;
	}

	if (n >= 16 / _Mysize) {
		auto qx = simd::sse::set1(val, T());

		if (n <= 32 / _Mysize) {
			simd::sse::storeu(reinterpret_cast<__m128i*>(s), qx);
			simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize)), qx);
			return;
		}

#if WJR_AVX2

		if (n < 128 / _Mysize) {
			auto qy = broadcast<simd::__m256i_tag, simd::__m128i_tag>(qx);
			auto delta = (n & (64 / _Mysize)) >> 1;
			simd::avx::storeu(reinterpret_cast<__m256i*>(s), qy);
			simd::avx::storeu(reinterpret_cast<__m256i*>(s + delta), qy);
			simd::avx::storeu(reinterpret_cast<__m256i*>(s + n - (32 / _Mysize) - delta), qy);
			simd::avx::storeu(reinterpret_cast<__m256i*>(s + n - (32 / _Mysize)), qy);
			return;
		}

#else

		if (n < 128 / _Mysize) {
			auto delta = (n & (64 / _Mysize)) >> 1;
			simd::sse::storeu(reinterpret_cast<__m128i*>(s), qx);
			simd::sse::storeu(reinterpret_cast<__m128i*>(s + (16 / _Mysize)), qx);
			simd::sse::storeu(reinterpret_cast<__m128i*>(s + delta), qx);
			simd::sse::storeu(reinterpret_cast<__m128i*>(s + delta + (16 / _Mysize)), qx);
			simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (32 / _Mysize) - delta), qx);
			simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize) - delta), qx);
			simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (32 / _Mysize)), qx);
			simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize)), qx);
			return;
		}
		
#endif // WJR_AVX2

		// [128, ...)
		auto q = broadcast<simd::__simd_wrapper_t<sint>, simd::__m128i_tag>(qx);

		if (is_likely(reinterpret_cast<uintptr_t>(s) % _Mysize == 0)) {

#if defined(_WJR_FAST_REP)
			if (__use_rep) {

				if (n < __rep_align64byte_threshold) {
					auto u64v = broadcast<uint64_t, T>(val);
					if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 8) &&
						reinterpret_cast<uintptr_t>(s) % 8 == 0) {
					}
					else {
						*reinterpret_cast<uint64_t*>(s) = u64v;
						auto __align_s = 8 - reinterpret_cast<uintptr_t>(s) % 8;
						s += __align_s / _Mysize;
						n -= __align_s / _Mysize;
					}
					*reinterpret_cast<uint64_t*>(s + n - 8 / _Mysize) = u64v;
					n &= -(8 / _Mysize);
					if constexpr (_Mysize == 1) {
						wjr::masm::rep_stosb(reinterpret_cast<uint8_t*>(s), val, n);
					}
					else {
						n /= (8 / _Mysize);
						wjr::masm::rep_stosq(reinterpret_cast<uint64_t*>(s), u64v, n);
					}
					return;
				}

				// align first 64 byte

				if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 64) &&
					reinterpret_cast<uintptr_t>(s) % 64 == 0) {
					// do nothing
				}
				else {
					__WJR_ALIGN64byte(s);
					auto __align_s = 64 - reinterpret_cast<uintptr_t>(s) % 64;
					s += __align_s / _Mysize;
					n -= __align_s / _Mysize;
				}

#if defined(_WJR_NON_TEMPORARY)
				if (n < __nt_threshold) {
#endif // _WJR_NON_TEMPORARY

					__WJR_ALIGN64byte(s + n - (64 / _Mysize));

					n = (n - 1) & (-(64 / _Mysize));
					if constexpr (_Mysize == 1) {
						wjr::masm::rep_stosb(reinterpret_cast<uint8_t*>(s), val, n);
					}
					else {
						n /= (8 / _Mysize);
						auto u64v = broadcast<uint64_t, T>(val);
						wjr::masm::rep_stosq(reinterpret_cast<uint64_t*>(s), u64v, n);
					}
					return;

#if defined(_WJR_NON_TEMPORARY)
				}
#endif // _WJR_NON_TEMPORARY

			}
			else {
#endif // _WJR_FAST_REP

#if defined(_WJR_NON_TEMPORARY)
				if (n < __nt_threshold) {
#endif // _WJR_NON_TEMPORARY

					if (is_constant_p(reinterpret_cast<uintptr_t>(s) % bound) &&
						reinterpret_cast<uintptr_t>(s) % bound == 0) {
						// do nothing
					}
					else {
						simd_t::storeu(reinterpret_cast<sint*>(s), q);
						auto __align_s = bound - reinterpret_cast<uintptr_t>(s) % bound;
						s += __align_s / _Mysize;
						n -= __align_s / _Mysize;
						if (n < width * 4) goto WJR_MACRO_LABEL(last_store);
					}

					do {
						simd_t::store(reinterpret_cast<sint*>(s), q);
						simd_t::store(reinterpret_cast<sint*>(s + width), q);
						simd_t::store(reinterpret_cast<sint*>(s + width * 2), q);
						simd_t::store(reinterpret_cast<sint*>(s + width * 3), q);
						s += width * 4;
						n -= width * 4;
					} while (n >= width * 4);
					
					goto WJR_MACRO_LABEL(last_store);

#if defined(_WJR_NON_TEMPORARY)
				}
#endif // _WJR_NON_TEMPORARY

#if defined(_WJR_FAST_REP)
			}
#endif // _WJR_FAST_REP

			// align first 64 byte

			if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 64) &&
				reinterpret_cast<uintptr_t>(s) % 64 == 0) {
				// do nothing
			}
			else {
				__WJR_ALIGN64byte(s);
				auto __align_s = 64 - reinterpret_cast<uintptr_t>(s) % 64;
				s += __align_s / _Mysize;
				n -= __align_s / _Mysize;
			}

			do {
				simd_t::stream(reinterpret_cast<sint*>(s), q);
				simd_t::stream(reinterpret_cast<sint*>(s + width), q);
				simd_t::stream(reinterpret_cast<sint*>(s + width * 2), q);
				simd_t::stream(reinterpret_cast<sint*>(s + width * 3), q);
				s += width * 4;
				n -= width * 4;
			} while (n >= width * 4);
			simd::sse::sfence();

			// store last (width * 4) byte

			WJR_MACRO_LABEL(last_store) :

			s += n;
			auto ptr = reinterpret_cast<T*>(
				(reinterpret_cast<uintptr_t>(s - width * 3)) & (~(bound - 1)));
			simd_t::store(reinterpret_cast<sint*>(ptr), q);
			simd_t::store(reinterpret_cast<sint*>(ptr + width), q);
			simd_t::store(reinterpret_cast<sint*>(ptr + width * 2), q);
			simd_t::storeu(reinterpret_cast<sint*>(s - width), q);

			return;
		}

		// non-aligned algorithm
		do {
			simd_t::storeu(reinterpret_cast<sint*>(s), q);
			simd_t::storeu(reinterpret_cast<sint*>(s + width), q);
			simd_t::storeu(reinterpret_cast<sint*>(s + width * 2), q);
			simd_t::storeu(reinterpret_cast<sint*>(s + width * 3), q);
			s += width * 4;
			n -= width * 4;
		} while (n >= width * 4);
		s += n;
		simd_t::storeu(reinterpret_cast<sint*>(s - width * 4), q);
		simd_t::storeu(reinterpret_cast<sint*>(s - width * 3), q);
		simd_t::storeu(reinterpret_cast<sint*>(s - width * 2), q);
		simd_t::storeu(reinterpret_cast<sint*>(s - width), q);
		return;
	}

	if constexpr (_Mysize == 8) {
		if (n != 0) {
			*s = val;
		}
		return;
	}

	if constexpr (_Mysize <= 4) {
		if (n >= 4 / _Mysize) {
			auto u32v = broadcast<uint32_t, T>(val);
			auto delta = (n & (8 / _Mysize)) >> 1;
			*reinterpret_cast<uint32_t*>(s) = u32v;
			*reinterpret_cast<uint32_t*>(s + delta) = u32v;
			if constexpr (_Mysize != 4) {
				*reinterpret_cast<uint32_t*>(s + n - (4 / _Mysize) - delta) = u32v;
			}
			*reinterpret_cast<uint32_t*>(s + n - (4 / _Mysize)) = u32v;
			return;
		}
	}

	if constexpr (_Mysize == 4) {
		return;
	}

	if constexpr (_Mysize == 2) {
		if (n != 0) *s = val;
		return;
	}

	if constexpr (_Mysize == 1) {
		if (n != 0) {
			*s = val;
			if (n != 1) {
				*reinterpret_cast<uint16_t*>(s + n - 2) = broadcast<uint16_t, uint8_t>(val);
			}
		}
		return;
	}
}

_WJR_ALGO_END

#undef __WJR_ALIGN64byte

#endif // _WJR_FAST_MEMSET