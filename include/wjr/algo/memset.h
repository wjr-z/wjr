#pragma once
#ifndef __WJR_ALGO_MEMSET_H
#define __WJR_ALGO_MEMSET_H

#include <wjr/algo/macro.h>

#if defined(__HAS_FAST_MEMSET)

_WJR_ALGO_BEGIN

template<typename T>
void __memset(T* s, T val, size_t n) {
	using namespace wjr::literals;
	constexpr size_t _Mysize = sizeof(T);

#if defined(__AVX2__)
	using simd_t = simd::avx;
#else
	using simd_t = simd::sse;
#endif // __AVX2__
	using sint = typename simd_t::int_type;
	constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);

	if (is_constant_p(n) && n <= 4_KiB) {
		for (size_t i = 0; i < n; ++i)s[i] = val;
		return;
	}

	if constexpr (_Mysize != 1) {
		if (is_constant_p(val) &&
			broadcast<T, uint8_t>(static_cast<uint8_t>(val)) == val) {
			return __memset(reinterpret_cast<uint8_t*>(s), static_cast<uint8_t>(val), n * sizeof(T));
		}
	}

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			auto q = simd_t::set1(val, T());
			// align algorithm
			if constexpr (_Mysize == 1) {
				// non-temporal algorithm
				if (n >= 4_MiB) { 
					if (!(is_constant_p(reinterpret_cast<uintptr_t>(s) % 64) &&
						reinterpret_cast<uintptr_t>(s) % 64 == 0)) {
#if defined(__AVX2__)
						simd_t::storeu(reinterpret_cast<sint*>(s), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width), q);
#else // SSE2
						simd_t::storeu(reinterpret_cast<sint*>(s), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width * 2), q);
						simd_t::storeu(reinterpret_cast<sint*>(s + width * 3), q);
#endif // __AVX2__
						auto __align_s = 64 - (reinterpret_cast<uintptr_t>(s) & 63);
						s += __align_s;
						n -= __align_s;
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
				}
				else {

					if (is_constant_p(reinterpret_cast<uintptr_t>(s) % width) &&
						reinterpret_cast<uintptr_t>(s) % width == 0) {
					}
					else {
						simd_t::storeu(reinterpret_cast<sint*>(s), q);
						auto __align_s = width - (reinterpret_cast<uintptr_t>(s) & (width - 1));
						s += __align_s;
						n -= __align_s;
						if (is_unlikely(n < width * 4)) {
							// n = [width * 3, width * 4)
							goto WJR_MACRO_LABEL(aft_align);
						}
					}

					do {
						simd_t::store(reinterpret_cast<sint*>(s), q);
						simd_t::store(reinterpret_cast<sint*>(s + width), q);
						simd_t::store(reinterpret_cast<sint*>(s + width * 2), q);
						simd_t::store(reinterpret_cast<sint*>(s + width * 3), q);
						s += width * 4;
						n -= width * 4;
					} while (n >= width * 4);
				}

				WJR_MACRO_LABEL(aft_align):

				auto t = s + n - width;
				s = reinterpret_cast<T*>(
					(reinterpret_cast<uintptr_t>(s) + n - width * 3) & (~(width - 1)));
				simd_t::store(reinterpret_cast<sint*>(s), q);
				simd_t::store(reinterpret_cast<sint*>(s + width), q);
				simd_t::store(reinterpret_cast<sint*>(s + width * 2), q);
				simd_t::storeu(reinterpret_cast<sint*>(t), q);
			}
			// unalign algorithm
			else {
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
			}
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

_WJR_ALGO_END

#endif // __HAS_FAST_MEMSET

#endif // __WJR_ALGO_MEMSET_H