#include <wjr/assert.hpp>

namespace wjr {

void __assert_failed(const char *expr, const char *file, const char *func, int line)noexcept {
    if (file[0] != '\0') {
        std::cerr << file << ':';
    }

    if (line != -1) {
        std::cerr << line << ':';
    }

    std::cerr << func << ": Assertion `" << expr << "' failed.\n";
    std::abort();
}

} // namespace wjr