#ifndef WJR_PREPROCESSOR_CONFIG_PLATFORM_HPP__
#define WJR_PREPROCESSOR_CONFIG_PLATFORM_HPP__

////////////////////////////////////////////////////////////////////////////////
// Os
////////////////////////////////////////////////////////////////////////////////

#if defined(WIN32) || defined(_WIN64) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // define something for Windows (32-bit and 64-bit, this part is common)
    #define WJR_OS_WINDOWS
#elif (defined(__freebsd__) || defined(__FreeBSD__))
    #define WJR_OS_FREEBSD
#elif defined(__OpenBSD__)
    #define WJR_OS_OPENBSD
#elif defined(__ANDROID__)
    #define WJR_OS_ANDROID
#elif defined(__linux__)
    #define WJR_OS_LINUX
#elif defined(__unix__) // all unices not caught above
    #define WJR_OS_UNIX
#endif

#endif // WJR_PREPROCESSOR_CONFIG_PLATFORM_HPP__