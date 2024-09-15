#ifndef WJR_JSON_JSON_HPP__
#define WJR_JSON_JSON_HPP__

#include <map>

#include <wjr/json/formatter.hpp>
#include <wjr/json/visitor.hpp>
#include <wjr/variant.hpp>

namespace wjr::json {

template <typename Traits>
class basic_json;

template <typename T>
struct is_json : std::false_type {};

template <typename Traits>
struct is_json<basic_json<Traits>> : std::true_type {};

template <typename T>
inline constexpr bool is_json_v = is_json<T>::value;

namespace detail {

template <template <typename Char, typename Traits, typename Alloc> typename String,
          template <typename Key, typename Ty, typename Pr, typename Alloc>
          typename Object,
          template <typename T, typename Alloc> typename Array>
struct basic_json_traits {
private:
    using json_type = basic_json<basic_json_traits>;

public:
    using string_type = String<char, std::char_traits<char>, memory_pool<char>>;
    using object_type = Object<string_type, json_type, std::less<>,
                               memory_pool<std::pair<const string_type, json_type>>>;
    using array_type = Array<json_type, memory_pool<json_type>>;

    using value_type = json_type;
    using reference = value_type &;
    using const_reference = const value_type &;

    using size_type = uint32_t;

    static_assert(has_compare_is_transparent_v<std::less<>>, "");

    template <typename Key>
    struct is_other_key
        : std::conjunction<
              std::negation<std::is_convertible<Key, size_type>>,
              std::negation<std::is_same<string_type, remove_cvref_t<Key>>>,
              has_compare<typename object_type::key_compare, string_type, Key>,
              has_compare<typename object_type::key_compare, Key, string_type>> {};
};

using default_json_traits = basic_json_traits<std::basic_string, std::map, vector>;
using __default_json_string = typename default_json_traits::string_type;

template <typename Json>
class basic_json_parser;

class check_parser;

} // namespace detail

WJR_INTRINSIC_INLINE result<void> check(const reader &rd) noexcept;

using json = basic_json<detail::default_json_traits>;

template <typename T>
struct __json_get_impl;

#define WJR_REGISTER_JSON_GET_IMPL(T)                                                    \
    template <>                                                                          \
    struct __json_get_impl<T##_t> {                                                      \
        template <typename Json>                                                         \
        WJR_PURE WJR_INTRINSIC_CONSTEXPR static auto get(Json &&j) noexcept              \
            -> decltype(std::declval<Json &&>().__get_##T()) {                           \
            return std::forward<Json>(j).__get_##T();                                    \
        }                                                                                \
    }

WJR_REGISTER_JSON_GET_IMPL(boolean);
WJR_REGISTER_JSON_GET_IMPL(number_unsigned);
WJR_REGISTER_JSON_GET_IMPL(number_signed);
WJR_REGISTER_JSON_GET_IMPL(number_float);
WJR_REGISTER_JSON_GET_IMPL(string);
WJR_REGISTER_JSON_GET_IMPL(object);
WJR_REGISTER_JSON_GET_IMPL(array);

#undef WJR_REGISTER_JSON_GET_IMPL

class bad_json_access : public std::exception {
public:
    explicit bad_json_access(error_code e) : m_err(std::move(e)) {}

    const error_code &error() const & { return m_err; }
    error_code &error() & { return m_err; }
    const error_code &&error() const && { return std::move(m_err); }
    error_code &&error() && { return std::move(m_err); }

    WJR_NODISCARD virtual const char *what() const noexcept override {
        return "Bad json access";
    }

private:
    error_code m_err;
};

WJR_REGISTER_HAS_TYPE(Mybase_assign_from_json,
                      Mybase::assign(std::declval<Json>(), std::declval<T &>()), Mybase,
                      T, Json);
WJR_REGISTER_HAS_TYPE(Mybase_construct_from_json, Mybase::construct(std::declval<Json>()),
                      Mybase, T, Json);

WJR_REGISTER_HAS_TYPE(Mybase_assign_to_json,
                      Mybase::assign(std::declval<T>(), std::declval<Json &>()), Mybase,
                      Json, T);
WJR_REGISTER_HAS_TYPE(Mybase_construct_to_json,
                      Mybase::template construct<Json>(std::declval<T>()), Mybase, Json,
                      T);

struct in_place_json_serializer_t {};
inline constexpr in_place_json_serializer_t in_place_json_serializer{};

struct __in_place_json_serializer_object_t {};
inline constexpr __in_place_json_serializer_object_t __in_place_json_serializer_object{};

template <typename Mybase>
struct json_serializer_auto_completion : private Mybase {
    using value_type = typename Mybase::value_type;

    /// from_json

    /// Use functions of Mybase.

    template <typename Json,
              WJR_REQUIRES(has_Mybase_assign_from_json_v<Mybase, value_type, Json &&>)>
    constexpr static void assign(Json &&j, value_type &val) {
        Mybase::assign(std::forward<Json>(j), val);
    }

    template <typename Json,
              WJR_REQUIRES(has_Mybase_construct_from_json_v<Mybase, value_type, Json &&>)>
    constexpr static value_type construct(Json &&j) {
        return Mybase::construct(std::forward<Json>(j));
    }

    /// Use member functions of value_type

    template <typename Json,
              WJR_REQUIRES(!has_Mybase_assign_from_json_v<Mybase, value_type, Json &&> &&
                           std::is_assignable_v<value_type &, Json &&>)>
    constexpr static void assign(Json &&j, value_type &val) {
        val = std::forward<Json>(j);
    }

    template <
        typename Json,
        WJR_REQUIRES(
            !has_Mybase_construct_from_json_v<Mybase, value_type, Json &&> &&
            std::is_constructible_v<value_type, Json &&, in_place_json_serializer_t>)>
    constexpr static value_type construct(Json &&j) {
        return value_type(std::forward<Json>(j), in_place_json_serializer);
    }

    /// to_json

    /// Use functions of Mybase.

    template <typename Json, typename U = value_type,
              WJR_REQUIRES(has_Mybase_assign_to_json_v<Mybase, Json, U &&>)>
    constexpr static void assign(U &&val, Json &j) {
        Mybase::assign(std::forward<U>(val), j);
    }

    template <typename Json, typename U = value_type,
              WJR_REQUIRES(has_Mybase_construct_to_json_v<Mybase, Json, U &&>)>
    constexpr static Json construct(U &&val) {
        return Mybase::template construct<Json>(std::forward<U>(val));
    }

    /// Use static member functions of value_type

    template <typename Json, typename U = value_type,
              WJR_REQUIRES(!has_Mybase_assign_to_json_v<Mybase, Json, U &&> &&
                           has_Mybase_assign_to_json_v<value_type, Json, U &&>)>
    constexpr static void assign(U &&val, Json &j) {
        value_type::assign(std::forward<U>(val), j);
    }

    template <typename Json, typename U = value_type,
              WJR_REQUIRES(!has_Mybase_construct_to_json_v<Mybase, Json, U &&> &&
                           has_Mybase_construct_to_json_v<value_type, Json, U &&>)>
    constexpr static Json construct(U &&val) {
        return value_type::template construct<Json>(std::forward<U>(val));
    }
};

template <typename T, typename Enable = void>
struct json_serializer_impl {
    using value_type = T;
};

template <typename T>
using json_serializer =
    json_serializer_auto_completion<json_serializer_impl<remove_cvref_t<T>>>;

template <typename Arithmetic>
struct __json_serializer_arithmetic;

template <typename T>
struct __json_serializer_array;

template <typename T>
struct __json_serializer_object;

WJR_REGISTER_HAS_TYPE(assign_from_json,
                      json_serializer<T>::assign(std::declval<Json>(),
                                                 std::declval<T &>()),
                      T, Json);
WJR_REGISTER_HAS_TYPE(construct_from_json,
                      json_serializer<T>::construct(std::declval<Json>()), T, Json);

WJR_REGISTER_HAS_TYPE(assign_to_json,
                      json_serializer<T>::assign(std::declval<T>(),
                                                 std::declval<Json &>()),
                      Json, T);
WJR_REGISTER_HAS_TYPE(construct_to_json,
                      json_serializer<T>::template construct<Json>(std::declval<T>()),
                      Json, T);

#define __WJR_REGISTER_JSON_SERIALIZER_COPY_CONSTRUCTOR_CALLER(var)                      \
    var(__wjr_obj.at(#var))
#define __WJR_REGISTER_JSON_SERIALIZER_MOVE_CONSTRUCTOR_CALLER(var)                      \
    var(std::move(__wjr_obj.at(#var)))

#define WJR_REGISTER_JSON_SERIALIZER_COPY_CONSTRUCTOR(Type, ...)                         \
    template <typename Object>                                                           \
    explicit Type(const Object &__wjr_obj,                                               \
                  ::wjr::json::__in_place_json_serializer_object_t)                      \
        : WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                    \
              (__VA_ARGS__), __WJR_REGISTER_JSON_SERIALIZER_COPY_CONSTRUCTOR_CALLER)) {} \
    template <typename Traits>                                                           \
    explicit Type(const ::wjr::json::basic_json<Traits> &__wjr_json,                     \
                  ::wjr::json::in_place_json_serializer_t)                               \
        : Type(__wjr_json.template get<::wjr::json::object_t>(),                         \
               ::wjr::json::__in_place_json_serializer_object) {}

#define WJR_REGISTER_JSON_SERIALIZER_MOVE_CONSTRUCTOR(Type, ...)                         \
    template <typename Object>                                                           \
    explicit Type(Object &&__wjr_obj, ::wjr::json::__in_place_json_serializer_object_t)  \
        : WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                    \
              (__VA_ARGS__), __WJR_REGISTER_JSON_SERIALIZER_MOVE_CONSTRUCTOR_CALLER)) {} \
    template <typename Traits>                                                           \
    explicit Type(::wjr::json::basic_json<Traits> &&__wjr_json,                          \
                  ::wjr::json::in_place_json_serializer_t)                               \
        : Type(std::move(__wjr_json.template get<::wjr::json::object_t>()),              \
               ::wjr::json::__in_place_json_serializer_object) {}

#define __WJR_REGISTER_JSON_SERIALIZER_COPY_ASSIGN_CALLER(var)                           \
    __wjr_obj.at(#var).get_to(var)
#define __WJR_REGISTER_JSON_SERIALIZER_MOVE_ASSIGN_CALLER(var)                           \
    std::move(__wjr_obj.at(#var)).get_to(var)

#define WJR_REGISTER_JSON_SERIALIZER_COPY_ASSIGNMENT(Type, ...)                          \
    template <typename Object>                                                           \
    Type &__assign(const Object &__wjr_obj,                                              \
                   ::wjr::json::__in_place_json_serializer_object_t) {                   \
        WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                      \
            (__VA_ARGS__), __WJR_REGISTER_JSON_SERIALIZER_COPY_ASSIGN_CALLER));          \
        return *this;                                                                    \
    }                                                                                    \
    template <typename Traits>                                                           \
    Type &operator=(const ::wjr::json::basic_json<Traits> &__wjr_json) {                 \
        return __assign(__wjr_json.template get<::wjr::json::object_t>(),                \
                        ::wjr::json::in_place_json_serializer);                          \
    }

#define WJR_REGISTER_JSON_SERIALIZER_MOVE_ASSIGNMENT(Type, ...)                          \
    template <typename Object>                                                           \
    Type &__assign(Object &&__wjr_obj,                                                   \
                   ::wjr::json::__in_place_json_serializer_object_t) {                   \
        WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                      \
            (__VA_ARGS__), __WJR_REGISTER_JSON_SERIALIZER_MOVE_ASSIGN_CALLER));          \
        return *this;                                                                    \
    }                                                                                    \
    template <typename Traits>                                                           \
    Type &operator=(::wjr::json::basic_json<Traits> &&__wjr_json) {                      \
        return __assign(std::move(__wjr_json.template get<::wjr::json::object_t>()),     \
                        ::wjr::json::in_place_json_serializer);                          \
    }

#define WJR_REGISTER_JSON_SERIALIZER_DEFAULT(Type, ...)                                  \
    WJR_REGISTER_JSON_SERIALIZER_COPY_CONSTRUCTOR(Type, __VA_ARGS__)                     \
    WJR_REGISTER_JSON_SERIALIZER_MOVE_CONSTRUCTOR(Type, __VA_ARGS__)                     \
    WJR_REGISTER_JSON_SERIALIZER_COPY_ASSIGNMENT(Type, __VA_ARGS__)                      \
    WJR_REGISTER_JSON_SERIALIZER_MOVE_ASSIGNMENT(Type, __VA_ARGS__)

template <typename T, typename... Args>
T *__json_create(Args &&...args) noexcept(
    noexcept(std::declval<memory_pool<T>>().allocate(1)) &&
    std::is_nothrow_constructible_v<T, Args &&...>) {
    memory_pool<T> al;
    auto *const ptr = al.allocate(1);
    wjr::construct_at(ptr, std::forward<Args>(args)...);
    return ptr;
}

template <typename T>
void __json_destroy(T *ptr) noexcept(std::is_nothrow_destructible_v<T> && noexcept(
    std::declval<memory_pool<T>>().deallocate(std::declval<T *>(), 1))) {
    std::destroy_at(ptr);
    memory_pool<T> al;
    al.deallocate(ptr, 1);
}

namespace detail {
template <typename Formatter, typename Traits>
void format_impl(Formatter fmt, const basic_json<Traits> &j, unsigned int depth) noexcept;
}

template <typename Formatter, typename Traits>
void format(Formatter fmt, const basic_json<Traits> &j) noexcept {
    detail::format_impl(fmt, j, 0);
}

/**
 * @details At present, it's a simple but flexible implementation solution. This is not as
 * good as a only iteration in terms of performance. \n
 * Performance (use std::map and wjr::vector as container on my local x64 machine) : \n
 * twitter.json : 700~720 MB/s \n
 * canada.json : 660~680 MB/s
 * @todo 1. For most strings, only need to check first character to compare.
 * 2. Maybe use hash like std::unordered_map, but for long strings, compare even faster
 * than hash.
 * 3. Use B plus tree ?
 * 4. In place construct without using low performance basic_json.
 *
 */
template <typename Traits>
class basic_json {
    friend class detail::basic_json_parser<basic_json>;

    template <typename T>
    friend struct __json_get_impl;

    template <typename T, typename Enable>
    friend struct json_serializer_impl;

    template <typename Arithmetic>
    friend struct __json_serializer_arithmetic;

    template <typename T>
    friend struct __json_serializer_array;

    template <typename T>
    friend struct __json_serializer_object;

public:
    using traits_type = Traits;

    using null_type = std::nullptr_t;
    using boolean_type = bool;
    using number_unsigned_type = uint64_t;
    using number_signed_type = int64_t;
    using number_float_type = double;
    using string_type = typename traits_type::string_type;
    using object_type = typename traits_type::object_type;
    using array_type = typename traits_type::array_type;

    using value_type = typename traits_type::value_type;
    using reference = typename traits_type::reference;
    using const_reference = typename traits_type::const_reference;

    using size_type = typename traits_type::size_type;

    basic_json() = default;
    basic_json(const basic_json &other) {
        switch (other.type()) {
        case value_t::null:
        case value_t::number_unsigned:
        case value_t::number_signed:
        case value_t::number_float: {
            // use a simple memcpy
            m_value = other.m_value;
            break;
        }
        case value_t::string: {
            m_value.set(string_t(), __json_create<string_type>(other.__get_string()));
            break;
        }
        case value_t::object: {
            m_value.set(object_t(), __json_create<object_type>(other.__get_object()));
            break;
        }
        case value_t::array: {
            m_value.set(array_t(), __json_create<array_type>(other.__get_array()));
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }
    }

    basic_json(basic_json &&other) noexcept : m_value(other.m_value) {
        other.m_value.m_type = value_t::null;
    }

    basic_json &operator=(const basic_json &other) {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        switch (type()) {
        case value_t::null:
        case value_t::boolean:
        case value_t::number_unsigned:
        case value_t::number_signed:
        case value_t::number_float: {
            switch (other.type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                m_value = other.m_value;
                break;
            }
            case value_t::string: {
                m_value.set(string_t(), __json_create<string_type>(other.__get_string()));
                break;
            }
            case value_t::object: {
                m_value.set(string_t(), __json_create<object_type>(other.__get_object()));
                break;
            }
            case value_t::array: {
                m_value.set(string_t(), __json_create<array_type>(other.__get_array()));
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }
            break;
        }
        case value_t::string: {
            switch (other.type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                __json_destroy(std::addressof(__get_string()));
                m_value = other.m_value;
                break;
            }
            case value_t::string: {
                __get_string() = other.__get_string();
                break;
            }
            case value_t::object: {
                __json_destroy(std::addressof(__get_string()));
                m_value.set(string_t(), __json_create<object_type>(other.__get_object()));
                break;
            }
            case value_t::array: {
                __json_destroy(std::addressof(__get_string()));
                m_value.set(string_t(), __json_create<array_type>(other.__get_array()));
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }
            break;
        }
        case value_t::object: {
            switch (other.type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                __json_destroy(std::addressof(__get_object()));
                m_value = other.m_value;
                break;
            }
            case value_t::string: {
                __json_destroy(std::addressof(__get_object()));
                m_value.set(string_t(), __json_create<string_type>(other.__get_string()));
                break;
            }
            case value_t::object: {
                __get_object() = other.__get_object();
                break;
            }
            case value_t::array: {
                __json_destroy(std::addressof(__get_object()));
                m_value.set(string_t(), __json_create<array_type>(other.__get_array()));
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }
            break;
        }
        case value_t::array: {
            switch (other.type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                __json_destroy(std::addressof(__get_array()));
                m_value = other.m_value;
                break;
            }
            case value_t::string: {
                __json_destroy(std::addressof(__get_array()));
                m_value.set(string_t(), __json_create<string_type>(other.__get_string()));
                break;
            }
            case value_t::object: {
                __json_destroy(std::addressof(__get_array()));
                m_value.set(string_t(), __json_create<object_type>(other.__get_object()));
                break;
            }
            case value_t::array: {
                __get_array() = other.__get_array();
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return *this;
    }

    basic_json &operator=(basic_json &&other) noexcept {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        __destroy();
        m_value = other.m_value;
        other.m_value.m_type = value_t::null;
        return *this;
    }

    /**
     * @brief Destroy the basic json object
     * @todo Optimize by not using recursion
     *
     */
    ~basic_json() noexcept { __destroy(); }

    explicit basic_json(null_t) noexcept : m_value(null_t()) {}
    basic_json(boolean_t, bool f) noexcept : m_value(boolean_t(), f) {}
    basic_json(number_signed_t, int64_t value) noexcept
        : m_value(number_unsigned_t(), value) {}
    basic_json(number_unsigned_t, uint64_t value) noexcept
        : m_value(number_signed_t(), value) {}
    basic_json(number_float_t, double value) noexcept
        : m_value(number_float_t(), value) {}

    basic_json(string_t, string_type *ptr) noexcept : m_value(string_t(), ptr) {}
    basic_json(object_t, object_type *ptr) noexcept : m_value(object_t(), ptr) {}
    basic_json(array_t, array_type *ptr) noexcept : m_value(array_t(), ptr) {}

    explicit basic_json(basic_value value) noexcept : m_value(value) {}
    explicit basic_json(dctor_t) noexcept : basic_json() {}

    WJR_PURE value_t type() const noexcept { return m_value.m_type; }

    template <typename T>
    WJR_PURE decltype(auto) get_unsafe() noexcept {
        return __json_get_impl<T>::get(*this);
    }

    template <typename T>
    WJR_PURE decltype(auto) get_unsafe() const noexcept {
        return __json_get_impl<T>::get(*this);
    }

    template <typename T>
    WJR_PURE decltype(auto) get() {
        if (WJR_UNLIKELY(type() != T::value)) {
            WJR_THROW(bad_json_access(error_code::INCORRECT_TYPE));
        }

        return get_unsafe<T>();
    }

    template <typename T>
    WJR_PURE decltype(auto) get() const {
        if (WJR_UNLIKELY(type() != T::value)) {
            WJR_THROW(bad_json_access(error_code::INCORRECT_TYPE));
        }

        return get_unsafe<T>();
    }

    WJR_CONST static size_type max_depth_size() noexcept { return 256; }

    static result<basic_json> parse(const reader &rd) noexcept;

    template <typename Container>
    void dump_impl(Container &cont, unsigned indents = -1) const noexcept {
        static_assert(std::is_same_v<typename Container::value_type, char>, "");

        if (indents == -1u) {
            format(minify_formatter(std::back_inserter(cont)), *this);
        } else {
            format(pretty_formatter(std::back_inserter(cont), indents), *this);
        }
    }

    template <typename _Traits = std::char_traits<char>,
              typename Alloc = std::allocator<char>>
    WJR_PURE std::basic_string<char, _Traits, Alloc>
    dump(unsigned indents = -1) const noexcept {
        std::basic_string<char, _Traits, Alloc> str;
        dump_impl(str, indents);
        return str;
    }

    /**
     * @brief Use minify formatter as default.
     */
    template <typename _Traits = std::char_traits<char>,
              typename Alloc = std::allocator<char>>
    WJR_PURE std::basic_string<char, _Traits, Alloc> to_string() const noexcept {
        return dump<_Traits, Alloc>();
    }

    reference at(size_type idx) { return get<array_t>().at(idx); }
    const_reference at(size_type idx) const { return get<array_t>().at(idx); }

    reference at(const typename object_type::key_type &key) {
        return get<object_t>().at(key);
    }

    const_reference at(const typename object_type::key_type &key) const {
        return get<object_t>().at(key);
    }

    template <typename Key, WJR_REQUIRES(traits_type::template is_other_key<Key>::value)>
    reference at(const Key &key) {
        auto &obj = get<object_t>();
        const auto iter = obj.find(key);

        if (iter == obj.end()) {
            WJR_THROW(std::out_of_range("basic_json::at"));
        }

        return iter->second;
    }

    template <typename Key, WJR_REQUIRES(traits_type::template is_other_key<Key>::value)>
    const_reference at(const Key &key) const {
        const auto &obj = get<object_t>();
        const auto iter = obj.find(key);

        if (iter == obj.end()) {
            WJR_THROW(std::out_of_range("basic_json::at"));
        }

        return iter->second;
    }

    reference operator[](size_type idx) { return get<array_t>()[idx]; }
    const_reference operator[](size_type idx) const { return get<array_t>()[idx]; }

    reference operator[](const typename object_type::key_type &key) {
        return get<object_t>()[key];
    }

    reference operator[](typename object_type::key_type &&key) {
        return get<object_t>()[std::move(key)];
    }

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_construct_to_json_v<basic_json, T &&>)>
    explicit basic_json(T &&) = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_construct_to_json_v<basic_json, T &&>)>
    explicit basic_json(T &&val)
        : basic_json(
              json_serializer<T>::template construct<basic_json>(std::forward<T>(val))) {}

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_assign_to_json_v<basic_json, T &&>)>
    basic_json &operator=(T &&) = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_assign_to_json_v<basic_json, T &&>)>
    basic_json &operator=(T &&val) {
        json_serializer<T>::assign(std::forward<T>(val), *this);
        return *this;
    }

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_construct_from_json_v<T, const basic_json &>)>
    explicit operator T() & = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_construct_from_json_v<T, basic_json &&>)>
    explicit operator T() const & = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_construct_from_json_v<T, basic_json &&>)>
    explicit operator T() && = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_construct_from_json_v<T, basic_json &&>)>
    explicit operator T() const && = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_construct_from_json_v<T, const basic_json &>)>
    explicit operator T() & {
        return json_serializer<T>::construct(*this);
    }

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_construct_from_json_v<T, const basic_json &>)>
    explicit operator T() const & {
        return json_serializer<T>::construct(*this);
    }

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_construct_from_json_v<T, basic_json &&>)>
    explicit operator T() && {
        return json_serializer<T>::construct(std::move(*this));
    }

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_construct_from_json_v<T, basic_json &&>)>
    explicit operator T() const && {
        return json_serializer<T>::construct(std::move(*this));
    }

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_assign_from_json_v<T, const basic_json &>)>
    void get_to(T &) & = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_assign_from_json_v<T, const basic_json &>)>
    void get_to(T &) const & = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_assign_from_json_v<T, basic_json &&>)>
    void get_to(T &) && = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       !has_assign_from_json_v<T, basic_json &&>)>
    void get_to(T &) const && = delete;

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_assign_from_json_v<T, const basic_json &>)>
    void get_to(T &val) & {
        std::as_const(*this).get_to(val);
    }

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_assign_from_json_v<T, const basic_json &>)>
    void get_to(T &val) const & {
        json_serializer<T>::assign(*this, val);
    }

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_assign_from_json_v<T, basic_json &&>)>
    void get_to(T &val) && {
        json_serializer<T>::assign(std::move(*this), val);
    }

    template <typename T, WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_json> &&
                                       has_assign_from_json_v<T, basic_json &&>)>
    void get_to(T &val) const && {
        json_serializer<T>::assign(std::move(*this), val);
    }

    void get_to(basic_json &val) & { val = *this; }
    void get_to(basic_json &val) const & { val = *this; }
    void get_to(basic_json &val) && { val = std::move(*this); }
    void get_to(basic_json &val) const && { val = std::move(*this); }

    void reset() noexcept {
        __destroy();
        m_value.set(null_t());
    }

    void swap(basic_json &other) noexcept { std::swap(m_value, other.m_value); }

    WJR_PURE bool is_null() const noexcept { return type() == value_t::null; }
    WJR_PURE bool is_boolean() const noexcept { return type() == value_t::boolean; }
    WJR_PURE bool is_number_unsigned() const noexcept {
        return type() == value_t::number_unsigned;
    }
    WJR_PURE bool is_number_signed() const noexcept {
        return type() == value_t::number_signed;
    }
    WJR_PURE bool is_number_float() const noexcept {
        return type() == value_t::number_float;
    }

    WJR_PURE bool is_number() const noexcept {
        switch (type()) {
        case value_t::number_unsigned:
        case value_t::number_signed:
        case value_t::number_float: {
            return true;
        }
        default: {
            return false;
        }
        }
    }

    WJR_PURE bool is_string() const noexcept { return type() == value_t::string; }
    WJR_PURE bool is_object() const noexcept { return type() == value_t::object; }
    WJR_PURE bool is_array() const noexcept { return type() == value_t::array; }

private:
    void __destroy() noexcept {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P(type())) {
            switch (type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                return;
            }
            default: {
                break;
            }
            }
        }

        __destroy_impl();
    }

    void __destroy_impl() noexcept {
        switch (type()) {
        case value_t::null:
        case value_t::boolean:
        case value_t::number_unsigned:
        case value_t::number_signed:
        case value_t::number_float: {
            break;
        }
        case value_t::string: {
            __json_destroy(std::addressof(__get_string()));
            break;
        }
        case value_t::object: {
            __json_destroy(std::addressof(__get_object()));
            break;
        }
        case value_t::array: {
            __json_destroy(std::addressof(__get_array()));
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }
    }

    WJR_PURE boolean_type &__get_boolean() noexcept { return m_value.m_boolean; }
    WJR_PURE const boolean_type &__get_boolean() const noexcept {
        return m_value.m_boolean;
    }

    WJR_PURE number_unsigned_type &__get_number_unsigned() noexcept {
        return m_value.m_number_unsigned;
    }
    WJR_PURE const number_unsigned_type &__get_number_unsigned() const noexcept {
        return m_value.m_number_unsigned;
    }

    WJR_PURE number_signed_type &__get_number_signed() noexcept {
        return m_value.m_number_signed;
    }
    WJR_PURE const number_signed_type &__get_number_signed() const noexcept {
        return m_value.m_number_signed;
    }

    WJR_PURE number_float_type &__get_number_float() noexcept {
        return m_value.m_number_float;
    }
    WJR_PURE const number_float_type &__get_number_float() const noexcept {
        return m_value.m_number_float;
    }

    WJR_PURE string_type &__get_string() noexcept {
        return *static_cast<string_type *>(m_value.m_ptr);
    }
    WJR_PURE const string_type &__get_string() const noexcept {
        return *static_cast<const string_type *>(m_value.m_ptr);
    }

    WJR_PURE object_type &__get_object() noexcept {
        return *static_cast<object_type *>(m_value.m_ptr);
    }
    WJR_PURE const object_type &__get_object() const noexcept {
        return *static_cast<const object_type *>(m_value.m_ptr);
    }

    WJR_PURE array_type &__get_array() noexcept {
        return *static_cast<array_type *>(m_value.m_ptr);
    }
    WJR_PURE const array_type &__get_array() const noexcept {
        return *static_cast<const array_type *>(m_value.m_ptr);
    }

    basic_value m_value;
};

template <>
struct json_serializer_impl<std::nullptr_t, void> {
    using value_type = std::nullptr_t;

    /// from_json

    template <typename Json>
    constexpr static void assign(const Json &j, value_type &val) {
        if (WJR_UNLIKELY(!j.is_null())) {
            WJR_THROW(bad_json_access(error_code::TAPE_ERROR));
        }

        val = nullptr;
    }

    template <typename Json>
    WJR_PURE static value_type construct(const Json &j) {
        value_type val;
        assign(j, val);
        return val;
    }

    /// to_json

    template <typename Json>
    constexpr static void assign(const value_type &, Json &j) {
        j.reset();
        j.m_value.set(null_t());
    }

    template <typename Json>
    constexpr static Json construct(const value_type &) {
        return Json(null_t());
    }
};

template <>
struct json_serializer_impl<bool, void> {
    using value_type = bool;

    /// from_json

    template <typename Json>
    constexpr static void assign(const Json &j, value_type &val) {
        val = j.template get<boolean_t>();
    }

    template <typename Json>
    WJR_PURE static value_type construct(const Json &j) {
        value_type val;
        assign(j, val);
        return val;
    }

    /// to_json

    template <typename Json>
    constexpr static void assign(const value_type &val, Json &j) {
        j.reset();
        j.m_value.set(boolean_t(), val);
    }

    template <typename Json>
    constexpr static Json construct(const value_type &val) {
        return Json(boolean_t(), val);
    }
};

template <typename Arithmetic>
struct __json_serializer_arithmetic {
    using value_type = Arithmetic;

    /// from_json

    template <typename Json>
    constexpr static void assign(const Json &j, value_type &val) {
        switch (j.type()) {
        case value_t::number_unsigned: {
            val = static_cast<value_type>(j.template get_unsafe<number_unsigned_t>());
            break;
        }
        case value_t::number_signed: {
            val = static_cast<value_type>(j.template get_unsafe<number_signed_t>());
            break;
        }
        case value_t::number_float: {
            val = static_cast<value_type>(j.template get_unsafe<number_float_t>());
            break;
        }
        default: {
            WJR_THROW(bad_expected_access(error_code::TAPE_ERROR));
            WJR_UNREACHABLE();
            break;
        }
        }
    }

    template <typename Json>
    WJR_PURE static value_type construct(const Json &j) {
        value_type val;
        assign(j, val);
        return val;
    }

    /// to_json

    template <typename Json>
    constexpr static void assign(const value_type &val, Json &j) {
        j.reset();
        if constexpr (std::is_floating_point_v<value_type>) {
            j.m_value.set(number_float_t(), val);
        } else if constexpr (std::is_unsigned_v<value_type>) {
            j.m_value.set(number_unsigned_t(), val);
        } else {
            j.m_value.set(number_signed_t(), val);
        }
    }

    template <typename Json>
    constexpr static Json construct(const value_type &val) {
        if constexpr (std::is_floating_point_v<value_type>) {
            return Json(number_float_t(), val);
        } else if constexpr (std::is_unsigned_v<value_type>) {
            return Json(number_unsigned_t(), val);
        } else {
            return Json(number_signed_t(), val);
        }
    }
};

template <typename Arithmetic>
struct json_serializer_impl<Arithmetic,
                            std::enable_if_t<std::is_arithmetic_v<Arithmetic>>>
    : __json_serializer_arithmetic<Arithmetic> {};

template <>
struct json_serializer_impl<std::string_view, void> {
    using value_type = std::string_view;

    /// from_json

    template <typename Json>
    constexpr static void assign(const Json &j, value_type &val) {
        val = j.template get<string_t>();
    }

    template <typename Json, WJR_REQUIRES(!std::is_lvalue_reference_v<Json>)>
    constexpr static void assign(Json &&j, value_type &val) = delete;

    template <typename Json>
    constexpr static value_type construct(const Json &j) {
        return value_type(j.template get<string_t>());
    }

    template <typename Json, WJR_REQUIRES(!std::is_lvalue_reference_v<Json>)>
    constexpr static value_type construct(Json &&j) = delete;

    /// to_json

    template <typename Json>
    constexpr static void assign(const value_type &val, Json &j) {
        if (j.type() != value_t::string) {
            j.reset();
            j.m_value.set(string_t(), __json_create<typename Json::string_type>(val));
        } else {
            j.template get_unsafe<string_t>() = val;
        }
    }

    template <typename Json>
    WJR_PURE constexpr static Json construct(const value_type &val) {
        return Json(string_t(), __json_create<typename Json::string_type>(val));
    }
};

template <typename Alloc>
struct json_serializer_impl<std::basic_string<char, std::char_traits<char>, Alloc>,
                            void> {
    using value_type = std::basic_string<char, std::char_traits<char>, Alloc>;

    /// from_json

    template <typename Json>
    constexpr static void assign(const Json &j, value_type &val) {
        val = j.template get<string_t>();
    }

    template <typename Json>
    constexpr static void assign(Json &&j, value_type &val) {
        val = std::move(j.template get<string_t>());
    }

    template <typename Json>
    constexpr static value_type construct(const Json &j) {
        return value_type(j.template get<string_t>());
    }

    template <typename Json>
    constexpr static value_type construct(Json &&j) {
        return value_type(std::move(j.template get<string_t>()));
    }

    /// to_json

    template <typename Json>
    constexpr static void assign(const value_type &val, Json &j) {
        if (j.type() != value_t::string) {
            j.reset();
            j.m_value.set(string_t(), __json_create<typename Json::string_type>(val));
        } else {
            j.template get_unsafe<string_t>() = val;
        }
    }

    template <typename Json>
    constexpr static void assign(value_type &&val, Json &j) {
        if (j.type() != value_t::string) {
            j.reset();
            j.m_value.set(string_t(),
                          __json_create<typename Json::string_type>(std::move(val)));
        } else {
            j.template get_unsafe<string_t>() = std::move(val);
        }
    }

    template <typename Json>
    constexpr static Json construct(const value_type &val) {
        return Json(string_t(), __json_create<typename Json::string_type>(val));
    }

    template <typename Json>
    constexpr static Json construct(value_type &&val) {
        return Json(string_t(),
                    __json_create<typename Json::string_type>(std::move(val)));
    }
};

template <typename T>
struct __json_serializer_array {
    using value_type = T;

    /// from_json

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(const Json &j, value_type &val) {
        val = j.template get<array_t>();
    }

private:
    template <typename Iter>
    constexpr static void assign(Iter first, Iter last, value_type &val) {
        if constexpr (std::is_assignable_v<value_type &, iterator_reference_t<Iter>>) {
            val.assign(first, last);
        } else {
            using size_type = typename value_type::size_type;
            const auto n = static_cast<size_type>(last - first);
            if (n > val.capacity()) {
                val.clear();
                val.reserve(n);

                if constexpr (has_container_append_v<value_type, Iter, Iter>) {
                    append(val, first, last);
                } else {
                    for (; first != last; ++first) {
                        val.emplace_back(*first);
                    }
                }

                return;
            }

            auto val_first = val.begin();

            if (n > val.size()) {
                const auto val_last = val.end();
                for (; val_first != val_last; ++first, ++val_first) {
                    first->get_to(*val_first);
                }

                if constexpr (has_container_append_v<value_type, Iter, Iter>) {
                    append(val, first, last);
                } else {
                    for (; first != last; ++first) {
                        val.emplace_back(*first);
                    }
                }
            } else {
                val.resize(n);
                for (; first != last; ++first, ++val_first) {
                    first->get_to(*val_first);
                }
            }
        }
    }

public:
    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(const Json &j, value_type &val) {
        auto &arr = j.template get<array_t>();
        assign(arr.begin(), arr.end(), val);
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(Json &&j, value_type &val) {
        val = std::move(j.template get<array_t>());
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(Json &&j, value_type &val) {
        auto &arr = j.template get<array_t>();
        assign(std::make_move_iterator(arr.begin()), std::make_move_iterator(arr.end()),
               val);
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static value_type construct(const Json &j) {
        return j.template get<array_t>();
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static value_type construct(const Json &j) {
        auto &arr = j.template get<array_t>();
        return value_type(arr.begin(), arr.end());
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static value_type construct(Json &&j) {
        return std::move(j.template get<array_t>());
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static value_type construct(Json &&j) {
        auto &arr = j.template get<array_t>();
        return value_type(std::make_move_iterator(arr.begin()),
                          std::make_move_iterator(arr.end()));
    }

    /// to_json

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(const value_type &val, Json &j) {
        if (j.type() != value_t::array) {
            j.reset();
            j.m_value.set(array_t(), __json_create<typename Json::array_type>(val));
        } else {
            j.template get_unsafe<array_t>() = val;
        }
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(const value_type &val, Json &j) {
        if (j.type() != value_t::array) {
            j.reset();
            j.m_value.set(array_t(), __json_create<typename Json::array_type>(val.begin(),
                                                                              val.end()));
        } else {
            j.template get_unsafe<array_t>().assign(val.begin(), val.end());
        }
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &&val, Json &j) {
        if (j.type() != value_t::array) {
            j.reset();
            j.m_value.set(array_t(),
                          __json_create<typename Json::array_type>(std::move(val)));
        } else {
            j.template get_unsafe<array_t>() = std::move(val);
        }
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &&val, Json &j) {
        if (j.type() != value_t::array) {
            j.reset();
            j.m_value.set(array_t(), __json_create<typename Json::array_type>(
                                         std::make_move_iterator(val.begin()),
                                         std::make_move_iterator(val.end())));
        } else {
            j.template get_unsafe<array_t>().assign(std::make_move_iterator(val.begin()),
                                                    std::make_move_iterator(val.end()));
        }
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static Json construct(const value_type &val) {
        return Json(array_t(), __json_create<typename Json::array_type>(val));
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static Json construct(const value_type &val) {
        return Json(array_t(),
                    __json_create<typename Json::array_type>(val.begin(), val.end()));
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static Json construct(value_type &&val) {
        return Json(array_t(), __json_create<typename Json::array_type>(std::move(val)));
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static Json construct(value_type &&val) {
        return Json(array_t(), __json_create<typename Json::array_type>(
                                   std::make_move_iterator(val.begin()),
                                   std::make_move_iterator(val.end())));
    }
};

template <typename T, typename Alloc>
struct json_serializer_impl<std::vector<T, Alloc>, void>
    : __json_serializer_array<std::vector<T, Alloc>> {};

template <typename Traits>
struct json_serializer_impl<basic_vector<Traits>, void>
    : __json_serializer_array<basic_vector<Traits>> {};

template <typename T>
struct __json_serializer_object {
    using value_type = T;
    using read_only = std::false_type;

    /// from_json

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(const Json &j, value_type &val) {
        val = j.template get<object_t>();
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(const Json &j, value_type &val) {
        auto &obj = j.template get<object_t>();
        val.clear();
        for (const auto &elem : obj) {
            val.emplace(elem);
        }
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(Json &&j, value_type &val) {
        val = std::move(j.template get<object_t>());
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(Json &&j, value_type &val) {
        auto &obj = j.template get<object_t>();
        val.clear();
        for (auto &elem : obj) {
            val.emplace(std::move(elem));
        }
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static value_type construct(const Json &j) {
        return j.template get<object_t>();
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static value_type construct(const Json &j) {
        auto &obj = j.template get<object_t>();
        return value_type(obj.begin(), obj.end());
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static value_type construct(Json &&j) {
        return std::move(j.template get<object_t>());
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static value_type construct(Json &&j) {
        auto &obj = j.template get<object_t>();
        return value_type(std::make_move_iterator(obj.begin()),
                          std::make_move_iterator(obj.end()));
    }

    /// to_json

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(const value_type &val, Json &j) {
        j.reset();
        j.m_value.set(object_t(), __json_create<typename Json::object_type>(val));
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(const value_type &val, Json &j) {
        j.reset();
        j.m_value.set(object_t(),
                      __json_create<typename Json::object_type>(val.begin(), val.end()));
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &&val, Json &j) {
        j.reset();
        j.m_value.set(object_t(),
                      __json_create<typename Json::object_type>(std::move(val)));
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &&val, Json &j) {
        j.reset();
        j.m_value.set(object_t(), __json_create<typename Json::object_type>(
                                      std::make_move_iterator(val.begin()),
                                      std::make_move_iterator(val.end())));
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static Json construct(const value_type &val) {
        return Json(object_t(), __json_create<typename Json::object_type>(val));
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static Json construct(const value_type &val) {
        return Json(object_t(),
                    __json_create<typename Json::object_type>(val.begin(), val.end()));
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static Json construct(value_type &&val) {
        return Json(object_t(),
                    __json_create<typename Json::object_type>(std::move(val)));
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static Json construct(value_type &&val) {
        return Json(object_t(), __json_create<typename Json::object_type>(
                                    std::make_move_iterator(val.begin()),
                                    std::make_move_iterator(val.end())));
    }
};

template <typename Key, typename Value, typename Pr, typename Alloc>
struct json_serializer_impl<std::map<Key, Value, Pr, Alloc>, void>
    : __json_serializer_object<std::map<Key, Value, Pr, Alloc>> {};

template <typename Key, typename Value, typename Pr, typename Alloc>
struct json_serializer_impl<std::multimap<Key, Value, Pr, Alloc>, void>
    : __json_serializer_object<std::multimap<Key, Value, Pr, Alloc>> {};

template <typename Key, typename Value, typename Hash, typename Eq, typename Alloc>
struct json_serializer_impl<std::unordered_map<Key, Value, Hash, Eq, Alloc>, void>
    : __json_serializer_array<std::unordered_map<Key, Value, Hash, Eq, Alloc>> {};

template <typename Key, typename Value, typename Hash, typename Eq, typename Alloc>
struct json_serializer_impl<std::unordered_multimap<Key, Value, Hash, Eq, Alloc>, void>
    : __json_serializer_array<std::unordered_multimap<Key, Value, Hash, Eq, Alloc>> {};

namespace detail {

template <typename Json>
class basic_json_parser {
    template <typename Parser>
    friend result<void> visitor_detail::parse(Parser &&par, const reader &rd) noexcept;

    using json_type = Json;
    using string_type = typename json_type::string_type;
    using object_type = typename json_type::object_type;
    using array_type = typename json_type::array_type;

public:
    WJR_INTRINSIC_INLINE result<json_type> parse(const reader &rd) noexcept {
        json_type j;
        current = std::addressof(j);
        WJR_EXPECTED_TRY(visitor_detail::parse(*this, rd));
        return j;
    }

protected:
    WJR_INTRINSIC_INLINE result<void> visit_root_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_null(const char *first) const noexcept {
        current->__get_array().emplace_back(null_t());
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_true(const char *first) const noexcept {
        current->m_value.set(boolean_t(), true);
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_true(const char *first) const noexcept {
        element->m_value.set(boolean_t(), true);
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_true(const char *first) const noexcept {
        current->__get_array().emplace_back(boolean_t(), true);
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_false(const char *first) const noexcept {
        current->m_value.set(boolean_t(), false);
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_false(const char *first) const noexcept {
        element->m_value.set(boolean_t(), false);
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_false(const char *first) const noexcept {
        current->__get_array().emplace_back(boolean_t(), false);
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_number(const char *first,
                                                        const char *last) const noexcept {
        WJR_EXPECTED_INIT(ret, parse_number(first, last));
        current->m_value = *ret;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_number(const char *first, const char *last) const noexcept {
        WJR_EXPECTED_INIT(ret, parse_number(first, last));
        element->m_value = *ret;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_number(const char *first, const char *last) const noexcept {
        WJR_EXPECTED_INIT(ret, parse_number(first, last));
        current->__get_array().emplace_back(*ret);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_string(const char *first,
                                                        const char *last) const noexcept {
        string_type *str = __json_create<string_type>();
        try_uninitialized_resize(*str, last - first);
        auto ret = parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            __json_destroy(str);
            return unexpected(std::move(ret).error());
        }
        str->resize(*ret - str->data());
        current->m_value.set(string_t(), str);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_string(const char *first, const char *last) const noexcept {
        string_type *str = __json_create<string_type>();
        try_uninitialized_resize(*str, last - first);
        auto ret = parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            __json_destroy(str);
            return unexpected(std::move(ret).error());
        }
        str->resize(*ret - str->data());
        element->m_value.set(string_t(), str);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_string(const char *first, const char *last) const noexcept {
        string_type *str = __json_create<string_type>();
        try_uninitialized_resize(*str, last - first);
        auto ret = parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            __json_destroy(str);
            return unexpected(std::move(ret).error());
        }
        str->resize(*ret - str->data());
        current->__get_array().emplace_back(string_t(), str);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_key_string(const char *first,
                                                              const char *last) noexcept {
        string_type str;
        try_uninitialized_resize(str, last - first);
        WJR_EXPECTED_INIT(ret, parse_string(str.data(), first, last));
        str.resize(*ret - str.data());
        const auto iter = current->__get_object().try_emplace(std::move(str), dctor);
        element = std::addressof(iter.first->second);
        if (WJR_UNLIKELY(!iter.second)) {
            element->reset();
            return {};
        }

        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_object(uint32_t) const noexcept {
        current->m_value.set(object_t(), __json_create<object_type>());
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_object(uint32_t) noexcept {
        element->m_value.set(object_t(), __json_create<object_type>());
        stk.emplace_back(current);
        current = element;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_object(uint32_t) noexcept {
        stk.emplace_back(current);
        current = std::addressof(current->__get_array().emplace_back(
            object_t(), __json_create<object_type>()));
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_array(uint32_t) const noexcept {
        current->m_value.set(array_t(), __json_create<array_type>());
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_array(uint32_t) noexcept {
        element->m_value.set(array_t(), __json_create<array_type>());
        stk.emplace_back(current);
        current = element;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_array(uint32_t) noexcept {
        stk.emplace_back(current);
        current = std::addressof(
            current->__get_array().emplace_back(array_t(), __json_create<array_type>()));
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_object(uint32_t) noexcept {
        current = stk.back();
        stk.pop_back();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_array(uint32_t) noexcept {
        current = stk.back();
        stk.pop_back();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_root(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_object(uint32_t) noexcept {
        current = stk.back();
        stk.pop_back();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_array(uint32_t) noexcept {
        current = stk.back();
        stk.pop_back();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_root(uint32_t) const noexcept {
        return {};
    }

private:
    inplace_vector<json_type *, 256> stk;
    json_type *current;
    json_type *element;
};

class check_parser {
    template <typename Parser>
    friend result<void> visitor_detail::parse(Parser &&par, const reader &rd) noexcept;

public:
    WJR_INTRINSIC_INLINE static result<void> parse(const reader &rd) noexcept {
        return visitor_detail::parse(check_parser(), rd);
    }

protected:
    WJR_INTRINSIC_INLINE result<void> visit_root_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_number(const char *first,
                                                        const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_number(const char *first, const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_number(const char *first, const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_string(const char *first,
                                                        const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_string(const char *first, const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_string(const char *first, const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_key_string(const char *first, const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_end_object_to_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_root(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_root(uint32_t) const noexcept {
        return {};
    }
};

} // namespace detail

namespace visitor_detail {

extern template result<void>
parse<detail::basic_json_parser<json> &>(detail::basic_json_parser<json> &par,
                                         const reader &rd) noexcept;

extern template result<void> parse<detail::check_parser>(detail::check_parser &&par,
                                                         const reader &rd) noexcept;
} // namespace visitor_detail

template <typename Traits>
result<basic_json<Traits>> basic_json<Traits>::parse(const reader &rd) noexcept {
    detail::basic_json_parser<basic_json<Traits>> par;
    return par.parse(rd);
}

inline result<void> check(const reader &rd) noexcept {
    return detail::check_parser::parse(rd);
}

namespace detail {

template <typename Formatter, typename Traits>
void format_impl(Formatter fmt, const basic_json<Traits> &j, unsigned depth) noexcept {
    switch (j.type()) {
    case value_t::null: {
        fmt.format_null();
        break;
    }
    case value_t::boolean: {
        if (j.template get_unsafe<boolean_t>()) {
            fmt.format_true();
        } else {
            fmt.format_false();
        }
        break;
    }
    case value_t::number_unsigned: {
        fmt.format_number_unsigned(j.template get_unsafe<number_unsigned_t>());
        break;
    }
    case value_t::number_signed: {
        fmt.format_number_signed(j.template get_unsafe<number_signed_t>());
        break;
    }
    case value_t::number_float: {
        fmt.format_number_float(j.template get_unsafe<number_float_t>());
        break;
    }
    case value_t::string: {
        fmt.format_string(j.template get_unsafe<string_t>());
        break;
    }
    case value_t::object: {
        fmt.format_start_object();
        const auto &obj = j.template get_unsafe<object_t>();
        auto begin = obj.begin();
        const auto end = obj.end();

        if (begin != end) {
            goto LOOP0_HEAD;

            do {
                fmt.format_comma();

            LOOP0_HEAD:

                fmt.format_newline();
                fmt.format_indents(depth + 1);

                fmt.format_key(begin->first);
                fmt.format_space();
                format_impl(fmt, begin->second, depth + 1);
            } while (++begin != end);

            fmt.format_newline();
            fmt.format_indents(depth);
        }

        fmt.format_end_object();
        break;
    }
    case value_t::array: {
        fmt.format_start_array();
        const auto &arr = j.template get_unsafe<array_t>();
        auto begin = arr.begin();
        const auto end = arr.end();

        if (begin != end) {
            goto LOOP1_HEAD;

            do {
                fmt.format_comma();

            LOOP1_HEAD:

                fmt.format_newline();
                fmt.format_indents(depth + 1);

                format_impl(fmt, *begin, depth + 1);
            } while (++begin != end);

            fmt.format_newline();
            fmt.format_indents(depth);
        }

        fmt.format_end_array();
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }
}

} // namespace detail

template <typename Traits, typename JsonTraits>
std::basic_ostream<char, Traits> &operator<<(std::basic_ostream<char, Traits> &os,
                                             const basic_json<JsonTraits> &j) {
    if (const std::ostream::sentry ok(os); ok) {
        unique_stack_allocator stkal(math_detail::stack_alloc);

        std::basic_string<char, Traits, math_detail::weak_stack_alloc<char>> buffer(
            stkal);
        buffer.reserve(256);

        const std::streamsize indents = os.width();

        j.dump_impl(buffer, indents);
        __ostream_write_unchecked(os, buffer.data(), buffer.size());
        os.width(0);
    } else {
        os.setstate(std::ios::badbit);
    }

    return os;
}

template <typename JsonTraits>
std::ostream &operator<<(std::ostream &os, const basic_json<JsonTraits> &j) {
    if (const std::ostream::sentry ok(os); ok) {
        unique_stack_allocator stkal(math_detail::stack_alloc);

        vector<char, math_detail::weak_stack_alloc<char>> buffer(stkal);
        buffer.clear_if_reserved(256);

        const std::streamsize indents = os.width();

        j.dump_impl(buffer, indents);
        __ostream_write_unchecked(os, buffer.data(), buffer.size());
        os.width(0);
    } else {
        os.setstate(std::ios::badbit);
    }

    return os;
}

template <typename Traits>
bool operator==(const basic_json<Traits> &lhs, const basic_json<Traits> &rhs) {
    switch (lhs.type()) {
    case value_t::null: {
        return rhs.is_null();
    }
    case value_t::boolean: {
        return rhs.is_boolean();
    }
    case value_t::number_unsigned:
    case value_t::number_signed:
    case value_t::number_float: {
        return rhs.is_number() && (static_cast<double>(lhs) == static_cast<double>(rhs));
    }
    case value_t::string: {
        return rhs.is_string() &&
               lhs.template get_unsafe<string_t>() == rhs.template get_unsafe<string_t>();
    }
    case value_t::object: {
        return rhs.is_object() &&
               lhs.template get_unsafe<object_t>() == rhs.template get_unsafe<object_t>();
    }
    case value_t::array: {
        return rhs.is_array() &&
               lhs.template get_unsafe<array_t>() == rhs.template get_unsafe<array_t>();
    }
    default: {
        WJR_UNREACHABLE();
        return false;
    }
    }
}

template <typename Traits>
bool operator!=(const basic_json<Traits> &lhs, const basic_json<Traits> &rhs) {
    return !(lhs == rhs);
}

} // namespace wjr::json

namespace wjr {

template <typename Traits>
struct get_relocate_mode<json::basic_json<Traits>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

} // namespace wjr

namespace std {

template <typename Traits>
constexpr void
swap(wjr::json::basic_json<Traits> &lhs,
     wjr::json::basic_json<Traits> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace std

#endif // WJR_JSON_JSON_HPP__