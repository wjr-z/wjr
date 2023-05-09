#pragma once
#ifndef __WJR_CONTAINER_TRAITS_H
#define __WJR_CONTAINER_TRAITS_H

#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename Container>
struct base_container_traits;

// do_move_construct :
// move container to other container
// 
// do_move_construct_by_element :
// move container to other container by element
// cannot directly move the pointer allocated by the allocator
// 
// do_tidy :
// destroy all elements in the container and free the memory
// 
// do_get_allocator :
// get the allocator of the container
// 
// do_copy_assign :
// copy container to other container
// need to destroy and free 
// 
// do_move_assign :
// move container to other container
// directly move the pointer, do not destroy elements, and do not free memory
// 
// do_move_assign_by_element :
// move container to other container by element
// need to destroy and free 
// 
// do_swap :
// swap two containers

template<typename Container>
class container_traits {
	using __traits = base_container_traits<Container>;
	using __allocator_type = typename __traits::allocator_type;
	using __allocator_traits = typename __traits::allocator_traits;
	using __allocator_is_always_equal = typename __allocator_traits::is_always_equal;
	using __allocator_propagate_on_container_move_assignment = 
		typename __allocator_traits::propagate_on_container_move_assignment;
	using __allocator_propagate_on_container_copy_assignment = 
		typename __allocator_traits::propagate_on_container_copy_assignment;
	using __allocator_propagate_on_container_swap = 
		typename __allocator_traits::propagate_on_container_swap;
	WJR_INTRINSIC_CONSTEXPR static bool do_equal_allocator(const Container& a, const Container& b) noexcept {
		return Container::do_get_allocator(a) == Container::do_get_allocator(b);
	}
	using __noexcept_move_construct = std::bool_constant<noexcept(
		Container::do_move_construct(std::declval<Container&>(), std::declval<Container&&>()))>;
	using __noexcept_move_construct_by_element = std::bool_constant < noexcept(
		Container::do_move_construct_by_element(std::declval<Container&>(), std::declval<Container&&>()))>;
	using __noexcept_tidy = std::bool_constant<noexcept(Container::do_tidy(std::declval<Container&>()))>;
	using __noexcept_get_allocator = 
		std::bool_constant<noexcept(Container::do_get_allocator(std::declval<Container&>()))>;
	using __noexcept_copy_assign = 
		std::bool_constant<noexcept(Container::do_copy_assign(std::declval<Container&>(), std::declval<const Container&>()))>;
	using __noexcept_move_assign = 
		std::bool_constant<noexcept(Container::do_move_assign(std::declval<Container&>(), std::declval<Container&&>()))>;
	using __noexcept_move_assign_by_element = 
		std::bool_constant<noexcept(Container::do_move_assign_by_element(std::declval<Container&>(), std::declval<Container&&>()))>;
	using __noexcept_swap = std::bool_constant<noexcept(Container::do_swap(std::declval<Container&>(), std::declval<Container&>()))>;

public:
	using noexcept_move_construct = std::conditional_t<
		__allocator_is_always_equal::value,
		__noexcept_move_construct,
		std::conjunction<__noexcept_move_construct, __noexcept_move_construct_by_element>>;
	WJR_INTRINSIC_CONSTEXPR static void move_construct(Container& a, Container&& b) 
		noexcept(noexcept_move_construct::value) {
		if constexpr (__allocator_is_always_equal::value) {
			Container::do_move_construct(a, std::move(b));
		} else {
			if (do_equal_allocator(a, b)) {
				Container::do_move_construct(a, std::move(b));
			}
			else {
				Container::do_move_construct_by_element(a, std::move(b));
			}
		}
	}

	using noexcept_copy_assign = std::conditional_t<
		std::conjunction_v<__allocator_propagate_on_container_copy_assignment,
		std::negation<__allocator_is_always_equal>>,
		std::conjunction<__noexcept_copy_assign, __noexcept_tidy>,
		__noexcept_copy_assign>;

	WJR_INTRINSIC_CONSTEXPR static void copy_assign(Container& a, const Container& b) 
		noexcept(noexcept_copy_assign::value) {
		if constexpr (std::conjunction_v<__allocator_propagate_on_container_copy_assignment,
			std::negation<__allocator_is_always_equal>>) {
			if (!do_equal_allocator(a, b)) {
				Container::do_tidy(a);
			}
		}
		if constexpr (__allocator_propagate_on_container_copy_assignment::value) {
			Container::do_get_allocator(a) = Container::do_get_allocator(b);
		}
		Container::do_copy_assign(a, b);
	}

	using noexcept_move_assign = std::conditional_t<
		std::disjunction_v<__allocator_propagate_on_container_move_assignment,
		__allocator_is_always_equal>,
		std::conjunction<__noexcept_move_assign, __noexcept_tidy>,
		std::conjunction<__noexcept_move_assign, __noexcept_tidy, __noexcept_move_assign_by_element>>;

	WJR_INTRINSIC_CONSTEXPR static void move_assign(Container& a, Container&& b)
		noexcept(noexcept_move_assign::value) {
		if constexpr (std::disjunction_v<__allocator_propagate_on_container_move_assignment,
			__allocator_is_always_equal>) {
			Container::do_tidy(a);
			if constexpr (__allocator_propagate_on_container_move_assignment::value) {
				Container::do_get_allocator(a) = std::move(Container::do_get_allocator(b));
			}
			Container::do_move_assign(a, std::move(b));
		}
		else {
			if (do_equal_allocator(a, b)) {
				Container::do_tidy(a);
				Container::do_move_assign(a, std::move(b));
			}
			else {
				Container::do_move_assign_by_element(a, std::move(b));
			}
		}
	}

	using noexcept_swap = __noexcept_swap;

	WJR_INTRINSIC_CONSTEXPR static void swap(Container& a, Container& b) 
		noexcept(noexcept_swap::value) {
		Container::do_swap(a, b);
		if constexpr (__allocator_propagate_on_container_swap::value) {
			std::swap(Container::do_get_allocator(a), Container::do_get_allocator(b));
		}
	}

};

_WJR_END

#endif // __WJR_CONTAINER_TRAITS_H