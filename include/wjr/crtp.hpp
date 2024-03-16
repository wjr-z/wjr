#ifndef WJR_CRT_HPP__
#define WJR_CRT_HPP__

#include <thread>
#include <type_traits>

#include <wjr/assert.hpp>

namespace wjr {

class __debug_nonsendable {
protected:
    __debug_nonsendable() : m_thread_id(std::this_thread::get_id()) {}
    __debug_nonsendable(const __debug_nonsendable &) = default;
    __debug_nonsendable(__debug_nonsendable &&) = default;
    __debug_nonsendable &operator=(const __debug_nonsendable &) = default;
    __debug_nonsendable &operator=(__debug_nonsendable &&) = default;
    ~__debug_nonsendable() { check(); }
    void check() const {
        WJR_ASSERT_L(2, m_thread_id == std::this_thread::get_id(),
                     "Cross-thread access detected.");
    }

    friend bool operator==(const __debug_nonsendable &lhs,
                           const __debug_nonsendable &rhs) {
        return lhs.m_thread_id == rhs.m_thread_id;
    }

    friend bool operator!=(const __debug_nonsendable &lhs,
                           const __debug_nonsendable &rhs) {
        return lhs.m_thread_id != rhs.m_thread_id;
    }

private:
    std::thread::id m_thread_id;
};

class __release_nonsendable {
protected:
    void check() const {};

    friend bool operator==(const __release_nonsendable &, const __release_nonsendable &) {
        return true;
    }

    friend bool operator!=(const __release_nonsendable &, const __release_nonsendable &) {
        return false;
    }
};

using nonsendable = WJR_DEBUG_IF(2, __debug_nonsendable, __release_nonsendable);

template <typename T>
struct is_nonsendable : std::is_base_of<nonsendable, T> {};

template <typename T>
inline constexpr bool is_nonsendable_v = is_nonsendable<T>::value;

template <typename T>
struct is_sendable : std::negation<is_nonsendable<T>> {};

template <typename T>
inline constexpr bool is_sendable_v = is_sendable<T>::value;

class noncopyable {
protected:
    noncopyable() = default;
    noncopyable(const noncopyable &) = delete;
    noncopyable(noncopyable &&) = default;
    noncopyable &operator=(const noncopyable &) = delete;
    noncopyable &operator=(noncopyable &&) = default;
    ~noncopyable() = default;
};

class nonmovable {
protected:
    nonmovable() = default;
    nonmovable(const nonmovable &) = default;
    nonmovable(nonmovable &&) = delete;
    nonmovable &operator=(const nonmovable &) = default;
    nonmovable &operator=(nonmovable &&) = delete;
    ~nonmovable() = default;
};

} // namespace wjr

#endif // WJR_CRT_HPP__