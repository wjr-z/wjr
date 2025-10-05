#include <exception> // for terminate

#include <wjr/assert.hpp>

#if defined(WJR_ENABLE_ASSERT_BACKTRACE)
    #include <cpptrace/cpptrace.hpp>
#endif

namespace wjr {

// LCOV_EXCL_START

namespace {
void print_backtrace() {
#if defined(WJR_ENABLE_ASSERT_BACKTRACE)
    cpptrace::generate_trace().print();
#endif
}
} // namespace

void __assert_failed(const char *expr, const char *file, const char *func, int line) noexcept {
    if (file[0] != '\0') {
        std::cerr << file << ':';
    }

    if (line != -1) {
        std::cerr << line << ':';
    }

    std::cerr << func << ": Assertion `" << expr << "' failed." << std::endl;
    print_backtrace();
    std::terminate();
}

WJR_NORETURN extern void __assert_light_failed(const char *expr) noexcept {
    std::cerr << "Assertion `" << expr << "' failed." << std::endl;
    print_backtrace();
    std::terminate();
}

// LCOV_EXCL_STOP

} // namespace wjr