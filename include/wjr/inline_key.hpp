#ifndef WJR_INLINE_KEY_HPP__
#define WJR_INLINE_KEY_HPP__

#include <memory>

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T, bool Inlined>
class inline_key {
public:
    constexpr inline_key() noexcept(std::is_nothrow_default_constructible_v<T>) = default;
    constexpr inline_key(const inline_key &other) noexcept(
        std::is_nothrow_copy_constructible_v<T>) = default;
    constexpr inline_key(inline_key &&other) noexcept(
        std::is_nothrow_move_constructible_v<T>) = default;
    constexpr inline_key &operator=(const inline_key &other) noexcept(
        std::is_nothrow_copy_assignable_v<T>) = default;
    constexpr inline_key &operator=(inline_key &&other) noexcept(
        std::is_nothrow_move_assignable_v<T>) = default;
    ~inline_key() noexcept(std::is_nothrow_destructible_v<T>) = default;

    constexpr inline_key(const T &value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : m_value(value) {}

    constexpr const T &get() const noexcept { return m_value; }
    constexpr const T &operator*() const noexcept { return m_value; }
    constexpr const T *operator->() const noexcept { return std::addressof(m_value); }

private:
    T m_value;
};

template <typename T>
class inline_key<T, false> {
public:
    constexpr inline_key() noexcept(std::is_nothrow_default_constructible_v<T>) = default;
    constexpr inline_key(const inline_key &other) noexcept(
        std::is_nothrow_copy_constructible_v<T>) = default;
    constexpr inline_key(inline_key &&other) noexcept(
        std::is_nothrow_move_constructible_v<T>) = default;
    constexpr inline_key &operator=(const inline_key &other) noexcept(
        std::is_nothrow_copy_assignable_v<T>) = default;
    constexpr inline_key &operator=(inline_key &&other) noexcept(
        std::is_nothrow_move_assignable_v<T>) = default;
    ~inline_key() noexcept(std::is_nothrow_destructible_v<T>) = default;

    constexpr inline_key(const T &value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : m_ptr(std::addressof(value)) {}

    constexpr const T &get() const noexcept { return *m_ptr; }
    constexpr const T &operator*() const noexcept { return *m_ptr; }
    constexpr const T *operator->() const noexcept { return m_ptr; }

private:
    const T *m_ptr;
};

template <typename T>
struct is_possible_inline_key : std::conjunction<std::is_trivially_copyable<T>> {};

template <typename T>
inline constexpr bool is_possible_inline_key_v = is_possible_inline_key<T>::value;

template <typename T, size_t Threshold = 8>
using auto_key = inline_key<T, is_possible_inline_key_v<T> && sizeof(T) <= Threshold>;

} // namespace wjr

#endif // WJR_INLINE_KEY_HPP__