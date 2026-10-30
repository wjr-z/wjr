#ifndef WJR_CONTAINER_DETAIL_BTREE_SLOT_HPP__
#define WJR_CONTAINER_DETAIL_BTREE_SLOT_HPP__

#include <wjr/memory/small_copy.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr::btree_detail {

template <typename Traits>
inline constexpr bool compact_slots_v = requires { typename Traits::compact_storage_tag; };

/// @brief Raw storage for one actual const-key pair; unused slots contain no value object.
template <typename Value>
struct alignas(Value) compact_slot {
    unsigned char bytes[sizeof(Value)];

    Value *raw() noexcept { return reinterpret_cast<Value *>(bytes); }
    const Value *raw() const noexcept { return reinterpret_cast<const Value *>(bytes); }
    Value *get() noexcept { return std::launder(raw()); }
    const Value *get() const noexcept { return std::launder(raw()); }
};

template <typename Traits, bool = compact_slots_v<Traits>>
struct slot_ops;

/// @brief Preserve the existing trivial/pointer slot operations for legacy trees.
template <typename Traits>
struct slot_ops<Traits, false> {
    using slot_type = typename Traits::ivalue_type;
    using insert_type = slot_type;
    using allocator_type = typename Traits::_Alty;

    static void assign(slot_type *dst, insert_type value, allocator_type &) noexcept {
        *dst = value;
    }
    static void transfer_one(slot_type *src, slot_type *dst, allocator_type &) noexcept {
        *dst = *src;
    }

    template <size_t Min, size_t Max>
    WJR_INTRINSIC_INLINE static void transfer(slot_type *first, slot_type *last, slot_type *dst,
                                              allocator_type &) noexcept {
        small_copy<Min, Max>(first, last, dst);
    }

    template <size_t Min, size_t Max>
    WJR_INTRINSIC_INLINE static void transfer_backward(slot_type *first, slot_type *last,
                                                       slot_type *dst_end,
                                                       allocator_type &) noexcept {
        small_copy<Min, Max>(first, last, dst_end - (last - first));
    }
};

/// @brief Construct and destructively transfer real pairs through the owning allocator hooks.
template <typename Traits>
struct slot_ops<Traits, true> {
    using value_type = typename Traits::value_type;
    using slot_type = typename Traits::ivalue_type;
    using insert_type = value_type *;
    using allocator_type = typename Traits::_Alty;

    template <typename... Args>
    static void construct(slot_type *dst, allocator_type &al, Args &&...args) noexcept {
        uninitialized_construct_using_allocator(dst->raw(), al, std::forward<Args>(args)...);
    }

    static void assign(slot_type *dst, insert_type value, allocator_type &al) noexcept {
        construct(dst, al, std::move(*value));
    }

    static void destroy(slot_type *slot, allocator_type &al) noexcept {
        destroy_at_using_allocator(slot->get(), al);
    }

    /// @pre src and dst are distinct; dst contains no live value.
    static void transfer_one(slot_type *src, slot_type *dst, allocator_type &al) noexcept {
        uninitialized_relocate_at_using_allocator(src->get(), dst->raw(), al);
    }

    /// @pre Ranges are disjoint or dst precedes first; each destination is empty when reached.
    template <size_t Min, size_t Max>
    static void transfer(slot_type *first, slot_type *last, slot_type *dst,
                         allocator_type &al) noexcept {
        for (; first != last; ++first, ++dst)
            transfer_one(first, dst, al);
    }

    /// @pre Ranges are disjoint or dst_end follows last; each destination is empty when reached.
    template <size_t Min, size_t Max>
    static void transfer_backward(slot_type *first, slot_type *last, slot_type *dst_end,
                                  allocator_type &al) noexcept {
        while (first != last)
            transfer_one(--last, --dst_end, al);
    }
};

/// @brief Allocator-constructed insertion temporary; it does not allocate separate value storage.
template <typename Value, typename Alloc>
class compact_temporary {
public:
    template <typename... Args>
    compact_temporary(Alloc &al, Args &&...args) noexcept : m_allocator(al) {
        uninitialized_construct_using_allocator(m_slot.raw(), al, std::forward<Args>(args)...);
    }
    compact_temporary(const compact_temporary &) = delete;
    compact_temporary &operator=(const compact_temporary &) = delete;
    ~compact_temporary() noexcept { destroy_at_using_allocator(m_slot.get(), m_allocator); }
    Value *get() noexcept { return m_slot.get(); }

private:
    compact_slot<Value> m_slot;
    Alloc &m_allocator;
};

} // namespace wjr::btree_detail

#endif // WJR_CONTAINER_DETAIL_BTREE_SLOT_HPP__
