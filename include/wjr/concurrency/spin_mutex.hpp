#ifndef WJR_CONCURRENCY_SPIN_MUTEX_HPP__
#define WJR_CONCURRENCY_SPIN_MUTEX_HPP__

#include <thread>

#include <wjr/assert.hpp>
#include <wjr/concurrency/atomic.hpp>
#include <wjr/concurrency/mutex-config.hpp>
#include <wjr/concurrency/pause.hpp>

namespace wjr {

/**
 * @class spin_mutex
 * @brief Maybe useless because futex spin before syscall.
 *
 */
class spin_mutex {
    static constexpr unsigned int yield_threshold = 8;

public:
    /// @brief Just likey atomic_init, initialize can be relaxed and not atomic.
    spin_mutex() noexcept : m_flag(0) {}
    spin_mutex(const spin_mutex &) = delete;
    spin_mutex &operator=(const spin_mutex &) = delete;

#if WJR_DEBUG_LEVEL > 2
    ~spin_mutex() { WJR_ASSERT_L0(!m_flag.load(memory_order_acquire)); }
#endif

    WJR_INTRINSIC_INLINE bool try_lock() noexcept {
        int val;

#if WJR_ATOMIC_PREFER_EXCHANGE_THAN_CAS
        if (WJR_LIKELY(!m_flag.exchange(1, memory_order_acquire))) {
            return true;
        }
#endif

        do {
            if (WJR_LIKELY(m_flag.compare_exchange_weak(val, 1, memory_order_acquire))) {
                return true;
            }
        } while (m_flag.load(memory_order_relaxed) == 0);

        return false;
    }

    WJR_NOINLINE void lock() noexcept {
        int val;

#if WJR_ATOMIC_PREFER_EXCHANGE_THAN_CAS
        if (WJR_LIKELY(!m_flag.exchange(1, memory_order_acquire))) {
            return;
        }
#else
        if (WJR_LIKELY(m_flag.compare_exchange_weak(val, 1, memory_order_acquire))) {
            return;
        }
#endif

        do {
            do {
                pause_intrinsic();
                val = m_flag.load(memory_order_relaxed);
            } while (val != 0);
        } while (!m_flag.compare_exchange_weak(val, 1, memory_order_acquire));
    }

    void unlock() noexcept { m_flag.store(0, memory_order_release); }

private:
    atomic<int> m_flag;
};

} // namespace wjr

#endif // WJR_CONCURRENCY_SPIN_MUTEX_HPP__