#include "json.h"
#include "../string_helper.h"
#include <cmath>
#include <string>
#include <algorithm>
#if 0 == 1
#include <intrin.h>
#endif

namespace wjr {

    inline namespace json_memory {
        
        // ' ','\n','\r','\t' 
        constexpr static bool is_white_space[256] ={0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        };

        constexpr static size_t string_step[256] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
        };

        const uint8_t* simple_skip_whitespace(const uint8_t* s, const uint8_t* e) {
            while (s != e && is_white_space[*s])++s;
            return s;
        }

        const uint8_t* skip_whitespace(const uint8_t* s,const uint8_t* e) {
        #if 0 == 1
            if(is_white_space[*s])
                ++s;
            else return s;

			static const char whitespace[16] = " \n\r\t";
			const __m128i w = _mm_load_si128((const __m128i*)&whitespace[0]);
			for (; s <= e - 16; s += 16) {
				const __m128i ss = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s));
				const int r = _mm_cmpistri(w, ss, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
				if (r != 16)
					return s + r;
			}
        #endif
			return simple_skip_whitespace(s,e);
		}

        const uint8_t* skip_string(const uint8_t* s, const uint8_t* e) {
            while (s != e && string_step[*s]) {
                s += string_step[*s];
            }
            return s;
        }

        bool check_num(const uint8_t*& s, const uint8_t* e) {
            if (*s == '+' || *s == '-')
                ++s;

            if (s == e || !isdigit(*s))
                return false;

            if (*s == '0') {
                ++s;
            }
            else {
                for (; s != e && isdigit(*s); ++s);
            }

            if (*s == '.') {
                for (++s; s != e && isdigit(*s); ++s);
            }

            if (*s == 'e' || *s == 'E') {
                ++s;
                if (*s == '+' || *s == '-')
                    ++s;
                if (s == e || !isdigit(*s))
                    return false;
                for (; s != e && isdigit(*s); ++s);
            }

            return true;
        }

        bool check_string(const uint8_t*& s, const uint8_t* e) {
            if (*s != '"')return false;
            ++s;
            while (s != e && *s != '"') {
                if (*s == '\\') {
                    ++s;
                    if (s == e)return false;
                    switch (*s) {
                    case '"':
                    case '\\':
                    case '/':
                    case 'b':
                    case 'f':
                    case 'n':
                    case 'r':
                    case 't':
                    case 'u':
                        break;
                    default:
                        return false;
                    }
                }
                else {
                    if (*s < 32 || *s == 127)
                        return false;
                }
                ++s;
            }
            return s != e;
        }

        json_string to_accurate_string(double val) {

            if ((val < INT64_MAX) && (val > INT64_MIN)) {
                if (val == static_cast<double>(static_cast<long long>(val)))
                    return (json_string)std::to_string(static_cast<long long>(val));
            }

            const auto _Len = static_cast<size_t>(_scprintf("%.17g", val));
            json_string _Str(_Len, '\0');
            sprintf_s(&_Str[0], _Len + 1, "%.17g", val);
            return _Str;
        }

    }

    // default constructor
    json::json()
        : _Type((uint8_t)(value_t::undefined)) {

    }

    json::json(const json& other)
        : _Number(other._Number), _Type(other._Type) {
        // copy Number firstly
        switch (_Type) {
            case (uint8_t)(value_t::string) : {
                _String = mallocator<String>().allocate();
                new (_String) String(*other._String);
                break;
            }
            case (uint8_t)(value_t::object) : {
                _Object = mallocator<Object>().allocate();
                new (_Object) Object(*other._Object);
                break;
            }
            case (uint8_t)(value_t::array) : {
                _Array = mallocator<Array>().allocate();
                new (_Array) Array(*other._Array);
                break;
            }
        }
    }

    json::json(json&& other)noexcept
        : _Number(other._Number), _Type(other._Type) {
        other._Type = (uint8_t)(value_t::undefined);
    }

    json::json(Null)
        : _Type((uint8_t)(value_t::null)) {

    }

    json::json(Boolean f)
        : _Boolean(f), _Type((uint8_t)(value_t::boolean)) {

    }

    json::json(Number v)
        : _Number(v), _Type((uint8_t)(value_t::number)) {

    }

    json::json(int v)
        : _Number(v), _Type((uint8_t)(value_t::number)) {

    }

    json::json(unsigned int v)
        : _Number(v), _Type((uint8_t)(value_t::number)) {

    }

    json::json(long long v)
        : _Number(v), _Type((uint8_t)(value_t::number)) {

    }

    json::json(unsigned long long v)
        : _Number(v), _Type((uint8_t)(value_t::number)) {

    }

    json::json(const Object& v)
        : _Object(mallocator<Object>().allocate()), _Type((uint8_t)(value_t::object)) {
        new (_Object) Object(v);
    }

    json::json(Object&& v)noexcept
        : _Object(mallocator<Object>().allocate()), _Type((uint8_t)(value_t::object)) {
        new (_Object) Object(std::move(v));
    }

    json::json(const Array& v)
        : _Array(mallocator<Array>().allocate()), _Type((uint8_t)(value_t::array)) {
        new (_Array) Array(v);
    }

    json::json(Array&& v)noexcept
        : _Array(mallocator<Array>().allocate()), _Type((uint8_t)(value_t::array)) {
        new (_Array) Array(std::move(v));
    }

    json::json(const size_type _Count, const json& _Val)
        : _Array(mallocator<Array>().allocate()), _Type((uint8_t)(value_t::array)) {
        new (_Array) Array(_Count, _Val);
    }

    json::json(std::initializer_list<json> il)
        : _Number(0), _Type((uint8_t)(value_t::undefined)) {
        bool is_object = std::all_of(il.begin(), il.end(),
            [](const json& json_ref) {
                return json_ref.is_array() && json_ref.size() == 2 && json_ref[0].is_string();
            }
        );
        if (is_object) {
            set_object();
            for (const json* i = il.begin(); i != il.end(); ++i) {
                json& element = *const_cast<json*>(i);
                insert(std::move(element[0].to_string()),std::move(element[1]));
            }
        }
        else {
            set_array();
            for (const json* i = il.begin(); i!=il.end(); ++i) {
                json& element = *const_cast<json*>(i);
                push_back(std::move(element));
            }
        }
    }

    void json::_Tidy() {
        switch (_Type) {
            case (uint8_t)(value_t::string) : {
                _String->~String();
                mallocator<String>().deallocate(_String);
                break;
            }
            case (uint8_t)(value_t::object) : {
                _Object->~Object();
                mallocator<Object>().deallocate(_Object);
                break;
            }
            case (uint8_t)(value_t::array) : {
                _Array->~Array();
                mallocator<Array>().deallocate(_Array);
                break;
            }
            default:
                break;
        }
    }

    void json::reset() {
        _Tidy();
        _Type = (uint8_t)(value_t::undefined);
    }

    void json::clear() {
        reset();
    }

    void json::swap(json& other) {
        std::swap(_Number,other._Number);
        std::swap(_Type,other._Type);
    }

    json::~json() {
        _Tidy();
    }

    json& json::operator=(const json& other) {
        if (this == &other) {
            return *this;
        }
        // different type
        if (_Type != other._Type) {
            _Tidy();
            _Type = other._Type;
            switch (_Type) {
                case (uint8_t)(value_t::string) : {
                    _String = mallocator<String>().allocate();
                    new (_String) String(*other._String);
                    break;
                }
                case (uint8_t)(value_t::object) : {
                    _Object = mallocator<Object>().allocate();
                    new (_Object) Object(*other._Object);
                    break;
                }
                case (uint8_t)(value_t::array) : {
                    _Array = mallocator<Array>().allocate();
                    new (_Array) Array(*other._Array);
                    break;
                }
                default: {
                    _Number = other._Number;
                    break;
                }
            }
        }
        else {
            switch (_Type) {
                case (uint8_t)(value_t::string) : {
                    *_String = *other._String;
                    break;
                }
                case (uint8_t)(value_t::object) : {
                    *_Object = *other._Object;
                    break;
                }
                case (uint8_t)(value_t::array) : {
                    *_Array = *other._Array;
                    break;
                }
                default: {
                    _Number = other._Number;
                    break;
                }
            }
        }
        return *this;
    }

    json& json::operator=(json&& other)noexcept {
        _Tidy();
        _Number = other._Number;
        _Type = other._Type;
        other._Type = (uint8_t)(value_t::undefined);
        return *this;
    }

    json& json::operator=(Null) {
        _Tidy();
        _Type = (uint8_t)(value_t::null);
        return *this;
    }

    json& json::operator=(Boolean f) {
        _Tidy();
        _Boolean = f;
        _Type = (uint8_t)(value_t::boolean);
        return *this;
    }

    json& json::operator=(Number v) {
        _Tidy();
        _Number = v;
        _Type = (uint8_t)(value_t::number);
        return *this;
    }

    json& json::operator=(int v) {
        _Tidy();
        _Number = v;
        _Type = (uint8_t)(value_t::number);
        return *this;
    }

    json& json::operator=(unsigned int v) {
        _Tidy();
        _Number = v;
        _Type = (uint8_t)(value_t::number);
        return *this;
    }

    json& json::operator=(long long v) {
        _Tidy();
        _Number = v;
        _Type = (uint8_t)(value_t::number);
        return *this;
    }

    json& json::operator=(unsigned long long v) {
        _Tidy();
        _Number = v;
        _Type = (uint8_t)(value_t::number);
        return *this;
    }

    json& json::operator=(const Object& v) {
        if (_Type != (uint8_t)(value_t::object)) {
            _Tidy();
            _Object = mallocator<Object>().allocate();
            new (_Object) Object(v);
            _Type = (uint8_t)(value_t::object);
        }
        else {
            *_Object = v;
        }
        return *this;
    }

    json& json::operator=(Object&& v)noexcept {
        if (_Type != (uint8_t)(value_t::object)) {
            _Tidy();
            _Object = mallocator<Object>().allocate();
            new (_Object) Object(std::move(v));
            _Type = (uint8_t)(value_t::object);
        }
        else {
            *_Object = std::move(v);
        }
        return *this;
    }

    json& json::operator=(const Array& v) {
        if (_Type != (uint8_t)(value_t::array)) {
            _Tidy();
            _Array = mallocator<Array>().allocate();
            new (_Array) Array(v);
            _Type = (uint8_t)(value_t::array);
        }
        else {
            *_Array = v;
        }
        return *this;
    }

    json& json::operator=(Array&& v)noexcept {
        if (_Type != (uint8_t)(value_t::array)) {
            _Tidy();
            _Array = mallocator<Array>().allocate();
            new (_Array) Array(std::move(v));
            _Type = (uint8_t)(value_t::array);
        }
        else {
            *_Array = std::move(v);
        }
        return *this;
    }

    bool json::is_null()const {
        return _Type == (uint8_t)(value_t::null);
    }

    bool json::is_boolean()const {
        return _Type == (uint8_t)(value_t::boolean);
    }

    bool json::is_number()const {
        return _Type == (uint8_t)(value_t::number);
    }

    bool json::is_string()const {
        return _Type == (uint8_t)(value_t::string);
    }

    bool json::is_object()const {
        return _Type == (uint8_t)(value_t::object);
    }

    bool json::is_array()const {
        return _Type == (uint8_t)(value_t::array);
    }

    json::Null json::to_null() {
        assert(is_null());
        return nullptr;
    }

    json::Null json::to_null()const {
        assert(is_null());
        return nullptr;
    }

    json::Boolean& json::to_boolean() {
        assert(is_boolean());
        return _Boolean;
    }

    json::Boolean json::to_boolean()const {
        assert(is_boolean());
        return _Boolean;
    }

    json::Number& json::to_number() {
        assert(is_number());
        return _Number;
    }

    json::Number json::to_number()const {
        assert(is_number());
        return _Number;
    }

    json::String& json::to_string() {
        assert(is_string());
        return *_String;
    }

    const json::String& json::to_string()const {
        assert(is_string());
        return *_String;
    }

    json::Object& json::to_object() {
        assert(is_object() || empty());
        if (empty()) {
            set_object();
        }
        return *_Object;
    }

    const json::Object& json::to_object()const {
        assert(is_object());
        return *_Object;
    }

    json::Array& json::to_array() {
        assert(is_array() || empty());
        if (empty()) {
            set_array();
        }
        return *_Array;
    }

    const json::Array& json::to_array()const {
        assert(is_array());
        return *_Array;
    }

    bool json::get_value(Null& v) const {
        if (_Type != (uint8_t)(value_t::null)) {
            return false;
        }
        v = nullptr;
        return true;
    }

    bool json::get_value(Boolean& f) const {
        if (_Type != (uint8_t)(value_t::boolean)) {
            return false;
        }
        f = _Boolean;
        return true;
    }

    bool json::get_value(Number& v) const {
        if (_Type != (uint8_t)(value_t::number)) {
            return false;
        }
        v = _Number;
        return true;
    }

    bool json::get_value(int& v) const {
        if (_Type != (uint8_t)(value_t::number)) {
            return false;
        }
        v = _Number;
        return true;
    }

    bool json::get_value(unsigned int& v) const {
        if (_Type != (uint8_t)(value_t::number)) {
            return false;
        }
        v = _Number;
        return true;
    }

    bool json::get_value(long long& v) const {
        if (_Type != (uint8_t)(value_t::number)) {
            return false;
        }
        v = _Number;
        return true;
    }

    bool json::get_value(unsigned long long& v) const {
        if (_Type != (uint8_t)(value_t::number)) {
            return false;
        }
        v = _Number;
        return true;
    }

    bool json::get_value(String& v) const {
        if (_Type != (uint8_t)(value_t::string)) {
            return false;
        }
        v = *_String;
        return true;
    }

    bool json::get_value(Object& v) const {
        if (_Type != (uint8_t)(value_t::object)) {
            return false;
        }
        v = *_Object;
        return true;
    }

    bool json::get_value(Array& v) const {
        if (_Type != (uint8_t)(value_t::array)) {
            return false;
        }
        v = *_Array;
        return true;
    }

    void json::parse(std::string_view str) {
        parse(str.data(),str.length());
    }

    void json::parse(const char* ptr) {
        parse(ptr,strlen(ptr));
    }

    void json::parse(const char* ptr, size_t len) {
        reset();
        if(!len)return ;
        auto s = (const uint8_t*) ptr;
        getValue(s,s + len);
    }

    json& json::operator[](const size_t index) {
        return to_array()[index];
    }

    const json& json::operator[](const size_t index)const {
        return to_array()[index];
    }

    json& json::at(const size_t index) {
        return to_array().at(index);
    }

    const json& json::at(const size_t index)const {
        return to_array().at(index);
    }

    void json::insert(const json& data) {
        to_array().push_back(data);
    }

    void json::insert(json&& data) {
        to_array().push_back(std::move(data));
    }

    void json::push_back(const json& data) {
        to_array().push_back(data);
    }

    void json::push_back(json&& data) {
        to_array().push_back(std::move(data));
    }

    void json::pop_back() {
        assert(is_array());
        _Array->pop_back();
    }

    void json::set_object() {
        _Tidy();
        _Object = mallocator<Object>().allocate();
        new (_Object) Object();
        _Type = (uint8_t)(value_t::object);
    }

    void json::set_object(json_object&& value) {
        _Tidy();
        _Object = mallocator<Object>().allocate();
        new (_Object) Object(std::move(value));
        _Type = (uint8_t)(value_t::object);
    }

    void json::set_array() {
        _Tidy();
        _Array = mallocator<Array>().allocate();
        new (_Array) Array();
        _Type = (uint8_t)(value_t::array);
    }

    void json::set_array(json_array&& value) {
        _Tidy();
        _Array = mallocator<Array>().allocate();
        new (_Array) Array(std::move(value));
        _Type = (uint8_t)(value_t::array);
    }

    size_t json::size()const {
        assert(is_array() || is_object());
        return is_array() ? _Array->size() : _Object->size();
    }

    bool json::count(const size_t index)const {
        assert(is_array());
        return _Array->size() > index;
    }

    bool json::empty()const {
        return _Type == (uint8_t)(value_t::undefined);
    }

    bool json::has_value()const {
        return _Type != (uint8_t)(value_t::undefined);
    }

    json_string json::stringify(int tab)const {
        return stringify(0, tab);
    }

    json_string json::minify()const {
        json_string str;
        _minify(str);
        return str;
    }

    void json::MergePatch(json& Target, const json& Patch) {
        if (Patch.is_object()) {
            if (!Target.is_object()) {
                Target.set_object();
            }
            auto& lhs_obj = Target.to_object();
            auto& rhs_obj = Patch.to_object();
            for (auto& Value : rhs_obj) {
                if (Value.second.is_null()) {
                    auto iter = lhs_obj.find(Value.first);
                    if(iter != lhs_obj.end())
                        lhs_obj.erase(iter);
                }
                else {
                    MergePatch(lhs_obj[Value.first],Value.second);
                }
            }
        }
        else {
            Target = Patch;
        }
    }

    void json::_minify(json_string& str)const {
        switch (_Type) {
            case (uint8_t)(value_t::null) : {
                str.append("null");
                break;
            }
            case (uint8_t)(value_t::boolean) : {
                if(to_boolean())
                    str.append("true");
                else str.append("false");
                break;
            }
            case (uint8_t)(value_t::number) : {
                str.append(to_accurate_string(to_number()));
                break;
            }
            case (uint8_t)(value_t::string) : {
                auto& s = to_string();
                str.reserve(str.size() + s.size() + 2);
                str.push_back('"');
                str.append(s);
                str.push_back('"');
                break;
            }
            case (uint8_t)(value_t::object) : {
                str.push_back('{');
                auto& obj = to_object();
                if (!obj.empty()) {
                    bool head = true;
                    for (auto& i : obj) {
                        if (head) head = false;
                        else str.push_back(',');
                        str.push_back('"');
                        str.append(i.first);
                        str.append("\":");
                        i.second._minify(str);
                    }
                }
                str.push_back('}');
                break;
            }
            case (uint8_t)(value_t::array) : {
                str.push_back('[');
                const auto& umap = to_array();
                if (!umap.empty()) {
                    bool head = true;
                    for (auto& i : umap) {
                        if (head) head = false;
                        else str.push_back(',');
                        i._minify(str);
                    }
                }
                str.push_back(']');
                break;
            }
            default: {
                assert(false);
                break;
            }
        }
    }

    const char* json::type_name()const {
        switch (_Type) {
            case (uint8_t)(value_t::undefined) : {
                return "undefined";
            }
            case (uint8_t)(value_t::null) : {
                return "null";
            }
            case (uint8_t)(value_t::boolean): {
                return "boolean";
            }
            case (uint8_t)(value_t::number): {
                return "number";
            }
            case (uint8_t)(value_t::object) : {
                return "object";
            }
            case (uint8_t)(value_t::array) : {
                return "array";
            }
            default: {
                assert(false);
                return "error";
            }
        }
    }

    void json::merge_patch(const json& apply_patch) {
        MergePatch(*this,apply_patch);
    }

    json json::eval(std::string_view str) {
        json x;
        x.parse(str.data(),str.length());
        return x;
    }

    bool check(const uint8_t*& s, const uint8_t* e,uint8_t state) {
        bool head = true;
        for (;;++s) {
            s = skip_whitespace(s,e);
            if(s == e)return false;

            if (state == '}') { // object
                if (*s == '}') {
                    if(!head) return false;
                    ++s;
                    return true;
                }
                if (*s != '"')return false;
                if(!check_string(s,e))
                    return false;
                ++s;
                s = skip_whitespace(s,e);
                if (s == e || *s != ':') return false;
                ++s;
                s = skip_whitespace(s, e);
                if (s == e) return false;
            }
            else {
                if (*s == ']') {
                    if(!head)return false;
                    ++s;
                    return true;
                }
            }
            head = false;

            switch (*s) {
            case 'n':
                if(memcmp(s,"null",sizeof(char)*4) != 0)
                    return false;
                s += 4;
                break;
            case 't':
                if(memcmp(s,"true",sizeof(char)*4) !=0)
                    return false;
                s += 4;
                break;
            case 'f':
                if(memcmp(s,"false",sizeof(char)*5)!=0)
                    return false;
                s += 5;
                break;
            case '"':
                if(!check_string(s,e))
                    return false;
                ++s;
                break;
            case '[':
                ++s;
                if(!check(s,e,']'))
                    return false;
                break;
            case '{':
                ++s;
                if(!check(s,e,'}'))
                    return false;
                break;
            default:
                if(!check_num(s,e))
                    return false;
                break;
            }

            s = skip_whitespace(s,e);
            if(*s == ',')
                continue;
            if(*s != state)
                return false;
            ++s;
            return true;
        }
    }

    bool json::accept(std::string_view str) {

        const uint8_t* s = (const uint8_t*)str.data();
        const uint8_t* e = s + str.length();

        s = skip_whitespace(s,e);

        if(s == e) // empty
            return false;

        switch (*s) {
        case '[':
            ++s;
            if(!check(s,e,']'))
                return false;
            s = skip_whitespace(s,e);
            return s == e;
        case '{':
            ++s;
            if(!check(s,e,'}'))
                return false;
            s = skip_whitespace(s, e);
            return s == e;
        default:
            return false;
        }

    }

    json json::array(std::initializer_list<json> il) {
        json x;
        x._Type = (uint8_t)(json::value_t::array);
        x._Array = mallocator<Array>().allocate();
        new (x._Array) Array(il);
        return x;
    }

    json json::object(std::initializer_list<std::pair<const String,json>> il) {
        json x;
        x._Type = (uint8_t)(json::value_t::object);
        x._Object = mallocator<Object>().allocate();
        new (x._Object) Object(il);
        return x;
    }

    bool operator==(const json& lhs, const json& rhs) {
        if (lhs._Type != rhs._Type)return false;
        switch (lhs._Type) {
            case (uint8_t)(json::value_t::boolean) : {
                return lhs._Boolean == rhs._Boolean;
            }
            case (uint8_t)(json::value_t::number) : {
                static double eps = 1e-8;
                return std::fabs(lhs._Number - rhs._Number) < eps;
            }
            case (uint8_t)(json::value_t::string) : {
                return lhs.to_string() == rhs.to_string();
            }
            case (uint8_t)(json::value_t::array) : {
                return lhs.to_array() == rhs.to_array();
            }
            case (uint8_t)(json::value_t::object) : {
                return lhs.to_object() == rhs.to_object();
            }
            default: {
                return true;
            }
        }
    }

    bool operator!=(const json& lhs, const json& rhs) {
        return !(lhs == rhs);
    }

    std::ostream& operator<<(std::ostream& out, const json& x) {
        return out << x.stringify();
    }

    json_string json::format_tab(int tab) {
        return json_string(tab, ' ');
    }

    json_string json::stringify(int tab, int delta)const {
        switch (_Type) {
            case (uint8_t)(value_t::null) : {
                return "null";
            }
            case (uint8_t)(value_t::boolean) : {
                return to_boolean() ? "true" : "false";
            }
            case (uint8_t)(value_t::number) : {
                return to_accurate_string(to_number());
            }
            case (uint8_t)(value_t::string) : {
                json_string str;
                auto& s = to_string();
                str.reserve(s.size() + 2);
                str.push_back('"');
                str.append(s);
                str.push_back('"');
                return str;
            }
            case (uint8_t)(value_t::object) : {
                json_string str;
                str.push_back('{');
                auto& obj = to_object();
                if (!obj.empty()) {
                    bool head = true;
                    for (auto& i : obj) {
                        if (head) head = false;
                        else str.push_back(',');
                        str.push_back('\n');
                        str.append(format_tab(tab + delta));
                        str.push_back('"');
                        str.append(i.first);
                        str.append("\":");
                        str.push_back(' ');
                        str.append(i.second.stringify(tab + delta, delta));
                    }
                    str.push_back('\n');
                    str.append(format_tab(tab));
                }
                str.push_back('}');
                return str;
            }
            case (uint8_t)(value_t::array) : {
                json_string str;
                str.push_back('[');
                const auto& umap = to_array();
                if (!umap.empty()) {
                    bool head = true;
                    for (auto& i : umap) {
                        if (head) head = false;
                        else str.push_back(',');
                        str.push_back('\n');
                        str.append(format_tab(tab + 2));
                        str.append(i.stringify(tab + delta, delta));
                    }
                    str.push_back('\n');
                    str.append(format_tab(tab));
                }
                str.push_back(']');
                return str;
            }
            default: {
                assert(false);
                return "";
            }
        }
    }

    void json::getValue(const uint8_t*& s,const uint8_t* e) {

        s = skip_whitespace(s,e);

        switch (*s) {
        case ']':
        case '}':
            break;
        case 'n': {
            _Type = (uint8_t)(value_t::null);
            s += 4;
            break;
        }
        case 't': {
            _Type = (uint8_t)(value_t::boolean);
            _Boolean = true;
            s += 4;
            break;
        }
        case 'f': {
            _Type = (uint8_t)(value_t::boolean);
            _Boolean = false;
            s += 5;
            break;
        }
        case '{': {
            _Type = (uint8_t)(value_t::object);
            _Object = mallocator<Object>().allocate();
            new (_Object) Object();
            auto& obj = *_Object;
            ++s;
            for (;;) {
                s = skip_whitespace(s,e);
                if (*s == '}') {
                    ++s;
                    break;
                }

                assert(*s == '"');

                ++s;
                auto p = skip_string(s, e);

                auto& iter = obj[String((const char*)s,  p - s)] = json();
                s = p + 1;
                s = skip_whitespace(s,e);
                assert(*s == ':');
                ++s;
                iter.getValue(s,e);
                s = skip_whitespace(s,e);
                if (*s == '}') {
                    ++s;
                    break;
                }
                assert(*s == ',');
                ++s;
            }
            break;
        }
        case '[': {
            _Type = (uint8_t)(value_t::array);
            _Array = mallocator<Array>().allocate();
            new (_Array) Array();
            auto& arr = *_Array;
            ++s;
            for (;;) {
                json _value;
                _value.getValue(s,e);
                if (!_value.has_value()) {
                    ++s;
                    break;
                }
                arr.push_back(std::move(_value));
                s = skip_whitespace(s,e);
                if (*s == ']') {
                    ++s;
                    break;
                }
                assert(*s == ',');
                ++s;
            }
            arr.shrink_to_fit();
            break;
        }
        case '"': {
            ++s;
            auto t = skip_string(s,e);
            _Type = (uint8_t)(value_t::string);
            _String = mallocator<String>().allocate();
            new (_String) String((const char*)s, t - s);
            s = t + 1;
            break;
        }
        case '-':
        case '+':
        default: {
            _Type = (uint8_t)(value_t::number);
            double _double;
            s += read_double((const char*)s,_double);
            _Number = _double;
            break;
        }
        }
    }

    json operator ""_json(const char* str, const size_t) {
        json x;
        x.parse(str);
        return x;
    }

}
