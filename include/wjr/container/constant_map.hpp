#ifndef WJR_CONTAINER_CONSTANT_MAP_HPP__
#define WJR_CONTAINER_CONSTANT_MAP_HPP__

#include <array>
#include <string_view>

#include <wjr/type_traits.hpp>

namespace wjr {

namespace constant {

template <size_t KeySize>
class string_map {
public:
    using key_type = std::string_view;

    constexpr string_map(const key_type (&keys)[KeySize]) noexcept : m_keys(keys) {}

private:
    std::array<key_type, KeySize> m_keys;
};

} // namespace constant

} // namespace wjr

#endif // WJR_CONTAINER_CONSTANT_MAP_HPP__