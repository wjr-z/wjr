#ifndef WJR_MEMORY_TEMPORARY_VALUE_ALLOCATOR_HPP__
#define WJR_MEMORY_TEMPORARY_VALUE_ALLOCATOR_HPP__

#include <wjr/memory/uninitialized.hpp>

namespace wjr {

template <typename Alloc>
class temporary_value_allocator {
public:
    using value_type = typename std::allocator_traits<Alloc>::value_type;
    using pointer = value_type *;
    using const_pointer = const value_type *;

    template <typename... Args>
    WJR_CONSTEXPR20 temporary_value_allocator(Alloc &al_, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<value_type, Args &&...>)
        : al(al_) {
        uninitialized_construct_using_allocator(get(), al, std::forward<Args>(args)...);
    }

    temporary_value_allocator(const temporary_value_allocator &) = delete;
    temporary_value_allocator(temporary_value_allocator &&) = delete;
    temporary_value_allocator &operator=(const temporary_value_allocator &) = delete;
    temporary_value_allocator &operator=(temporary_value_allocator &&) = delete;

    ~temporary_value_allocator() noexcept(noexcept(
        destroy_at_using_allocator(std::declval<pointer>(), std::declval<Alloc &>()))) {
        destroy_at_using_allocator(get(), al);
    }

    WJR_CONSTEXPR20 pointer get() noexcept { return reinterpret_cast<pointer>(storage); }
    WJR_CONSTEXPR20 const_pointer get() const noexcept {
        return reinterpret_cast<const_pointer>(storage);
    }

private:
    Alloc &al;
    std::aligned_storage_t<sizeof(value_type), alignof(value_type)> storage[1];
};

template <typename Alloc, typename... Args>
temporary_value_allocator(Alloc &, Args &&...) -> temporary_value_allocator<Alloc>;

} // namespace wjr

#endif // WJR_MEMORY_TEMPORARY_VALUE_ALLOCATOR_HPP__