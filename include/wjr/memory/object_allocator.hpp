/**
 * @file object_allocator.hpp
 * @author wjr
 * @brief 
 * @version 0.1
 * @date 2025-02-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef WJR_MEMORY_OBJECT_ALLOCATOR_HPP__
#define WJR_MEMORY_OBJECT_ALLOCATOR_HPP__

#include <wjr/container/forward_list.hpp>

namespace wjr {

struct object_allocator_node : intrusive::hlist_node<object_allocator_node> {};

template <typename T, size_t CacheSize>
class object_allocator {
    static_assert(CacheSize > sizeof(T), "Useless. It will become inefficient.");

    static constexpr size_t RealSize = std::max(sizeof(T), sizeof(object_allocator_node));
    static constexpr size_t RealNum = (CacheSize - 1) / RealSize + 1;
    static constexpr size_t RealMemSize = RealSize * RealNum;

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
    using is_always_equal = std::false_type;
    using is_trivially_allocator = std::true_type;

    template <typename Other>
    struct rebind {
        using other = object_allocator<Other, CacheNum>;
    };

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(object_allocator);

public:
    friend bool operator==(const object_allocator &lhs, const object_allocator &rhs) noexcept {
        return std::addressof(lhs) == std::addressof(rhs);
    }

    friend bool operator!=(const object_allocator &lhs, const object_allocator &rhs) noexcept {
        return !(lhs == rhs);
    }

private:
    object_allocator_node m_head;
};

} // namespace wjr

#endif // WJR_MEMORY_OBJECT_ALLOCATOR_HPP__