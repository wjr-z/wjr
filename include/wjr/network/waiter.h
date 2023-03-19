#pragma once
#ifndef __WJR_NETWORK_WAITER_H
#define __WJR_NETWORK_WAITER_H
#include <chrono>
#include <thread>

#include <wjr/asm/asm.h>

_WJR_BEGIN

// waiter : non-blocking
// use pause when the number of times is small, 
// use yield if the wait time is long,
// use sleep when wait time is too long
// pause : about 5 ~ 20ns (default : 2^5 times -> 160 ` 640 ns)
// yield : about 100 ~ 1000ns (default : 2^16 times -> 6 ~ 60 ms)
// sleep : about 10 ms (sleep after 60 ms)
class waiter {
public:
	constexpr static unsigned int default_pause_limit = 1 << 5;
	constexpr static unsigned int default_yield_limit = 1 << 16;
	constexpr static std::chrono::milliseconds default_sleep_time = std::chrono::milliseconds(10);

	WJR_INTRINSIC_CONSTEXPR waiter() noexcept = default;
	WJR_INTRINSIC_CONSTEXPR waiter(
		unsigned int pause_limit,
		unsigned int yield_limit = default_yield_limit,
		const std::chrono::milliseconds& sleep_time = default_sleep_time)
		: m_pause_limit(pause_limit), m_yield_limit(yield_limit), m_sleep_time(sleep_time) {}

	WJR_INTRINSIC_CONSTEXPR waiter(const waiter& other) noexcept = default;
	WJR_INTRINSIC_CONSTEXPR waiter& operator=(const waiter& other) noexcept = default;

	WJR_INTRINSIC_INLINE void operator()() noexcept {
		assume_no_add_overflow(m_count, 1u);
		if (m_count < m_pause_limit) {
			++m_count;
			wjr::masm::pause();
		}
		else {
			if (m_count < m_yield_limit) {
				++m_count;
				std::this_thread::yield();
			}
			else {
				std::this_thread::sleep_for(m_sleep_time);
			}
		}
	}

private:
	unsigned int m_count = 0;
	unsigned int m_pause_limit = default_pause_limit;
	unsigned int m_yield_limit = default_yield_limit;
	std::chrono::milliseconds m_sleep_time = default_sleep_time;
};

WJR_INTRINSIC_INLINE void wait(waiter& it) {
	it();
}


template<typename Func>
WJR_INTRINSIC_INLINE void wait(waiter& it, Func fn) {
	while (!fn()) {
		wait(it);
	}
}

template<typename Clock, typename Duration>
WJR_INTRINSIC_INLINE void wait_until(waiter& it,
	const std::chrono::time_point<Clock, Duration>& rel_time) {
	while (true) {
		const auto _Now = Clock::now();
		if (_Now >= rel_time) {
			return;
		}
		wait(it);
	}
}

// if time out return false
// else return true
template<typename Clock, typename Duration, typename Func>
WJR_INTRINSIC_INLINE bool wait_until(waiter& it,
	const std::chrono::time_point<Clock, Duration>& rel_time, Func fn) {
	while (!fn()) {
		const auto _Now = Clock::now();
		if (_Now >= rel_time) {
			return false;
		}
		wait(it);
	}
	return true;
}

template<typename Rep, typename Period>
WJR_INTRINSIC_INLINE void wait_for(waiter& it,
	const std::chrono::duration<Rep, Period>& rel_time) {
	wait_until(it, std::chrono::steady_clock::now() + rel_time);
}

template<typename Rep, typename Period, typename Func>
WJR_INTRINSIC_INLINE void wait_for(waiter& it,
	const std::chrono::duration<Rep, Period>& rel_time, Func fn) {
	wait_until(it, std::chrono::steady_clock::now() + rel_time, std::move(fn));
}

_WJR_END

#endif // __WJR_NETWORK_WAITER_H