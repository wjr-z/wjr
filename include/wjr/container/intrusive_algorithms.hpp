#ifndef WJR_CONTAINER_INTRUSIVE_ALGORITHMS_HPP__
#define WJR_CONTAINER_INTRUSIVE_ALGORITHMS_HPP__

#include <concepts>
#include <functional>
#include <type_traits>

#include <wjr/container/forward_list.hpp>
#include <wjr/container/list.hpp>

namespace wjr::intrusive {

namespace detail {

/// @brief Consume a detached sequence; also used when its sentinel is being destroyed.
template <class Node, class Disposer>
constexpr void dispose_nodes(Node *first, std::type_identity_t<Node> *last,
                             Disposer &&dispose) noexcept {
    while (first != last) {
        auto *next = first->next();
        std::invoke(dispose, first->self());
        first = next;
    }
}

} // namespace detail

/**
 * @brief Empty a ring and invoke dispose once on each payload pointer in order.
 * @pre head is a valid sentinel. Disposer invocation must not throw or modify
 * head or any element that has not yet been passed to dispose.
 * @note Linear time, no allocation, copying, or automatic payload destruction.
 * The head is empty before disposal starts and is reusable on return. Each next
 * link is saved before disposal, so dispose may destroy its current object.
 * Removed hook links are not reset and must not be followed or unlinked.
 */
template <class T, class Tag, class Disposer>
requires std::is_nothrow_invocable_v<Disposer &, T *>
constexpr void clear_and_dispose(list_node<T, Tag> &head, Disposer dispose) noexcept {
    auto *first = head.next();
    head.init_self();
    detail::dispose_nodes(first, &head, dispose);
}

/// @brief Empty an hlist with the same disposal and lifetime contract as the ring overload.
template <class T, class Tag, class Disposer>
requires std::is_nothrow_invocable_v<Disposer &, T *>
constexpr void clear_and_dispose(hlist_node<T, Tag> &head, Disposer dispose) noexcept {
    auto *first = head.next();
    head.init_self();
    detail::dispose_nodes(first, static_cast<hlist_node<T, Tag> *>(nullptr), dispose);
}

/**
 * @brief Unlink elements satisfying pred, returning the number removed.
 * @pre head is a valid sentinel; pred does not change links or destroy elements.
 * @note Linear time, no allocation or destruction. Removed links are not reset.
 * If pred throws, prior removals remain applied and the ring stays valid.
 */
template <class T, class Tag, class Pred>
requires(!std::is_void_v<T> && std::predicate<Pred &, T &>)
constexpr size_t remove_if(list_node<T, Tag> &head,
                           Pred pred) noexcept(std::is_nothrow_invocable_r_v<bool, Pred &, T &>) {
    size_t removed = 0;
    auto *current = head.next();
    while (current != &head) {
        auto *next = current->next();
        if (std::invoke(pred, *current->self())) {
            current->remove();
            ++removed;
        }
        current = next;
    }
    return removed;
}

/// @brief The hlist counterpart of remove_if, with the same lifetime/exception contract.
template <class T, class Tag, class Pred>
requires(!std::is_void_v<T> && std::predicate<Pred &, T &>)
constexpr size_t remove_if(hlist_node<T, Tag> &head,
                           Pred pred) noexcept(std::is_nothrow_invocable_r_v<bool, Pred &, T &>) {
    size_t removed = 0;
    auto *previous = &head;
    auto *current = head.next();
    while (current != nullptr) {
        auto *next = current->next();
        if (std::invoke(pred, *current->self())) {
            previous->set_next(next);
            ++removed;
        } else {
            previous = current;
        }
        current = next;
    }
    return removed;
}

/**
 * @brief Unlink adjacent equivalent elements, retaining the first of each group.
 * @pre head is a valid sentinel; equal is an equivalence relation and does not
 * change links or destroy elements.
 * @return The number removed. Does not allocate, destroy, or reset removed links.
 * @note Linear time. If equal throws, prior removals remain applied and the ring
 * stays valid. Compares the last retained element with the next element.
 */
template <class T, class Tag, class Equal = std::equal_to<>>
requires(!std::is_void_v<T> && std::predicate<Equal &, T &, T &>)
constexpr size_t
unique(list_node<T, Tag> &head,
       Equal equal = {}) noexcept(std::is_nothrow_invocable_r_v<bool, Equal &, T &, T &>) {
    size_t removed = 0;
    auto *previous = head.next();
    if (previous == &head) {
        return removed;
    }
    auto *current = previous->next();
    while (current != &head) {
        auto *next = current->next();
        if (std::invoke(equal, *previous->self(), *current->self())) {
            current->remove();
            ++removed;
        } else {
            previous = current;
        }
        current = next;
    }
    return removed;
}

/// @brief The hlist counterpart of unique, with the same lifetime/exception contract.
template <class T, class Tag, class Equal = std::equal_to<>>
requires(!std::is_void_v<T> && std::predicate<Equal &, T &, T &>)
constexpr size_t
unique(hlist_node<T, Tag> &head,
       Equal equal = {}) noexcept(std::is_nothrow_invocable_r_v<bool, Equal &, T &, T &>) {
    size_t removed = 0;
    auto *previous = head.next();
    if (previous == nullptr) {
        return removed;
    }
    auto *current = previous->next();
    while (current != nullptr) {
        auto *next = current->next();
        if (std::invoke(equal, *previous->self(), *current->self())) {
            previous->set_next(next);
            ++removed;
        } else {
            previous = current;
        }
        current = next;
    }
    return removed;
}

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_INTRUSIVE_ALGORITHMS_HPP__
