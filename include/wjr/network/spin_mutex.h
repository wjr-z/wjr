#pragma once
#ifndef __WJR_NETWORK_SPIN_MUTEX_H
#define __WJR_NETWORK_SPIN_MUTEX_H

#include <atomic>
#include <mutex>

#include <wjr/network/waiter.h>

_WJR_BEGIN

class spin_mutex {
public:
	WJR_INTRINSIC_INLINE spin_mutex() = default;
	spin_mutex(const spin_mutex&) = delete;
	spin_mutex& operator=(const spin_mutex&) = delete;
	WJR_INTRINSIC_INLINE ~spin_mutex() = default;

	template<typename Waiter>
	WJR_INTRINSIC_INLINE void lock(Waiter it) noexcept {
		while (m_flag.test_and_set(std::memory_order_acquire)) {
			it();
		}
	}

	WJR_INTRINSIC_INLINE void lock() noexcept {
		lock(waiter());
	}

	WJR_INTRINSIC_INLINE bool try_lock() noexcept {
		return !m_flag.test_and_set(std::memory_order_acquire);
	}

	WJR_INTRINSIC_INLINE void unlock() noexcept {
		m_flag.clear(std::memory_order_release);
	}

	WJR_INTRINSIC_INLINE std::atomic_flag& flag() { return m_flag; }
	WJR_INTRINSIC_INLINE const std::atomic_flag& flag() const { return m_flag; }
	
private:
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

class shared_spin_mutex {
public:
	WJR_INTRINSIC_INLINE shared_spin_mutex() = default;
	shared_spin_mutex(const shared_spin_mutex&) = delete;
	shared_spin_mutex& operator=(const shared_spin_mutex&) = delete;
	WJR_INTRINSIC_INLINE ~shared_spin_mutex() = default;

	WJR_INTRINSIC_INLINE void lock() noexcept {
		m_write.lock();
	}

	WJR_INTRINSIC_INLINE bool try_lock() noexcept {
		return m_write.try_lock();
	}

	WJR_INTRINSIC_INLINE void unlock() noexcept {
		m_write.unlock();
	}

	WJR_INTRINSIC_INLINE void lock_shared() noexcept {
		m_read.lock();
		if (++m_count == 1)m_write.lock();
		m_read.unlock();
	}

	WJR_INTRINSIC_INLINE bool try_lock_shared() noexcept {
		if (!m_read.try_lock())return false;
		if (++m_count == 1) {
			if (!m_write.try_lock()) {
				--m_count;
				m_read.unlock();
				return false;
			}
		}
		m_read.unlock();
		return true;
	}

	WJR_INTRINSIC_INLINE void unlock_shared() noexcept {
		m_read.lock();
		if (--m_count == 0)m_write.unlock();
		m_read.unlock();
	}

	WJR_INTRINSIC_INLINE spin_mutex& read_flag() { return m_read; }
	WJR_INTRINSIC_INLINE const spin_mutex& read_flag() const { return m_read; }
	WJR_INTRINSIC_INLINE spin_mutex& write_flag() { return m_write; }
	WJR_INTRINSIC_INLINE const spin_mutex& write_flag() const { return m_write; }

private:
	spin_mutex m_read = {};
	spin_mutex m_write = {};
	size_t m_count = 0;
};

template<typename Mutex>
WJR_INTRINSIC_INLINE void wait(std::unique_lock<Mutex>& lck, waiter& it) {
	lck.unlock();
	it();
	lck.lock();
}


template<typename Mutex, typename Func>
WJR_INTRINSIC_INLINE void wait(std::unique_lock<Mutex>& lck, waiter& it, Func fn) {
	while (!fn()) {
		wait(lck, it);
	}
}

template<typename Mutex, typename Clock, typename Duration>
WJR_INTRINSIC_INLINE void wait_until(std::unique_lock<Mutex>& lck, waiter& it,
	const std::chrono::time_point<Clock, Duration>& rel_time){
	while (true) {
		const auto _Now = Clock::now();
		if (_Now >= rel_time) {
			return;
		}
		wait(lck, it);
	}
}

// if time out return false
// else return true
template<typename Mutex, typename Clock, typename Duration, typename Func>
WJR_INTRINSIC_INLINE bool wait_until(std::unique_lock<Mutex>& lck, waiter& it,
	const std::chrono::time_point<Clock, Duration>& rel_time, Func fn) {
	while (!fn()) {
		const auto _Now = Clock::now();
		if (_Now >= rel_time) {
			return false;
		}
		wait(lck, it);
	}
	return true;
}

template<typename Mutex, typename Rep, typename Period>
WJR_INTRINSIC_INLINE void wait_for(std::unique_lock<Mutex>& lck, waiter& it,
	const std::chrono::duration<Rep, Period>& rel_time) {
	wait_until(lck, it, std::chrono::steady_clock::now() + rel_time);
}

template<typename Mutex, typename Rep, typename Period, typename Func>
WJR_INTRINSIC_INLINE void wait_for(std::unique_lock<Mutex>& lck, waiter& it,
	const std::chrono::duration<Rep, Period>& rel_time, Func fn) {
	wait_until(lck, it, std::chrono::steady_clock::now() + rel_time, std::move(fn));
}

_WJR_END

#endif //__WJR_NETWORK_SPIN_MUTEX_H