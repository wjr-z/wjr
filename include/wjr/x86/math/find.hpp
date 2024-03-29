#ifndef WJR_X86_MATH_FIND_HPP__
#define WJR_X86_MATH_FIND_HPP__

#include <wjr/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_FIND_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_FIND_NOT_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_FIND_NOT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(FIND_N)

template <typename T>
WJR_COLD size_t large_builtin_find_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_FIND_N(index)                                                       \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 + (index)));                                \
        auto y = sse::loadu((__m128i *)(src1 + (index)));                                \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)

    size_t idx = 0;

    if (WJR_UNLIKELY(n & 1)) {
        if (WJR_UNLIKELY(src0[0] == src1[0])) {
            return 0;
        }

        ++idx;
    }

    if (WJR_UNLIKELY(n & 2)) {
        WJR_REGISTER_FIND_N(idx);

        idx += 2;
    }

    if (WJR_UNLIKELY(n & 4)) {
        WJR_REGISTER_FIND_N(idx);
        WJR_REGISTER_FIND_N(idx + 2);

        idx += 4;
    }

    if (WJR_UNLIKELY(idx == n)) {
        return n;
    }

    do {
        WJR_REGISTER_FIND_N(idx);
        WJR_REGISTER_FIND_N(idx + 2);
        WJR_REGISTER_FIND_N(idx + 4);
        WJR_REGISTER_FIND_N(idx + 6);

        idx += 8;
    } while (WJR_LIKELY(idx != n));

    return n;

#undef WJR_REGISTER_FIND_N
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_n(const T *src0, const T *src1, size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_UNLIKELY(n < 4)) {
        if (WJR_UNLIKELY(src0[0] == src1[0])) {
            return 0;
        }

        if (n == 1 || WJR_UNLIKELY(src0[1] == src1[1])) {
            return 1;
        }

        if (n == 2 || WJR_UNLIKELY(src0[2] == src1[2])) {
            return 2;
        }

        return 3;
    }

    return large_builtin_find_n(src0, src1, n);
}

template <typename T>
WJR_COLD size_t large_builtin_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_N(index)                                                       \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src + (index)));                                 \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) + (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)

    size_t idx = 0;

    if (WJR_UNLIKELY(n & 1)) {
        if (WJR_UNLIKELY(src[0] == val)) {
            return 0;
        }

        ++idx;
    }

    auto y = sse::set1(val, T());

    if (WJR_UNLIKELY(n & 2)) {
        WJR_REGISTER_FIND_N(idx);

        idx += 2;
    }

    if (WJR_UNLIKELY(n & 4)) {
        WJR_REGISTER_FIND_N(idx);
        WJR_REGISTER_FIND_N(idx + 2);

        idx += 4;
    }

    if (WJR_UNLIKELY(idx == n)) {
        return n;
    }

    do {
        WJR_REGISTER_FIND_N(idx);
        WJR_REGISTER_FIND_N(idx + 2);
        WJR_REGISTER_FIND_N(idx + 4);
        WJR_REGISTER_FIND_N(idx + 6);

        idx += 8;
    } while (WJR_LIKELY(idx != n));

    return n;

#undef WJR_REGISTER_FIND_N
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_UNLIKELY(n < 4)) {
        if (WJR_UNLIKELY(src[0] == val)) {
            return 0;
        }

        if (n == 1 || WJR_UNLIKELY(src[1] == val)) {
            return 1;
        }

        if (n == 2 || WJR_UNLIKELY(src[2] == val)) {
            return 2;
        }

        return 3;
    }

    return large_builtin_find_n(src, val, n);
}

#endif // WJR_HAS_BUILTIN(FIND_N)

#if WJR_HAS_BUILTIN(FIND_NOT_N)

template <typename T>
WJR_COLD size_t large_builtin_find_not_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_FIND_NOT_N(index, expect)                                           \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 + (index)));                                \
        auto y = sse::loadu((__m128i *)(src1 + (index)));                                \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_EXPECT(mask != sse::mask(), expect)) {                                   \
            return (index) + (mask == 0x00FF);                                           \
        }                                                                                \
    } while (0)

    size_t idx;

    if (WJR_LIKELY(n & 1)) {
        idx = 1;
    } else {
        if (WJR_LIKELY(src0[1] != src1[1])) {
            return 1;
        }

        idx = 2;
    }

    size_t m = n - 1;

    if (WJR_UNLIKELY(m & 2)) {
        WJR_REGISTER_FIND_NOT_N(idx, true);

        idx += 2;
    }

    if (WJR_UNLIKELY(m & 4)) {
        WJR_REGISTER_FIND_NOT_N(idx, true);
        WJR_REGISTER_FIND_NOT_N(idx + 2, true);

        idx += 4;
    }

    if (WJR_UNLIKELY(idx == n)) {
        return n;
    }

    do {
        WJR_REGISTER_FIND_NOT_N(idx, false);
        WJR_REGISTER_FIND_NOT_N(idx + 2, false);
        WJR_REGISTER_FIND_NOT_N(idx + 4, false);
        WJR_REGISTER_FIND_NOT_N(idx + 6, false);

        idx += 8;
    } while (WJR_LIKELY(idx != n));

    return n;

#undef WJR_REGISTER_FIND_NOT_N
}

extern template WJR_COLD size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src0,
                                                                   const uint64_t *src1,
                                                                   size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_not_n(const T *src0, const T *src1, size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_LIKELY(src0[0] != src1[0])) {
        return 0;
    }

    if (WJR_UNLIKELY(n < 4)) {
        if (n == 1 || WJR_LIKELY(src0[1] != src1[1])) {
            return 1;
        }

        if (n == 2 || WJR_LIKELY(src0[2] != src1[2])) {
            return 2;
        }

        return 3;
    }

    size_t ret = large_builtin_find_not_n(src0, src1, n);
    WJR_ASSUME(ret != 0);
    return ret;
}

template <typename T>
WJR_COLD size_t large_builtin_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_NOT_N(index, expect)                                           \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src + (index)));                                 \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_EXPECT(mask != sse::mask(), expect)) {                                   \
            return (index) + (mask == 0x00FF);                                           \
        }                                                                                \
    } while (0)

    size_t idx;

    if (WJR_LIKELY(n & 1)) {
        idx = 1;
    } else {
        if (WJR_LIKELY(src[1] != val)) {
            return 1;
        }

        idx = 2;
    }

    size_t m = n - 1;

    auto y = sse::set1(val, T());

    if (WJR_UNLIKELY(m & 2)) {
        WJR_REGISTER_FIND_NOT_N(idx, true);

        idx += 2;
    }

    if (WJR_UNLIKELY(m & 4)) {
        WJR_REGISTER_FIND_NOT_N(idx, true);
        WJR_REGISTER_FIND_NOT_N(idx + 2, true);

        idx += 4;
    }

    if (WJR_UNLIKELY(idx == n)) {
        return n;
    }

    do {
        WJR_REGISTER_FIND_NOT_N(idx, false);
        WJR_REGISTER_FIND_NOT_N(idx + 2, false);
        WJR_REGISTER_FIND_NOT_N(idx + 4, false);
        WJR_REGISTER_FIND_NOT_N(idx + 6, false);

        idx += 8;
    } while (WJR_LIKELY(idx != n));

    return n;

#undef WJR_REGISTER_FIND_NOT_N
}

extern template WJR_COLD size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src,
                                                                   uint64_t val,
                                                                   size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_not_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_LIKELY(src[0] != val)) {
        return 0;
    }

    if (WJR_UNLIKELY(n < 4)) {
        if (n == 1 || WJR_LIKELY(src[1] != val)) {
            return 1;
        }

        if (n == 2 || WJR_LIKELY(src[2] != val)) {
            return 2;
        }

        return 3;
    }

    size_t ret = large_builtin_find_not_n(src, val, n);
    WJR_ASSUME(ret != 0);
    return ret;
}

#endif // WJR_HAS_BUILTIN(FIND_NOT_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_N)

template <typename T>
WJR_COLD size_t large_builtin_reverse_find_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N(index)                                               \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 + (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 + (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) - (mask == 0x00FF);                                           \
        }                                                                                \
    } while (0)

    size_t idx;

    if (WJR_LIKELY(!(n & 1))) {
        idx = n;
    } else {
        if (WJR_UNLIKELY(src0[n - 1] == src1[n - 1])) {
            return n;
        }

        idx = n - 1;
    }

    if (WJR_UNLIKELY(n & 2)) {
        WJR_REGISTER_REVERSE_FIND_N(idx);

        idx -= 2;
    }

    if (WJR_UNLIKELY(n & 4)) {
        WJR_REGISTER_REVERSE_FIND_N(idx);
        WJR_REGISTER_REVERSE_FIND_N(idx - 2);

        idx -= 4;
    }

    if (WJR_UNLIKELY(idx == 0)) {
        return 0;
    }

    do {
        WJR_REGISTER_REVERSE_FIND_N(idx);
        WJR_REGISTER_REVERSE_FIND_N(idx - 2);
        WJR_REGISTER_REVERSE_FIND_N(idx - 4);
        WJR_REGISTER_REVERSE_FIND_N(idx - 6);

        idx -= 8;
    } while (WJR_LIKELY(idx != 0));

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_n(const T *src0, const T *src1,
                                                   size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_UNLIKELY(n < 4)) {
        if (WJR_UNLIKELY(src0[n - 1] == src1[n - 1])) {
            return n;
        }

        if (n == 1 || WJR_UNLIKELY(src0[n - 2] == src1[n - 2])) {
            return n - 1;
        }

        if (n == 2 || WJR_UNLIKELY(src0[n - 3] == src1[n - 3])) {
            return n - 2;
        }

        return 0;
    }

    return large_builtin_reverse_find_n(src0, src1, n);
}

template <typename T>
WJR_COLD size_t large_builtin_reverse_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N(index)                                               \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src - 2 + (index)));                             \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_UNLIKELY(mask != 0)) {                                                   \
            return (index) - (mask == 0x00FF);                                           \
        }                                                                                \
    } while (0)

    size_t idx;

    if (WJR_LIKELY(!(n & 1))) {
        idx = n;
    } else {
        if (WJR_UNLIKELY(src[n - 1] == val)) {
            return n;
        }

        idx = n - 1;
    }

    auto y = sse::set1(val, T());

    if (WJR_UNLIKELY(n & 2)) {
        WJR_REGISTER_REVERSE_FIND_N(idx);

        idx -= 2;
    }

    if (WJR_UNLIKELY(n & 4)) {
        WJR_REGISTER_REVERSE_FIND_N(idx);
        WJR_REGISTER_REVERSE_FIND_N(idx - 2);

        idx -= 4;
    }

    if (WJR_UNLIKELY(idx == 0)) {
        return 0;
    }

    do {
        WJR_REGISTER_REVERSE_FIND_N(idx);
        WJR_REGISTER_REVERSE_FIND_N(idx - 2);
        WJR_REGISTER_REVERSE_FIND_N(idx - 4);
        WJR_REGISTER_REVERSE_FIND_N(idx - 6);

        idx -= 8;
    } while (WJR_LIKELY(idx != 0));

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_UNLIKELY(n < 4)) {
        if (WJR_UNLIKELY(src[n - 1] == val)) {
            return n;
        }

        if (n == 1 || WJR_UNLIKELY(src[n - 2] == val)) {
            return n - 1;
        }

        if (n == 2 || WJR_UNLIKELY(src[n - 3] == val)) {
            return n - 2;
        }

        return 0;
    }

    return large_builtin_reverse_find_n(src, val, n);
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

template <typename T>
WJR_COLD size_t large_builtin_reverse_find_not_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_NOT_N(index, expect)                                   \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src0 - 2 + (index)));                            \
        auto y = sse::loadu((__m128i *)(src1 - 2 + (index)));                            \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_EXPECT(mask != sse::mask(), expect)) {                                   \
            return (index) - (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)

    size_t idx;

    if (WJR_LIKELY(n & 1)) {
        idx = n - 1;
    } else {
        if (WJR_LIKELY(src0[n - 2] != src1[n - 2])) {
            return n - 1;
        }

        idx = n - 2;
    }

    size_t m = n - 1;

    if (WJR_UNLIKELY(m & 2)) {
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx, true);

        idx -= 2;
    }

    if (WJR_UNLIKELY(m & 4)) {
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx, true);
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx - 2, true);

        idx -= 4;
    }

    if (WJR_UNLIKELY(idx == 0)) {
        return 0;
    }

    do {
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx, false);
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx - 2, false);
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx - 4, false);
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx - 6, false);

        idx -= 8;
    } while (WJR_LIKELY(idx != 0));

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_NOT_N
}

extern template WJR_COLD size_t large_builtin_reverse_find_not_n<uint64_t>(
    const uint64_t *src0, const uint64_t *src1, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_not_n(const T *src0, const T *src1,
                                                       size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_LIKELY(src0[n - 1] != src1[n - 1])) {
        return n;
    }

    if (WJR_UNLIKELY(n < 4)) {
        if (n == 1 || WJR_LIKELY(src0[n - 2] != src1[n - 2])) {
            return n - 1;
        }

        if (n == 2 || WJR_LIKELY(src0[n - 3] != src1[n - 3])) {
            return n - 2;
        }

        return 0;
    }

    size_t ret = large_builtin_reverse_find_not_n(src0, src1, n);
    WJR_ASSUME(ret != n);
    return ret;
}

template <typename T>
WJR_COLD size_t large_builtin_reverse_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_NOT_N(index, expect)                                   \
    do {                                                                                 \
        auto x = sse::loadu((__m128i *)(src - 2 + (index)));                             \
        auto r = sse::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = sse::movemask_epi8(r);                                               \
        if (WJR_EXPECT(mask != sse::mask(), expect)) {                                   \
            return (index) - (mask == 0xFF00);                                           \
        }                                                                                \
    } while (0)

    size_t idx;

    if (WJR_LIKELY(n & 1)) {
        idx = n - 1;
    } else {
        if (WJR_LIKELY(src[n - 2] != val)) {
            return n - 1;
        }

        idx = n - 2;
    }

    size_t m = n - 1;

    auto y = sse::set1(val, T());

    if (WJR_UNLIKELY(m & 2)) {
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx, true);

        idx -= 2;
    }

    if (WJR_UNLIKELY(m & 4)) {
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx, true);
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx - 2, true);

        idx -= 4;
    }

    if (WJR_UNLIKELY(idx == 0)) {
        return 0;
    }

    do {
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx, false);
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx - 2, false);
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx - 4, false);
        WJR_REGISTER_REVERSE_FIND_NOT_N(idx - 6, false);

        idx -= 8;
    } while (WJR_LIKELY(idx != 0));

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_NOT_N
}

extern template WJR_COLD size_t
large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src, uint64_t val, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_not_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_LIKELY(src[n - 1] != val)) {
        return n;
    }

    if (WJR_UNLIKELY(n < 4)) {
        if (n == 1 || WJR_LIKELY(src[n - 2] != val)) {
            return n - 1;
        }

        if (n == 2 || WJR_LIKELY(src[n - 3] != val)) {
            return n - 2;
        }

        return 0;
    }

    size_t ret = large_builtin_reverse_find_not_n(src, val, n);
    WJR_ASSUME(ret != n);
    return ret;
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

} // namespace wjr

#endif // WJR_X86_MATH_FIND_HPP__