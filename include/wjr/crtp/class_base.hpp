#ifndef WJR_CRTP_CLASS_BASE_HPP__
#define WJR_CRTP_CLASS_BASE_HPP__

#include <type_traits>

namespace wjr {

struct enable_default_constructor_t {
    constexpr explicit enable_default_constructor_t() noexcept = default;
};

inline constexpr enable_default_constructor_t enable_default_constructor{};

template <bool Enable, typename = void>
class enable_default_constructor_base {
protected:
    constexpr enable_default_constructor_base() noexcept = default;
    constexpr enable_default_constructor_base(
        const enable_default_constructor_base &) noexcept = default;
    constexpr enable_default_constructor_base(
        enable_default_constructor_base &&) noexcept = default;
    constexpr enable_default_constructor_base &
    operator=(const enable_default_constructor_base &) noexcept = default;
    constexpr enable_default_constructor_base &
    operator=(enable_default_constructor_base &&) noexcept = default;

    constexpr explicit enable_default_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <typename Tag>
class enable_default_constructor_base<false, Tag> {
protected:
    constexpr enable_default_constructor_base() noexcept = delete;
    constexpr enable_default_constructor_base(
        const enable_default_constructor_base &) noexcept = default;
    constexpr enable_default_constructor_base(
        enable_default_constructor_base &&) noexcept = default;
    constexpr enable_default_constructor_base &
    operator=(const enable_default_constructor_base &) noexcept = default;
    constexpr enable_default_constructor_base &
    operator=(enable_default_constructor_base &&) noexcept = default;

    constexpr explicit enable_default_constructor_base(
        enable_default_constructor_t) noexcept {}
};

template <bool Copy, bool Move, bool CopyAssign, bool MoveAssign, typename Tag = void>
class enable_copy_move_base {
protected:
    constexpr enable_copy_move_base() noexcept = default;
    constexpr enable_copy_move_base(const enable_copy_move_base &) noexcept = default;
    constexpr enable_copy_move_base(enable_copy_move_base &&) noexcept = default;
    constexpr enable_copy_move_base &
    operator=(const enable_copy_move_base &) noexcept = default;
    constexpr enable_copy_move_base &
    operator=(enable_copy_move_base &&) noexcept = default;
};

#define __WJR_ENABLE_COPY_MOVE_BASE_true default
#define __WJR_ENABLE_COPY_MOVE_BASE_false delete

#define WJR_REGISTER_ENABLE_COPY_MOVE_BASE(Copy, Move, CopyAssign, MoveAssign)           \
    template <typename Tag>                                                              \
    class enable_copy_move_base<Copy, Move, CopyAssign, MoveAssign, Tag> {               \
    protected:                                                                           \
        constexpr enable_copy_move_base() noexcept = default;                            \
        constexpr enable_copy_move_base(const enable_copy_move_base &) noexcept =        \
            WJR_PP_CONCAT(__WJR_ENABLE_COPY_MOVE_BASE, Copy);                            \
        constexpr enable_copy_move_base(enable_copy_move_base &&) noexcept =             \
            WJR_PP_CONCAT(__WJR_ENABLE_COPY_MOVE_BASE, Move);                            \
        constexpr enable_copy_move_base &                                                \
        operator=(const enable_copy_move_base &) noexcept =                              \
            WJR_PP_CONCAT(__WJR_ENABLE_COPY_MOVE_BASE, CopyAssign);                      \
        constexpr enable_copy_move_base &operator=(enable_copy_move_base &&) noexcept =  \
            WJR_PP_CONCAT(__WJR_ENABLE_COPY_MOVE_BASE, MoveAssign);                      \
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

#undef __WJR_ENABLE_COPY_MOVE_BASE_false
#undef __WJR_ENABLE_COPY_MOVE_BASE_true

} // namespace wjr

#endif // WJR_CRTP_CLASS_BASE_HPP__