#define main paired_driver_main
#include "driver.cpp"
#undef main
#define strong_reference simd_owned
#include "simdjson-reference.hpp"
#undef strong_reference
#define strong_reference hand_owned
#include "hand-reference.hpp"
#undef strong_reference

template<class Function> double stage_time(Function&& function,size_t iterations) {
    uint64_t sum=0;
    const auto begin=std::chrono::steady_clock::now();
    for(size_t i=0;i!=iterations;++i) {
        std::atomic_signal_fence(std::memory_order_seq_cst);
        sum+=function();
    }
    sink=sum;
    return std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count();
}
int main() {
    pin_cpu();
    auto fixtures=probe::fixtures();
    std::cout << "fixture,repeat,iterations,stage,seconds\n";
    for(auto& fixture:fixtures) {
        if(fixture.name.find("records-")!=0) continue;
        wjr::json::ondemand_reader reader;
        simd_owned::parser simd(fixture.input);
        hand_owned::parser hand(fixture.input);
        const size_t iterations=1024;
        for(int repeat=0;repeat!=7;++repeat) {
            auto print=[&](const char* label,double seconds) {
                std::cout << fixture.name << ',' << repeat << ',' << iterations << ',' << label << ',' << seconds << '\n';
            };
            print("wjr-tokenize",stage_time([&] {
                reader.read(fixture.input);
                uint64_t sum=0;
                for(auto token=reader.get();token!=wjr::json::ondemand_reader::npos;token=reader.get()) sum+=token;
                return sum;
            },iterations));
            print("simd-stage1-init",stage_time([&] {
                auto document=simd.reader.iterate(simd.input);
                return static_cast<uint64_t>(document.type().value());
            },iterations));
            print("wjr-check",stage_time([&] {
                reader.read(fixture.input);
                auto result=wjr::json::check(reader);
                if(!result) std::abort();
                return uint64_t{1};
            },iterations));
            print("wjr-direct",stage_time([&] {
                reader.read(fixture.input);
                return fixture.direct(reader);
            },iterations));
            print("wjr-hand",stage_time([&] {return hand.run();},iterations));
            print("simd-hand",stage_time([&] {return simd.run();},iterations));
        }
    }
}
