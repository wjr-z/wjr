#ifndef WJR_JSON_JSON_HPP__
#define WJR_JSON_JSON_HPP__

#include <map>

#include <wjr/json/visitor.hpp>

namespace wjr::json {

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
    using object_type = Object<string_type, json_type, std::less<string_type>,
                               memory_pool<std::pair<const string_type, json_type>>>;
    using array_type = Array<json_type, memory_pool<json_type>>;
};

using default_json_traits = basic_json_traits<std::basic_string, std::map, vector>;
using __default_json_string = typename default_json_traits::string_type;

template <typename Json>
class basic_json_parser;

} // namespace detail

using json = basic_json<detail::default_json_traits>;

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

private:
    using __tp_list =
        tp_list<null_type, boolean_type, number_unsigned_type, number_signed_type,
                number_float_type, string_type, object_type, array_type>;

public:
    basic_json() = default;
    basic_json(const basic_json &other) = delete;

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
    ~basic_json() noexcept { __destroy_and_deallocate(); }

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

    void __destroy_and_deallocate() { __destroy(); }

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
        WJR_EXPECTED_INIT(ret, visitor_detail::parse(*this, rd));
        return j;
    }

protected:
    WJR_INTRINSIC_INLINE result<void> visit_root_null(const char *first) const noexcept {
        return detail::check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_null(const char *first) const noexcept {
        return detail::check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_null(const char *first) const noexcept {
        current->__get_array().emplace_back(null_t());
        return detail::check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_true(const char *first) const noexcept {
        current->m_value.set(boolean_t(), true);
        return detail::check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_true(const char *first) const noexcept {
        element->m_value.set(boolean_t(), true);
        return detail::check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_true(const char *first) const noexcept {
        current->__get_array().emplace_back(boolean_t(), true);
        return detail::check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_false(const char *first) const noexcept {
        return detail::check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_false(const char *first) const noexcept {
        element->m_value.set(boolean_t(), false);
        return detail::check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_false(const char *first) const noexcept {
        current->__get_array().emplace_back(boolean_t(), false);
        return detail::check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_number(const char *first,
                                                        const char *last) const noexcept {
        WJR_EXPECTED_INIT(ret, detail::parse_number(first, last));
        current->m_value = *ret;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_number(const char *first, const char *last) const noexcept {
        WJR_EXPECTED_INIT(ret, detail::parse_number(first, last));
        element->m_value = *ret;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_number(const char *first, const char *last) const noexcept {
        WJR_EXPECTED_INIT(ret, detail::parse_number(first, last));
        current->__get_array().emplace_back(*ret);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_string(const char *first,
                                                        const char *last) const noexcept {
        memory_pool<string_type> al;
        string_type *str = al.allocate(1);
        wjr::construct_at(str);
        try_uninitialized_resize(*str, last - first);
        auto ret = detail::parse_string(str->data(), first, last);
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
        auto ret = detail::parse_string(str->data(), first, last);
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
        auto ret = detail::parse_string(str->data(), first, last);
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
        WJR_EXPECTED_INIT(ret, detail::parse_string(str.data(), first, last));
        str.resize(*ret - str.data());
        const auto iter = current->__get_object().emplace(std::move(str), dctor);
        WJR_ASSUME(str.size() == 0);
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