#ifndef WJR_CRTP_CLASS_BASE_HPP__
#define WJR_CRTP_CLASS_BASE_HPP__

#include <cstddef>
#include <type_traits>

#include <wjr/preprocessor.hpp>

namespace wjr {

struct enable_default_constructor_t {
    explicit enable_default_constructor_t() = default;
};

inline constexpr enable_default_constructor_t enable_default_constructor{};

template <bool Enable, typename Tag>
struct enable_default_constructor_base {
    enable_default_constructor_base() = default;
    enable_default_constructor_base(const enable_default_constructor_base &) = default;
    enable_default_constructor_base(enable_default_constructor_base &&) = default;
    enable_default_constructor_base &
    operator=(const enable_default_constructor_base &) = default;
    enable_default_constructor_base &
    operator=(enable_default_constructor_base &&) = default;
    ~enable_default_constructor_base() = default;

protected:
    constexpr explicit enable_default_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <typename Tag>
struct enable_default_constructor_base<false, Tag> {
    enable_default_constructor_base() = delete;
    enable_default_constructor_base(const enable_default_constructor_base &) = default;
    enable_default_constructor_base(enable_default_constructor_base &&) = default;
    enable_default_constructor_base &
    operator=(const enable_default_constructor_base &) = default;
    enable_default_constructor_base &
    operator=(enable_default_constructor_base &&) = default;
    ~enable_default_constructor_base() = default;

protected:
    constexpr explicit enable_default_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <bool Enable, typename Tag>
struct enable_copy_constructor_base {
    enable_copy_constructor_base() = default;
    enable_copy_constructor_base(const enable_copy_constructor_base &) = default;
    enable_copy_constructor_base(enable_copy_constructor_base &&) = default;
    enable_copy_constructor_base &
    operator=(const enable_copy_constructor_base &) = default;
    enable_copy_constructor_base &operator=(enable_copy_constructor_base &&) = default;
    ~enable_copy_constructor_base() = default;

protected:
    constexpr explicit enable_copy_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <typename Tag>
struct enable_copy_constructor_base<false, Tag> {
    enable_copy_constructor_base() = default;
    enable_copy_constructor_base(const enable_copy_constructor_base &) = delete;
    enable_copy_constructor_base(enable_copy_constructor_base &&) = default;
    enable_copy_constructor_base &
    operator=(const enable_copy_constructor_base &) = default;
    enable_copy_constructor_base &operator=(enable_copy_constructor_base &&) = default;
    ~enable_copy_constructor_base() = default;

protected:
    constexpr explicit enable_copy_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <bool Enable, typename Tag>
struct enable_move_constructor_base {
    enable_move_constructor_base() = default;
    enable_move_constructor_base(const enable_move_constructor_base &) = default;
    enable_move_constructor_base(enable_move_constructor_base &&) = default;
    enable_move_constructor_base &
    operator=(const enable_move_constructor_base &) = default;
    enable_move_constructor_base &operator=(enable_move_constructor_base &&) = default;
    ~enable_move_constructor_base() = default;

protected:
    constexpr explicit enable_move_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <typename Tag>
struct enable_move_constructor_base<false, Tag> {
    enable_move_constructor_base() = default;
    enable_move_constructor_base(const enable_move_constructor_base &) = default;
    enable_move_constructor_base(enable_move_constructor_base &&) = delete;
    enable_move_constructor_base &
    operator=(const enable_move_constructor_base &) = default;
    enable_move_constructor_base &operator=(enable_move_constructor_base &&) = default;
    ~enable_move_constructor_base() = default;

protected:
    constexpr explicit enable_move_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <bool Enable, typename Tag>
struct enable_copy_assignment_base {
    enable_copy_assignment_base() = default;
    enable_copy_assignment_base(const enable_copy_assignment_base &) = default;
    enable_copy_assignment_base(enable_copy_assignment_base &&) = default;
    enable_copy_assignment_base &operator=(const enable_copy_assignment_base &) = default;
    enable_copy_assignment_base &operator=(enable_copy_assignment_base &&) = default;
    ~enable_copy_assignment_base() = default;

protected:
    constexpr explicit enable_copy_assignment_base(
        enable_default_constructor_t) noexcept {}
};

template <typename Tag>
struct enable_copy_assignment_base<false, Tag> {
    enable_copy_assignment_base() = default;
    enable_copy_assignment_base(const enable_copy_assignment_base &) = default;
    enable_copy_assignment_base(enable_copy_assignment_base &&) = default;
    enable_copy_assignment_base &operator=(const enable_copy_assignment_base &) = delete;
    enable_copy_assignment_base &operator=(enable_copy_assignment_base &&) = default;
    ~enable_copy_assignment_base() = default;

protected:
    constexpr explicit enable_copy_assignment_base(
        enable_default_constructor_t) noexcept {}
};

template <bool Enable, typename Tag>
struct enable_move_assignment_base {
    enable_move_assignment_base() = default;
    enable_move_assignment_base(const enable_move_assignment_base &) = default;
    enable_move_assignment_base(enable_move_assignment_base &&) = default;
    enable_move_assignment_base &operator=(const enable_move_assignment_base &) = default;
    enable_move_assignment_base &operator=(enable_move_assignment_base &&) = default;
    ~enable_move_assignment_base() = default;

protected:
    constexpr explicit enable_move_assignment_base(
        enable_default_constructor_t) noexcept {}
};

template <typename Tag>
struct enable_move_assignment_base<false, Tag> {
    enable_move_assignment_base() = default;
    enable_move_assignment_base(const enable_move_assignment_base &) = default;
    enable_move_assignment_base(enable_move_assignment_base &&) = default;
    enable_move_assignment_base &operator=(const enable_move_assignment_base &) = default;
    enable_move_assignment_base &operator=(enable_move_assignment_base &&) = delete;
    ~enable_move_assignment_base() = default;

protected:
    constexpr explicit enable_move_assignment_base(
        enable_default_constructor_t) noexcept {}
};

template <bool Enable, typename Tag>
struct enable_destructor_base {
    enable_destructor_base() = default;
    enable_destructor_base(const enable_destructor_base &) = default;
    enable_destructor_base(enable_destructor_base &&) = default;
    enable_destructor_base &operator=(const enable_destructor_base &) = default;
    enable_destructor_base &operator=(enable_destructor_base &&) = default;
    ~enable_destructor_base() = default;

protected:
    constexpr explicit enable_destructor_base(enable_default_constructor_t) noexcept {}
};

template <typename Tag>
struct enable_destructor_base<false, Tag> {
    enable_destructor_base() = default;
    enable_destructor_base(const enable_destructor_base &) = default;
    enable_destructor_base(enable_destructor_base &&) = default;
    enable_destructor_base &operator=(const enable_destructor_base &) = default;
    enable_destructor_base &operator=(enable_destructor_base &&) = default;
    ~enable_destructor_base() = delete;

protected:
    constexpr explicit enable_destructor_base(enable_default_constructor_t) noexcept {}
};

template <bool Default, bool Destructor, bool Copy, bool Move, bool CopyAssign,
          bool MoveAssign, typename Tag = void>
struct WJR_EMPTY_BASES enable_special_members_base
    : enable_default_constructor_base<Default, Tag>,
      enable_destructor_base<Destructor, Tag>,
      enable_copy_constructor_base<Copy, Tag>,
      enable_move_constructor_base<Move, Tag>,
      enable_copy_assignment_base<CopyAssign, Tag>,
      enable_move_assignment_base<MoveAssign, Tag> {

private:
    using Mybase = enable_default_constructor_base<Default, Tag>;

public:
    enable_special_members_base() = default;
    enable_special_members_base(const enable_special_members_base &) = default;
    enable_special_members_base(enable_special_members_base &&) = default;
    enable_special_members_base &operator=(const enable_special_members_base &) = default;
    enable_special_members_base &operator=(enable_special_members_base &&) = default;
    ~enable_special_members_base() = default;

protected:
    constexpr explicit enable_special_members_base(enable_default_constructor_t) noexcept
        : Mybase(enable_default_constructor) {}
};

template <bool Copy, bool Move, bool CopyAssign, bool MoveAssign, typename Tag = void>
using enable_copy_move_base =
    enable_special_members_base<true, true, Copy, Move, CopyAssign, MoveAssign, Tag>;

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