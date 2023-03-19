#include <wjr/network/thread_pool.h>

_WJR_BEGIN

thread_pool::thread_pool(
	unsigned int core_threads_size){
	create_all_threads(core_threads_size);
}

thread_pool::~thread_pool() {
	flush();
	{
		std::unique_lock fast_task_lock(m_fast_task_mutex);
		m_valid = false;
	}
	for (auto& i : m_core_threads) i.join();
}

void thread_pool::pause() {
	m_pause.store(true, std::memory_order_relaxed);
}

void thread_pool::unpause() {
	m_pause.store(false, std::memory_order_relaxed);
}

void thread_pool::flush() {
	waiter it;
	std::unique_lock fast_task_lock(m_fast_task_mutex);
	wjr::wait(fast_task_lock, it, [this] {
		return (m_real_tasks.load(std::memory_order_relaxed) 
			== (m_pause.load(std::memory_order_relaxed) ? m_task_queue.size() : 0));
		});
}

bool thread_pool::is_valid() const {
	return m_valid;
}

bool thread_pool::is_paused() const {
	return m_pause.load(std::memory_order_relaxed);
}

unsigned int thread_pool::get_threads_size() const {
	return m_core_threads.size();
}

void thread_pool::core_work() {

	for (;;) {
		std::function<void()> task;

		{
			waiter it;
			std::unique_lock fast_task_lock(m_fast_task_mutex);
			wjr::wait(fast_task_lock, it, [this] {
				return !m_task_queue.empty()
					|| !m_valid;
				});

			if (is_likely(m_valid)) {
				if (!m_pause.load(std::memory_order_relaxed)) {
					task = std::move(m_task_queue.front());
					m_task_queue.pop_front();
					fast_task_lock.unlock();
					task();
					m_real_tasks.fetch_sub(1, std::memory_order_relaxed);
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