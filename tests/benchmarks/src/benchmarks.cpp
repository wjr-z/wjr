#include <benchmark/benchmark.h>

#include <wjr/vector.h>
#include <wjr/allocator.h>

#include <wjr/test_product.h>
#include <wjr/algorithm.h>

#include <ranges>

template<typename T>
using default_allocator = std::allocator<T>;

template<typename T>
using test_allocator = wjr::test_allocator<T, default_allocator<T>>;

using string = std::string;

template<typename T, typename Alloc>
using normal_vector = wjr::vector<T, Alloc, wjr::vector_core<T, Alloc>>;

template<typename T, typename Alloc>
using sso_vector_0 = wjr::sso_vector<T, Alloc, 0>;

template<typename T, typename Alloc>
using sso_vector_1 = wjr::sso_vector<T, Alloc, 1>;

template<typename T, typename Alloc>
using sso_vector_2 = wjr::sso_vector<T, Alloc, 2>;

template<typename T, typename Alloc>
using sso_vector_4 = wjr::sso_vector<T, Alloc, 4>;

template<typename T, typename Alloc>
using sso_vector_8 = wjr::sso_vector<T, Alloc, 8>;

template<typename T, typename Alloc>
using sso_vector_16 = wjr::sso_vector<T, Alloc, 16>;

template<typename T, typename Alloc>
using sso_vector_32 = wjr::sso_vector<T, Alloc, 32>;

template<typename T, typename Alloc>
using sso_vector_64 = wjr::sso_vector<T, Alloc, 64>;

template<typename T, typename Alloc>
using sso_vector_128 = wjr::sso_vector<T, Alloc, 128>;

template<typename T, typename Alloc>
using std_vector = std::vector<T, Alloc>;

const int BUF_SIZE = 64;

#define __MY_BENCHMARK_3(FUNC, VEC, T, ALLOC, RUNNER)						\
	  BENCHMARK_PRIVATE_DECLARE(_benchmark_) =								\
      (::benchmark::internal::RegisterBenchmarkInternal(					\
          new ::benchmark::internal::FunctionBenchmark(						\
              #FUNC "/" #VEC "<" #T "," #ALLOC ">" ,						\
              [](::benchmark::State& st) { FUNC<VEC<T,ALLOC<T>>>(st); })))	\
	RUNNER;

#define __MY_BENCHMARK_2(FUNC, T, ALLOC, RUNNER)							\
	__MY_BENCHMARK_3(FUNC, normal_vector, T, ALLOC, RUNNER)					\
	__MY_BENCHMARK_3(FUNC, sso_vector_0, T, ALLOC, RUNNER)					\
	__MY_BENCHMARK_3(FUNC, sso_vector_4, T, ALLOC, RUNNER)					\
	__MY_BENCHMARK_3(FUNC, sso_vector_16, T, ALLOC, RUNNER)					\
	__MY_BENCHMARK_3(FUNC, sso_vector_64, T, ALLOC, RUNNER)					\
	__MY_BENCHMARK_3(FUNC, sso_vector_128, T, ALLOC, RUNNER)				\
	__MY_BENCHMARK_3(FUNC, std_vector, T, ALLOC, RUNNER)

#define __MY_BENCHMARK_1(FUNC, T, RUNNER)									\
	__MY_BENCHMARK_2(FUNC, T, default_allocator, RUNNER);					\
	__MY_BENCHMARK_2(FUNC, T, test_allocator, RUNNER);						\

#define MY_BENCHMARK(FUNC, RUNNER)											\
	__MY_BENCHMARK_1(FUNC, char, RUNNER);									\
	__MY_BENCHMARK_1(FUNC, int, RUNNER);									\
	__MY_BENCHMARK_1(FUNC, string, RUNNER);									

#define _VEC_VAL typename VEC::value_type
#define _VEC_ALLOC typename VEC::allocator_type

template<typename T>
static void test1(benchmark::State& state) {
	int n = state.range(0);
	const int N = 256;
	std::vector<std::pair<T, std::vector<T>>> vec(N);
	for (auto& i : vec) {
		i.second.resize(n);
		for (auto& j : i.second) {
			j = wjr::math::random<T>();
		}
		T val = i.second[wjr::math::random<int>.uniform(0, n - 1)];
		i.first = val;
	}
	int g = 0;
	for (auto _ : state) {
		auto val = vec[g % N].first;
		auto& v = vec[g % N].second;
		auto x = std::find(v.data(), v.data() + v.size(), val);
		benchmark::DoNotOptimize(x);
		++g;
	}
}

template<typename T>
static void test2(benchmark::State& state) {
	int n = state.range(0);
	const int N = 256;
	std::vector<std::pair<T, std::vector<T>>> vec(N);
	for (auto& i : vec) {
		i.second.resize(n);
		for (auto& j : i.second) {
			j = wjr::math::random<T>();
		}
		T val = i.second[wjr::math::random<int>.uniform(0, n - 1)];
		i.first = val;
	}
	int g = 0;
	for (auto _ : state) {
		auto val = vec[g % N].first;
		auto& v = vec[g % N].second;
		auto x = wjr::find(v.data(), v.data() + v.size(), val);
		benchmark::DoNotOptimize(x);
		++g;
	}
}

BENCHMARK(test1<uint8_t>)->RangeMultiplier(2)->Range(1, 1)->Repetitions(8);
BENCHMARK(test2<uint8_t>)->RangeMultiplier(2)->Range(1, 1 << 16)->Repetitions(8);
BENCHMARK(test1<uint16_t>)->RangeMultiplier(2)->Range(1, 1 << 16)->Repetitions(8);
BENCHMARK(test2<uint16_t>)->RangeMultiplier(2)->Range(1, 1 << 16)->Repetitions(8);
BENCHMARK(test1<uint32_t>)->RangeMultiplier(2)->Range(1, 1 << 16)->Repetitions(8);
BENCHMARK(test2<uint32_t>)->RangeMultiplier(2)->Range(1, 1 << 16)->Repetitions(8);

BENCHMARK_MAIN();

// vector()
template<typename VEC>
static void vector_construct_1(benchmark::State& state) {
	for (auto _ : state) {
		VEC v;
		benchmark::DoNotOptimize(v);
	}
}

MY_BENCHMARK(vector_construct_1, ->MinTime(2));

// vector(const allocator_type& alloc)
template<typename VEC>
static void vector_construct_2(benchmark::State& state) {
	for (auto _ : state) {
		_VEC_ALLOC al;
		VEC v(al);
		benchmark::DoNotOptimize(v);
	}
}

MY_BENCHMARK(vector_construct_2, ->MinTime(2));

// vector(size_type count, const T& value, const allocator_type& alloc = allocator_type())
template<typename VEC>
static void vector_construct_3(benchmark::State& state) {
	int n = state.range(0);
	auto val = wjr::__test_random<_VEC_VAL>{}();
	for (auto _ : state) {
		VEC v(n, val);
		benchmark::DoNotOptimize(v);
	}
}

#define __RUNNER ->RangeMultiplier(2)->Range(1, 1 << 10)
MY_BENCHMARK(vector_construct_3, __RUNNER);
#undef __RUNNER

// vector(size_type count, const allocator_type& alloc = allocator_type())
template<typename VEC>
static void vector_construct_4(benchmark::State& state) {
	int n = state.range(0);
	for (auto _ : state) {
		VEC v(n);
		benchmark::DoNotOptimize(v);
	}
}

#define __RUNNER ->RangeMultiplier(2)->Range(1, 1 << 10)
MY_BENCHMARK(vector_construct_4, __RUNNER);
#undef __RUNNER

// vector(iter first, iter last, const allocator_type& alloc = allocator_type())
template<typename VEC>
static void vector_construct_5(benchmark::State& state) {
	int n = state.range(0);
	wjr::__test_vector_product<_VEC_VAL> v(n);
	for (auto _ : state) {
		VEC v2(v.begin(), v.end());
		benchmark::DoNotOptimize(v2);
	}
}

#define __RUNNER ->RangeMultiplier(2)->Range(1, 1 << 10)
MY_BENCHMARK(vector_construct_5, __RUNNER);
#undef __RUNNER

// vector(const vector& other)
template<typename VEC>
static void vector_construct_6(benchmark::State& state) {
	int n = state.range(0);
	VEC v(n);
	for (auto _ : state) {
		VEC v2(v);
		benchmark::DoNotOptimize(v2);
	}
}

#define __RUNNER ->RangeMultiplier(2)->Range(1, 1 << 10)
MY_BENCHMARK(vector_construct_6, __RUNNER);
#undef __RUNNER

// vector(const vector& other, const allocator_type& alloc)
// TODO;

// vector(vector&& other)
template<typename VEC>
static void vector_construct_8(benchmark::State& state) {
	int n = state.range(0);
	for (auto _ : state) {
		state.PauseTiming();
		wjr::sso_vector<VEC, default_allocator<VEC>, BUF_SIZE> buf;
		for (int i = 0; i < BUF_SIZE; ++i) {
			VEC v(n);
			buf.emplace_back(std::move(v));
		}
		state.ResumeTiming();
		for (int i = 0; i < BUF_SIZE; ++i) {
			VEC v(std::move(buf[i]));
			benchmark::DoNotOptimize(v);
		}
	}
}

#define __RUNNER ->RangeMultiplier(2)->Range(1, 1 << 10)
MY_BENCHMARK(vector_construct_8, __RUNNER);
#undef __RUNNER

// vector(vector&& other, const allocator_type& alloc)
// TODO;

// vector(initializer_list<T> init, const allocator_type& alloc = allocator_type())
// TODO;

// vector& operator=(const vector& other)
// 1. n <= size()
// 2. size() < n <= capacity()
// 3. capacity() < n
template<typename VEC>
static void vector_assign_1_branch_1(benchmark::State& state) {
	int n = state.range(0);
	for (auto _ : state) {
		state.PauseTiming();
		wjr::sso_vector<VEC, default_allocator<VEC>, BUF_SIZE> buf1;
		wjr::sso_vector<VEC, default_allocator<VEC>, BUF_SIZE> buf2;
		for (int i = 0; i < BUF_SIZE; ++i) {
			VEC v(n);
			buf1.emplace_back(std::move(v));
			VEC v2(n);
			buf2.emplace_back(std::move(v2));
		}
		state.ResumeTiming();
		for (int i = 0; i < BUF_SIZE; ++i) {
			buf2[i] = buf1[i];
			benchmark::DoNotOptimize(buf2[i]);
		}
	}
}

template<typename VEC>
static void vector_assign_1_branch_2(benchmark::State& state) {
	int n = state.range(0);
	for (auto _ : state) {
		state.PauseTiming();
		wjr::sso_vector<VEC, default_allocator<VEC>, BUF_SIZE> buf1;
		wjr::sso_vector<VEC, default_allocator<VEC>, BUF_SIZE> buf2;
		for (int i = 0; i < BUF_SIZE; ++i) {
			VEC v(n);
			buf1.emplace_back(std::move(v));
			VEC v2;
			v2.reserve(n);
			buf2.emplace_back(std::move(v2));
		}
		state.ResumeTiming();
		for (int i = 0; i < BUF_SIZE; ++i) {
			buf2[i] = buf1[i];
			benchmark::DoNotOptimize(buf2[i]);
		}
	}
}

template<typename VEC>
static void vector_assign_1_branch_3(benchmark::State& state) {
	int n = state.range(0);
	for (auto _ : state) {
		state.PauseTiming();
		wjr::sso_vector<VEC, default_allocator<VEC>, BUF_SIZE> buf1;
		wjr::sso_vector<VEC, default_allocator<VEC>, BUF_SIZE> buf2;
		for (int i = 0; i < BUF_SIZE; ++i) {
			VEC v(n);
			buf1.emplace_back(std::move(v));
			VEC v2;
			buf2.emplace_back(std::move(v2));
		}
		state.ResumeTiming();
		for (int i = 0; i < BUF_SIZE; ++i) {
			buf2[i] = buf1[i];
			benchmark::DoNotOptimize(buf2[i]);
		}
	}
}

#define __RUNNER ->RangeMultiplier(2)->Range(1, 1 << 10)
MY_BENCHMARK(vector_assign_1_branch_1, __RUNNER);
MY_BENCHMARK(vector_assign_1_branch_2, __RUNNER);
MY_BENCHMARK(vector_assign_1_branch_3, __RUNNER);
#undef __RUNNER

// vector& operator=(vector&& other)
template<typename VEC>
static void vector_assign_2(benchmark::State& state) {
	int n = state.range(0);
	for (auto _ : state) {
		state.PauseTiming();
		wjr::sso_vector<VEC, default_allocator<VEC>, BUF_SIZE> buf1;
		wjr::sso_vector<VEC, default_allocator<VEC>, BUF_SIZE> buf2;
		for (int i = 0; i < BUF_SIZE; ++i) {
			VEC v(n);
			buf1.emplace_back(std::move(v));
			VEC v2(n);
			buf2.emplace_back(std::move(v2));
		}
		state.ResumeTiming();
		for (int i = 0; i < BUF_SIZE; ++i) {
			buf2[i] = std::move(buf1[i]);
			benchmark::DoNotOptimize(buf2[i]);
		}
	}
}

#define __RUNNER ->RangeMultiplier(2)->Range(1, 1 << 10)
MY_BENCHMARK(vector_assign_2, __RUNNER);
#undef __RUNNER

 //BENCHMARK_MAIN();

/*
#define BENCHMARK_NAME(func, PREFIX, SUFIX)					\
  BENCHMARK_PRIVATE_DECLARE(_benchmark_) =					\
      (::benchmark::internal::RegisterBenchmarkInternal(	\
          new ::benchmark::internal::FunctionBenchmark(		\
              #PREFIX "/" #func #SUFIX,						\
              [](::benchmark::State& st) { func(st); })))

#define BENCHMARK_NAME_ARGS(func, PREFIX, SUFIX, ...)		\
  BENCHMARK_PRIVATE_DECLARE(_benchmark_) =					\
      (::benchmark::internal::RegisterBenchmarkInternal(	\
          new ::benchmark::internal::FunctionBenchmark(		\
              #PREFIX "/" #func #SUFIX,						\
              [](::benchmark::State& st) { func(st , __VA_ARGS__); })))

#define VEC_AL typename _Vec::allocator_type
#define VEC_VAL typename _Vec::value_type

#define DEFAULT_RUNNER(cp)								\
->DenseRange(1, 8)->RangeMultiplier(2)					\
->Range(1 << 4, 1 << 10)->Complexity(cp)->MinTime(0.25)

template<typename T, bool input = false>
using prod = std::conditional_t<input, product<std::input_iterator_tag, T>, product<T>>;

// vector()
template<typename _Vec>
static void vector_constructor_1(benchmark::State& state) {
	for (auto _ : state) {
		_Vec vec;
	}
}

BENCHMARK_NAME(vector_constructor_1<wjr::vector<int>>, wjr,);
BENCHMARK_NAME(vector_constructor_1<std::vector<int>>, std,);
BENCHMARK_NAME(vector_constructor_1<wjr::vector<std::string>>, wjr,);
BENCHMARK_NAME(vector_constructor_1<std::vector<std::string>>, std,);

// vector(const allocator_type& alloc)
template<typename _Vec>
static void vector_constructor_2(benchmark::State& state) {
	for (auto _ : state) {
		VEC_AL al;
		_Vec vec(al);
	}
}

BENCHMARK_NAME(vector_constructor_2<wjr::vector<int>>, wjr,);
BENCHMARK_NAME(vector_constructor_2<std::vector<int>>, std,);
BENCHMARK_NAME(vector_constructor_2<wjr::vector<std::string>>, wjr,);
BENCHMARK_NAME(vector_constructor_2<std::vector<std::string>>, std,);

// vector(size_type count, const T& value, const allocator_type& alloc = allocator_type())
template<typename _Vec>
static void vector_constructor_3(benchmark::State& state, const VEC_VAL& _Val) {
	int n = state.range(0);
	for (auto _ : state) {
		VEC_AL al;
		_Vec vec(state.range(0), _Val, al);
	}
	state.SetComplexityN(n);
}

BENCHMARK_NAME_ARGS(vector_constructor_3<wjr::vector<int>>, wjr, /0, 0)DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME_ARGS(vector_constructor_3<std::vector<int>>, std, /0, 0)DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME_ARGS(vector_constructor_3<wjr::vector<int>>, wjr, /1, 1)DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME_ARGS(vector_constructor_3<std::vector<int>>, std, /1, 1)DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME_ARGS(vector_constructor_3<wjr::vector<std::string>>, wjr,, "0")DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME_ARGS(vector_constructor_3<std::vector<std::string>>, wjr,, "0")DEFAULT_RUNNER(benchmark::oN);

// vector(size_type count, const allocator_type& alloc = allocator_type())
// TODO

// vector(iter _First, iter _Last, const allocator_type& alloc = allocator_type())
// TODO : optimize
template<typename _Vec>
static void vector_constructor_5(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	for (auto _ : state) {
		VEC_AL al;
		_Vec vec(pr.begin(), pr.end(), al);
	}
	state.SetComplexityN(n);
}

BENCHMARK_NAME(vector_constructor_5<wjr::vector<int>>, wjr,)DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_constructor_5<std::vector<int>>, std,)DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_constructor_5<wjr::vector<std::string>>, wjr,)DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_constructor_5<std::vector<std::string>>, std,)DEFAULT_RUNNER(benchmark::oN);

// vector(const vector& other)
// TODO : optimize
template<typename _Vec>
static void vector_constructor_6(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	_Vec vec(pr.begin(), pr.end());
	for (auto _ : state) {
		_Vec vec2(vec);
	}
	state.SetComplexityN(n);
}

BENCHMARK_NAME(vector_constructor_6<wjr::vector<int>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_constructor_6<std::vector<int>>, std, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_constructor_6<wjr::vector<std::string>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_constructor_6<std::vector<std::string>>, std, )DEFAULT_RUNNER(benchmark::oN);

// vector(const vector& other, const allocator_type& alloc)
// TODOT

// vector(vector&& other)
template<typename _Vec>
static void vector_constructor_8(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	for (auto _ : state) {
		_Vec vec(pr.begin(), pr.end());
		_Vec vec2(std::move(vec));
	}
	state.SetComplexityN(n);
}

BENCHMARK_NAME(vector_constructor_8<wjr::vector<int>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_constructor_8<std::vector<int>>, std, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_constructor_8<wjr::vector<std::string>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_constructor_8<std::vector<std::string>>, std, )DEFAULT_RUNNER(benchmark::oN);

// vector(vector&& other, const allocator_type& alloc)
// TODO

// vector(initializer_list<T> init, const allocator_type& alloc = allocator_type())
// TODO

// vector& operator=(const vector& other)

//case 1 : n > capacity()
template<typename _Vec>
static void vector_assignment_1_case_1(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	_Vec vec(pr.begin(), pr.end());
	for (auto _ : state) {
		_Vec vec2;
		vec2 = vec;
	}
	state.SetComplexityN(n);
}

// case 2 : size() < n && n <= capacity()
template<typename _Vec>
static void vector_assignment_1_case_2(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	_Vec vec(pr.begin(), pr.end());
	for (auto _ : state) {
		_Vec vec2;
		vec2.reserve(n);
		vec2 = vec;
	}
	state.SetComplexityN(n);
}

// case 3 : n <= size()
template<typename _Vec>
static void vector_assignment_1_case_3(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	_Vec vec(pr.begin(), pr.end());
	for (auto _ : state) {
		_Vec vec2;
		vec2.resize(n);
		vec2 = vec;
	}
	state.SetComplexityN(n);
}


BENCHMARK_NAME(vector_assignment_1_case_1<wjr::vector<int>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_1_case_1<std::vector<int>>, std, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_1_case_1<wjr::vector<std::string>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_1_case_1<std::vector<std::string>>, std, )DEFAULT_RUNNER(benchmark::oN);

BENCHMARK_NAME(vector_assignment_1_case_2<wjr::vector<int>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_1_case_2<std::vector<int>>, std, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_1_case_2<wjr::vector<std::string>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_1_case_2<std::vector<std::string>>, std, )DEFAULT_RUNNER(benchmark::oN);

BENCHMARK_NAME(vector_assignment_1_case_3<wjr::vector<int>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_1_case_3<std::vector<int>>, std, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_1_case_3<wjr::vector<std::string>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_1_case_3<std::vector<std::string>>, std, )DEFAULT_RUNNER(benchmark::oN);

// vector& operator=(vector&& other)
template<typename _Vec>
static void vector_assignment_2(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	for (auto _ : state) {
		_Vec vec(pr.begin(), pr.end());
		_Vec vec2;
		vec2 = std::move(vec);
	}
	state.SetComplexityN(n);
}

BENCHMARK_NAME(vector_assignment_2<wjr::vector<int>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_2<std::vector<int>>, std, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_2<wjr::vector<std::string>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_assignment_2<std::vector<std::string>>, std, )DEFAULT_RUNNER(benchmark::oN);

// vector& operator=(initializer_list<T> ilist)
// TODO

// vector::at(size_type pos)
template<typename _Vec>
static void vector_at(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	_Vec vec(pr.begin(), pr.end());
	for (auto _ : state) {
		for (int i = 0; i < n; ++i) {
			benchmark::DoNotOptimize(vec.at(i));
		}
	}
	state.SetComplexityN(n);
}

BENCHMARK_NAME(vector_at<wjr::vector<int>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_at<std::vector<int>>, std, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_at<wjr::vector<std::string>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_at<std::vector<std::string>>, std, )DEFAULT_RUNNER(benchmark::oN);

// vector::operator[](size_type pos)
template<typename _Vec>
static void vector_subscript(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	_Vec vec(pr.begin(), pr.end());
	for (auto _ : state) {
		for (int i = 0; i < n; ++i) {
			benchmark::DoNotOptimize(vec[i]);
		}
	}
	state.SetComplexityN(n);
}

BENCHMARK_NAME(vector_subscript<wjr::vector<int>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_subscript<std::vector<int>>, std, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_subscript<wjr::vector<std::string>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(vector_subscript<std::vector<std::string>>, std, )DEFAULT_RUNNER(benchmark::oN);

// vector::front()
// TODO

// vector::back()
// TODO

// vector::data()
// TODO

// vector::empty()
// TODO

// vector::size()
template<typename _Vec>
static void vector_size(benchmark::State& state) {
	int n = state.range(0);
	prod<VEC_VAL> pr(n);
	_Vec vec(pr.begin(), pr.end());
	for (auto _ : state) {
		benchmark::DoNotOptimize(vec.size());
	}
	state.SetComplexityN(n);
}

BENCHMARK_NAME(vector_size<wjr::vector<int>>, wjr, )DEFAULT_RUNNER(benchmark::o1);
BENCHMARK_NAME(vector_size<std::vector<int>>, std, )DEFAULT_RUNNER(benchmark::o1);
BENCHMARK_NAME(vector_size<wjr::vector<std::string>>, wjr, )DEFAULT_RUNNER(benchmark::o1);
BENCHMARK_NAME(vector_size<std::vector<std::string>>, std, )DEFAULT_RUNNER(benchmark::o1);
 
BENCHMARK_MAIN();

template<typename _Vec>
void test_vector_char(benchmark::State& state) {
	int n = state.range(0);
	prod<char> pr(n);
	for (auto _ : state) {
		_Vec vec(pr.begin(), pr.end());
	}
	state.SetComplexityN(n);
}

using A = wjr::vector<char, std::allocator<char>, wjr::vector_sso_core<char, std::allocator<char>>>;
BENCHMARK_NAME(test_vector_char<wjr::vector<char>>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(test_vector_char<std::vector<char>>, std, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(test_vector_char<A>, wjr, )DEFAULT_RUNNER(benchmark::oN);
BENCHMARK_NAME(test_vector_char<std::string>, std, )DEFAULT_RUNNER(benchmark::oN);

//BENCHMARK_MAIN();

*/

