#ifndef __WJR_JSON_H
#define __WJR_JSON_H

#include <cstdlib>
#include <cstdint>
#include <string.h>
#include "typeInfo.h"
#include "mString.h"

namespace wjr {

    class json;

    //Use mallocator as default allocator
    using json_boolean = bool;
    using json_number  = double;
    using json_string  = String;
    using json_object  = std::map<json_string, json, std::less<json_string>,
                         mallocator<std::pair<const json_string, json>>>;
    using json_array   = std::vector<json,mallocator<json>>;

    class json {
    private:
    public:

        template<typename...Args>
        struct CHECK_TO_JSON {
            template<typename...U>
            static auto check(int) ->
                decltype(to_json(std::declval<json&>(),std::declval<U>()...),std::true_type());
            template<typename...U>
            static auto check(...) -> std::false_type;
            constexpr static bool value = std::is_same_v<decltype(check<Args...>(0)),std::true_type>;
        };

        template<typename...Args>
        struct CHECK_FROM_JSON {
            template<typename...U>
            static auto check(int) ->
                decltype(from_json(std::declval<const json&>(), std::declval<U>()...), std::true_type());
            template<typename...U>
            static auto check(...)->std::false_type;
            constexpr static bool value = 
                std::conjunction_v<
                decltype(check<Args...>(0))>;
        };

        // container_tag is used to simplify writing

        template<typename...Args>
        constexpr static bool is_json_container = false;

        template<typename T>
        constexpr static bool is_json_container<T> = 
            typeInfo<std::decay_t<T>>::container_tag & 0x0F;

        template<typename...Args>
        constexpr static bool is_constructible_to_json = 
            is_json_container<Args...> || 
            CHECK_TO_JSON<Args...>::value;

        template<typename...Args>
        constexpr static bool is_constructible_from_json = 
            is_json_container<Args...> || 
            CHECK_FROM_JSON<Args...>::value;

        template<typename...Args>
        using _Is_constructible_to_object = 
            std::enable_if_t<std::is_constructible_v<json_object,Args...>,int>;

        template<typename...Args>
        using _Is_constructible_to_array = 
            std::enable_if_t<std::is_constructible_v<json_array,Args...>,int>;

        template<typename T>
        using _Is_size_t = std::enable_if_t<
            std::is_constructible_v<size_t, T>, int>;

        // default string
        template <typename..._StringViewIsh>
        using _Is_string_view_ish = std::enable_if_t<
            !is_constructible_to_json<_StringViewIsh...> &&
            !is_constructible_from_json<_StringViewIsh...> &&
            !std::is_constructible_v<size_t, _StringViewIsh...> &&
            std::is_constructible_v<json_string,
            _StringViewIsh...>, int>;

    public:

        enum class value_t : uint8_t {
            undefined = 0x00,
            null      = 0x01,
            boolean   = 0x02,
            number    = 0x03,
            string    = 0x04,
            object    = 0x05,
            array     = 0x06,
        };

        using Null    = std::nullptr_t;
        using Boolean = json_boolean;
        using Number  = json_number;
        using String  = json_string;
        using Object  = json_object;
        using Array   = json_array;

        // only need to move copy Number when need to copy data of union
        static_assert(sizeof(Number) >= sizeof(String*),"");

        using size_type = size_t;

        json();
        json(const json&);
        json(json&&)noexcept;
        json(Null);
        json(Boolean);
        json(Number);
        json(int);
        json(unsigned int);
        json(long long);
        json(unsigned long long);
        json(const Object&);
        json(Object&&)noexcept;
        json(const Array&);
        json(Array&&)noexcept;

        template<typename...Args, _Is_string_view_ish<Args...> = 0>
        explicit json(Args&&...args)noexcept;

        // you can customize the constructor by using a to_json function
        template<typename T, typename...Args, std::enable_if_t<is_constructible_to_json<T,Args...>,int> = 0>
        explicit json(const T&value, Args&&...args)noexcept;

        // construct a array of length _Count with value _Val
        json(const size_type _Count, const json& _Val);

        json(std::initializer_list<json> il);

        ~json();

        json& operator=(const json&);
        json& operator=(json&&)noexcept;
        json& operator=(Null);
        json& operator=(Boolean);
        json& operator=(Number);
        json& operator=(int);
        json& operator=(unsigned int);
        json& operator=(long long);
        json& operator=(unsigned long long);
        json& operator=(const Object&);
        json& operator=(Object&&)noexcept;
        json& operator=(const Array&);
        json& operator=(Array&&)noexcept;

        template<typename T, _Is_string_view_ish<T> = 0>
        json& operator=(T&& value)noexcept;

        void reset();
        void clear();
        void swap(json&);

        template<typename T, _Is_string_view_ish<T> = 0>
        json& operator[](T&& name);

        template<typename T, _Is_string_view_ish<T> = 0>
        json& at(T&&name);

        template<typename T, _Is_string_view_ish<T> = 0>
        const json& at(T&& name)const;

        json& operator[](const size_t index);
        const json& operator[](const size_t index)const;

        json& at(const size_t inedx);
        const json& at(const size_t index)const;

        void insert(const json& data);
        void insert(json&& data);

        template<typename T, typename U, _Is_string_view_ish<T> = 0,
            std::enable_if_t<std::is_same_v<std::decay_t<U>, json>, int> = 0>
        void insert(T&& name, U&& data);

        template<typename T, _Is_string_view_ish<T> = 0>
        void erase(T&& name);

        void push_back(const json& data);
        void push_back(json&& data);

        // the type must be array
        void pop_back();

        // will clear at first
        void set_object();
        void set_object(json_object&&);
        void set_array();
        void set_array(json_array&&);

        size_t size()const;

        template<typename T, _Is_string_view_ish<T> = 0>
        bool count(const T& name)const;

        bool count(const size_t index)const;

        bool empty()const;
        bool has_value()const;

        bool is_null()const;
        bool is_boolean()const;
        bool is_number()const;
        bool is_string()const;
        bool is_object()const;
        bool is_array()const;

        Null to_null();
        Null to_null()const;
        Boolean& to_boolean();
        Boolean to_boolean()const;
        Number& to_number();
        Number to_number()const;
        String& to_string();
        const String& to_string()const;

        // if is empty 
        // then will be automatically converted to the corresponding type
        // else if the types do not match
        // an error will be reported in the debug version
        Object& to_object();
        const Object& to_object()const;
        Array& to_array();
        const Array& to_array()const;

        // if the value is set successfully 
        // then will return true
        // else will return false
        bool get_value(Null&)const;
        bool get_value(Boolean&)const;
        bool get_value(Number&)const;
        bool get_value(int&)const;
        bool get_value(unsigned int&)const;
        bool get_value(long long&)const;
        bool get_value(unsigned long long&)const;
        bool get_value(String&)const;
        bool get_value(Object&)const;
        bool get_value(Array&)const;

        template<typename T, _Is_string_view_ish<T> = 0>
        bool get_value(T& value)const;

        template<typename T, typename...Args,std::enable_if_t<is_constructible_from_json<T,Args...>,int> = 0>
        bool get_value(T&value, Args&&...args)const;

        uint8_t type()const { return vtype; }

        json_string stringify(int tab = 2)const;
        json_string minify()const;

        const char* type_name()const;

        void merge_patch(const json& apply_patch);

        static json parse(String_view str);
        static bool accept(String_view str);

        static json array(std::initializer_list<json> il = {});
        static json object(std::initializer_list<std::pair<const String, json>> il = {});

        template<typename T, _Is_string_view_ish<T> = 0>
        static json object(std::initializer_list<std::pair<const T, json>> il = {});

        friend bool operator==(const json& lhs, const json& rhs);
        friend bool operator!=(const json& lhs, const json& rhs);

        friend std::ostream& operator<<(std::ostream& out, const json& x);

    private:

        static void MergePatch(json&Target,const json&Patch);

        void _minify(json_string&)const;

        static json_string format_tab(int tab);

        json_string stringify(int tab, int delta)const;

        void dfs_parse(const uint8_t*& s, const uint8_t* e);

        inline void _Tidy();

        uint8_t vtype;

        union {
            Boolean _Boolean;
            Number _Number;
            String* _String;
            Object* _Object;
            Array* _Array;
        };

    };

    template<typename...Args, json::_Is_string_view_ish<Args...>>
    json::json(Args&&...args)noexcept
        : _String(mallocator<String>().allocate(1)), vtype((uint8_t)value_t::string) {
        new (_String) String(std::forward<Args>(args)...);
    }

    template<typename T, typename...Args, 
        std::enable_if_t<json::is_constructible_to_json<T,Args...>,int>>
    json::json(const T& value, Args&&...args) noexcept
        : _Number(0), vtype((uint8_t)(value_t::undefined)) {
        using vtype = std::decay_t<T>;
        using _Type_Info = typeInfo<vtype>;
        if constexpr (json::CHECK_TO_JSON<Args...>::value) {
            to_json(*this, value, std::forward<Args>(args)...);
        }
        else if constexpr (_Type_Info::container_tag & default_json) {
            auto& it = _Type_Info::fields;
            set_object();
            it.forEach(
                [this, &value](auto& x) {
                    insert((json_string)x.name, (json)x.get_value(value));
                }
            );
        }
        else if constexpr (_Type_Info::container_tag & array_json) {
            set_array();
            for (auto& i : value) {
                insert((json)i);
            }
        }
        else if constexpr (_Type_Info::container_tag & object_json) {
            set_object();
            for (auto& i : value) {
                insert((json_string)i.first, (json)i.second);
            }
        }
    }

    template<typename T, json::_Is_string_view_ish<T> >
    json& json::operator=(T&& value)noexcept {
        if (vtype != (uint8_t)(value_t::string)) {
            _Tidy();
            _String = mallocator<String>().allocate(1);
            new (_String) String((String)(std::forward<T>(value)));
            vtype = (uint8_t)(value_t::string);
        }
        else *_String = std::forward<T>(value);
        return *this;
    }

    template<typename T, json::_Is_string_view_ish<T> >
    json& json::operator[](T&& name) {
        return to_object()[(String)std::forward<T>(name)];
    }

    template<typename T,json::_Is_string_view_ish<T> >
    json& json::at(T&& name) {
        return to_object().at((String)std::forward<T>(name));
    }

    template<typename T,json::_Is_string_view_ish<T> >
    const json& json::at(T&& name)const {
        return to_object().at((String)std::forward<T>(name));
    }

    template<typename T, typename U, json::_Is_string_view_ish<T>,
        std::enable_if_t<std::is_same_v<std::decay_t<U>, json>, int> >
        void json::insert(T&& name, U&& data) {
        to_object()[(String)std::forward<T>(name)] = std::forward<U>(data);
    }

    template<typename T, json::_Is_string_view_ish<T> >
    void json::erase(T&& name) {
        to_object().erase((String)std::forward<T>(name));
    }

    template<typename T, json::_Is_string_view_ish<T> >
    bool json::count(const T& name) const {
        return to_object().count((String)name);
    }

    template<typename T, typename...Args, std::enable_if_t<
        json::is_constructible_from_json<T,Args...>,int>>
    bool json::get_value(T& value,Args&&...args) const {
        using vtype = std::decay_t<T>;
        using _Type_Info = typeInfo<vtype>;
        if constexpr (json::CHECK_FROM_JSON<T>::value) {
            static_assert(std::is_same_v<decltype(from_json(std::declval<const json&>(), 
                std::declval<T&>(), std::declval<Args>(args)...)), bool>
                , "the return value must be bool");
            if (!from_json(*this, value,std::forward<Args>(args)...))return false;
        }
        else if constexpr (_Type_Info::container_tag & default_json) {
            auto& it = _Type_Info::fields;
            it.forEach(
                [this, &value](auto& x) {
                    if (!to_object().at(String(x.name)).get_value(x.get_value(value)))return false;
                }
            );
        }
        else if constexpr (_Type_Info::container_tag & array_json) {
            auto& arr = to_array();
            using value_type = std::decay_t<decltype(std::declval<T>()[0])>;
            for (auto& it : arr) {
                value.push_back(value_type());
                if (!it.get_value(value.back()))return false;
            }
        }
        else if constexpr (_Type_Info::container_tag & object_json) {
            auto& obj = to_object();
            for (auto& it : obj) {
                auto& val = value[String(it.first)];
                if (!it.second.get_value(val))return false;
            }
        }
        return true;
    }

    template<typename T, json::_Is_string_view_ish<T> >
    bool json::get_value(T& value) const {
        if (vtype != (uint8_t)(value_t::string))return false;
        value = (T)to_string();
        return true;
    }

    template<typename T, json::_Is_string_view_ish<T> >
    json json::object(std::initializer_list<std::pair<const T, json>> il) {
        json x;
        x.vtype = (uint8_t)(json::value_t::object);
        x._Object = mallocator<Object>().allocate(1);
        new (x._Object) Object();
        for (auto i = il.begin(); i != il.end(); ++i) {
            x.insert((String)i->first, i->second);
        }
        return x;
    }

    json operator ""_json(const char* str, const size_t);

}

#endif
