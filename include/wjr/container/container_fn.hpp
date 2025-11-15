/**
 * @file container_fn.hpp
 * @author wjr
 * @brief Implement the corresponding function to automatically implement the standard allocator
 * specification.
 * @version 0.1
 * @date 2025-01-18
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef WJR_CONTAINER_CONTAINER_FN_HPP__
#define WJR_CONTAINER_CONTAINER_FN_HPP__

#include <memory>
#include <type_traits>

#include <wjr/assert.hpp>
#include <wjr/config.hpp>

namespace wjr {

/**
 * @class container_fn<Alloc>
 * @brief The same characteristics and behavior of all allocator containers
 *
 * @details container must have the following member functions:
 * 1. auto& _get_allocator() noexcept
 * 2. void _destroy_and_deallocate() noexcept
 * 3. void _release_before_copy()
 * 4. void _copy_element(const container& other)
 * 5. void _take_storage(container&& other)
 * 6. void _destroy_and_move_element(container&& other)
 * 7. void _swap_storage(container& other)
 *
 * 1   : Is used to manage the allocator of the container. \n
 * 2   : Destroy element but not release memory. \n
 * worry about whether it is in an empty state.  \n
 * 3   : _destroy_and_deallocate and clear, set to empty state.    \n
 * 4-7 : Is used to assign the container data. Shouldn't change the allocator.
 *
 */
template <typename Alloc>
class container_fn {
private:
    using allocator_traits = std::allocator_traits<Alloc>;
    using is_always_equal = typename allocator_traits::is_always_equal;
    using propagate_on_container_copy_assignment =
        typename allocator_traits::propagate_on_container_copy_assignment;
    using propagate_on_container_move_assignment =
        typename allocator_traits::propagate_on_container_move_assignment;
    using propagate_on_container_swap = typename allocator_traits::propagate_on_container_swap;

public:
    template <typename Container>
    constexpr static void copy_assign(Container &lhs, const Container &rhs) noexcept(
        noexcept(lhs._copy_element(rhs)) && !propagate_on_container_copy_assignment::value
            ? true
            : (std::is_nothrow_copy_assignable_v<Alloc> && is_always_equal::value
                   ? true
                   : noexcept(lhs._destroy_and_deallocate()))) {
        if constexpr (propagate_on_container_copy_assignment::value) {
            auto &lhs_allocator = lhs._get_allocator();
            const auto &rhs_allocator = rhs._get_allocator();
            if constexpr (!is_always_equal::value) {
                if (lhs_allocator != rhs_allocator) {
                    lhs._release_before_copy();
                }
            }

            lhs_allocator = rhs_allocator;
        }

        lhs._copy_element(rhs);
    }

    template <typename Container>
    constexpr static void move_assign(Container &lhs, Container &&rhs) noexcept(
        noexcept(lhs._destroy_and_deallocate()) && noexcept(lhs._take_storage(std::move(rhs))) &&
                std::disjunction_v<propagate_on_container_move_assignment, is_always_equal>
            ? (!propagate_on_container_move_assignment::value
                   ? true
                   : std::is_nothrow_move_assignable_v<Alloc>)
            : (noexcept(lhs._destroy_and_move_element(std::move(rhs))))) {
        if constexpr (std::disjunction_v<propagate_on_container_move_assignment, is_always_equal>) {
            lhs._destroy_and_deallocate();
            if constexpr (propagate_on_container_move_assignment::value) {
                lhs._get_allocator() = std::move(rhs._get_allocator());
            }
            lhs._take_storage(std::move(rhs));
        } else {
            if (lhs._get_allocator() != rhs._get_allocator()) {
                lhs._destroy_and_move_element(std::move(rhs));
            } else {
                lhs._destroy_and_deallocate();
                lhs._take_storage(std::move(rhs));
            }
        }
    }

    template <typename Container>
    constexpr static void swap(Container &lhs, Container &rhs) noexcept(
        noexcept(lhs._swap_storage(rhs)) &&
                !std::conjunction_v<propagate_on_container_swap, std::negation<is_always_equal>>
            ? true
            : std::is_nothrow_swappable_v<Alloc>) {
        if constexpr (std::negation_v<is_always_equal>) {
            if constexpr (propagate_on_container_swap::value) {
                auto &lhs_allocator = lhs._get_allocator();
                auto &rhs_allocator = rhs._get_allocator();
                if (lhs_allocator != rhs_allocator) {
                    std::swap(lhs_allocator, rhs_allocator);
                }
            } else {
                WJR_ASSERT(lhs._get_allocator() == rhs._get_allocator());
            }
        }

        lhs._swap_storage(rhs);
    }
};

} // namespace wjr

#endif // WJR_CONTAINER_CONTAINER_FN_HPP__