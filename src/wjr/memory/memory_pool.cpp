#include <array>

#include <wjr/assert.hpp>
#include <wjr/memory/memory_pool.hpp>

namespace wjr {

char *__default_alloc_template__::object::chunk_alloc(unsigned int idx,
                                                      unsigned int &nobjs) noexcept {
    const size_t size = __get_size(idx);
    const size_t total_bytes = size * nobjs;
    auto bytes_left = static_cast<size_t>(end_free - start_free);

    if (bytes_left >= total_bytes) {
        auto *const result = start_free;
        start_free += total_bytes;
        return result;
    }

    if (bytes_left >= size) {
        nobjs = bytes_left >> (idx + 3);
        auto *const result = start_free;
        start_free += nobjs << (idx + 3);
        return result;
    }

    // Try to make use of the left-over piece.
    if (bytes_left != 0) {
        WJR_ASSERT(!(bytes_left & 7));

        unsigned int __idx = __get_index(bytes_left);
        unsigned int __size = __get_size(__idx);
        for (;; --__idx, __size >>= 1) {
            if (bytes_left >= __size) {
                obj *volatile *my_free_list = free_list + __idx;
                reinterpret_cast<obj *>(start_free)->free_list_link = *my_free_list;
                *my_free_list = reinterpret_cast<obj *>(start_free);

                if (bytes_left == __size) {
                    break;
                }

                start_free += __size;
                bytes_left -= __size;
            }
        }
    }

    const size_t bytes_to_get = 2 * total_bytes + __round_up(heap_size >> 3);

    start_free = static_cast<char *>(get_chunk().allocate(bytes_to_get));
    WJR_ASSERT(start_free != nullptr);

    heap_size += bytes_to_get;
    end_free = start_free + bytes_to_get;
    return (chunk_alloc(idx, nobjs));
}

static constexpr std::array<uint8_t, 12> __nobjs_table = {32, 32, 32, 32, 16, 16,
                                                          16, 16, 8,  8,  4,  4};

void *__default_alloc_template__::object::refill(unsigned int idx) noexcept {
    auto nobjs = static_cast<unsigned int>(__nobjs_table[idx]);
    char *const chunk = chunk_alloc(idx, nobjs);
    obj *current_obj;
    obj *next_obj;

    if (nobjs == 1) {
        return chunk;
    }

    obj *volatile *my_free_list = free_list + idx;
    const size_t n = __get_size(idx);

    // Build free list in chunk
    auto *const result = reinterpret_cast<obj *>(chunk);

    *my_free_list = current_obj = reinterpret_cast<obj *>(chunk + n);
    nobjs -= 2;
    while (nobjs) {
        --nobjs;
        next_obj = reinterpret_cast<obj *>((char *)current_obj + n);
        current_obj->free_list_link = next_obj;
        current_obj = next_obj;
    }

    current_obj->free_list_link = nullptr;
    return (result);
}

} // namespace wjr