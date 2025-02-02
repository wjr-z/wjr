#ifndef WJR_CONCURRENCY_BARRIER_HPP__
#define WJR_CONCURRENCY_BARRIER_HPP__

#include <atomic>

#include <wjr/preprocessor.hpp>

namespace wjr {
namespace bar {
WJR_INTRINSIC_INLINE void compiler_barrier() noexcept {
    std::atomic_signal_fence(std::memory_order_seq_cst);
}

WJR_INTRINSIC_INLINE void smp_mb() noexcept { std::atomic_thread_fence(std::memory_order_seq_cst); }

WJR_INTRINSIC_INLINE void smp_rmb() noexcept {
    std::atomic_thread_fence(std::memory_order_acquire);
}

WJR_INTRINSIC_INLINE void smp_wmb() noexcept {
    std::atomic_thread_fence(std::memory_order_release);
}
} // namespace bar

using bar::compiler_barrier;

} // namespace wjr

#endif // WJR_CONCURRENCY_BARRIER_HPP__