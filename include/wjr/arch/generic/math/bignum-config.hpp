#ifndef WJR_GENERIC_MATH_BIGNUM_CONFIG_HPP__
#define WJR_GENERIC_MATH_BIGNUM_CONFIG_HPP__

#ifndef WJR_TOOM22_MUL_THRESHOLD
    #define WJR_TOOM22_MUL_THRESHOLD 22
#endif

#ifndef WJR_TOOM33_MUL_THRESHOLD
    #define WJR_TOOM33_MUL_THRESHOLD 84
#endif

#ifndef WJR_TOOM44_MUL_THRESHOLD
    #define WJR_TOOM44_MUL_THRESHOLD 208
#endif

#ifndef WJR_TOOM55_MUL_THRESHOLD
    #define WJR_TOOM55_MUL_THRESHOLD 800
#endif

#ifndef WJR_TOOM32_TO_TOOM43_MUL_THRESHOLD
    #define WJR_TOOM32_TO_TOOM43_MUL_THRESHOLD 73
#endif

#ifndef WJR_TOOM32_TO_TOOM53_MUL_THRESHOLD
    #define WJR_TOOM32_TO_TOOM53_MUL_THRESHOLD 153
#endif

#ifndef WJR_TOOM42_TO_TOOM53_MUL_THRESHOLD
    #define WJR_TOOM42_TO_TOOM53_MUL_THRESHOLD 137
#endif

#ifndef WJR_TOOM42_TO_TOOM63_MUL_THRESHOLD
    #define WJR_TOOM42_TO_TOOM63_MUL_THRESHOLD 153
#endif

#ifndef WJR_TOOM2_SQR_THRESHOLD
    #define WJR_TOOM2_SQR_THRESHOLD 34
#endif

#ifndef WJR_TOOM3_SQR_THRESHOLD
    #define WJR_TOOM3_SQR_THRESHOLD 124
#endif

#ifndef WJR_TOOM4_SQR_THRESHOLD
    #define WJR_TOOM4_SQR_THRESHOLD 288
#endif

#ifndef WJR_TOOM5_SQR_THRESHOLD
    #define WJR_TOOM5_SQR_THRESHOLD 980
#endif

#ifndef WJR_DC_DIV_QR_THRESHOLD
    #define WJR_DC_DIV_QR_THRESHOLD (WJR_TOOM22_MUL_THRESHOLD * 2)
#endif // WJR_DC_DIV_QR_THRESHOLD

#ifndef WJR_DC_BIGNUM_TO_CHARS_THRESHOLD
    #define WJR_DC_BIGNUM_TO_CHARS_THRESHOLD 18
#endif

/// @deprecated
#ifndef WJR_DC_BIGNUM_TO_CHARS_PRECOMPUTE_THRESHOLD
    #define WJR_DC_BIGNUM_TO_CHARS_PRECOMPUTE_THRESHOLD 20
#endif

#ifndef WJR_DC_BIGNUM_FROM_CHARS_THRESHOLD
    #define WJR_DC_BIGNUM_FROM_CHARS_THRESHOLD 1670
#endif

#ifndef WJR_DC_BIGNUM_FROM_CHARS_PRECOMPUTE_THRESHOLD
    #define WJR_DC_BIGNUM_FROM_CHARS_PRECOMPUTE_THRESHOLD 3105
#endif

#define WJR_MULMOD_BNM1_THRESHOLD 15
#define WJR_SQRMOD_BNM1_THRESHOLD 17

#define WJR_MUL_FFT_MODF_THRESHOLD 404 /* k = 5 */
#define WJR_MUL_FFT_TABLE3                                                                         \
    {                                                                                              \
        {404, 5}, {21, 6}, {11, 5}, {23, 6}, {12, 5}, {25, 6}, {28, 7}, {15, 6}, {31, 7}, {21, 8}, \
            {11, 7}, {25, 8}, {13, 7}, {28, 8}, {15, 7}, {32, 8}, {17, 7}, {35, 8}, {19, 7},       \
            {39, 8}, {21, 9}, {11, 8}, {23, 7}, {47, 8}, {27, 9}, {15, 8}, {35, 9}, {19, 8},       \
            {41, 9}, {23, 8}, {49, 9}, {27, 10}, {15, 9}, {39, 8}, {79, 9}, {43, 10}, {23, 9},     \
            {55, 11}, {15, 10}, {31, 9}, {71, 10}, {39, 9}, {83, 10}, {47, 9}, {99, 10}, {55, 11}, \
            {31, 10}, {79, 11}, {47, 10}, {103, 12}, {31, 11}, {63, 10}, {135, 11}, {79, 10},      \
            {167, 11}, {95, 10}, {191, 9}, {383, 10}, {199, 11}, {111, 12}, {63, 11}, {127, 10},   \
            {255, 9}, {511, 10}, {271, 9}, {543, 11}, {143, 10}, {287, 9}, {575, 11}, {159, 12},   \
            {95, 11}, {191, 10}, {383, 13}, {63, 12}, {127, 11}, {255, 10}, {511, 11}, {271, 10},  \
            {543, 11}, {287, 10}, {575, 11}, {303, 12}, {159, 11}, {319, 10}, {639, 11},           \
            {335, 10}, {671, 11}, {351, 10}, {703, 12}, {191, 11}, {383, 10}, {767, 11},           \
            {415, 12}, {223, 11}, {447, 13}, {127, 12}, {255, 11}, {543, 12}, {287, 11},           \
            {607, 12}, {319, 11}, {671, 12}, {351, 11}, {703, 13}, {191, 12}, {383, 11},           \
            {767, 12}, {415, 11}, {831, 12}, {479, 14}, {127, 13}, {255, 12}, {543, 11},           \
            {1087, 12}, {607, 13}, {319, 12}, {671, 11}, {1343, 12}, {703, 13}, {383, 12},         \
            {831, 13}, {447, 12}, {959, 13}, {511, 12}, {1087, 13}, {575, 12}, {1151, 13},         \
            {639, 12}, {1343, 13}, {703, 14}, {383, 13}, {767, 12}, {1535, 13}, {831, 12},         \
            {1727, 13}, {959, 14}, {511, 13}, {1087, 12}, {2175, 13}, {1151, 14}, {639, 13},       \
            {1343, 12}, {2687, 13}, {1407, 14}, {767, 13}, {1599, 12}, {3199, 13}, {1663, 14},     \
            {895, 13}, {1791, 15}, {511, 14}, {1023, 13}, {2175, 14}, {1151, 13}, {2431, 12},      \
            {4863, 14}, {1279, 13}, {2687, 14}, {1407, 13}, {2815, 15}, {767, 14}, {1535, 13},     \
            {3199, 14}, {1663, 13}, {3455, 12}, {6911, 14}, {1791, 16}, {511, 15}, {1023, 14},     \
            {2175, 13}, {4351, 14}, {2431, 13}, {4863, 15}, {1279, 14}, {2943, 13}, {5887, 15},    \
            {1535, 14}, {3455, 13}, {6911, 15}, {1791, 14}, {3839, 13}, {7679, 16}, {1023, 15},    \
            {2047, 14}, {4223, 15}, {2303, 14}, {4863, 15}, {2559, 14}, {5247, 15}, {2815, 14},    \
            {5887, 16}, {1535, 15}, {3327, 14}, {6911, 15}, {3839, 14}, {7679, 17}, {1023, 16},    \
            {2047, 15}, {4351, 14}, {8703, 15}, {4863, 16}, {2559, 15}, {5887, 14}, {11775, 16},   \
            {3071, 15}, {6911, 16}, {3583, 15}, {7679, 14}, {15359, 15}, {7935, 17}, {2047, 16},   \
            {4095, 15}, {8703, 16}, {4607, 15}, {9983, 14}, {19967, 16}, {5631, 15}, {11775, 17},  \
            {3071, 16}, {65536, 17}, {131072, 18}, {262144, 19}, {524288, 20}, {1048576, 21},      \
            {2097152, 22}, {4194304, 23}, {                                                        \
            8388608, 24                                                                            \
        }                                                                                          \
    }
#define WJR_MUL_FFT_TABLE3_SIZE 227
#define WJR_MUL_FFT_THRESHOLD 4800

#define WJR_SQR_FFT_MODF_THRESHOLD 400 /* k = 5 */
#define WJR_SQR_FFT_TABLE3                                                                         \
    {                                                                                              \
        {400, 5}, {25, 6}, {13, 5}, {27, 6}, {29, 7}, {15, 6}, {31, 7}, {28, 8}, {15, 7}, {32, 8}, \
            {17, 7}, {35, 8}, {19, 7}, {39, 8}, {27, 9}, {15, 8}, {33, 9}, {19, 8}, {39, 9},       \
            {23, 8}, {47, 9}, {27, 10}, {15, 9}, {39, 10}, {23, 9}, {51, 11}, {15, 10}, {31, 9},   \
            {67, 10}, {39, 9}, {79, 10}, {55, 11}, {31, 10}, {79, 11}, {47, 10}, {95, 12},         \
            {31, 11}, {63, 10}, {135, 11}, {79, 10}, {159, 9}, {319, 11}, {95, 12}, {63, 11},      \
            {127, 10}, {255, 9}, {511, 10}, {271, 11}, {143, 10}, {287, 9}, {575, 10}, {303, 11},  \
            {159, 10}, {319, 12}, {95, 10}, {383, 13}, {63, 12}, {127, 11}, {255, 10}, {511, 11},  \
            {271, 10}, {543, 11}, {287, 10}, {575, 11}, {303, 10}, {607, 12}, {159, 11},           \
            {319, 10}, {639, 11}, {335, 10}, {671, 11}, {351, 10}, {703, 11}, {367, 10},           \
            {735, 11}, {383, 10}, {767, 11}, {415, 10}, {831, 12}, {223, 11}, {479, 13},           \
            {127, 12}, {255, 11}, {543, 12}, {287, 11}, {607, 12}, {319, 11}, {671, 12},           \
            {351, 11}, {735, 12}, {383, 11}, {799, 12}, {415, 11}, {831, 12}, {479, 14},           \
            {127, 13}, {255, 12}, {511, 11}, {1023, 12}, {607, 13}, {319, 12}, {735, 13},          \
            {383, 12}, {831, 13}, {447, 12}, {959, 13}, {511, 12}, {1023, 13}, {575, 12},          \
            {1151, 13}, {639, 12}, {1279, 13}, {703, 12}, {1407, 14}, {383, 13}, {767, 12},        \
            {1535, 13}, {831, 12}, {1727, 13}, {895, 12}, {1791, 13}, {959, 14}, {511, 13},        \
            {1087, 12}, {2175, 13}, {1151, 14}, {639, 13}, {1343, 12}, {2687, 13}, {1407, 14},     \
            {767, 13}, {1599, 12}, {3199, 13}, {1663, 14}, {895, 13}, {1791, 15}, {511, 14},       \
            {1023, 13}, {2175, 14}, {1151, 13}, {2431, 12}, {4863, 14}, {1279, 13}, {2687, 14},    \
            {1407, 15}, {767, 14}, {1535, 13}, {3199, 14}, {1663, 13}, {3455, 14}, {1791, 16},     \
            {511, 15}, {1023, 14}, {2431, 13}, {4863, 15}, {1279, 14}, {2943, 13}, {5887, 15},     \
            {1535, 14}, {3455, 15}, {1791, 14}, {3839, 16}, {1023, 15}, {2047, 14}, {4223, 15},    \
            {2303, 14}, {4863, 15}, {2559, 14}, {5119, 15}, {2815, 14}, {5887, 16}, {1535, 15},    \
            {3071, 14}, {6143, 15}, {3327, 14}, {6911, 15}, {3839, 17}, {1023, 16}, {2047, 15},    \
            {4863, 16}, {2559, 15}, {5887, 14}, {11775, 16}, {3071, 15}, {6911, 16}, {3583, 15},   \
            {7679, 14}, {15359, 17}, {2047, 16}, {4095, 15}, {8191, 16}, {4607, 15}, {9983, 14},   \
            {19967, 16}, {5631, 15}, {11775, 17}, {3071, 16}, {65536, 17}, {131072, 18},           \
            {262144, 19}, {524288, 20}, {1048576, 21}, {2097152, 22}, {4194304, 23}, {             \
            8388608, 24                                                                            \
        }                                                                                          \
    }
#define WJR_SQR_FFT_TABLE3_SIZE 205
#define WJR_SQR_FFT_THRESHOLD 3200

#endif // WJR_GENERIC_MATH_BIGNUM_CONFIG_HPP__