#ifndef WJR_COMPRESSED_VALUE_HPP__
#define WJR_COMPRESSED_VALUE_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {
template <typename T, T uniq_success>
class compressed_value {
    static_assert(std::is_trivial_v<T>, "Only support trivial type currently.");
public:
    constexpr compressed_value() noexcept : m_val(uniq_success) {}
    compressed_value(const compressed_value &) = default;
    compressed_value(compressed_value &&) = default;
    compressed_value &operator=(const compressed_value &) = default;
    compressed_value &operator=(compressed_value &&) = default;
    ~compressed_value() = default;

    constexpr compressed_value(T val) noexcept : m_val(val) {
        WJR_ASSERT_ASSUME_L2(m_val != uniq_success);
    }

    WJR_PURE constexpr operator T() const noexcept { return m_val; }

private:
    T m_val;
};
} // namespace wjr

#endif // WJR_COMPRESSED_VALUE_HPP__