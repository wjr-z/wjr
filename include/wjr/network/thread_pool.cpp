#include <wjr/network/thread_pool.h>

_WJR_BEGIN

thread_pool::thread_pool(
	unsigned int core_threads_size,
	size_t task_limit,
	unsigned int max_threads_size)
	: thread_pool(core_threads_size, task_limit, max_threads_size, std::chrono::nanoseconds(0)) {}

thread_pool::~thread_pool() {
	flush();
	{
		std::unique_lock slow_task_lock(m_slow_task_mutex);
		m_valid = false;
	}
	m_task_cv.notify_all();
	for (auto& i : m_core_threads) i.join();
}

void thread_pool::start(
	unsigned int core_threads_size,
	size_t task_limit,
	unsigned int max_threads_size) {
	start(core_threads_size, task_limit, max_threads_size, std::chrono::nanoseconds(0));
}

void thread_pool::pause() {
	m_pause.store(true);
}

void thread_pool::unpause() {
	m_pause.store(false);
}

void thread_pool::flush() {
	std::unique_lock fast_flush_lock(m_fast_flush_mutex);
	std::unique_lock slow_task_lock(m_slow_task_mutex);
	m_flush_cv.wait(slow_task_lock, [this] {
		return (m_real_tasks.load(std::memory_order_relaxed) == (m_pause ? m_task_queue.size() : 0));
		});
}

bool thread_pool::is_valid() const {
	return m_valid;
}

bool thread_pool::is_paused() const {
	return m_pause;
}

unsigned int thread_pool::get_threads_size() const {
	return m_core_threads.size();
}

void thread_pool::core_work() {

	for (;;) {
		std::function<void()> task;

		{
			std::unique_lock slow_task_lock(m_slow_task_mutex);
			m_task_cv.wait(slow_task_lock, [this] {
				return !m_task_queue.empty()
					|| !m_valid;
				});

			if (is_likely(m_valid)) {
				if (!m_pause) {
					task = std::move(m_task_queue.front());
					m_task_queue.pop_front();
					slow_task_lock.unlock();
					task();
					if (is_likely(m_fast_flush_mutex.try_lock())) {
						m_real_tasks.fetch_sub(1, std::memory_order_relaxed);
						m_fast_flush_mutex.unlock();
					}
					else {
						slow_task_lock.lock();
						m_real_tasks.fetch_sub(1, std::memory_order_relaxed);
						slow_task_lock.unlock();
						m_flush_cv.notify_one();
					}
				}
				continue;
			}

			break;
		}
	}

}

void thread_pool::create_all_threads(
	unsigned int core_threads_size,
	size_t task_limit,
	unsigned int max_threads_size,
	std::chrono::nanoseconds alive_limit
) {
#if defined(_WJR_EXCEPTION)
	if (!core_threads_size) {
		throw std::invalid_argument("!core_threads_size");
	}
#endif // _WJR_EXCEPTION

	m_task_limit = task_limit;
	m_alive_limit = alive_limit;

	m_core_threads.reserve(core_threads_size);

	m_valid = true;
	m_pause = false;

	for (unsigned int i = 0; i < core_threads_size; ++i) {
		m_core_threads.emplace_back(&thread_pool::core_work, this);
	}
}

size_t thread_pool::default_threads_size() {
	return std::max(static_cast<size_t>(1), static_cast<size_t>(std::thread::hardware_concurrency()));
}

_WJR_END