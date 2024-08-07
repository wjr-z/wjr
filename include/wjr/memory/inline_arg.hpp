#ifndef inline_kWJR_INLINE_ARG_HPP__ey
#define inline_kWJR_INLINE_ARG_HPP__ey

/**
 * @file inline_arg.hpp
 * @author wjr
 * @brief Sometimes need to use T instead of const T& to optimize.
 *
 * @version 0.1
 * @date 2024-07-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <wjr/memory/uninitialized.hpp>

namespace wjr {

/**
 * @brief Inlined arg for T.
 *
 * @tparam T
 * @tparam Inlined
 */
template <typename T, bool Inlined>
class inline_arg {
public:
    using value_type = std::remove_const_t<T>;
    using reference = std::add_const_t<T> &;
    using pointer = std::add_const_t<T> *;

    static constexpr bool is_inlined = true;

private:
    using storage_type = uninitialized<value_type>;

public:
    inline_arg() = delete;
    inline_arg(const inline_arg &) = delete;
    inline_arg(inline_arg &&) = delete;
    inline_arg &operator=(const inline_arg &) = delete;
    inline_arg &operator=(inline_arg &&) = delete;
    ~inline_arg() = default;

    constexpr inline_arg(reference value) noexcept(
        std::is_nothrow_constructible_v<storage_type, reference>)
        : m_storage(value) {}

    constexpr reference get() const noexcept { return m_storage.get(); }
    constexpr reference operator*() const noexcept { return *m_storage; }
    constexpr pointer operator->() const noexcept { return m_storage.operator->(); }

private:
    storage_type m_storage;
};

template <typename T>
class inline_arg<T, false> {
public:
    static_assert(!std::is_const_v<T>, "");

    using value_type = T;
    using reference = std::add_const_t<T> &;
    using pointer = std::add_const_t<T> *;

    static constexpr bool is_inlined = false;

    inline_arg() = delete;
    inline_arg(const inline_arg &) = delete;
    inline_arg(inline_arg &&) = delete;
    inline_arg &operator=(const inline_arg &) = delete;
    inline_arg &operator=(inline_arg &&) = delete;
    ~inline_arg() = default;

    constexpr inline_arg(reference value) noexcept : m_ptr(std::addressof(value)) {}

    constexpr reference get() const noexcept { return *m_ptr; }
    constexpr reference operator*() const noexcept { return *m_ptr; }
    constexpr pointer operator->() const noexcept { return m_ptr; }

private:
    pointer m_ptr;
};

template <typename T>
struct __is_possible_inline_arg_impl
    : std::conjunction<std::is_trivially_copy_constructible<T>,
                       std::is_trivially_destructible<T>> {};

template <typename T>
struct is_possible_inline_arg : __is_possible_inline_arg_impl<aligned_storage<T>> {};

template <typename T>
inline constexpr bool is_possible_inline_arg_v = is_possible_inline_arg<T>::value;

template <typename T, size_t Threshold = sizeof(void *) * 2>
using auto_arg = inline_arg<T, is_possible_inline_arg_v<T> && sizeof(T) <= Threshold>;

} // namespace wjr

#endif // inline_kWJR_INLINE_ARG_HPP__ey