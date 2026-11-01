#include <wjr/json/pmr.hpp>
#include <wjr/memory/mimalloc.hpp>
#include <wjr/vector.hpp>

#include <array>
#include <cstdint>
#include <memory>
#include <new>
#include <string>
#include <thread>
#include <vector>

namespace {
using allocator = wjr::mimalloc_allocator<int>;
static_assert(std::is_empty_v<allocator> && std::is_trivially_copyable_v<allocator>);
static_assert(std::is_trivially_default_constructible_v<allocator>);
static_assert(std::is_nothrow_constructible_v<allocator, wjr::mimalloc_allocator<char>>);
static_assert(std::allocator_traits<allocator>::is_always_equal::value);
static_assert(std::same_as<std::allocator_traits<allocator>::rebind_alloc<char>,
                           wjr::mimalloc_allocator<char>>);
static_assert(wjr::is_trivially_allocator_construct_v<allocator, int, int &&>);
static_assert(wjr::is_trivially_allocator_destroy_v<allocator, int>);
static_assert(sizeof(allocator) == sizeof(std::allocator<int>));
static_assert(sizeof(wjr::mimalloc_memory_resource) == sizeof(std::pmr::memory_resource));

struct alignas(4096) page {
    unsigned char data[4096];
};

int typed_storage() {
    wjr::mimalloc_allocator<page> pages;
    for (std::size_t count : {0u, 1u, 3u, 16u}) {
        auto *pointer = pages.allocate(count);
        if (reinterpret_cast<uintptr_t>(pointer) % alignof(page) != 0 || !mi_check_owned(pointer))
            return 1;
        if (count) {
            pointer[0].data[0] = 1;
            pointer[count - 1].data[4095] = 2;
        }
        pages.deallocate(pointer, count);
    }
    {
        wjr::vector<std::unique_ptr<int>, wjr::mimalloc_allocator<std::unique_ptr<int>>> values;
        for (int i = 0; i != 128; ++i)
            values.emplace_back(std::make_unique<int>(i));
        values.reserve(512);
        auto moved = std::move(values);
        for (int i = 0; i != 128; ++i)
            if (*moved[i] != i)
                return 2;
    }
    std::vector<std::string, wjr::mimalloc_allocator<std::string>> strings(4, std::string(80, 'x'));
    if (!mi_check_owned(strings.data()) || strings[3].size() != 80)
        return 3;
    return 0;
}

int resource_storage() {
    wjr::mimalloc_memory_resource resource, other;
    if (!resource.is_equal(resource) || resource.is_equal(other) ||
        resource.is_equal(*std::pmr::new_delete_resource()))
        return 4;
    for (std::size_t alignment : {1u, 8u, 16u, 64u, 4096u, 65536u})
        for (std::size_t bytes : {0u, 1u, 17u, 4097u}) {
            auto *pointer = resource.allocate(bytes, alignment);
            if (reinterpret_cast<uintptr_t>(pointer) % alignment || !mi_check_owned(pointer))
                return 5;
            if (bytes) {
                auto *payload = static_cast<volatile unsigned char *>(pointer);
                payload[0] = 1;
                payload[bytes - 1] = 2;
            }
            resource.deallocate(pointer, bytes, alignment);
        }
    std::array<void *, 256> pointers{};
    std::thread producer([&] {
        for (auto &pointer : pointers)
            pointer = resource.allocate(80, 32);
    });
    producer.join();
    std::thread consumer([&] {
        for (auto *pointer : pointers)
            resource.deallocate(pointer, 80, 32);
    });
    consumer.join();

    std::pmr::vector<std::pmr::string> nested(&resource);
    nested.emplace_back(80, 'a');
    if (nested.front().get_allocator().resource() != &resource ||
        !mi_check_owned(nested.front().data()))
        return 6;
    wjr::json::ondemand_reader reader;
    reader.read(
        R"({"description":"long owned string backed by the selected resource","items":[1,2,3]})");
    auto parsed = wjr::json::pmr_document::parse_with_allocator(
        reader, std::pmr::polymorphic_allocator<std::byte>(&resource));
    if (!parsed || parsed->get_allocator().resource() != &resource)
        return 7;
    const auto &object = parsed->get<wjr::json::object_t>();
    const auto &text = object.find("description")->second.get<wjr::json::string_t>();
    if (text.get_allocator().resource() != &resource || !mi_check_owned(text.data()))
        return 8;
#if !defined(WJR_DISABLE_EXCEPTIONS)
    try {
        wjr::mimalloc_allocator<uint64_t> integers;
        (void)integers.allocate(integers.max_size() + 1);
        return 9;
    } catch (const std::bad_alloc &) {
    }
    try {
        (void)resource.allocate(std::numeric_limits<std::size_t>::max(), 16);
        return 10;
    } catch (const std::bad_alloc &) {
    }
#endif
    return 0;
}
} // namespace

int main() {
    if (int result = typed_storage())
        return result;
    return resource_storage();
}
