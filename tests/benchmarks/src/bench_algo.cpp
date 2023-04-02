#include <wjr/random.h>
#include <wjr/vector.h>
#include <wjr/allocator.h>
#include <wjr/string.h>
#include <wjr/deque.h>
#include <deque>
#include <queue>
#include <wjr/network/thread_pool.h>
#include <sstream>

#include <benchmark/benchmark.h>

#if WJR_AVX2
#define __WIDTH (32)
#elif WJR_SSE2
#define __WIDTH (16)
#endif

#define __POS1 (0)
#define __POS2 (1.0)

static void __product1_small(benchmark::internal::Benchmark* state) {
	for (int i = 0; i < __WIDTH; ++i) {
		state->Args({ i, (long long)(__POS1) });
	}
	for (int i = __WIDTH * 1; i <= __WIDTH * 7; i += __WIDTH) {
		state->Args({ i, (long long)(__POS1) });
	}
}

static void __product1(benchmark::internal::Benchmark* state) {
	__product1_small(state);
	for (int i = __WIDTH * (1 << 3); i <= __WIDTH * (1 << 20); i *= 2) {
		state->Args({ i, (long long)(__POS1) });
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

using al1 = std::allocator<int>;
using al2 = wjr::aligned_allocator<int, 32, 0>;
using al3 = wjr::aligned_allocator<char, 32, 0>;
using vec1 = std::vector<int, al1>;
using vec2 = wjr::sso_vector<int, 2, al2>;

static void __test1(benchmark::State& state) {
	const int  N = 1e5;
	const auto n = state.range(0);
	std::vector<size_t> VEC(N);
	for (auto& i : VEC) {
		i = wjr::Random::get<size_t>(0, n);
	}
	int g = 0;

	std::vector<char> vec(n);
	auto func = [&]() {
		auto m = VEC.data()[g];
		memset(vec.data(), 0, m);
		++g;
		g = g == N ? 0 : g;
		return 0;
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

static void __test2(benchmark::State& state) {
	const int  N = 1e5;
	const auto n = state.range(0);
	std::vector<size_t> VEC(N);
	for (auto& i : VEC) {
		i = wjr::Random::get<size_t>(0, n);
	}
	int g = 0;

	std::vector<char> vec(n);
	auto func = [&]() {
		auto m = VEC.data()[g];
		wjr::fill_n(vec.data(), m, 0);
		++g;
		g = g == N ? 0 : g;
		return 0;
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

static void __test3(benchmark::State& state) {
	const int  N = 1e5;
	const auto n = state.range(0);
	std::vector<size_t> VEC(N);
	for (auto& i : VEC) {
		i = wjr::Random::get<size_t>(0, n);
	}
	int g = 0;

	std::vector<char, wjr::aligned_allocator<char, 16, 0>> vec1(n + 1, '0');
	std::vector<char, wjr::aligned_allocator<char, 16, 0>> vec2(n + 1, '0');
	auto func = [&]() {
		auto m = VEC.data()[g];
		//++vec1[m];
		auto p = std::char_traits<char>::compare(vec1.data(), vec2.data(), n + 1);
		//--vec1[m];
		++g;
		g = g == N ? 0 : g;
		return p;
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

static void __test4(benchmark::State& state) {
	const int  N = 1e5;
	const auto n = state.range(0);
	std::vector<size_t> VEC(N);
	for (auto& i : VEC) {
		i = wjr::Random::get<size_t>(0, n);
	}
	int g = 0;

	std::vector<char, wjr::aligned_allocator<char, 16, 0>> vec1(n + 1, '0');
	std::vector<char, wjr::aligned_allocator<char, 16, 0>> vec2(n + 1, '0');
	auto func = [&]() {
		auto m = VEC.data()[g];
		//++vec1[m];
		auto p = wjr::algo::memcmp(vec1.data(), vec2.data(), n + 1, std::equal_to<>{});
		//--vec1[m];
		++g;
		g = g == N ? 0 : g;
		return p;
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

//BENCHMARK(__test1)->Name("std::memset")->Repetitions(4)->DenseRange(1, 31)
//->DenseRange(32, 240, 16)->DenseRange(256, 1024 - 32, 32)->DenseRange(1024, 2048, 128)
//->RangeMultiplier(2)->Range(1 << 12, 1 << 24)
//;
//BENCHMARK(__test2)->Name("wjr::memset")->Repetitions(4)->DenseRange(1, 31)
//->DenseRange(32, 240, 16)->DenseRange(256, 1024 - 32, 32)->DenseRange(1024, 2048, 128)
//->RangeMultiplier(2)->Range(1 << 12, 1 << 24)
//;

//BENCHMARK(__test3)->Name("std::memset")->Repetitions(4)->DenseRange(1, 15)
//->DenseRange(32, 240, 16)->DenseRange(256, 1024 - 32, 32)->DenseRange(1024, 2048, 128)
//->RangeMultiplier(2)->Range(1 << 12, 1 << 24)
//;
//BENCHMARK(__test4)->Name("wjr::memset")->Repetitions(4)->DenseRange(1, 15)
//->DenseRange(32, 240, 16)->DenseRange(256, 1024 - 32, 32)->DenseRange(1024, 2048, 128)
//->RangeMultiplier(2)->Range(1 << 12, 1 << 24)
//;

static void __test5(benchmark::State& state) {
	const int N = 1e5;
	const auto n = state.range(0);
	const auto m = 64;
	wjr::string s1;
	for (int i = 0; i < n; ++i)s1.push_back('a');
	std::vector<wjr::string> vec(N);
	for (auto& i : vec) {
		i.resize(m);
		for (auto& j : i) {
			j = wjr::Random::get<char>('a', 'z');
		}
	}
	using string_view = wjr::basic_string_view<char>;
	int g = 0;
	auto func = [&]() {
		auto& s2 = vec[g];
		++g;
		g = g == N ? 0 : g;
		return (string_view(s1)).find((string_view)(s2));
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

static void __test6(benchmark::State& state) {
	const int N = 1e5;
	const auto n = state.range(0);
	const auto m = 64;
	wjr::string s1;
	for (int i = 0; i < n; ++i)s1.push_back('a');
	std::vector<wjr::string> vec(N);
	for (auto& i : vec) {
		i.resize(m);
		for (auto& j : i) {
			j = wjr::Random::get<char>('a', 'z');
		}
	}
	using string_view = wjr::basic_string_view<char>;
	string_view it;
	int g = 0;
	auto func = [&]() {
		auto& s2 = vec[g];
		++g;
		g = g == N ? 0 : g;
		return (string_view(s1)).rfind((string_view)(s2));
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

//BENCHMARK(__test5)->Name("std::memset")->Repetitions(8)->DenseRange(1, 15)
//->DenseRange(32, 240, 16)->DenseRange(256, 1024 - 32, 32)->DenseRange(1024, 2048, 128)
//->RangeMultiplier(2)->Range(1 << 12, 1 << 24)
//;
//BENCHMARK(__test6)->Name("wjr::memset")->Repetitions(8)->DenseRange(1, 15)
//->DenseRange(32, 240, 16)->DenseRange(256, 1024 - 32, 32)->DenseRange(1024, 2048, 128)
//->RangeMultiplier(2)->Range(1 << 12, 1 << 24)
//;

static void __test11(benchmark::State& state) {
	const int  N = 1e5;
	const auto n = state.range(0);
	std::vector<size_t> VEC(N);
	for (auto& i : VEC) {
		i = wjr::Random::get<size_t>(0, n);
	}
	int g = 0;

	std::vector<char> vec(n);
	auto func = [&]() {
		auto a = memchr(vec.data(), 1, n);
		return a;
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

static void __test12(benchmark::State& state) {
	const int  N = 1e5;
	const auto n = state.range(0);
	std::vector<size_t> VEC(N);
	for (auto& i : VEC) {
		i = wjr::Random::get<size_t>(0, n);
	}
	int g = 0;

	std::vector<char> vec(n);
	auto func = [&]() {
		auto a = wjr::find(vec.data(), vec.data() + n, 1);
		return a;
	};
	for (auto _ : state) {
		benchmark::DoNotOptimize(func());
	}
}

BENCHMARK(__test11)->Name("std::memchr")->Repetitions(4)->DenseRange(1, 31)
->DenseRange(32, 240, 16)->DenseRange(256, 1024 - 32, 32)->DenseRange(1024, 2048, 128)
->RangeMultiplier(2)->Range(1 << 12, 1 << 24)
;
BENCHMARK(__test12)->Name("wjr::memchr")->Repetitions(4)->DenseRange(1, 31)
->DenseRange(32, 240, 16)->DenseRange(256, 1024 - 32, 32)->DenseRange(1024, 2048, 128)
->RangeMultiplier(2)->Range(1 << 12, 1 << 24)
;

//BENCHMARK_MAIN();