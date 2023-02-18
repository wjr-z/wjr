#pragma once
#ifndef __WJR_ALGO_MEMRCHR_H
#define __WJR_ALGO_MEMRCHR_H

#include <wjr/algo/macro.h>
#if defined(__HAS_FAST_MEMCHR)

_WJR_ALGO_BEGIN

template<typename T>
const T* __memrchr(const T* s, T val, size_t n) {
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
		for (size_t i = n; i > 0; --i) {
			if (s[i - 1] == val) {
				return s + i;
			}
		}
		return s;
	}

	if (is_unlikely(n == 0)) return s;

	s += n;

	if (n >= 16 / _Mysize) {
		if (n >= width * 4) {
			auto y = simd_t::set1(val, T());
			// align algorithm
			const T* _last;
			if constexpr (_Mysize == 1) {
				if (is_constant_p(reinterpret_cast<uintptr_t>(s) % width) &&
					reinterpret_cast<uintptr_t>(s) % width == 0) {
				}
				else {
					simd_t::mask_type z = simd_t::movemask_epi8(simd_t::cmpeq(
						simd_t::loadu(reinterpret_cast<const sint*>(s - width)),
						y, T()));
					if (z != 0) {
						return s - wjr::countl_zero(z);
					}
					auto __align_s = (reinterpret_cast<uintptr_t>(s) & (width - 1));
					s -= __align_s;
					n -= __align_s;
					if (is_unlikely(n < width * 4)) {
						// n = [width * 3, width * 4)
						_last = s - n;
						goto WJR_MACRO_LABEL(aft_align);
					}
				}

				do {
					auto x0 = simd_t::load(reinterpret_cast<const sint*>(s - width * 4));
					auto x1 = simd_t::load(reinterpret_cast<const sint*>(s - width * 3));
					auto x2 = simd_t::load(reinterpret_cast<const sint*>(s - width * 2));
					auto x3 = simd_t::load(reinterpret_cast<const sint*>(s - width));

					auto r0 = simd_t::cmpeq(x0, y, T());
					auto r1 = simd_t::cmpeq(x1, y, T());
					auto r2 = simd_t::cmpeq(x2, y, T());
					auto r3 = simd_t::cmpeq(x3, y, T());

					r0 = simd_t::Or(simd_t::Or(r0, r1), simd_t::Or(r2, r3));

					simd_t::mask_type z = simd_t::movemask_epi8(r0);
					if (z != 0) {
						simd_t::mask_type tmp = simd_t::movemask_epi8(r3);
						if (tmp) {
							return s - wjr::countl_zero(tmp);
						}
						tmp = simd_t::movemask_epi8(r2);
						if (tmp) {
							return s - width - wjr::countl_zero(tmp);
						}
						tmp = simd_t::movemask_epi8(r1);
						if (tmp) {
							return s - width * 2 - wjr::countl_zero(tmp);
						}
						tmp = z;
						return s - width * 3 - wjr::countl_zero(tmp);
					}
					s -= width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				_last = s - n;
				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						WJR_MACRO_LABEL(aft_align) :
							s -= width;
						simd_t::mask_type z = simd_t::movemask_epi8(simd_t::cmpeq(
							simd_t::load(reinterpret_cast<const sint*>(s)),
							y, T()));
						if (z != 0) {
							return s + width - wjr::countl_zero(z);
						}
					}
					case 3: {
						s -= width;
						simd_t::mask_type z = simd_t::movemask_epi8(simd_t::cmpeq(
							simd_t::load(reinterpret_cast<const sint*>(s)),
							y, T()));
						if (z != 0) {
							return s + width - wjr::countl_zero(z);
						}
					}
					case 2: {
						s -= width;
						simd_t::mask_type z = simd_t::movemask_epi8(simd_t::cmpeq(
							simd_t::load(reinterpret_cast<const sint*>(s)),
							y, T()));
						if (z != 0) {
							return s + width - wjr::countl_zero(z);
						}
					}
					case 1: {
						simd_t::mask_type z = simd_t::movemask_epi8(simd_t::cmpeq(
							simd_t::loadu(reinterpret_cast<const sint*>(_last)),
							y, T()));
						if (z != 0) {
							return _last + width - wjr::countl_zero(z);
						}
					}
					}
				}
				return _last;
			}
			// unalign algorithm
			else {
				do {
					auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 4));
					auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 3));
					auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s - width * 2));
					auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s - width));

					auto r0 = simd_t::cmpeq(x0, y, T());
					auto r1 = simd_t::cmpeq(x1, y, T());
					auto r2 = simd_t::cmpeq(x2, y, T());
					auto r3 = simd_t::cmpeq(x3, y, T());

					r0 = simd_t::Or(simd_t::Or(r0, r1), simd_t::Or(r2, r3));

					simd_t::mask_type z = simd_t::movemask_epi8(r0);
					if (z != 0) {
						simd_t::mask_type tmp = simd_t::movemask_epi8(r3);
						if (tmp) {
							return s - wjr::countl_zero(tmp) / _Mysize;
						}
						tmp = simd_t::movemask_epi8(r2);
						if (tmp) {
							return s - width - wjr::countl_zero(tmp) / _Mysize;
						}
						tmp = simd_t::movemask_epi8(r1);
						if (tmp) {
							return s - width * 2 - wjr::countl_zero(tmp) / _Mysize;
						}
						tmp = z;
						return s - width * 3 - wjr::countl_zero(tmp) / _Mysize;
					}
					s -= width * 4;
					n -= width * 4;
				} while (n >= width * 4);

				_last = s - n;
				if (n != 0) {
					switch ((n + width - 1) / width) {
					default: unreachable(); break;
					case 4: {
						s -= width;
						simd_t::mask_type z = simd_t::movemask_epi8(simd_t::cmpeq(
							simd_t::loadu(reinterpret_cast<const sint*>(s)),
							y, T()));
						if (z != 0) {
							return s + width - wjr::countl_zero(z) / _Mysize;
						}
					}
					case 3: {
						s -= width;
						simd_t::mask_type z = simd_t::movemask_epi8(simd_t::cmpeq(
							simd_t::loadu(reinterpret_cast<const sint*>(s)),
							y, T()));
						if (z != 0) {
							return s + width - wjr::countl_zero(z) / _Mysize;
						}
					}
					case 2: {
						s -= width;
						simd_t::mask_type z = simd_t::movemask_epi8(simd_t::cmpeq(
							simd_t::loadu(reinterpret_cast<const sint*>(s)),
							y, T()));
						if (z != 0) {
							return s + width - wjr::countl_zero(z) / _Mysize;
						}
					}
					case 1: {
						simd_t::mask_type z = simd_t::movemask_epi8(simd_t::cmpeq(
							simd_t::loadu(reinterpret_cast<const sint*>(_last)),
							y, T()));
						if (z != 0) {
							return _last + width - wjr::countl_zero(z) / _Mysize;
						}
					}
					}
				}
				return _last;
			}
		}

#if defined(__AVX2__)
		static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
		if (n >= 64 / _Mysize) {
			auto y = simd::avx::set1(val, T());

			auto r0 = simd::avx::cmpeq(simd::avx::loadu(reinterpret_cast<const __m256i*>(s - n)), y, T());
			auto r1 = simd::avx::cmpeq(simd::avx::loadu(reinterpret_cast<const __m256i*>(s - n + 32 / _Mysize)), y, T());
			auto r2 = simd::avx::cmpeq(simd::avx::loadu(reinterpret_cast<const __m256i*>(s - (64 / _Mysize))), y, T());
			auto r3 = simd::avx::cmpeq(simd::avx::loadu(reinterpret_cast<const __m256i*>(s - (32 / _Mysize))), y, T());

			r0 = simd::avx::Or(simd::avx::Or(r0, r1), simd::avx::Or(r2, r3));

			uint32_t z = simd::avx::movemask_epi8(r0);
			if (z != 0) {
				uint32_t tmp = simd::avx::movemask_epi8(r3);
				if (tmp) {
					return s - wjr::countl_zero(tmp) / _Mysize;
				}
				tmp = simd::avx::movemask_epi8(r2);
				if (tmp) {
					return s - (32 / _Mysize) - wjr::countl_zero(tmp) / _Mysize;
				}
				tmp = simd::avx::movemask_epi8(r1);
				if (tmp) {
					return s - n + (64 / _Mysize) - wjr::countl_zero(tmp) / _Mysize;
				}
				tmp = z;
				return s - n + (32 / _Mysize) - wjr::countl_zero(tmp) / _Mysize;
			}
			return s - n;
		}
#endif // __AVX2__

		auto y = simd::sse::set1(val, T());
		auto delta = (n & (32 / _Mysize)) >> 1;

		auto r0 = simd::sse::cmpeq(simd::sse::loadu(reinterpret_cast<const __m128i*>(s - n)), y, T());
		auto r1 = simd::sse::cmpeq(simd::sse::loadu(reinterpret_cast<const __m128i*>(s - n + delta)), y, T());
		auto r2 = simd::sse::cmpeq(simd::sse::loadu(reinterpret_cast<const __m128i*>(s - (16 / _Mysize) - delta)), y, T());
		auto r3 = simd::sse::cmpeq(simd::sse::loadu(reinterpret_cast<const __m128i*>(s - (16 / _Mysize))), y, T());

		r0 = simd::sse::Or(simd::sse::Or(r0, r1), simd::sse::Or(r2, r3));

		uint16_t z = simd::sse::movemask_epi8(r0);
		if (z != 0) {
			uint16_t tmp = simd::sse::movemask_epi8(r3);
			if (tmp) {
				return s - wjr::countl_zero(tmp) / _Mysize;
			}
			tmp = simd::sse::movemask_epi8(r2);
			if (tmp) {
				return s - delta - wjr::countl_zero(tmp) / _Mysize;
			}
			tmp = simd::sse::movemask_epi8(r1);
			if (tmp) {
				return s - n + delta + (16 / _Mysize) - wjr::countl_zero(tmp) / _Mysize;
			}
			tmp = z;
			return s - n + (16 / _Mysize) - wjr::countl_zero(tmp) / _Mysize;
		}
		return s - n;
	}

	if constexpr (_Mysize == 8) {
		// n = [1, 2)
		return s[-1] == val ? s : s - 1;
	}

	if constexpr (_Mysize == 2) {
		// n = [1, 8)
		if (n >= 4) {
			// n = [4, 8)
			auto A = *reinterpret_cast<const uint64_t*>(s - n);
			auto B = *reinterpret_cast<const uint64_t*>(s - 4);

			auto x = simd::sse::set_epi64x(B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmpeq(x, y, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0)return s - n;
			auto i = wjr::countl_zero(z) / _Mysize;
			auto q = i >= 4 ? i + n - 4 : i;
			return s - q;
		}
	}

	if constexpr (_Mysize == 1) {
		// n = [1, 16)
		if (n >= 4) {
			// n = [4, 16)
			auto delta = (n & 8) >> 1;

			auto A = *reinterpret_cast<const uint32_t*>(s - n);
			auto B = *reinterpret_cast<const uint32_t*>(s - n + delta);
			auto C = *reinterpret_cast<const uint32_t*>(s - 4 - delta);
			auto D = *reinterpret_cast<const uint32_t*>(s - 4);

			auto x = simd::sse::set_epi32(D, C, B, A);
			auto y = simd::sse::set1(val, T());
			auto r = simd::sse::cmpeq(x, y, T());
			uint16_t z = simd::sse::movemask_epi8(r);

			if (z == 0) return s - n;
			size_t i = wjr::countl_zero(z);
			auto q = i >= 8 ? i + (n - 12 - delta) : i;
			return s - q;
		}
	}

	if constexpr (_Mysize <= 4) {
		// n = [1, 4)
		if (s[-1] == val) return s;
		if (n == 1) return s - 1;
		if (s[-2] == val) return s - 1;
		if (n == 2) return s - 2;
		return s[-3] == val ? s - 2 : s - 3;
	}
}

_WJR_ALGO_END

#endif // __HAS_FAST_MEMCHR

#endif // __WJR_ALGO_MEMRCHR_H