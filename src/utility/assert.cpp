#include <exception> // for terminate

#include <wjr/assert.hpp>

namespace wjr {

void __assert_failed(const char *expr, const char *file, const char *func, int line) noexcept {
    if (file[0] != '\0') {
        std::cerr << file << ':';
    }

    if (line != -1) {
        std::cerr << line << ':';
    }

    std::cerr << func << ": Assertion `" << expr << "' failed.\n";
    std::terminate();
}

WJR_NORETURN extern void __assert_light_failed(const char *expr) noexcept {
    std::cerr << "Assertion `" << expr << "' failed.\n";
    std::terminate();
}

} // namespace wjr