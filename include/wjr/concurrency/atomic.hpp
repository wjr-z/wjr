#ifndef WJR_CONCURRENCY_ATOMIC_HPP__
#define WJR_CONCURRENCY_ATOMIC_HPP__

#include <atomic>

namespace wjr {

template <typename T>
T atomic_ref_load(T *obj, std::memory_order order = std::memory_order_seq_cst) noexcept {
    std::atomic_ref<T> ref(*obj);
    return obj.load(order);
}

template <typename T>
void atomic_ref_store(T *obj, T val, std::memory_order order = std::memory_order_seq_cst) noexcept {
    std::atomic_ref<T> ref(*obj);
    ref.store(val, order);
}

template <typename T>
T atomic_ref_fetch_add(T *obj, typename std::atomic_ref<T>::difference_type arg,
                       std::memory_order order = std::memory_order_seq_cst) noexcept {
    std::atomic_ref<T> ref(*obj);
    return ref.fetch_add(arg, order);
}

template <typename T>
T atomic_ref_fetch_sub(T *obj, typename std::atomic_ref<T>::difference_type arg,
                       std::memory_order order = std::memory_order_seq_cst) noexcept {
    std::atomic_ref<T> ref(*obj);
    return ref.fetch_sub(arg, order);
}

template <typename T>
T atomic_ref_fetch_or(T *obj, T arg, std::memory_order order = std::memory_order_seq_cst) noexcept {
    std::atomic_ref<T> ref(*obj);
    return ref.fetch_or(arg, order);
}

template <typename T>
T atomic_ref_fetch_and(T *obj, T arg,
                       std::memory_order order = std::memory_order_seq_cst) noexcept {
    std::atomic_ref<T> ref(*obj);
    return ref.fetch_and(arg, order);
}

template <typename T>
T atomic_ref_fetch_xor(T *obj, T arg,
                       std::memory_order order = std::memory_order_seq_cst) noexcept {
    std::atomic_ref<T> ref(*obj);
    return ref.fetch_xor(arg, order);
}

template <typename T>
T atomic_ref_fetch_exchange(T *obj, typename std::atomic_ref<T>::value_type desired,
                            std::memory_order order = std::memory_order_seq_cst) noexcept {
    std::atomic_ref<T> ref(*obj);
    return ref.exchange(desired, order);
}

} // namespace wjr

#endif // WJR_CONCURRENCY_ATOMIC_HPP__