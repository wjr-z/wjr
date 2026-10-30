#ifndef WJR_CONTAINER_INTRUSIVE_ALGORITHMS_HPP__
#define WJR_CONTAINER_INTRUSIVE_ALGORITHMS_HPP__

#include <concepts>
#include <functional>

#include <wjr/container/forward_list.hpp>
#include <wjr/container/list.hpp>

namespace wjr::intrusive {

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
