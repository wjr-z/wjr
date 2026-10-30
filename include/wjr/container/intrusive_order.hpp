#ifndef WJR_CONTAINER_INTRUSIVE_ORDER_HPP__
#define WJR_CONTAINER_INTRUSIVE_ORDER_HPP__

#include <array>
#include <concepts>
#include <functional>
#include <limits>

#include <wjr/container/forward_list.hpp>
#include <wjr/container/list.hpp>

namespace wjr::intrusive {

/// @brief Reverse a valid ring in linear time without moving or destroying payloads.
template <class T, class Tag>
constexpr void reverse(list_node<T, Tag> &head) noexcept {
    auto *current = &head;
    do {
        auto *next = current->next();
        current->set_next(current->prev());
        current->set_prev(next);
        current = next;
    } while (current != &head);
}

/// @brief Reverse a valid hlist in linear time without moving or destroying payloads.
template <class T, class Tag>
constexpr void reverse(hlist_node<T, Tag> &head) noexcept {
    hlist_node<T, Tag> *previous = nullptr;
    auto *current = head.next();
    while (current != nullptr) {
        auto *next = current->next();
        current->set_next(previous);
        previous = current;
        current = next;
    }
    head.set_next(previous);
}

/**
 * @brief Stably merge source into destination, leaving source empty.
 * @pre Both sentinels head valid, disjoint rings sorted by compare; a self-merge
 * is a no-op. compare is a strict weak order and does not change links or destroy
 * elements. Equivalent destination elements precede equivalent source elements.
 * @note Linear comparisons, no allocation or payload movement. If compare throws,
 * prior transfers remain applied and both rings stay valid without lost nodes.
 */
template <class T, class Tag, class Compare = std::less<>>
requires(!std::is_void_v<T> && std::strict_weak_order<Compare &, T &, T &>)
constexpr void
merge(list_node<T, Tag> &destination, list_node<T, Tag> &source,
      Compare compare = {}) noexcept(std::is_nothrow_invocable_r_v<bool, Compare &, T &, T &>) {
    if (&destination == &source || source.empty()) {
        return;
    }
    if (destination.empty()) {
        destination.splice_all_before(&source);
        return;
    }
    auto *left = destination.next();
    auto *right = source.next();
    while (left != &destination && right != &source) {
        if (std::invoke(compare, *right->self(), *left->self())) {
            auto *next = right->next();
            left->splice_before(right);
            right = next;
        } else {
            left = left->next();
        }
    }
    destination.splice_all_before(&source);
}

/// @brief The hlist counterpart of merge, with the same stability/exception contract.
template <class T, class Tag, class Compare = std::less<>>
requires(!std::is_void_v<T> && std::strict_weak_order<Compare &, T &, T &>)
constexpr void
merge(hlist_node<T, Tag> &destination, hlist_node<T, Tag> &source,
      Compare compare = {}) noexcept(std::is_nothrow_invocable_r_v<bool, Compare &, T &, T &>) {
    if (&destination == &source) {
        return;
    }
    auto *previous = &destination;
    auto *left = destination.next();
    auto *right = source.next();
    while (left != nullptr && right != nullptr) {
        if (std::invoke(compare, *right->self(), *left->self())) {
            auto *next = right->next();
            source.set_next(next);
            previous->set_next(right);
            right->set_next(left);
            previous = right;
            right = next;
        } else {
            previous = left;
            left = left->next();
        }
    }
    if (right != nullptr) {
        previous->set_next(right);
        source.init_self();
    }
}

/**
 * @brief Stable bottom-up merge sort of a ring in O(n log n) comparisons.
 * @pre head is a valid sentinel; compare is a strict weak order and does not
 * change links or destroy elements.
 * @note Allocates nothing and never copies/moves payloads. Uses a bounded array
 * of size_t-bit-count sentinels on the stack, and one comparator instance. If
 * compare throws, all original nodes remain in head, in unspecified order.
 * Element addresses, references, and other hooks remain unchanged.
 */
template <class T, class Tag, class Compare = std::less<>>
requires(!std::is_void_v<T> && std::strict_weak_order<Compare &, T &, T &>)
constexpr void
sort(list_node<T, Tag> &head,
     Compare compare = {}) noexcept(std::is_nothrow_invocable_r_v<bool, Compare &, T &, T &>) {
    if (head.empty() || head.next()->next() == &head) {
        return;
    }
    using node = list_node<T, Tag>;
    using buckets = std::array<node, std::numeric_limits<size_t>::digits>;
    node carry;
    buckets bins;
    struct recovery {
        node &head;
        node &carry;
        buckets &bins;
        bool active = true;
        constexpr ~recovery() {
            if (active) {
                head.splice_all_before(&carry);
                for (auto &bin : bins) {
                    head.splice_all_before(&bin);
                }
            }
        }
    } recover{head, carry, bins};

    size_t filled = 0;
    while (!head.empty()) {
        carry.splice_before(head.next());
        size_t index = 0;
        while (index < filled && !bins[index].empty()) {
            merge(bins[index], carry, std::ref(compare));
            carry.splice_all_before(&bins[index]);
            ++index;
        }
        bins[index].splice_all_before(&carry);
        if (index == filled) {
            ++filled;
        }
    }
    for (size_t index = 1; index < filled; ++index) {
        merge(bins[index], bins[index - 1], std::ref(compare));
    }
    head.splice_all_before(&bins[filled - 1]);
    recover.active = false;
}

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_INTRUSIVE_ORDER_HPP__
