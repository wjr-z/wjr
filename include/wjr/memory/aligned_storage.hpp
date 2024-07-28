#ifndef WJR_MEMORY_ALIGNED_STORAGE_HPP__
#define WJR_MEMORY_ALIGNED_STORAGE_HPP__

#include <wjr/memory/union_storage.hpp>

namespace wjr {

template <typename T>
class aligned_storage : union2_storage<T, std::aligned_storage_t<sizeof(T), alignof(T)>> {
    using Mybase = union2_storage<T, std::aligned_storage_t<sizeof(T), alignof(T)>>;

public:
    using Mybase::Mybase;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(aligned_storage);

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr aligned_storage(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : Mybase(std::in_place_index<0>, std::forward<Args>(args)...) {}

    constexpr T &operator*() & noexcept { return Mybase::first; }
    constexpr const T &operator*() const & noexcept { return Mybase::first; }
    constexpr T &&operator*() && noexcept { return static_cast<T &&>(operator*()); }
    constexpr const T &&operator*() const && noexcept {
        return static_cast<const T &&>(operator*());
    }

    constexpr T *get() noexcept { return std::addressof(Mybase::first); }
    constexpr const T *get() const noexcept { return std::addressof(Mybase::first); }

    constexpr T *operator->() noexcept { return get(); }
    constexpr const T *operator->() const noexcept { return get(); }
};

} // namespace wjr

#endif // WJR_MEMORY_ALIGNED_STORAGE_HPP__