#ifndef WJR_CONFIG_BUILTIN_HPP__
#define WJR_CONFIG_BUILTIN_HPP__

#if defined(__cpp_char8_t)
    #define WJR_CHAR8_T
#endif // __cpp_char8_t

#if defined(__LINE__)
    #define WJR_LINE __LINE__
#elif defined(__COUNTER__)
    #define WJR_LINE __COUNTER__
#else
    #define WJR_LINE -1
#endif

#ifdef __FILE__
    #define WJR_FILE __FILE__
#else
    #define WJR_FILE ""
#endif

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) ||                        \
    (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
    #define WJR_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
    #define WJR_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
    #define WJR_CURRENT_FUNCTION __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) ||                                  \
    (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
    #define WJR_CURRENT_FUNCTION __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
    #define WJR_CURRENT_FUNCTION __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
    #define WJR_CURRENT_FUNCTION __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
    #define WJR_CURRENT_FUNCTION __func__
#else
    #define WJR_CURRENT_FUNCTION "(unknown)"
#endif

#endif // WJR_CONFIG_BUILTIN_HPP__