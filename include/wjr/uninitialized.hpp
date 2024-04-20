#ifndef WJR_UNINITIALIZED_HPP__
#define WJR_UNINITIALIZED_HPP__

#include <wjr/memory/uninitialized.hpp>

namespace wjr {

template <typename T, bool Trivial = std::is_trivial_v<T>>
class uninitialized {
public:
    constexpr uninitialized() noexcept = default;

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr uninitialized(Args &&...args) : m_value(std::forward<Args>(args)...) {}

    constexpr uninitialized(dctor_t) noexcept {}

    constexpr uninitialized(const uninitialized &) = default;
    constexpr uninitialized(uninitialized &&) noexcept = default;
    constexpr uninitialized &operator=(const uninitialized &) = default;
    constexpr uninitialized &operator=(uninitialized &&) noexcept = default;

    ~uninitialized() = default;

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr uninitialized &emplace(Args &&...args) {
        construct_at(std::addressof(m_value), std::forward<Args>(args)...);
        return *this;
    }

    constexpr void reset() noexcept {}

    constexpr T &get() & noexcept { return m_value; }
    constexpr const T &get() const & noexcept { return m_value; }
    constexpr T &&get() && noexcept { return std::move(m_value); }
    constexpr const T &&get() const && noexcept { return std::move(m_value); }

private:
    T m_value;
};

template <typename T>
class uninitialized<T, false> {
public:
private:
    std::aligned_storage_t<sizeof(T), alignof(T)> m_storage;
};

} // namespace wjr

#endif // WJR_UNINITIALIZED_HPP__