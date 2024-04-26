#ifndef WJR_MEMORY_ALIGNED_ALLOCATOR_HPP
#define WJR_MEMORY_ALIGNED_ALLOCATOR_HPP

#include <wjr/capture_leaf.hpp>
#include <wjr/crtp/trivially_allocator_base.hpp>
#include <wjr/math/details.hpp>
#include <wjr/memory/details.hpp>

namespace wjr {

namespace {
WJR_REGISTER_HAS_TYPE(aligned_allocate,
                      std::declval<Alloc>().aligned_alocate(std::declval<Size>(),
                                                            std::declval<size_t>()),
                      Alloc, Size);
WJR_REGISTER_HAS_TYPE(aligned_allocate_at_least,
                      std::declval<Alloc>().aligned_alocate_at_least(
                          std::declval<Size>(), std::declval<size_t>()),
                      Alloc, Size);
} // namespace

template <typename Alloc, size_t alignment>
class aligned_allocator;

template <typename Alloc, size_t alignment>
using aligned_allocator_base = std::conditional_t<
    is_compressed_v<Alloc>,
    compressed_capture_leaf<Alloc, aligned_allocator<Alloc, alignment>>,
    capture_leaf<Alloc, aligned_allocator<Alloc, alignment>>>;

template <typename Alloc>
using __aligned_allocator_alloc =
    typename std::allocator_traits<Alloc>::template rebind_alloc<uint8_t>;

template <typename Alloc, size_t alignment>
class aligned_allocator
    : aligned_allocator_base<__aligned_allocator_alloc<Alloc>, alignment> {
    using Alty = __aligned_allocator_alloc<Alloc>;
    using Mybase = aligned_allocator_base<Alty, alignment>;
    using Trivially_traits = trivially_allocator_traits<Alty>;

    using Alty_traits = std::allocator_traits<Alloc>;

public:
    template <typename U>
    struct rebind {
        using other =
            aligned_allocator<typename Alty_traits::template rebind_alloc<U>, alignment>;
    };

    using value_type = typename Alty_traits::value_type;
    using pointer = typename Alty_traits::pointer;
    using const_pointer = typename Alty_traits::const_pointer;
    using size_type = typename Alty_traits::size_type;
    using difference_type = typename Alty_traits::difference_type;
    using is_always_equal = typename Alty_traits::is_always_equal;
    using is_trivially_allocator = typename Trivially_traits::is_trivially;
    using is_trivially_allocator_constructible =
        typename Trivially_traits::is_trivially_constructible;
    using is_trivially_allocator_destructible =
        typename Trivially_traits::is_trivially_destructible;

    static_assert(std::is_pointer_v<pointer>, "pointer must be a pointer type.");
    static_assert(sizeof(typename std::allocator_traits<Alty>::value_type) == 1, "");
    static_assert(alignment > 0 && alignment < 256, "alignment must be in [1, 255].");

    WJR_CONSTEXPR20 aligned_allocator() noexcept(
        std::is_nothrow_default_constructible_v<Mybase>) = default;
    WJR_CONSTEXPR20 aligned_allocator(const aligned_allocator &) noexcept(
        std::is_nothrow_copy_constructible_v<Mybase>) = default;
    template <typename U, size_t alignment2>
    WJR_CONSTEXPR20 aligned_allocator(
        const aligned_allocator<U, alignment2> &other) noexcept(noexcept(Mybase(other)))
        : Mybase(other) {}

    ~aligned_allocator() = default;

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<Alty, Args &&...>)>
    WJR_CONSTEXPR20 aligned_allocator(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<Alty, Args &&...>)
        : Mybase(std::forward<Args>(args)...) {}

    WJR_CONSTEXPR20 WJR_ALIGNED(alignment) pointer allocate(size_type n) {
        if constexpr (alignment == 1) {
            return reinterpret_cast<pointer>(base().allocate(n * sizeof(value_type)));
        } else if constexpr (has_aligned_allocate_v<Alty, size_type>) {
            return reinterpret_cast<pointer>(
                base().aligned_allocate(n * sizeof(value_type), alignment));
        } else {
            const uintptr_t mem = reinterpret_cast<uintptr_t>(
                base().allocate(n * sizeof(value_type) + alignment));
            const uint8_t offset = alignment - __align_down_offset(mem, alignment);
            uint8_t *const u8ptr = reinterpret_cast<uint8_t *>(mem + offset);
            u8ptr[-1] = offset;
            return reinterpret_cast<pointer>(u8ptr);
        }
    }

    WJR_CONSTEXPR20 allocation_result<pointer, size_type> allocate_at_least(size_type n) {
        if constexpr (alignment == 1) {
            return wjr::allocate_at_least(base(), n * sizeof(value_type));
        } else if constexpr (has_aligned_allocate_at_least_v<Alty, size_type>) {
            return reinterpret_cast<pointer>(
                base().aligned_allocate_at_least(n, alignment));
        } else {
            const auto alloc =
                wjr::allocate_at_least(base(), n * sizeof(value_type) + alignment);
            const uintptr_t mem = reinterpret_cast<uintptr_t>(alloc.ptr);
            const uint8_t offset = alignment - __align_down_offset(mem, alignment);
            uint8_t *const u8ptr = reinterpret_cast<uint8_t *>(mem + offset);
            u8ptr[-1] = offset;
            return {reinterpret_cast<pointer>(u8ptr),
                    (alloc.count - offset) / sizeof(value_type)};
        }
    }

    WJR_CONSTEXPR20 void deallocate(pointer ptr, size_type n) {
        if constexpr (alignment == 1) {
            base().deallocate(reinterpret_cast<uint8_t *>(ptr), n * sizeof(value_type));
        } else if constexpr (has_aligned_allocate_v<Alty, size_type>) {
            base().aligned_deallocate(reinterpret_cast<uint8_t *>(ptr),
                                      n * sizeof(value_type), alignment);
        } else {
            uint8_t *const u8ptr = reinterpret_cast<uint8_t *>(ptr);
            const uint8_t offset = u8ptr[-1];
            base().deallocate(u8ptr - offset, n * sizeof(value_type) + alignment);
        }
    }

protected:
    WJR_CONSTEXPR20 Alty &base() noexcept { return Mybase::get(); }
    WJR_CONSTEXPR20 const Alty &base() const noexcept { return Mybase::get(); }
};

} // namespace wjr

#endif // WJR_MEMORY_ALIGNED_ALLOCATOR_HPP