#ifndef WJR_CONCURRENCY_SPIN_MUTEX_HPP__
#define WJR_CONCURRENCY_SPIN_MUTEX_HPP__

#include <thread>

#include <wjr/assert.hpp>
#include <wjr/concurrency/atomic.hpp>
#include <wjr/concurrency/pause.hpp>

namespace wjr {

/**
 * @brief This may not be necessary as it does not prohibit system interrupts. When a
 * thread switch occurs during a lock, it may cause other threads to spin inefficiently.
 *
 */
class spin_mutex {
    static constexpr unsigned int yield_threshold = 16;

public:
    spin_mutex() = default;
    spin_mutex(const spin_mutex &) = delete;
    spin_mutex &operator=(const spin_mutex &) = delete;

    WJR_INTRINSIC_INLINE bool try_lock() {
        return !m_flag.test_and_set(memory_order_acquire);
    }

    void lock() {
        unsigned int pause_cnt = 0;
        do {
            if (WJR_LIKELY(try_lock())) {
                return;
            }

            if (WJR_LIKELY(try_lock())) {
                return;
            }

            ++pause_cnt;
            if (WJR_UNLIKELY(!(pause_cnt & (yield_threshold - 1)))) {
                std::this_thread::yield();
            } else {
                pause_intrinsic();
            }
        } while (true);
    }

    void unlock() { m_flag.clear(memory_order_release); }

#if WJR_DEBUG_LEVEL > 2
    ~spin_mutex() {
        WJR_ASSERT_L0(try_lock());
        unlock();
    }
#endif

private:
    atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

} // namespace wjr

#endif // WJR_CONCURRENCY_SPIN_MUTEX_HPP__