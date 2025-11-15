/**
 * @file crtp/class_base.hpp
 * @brief CRTP base classes for controlling special member functions
 * @author wjr
 *
 * Provides base classes to selectively enable/disable default constructor,
 * copy/move constructors, and copy/move assignment operators using CRTP pattern.
 */

#ifndef WJR_CRTP_CLASS_BASE_HPP__
#define WJR_CRTP_CLASS_BASE_HPP__

#include <cstddef>
#include <type_traits>

#include <wjr/config.hpp>

namespace wjr {

struct enable_default_constructor_t {
    explicit enable_default_constructor_t() = default;
};

inline constexpr enable_default_constructor_t enable_default_constructor{};

template <bool Default, bool Destructor, bool Copy, bool Move, bool CopyAssign, bool MoveAssign,
          typename Tag = void>
struct WJR_EMPTY_BASES enable_special_members_base {
    enable_special_members_base()
    requires(Default)
    = default;

    enable_special_members_base()
    requires(!Default)
    = delete;

    enable_special_members_base(const enable_special_members_base &)
    requires(Copy)
    = default;

    enable_special_members_base(const enable_special_members_base &)
    requires(!Copy)
    = delete;

    enable_special_members_base(enable_special_members_base &&)
    requires(Move)
    = default;

    enable_special_members_base(enable_special_members_base &&)
    requires(!Move)
    = delete;

    enable_special_members_base &operator=(const enable_special_members_base &)
    requires(CopyAssign)
    = default;

    enable_special_members_base &operator=(const enable_special_members_base &)
    requires(!CopyAssign)
    = delete;

    enable_special_members_base &operator=(enable_special_members_base &&)
    requires(MoveAssign)
    = default;

    enable_special_members_base &operator=(enable_special_members_base &&)
    requires(!MoveAssign)
    = delete;

    ~enable_special_members_base()
    requires(Destructor)
    = default;

    ~enable_special_members_base()
    requires(!Destructor)
    = delete;

protected:
    constexpr explicit enable_special_members_base(enable_default_constructor_t) noexcept {}
};

template <bool Default, typename Tag = void>
using enable_default_constructor_base =
    enable_special_members_base<Default, true, true, true, true, true, Tag>;

template <bool Copy, bool Move, bool CopyAssign, bool MoveAssign, typename Tag = void>
using enable_copy_move_base =
    enable_special_members_base<true, true, Copy, Move, CopyAssign, MoveAssign, Tag>;

template <typename Tag = void>
using noncopyable = enable_copy_move_base<false, true, false, true, Tag>;

template <typename Tag = void>
using nonmoveable = enable_copy_move_base<false, true, false, true, Tag>;

template <typename Tag = void, typename... Args>
using enable_special_members_of_args_base =
    enable_special_members_base<std::conjunction_v<std::is_default_constructible<Args>...>,
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

template <typename _Mybase>
struct control_copy_ctor_base : _Mybase {
private:
    using Mybase = _Mybase;

public:
    using Mybase::Mybase;

    control_copy_ctor_base() = default;
    constexpr control_copy_ctor_base(const control_copy_ctor_base &other) noexcept(
        noexcept(std::declval<Mybase>()._copy_construct(std::declval<const Mybase &>())))
        : Mybase(enable_default_constructor) {
        Mybase::_copy_construct(static_cast<const Mybase &>(other));
    }
    control_copy_ctor_base(control_copy_ctor_base &&) = default;
    control_copy_ctor_base &operator=(const control_copy_ctor_base &) = default;
    control_copy_ctor_base &operator=(control_copy_ctor_base &&) = default;

protected:
    constexpr explicit control_copy_ctor_base(enable_default_constructor_t) noexcept
        : Mybase(enable_default_constructor) {}
};

template <typename _Mybase>
struct control_move_ctor_base : _Mybase {
private:
    using Mybase = _Mybase;

public:
    using Mybase::Mybase;

    control_move_ctor_base() = default;
    control_move_ctor_base(const control_move_ctor_base &) = default;
    constexpr control_move_ctor_base(control_move_ctor_base &&other) noexcept(
        noexcept(std::declval<Mybase>()._move_construct(std::declval<Mybase &&>())))
        : Mybase(enable_default_constructor) {
        Mybase::_move_construct(static_cast<Mybase &&>(other));
    }
    control_move_ctor_base &operator=(const control_move_ctor_base &) = default;
    control_move_ctor_base &operator=(control_move_ctor_base &&) = default;

protected:
    constexpr explicit control_move_ctor_base(enable_default_constructor_t) noexcept
        : Mybase(enable_default_constructor) {}
};

template <typename _Mybase>
struct control_copy_assign_base : _Mybase {
private:
    using Mybase = _Mybase;

public:
    using Mybase::Mybase;

    control_copy_assign_base() = default;
    control_copy_assign_base(const control_copy_assign_base &) = default;
    control_copy_assign_base(control_copy_assign_base &&) = default;
    constexpr control_copy_assign_base &operator=(const control_copy_assign_base &other) noexcept(
        noexcept(std::declval<Mybase>()._copy_assign(std::declval<const Mybase &>()))) {
        Mybase::_copy_assign(static_cast<const Mybase &>(other));
        return *this;
    }
    control_copy_assign_base &operator=(control_copy_assign_base &&) = default;

protected:
    constexpr explicit control_copy_assign_base(enable_default_constructor_t) noexcept
        : Mybase(enable_default_constructor) {}
};

template <typename _Mybase>
struct control_move_assign_base : _Mybase {
private:
    using Mybase = _Mybase;

public:
    using Mybase::Mybase;

    control_move_assign_base() = default;
    control_move_assign_base(const control_move_assign_base &) = default;
    control_move_assign_base(control_move_assign_base &&) = default;
    control_move_assign_base &operator=(const control_move_assign_base &) = default;
    constexpr control_move_assign_base &operator=(control_move_assign_base &&other) noexcept(
        noexcept(std::declval<Mybase>()._move_assign(std::declval<Mybase &&>()))) {
        Mybase::_move_assign(static_cast<Mybase &&>(other));
        return *this;
    }

protected:
    constexpr explicit control_move_assign_base(enable_default_constructor_t) noexcept
        : Mybase(enable_default_constructor) {}
};

template <bool F, template <typename> typename Control, typename Mybase>
using _control_base_selector = std::conditional_t<F, Mybase, Control<Mybase>>;

template <typename Mybase, bool Copy, bool Move, bool CopyAssign, bool MoveAssign>
using control_special_members_base = _control_base_selector<
    Copy, control_copy_ctor_base,
    _control_base_selector<
        Move, control_move_ctor_base,
        _control_base_selector<
            CopyAssign, control_copy_assign_base,
            _control_base_selector<MoveAssign, control_move_assign_base, Mybase>>>>;

} // namespace wjr

#endif // WJR_CRTP_CLASS_BASE_HPP__