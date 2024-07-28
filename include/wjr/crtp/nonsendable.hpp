#ifndef WJR_CRTP_NONSENDABLE_HPP__
#define WJR_CRTP_NONSENDABLE_HPP__

/**
 * @file nonsendable.hpp
 * @author wjr
 * @brief  A type to disable sending the object to another thread.
 *
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
 */

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
 * @brief A type to disable sending the object to another thread.
 *
 */
template <typename Tag = void>
class nonsendable {
public:
    static constexpr bool is_nonsendable = true;

protected:
    nonsendable() noexcept : m_thread_id(std::this_thread::get_id()) {}
    nonsendable(const nonsendable &other) noexcept { check(other); }
    nonsendable(nonsendable &&other) noexcept { check(other); }
    nonsendable &operator=(const nonsendable &other) noexcept {
        check(other);
        return *this;
    }
    nonsendable &operator=(nonsendable &&other) noexcept {
        check(other);
        return *this;
    }
    ~nonsendable() = default;

private:
    void check(const nonsendable &other) noexcept {
        WJR_ASSERT_L0(m_thread_id == other.m_thread_id,
                      "Cross-thread access detected when using a nonsendable object.");
    }

    std::thread::id m_thread_id;
};

#else

/**
 * @brief A type to disable sending the object to another thread.
 *
 */
template <typename Tag = void>
class nonsendable {
public:
    static constexpr bool is_nonsendable = true;

protected:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(nonsendable);
};

#endif

} // namespace wjr

#endif // WJR_CRTP_NONSENDABLE_HPP__