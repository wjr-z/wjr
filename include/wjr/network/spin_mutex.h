#pragma once
#ifndef __WJR_NETWORK_SPIN_MUTEX_H
#define __WJR_NETWORK_SPIN_MUTEX_H

#include <atomic>
#include <mutex>

#include <wjr/macro.h>

_WJR_BEGIN

class spin_mutex {
public:
	WJR_INTRINSIC_INLINE spin_mutex() = default;
	spin_mutex(const spin_mutex&) = delete;
	spin_mutex& operator=(const spin_mutex&) = delete;
	WJR_INTRINSIC_INLINE ~spin_mutex() = default;

	WJR_INTRINSIC_INLINE void lock() noexcept {
		while (m_flag.test_and_set(std::memory_order_acquire));
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

class rw_spin_mutex {
public:
	WJR_INTRINSIC_INLINE rw_spin_mutex() = default;
	rw_spin_mutex(const rw_spin_mutex&) = delete;
	rw_spin_mutex& operator=(const rw_spin_mutex&) = delete;
	WJR_INTRINSIC_INLINE ~rw_spin_mutex() = default;

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

_WJR_END

#endif //__WJR_NETWORK_SPIN_MUTEX_H