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

#include <exception>

#include <wjr/assert.hpp>
#include <wjr/crtp/class_base.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

struct unexpect_t {};

inline constexpr unexpect_t unexpect = {};

template <typename E, typename Tag>
using unexpected_base = enable_special_members_base<
    std::is_trivially_default_constructible_v<E>, std::is_destructible_v<E>,
    std::is_copy_constructible_v<E>, std::is_move_constructible_v<E>,
    std::is_copy_assignable_v<E>, std::is_move_assignable_v<E>, Tag>;

template <typename E>
class unexpected : unexpected_base<E, unexpected<E>> {
    using Mybase = unexpected_base<E, unexpected<E>>;

public:
    using Mybase::Mybase;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(unexpected);

    template <typename Err = E,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<Err>, unexpected> &&
                           !std::is_same_v<remove_cvref_t<Err>, std::in_place_t> &&
                           std::is_constructible_v<E, Err &&>)>
    constexpr explicit unexpected(Err &&e) noexcept(
        std::is_nothrow_constructible_v<E, Err &&>)
        : Mybase(enable_default_constructor), m_err(std::forward<Err>(e)) {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<E, Args...>)>
    constexpr explicit unexpected(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>)
        : Mybase(enable_default_constructor), m_err(std::forward<Args>(args)...) {}

    template <
        typename U, typename... Args,
        WJR_REQUIRES(std::is_constructible_v<E, std::initializer_list<U> &, Args...>)>
    constexpr explicit unexpected(
        std::in_place_t, std::initializer_list<U> il,
        Args &&...args) noexcept(std::is_constructible_v<E, std::initializer_list<U> &,
                                                         Args...>)
        : Mybase(enable_default_constructor), m_err(il, std::forward<Args>(args)...) {}

    constexpr E &error() & noexcept { return m_err; }
    constexpr const E &error() const & noexcept { return m_err; }
    constexpr E &&error() && noexcept { return std::move(m_err); }
    constexpr const E &&error() const && noexcept { return std::move(m_err); }

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

template <typename E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public std::exception {
public:
    WJR_NODISCARD virtual const char *what() const noexcept override {
        return "Bad expected access";
    }
};

template <typename E>
class bad_expected_access : public bad_expected_access<void> {
public:
    explicit bad_expected_access(E e) : m_val(std::move(e)) {}

    const E &error() const & { return m_val; }
    E &error() & { return m_val; }
    const E &&error() const && { return std::move(m_val); }
    E &&error() && { return std::move(m_val); }

private:
    E m_val;
};

template <typename E, E init>
class compressed_unexpected {
    static_assert(std::is_trivial_v<E>, "Only support trivial type currently.");

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(compressed_unexpected);

    constexpr compressed_unexpected(E err) noexcept : m_err(err) {
        WJR_ASSERT(err != init);
    }

    constexpr operator E() const noexcept { return m_err; }

private:
    E m_err;
};

template <typename E>
struct __expected_error_type {
    using type = E;
};

template <typename E, E init>
struct __expected_error_type<compressed_unexpected<E, init>> {
    using type = E;
};

template <typename E>
using __expected_error_type_t = typename __expected_error_type<E>::type;

template <typename T, typename E>
class expected;

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
            WJR_XTHROW;
        }
    }
}

namespace expected_detail {

template <typename T, typename U>
struct is_void_or : U {};

template <typename U>
struct is_void_or<void, U> : std::true_type {};

template <typename T, typename U>
inline constexpr bool is_void_or_v = is_void_or<T, U>::value;

template <typename T, typename E,
          bool Destructor = is_void_or_v<T, std::is_trivially_destructible<T>>
              &&std::is_trivially_destructible_v<__expected_error_type_t<E>>>
struct expected_storage_base {
    constexpr expected_storage_base() noexcept(std::is_nothrow_default_constructible_v<T>)
        : m_has_val(true), m_val() {}

    template <typename... Args>
    constexpr expected_storage_base(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : m_has_val(true), m_val(std::forward<Args>(args)...) {}

    template <typename... Args>
    constexpr expected_storage_base(unexpect_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>)
        : m_has_val(false), m_err(std::forward<Args>(args)...) {}

    expected_storage_base(const expected_storage_base &) = default;
    expected_storage_base(expected_storage_base &&) = default;
    expected_storage_base &operator=(const expected_storage_base &) = default;
    expected_storage_base &operator=(expected_storage_base &&) = default;

    constexpr expected_storage_base(enable_default_constructor_t) noexcept {}

    ~expected_storage_base() = default;

    WJR_PURE constexpr bool has_value() const noexcept { return m_has_val; }
    constexpr void set_valid() noexcept { m_has_val = true; }
    constexpr void set_invalid() noexcept { m_has_val = false; }

    bool m_has_val;
    union {
        T m_val;
        E m_err;
    };
};

template <typename T, typename E>
struct expected_storage_base<T, E, false> {
    constexpr expected_storage_base() noexcept(std::is_nothrow_default_constructible_v<T>)
        : m_has_val(true), m_val() {}

    template <typename... Args>
    constexpr expected_storage_base(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : m_has_val(true), m_val(std::forward<Args>(args)...) {}

    template <typename... Args>
    constexpr expected_storage_base(unexpect_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>)
        : m_has_val(false), m_err(std::forward<Args>(args)...) {}

    expected_storage_base(const expected_storage_base &) = default;
    expected_storage_base(expected_storage_base &&) = default;
    expected_storage_base &operator=(const expected_storage_base &) = default;
    expected_storage_base &operator=(expected_storage_base &&) = default;

    constexpr expected_storage_base(enable_default_constructor_t) noexcept {}

    ~expected_storage_base() noexcept(
        std::is_nothrow_destructible_v<T> &&std::is_nothrow_destructible_v<E>) {
        if (this->m_has_val) {
            std::destroy_at(std::addressof(this->m_val));
        } else {
            std::destroy_at(std::addressof(this->m_err));
        }
    }

    WJR_PURE constexpr bool has_value() const noexcept { return m_has_val; }
    constexpr void set_valid() noexcept { m_has_val = true; }
    constexpr void set_invalid() noexcept { m_has_val = false; }

    bool m_has_val;
    union {
        T m_val;
        E m_err;
    };
};

template <typename E>
struct expected_storage_base<void, E, true> {
    constexpr expected_storage_base() noexcept : m_has_val(true) {}

    constexpr expected_storage_base(std::in_place_t) noexcept : m_has_val(true) {}

    template <typename... Args>
    constexpr expected_storage_base(unexpect_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>)
        : m_has_val(false), m_err(std::forward<Args>(args)...) {}

    expected_storage_base(const expected_storage_base &) = default;
    expected_storage_base(expected_storage_base &&) = default;
    expected_storage_base &operator=(const expected_storage_base &) = default;
    expected_storage_base &operator=(expected_storage_base &&) = default;

    constexpr expected_storage_base(enable_default_constructor_t) noexcept {}

    ~expected_storage_base() = default;

    WJR_PURE constexpr bool has_value() const noexcept { return m_has_val; }
    constexpr void set_valid() noexcept { m_has_val = true; }
    constexpr void set_invalid() noexcept { m_has_val = false; }

    bool m_has_val;
    union {
        E m_err;
    };
};

template <typename E>
struct expected_storage_base<void, E, false> {
    constexpr expected_storage_base() noexcept : m_has_val(true) {}

    constexpr expected_storage_base(std::in_place_t) noexcept : m_has_val(true) {}

    template <typename... Args>
    constexpr expected_storage_base(unexpect_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>)
        : m_has_val(false), m_err(std::forward<Args>(args)...) {}

    expected_storage_base(const expected_storage_base &) = default;
    expected_storage_base(expected_storage_base &&) = default;
    expected_storage_base &operator=(const expected_storage_base &) = default;
    expected_storage_base &operator=(expected_storage_base &&) = default;

    constexpr expected_storage_base(enable_default_constructor_t) noexcept {}

    ~expected_storage_base() noexcept(std::is_nothrow_destructible_v<E>) {
        if (!this->m_has_val) {
            std::destroy_at(std::addressof(this->m_err));
        }
    }

    WJR_PURE constexpr bool has_value() const noexcept { return m_has_val; }
    constexpr void set_valid() noexcept { m_has_val = true; }
    constexpr void set_invalid() noexcept { m_has_val = false; }

    bool m_has_val;
    union {
        E m_err;
    };
};

template <typename T, typename E, E init>
struct expected_storage_base<T, compressed_unexpected<E, init>, true> {
    constexpr expected_storage_base() noexcept(std::is_nothrow_default_constructible_v<T>)
        : m_val(), m_err(init) {}

    template <typename... Args>
    constexpr expected_storage_base(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : m_val(std::forward<Args>(args)...), m_err(init) {}

    template <typename... Args>
    constexpr expected_storage_base(unexpect_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>)
        : m_err(std::forward<Args>(args)...) {}

    expected_storage_base(const expected_storage_base &) = default;
    expected_storage_base(expected_storage_base &&) = default;
    expected_storage_base &operator=(const expected_storage_base &) = default;
    expected_storage_base &operator=(expected_storage_base &&) = default;

    constexpr expected_storage_base(enable_default_constructor_t) noexcept {}

    ~expected_storage_base() = default;

    WJR_PURE constexpr bool has_value() const noexcept { return m_err == init; }
    constexpr void set_valid() noexcept { m_err = init; }
    constexpr void set_invalid() noexcept {}

    union {
        T m_val;
    };

    E m_err;
};

template <typename T, typename E, E init>
struct expected_storage_base<T, compressed_unexpected<E, init>, false> {
    constexpr expected_storage_base() noexcept(std::is_nothrow_default_constructible_v<T>)
        : m_val(), m_err(init) {}

    template <typename... Args>
    constexpr expected_storage_base(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : m_val(std::forward<Args>(args)...), m_err(init) {}

    template <typename... Args>
    constexpr expected_storage_base(unexpect_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>)
        : m_err(std::forward<Args>(args)...) {}

    expected_storage_base(const expected_storage_base &) = default;
    expected_storage_base(expected_storage_base &&) = default;
    expected_storage_base &operator=(const expected_storage_base &) = default;
    expected_storage_base &operator=(expected_storage_base &&) = default;

    constexpr expected_storage_base(enable_default_constructor_t) noexcept {}

    ~expected_storage_base() noexcept(
        std::is_nothrow_destructible_v<T> &&std::is_nothrow_destructible_v<E>) {
        if (this->has_value()) {
            std::destroy_at(std::addressof(this->m_val));
        } else {
            std::destroy_at(std::addressof(this->m_err));
        }
    }

    WJR_PURE constexpr bool has_value() const noexcept { return m_err == init; }
    constexpr void set_valid() noexcept { m_err = init; }
    constexpr void set_invalid() noexcept {}

    union {
        T m_val;
    };

    E m_err;
};

template <typename E, E init>
struct expected_storage_base<void, compressed_unexpected<E, init>, true> {
    constexpr expected_storage_base() noexcept : m_err(init) {}

    constexpr expected_storage_base(std::in_place_t) noexcept : m_err(init) {}

    template <typename... Args>
    constexpr expected_storage_base(unexpect_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>)
        : m_err(std::forward<Args>(args)...) {}

    expected_storage_base(const expected_storage_base &) = default;
    expected_storage_base(expected_storage_base &&) = default;
    expected_storage_base &operator=(const expected_storage_base &) = default;
    expected_storage_base &operator=(expected_storage_base &&) = default;

    constexpr expected_storage_base(enable_default_constructor_t) noexcept {}

    ~expected_storage_base() = default;

    WJR_PURE constexpr bool has_value() const noexcept { return m_err == init; }
    constexpr void set_valid() noexcept { m_err = init; }
    constexpr void set_invalid() noexcept {}

    E m_err;
};

template <typename T, typename E>
struct expected_operations_base : expected_storage_base<T, E> {
    using Mybase = expected_storage_base<T, E>;
    using Mybase::Mybase;

    using error_type = __expected_error_type_t<E>;

    template <typename... Args>
    constexpr void construct_value(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>) {
        this->set_valid();
        construct_at(std::addressof(this->m_val), std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void construct_error(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<error_type, Args...>) {
        this->set_invalid();
        construct_at(std::addressof(this->m_err), std::forward<Args>(args)...);
    }

    WJR_CONSTEXPR20 void __copy_construct(const expected_operations_base &other) {
        if (other.has_value()) {
            construct_value(other.m_val);
        } else {
            construct_error(other.m_err);
        }
    }

    WJR_CONSTEXPR20 void __move_construct(expected_operations_base &&other) noexcept(
        std::is_nothrow_move_constructible_v<T>
            &&std::is_nothrow_move_constructible_v<error_type>) {
        if (other.has_value()) {
            construct_value(std::move(other.m_val));
        } else {
            construct_error(std::move(other.m_err));
        }
    }

    WJR_CONSTEXPR20 void __copy_assign(const expected_operations_base &other) {
        if (this->has_value()) {
            if (WJR_LIKELY(other.has_value())) {
                this->m_val = other.m_val;
            } else {
                reinit_expected(this->m_err, this->m_val, other.m_err);
                this->set_invalid();
            }
        } else {
            if (WJR_LIKELY(!other.has_value())) {
                this->m_err = other.m_err;
            } else {
                reinit_expected(this->m_val, this->m_err, other.m_val);
                this->set_valid();
            }
        }
    }

    WJR_CONSTEXPR20 void __move_assign(expected_operations_base &&other) noexcept(
        std::is_nothrow_move_assignable_v<T> &&std::is_nothrow_move_assignable_v<E>) {
        if (this->has_value()) {
            if (WJR_LIKELY(other.has_value())) {
                this->m_val = std::move(other.m_val);
            } else {
                reinit_expected(this->m_err, this->m_val, std::move(other.m_err));
                this->set_invalid();
            }
        } else {
            if (WJR_LIKELY(!other.has_value())) {
                this->m_err = std::move(other.m_err);
            } else {
                reinit_expected(this->m_val, this->m_err, std::move(other.m_val));
                this->set_valid();
            }
        }
    }
};

template <typename E>
struct expected_operations_base<void, E> : expected_storage_base<void, E> {
    using Mybase = expected_storage_base<void, E>;
    using Mybase::Mybase;

    using error_type = __expected_error_type_t<E>;

    template <typename... Args>
    constexpr void construct_error(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<error_type, Args...>) {
        this->set_invalid();
        construct_at(std::addressof(this->m_err), std::forward<Args>(args)...);
    }

    WJR_CONSTEXPR20 void __copy_construct(const expected_operations_base &other) {
        if (!other.has_value()) {
            construct_error(other.m_err);
        }
    }

    WJR_CONSTEXPR20 void __move_construct(expected_operations_base &&other) noexcept(
        std::is_nothrow_move_constructible_v<error_type>) {
        if (!other.has_value()) {
            construct_error(std::move(other.m_err));
        }
    }

    WJR_CONSTEXPR20 void __copy_assign(const expected_operations_base &other) {
        if (this->has_value()) {
            if (!other.has_value()) {
                construct_at(std::addressof(this->m_err), other.m_err);
                this->set_invalid();
            }
        } else {
            if (WJR_LIKELY(!other.has_value())) {
                this->m_err = other.m_err;
            } else {
                std::destroy_at(std::addressof(this->m_err));
                this->set_valid();
            }
        }
    }

    WJR_CONSTEXPR20 void __move_assign(expected_operations_base &&other) noexcept(
        std::is_nothrow_move_assignable_v<error_type>) {
        if (this->has_value()) {
            if (!other.has_value()) {
                construct_at(std::addressof(this->m_err), std::move(other.m_err));
                this->set_invalid();
            }
        } else {
            if (WJR_LIKELY(!other.has_value())) {
                this->m_err = std::move(other.m_err);
            } else {
                std::destroy_at(std::addressof(this->m_err));
                this->set_valid();
            }
        }
    }
};

template <typename T, typename E>
struct __expected_storage_impl {
    using error_type = __expected_error_type_t<E>;

    using type = control_special_members_base<
        expected_operations_base<T, E>,
        std::is_trivially_copy_constructible_v<T> &&
            std::is_trivially_copy_constructible_v<error_type>,
        std::is_trivially_move_constructible_v<T> &&
            std::is_trivially_move_constructible_v<error_type>,
        std::is_trivially_copy_assignable_v<T> &&
            std::is_trivially_copy_constructible_v<T> &&
            std::is_trivially_destructible_v<T> &&
            std::is_trivially_copy_assignable_v<error_type> &&
            std::is_trivially_copy_constructible_v<error_type> &&
            std::is_trivially_destructible_v<error_type>,
        std::is_trivially_move_assignable_v<T> &&
            std::is_trivially_move_constructible_v<T> &&
            std::is_trivially_destructible_v<T> &&
            std::is_trivially_move_assignable_v<error_type> &&
            std::is_trivially_move_constructible_v<error_type> &&
            std::is_trivially_destructible_v<error_type>>;
};

template <typename E>
struct __expected_storage_impl<void, E> {
    using error_type = __expected_error_type_t<E>;

    using type = control_special_members_base<
        expected_operations_base<void, E>,
        std::is_trivially_copy_constructible_v<error_type>,
        std::is_trivially_move_constructible_v<error_type>,
        std::is_trivially_copy_assignable_v<error_type> &&
            std::is_trivially_copy_constructible_v<error_type> &&
            std::is_trivially_destructible_v<error_type>,
        std::is_trivially_move_assignable_v<error_type> &&
            std::is_trivially_move_constructible_v<error_type> &&
            std::is_trivially_destructible_v<error_type>>;
};

template <typename T, typename E>
using expected_storage = typename __expected_storage_impl<T, E>::type;

template <typename T, typename E>
struct __enable_expeted_storage_impl {
    using type = enable_special_members_base<
        std::is_default_constructible_v<T>, true,
        std::is_copy_constructible_v<T> && std::is_copy_constructible_v<E>,
        std::is_move_constructible_v<T> && std::is_move_constructible_v<E>,
        std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T> &&
            std::is_copy_assignable_v<E> && std::is_copy_constructible_v<E> &&
            (std::is_nothrow_move_constructible_v<T> ||
             std::is_nothrow_move_constructible_v<E>),
        std::is_move_assignable_v<T> && std::is_move_constructible_v<T> &&
            std::is_move_assignable_v<E> && std::is_move_constructible_v<E> &&
            (std::is_nothrow_move_constructible_v<T> ||
             std::is_nothrow_move_constructible_v<E>)>;
};

template <typename E>
struct __enable_expeted_storage_impl<void, E> {
    using type = enable_special_members_base<
        true, true, std::is_copy_constructible_v<E>, std::is_move_constructible_v<E>,
        std::is_copy_assignable_v<E> && std::is_copy_constructible_v<E>,
        std::is_move_assignable_v<E> && std::is_move_constructible_v<E>>;
};

template <typename T, typename E>
using enable_expected_storage = typename __enable_expeted_storage_impl<T, E>::type;

template <typename T, typename E, typename U, typename G>
struct expected_construct_with
    : std::conjunction<
          std::disjunction<std::is_same<std::remove_cv_t<T>, bool>,
                           std::negation<std::disjunction<
                               std::is_constructible<T, expected<U, G> &>,
                               std::is_constructible<T, expected<U, G>>,
                               std::is_constructible<T, const expected<U, G> &>,
                               std::is_constructible<T, const expected<U, G>>,
                               std::is_convertible<expected<U, G> &, T>,
                               std::is_convertible<expected<U, G>, T>,
                               std::is_convertible<const expected<U, G> &, T>,
                               std::is_convertible<const expected<U, G>, T>>>>,
          std::negation<std::disjunction<
              std::is_constructible<unexpected<E>, expected<U, G> &>,
              std::is_constructible<unexpected<E>, expected<U, G>>,
              std::is_constructible<unexpected<E>, const expected<U, G> &>,
              std::is_constructible<unexpected<E>, const expected<U, G>>>>> {};

template <typename E, typename G>
struct expected_construct_with<void, E, void, G>
    : std::negation<std::disjunction<
          std::is_constructible<unexpected<E>, expected<void, G> &>,
          std::is_constructible<unexpected<E>, expected<void, G>>,
          std::is_constructible<unexpected<E>, const expected<void, G> &>,
          std::is_constructible<unexpected<E>, const expected<void, G>>>> {};

template <typename T, typename E, typename U, typename G>
inline constexpr bool expected_construct_with_v =
    expected_construct_with<T, E, U, G>::value;

template <typename T>
struct is_not_unexpected : std::true_type {};

template <typename E>
struct is_not_unexpected<unexpected<E>> : std::false_type {};

template <typename T>
struct is_not_expected : std::true_type {};

template <typename T, typename E>
struct is_not_expected<expected<T, E>> : std::false_type {};

template <typename T, typename E, typename U>
struct expected_construct_with_arg
    : std::conjunction<std::negation<std::is_same<remove_cvref_t<U>, std::in_place_t>>,
                       std::negation<std::is_same<remove_cvref_t<U>, expected<T, E>>>,
                       std::is_constructible<T, U>, is_not_unexpected<remove_cvref_t<U>>,
                       std::disjunction<std::is_same<remove_cvref_t<U>, bool>,
                                        is_not_expected<remove_cvref_t<U>>>> {};

template <typename T, typename E, typename U>
inline constexpr bool expected_construct_with_arg_v =
    expected_construct_with_arg<T, E, U>::value;

} // namespace expected_detail

template <typename Func, typename... Args>
using __expected_result = std::remove_cv_t<std::invoke_result_t<Func, Args...>>;

template <typename T, typename E>
class WJR_EMPTY_BASES expected
    : expected_detail::expected_storage<T, E>,
      expected_detail::enable_expected_storage<T, __expected_error_type_t<E>> {
    using Mybase = expected_detail::expected_storage<T, E>;

public:
    using value_type = T;
    using error_type = typename Mybase::error_type;
    using unexpected_type = unexpected<error_type>;

    using Mybase::Mybase;

    expected() = default;
    expected(const expected &) = default;
    expected(expected &&) = default;
    expected &operator=(const expected &) = default;
    expected &operator=(expected &&) = default;
    ~expected() = default;

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr explicit expected(std::in_place_t, Args &&...args)
        : Mybase(std::in_place, std::forward<Args>(args)...) {}

    template <
        typename U, typename... Args,
        WJR_REQUIRES(std::is_constructible_v<T, std::initializer_list<U> &, Args...>)>
    constexpr explicit expected(std::in_place_t, std::initializer_list<U> il,
                                Args &&...args)
        : Mybase(std::in_place, il, std::forward<Args>(args)...) {}

    template <typename... Args,
              WJR_REQUIRES(std::is_constructible_v<error_type, Args...>)>
    constexpr explicit expected(unexpect_t, Args &&...args)
        : Mybase(unexpect, std::forward<Args>(args)...) {}

    template <typename U, typename... Args,
              WJR_REQUIRES(std::is_constructible_v<error_type, std::initializer_list<U> &,
                                                   Args...>)>
    constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args &&...args)
        : Mybase(unexpect, il, std::forward<Args>(args)...) {}

#if defined(__cpp_conditional_explicit)
    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, const G &>)>
    constexpr explicit(!std::is_convertible_v<const G &, error_type>)
        expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, G>)>
    constexpr explicit(!std::is_convertible_v<G, error_type>) expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
#else
    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, const G &> &&
                                           std::is_convertible_v<const G &, error_type>)>
    constexpr expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, const G &> &&
                                       !std::is_convertible_v<const G &, error_type>)>
    constexpr explicit expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, G>
                                           &&std::is_convertible_v<G, error_type>)>
    constexpr expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, G> &&
                                       !std::is_convertible_v<G, error_type>)>
    constexpr explicit expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
#endif

#if defined(__cpp_conditional_explicit)
    template <typename U, typename G,
              WJR_REQUIRES(
                  std::is_constructible_v<T, const U &> &&std::is_constructible_v<
                      error_type, const G &>
                      &&expected_detail::expected_construct_with_v<T, error_type, U, G>)>
    constexpr explicit(!(std::is_convertible_v<const U &, T> &&
                         std::is_convertible_v<const G &, error_type>))
        expected(const expected<U, G> &other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<const U &>(other.m_val));
        } else {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename U, typename G,
              WJR_REQUIRES(
                  std::is_constructible_v<T, U> &&std::is_constructible_v<error_type, G>
                      &&expected_detail::expected_construct_with_v<T, error_type, U, G>)>
    constexpr explicit(!(std::is_convertible_v<U, T> &&
                         std::is_convertible_v<G, error_type>))
        expected(expected<U, G> &&other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<U>(other.m_val));
        } else {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }
#else
    template <typename U, typename G,
              WJR_REQUIRES(
                  std::is_constructible_v<T, const U &> &&std::is_constructible_v<
                      error_type, const G &>
                      &&expected_detail::expected_construct_with_v<T, error_type, U, G> &&
                  (std::is_convertible_v<const U &, T> &&
                   std::is_convertible_v<const G &, error_type>))>
    constexpr expected(const expected<U, G> &other) : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<const U &>(other.m_val));
        } else {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename U, typename G,
              WJR_REQUIRES(
                  std::is_constructible_v<T, const U &> &&std::is_constructible_v<
                      error_type, const G &>
                      &&expected_detail::expected_construct_with_v<T, error_type, U, G> &&
                  !(std::is_convertible_v<const U &, T> &&
                    std::is_convertible_v<const G &, error_type>))>
    constexpr explicit expected(const expected<U, G> &other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<const U &>(other.m_val));
        } else {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename U, typename G,
              WJR_REQUIRES(
                  std::is_constructible_v<T, U> &&std::is_constructible_v<error_type, G>
                      &&expected_detail::expected_construct_with_v<T, error_type, U, G> &&
                  (std::is_convertible_v<U, T> && std::is_convertible_v<G, error_type>))>
    constexpr expected(expected<U, G> &&other) : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<U>(other.m_val));
        } else {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }

    template <typename U, typename G,
              WJR_REQUIRES(
                  std::is_constructible_v<T, U> &&std::is_constructible_v<error_type, G>
                      &&expected_detail::expected_construct_with_v<T, error_type, U, G> &&
                  !(std::is_convertible_v<U, T> && std::is_convertible_v<G, error_type>))>
    constexpr explicit expected(expected<U, G> &&other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<U>(other.m_val));
        } else {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }
#endif

#if defined(__cpp_conditional_explicit)
    template <typename U = T, WJR_REQUIRES(expected_detail::expected_construct_with_arg_v<
                                           T, error_type, U>)>
    constexpr explicit(!std::is_convertible_v<U, T>) expected(U &&v)
        : Mybase(std::in_place, std::forward<U>(v)) {}
#else
    template <typename U = T,
              WJR_REQUIRES(expected_detail::expected_construct_with_arg_v<
                           T, error_type, U> &&std::is_convertible_v<U, T>)>
    constexpr expected(U &&v) : Mybase(std::in_place, std::forward<U>(v)) {}

    template <typename U = T, WJR_REQUIRES(expected_detail::expected_construct_with_arg_v<
                                               T, error_type, U> &&
                                           !std::is_convertible_v<U, T>)>
    constexpr explicit expected(U &&v) : Mybase(std::in_place, std::forward<U>(v)) {}
#endif

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<error_type, const G &>
                               &&std::is_assignable_v<error_type &, const G &> &&
                           (std::is_nothrow_constructible_v<error_type, const G &> ||
                            std::is_nothrow_move_constructible_v<T> ||
                            std::is_nothrow_move_constructible_v<error_type>))>
    WJR_CONSTEXPR20 expected &operator=(const unexpected<G> &e) {
        if (has_value()) {
            reinit_expected(this->m_err, this->m_val, std::forward<const G &>(e.error()));
            this->set_invalid();
        } else {
            this->m_err = std::forward<const G &>(e.error());
        }

        return *this;
    }

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<error_type, G>
                               &&std::is_assignable_v<error_type &, G> &&
                           (std::is_nothrow_constructible_v<error_type, G> ||
                            std::is_nothrow_move_constructible_v<T> ||
                            std::is_nothrow_move_constructible_v<error_type>))>
    WJR_CONSTEXPR20 expected &operator=(unexpected<G> &&e) {
        if (has_value()) {
            reinit_expected(this->m_err, this->m_val, std::forward<G>(e.error()));
            this->set_invalid();
        } else {
            this->m_err = std::forward<G>(e.error());
        }

        return *this;
    }

    template <typename U,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<U>, expected> &&
                           expected_detail::is_not_unexpected<remove_cvref_t<U>>::value &&
                           std::is_constructible_v<T, U> &&
                           std::is_assignable_v<T &, U> &&
                           (std::is_nothrow_constructible_v<T, U> ||
                            std::is_nothrow_move_constructible_v<T> ||
                            std::is_nothrow_move_constructible_v<error_type>))>
    WJR_CONSTEXPR20 expected &operator=(U &&v) {
        if (has_value()) {
            this->m_val = std::forward<U>(v);
        } else {
            reinit_expected(this->m_val, this->m_err, std::forward<U>(v));
            this->set_valid();
        }

        return *this;
    }

    using Mybase::has_value;
    WJR_PURE constexpr explicit operator bool() const noexcept { return has_value(); }

    constexpr T *operator->() noexcept { return std::addressof(this->m_val); }
    constexpr const T *operator->() const noexcept { return std::addressof(this->m_val); }

    constexpr T &operator*() & noexcept {
        WJR_ASSERT(has_value());
        return this->m_val;
    }
    constexpr const T &operator*() const & noexcept {
        WJR_ASSERT(has_value());
        return this->m_val;
    }
    constexpr T &&operator*() && noexcept {
        WJR_ASSERT(has_value());
        return std::move(this->m_val);
    }
    constexpr const T &&operator*() const && noexcept {
        WJR_ASSERT(has_value());
        return std::move(this->m_val);
    }

    constexpr T &value() & {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(
                bad_expected_access<std::decay_t<error_type>>(std::as_const(error())));
        }

        return this->m_val;
    }

    constexpr const T &value() const & {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(
                bad_expected_access<std::decay_t<error_type>>(std::as_const(error())));
        }

        return this->m_val;
    }

    constexpr T &&value() && {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_expected_access<std::decay_t<error_type>>(std::move(error())));
        }

        return std::move(this->m_val);
    }

    constexpr const T &&value() const && {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_expected_access<std::decay_t<error_type>>(std::move(error())));
        }

        return std::move(this->m_val);
    }

    constexpr error_type &error() & noexcept {
        WJR_ASSERT(!has_value());
        return this->m_err;
    }
    constexpr const error_type &error() const & noexcept {
        WJR_ASSERT(!has_value());
        return this->m_err;
    }
    constexpr error_type &&error() && noexcept {
        WJR_ASSERT(!has_value());
        return std::move(this->m_err);
    }
    constexpr const error_type &&error() const && noexcept {
        WJR_ASSERT(!has_value());
        return std::move(this->m_err);
    }

    template <typename U>
    constexpr T value_or(U &&default_value) const & {
        return has_value() ? **this : static_cast<T>(std::forward<U>(default_value));
    }

    template <typename U>
    constexpr T value_or(U &&default_value) && {
        return has_value() ? std::move(**this)
                           : static_cast<T>(std::forward<U>(default_value));
    }

    template <typename G = error_type>
    constexpr error_type error_or(G &&default_value) const & {
        return has_value() ? std::forward<G>(default_value) : error();
    }

    template <typename G = error_type>
    constexpr error_type error_or(G &&default_value) && {
        return has_value() ? std::forward<G>(default_value) : std::move(error());
    }

    template <typename Func, typename U = __expected_result<Func, T &>>
    constexpr U and_then(Func &&func) & {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func), this->m_val);
        }

        return U(unexpect, error());
    }

    template <typename Func, typename U = __expected_result<Func, const T &>>
    constexpr U and_then(Func &&func) const & {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func), this->m_val);
        }

        return U(unexpect, error());
    }

    template <typename Func, typename U = __expected_result<Func, T &&>>
    constexpr U and_then(Func &&func) && {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func), std::move(this->m_val));
        }

        return U(unexpect, std::move(error()));
    }

    template <typename Func, typename U = __expected_result<Func, const T &&>>
    constexpr U and_then(Func &&func) const && {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func), std::move(this->m_val));
        }

        return U(unexpect, std::move(error()));
    }

    template <typename Func, typename U = __expected_result<Func, E &>>
    constexpr U or_else(Func &&func) & {
        if (!has_value()) {
            return std::invoke(std::forward<Func>(func), error());
        }

        return U(std::in_place, this->m_val);
    }

    template <typename Func, typename U = __expected_result<Func, const E &>>
    constexpr U or_else(Func &&func) const & {
        if (!has_value()) {
            return std::invoke(std::forward<Func>(func), error());
        }

        return U(std::in_place, this->m_val);
    }

    template <typename Func, typename U = __expected_result<Func, E &&>>
    constexpr U or_else(Func &&func) && {
        if (!has_value()) {
            return std::invoke(std::forward<Func>(func), std::move(error()));
        }

        return U(std::in_place, std::move(this->m_val));
    }

    template <typename Func, typename U = __expected_result<Func, const E &&>>
    constexpr U or_else(Func &&func) const && {
        if (!has_value()) {
            return std::invoke(std::forward<Func>(func), std::move(error()));
        }

        return U(std::in_place, std::move(this->m_val));
    }

    template <typename Func, typename U = __expected_result<Func, T &>>
    constexpr expected<U, E> transform(Func &&func) & {
        if (has_value()) {
            if constexpr (!std::is_void_v<U>) {
                return std::invoke(std::forward<Func>(func), this->m_val);
            } else {
                std::invoke(std::forward<Func>(func), this->m_val);
                return {};
            }
        }

        return expected<U, E>(unexpect, error());
    }

    template <typename Func, typename U = __expected_result<Func, const T &>>
    constexpr expected<U, E> transform(Func &&func) const & {
        if (has_value()) {
            if constexpr (!std::is_void_v<U>) {
                return std::invoke(std::forward<Func>(func), this->m_val);
            } else {
                std::invoke(std::forward<Func>(func), this->m_val);
                return {};
            }
        }

        return expected<U, E>(unexpect, error());
    }

    template <typename Func, typename U = __expected_result<Func, T &&>>
    constexpr expected<U, E> transform(Func &&func) && {
        if (has_value()) {
            if constexpr (!std::is_void_v<U>) {
                return std::invoke(std::forward<Func>(func), std::move(this->m_val));
            } else {
                std::invoke(std::forward<Func>(func), std::move(this->m_val));
                return {};
            }
        }

        return expected<U, E>(unexpect, std::move(error()));
    }

    template <typename Func, typename U = __expected_result<Func, const T &&>>
    constexpr expected<U, E> transform(Func &&func) const && {
        if (has_value()) {
            if constexpr (!std::is_void_v<U>) {
                return std::invoke(std::forward<Func>(func), std::move(this->m_val));
            } else {
                std::invoke(std::forward<Func>(func), std::move(this->m_val));
                return {};
            }
        }

        return expected<U, E>(unexpect, std::move(error()));
    }

    template <typename Func, typename G = __expected_result<Func, error_type &>>
    constexpr expected<T, G> transform_error(Func &&func) & {
        if (!has_value()) {
            return expected<T, G>(unexpect,
                                  std::invoke(std::forward<Func>(func), error()));
        }

        return expected<T, G>(std::in_place, this->m_val);
    }

    template <typename Func, typename G = __expected_result<Func, const error_type &>>
    constexpr expected<T, G> transform_error(Func &&func) const & {
        if (!has_value()) {
            return expected<T, G>(unexpect,
                                  std::invoke(std::forward<Func>(func), error()));
        }

        return expected<T, G>(std::in_place, this->m_val);
    }

    template <typename Func, typename G = __expected_result<Func, error_type &&>>
    constexpr expected<T, G> transform_error(Func &&func) && {
        if (!has_value()) {
            return expected<T, G>(
                unexpect, std::invoke(std::forward<Func>(func), std::move(error())));
        }

        return expected<T, G>(std::in_place, std::move(this->m_val));
    }

    template <typename Func, typename G = __expected_result<Func, const error_type &&>>
    constexpr expected<T, G> transform_error(Func &&func) const && {
        if (!has_value()) {
            return expected<T, G>(
                unexpect, std::invoke(std::forward<Func>(func), std::move(error())));
        }

        return expected<T, G>(std::in_place, std::move(this->m_val));
    }

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr T &emplace(Args &&...args) noexcept {
        if (has_value()) {
            std::destroy_at(std::addressof(this->m_val));
        } else {
            std::destroy_at(std::addressof(this->m_err));
            this->set_valid();
        }

        construct_at(std::addressof(this->m_val), std::forward<Args>(args)...);
        return this->m_val;
    }

    template <
        typename U, typename... Args,
        WJR_REQUIRES(std::is_constructible_v<T, std::initializer_list<U> &, Args...>)>
    constexpr T &emplace(std::initializer_list<U> il, Args &&...args) noexcept {
        if (has_value()) {
            std::destroy_at(std::addressof(this->m_val));
        } else {
            std::destroy_at(std::addressof(this->m_err));
            this->set_valid();
        }

        construct_at(std::addressof(this->m_val), il, std::forward<Args>(args)...);
        return this->m_val;
    }

private:
    WJR_CONSTEXPR20 void __swap_impl(expected &other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&std::is_nothrow_swappable_v<T>
            &&std::is_nothrow_move_constructible_v<error_type>
                &&std::is_nothrow_swappable_v<error_type>) {
        if constexpr (std::is_nothrow_move_constructible_v<error_type>) {
            error_type temp(std::move(other.m_err));
            std::destroy_at(std::addressof(other.m_err));
            WJR_TRY {
                construct_at(std::addressof(other.m_val), std::move(this->m_val));
                std::destroy_at(std::addressof(this->m_val));
                construct_at(std::addressof(this->m_err), std::move(temp));
            }
            WJR_CATCH(...) {
                construct_at(std::addressof(other.m_err), std::move(temp));
                WJR_XTHROW;
            }
        } else {
            T temp(std::move(this->m_val));
            std::destroy_at(std::addressof(this->m_val));
            WJR_TRY {
                construct_at(std::addressof(this->m_err), std::move(other.m_err));
                std::destroy_at(std::addressof(other.m_err));
                construct_at(std::addressof(other.m_val), std::move(temp));
            }
            WJR_CATCH(...) {
                construct_at(std::addressof(this->m_val), std::move(temp));
                WJR_XTHROW;
            }
        }
    }

public:
    template <typename _T = T,
              WJR_REQUIRES(std::is_swappable_v<_T> &&std::is_swappable_v<error_type>
                               &&std::is_move_constructible_v<_T>
                                   &&std::is_move_constructible_v<error_type> &&
                           (std::is_nothrow_move_constructible_v<_T> ||
                            std::is_nothrow_move_constructible_v<error_type>))>
    WJR_CONSTEXPR20 void swap(expected &other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&std::is_nothrow_swappable_v<T>
            &&std::is_nothrow_move_constructible_v<error_type>
                &&std::is_nothrow_swappable_v<error_type>) {
        using std::swap;
        if (has_value()) {
            if (other.has_value()) {
                std::swap(this->m_val, other.m_val);
            } else {
                __swap_impl(other);
            }
        } else {
            if (!other.has_value()) {
                std::swap(this->m_err, other.m_err);
            } else {
                other.__swap_impl(*this);
            }
        }
    }

    template <typename T2, typename E2>
    friend constexpr bool operator==(const expected &lhs, const expected<T2, E2> &rhs) {
        if (WJR_UNLIKELY(lhs.has_value() != rhs.has_value())) {
            return false;
        }

        if (lhs.has_value()) {
            return *lhs == *rhs;
        }

        return lhs.error() == rhs.error();
    }

    template <typename E2>
    friend constexpr bool operator==(const expected &lhs, const unexpected<E2> &rhs) {
        return !lhs.has_value() && lhs.error() == rhs.error();
    }

    template <typename E2>
    friend constexpr bool operator==(const unexpected<E2> &lhs, const expected &rhs) {
        return rhs == lhs;
    }

    template <
        typename U,
        WJR_REQUIRES(expected_detail::is_not_expected<remove_cvref_t<U>>::value
                         &&expected_detail::is_not_unexpected<remove_cvref_t<U>>::value)>
    friend constexpr bool operator==(const expected &lhs, const U &rhs) {
        return lhs.has_value() && *lhs == rhs;
    }

    template <
        typename U,
        WJR_REQUIRES(expected_detail::is_not_expected<remove_cvref_t<U>>::value
                         &&expected_detail::is_not_unexpected<remove_cvref_t<U>>::value)>
    friend constexpr bool operator==(const U &lhs, const expected &rhs) {
        return rhs == lhs;
    }
};

template <typename E>
class WJR_EMPTY_BASES expected<void, E>
    : expected_detail::expected_storage<void, E>,
      expected_detail::enable_expected_storage<void, __expected_error_type_t<E>> {
    using Mybase = expected_detail::expected_storage<void, E>;

public:
    using value_type = void;
    using error_type = typename Mybase::error_type;
    using unexpected_type = unexpected<error_type>;

    using Mybase::Mybase;

    expected() = default;
    expected(const expected &) = default;
    expected(expected &&) = default;
    expected &operator=(const expected &) = default;
    expected &operator=(expected &&) = default;
    ~expected() = default;

    constexpr explicit expected(std::in_place_t) : Mybase(std::in_place) {}

    template <typename... Args,
              WJR_REQUIRES(std::is_constructible_v<error_type, Args...>)>
    constexpr explicit expected(unexpect_t, Args &&...args)
        : Mybase(unexpect, std::forward<Args>(args)...) {}

    template <typename U, typename... Args,
              WJR_REQUIRES(std::is_constructible_v<error_type, std::initializer_list<U> &,
                                                   Args...>)>
    constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args &&...args)
        : Mybase(unexpect, il, std::forward<Args>(args)...) {}

#if defined(__cpp_conditional_explicit)
    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, const G &>)>
    constexpr explicit(!std::is_convertible_v<const G &, error_type>)
        expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, G>)>
    constexpr explicit(!std::is_convertible_v<G, error_type>) expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
#else
    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, const G &> &&
                                           std::is_convertible_v<const G &, error_type>)>
    constexpr expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, const G &> &&
                                       !std::is_convertible_v<const G &, error_type>)>
    constexpr explicit expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, G>
                                           &&std::is_convertible_v<G, error_type>)>
    constexpr expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, G> &&
                                       !std::is_convertible_v<G, error_type>)>
    constexpr explicit expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
#endif

#if defined(__cpp_conditional_explicit)
    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, const G &>
                                           &&expected_detail::expected_construct_with_v<
                                               void, error_type, void, G>)>
    constexpr explicit(!std::is_convertible_v<const G &, error_type>)
        expected(const expected<void, G> &other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<error_type, G> &&expected_detail::
                               expected_construct_with_v<void, error_type, void, G>)>
    constexpr explicit(!std::is_convertible_v<G, error_type>)
        expected(expected<void, G> &&other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }
#else
    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<error_type, const G &>
                               &&expected_detail::expected_construct_with_v<
                                   void, error_type, void, G>
                                   &&std::is_convertible_v<const G &, error_type>)>
    constexpr expected(const expected<void, G> &other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, const G &>
                                           &&expected_detail::expected_construct_with_v<
                                               void, error_type, void, G> &&
                                       !std::is_convertible_v<const G &, error_type>)>
    constexpr explicit expected(const expected<void, G> &other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<error_type, G> &&expected_detail::
                               expected_construct_with_v<void, error_type, void, G>
                                   &&std::is_convertible_v<G, error_type>)>
    constexpr expected(expected<void, G> &&other) : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<error_type, G> &&expected_detail::
                               expected_construct_with_v<void, error_type, void, G> &&
                           !std::is_convertible_v<G, error_type>)>
    constexpr explicit expected(expected<void, G> &&other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }
#endif

    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, const G &> &&
                                           std::is_assignable_v<error_type &, const G &>)>
    WJR_CONSTEXPR20 expected &operator=(const unexpected<G> &e) {
        if (has_value()) {
            construct_at(std::addressof(this->m_err), std::forward<const G &>(e.error()));
            this->set_invalid();
        } else {
            this->m_err = std::forward<const G &>(e.error());
        }

        return *this;
    }

    template <typename G, WJR_REQUIRES(std::is_constructible_v<error_type, G>
                                           &&std::is_assignable_v<error_type &, G>)>
    WJR_CONSTEXPR20 expected &operator=(unexpected<G> &&e) {
        if (has_value()) {
            construct_at(std::addressof(this->m_err), std::forward<G>(e.error()));
            this->set_invalid();
        } else {
            this->m_err = std::forward<G>(e.error());
        }

        return *this;
    }

    using Mybase::has_value;
    WJR_PURE constexpr explicit operator bool() const noexcept { return has_value(); }

    constexpr void operator*() const noexcept { WJR_ASSERT(has_value()); }

    constexpr void value() const & {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(
                bad_expected_access<std::decay_t<error_type>>(std::as_const(error())));
        }
    }

    constexpr void value() && {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_expected_access<std::decay_t<error_type>>(std::move(error())));
        }
    }

    constexpr error_type &error() & noexcept {
        WJR_ASSERT(!has_value());
        return this->m_err;
    }
    constexpr const error_type &error() const & noexcept {
        WJR_ASSERT(!has_value());
        return this->m_err;
    }
    constexpr error_type &&error() && noexcept {
        WJR_ASSERT(!has_value());
        return std::move(this->m_err);
    }
    constexpr const error_type &&error() const && noexcept {
        WJR_ASSERT(!has_value());
        return std::move(this->m_err);
    }

    template <typename G = error_type>
    constexpr error_type error_or(G &&default_value) const & {
        return has_value() ? std::forward<G>(default_value) : error();
    }

    template <typename G = error_type>
    constexpr error_type error_or(G &&default_value) && {
        return has_value() ? std::forward<G>(default_value) : std::move(error());
    }

    template <typename Func, typename U = __expected_result<Func>>
    constexpr U and_then(Func &&func) & {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func));
        }

        return U(unexpect, error());
    }

    template <typename Func, typename U = __expected_result<Func>>
    constexpr U and_then(Func &&func) const & {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func));
        }

        return U(unexpect, error());
    }

    template <typename Func, typename U = __expected_result<Func>>
    constexpr U and_then(Func &&func) && {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func));
        }

        return U(unexpect, std::move(error()));
    }

    template <typename Func, typename U = __expected_result<Func>>
    constexpr U and_then(Func &&func) const && {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func));
        }

        return U(unexpect, std::move(error()));
    }

    template <typename Func, typename U = __expected_result<Func, E &>>
    constexpr U or_else(Func &&func) & {
        if (!has_value()) {
            return std::invoke(std::forward<Func>(func), error());
        }

        return U();
    }

    template <typename Func, typename U = __expected_result<Func, const E &>>
    constexpr U or_else(Func &&func) const & {
        if (!has_value()) {
            return std::invoke(std::forward<Func>(func), error());
        }

        return U();
    }

    template <typename Func, typename U = __expected_result<Func, E &&>>
    constexpr U or_else(Func &&func) && {
        if (!has_value()) {
            return std::invoke(std::forward<Func>(func), std::move(error()));
        }

        return U();
    }

    template <typename Func, typename U = __expected_result<Func, const E &&>>
    constexpr U or_else(Func &&func) const && {
        if (!has_value()) {
            return std::invoke(std::forward<Func>(func), std::move(error()));
        }

        return U();
    }

    template <typename Func, typename U = __expected_result<Func>>
    constexpr expected<U, E> transform(Func &&func) & {
        if (has_value()) {
            if constexpr (!std::is_void_v<U>) {
                return std::invoke(std::forward<Func>(func));
            } else {
                std::invoke(std::forward<Func>(func));
                return {};
            }
        }

        return expected<U, E>(unexpect, error());
    }

    template <typename Func, typename U = __expected_result<Func>>
    constexpr expected<U, E> transform(Func &&func) const & {
        if (has_value()) {
            if constexpr (!std::is_void_v<U>) {
                return std::invoke(std::forward<Func>(func));
            } else {
                std::invoke(std::forward<Func>(func));
                return {};
            }
        }

        return expected<U, E>(unexpect, error());
    }

    template <typename Func, typename U = __expected_result<Func>>
    constexpr expected<U, E> transform(Func &&func) && {
        if (has_value()) {
            if constexpr (!std::is_void_v<U>) {
                return std::invoke(std::forward<Func>(func));
            } else {
                std::invoke(std::forward<Func>(func));
                return {};
            }
        }

        return expected<U, E>(unexpect, std::move(error()));
    }

    template <typename Func, typename U = __expected_result<Func>>
    constexpr expected<U, E> transform(Func &&func) const && {
        if (has_value()) {
            if constexpr (!std::is_void_v<U>) {
                return std::invoke(std::forward<Func>(func));
            } else {
                std::invoke(std::forward<Func>(func));
                return {};
            }
        }

        return expected<U, E>(unexpect, std::move(error()));
    }

    template <typename Func, typename G = __expected_result<Func, error_type &>>
    constexpr expected<void, G> transform_error(Func &&func) & {
        if (!has_value()) {
            return expected<void, G>(unexpect,
                                     std::invoke(std::forward<Func>(func), error()));
        }

        return expected<void, G>(std::in_place);
    }

    template <typename Func, typename G = __expected_result<Func, const error_type &>>
    constexpr expected<void, G> transform_error(Func &&func) const & {
        if (!has_value()) {
            return expected<void, G>(unexpect,
                                     std::invoke(std::forward<Func>(func), error()));
        }

        return expected<void, G>(std::in_place);
    }

    template <typename Func, typename G = __expected_result<Func, error_type &&>>
    constexpr expected<void, G> transform_error(Func &&func) && {
        if (!has_value()) {
            return expected<void, G>(
                unexpect, std::invoke(std::forward<Func>(func), std::move(error())));
        }

        return expected<void, G>(std::in_place);
    }

    template <typename Func, typename G = __expected_result<Func, const error_type &&>>
    constexpr expected<void, G> transform_error(Func &&func) const && {
        if (!has_value()) {
            return expected<void, G>(
                unexpect, std::invoke(std::forward<Func>(func), std::move(error())));
        }

        return expected<void, G>(std::in_place);
    }

    constexpr void emplace() noexcept {
        if (!has_value()) {
            std::destroy_at(std::addressof(this->m_err));
            this->set_valid();
        }
    }

private:
    WJR_CONSTEXPR20 void
    __swap_impl(expected &other) noexcept(std::is_nothrow_move_constructible_v<error_type>
                                              &&std::is_nothrow_swappable_v<error_type>) {
        if constexpr (std::is_nothrow_move_constructible_v<error_type>) {
            construct_at(std::addressof(this->m_err), std::move(other.m_err));
            std::destroy_at(std::addressof(other.m_err));
        } else {
            WJR_TRY {
                construct_at(std::addressof(this->m_err), std::move(other.m_err));
                std::destroy_at(std::addressof(other.m_err));
            }
            WJR_CATCH(...) { WJR_XTHROW; }
        }
    }

public:
    template <typename _E = error_type,
              WJR_REQUIRES(std::is_swappable_v<_E> &&std::is_move_constructible_v<_E>)>
    WJR_CONSTEXPR20 void
    swap(expected &other) noexcept(std::is_nothrow_move_constructible_v<error_type>
                                       &&std::is_nothrow_swappable_v<error_type>) {
        using std::swap;
        if (has_value()) {
            if (!other.has_value()) {
                __swap_impl(other);
            }
        } else {
            if (!other.has_value()) {
                std::swap(this->m_err, other.m_err);
            } else {
                other.__swap_impl(*this);
            }
        }
    }

    template <typename T2, typename E2>
    friend constexpr bool operator==(const expected &lhs, const expected<T2, E2> &rhs) {
        if (WJR_UNLIKELY(lhs.has_value() != rhs.has_value())) {
            return false;
        }

        if (lhs.has_value()) {
            return true;
        }

        return lhs.error() == rhs.error();
    }

    template <typename E2>
    friend constexpr bool operator==(const expected &lhs, const unexpected<E2> &rhs) {
        return !lhs.has_value() && lhs.error() == rhs.error();
    }

    template <typename E2>
    friend constexpr bool operator==(const unexpected<E2> &lhs, const expected &rhs) {
        return rhs == lhs;
    }
};

template <typename T, typename E, E init>
using compressed_expected = expected<T, compressed_unexpected<E, init>>;

#define WJR_EXPECTED_TRY(...)                                                            \
    do {                                                                                 \
        if (auto exp = (__VA_ARGS__); WJR_UNLIKELY(!exp)) {                              \
            return ::wjr::unexpected(std::move(exp).error());                            \
        }                                                                                \
    } while (0)

#define WJR_EXPECTED_INIT(NAME, ...)                                                     \
    auto __wjr_exp_##NAME = (__VA_ARGS__);                                               \
    if (WJR_UNLIKELY(!__wjr_exp_##NAME)) {                                               \
        return ::wjr::unexpected(std::move(__wjr_exp_##NAME).error());                   \
    }                                                                                    \
    auto &NAME = *__wjr_exp_##NAME

#define WJR_EXPECTED_SET(VAR, ...)                                                       \
    do {                                                                                 \
        if (auto exp = (__VA_ARGS__); WJR_UNLIKELY(!exp)) {                              \
            return ::wjr::unexpected(std::move(exp).error());                            \
        } else {                                                                         \
            VAR = *std::move(exp);                                                       \
        }                                                                                \
    } while (0)

} // namespace wjr

namespace std {

template <typename E>
constexpr void swap(wjr::unexpected<E> &lhs,
                    wjr::unexpected<E> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <typename T, typename E>
constexpr void swap(wjr::expected<T, E> &lhs,
                    wjr::expected<T, E> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace std

#endif // WJR_EXPECTED_HPP__