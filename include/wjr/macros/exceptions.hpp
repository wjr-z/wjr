#ifndef WJR_MACROS_EXCEPTIONS_HPP__
#define WJR_MACROS_EXCEPTIONS_HPP__

#if defined(WJR_DISABLE_EXCEPTIONS)
    #include <cstdlib>

    #define WJR_TRY if constexpr (true)
    #define WJR_CATCH(...) if constexpr (false)
    #define WJR_THROW(X) std::exit(EXIT_FAILURE)
    #define WJR_XTHROW std::exit(EXIT_FAILURE)
    #define WJR_NOEXCEPT_X true
#else
    #define WJR_TRY try
    #define WJR_CATCH(...) catch (__VA_ARGS__)
    #define WJR_THROW(X) throw X
    #define WJR_XTHROW throw
    #define WJR_NOEXCEPT_X false
#endif

#endif // WJR_MACROS_EXCEPTIONS_HPP__