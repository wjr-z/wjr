/**
 * @file memory/asan.hpp
 * @brief AddressSanitizer (ASan) utility wrappers
 * @author wjr
 *
 * Detects AddressSanitizer at compile time and provides portable macros for:
 * - Poisoning / unpoisoning memory regions
 * - Querying whether an address or region is poisoned
 *
 * All macros expand to harmless no-ops when ASan is not enabled, so they
 * can be used unconditionally without `#ifdef` guards at the call site.
 *
 * Usage example:
 * @code
 *   // After deallocating internal buffer slots, poison them so stale
 *   // accesses are caught immediately under ASan.
 *   WJR_ASAN_POISON_MEMORY_REGION(ptr, n * sizeof(T));
 *   // ... later, before handing the slots back to the user:
 *   WJR_ASAN_UNPOISON_MEMORY_REGION(ptr, n * sizeof(T));
 * @endcode
 */

#ifndef WJR_MEMORY_ASAN_HPP__
#define WJR_MEMORY_ASAN_HPP__

#include <wjr/config/compiler.hpp>

// ---------------------------------------------------------------------------
// ASan detection
// ---------------------------------------------------------------------------
// GCC, Clang and MSVC all define __SANITIZE_ADDRESS__ when compiled with
// -fsanitize=address.  Clang additionally exposes __has_feature() which
// works even in headers that are parsed before the sanitizer flags take
// effect (e.g. pre-compiled headers), so we check that as a fallback.
#if defined(__SANITIZE_ADDRESS__)
    #define WJR_HAS_ADDRESS_SANITIZER 1
#elif defined(WJR_COMPILER_CLANG) && defined(__has_feature)
    #if __has_feature(address_sanitizer)
        #define WJR_HAS_ADDRESS_SANITIZER 1
    #endif
#endif

// ---------------------------------------------------------------------------
// Wrapper macros
// ---------------------------------------------------------------------------
#if defined(WJR_HAS_ADDRESS_SANITIZER)

    #include <sanitizer/asan_interface.h>

    /**
     * @brief Mark [addr, addr+size) as poisoned (inaccessible).
     *
     * Any instrumented access to this region will be reported as an error.
     * The memory must have been previously allocated by the user program.
     */
    #define WJR_ASAN_POISON_MEMORY_REGION(addr, size) ASAN_POISON_MEMORY_REGION((addr), (size))

    /**
     * @brief Mark [addr, addr+size) as unpoisoned (accessible).
     *
     * Reverses a previous @ref WJR_ASAN_POISON_MEMORY_REGION call.
     * Due to ASan alignment restrictions, a slightly larger region may be
     * unpoisoned.
     */
    #define WJR_ASAN_UNPOISON_MEMORY_REGION(addr, size) ASAN_UNPOISON_MEMORY_REGION((addr), (size))

    /**
     * @brief Return non-zero if the single byte at @p addr is poisoned.
     */
    #define WJR_ASAN_ADDRESS_IS_POISONED(addr) __asan_address_is_poisoned((addr))

    /**
     * @brief Return a pointer to the first poisoned byte in [addr, addr+size),
     *        or @c nullptr if the whole region is accessible.
     */
    #define WJR_ASAN_REGION_IS_POISONED(addr, size) __asan_region_is_poisoned((addr), (size))

    /**
     * @brief Annotate a contiguous container's valid range for ASan.
     *
     * Informs ASan that the accessible range within [beg, end) has changed
     * from [beg, old_mid) to [beg, new_mid).  This must be called whenever
     * the logical size of a container changes without going through a
     * standard allocator (e.g. after hacking the size field directly).
     *
     * @param beg     Start of the allocated buffer.
     * @param end     One past the end of the allocated buffer (beg + capacity + 1 for strings).
     * @param old_mid One past the last previously accessible byte (beg + old_size).
     * @param new_mid One past the last newly accessible byte  (beg + new_size).
     */
    #define WJR_ASAN_ANNOTATE_CONTIGUOUS_CONTAINER(beg, end, old_mid, new_mid)                     \
        __sanitizer_annotate_contiguous_container((beg), (end), (old_mid), (new_mid))

#else // !WJR_HAS_ADDRESS_SANITIZER — all macros are no-ops

    #define WJR_ASAN_POISON_MEMORY_REGION(addr, size)
    #define WJR_ASAN_UNPOISON_MEMORY_REGION(addr, size)
    #define WJR_ASAN_ADDRESS_IS_POISONED(addr)
    #define WJR_ASAN_REGION_IS_POISONED(addr, size)
    #define WJR_ASAN_ANNOTATE_CONTIGUOUS_CONTAINER(beg, end, old_mid, new_mid)

#endif // WJR_HAS_ADDRESS_SANITIZER

#endif // WJR_MEMORY_ASAN_HPP__