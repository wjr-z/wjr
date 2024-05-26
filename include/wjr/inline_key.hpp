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
class inline_key : private lazy<T> {
    using Mybase = lazy<T>;

public:
    using Mybase::Mybase;
    static_assert(!std::is_const_v<T>, "");

    constexpr inline_key(const T &value) noexcept(
        std::is_nothrow_constructible_v<Mybase, const T &>)
        : Mybase(value) {}

    constexpr const T &get() const noexcept { return Mybase::operator*(); }
    constexpr const T &operator*() const noexcept { return Mybase::operator*(); }
    constexpr const T *operator->() const noexcept { return Mybase::operator->(); }
};

template <typename T>
class inline_key<T, false> {
public:
    static_assert(!std::is_const_v<T>, "");

    constexpr inline_key() noexcept = default;
    constexpr inline_key(const inline_key &other) noexcept = default;
    constexpr inline_key(inline_key &&other) noexcept = default;
    constexpr inline_key &operator=(const inline_key &other) noexcept = default;
    constexpr inline_key &operator=(inline_key &&other) noexcept = default;
    ~inline_key() noexcept = default;

    constexpr inline_key(const T &value) noexcept : m_ptr(std::addressof(value)) {}

    constexpr const T &get() noexcept { return *m_ptr; }
    constexpr const T &operator*() noexcept { return *m_ptr; }
    constexpr const T *operator->() noexcept { return m_ptr; }

private:
    T *m_ptr;
};

template <typename T>
struct __is_possible_inline_key_impl
    : std::conjunction<std::is_trivially_copy_constructible<T>,
                       std::is_trivially_copy_assignable<T>,
                       std::is_trivially_destructible<T>> {};

template <typename T>
struct is_possible_inline_key : __is_possible_inline_key_impl<lazy<T>> {};

template <typename T>
inline constexpr bool is_possible_inline_key_v = is_possible_inline_key<T>::value;

template <typename T, size_t Threshold = sizeof(void *)>
using auto_key = inline_key<T, is_possible_inline_key_v<T> && sizeof(T) <= Threshold>;

} // namespace wjr

#endif // WJR_INLINE_KEY_HPP__