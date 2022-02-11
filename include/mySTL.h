#ifndef __WJR_MY_STL_H
#define __WJR_MY_STL_H

#include <cassert>
#include <cstdint>
#include <ctime>
#include <functional>
#include <random>
#include <type_traits>
#include <locale>
#ifdef __SSE4_2__
#include <intrin.h>
#endif

namespace wjr {

#ifndef is_little_endian
#if defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define is_little_endian true
#else
#define is_little_endian false
#endif
#else
#define is_little_endian true
#endif
#endif

#if defined(__has_builtin)
#define whas_builtin(x) __has_builtin(x)
#else
#define whas_builtin(x) 0
#endif

#if whas_builtin(__builtin_expect)
#ifndef likely
#define likely(expr) __builtin_expect(!!(expr), 1)
#endif
#ifndef unlikely
#define unlikely(expr) __builtin_expect(!!(expr),0)
#endif
#else
#ifndef likely
#define likely(expr) expr
#endif
#ifndef unlikely
#define unlikely(expr) expr
#endif
#endif

#ifndef USHORT_MAX
#define USHORT_MAX 0xFFFF
#endif

#ifdef __USE_THREADS
#define USE_THREAD_LOCAL thread_local
#else
#define USE_THREAD_LOCAL
#endif

#undef KB
#define KB *(size_t(1) << 10)

#undef MB
#define MB *((size_t)(1) << 20)

#undef GB
#define GB *((size_t)(1) << 30)

	template <typename T>
	struct default_array_delete { // default deleter for unique_ptr
		constexpr default_array_delete() noexcept = default;

		template <typename T2, std::enable_if_t<std::is_convertible_v<T2*, T*>, int> = 0>
		default_array_delete(const default_array_delete<T2>&) noexcept {}

		void operator()(T* _Ptr) const noexcept {
			static_assert(0 < sizeof(T), "can't delete an incomplete type");
			delete[] _Ptr;
		}
	};

	template<typename T,typename D = default_array_delete<T>>
	using unique_array_ptr = std::unique_ptr<T,D>;

	inline namespace wjr_math {
		constexpr static unsigned long long binary_mask[65] = {
			0x0000000000000000,
			0x0000000000000001,0x0000000000000003,0x0000000000000007,0x000000000000000f,
			0x000000000000001f,0x000000000000003f,0x000000000000007f,0x00000000000000ff,
			0x00000000000001ff,0x00000000000003ff,0x00000000000007ff,0x0000000000000fff,
			0x0000000000001fff,0x0000000000003fff,0x0000000000007fff,0x000000000000ffff,
			0x000000000001ffff,0x000000000003ffff,0x000000000007ffff,0x00000000000fffff,
			0x00000000001fffff,0x00000000003fffff,0x00000000007fffff,0x0000000000ffffff,
			0x0000000001ffffff,0x0000000003ffffff,0x0000000007ffffff,0x000000000fffffff,
			0x000000001fffffff,0x000000003fffffff,0x000000007fffffff,0x00000000ffffffff,
			0x00000001ffffffff,0x00000003ffffffff,0x00000007ffffffff,0x0000000fffffffff,
			0x0000001fffffffff,0x0000003fffffffff,0x0000007fffffffff,0x000000ffffffffff,
			0x000001ffffffffff,0x000003ffffffffff,0x000007ffffffffff,0x00000fffffffffff,
			0x00001fffffffffff,0x00003fffffffffff,0x00007fffffffffff,0x0000ffffffffffff,
			0x0001ffffffffffff,0x0003ffffffffffff,0x0007ffffffffffff,0x000fffffffffffff,
			0x001fffffffffffff,0x003fffffffffffff,0x007fffffffffffff,0x00ffffffffffffff,
			0x01ffffffffffffff,0x03ffffffffffffff,0x07ffffffffffffff,0x0fffffffffffffff,
			0x1fffffffffffffff,0x3fffffffffffffff,0x7fffffffffffffff,0xffffffffffffffff
		};

		constexpr static unsigned int quick_log2_tabel[32] =
		{ 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3 };

		constexpr unsigned int quick_log2(unsigned int x) {
			unsigned int ans = 0;
			if (x >> 16) { ans += 16; x >>= 16; }
			if (x >> 8) { ans += 8; x >>= 8; }
			if (x >> 4) { ans += 4; x >>= 4; }
			return ans + quick_log2_tabel[x];
		}

		constexpr unsigned int quick_log2(int x) {
			assert(x >= 0);
			return quick_log2((unsigned int)x);
		}

		constexpr unsigned int quick_log2(unsigned long long x) {
			unsigned int ans = 0;
			if (x >> 32) { ans += 32; x >>= 32; }
			if (x >> 16) { ans += 16; x >>= 16; }
			if (x >> 8) { ans += 8; x >>= 8; }
			if (x >> 4) { ans += 4; x >>= 4; }
			return ans + quick_log2_tabel[x];
		}

		constexpr unsigned int quick_log2(long long x) {
			assert(x >= 0);
			return quick_log2((unsigned long long)x);
		}

		constexpr uint16_t bswap_16(uint16_t x) {
			return (x >> 8) | (x << 8);
		}

		constexpr uint32_t bswap_32(uint32_t x) {
			x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0x00FF00FF);
			return (x >> 16) | (x << 16);
		}

	}

	inline namespace wjr_type_traits {

		template<typename...>
		struct is_any_of {
			constexpr static bool value = false;
		};

		template<typename T, typename U>
		struct is_any_of<T, U> {
			constexpr static bool value = std::is_same_v<T, U>;
		};

		template<typename T, typename U, typename...args>
		struct is_any_of<T, U, args...> {
			constexpr static bool value = std::disjunction_v<is_any_of<T, U>, is_any_of<T, args...>>;
		};

		template<typename...args>
		constexpr static bool is_any_of_v = is_any_of<args...>::value;

		template<typename T>
		struct is_reverse_iterator : std::false_type {};

		template<typename T>
		struct is_reverse_iterator<std::reverse_iterator<T>> : std::true_type {};

	#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1913))
		template<typename F,typename...Args>
		struct wjr_result_of {
			using type = std::invoke_result_t<F,Args...>;
		};
	#else
		template<typename F,typename ...Args>
		struct wjr_result_of {
			using type = std::result_of_t<F(Args...)>;
		};
	#endif
		template<typename F,typename...Args>
		using wjr_result_of_t = typename wjr_result_of<F,Args...>::type;

		template<typename Pred_eq>
		struct is_default_equal : std::false_type {};

		template<>
		struct is_default_equal<std::equal_to<>> : std::true_type {};

	}

	inline namespace wjr_hash {
	#if defined(_WIN64)
		constexpr static size_t _FNV_offset_basis = 14695981039346656037ULL;
		constexpr static size_t _FNV_prime = 1099511628211ULL;
	#else // defined(_WIN64)
		constexpr static size_t _FNV_offset_basis = 2166136261U;
		constexpr static size_t _FNV_prime = 16777619U;
	#endif // defined(_WIN64)

		constexpr size_t normal_fnv1a_append_bytes(size_t _Val, const unsigned char* const _First,
			const size_t _Count) noexcept { // accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
			for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
				_Val ^= static_cast<size_t>(_First[_Idx]);
				_Val *= _FNV_prime;
			}
			return _Val;
		}

		template<size_t byte_size, typename const_pointer>
		constexpr size_t constexpr_fnv1a_append_bytes(size_t _Val, const_pointer _First,
			const size_t _Count)noexcept {
			for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
				auto val = static_cast<size_t>(_First[_Idx]);
				if constexpr (is_little_endian) {
					for (size_t i = 0; i < byte_size; ++i) {
						_Val ^= (val >> (i << 3)) & 0xFF;
						_Val *= _FNV_prime;
					}
				}
				else {
					for (size_t i = byte_size - 1; i > 0; ++i) {
						_Val ^= (val >> (i << 3)) & 0xFF;
						_Val *= _FNV_prime;
					}
				}
			}
			return _Val;
		}

		template<typename const_pointer>
		constexpr size_t fnv1a_append_bytes(size_t _Val, const_pointer _First,
			const size_t _Count)noexcept {
			using value_type = typename std::iterator_traits<const_pointer>::value_type;
			constexpr size_t byte_size = sizeof(value_type);
			if constexpr (byte_size >= 1 && byte_size <= sizeof(size_t)) {
				return constexpr_fnv1a_append_bytes<byte_size>(_Val, _First, _Count);
			}
			else {
				return normal_fnv1a_append_bytes(_Val,
					reinterpret_cast<const unsigned char*>(_First), byte_size * _Count);
			}
		}

		template <class _Ty>
		constexpr size_t fnv1a_append_range(const size_t _Val, const _Ty* const _First,
			const _Ty* const _Last) noexcept { // accumulate range [_First, _Last) into partial FNV-1a hash _Val
			static_assert(std::is_trivial_v<_Ty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(_Val, _First, static_cast<size_t>(_Last - _First));
		}

		template <class _Kty>
		constexpr size_t fnv1a_append_value(
			const size_t _Val, const _Kty& _Keyval) noexcept { // accumulate _Keyval into partial FNV-1a hash _Val
			static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(_Val, reinterpret_cast<const _Kty*>(&_Keyval), 1);
		}

		// FUNCTION TEMPLATE _Hash_representation
		template <class _Kty>
		constexpr size_t hash_representation(const _Kty& _Keyval) noexcept { // bitwise hashes the representation of a key
			return fnv1a_append_value(_FNV_offset_basis, _Keyval);
		}

		// FUNCTION TEMPLATE _Hash_array_representation
		template <class _Kty>
		constexpr size_t hash_array_representation(
			const _Kty* const _First, const size_t _Count) noexcept { // bitwise hashes the representation of an array
			static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(
				_FNV_offset_basis, _First, _Count);
		}
	}

	inline namespace wjr_mem {
		// ' ','\n','\r','\t' 
		constexpr static bool is_white_space_char[256] = { 0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		};

		constexpr static void* simple_skip_whitespace(const void* _s, const void* _e) {
			const uint8_t* s = (const uint8_t*)_s,*e = (const uint8_t*)_e;
			while (s != e && is_white_space_char[*s])++s;
			return (void*)s;
		}

		constexpr static void* skip_whitespace(const void* s, const void* e) {
		#ifdef __SSE4_2__
			if (is_white_space[*s])
				++s;
			else return s;

			static const char whitespace[16] = " \n\r\t";
			const __m128i w = _mm_load_si128((const __m128i*) & whitespace[0]);
			for (; s <= e - 16; s += 16) {
				const __m128i ss = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s));
				const int r = _mm_cmpistri(w, ss, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
				if (r != 16)
					return s + r;
			}
		#endif
			return simple_skip_whitespace(s, e);
		}

	}

}

#endif