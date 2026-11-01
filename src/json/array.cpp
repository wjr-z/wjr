#include <wjr/json/array.hpp>

#include <stdexcept>

namespace wjr::json::detail {

void throw_array_size_mismatch() {
    WJR_THROW(std::length_error("JSON array projection size mismatch"));
}

} // namespace wjr::json::detail
