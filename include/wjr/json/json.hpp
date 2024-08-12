#ifndef WJR_JSON_JSON_HPP__
#define WJR_JSON_JSON_HPP__

#include <map>

#include <wjr/json/visitor.hpp>

namespace wjr::json {

namespace detail {
extern result<void> __check_impl(const reader &rd) noexcept;
}

WJR_INTRINSIC_INLINE result<void> check(const reader &rd) noexcept {
    return detail::__check_impl(rd);
}

template <typename Traits>
class basic_json;

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

} // namespace detail

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

template <typename Mybase>
struct json_serializer_auto_completion : private Mybase {
    using value_type = typename Mybase::value_type;

    /// Use functions of Mybase.

    template <typename Json, WJR_REQUIRES(has_Mybase_assign_from_json_v<
                                          Mybase, value_type, const Json &>)>
    constexpr static void assign(const Json &j, value_type &val) {
        Mybase::template assign(j, val);
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_lvalue_reference_v<Json> &&
                           has_Mybase_assign_from_json_v<Mybase, value_type, Json &&>)>
    constexpr static void assign(Json &&j, value_type &val) {
        Mybase::template assign(std::move(j), val);
    }

    template <typename Json, WJR_REQUIRES(has_Mybase_construct_from_json_v<
                                          Mybase, value_type, const Json &>)>
    constexpr static value_type construct(const Json &j) {
        return Mybase::template construct(j);
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_lvalue_reference_v<Json> &&
                           has_Mybase_construct_from_json_v<Mybase, value_type, Json &&>)>
    constexpr static value_type construct(Json &&j) {
        return Mybase::template construct(std::move(j));
    }

    /// Use member functions of value_type

    template <typename Json, WJR_REQUIRES(!has_Mybase_assign_from_json_v<
                                              Mybase, value_type, const Json &> &&
                                          std::is_assignable_v<value_type, const Json &>)>
    constexpr static void assign(const Json &j, value_type &val) {
        val = j;
    }

    template <typename Json, WJR_REQUIRES(!has_Mybase_assign_from_json_v<
                                              Mybase, value_type, const Json &> &&
                                          std::is_assignable_v<value_type, Json &&>)>
    constexpr static void assign(Json &&j, value_type &val) {
        val = std::move(j);
    }

    template <typename Json,
              WJR_REQUIRES(
                  !has_Mybase_construct_from_json_v<Mybase, value_type, const Json &> &&
                  std::is_constructible_v<value_type, const Json &>)>
    constexpr static value_type construct(const Json &j) {
        return value_type(j);
    }

    template <typename Json, WJR_REQUIRES(!std::is_lvalue_reference_v<Json> &&
                                          !has_Mybase_construct_from_json_v<
                                              Mybase, value_type, const Json &> &&
                                          std::is_constructible_v<value_type, Json &&>)>
    constexpr static value_type construct(Json &&j) {
        return value_type(std::move(j));
    }

    template <
        typename Json,
        WJR_REQUIRES(!std::is_lvalue_reference_v<Json> &&
                     !has_Mybase_assign_from_json_v<Mybase, value_type, Json &&> &&
                     !std::is_assignable_v<value_type, value_type &&> &&
                     (has_Mybase_assign_from_json_v<Mybase, value_type, const Json &> ||
                      std::is_assignable_v<value_type, const Json &>))>
    constexpr static void assign(Json &&j, value_type &val) {
        assign<const Json>(j, val);
    }

    template <typename Json,
              WJR_REQUIRES(
                  !std::is_lvalue_reference_v<Json> &&
                  !has_Mybase_construct_from_json_v<Mybase, value_type, Json &&> &&
                  !std::is_constructible_v<value_type, value_type &&> &&
                  (has_Mybase_construct_from_json_v<Mybase, value_type, const Json &> ||
                   std::is_constructible_v<value_type, const Json &>))>
    constexpr static value_type construct(Json &&j) {
        return construct<const Json>(j);
    }
};

template <typename T, typename Enable = void>
struct json_serializer_impl {
    using value_type = char;
};

template <typename T>
struct json_serializer : json_serializer_auto_completion<json_serializer_impl<T>> {};

WJR_REGISTER_HAS_TYPE(assign_from_json,
                      json_serializer<T>::assign(std::declval<Json>(),
                                                 std::declval<T &>()),
                      T, Json);
WJR_REGISTER_HAS_TYPE(construct_from_json,
                      json_serializer<T>::construct(std::declval<Json>()), T, Json);

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

    template <typename U>
    friend struct __json_get_impl;

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
    basic_json(const basic_json &other) noexcept {
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
            memory_pool<string_type> al;
            string_type *const str = al.allocate(1);
            wjr::construct_at(str, other.__get_string());
            m_value.set(string_t(), str);
            break;
        }
        case value_t::object: {
            memory_pool<object_type> al;
            object_type *const obj = al.allocate(1);
            wjr::construct_at(obj, other.__get_object());
            m_value.set(object_t(), obj);
            break;
        }
        case value_t::array: {
            memory_pool<array_type> al;
            array_type *const arr = al.allocate(1);
            wjr::construct_at(arr, other.__get_array());
            m_value.set(array_t(), arr);
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

    basic_json &operator=(const basic_json &other) = delete;

    basic_json &operator=(basic_json &&other) noexcept {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

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

    basic_json(null_t) noexcept : m_value(null_t()) {}
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

    basic_json(basic_value value) noexcept : m_value(value) {}
    basic_json(dctor_t) noexcept : basic_json() {}

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

    template <typename T, WJR_REQUIRES(has_construct_from_json_v<T, const basic_json &>)>
    explicit operator T() const & {
        return json_serializer<T>::construct(*this);
    }

    template <typename T, WJR_REQUIRES(has_construct_from_json_v<T, basic_json &&>)>
    explicit operator T() && {
        return json_serializer<T>::construct(std::move(*this));
    }

    template <typename T, WJR_REQUIRES(has_assign_from_json_v<T, const basic_json &>)>
    void get_to(T &val) const & {
        return json_serializer<T>::assign(*this, val);
    }

    template <typename T, WJR_REQUIRES(has_assign_from_json_v<T, basic_json &&>)>
    void get_to(T &val) && {
        return json_serializer<T>::assign(std::move(*this), val);
    }

private:
    void __destroy_impl() {
        switch (type()) {
        case value_t::null:
        case value_t::boolean:
        case value_t::number_unsigned:
        case value_t::number_signed:
        case value_t::number_float: {
            break;
        }
        case value_t::string: {
            auto &str = __get_string();
            std::destroy_at(std::addressof(str));
            memory_pool<string_type> al;
            al.deallocate(std::addressof(str), 1);
            break;
        }
        case value_t::object: {
            auto &obj = __get_object();
            std::destroy_at(std::addressof(obj));
            memory_pool<object_type> al;
            al.deallocate(std::addressof(obj), 1);
            break;
        }
        case value_t::array: {
            auto &arr = __get_array();
            std::destroy_at(std::addressof(arr));
            memory_pool<array_type> al;
            al.deallocate(std::addressof(arr), 1);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }
    }

    void __destroy() {
        if (WJR_BUILTIN_CONSTANT_P_TRUE(type() == value_t::null)) {
            return;
        }

        __destroy_impl();
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

    template <typename Json>
    constexpr static void assign(const Json &j, value_type &val) {
        if (WJR_UNLIKELY(!j.is_null())) {
            WJR_THROW(bad_json_access(error_code::TAPE_ERROR));
        }

        val = nullptr;
    }
};

template <>
struct json_serializer_impl<bool, void> {

    using value_type = bool;

    template <typename Json>
    constexpr static void assign(const Json &j, value_type &val) {
        val = j.template get<bool>();
    }
};

template <typename arithmetic>
struct __json_serializer_arithmetic {
    using value_type = arithmetic;

    template <typename Json>
    constexpr static void assign(const Json &j, value_type &val) {
        switch (j.type()) {
        case value_t::number_unsigned: {
            val = static_cast<arithmetic>(j.template get_unsafe<number_unsigned_t>());
            break;
        }
        case value_t::number_signed: {
            val = static_cast<arithmetic>(j.template get_unsafe<number_signed_t>());
            break;
        }
        case value_t::number_float: {
            val = static_cast<arithmetic>(j.template get_unsafe<number_float_t>());
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
    constexpr static value_type construct(const Json &j) {
        value_type val;
        assign(j, val);
        return val;
    }
};

template <typename arithmetic>
struct json_serializer_impl<arithmetic,
                            std::enable_if_t<std::is_arithmetic_v<arithmetic>>>
    : __json_serializer_arithmetic<arithmetic> {};

template <typename Char, typename Traits, typename Alloc>
struct json_serializer_impl<std::basic_string<Char, Traits, Alloc>, void> {
    using value_type = std::basic_string<Char, Traits, Alloc>;

    template <typename Json>
    constexpr static void assign(value_type &val, const Json &j) {
        val = j.template get<string_t>();
    }

    template <typename Json>
    constexpr static void assign(value_type &val, Json &&j) {
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
};

template <typename T>
struct __json_serializer_array {
    using value_type = T;

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &val, const Json &j) {
        val = j.template get<array_t>();
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &val, const Json &j) {
        auto &arr = j.template get<array_t>();
        val.assign(arr.begin(), arr.end());
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &val, Json &&j) {
        val = std::move(j.template get<array_t>());
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &val, Json &&j) {
        auto &arr = j.template get<array_t>();
        val.assign(std::make_move_iterator(arr.begin()),
                   std::make_move_iterator(arr.end()));
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
};

template <typename T, typename Alloc>
struct json_serializer_impl<std::vector<T, Alloc>, void>
    : __json_serializer_array<std::vector<T, Alloc>> {
    using value_type = std::vector<T, Alloc>;
};

template <typename Traits>
struct json_serializer_impl<basic_vector<Traits>, void>
    : __json_serializer_array<basic_vector<Traits>> {
    using value_type = basic_vector<Traits>;
};

template <typename T>
struct __json_serializer_object {
    using value_type = T;

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &val, const Json &j) {
        val = j.template get<object_t>();
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &val, const Json &j) {
        auto &obj = j.template get<object_t>();
        val.clear();
        for (auto &elem : obj) {
            val.emplace(elem);
        }
    }

    template <typename Json,
              WJR_REQUIRES(std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &val, Json &&j) {
        val = std::move(j.template get<object_t>());
    }

    template <typename Json,
              WJR_REQUIRES(!std::is_same_v<typename Json::array_type, value_type>)>
    constexpr static void assign(value_type &val, Json &&j) {
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
};

template <typename Key, typename Value, typename Pr, typename Alloc>
struct json_serializer_impl<std::map<Key, Value, Pr, Alloc>, void>
    : __json_serializer_object<std::map<Key, Value, Pr, Alloc>> {
    using value_type = std::map<Key, Value, Pr, Alloc>;
};

template <typename Key, typename Value, typename Pr, typename Alloc>
struct json_serializer_impl<std::multimap<Key, Value, Pr, Alloc>, void>
    : __json_serializer_object<std::multimap<Key, Value, Pr, Alloc>> {
    using value_type = std::multimap<Key, Value, Pr, Alloc>;
};

template <typename Key, typename Value, typename Hash, typename Eq, typename Alloc>
struct json_serializer_impl<std::unordered_map<Key, Value, Hash, Eq, Alloc>, void>
    : __json_serializer_array<std::unordered_map<Key, Value, Hash, Eq, Alloc>> {
    using value_type = std::unordered_map<Key, Value, Hash, Eq, Alloc>;
};

template <typename Key, typename Value, typename Hash, typename Eq, typename Alloc>
struct json_serializer_impl<std::unordered_multimap<Key, Value, Hash, Eq, Alloc>, void>
    : __json_serializer_array<std::unordered_multimap<Key, Value, Hash, Eq, Alloc>> {
    using value_type = std::unordered_multimap<Key, Value, Hash, Eq, Alloc>;
};

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
        memory_pool<string_type> al;
        string_type *str = al.allocate(1);
        wjr::construct_at(str);
        try_uninitialized_resize(*str, last - first);
        auto ret = parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            std::destroy_at(str);
            al.deallocate(str, 1);
            return unexpected(std::move(ret).error());
        }
        str->resize(*ret - str->data());
        current->m_value.set(string_t(), str);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_string(const char *first, const char *last) const noexcept {
        memory_pool<string_type> al;
        string_type *str = al.allocate(1);
        wjr::construct_at(str);
        try_uninitialized_resize(*str, last - first);
        auto ret = parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            std::destroy_at(str);
            al.deallocate(str, 1);
            return unexpected(std::move(ret).error());
        }
        str->resize(*ret - str->data());
        element->m_value.set(string_t(), str);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_string(const char *first, const char *last) const noexcept {
        memory_pool<string_type> al;
        string_type *str = al.allocate(1);
        wjr::construct_at(str);
        try_uninitialized_resize(*str, last - first);
        auto ret = parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            std::destroy_at(str);
            al.deallocate(str, 1);
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
        const auto iter = current->__get_object().emplace(std::move(str), dctor);
        element = std::addressof(iter.first->second);
        if (WJR_UNLIKELY(!iter.second)) {
            std::destroy_at(element);
            wjr::construct_at(element, dctor);
            return {};
        }

        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_object(uint32_t) const noexcept {
        memory_pool<object_type> al;
        object_type *const obj = al.allocate(1);
        wjr::construct_at(obj);
        current->m_value.set(object_t(), obj);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_object(uint32_t) noexcept {
        memory_pool<object_type> al;
        object_type *const obj = al.allocate(1);
        wjr::construct_at(obj);
        element->m_value.set(object_t(), obj);
        stk.emplace_back(current);
        current = element;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_object(uint32_t) noexcept {
        memory_pool<object_type> al;
        object_type *const obj = al.allocate(1);
        wjr::construct_at(obj);
        stk.emplace_back(current);
        current = std::addressof(current->__get_array().emplace_back(object_t(), obj));
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_array(uint32_t) const noexcept {
        memory_pool<array_type> al;
        array_type *const arr = al.allocate(1);
        wjr::construct_at(arr);
        current->m_value.set(array_t(), arr);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_array(uint32_t) noexcept {
        memory_pool<array_type> al;
        array_type *const arr = al.allocate(1);
        wjr::construct_at(arr);
        element->m_value.set(array_t(), arr);
        stk.emplace_back(current);
        current = element;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_array(uint32_t) noexcept {
        memory_pool<array_type> al;
        array_type *const arr = al.allocate(1);
        wjr::construct_at(arr);
        stk.emplace_back(current);
        current = std::addressof(current->__get_array().emplace_back(array_t(), arr));
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

} // namespace detail

template <typename Traits>
result<basic_json<Traits>> basic_json<Traits>::parse(const reader &rd) noexcept {
    detail::basic_json_parser<basic_json<Traits>> par;
    return par.parse(rd);
}

} // namespace wjr::json

namespace wjr {

template <typename Traits>
struct get_relocate_mode<json::basic_json<Traits>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

} // namespace wjr

#endif // WJR_JSON_JSON_HPP__