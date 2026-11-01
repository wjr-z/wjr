#include <wjr/json/pmr.hpp>
#include <wjr/memory/mimalloc.hpp>
#include <mimalloc.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <memory_resource>
#include <new>
#include <string>

using mi_resource = wjr::mimalloc_memory_resource;

volatile size_t checksum;
WJR_NOINLINE size_t batch(std::string_view text, std::pmr::memory_resource* resource, size_t count) {
    wjr::json::ondemand_reader reader;
    size_t total=0;
    for(size_t i=0;i<count;++i) {
        reader.read(text);
        auto parsed=wjr::json::pmr_document::parse_with_allocator(reader, std::pmr::polymorphic_allocator<std::byte>(resource));
        if(!parsed) std::abort();
        total+=parsed->template get<wjr::json::object_t>().size();
    }
    return total;
}

#if defined(_WIN32)
extern "C" __declspec(dllimport) void* __stdcall GetCurrentProcess();
extern "C" __declspec(dllimport) int __stdcall SetProcessAffinityMask(void*, uintptr_t);
#endif

int main(int argc,char** argv) {
#if defined(_WIN32)
    if(!SetProcessAffinityMask(GetCurrentProcess(),4)) std::abort();
#endif
    std::ifstream input(argv[1],std::ios::binary);
    std::string twitter{std::istreambuf_iterator<char>(input),{}};
    if(twitter.size()<1000) return 1;
    std::string small=R"({"name":"small record sufficiently long for heap string storage","ids":[1,2,3,4,5,6,7,8],"nested":{"text":"reused long user string value with real ownership"}})";
    std::string wide="{";
    for(int i=0;i<64;++i) {
        if(i)wide+=',';
        wide+='"'+std::to_string(i)+"\":\"non-sso dynamically allocated value text for the key\"";
    }
    wide+='}';
    mi_resource resource;
    std::pmr::unsynchronized_pool_resource pool;
    auto* standard=std::pmr::new_delete_resource();
    auto* pointer=standard->allocate(64);
    if(mi_check_owned(pointer)) return 2;
    standard->deallocate(pointer,64);
    std::pmr::memory_resource* resources[]={standard,&resource,&pool};
    const char* names[]={"std_new_delete","mimalloc","std_unsynchronized_pool"};
    std::puts("case,resource,sample,iterations,ns_per_document");
    for(auto item:{std::pair{"small",&small},std::pair{"wide",&wide},std::pair{"twitter",&twitter}}) {
        size_t count=1;
        while(count<1048576) {
            auto begin=std::chrono::steady_clock::now();
            checksum=batch(*item.second,standard,count);
            if(std::chrono::steady_clock::now()-begin>std::chrono::milliseconds(80))break;
            count*=2;
        }
        for(auto* provider:resources) checksum=batch(*item.second,provider,count);
        for(int sample=0;sample<7;++sample)for(int order=0;order<3;++order) {
            int index=(order+sample)%3;
            auto begin=std::chrono::steady_clock::now();
            checksum=batch(*item.second,resources[index],count);
            double ns=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-begin).count()/count;
            std::printf("%s,%s,%d,%zu,%.6f\n",item.first,names[index],sample,count,ns);
        }
    }
}
