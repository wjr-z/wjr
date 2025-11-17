#ifndef WJR_CONCURRENCY_SCHEDULER_TASK_HPP__
#define WJR_CONCURRENCY_SCHEDULER_TASK_HPP__

namespace wjr::sched {
enum task_state {
    TASK_RUNNING,
    TASK_INTERRUPTIBLE,
    TASK_STOPPED,
};

class task {
public:
private:
    task_state m_state;
};

} // namespace wjr::sched

#endif // WJR_CONCURRENCY_SCHEDULER_TASK_HPP__