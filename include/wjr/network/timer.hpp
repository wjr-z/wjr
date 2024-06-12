#ifndef WJR_NETWORK_TIMER_HPP__
#define WJR_NETWORK_TIMER_HPP__

/**
 * @file timer.hpp
 * @author wjr
 *
 * @version 0.1
 * @date 2024-06-05
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <chrono>

#include <wjr/container/intrusive/list.hpp>
#include <wjr/math/clz.hpp>
#include <wjr/memory/memory_pool.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr {

class timer {
public:
    using node_type = list_node<>;

protected:
    struct timing_wheel {
        node_type m_buckets[16];
    };

    struct wheel_offset {
        unsigned int m_wheel;
        unsigned int m_bucket;
    };

public:
    struct task_type : node_type {
        node_type *get_node() noexcept { return this; }

        template <typename Func>
        task_type(Func &&func, uint32_t offset)
            : m_offset(offset), m_func(std::forward<Func>(func)) {
            init(get_node());
        }

        ~task_type() noexcept = default;

        void operator()() noexcept { m_func(); }

        uint32_t m_offset;
        std::function<void()> m_func;
    };

    using iterator = typename node_type::iterator;
    using const_iterator = typename node_type::const_iterator;

    timer(uint32_t current) noexcept : m_current(current) {
        for (auto &wheel : m_wheels) {
            for (auto &bucket : wheel.m_buckets) {
                init(&bucket);
            }
        }
    }

    timer(const timer &) = delete;
    timer(timer &&) = delete;
    timer &operator=(const timer &) = delete;
    timer &operator=(timer &&) = delete;

    ~timer() noexcept { clear(); }

    void clear() noexcept {
        memory_pool<task_type> al;
        for (auto &wheel : m_wheels) {
            for (auto &bucket : wheel.m_buckets) {
                for (auto &task : bucket) {
                    const auto ptr = static_cast<task_type *>(&task);
                    destroy_at_using_allocator(ptr, al);
                    al.deallocate(ptr, 1);
                }

                init(&bucket);
            }
        }
    }

    template <typename Func, WJR_REQUIRES(std::is_invocable_v<Func &&>)>
    iterator add(Func &&func, uint32_t offset) noexcept {
        const uint32_t real = offset + m_current;

        memory_pool<task_type> al;
        task_type *const ptr = al.allocate(1);
        uninitialized_construct_using_allocator(ptr, al, std::forward<Func>(func), real);

        WJR_ASSERT(offset != 0, "undefined behavior");

        auto [wheel, bucket] = __get_offset(offset, real);
        push_back(&m_wheels[wheel].m_buckets[bucket], ptr);

        return iterator{ptr->get_node()};
    }

    void remove(iterator it) noexcept {
        node_type *const node = &*it;
        remove_uninit(node);
        const auto ptr = static_cast<task_type *>(node);
        memory_pool<task_type> al;
        destroy_at_using_allocator(ptr, al);
        al.deallocate(ptr, 1);
    }

    node_type &get_head() noexcept { return m_wheels[0].m_buckets[m_current & 0xf]; }
    const node_type &get_head() const noexcept {
        return m_wheels[0].m_buckets[m_current & 0xf];
    }

    template <typename Func, WJR_REQUIRES(std::is_invocable_v<Func &&, task_type &>)>
    void run(Func &&func) noexcept {
        memory_pool<task_type> al;

        auto &bucket = m_wheels[0].m_buckets[m_current & 0xf];
        for (auto node = bucket.begin(); node != bucket.end();) {
            node_type &task = *node;
            auto next = wjr::next(&task);
            const auto ptr = static_cast<task_type *>(&task);

            func(*ptr);

            destroy_at_using_allocator(ptr, al);
            al.deallocate(ptr, 1);

            node = next;
        }

        init(&bucket);
    }

    void run() noexcept {
        run([](task_type &task) { task(); });
    }

    void next() noexcept {
        ++m_current;
        uint32_t tmp = m_current;

        for (int i = 1; i < 8 && (tmp & 0xf) == 0; ++i) {
            tmp >>= 4;
            const uint32_t bucket = tmp & 0xf;

            auto &now_bucket = m_wheels[i].m_buckets[bucket];

            for (auto node = now_bucket.begin(); node != now_bucket.end();) {
                node_type &task = *node;
                auto next = wjr::next(&task);

                const auto ptr = static_cast<task_type *>(&task);
                const uint32_t offset = ptr->m_offset;
                const uint32_t real = offset + m_current;

                auto [wheel, new_bucket] = __get_offset(offset, real);
                push_back(&m_wheels[wheel].m_buckets[new_bucket], ptr);

                node = next;
            }

            init(&m_wheels[i].m_buckets[bucket]);
        }
    }

protected:
    WJR_INTRINSIC_INLINE WJR_CONST static wheel_offset
    __get_offset(uint32_t offset, uint32_t real) noexcept {
        if (offset < 0x10) {
            return {0, real & 0x0f};
        }

        const unsigned int wheel = (31 - clz<uint32_t>(offset)) / 4;
        return {wheel, (real >> (wheel * 4)) & 0x0f};
    }

    uint32_t m_current = 0;
    timing_wheel m_wheels[8];
};

} // namespace wjr

#endif // WJR_NETWORK_TIMER_HPP__