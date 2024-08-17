#ifndef WJR_TYPE_TRAITS_HPP__
#define WJR_TYPE_TRAITS_HPP__

#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

#include <wjr/preprocessor.hpp>

// GCC and compatible compilers define internal macros with builtin type traits

#if defined(__SIZEOF_INT__)
    #define WJR_SIZEOF_INT __SIZEOF_INT__
#endif

#if defined(__SIZEOF_LONG__)
    #define WJR_SIZEOF_LONG __SIZEOF_LONG__
#endif

#if defined(__SIZEOF_LONG_LONG__)
    #define WJR_SIZEOF_LONG_LONG __SIZEOF_LONG_LONG__
#endif

#if defined(__SIZEOF_SHORT__)
    #define WJR_SIZEOF_SHORT __SIZEOF_SHORT__
#endif

#if defined(__SIZEOF_POINTER__)
    #define WJR_SIZEOF_POINTER __SIZEOF_POINTER__
#elif defined(_MSC_VER)
    #if defined(_M_AMD64) || defined(_M_ARM64) || defined(_M_IA64)
        #define WJR_SIZEOF_POINTER 8
    #else
        #define WJR_SIZEOF_POINTER 4
    #endif
#endif

#if defined(__SIZEOF_WCHAR_T__)
    #define WJR_SIZEOF_WCHAR_T __SIZEOF_WCHAR_T__
#endif

#if !defined(WJR_SIZEOF_SHORT) || !defined(WJR_SIZEOF_INT) ||                            \
    !defined(WJR_SIZEOF_LONG) || !defined(WJR_SIZEOF_LONG_LONG) ||                       \
    !defined(WJR_SIZEOF_POINTER)

    #if !defined(WJR_SIZEOF_SHORT)
        #if (USHRT_MAX + 0) == 0xff
            #define WJR_SIZEOF_SHORT 1
        #elif (USHRT_MAX + 0) == 0xffff
            #define WJR_SIZEOF_SHORT 2
        #elif (USHRT_MAX + 0) == 0xffffffff
            #define WJR_SIZEOF_SHORT 4
        #elif (USHRT_MAX + 0) == UINT64_C(0xffffffffffffffff)
            #define WJR_SIZEOF_SHORT 8
        #endif
    #endif // !defined(WJR_SIZEOF_SHORT)

    #if !defined(WJR_SIZEOF_INT)
        #if (UINT_MAX + 0) == 0xff
            #define WJR_SIZEOF_INT 1
        #elif (UINT_MAX + 0) == 0xffff
            #define WJR_SIZEOF_INT 2
        #elif (UINT_MAX + 0) == 0xffffffff
            #define WJR_SIZEOF_INT 4
        #elif (UINT_MAX + 0) == UINT64_C(0xffffffffffffffff)
            #define WJR_SIZEOF_INT 8
        #endif
    #endif // !defined(WJR_SIZEOF_INT)

    #if !defined(WJR_SIZEOF_LONG)
        #if (ULONG_MAX + 0) == 0xff
            #define WJR_SIZEOF_LONG 1
        #elif (ULONG_MAX + 0) == 0xffff
            #define WJR_SIZEOF_LONG 2
        #elif (ULONG_MAX + 0) == 0xffffffff
            #define WJR_SIZEOF_LONG 4
        #elif (ULONG_MAX + 0) == UINT64_C(0xffffffffffffffff)
            #define WJR_SIZEOF_LONG 8
        #endif
    #endif // !defined(WJR_SIZEOF_LONG)

    #if !defined(WJR_SIZEOF_LONG_LONG)
        #if defined(__hpux) // HP-UX's value of ULONG_LONG_MAX is unusable in preprocessor
                            // expressions
            #define WJR_SIZEOF_LONG_LONG 8
        #else

            // The list of the non-standard macros (the ones except ULLONG_MAX) is taken
            // from cstdint.hpp
            #if defined(ULLONG_MAX)
                #define WJR_ULLONG_MAX ULLONG_MAX
            #elif defined(ULONG_LONG_MAX)
                #define WJR_ULLONG_MAX ULONG_LONG_MAX
            #elif defined(ULONGLONG_MAX)
                #define WJR_ULLONG_MAX ULONGLONG_MAX
            #elif defined(_LLONG_MAX) // strangely enough, this one seems to be holding
                                      // the limit for the unsigned integer
                #define WJR_ULLONG_MAX _LLONG_MAX
            #endif

            #if (WJR_ULLONG_MAX + 0) == 0xff
                #define WJR_SIZEOF_LONG_LONG 1
            #elif (WJR_ULLONG_MAX + 0) == 0xffff
                #define WJR_SIZEOF_LONG_LONG 2
            #elif (WJR_ULLONG_MAX + 0) == 0xffffffff
                #define WJR_SIZEOF_LONG_LONG 4
            #elif (WJR_ULLONG_MAX + 0) == UINT64_C(0xffffffffffffffff)
                #define WJR_SIZEOF_LONG_LONG 8
            #endif

        #endif // defined(__hpux)
    #endif     // !defined(WJR_SIZEOF_LONG_LONG)

    #if !defined(WJR_SIZEOF_POINTER) && defined(UINTPTR_MAX)
        #if (UINTPTR_MAX + 0) == 0xffff
            #define WJR_SIZEOF_POINTER 2
        #elif (UINTPTR_MAX + 0) == 0xffffffff
            #define WJR_SIZEOF_POINTER 4
        #elif (UINTPTR_MAX + 0) == UINT64_C(0xffffffffffffffff)
            #define WJR_SIZEOF_POINTER 8
        #endif
    #endif // !defined(WJR_SIZEOF_POINTER) && defined(UINTPTR_MAX)

#endif

#if !defined(WJR_SIZEOF_WCHAR_T)

    #include <wchar.h>

    #if defined(_MSC_VER) && (_MSC_VER <= 1310 || defined(UNDER_CE) && _MSC_VER <= 1500)
        // MSVC 7.1 and MSVC 8 (arm) define WCHAR_MAX to a value not suitable for constant
        // expressions
        #define WJR_SIZEOF_WCHAR_T 2
    #elif (WCHAR_MAX + 0) == 0xff || (WCHAR_MAX + 0) == 0x7f
        #define WJR_SIZEOF_WCHAR_T 1
    #elif (WCHAR_MAX + 0) == 0xffff || (WCHAR_MAX + 0) == 0x7fff
        #define WJR_SIZEOF_WCHAR_T 2
    #elif (WCHAR_MAX + 0) == 0xffffffff || (WCHAR_MAX + 0) == 0x7fffffff
        #define WJR_SIZEOF_WCHAR_T 4
    #elif (WCHAR_MAX + 0) == UINT64_C(0xffffffffffffffff) ||                             \
        (WCHAR_MAX + 0) == INT64_C(0x7fffffffffffffff)
        #define WJR_SIZEOF_WCHAR_T 8
    #endif
#endif

#if !defined(WJR_SIZEOF_SHORT) || !defined(WJR_SIZEOF_INT) ||                            \
    !defined(WJR_SIZEOF_LONG) || !defined(WJR_SIZEOF_LONG_LONG) ||                       \
    !defined(WJR_SIZEOF_WCHAR_T)
    #error "Not support"
#endif

// Detect value sizes of the different floating point types. The value sizes may be less
// than the corresponding type sizes if the type contains padding bits. This is typical
// e.g. with x87 80-bit extended double types, which are often represented as 96 or
// 128-bit types. See: https://en.wikipedia.org/wiki/IEEE_754 For Intel x87 extended
// double see:
// https://en.wikipedia.org/wiki/Extended_precision#x86_Architecture_Extended_Precision_Format
// For IBM extended double (a.k.a. double-double) see:
// https://en.wikipedia.org/wiki/Long_double#Implementations,
// https://gcc.gnu.org/wiki/Ieee128PowerPC

#if defined(__FLT_RADIX__) && defined(__FLT_MANT_DIG__) && defined(__FLT_MAX_EXP__) &&   \
    defined(__DBL_MANT_DIG__) && defined(__DBL_MAX_EXP__) &&                             \
    defined(__LDBL_MANT_DIG__) && defined(__LDBL_MAX_EXP__)

    #if (__FLT_RADIX__ == 2)

        #if (__FLT_MANT_DIG__ == 11) && (__FLT_MAX_EXP__ == 16)     // IEEE 754 binary16
            #define WJR_SIZEOF_FLOAT_VALUE 2
        #elif (__FLT_MANT_DIG__ == 24) && (__FLT_MAX_EXP__ == 128)  // IEEE 754 binary32
            #define WJR_SIZEOF_FLOAT_VALUE 4
        #elif (__FLT_MANT_DIG__ == 53) && (__FLT_MAX_EXP__ == 1024) // IEEE 754 binary64
            #define WJR_SIZEOF_FLOAT_VALUE 8
        #elif (__FLT_MANT_DIG__ == 64 || __FLT_MANT_DIG__ == 53 ||                       \
               __FLT_MANT_DIG__ == 24) &&                                                \
            (__FLT_MAX_EXP__ == 16384) // x87 extended double, with full 64-bit
                                       // significand or reduced to 53 or 24 bits
            #define WJR_SIZEOF_FLOAT_VALUE 10
        #elif (__FLT_MANT_DIG__ == 106) &&                                               \
            (__FLT_MAX_EXP__ == 1024) // IBM extended double
            #define WJR_SIZEOF_FLOAT_VALUE 16
        #elif (__FLT_MANT_DIG__ == 113) &&                                               \
            (__FLT_MAX_EXP__ == 16384) // IEEE 754 binary128
            #define WJR_SIZEOF_FLOAT_VALUE 16
        #elif (__FLT_MANT_DIG__ == 237) &&                                               \
            (__FLT_MAX_EXP__ == 262144) // IEEE 754 binary256
            #define WJR_SIZEOF_FLOAT_VALUE 32
        #endif

        #if (__DBL_MANT_DIG__ == 11) && (__DBL_MAX_EXP__ == 16)     // IEEE 754 binary16
            #define WJR_SIZEOF_DOUBLE_VALUE 2
        #elif (__DBL_MANT_DIG__ == 24) && (__DBL_MAX_EXP__ == 128)  // IEEE 754 binary32
            #define WJR_SIZEOF_DOUBLE_VALUE 4
        #elif (__DBL_MANT_DIG__ == 53) && (__DBL_MAX_EXP__ == 1024) // IEEE 754 binary64
            #define WJR_SIZEOF_DOUBLE_VALUE 8
        #elif (__DBL_MANT_DIG__ == 64 || __DBL_MANT_DIG__ == 53 ||                       \
               __DBL_MANT_DIG__ == 24) &&                                                \
            (__DBL_MAX_EXP__ == 16384) // x87 extended double, with full 64-bit
                                       // significand or reduced to 53 or 24 bits
            #define WJR_SIZEOF_DOUBLE_VALUE 10
        #elif (__DBL_MANT_DIG__ == 106) &&                                               \
            (__DBL_MAX_EXP__ == 1024) // IBM extended double
            #define WJR_SIZEOF_DOUBLE_VALUE 16
        #elif (__DBL_MANT_DIG__ == 113) &&                                               \
            (__DBL_MAX_EXP__ == 16384) // IEEE 754 binary128
            #define WJR_SIZEOF_DOUBLE_VALUE 16
        #elif (__DBL_MANT_DIG__ == 237) &&                                               \
            (__DBL_MAX_EXP__ == 262144) // IEEE 754 binary256
            #define WJR_SIZEOF_DOUBLE_VALUE 32
        #endif

        #if (__LDBL_MANT_DIG__ == 11) && (__LDBL_MAX_EXP__ == 16)     // IEEE 754 binary16
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 2
        #elif (__LDBL_MANT_DIG__ == 24) && (__LDBL_MAX_EXP__ == 128)  // IEEE 754 binary32
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 4
        #elif (__LDBL_MANT_DIG__ == 53) && (__LDBL_MAX_EXP__ == 1024) // IEEE 754 binary64
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 8
        #elif (__LDBL_MANT_DIG__ == 64 || __LDBL_MANT_DIG__ == 53 ||                     \
               __LDBL_MANT_DIG__ == 24) &&                                               \
            (__LDBL_MAX_EXP__ == 16384) // x87 extended double, with full 64-bit
                                        // significand or reduced to 53 or 24 bits
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 10
        #elif (__LDBL_MANT_DIG__ == 106) &&                                              \
            (__LDBL_MAX_EXP__ == 1024) // IBM extended double
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 16
        #elif (__LDBL_MANT_DIG__ == 113) &&                                              \
            (__LDBL_MAX_EXP__ == 16384) // IEEE 754 binary128
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 16
        #elif (__LDBL_MANT_DIG__ == 237) &&                                              \
            (__LDBL_MAX_EXP__ == 262144) // IEEE 754 binary256
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 32
        #endif

    #elif (__FLT_RADIX__ == 10)

        #if (__FLT_MANT_DIG__ == 7) && (__FLT_MAX_EXP__ == 97)      // IEEE 754 decimal32
            #define WJR_SIZEOF_FLOAT_VALUE 4
        #elif (__FLT_MANT_DIG__ == 16) && (__FLT_MAX_EXP__ == 385)  // IEEE 754 decimal64
            #define WJR_SIZEOF_FLOAT_VALUE 8
        #elif (__FLT_MANT_DIG__ == 34) && (__FLT_MAX_EXP__ == 6145) // IEEE 754 decimal128
            #define WJR_SIZEOF_FLOAT_VALUE 16
        #endif

        #if (__DBL_MANT_DIG__ == 7) && (__DBL_MAX_EXP__ == 97)      // IEEE 754 decimal32
            #define WJR_SIZEOF_DOUBLE_VALUE 4
        #elif (__DBL_MANT_DIG__ == 16) && (__DBL_MAX_EXP__ == 385)  // IEEE 754 decimal64
            #define WJR_SIZEOF_DOUBLE_VALUE 8
        #elif (__DBL_MANT_DIG__ == 34) && (__DBL_MAX_EXP__ == 6145) // IEEE 754 decimal128
            #define WJR_SIZEOF_DOUBLE_VALUE 16
        #endif

        #if (__LDBL_MANT_DIG__ == 7) && (__LDBL_MAX_EXP__ == 97)     // IEEE 754 decimal32
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 4
        #elif (__LDBL_MANT_DIG__ == 16) && (__LDBL_MAX_EXP__ == 385) // IEEE 754 decimal64
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 8
        #elif (__LDBL_MANT_DIG__ == 34) &&                                               \
            (__LDBL_MAX_EXP__ == 6145) // IEEE 754 decimal128
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 16
        #endif

    #endif

#else // defined(__FLT_RADIX__) ...

    #include <cfloat>

    #if (FLT_RADIX == 2)

        #if (FLT_MANT_DIG == 11) && (FLT_MAX_EXP == 16)     // IEEE 754 binary16
            #define WJR_SIZEOF_FLOAT_VALUE 2
        #elif (FLT_MANT_DIG == 24) && (FLT_MAX_EXP == 128)  // IEEE 754 binary32
            #define WJR_SIZEOF_FLOAT_VALUE 4
        #elif (FLT_MANT_DIG == 53) && (FLT_MAX_EXP == 1024) // IEEE 754 binary64
            #define WJR_SIZEOF_FLOAT_VALUE 8
        #elif (FLT_MANT_DIG == 64 || FLT_MANT_DIG == 53 || FLT_MANT_DIG == 24) &&        \
            (FLT_MAX_EXP == 16384) // x87 extended double, with full 64-bit significand or
                                   // reduced to 53 or 24 bits
            #define WJR_SIZEOF_FLOAT_VALUE 10
        #elif (FLT_MANT_DIG == 106) && (FLT_MAX_EXP == 1024)   // IBM extended double
            #define WJR_SIZEOF_FLOAT_VALUE 16
        #elif (FLT_MANT_DIG == 113) && (FLT_MAX_EXP == 16384)  // IEEE 754 binary128
            #define WJR_SIZEOF_FLOAT_VALUE 16
        #elif (FLT_MANT_DIG == 237) && (FLT_MAX_EXP == 262144) // IEEE 754 binary256
            #define WJR_SIZEOF_FLOAT_VALUE 32
        #endif

        #if (DBL_MANT_DIG == 11) && (DBL_MAX_EXP == 16)     // IEEE 754 binary16
            #define WJR_SIZEOF_DOUBLE_VALUE 2
        #elif (DBL_MANT_DIG == 24) && (DBL_MAX_EXP == 128)  // IEEE 754 binary32
            #define WJR_SIZEOF_DOUBLE_VALUE 4
        #elif (DBL_MANT_DIG == 53) && (DBL_MAX_EXP == 1024) // IEEE 754 binary64
            #define WJR_SIZEOF_DOUBLE_VALUE 8
        #elif (DBL_MANT_DIG == 64 || DBL_MANT_DIG == 53 || DBL_MANT_DIG == 24) &&        \
            (DBL_MAX_EXP == 16384) // x87 extended double, with full 64-bit significand or
                                   // reduced to 53 or 24 bits
            #define WJR_SIZEOF_DOUBLE_VALUE 10
        #elif (DBL_MANT_DIG == 106) && (DBL_MAX_EXP == 1024)   // IBM extended double
            #define WJR_SIZEOF_DOUBLE_VALUE 16
        #elif (DBL_MANT_DIG == 113) && (DBL_MAX_EXP == 16384)  // IEEE 754 binary128
            #define WJR_SIZEOF_DOUBLE_VALUE 16
        #elif (DBL_MANT_DIG == 237) && (DBL_MAX_EXP == 262144) // IEEE 754 binary256
            #define WJR_SIZEOF_DOUBLE_VALUE 32
        #endif

        #if (LDBL_MANT_DIG == 11) && (LDBL_MAX_EXP == 16)     // IEEE 754 binary16
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 2
        #elif (LDBL_MANT_DIG == 24) && (LDBL_MAX_EXP == 128)  // IEEE 754 binary32
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 4
        #elif (LDBL_MANT_DIG == 53) && (LDBL_MAX_EXP == 1024) // IEEE 754 binary64
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 8
        #elif (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 53 || LDBL_MANT_DIG == 24) &&     \
            (LDBL_MAX_EXP == 16384) // x87 extended double, with full 64-bit significand
                                    // or reduced to 53 or 24 bits
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 10
        #elif (LDBL_MANT_DIG == 106) && (LDBL_MAX_EXP == 1024)   // IBM extended double
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 16
        #elif (LDBL_MANT_DIG == 113) && (LDBL_MAX_EXP == 16384)  // IEEE 754 binary128
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 16
        #elif (LDBL_MANT_DIG == 237) && (LDBL_MAX_EXP == 262144) // IEEE 754 binary256
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 32
        #endif

    #elif (FLT_RADIX == 10)

        #if (FLT_MANT_DIG == 7) && (FLT_MAX_EXP == 97)      // IEEE 754 decimal32
            #define WJR_SIZEOF_FLOAT_VALUE 4
        #elif (FLT_MANT_DIG == 16) && (FLT_MAX_EXP == 385)  // IEEE 754 decimal64
            #define WJR_SIZEOF_FLOAT_VALUE 8
        #elif (FLT_MANT_DIG == 34) && (FLT_MAX_EXP == 6145) // IEEE 754 decimal128
            #define WJR_SIZEOF_FLOAT_VALUE 16
        #endif

        #if (DBL_MANT_DIG == 7) && (DBL_MAX_EXP == 97)      // IEEE 754 decimal32
            #define WJR_SIZEOF_DOUBLE_VALUE 4
        #elif (DBL_MANT_DIG == 16) && (DBL_MAX_EXP == 385)  // IEEE 754 decimal64
            #define WJR_SIZEOF_DOUBLE_VALUE 8
        #elif (DBL_MANT_DIG == 34) && (DBL_MAX_EXP == 6145) // IEEE 754 decimal128
            #define WJR_SIZEOF_DOUBLE_VALUE 16
        #endif

        #if (LDBL_MANT_DIG == 7) && (LDBL_MAX_EXP == 97)      // IEEE 754 decimal32
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 4
        #elif (LDBL_MANT_DIG == 16) && (LDBL_MAX_EXP == 385)  // IEEE 754 decimal64
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 8
        #elif (LDBL_MANT_DIG == 34) && (LDBL_MAX_EXP == 6145) // IEEE 754 decimal128
            #define WJR_SIZEOF_LONG_DOUBLE_VALUE 16
        #endif

    #endif

#endif // defined(__FLT_RADIX__) ...

// GCC and compatible compilers define internal macros with builtin type traits
#if defined(__SIZEOF_FLOAT__)
    #define WJR_SIZEOF_FLOAT __SIZEOF_FLOAT__
#endif
#if defined(__SIZEOF_DOUBLE__)
    #define WJR_SIZEOF_DOUBLE __SIZEOF_DOUBLE__
#endif
#if defined(__SIZEOF_LONG_DOUBLE__)
    #define WJR_SIZEOF_LONG_DOUBLE __SIZEOF_LONG_DOUBLE__
#endif

#if !defined(WJR_SIZEOF_FLOAT) || !defined(WJR_SIZEOF_DOUBLE) ||                         \
    !defined(WJR_SIZEOF_LONG_DOUBLE)

    #define WJR_ALIGN_SIZE_TO_POWER_OF_2(x)                                              \
        ((x) == 1u                                                                       \
             ? 1u                                                                        \
             : ((x) == 2u                                                                \
                    ? 2u                                                                 \
                    : ((x) <= 4u                                                         \
                           ? 4u                                                          \
                           : ((x) <= 8u                                                  \
                                  ? 8u                                                   \
                                  : ((x) <= 16u ? 16u : ((x) <= 32u ? 32u : (x)))))))

    // Make our best guess. These sizes may not be accurate, but they are good enough to
    // estimate the size of the storage required to hold these types.
    #if !defined(WJR_SIZEOF_FLOAT) && defined(WJR_SIZEOF_FLOAT_VALUE)
        #define WJR_SIZEOF_FLOAT WJR_ALIGN_SIZE_TO_POWER_OF_2(WJR_SIZEOF_FLOAT_VALUE)
    #endif
    #if !defined(WJR_SIZEOF_DOUBLE) && defined(WJR_SIZEOF_DOUBLE_VALUE)
        #define WJR_SIZEOF_DOUBLE WJR_ALIGN_SIZE_TO_POWER_OF_2(WJR_SIZEOF_DOUBLE_VALUE)
    #endif
    #if !defined(WJR_SIZEOF_LONG_DOUBLE) && defined(WJR_SIZEOF_LONG_DOUBLE_VALUE)
        #define WJR_SIZEOF_LONG_DOUBLE                                                   \
            WJR_ALIGN_SIZE_TO_POWER_OF_2(WJR_SIZEOF_LONG_DOUBLE_VALUE)
    #endif

#endif // !defined(WJR_SIZEOF_FLOAT) ||
       // !defined(WJR_SIZEOF_DOUBLE) ||
       // !defined(WJR_SIZEOF_LONG_DOUBLE)

#if !defined(WJR_SIZEOF_FLOAT_VALUE) || !defined(WJR_SIZEOF_FLOAT) ||                    \
    !defined(WJR_SIZEOF_DOUBLE_VALUE) || !defined(WJR_SIZEOF_DOUBLE) ||                  \
    !defined(WJR_SIZEOF_LONG_DOUBLE_VALUE) || !defined(WJR_SIZEOF_LONG_DOUBLE)
    #error "Not support"
#endif

namespace wjr {

enum class branch {
    free,
    full,
};

struct in_place_empty_t {};

inline constexpr in_place_empty_t in_place_empty = {};

/**
 * @brief Tag of default constructor.
 *
 * @details Use dctor to indicate default constructor. \n
 * Used to avoid value initialization.  \n
 * For example : \n
 * @code
 * wjr::vector<int> vec(10, dctor); // default construct with 10 elements.
 * wjr::vector<int> vec2(10); // value construct with 10 elements.
 * wjr::vector<int> vec3(10, 0); // value construct with 10 elements.
 * wjr::vector<int> vec4(10, vctor); // value construct with 10 elements.
 * @endcode
 * elements of vec are not initialized. \n
 * elements of vec2, vec3, vec4 are initialized with 0.
 */
struct dctor_t {};

/**
 * @see dctor_t
 */
inline constexpr dctor_t dctor = {};

/**
 * @brief Tag of value constructor.
 *
 */
struct vctor_t {};

inline constexpr vctor_t vctor = {};

struct in_place_reallocate_t {};

inline constexpr in_place_reallocate_t in_place_reallocate = {};

struct in_place_reserve_t {};

inline constexpr in_place_reserve_t in_place_reserve = {};

struct in_place_move_t {};

inline constexpr in_place_move_t in_place_move = {};

struct in_place_max_t {
    template <typename T>
    WJR_CONST constexpr operator T() const noexcept {
        return std::numeric_limits<T>::max();
    }
};

inline constexpr in_place_max_t in_place_max = {};

struct in_place_min_t {
    template <typename T>
    WJR_CONST constexpr operator T() const noexcept {
        return std::numeric_limits<T>::min();
    }
};

inline constexpr in_place_min_t in_place_min = {};

struct self_init_t {};

inline constexpr self_init_t self_init = {};

inline constexpr std::size_t dynamic_extent = in_place_max;

template <typename... Args>
struct multi_conditional;

template <typename... Args>
using multi_conditional_t = typename multi_conditional<Args...>::type;

template <bool f, typename T, typename... Args>
struct multi_conditional_impl {
    using type = T;
};

template <typename T, typename... Args>
struct multi_conditional_impl<false, T, Args...> {
    using type = multi_conditional_t<Args...>;
};

template <typename F, typename T, typename U>
struct multi_conditional<F, T, U> {
    using type = std::conditional_t<F::value, T, U>;
};

template <typename F, typename T, typename U, typename V, typename... Args>
struct multi_conditional<F, T, U, V, Args...> {
    using type = typename multi_conditional_impl<F::value, T, U, V, Args...>::type;
};

template <typename T, typename... Args>
struct is_any_of : std::disjunction<std::is_same<T, Args>...> {};

template <typename T, typename... Args>
inline constexpr bool is_any_of_v = is_any_of<T, Args...>::value;

template <typename T>
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

/// @private
template <size_t n>
struct __uint_selector {};

/// @private
template <>
struct __uint_selector<8> {
    using type = std::uint8_t;
};

/// @private
template <>
struct __uint_selector<16> {
    using type = std::uint16_t;
};

/// @private
template <>
struct __uint_selector<32> {
    using type = std::uint32_t;
};

/// @private
template <>
struct __uint_selector<64> {
    using type = std::uint64_t;
};

/// @private
template <size_t n>
struct __int_selector {
    using type = std::make_signed_t<typename __uint_selector<n>::type>;
};

#if WJR_HAS_FEATURE(INT128)
/// @private
template <>
struct __uint_selector<128> {
    using type = __uint128_t;
};

/// @private
template <>
struct __int_selector<128> {
    using type = __int128_t;
};
#endif

template <size_t n>
using uint_t = typename __uint_selector<n>::type;

template <size_t n>
using int_t = typename __int_selector<n>::type;

template <size_t n, bool __s>
using usint_t = std::conditional_t<__s, int_t<n>, uint_t<n>>;

using ssize_t = std::make_signed_t<size_t>;

template <typename T>
struct is_nonbool_integral
    : std::conjunction<std::is_integral<T>, std::negation<std::is_same<T, bool>>> {};

template <typename T>
inline constexpr bool is_nonbool_integral_v = is_nonbool_integral<T>::value;

template <typename T>
struct is_unsigned_integral : std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {
};

template <typename T>
inline constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

template <typename T>
struct is_signed_integral : std::conjunction<std::is_integral<T>, std::is_signed<T>> {};

template <typename T>
inline constexpr bool is_signed_integral_v = is_signed_integral<T>::value;

template <typename T>
struct is_nonbool_unsigned_integral
    : std::conjunction<is_unsigned_integral<T>, std::negation<std::is_same<T, bool>>> {};

template <typename T>
inline constexpr bool is_nonbool_unsigned_integral_v =
    is_nonbool_unsigned_integral<T>::value;

template <typename T>
struct is_nonbool_signed_integral
    : std::conjunction<is_signed_integral<T>, std::negation<std::is_same<T, bool>>> {};

template <typename T>
inline constexpr bool is_nonbool_signed_integral_v = is_nonbool_signed_integral<T>::value;

// type identity
template <typename T>
struct type_identity {
    using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

template <typename T>
struct add_restrict {
    using type = T;
};

template <typename T>
struct add_restrict<T *> {
    using type = T *WJR_RESTRICT;
};

template <typename T>
using add_restrict_t = typename add_restrict<T>::type;

/**
 * @brief Return if is constant evaluated.
 *
 * @details Use macro WJR_IS_CONSTANT_EVALUATED(). \n
 * Use std::is_constant_evaluated() if C++ 20 is supported. \n
 * Otherwise, use __builtin_constant_evaluated() if
 * WJR_HAS_BUILTIN(__builtin_is_constant_evaluated). Otherwise, return false.
 *
 */
WJR_INTRINSIC_CONSTEXPR bool is_constant_evaluated() noexcept {
    return WJR_IS_CONSTANT_EVALUATED();
}

template <typename T, typename U, typename = void>
struct __is_swappable_with : std::false_type {};

template <typename T, typename U>
struct __is_swappable_with<
    T, U, std::void_t<decltype(std::swap(std::declval<T &>(), std::declval<U &>()))>>
    : std::true_type {};

template <typename T, typename U>
struct is_swappable_with
    : std::conjunction<__is_swappable_with<T, U>, __is_swappable_with<U, T>> {};

template <typename T, typename U>
inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

template <typename T>
struct is_swappable
    : is_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>> {
};

template <typename T>
inline constexpr bool is_swappable_v = is_swappable<T>::value;

/// @private
template <typename T>
struct __unref_wrapper_helper {
    using type = T;
};

/// @private
template <typename T>
struct __unref_wrapper_helper<std::reference_wrapper<T>> {
    using type = T &;
};

template <typename T>
struct unref_wrapper {
    using type = typename __unref_wrapper_helper<std::decay_t<T>>::type;
};

template <typename T>
using unref_wrapper_t = typename unref_wrapper<T>::type;

template <typename T, typename = void>
struct __is_default_convertible : std::false_type {};

template <typename T>
void __test_default_convertible(const T &);

template <typename T>
struct __is_default_convertible<T,
                                std::void_t<decltype(__test_default_convertible<T>({}))>>
    : std::true_type {};

template <typename T>
using is_default_convertible = __is_default_convertible<T>;

template <typename T>
inline constexpr bool is_default_convertible_v = is_default_convertible<T>::value;

template <typename T>
struct get_place_index {};

template <size_t idx>
struct get_place_index<std::in_place_index_t<idx>> {
    static constexpr size_t value = idx;
};

template <typename T>
inline constexpr size_t get_place_index_v = get_place_index<T>::value;

// C++ 17 concept adapt

template <typename Derived, typename Base>
struct is_derived_from
    : std::conjunction<
          std::is_base_of<Base, Derived>,
          std::is_convertible<const volatile Derived *, const volatile Base *>> {};

template <typename Derived, typename Base>
inline constexpr bool is_derived_from_v = is_derived_from<Derived, Base>::Value;

/// @private
template <typename From, typename To, typename = void>
struct __is_convertible_to_helper : std::false_type {};

/// @private
template <typename From, typename To>
struct __is_convertible_to_helper<
    From, To, std::void_t<decltype(static_cast<To>(std::declval<From>()))>>
    : std::true_type {};

template <typename From, typename To>
struct is_convertible_to : std::conjunction<std::is_convertible<From, To>,
                                            __is_convertible_to_helper<From, To, void>> {
};

template <typename From, typename To>
inline constexpr bool is_convertible_to_v = is_convertible_to<From, To>::value;

template <typename Value, WJR_REQUIRES(is_nonbool_integral_v<Value>)>
WJR_CONST constexpr std::make_signed_t<Value> to_signed(Value value) noexcept {
    return static_cast<std::make_signed_t<Value>>(value);
}

template <typename Value, WJR_REQUIRES(is_nonbool_integral_v<Value>)>
WJR_CONST constexpr std::make_unsigned_t<Value> to_unsigned(Value value) noexcept {
    return static_cast<std::make_unsigned_t<Value>>(value);
}

template <typename T, typename U>
WJR_CONST constexpr bool cmp_equal(T t, U u) noexcept {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
        return t == u;
    } else if constexpr (std::is_signed_v<T>) {
        return t >= 0 && to_unsigned(t) == u;
    } else {
        return u >= 0 && to_unsigned(u) == t;
    }
}

template <typename T, typename U>
WJR_CONST constexpr bool cmp_not_equal(T t, U u) noexcept {
    return !cmp_equal(t, u);
}

template <typename T, typename U>
WJR_CONST constexpr bool cmp_less(T t, U u) noexcept {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
        return t < u;
    } else if constexpr (std::is_signed_v<T>) {
        return t < 0 || to_unsigned(t) < u;
    } else {
        return u >= 0 && t < to_unsigned(u);
    }
}

template <typename T, typename U>
WJR_CONST constexpr bool cmp_greater(T t, U u) noexcept {
    return cmp_less(u, t);
}

template <typename T, typename U>
WJR_CONST constexpr bool cmp_less_equal(T t, U u) noexcept {
    return !cmp_less(u, t);
}

template <typename T, typename U>
WJR_CONST constexpr bool cmp_greater_equal(T t, U u) noexcept {
    return !cmp_less(t, u);
}

template <typename T, typename U>
WJR_CONST constexpr bool in_range(U value) noexcept {
    if constexpr (std::is_same_v<T, U>) {
        return true;
    } else {
        if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
            if constexpr (std::is_signed_v<T>) {
                return value >= std::numeric_limits<T>::min() &&
                       value <= std::numeric_limits<T>::max();
            } else {
                return value <= std::numeric_limits<T>::max();
            }
        } else if constexpr (std::is_signed_v<T>) {
            return value <= to_unsigned(std::numeric_limits<T>::max());
        } else {
            return value >= 0 && to_unsigned(value) <= std::numeric_limits<T>::max();
        }
    }
}

template <typename From, typename To, typename Enable = void>
struct __is_value_preserving_impl : std::false_type {};

template <typename From, typename To>
struct __is_value_preserving_impl<From, To, std::enable_if_t<std::is_integral_v<From>>>
    : std::bool_constant<in_range<To>(std::numeric_limits<From>::min()) &&
                         in_range<To>(std::numeric_limits<From>::max())> {};

template <typename From, typename To>
struct is_value_preserving : __is_value_preserving_impl<From, To> {};

template <typename From>
struct is_value_preserving<From, From> : std::true_type {};

template <typename From, typename To>
inline constexpr bool is_value_preserving_v = is_value_preserving<From, To>::value;

template <typename From, typename To>
struct is_value_preserving_or_int
    : std::disjunction<
          std::is_same<From, int>, is_value_preserving<From, To>,
          std::conjunction<std::is_unsigned<To>, std::is_same<From, unsigned int>>> {};

template <typename From, typename To>
inline constexpr bool is_value_preserving_or_int_v =
    is_value_preserving_or_int<From, To>::value;

template <typename T, typename U,
          WJR_REQUIRES(std::is_integral_v<T> &&std::is_integral_v<U>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T fast_cast(U value) noexcept {
    WJR_ASSUME(in_range<T>(value));
    return static_cast<T>(value);
}

#define __WJR_REGISTER_TYPENAMES(...)                                                    \
    WJR_PP_QUEUE_EXPAND(                                                                 \
        WJR_PP_QUEUE_TRANSFORM((__VA_ARGS__), __WJR_REGISTER_TYPENAMES_CALLER))
#define __WJR_REGISTER_TYPENAMES_CALLER(x) typename x

#define WJR_REGISTER_HAS_TYPE_0(NAME, HAS_EXPR)                                          \
    template <typename Enable, typename... Args>                                         \
    struct __has_##NAME : std::false_type {};                                            \
    template <typename... Args>                                                          \
    struct __has_##NAME<std::void_t<decltype(HAS_EXPR)>, Args...> : std::true_type {};   \
    template <typename... Args>                                                          \
    struct has_##NAME : __has_##NAME<void, Args...> {};                                  \
    template <typename... Args>                                                          \
    constexpr bool has_##NAME##_v = has_##NAME<Args...>::value

#define WJR_REGISTER_HAS_TYPE_MORE(NAME, HAS_EXPR, ...)                                  \
    template <typename Enable, __WJR_REGISTER_TYPENAMES(__VA_ARGS__), typename... Args>  \
    struct __has_##NAME : std::false_type {};                                            \
    template <__WJR_REGISTER_TYPENAMES(__VA_ARGS__), typename... Args>                   \
    struct __has_##NAME<std::void_t<decltype(HAS_EXPR)>, __VA_ARGS__, Args...>           \
        : std::true_type {};                                                             \
    template <__WJR_REGISTER_TYPENAMES(__VA_ARGS__), typename... Args>                   \
    struct has_##NAME : __has_##NAME<void, __VA_ARGS__, Args...> {};                     \
    template <__WJR_REGISTER_TYPENAMES(__VA_ARGS__), typename... Args>                   \
    constexpr bool has_##NAME##_v = has_##NAME<__VA_ARGS__, Args...>::value

#define WJR_REGISTER_HAS_TYPE(NAME, ...)                                                 \
    WJR_REGISTER_HAS_TYPE_N(WJR_PP_ARGS_LEN(__VA_ARGS__), NAME, __VA_ARGS__)
#define WJR_REGISTER_HAS_TYPE_N(N, ...)                                                  \
    WJR_PP_BOOL_IF(WJR_PP_EQ(N, 1), WJR_REGISTER_HAS_TYPE_0, WJR_REGISTER_HAS_TYPE_MORE) \
    (__VA_ARGS__)

// used for SFINAE
constexpr static void allow_true_type(std::true_type) noexcept {}
constexpr static void allow_false_type(std::false_type) noexcept {}

WJR_REGISTER_HAS_TYPE(compare, std::declval<Comp>()(std::declval<T>(), std::declval<U>()),
                      Comp, T, U);
WJR_REGISTER_HAS_TYPE(
    noexcept_compare,
    allow_true_type(std::declval<std::bool_constant<noexcept(
                        std::declval<Comp>()(std::declval<T>(), std::declval<U>()))>>()),
    Comp, T, U);

#define WJR_REGISTER_HAS_COMPARE(NAME, STD)                                              \
    template <typename T, typename U>                                                    \
    struct has_##NAME : has_compare<STD, T, U> {};                                       \
    template <typename T, typename U>                                                    \
    inline constexpr bool has_##NAME##_v = has_##NAME<T, U>::value;                      \
    template <typename T, typename U>                                                    \
    struct has_noexcept_##NAME : has_noexcept_compare<STD, T, U> {};                     \
    template <typename T, typename U>                                                    \
    inline constexpr bool has_noexcept_##NAME##_v = has_noexcept_##NAME<T, U>::value;

WJR_REGISTER_HAS_COMPARE(equal_to, std::equal_to<>);
WJR_REGISTER_HAS_COMPARE(not_equal_to, std::not_equal_to<>);
WJR_REGISTER_HAS_COMPARE(less, std::less<>);
WJR_REGISTER_HAS_COMPARE(less_equal, std::less_equal<>);
WJR_REGISTER_HAS_COMPARE(greater, std::greater<>);
WJR_REGISTER_HAS_COMPARE(greater_equal, std::greater_equal<>);

#undef WJR_REGISTER_HAS_COMPARE

WJR_REGISTER_HAS_TYPE(invocable,
                      std::invoke(std::declval<Func>(), std::declval<Args>()...), Func);

WJR_REGISTER_HAS_TYPE(compare_is_transparent,
                      std::declval<typename Compare::is_transparent>(), Compare);

template <typename T>
struct is_bounded_array : std::false_type {};

template <typename T, std::size_t N>
struct is_bounded_array<T[N]> : std::true_type {};

template <typename T>
inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

template <typename T>
struct is_unbounded_array : std::false_type {};

template <typename T>
struct is_unbounded_array<T[]> : std::true_type {};

template <typename T>
inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

template <typename Unqualified, bool IsConst, bool IsVol>
struct __cv_selector;

template <typename Unqualified>
struct __cv_selector<Unqualified, false, false> {
    using __type = Unqualified;
};

template <typename Unqualified>
struct __cv_selector<Unqualified, false, true> {
    using __type = volatile Unqualified;
};

template <typename Unqualified>
struct __cv_selector<Unqualified, true, false> {
    using __type = const Unqualified;
};

template <typename Unqualified>
struct __cv_selector<Unqualified, true, true> {
    using __type = const volatile Unqualified;
};

template <typename _Qualified, typename Unqualified,
          bool IsConst = std::is_const<_Qualified>::value,
          bool IsVol = std::is_volatile<_Qualified>::value>
class __match_cv_qualifiers {
    using __match = __cv_selector<Unqualified, IsConst, IsVol>;

public:
    using __type = typename __match::__type;
};

template <typename From, typename To>
using __copy_cv = typename __match_cv_qualifiers<From, To>::__type;

template <typename Xp, typename Yp>
using __cond_res =
    decltype(false ? std::declval<Xp (&)()>()() : std::declval<Yp (&)()>()());

template <typename _Ap, typename _Bp, typename = void>
struct __common_ref_impl {};

// [meta.trans.other], COMMON-REF(A, B)
template <typename _Ap, typename _Bp>
using __common_ref = typename __common_ref_impl<_Ap, _Bp>::type;

// COND-RES(COPYCV(X, Y) &, COPYCV(Y, X) &)
template <typename _Xp, typename _Yp>
using __condres_cvref = __cond_res<__copy_cv<_Xp, _Yp> &, __copy_cv<_Yp, _Xp> &>;

// If A and B are both lvalue reference types, ...
template <typename _Xp, typename _Yp>
struct __common_ref_impl<_Xp &, _Yp &, std::void_t<__condres_cvref<_Xp, _Yp>>>
    : std::enable_if<std::is_reference_v<__condres_cvref<_Xp, _Yp>>,
                     __condres_cvref<_Xp, _Yp>> {};

// let C be remove_reference_t<COMMON-REF(X&, Y&)>&&
template <typename _Xp, typename _Yp>
using __common_ref_C = std::remove_reference_t<__common_ref<_Xp &, _Yp &>> &&;

// If A and B are both rvalue reference types, ...
template <typename _Xp, typename _Yp>
struct __common_ref_impl<_Xp &&, _Yp &&,
                         std::enable_if_t<std::conjunction_v<
                             std::is_convertible<_Xp &&, __common_ref_C<_Xp, _Yp>>,
                             std::is_convertible<_Yp &&, __common_ref_C<_Xp, _Yp>>>>> {
    using type = __common_ref_C<_Xp, _Yp>;
};

// let D be COMMON-REF(const X&, Y&)
template <typename _Xp, typename _Yp>
using __common_ref_D = __common_ref<const _Xp &, _Yp &>;

// If A is an rvalue reference and B is an lvalue reference, ...
template <typename _Xp, typename _Yp>
struct __common_ref_impl<
    _Xp &&, _Yp &,
    std::enable_if_t<std::is_convertible_v<_Xp &&, __common_ref_D<_Xp, _Yp>>>> {
    using type = __common_ref_D<_Xp, _Yp>;
};

// If A is an lvalue reference and B is an rvalue reference, ...
template <typename _Xp, typename _Yp>
struct __common_ref_impl<_Xp &, _Yp &&> : __common_ref_impl<_Yp &&, _Xp &> {};

template <typename Tp, typename Up, template <typename> typename TQual,
          template <typename> typename UQual, typename Enable = void>
struct __basic_common_reference_impl {};

template <typename Tp, typename Up, template <typename> typename TQual,
          template <typename> typename UQual>
struct basic_common_reference : __basic_common_reference_impl<Tp, Up, TQual, UQual> {};

/// @cond undocumented
template <typename Tp>
struct __xref {
    template <typename Up>
    using __type = __copy_cv<Tp, Up>;
};

template <typename Tp>
struct __xref<Tp &> {
    template <typename Up>
    using __type = __copy_cv<Tp, Up> &;
};

template <typename Tp>
struct __xref<Tp &&> {
    template <typename Up>
    using __type = __copy_cv<Tp, Up> &&;
};

template <typename Tp1, typename Tp2>
using __basic_common_ref =
    typename basic_common_reference<remove_cvref_t<Tp1>, remove_cvref_t<Tp2>,
                                    __xref<Tp1>::template __type,
                                    __xref<Tp2>::template __type>::type;

template <typename... Tp>
struct common_reference;

template <typename... Tp>
using common_reference_t = typename common_reference<Tp...>::type;

// If sizeof...(T) is zero, there shall be no member type.
template <>
struct common_reference<> {};

// If sizeof...(T) is one ...
template <typename Tp0>
struct common_reference<Tp0> {
    using type = Tp0;
};

template <typename Tp1, typename Tp2, int _Bullet = 1, typename = void>
struct __common_reference_impl : __common_reference_impl<Tp1, Tp2, _Bullet + 1> {};

// If sizeof...(T) is two ...
template <typename Tp1, typename Tp2>
struct common_reference<Tp1, Tp2> : __common_reference_impl<Tp1, Tp2> {};

// If T1 and T2 are reference types and COMMON-REF(T1, T2) is well-formed, ...
template <typename Tp1, typename Tp2>
struct __common_reference_impl<Tp1 &, Tp2 &, 1, std::void_t<__common_ref<Tp1 &, Tp2 &>>> {
    using type = __common_ref<Tp1 &, Tp2 &>;
};

template <typename Tp1, typename Tp2>
struct __common_reference_impl<Tp1 &&, Tp2 &&, 1,
                               std::void_t<__common_ref<Tp1 &&, Tp2 &&>>> {
    using type = __common_ref<Tp1 &&, Tp2 &&>;
};

template <typename Tp1, typename Tp2>
struct __common_reference_impl<Tp1 &, Tp2 &&, 1,
                               std::void_t<__common_ref<Tp1 &, Tp2 &&>>> {
    using type = __common_ref<Tp1 &, Tp2 &&>;
};

template <typename Tp1, typename Tp2>
struct __common_reference_impl<Tp1 &&, Tp2 &, 1,
                               std::void_t<__common_ref<Tp1 &&, Tp2 &>>> {
    using type = __common_ref<Tp1 &&, Tp2 &>;
};

// Otherwise, if basic_common_reference<...>::type is well-formed, ...
template <typename Tp1, typename Tp2>
struct __common_reference_impl<Tp1, Tp2, 2, std::void_t<__basic_common_ref<Tp1, Tp2>>> {
    using type = __basic_common_ref<Tp1, Tp2>;
};

// Otherwise, if COND-RES(T1, T2) is well-formed, ...
template <typename Tp1, typename Tp2>
struct __common_reference_impl<Tp1, Tp2, 3, std::void_t<__cond_res<Tp1, Tp2>>> {
    using type = __cond_res<Tp1, Tp2>;
};

// Otherwise, if common_type_t<T1, T2> is well-formed, ...
template <typename Tp1, typename Tp2>
struct __common_reference_impl<Tp1, Tp2, 4, std::void_t<std::common_type_t<Tp1, Tp2>>> {
    using type = std::common_type_t<Tp1, Tp2>;
};

// Otherwise, there shall be no member type.
template <typename Tp1, typename Tp2>
struct __common_reference_impl<Tp1, Tp2, 5, void> {};

// Otherwise, if sizeof...(T) is greater than two, ...
template <typename Tp1, typename Tp2, typename... Rest>
struct common_reference<Tp1, Tp2, Rest...>
    : common_reference<common_reference_t<Tp1, Tp2>, Rest...> {};

template <typename Enum>
WJR_CONST constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
    return static_cast<std::underlying_type_t<Enum>>(e);
}

enum class relocate_t {
    normal = 0,        /** sepreate move and destroy into two loops. */
    maybe_trivial = 1, /** combine move and destroy in one loop. */
    trivial = 2,       /** use single memcpy. */
};

template <typename T>
struct get_relocate_mode {
    static constexpr relocate_t value =
        std::is_trivially_copyable_v<T> ? relocate_t::trivial : relocate_t::normal;
};

template <typename T>
inline constexpr relocate_t get_relocate_mode_v = get_relocate_mode<T>::value;

template <relocate_t Mode, typename... Args>
struct __get_common_relocate_mode_impl;

template <relocate_t Mode>
struct __get_common_relocate_mode_impl<Mode> {
    static constexpr relocate_t value = Mode;
};

template <relocate_t Mode, typename T, typename... Args>
struct __get_common_relocate_mode_impl<Mode, T, Args...> {
    static constexpr relocate_t value = static_cast<relocate_t>(
        std::min(to_underlying(Mode), to_underlying(get_relocate_mode_v<T>)));
};

template <typename... Args>
struct get_common_relocate_mode
    : __get_common_relocate_mode_impl<relocate_t::trivial, Args...> {};

template <typename... Args>
inline constexpr relocate_t get_common_relocate_mode_v =
    get_common_relocate_mode<Args...>::value;

#define __WJR_INDEXS_RANGE_I(START, END)                                                 \
    WJR_PP_QUEUE_POP_FRONT_N((WJR_PP_IOTA(END)), START)

#define WJR__INDEXS_RANGE(START, END)                                                    \
    WJR_PP_QUEUE_EXPAND(__WJR_INDEXS_RANGE_I(START, END))

#define __WJR_CASES_RANGE_CALLBACK(x) case (x):

#define WJR_CASES_RANGE(START, END)                                                      \
    WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(__WJR_INDEXS_RANGE_I(START, END),            \
                                            __WJR_CASES_RANGE_CALLBACK))

} // namespace wjr

#endif // ! WJR_TYPE_TRAITS_HPP__