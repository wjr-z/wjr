#ifndef WJR_CONTAINER_COMPRESSED_VALUE_HPP__
#define WJR_CONTAINER_COMPRESSED_VALUE_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

// todo: Remove this, use sentinel_traits instead.
template <typename T, T uniq_value>
class compressed_value {
    static_assert(std::is_trivially_copyable_v<T>, "Only support trivial type currently.");
    static_assert(sizeof(T) <= sizeof(void *), "Don't need to compress.");

    struct _tag {};
    constexpr explicit compressed_value(_tag) noexcept {}
};
} // namespace wjr

#endif // WJR_CONTAINER_COMPRESSED_VALUE_HPP__