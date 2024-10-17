#ifndef WJR_CONTAINER_CONSTANT_MAP_HPP__
#define WJR_CONTAINER_CONSTANT_MAP_HPP__

#include <array>
#include <string_view>

#include <wjr/algorithm.hpp>

namespace wjr {

namespace constant {

template <size_t KeySize>
class string_map {
public:
    using key_type = std::string_view;

    constexpr string_map(const key_type (&keys)[KeySize]) noexcept : m_keys(keys) {
        constant::sort(m_keys.begin(), m_keys.end());
    }

private:
    std::array<key_type, KeySize> m_keys;
};

template <size_t KeySize>
string_map(const std::string_view (&keys)[KeySize]) -> string_map<KeySize>;

} // namespace constant

} // namespace wjr

#endif // WJR_CONTAINER_CONSTANT_MAP_HPP__