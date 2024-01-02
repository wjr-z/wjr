#ifndef WJR_GEN_FAST_1_2_8_HPP__
#define WJR_GEN_FAST_1_2_8_HPP__

#define WJR_GEN_SMALL_FAST_1_2_8(n, LOOP1, INIT1, RET, LARGE, ...)                       \
    do {                                                                                 \
        INIT1(0);                                                                        \
                                                                                         \
        if (WJR_UNLIKELY(n < 4)) {                                                       \
                                                                                         \
            if (WJR_UNLIKELY(n == 0)) {                                                  \
                return RET();                                                            \
            }                                                                            \
                                                                                         \
            LOOP1(0);                                                                    \
                                                                                         \
            if (WJR_UNLIKELY(n == 1)) {                                                  \
                return RET();                                                            \
            }                                                                            \
                                                                                         \
            LOOP1(1);                                                                    \
                                                                                         \
            if (WJR_UNLIKELY(n == 2)) {                                                  \
                return RET();                                                            \
            }                                                                            \
                                                                                         \
            LOOP1(2);                                                                    \
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
        WJR_ASSUME(gen_offset + gen_n == n);                                             \
        LARGE(gen_offset, gen_n, n, __VA_ARGS__);                                        \
    } while (0)

#define WJR_GEN_LARGE_FAST_1_2_8(gen_offset, gen_n, n, LOOP2, LOOP8, INIT2, INIT8, RET)  \
    do {                                                                                 \
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

#define WJR_GEN_FAST_1_2_8(LOOP1, LOOP2, LOOP8, INIT1, INIT2, INIT8, RET)                \
    WJR_GEN_SMALL_FAST_1_2_8(n, LOOP1, INIT1, RET, WJR_GEN_LARGE_FAST_1_2_8, LOOP2,      \
                             LOOP8, INIT2, INIT8, RET)

#endif // WJR_GEN_FAST_1_2_8_HPP__