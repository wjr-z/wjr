#include <wjr/assert.hpp>

namespace wjr {

void __assert_failed(const char *expr, const char *file, const char *func,
                     int line) noexcept {
    if (file[0] != '\0') {
        fprintf(stderr, "%s:", file);
    }

    if (line != -1) {
        fprintf(stderr, "%d:", line);
    }

    fprintf(stderr, "%s: Assertion `%s' failed.\n", func, expr);
    std::terminate();
}

WJR_NORETURN extern void __assert_light_failed(const char *expr) noexcept {
    fprintf(stderr, "Assertion `%s' failed.\n", expr);
    std::terminate();
}

} // namespace wjr