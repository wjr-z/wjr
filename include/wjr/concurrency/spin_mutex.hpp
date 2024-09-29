#ifndef WJR_CONCURRENCY_SPIN_MUTEX_HPP__
#define WJR_CONCURRENCY_SPIN_MUTEX_HPP__

#include <atomic>
#include <thread>

#include <wjr/assert.hpp>
#include <wjr/concurrency/pause.hpp>

namespace wjr {

class spin_mutex {
    static constexpr int fast_lock_threshold = 4;
    static constexpr int pause_per_lock_threshold = 8;
    static constexpr int yield_per_pause_threshold = 12;

public:
    spin_mutex() = default;
    spin_mutex(const spin_mutex &) = delete;
    spin_mutex &operator=(const spin_mutex &) = delete;

    WJR_INTRINSIC_INLINE bool try_lock() {
        return !m_flag.test_and_set(std::memory_order_acquire);
    }

    void lock() {
        if (WJR_UNLIKELY(!try_lock())) {
            for (int i = 0; i < fast_lock_threshold; ++i) {
                if (try_lock()) {
                    return;
                }
            }

            do {
                int yield_cnt = yield_per_pause_threshold;

                do {
                    pause();

                    int puase_cnt = pause_per_lock_threshold;
                    do {
                        if (try_lock()) {
                            return;
                        }
                    } while (--puase_cnt);
                } while (--yield_cnt);

                std::this_thread::yield();
            } while (!try_lock());
        }
    }

    void unlock() { m_flag.clear(std::memory_order_release); }

#if WJR_DEBUG_LEVEL > 2
    ~spin_mutex() {
        WJR_ASSERT_L0(try_lock());
        unlock();
    }
#endif

private:
    std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

} // namespace wjr

#endif // WJR_CONCURRENCY_SPIN_MUTEX_HPP__