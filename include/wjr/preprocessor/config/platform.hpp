#ifndef WJR_PREPROCESSOR_CONFIG_PLATFORM_HPP__
#define WJR_PREPROCESSOR_CONFIG_PLATFORM_HPP__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // define something for Windows (32-bit and 64-bit, this part is common)
    #define WJR_WINDOWS
#elif defined(__linux__)
    #define WJR_LINUX
#elif defined(__unix__) // all unices not caught above
    #define WJR_UNIX
#endif

#endif // WJR_PREPROCESSOR_CONFIG_PLATFORM_HPP__