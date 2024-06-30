#include <wjr/assert.hpp>
#include <wjr/memory/memory_pool.hpp>

namespace wjr {

char *__default_alloc_template__::object::chunk_alloc(uint8_t idx, int &nobjs) noexcept {
    const size_t size = __get_size(idx);
    const size_t total_bytes = size * nobjs;
    auto bytes_left = static_cast<size_t>(end_free - start_free);

    if (bytes_left >= total_bytes) {
        const auto result = start_free;
        start_free += total_bytes;
        return result;
    }

    if (bytes_left >= size) {
        nobjs = bytes_left >> (idx + 3);
        const auto result = start_free;
        start_free += size * nobjs;
        return result;
    }

    // Try to make use of the left-over piece.
    if (bytes_left != 0) {
        WJR_ASSERT(!(bytes_left & 7));

        uint8_t __idx = __get_index(bytes_left);
        for (;; --__idx) {
            const auto __size = __get_size(__idx);
            if (bytes_left >= __size) {
                obj *volatile *my_free_list = free_list + __idx;
                ((obj *)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj *)start_free;

                if (bytes_left == __size) {
                    break;
                }

                start_free += __size;
                bytes_left -= __size;
            }
        }
    }

    const size_t bytes_to_get = 2 * total_bytes + __round_up(heap_size >> 3);

    start_free = (char *)get_chunk().allocate(bytes_to_get);
    WJR_ASSERT(start_free != nullptr);

    heap_size += bytes_to_get;
    end_free = start_free + bytes_to_get;
    return (chunk_alloc(idx, nobjs));
}

void *__default_alloc_template__::object::refill(uint8_t idx) noexcept {
    int nobjs = idx < 4 ? 32 : idx < 8 ? 16 : 4;
    char *chunk = chunk_alloc(idx, nobjs);
    obj *current_obj;
    obj *next_obj;

    if (1 == nobjs) {
        return (chunk);
    }

    obj *volatile *my_free_list = free_list + idx;

    const size_t n = __get_size(idx);

    // Build free list in chunk
    obj *result = (obj *)chunk;

    *my_free_list = current_obj = (obj *)(chunk + n);
    nobjs -= 2;
    while (nobjs) {
        --nobjs;
        next_obj = (obj *)((char *)current_obj + n);
        current_obj->free_list_link = next_obj;
        current_obj = next_obj;
    }
    current_obj->free_list_link = 0;
    return (result);
}

} // namespace wjr