#ifndef WJR_PREPROCESSOR_CONFIG_SIZEOF_HPP__
#define WJR_PREPROCESSOR_CONFIG_SIZEOF_HPP__

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

#endif // WJR_PREPROCESSOR_CONFIG_SIZEOF_HPP__