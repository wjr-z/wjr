#ifndef WJR_MEMORY_ALIGNED_STORAGE_HPP__
#define WJR_MEMORY_ALIGNED_STORAGE_HPP__

#include <wjr/crtp/class_base.hpp>

namespace wjr {

/// @private
template <typename T, typename Tag>
using __aligned_storage_checker_base_enabler_select =
    enable_special_members_base<true, true, std::is_trivially_copy_constructible_v<T>,
                                std::is_trivially_move_constructible_v<T>,
                                std::is_trivially_copy_assignable_v<T>,
                                std::is_trivially_move_assignable_v<T>, Tag>;

/// @private
template <typename T, bool Default, bool Destructor>
class __aligned_storage_base;

#define WJR_REGISTER_UNINITIALIZED_BASE(DEF, DES)                                        \
    template <typename T>                                                                \
    class __aligned_storage_base<T, DEF, DES>                                            \
        : __aligned_storage_checker_base_enabler_select<                                 \
              T, __aligned_storage_base<T, DEF, DES>> {                                  \
        using Mybase = __aligned_storage_checker_base_enabler_select<                    \
            T, __aligned_storage_base<T, DEF, DES>>;                                     \
                                                                                         \
    public:                                                                              \
        constexpr __aligned_storage_base() noexcept WJR_PP_BOOL_IF(DEF, = default,       \
                                                                   : m_storage(){});     \
                                                                                         \
        template <typename... Args,                                                      \
                  WJR_REQUIRES(std::is_constructible_v<T, Args &&...>)>                  \
        constexpr __aligned_storage_base(Args &&...args) noexcept(                       \
            std::is_nothrow_constructible_v<T, Args &&...>)                              \
            : m_value(std::forward<Args>(args)...) {}                                    \
                                                                                         \
        ~__aligned_storage_base() noexcept WJR_PP_BOOL_IF(DES, = default, {});           \
                                                                                         \
        constexpr T &operator*() & noexcept { return m_value; }                          \
        constexpr const T &operator*() const & noexcept { return m_value; }              \
        constexpr T &&operator*() && noexcept { return static_cast<T &&>(operator*()); } \
        constexpr const T &&operator*() const && noexcept {                              \
            return static_cast<const T &&>(operator*());                                 \
        }                                                                                \
                                                                                         \
        constexpr T *get() noexcept { return std::addressof(m_value); }                  \
        constexpr const T *get() const noexcept { return std::addressof(m_value); }      \
                                                                                         \
        constexpr T *operator->() noexcept { return get(); }                             \
        constexpr const T *operator->() const noexcept { return get(); }                 \
                                                                                         \
    private:                                                                             \
        union {                                                                          \
            T m_value;                                                                   \
            std::aligned_storage_t<sizeof(T), alignof(T)> m_storage;                     \
        };                                                                               \
    }

WJR_REGISTER_UNINITIALIZED_BASE(1, 1);
WJR_REGISTER_UNINITIALIZED_BASE(1, 0);
WJR_REGISTER_UNINITIALIZED_BASE(0, 1);
WJR_REGISTER_UNINITIALIZED_BASE(0, 0);

#undef WJR_REGISTER_UNINITIALIZED_BASE

/// @private
template <typename T>
class algined_storage
    : public __aligned_storage_base<T, std::is_trivially_default_constructible_v<T>,
                                    std::is_trivially_destructible_v<T>> {
    using Mybase = __aligned_storage_base<T, std::is_trivially_default_constructible_v<T>,
                                          std::is_trivially_destructible_v<T>>;

public:
    using Mybase::Mybase;
};

} // namespace wjr

#endif // WJR_MEMORY_ALIGNED_STORAGE_HPP__