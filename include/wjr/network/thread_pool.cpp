#include <wjr/network/thread_pool.h>

_WJR_BEGIN

size_t thread_pool::default_threads_size() {
	return std::max(static_cast<size_t>(1), static_cast<size_t>(std::thread::hardware_concurrency()));
}

thread_pool::thread_pool(
	unsigned int core_threads_size) {
	create_all_threads(core_threads_size);
}

thread_pool::~thread_pool() {
	if (m_valid) {
		shutdown();
	}
}

std::mutex& thread_pool::get_mutex() {
	return m_task_mutex;
}

void thread_pool::pause() {
	std::unique_lock task_lock(m_task_mutex);
	m_pause = true;
}

void thread_pool::unpause() {
	std::unique_lock task_lock(m_task_mutex);
	m_pause = false;
}

void thread_pool::flush() {
	std::unique_lock task_lock(m_task_mutex);
	// can't flush multiple times
	if (m_flush) return;
	m_flush = true;
	m_flush_cv.wait(task_lock, [this] {
		return (m_real_tasks.load(std::memory_order_relaxed)
			== (m_pause ? m_task_queue.size() : 0));
		});
	m_flush = false;
}

void thread_pool::shutdown() {
#if defined(_WJR_EXCEPTION)
	if (!m_valid) {
		throw std::runtime_error("can't shutdown thread pool more than once");
	}
#endif // _WJR_EXCEPTION
	flush();
	{
		std::unique_lock task_lock(m_task_mutex);
		m_valid = false;
	}
	m_task_cv.notify_all();
	for (auto& i : m_core_threads) i.join();
}

typename thread_pool::queue_type thread_pool::shutdown(bool) {
	shutdown();
	std::unique_lock task_lock(m_task_mutex);
	return std::move(m_task_queue);
}

unsigned int thread_pool::get_threads_size() const {
	return static_cast<unsigned int>(m_core_threads.size());
}

void thread_pool::core_work() {

	for (;;) {
		std::function<void()> task;

		{
			using namespace wjr::enum_ops;
			std::unique_lock task_lock(m_task_mutex);
			m_task_cv.wait(task_lock, [this] {
				return !m_task_queue.empty() || !m_valid;
				});

			if (WJR_LIKELY(m_valid)) {
				if (!m_pause) {
					task = std::move(m_task_queue.front());
					m_task_queue.pop_front();
					task_lock.unlock(); 
					task();
					task_lock.lock();
					m_real_tasks.fetch_sub(1, std::memory_order_relaxed);
					if (m_flush) {
						task_lock.unlock();
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
	unsigned int core_threads_size) {
#if defined(_WJR_EXCEPTION)
	if (!core_threads_size) {
		throw std::invalid_argument("!core_threads_size");
	}
#endif // _WJR_EXCEPTION

	m_core_threads.reserve(core_threads_size);

	for (unsigned int i = 0; i < core_threads_size; ++i) {
		m_core_threads.emplace_back(&thread_pool::core_work, this);
	}
}


_WJR_END