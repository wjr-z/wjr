#include <benchmark/benchmark.h>
#include <sched.h>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <limits>
#include <utility>
struct Item { uint64_t lo, hi, d0, d1; };
struct Result { uint64_t quotient, lo, hi; };
using Fn = void (*)(const Item *, Result *, size_t);
using CmpFn = unsigned (*)(uint64_t,uint64_t,uint64_t,uint64_t);
extern "C" void old_divide(const Item *, Result *, size_t);
extern "C" void new_divide(const Item *, Result *, size_t);
extern "C" unsigned old_compare(uint64_t,uint64_t,uint64_t,uint64_t);
extern "C" unsigned new_compare(uint64_t,uint64_t,uint64_t,uint64_t);
static std::array<std::array<Item,256>,3> inputs;
static std::array<Result,256> output;
static uint64_t random64() {
    static uint64_t state = 0x1e4affb036d2ab59;
    state ^= state >> 12; state ^= state << 25; state ^= state >> 27;
    return state * UINT64_C(2685821657736338717);
}
static void check() {
    for (CmpFn fn : {old_compare,new_compare}) {
        constexpr std::array<uint64_t,7> edges={0,1,2,UINT64_C(1)<<63,(UINT64_C(1)<<63)-1,UINT64_MAX-1,UINT64_MAX};
        for (auto a:edges) for(auto b:edges) for(auto c:edges) for(auto d:edges) {
            const __uint128_t lhs=(__uint128_t(a)<<64)|b, rhs=(__uint128_t(c)<<64)|d;
            if(fn(b,a,d,c)!=(unsigned(lhs<rhs)|(unsigned(lhs<=rhs)<<1))) std::abort();
        }
        for(size_t i=0;i<100000;++i) {
            const auto a=random64(), b=random64(), c=random64(), d=random64();
            const __uint128_t lhs=(__uint128_t(a)<<64)|b, rhs=(__uint128_t(c)<<64)|d;
            if(fn(b,a,d,c)!=(unsigned(lhs<rhs)|(unsigned(lhs<=rhs)<<1))) std::abort();
        }
    }
    for(size_t mode=0;mode<inputs.size();++mode) for(auto &x:inputs[mode]) {
        x={random64(), random64(),random64(),random64()|(UINT64_C(1)<<63)};
        if(mode==1) x.hi=x.d1;
        if(mode==2) {x.hi=x.d1; x.lo=x.d0;}
    }
    for(Fn fn:{old_divide,new_divide}) for(const auto &data:inputs) {
        fn(data.data(),output.data(),data.size());
        for(size_t i=0;i<data.size();++i) {
            const auto &x=data[i]; const auto &out=output[i];
            const __uint128_t n=(__uint128_t(x.hi)<<64)|x.lo, d=(__uint128_t(x.d1)<<64)|x.d0;
            if(out.quotient!=n/d || out.lo!=uint64_t(n%d) || out.hi!=uint64_t((n%d)>>64)) std::abort();
        }
    }
    std::puts("edge aliases, 100000 random comparisons and actual public division results passed");
}
static void run(benchmark::State &state,Fn fn) {
    const auto &data=inputs[state.range(0)];
    for(auto _:state) {
        fn(data.data(),output.data(),data.size());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations()*data.size());
}
int main(int argc,char **argv) {
    cpu_set_t allowed,selected;
    if(sched_getaffinity(0,sizeof(allowed),&allowed)) return 2;
    CPU_ZERO(&selected); int cpu=0;
    while(cpu<CPU_SETSIZE&&!CPU_ISSET(cpu,&allowed)) ++cpu;
    if(cpu==CPU_SETSIZE) return 2;
    CPU_SET(cpu,&selected);
    if(sched_setaffinity(0,sizeof(selected),&selected)) return 2;
    check();
    for(auto entry:{std::pair<const char*,Fn>{"old",old_divide},{"new",new_divide}})
        benchmark::RegisterBenchmark(entry.first,run,entry.second)->Arg(0)->Arg(1)->Arg(2)->UseRealTime();
    benchmark::Initialize(&argc,argv);
    if(benchmark::ReportUnrecognizedArguments(argc,argv)) return 1;
    benchmark::RunSpecifiedBenchmarks(); benchmark::Shutdown();
}
