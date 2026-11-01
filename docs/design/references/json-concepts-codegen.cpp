/// @file Compiler-inspection probes; C++ callers and stable symbol names only.
#include <wjr/json/document.hpp>

using document = wjr::json::document;

extern "C" void init_number(document *pointer, int64_t value) { ::new (pointer) document(value); }
extern "C" void assign_number(document &value, int64_t number) { value = number; }
extern "C" int64_t read_number(const document &value) { return static_cast<int64_t>(value); }
extern "C" bool read_boolean(const document &value) { return static_cast<bool>(value); }
extern "C" std::string_view read_view(const document &value) {
    return static_cast<std::string_view>(value);
}
extern "C" void assign_array(document &value, document::array_type &array) {
    value = std::move(array);
}
