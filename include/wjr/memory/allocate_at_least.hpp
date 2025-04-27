#ifndef WJR_MEMORY_ALLOCATE_AT_LEAST_HPP__
#define WJR_MEMORY_ALLOCATE_AT_LEAST_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {
template <class Pointer, class SizeType = std::size_t>
struct allocation_result {
    Pointer ptr;
    SizeType count;
};

WJR_REGISTER_HAS_TYPE(allocate_at_least,
                      std::declval<Allocator>().allocate_at_least(std::declval<SizeType>()),
                      Allocator, SizeType);

template <typename Allocator, typename SizeType>
WJR_NODISCARD auto allocate_at_least(Allocator &alloc, SizeType count) {
    if constexpr (has_allocate_at_least_v<Allocator, SizeType>) {
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