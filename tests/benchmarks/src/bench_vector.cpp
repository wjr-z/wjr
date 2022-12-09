#include <benchmark/benchmark.h>

#include <wjr/vector.h>
#include <wjr/allocator.h>

#include <wjr/test_product.h>
#include <wjr/algorithm.h>

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