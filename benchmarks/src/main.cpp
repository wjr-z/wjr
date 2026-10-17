#include "detail.hpp"

int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }
    benchmark::AddCustomContext("wjr_compiler", WJR_BENCHMARK_COMPILER);
    benchmark::AddCustomContext("wjr_cxx_standard", WJR_BENCHMARK_CXX_STANDARD);
    benchmark::AddCustomContext("wjr_cpu_target", WJR_BENCHMARK_CPU_TARGET);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}