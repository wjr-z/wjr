#include "simdjson.hpp"

#include <atomic>
#include <chrono>
#include <format>

// #include <csignal>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <semaphore>
#include <set>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>

#include <wjr/concurrency/spin_mutex.hpp>
#if !(defined(WJR_CXX_20))
    #include <boost/atomic.hpp>
#endif

#include <condition_variable>

#include <charconv>
#include <queue>
#include <wjr/biginteger.hpp>
#include <wjr/concurrency/pause.hpp>
#include <wjr/expected.hpp>
#include <wjr/format/dragonbox.hpp>
#include <wjr/format/fastfloat.hpp>
#include <wjr/json/document.hpp>
#include <wjr/json/formatter.hpp>
#include <wjr/json/visitor.hpp>
#include <wjr/simd/simd.hpp>

#include <wjr/algorithm.hpp>

#include <bitset>

auto get() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

using namespace wjr;

template <typename Func>
auto test(Func fn) {
    auto s = get();
    fn();
    auto x = get() - s;
    std::cout << x << std::endl;
    return x;
}

struct node {
    size_t idx;
    int st;
    bool release;
};

namespace wjr::json {

class virtual_json_parser;

class virtual_json_visitor {
public:
    template <typename T>
    virtual_json_visitor(T *ptr) noexcept : ptr((void *)ptr) {}

    virtual ~virtual_json_visitor() noexcept = default;

    virtual result<void> visit_root_null(const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_object_null(const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_array_null(const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_root_true(const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_object_true(const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_array_true(const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_root_false(const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_object_false(const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_array_false(const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_root_number(const char *, const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_object_number(const char *, const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_array_number(const char *, const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_root_string(const char *, const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_object_string(const char *, const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_array_string(const char *, const char *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_object_key_string(const char *, const char *,
                                                 virtual_json_parser *) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_root_start_object(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_object_start_object(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_array_start_object(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_root_start_array(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_object_start_array(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_array_start_array(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_end_object_to_object(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_end_object_to_array(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_end_object_to_root(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_end_array_to_object(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_end_array_to_array(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    virtual result<void> visit_end_array_to_root(uint32_t) noexcept {
        return unexpected(error_code::TAPE_ERROR);
    }

    void *get() noexcept { return ptr; }

private:
    void *ptr;
};

template <typename T>
class parser;

class virtual_json_parser {
    template <typename Parser>
    friend result<void> visitor_detail::parse(Parser &&par, const reader &rd) noexcept;

public:
    template <typename T>
    WJR_INTRINSIC_INLINE void push(T *x) noexcept {
        static_assert(sizeof(parser<T>) <= 16, "");
        wjr::construct_at((parser<T> *)ptr, x);
        ptr += 16;
    }

    void pop() noexcept {
        std::destroy_at(get());
        ptr -= 16;
    }

    template <typename T>
    WJR_INTRINSIC_INLINE result<T> parse(const reader &rd) noexcept {
        T ret;
        push(&ret);
        WJR_EXPECTED_TRY(visitor_detail::parse(*this, rd));
        return ret;
    }

protected:
    WJR_INTRINSIC_INLINE result<void> visit_root_null(const char *first) noexcept {
        return get()->visit_root_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_null(const char *first) noexcept {
        WJR_EXPECTED_TRY(get()->visit_object_null(first));
        pop();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_null(const char *first) noexcept {
        return get()->visit_array_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_true(const char *first) noexcept {
        return get()->visit_root_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_true(const char *first) noexcept {
        WJR_EXPECTED_TRY(get()->visit_object_true(first));
        pop();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_true(const char *first) noexcept {
        return get()->visit_array_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_false(const char *first) noexcept {
        return get()->visit_root_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_false(const char *first) noexcept {
        WJR_EXPECTED_TRY(get()->visit_object_false(first));
        pop();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_false(const char *first) noexcept {
        return get()->visit_array_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_number(const char *first,
                                                        const char *last) noexcept {
        return get()->visit_root_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_number(const char *first,
                                                          const char *last) noexcept {
        WJR_EXPECTED_TRY(get()->visit_object_number(first, last));
        pop();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_number(const char *first,
                                                         const char *last) noexcept {
        return get()->visit_array_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_string(const char *first,
                                                        const char *last) noexcept {
        return get()->visit_root_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_string(const char *first,
                                                          const char *last) noexcept {
        WJR_EXPECTED_TRY(get()->visit_object_string(first, last));
        pop();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_string(const char *first,
                                                         const char *last) noexcept {
        return get()->visit_array_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_key_string(const char *first,
                                                              const char *last) noexcept {
        return get()->visit_object_key_string(first, last, this);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_object(uint32_t pos) noexcept {
        return get()->visit_root_start_object(pos);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_object(uint32_t pos) noexcept {
        return get()->visit_object_start_object(pos);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_object(uint32_t pos) noexcept {
        return get()->visit_array_start_object(pos);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_array(uint32_t pos) noexcept {
        return get()->visit_root_start_array(pos);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_array(uint32_t pos) noexcept {
        return get()->visit_object_start_array(pos);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_array(uint32_t pos) noexcept {
        return get()->visit_array_start_array(pos);
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_object(uint32_t pos) noexcept {
        WJR_EXPECTED_TRY(get()->visit_end_object_to_object(pos));
        std::destroy_at(get());
        ptr -= 16;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_array(uint32_t pos) noexcept {
        WJR_EXPECTED_TRY(get()->visit_end_object_to_array(pos));
        std::destroy_at(get());
        ptr -= 16;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_root(uint32_t pos) noexcept {
        WJR_EXPECTED_TRY(get()->visit_end_object_to_root(pos));
        std::destroy_at(get());
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_object(uint32_t pos) noexcept {
        WJR_EXPECTED_TRY(get()->visit_end_array_to_object(pos));
        std::destroy_at(get());
        ptr -= 16;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_array(uint32_t pos) noexcept {
        WJR_EXPECTED_TRY(get()->visit_end_array_to_array(pos));
        std::destroy_at(get());
        ptr -= 16;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_root(uint32_t pos) noexcept {
        WJR_EXPECTED_TRY(get()->visit_end_array_to_root(pos));
        std::destroy_at(get());
        return {};
    }

private:
    virtual_json_visitor *get() noexcept {
        return reinterpret_cast<virtual_json_visitor *>(ptr - 16);
    }

    char buffer[256 * 8];
    char *ptr = buffer;
};

} // namespace wjr::json

struct pt {
    std::string name;
    int age;
    int sex;
    int version;
    int zc;

    pt() = default;
};

namespace wjr::json {

template <>
class parser<int> : public virtual_json_visitor {
    using Mybase = virtual_json_visitor;

    int *get() { return (int *)Mybase::get(); }

public:
    using Mybase::Mybase;

    virtual result<void> visit_root_number(const char *first, const char *last) noexcept {
        WJR_EXPECTED_INIT(ret, detail::parse_number(first, last));
        *get() = ret->m_number_unsigned;
        return {};
    }

    virtual result<void> visit_object_number(const char *first,
                                             const char *last) noexcept {
        WJR_EXPECTED_INIT(ret, detail::parse_number(first, last));
        *get() = ret->m_number_unsigned;
        return {};
    }

    virtual result<void> visit_end_object_to_object(uint32_t) noexcept { return {}; }
    virtual result<void> visit_end_object_to_array(uint32_t) noexcept { return {}; }
    virtual result<void> visit_end_object_to_root(uint32_t) noexcept { return {}; }
};

template <>
class parser<std::string> : public virtual_json_visitor {
    using Mybase = virtual_json_visitor;

    std::string *get() { return (std::string *)Mybase::get(); }

public:
    using Mybase::Mybase;

    virtual result<void> visit_root_string(const char *first, const char *last) noexcept {
        auto *str = get();
        try_uninitialized_resize(*str, last - first);

        auto ret = detail::parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            return unexpected(std::move(ret).error());
        }

        str->resize(*ret - str->data());
        return {};
    }

    virtual result<void> visit_object_string(const char *first,
                                             const char *last) noexcept {
        auto *str = get();
        try_uninitialized_resize(*str, last - first);

        auto ret = detail::parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            return unexpected(std::move(ret).error());
        }

        str->resize(*ret - str->data());
        return {};
    }

    virtual result<void> visit_end_object_to_object(uint32_t) noexcept { return {}; }
    virtual result<void> visit_end_object_to_array(uint32_t) noexcept { return {}; }
    virtual result<void> visit_end_object_to_root(uint32_t) noexcept { return {}; }
};

template <>
class parser<pt> : public virtual_json_visitor {
    using Mybase = virtual_json_visitor;

    pt *get() { return (pt *)Mybase::get(); }

public:
    using Mybase::Mybase;

    virtual result<void> visit_object_key_string(const char *first, const char *last,
                                                 virtual_json_parser *par) noexcept {
        std::string_view view(first, last - first);
        if (view == "name") {
            par->push(&get()->name);
        } else if (view == "age") {
            par->push(&get()->age);
        } else if (view == "version") {
            par->push(&get()->version);
        } else if (view == "zc") {
            par->push(&get()->zc);
        } else if (view == "sex") {
            par->push(&get()->sex);
        } else {
            return unexpected(error_code::TAPE_ERROR);
        }

        return {};
    }

    virtual result<void> visit_root_start_object(uint32_t) noexcept { return {}; }

    virtual result<void> visit_end_object_to_object(uint32_t) noexcept { return {}; }
    virtual result<void> visit_end_object_to_array(uint32_t) noexcept { return {}; }
    virtual result<void> visit_end_object_to_root(uint32_t) noexcept { return {}; }
};

} // namespace wjr::json

static constexpr unsigned int yield_threshold1 = 4;
static constexpr unsigned int yield_threshold2 = 4;

struct mutex1 {

    WJR_INTRINSIC_INLINE bool try_lock() {
        return !m_flag.test_and_set(memory_order_acquire);
    }

    void lock() {
        unsigned int pause_cnt = 0;
        do {
            if (WJR_LIKELY(try_lock())) {
                return;
            }

            ++pause_cnt;
            if (WJR_UNLIKELY(!(pause_cnt & (yield_threshold1 - 1)))) {
                std::this_thread::yield();
            } else {
                pause_intrinsic();
            }
        } while (true);
    }

    void unlock() { m_flag.clear(memory_order_release); }

    wjr::atomic_flag m_flag;
};

struct mutex2 {

    WJR_INTRINSIC_INLINE bool try_lock() {
        return !m_flag.test_and_set(memory_order_acquire);
    }

    void lock() {
        unsigned int pause_cnt = 0;
        do {
            if (WJR_LIKELY(try_lock())) {
                return;
            }

            ++pause_cnt;
            if (WJR_UNLIKELY(!(pause_cnt & (yield_threshold2 - 1)))) {
                m_wait.fetch_add(1, memory_order_release);
                m_flag.wait(true, memory_order_acquire);
                m_wait.fetch_sub(1, memory_order_relaxed);
            } else {
                pause_intrinsic();
            }
        } while (true);
    }

    void unlock() {
        m_flag.clear(memory_order_release);
        if (m_wait.load(memory_order_acquire) != 0) {
            m_flag.notify_one();
        }
    }

    atomic_flag m_flag;
    atomic<unsigned int> m_wait;
};

int main() {

    {
        std::formatter<int>;
        std::format("{}", 3);
        return 0;
    }

    {
        const int N = 1e7;
        const int M = 4;

        test([&]() {
            mutex1 p;
            for (int i = 0; i < N; ++i) {
                p.lock();
                p.unlock();
            }
        });

        test([&]() {
            mutex2 p;
            for (int i = 0; i < N; ++i) {
                p.lock();
                p.unlock();
            }
        });

        test([&]() {
            spin_mutex p;
            vector<std::thread> vec;
            for (int t = 0; t < M; ++t)
                vec.emplace_back([&]() {
                    for (int i = 0; i < N; ++i) {
                        p.lock();
                        for (int j = 0; j < 8; ++j)
                            asm volatile("nop");
                        p.unlock();
                        for (int j = 0; j < 8; ++j)
                            asm volatile("nop");
                    }
                });

            for (auto &t : vec)
                t.join();
        });

        // test([&]() {
        //     mutex2 p;
        //     vector<std::thread> vec;
        //     for (int t = 0; t < M; ++t)
        //         vec.emplace_back([&]() {
        //             for (int i = 0; i < N; ++i) {
        //                 p.lock();
        //                 for (int j = 0; j < 4; ++j)
        //                     asm volatile("nop");
        //                 p.unlock();
        //             }
        //         });

        //     for (auto &t : vec)
        //         t.join();
        // });

        test([&]() {
            std::mutex p;
            vector<std::thread> vec;
            for (int t = 0; t < M; ++t)
                vec.emplace_back([&]() {
                    for (int i = 0; i < N; ++i) {
                        p.lock();
                        for (int j = 0; j < 8; ++j)
                            asm volatile("nop");
                        p.unlock();
                        for (int j = 0; j < 8; ++j)
                            asm volatile("nop");
                    }
                });

            for (auto &t : vec)
                t.join();
        });

        test([&]() {
            // std::mutex p;
            std::binary_semaphore p(1);
            vector<std::thread> vec;
            for (int t = 0; t < M; ++t)
                vec.emplace_back([&]() {
                    for (int i = 0; i < N; ++i) {
                        // p.lock();
                        p.acquire();
                        for (int j = 0; j < 8; ++j)
                            asm volatile("nop");
                        // p.unlock();
                        p.release();
                        for (int j = 0; j < 8; ++j)
                            asm volatile("nop");
                    }
                });

            for (auto &t : vec)
                t.join();
        });

        return 0;
    }

    // {
    //     const int N = 1e6;
    //     constexpr unsigned arena_size = 2_MB;
    //     constexpr unsigned chunk_size = 64_KB;

    //     char *arena = new char[arena_size];

    //     test([&]() {
    //         auto ptr = reinterpret_cast<heap_linker *>(arena);
    //         for (int i = 0; i < N; ++i) {
    //             asm volatile("" : "+r"(ptr));
    //             fast_link(ptr, arena_size / chunk_size, chunk_size, link_heap);
    //         }
    //     });

    //     return 0;
}