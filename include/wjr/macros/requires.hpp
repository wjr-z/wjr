#ifndef WJR_MACROS_REQUIRES_HPP__
#define WJR_MACROS_REQUIRES_HPP__

#define WJR_REQUIRES(...) std::enable_if_t<(__VA_ARGS__), int> = 0
#define WJR_REQUIRES_I(...) std::enable_if_t<(__VA_ARGS__), int>

#endif // WJR_MACROS_REQUIRES_HPP__