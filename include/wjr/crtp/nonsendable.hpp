#ifndef WJR_CRTP_NONSENDABLE_HPP__
#define WJR_CRTP_NONSENDABLE_HPP__

#include <memory>

#include <wjr/assert.hpp>

#if WJR_DEBUG_LEVEL > 2
#define WJR_HAS_DEBUG_NONSENDABLE_CHECKER WJR_HAS_DEF
#endif

#if WJR_HAS_DEBUG(NONSENDABLE_CHECKER)
#include <thread>
#endif

namespace wjr {

#if WJR_HAS_DEBUG(NONSENDABLE_CHECKER)

/**
 * @brief Disable sending the object to another thread and check the thread id.
 *
 * @note Only check if WJR_DEBUG_LEVEL > 2.
 */
template <typename Tag = void>
class __nonsendable_checker {
public:
    static constexpr bool is_nonsendable = true;

protected:
    __nonsendable_checker() : m_thread_id(std::this_thread::get_id()) {}
    __nonsendable_checker(const __nonsendable_checker &) = default;
    __nonsendable_checker(__nonsendable_checker &&) = default;
    __nonsendable_checker &operator=(const __nonsendable_checker &) = default;
    __nonsendable_checker &operator=(__nonsendable_checker &&) = default;
    ~__nonsendable_checker() { check(); }

    void check() const {
        WJR_ASSERT_LX(m_thread_id == std::this_thread::get_id(),
                      "Cross-thread access detected when using a nonsendable object.");
    }

    friend bool operator==(const __nonsendable_checker &lhs,
                           const __nonsendable_checker &rhs) {
        return lhs.m_thread_id == rhs.m_thread_id;
    }

    friend bool operator!=(const __nonsendable_checker &lhs,
                           const __nonsendable_checker &rhs) {
        return lhs.m_thread_id != rhs.m_thread_id;
    }

private:
    std::thread::id m_thread_id;
};

#else

/**
 * @brief Disable sending the object to another thread without checking.
 *
 */
template <typename Tag = void>
class __nonsendable_checker {
public:
    static constexpr bool is_nonsendable = true;

protected:
    constexpr static void check(){};

    friend bool operator==(const __nonsendable_checker &, const __nonsendable_checker &) {
        return true;
    }

    friend bool operator!=(const __nonsendable_checker &, const __nonsendable_checker &) {
        return false;
    }
};

#endif

/**
 * @brief A type to disable sending the object to another thread.
 *
 * @note By default, only check if object is destroyed and WJR_DEBUG_LEVEL > 2.
 * Use nonsendable::check() to manually check.
 *
 */
template <typename Tag = void>
using nonsendable = __nonsendable_checker<Tag>;

} // namespace wjr

#endif // WJR_CRTP_NONSENDABLE_HPP__