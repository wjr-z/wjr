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
    std::exit(EXIT_FAILURE);
}

} // namespace wjr