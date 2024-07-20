/**
 * @file expected.hpp
 * @author wjr
 * @detail
 * @version 0.1
 * @date 2024-07-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WJR_EXPECTED_HPP__
#define WJR_EXPECTED_HPP__

#include <exception>

#include <wjr/crtp/class_base.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

struct unexpect_t {};

inline constexpr unexpect_t unexpect = {};

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

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<E, Args...>)>
    constexpr explicit unexpected(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>)
        : m_err(std::forward<Args>(args)...) {}

    template <
        typename U, typename... Args,
        WJR_REQUIRES(std::is_constructible_v<E, std::initializer_list<U> &, Args...>)>
    constexpr explicit unexpected(
        std::in_place_t, std::initializer_list<U> il,
        Args &&...args) noexcept(std::
                                     is_nothrow_constructible_v<
                                         E, std::initializer_list<U> &, Args...>)
        : m_err(il, std::forward<Args>(args)...) {}

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
              &&std::is_trivially_destructible_v<E>>
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

    bool m_has_val;
    union {
        E m_err;
    };
};

template <typename T, typename E>
struct expected_operations_base : expected_storage_base<T, E> {
    using Mybase = expected_storage_base<T, E>;
    using Mybase::Mybase;

    template <typename... Args>
    constexpr void construct_value(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>) {
        this->m_has_val = true;
        construct_at(this->m_val, std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void construct_error(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>) {
        this->m_has_val = false;
        construct_at(this->m_err, std::forward<Args>(args)...);
    }

    WJR_CONSTEXPR20 void __copy_construct(const expected_operations_base &other) {
        if (other.m_has_val) {
            construct_value(other.m_val);
        } else {
            construct_error(other.m_err);
        }
    }

    WJR_CONSTEXPR20 void __move_construct(expected_operations_base &&other) noexcept(
        std::is_nothrow_move_constructible_v<T>
            &&std::is_nothrow_move_constructible_v<E>) {
        if (other.m_has_val) {
            construct_value(std::move(other.m_val));
        } else {
            construct_error(std::move(other.m_err));
        }
    }

    WJR_CONSTEXPR20 void __copy_assign(const expected_operations_base &other) {
        if (this->m_has_val) {
            if (WJR_LIKELY(other.m_has_val)) {
                this->m_val = other.m_val;
            } else {
                reinit_expected(this->m_err, this->m_val, other.m_err);
                this->m_has_val = false;
            }
        } else {
            if (WJR_LIKELY(!other.m_has_val)) {
                this->m_err = other.m_err;
            } else {
                reinit_expected(this->m_val, this->m_err, other.m_val);
                this->m_has_val = true;
            }
        }
    }

    WJR_CONSTEXPR20 void __move_assign(expected_operations_base &&other) noexcept(
        std::is_nothrow_move_assignable_v<T> &&std::is_nothrow_move_assignable_v<E>) {
        if (this->m_has_val) {
            if (WJR_LIKELY(other.m_has_val)) {
                this->m_val = std::move(other.m_val);
            } else {
                reinit_expected(this->m_err, this->m_val, std::move(other.m_err));
                this->m_has_val = false;
            }
        } else {
            if (WJR_LIKELY(!other.m_has_val)) {
                this->m_err = std::move(other.m_err);
            } else {
                reinit_expected(this->m_val, this->m_err, std::move(other.m_val));
                this->m_has_val = true;
            }
        }
    }

    constexpr bool has_value() const { return this->m_has_val; }
};

template <typename E>
struct expected_operations_base<void, E> : expected_storage_base<void, E> {
    using Mybase = expected_storage_base<void, E>;
    using Mybase::Mybase;

    template <typename... Args>
    constexpr void construct_error(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<E, Args...>) {
        this->m_has_val = false;
        construct_at(this->m_err, std::forward<Args>(args)...);
    }

    WJR_CONSTEXPR20 void __copy_construct(const expected_operations_base &other) {
        if (!other.m_has_val) {
            construct_error(other.m_err);
        }
    }

    WJR_CONSTEXPR20 void __move_construct(expected_operations_base &&other) noexcept(
        std::is_nothrow_move_constructible_v<E>) {
        if (!other.m_has_val) {
            construct_error(std::move(other.m_err));
        }
    }

    WJR_CONSTEXPR20 void __copy_assign(const expected_operations_base &other) {
        if (this->m_has_val) {
            if (!other.m_has_val) {
                construct_at(std::addressof(this->m_err), other.m_err);
                this->m_has_val = false;
            }
        } else {
            if (WJR_LIKELY(!other.m_has_val)) {
                this->m_err = other.m_err;
            } else {
                std::destroy_at(std::addressof(this->m_err));
                this->m_has_val = true;
            }
        }
    }

    WJR_CONSTEXPR20 void __move_assign(expected_operations_base &&other) noexcept(
        std::is_nothrow_move_assignable_v<E>) {
        if (this->m_has_val) {
            if (!other.m_has_val) {
                construct_at(std::addressof(this->m_err), std::move(other.m_err));
                this->m_has_val = false;
            }
        } else {
            if (WJR_LIKELY(!other.m_has_val)) {
                this->m_err = std::move(other.m_err);
            } else {
                std::destroy_at(std::addressof(this->m_err));
                this->m_has_val = true;
            }
        }
    }

    constexpr bool has_value() const { return this->m_has_val; }
};

template <typename T, typename E>
struct __expected_storage_impl {
    using type =
        control_special_members_base<expected_operations_base<T, E>,
                                     std::is_trivially_copy_constructible_v<T> &&
                                         std::is_trivially_copy_constructible_v<E>,
                                     std::is_trivially_move_constructible_v<T> &&
                                         std::is_trivially_move_constructible_v<E>,
                                     std::is_trivially_copy_assignable_v<T> &&
                                         std::is_trivially_copy_constructible_v<T> &&
                                         std::is_trivially_destructible_v<T> &&
                                         std::is_trivially_copy_assignable_v<E> &&
                                         std::is_trivially_copy_constructible_v<E> &&
                                         std::is_trivially_destructible_v<E>,
                                     std::is_trivially_move_assignable_v<T> &&
                                         std::is_trivially_move_constructible_v<T> &&
                                         std::is_trivially_destructible_v<T> &&
                                         std::is_trivially_move_assignable_v<E> &&
                                         std::is_trivially_move_constructible_v<E> &&
                                         std::is_trivially_destructible_v<E>>;
};

template <typename E>
struct __expected_storage_impl<void, E> {
    using type =
        control_special_members_base<expected_operations_base<void, E>,
                                     std::is_trivially_copy_constructible_v<E>,
                                     std::is_trivially_move_constructible_v<E>,
                                     std::is_trivially_copy_assignable_v<E> &&
                                         std::is_trivially_copy_constructible_v<E> &&
                                         std::is_trivially_destructible_v<E>,
                                     std::is_trivially_move_assignable_v<E> &&
                                         std::is_trivially_move_constructible_v<E> &&
                                         std::is_trivially_destructible_v<E>>;
};

template <typename T, typename E>
using expected_storage = typename __expected_storage_impl<T, E>::type;

template <typename T, typename E>
struct __enable_expeted_storage_impl {
    using type = enable_special_members_base<
        std::is_default_constructible_v<T> && std::is_default_constructible_v<E>, true,
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
        std::is_default_constructible_v<E>, true, std::is_copy_constructible_v<E>,
        std::is_move_constructible_v<E>,
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

template <typename T, typename E>
class WJR_EMPTY_BASES expected : expected_detail::expected_storage<T, E>,
                                 expected_detail::enable_expected_storage<T, E> {
    using Mybase = expected_detail::expected_storage<T, E>;

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

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

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<E, Args...>)>
    constexpr explicit expected(unexpect_t, Args &&...args)
        : Mybase(unexpect, std::forward<Args>(args)...) {}

    template <
        typename U, typename... Args,
        WJR_REQUIRES(std::is_constructible_v<E, std::initializer_list<U> &, Args...>)>
    constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args &&...args)
        : Mybase(unexpect, il, std::forward<Args>(args)...) {}

#if defined(__cpp_conditional_explicit)
    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, const G &>)>
    constexpr explicit(!std::is_convertible_v<const G &, E>)
        expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, G>)>
    constexpr explicit(!std::is_convertible_v<G, E>) expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
#else
    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, const G &>
                                           &&std::is_convertible_v<const G &, E>)>
    constexpr expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, const G &> &&
                                       !std::is_convertible_v<const G &, E>)>
    constexpr explicit expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<E, G> &&std::is_convertible_v<G, E>)>
    constexpr expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<E, G> && !std::is_convertible_v<G, E>)>
    constexpr explicit expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
#endif

#if defined(__cpp_conditional_explicit)
    template <typename U, typename G,
              WJR_REQUIRES(std::is_constructible_v<
                           T, const U &> &&std::is_constructible_v<E, const G &>
                               &&expected_detail::expected_construct_with_v<T, E, U, G>)>
    constexpr explicit(!(std::is_convertible_v<const U &, T> &&
                         std::is_convertible_v<const G &, E>))
        expected(const expected<U, G> &other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<const U &>(other.m_val));
        } else {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename U, typename G,
              WJR_REQUIRES(std::is_constructible_v<T, U> &&std::is_constructible_v<E, G>
                               &&expected_detail::expected_construct_with_v<T, E, U, G>)>
    constexpr explicit(!(std::is_convertible_v<U, T> && std::is_convertible_v<G, E>))
        expected(expected<U, G> &&other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<U>(other.m_val));
        } else {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }
#else
    template <
        typename U, typename G,
        WJR_REQUIRES(
            std::is_constructible_v<T, const U &> &&std::is_constructible_v<E, const G &>
                &&expected_detail::expected_construct_with_v<T, E, U, G> &&
            (std::is_convertible_v<const U &, T> && std::is_convertible_v<const G &, E>))>
    constexpr expected(const expected<U, G> &other) : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<const U &>(other.m_val));
        } else {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename U, typename G,
              WJR_REQUIRES(std::is_constructible_v<
                               T, const U &> &&std::is_constructible_v<E, const G &>
                               &&expected_detail::expected_construct_with_v<T, E, U, G> &&
                           !(std::is_convertible_v<const U &, T> &&
                             std::is_convertible_v<const G &, E>))>
    constexpr explicit expected(const expected<U, G> &other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<const U &>(other.m_val));
        } else {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename U, typename G,
              WJR_REQUIRES(std::is_constructible_v<T, U> &&std::is_constructible_v<E, G>
                               &&expected_detail::expected_construct_with_v<T, E, U, G> &&
                           (std::is_convertible_v<U, T> && std::is_convertible_v<G, E>))>
    constexpr expected(expected<U, G> &&other) : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<U>(other.m_val));
        } else {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }

    template <typename U, typename G,
              WJR_REQUIRES(std::is_constructible_v<T, U> &&std::is_constructible_v<E, G>
                               &&expected_detail::expected_construct_with_v<T, E, U, G> &&
                           !(std::is_convertible_v<U, T> && std::is_convertible_v<G, E>))>
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
    template <typename U = T,
              WJR_REQUIRES(expected_detail::expected_construct_with_arg_v<T, E, U>)>
    constexpr explicit(!std::is_convertible_v<U, T>) expected(U &&v)
        : Mybase(std::in_place, std::forward<U>(v)) {}
#else
    template <typename U = T, WJR_REQUIRES(expected_detail::expected_construct_with_arg_v<
                                           T, E, U> &&std::is_convertible_v<U, T>)>
    constexpr expected(U &&v) : Mybase(std::in_place, std::forward<U>(v)) {}

    template <typename U = T,
              WJR_REQUIRES(expected_detail::expected_construct_with_arg_v<T, E, U> &&
                           !std::is_convertible_v<U, T>)>
    constexpr explicit expected(U &&v) : Mybase(std::in_place, std::forward<U>(v)) {}
#endif

    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, const G &>
                                           &&std::is_assignable_v<E &, const G &> &&
                                       (std::is_nothrow_constructible_v<E, const G &> ||
                                        std::is_nothrow_move_constructible_v<T> ||
                                        std::is_nothrow_move_constructible_v<E>))>
    WJR_CONSTEXPR20 expected &operator=(const unexpected<G> &e) {
        if (has_value()) {
            reinit_expected(this->m_err, this->m_val, std::forward<const G &>(e.error()));
            this->m_has_val = false;
        } else {
            this->m_err = std::forward<const G &>(e.error());
        }

        return *this;
    }

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<E, G> &&std::is_assignable_v<E &, G> &&
                           (std::is_nothrow_constructible_v<E, G> ||
                            std::is_nothrow_move_constructible_v<T> ||
                            std::is_nothrow_move_constructible_v<E>))>
    WJR_CONSTEXPR20 expected &operator=(unexpected<G> &&e) {
        if (has_value()) {
            reinit_expected(this->m_err, this->m_val, std::forward<G>(e.error()));
            this->m_has_val = false;
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
                            std::is_nothrow_move_constructible_v<E>))>
    WJR_CONSTEXPR20 expected &operator=(U &&v) {
        if (has_value()) {
            this->m_val = std::forward<U>(v);
        } else {
            reinit_expected(this->m_val, this->m_err, std::forward<U>(v));
            this->m_has_val = true;
        }

        return *this;
    }

    using Mybase::has_value;
    constexpr explicit operator bool() const noexcept { return has_value(); }

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
            WJR_THROW(bad_expected_access<std::decay_t<E>>(std::as_const(error())));
        }

        return this->m_val;
    }

    constexpr const T &value() const & {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_expected_access<std::decay_t<E>>(std::as_const(error())));
        }

        return this->m_val;
    }

    constexpr T &&value() && {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_expected_access<std::decay_t<E>>(std::move(error())));
        }

        return std::move(this->m_val);
    }

    constexpr const T &&value() const && {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_expected_access<std::decay_t<E>>(std::move(error())));
        }

        return std::move(this->m_val);
    }

    constexpr E &error() & noexcept {
        WJR_ASSERT(!has_value());
        return this->m_err;
    }
    constexpr const E &error() const & noexcept {
        WJR_ASSERT(!has_value());
        return this->m_err;
    }
    constexpr E &&error() && noexcept {
        WJR_ASSERT(!has_value());
        return std::move(this->m_err);
    }
    constexpr const E &&error() const && noexcept {
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

    template <typename G = E>
    constexpr E error_or(G &&default_value) const & {
        return has_value() ? std::forward<G>(default_value) : error();
    }

    template <typename G = E>
    constexpr E error_or(G &&default_value) && {
        return has_value() ? std::forward<G>(default_value) : std::move(error());
    }

    template <typename... Args, WJR_REQUIRES(std::is_nothrow_constructible_v<T, Args...>)>
    constexpr T &emplace(Args &&...args) noexcept {
        if (has_value()) {
            std::destroy_at(std::addressof(this->m_val));
        } else {
            std::destroy_at(std::addressof(this->m_err));
            this->m_has_val = true;
        }

        construct_at(std::addressof(this->m_val), std::forward<Args>(args)...);
        return this->m_val;
    }

    template <typename U, typename... Args,
              WJR_REQUIRES(std::is_nothrow_constructible_v<T, std::initializer_list<U> &,
                                                           Args...>)>
    constexpr T &emplace(std::initializer_list<U> il, Args &&...args) noexcept {
        if (has_value()) {
            std::destroy_at(std::addressof(this->m_val));
        } else {
            std::destroy_at(std::addressof(this->m_err));
            this->m_has_val = true;
        }

        construct_at(std::addressof(this->m_val), il, std::forward<Args>(args)...);
        return this->m_val;
    }

private:
    WJR_CONSTEXPR20 void __swap_impl(expected &other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&std::is_nothrow_swappable_v<T>
            &&std::is_nothrow_move_constructible_v<E> &&std::is_nothrow_swappable_v<E>) {
        if constexpr (std::is_nothrow_move_constructible_v<E>) {
            E temp(std::move(other.m_err));
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
    template <typename _T = T, WJR_REQUIRES(std::is_swappable_v<_T> &&std::is_swappable_v<
                                                E> &&std::is_move_constructible_v<_T>
                                                &&std::is_move_constructible_v<E> &&
                                            (std::is_nothrow_move_constructible_v<_T> ||
                                             std::is_nothrow_move_constructible_v<E>))>
    WJR_CONSTEXPR20 void swap(expected &other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&std::is_nothrow_swappable_v<T>
            &&std::is_nothrow_move_constructible_v<E> &&std::is_nothrow_swappable_v<E>) {
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
      expected_detail::enable_expected_storage<void, E> {
    using Mybase = expected_detail::expected_storage<void, E>;

public:
    using value_type = void;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    using Mybase::Mybase;

    expected() = default;
    expected(const expected &) = default;
    expected(expected &&) = default;
    expected &operator=(const expected &) = default;
    expected &operator=(expected &&) = default;
    ~expected() = default;

    constexpr explicit expected(std::in_place_t) : Mybase(std::in_place) {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<E, Args...>)>
    constexpr explicit expected(unexpect_t, Args &&...args)
        : Mybase(unexpect, std::forward<Args>(args)...) {}

    template <
        typename U, typename... Args,
        WJR_REQUIRES(std::is_constructible_v<E, std::initializer_list<U> &, Args...>)>
    constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args &&...args)
        : Mybase(unexpect, il, std::forward<Args>(args)...) {}

#if defined(__cpp_conditional_explicit)
    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, const G &>)>
    constexpr explicit(!std::is_convertible_v<const G &, E>)
        expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, G>)>
    constexpr explicit(!std::is_convertible_v<G, E>) expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
#else
    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, const G &>
                                           &&std::is_convertible_v<const G &, E>)>
    constexpr expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, const G &> &&
                                       !std::is_convertible_v<const G &, E>)>
    constexpr explicit expected(const unexpected<G> &e)
        : Mybase(unexpect, std::forward<const G &>(e.error())) {}

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<E, G> &&std::is_convertible_v<G, E>)>
    constexpr expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<E, G> && !std::is_convertible_v<G, E>)>
    constexpr explicit expected(unexpected<G> &&e)
        : Mybase(unexpect, std::forward<G>(e.error())) {}
#endif

#if defined(__cpp_conditional_explicit)
    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<E, const G &> &&expected_detail::
                               expected_construct_with_v<void, E, void, G>)>
    constexpr explicit(!std::is_convertible_v<const G &, E>)
        expected(const expected<void, G> &other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, G> &&expected_detail::
                                           expected_construct_with_v<void, E, void, G>)>
    constexpr explicit(!std::is_convertible_v<G, E>) expected(expected<void, G> &&other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }
#else
    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<E, const G &> &&expected_detail::
                               expected_construct_with_v<void, E, void, G>
                                   &&std::is_convertible_v<const G &, E>)>
    constexpr expected(const expected<void, G> &other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<E, const G &> &&expected_detail::
                               expected_construct_with_v<void, E, void, G> &&
                           !std::is_convertible_v<const G &, E>)>
    constexpr explicit expected(const expected<void, G> &other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<const G &>(other.m_err));
        }
    }

    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, G> &&expected_detail::
                                           expected_construct_with_v<void, E, void, G>
                                               &&std::is_convertible_v<G, E>)>
    constexpr expected(expected<void, G> &&other) : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }

    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, G> &&expected_detail::
                                           expected_construct_with_v<void, E, void, G> &&
                                       !std::is_convertible_v<G, E>)>
    constexpr explicit expected(expected<void, G> &&other)
        : Mybase(enable_default_constructor) {
        if (!other.has_value()) {
            this->construct_error(std::forward<G>(other.m_err));
        }
    }
#endif

    template <typename G, WJR_REQUIRES(std::is_constructible_v<E, const G &>
                                           &&std::is_assignable_v<E &, const G &>)>
    WJR_CONSTEXPR20 expected &operator=(const unexpected<G> &e) {
        if (has_value()) {
            construct_at(std::addressof(this->m_err), std::forward<const G &>(e.error()));
            this->m_has_val = false;
        } else {
            this->m_err = std::forward<const G &>(e.error());
        }

        return *this;
    }

    template <typename G,
              WJR_REQUIRES(std::is_constructible_v<E, G> &&std::is_assignable_v<E &, G>)>
    WJR_CONSTEXPR20 expected &operator=(unexpected<G> &&e) {
        if (has_value()) {
            construct_at(std::addressof(this->m_err), std::forward<G>(e.error()));
            this->m_has_val = false;
        } else {
            this->m_err = std::forward<G>(e.error());
        }

        return *this;
    }

    using Mybase::has_value;
    constexpr explicit operator bool() const noexcept { return has_value(); }

    constexpr void operator*() const noexcept { WJR_ASSERT(has_value()); }

    constexpr void value() const & {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_expected_access<std::decay_t<E>>(std::as_const(error())));
        }
    }

    constexpr void value() && {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_expected_access<std::decay_t<E>>(std::move(error())));
        }
    }

    constexpr E &error() & noexcept {
        WJR_ASSERT(!has_value());
        return this->m_err;
    }
    constexpr const E &error() const & noexcept {
        WJR_ASSERT(!has_value());
        return this->m_err;
    }
    constexpr E &&error() && noexcept {
        WJR_ASSERT(!has_value());
        return std::move(this->m_err);
    }
    constexpr const E &&error() const && noexcept {
        WJR_ASSERT(!has_value());
        return std::move(this->m_err);
    }

    template <typename G = E>
    constexpr E error_or(G &&default_value) const & {
        return has_value() ? std::forward<G>(default_value) : error();
    }

    template <typename G = E>
    constexpr E error_or(G &&default_value) && {
        return has_value() ? std::forward<G>(default_value) : std::move(error());
    }

    constexpr void emplace() noexcept {
        if (!has_value()) {
            std::destroy_at(std::addressof(this->m_err));
            this->m_has_val = true;
        }
    }

private:
    WJR_CONSTEXPR20 void __swap_impl(expected &other) noexcept(
        std::is_nothrow_move_constructible_v<E> &&std::is_nothrow_swappable_v<E>) {
        if constexpr (std::is_nothrow_move_constructible_v<E>) {
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
    template <typename _E = E,
              WJR_REQUIRES(std::is_swappable_v<_E> &&std::is_move_constructible_v<_E>)>
    WJR_CONSTEXPR20 void swap(expected &other) noexcept(
        std::is_nothrow_move_constructible_v<E> &&std::is_nothrow_swappable_v<E>) {
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

#define WJR_EXPECTED_TRY(expr)                                                           \
    do {                                                                                 \
        if (auto result = (expr); !result) {                                             \
            return ::wjr::unexpected<typename decltype(expr)::error_type>(               \
                std::move(result).error());                                              \
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