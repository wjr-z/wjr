/**
 * @file union_storage.hpp
 * @author wjr
 * @todo change to use enable_special_members_ctrl_base.
 * @version 0.1
 * @date 2024-07-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WJR_MEMORY_UNION_STORAGE_HPP__
#define WJR_MEMORY_UNION_STORAGE_HPP__

#include <wjr/crtp/class_base.hpp>

namespace wjr {

template <typename T, typename U, typename Tag>
using __union2_storage_enabler_select = enable_special_members_base<
    true, true,
    std::is_trivially_copy_constructible_v<T> &&
        std::is_trivially_copy_constructible_v<U>,
    std::is_trivially_move_constructible_v<T> &&
        std::is_trivially_move_constructible_v<U>,
    std::is_trivially_copy_assignable_v<T> && std::is_trivially_copy_assignable_v<U>,
    std::is_trivially_move_assignable_v<T> && std::is_trivially_move_assignable_v<U>,
    Tag>;

template <typename T, typename U, bool Constructor, bool Destructor>
class __union2_storage_base;

#define WJR_REGISTER_UNION_BASE(CON, DES)                                                \
    template <typename T, typename U>                                                    \
    class __union2_storage_base<T, U, CON, DES>                                          \
        : __union2_storage_enabler_select<T, U, __union2_storage_base<T, U, CON, DES>> { \
        using Mybase =                                                                   \
            __union2_storage_enabler_select<T, U,                                        \
                                            __union2_storage_base<T, U, CON, DES>>;      \
                                                                                         \
    public:                                                                              \
        constexpr __union2_storage_base() WJR_PP_BOOL_IF(CON, = default, noexcept {});   \
                                                                                         \
        template <typename... Args>                                                      \
        constexpr __union2_storage_base(                                                 \
            std::in_place_index_t<0>,                                                    \
            Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)     \
            : first(std::forward<Args>(args)...) {}                                      \
                                                                                         \
        template <typename... Args>                                                      \
        constexpr __union2_storage_base(                                                 \
            std::in_place_index_t<1>,                                                    \
            Args &&...args) noexcept(std::is_nothrow_constructible_v<U, Args...>)     \
            : second(std::forward<Args>(args)...) {}                                     \
                                                                                         \
        ~__union2_storage_base() WJR_PP_BOOL_IF(DES, = default, noexcept {});            \
                                                                                         \
        union {                                                                          \
            T first;                                                                     \
            U second;                                                                    \
        };                                                                               \
    }

WJR_REGISTER_UNION_BASE(0, 0);
WJR_REGISTER_UNION_BASE(0, 1);
WJR_REGISTER_UNION_BASE(1, 0);
WJR_REGISTER_UNION_BASE(1, 1);

#undef WJR_REGISTER_UNION_BASE

template <typename T, typename U>
using __union2_storage_base_select =
    __union2_storage_base<T, U,
                          std::is_trivially_default_constructible_v<T> &&
                              std::is_trivially_default_constructible_v<U>,
                          std::is_trivially_destructible_v<T> &&
                              std::is_trivially_destructible_v<U>>;

template <typename T, typename U>
class union2_storage : public __union2_storage_base_select<T, U> {
    using Mybase = __union2_storage_base_select<T, U>;

public:
    using Mybase::Mybase;
};

} // namespace wjr

#endif // WJR_MEMORY_UNION_STORAGE_HPP__