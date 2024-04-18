#ifndef WJR_RESULT_HPP__
#define WJR_RESULT_HPP__

#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

#include <wjr/compressed_pair.hpp>
#include <wjr/error_format.hpp>
#include <wjr/tuple.hpp>

namespace wjr {

struct result_monostate {};

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os,
                                              const result_monostate &) {
    return os << "unhandled error result";
}

class result_exception : public std::runtime_error {
    using Mybase = std::runtime_error;

public:
    using Mybase::Mybase;
};

struct result_exception_error_handler {
    template <typename Err>
    void operator()(Err &&err) {
        std::stringstream ss;
        error_format<remove_cvref_t<Err>>::format(ss, std::forward<Err>(err));
        throw result_exception(ss.str());
    }

    void operator()(const char *err) { throw result_exception(err); }
    void operator()(const std::string &err) { throw result_exception(err); }
};

struct result_abort_error_handler {
    template <typename Err>
    WJR_NORETURN void operator()(Err &&err) {
        std::cerr << err << std::endl;
        std::abort();
    }
};

template <typename Ret, typename Err>
struct result_traits {};

template <typename Ret>
struct result_traits<Ret, std::errc> {
    static constexpr bool is_err_ok(std::errc err) noexcept { return err == std::errc{}; }
    static constexpr void reset_err(std::errc &err) noexcept { err = std::errc{}; }
};

template <typename Ret>
struct result_traits<Ret, const char *> {
    static constexpr bool is_err_ok(const char *err) noexcept { return err == nullptr; }
    static constexpr void reset_err(const char *&err) noexcept { err = nullptr; }
};

namespace result_details {

WJR_REGISTER_HAS_TYPE(is_ret_ok,
                      (result_traits<Ret, Err>::is_ret_ok(std::declval<Ret>())), Ret,
                      Err);
WJR_REGISTER_HAS_TYPE(is_err_ok,
                      (result_traits<Ret, Err>::is_err_ok(std::declval<Err>())), Ret,
                      Err);

} // namespace result_details

template <typename Ret, typename Err, typename ErrorHandler, size_t I>
class basic_result_base;

template <typename... Args>
class result_ok {
public:
    result_ok(const Args &...args) : m_tp(args...) {}

    template <size_t idx>
    constexpr decltype(auto) get() && {
        return std::move(m_tp).template get<idx>();
    }

private:
    tuple<Args...> m_tp;
};

template <typename... Args>
result_ok(Args...) -> result_ok<Args...>;

template <typename... Args>
class result_err {
public:
    result_err(const Args &...args) : m_tp(args...) {}

    template <size_t idx>
    constexpr decltype(auto) get() && {
        return std::move(m_tp).template get<idx>();
    }

private:
    tuple<Args...> m_tp;
};

template <typename... Args>
result_err(Args...) -> result_err<Args...>;

enum result_id_index {
    RESULT_OK,
    RESULT_ERR,
};

template <typename Ret, typename Err, typename ErrorHandler>
class basic_result_base<Ret, Err, ErrorHandler, 0>
    : enable_special_members_of_args_base<basic_result_base<Ret, Err, ErrorHandler, 0>,
                                          std::variant<Err, Ret>> {
    using Storage = std::variant<Err, Ret>;
    using Mybase =
        enable_special_members_of_args_base<basic_result_base<Ret, Err, ErrorHandler, 0>,
                                            Storage>;

public:
    using Mybase::Mybase;

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<Storage, Args...>)>
    constexpr basic_result_base(Args &&...args)
        : m_storage(std::forward<Args>(args)...) {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<
                                             Storage, std::in_place_index_t<1>, Args...>)>
    constexpr basic_result_base(std::in_place_index_t<RESULT_OK>, Args &&...args)
        : m_storage(std::in_place_index<1>, std::forward<Args>(args)...) {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<
                                             Storage, std::in_place_index_t<0>, Args...>)>
    constexpr basic_result_base(std::in_place_index_t<RESULT_ERR>, Args &&...args)
        : m_storage(std::in_place_index<0>, std::forward<Args>(args)...) {}

    constexpr bool is_ok() const noexcept { return m_storage.index() == 1; }
    constexpr bool is_err() const noexcept { return m_storage.index() == 0; }

    constexpr void check() const & {
        if (WJR_UNLIKELY(is_err())) {
            ErrorHandler{}(std::get<0>(m_storage));
            WJR_UNREACHABLE();
        }
    }

    constexpr void check() && {
        if (WJR_UNLIKELY(is_err())) {
            ErrorHandler{}(std::move(std::get<0>(m_storage)));
            WJR_UNREACHABLE();
        }
    }

    constexpr Ret &value_unsafe() & { return std::get<1>(m_storage); }
    constexpr const Ret &value_unsafe() const & { return std::get<1>(m_storage); }
    constexpr Ret &&value_unsafe() && { return std::get<1>(std::move(m_storage)); }
    constexpr const Ret &&value_unsafe() const && {
        return std::move(std::get<1>(m_storage));
    }

    constexpr Err &error_unsafe() & { return std::get<0>(m_storage); }
    constexpr const Err &error_unsafe() const & { return std::get<0>(m_storage); }
    constexpr Err &&error_unsafe() && { return std::get<0>(std::move(m_storage)); }
    constexpr const Err &&error_unsafe() const && {
        return std::move(std::get<0>(m_storage));
    }

    constexpr void
    swap(basic_result_base &other) noexcept(std::is_nothrow_swappable_v<Storage>) {
        m_storage.swap(other.m_storage);
    }

    constexpr void reset() noexcept { m_storage.reset(); }

private:
    Storage m_storage;
};

template <typename Ret, typename Err, typename ErrorHandler>
class basic_result_base<Ret, Err, ErrorHandler, 1>
    : enable_special_members_of_args_base<basic_result_base<Ret, Err, ErrorHandler, 0>,
                                          Ret, Err> {
    using Mybase =
        enable_special_members_of_args_base<basic_result_base<Ret, Err, ErrorHandler, 0>,
                                            Ret, Err>;
    using Traits = result_traits<Ret, Err>;

public:
    using Mybase::Mybase;

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<Ret, Args...>)>
    constexpr basic_result_base(std::in_place_index_t<RESULT_OK>, Args &&...args)
        : m_ok(std::forward<Args>(args)...) {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<Err, Args...>)>
    constexpr basic_result_base(std::in_place_index_t<RESULT_ERR>, Args &&...args)
        : m_ok(), m_err(std::forward<Args>(args)...) {}

    constexpr basic_result_base(const basic_result_base &other)
        : m_ok(other.m_ok), m_err(other.m_err) {}

    constexpr basic_result_base(basic_result_base &&other) noexcept(
        std::is_nothrow_move_constructible_v<Ret>
            &&std::is_nothrow_move_constructible_v<Err>)
        : m_ok(std::move(other.m_ok)), m_err(std::move(other.m_err)) {
        Traits::reset_ok(other.m_ok);
    }

    constexpr basic_result_base &operator=(const basic_result_base &other) {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        m_ok = other.m_ok;
        m_err = other.m_err;
        return *this;
    }

    constexpr basic_result_base &operator=(basic_result_base &&other) noexcept(
        std::is_nothrow_move_assignable_v<Ret> &&std::is_nothrow_move_assignable_v<Err>) {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        m_ok = std::move(other.m_ok);
        m_err = std::move(other.m_err);
        Traits::reset_ok(other.m_ok);
        return *this;
    }

    constexpr bool is_ok() const noexcept { return Traits::is_ret_ok(m_ok); }
    constexpr bool is_err() const noexcept { return !is_ok(); }

    constexpr void check() const & {
        if (WJR_UNLIKELY(is_err())) {
            ErrorHandler{}(m_err);
            WJR_UNREACHABLE();
        }
    }

    constexpr void check() && {
        if (WJR_UNLIKELY(is_err())) {
            ErrorHandler{}(std::move(m_err));
            WJR_UNREACHABLE();
        }
    }

    constexpr Ret &value_unsafe() & { return m_ok; }
    constexpr const Ret &value_unsafe() const & { return m_ok; }
    constexpr Ret &&value_unsafe() && { return std::move(m_ok); }
    constexpr const Ret &&value_unsafe() const && { return std::move(m_ok); }

    constexpr Err &error_unsafe() & { return m_err; }
    constexpr const Err &error_unsafe() const & { return m_err; }
    constexpr Err &&error_unsafe() && { return std::move(m_err); }
    constexpr const Err &&error_unsafe() const && { return std::move(m_err); }

    constexpr void swap(basic_result_base &other) {
        using std::swap;
        swap(m_ok, other.m_ok);
        swap(m_err, other.m_err);
    }

    constexpr void reset() { this->~basic_result_base(); }

private:
    Ret m_ok;
    Err m_err;
};

template <typename Ret, typename Err, typename ErrorHandler>
class basic_result_base<Ret, Err, ErrorHandler, 2>
    : enable_special_members_of_args_base<basic_result_base<Ret, Err, ErrorHandler, 0>,
                                          Ret, Err> {
    using Mybase =
        enable_special_members_of_args_base<basic_result_base<Ret, Err, ErrorHandler, 0>,
                                            Ret, Err>;
    using Traits = result_traits<Ret, Err>;

public:
    using Mybase::Mybase;

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<Ret, Args...>)>
    constexpr basic_result_base(std::in_place_index_t<RESULT_OK>, Args &&...args)
        : m_err(), m_ok(std::forward<Args>(args)...) {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<Err, Args...>)>
    constexpr basic_result_base(std::in_place_index_t<RESULT_ERR>, Args &&...args)
        : m_err(std::forward<Args>(args)...) {}

    constexpr basic_result_base(const basic_result_base &other)
        : m_err(other.m_err), m_ok(other.m_ok) {}

    constexpr basic_result_base(basic_result_base &&other) noexcept(
        std::is_nothrow_move_constructible_v<Ret>
            &&std::is_nothrow_move_constructible_v<Err>)
        : m_err(std::move(other.m_err)), m_ok(std::move(other.m_ok)) {
        Traits::reset_err(other.m_err);
    }

    constexpr basic_result_base &operator=(const basic_result_base &other) {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        m_err = other.m_err;
        m_ok = other.m_ok;
        return *this;
    }

    constexpr basic_result_base &operator=(basic_result_base &&other) noexcept(
        std::is_nothrow_move_assignable_v<Ret> &&std::is_nothrow_move_assignable_v<Err>) {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        m_ok = std::move(other.m_ok);
        m_err = std::move(other.m_err);
        Traits::reset_err(other.m_err);
        return *this;
    }

    constexpr bool is_ok() const noexcept { return Traits::is_err_ok(m_err); }
    constexpr bool is_err() const noexcept { return !is_ok(); }

    constexpr void check() const & {
        if (WJR_UNLIKELY(is_err())) {
            ErrorHandler{}(m_err);
            WJR_UNREACHABLE();
        }
    }

    constexpr void check() && {
        if (WJR_UNLIKELY(is_err())) {
            ErrorHandler{}(std::move(m_err));
            WJR_UNREACHABLE();
        }
    }

    constexpr Ret &value_unsafe() & { return m_ok; }
    constexpr const Ret &value_unsafe() const & { return m_ok; }
    constexpr Ret &&value_unsafe() && { return std::move(m_ok); }
    constexpr const Ret &&value_unsafe() const && { return std::move(m_ok); }

    constexpr Err &error_unsafe() & { return m_err; }
    constexpr const Err &error_unsafe() const & { return m_err; }
    constexpr Err &&error_unsafe() && { return std::move(m_err); }
    constexpr const Err &&error_unsafe() const && { return std::move(m_err); }

    constexpr void swap(basic_result_base &other) {
        using std::swap;
        swap(m_ok, other.m_ok);
        swap(m_err, other.m_err);
    }

    constexpr void reset() { this->~basic_result_base(); }

private:
    Err m_err;
    Ret m_ok;
};

template <typename Ret, typename Err, typename Mybase>
class basic_result : Mybase {
public:
    using Mybase::Mybase;

    using Mybase::check;
    using Mybase::error_unsafe;
    using Mybase::is_err;
    using Mybase::is_ok;
    using Mybase::reset;
    using Mybase::swap;
    using Mybase::value_unsafe;

private:
    template <typename Tuple, typename ID, size_t... Indexs>
    constexpr basic_result(Tuple &&tp, ID id, std::index_sequence<Indexs...>)
        : Mybase(id, (std::move(tp).template get<Indexs>())...) {}

public:
    template <typename... Args,
              WJR_REQUIRES(std::is_constructible_v<
                           Mybase, std::in_place_index_t<RESULT_OK>, Args...>)>
    constexpr basic_result(result_ok<Args...> &&id)
        : basic_result(std::move(id), std::in_place_index<RESULT_OK>,
                       std::index_sequence_for<Args...>{}) {}

    template <typename... Args,
              WJR_REQUIRES(std::is_constructible_v<
                           Mybase, std::in_place_index_t<RESULT_ERR>, Args...>)>
    constexpr basic_result(result_err<Args...> &&id)
        : basic_result(std::move(id), std::in_place_index<RESULT_ERR>,
                       std::index_sequence_for<Args...>{}) {}

    template <typename... Args,
              WJR_REQUIRES(std::is_constructible_v<
                           Mybase, std::in_place_index_t<RESULT_OK>, Args...>)>
    constexpr basic_result &emplace(std::in_place_index_t<RESULT_OK>, Args &&...args) {
        this->reset();
        new (this)
            basic_result(std::in_place_index<RESULT_OK>, std::forward<Args>(args)...);
        return *this;
    }

    template <typename... Args,
              WJR_REQUIRES(std::is_constructible_v<
                           Mybase, std::in_place_index_t<RESULT_ERR>, Args...>)>
    constexpr basic_result &emplace(std::in_place_index_t<RESULT_ERR>, Args &&...args) {
        this->reset();
        new (this)
            basic_result(std::in_place_index<RESULT_ERR>, std::forward<Args>(args)...);
        return *this;
    }

    template <typename... Args,
              WJR_REQUIRES(std::is_constructible_v<
                           Mybase, std::in_place_index_t<RESULT_OK>, Args...>)>
    constexpr basic_result &emplace(result_ok<Args...> &&tp) {
        this->reset();
        new (this) basic_result(std::move(tp));
        return *this;
    }

    template <typename... Args,
              WJR_REQUIRES(std::is_constructible_v<
                           Mybase, std::in_place_index_t<RESULT_ERR>, Args...>)>
    constexpr basic_result &emplace(result_err<Args...> &&tp) {
        this->reset();
        new (this) basic_result(std::move(tp));
        return *this;
    }

    constexpr Ret &value() & {
        check();
        return value_unsafe();
    }

    constexpr const Ret &value() const & {
        check();
        return value_unsafe();
    }

    constexpr Ret &&value() && {
        check();
        return std::move(value_unsafe());
    }

    constexpr const Ret &&value() const && {
        check();
        return std::move(value_unsafe());
    }

    constexpr Ret &&take_value() && {
        check();
        return std::move(value_unsafe());
    }

    constexpr const Ret &&take_value() const && {
        check();
        return std::move(value_unsafe());
    }
};

template <typename Ret, typename Err>
inline constexpr int basic_result_base_selector_v =
    result_details::has_is_ret_ok_v<Ret, Err>
        ? 1
        : (result_details::has_is_err_ok_v<Ret, Err> ? 2 : 0);

/**
 * @details UB if not constructed. The default construction is not guaranteed to be
 * err or ok.
 *
 */
template <typename Ret, typename Err = result_monostate,
          typename ErrorHandler = result_exception_error_handler>
using result = basic_result<
    Ret, Err,
    basic_result_base<Ret, Err, ErrorHandler, basic_result_base_selector_v<Ret, Err>>>;

} // namespace wjr

#endif // WJR_RESULT_HPP__