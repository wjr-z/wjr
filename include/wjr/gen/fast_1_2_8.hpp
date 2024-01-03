#ifndef WJR_GEN_FAST_1_2_8_HPP__
#define WJR_GEN_FAST_1_2_8_HPP__

#define WJR_GEN_SMALL_FAST_1_2_8(n, LOOP1, LOOP2, INIT1, INIT2, RET, LARGE)              \
    do {                                                                                 \
        INIT1(0);                                                                        \
                                                                                         \
        if (WJR_UNLIKELY(n < 4)) {                                                       \
                                                                                         \
            do {                                                                         \
                if (WJR_UNLIKELY(n == 0)) {                                              \
                    break;                                                               \
                }                                                                        \
                                                                                         \
                LOOP1(0);                                                                \
                                                                                         \
                if (WJR_UNLIKELY(n == 1)) {                                              \
                    break;                                                               \
                }                                                                        \
                                                                                         \
                LOOP1(1);                                                                \
                                                                                         \
                if (WJR_UNLIKELY(n == 2)) {                                              \
                    break;                                                               \
                }                                                                        \
                                                                                         \
                LOOP1(2);                                                                \
            } while (0);                                                                 \
                                                                                         \
            return RET();                                                                \
        }                                                                                \
                                                                                         \
        size_t gen_n = n;                                                                \
        size_t gen_offset = 0;                                                           \
                                                                                         \
        LOOP1(gen_offset);                                                               \
                                                                                         \
        gen_offset += 1;                                                                 \
        gen_n -= 1;                                                                      \
                                                                                         \
        WJR_ASSUME(gen_n >= 3);                                                          \
                                                                                         \
        if (gen_n & 1) {                                                                 \
            LOOP1(gen_offset);                                                           \
                                                                                         \
            ++gen_offset;                                                                \
            --gen_n;                                                                     \
        }                                                                                \
                                                                                         \
        WJR_ASSUME(gen_n % 2 == 0);                                                      \
                                                                                         \
        if (WJR_UNLIKELY(gen_n > 32)) {                                                  \
            WJR_ASSUME(gen_offset + gen_n == n);                                         \
            LARGE(gen_offset, gen_n, n);                                                 \
            WJR_UNREACHABLE;                                                             \
        }                                                                                \
                                                                                         \
        INIT2(gen_offset);                                                               \
                                                                                         \
        do {                                                                             \
            LOOP2(gen_offset);                                                           \
                                                                                         \
            gen_offset += 2;                                                             \
        } while (WJR_LIKELY(gen_offset != n));                                           \
        return RET();                                                                    \
    } while (0)

#define WJR_GEN_LARGE_FAST_1_2_8(n, LOOP2, LOOP8, INIT2, INIT8, RET)                     \
    do {                                                                                 \
        size_t gen_offset = 0;                                                           \
        size_t gen_n = n;                                                                \
                                                                                         \
        WJR_ASSUME(gen_n % 2 == 0);                                                      \
                                                                                         \
        INIT2(gen_offset);                                                               \
                                                                                         \
        if (gen_n & 2) {                                                                 \
            LOOP2(gen_offset);                                                           \
                                                                                         \
            gen_offset += 2;                                                             \
            gen_n -= 2;                                                                  \
        }                                                                                \
                                                                                         \
        if (gen_n & 4) {                                                                 \
            LOOP2(gen_offset);                                                           \
            LOOP2(gen_offset + 2);                                                       \
                                                                                         \
            gen_offset += 4;                                                             \
            gen_n -= 4;                                                                  \
        }                                                                                \
                                                                                         \
        WJR_ASSUME(gen_n % 8 == 0);                                                      \
                                                                                         \
        if (WJR_UNLIKELY(!gen_n)) {                                                      \
            return RET();                                                                \
        }                                                                                \
                                                                                         \
        INIT8(gen_offset);                                                               \
                                                                                         \
        do {                                                                             \
            LOOP8(gen_offset);                                                           \
                                                                                         \
            gen_offset += 8;                                                             \
        } while (WJR_LIKELY(gen_offset != n));                                           \
                                                                                         \
        return RET();                                                                    \
    } while (0)

#endif // WJR_GEN_FAST_1_2_8_HPP__