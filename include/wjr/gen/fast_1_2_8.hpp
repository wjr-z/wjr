#ifndef WJR_GEN_FAST_1_2_8_HPP__
#define WJR_GEN_FAST_1_2_8_HPP__

#define WJR_GEN_FAST_1_2_8(LOOP1, LOOP2, LOOP8, INIT1, INIT2, INIT8, RET)                \
    do {                                                                                 \
        size_t gen_n = n;                                                                \
        size_t gen_offset = 0;                                                           \
        INIT1(gen_offset);                                                               \
                                                                                         \
        if (WJR_UNLIKELY(gen_n < 4)) {                                                   \
                                                                                         \
            if (WJR_UNLIKELY(gen_n == 0)) {                                              \
                return RET();                                                            \
            }                                                                            \
                                                                                         \
            LOOP1(gen_offset + 0);                                                       \
                                                                                         \
            if (WJR_UNLIKELY(gen_n == 1)) {                                              \
                return RET();                                                            \
            }                                                                            \
                                                                                         \
            LOOP1(gen_offset + 1);                                                       \
                                                                                         \
            if (WJR_UNLIKELY(gen_n == 2)) {                                              \
                return RET();                                                            \
            }                                                                            \
                                                                                         \
            LOOP1(gen_offset + 2);                                                       \
                                                                                         \
            return RET();                                                                \
        }                                                                                \
                                                                                         \
        LOOP1(gen_offset + 0);                                                           \
                                                                                         \
        gen_offset += 1;                                                                 \
        gen_n -= 1;                                                                      \
                                                                                         \
        WJR_ASSUME(gen_n >= 3);                                                          \
                                                                                         \
        {                                                                                \
            size_t gen_k = gen_n % 2;                                                    \
            do {                                                                         \
                if (gen_k == 0) {                                                        \
                    break;                                                               \
                }                                                                        \
                                                                                         \
                LOOP1(gen_offset + 0);                                                   \
                                                                                         \
            } while (0);                                                                 \
                                                                                         \
            gen_offset += gen_k;                                                         \
            gen_n -= gen_k;                                                              \
        }                                                                                \
                                                                                         \
        WJR_ASSUME(gen_n % 2 == 0);                                                      \
                                                                                         \
        INIT2(gen_offset);                                                               \
                                                                                         \
        {                                                                                \
            size_t gen_k = gen_n % 8;                                                    \
            do {                                                                         \
                if (gen_k == 0) {                                                        \
                    break;                                                               \
                }                                                                        \
                                                                                         \
                LOOP2(gen_offset + 0);                                                   \
                                                                                         \
                if (gen_k == 2) {                                                        \
                    break;                                                               \
                }                                                                        \
                                                                                         \
                LOOP2(gen_offset + 2);                                                   \
                                                                                         \
                if (gen_k == 4) {                                                        \
                    break;                                                               \
                }                                                                        \
                                                                                         \
                LOOP2(gen_offset + 4);                                                   \
                                                                                         \
            } while (0);                                                                 \
                                                                                         \
            gen_offset += gen_k;                                                         \
            gen_n -= gen_k;                                                              \
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
            LOOP8(gen_offset + 0);                                                       \
                                                                                         \
            gen_offset += 8;                                                             \
        } while (WJR_LIKELY(gen_offset != n));                                           \
                                                                                         \
        return RET();                                                                    \
    } while (0);

#endif // WJR_GEN_FAST_1_2_8_HPP__