#ifndef WJR_BENCHMARKS_NP_DETAIL_HPP__
#define WJR_BENCHMARKS_NP_DETAIL_HPP__

#include <benchmark/benchmark.h>

namespace np_benchmark {

// Escape only the preparation temporary. The returned value is an independent
// snapshot, so loop barriers cannot force one backend to reload live inputs.
template <class T>
inline T runtime_input(T value) {
    benchmark::DoNotOptimize(&value);
    return value;
}

} // namespace np_benchmark

#endif // WJR_BENCHMARKS_NP_DETAIL_HPP__
