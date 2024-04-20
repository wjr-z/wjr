#ifndef WJR_VARIANT_HPP__
#define WJR_VARIANT_HPP__

#include <wjr/memory/uninitialized.hpp>

namespace wjr {

template <typename... Ts>
using __variant_index_type = std::conditional_t<(sizeof...(Ts) < 256), uint8_t, uint16_t>;

}

#endif // WJR_VARIANT_HPP__