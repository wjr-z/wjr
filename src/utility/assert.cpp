#include <exception> // for terminate

#include <wjr/assert.hpp>

namespace wjr {

// LCOV_EXCL_START

void __assert_failed(const char *expr, const char *file, const char *func, int line) noexcept {
    if (file[0] != '\0') {
        std::cerr << file << ':';
    }

    if (line != -1) {
        std::cerr << line << ':';
    }

    std::cerr << func << ": Assertion `" << expr << "' failed." << std::endl;
    std::terminate();
}

WJR_NORETURN extern void __assert_light_failed(const char *expr) noexcept {
    std::cerr << "Assertion `" << expr << "' failed." << std::endl;
    std::terminate();
}

// LCOV_EXCL_STOP

} // namespace wjr