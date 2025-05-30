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

#ifndef WJR_CONTAINER_CONTAINER_TRAITS_HPP__
#define WJR_CONTAINER_CONTAINER_TRAITS_HPP__

#include <memory>
#include <type_traits>

#include <wjr/assert.hpp>
#include <wjr/preprocessor.hpp>

namespace wjr {

/**
 * @class container_fn<Alloc>
 * @brief The same characteristics and behavior of all allocator containers
 *
 * @details container must have the following member functions:
 * 1. auto& __get_allocator() noexcept
 * 2. void __destroy_and_deallocate() noexcept
 * 3. void __release_before_copy()
 * 4. void __copy_element(const container& other)
 * 5. void __take_storage(container&& other)
 * 6. void __destroy_and_move_element(container&& other)
 * 7. void __swap_storage(container& other)
 *
 * 1   : Is used to manage the allocator of the container. \n
 * 2   : Destroy element but not release memory. \n
 * worry about whether it is in an empty state.  \n
 * 3   : __destroy_and_deallocate and clear, set to empty state.    \n
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
    WJR_CONSTEXPR20 static void copy_assign(Container &lhs, const Container &rhs) noexcept(
        noexcept(lhs.__copy_element(rhs)) && !propagate_on_container_copy_assignment::value
            ? true
            : (std::is_nothrow_copy_assignable_v<Alloc> && is_always_equal::value
                   ? true
                   : noexcept(lhs.__destroy_and_deallocate()))) {
        if constexpr (propagate_on_container_copy_assignment::value) {
            auto &lhs_allocator = lhs.__get_allocator();
            const auto &rhs_allocator = rhs.__get_allocator();
            if constexpr (!is_always_equal::value) {
                if (lhs_allocator != rhs_allocator) {
                    lhs.__release_before_copy();
                }
            }

            lhs_allocator = rhs_allocator;
        }

        lhs.__copy_element(rhs);
    }

    template <typename Container>
    WJR_CONSTEXPR20 static void move_assign(Container &lhs, Container &&rhs) noexcept(
        noexcept(lhs.__destroy_and_deallocate()) && noexcept(lhs.__take_storage(std::move(rhs))) &&
                std::disjunction_v<propagate_on_container_move_assignment, is_always_equal>
            ? (!propagate_on_container_move_assignment::value
                   ? true
                   : std::is_nothrow_move_assignable_v<Alloc>)
            : (noexcept(lhs.__destroy_and_move_element(std::move(rhs))))) {
        if constexpr (std::disjunction_v<propagate_on_container_move_assignment, is_always_equal>) {
            lhs.__destroy_and_deallocate();
            if constexpr (propagate_on_container_move_assignment::value) {
                lhs.__get_allocator() = std::move(rhs.__get_allocator());
            }
            lhs.__take_storage(std::move(rhs));
        } else {
            if (lhs.__get_allocator() != rhs.__get_allocator()) {
                lhs.__destroy_and_move_element(std::move(rhs));
            } else {
                lhs.__destroy_and_deallocate();
                lhs.__take_storage(std::move(rhs));
            }
        }
    }

    template <typename Container>
    WJR_CONSTEXPR20 static void swap(Container &lhs, Container &rhs) noexcept(
        noexcept(lhs.__swap_storage(rhs)) &&
                !std::conjunction_v<propagate_on_container_swap, std::negation<is_always_equal>>
            ? true
            : std::is_nothrow_swappable_v<Alloc>) {
        if constexpr (std::negation_v<is_always_equal>) {
            if constexpr (propagate_on_container_swap::value) {
                auto &lhs_allocator = lhs.__get_allocator();
                auto &rhs_allocator = rhs.__get_allocator();
                if (lhs_allocator != rhs_allocator) {
                    std::swap(lhs_allocator, rhs_allocator);
                }
            } else {
                WJR_ASSERT(lhs.__get_allocator() == rhs.__get_allocator());
            }
        }

        lhs.__swap_storage(rhs);
    }
};

} // namespace wjr

#endif // WJR_CONTAINER_CONTAINER_TRAITS_HPP__