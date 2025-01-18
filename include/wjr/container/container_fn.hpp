/**
 * @file container_fn.hpp
 * @author wjr
 * @brief 
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

#include <wjr/preprocessor.hpp>

namespace wjr {

/**
 * @class container_fn<Alloc>
 * @brief The same characteristics and behavior of all allocator containers
 *
 * @details container must have the following member functions:
 * -# auto& __get_allocator() noexcept
 * -# void __destroy() noexcept
 * -# void __destroy_and_deallocate() noexcept
 * -# void __copy_element(const container& other)
 * -# void __take_storage(container&& other)
 * -# void __move_element(container&& other)
 * -# void __swap_storage(container& other)
 *
 * 1 : is used to manage the allocator of the container. \n
 * 2-3 : is used to destroy the container and deallocate the memory. \n
 * 4-7 : is used to assign the container data. Shouldn't change the allocator.
 *
 */
template <typename Alloc>
class container_fn {
private:
    using allocator_type = Alloc;
    using allocator_traits = std::allocator_traits<allocator_type>;
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
            : (noexcept(lhs.__get_allocator() = rhs.__get_allocator()) && is_always_equal::value
                   ? true
                   : noexcept(lhs.__destroy_and_deallocate()))) {
        if constexpr (propagate_on_container_copy_assignment::value) {
            auto &lhs_allocator = lhs.__get_allocator();
            const auto &rhs_allocator = rhs.__get_allocator();
            if constexpr (!is_always_equal::value) {
                if (lhs_allocator != rhs_allocator) {
                    lhs.__destroy_and_deallocate();
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
                   : noexcept(lhs.__get_allocator() = std::move(rhs.__get_allocator())))
            : (noexcept(lhs.__destroy()) && noexcept(lhs.__move_element(std::move(rhs))))) {
        if constexpr (std::disjunction_v<propagate_on_container_move_assignment, is_always_equal>) {
            lhs.__destroy_and_deallocate();
            if constexpr (propagate_on_container_move_assignment::value) {
                lhs.__get_allocator() = std::move(rhs.__get_allocator());
            }
            lhs.__take_storage(std::move(rhs));
        } else {
            if (lhs.__get_allocator() != rhs.__get_allocator()) {
                lhs.__destroy();
                lhs.__move_element(std::move(rhs));
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
            : noexcept(std::swap(lhs.__get_allocator(), rhs.__get_allocator()))) {
        if constexpr (std::conjunction_v<propagate_on_container_swap,
                                         std::negation<is_always_equal>>) {
            auto &lhs_allocator = lhs.__get_allocator();
            auto &rhs_allocator = rhs.__get_allocator();
            if (lhs_allocator != rhs_allocator) {
                std::swap(lhs_allocator, rhs_allocator);
            }
        }

        lhs.__swap_storage(rhs);
    }
};

} // namespace wjr

#endif // WJR_CONTAINER_CONTAINER_TRAITS_HPP__