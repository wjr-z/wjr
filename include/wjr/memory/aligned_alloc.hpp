#ifndef WJR_MEMORY_ALIGNED_ALLOC_HPP__
#define WJR_MEMORY_ALIGNED_ALLOC_HPP__

#include <wjr/memory/align.hpp>

#if defined(__APPLE__) || defined(__APPLE_CC__) || defined(macintosh)
    #include <AvailabilityMacros.h>
#endif

#if defined(_MSC_VER) && !defined(UNDER_CE)
    #include <wjr/arch/preview/memory/aligned_alloc_msvc.hpp>
#elif defined(__MINGW32__) && (__MSVCRT_VERSION__ >= 0x0700)
    #include <wjr/arch/preview/memory/aligned_alloc_msvc.hpp>
#elif defined(__MINGW32__)
    #include <wjr/arch/preview/memory/aligned_alloc_mingw.hpp>
#elif MAC_OS_X_VERSION_MIN_REQUIRED >= 1090
    #include <wjr/arch/preview/memory/aligned_alloc_posix.hpp>
#elif MAC_OS_X_VERSION_MIN_REQUIRED >= 1060
    #include <wjr/arch/preview/memory/aligned_alloc_macos.hpp>
#elif defined(__ANDROID__)
    #include <wjr/arch/preview/memory/aligned_alloc_android.hpp>
#elif defined(__SunOS_5_11) || defined(__SunOS_5_12)
    #include <wjr/arch/preview/memory/aligned_alloc_posix.hpp>
#elif defined(sun) || defined(__sun)
    #include <wjr/arch/preview/memory/aligned_alloc_sunos.hpp>
#elif defined(_POSIX_VERSION)
    #include <wjr/arch/preview/memory/aligned_alloc_posix.hpp>
#else
    #include <wjr/arch/preview/memory/aligned_alloc_malloc.hpp>
#endif

#endif // WJR_MEMORY_ALIGNED_ALLOC_HPP__