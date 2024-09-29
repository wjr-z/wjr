#include <array>
#include <mutex>

#include <wjr/assert.hpp>
#include <wjr/concurrency/lkf_forward_list.hpp>
#include <wjr/memory/memory_pool.hpp>
#include <wjr/memory/uninitialized.hpp>

#ifndef WJR_MEMORY_POOL_ARENA_NUM
    #define WJR_MEMORY_POOL_ARENA_NUM 4
#endif

namespace wjr {

using namespace intrusive;

namespace memory {

namespace {

/**
 * @todo Add level to bin slow start.
 *
 */
inline constexpr unsigned int bin_slow_start_table[bin_table_size] = {
    32, 32, 32, 32, 16, 16, 16, 16, 8, 8, 8, 8, 4, 2};

inline constexpr unsigned int bin_table_is_fast_div = 0b11'010'101'010'111;
inline constexpr uint32_t bin_table_fast_div_mul = 2863311531;
inline constexpr uint8_t bin_table_fast_div_rshift[bin_table_size] = {
    3, 4, 5, 37, 6, 38, 7, 39, 8, 40, 9, 41, 10, 11};

inline constexpr unsigned int max_bin_cached_table[bin_table_size] = {
    4096, 4096, 4096, 4096, 2048, 2048, 2048, 2048, 1024, 1024, 1024, 512, 512, 256};

inline constexpr unsigned int max_bin_cached_threshold_table[bin_table_size] = {
    32, 32, 32, 32, 24, 24, 24, 24, 16, 16, 16, 12, 12, 8};

inline constexpr unsigned int global_heap_slow_start_max_level = 15;

inline constexpr uint32_t global_heap_slow_start_table[16] = {
    2_MB, 2_MB, 2_MB, 2_MB, 4_MB,  4_MB,  4_MB,  4_MB,
    8_MB, 8_MB, 8_MB, 8_MB, 16_MB, 16_MB, 16_MB, 16_MB};

inline constexpr unsigned int arena_num = WJR_MEMORY_POOL_ARENA_NUM;
static_assert(has_single_bit(arena_num), "");

using object = thread_cache_object;
using obj = typename object::obj;

struct obj_linker {
    intrusive::forward_list_node link;
    obj *head;
    unsigned int size;
};

static_assert(std::is_standard_layout_v<obj_linker>, "");

inline constexpr unsigned int obj_linker_index = get_bin_index(sizeof(obj_linker));

struct heap_linker {
    intrusive::forward_list_node link;
    uint32_t size;
};

WJR_INTRINSIC_INLINE intrusive::forward_list_node *get_obj_node(obj *ptr) {
    return &ptr->link;
}

WJR_INTRINSIC_INLINE obj *get_obj(intrusive::forward_list_node *node) {
    return WJR_CONTAINER_OF(node, obj, link);
}

WJR_INTRINSIC_INLINE obj_linker *get_obj_linker(intrusive::forward_list_node *node) {
    return WJR_CONTAINER_OF(node, obj_linker, link);
}

WJR_INTRINSIC_INLINE heap_linker *get_heap_linker(intrusive::forward_list_node *node) {
    return WJR_CONTAINER_OF(node, heap_linker, link);
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR unsigned int bin_fast_divide(unsigned int size,
                                                               unsigned int idx) {
    if ((bin_table_is_fast_div >> idx) & 1) {
        return size >> bin_table_fast_div_rshift[idx];
    }

    return (static_cast<uint64_t>(size) * bin_table_fast_div_mul) >>
           bin_table_fast_div_rshift[idx];
}

WJR_MALLOC WJR_INTRINSIC_INLINE void *__forceinline_allocate_impl(object &object,
                                                                  unsigned int idx);

WJR_INTRINSIC_INLINE void __forceinline_reuse_heap(object &object, char *mem,
                                                   unsigned int bytes_left) noexcept;

WJR_INTRINSIC_INLINE void __forceinline_deallocate_impl(object &object, void *mem,
                                                        unsigned int idx) noexcept
    WJR_NONNULL(2);

class global_memory_pool {
public:
    global_memory_pool() noexcept {
        for (auto &head : bin_head) {
            init(&head);
        }

        for (auto &head : recycle_bin_head) {
            init(&head);
        }

        init(&linker_head);
        init(&heap_head);

        heap_stamp.store(0, std::memory_order_relaxed);
        heap_level.store(0, std::memory_order_relaxed);
        arena_id.store(0, std::memory_order_relaxed);
    }

    WJR_INTRINSIC_INLINE obj_linker *raw_linker_allocate() noexcept {
        return get_obj_linker(pop_front(&linker_head));
    }

    WJR_INTRINSIC_INLINE obj_linker *linker_allocate(object *obj) {
        if (obj_linker *linker = raw_linker_allocate(); linker != nullptr) {
            return linker;
        }

        return static_cast<obj_linker *>(
            __forceinline_allocate_impl(*obj, obj_linker_index));
    }

    WJR_INTRINSIC_INLINE void linker_deallocate(obj_linker *linker) noexcept {
        push_front(&linker_head, &linker->link);
    }

    WJR_INTRINSIC_INLINE WJR_MALLOC obj_linker *bin_allocate(unsigned int idx) noexcept {
        return get_obj_linker(pop_front(bin_head + idx));
    }

    WJR_INTRINSIC_INLINE void bin_deallocate(obj_linker *linker,
                                             unsigned int idx) noexcept {
        push_front(bin_head + idx, &linker->link);
    }

    WJR_INTRINSIC_INLINE void recycle_bin(obj_linker *linker, unsigned int idx) noexcept {
        if (WJR_LIKELY(linker->size < max_bin_cached_threshold_table[idx])) {
            push_front(recycle_bin_head + idx, &linker->link);
            return;
        }

        bin_deallocate(linker, idx);
    }

    WJR_INTRINSIC_INLINE void heap_deallocate(heap_linker *linker) noexcept {
        push_front(&heap_head, &linker->link);
    }

    WJR_INTRINSIC_INLINE void *heap_allocate(unsigned int &alloc_size) noexcept {
        const unsigned int stamp = heap_stamp.fetch_add(1, std::memory_order_relaxed);

        if (heap_linker *const result = get_heap_linker(pop_front(&heap_head));
            result != nullptr) {

            if (WJR_LIKELY(result->size > alloc_size)) {
                const unsigned int rest_size = result->size - alloc_size;
                if (WJR_LIKELY(rest_size >= bin_page_size)) {
                    heap_linker *const linker = reinterpret_cast<heap_linker *>(
                        reinterpret_cast<char *>(result) + alloc_size);
                    linker->size = rest_size;
                    heap_deallocate(linker);
                    return result;
                } 
            }

            alloc_size = result->size;
            WJR_ASSERT(alloc_size >= bin_page_size);
            return result;
        }

        int level = static_cast<int>(heap_level.load(std::memory_order_relaxed));

        // Maybe some small heap merged to list.
        // Maintain dynamically.
        if (heap_stamp_mutex.try_lock()) {
            if (const auto diff_stamp = static_cast<int>(stamp - heap_slow_start_stamp);
                diff_stamp < 16) {
                if (diff_stamp < 6) {
                    level += 2;
                } else {
                    level += 1;
                }

                level =
                    static_cast<unsigned int>(level) > global_heap_slow_start_max_level
                        ? global_heap_slow_start_max_level
                        : level;
            } else if (diff_stamp > 32) {
                if (diff_stamp < 42) {
                    level -= 1;
                } else {
                    level -= 2;
                }

                level = level < 0 ? 0 : level;
            }

            heap_slow_start_stamp = stamp;
            heap_level.store(level, std::memory_order_relaxed);

            heap_stamp_mutex.unlock();
        }

        const uint32_t heap_alloc_size = global_heap_slow_start_table[level];
        const unsigned int id = get_arena_id();

        char *result;

        {
            auto &mutex = arena[id].heap_mutex;
            std::lock_guard<std::mutex> lock(mutex);
            result = new char[heap_alloc_size];
        }

        heap_linker *linker = reinterpret_cast<heap_linker *>(result + alloc_size);
        linker->size = heap_alloc_size - alloc_size;

        heap_deallocate(linker);
        return result;
    }

    WJR_INTRINSIC_INLINE unsigned int get_arena_id() noexcept {
        return arena_id.fetch_add(1, std::memory_order_relaxed) % arena_num;
    }

private:
    // size >= max_bin_cached_threshold_table[idx]
    lkf_forward_list_node bin_head[bin_table_size];

    // size < max_bin_cached_threshold_table[idx]
    // This is not used currently.
    lkf_forward_list_node recycle_bin_head[bin_table_size];

    lkf_forward_list_node linker_head;

    lkf_forward_list_node heap_head;
    std::mutex heap_stamp_mutex;
    std::atomic<unsigned int> heap_stamp;
    std::atomic<unsigned int> heap_level;
    unsigned int heap_slow_start_stamp = 0;

    /// @todo : optimize
    struct arena_t {
        std::mutex heap_mutex;
    };

    std::atomic<unsigned int> arena_id;
    arena_t arena[arena_num];
};

global_memory_pool global_memory_pool_object;

class thread_cache_recycler {
public:
    thread_cache_recycler(object *obj_) noexcept : m_obj(obj_) {}

    WJR_NOINLINE ~thread_cache_recycler() noexcept;

    /**
     * @brief Use guard when thread_cache_object changes from an empty state to a non
     * empty state.
     *
     */
    static void guard(object *obj_) noexcept {
        thread_local thread_cache_recycler recycler(obj_);
    }

private:
    object *m_obj;
};

thread_cache_recycler::~thread_cache_recycler() noexcept {
    static constexpr auto linker_index = obj_linker_index;
    static constexpr auto linker_size = bin_table[linker_index];
    static constexpr auto alloc_linker_size = bin_table_size * 2;
    static_assert(alloc_linker_size < max_bin_cached_table[bin_table_size - 1], "");

    bool use_fast_heap;
    obj_linker *fast_heap_linker;

    // No enough heap memory for max memory usage of linker.
    if (auto heap_size = static_cast<size_t>(m_obj->end_free - m_obj->start_free);
        heap_size < linker_size * alloc_linker_size) {
        // don't need to initialize fast_heap_linker
        use_fast_heap = false;
        if (heap_size) {
            __forceinline_reuse_heap(*m_obj, m_obj->start_free, heap_size);
        }
    } else {
        use_fast_heap = true;
        // Avoid to use global memory or new memory.
        fast_heap_linker = reinterpret_cast<obj_linker *>(m_obj->start_free);

        m_obj->start_free += linker_size * alloc_linker_size;
        heap_size -= linker_size * alloc_linker_size;

        if (heap_size < bin_page_size) {
            __forceinline_reuse_heap(*m_obj, m_obj->start_free, heap_size);
        } else {
            heap_linker *linker = reinterpret_cast<heap_linker *>(m_obj->start_free);
            linker->size = heap_size;
            global_memory_pool_object.heap_deallocate(linker);
        }
    }

    unsigned int cnt = 0;

    // May be optimized to use SIMD
    for (unsigned int idx = 0; idx < bin_table_size; ++idx) {
        cnt += m_obj->free_list_size[idx] != 0;
    }

    // May be optimized to use SIMD
    for (unsigned int idx = 0; idx < bin_table_size; ++idx) {
        cnt += m_obj->cached_free_list_size[idx] != 0;
    }

    // Use linker from heap.
    if (use_fast_heap) {
        WJR_ASSERT(fast_heap_linker != nullptr);

        for (unsigned int idx = 0; idx < bin_table_size; ++idx) {
            if (m_obj->free_list_size[idx] != 0) {
                obj_linker *const linker = fast_heap_linker++;
                linker->head = m_obj->free_list[idx];
                linker->size = m_obj->free_list_size[idx];

                global_memory_pool_object.recycle_bin(linker, idx);
            }

            if (m_obj->cached_free_list_size[idx] != 0) {
                obj_linker *const linker = fast_heap_linker++;
                linker->head = m_obj->cached_free_list[idx];
                linker->size = m_obj->cached_free_list_size[idx];

                global_memory_pool_object.recycle_bin(linker, idx);
            }
        }

        // Recycle excess linker to global.
        while (cnt != alloc_linker_size) {
            global_memory_pool_object.linker_deallocate(fast_heap_linker++);
            ++cnt;
        }

        return;
    }

    obj_linker *linker_head[linker_size];
    unsigned int head_idx = 0;

    // Try to get linker from free_list[linker_index], reserve a size of 4 for
    // free_list[linker_index]
    if (unsigned int obj_linker_size = m_obj->free_list_size[linker_index];
        obj_linker_size > 4) {
        auto total = std::min<unsigned int>(obj_linker_size - 4, cnt);
        unsigned int iter = total;

        obj *current_obj = m_obj->free_list[linker_index];
        do {
            linker_head[--iter] = reinterpret_cast<obj_linker *>(current_obj);
            current_obj = get_obj(current_obj->link.next);
        } while (iter);

        // Modify
        m_obj->free_list[linker_index] = current_obj;
        m_obj->free_list_size[linker_index] -= total;

        head_idx = total;
    }

    // Try to get linker from global.
    while (head_idx != cnt) {
        // This is very unlikely. Maybe can be optimmized.
        if (auto linker = global_memory_pool_object.raw_linker_allocate();
            linker == nullptr) {
            auto *ptr = new obj_linker[cnt - head_idx];
            while (head_idx != cnt) {
                linker_head[head_idx++] = ptr++;
            }
            break;
        } else {
            linker_head[head_idx++] = linker;
        }
    }

    head_idx = 0;
    for (unsigned int idx = 0; idx < bin_table_size; ++idx) {
        if (m_obj->free_list_size[idx] != 0) {
            obj_linker *linker = linker_head[head_idx++];
            linker->head = m_obj->free_list[idx];
            linker->size = m_obj->free_list_size[idx];

            global_memory_pool_object.recycle_bin(linker, idx);
        }

        if (m_obj->cached_free_list_size[idx] != 0) {
            obj_linker *linker = linker_head[head_idx++];
            linker->head = m_obj->cached_free_list[idx];
            linker->size = m_obj->cached_free_list_size[idx];

            global_memory_pool_object.recycle_bin(linker, idx);
        }
    }
}

inline void *__forceinline_allocate_impl(object &object, unsigned int idx) {
    // If size is zero, then it will be overwrite later.
    // Reducing dependencies can improve performance.
    --object.free_list_size[idx];

    // Fast path. Get from free_list if possible.
    if (obj *const result = object.free_list[idx]; WJR_LIKELY(result != nullptr)) {
        object.free_list[idx] = get_obj(result->link.next);
        return result;
    }

    if (obj *const result = object.cached_free_list[idx]; WJR_LIKELY(result != nullptr)) {
        // Exchange cached_free_list with free_list.

        object.free_list[idx] = get_obj(result->link.next);
        object.cached_free_list[idx] = nullptr;
        object.free_list_size[idx] = object.cached_free_list_size[idx] - 1;
        object.cached_free_list_size[idx] = 0;
        return result;
    }

    return object.__refill(idx);
}

inline void __forceinline_reuse_heap(object &object, char *mem,
                                     unsigned int bytes_left) noexcept {
    for (unsigned int idx = bin_table_size - 1;; --idx) {
        WJR_ASSERT(~idx);
        const unsigned int __size = bin_table[idx];
        if (bytes_left >= __size) {
            __forceinline_deallocate_impl(object, mem, idx);

            mem += __size;
            bytes_left -= __size;

            if (bytes_left == 0) {
                break;
            }
        }
    }
}

inline void __forceinline_deallocate_impl(object &object, void *mem,
                                          unsigned int idx) noexcept {
    auto *const mem_obj = static_cast<obj *>(mem);
    const auto size = object.free_list_size[idx];

    if (WJR_LIKELY(size != max_bin_cached_table[idx])) {
        object.free_list_size[idx] = size + 1;
        mem_obj->link.next = get_obj_node(object.free_list[idx]);
        object.free_list[idx] = mem_obj;
        return;
    }

    if (WJR_LIKELY(object.cached_free_list_size[idx] != size)) {
        ++object.cached_free_list_size[idx];
        mem_obj->link.next = get_obj_node(object.cached_free_list[idx]);
        object.cached_free_list[idx] = mem_obj;
        return;
    }

    object.__global_recycle(size, idx);
}

} // namespace

void *thread_cache_object::__allocate_impl(unsigned int idx) {
    return __forceinline_allocate_impl(*this, idx);
}

void thread_cache_object::__deallocate_impl(void *mem, unsigned int idx) noexcept {
    // empty -> non empty, need to guard.
    if (WJR_UNLIKELY(need_guard)) {
        need_guard = false;
        return __guard_deallocate(mem, idx);
    }

    __forceinline_deallocate_impl(*this, mem, idx);
}

void *thread_cache_object::__refill(unsigned int idx) {
    // empty -> non empty, need to guard.
    if (WJR_UNLIKELY(need_guard)) {
        need_guard = false;
        thread_cache_recycler::guard(this);
    }

    ++slow_start_stamp;

    if (obj_linker *const linker = global_memory_pool_object.bin_allocate(idx);
        linker != nullptr) {
        WJR_ASSERT(linker->size >= max_bin_cached_threshold_table[idx]);

        obj *const result = linker->head;
        free_list[idx] = get_obj(result->link.next);
        free_list_size[idx] = linker->size - 1;

        global_memory_pool_object.linker_deallocate(linker);
        return result;
    }

    auto nobjs = static_cast<unsigned int>(bin_slow_start_table[idx]);
    const unsigned int bin_size = bin_table[idx];
    void *chunk;

    const size_t total_bytes = bin_size * nobjs;

    do {
        auto bytes_left = static_cast<size_t>(end_free - start_free);

        if (bytes_left >= total_bytes) {
            chunk = start_free;
            start_free += total_bytes;
            break;
        }

        if (bytes_left >= bin_size) {
            nobjs = bin_fast_divide(bytes_left, idx);
            WJR_ASSERT(nobjs != 0);

            chunk = start_free;
            start_free += nobjs * bin_table[idx];
            break;
        }

        // Try to make use of the left-over piece.
        // This is very unlikely.
        if (bytes_left != 0) {
            WJR_ASSERT(!(bytes_left & 7));
            __forceinline_reuse_heap(*this, start_free,
                                     static_cast<unsigned int>(bytes_left));
        }

        WJR_ASSERT(total_bytes <= 512_KB);

        unsigned int bytes_to_get;
        if (heap_size <= 1_MB) {
            bytes_to_get = __align_up(2 * total_bytes + (heap_size >> 2), 16384);

            start_free = new char[bytes_to_get];
            end_free = start_free + bytes_to_get;
            heap_size += bytes_to_get;

            chunk = start_free;
            start_free += total_bytes;
            break;
        }

        bytes_to_get = __align_up(2 * total_bytes, 16384);
        start_free =
            static_cast<char *>(global_memory_pool_object.heap_allocate(bytes_to_get));
        end_free = start_free + bytes_to_get;
        chunk = start_free;

        WJR_ASSERT(bytes_to_get >= bin_size);

        if (bytes_to_get >= total_bytes) {
            start_free += total_bytes;
            break;
        }

        WJR_ASSERT(bytes_to_get >= bin_size);

        nobjs = bin_fast_divide(bytes_to_get, idx);
        WJR_ASSERT(nobjs != 0);

        start_free += nobjs * bin_table[idx];
    } while (0);

    free_list_size[idx] = nobjs - 1;

    if (nobjs == 1) {
        return chunk;
    }

    char *current_obj;

    // Build free list in chunk
    auto *const result = static_cast<obj *>(chunk);

    current_obj = static_cast<char *>(chunk) + bin_size;
    free_list[idx] = reinterpret_cast<obj *>(current_obj);
    nobjs -= 2;

    if (WJR_LIKELY(nobjs >= 4)) {
        auto mod4n = nobjs & 3;
        if (mod4n) {
            do {
                auto *const next_obj = current_obj + bin_size;
                reinterpret_cast<obj *>(current_obj)->link.next =
                    get_obj_node(reinterpret_cast<obj *>(next_obj));
                current_obj += bin_size;
            } while (--mod4n);
        }

        nobjs = __align_down(nobjs, 4);

        do {
            auto *const obj0 = current_obj + bin_size;
            auto *const obj1 = current_obj + bin_size * 2;
            auto *const obj2 = current_obj + bin_size * 3;
            auto *const obj3 = current_obj + bin_size * 4;

            reinterpret_cast<obj *>(current_obj)->link.next =
                get_obj_node(reinterpret_cast<obj *>(obj0));

            reinterpret_cast<obj *>(obj0)->link.next =
                get_obj_node(reinterpret_cast<obj *>(obj1));

            reinterpret_cast<obj *>(obj1)->link.next =
                get_obj_node(reinterpret_cast<obj *>(obj2));

            reinterpret_cast<obj *>(obj2)->link.next =
                get_obj_node(reinterpret_cast<obj *>(obj3));

            current_obj = obj3;
            nobjs -= 4;
        } while (nobjs);
    } else {
        if (WJR_LIKELY(nobjs != 0)) {
            do {
                auto *const next_obj = current_obj + bin_size;
                reinterpret_cast<obj *>(current_obj)->link.next =
                    get_obj_node(reinterpret_cast<obj *>(next_obj));
                current_obj = next_obj;
            } while (--nobjs);
        }
    }

    reinterpret_cast<obj *>(current_obj)->link.next = nullptr;
    return result;
}

void thread_cache_object::__global_recycle(unsigned int cached_size,
                                           unsigned int idx) noexcept {
    obj *const head = cached_free_list[idx];
    cached_free_list[idx] = nullptr;
    cached_free_list_size[idx] = 0;

    obj_linker *const linker = global_memory_pool_object.linker_allocate(this);
    linker->head = head;
    linker->size = cached_size;
    global_memory_pool_object.bin_deallocate(linker, idx);
}

/**
 * @brief All free lists and heap are empty,
 *
 */
void thread_cache_object::__guard_deallocate(void *mem, unsigned int idx) noexcept {
    thread_cache_recycler::guard(this);

    auto *const mem_obj = static_cast<obj *>(mem);
    free_list_size[idx] = 1;
    mem_obj->link.next = nullptr;
    free_list[idx] = mem_obj;
}

} // namespace memory

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
    return chunk_alloc(idx, nobjs);
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