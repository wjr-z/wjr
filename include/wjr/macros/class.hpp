#ifndef WJR_MACROS_CLASS_HPP__
#define WJR_MACROS_CLASS_HPP__

#define WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(CLASS)                                                  \
    CLASS() = default;                                                                             \
    CLASS(const CLASS &) = default;                                                                \
    CLASS(CLASS &&) = default;                                                                     \
    CLASS &operator=(const CLASS &) = default;                                                     \
    CLASS &operator=(CLASS &&) = default;                                                          \
    ~CLASS() = default

#endif // WJR_MACROS_CLASS_HPP__