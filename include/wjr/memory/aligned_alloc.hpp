#ifndef WJR_MEMORY_ALIGNED_ALLOC_HPP__
#define WJR_MEMORY_ALIGNED_ALLOC_HPP__

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
#else
    #if defined(WJR_HAS_UNISTD_H)
        #include <unistd.h>
    #endif

    #if defined(_POSIX_VERSION)
        #include <wjr/arch/preview/memory/aligned_alloc_posix.hpp>
    #else
        #include <wjr/arch/preview/memory/aligned_alloc_malloc.hpp>
    #endif
#endif

#include <wjr/memory/align.hpp>

namespace wjr {
namespace mem {
template <size_t _Align = default_new_alignment>
WJR_MALLOC void *__default_allocate(size_t size) {
    if constexpr (default_new_alignment <= stdcpp_default_new_alignment &&
                  _Align <= stdcpp_default_new_alignment) {
        return ::operator new[](size);
    } else {
        return ::operator new[](size,
                                std::align_val_t{std::max<size_t>(default_new_alignment, _Align)});
    }
}

template <size_t _Align = default_new_alignment>
WJR_MALLOC void *__default_allocate(size_t size, std::nothrow_t) {
    if constexpr (default_new_alignment <= stdcpp_default_new_alignment &&
                  _Align <= stdcpp_default_new_alignment) {
        return ::operator new[](size, std::nothrow);
    } else {
        return ::operator new[](
            size, std::align_val_t{std::max<size_t>(default_new_alignment, _Align)}, std::nothrow);
    }
}

template <size_t _Align = default_new_alignment>
void __default_deallocate(void *ptr) {
    if constexpr (default_new_alignment <= stdcpp_default_new_alignment &&
                  _Align <= stdcpp_default_new_alignment) {
        ::operator delete[](ptr);
    } else {
        ::operator delete[](ptr, std::align_val_t{std::max<size_t>(default_new_alignment, _Align)});
    }
}

template <size_t _Align = default_new_alignment>
void __default_deallocate(void *ptr, std::nothrow_t) {
    if constexpr (default_new_alignment <= stdcpp_default_new_alignment &&
                  _Align <= stdcpp_default_new_alignment) {
        ::operator delete[](ptr, std::nothrow);
    } else {
        ::operator delete[](ptr, std::align_val_t{std::max<size_t>(default_new_alignment, _Align)},
                            std::nothrow);
    }
}

} // namespace mem
} // namespace wjr

#endif // WJR_MEMORY_ALIGNED_ALLOC_HPP__