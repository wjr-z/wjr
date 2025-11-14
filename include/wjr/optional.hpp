/**
 * @file optional.hpp
 * @author wjr
 * @details
 * @version 0.1
 * @date 2024-11-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WJR_OPTIONAL_HPP__
#define WJR_OPTIONAL_HPP__

#include <exception>

#include <wjr/assert.hpp>
#include <wjr/container/compressed_value.hpp>
#include <wjr/crtp/class_base.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr {

struct nullopt_t {
    struct _tag {};
    constexpr explicit nullopt_t(_tag) noexcept {}
};

inline constexpr nullopt_t nullopt{nullopt_t::_tag{}};

struct voidopt_t {
    struct _tag {};
    constexpr explicit voidopt_t(_tag) noexcept {}
};

inline constexpr voidopt_t voidopt{voidopt_t::_tag{}};

class bad_optional_access : public std::exception {
public:
    WJR_NODISCARD virtual const char *what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
struct _optional_unwrap {
    using type = T;
};

template <typename T, T uniq_success>
struct _optional_unwrap<compressed_value<T, uniq_success>> {
    using type = T;
};

template <typename T>
using _optional_unwrap_t = typename _optional_unwrap<T>::type;

template <typename T>
class optional;

namespace optional_detail {

template <typename T, bool Destructor = is_void_or_v<T, std::is_trivially_destructible<T>>>
struct optional_storage_base {
    constexpr optional_storage_base() noexcept : m_has_val(false) {}

    template <typename... Args>
    constexpr optional_storage_base(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : m_has_val(true), m_val(std::forward<Args>(args)...) {}

    constexpr optional_storage_base(nullopt_t) noexcept : m_has_val(false) {}

    optional_storage_base(const optional_storage_base &) = default;
    optional_storage_base(optional_storage_base &&) = default;
    optional_storage_base &operator=(const optional_storage_base &) = default;
    optional_storage_base &operator=(optional_storage_base &&) = default;

    WJR_CONSTEXPR20 optional_storage_base(enable_default_constructor_t) noexcept {}

    ~optional_storage_base() = default;

    WJR_PURE constexpr bool has_value() const noexcept { return m_has_val; }
    constexpr void set_valid() noexcept { m_has_val = true; }
    constexpr void set_invalid() noexcept { m_has_val = false; }

    bool m_has_val;
    union {
        T m_val;
    };
};

template <typename T>
struct optional_storage_base<T, false> {
    constexpr optional_storage_base() noexcept : m_has_val(false) {}

    template <typename... Args>
    constexpr optional_storage_base(std::in_place_t, Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : m_has_val(true), m_val(std::forward<Args>(args)...) {}

    constexpr optional_storage_base(nullopt_t) noexcept : m_has_val(false) {}

    optional_storage_base(const optional_storage_base &) = default;
    optional_storage_base(optional_storage_base &&) = default;
    optional_storage_base &operator=(const optional_storage_base &) = default;
    optional_storage_base &operator=(optional_storage_base &&) = default;

    WJR_CONSTEXPR20 optional_storage_base(enable_default_constructor_t) noexcept {}

    ~optional_storage_base() noexcept(std::is_nothrow_destructible_v<T>) {
        if (m_has_val) {
            std::destroy_at(std::addressof(m_val));
        }
    }

    WJR_PURE constexpr bool has_value() const noexcept { return m_has_val; }
    constexpr void set_valid() noexcept { m_has_val = true; }
    constexpr void set_invalid() noexcept { m_has_val = false; }

    bool m_has_val;
    union {
        T m_val;
    };
};

template <>
struct optional_storage_base<void, true> {
    constexpr optional_storage_base() noexcept : m_has_val(false) {}
    constexpr optional_storage_base(std::in_place_t) noexcept : m_has_val(true) {}
    constexpr optional_storage_base(nullopt_t) noexcept : m_has_val(false) {}

    optional_storage_base(const optional_storage_base &) = default;
    optional_storage_base(optional_storage_base &&) = default;
    optional_storage_base &operator=(const optional_storage_base &) = default;
    optional_storage_base &operator=(optional_storage_base &&) = default;

    WJR_CONSTEXPR20 optional_storage_base(enable_default_constructor_t) noexcept
        : m_has_val(false) {}

    ~optional_storage_base() = default;

    WJR_PURE constexpr bool has_value() const noexcept { return m_has_val; }
    constexpr void set_valid() noexcept { m_has_val = true; }
    constexpr void set_invalid() noexcept { m_has_val = false; }

    bool m_has_val;
};

template <typename T, T uniq_failed>
struct optional_storage_base<compressed_value<T, uniq_failed>, true> {
    constexpr optional_storage_base() noexcept : m_val(uniq_failed) {}

    template <typename... Args>
    constexpr optional_storage_base(std::in_place_t, Args &&...args) noexcept
        : m_val(std::forward<Args>(args)...) {
        WJR_ASSERT_ASSUME_L2(has_value());
    }

    constexpr optional_storage_base(nullopt_t) noexcept : m_val(uniq_failed) {}

    optional_storage_base(const optional_storage_base &) = default;
    optional_storage_base(optional_storage_base &&) = default;
    optional_storage_base &operator=(const optional_storage_base &) = default;
    optional_storage_base &operator=(optional_storage_base &&) = default;

    WJR_CONSTEXPR20 optional_storage_base(enable_default_constructor_t) noexcept {}

    ~optional_storage_base() = default;

    WJR_PURE constexpr bool has_value() const noexcept { return m_val != uniq_failed; }
    constexpr void set_valid() noexcept {}
    constexpr void set_invalid() noexcept { m_val = uniq_failed; }

    T m_val;
};

template <typename T>
struct optional_operations_base : optional_storage_base<T> {
    using Mybase = optional_storage_base<T>;
    using Mybase::Mybase;

    using value_type = _optional_unwrap_t<T>;

    constexpr void set_valid() noexcept { Mybase::set_valid(); }
    constexpr void set_invalid() noexcept { Mybase::set_invalid(); }

    template <typename... Args>
    constexpr void
    construct_value(Args &&...args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>) {
        wjr::construct_at(std::addressof(this->m_val), std::forward<Args>(args)...);
        set_valid();
    }

    constexpr void construct_error() noexcept { set_invalid(); }

    WJR_CONSTEXPR20 void _copy_construct(const optional_operations_base &other) {
        if (other.has_value()) {
            construct_value(other.m_val);
        } else {
            construct_error();
        }
    }

    WJR_CONSTEXPR20 void _move_construct(optional_operations_base &&other) noexcept(
        std::is_nothrow_move_constructible_v<value_type>) {
        if (other.has_value()) {
            construct_value(std::move(other.m_val));
        } else {
            construct_error();
        }
    }

    WJR_CONSTEXPR20 void _copy_assign(const optional_operations_base &other) {
        if (this->has_value()) {
            if (WJR_LIKELY(other.has_value())) {
                this->m_val = other.m_val;
            } else {
                std::destroy_at(std::addressof(this->m_val));
                set_invalid();
            }
        } else {
            if (WJR_LIKELY(!other.has_value())) {
            } else {
                wjr::construct_at(std::addressof(this->m_val), other.m_val);
                set_valid();
            }
        }
    }

    WJR_CONSTEXPR20 void _move_assign(optional_operations_base &&other) noexcept(
        std::is_nothrow_move_assignable_v<value_type>) {
        if (this->has_value()) {
            if (WJR_LIKELY(other.has_value())) {
                this->m_val = std::move(other.m_val);
            } else {
                std::destroy_at(this->m_val);
                set_invalid();
            }
        } else {
            if (WJR_LIKELY(!other.has_value())) {
            } else {
                wjr::construct_at(std::addressof(this->m_val), std::move(other.m_val));
                set_valid();
            }
        }
    }
};

template <>
struct optional_operations_base<void> : optional_storage_base<void> {
    using Mybase = optional_storage_base<void>;
    using Mybase::Mybase;

    using value_type = void;

    constexpr void set_valid() noexcept { Mybase::set_valid(); }
    constexpr void set_invalid() noexcept { Mybase::set_invalid(); }
};

template <typename T>
struct _optional_storage_impl {
    using value_type = _optional_unwrap_t<T>;

    using type =
        control_special_members_base<optional_operations_base<T>,
                                     std::is_trivially_copy_constructible_v<value_type>,
                                     std::is_trivially_move_constructible_v<value_type>,
                                     std::is_trivially_copy_assignable_v<value_type> &&
                                         std::is_trivially_copy_constructible_v<value_type> &&
                                         std::is_trivially_destructible_v<value_type>,
                                     std::is_trivially_move_assignable_v<value_type> &&
                                         std::is_trivially_move_constructible_v<value_type> &&
                                         std::is_trivially_destructible_v<value_type>>;
};

template <>
struct _optional_storage_impl<void> {
    using type =
        control_special_members_base<optional_operations_base<void>, true, true, true, true>;
};

template <typename T>
using optional_storage = typename _optional_storage_impl<T>::type;

template <typename T>
struct _enable_optional_storage_impl {
    using type = enable_special_members_base<
        true, true, std::is_copy_constructible_v<T>, std::is_move_constructible_v<T>,
        std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T> &&
            std::is_nothrow_move_constructible_v<T>,
        std::is_move_assignable_v<T> && std::is_move_constructible_v<T> &&
            std::is_nothrow_move_constructible_v<T>>;
};

template <>
struct _enable_optional_storage_impl<void> {
    using type = enable_special_members_base<true, true, true, true, true, true>;
};

template <typename T>
using enable_optional_storage = typename _enable_optional_storage_impl<T>::type;

template <typename T, typename U>
struct optional_construct_with
    : std::disjunction<
          std::is_same<std::remove_cv_t<T>, bool>,
          std::negation<std::disjunction<
              std::is_constructible<T, optional<U> &>, std::is_constructible<T, optional<U>>,
              std::is_constructible<T, const optional<U> &>,
              std::is_constructible<T, const optional<U>>, std::is_convertible<optional<U> &, T>,
              std::is_convertible<optional<U>, T>, std::is_convertible<const optional<U> &, T>,
              std::is_convertible<const optional<U>, T>>>> {};

template <>
struct optional_construct_with<void, void> : std::true_type {};

template <typename T, typename U>
inline constexpr bool optional_construct_with_v = optional_construct_with<T, U>::value;

template <typename T>
struct is_not_nullopt_t : std::true_type {};

template <>
struct is_not_nullopt_t<nullopt_t> : std::false_type {};

template <typename T>
struct is_not_optional : std::true_type {};

template <typename T>
struct is_not_optional<optional<T>> : std::false_type {};

template <typename T, typename U, typename VU = remove_cvref_t<U>>
struct optional_construct_with_arg
    : std::conjunction<std::negation<std::is_same<VU, std::in_place_t>>,
                       std::negation<std::is_same<VU, optional<T>>>, std::is_constructible<T, U>,
                       is_not_nullopt_t<VU>,
                       std::disjunction<std::is_same<VU, bool>, is_not_optional<VU>>> {};

template <typename T, typename U>
inline constexpr bool optional_construct_with_arg_v = optional_construct_with_arg<T, U>::value;

} // namespace optional_detail

template <typename Func, typename... Args>
using _optional_result = std::remove_cv_t<std::invoke_result_t<Func, Args...>>;

template <typename T>
class WJR_EMPTY_BASES optional : optional_detail::optional_storage<T>,
                                 optional_detail::enable_optional_storage<_optional_unwrap_t<T>> {
    using Mybase = optional_detail::optional_storage<T>;

    template <typename OT>
    friend class optional;

public:
    using value_type = typename Mybase::value_type;

    optional() = default;
    optional(const optional &) = default;
    optional(optional &&) = default;
    optional &operator=(const optional &) = default;
    optional &operator=(optional &&) = default;
    ~optional() = default;

    template <typename... Args>
    requires(std::is_constructible_v<value_type, Args...>)
    constexpr explicit optional(std::in_place_t, Args &&...args)
        : Mybase(std::in_place, std::forward<Args>(args)...) {}

    template <typename U, typename... Args>
    requires(std::is_constructible_v<value_type, std::initializer_list<U> &, Args...>)
    constexpr explicit optional(std::in_place_t, std::initializer_list<U> il, Args &&...args)
        : Mybase(std::in_place, il, std::forward<Args>(args)...) {}

    constexpr optional(nullopt_t) : Mybase(nullopt) {}

#if defined(__cpp_conditional_explicit)
    template <typename U>
    requires(std::is_constructible_v<value_type, const U &> &&
             optional_detail::optional_construct_with_v<value_type, U>)
    WJR_CONSTEXPR20 explicit(!std::is_convertible_v<const U &, value_type>)
        optional(const optional<U> &other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<const U &>(other.m_val));
        } else {
            this->construct_error();
        }
    }

    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             optional_detail::optional_construct_with_v<value_type, U>)
    WJR_CONSTEXPR20 explicit(!std::is_convertible_v<U, value_type>) optional(optional<U> &&other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<U>(other.m_val));
        } else {
            this->construct_error();
        }
    }
#else
    template <typename U>
    requires(std::is_constructible_v<value_type, const U &> &&
             optional_detail::optional_construct_with_v<value_type, U> &&
             std::is_convertible_v<const U &, value_type>)
    WJR_CONSTEXPR20 optional(const optional<U> &other) : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<const U &>(other.m_val));
        } else {
            this->construct_error();
        }
    }

    template <typename U>
    requires(std::is_constructible_v<value_type, const U &> &&
             optional_detail::optional_construct_with_v<value_type, U> &&
             !std::is_convertible_v<const U &, value_type>)
    WJR_CONSTEXPR20 explicit optional(const optional<U> &other)
        : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<const U &>(other.m_val));
        } else {
            this->construct_error();
        }
    }

    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             optional_detail::optional_construct_with_v<value_type, U> &&
             std::is_convertible_v<U, value_type>)
    WJR_CONSTEXPR20 optional(optional<U> &&other) : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<U>(other.m_val));
        } else {
            this->construct_error();
        }
    }

    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             optional_detail::optional_construct_with_v<value_type, U> &&
             !std::is_convertible_v<U, value_type>)
    WJR_CONSTEXPR20 explicit optional(optional<U> &&other) : Mybase(enable_default_constructor) {
        if (other.has_value()) {
            this->construct_value(std::forward<U>(other.m_val));
        } else {
            this->construct_error();
        }
    }
#endif

#if defined(__cpp_conditional_explicit)
    template <typename U = value_type>
    requires(optional_detail::optional_construct_with_arg_v<value_type, U>)
    constexpr explicit(!std::is_convertible_v<U, value_type>) optional(U &&v)
        : Mybase(std::in_place, std::forward<U>(v)) {}
#else
    template <typename U = value_type>
    requires(optional_detail::optional_construct_with_arg_v<value_type, U> &&
             std::is_convertible_v<U, value_type>)
    constexpr optional(U &&v) : Mybase(std::in_place, std::forward<U>(v)) {}

    template <typename U = value_type>
    requires(optional_detail::optional_construct_with_arg_v<value_type, U> &&
             !std::is_convertible_v<U, value_type>)
    constexpr explicit optional(U &&v) : Mybase(std::in_place, std::forward<U>(v)) {}
#endif

    WJR_CONSTEXPR20 optional &operator=(nullopt_t) {
        if (has_value()) {
            std::destroy_at(std::addressof(this->m_val));
            this->set_invalid();
        }

        return *this;
    }

    template <typename U>
    requires(!std::is_same_v<remove_cvref_t<U>, optional> &&
             optional_detail::is_not_nullopt_t<remove_cvref_t<U>>::value &&
             std::is_constructible_v<value_type, U> && std::is_assignable_v<value_type &, U> &&
             (std::is_nothrow_constructible_v<value_type, U> ||
              std::is_nothrow_move_constructible_v<value_type>))
    WJR_CONSTEXPR20 optional &operator=(U &&v) {
        if (has_value()) {
            this->m_val = std::forward<U>(v);
        } else {
            wjr::construct_at(std::addressof(this->m_val), std::forward<U>(v));
            this->set_valid();
        }

        return *this;
    }

    using Mybase::has_value;
    constexpr explicit operator bool() const noexcept { return has_value(); }

    constexpr value_type *operator->() noexcept { return std::addressof(this->m_val); }
    constexpr const value_type *operator->() const noexcept { return std::addressof(this->m_val); }

    constexpr value_type &operator*() & noexcept {
        WJR_ASSERT(has_value());
        return this->m_val;
    }
    constexpr const value_type &operator*() const & noexcept {
        WJR_ASSERT(has_value());
        return this->m_val;
    }
    WJR_PURE constexpr value_type &&operator*() && noexcept {
        WJR_ASSERT(has_value());
        return std::move(this->m_val);
    }
    constexpr const value_type &&operator*() const && noexcept {
        WJR_ASSERT(has_value());
        return std::move(this->m_val);
    }

    constexpr value_type &value() & {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_optional_access{});
        }

        return this->m_val;
    }

    constexpr const value_type &value() const & {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_optional_access{});
        }

        return this->m_val;
    }

    constexpr value_type &&value() && {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_optional_access{});
        }

        return std::move(this->m_val);
    }

    constexpr const value_type &&value() const && {
        if (WJR_UNLIKELY(!has_value())) {
            WJR_THROW(bad_optional_access{});
        }

        return std::move(this->m_val);
    }

    template <typename U>
    constexpr value_type value_or(U &&default_value) const & {
        return has_value() ? **this : static_cast<value_type>(std::forward<U>(default_value));
    }

    template <typename U>
    constexpr value_type value_or(U &&default_value) && {
        return has_value() ? std::move(**this)
                           : static_cast<value_type>(std::forward<U>(default_value));
    }

    template <typename Func, typename U = _optional_result<Func, value_type &>>
    constexpr U and_then(Func &&func) & {
        if (WJR_LIKELY(has_value())) {
            return std::invoke(std::forward<Func>(func), this->m_val);
        }

        return U{};
    }

    template <typename Func, typename U = _optional_result<Func, const value_type &>>
    constexpr U and_then(Func &&func) const & {
        if (WJR_LIKELY(has_value())) {
            return std::invoke(std::forward<Func>(func), this->m_val);
        }

        return U{};
    }

    template <typename Func, typename U = _optional_result<Func, value_type &&>>
    constexpr U and_then(Func &&func) && {
        if (WJR_LIKELY(has_value())) {
            return std::invoke(std::forward<Func>(func), std::move(this->m_val));
        }

        return U{};
    }

    template <typename Func, typename U = _optional_result<Func, const value_type &&>>
    constexpr U and_then(Func &&func) const && {
        if (WJR_LIKELY(has_value())) {
            return std::invoke(std::forward<Func>(func), std::move(this->m_val));
        }

        return U{};
    }

    template <typename Func, typename U = _optional_result<Func>>
    constexpr U or_else(Func &&func) & {
        if (WJR_UNLIKELY(!has_value())) {
            return std::invoke(std::forward<Func>(func));
        }

        return U(std::in_place, this->m_val);
    }

    template <typename Func, typename U = _optional_result<Func>>
    constexpr U or_else(Func &&func) const & {
        if (WJR_UNLIKELY(!has_value())) {
            return std::invoke(std::forward<Func>(func));
        }

        return U(std::in_place, this->m_val);
    }

    template <typename Func, typename U = _optional_result<Func>>
    constexpr U or_else(Func &&func) && {
        if (WJR_UNLIKELY(!has_value())) {
            return std::invoke(std::forward<Func>(func));
        }

        return U(std::in_place, std::move(this->m_val));
    }

    template <typename Func, typename U = _optional_result<Func>>
    constexpr U or_else(Func &&func) const && {
        if (WJR_UNLIKELY(!has_value())) {
            return std::invoke(std::forward<Func>(func));
        }

        return U(std::in_place, std::move(this->m_val));
    }

    template <typename Func, typename U = _optional_result<Func, value_type &>>
    constexpr optional<U> transform(Func &&func) & {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func), this->m_val);
        }

        return optional<U>{};
    }

    template <typename Func, typename U = _optional_result<Func, const value_type &>>
    constexpr optional<U> transform(Func &&func) const & {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func), this->m_val);
        }

        return optional<U>{};
    }

    template <typename Func, typename U = _optional_result<Func, value_type &&>>
    constexpr optional<U> transform(Func &&func) && {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func), std::move(this->m_val));
        }

        return optional<U>{};
    }

    template <typename Func, typename U = _optional_result<Func, const value_type &&>>
    constexpr optional<U> transform(Func &&func) const && {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func), std::move(this->m_val));
        }

        return optional<U>{};
    }

    template <typename... Args>
    requires(std::is_constructible_v<value_type, Args...>)
    constexpr value_type &emplace(Args &&...args) noexcept {
        if (has_value()) {
            std::destroy_at(std::addressof(this->m_val));
        } else {
            this->set_valid();
        }

        wjr::construct_at(std::addressof(this->m_val), std::forward<Args>(args)...);
        return this->m_val;
    }

    template <typename U, typename... Args>
    requires(std::is_constructible_v<T, std::initializer_list<U> &, Args...>)
    constexpr T &emplace(std::initializer_list<U> il, Args &&...args) noexcept {
        if (has_value()) {
            std::destroy_at(std::addressof(this->m_val));
        } else {
            this->set_valid();
        }

        wjr::construct_at(std::addressof(this->m_val), il, std::forward<Args>(args)...);
        return this->m_val;
    }

private:
    WJR_CONSTEXPR20 void
    _swap_impl(optional &other) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                         std::is_nothrow_swappable_v<T>) {
        wjr::construct_at(std::addressof(other.m_val), std::move(this->m_val));
        std::destroy_at(std::addressof(this->m_val));
        this->set_invalid();
        other.set_valid();
    }

public:
    template <typename _T = value_type>
    requires(std::is_swappable_v<_T> && std::is_move_constructible_v<_T> &&
             std::is_nothrow_move_constructible_v<_T>)
    WJR_CONSTEXPR20 void
    swap(optional &other) noexcept(std::is_nothrow_move_constructible_v<value_type> &&
                                   std::is_nothrow_swappable_v<value_type>) {
        using std::swap;
        if (has_value()) {
            if (other.has_value()) {
                std::swap(this->m_val, other.m_val);
            } else {
                _swap_impl(other);
            }
        } else {
            if (!other.has_value()) {
            } else {
                other._swap_impl(*this);
            }
        }
    }

    template <typename T2>
    friend constexpr bool operator==(const optional &lhs, const optional<T2> &rhs) {
        if (WJR_UNLIKELY(lhs.has_value() != rhs.has_value())) {
            return false;
        }

        if (lhs.has_value()) {
            return *lhs == *rhs;
        }

        return true;
    }

    friend constexpr bool operator==(const optional &lhs, nullopt_t) noexcept {
        return !lhs.has_value();
    }
    friend constexpr bool operator==(nullopt_t, const optional &rhs) noexcept {
        return rhs == nullopt;
    }

    template <typename U>
    requires(optional_detail::is_not_optional<remove_cvref_t<U>>::value &&
             optional_detail::is_not_nullopt_t<remove_cvref_t<U>>::value)
    friend constexpr bool operator==(const optional &lhs, const U &rhs) {
        return lhs.has_value() && *lhs == rhs;
    }

    template <typename U>
    requires(optional_detail::is_not_optional<remove_cvref_t<U>>::value &&
             optional_detail::is_not_nullopt_t<remove_cvref_t<U>>::value)
    friend constexpr bool operator==(const U &lhs, const optional &rhs) {
        return rhs == lhs;
    }
};

template <>
class WJR_EMPTY_BASES optional<void> : optional_detail::optional_storage<void>,
                                       optional_detail::enable_optional_storage<void> {
    using Mybase = optional_detail::optional_storage<void>;

    template <typename OT>
    friend class optional;

public:
    using value_type = void;

    optional() = default;
    optional(const optional &) = default;
    optional(optional &&) = default;
    optional &operator=(const optional &) = default;
    optional &operator=(optional &&) = default;
    ~optional() = default;

    constexpr explicit optional(std::in_place_t) : Mybase(std::in_place) {}
    constexpr optional(voidopt_t) : Mybase(std::in_place) {}
    constexpr optional(nullopt_t) : Mybase(nullopt) {}

    WJR_CONSTEXPR20 optional &operator=(nullopt_t) {
        this->set_invalid();
        return *this;
    }

    using Mybase::has_value;
    constexpr explicit operator bool() const noexcept { return has_value(); }

    template <typename Func, typename U = _optional_result<Func>>
    constexpr U and_then(Func &&func) const {
        if (WJR_LIKELY(has_value())) {
            return std::invoke(std::forward<Func>(func));
        }

        return U{};
    }

    template <typename Func, typename U = _optional_result<Func>>
    constexpr U or_else(Func &&func) const {
        if (WJR_UNLIKELY(!has_value())) {
            return std::invoke(std::forward<Func>(func));
        }

        return U(std::in_place);
    }

    template <typename Func, typename U = _optional_result<Func>>
    constexpr optional<U> transform(Func &&func) const {
        if (has_value()) {
            return std::invoke(std::forward<Func>(func));
        }

        return optional<U>{};
    }

    constexpr void emplace() noexcept { this->set_valid(); }

    WJR_CONSTEXPR20 void swap(optional &other) noexcept {
        std::swap(this->m_has_val, other.m_has_val);
    }

    template <typename T2>
    friend constexpr bool operator==(const optional &lhs, const optional<T2> &rhs) {
        return lhs.has_value() == rhs.has_value();
    }

    friend constexpr bool operator==(const optional &lhs, nullopt_t) noexcept {
        return !lhs.has_value();
    }
    friend constexpr bool operator==(nullopt_t, const optional &rhs) noexcept {
        return rhs == nullopt;
    }
};

template <typename T, typename = void>
struct compressed_optional_value {};

template <typename T>
struct compressed_optional_value<T *, void> {
    using type = compressed_value<T *, nullptr>;
};

template <typename T>
using compressed_optional_value_t = typename compressed_optional_value<T>::type;

template <typename T>
using compressed_optional = optional<compressed_optional_value_t<T>>;

#define WJR_OPTIONAL_TRY(...)                                                                      \
    do {                                                                                           \
        if (auto exp = (__VA_ARGS__); WJR_UNLIKELY(!exp)) {                                        \
            return ::wjr::nullopt;                                                                 \
        }                                                                                          \
    } while (0)

#define WJR_OPTIONAL_INIT(NAME, ...)                                                               \
    auto NAME = (__VA_ARGS__);                                                                     \
    if (WJR_UNLIKELY(!NAME)) {                                                                     \
        return ::wjr::nullopt;                                                                     \
    }

#define WJR_OPTIONAL_SET(VAR, ...)                                                                 \
    do {                                                                                           \
        if (auto exp = (__VA_ARGS__); WJR_UNLIKELY(!exp)) {                                        \
            return ::wjr::nullopt;                                                                 \
        } else {                                                                                   \
            VAR = *std::move(exp);                                                                 \
        }                                                                                          \
    } while (0)

} // namespace wjr

namespace std {

template <typename T>
constexpr void swap(wjr::optional<T> &lhs,
                    wjr::optional<T> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace std

#endif // WJR_OPTIONAL_HPP__