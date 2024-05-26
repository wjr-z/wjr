#ifndef WJR_INLINE_KEY_HPP__
#define WJR_INLINE_KEY_HPP__

/**
 * @file inline_key.hpp
 * @author wjr
 * @brief Optimize using T * or T for different types of `T'.
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <wjr/memory/uninitialized.hpp>

namespace wjr {

template <typename T, bool Inlined>
class inline_key {
    using non_const_t = std::remove_const_t<T>;
    using data_type = lazy<non_const_t>;

public:
    constexpr inline_key() noexcept(std::is_nothrow_default_constructible_v<data_type>) =
        default;
    constexpr inline_key(const inline_key &other) noexcept(
        std::is_nothrow_copy_constructible_v<data_type>) = default;
    constexpr inline_key(inline_key &&other) noexcept(
        std::is_nothrow_move_constructible_v<data_type>) = default;
    constexpr inline_key &operator=(const inline_key &other) noexcept(
        std::is_nothrow_copy_assignable_v<data_type>) = default;
    constexpr inline_key &operator=(inline_key &&other) noexcept(
        std::is_nothrow_move_assignable_v<data_type>) = default;
    ~inline_key() noexcept(std::is_nothrow_destructible_v<data_type>) = default;

    constexpr inline_key(const non_const_t &value) noexcept(
        std::is_nothrow_constructible_v<data_type, const non_const_t &>)
        : m_value(value) {}

    constexpr T &get() const noexcept { return *m_value; }
    constexpr T &operator*() const noexcept { return *m_value; }
    constexpr T *operator->() const noexcept { return m_value.operator->(); }

private:
    data_type m_value;
};

template <typename T>
class inline_key<T, false> {
public:
    constexpr inline_key() noexcept = default;
    constexpr inline_key(const inline_key &other) noexcept = default;
    constexpr inline_key(inline_key &&other) noexcept = default;
    constexpr inline_key &operator=(const inline_key &other) noexcept = default;
    constexpr inline_key &operator=(inline_key &&other) noexcept = default;
    ~inline_key() noexcept = default;

    constexpr inline_key(T &value) noexcept : m_ptr(std::addressof(value)) {}

    constexpr T &get() const noexcept { return *m_ptr; }
    constexpr T &operator*() const noexcept { return *m_ptr; }
    constexpr T *operator->() const noexcept { return m_ptr; }

private:
    T *m_ptr;
};

template <typename T>
struct is_possible_inline_key : std::conjunction<std::is_trivially_copy_constructible<T>,
                                                 std::is_trivially_destructible<T>> {};

template <typename T>
inline constexpr bool is_possible_inline_key_v = is_possible_inline_key<T>::value;

template <typename T, size_t Threshold = sizeof(char *)>
using auto_key = inline_key<T, is_possible_inline_key_v<T> && sizeof(T) <= Threshold>;

} // namespace wjr

#endif // WJR_INLINE_KEY_HPP__