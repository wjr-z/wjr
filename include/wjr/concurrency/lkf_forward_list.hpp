#ifndef WJR_CONCURRENCY_LKF_FORWARD_LIST_HPP__
#define WJR_CONCURRENCY_LKF_FORWARD_LIST_HPP__

#include <atomic>

#include <wjr/container/forward_list.hpp>

namespace wjr::intrusive {

struct lkf_forward_list_node {
    std::atomic<forward_list_node *> next;
};

inline void init(lkf_forward_list_node *head) noexcept {
    head->next.store(nullptr, std::memory_order_relaxed);
}

inline void push_front(lkf_forward_list_node *head, forward_list_node *node) noexcept {
    auto old_node = head->next.load(std::memory_order_relaxed);
    do {
        node->next = old_node;
    } while (!head->next.compare_exchange_weak(old_node, node, std::memory_order_release,
                                               std::memory_order_relaxed));
}

inline forward_list_node *pop_front(lkf_forward_list_node *head) noexcept {
    auto node = head->next.load(std::memory_order_relaxed);
    forward_list_node *next_node;

    do {
        if (node == nullptr) {
            return nullptr;
        }

        next_node = node->next;
    } while (!head->next.compare_exchange_weak(node, next_node, std::memory_order_release,
                                               std::memory_order_relaxed));

    WJR_ASSUME(node != nullptr);
    return node;
}

inline forward_list_node *clear(lkf_forward_list_node *head) noexcept {
    return head->next.exchange(nullptr, std::memory_order_acq_rel);
}

} // namespace wjr::intrusive

#endif // WJR_CONCURRENCY_LKF_FORWARD_LIST_HPP__