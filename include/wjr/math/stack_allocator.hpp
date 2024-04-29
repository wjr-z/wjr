#ifndef WJR_MATH_STACK_ALLOCATOR_HPP__
#define WJR_MATH_STACK_ALLOCATOR_HPP__

#include <wjr/memory/stack_allocator.hpp>

namespace wjr::math_details {

using stack_alloc_object = singleton_stack_allocator_object<36 * 1024, 16 * 1024>;
inline constexpr stack_alloc_object stack_alloc = {};

using unique_stack_alloc = unique_stack_allocator<stack_alloc_object>;

template <typename T>
using weak_stack_alloc = weak_stack_allocator<T, stack_alloc_object>;

} // namespace wjr::math_details

#endif // WJR_MATH_STACK_ALLOCATOR_HPP__