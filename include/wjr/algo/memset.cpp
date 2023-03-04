#include <wjr/algo/memset.h>
#include <wjr/literals.h>
#include <wjr/simd/simd_intrin.h>

#if defined(_WJR_FAST_MEMSET)

_WJR_ALGO_BEGIN

template<typename T>
void __memset_helper(T* s, T val, size_t n) {
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

	if constexpr (_Mysize != 1) {
		if (
			is_constant_p(val) &&
			broadcast<T, uint8_t>(static_cast<uint8_t>(val)) == val) {
			return __memset(reinterpret_cast<uint8_t*>(s), static_cast<uint8_t>(val), n * _Mysize);
		}
	}

	constexpr size_t __constant_threshold = 2_KiB / _Mysize;
	constexpr size_t __rep_noaligned_threshold = 1_KiB / _Mysize;

#if defined(WJR_INLINE_ASM)
	const bool __use_rep = wjr::is_enhanced_rep();
#else
	const bool __use_rep = false;
#endif

	const size_t __nt_threshold = wjr::max_cache_size / _Mysize;

	const size_t __large_threshold = __use_rep ? 256 / _Mysize : __nt_threshold;

	if (is_constant_p(n) && n <= __constant_threshold) {
		for (size_t i = 0; i < n; ++i) {
			s[i] = val;
		}
		return;
	}

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			auto q = simd_t::set1(val, T());

			// non-temporal algorithm
			if (_Mysize == 1 ||
				(is_likely(reinterpret_cast<uintptr_t>(s) % _Mysize == 0))) {

				if (is_unlikely(n >= __large_threshold)) {

#if defined(WJR_INLINE_ASM)
					auto u64v = broadcast<uint64_t, T>(val);
					if (__use_rep && n < __rep_noaligned_threshold) {
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
						n /= (8 / _Mysize);
						asm volatile(
							"rep stosq"
							: "+D"(s), "+c"(n)
							: "a"(u64v)
							: "memory"
							);
						return;
					}
#endif // WJR_INLINE_ASM

					// align(64)
					if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 64) &&
						reinterpret_cast<uintptr_t>(s) % 64 == 0) {
						// do nothing
					}
					else {
#if defined(__AVX2__)
						simd_t::storeu(reinterpret_cast<sint*>(s), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width), q);
#else // SSE2
						simd_t::storeu(reinterpret_cast<sint*>(s), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width * 2), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width * 3), q);
#endif // __AVX2__
						auto __align_s = 64 - reinterpret_cast<uintptr_t>(s) % 64;
						s += __align_s / _Mysize;
						n -= __align_s / _Mysize;
					}

#if defined(WJR_INLINE_ASM)
					if (__use_rep && n < __nt_threshold) {

#if defined(__AVX2__)
						simd_t::storeu(reinterpret_cast<sint*>(s + n - width * 2), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + n - width), q);
#else // SSE2
						simd_t::storeu(reinterpret_cast<sint*>(s + n - width * 4), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + n - width * 3), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + n - width * 2), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + n - width), q);
#endif // __AVX2__

						n &= -(64 / _Mysize);
						n /= (8 / _Mysize);
						asm volatile(
							"rep stosq"
							: "+D"(s), "+c"(n)
							: "a"(broadcast<uint64_t, T>(val))
							: "memory"
							);
						return;
					}
#endif

					do {
						simd_t::stream(reinterpret_cast<sint*>(s), q);
						simd_t::stream(reinterpret_cast<sint*>(s + width), q);
						simd_t::stream(reinterpret_cast<sint*>(s + width * 2), q);
						simd_t::stream(reinterpret_cast<sint*>(s + width * 3), q);
						s += width * 4;
						n -= width * 4;
					} while (n >= width * 4);
					simd::sse::sfence();
					goto WJR_MACRO_LABEL(aft_align);
				}
			}

			// align algorithm
			if (is_constant_p(reinterpret_cast<uintptr_t>(s) % bound) &&
				reinterpret_cast<uintptr_t>(s) % bound == 0) {
				// do nothing
			}
			else if (_Mysize == 1 ||
				is_likely(reinterpret_cast<uintptr_t>(s) % _Mysize == 0)) {
				simd_t::storeu(reinterpret_cast<sint*>(s), q);
				auto __align_s = bound - reinterpret_cast<uintptr_t>(s) % bound;
				s += __align_s / _Mysize;
				n -= __align_s / _Mysize;
				if (is_unlikely(n < width * 4)) {
					// n = [width * 4 - width / _Mysize, width * 4)
					goto WJR_MACRO_LABEL(aft_align);
				}
			}
			else {
				// unalign algorithm
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

			do {
				simd_t::store(reinterpret_cast<sint*>(s), q);
				simd_t::store(reinterpret_cast<sint*>(s + width), q);
				simd_t::store(reinterpret_cast<sint*>(s + width * 2), q);
				simd_t::store(reinterpret_cast<sint*>(s + width * 3), q);
				s += width * 4;
				n -= width * 4;
			} while (n >= width * 4);

			WJR_MACRO_LABEL(aft_align) :

				auto t = s + n - width;
			s = reinterpret_cast<T*>(
				(reinterpret_cast<uintptr_t>(s + n - width * 3)) & (~(bound - 1)));
			simd_t::store(reinterpret_cast<sint*>(s), q);
			simd_t::store(reinterpret_cast<sint*>(s + width), q);
			simd_t::store(reinterpret_cast<sint*>(s + width * 2), q);
			simd_t::storeu(reinterpret_cast<sint*>(t), q);

			return;
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto q = simd::avx::set1(val, T());
			simd::avx::storeu(reinterpret_cast<__m256i*>(s), q);
			simd::avx::storeu(reinterpret_cast<__m256i*>(s + 32 / _Mysize), q);
			simd::avx::storeu(reinterpret_cast<__m256i*>(s + n - 64 / _Mysize), q);
			simd::avx::storeu(reinterpret_cast<__m256i*>(s + n - 32 / _Mysize), q);
			return;
		}
#endif // __AVX2__
		auto q = simd::sse::set1(val, T());
		auto delta = (n & (32 / _Mysize)) >> 1;
		simd::sse::storeu(reinterpret_cast<__m128i*>(s), q);
		simd::sse::storeu(reinterpret_cast<__m128i*>(s + delta), q);
		simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize) - delta), q);
		simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize)), q);
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

void __memset(uint8_t* s, uint8_t val, size_t n) { return __memset_helper(s, val, n); }
void __memset(uint16_t* s, uint16_t val, size_t n) { return __memset_helper(s, val, n); }
void __memset(uint32_t* s, uint32_t val, size_t n) { return __memset_helper(s, val, n); }
#if defined(WJR_X86_64)
void __memset(uint64_t* s, uint64_t val, size_t n) { return __memset_helper(s, val, n); }
#endif

_WJR_ALGO_END

#endif // _WJR_FAST_MEMSET