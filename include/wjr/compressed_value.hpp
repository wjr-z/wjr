#ifndef WJR_COMPRESSED_VALUE_HPP__
#define WJR_COMPRESSED_VALUE_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {
template <typename T, T uniq_value>
class compressed_value {
    static_assert(std::is_trivially_copyable_v<T>, "Only support trivial type currently.");
    static_assert(sizeof(T) <= sizeof(void *), "Don't need to compress.");

public:
    constexpr compressed_value() noexcept : m_val(uniq_value) {}
    compressed_value(const compressed_value &) = default;
    compressed_value(compressed_value &&) = default;
    compressed_value &operator=(const compressed_value &) = default;
    compressed_value &operator=(compressed_value &&) = default;
    ~compressed_value() = default;

    constexpr compressed_value(T val) noexcept : m_val(val) {
        WJR_ASSERT_ASSUME_L2(m_val != uniq_value);
    }

    WJR_PURE constexpr operator T() const noexcept { return m_val; }

private:
    T m_val;
};
} // namespace wjr

#endif // WJR_COMPRESSED_VALUE_HPP__