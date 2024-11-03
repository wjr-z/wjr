#ifndef WJR_PREPROCESSOR_CONFIG_COMPILER_HPP__
#define WJR_PREPROCESSOR_CONFIG_COMPILER_HPP__

#if defined(__clang__)
    #define WJR_COMPILER_CLANG
#elif defined(__GNUC__)
    #define WJR_COMPILER_GCC
#elif defined(_MSC_VER)
    #define WJR_COMPILER_MSVC
#else
    #error "Not support"
#endif

#if defined(__GNUC__)
    #define WJR_HAS_GCC(major, minor, patchlevel)                                                  \
        ((__GNUC__ > (major)) || (__GNUC__ == (major) && __GNUC_MINOR__ > (minor)) ||              \
         (__GNUC__ == (major) && __GNUC_MINOR__ == (minor) &&                                      \
          __GNUC_PATCHLEVEL__ >= (patchlevel)))
#else
    #define WJR_HAS_GCC(major, minor, patchlevel) 0
#endif // __GNUC__

#if defined(__clang__)
    #if defined(__apple_build_version__)
        #error "Not support"
    #endif

    #define WJR_HAS_CLANG(major, minor, patchlevel)                                                \
        ((__clang_major__ > (major)) ||                                                            \
         (__clang_major__ == (major) && __clang_minor__ > (minor)) ||                              \
         (__clang_major__ == (major) && __clang_minor__ == (minor) &&                              \
          __clang_patchlevel__ >= (patchlevel)))
#else
    #define WJR_HAS_CLANG(major, minor, patchlevel) 0
#endif

#if defined(_MSC_VER)
    #define WJR_HAS_MSVC(minor, level) (_MSC_VER >= (minor)*100 + (level))
#else
    #define WJR_HAS_MSVC(minor, level) 0
#endif

#if (defined(WJR_COMPILER_GCC) && !WJR_HAS_GCC(7, 1, 0)) ||                                        \
    (defined(WJR_COMPILER_CLANG) && !WJR_HAS_CLANG(5, 0, 0))
    #error "GCC 7.1.0 or Clang 5.0.0 or later is required"
#endif

#if defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC)
    #define WJR_CPP_STANDARD __cplusplus
#elif defined(WJR_COMPILER_MSVC)
    #define WJR_CPP_STANDARD _MSVC_LANG
#endif

#if WJR_CPP_STANDARD >= 199711L
    #define WJR_CPP_03
#endif
#if WJR_CPP_STANDARD >= 201103L
    #define WJR_CPP_11
#endif
#if WJR_CPP_STANDARD >= 201402L
    #define WJR_CPP_14
#endif
#if WJR_CPP_STANDARD >= 201703L
    #define WJR_CPP_17
#endif
#if WJR_CPP_STANDARD >= 202002L
    #define WJR_CPP_20
#endif

#ifndef WJR_CPP_17
    #error "required C++17 or later"
#endif // c++17

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

#endif // !WJR_PREPROCESSOR_CONFIG_COMPILER_HPP__