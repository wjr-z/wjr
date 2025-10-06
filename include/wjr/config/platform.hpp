/**
 * @file config/platform.hpp
 * @brief Operating system detection
 * @author wjr
 *
 * Detects the target operating system (Windows, Linux, FreeBSD, Android, etc.)
 * and includes platform-specific configuration headers.
 */

#ifndef WJR_CONFIG_PLATFORM_HPP__
#define WJR_CONFIG_PLATFORM_HPP__

////////////////////////////////////////////////////////////////////////////////
// Os
////////////////////////////////////////////////////////////////////////////////

#if defined(WIN32) || defined(_WIN64) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // define something for Windows (32-bit and 64-bit, this part is common)
    #include <wjr/config/platform/windows.hpp>
#elif (defined(__freebsd__) || defined(__FreeBSD__))
    #define WJR_OS_FREEBSD
#elif defined(__OpenBSD__)
    #define WJR_OS_OPENBSD
#elif defined(__ANDROID__)
    #define WJR_OS_ANDROID
#elif (defined(linux) || defined(__linux) || defined(__linux__))
    #include <wjr/config/platform/linux.hpp>
#elif defined(__unix__) // all unices not caught above
    #define WJR_OS_UNIX
#else
    #error "Unknown platform!"
#endif

#endif // WJR_CONFIG_PLATFORM_HPP__