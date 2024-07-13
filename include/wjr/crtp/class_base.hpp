#ifndef WJR_CRTP_CLASS_BASE_HPP__
#define WJR_CRTP_CLASS_BASE_HPP__

#include <cstddef>
#include <type_traits>

#include <wjr/preprocessor.hpp>

namespace wjr {

struct enable_default_constructor_t {
    constexpr explicit enable_default_constructor_t() = default;
};

inline constexpr enable_default_constructor_t enable_default_constructor{};

template <bool Enable, typename = void>
struct enable_default_constructor_base {
    constexpr enable_default_constructor_base() = default;
    constexpr enable_default_constructor_base(const enable_default_constructor_base &) =
        default;
    constexpr enable_default_constructor_base(enable_default_constructor_base &&) =
        default;
    constexpr enable_default_constructor_base &
    operator=(const enable_default_constructor_base &) = default;
    constexpr enable_default_constructor_base &
    operator=(enable_default_constructor_base &&) = default;

protected:
    constexpr explicit enable_default_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <typename Tag>
struct enable_default_constructor_base<false, Tag> {
    constexpr enable_default_constructor_base() noexcept = delete;
    constexpr enable_default_constructor_base(const enable_default_constructor_base &) =
        default;
    constexpr enable_default_constructor_base(enable_default_constructor_base &&) =
        default;
    constexpr enable_default_constructor_base &
    operator=(const enable_default_constructor_base &) = default;
    constexpr enable_default_constructor_base &
    operator=(enable_default_constructor_base &&) = default;

protected:
    constexpr explicit enable_default_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <bool Enable, typename Tag = void>
struct enable_destructor_base {
    constexpr enable_destructor_base() = default;
    constexpr enable_destructor_base(const enable_destructor_base &) = default;
    constexpr enable_destructor_base(enable_destructor_base &&) = default;
    constexpr enable_destructor_base &operator=(const enable_destructor_base &) = default;
    constexpr enable_destructor_base &operator=(enable_destructor_base &&) = default;
    ~enable_destructor_base() = default;

protected:
    constexpr explicit enable_destructor_base(enable_default_constructor_t) noexcept {}
};

template <typename Tag>
struct enable_destructor_base<false, Tag> {
    constexpr enable_destructor_base() = default;
    constexpr enable_destructor_base(const enable_destructor_base &) = default;
    constexpr enable_destructor_base(enable_destructor_base &&) = default;
    constexpr enable_destructor_base &operator=(const enable_destructor_base &) = default;
    constexpr enable_destructor_base &operator=(enable_destructor_base &&) = default;
    ~enable_destructor_base() noexcept = delete;

protected:
    constexpr explicit enable_destructor_base(enable_default_constructor_t) noexcept {}
};

template <bool Copy, bool Move, bool CopyAssign, bool MoveAssign, typename Tag = void>
struct enable_copy_move_base {
    constexpr enable_copy_move_base() = default;
    constexpr enable_copy_move_base(const enable_copy_move_base &) = default;
    constexpr enable_copy_move_base(enable_copy_move_base &&) = default;
    constexpr enable_copy_move_base &operator=(const enable_copy_move_base &) = default;
    constexpr enable_copy_move_base &operator=(enable_copy_move_base &&) = default;
    ~enable_copy_move_base() = default;

protected:
    constexpr explicit enable_copy_move_base(enable_default_constructor_t) noexcept {}
};

template <bool Default, bool Destructor, bool Copy, bool Move, bool CopyAssign,
          bool MoveAssign, typename Tag = void>
struct enable_special_members_base
    : public enable_copy_move_base<Copy, Move, CopyAssign, MoveAssign, Tag> {
    constexpr enable_special_members_base() = default;
    constexpr enable_special_members_base(const enable_special_members_base &) = default;
    constexpr enable_special_members_base(enable_special_members_base &&) = default;
    constexpr enable_special_members_base &
    operator=(const enable_special_members_base &) = default;
    constexpr enable_special_members_base &
    operator=(enable_special_members_base &&) = default;
    ~enable_special_members_base() = default;

protected:
    constexpr explicit enable_special_members_base(
        enable_default_constructor_t) noexcept {}
};

#define __WJR_ENABLE_BSAE_true default
#define __WJR_ENABLE_BSAE_false delete

#define WJR_REGISTER_ENABLE_COPY_MOVE_BASE(Copy, Move, CopyAssign, MoveAssign)           \
    template <typename Tag>                                                              \
    struct enable_copy_move_base<Copy, Move, CopyAssign, MoveAssign, Tag> {              \
        constexpr enable_copy_move_base() = default;                                     \
        constexpr enable_copy_move_base(const enable_copy_move_base &) noexcept =        \
            WJR_PP_CONCAT(__WJR_ENABLE_BSAE_, Copy);                                     \
        constexpr enable_copy_move_base(enable_copy_move_base &&) noexcept =             \
            WJR_PP_CONCAT(__WJR_ENABLE_BSAE_, Move);                                     \
        constexpr enable_copy_move_base &operator=(                                      \
            const enable_copy_move_base &) noexcept = WJR_PP_CONCAT(__WJR_ENABLE_BSAE_,  \
                                                                    CopyAssign);         \
        constexpr enable_copy_move_base &operator=(enable_copy_move_base &&) noexcept =  \
            WJR_PP_CONCAT(__WJR_ENABLE_BSAE_, MoveAssign);                               \
    }

WJR_REGISTER_ENABLE_COPY_MOVE_BASE(false, true, true, true);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(true, false, true, true);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(false, false, true, true);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(true, true, false, true);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(false, true, false, true);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(true, false, false, true);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(false, false, false, true);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(true, true, true, false);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(false, true, true, false);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(true, false, true, false);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(false, false, true, false);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(true, true, false, false);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(false, true, false, false);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(true, false, false, false);
WJR_REGISTER_ENABLE_COPY_MOVE_BASE(false, false, false, false);

#undef WJR_REGISTER_ENABLE_COPY_MOVE_BASE

#define WJR_REGISTER_ENABLE_SPECIAL_MEMBERS_BASE(Default, Destructor)                    \
    template <bool Copy, bool Move, bool CopyAssign, bool MoveAssign, typename Tag>      \
    struct enable_special_members_base<Default, Destructor, Copy, Move, CopyAssign,      \
                                       MoveAssign, Tag>                                  \
        : public enable_copy_move_base<Copy, Move, CopyAssign, MoveAssign, Tag> {        \
        constexpr enable_special_members_base() noexcept =                               \
            WJR_PP_CONCAT(__WJR_ENABLE_BSAE_, Default);                                  \
        constexpr enable_special_members_base(const enable_special_members_base &) =     \
            default;                                                                     \
        constexpr enable_special_members_base(enable_special_members_base &&) = default; \
        constexpr enable_special_members_base &                                          \
        operator=(const enable_special_members_base &) = default;                        \
        constexpr enable_special_members_base &                                          \
        operator=(enable_special_members_base &&) = default;                             \
        ~enable_special_members_base() noexcept = WJR_PP_CONCAT(__WJR_ENABLE_BSAE_,      \
                                                                Destructor);             \
                                                                                         \
    protected:                                                                           \
        constexpr explicit enable_special_members_base(                                  \
            enable_default_constructor_t) noexcept {}                                    \
    }

WJR_REGISTER_ENABLE_SPECIAL_MEMBERS_BASE(false, true);
WJR_REGISTER_ENABLE_SPECIAL_MEMBERS_BASE(true, false);
WJR_REGISTER_ENABLE_SPECIAL_MEMBERS_BASE(false, false);

#undef WJR_REGISTER_ENABLE_SPECIAL_MEMBERS_BASE

#undef __WJR_ENABLE_BSAE_false
#undef __WJR_ENABLE_BSAE_true

template <typename Tag = void>
using noncopyable = enable_copy_move_base<false, true, false, true, Tag>;

template <typename Tag = void>
using nonmoveable = enable_copy_move_base<false, true, false, true, Tag>;

template <typename Tag = void, typename... Args>
using enable_special_members_of_args_base = enable_special_members_base<
    std::conjunction_v<std::is_default_constructible<Args>...>,
    std::conjunction_v<std::is_destructible<Args>...>,
    std::conjunction_v<std::is_copy_constructible<Args>...>,
    std::conjunction_v<std::is_move_constructible<Args>...>,
    std::conjunction_v<std::is_copy_assignable<Args>...>,
    std::conjunction_v<std::is_move_assignable<Args>...>, Tag>;

template <typename Tag = void, typename... Args>
using enable_trivially_special_members_of_args_base = enable_special_members_base<
    std::conjunction_v<std::is_trivially_default_constructible<Args>...>,
    std::conjunction_v<std::is_destructible<Args>...>,
    std::conjunction_v<std::is_trivially_copy_constructible<Args>...>,
    std::conjunction_v<std::is_trivially_move_constructible<Args>...>,
    std::conjunction_v<std::is_trivially_copy_assignable<Args>...>,
    std::conjunction_v<std::is_trivially_move_assignable<Args>...>, Tag>;

template <size_t I, typename T>
struct enable_base_identity_t {};

} // namespace wjr

#endif // WJR_CRTP_CLASS_BASE_HPP__