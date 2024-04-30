#ifndef WJR_MEMORY_MEMORY_POOL_HPP__
#define WJR_MEMORY_MEMORY_POOL_HPP__

#include <wjr/crtp/trivially_allocator_base.hpp>
#include <wjr/memory/details.hpp>

namespace wjr {

template <int __inst>
class __malloc_alloc_template__ {
private:
    WJR_NOINLINE static void *_S_oom_malloc(size_t);
    static void *_S_oom_realloc(void *, size_t);

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
    static void (*__malloc_alloc_oom_handler)();
#endif

public:
    WJR_INTRINSIC_INLINE static void *allocate(size_t __n) {
        void *__result = malloc(__n);
        if (WJR_LIKELY(0 != __result))
            return __result;
        return _S_oom_malloc(__n);
    }

    static void deallocate(void *__p, size_t /* __n */) { free(__p); }

    static void *reallocate(void *__p, size_t /* old_sz */, size_t __new_sz) {
        void *__result = realloc(__p, __new_sz);
        if (WJR_LIKELY(0 != __result))
            return __result;
        return _S_oom_realloc(__p, __new_sz);
    }

    static void (*__set_malloc_handler(void (*__f)()))() {
        void (*__old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = __f;
        return (__old);
    }
};

// malloc_alloc out-of-memory handling

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
template <int __inst>
void (*__malloc_alloc_template__<__inst>::__malloc_alloc_oom_handler)() = 0;
#endif

template <int __inst>
void *__malloc_alloc_template__<__inst>::_S_oom_malloc(size_t __n) {
    void (*__my_malloc_handler)();
    void *__result;

    for (;;) {
        __my_malloc_handler = __malloc_alloc_oom_handler;
        (*__my_malloc_handler)();
        __result = malloc(__n);
        if (__result)
            return (__result);
    }
}

template <int __inst>
void *__malloc_alloc_template__<__inst>::_S_oom_realloc(void *__p, size_t __n) {
    void (*__my_malloc_handler)();
    void *__result;

    for (;;) {
        __my_malloc_handler = __malloc_alloc_oom_handler;

        (*__my_malloc_handler)();
        __result = realloc(__p, __n);
        if (__result)
            return (__result);
    }
}

template <int inst>
class __default_alloc_template__ {
private:
    using allocator_type = __malloc_alloc_template__<0>;

    enum { ALLOC_ALIGN = 8 };
    enum { ALLOC_MAX_BYTES = 256 };
    enum { ALLOC_NFRELISTS = ALLOC_MAX_BYTES / ALLOC_ALIGN };

    enum { __ALIGN = ALLOC_ALIGN };
    enum { __MAX_BYTES = ALLOC_MAX_BYTES };
    enum { __NFREELISTS = ALLOC_NFRELISTS };

    union obj {
        union obj *free_list_link;
        char client_data[1];
    };

    struct object {
        obj *volatile free_list[__NFREELISTS] = {nullptr};
        char *start_free = nullptr;
        char *end_free = nullptr;
        size_t heap_size = 0;
    };

    static object &get_instance() noexcept {
        static thread_local object instance;
        return instance;
    }

    static obj *volatile *get_free_list() noexcept { return get_instance().free_list; }
    static char *&get_start_free() noexcept { return get_instance().start_free; }
    static char *&get_end_free() noexcept { return get_instance().end_free; }
    static size_t &get_heap_size() noexcept { return get_instance().heap_size; }

    static inline size_t ROUND_UP(size_t bytes) {
        return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
    }

    static inline size_t FREELIST_INDEX(size_t bytes) {
        return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
    }

    // Returns an object of size n, and optionally adds to size n free list.
    WJR_NOINLINE static void *refill(size_t n) noexcept;

    // Allocates a chunk for nobjs of size "size".  nobjs may be reduced
    // if it is inconvenient to allocate the requested number.
    static char *chunk_alloc(size_t size, int &nobjs) noexcept;

public:
    inline static void *allocate(size_t n) noexcept // n must be > 0
    {
        if (n > (size_t)__MAX_BYTES) {
            return allocator_type::allocate(n);
        }
        obj *volatile *my_free_list = get_free_list() + FREELIST_INDEX(n);
        obj *result = *my_free_list;
        if (WJR_LIKELY(result != nullptr)) {
            *my_free_list = result->free_list_link;
            return result;
        }
        return refill(ROUND_UP(n));
    }

    inline static void deallocate(void *p, size_t n) noexcept // p may not be 0
    {

        if (n > (size_t)__MAX_BYTES) {
            allocator_type::deallocate(p, n);
            return;
        }

        obj *q = (obj *)p;
        obj *volatile *my_free_list = get_free_list() + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
};

//----------------------------------------------
// We allocate memory in large chunks in order to
// avoid fragmentingthe malloc heap too much.
// We assume that size is properly aligned.
// We hold the allocation lock.
//----------------------------------------------
template <int inst>
char *__default_alloc_template__<inst>::chunk_alloc(size_t size, int &nobjs) noexcept {
    char *result;
    size_t total_bytes = size * nobjs;
    const auto bytes_left = static_cast<size_t>(get_end_free() - get_start_free());

    if (bytes_left >= total_bytes) {
        result = get_start_free();
        get_start_free() += total_bytes;
        return (result);
    }

    if (bytes_left >= size) {
        nobjs = static_cast<int>(bytes_left / size);
        total_bytes = size * nobjs;
        result = get_start_free();
        get_start_free() += total_bytes;
        return (result);
    }

    const size_t bytes_to_get = 2 * total_bytes + ROUND_UP(get_heap_size() >> 4);
    // Try to make use of the left-over piece.
    if (bytes_left > 0) {
        obj *volatile *my_free_list = get_free_list() + FREELIST_INDEX(bytes_left);

        ((obj *)get_start_free())->free_list_link = *my_free_list;
        *my_free_list = (obj *)get_start_free();
    }
    get_start_free() = (char *)malloc(bytes_to_get);
    if (WJR_UNLIKELY(0 == get_start_free())) {
        obj *volatile *my_free_list, *p;

        // Try to make do with what we have. That can't
        // hurt. We do not try smaller requests, since that tends
        // to result in disaster on multi-process machines.
        for (int i = static_cast<int>(size); i <= __MAX_BYTES; i += __ALIGN) {
            my_free_list = get_free_list() + FREELIST_INDEX(i);
            p = *my_free_list;
            if (0 != p) {
                *my_free_list = p->free_list_link;
                get_start_free() = (char *)p;
                get_end_free() = get_start_free() + i;
                return (chunk_alloc(size, nobjs));
                // Any leftover piece will eventually make it to the
                // right free list.
            }
        }
        get_end_free() = 0; // In case of exception.
        get_start_free() = (char *)malloc(bytes_to_get);
        // This should either throw an exception or
        // remedy the situation. Thus we assume it
        // succeeded.
    }
    get_heap_size() += bytes_to_get;
    get_end_free() = get_start_free() + bytes_to_get;
    return (chunk_alloc(size, nobjs));
}

//----------------------------------------------
// Returns an object of size n, and optionally adds
// to size n free list.We assume that n is properly aligned.
// We hold the allocation lock.
//----------------------------------------------
template <int inst>
void *__default_alloc_template__<inst>::refill(size_t n) noexcept {
    int nobjs = 20;
    char *chunk = chunk_alloc(n, nobjs);
    obj *current_obj;
    obj *next_obj;

    if (1 == nobjs)
        return (chunk);
    obj *volatile *my_free_list = get_free_list() + FREELIST_INDEX(n);

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

template <typename Ty>
class memory_pool {
private:
    using allocator_type = __default_alloc_template__<0>;

public:
    using value_type = Ty;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;
    using is_trivially_allocator = std::true_type;

    template <typename Other>
    struct rebind {
        using other = memory_pool<Other>;
    };

    constexpr memory_pool() noexcept = default;
    constexpr memory_pool(const memory_pool &) noexcept = default;
    template <typename Other>
    constexpr memory_pool(const memory_pool<Other> &) noexcept {}
    ~memory_pool() = default;
    memory_pool &operator=(const memory_pool &) noexcept = default;

    WJR_NODISCARD WJR_CONSTEXPR20 WJR_MALLOC Ty *allocate(size_t n) const noexcept {
        if (WJR_UNLIKELY(0 == n)) {
            return nullptr;
        }

        return static_cast<Ty *>(allocator_type::allocate(sizeof(Ty) * n));
    }

    WJR_CONSTEXPR20 void deallocate(Ty *ptr, size_t n) const noexcept {
        if (WJR_UNLIKELY(0 == n)) {
            return;
        }

        return allocator_type::deallocate(static_cast<void *>(ptr), sizeof(Ty) * n);
    }

    constexpr size_t max_size() const { return static_cast<size_t>(-1) / sizeof(Ty); }
};

template <typename T, typename U>
constexpr bool operator==(const memory_pool<T> &, const memory_pool<U> &) {
    return true;
}

template <typename T, typename U>
constexpr bool operator!=(const memory_pool<T> &, const memory_pool<U> &) {
    return false;
}

} // namespace wjr

#endif // WJR_MEMORY_MEMORY_POOL_HPP__