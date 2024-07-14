/**
 * @file timer.hpp
 * @author wjr
 * @brief only preview
 * @version 0.1
 * @date 2024-07-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WJR_NETWORK_TIMER_HPP__
#define WJR_NETWORK_TIMER_HPP__

/**
 * @file timing_wheels.hpp
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

class timing_wheels {
public:
    using node_type = list_node<>;

protected:
    struct wheel {
        node_type m_buckets[256];
    };

    struct wheel_offset {
        unsigned int m_wheel;
        unsigned int m_bucket;
    };

public:
    struct task_type : node_type {
        node_type *get_node() noexcept { return this; }

        template <typename Func>
        task_type(Func &&func, uint32_t offset) noexcept
            : m_offset(offset), m_func(std::forward<Func>(func)) {
            init(get_node());
        }

        ~task_type() = default;

        void operator()() noexcept { m_func(); }

        uint32_t m_offset;
        std::function<void()> m_func;
    };

    using iterator = typename node_type::iterator;
    using const_iterator = typename node_type::const_iterator;

    timing_wheels(uint32_t current) noexcept : m_current(current) {
        for (auto &wheel : m_wheels) {
            for (auto &bucket : wheel.m_buckets) {
                init(&bucket);
            }
        }
    }

    timing_wheels(const timing_wheels &) = delete;
    timing_wheels(timing_wheels &&) = delete;
    timing_wheels &operator=(const timing_wheels &) = delete;
    timing_wheels &operator=(timing_wheels &&) = delete;

    ~timing_wheels() noexcept
#if WJR_DEBUG_LEVEL > 1
    {
        for (auto &wheel : m_wheels) {
            for (auto &bucket : wheel.m_buckets) {
                WJR_ASSERT_L0(bucket.empty(), "undefined behavior");
            }
        }
    }
#else
        = default;
#endif

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

    node_type &get_head() noexcept { return m_wheels[0].m_buckets[m_current & 0xff]; }
    const node_type &get_head() const noexcept {
        return m_wheels[0].m_buckets[m_current & 0xff];
    }

    void next() noexcept {
        ++m_current;
        uint32_t tmp = m_current;

        for (int i = 1; i < 4 && (tmp & 0xff) == 0; ++i) {
            tmp >>= 8;
            const uint32_t bucket = tmp & 0xff;

            auto &now_bucket = m_wheels[i].m_buckets[bucket];

            if (auto node = now_bucket.begin(), end = now_bucket.end(); node != end) {
                do {
                    node_type &task = *node;
                    const auto next = wjr::next(&task);

                    const auto ptr = static_cast<task_type *>(&task);
                    const uint32_t offset = ptr->m_offset;
                    const uint32_t real = offset + m_current;

                    auto [wheel, new_bucket] = __get_offset(offset, real);
                    push_back(&m_wheels[wheel].m_buckets[new_bucket], ptr);

                    node = next;
                } while (node != end);

                init(&m_wheels[i].m_buckets[bucket]);
            }
        }
    }

protected:
    WJR_INTRINSIC_INLINE WJR_CONST static wheel_offset
    __get_offset(uint32_t offset, uint32_t real) noexcept {
        if (offset < 0x100) {
            return {0, real & 0xff};
        }

        if (offset < 0x10'000) {
            return {1, real & 0xffff};
        }

        if (offset < 0x1'000'000) {
            return {2, real & 0xffffff};
        }

        return {3, real};
    }

    uint32_t m_current = 0;
    alignas(16) wheel m_wheels[4];
};

} // namespace wjr

#endif // WJR_NETWORK_TIMER_HPP__