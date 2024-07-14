/**
 * @file expected.hpp
 * @author wjr
 * @details
 * @version 0.1
 * @date 2024-07-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WJR_EXPECTED_HPP__
#define WJR_EXPECTED_HPP__

#include <wjr/memory/uninitialized.hpp>
#include <wjr/tuple.hpp>
#include <wjr/type_traits.hpp>
#include <wjr/variant.hpp>

namespace wjr {

struct unexpect_t {};

template <typename E, typename Tag>
using unexpected_base = enable_special_members_base<
    std::is_trivially_default_constructible_v<E>, std::is_destructible_v<E>,
    std::is_copy_constructible_v<E>, std::is_move_constructible_v<E>,
    std::is_trivially_copy_assignable_v<E>, std::is_trivially_move_assignable_v<E>, Tag>;

template <typename E>
class unexpected : unexpected_base<E, unexpected<E>> {
    using Mybase = unexpected_base<E, unexpected<E>>;

public:
    using Mybase::Mybase;

    template <typename Err = E,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<Err>, unexpected> &&
                           !std::is_same_v<remove_cvref_t<Err>, std::in_place_t> &&
                           std::is_constructible_v<E, Err &&>)>
    constexpr explicit unexpected(Err &&e) noexcept(
        std::is_nothrow_constructible_v<E, Err &&>)
        : m_err(std::forward<Err>(e)) {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<E, Args &&...>)>
    constexpr explicit unexpected(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args &&...>)
        : m_err(std::forward<Args>(args)...) {}

    template <
        typename U, typename... Args,
        WJR_REQUIRES(std::is_constructible_v<E, std::initializer_list<U> &, Args &&...>)>
    constexpr explicit unexpected(
        std::in_place_t, std::initializer_list<U> il,
        Args &&...args) noexcept(std::
                                     is_nothrow_constructible_v<
                                         E, std::initializer_list<U> &, Args &&...>)
        : m_err(il, std::forward<Args>(args)...) {}

    constexpr const E &error() const & noexcept { return m_err; }
    constexpr E &error() & noexcept { return m_err; }
    constexpr const E &&error() const && noexcept { return m_err; }
    constexpr E &&error() && noexcept { return m_err; }

    constexpr void swap(unexpected &other) noexcept(std::is_nothrow_swappable_v<E>) {
        std::swap(m_err, other.m_err);
    }

    template <typename E2>
    friend constexpr bool operator==(unexpected &x, unexpected<E2> &y) {
        return x.error() == y.error();
    }

    template <typename E2>
    friend constexpr bool operator!=(unexpected &x, unexpected<E2> &y) {
        return x.error() != y.error();
    }

private:
    E m_err;
};

template <typename Err>
unexpected(Err) -> unexpected<Err>;

template <typename T, typename E>
class __expected_storage_base
    : enable_special_members_of_args_base<__expected_storage_base<T, E>,
                                          union2_storage<T, E>> {
    using storage_type = union2_storage<T, E>;
    using Mybase =
        enable_special_members_of_args_base<__expected_storage_base<T, E>, storage_type>;

public:
    using Mybase::Mybase;

    template <typename... Args>
    constexpr __expected_storage_base(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args &&...>)
        : has_val(true), m_storage(std::in_place_index<0>, std::forward<Args>(args)...) {}

    template <typename... Args>
    constexpr __expected_storage_base(unexpect_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args &&...>)
        : has_val(false), m_storage(std::in_place_index<1>, std::forward<Args>(args)...) {
    }

protected:
    constexpr __expected_storage_base(in_place_empty_t, bool val) noexcept
        : has_val(val) {}

    template <typename U>
    constexpr void
    construct_value(U &&value) noexcept(std::is_nothrow_constructible_v<T, U &&>) {
        construct_at(std::addressof(m_storage.first), value);
    }

    template <typename U>
    constexpr void
    construct_error(U &&error) noexcept(std::is_nothrow_constructible_v<E, U &&>) {
        construct_at(std::addressof(m_storage.first), error);
    }

public:
    constexpr T &value() & noexcept { return m_storage.first; }
    constexpr const T &value() const & noexcept { return m_storage.first; }
    constexpr T &&value() && noexcept { return std::move(value()); }
    constexpr const T &&value() const && noexcept { return std::move(value()); }

    constexpr E &error() & noexcept { return m_storage.second; }
    constexpr const E &error() const & noexcept { return m_storage.second; }
    constexpr E &&error() && noexcept { return std::move(error()); }
    constexpr const E &&error() const && noexcept { return std::move(error()); }

protected:
    bool has_val;
    storage_type m_storage;
};

template <typename T, typename E, bool>
class __expected_storage_copy_ctor : protected __expected_storage_base<T, E> {
    using Mybase = __expected_storage_base<T, E>;

public:
    using Mybase::Mybase;
};

template <typename T, typename E>
class __expected_storage_copy_ctor<T, E, false>
    : protected __expected_storage_base<T, E> {
    using Mybase = __expected_storage_base<T, E>;

public:
    using Mybase::Mybase;

    constexpr __expected_storage_copy_ctor(
        const __expected_storage_copy_ctor
            &other) noexcept(std::is_nothrow_copy_constructible_v<T>
                                 &&std::is_nothrow_copy_constructible_v<E>)
        : Mybase(in_place_empty, other.has_val) {
        if (other.has_val) {
        } else {
            construct_at(std::addressof(Mybase::m_storage.second), other.error());
        }
    }
};

template <typename T, typename E>
using __expected_storage_copy_ctor_select = __expected_storage_copy_ctor<
    T, E,
    std::is_trivially_copy_constructible_v<__expected_storage_base<T, E>> ||
        (!std::is_copy_constructible_v<T> && !std::is_copy_constructible_v<E>)>;

template <typename T, typename E, bool>
class __expected_storage_move_ctor : protected __expected_storage_copy_ctor_select<T, E> {
    using Mybase = __expected_storage_copy_ctor_select<T, E>;

public:
    using Mybase::Mybase;
};

template <typename T, typename E>
class __expected_storage_move_ctor<T, E, false>
    : protected __expected_storage_copy_ctor_select<T, E> {
    using Mybase = __expected_storage_copy_ctor_select<T, E>;

public:
    using Mybase::Mybase;

    constexpr __expected_storage_move_ctor(__expected_storage_move_ctor &&other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&std::is_nothrow_move_constructible_v<E>)
        : Mybase(in_place_empty, other.has_val) {
        if (other.has_val) {
            construct_at(std::addressof(Mybase::m_storage.first),
                         std::move(other.value()));
        } else {
            construct_at(std::addressof(Mybase::m_storage.second),
                         std::move(other.error()));
        }
    }
};

template <typename T, typename E>
using __expected_storage_move_ctor_select = __expected_storage_move_ctor<
    T, E,
    std::is_trivially_move_constructible_v<__expected_storage_base<T, E>> ||
        (!std::is_move_constructible_v<T> && !std::is_move_constructible_v<E>)>;

template <typename NewType, typename OldType, typename... Args>
WJR_CONSTEXPR20 void reinit_expected(NewType &new_val, OldType &old_val, Args &&...args) {
    if constexpr (std::is_nothrow_constructible_v<NewType, Args...>) {
        std::destroy_at(std::addressof(old_val));
        construct_at(std::addressof(new_val), std::forward<Args>(args)...);
    } else if constexpr (std::is_nothrow_move_constructible_v<NewType>) {
        NewType temp(std::forward<Args>(args)...);
        std::destroy_at(std::addressof(old_val));
        construct_at(std::addressof(new_val), std::move(temp));
    } else {
        OldType temp(std::move(old_val));
        std::destroy_at(std::addressof(old_val));
        WJR_TRY { construct_at(std::addressof(new_val), std::forward<Args>(args)...); }
        WJR_CATCH(...) {
            construct_at(std::addressof(old_val), std::move(temp));
            throw;
        }
    }
}

template <typename T, typename E, bool>
class __expected_storage_copy_ass : protected __expected_storage_move_ctor_select<T, E> {
    using Mybase = __expected_storage_move_ctor_select<T, E>;

public:
    using Mybase::Mybase;
};

template <typename T, typename E>
class __expected_storage_copy_ass<T, E, false>
    : protected __expected_storage_move_ctor_select<T, E> {
    using Mybase = __expected_storage_move_ctor_select<T, E>;

public:
    using Mybase::Mybase;

    WJR_CONSTEXPR20 __expected_storage_copy_ass &
    operator=(const __expected_storage_copy_ass &other) noexcept(
        std::is_nothrow_copy_assignable_v<T> &&std::is_nothrow_copy_assignable_v<E>) {
        if (has_val) {
            if (other.has_val) {
                value() = other.value();
            } else {
                reinit_expected(error(), value(), other.error());
            }
        } else {
            if (!other.has_val) {
                error() = other.error();
            } else {
                reinit_expected(value(), error(), other.value());
            }
        }
    }
};

template <typename T, typename E>
using __expected_storage_copy_ass_select = __expected_storage_copy_ass<
    T, E,
    std::is_trivially_copy_assignable_v<__expected_storage_base<T, E>> ||
        (!std::is_copy_assignable_v<T> && !std::is_copy_assignable_v<E>)>;

template <typename T, typename E, bool>
class __expected_storage_move_ass : protected __expected_storage_copy_ass_select<T, E> {
    using Mybase = __expected_storage_copy_ass_select<T, E>;

public:
    using Mybase::Mybase;
};

template <typename T, typename E>
class __expected_storage_move_ass<T, E, false>
    : protected __expected_storage_copy_ass_select<T, E> {
    using Mybase = __expected_storage_copy_ass_select<T, E>;

public:
    using Mybase::Mybase;

    WJR_CONSTEXPR20 __expected_storage_move_ass &
    operator=(__expected_storage_move_ass &&other) noexcept(
        std::is_nothrow_copy_assignable_v<T> &&std::is_nothrow_copy_assignable_v<E>) {
        WJR_ASSERT(this != std::addressof(other));

        if (has_val) {
            if (other.has_val) {
                value() = std::move(other.value());
            } else {
                reinit_expected(error(), value(), std::move(other.error()));
            }
        } else {
            if (!other.has_val) {
                error() = std::move(other.error());
            } else {
                reinit_expected(value(), error(), std::move(other.value()));
            }
        }
    }
};

template <typename T, typename E>
using __expected_storage_move_ass_select = __expected_storage_move_ass<
    T, E,
    std::is_trivially_move_assignable_v<__expected_storage_base<T, E>> ||
        (!std::is_move_assignable_v<T> && !std::is_move_assignable_v<E>)>;

template <typename T, typename E>
class expected : __expected_storage_move_ass_select<T, E> {
    using Mybase = __expected_storage_move_ass_select<T, E>;

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    using Mybase::Mybase;

    template <typename _T = T, WJR_REQUIRES(std::is_default_constructible_v<T>)>
    constexpr expected() noexcept(std::is_nothrow_default_constructible_v<T>)
        : Mybase(std::in_place) {}

    constexpr bool has_value() const noexcept { return this->has_val; }
};

} // namespace wjr

#endif // WJR_EXPECTED_HPP__