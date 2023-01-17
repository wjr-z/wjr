#include <benchmark/benchmark.h>

#include <wjr/algorithm.h>
#include <wjr/random.h>

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

static void __product2_small(benchmark::internal::Benchmark* state) {
	for (int i = 0; i < __WIDTH; ++i) {
		state->Args({ i, (long long)(i * __POS2) });
	}
	for (int i = __WIDTH * 1; i <= __WIDTH * 7; i += __WIDTH) {
		state->Args({ i, (long long)(i * __POS2) });
	}
}

static void __product2(benchmark::internal::Benchmark* state) {
	__product2_small(state);
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
	//int n = state.range(0);
	//int m = state.range(1);
	const int n = 128;
	const int m = n;

	using value_type = uint8_t;
	std::vector<value_type> vec(n);
	for (auto& i : vec) {
		i = wjr::Random::get<value_type>(0, 254);
	}
	if (m < n) {
		vec[m] = 255;
	}
	for (auto _ : state) {
		auto x = __test_memchr(vec.data(), 255, n);
		benchmark::DoNotOptimize(x);
	}
}

template<typename T, typename _Pred = std::equal_to<>>
inline auto __test_memcmp(const T* s0, const T* s1, size_t n, _Pred pred = _Pred{}) {
#ifndef __BENCHMARK_TEST_STD
	return wjr::equal(s0, s0 + n, s1, pred);
#else 
	if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
		return memcmp(s0, s1, n * sizeof(T)) == 0;
	}
	else {
		for (int i = 0; i < n; ++i) {
			if (!pred(s0[i], s1[i])) {
				return false;
			}
		}
		return true;
	}
#endif
}

static void __memcmpeq(benchmark::State& state) {
	//int n = state.range(0);
	//int m = state.range(1);
	const int n = 17;
	const int m = n;

	std::vector<uint8_t> vec1(n), vec2(n);
	for (auto& i : vec1) {
		i = wjr::Random::get<uint8_t>(0, 254);
	}
	vec2 = vec1;
	if (m < n) {
		++vec2[m];
	}
	for (auto _ : state) {
		auto x = __test_memcmp(vec1.data(), vec2.data(), n);
		benchmark::DoNotOptimize(x);
	}
}

template<typename T, typename _Pred = std::equal_to<>>
inline auto __test_memmis(const T* s0, const T* s1, size_t n, _Pred pred = _Pred{}) {
#ifndef __BENCHMARK_TEST_STD
	return wjr::mismatch(s0, s0 + n, s1, pred);
#else 
	for (int i = 0; i < n; ++i) {
		if (!pred(s0[i], s1[i])) {
			return s0 + i;
		}
	}
	return s0 + n;
#endif
}

static void __memmis(benchmark::State& state) {
	int n = state.range(0);
	int m = state.range(1);

	std::vector<uint32_t> vec1(n), vec2(n);
	for (int i = 0; i < n; ++i) {
		auto val = wjr::Random::get<uint32_t>(0, 254);
		vec1[i] = val;
		vec2[i] = val;
	}
	if (m < n) {
		++vec2[m];
	}
	for (auto _ : state) {
		auto x = __test_memmis(vec1.data(), vec2.data(), vec1.size());
		benchmark::DoNotOptimize(x);
	}
}

template<typename T>
inline auto __test_memcnt(const T* s, T val, size_t n) {
#ifndef __BENCHMARK_TEST_STD
	return wjr::count(s, s + n, val);
#else 
	size_t cnt = 0;
	for (int i = 0; i < n; ++i) {
		cnt += s[i] == val;
	}
	return cnt;
#endif
}

static void __memcnt(benchmark::State& state) {
	int n = state.range(0);

	using value_type = uint32_t;
	auto val = wjr::Random::get<value_type>(0, 254);
	std::vector<value_type> vec(n, val);
	for (int i = 0; i < n; ++i) {
		if(wjr::Random::get<bool>()) {
			++vec[i];
		}
	}
	for (auto _ : state) {
		auto x = __test_memcnt(vec.data(), val, vec.size());
		benchmark::DoNotOptimize(x);
	}
}

//BENCHMARK(__memchr)->Repetitions(4);
//->Apply(__product2)->Repetitions(4);

BENCHMARK(__memcmpeq)->Repetitions(4);
//->Apply(__product2);

//BENCHMARK(__memmis)
//->Apply(__product2);

//BENCHMARK(__memcnt)
//->Apply(__product2);

BENCHMARK_MAIN(); 