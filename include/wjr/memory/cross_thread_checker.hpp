#ifndef WJR_MEMORY_CROSS_THREAD_CHECKER_HPP__
#define WJR_MEMORY_CROSS_THREAD_CHECKER_HPP__

#include <thread>

#include <wjr/assert.hpp>

namespace wjr {

class __debug_cross_thread_checker {
public:
    __debug_cross_thread_checker() : m_thread_id(std::this_thread::get_id()) {}
    __debug_cross_thread_checker(const __debug_cross_thread_checker &) = default;
    __debug_cross_thread_checker &
    operator=(const __debug_cross_thread_checker &) = default;
    ~__debug_cross_thread_checker() = default;

    void check() const {
        WJR_ASSERT_L(1, m_thread_id == std::this_thread::get_id(),
                     "Cross-thread access detected");
    }

    friend bool operator==(const __debug_cross_thread_checker &lhs,
                           const __debug_cross_thread_checker &rhs) {
        return lhs.m_thread_id == rhs.m_thread_id;
    }

    friend bool operator!=(const __debug_cross_thread_checker &lhs,
                           const __debug_cross_thread_checker &rhs) {
        return lhs.m_thread_id != rhs.m_thread_id;
    }

private:
    std::thread::id m_thread_id;
};

class __release_cross_thread_checker {
public:
    void check() const {};

    friend bool operator==(const __release_cross_thread_checker &,
                           const __release_cross_thread_checker &) {
        return true;
    }

    friend bool operator!=(const __release_cross_thread_checker &,
                           const __release_cross_thread_checker &) {
        return false;
    }
};

using cross_thread_checker = WJR_DEBUG_IF(1, __debug_cross_thread_checker,
                                          __release_cross_thread_checker);

} // namespace wjr

#endif // WJR_MEMORY_CROSS_THREAD_CHECKER_HPP__