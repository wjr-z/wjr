#include <benchmark/benchmark.h>

#include <wjr/vector.h>
#include <wjr/allocator.h>

#include <wjr/test_product.h>
#include <wjr/algorithm.h>
#include <wjr/simd/simd.h>

//#define __BENCHMARK_TEST_STD

#if defined(__AVX2__)
#define __WIDTH (32)
#elif defined(__SSE2__)
#define __WIDTH (16)
#endif

#define __POS1 (0)
#define __POS2 (1.0)

static void __product1(benchmark::internal::Benchmark* state) {
	bool f = true;
	for (int i = 0; i < __WIDTH; ++i) {
		if (__POS1 >= i) {
			if (!f) {
				break;
			}
			f = false;
		}
		state->Args({ i, __POS1 });
	}
	f = true;
	for (int i = __WIDTH * 1; i <= __WIDTH * 7; i += __WIDTH) {
		if (__POS1 >= i) {
			if (!f) {
				break;
			}
			f = false;
		}
		state->Args({ i, __POS1 });
	}
	f = true;
	for (int i = __WIDTH * (1 << 3); i <= __WIDTH * (1 << 20); i *= 2) {
		if (__POS1 >= i) {
			if (!f) {
				break;
			}
			f = false;
		}
		state->Args({ i, __POS1 });
	}
}

static void __product2(benchmark::internal::Benchmark* state) {
	for (int i = 0; i < __WIDTH; ++i) {
		state->Args({ i, (long long)(i * __POS2) });
	}
	for (int i = __WIDTH * 1; i <= __WIDTH * 7; i += __WIDTH) {
		state->Args({ i, (long long)(i * __POS2) });
	}
	for (int i = __WIDTH * (1 << 3); i <= __WIDTH * (1 << 20); i *= 2) {
		state->Args({ i, (long long)(i * __POS2) });
	}
}

inline auto __test_memchr(const void* s, int val, size_t n) {
#ifndef __BENCHMARK_TEST_STD
	auto __s = (uint8_t*)s;
	return wjr::find(__s, __s + n, val);
#else
	return memchr(s, val, n);
#endif
}

static void __memchr(benchmark::State& state) {
	int n = state.range(0);
	int m = state.range(1);

	std::vector<uint8_t> vec(n);
	for (auto& i : vec) {
		i = wjr::math::random<uint8_t>.uniform(0, 254);
	}
	if (m < n) {
		vec[m] = 255;
	}
	for (auto _ : state) {
		auto x = __test_memchr(vec.data(), 255, vec.size());
		benchmark::DoNotOptimize(x);
	}
}

inline auto __test_memcmp(const void* s0, const void* s1, size_t n) {
#ifndef __BENCHMARK_TEST_STD
	auto __s0 = (uint8_t*)s0;
	auto __s1 = (uint8_t*)s1;
	return wjr::equal(__s0, __s0 + n, __s1);
#else 
	return memcmp(s0, s1, n) == 0;
#endif
}

static void __memcmp(benchmark::State& state) {
	int n = state.range(0);
	int m = state.range(1);

	std::vector<uint8_t> vec1(n), vec2(n);
	for (auto& i : vec1) {
		i = wjr::math::random<uint8_t>.uniform(0, 254);
	}
	vec2 = vec1;
	if (m < n) {
		++vec2[m];
	}
	for (auto _ : state) {
		auto x = __test_memcmp(vec1.data(), vec2.data(), vec1.size());
		benchmark::DoNotOptimize(x);
	}
}

BENCHMARK(__memchr)
->Apply(__product2);

//BENCHMARK(__memcmp)
//->Apply(__product2);

//BENCHMARK_MAIN();