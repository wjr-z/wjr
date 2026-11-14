#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <locale>
#include <string>
#include <vector>
#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
#else
    #include <dlfcn.h>
    #include <gnu/libc-version.h>
#endif
#include <wjr/json/document.hpp>

using document = wjr::json::document;
using array = document::array_type;
static_assert(wjr::get_relocate_mode_v<document> == wjr::relocate_t::trivial);
static_assert(std::is_nothrow_destructible_v<document>);

WJR_NOINLINE void existing_erase(array &values, size_t position, size_t count) {
    values.erase(values.cbegin() + position, values.cbegin() + position + count);
}
WJR_NOINLINE void handwritten_erase(array &values, size_t position, size_t count) {
    if (count == 0)
        return;
    auto *first = values.data() + position;
    auto *last = first + count;
    auto *end = values.data() + values.size();
    std::destroy(first, last);
    if (last != end)
        std::memmove(first, last, size_t(end - last) * sizeof(document));
    values.get_storage().size() = values.size() - count;
}

std::string make_input(size_t count) {
    std::string input = "[";
    for (size_t index = 0; index != count; ++index) {
        if (index)
            input += ',';
        switch (index % 4) {
        case 0:
            input += std::to_string(index);
            break;
        case 1:
            input += "\"owned long string payload-" + std::to_string(index) + "\"";
            break;
        case 2:
            input += "{\"id\":" + std::to_string(index) + ",\"name\":\"record payload\"}";
            break;
        default:
            input += "[true,null,1.25]";
            break;
        }
    }
    input += ']';
    return input;
}

WJR_NOINLINE void existing_insert(array &values, size_t position, size_t count,
                                  const array &incoming) {
    values.insert_range(values.cbegin() + position, std::span(incoming.data(), count));
}

int main(int argc, char **argv) {
#ifdef _WIN32
    if (!SetThreadAffinityMask(GetCurrentThread(), 1))
        return 1;
#endif
#ifndef _WIN32
    void *backend = dlsym(RTLD_DEFAULT, "memmove");
    Dl_info info{};
    dladdr(backend, &info);
    std::fprintf(stderr, "glibc=%s memmove_offset=0x%zx\n", gnu_get_libc_version(),
                 reinterpret_cast<std::uintptr_t>(backend) -
                     reinterpret_cast<std::uintptr_t>(info.dli_fbase));
#endif
    const bool inserting = argc > 1 && std::string_view(argv[1]) == "insert";
    std::cout << "size,count,repeat,mode,ns_per_call\n";
    for (size_t size : {128, 1024, 4096}) {
        auto input = make_input(size);
        wjr::json::ondemand_reader reader;
        reader.read(input);
        auto source = wjr::json::document::parse(reader);
        if (!source)
            return 2;
        const auto &original = source->get<wjr::json::array_t>();
        for (size_t count : {size_t{1}, size_t{16}, size / 2}) {
            const auto position = size / 4;
            auto expected = original;
            if (inserting)
                existing_insert(expected, position, count, original);
            else
                existing_erase(expected, position, count);
            for (int repeat = 0; repeat != 11; ++repeat) {
                for (int step = 0; step != 1; ++step) {
                    const bool manual = false;
                    std::vector<array> batches;
                    for (int batch = 0; batch != 32; ++batch) {
                        batches.emplace_back(original);
                        if (inserting)
                            batches.back().reserve(size + count);
                    }
                    auto operation = manual ? handwritten_erase : existing_erase;
                    std::atomic_signal_fence(std::memory_order_seq_cst);
                    const auto begin = std::chrono::steady_clock::now();
                    for (auto &batch : batches) {
                        if (inserting)
                            existing_insert(batch, position, count, original);
                        else
                            operation(batch, position, count);
                    }
                    const auto end = std::chrono::steady_clock::now();
                    for (const auto &batch : batches)
                        if (!std::ranges::equal(batch, expected))
                            std::abort();
                    const auto seconds = std::chrono::duration<double>(end - begin).count();
                    std::cout << size << ',' << count << ',' << repeat << ','
                              << (manual ? "handwritten" : "existing") << ','
                              << seconds * 1e9 / batches.size() << '\n';
                }
            }
        }
    }
}
