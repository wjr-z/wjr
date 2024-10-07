#include <wjr/concurrency/spin_mutex.hpp>

namespace wjr {

bool spin_mutex::try_lock() noexcept {
    int val;

#if WJR_ATOMIC_PREFER_EXCHANGE_THAN_CAS
    if (WJR_LIKELY(!m_flag.exchange(1, memory_order_acquire))) {
        return true;
    }
#else
    if (WJR_LIKELY(m_flag.compare_exchange_weak(val, 1, memory_order_acquire))) {
        return true;
    }
#endif

    if (m_flag.load(memory_order_relaxed) == 0) {
        do {
            if (WJR_LIKELY(m_flag.compare_exchange_weak(val, 1, memory_order_acquire))) {
                return true;
            }
        } while (m_flag.load(memory_order_relaxed) == 0);
    }

    return false;
}

void spin_mutex::lock() noexcept {
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

} // namespace wjr