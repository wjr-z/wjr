#ifndef WJR_MEMORY_ALLOCATE_AT_LEAST_HPP__
#define WJR_MEMORY_ALLOCATE_AT_LEAST_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {
template <class Pointer, class SizeType = std::size_t>
struct allocation_result {
    Pointer ptr;
    SizeType count;
};

namespace detail {
template <typename Allocator, typename SizeType>
concept has_allocate_at_least = requires(Allocator &alloc, SizeType count) {
    {
        alloc.allocate_at_least(count)
    } -> std::convertible_to<
          allocation_result<typename std::allocator_traits<Allocator>::pointer, SizeType>>;
};
} // namespace detail

template <typename Allocator, typename SizeType>
WJR_NODISCARD constexpr auto allocate_at_least(Allocator &alloc, SizeType count) {
    if constexpr (detail::has_allocate_at_least<Allocator, SizeType>) {
        auto result = alloc.allocate_at_least(count);
        WJR_ASSUME(result.count >= count);
        return result;
    } else {
        auto *const ptr = std::allocator_traits<Allocator>::allocate(alloc, count);
        return allocation_result<remove_cvref_t<decltype(ptr)>, SizeType>{ptr, count};
    }
}
} // namespace wjr

#endif // WJR_MEMORY_ALLOCATE_AT_LEAST_HPP__