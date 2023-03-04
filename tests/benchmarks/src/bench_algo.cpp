#include <wjr/algorithm.h>
#include <wjr/random.h>
#include <wjr/allocator.h>
#include <wjr/vector.h>
#include <iostream>

#include <benchmark/benchmark.h>

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
	int n = state.range(0);
	int m = state.range(1);

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
	int n = state.range(0);
	int m = state.range(1);

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
		if (wjr::Random::get<bool>()) {
			++vec[i];
		}
	}
	for (auto _ : state) {
		auto x = __test_memcnt(vec.data(), val, vec.size());
		benchmark::DoNotOptimize(x);
	}
}

template<typename T>
inline auto __test_memset(T* s, T val, size_t n) {
#ifndef __BENCHMARK_TEST_STD
	wjr::fill_n(s, n, val);
	return n;
#else
	std::fill_n(s, n, val);
	return n;
#endif
}

static void __memset(benchmark::State& state) {
	int n = state.range(0);
	using value_type = uint32_t;
	std::vector<value_type> vec(n);
	for (int i = 0; i < n; ++i) {
		vec[i] = wjr::Random::get<uint32_t>();
	}
	auto val = wjr::Random::get<uint32_t>();
	for (auto _ : state) {
		auto x = __test_memset(vec.data(), val, vec.size());
		benchmark::DoNotOptimize(x);
	}
}

//BENCHMARK(__memchr)
//->Apply(__product2);

//BENCHMARK(__memcmpeq)->Repetitions(4)->Args({ 1 << 26, 1 << 26 });
//->Apply(__product2)->Repetitions(2);

//BENCHMARK(__memmis)
//->Apply(__product2);

//BENCHMARK(__memcnt)
//->Apply(__product2);

//BENCHMARK(__memset)
//->Apply(__product2);

static void ptest(benchmark::internal::Benchmark* state) {
	for (int i = 0; i < 16; ++i) {
		for (int j = -1; j < i; ++j) {
			state->Args({ i, j });
		}
	}
	/*for (int i = 16; i < 64; i += 8) {
		state->Args({ i, 0 });
		for (int j = 0; j < i; j += 8) {
			state->Args({ i, j });
		}
	}*/
	//for (int i = 64; i < 1024; i *= 2) {
		//for (int j = 0; j < 64; j += 8) {
			//state->Args({ i + j, -1 });
		//}
	//}
}

static void test1(benchmark::State& state) {
	size_t n = state.range(0);
	const int m = 0;
	//const int l = 16;
	using value_type = uint8_t;
	auto val = 1;
	std::vector<value_type, wjr::aligned_allocator<value_type, 32, m>> vec1(n, val);
	//std::vector<value_type, wjr::aligned_allocator<value_type, 32, l>> vec2(n);
	auto func = [&]() {
		return std::count(vec1.data(), vec1.data() + vec1.size(), val);
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

static void test2(benchmark::State& state) {
	size_t n = state.range(0);
	const int m = 0;
	//const int l = 16;
	using value_type = uint8_t;
	auto val = 1;
	std::vector<value_type, wjr::aligned_allocator<value_type, 32, m>> vec1(n, val);
	//std::vector<value_type, wjr::aligned_allocator<value_type, 32, l>> vec2(n);
	auto func = [&]() {
		return wjr::count(vec1.data(), vec1.data() + vec1.size(), val);
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

//BENCHMARK(test1)->Apply(ptest)->MinTime(0.0005);
//BENCHMARK(test1)->Repetitions(4)-> RangeMultiplier(2)->Range(1 << 20, 1 << 26);
//BENCHMARK(test1)->Repetitions(4)->DenseRange(1, 3)->DenseRange(4, 16, 4)
//->DenseRange(32, 64, 16)->RangeMultiplier(2)->Range(128, 1024);
//BENCHMARK(test2)->Repetitions(4)->DenseRange(1, 3)->DenseRange(4, 16, 4)
//->DenseRange(32, 64, 16)->RangeMultiplier(2)->Range(128, 1024);

//struct node {
	//uint8_t x[64];
//};

using al1 = std::allocator<int>;
using al2 = wjr::aligned_allocator<int, 32, 0>;
using al3 = wjr::aligned_allocator<int, 32, 0>;
using vec1 = std::vector<int, al2>;
using vec2 = wjr::vector<int, al2>;

static void __test1(benchmark::State& state) {
	size_t n = state.range(0);
	vec1 vec;
	auto func = [&]() {
		vec.clear();
		for (int i = 0; i < n; ++i) {
			vec.emplace_back(i);
		}
	};
	for (auto _ : state) {
		func();
	}
}

static void __test2(benchmark::State& state) {
	size_t n = state.range(0);
	vec2 vec;
	auto func = [&]() {
		vec.clear();
		for (int i = 0; i < n; ++i) {
			vec.emplace_back(i);
		}
	};
	for (auto _ : state) {
		func();
	}
}

BENCHMARK(__test1)->Repetitions(4)->Arg(1 << 4);
BENCHMARK(__test2)->Repetitions(4)->Arg(1 << 4);

BENCHMARK_MAIN();