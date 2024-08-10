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
            -> decltype(std::declval<Json &&>().get_##T##_unsafe()) {                    \
            return std::forward<Json>(j).get_##T##_unsafe();                             \
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

class bad_json_access : std::exception {
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

/**
 * @details At present, it's a simple but flexible implementation solution. This is not as
 * good as a only iteration in terms of performance. \n
 * Performance (on my local x64 machine) : \n
 * twitter.json : 700~720 MB/s \n
 * canada.json : 660~680 MB/s
 *
 */
template <typename Traits>
class basic_json {
    friend class detail::basic_json_parser<basic_json>;

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

    using size_type = uint32_t;

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

    reference at(size_type idx) noexcept { return get<array_t>().at(idx); }
    const_reference at(size_type idx) const noexcept { return get<array_t>().at(idx); }

    static result<basic_json> parse(const reader &rd) noexcept;

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
    static_vector<json_type *, 256> stk;
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