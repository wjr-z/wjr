#ifndef WJR_BIGINTEGER_HPP__
#define WJR_BIGINTEGER_HPP__

#ifndef WJR_BIGINTEGER_BIGINTEGER_HPP__
#define WJR_BIGINTEGER_BIGINTEGER_HPP__

#include <optional>

#ifndef WJR_COMPRESSED_PAIR_HPP__
#define WJR_COMPRESSED_PAIR_HPP__

#include <array>
#include <tuple>

#ifndef WJR_TYPE_TRAITS_HPP__
#define WJR_TYPE_TRAITS_HPP__

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

#ifndef WJR_PREPROCESSOR_HPP__
#define WJR_PREPROCESSOR_HPP__

#ifndef WJR_PREPROCESSOR_PREVIEW_HPP__
#define WJR_PREPROCESSOR_PREVIEW_HPP__

// testing ...

#ifndef WJR_PREPROCESSOR_ARITHMATIC_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_HPP__

#ifndef WJR_PREPROCESSOR_ARITHMATIC_INC_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_INC_HPP__

#define WJR_PP_INC(x) WJR_PP_INC_I(x)
#define WJR_PP_INC_I(x) WJR_PP_INC_##x

#define WJR_PP_INC_0 1
#define WJR_PP_INC_1 2
#define WJR_PP_INC_2 3
#define WJR_PP_INC_3 4
#define WJR_PP_INC_4 5
#define WJR_PP_INC_5 6
#define WJR_PP_INC_6 7
#define WJR_PP_INC_7 8
#define WJR_PP_INC_8 9
#define WJR_PP_INC_9 10
#define WJR_PP_INC_10 11
#define WJR_PP_INC_11 12
#define WJR_PP_INC_12 13
#define WJR_PP_INC_13 14
#define WJR_PP_INC_14 15
#define WJR_PP_INC_15 16
#define WJR_PP_INC_16 17
#define WJR_PP_INC_17 18
#define WJR_PP_INC_18 19
#define WJR_PP_INC_19 20
#define WJR_PP_INC_20 21
#define WJR_PP_INC_21 22
#define WJR_PP_INC_22 23
#define WJR_PP_INC_23 24
#define WJR_PP_INC_24 25
#define WJR_PP_INC_25 26
#define WJR_PP_INC_26 27
#define WJR_PP_INC_27 28
#define WJR_PP_INC_28 29
#define WJR_PP_INC_29 30
#define WJR_PP_INC_30 31
#define WJR_PP_INC_31 32
#define WJR_PP_INC_32 33
#define WJR_PP_INC_33 34
#define WJR_PP_INC_34 35
#define WJR_PP_INC_35 36
#define WJR_PP_INC_36 37
#define WJR_PP_INC_37 38
#define WJR_PP_INC_38 39
#define WJR_PP_INC_39 40
#define WJR_PP_INC_40 41
#define WJR_PP_INC_41 42
#define WJR_PP_INC_42 43
#define WJR_PP_INC_43 44
#define WJR_PP_INC_44 45
#define WJR_PP_INC_45 46
#define WJR_PP_INC_46 47
#define WJR_PP_INC_47 48
#define WJR_PP_INC_48 49
#define WJR_PP_INC_49 50
#define WJR_PP_INC_50 51
#define WJR_PP_INC_51 52
#define WJR_PP_INC_52 53
#define WJR_PP_INC_53 54
#define WJR_PP_INC_54 55
#define WJR_PP_INC_55 56
#define WJR_PP_INC_56 57
#define WJR_PP_INC_57 58
#define WJR_PP_INC_58 59
#define WJR_PP_INC_59 60
#define WJR_PP_INC_60 61
#define WJR_PP_INC_61 62
#define WJR_PP_INC_62 63
#define WJR_PP_INC_63 0

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_INC_HPP__
#ifndef WJR_PREPROCESSOR_ARITHMATIC_DEC_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_DEC_HPP__

#define WJR_PP_DEC(x) WJR_PP_DEC_I(x)

#define WJR_PP_DEC_I(x) WJR_PP_DEC_##x

#define WJR_PP_DEC_0 63
#define WJR_PP_DEC_1 0
#define WJR_PP_DEC_2 1
#define WJR_PP_DEC_3 2
#define WJR_PP_DEC_4 3
#define WJR_PP_DEC_5 4
#define WJR_PP_DEC_6 5
#define WJR_PP_DEC_7 6
#define WJR_PP_DEC_8 7
#define WJR_PP_DEC_9 8
#define WJR_PP_DEC_10 9
#define WJR_PP_DEC_11 10
#define WJR_PP_DEC_12 11
#define WJR_PP_DEC_13 12
#define WJR_PP_DEC_14 13
#define WJR_PP_DEC_15 14
#define WJR_PP_DEC_16 15
#define WJR_PP_DEC_17 16
#define WJR_PP_DEC_18 17
#define WJR_PP_DEC_19 18
#define WJR_PP_DEC_20 19
#define WJR_PP_DEC_21 20
#define WJR_PP_DEC_22 21
#define WJR_PP_DEC_23 22
#define WJR_PP_DEC_24 23
#define WJR_PP_DEC_25 24
#define WJR_PP_DEC_26 25
#define WJR_PP_DEC_27 26
#define WJR_PP_DEC_28 27
#define WJR_PP_DEC_29 28
#define WJR_PP_DEC_30 29
#define WJR_PP_DEC_31 30
#define WJR_PP_DEC_32 31
#define WJR_PP_DEC_33 32
#define WJR_PP_DEC_34 33
#define WJR_PP_DEC_35 34
#define WJR_PP_DEC_36 35
#define WJR_PP_DEC_37 36
#define WJR_PP_DEC_38 37
#define WJR_PP_DEC_39 38
#define WJR_PP_DEC_40 39
#define WJR_PP_DEC_41 40
#define WJR_PP_DEC_42 41
#define WJR_PP_DEC_43 42
#define WJR_PP_DEC_44 43
#define WJR_PP_DEC_45 44
#define WJR_PP_DEC_46 45
#define WJR_PP_DEC_47 46
#define WJR_PP_DEC_48 47
#define WJR_PP_DEC_49 48
#define WJR_PP_DEC_50 49
#define WJR_PP_DEC_51 50
#define WJR_PP_DEC_52 51
#define WJR_PP_DEC_53 52
#define WJR_PP_DEC_54 53
#define WJR_PP_DEC_55 54
#define WJR_PP_DEC_56 55
#define WJR_PP_DEC_57 56
#define WJR_PP_DEC_58 57
#define WJR_PP_DEC_59 58
#define WJR_PP_DEC_60 59
#define WJR_PP_DEC_61 60
#define WJR_PP_DEC_62 61
#define WJR_PP_DEC_63 62

#endif // WJR_PREPROCESSOR_ARITHMATIC_DEC_HPP__
#ifndef WJR_PREPROCESSOR_ARITHMATIC_CMP_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_CMP_HPP__

#ifndef WJR_PREPROCESSOR_ARITHMATIC_BASIC_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_BASIC_HPP__

#define WJR_PP_ARITHMATIC_FROM_NUMBER(x) WJR_PP_ARITHMATIC_FROM_NUMBER_I(x)
#define WJR_PP_ARITHMATIC_FROM_NUMBER_I(x) WJR_PP_ARITHMATIC_FROM_NUMBER_##x

#define WJR_PP_ARITHMATIC_TO_NUMBER(x) WJR_PP_ARITHMATIC_TO_NUMBER_I(x)
#define WJR_PP_ARITHMATIC_TO_NUMBER_I(x) __wjr_pp_arithmatic_from_number_##x

#define WJR_PP_ARITHMATIC_FROM_NUMBER_0
#define WJR_PP_ARITHMATIC_FROM_NUMBER_1 x
#define WJR_PP_ARITHMATIC_FROM_NUMBER_2 xx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_3 xxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_4 xxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_5 xxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_6 xxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_7 xxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_8 xxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_9 xxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_10 xxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_11 xxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_12 xxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_13 xxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_14 xxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_15 xxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_16 xxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_17 xxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_18 xxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_19 xxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_20 xxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_21 xxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_22 xxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_23 xxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_24 xxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_25 xxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_26 xxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_27 xxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_28 xxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_29 xxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_30 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_31 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_32 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_33 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_34 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_35 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_36 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_37 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_38 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_39 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_40 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_41 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_42 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_43 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_44 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_45 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_46 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_47 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_48 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_49 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_50                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_51                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_52                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_53                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_54                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_55                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_56                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_57                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_58                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_59                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_60                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_61                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_62                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define WJR_PP_ARITHMATIC_FROM_NUMBER_63                                                 \
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#define __wjr_pp_arithmatic_from_number_ 0
#define __wjr_pp_arithmatic_from_number_x 1
#define __wjr_pp_arithmatic_from_number_xx 2
#define __wjr_pp_arithmatic_from_number_xxx 3
#define __wjr_pp_arithmatic_from_number_xxxx 4
#define __wjr_pp_arithmatic_from_number_xxxxx 5
#define __wjr_pp_arithmatic_from_number_xxxxxx 6
#define __wjr_pp_arithmatic_from_number_xxxxxxx 7
#define __wjr_pp_arithmatic_from_number_xxxxxxxx 8
#define __wjr_pp_arithmatic_from_number_xxxxxxxxx 9
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxx 10
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxx 11
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxx 12
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxx 13
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxx 14
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxx 15
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxx 16
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxx 17
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxx 18
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxx 19
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxx 20
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxx 21
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxx 22
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxx 23
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxx 24
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxx 25
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxx 26
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxx 27
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxx 28
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxx 29
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 30
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 31
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 32
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 33
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 34
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 35
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 36
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 37
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 38
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 39
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 40
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 41
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 42
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 43
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 44
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 45
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 46
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 47
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    48
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    49
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    50
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    51
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    52
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    53
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    54
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    55
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    56
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    57
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    58
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    59
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    60
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    61
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    62
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    63
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    2
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    3
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    4
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    5
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    6
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    7
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    8
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    9
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    10
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    11
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    12
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    13
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    14
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    15
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    16
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    17
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    18
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    19
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    20
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    21
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    22
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    23
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    24
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    25
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    26
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    27
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    28
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    29
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    30
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    31
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    32
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    33
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    34
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    35
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    36
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    37
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    38
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    39
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    40
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    41
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    42
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    43
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    44
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    45
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    46
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    47
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    48
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    49
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    50
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    51
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    52
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    53
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    54
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    55
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    56
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    57
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    58
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    59
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    60
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    61
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    62
#define __wjr_pp_arithmatic_from_number_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    63

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_BASIC_HPP__
// Already included
#ifndef WJR_PREPROCESSOR_ARITHMATIC_NEG_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_NEG_HPP__

#define WJR_PP_NEG(x) WJR_PP_NEG_I(x)
#define WJR_PP_NEG_I(x) WJR_PP_NEG_##x

#define WJR_PP_NEG_0 63
#define WJR_PP_NEG_1 62
#define WJR_PP_NEG_2 61
#define WJR_PP_NEG_3 60
#define WJR_PP_NEG_4 59
#define WJR_PP_NEG_5 58
#define WJR_PP_NEG_6 57
#define WJR_PP_NEG_7 56
#define WJR_PP_NEG_8 55
#define WJR_PP_NEG_9 54
#define WJR_PP_NEG_10 53
#define WJR_PP_NEG_11 52
#define WJR_PP_NEG_12 51
#define WJR_PP_NEG_13 50
#define WJR_PP_NEG_14 49
#define WJR_PP_NEG_15 48
#define WJR_PP_NEG_16 47
#define WJR_PP_NEG_17 46
#define WJR_PP_NEG_18 45
#define WJR_PP_NEG_19 44
#define WJR_PP_NEG_20 43
#define WJR_PP_NEG_21 42
#define WJR_PP_NEG_22 41
#define WJR_PP_NEG_23 40
#define WJR_PP_NEG_24 39
#define WJR_PP_NEG_25 38
#define WJR_PP_NEG_26 37
#define WJR_PP_NEG_27 36
#define WJR_PP_NEG_28 35
#define WJR_PP_NEG_29 34
#define WJR_PP_NEG_30 33
#define WJR_PP_NEG_31 32
#define WJR_PP_NEG_32 31
#define WJR_PP_NEG_33 30
#define WJR_PP_NEG_34 29
#define WJR_PP_NEG_35 28
#define WJR_PP_NEG_36 27
#define WJR_PP_NEG_37 26
#define WJR_PP_NEG_38 25
#define WJR_PP_NEG_39 24
#define WJR_PP_NEG_40 23
#define WJR_PP_NEG_41 22
#define WJR_PP_NEG_42 21
#define WJR_PP_NEG_43 20
#define WJR_PP_NEG_44 19
#define WJR_PP_NEG_45 18
#define WJR_PP_NEG_46 17
#define WJR_PP_NEG_47 16
#define WJR_PP_NEG_48 15
#define WJR_PP_NEG_49 14
#define WJR_PP_NEG_50 13
#define WJR_PP_NEG_51 12
#define WJR_PP_NEG_52 11
#define WJR_PP_NEG_53 10
#define WJR_PP_NEG_54 9
#define WJR_PP_NEG_55 8
#define WJR_PP_NEG_56 7
#define WJR_PP_NEG_57 6
#define WJR_PP_NEG_58 5
#define WJR_PP_NEG_59 4
#define WJR_PP_NEG_60 3
#define WJR_PP_NEG_61 2
#define WJR_PP_NEG_62 1
#define WJR_PP_NEG_63 0

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_NEG_HPP__
#ifndef WJR_PREPROCESSOR_DETAILS_BASIC_HPP__
#define WJR_PREPROCESSOR_DETAILS_BASIC_HPP__

#define WJR_PP_EMPTY(...)

#define WJR_PP_CONCAT(x, y) WJR_PP_CONCAT_I(x, y)
#define WJR_PP_CONCAT_I(x, y) x##y

#define WJR_PP_EXPAND(...) WJR_PP_EXPAND_I(__VA_ARGS__)
#define WJR_PP_EXPAND_I(...) __VA_ARGS__

#define WJR_PP_STR(x) WJR_PP_STR_I(x)
#define WJR_PP_STR_I(x) #x

#define WJR_PP_STRS(...) WJR_PP_STRS_I(__VA_ARGS__)
#define WJR_PP_STRS_I(...) # __VA_ARGS__

#define WJR_PP_ESC(x) WJR_PP_ESC_(WJR_PP_ESC_I, x)
#define WJR_PP_ESC_(M, x) M x
#define WJR_PP_ESC_I(...) __VA_ARGS__

// don't support 0 agument
#define WJR_PP_ARGS_LEN(...) WJR_PP_ARGS_LEN_I(__VA_ARGS__)

#define WJR_PP_ARGS_LEN_I(...)                                                           \
    WJR_PP_EXPAND(WJR_PP_ARGS_LEN_(0, ##__VA_ARGS__, WJR_PP_ARGS_LEN_RSEQ_N()))

#define WJR_PP_ARGS_LEN_RSEQ_N()                                                         \
    64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44,  \
        43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24,  \
        23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2,  \
        1, 0
#define WJR_PP_ARGS_LEN_(...) WJR_PP_EXPAND(WJR_PP_ARGS_LEN_N(__VA_ARGS__))
#define WJR_PP_ARGS_LEN_N(                                                               \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18,     \
    _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, \
    _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, \
    _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, N, ...)             \
    N

#define WJR_PP_IS_NULLPTR(VAL)                                                           \
    WJR_PP_IS_NULLPTR_I(WJR_PP_CONCAT(WJR_PP_IS_NULLPTR_, VAL), 0)
#define WJR_PP_IS_NULLPTR_I(...) WJR_PP_IS_NULLPTR_II(__VA_ARGS__)
#define WJR_PP_IS_NULLPTR_II(HOLDER, VAL, ...) VAL
#define WJR_PP_IS_NULLPTR_WJR_PP_NULLPTR WJR_PP_HOLDER, 1

#define WJR_PP_MAP_DEF(VAL) WJR_PP_HOLDER, VAL

// if MAP ## KEY is defined as WJR_PP_MAP_DEF, then return VAL
// else return WJR_PP_NULLPTR
#define WJR_PP_MAP_FIND(MAP, KEY) WJR_PP_MAP_FIND_I(MAP, KEY)
#define WJR_PP_MAP_FIND_I(MAP, KEY)                                                      \
    WJR_PP_MAP_FIND_II(WJR_PP_CONCAT(MAP, KEY), WJR_PP_NULLPTR)
#define WJR_PP_MAP_FIND_II(...) WJR_PP_MAP_FIND_III(__VA_ARGS__)
#define WJR_PP_MAP_FIND_III(HOLDER, VAL, ...) VAL

#endif // ! WJR_PREPROCESSOR_DETAILS_BASIC_HPP__
#ifndef WJR_PREPROCESSOR_LOGICAL_BASIC_HPP__
#define WJR_PREPROCESSOR_LOGICAL_BASIC_HPP__

#define WJR_PP_BOOL_IF(cond, t, f) WJR_PP_BOOL_IF_I(cond, t, f)
#define WJR_PP_BOOL_IF_I(cond, t, f) WJR_PP_BOOL_IF_I##cond(t, f)
#define WJR_PP_BOOL_IF_I0(t, f) f
#define WJR_PP_BOOL_IF_I1(t, f) t

#define WJR_PP_BOOL_NOT(x) WJR_PP_BOOL_IF(x, 0, 1)

#define WJR_PP_BOOL_AND(x, y) WJR_PP_BOOL_IF(x, WJR_PP_BOOL_IF(y, 1, 0), 0)
#define WJR_PP_BOOL_OR(x, y) WJR_PP_BOOL_IF(x, 1, WJR_PP_BOOL_IF(y, 1, 0))
#define WJR_PP_BOOL_XOR(x, y) WJR_PP_BOOL_IF(x, WJR_PP_BOOL_NOT(y), y)

#endif // ! WJR_PREPROCESSOR_LOGICAL_BASIC_HPP__
#ifndef WJR_PREPROCESSOR_LOGICAL_BOOL_HPP__
#define WJR_PREPROCESSOR_LOGICAL_BOOL_HPP__

#define WJR_PP_BOOL(x) WJR_PP_BOOL_I(x)

#define WJR_PP_BOOL_I(x) WJR_PP_BOOL_##x

#define WJR_PP_BOOL_0 0
#define WJR_PP_BOOL_1 1
#define WJR_PP_BOOL_2 1
#define WJR_PP_BOOL_3 1
#define WJR_PP_BOOL_4 1
#define WJR_PP_BOOL_5 1
#define WJR_PP_BOOL_6 1
#define WJR_PP_BOOL_7 1
#define WJR_PP_BOOL_8 1
#define WJR_PP_BOOL_9 1
#define WJR_PP_BOOL_10 1
#define WJR_PP_BOOL_11 1
#define WJR_PP_BOOL_12 1
#define WJR_PP_BOOL_13 1
#define WJR_PP_BOOL_14 1
#define WJR_PP_BOOL_15 1
#define WJR_PP_BOOL_16 1
#define WJR_PP_BOOL_17 1
#define WJR_PP_BOOL_18 1
#define WJR_PP_BOOL_19 1
#define WJR_PP_BOOL_20 1
#define WJR_PP_BOOL_21 1
#define WJR_PP_BOOL_22 1
#define WJR_PP_BOOL_23 1
#define WJR_PP_BOOL_24 1
#define WJR_PP_BOOL_25 1
#define WJR_PP_BOOL_26 1
#define WJR_PP_BOOL_27 1
#define WJR_PP_BOOL_28 1
#define WJR_PP_BOOL_29 1
#define WJR_PP_BOOL_30 1
#define WJR_PP_BOOL_31 1
#define WJR_PP_BOOL_32 1
#define WJR_PP_BOOL_33 1
#define WJR_PP_BOOL_34 1
#define WJR_PP_BOOL_35 1
#define WJR_PP_BOOL_36 1
#define WJR_PP_BOOL_37 1
#define WJR_PP_BOOL_38 1
#define WJR_PP_BOOL_39 1
#define WJR_PP_BOOL_40 1
#define WJR_PP_BOOL_41 1
#define WJR_PP_BOOL_42 1
#define WJR_PP_BOOL_43 1
#define WJR_PP_BOOL_44 1
#define WJR_PP_BOOL_45 1
#define WJR_PP_BOOL_46 1
#define WJR_PP_BOOL_47 1
#define WJR_PP_BOOL_48 1
#define WJR_PP_BOOL_49 1
#define WJR_PP_BOOL_50 1
#define WJR_PP_BOOL_51 1
#define WJR_PP_BOOL_52 1
#define WJR_PP_BOOL_53 1
#define WJR_PP_BOOL_54 1
#define WJR_PP_BOOL_55 1
#define WJR_PP_BOOL_56 1
#define WJR_PP_BOOL_57 1
#define WJR_PP_BOOL_58 1
#define WJR_PP_BOOL_59 1
#define WJR_PP_BOOL_60 1
#define WJR_PP_BOOL_61 1
#define WJR_PP_BOOL_62 1
#define WJR_PP_BOOL_63 1

#endif // WJR_PREPROCESSOR_LOGICAL_BOOL_HPP__

#define WJR_PP_LT(x, y) WJR_PP_BOOL_IF(WJR_PP_BOOL(y), WJR_PP_ADD_OVERFLOW(x, y), 0)
#define WJR_PP_GT(x, y) WJR_PP_LT(y, x)
#define WJR_PP_LE(x, y) WJR_PP_BOOL_NOT(WJR_PP_GT(x, y))
#define WJR_PP_GE(x, y) WJR_PP_LE(y, x)
#define WJR_PP_NE(x, y) WJR_PP_BOOL_OR(WJR_PP_LT(x, y), WJR_PP_LT(y, x))
#define WJR_PP_EQ(x, y) WJR_PP_BOOL_NOT(WJR_PP_NE(x, y))

#define WJR_PP_ADD_OVERFLOW(x, y)                                                        \
    WJR_PP_ADD_OVERFLOW_I(                                                               \
        WJR_PP_CONCAT(WJR_PP_ARITHMATIC_FROM_NUMBER(x),                                  \
                      WJR_PP_ARITHMATIC_FROM_NUMBER(WJR_PP_INC(WJR_PP_NEG(y)))))
#define WJR_PP_ADD_OVERFLOW_I(x) WJR_PP_ADD_OVERFLOW_II(x)
#define WJR_PP_ADD_OVERFLOW_II(x) __wjr_arithmatic_add_overflow_##x

#define __wjr_arithmatic_add_overflow_ 1
#define __wjr_arithmatic_add_overflow_x 1
#define __wjr_arithmatic_add_overflow_xx 1
#define __wjr_arithmatic_add_overflow_xxx 1
#define __wjr_arithmatic_add_overflow_xxxx 1
#define __wjr_arithmatic_add_overflow_xxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    1
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0
#define __wjr_arithmatic_add_overflow_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \
    0

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_CMP_HPP__
#ifndef WJR_PREPROCESSOR_ARITHMATIC_SUB_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_SUB_HPP__

#ifndef WJR_PREPROCESSOR_ARITHMATIC_ADD_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_ADD_HPP__

// Already included
// Already included

#define WJR_PP_ADD(x, y) WJR_PP_ADD_I(x, y)
#define WJR_PP_ADD_I(x, y)                                                               \
    WJR_PP_ARITHMATIC_TO_NUMBER(WJR_PP_CONCAT(WJR_PP_ARITHMATIC_FROM_NUMBER(x),          \
                                              WJR_PP_ARITHMATIC_FROM_NUMBER(y)))

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_ADD_HPP__
// Already included
// Already included

#define WJR_PP_SUB(x, y) WJR_PP_ADD(x, WJR_PP_INC(WJR_PP_NEG(y)))

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_SUB_HPP__

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_HPP__
#ifndef WJR_PREPROCESSOR_COMPILER_HPP__
#define WJR_PREPROCESSOR_COMPILER_HPP__

// TODO : 
// A universal macro for determining the existence of macros

#ifndef WJR_PREPROCESSOR_COMPILER_ATTRIBUTE_HPP__
#define WJR_PREPROCESSOR_COMPILER_ATTRIBUTE_HPP__

#ifndef WJR_PREPROCESSOR_COMPILER_HAS_HPP__
#define WJR_PREPROCESSOR_COMPILER_HAS_HPP__

#ifndef WJR_PREPROCESSOR_COMPILER_ARCH_HPP__
#define WJR_PREPROCESSOR_COMPILER_ARCH_HPP__

#if defined(__pnacl__) || defined(__CLR_VER)
#define WJR_VM
#endif

#if (defined(_M_IX86) || defined(__i386__)) && !defined(WJR_VM)
#define WJR_X86_32
#endif

#if (defined(_M_X64) || defined(__x86_64__)) && !defined(WJR_VM)
#define WJR_X86_64
#endif

#if defined(WJR_X86_32) || defined(WJR_X86_64)
#define WJR_X86
#endif

#if (defined(__arm__) || defined(_M_ARM))
#define WJR_ARM
#endif

#if defined(__aarch64__)
#define WJR_AARCH64
#endif

#if defined(__powerpc64__)
#define WJR_PPC64
#endif

#if defined(WJR_X86_64)
#if defined(__i386__) || defined(_M_IX86) || defined(_X86_)
#define CPU_INTEL
#elif defined(_M_AMD64)
#define CPU_AMD
#endif
#else
#define CPU_UNKNOWN
#endif

#endif // !WJR_PREPROCESSOR_COMPILER_ARCH_HPP__
#ifndef WJR_PREPROCESSOR_COMPILER_COMPILER_HPP__
#define WJR_PREPROCESSOR_COMPILER_COMPILER_HPP__

#if defined(__clang__)
#define WJR_COMPILER_CLANG
#elif defined(__GNUC__)
#define WJR_COMPILER_GCC
#elif defined(_MSC_VER)
#define WJR_COMPILER_MSVC
#endif

#if defined(_MSC_VER)
#define WJR_MSVC
#endif // _MSC_VER

#if defined(__GNUC__)
#define WJR_HAS_GCC(major, minor, patchlevel)                                            \
    ((__GNUC__ > (major)) || (__GNUC__ == (major) && __GNUC_MINOR__ > (minor)) ||        \
     (__GNUC__ == (major) && __GNUC_MINOR__ == (minor) &&                                \
      __GNUC_PATCHLEVEL__ >= (patchlevel)))
#else
#define WJR_HAS_GCC(major, minor, patchlevel) 0
#endif // __GNUC__

#if defined(__clang__)
#define WJR_HAS_CLANG(major, minor, patchlevel)                                          \
    ((__clang_major__ > (major)) ||                                                      \
     (__clang_major__ == (major) && __clang_minor__ > (minor)) ||                        \
     (__clang_major__ == (major) && __clang_minor__ == (minor) &&                        \
      __clang_patchlevel__ >= (patchlevel)))
#else
#define WJR_HAS_CLANG(major, minor, patchlevel) 0
#endif

#if defined(_MSC_VER)
#define WJR_HAS_MSVC(minor, level) (_MSC_VER >= (minor)*100 + (level))
#else
#define WJR_HAS_MSVC(minor, level) 0
#endif

#if (defined(WJR_COMPILER_GCC) && !WJR_HAS_GCC(7, 1, 0)) ||                              \
    (defined(WJR_COMPILER_CLANG) && !WJR_HAS_CLANG(5, 0, 0))
#error "GCC 7.1.0 or Clang 5.0.0 or later is required"
#endif

#if defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC)
#define WJR_CPP_STANDARD __cplusplus
#elif defined(WJR_COMPILER_MSVC)
#define WJR_CPP_STANDARD _MSVC_LANG
#endif

#if WJR_CPP_STANDARD >= 199711L
#define WJR_CPP_03
#endif
#if WJR_CPP_STANDARD >= 201103L
#define WJR_CPP_11
#endif
#if WJR_CPP_STANDARD >= 201402L
#define WJR_CPP_14
#endif
#if WJR_CPP_STANDARD >= 201703L
#define WJR_CPP_17
#endif
#if WJR_CPP_STANDARD >= 202002L
#define WJR_CPP_20
#endif

#ifndef WJR_CPP_17
#error "required C++17 or later"
#endif // c++17

#if defined(__cpp_char8_t)
#define WJR_CHAR8_T
#endif // __cpp_char8_t

#if defined(__LINE__)
#define WJR_LINE __LINE__
#elif defined(__COUNTER__)
#define WJR_LINE __COUNTER__
#else
#define WJR_LINE -1
#endif

#ifdef __FILE__
#define WJR_FILE __FILE__
#else
#define WJR_FILE ""
#endif

#endif // !WJR_PREPROCESSOR_COMPILER_COMPILER_HPP__
// Already included
// Already included

#define WJR_HAS_DEF_VAR(var) WJR_PP_MAP_DEF(var)
#define WJR_HAS_DEF WJR_HAS_DEF_VAR(1)

#define WJR_HAS_FIND(MAP, KEY) WJR_HAS_FIND_I(WJR_PP_MAP_FIND(MAP, KEY))
#define WJR_HAS_FIND_I(VAL) WJR_PP_BOOL_IF(WJR_PP_IS_NULLPTR(VAL), 0, VAL)

// Currently only has_builtin, has_attribute, has_feature are supported.
#define WJR_HAS_BUILTIN_FIND(KEY) WJR_HAS_FIND(WJR_HAS_BUILTIN_, KEY)
#define WJR_HAS_ATTRIBUTE_FIND(KEY) WJR_HAS_FIND(WJR_HAS_ATTRIBUTE_, KEY)
#define WJR_HAS_FEATURE_FIND(KEY) WJR_HAS_FIND(WJR_HAS_FEATURE_, KEY)
#define WJR_HAS_SIMD_FIND(KEY) WJR_HAS_FIND(WJR_HAS_SIMD_, KEY)

//

#if (defined(WJR_COMPILER_GCC) && WJR_HAS_GCC(10, 1, 0)) ||                              \
    (defined(WJR_COMPILER_CLANG) && WJR_HAS_CLANG(10, 0, 0)) ||                          \
    (!defined(WJR_COMPILER_GCC) && !defined(WJR_COMPILER_CLANG) &&                       \
     defined(__has_builtin))
#define WJR_HAS_BUILTIN(x) WJR_PP_BOOL_IF(WJR_HAS_BUILTIN_FIND(x), 1, __has_builtin(x))
#else
#define WJR_HAS_BUILTIN(x) WJR_HAS_BUILTIN_FIND(x)
#endif

#if defined(__has_include)
#define WJR_HAS_INCLUDE(x) __has_include(x)
#else
#define WJR_HAS_INCLUDE(x) 0
#endif // __has_includeF

#if defined(__has_attribute)
#define WJR_HAS_ATTRIBUTE(x)                                                             \
    WJR_PP_BOOL_IF(WJR_HAS_ATTRIBUTE_FIND(x), 1, __has_attribute(x))
#else
#define WJR_HAS_ATTRIBUTE(x) WJR_HAS_ATTRIBUTE_FIND(x)
#endif

#if defined(__has_cpp_attribute)
#define WJR_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define WJR_HAS_CPP_ATTRIBUTE(x) 0
#endif

#define WJR_HAS_FEATURE(x) WJR_HAS_FEATURE_FIND(x)

#define WJR_HAS_SIMD(x) WJR_HAS_SIMD_FIND(x)

// WJR_HAS_BUILTIN

#if WJR_HAS_GCC(7, 1, 0) || WJR_HAS_CLANG(5, 0, 0)
#define WJR_HAS_BUILTIN___builtin_unreachable WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_expect WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_constant_p WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_clz WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_ctz WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_popcount WJR_HAS_DEF
#endif

#if WJR_HAS_GCC(9, 1, 0) || WJR_HAS_CLANG(9, 0, 0)
#define WJR_HAS_BUILTIN___builtin_is_constant_evaluated WJR_HAS_DEF
#endif

#if WJR_HAS_CLANG(5, 0, 0)
#define WJR_HAS_BUILTIN___builtin_addc WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_subc WJR_HAS_DEF
#endif

#if WJR_HAS_GCC(9, 1, 0) || WJR_HAS_CLANG(11, 0, 0)
#define WJR_HAS_BUILTIN___builtin_expect_with_probability WJR_HAS_DEF
#endif

// WJR_HAS_FEATURE

#if WJR_HAS_GCC(7, 1, 0) || WJR_HAS_CLANG(5, 0, 0)
#define WJR_HAS_FEATURE_PRAGMA_UNROLL WJR_HAS_DEF
#endif

#if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_MSVC)
#define WJR_HAS_FEATURE_INLINE_ASM WJR_HAS_DEF
#ifndef WJR_COMPILER_MSVC
#define WJR_HAS_FEATURE_GCC_STYLE_INLINE_ASM WJR_HAS_DEF
#endif
#endif

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)

#if WJR_HAS_GCC(7, 1, 0) || WJR_HAS_CLANG(9, 0, 0)
#define WJR_HAS_FEATURE_INLINE_ASM_GOTO WJR_HAS_DEF
#endif

#if WJR_HAS_GCC(11, 1, 0) || WJR_HAS_CLANG(11, 0, 0)
#define WJR_HAS_FEATURE_INLINE_ASM_GOTO_OUTPUT WJR_HAS_DEF
#endif

#if defined(WJR_COMPILER_GCC) || WJR_HAS_CLANG(9, 0, 0)
#define WJR_HAS_FEATURE_INLINE_ASM_CCCOND WJR_HAS_DEF
#endif

#endif

#if defined(__SIZEOF_INT128__)
#define WJR_HAS_FEATURE_INT128 WJR_HAS_DEF
#if !(defined(__clang__) && defined(LIBDIVIDE_VC))
#define WJR_HAS_FEATURE_INT128_DIV WJR_HAS_DEF
#endif
#endif

// There are some issues with the optimization of int128 in both lower and higher versions
// (13.1/13.2) of GCC.
#if WJR_HAS_FEATURE(INT128) &&                                                           \
    (defined(WJR_COMPILER_CLANG) ||                                                      \
     (defined(WJR_COMPILER_GCC) && WJR_HAS_GCC(8, 1, 0) && !WJR_HAS_GCC(13, 1, 0)))
#define WJR_HAS_FEATURE_FAST_INT128_COMPARE WJR_HAS_DEF
#endif

// performance bug
#if WJR_HAS_FEATURE(INT128) && defined(WJR_COMPILER_CLANG)
// #define WJR_HAS_FEATURE_FAST_INT128_ADDSUB WJR_HAS_DEF
#endif

#if WJR_HAS_GCC(11, 1, 0) || WJR_HAS_CLANG(5, 0, 0)
#define WJR_HAS_ATTRIBUTE_FORCEINLINE_LAMBDA WJR_HAS_DEF
#endif

#if defined(__AVX512VL__)
#define WJR_HAS_SIMD_AVX512VL WJR_HAS_DEF
#endif

#if defined(__AVX512BW__)
#define WJR_HAS_SIMD_AVX512BW WJR_HAS_DEF
#endif

#if defined(__AVX512DQ__)
#define WJR_HAS_SIMD_AVX512DQ WJR_HAS_DEF
#endif

#if defined(__AVX512F__) ||                                                              \
    (WJR_HAS_SIMD(AVX512VL) && WJR_HAS_SIMD(AVX512BW) && WJR_HAS_SIMD(AVX512DQ))
#define WJR_HAS_SIMD_AVX512F WJR_HAS_DEF
#endif

#if defined(__AVX512__) ||                                                               \
    (WJR_HAS_SIMD(AVX512F) && WJR_HAS_SIMD(AVX512BW) && WJR_HAS_SIMD(AVX512DQ))
#define WJR_HAS_SIMD_AVX512 WJR_HAS_DEF
#endif

#if defined(__AVX2__) || (WJR_HAS_SIMD(AVX512) || WJR_HAS_SIMD(AVX512F))
#define WJR_HAS_SIMD_AVX2 WJR_HAS_DEF
#endif

#if defined(__AVX__) || WJR_HAS_SIMD(AVX2)
#define WJR_HAS_SIMD_AVX WJR_HAS_DEF
#endif

#if defined(__SSE4_2__) || WJR_HAS_SIMD(AVX)
#define WJR_HAS_SIMD_SSE4_2 WJR_HAS_DEF
#endif

#if defined(__SSE4_1__) || WJR_HAS_SIMD(SSE4_2)
#define WJR_HAS_SIMD_SSE4_1 WJR_HAS_DEF
#endif

#if defined(__SSSE3__) || WJR_HAS_SIMD(SSE4_1)
#define WJR_HAS_SIMD_SSSE3 WJR_HAS_DEF
#endif

#if defined(__SSE3__) || WJR_HAS_SIMD(SSSE3)
#define WJR_HAS_SIMD_SSE3 WJR_HAS_DEF
#endif

#if defined(__SSE2__) || WJR_HAS_SIMD(SSE3) || _M_IX86_FP >= 2 ||                        \
    (defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_X64)))
#define WJR_HAS_SIMD_SSE2 WJR_HAS_DEF
#endif

#if defined(__SSE__) || WJR_HAS_SIMD(SSE2) || _M_IX86_FP >= 1
#define WJR_HAS_SIMD_SSE WJR_HAS_DEF
#endif

#if defined(__MMX__) || WJR_HAS_SIMD(SSE)
#define WJR_HAS_SIMD_MMX WJR_HAS_DEF
#endif

#if defined(__XOP__)
#define WJR_HAS_SIMD_XOP WJR_HAS_DEF
#endif

#if defined(__POPCNT__)
#define WJR_HAS_SIMD_POPCNT WJR_HAS_DEF
#endif

#endif // WJR_PREPROCESSOR_COMPILER_HAS_HPP__

#if WJR_HAS_CPP_ATTRIBUTE(fallthrough)
#define WJR_FALLTHROUGH [[fallthrough]]
#elif WJR_HAS_ATTRIBUTE(fallthrough)
#define WJR_FALLTHROUGH __attribute__((fallthrough))
#elif defined(_MSC_VER) && defined(__fallthrough)
#define WJR_FALLTHROUGH __fallthrough
#else
#define WJR_FALLTHROUGH
#endif

#if WJR_HAS_CPP_ATTRIBUTE(noreturn)
#define WJR_NORETURN [[noreturn]]
#elif WJR_HAS_ATTRIBUTE(noreturn)
#define WJR_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define WJR_NORETURN __declspec(noreturn)
#else
#define WJR_NORETURN
#endif

#if WJR_HAS_CPP_ATTRIBUTE(nodiscard)
#define WJR_NODISCARD [[nodiscard]]
#elif WJR_HAS_ATTRIBUTE(nodiscard)
#define WJR_NODISCARD __attribute__((nodiscard))
#elif defined(_MSC_VER)
#define WJR_NODISCARD _Check_return_
#else
#define WJR_NODISCARD
#endif

#if WJR_HAS_CPP_ATTRIBUTE(deprecated)
#define WJR_DEPRECATED [[deprecated]]
#elif WJR_HAS_ATTRIBUTE(deprecated)
#define WJR_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define WJR_DEPRECATED __declspec(deprecated)
#else
#define WJR_DEPRECATED
#endif

#if WJR_HAS_CPP_ATTRIBUTE(maybe_unused)
#define WJR_MAYBE_UNUSED [[maybe_unused]]
#elif WJR_HAS_ATTRIBUTE(maybe_unused)
#define WJR_MAYBE_UNUSED __attribute__((maybe_unused))
#elif defined(_MSC_VER)
#define WJR_MAYBE_UNUSED
#else
#define WJR_MAYBE_UNUSED
#endif

#if WJR_HAS_ATTRIBUTE(always_inline)
#define WJR_FORCEINLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#else
#define WJR_FORCEINLINE
#endif

#if WJR_HAS_ATTRIBUTE(FORCEINLINE_LAMBDA)
#define WJR_FORCEINLINE_LAMBDA WJR_FORCEINLINE
#else
#define WJR_FORCEINLINE_LAMBDA
#endif

// NOINLINE for MSVC/GCC/CLANG ...
#if WJR_HAS_ATTRIBUTE(noinline)
#define WJR_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define WJR_NOINLINE __declspec(noinline)
#else
#define WJR_NOINLINE
#endif

#if WJR_HAS_ATTRIBUTE(hot)
#define WJR_HOT __attribute__((hot))
#elif defined(_MSC_VER)
#define WJR_HOT
#else
#define WJR_HOT
#endif

#if WJR_HAS_ATTRIBUTE(cold)
#define WJR_COLD __attribute__((cold))
#elif defined(_MSC_VER)
#define WJR_COLD
#else
#define WJR_COLD
#endif

#if defined(__cpp_lib_unreachable)
#define WJR_UNREACHABLE() std::unreachable()
#elif WJR_HAS_BUILTIN(__builtin_unreachable)
#define WJR_UNREACHABLE() __builtin_unreachable()
#elif defined(WJR_COMPILER_MSVC)
#define WJR_UNREACHABLE() __assume(0)
#else
#define WJR_UNREACHABLE()
#endif

#if defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC)
#define WJR_RESTRICT __restrict
#else
#define WJR_RESTRICT
#endif

#if WJR_HAS_BUILTIN(__builtin_assume)
#define WJR_ASSUME(expr) __builtin_assume(expr)
#elif defined(WJR_COMPILER_MSVC)
#define WJR_ASSUME(expr) __assume(expr)
#elif WJR_HAS_CPP_ATTRIBUTE(assume)
#define WJR_ASSUME(expr) [[assume(expr)]]
#else
#define WJR_ASSUME(expr)                                                                 \
    do {                                                                                 \
        if (!(expr)) {                                                                   \
            WJR_UNREACHABLE();                                                           \
        }                                                                                \
    } while (0)
#endif

#define WJR_BOOL_EXPR(expr) (!!(expr))

#if WJR_HAS_BUILTIN(__builtin_expect)
#define WJR_EXPECT(expr, expect) __builtin_expect((expr), (expect))
#else
#define WJR_EXPECT(expr, expect) (expr)
#endif

#define WJR_LIKELY(expr) WJR_EXPECT(WJR_BOOL_EXPR(expr), true)
#define WJR_UNLIKELY(expr) WJR_EXPECT(WJR_BOOL_EXPR(expr), false)

#define WJR_HAS_FEATURE_IS_CONSTANT_EVALUATED WJR_HAS_DEF

#if WJR_HAS_BUILTIN(__builtin_expect_with_probability)
#define WJR_EXPECT_WITH_PROBABILITY(exp, c, probability)                                 \
    __builtin_expect_with_probability(exp, c, probability)
#else
#define WJR_EXPECT_WITH_PROBABILITY(exp, c, probability) (expr)
#endif

#if WJR_HAS_BUILTIN(__builtin_expect_with_probability)
#define WJR_VERY_LIKELY(exp, probability)                                                \
    WJR_EXPECT_WITH_PROBABILITY(exp, true, probability)
#define WJR_VERY_UNLIKELY(exp, probability)                                              \
    WJR_EXPECT_WITH_PROBABILITY(exp, false, probability)
#else
#define WJR_VERY_LIKELY(exp, probability) WJR_LIKELY((exp))
#define WJR_VERY_UNLIKELY(exp, probability) WJR_UNLIKELY((exp))
#endif

#if defined(__cpp_lib_is_constant_evaluated)
#define WJR_IS_CONSTANT_EVALUATED() std::is_constant_evaluated()
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated)
#define WJR_IS_CONSTANT_EVALUATED() __builtin_is_constant_evaluated()
#else
#define WJR_IS_CONSTANT_EVALUATED() false
#undef WJR_HAS_FEATURE_IS_CONSTANT_EVALUATED
#endif

#if WJR_HAS_BUILTIN(__builtin_constant_p)
#define WJR_BUILTIN_CONSTANT_P(expr) __builtin_constant_p(expr)
#else
#define WJR_BUILTIN_CONSTANT_P(expr) false
#endif

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_COMPILER_BARRIER() asm volatile("" ::: "memory")
#else
#define WJR_COMPILER_BARRIER()
#endif

#define WJR_CONSTEXPR_COMPILER_BARRIER()                                                 \
    do {                                                                                 \
        if (!(WJR_IS_CONSTANT_EVALUATED())) {                                            \
            WJR_COMPILER_BARRIER();                                                      \
        }                                                                                \
    } while (0)

// Lower versions of clang cannot predict branches well
#if defined(WJR_COMPILER_CLANG) && !WJR_HAS_CLANG(13, 0, 0)
#define WJR_FORCE_BRANCH_BARRIER() WJR_CONSTEXPR_COMPILER_BARRIER()
#else
#define WJR_FORCE_BRANCH_BARRIER()
#endif

#ifdef WJR_CPP_20
#define WJR_EXPLICIT(expression) explicit(expression)
#define WJR_EXPLICIT20(expreesion) explicit(expression)
#else
#define WJR_EXPLICIT(expression) explicit
#define WJR_EXPLICIT20(expression)
#endif

#if defined(WJR_FORCEINLINE)
#define WJR_INTRINSIC_INLINE inline WJR_FORCEINLINE
#else
#define WJR_INTRINSIC_INLINE inline
#endif

// pure attribute
#if WJR_HAS_ATTRIBUTE(pure)
#define WJR_PURE __attribute__((pure))
#else
#define WJR_PURE
#endif

// const attribute
#if WJR_HAS_ATTRIBUTE(const)
#define WJR_CONST __attribute__((const))
#else
#define WJR_CONST
#endif

#if WJR_HAS_ATTRIBUTE(malloc)
#define WJR_MALLOC __attribute__((malloc))
#else
#define WJR_MALLOC
#endif

#define WJR_INLINE inline
#define WJR_CONSTEXPR constexpr

#if defined(WJR_CPP_20)
#define WJR_CONSTEXPR20 constexpr
#else
#define WJR_CONSTEXPR20
#endif

#if WJR_HAS_FEATURE(IS_CONSTANT_EVALUATED)
#define WJR_CONSTEXPR_E constexpr
#else
#define WJR_CONSTEXPR_E
#endif

#define WJR_INTRINSIC_CONSTEXPR WJR_INTRINSIC_INLINE constexpr
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE WJR_CONSTEXPR20
#define WJR_INTRINSIC_CONSTEXPR_E WJR_INTRINSIC_INLINE WJR_CONSTEXPR_E

#define WJR_INLINE_CONSTEXPR inline constexpr
#define WJR_INLINE_CONSTEXPR20 inline WJR_CONSTEXPR20
#define WJR_INLINE_CONSTEXPR_E inline WJR_CONSTEXPR_E

#define WJR_ATTRIBUTE(attribute) WJR_ATTRIBUTE_I(attribute)
#define WJR_ATTRIBUTE_I(attribute) WJR_##attribute

#endif // WJR_PREPROCESSOR_COMPILER_ATTRIBUTE_HPP__

#endif // ! WJR_PREPROCESSOR_COMPILER_HPP__
#ifndef WJR_PREPROCESSOR_DETAILS_HPP__
#define WJR_PREPROCESSOR_DETAILS_HPP__

// Already included
#ifndef WJR_PREPROCESSOR_DETAILS_IOTA_HPP__
#define WJR_PREPROCESSOR_DETAILS_IOTA_HPP__

#define WJR_PP_IOTA(n) WJR_PP_IOTA_I(n)
#define WJR_PP_IOTA_I(n) WJR_PP_IOTA_##n

#define WJR_PP_IOTA_0
#define WJR_PP_IOTA_1 0
#define WJR_PP_IOTA_2 WJR_PP_IOTA_1, 1
#define WJR_PP_IOTA_3 WJR_PP_IOTA_2, 2
#define WJR_PP_IOTA_4 WJR_PP_IOTA_3, 3
#define WJR_PP_IOTA_5 WJR_PP_IOTA_4, 4
#define WJR_PP_IOTA_6 WJR_PP_IOTA_5, 5
#define WJR_PP_IOTA_7 WJR_PP_IOTA_6, 6
#define WJR_PP_IOTA_8 WJR_PP_IOTA_7, 7
#define WJR_PP_IOTA_9 WJR_PP_IOTA_8, 8
#define WJR_PP_IOTA_10 WJR_PP_IOTA_9, 9
#define WJR_PP_IOTA_11 WJR_PP_IOTA_10, 10
#define WJR_PP_IOTA_12 WJR_PP_IOTA_11, 11
#define WJR_PP_IOTA_13 WJR_PP_IOTA_12, 12
#define WJR_PP_IOTA_14 WJR_PP_IOTA_13, 13
#define WJR_PP_IOTA_15 WJR_PP_IOTA_14, 14
#define WJR_PP_IOTA_16 WJR_PP_IOTA_15, 15
#define WJR_PP_IOTA_17 WJR_PP_IOTA_16, 16
#define WJR_PP_IOTA_18 WJR_PP_IOTA_17, 17
#define WJR_PP_IOTA_19 WJR_PP_IOTA_18, 18
#define WJR_PP_IOTA_20 WJR_PP_IOTA_19, 19
#define WJR_PP_IOTA_21 WJR_PP_IOTA_20, 20
#define WJR_PP_IOTA_22 WJR_PP_IOTA_21, 21
#define WJR_PP_IOTA_23 WJR_PP_IOTA_22, 22
#define WJR_PP_IOTA_24 WJR_PP_IOTA_23, 23
#define WJR_PP_IOTA_25 WJR_PP_IOTA_24, 24
#define WJR_PP_IOTA_26 WJR_PP_IOTA_25, 25
#define WJR_PP_IOTA_27 WJR_PP_IOTA_26, 26
#define WJR_PP_IOTA_28 WJR_PP_IOTA_27, 27
#define WJR_PP_IOTA_29 WJR_PP_IOTA_28, 28
#define WJR_PP_IOTA_30 WJR_PP_IOTA_29, 29
#define WJR_PP_IOTA_31 WJR_PP_IOTA_30, 30
#define WJR_PP_IOTA_32 WJR_PP_IOTA_31, 31
#define WJR_PP_IOTA_33 WJR_PP_IOTA_32, 32
#define WJR_PP_IOTA_34 WJR_PP_IOTA_33, 33
#define WJR_PP_IOTA_35 WJR_PP_IOTA_34, 34
#define WJR_PP_IOTA_36 WJR_PP_IOTA_35, 35
#define WJR_PP_IOTA_37 WJR_PP_IOTA_36, 36
#define WJR_PP_IOTA_38 WJR_PP_IOTA_37, 37
#define WJR_PP_IOTA_39 WJR_PP_IOTA_38, 38
#define WJR_PP_IOTA_40 WJR_PP_IOTA_39, 39
#define WJR_PP_IOTA_41 WJR_PP_IOTA_40, 40
#define WJR_PP_IOTA_42 WJR_PP_IOTA_41, 41
#define WJR_PP_IOTA_43 WJR_PP_IOTA_42, 42
#define WJR_PP_IOTA_44 WJR_PP_IOTA_43, 43
#define WJR_PP_IOTA_45 WJR_PP_IOTA_44, 44
#define WJR_PP_IOTA_46 WJR_PP_IOTA_45, 45
#define WJR_PP_IOTA_47 WJR_PP_IOTA_46, 46
#define WJR_PP_IOTA_48 WJR_PP_IOTA_47, 47
#define WJR_PP_IOTA_49 WJR_PP_IOTA_48, 48
#define WJR_PP_IOTA_50 WJR_PP_IOTA_49, 49
#define WJR_PP_IOTA_51 WJR_PP_IOTA_50, 50
#define WJR_PP_IOTA_52 WJR_PP_IOTA_51, 51
#define WJR_PP_IOTA_53 WJR_PP_IOTA_52, 52
#define WJR_PP_IOTA_54 WJR_PP_IOTA_53, 53
#define WJR_PP_IOTA_55 WJR_PP_IOTA_54, 54
#define WJR_PP_IOTA_56 WJR_PP_IOTA_55, 55
#define WJR_PP_IOTA_57 WJR_PP_IOTA_56, 56
#define WJR_PP_IOTA_58 WJR_PP_IOTA_57, 57
#define WJR_PP_IOTA_59 WJR_PP_IOTA_58, 58
#define WJR_PP_IOTA_60 WJR_PP_IOTA_59, 59
#define WJR_PP_IOTA_61 WJR_PP_IOTA_60, 60
#define WJR_PP_IOTA_62 WJR_PP_IOTA_61, 61
#define WJR_PP_IOTA_63 WJR_PP_IOTA_62, 62
#define WJR_PP_IOTA_64 WJR_PP_IOTA_63, 63

#endif // ! WJR_PREPROCESSOR_DETAILS_IOTA_HPP__
#ifndef WJR_PREPROCESSOR_DETAILS_REPEAT_HPP__
#define WJR_PREPROCESSOR_DETAILS_REPEAT_HPP__

#define WJR_PP_REPEAT(x, n) WJR_PP_REPEAT_I(x, n)
#define WJR_PP_REPEAT_I(x, n) WJR_PP_REPEAT_##n(x)

#define WJR_PP_REPEAT_0(x)
#define WJR_PP_REPEAT_1(x) x
#define WJR_PP_REPEAT_2(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_1(x)
#define WJR_PP_REPEAT_3(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x)
#define WJR_PP_REPEAT_4(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_2(x)
#define WJR_PP_REPEAT_5(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x)
#define WJR_PP_REPEAT_6(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x)
#define WJR_PP_REPEAT_7(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x)
#define WJR_PP_REPEAT_8(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_4(x)
#define WJR_PP_REPEAT_9(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_10(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_11(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_12(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_13(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_14(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_15(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_16(x) WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_8(x)
#define WJR_PP_REPEAT_17(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_18(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_19(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_20(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_21(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_22(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_23(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_24(x) WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_25(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_26(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_27(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_28(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_29(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_30(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_31(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x),      \
        WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_32(x) WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_16(x)
#define WJR_PP_REPEAT_33(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_34(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_35(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_36(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_37(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_38(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_39(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_40(x) WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_41(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_42(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_43(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_44(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_45(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_46(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_47(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x),      \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_48(x) WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_49(x) WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_50(x) WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_51(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_52(x) WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_53(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_54(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_55(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_16(x),     \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_56(x) WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_57(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_58(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_59(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x),     \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_60(x)                                                              \
    WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_61(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x),     \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_62(x)                                                              \
    WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x), WJR_PP_REPEAT_16(x),     \
        WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_63(x)                                                              \
    WJR_PP_REPEAT_1(x), WJR_PP_REPEAT_2(x), WJR_PP_REPEAT_4(x), WJR_PP_REPEAT_8(x),      \
        WJR_PP_REPEAT_16(x), WJR_PP_REPEAT_32(x)
#define WJR_PP_REPEAT_64(x) WJR_PP_REPEAT_32(x), WJR_PP_REPEAT_32(x)

#endif // ! WJR_PREPROCESSOR_DETAILS_REPEAT_HPP__

#endif // ! WJR_PREPROCESSOR_DETAILS_HPP__
#ifndef WJR_PREPROCESSOR_LOGICAL_HPP__
#define WJR_PREPROCESSOR_LOGICAL_HPP__

// Already included
// Already included

#endif // ! WJR_PREPROCESSOR_LOGICAL_HPP__

// TODO : Recursive nested queue algorithm
// Due to the fact that call is not a simple expansion, but takes the previous output as
// the next input, the difficulty of implementing recursion is also high.
#ifndef WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__
#define WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__

// Already included
// Already included
#ifndef WJR_PREPROCESSOR_QUEUE_CALL_HPP__
#define WJR_PREPROCESSOR_QUEUE_CALL_HPP__

// Already included
// Already included
// Already included
#ifndef WJR_PREPROCCESSOR_QUEUE_BASIC_HPP__
#define WJR_PREPROCCESSOR_QUEUE_BASIC_HPP__

// Already included

#define WJR_PP_QUEUE_EXPAND(queue) WJR_PP_QUEUE_EXPAND_I queue
#define WJR_PP_QUEUE_EXPAND_I(...) __VA_ARGS__

#define WJR_PP_QUEUE_FRONT(queue) WJR_PP_QUEUE_FRONT_I queue
#define WJR_PP_QUEUE_FRONT_I(x, ...) x

#define WJR_PP_QUEUE_POP_FRONT(queue) WJR_PP_QUEUE_POP_FRONT_I queue
#define WJR_PP_QUEUE_POP_FRONT_I(x, ...) (__VA_ARGS__)

#define WJR_PP_QUEUE_PUSH_FRONT(queue, x) WJR_PP_QUEUE_PUSH_FRONT_I(queue, x)
#define WJR_PP_QUEUE_PUSH_FRONT_I(queue, x) (x, WJR_PP_QUEUE_EXPAND(queue))

#define WJR_PP_QUEUE_PUSH_BACK(queue, x) WJR_PP_QUEUE_PUSH_BACK_I(queue, x)
#define WJR_PP_QUEUE_PUSH_BACK_I(queue, x) (WJR_PP_QUEUE_EXPAND(queue), x)

#define WJR_PP_QUEUE_SIZE(queue) WJR_PP_QUEUE_SIZE_I(queue)
#define WJR_PP_QUEUE_SIZE_I(queue) WJR_PP_ARGS_LEN queue

#endif // ! WJR_PREPROCCESSOR_QUEUE_BASIC_HPP__

#define WJR_PP_QUEUE_CALL(args, ops)                                                     \
    WJR_PP_QUEUE_CALL_N_I(args, ops, WJR_PP_QUEUE_SIZE(args))
#define WJR_PP_QUEUE_CALL_N(args, ops, N) WJR_PP_QUEUE_CALL_N_I(args, ops, WJR_PP_INC(N))

#define WJR_PP_QUEUE_CALL_N_I(args, ops, N) WJR_PP_QUEUE_CALL_N_II(args, ops, N)
#define WJR_PP_QUEUE_CALL_N_II(args, ops, N) WJR_PP_QUEUE_CALL_##N(args, ops)

#define WJR_PP_QUEUE_CALL_GEN(args, ops)                                                 \
    WJR_PP_QUEUE_CALL_GEN_I(WJR_PP_QUEUE_FRONT(ops), WJR_PP_QUEUE_FRONT(args),           \
                            WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(args)))
#define WJR_PP_QUEUE_CALL_GEN_I(op, arg1, arg2) op(arg1, arg2)

#define WJR_PP_QUEUE_CALL_NEW_ARGS_EQ(args, ops) (WJR_PP_QUEUE_CALL_GEN(args, ops))

#define WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)                                         \
    WJR_PP_QUEUE_CALL_NEW_ARGS_NE_I(                                                     \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(args)),                            \
        WJR_PP_QUEUE_CALL_GEN(args, ops))
#define WJR_PP_QUEUE_CALL_NEW_ARGS_NE_I(arg1, arg2) WJR_PP_QUEUE_PUSH_FRONT(arg1, arg2)

#define WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops)                                            \
    WJR_PP_QUEUE_CALL_NEW_ARGS_I(args, ops, WJR_PP_QUEUE_SIZE(args))
#define WJR_PP_QUEUE_CALL_NEW_ARGS_I(args, ops, N)                                       \
    WJR_PP_QUEUE_CALL_NEW_ARGS_II(args, ops, N)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_II(args, ops, N)                                      \
    WJR_PP_QUEUE_CALL_NEW_ARGS_##N(args, ops)

#define WJR_PP_QUEUE_CALL_1(args, ops) args

#define WJR_PP_QUEUE_CALL_2(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops)

#define WJR_PP_QUEUE_CALL_3(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_2(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_4(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_3(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_5(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_4(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_6(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_5(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_7(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_6(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_8(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_7(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_9(args, ops)                                                   \
    WJR_PP_QUEUE_CALL_8(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_10(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_9(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                           \
                        WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_11(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_10(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_12(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_11(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_13(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_12(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_14(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_13(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_15(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_14(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_16(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_15(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_17(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_16(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_18(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_17(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_19(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_18(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_20(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_19(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_21(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_20(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_22(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_21(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_23(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_22(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_24(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_23(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_25(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_24(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_26(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_25(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_27(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_26(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_28(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_27(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_29(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_28(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_30(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_29(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_31(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_30(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_32(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_31(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_33(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_32(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_34(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_33(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_35(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_34(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_36(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_35(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_37(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_36(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_38(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_37(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_39(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_38(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_40(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_39(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_41(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_40(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_42(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_41(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_43(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_42(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_44(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_43(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_45(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_44(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_46(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_45(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_47(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_46(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_48(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_47(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_49(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_48(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_50(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_49(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_51(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_50(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_52(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_51(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_53(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_52(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_54(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_53(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_55(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_54(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_56(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_55(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_57(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_56(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_58(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_57(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_59(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_58(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_60(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_59(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_61(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_60(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_62(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_61(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_63(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_62(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))
#define WJR_PP_QUEUE_CALL_64(args, ops)                                                  \
    WJR_PP_QUEUE_CALL_63(WJR_PP_QUEUE_CALL_NEW_ARGS(args, ops),                          \
                         WJR_PP_QUEUE_POP_FRONT(ops))

#define WJR_PP_QUEUE_CALL_NEW_ARGS_2(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_EQ(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_3(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_4(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_5(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_6(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_7(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_8(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_9(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_10(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_11(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_12(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_13(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_14(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_15(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_16(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_17(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_18(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_19(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_20(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_21(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_22(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_23(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_24(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_25(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_26(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_27(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_28(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_29(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_30(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_31(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_32(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_33(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_34(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_35(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_36(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_37(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_38(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_39(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_40(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_41(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_42(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_43(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_44(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_45(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_46(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_47(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_48(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_49(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_50(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_51(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_52(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_53(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_54(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_55(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_56(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_57(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_58(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_59(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_60(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_61(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_62(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_63(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)
#define WJR_PP_QUEUE_CALL_NEW_ARGS_64(args, ops) WJR_PP_QUEUE_CALL_NEW_ARGS_NE(args, ops)

#endif // ! WJR_PREPROCESSOR_QUEUE_CALL_HPP__

#define WJR_PP_QUEUE_INIT_N(x, N) WJR_PP_QUEUE_INIT_N_I(x, N)
#define WJR_PP_QUEUE_INIT_N_I(x, N) (WJR_PP_REPEAT(x, N))

#define WJR_PP_QUEUE_CALL_N_SAME(args, op, N)                                            \
    WJR_PP_QUEUE_CALL_N(args, WJR_PP_QUEUE_INIT_N(op, N), N)

#define WJR_PP_QUEUE_CALL_SAME(args, op)                                                 \
    WJR_PP_QUEUE_CALL_N_SAME(args, op, WJR_PP_QUEUE_SIZE(args))

// (1,2,3), (f, g, h) -> (f(1), g(2), h(3))
#define WJR_PP_QUEUE_TRANSFORMS(queue, ops)                                              \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME(                  \
        WJR_PP_QUEUE_PUSH_FRONT(queue, WJR_PP_QUEUE_PUSH_BACK(ops, 0)),                  \
        WJR_PP_QUEUE_TRANSFORMS_CALLER, WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_TRANSFORMS_CALLER(x, y)                                             \
    WJR_PP_QUEUE_PUSH_BACK(WJR_PP_QUEUE_POP_FRONT(x), WJR_PP_QUEUE_FRONT(x)(y))

// (1,2,3), f -> (f(1), f(2), f(3))
#define WJR_PP_QUEUE_TRANSFORM(queue, op)                                                \
    WJR_PP_QUEUE_TRANSFORMS(queue, WJR_PP_QUEUE_INIT_N(op, WJR_PP_QUEUE_SIZE(queue)))

// 0, (1, 2, 3), (f, g, h) -> h(g(f(0, 1), 2), 3)
#define WJR_PP_QUEUE_ACCUMULATES(init, queue, ops)                                       \
    WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME(                      \
        WJR_PP_QUEUE_PUSH_FRONT(                                                         \
            queue, WJR_PP_QUEUE_PUSH_BACK(WJR_PP_QUEUE_PUSH_FRONT(ops, init), 0)),       \
        WJR_PP_QUEUE_ACCUMULATES_CALLER, WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_ACCUMULATES_CALLER(x, y)                                            \
    WJR_PP_QUEUE_PUSH_FRONT(                                                             \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_POP_FRONT(x)),                               \
        WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT(x))(WJR_PP_QUEUE_FRONT(x), y))

// 0, (1, 2, 3), f
#define WJR_PP_QUEUE_ACCUMULATE(init, queue, op)                                         \
    WJR_PP_QUEUE_ACCUMULATES(init, queue,                                                \
                             WJR_PP_QUEUE_INIT_N(op, WJR_PP_QUEUE_SIZE(queue)))

// (1, 2, 3) -> 3
#define WJR_PP_QUEUE_BACK(queue)                                                         \
    WJR_PP_QUEUE_ACCUMULATE(0, queue, WJR_PP_QUEUE_BACK_CALLER)

#define WJR_PP_QUEUE_BACK_CALLER(x, y) y

// (1, 2, 3, 4, 5), 2 -> (3, 4, 5)
#define WJR_PP_QUEUE_POP_FRONT_N(queue, N)                                               \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N(                       \
        WJR_PP_QUEUE_PUSH_FRONT(WJR_PP_QUEUE_PUSH_FRONT(queue, holder), (0)),            \
        (WJR_PP_REPEAT(WJR_PP_QUEUE_POP_FRONT_N_HEADER_CALLER, WJR_PP_INC(N)),           \
         WJR_PP_REPEAT(WJR_PP_QUEUE_POP_FRONT_N_TAILER_CALLER,                           \
                       WJR_PP_QUEUE_SIZE(queue))),                                       \
        WJR_PP_INC(WJR_PP_QUEUE_SIZE(queue)))))

#define WJR_PP_QUEUE_POP_FRONT_N_HEADER_CALLER(x, y) x
#define WJR_PP_QUEUE_POP_FRONT_N_TAILER_CALLER(x, y) WJR_PP_QUEUE_PUSH_BACK(x, y)

// (1, 2, 3, 4, 5), 2 -> (1, 2, 3)
#define WJR_PP_QUEUE_POP_BACK_N(queue, N)                                                \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(                                           \
        WJR_PP_QUEUE_CALL(WJR_PP_QUEUE_PUSH_FRONT(queue, (0)),                           \
                          WJR_PP_QUEUE_POP_FRONT_N(                                      \
                              (WJR_PP_REPEAT(WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER,      \
                                             WJR_PP_QUEUE_SIZE(queue)),                  \
                               WJR_PP_REPEAT(WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER, N)), \
                              N))))

#define WJR_PP_QUEUE_POP_BACK_N_HEADER_CALLER(x, y) WJR_PP_QUEUE_PUSH_BACK(x, y)
#define WJR_PP_QUEUE_POP_BACK_N_TAILER_CALLER(x, y) x

#define WJR_PP_QUEUE_POP_BACK(queue) WJR_PP_QUEUE_POP_BACK_N(queue, 1)

#define WJR_PP_QUEUE_AT(queue, N) WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_POP_FRONT_N(queue, N))

#define WJR_PP_QUEUE_REVERSE(queue)                                                      \
    WJR_PP_QUEUE_POP_BACK(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME(                   \
        WJR_PP_QUEUE_PUSH_FRONT(queue, (0)), WJR_PP_QUEUE_REVERSE_CALLER,                \
        WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_REVERSE_CALLER(x, y) WJR_PP_QUEUE_PUSH_FRONT(x, y)

// (a, b, c) -> a b c
#define WJR_PP_QUEUE_PUT(queue) WJR_PP_QUEUE_ACCUMULATE(, queue, WJR_PP_QUEUE_PUT_CALLER)

#define WJR_PP_QUEUE_PUT_CALLER(x, y) x y

// ((A), (B), (C)) -> (A, B, C)
#define WJR_PP_QUEUE_UNWRAP(queue)                                                       \
    WJR_PP_QUEUE_TRANSFORM(queue, WJR_PP_QUEUE_UNWRAP_CALLER)

#define WJR_PP_QUEUE_UNWRAP_CALLER(x) WJR_PP_QUEUE_UNWRAP_CALLER_I x
#define WJR_PP_QUEUE_UNWRAP_CALLER_I(x) x

// ((A), (B), (C)) -> A B C
#define WJR_PP_QUEUE_UNWRAP_PUT(queue)                                                   \
    WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME(                     \
        WJR_PP_QUEUE_PUSH_FRONT(queue, ()), WJR_PP_QUEUE_UNWRAP_PUT_CALLER,              \
        WJR_PP_QUEUE_SIZE(queue))))

#define WJR_PP_QUEUE_UNWRAP_PUT_CALLER(x, y)                                             \
    (WJR_PP_QUEUE_EXPAND(x) WJR_PP_QUEUE_EXPAND(y))

// (A, B, C) (x, y, z) -> ((A, x), (B, y), (C, z))
#define WJR_PP_QUEUE_ZIP_2(queue1, queue2)                                               \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME(                  \
        ((queue1), queue2), WJR_PP_QUEUE_ZIP_2_CALLER, WJR_PP_QUEUE_SIZE(queue1))))

#define WJR_PP_QUEUE_ZIP_2_CALLER(x, y)                                                  \
    WJR_PP_QUEUE_PUSH_FRONT(                                                             \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_PUSH_BACK(                                   \
            x, (WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(x)), WJR_PP_QUEUE_FRONT(y)))),     \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(x))),                                  \
        WJR_PP_QUEUE_POP_FRONT(y)

// ((A), (B), (C)) (x, y, z) -> ((A, x), (B, y), (C, z))
#define WJR_PP_QUEUE_ZIP_MORE(queue1, queue2)                                            \
    WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_CALL_N_SAME(                  \
        ((queue1), queue2), WJR_PP_QUEUE_ZIP_MORE_CALLER, WJR_PP_QUEUE_SIZE(queue1))))

#define WJR_PP_QUEUE_ZIP_MORE_CALLER(x, y)                                               \
    WJR_PP_QUEUE_PUSH_FRONT(                                                             \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_PUSH_BACK(                                   \
            x, (WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_FRONT(WJR_PP_QUEUE_FRONT(x))),          \
                WJR_PP_QUEUE_FRONT(y)))),                                                \
        WJR_PP_QUEUE_POP_FRONT(WJR_PP_QUEUE_FRONT(x))),                                  \
        WJR_PP_QUEUE_POP_FRONT(y)

#endif // WJR_PREPROCESSOR_QUEUE_ALGORITHM_HPP__

// (a, b, c) -> f(a) f(b) f(c)
#define WJR_PP_TRANSFORM_PUT(queue, op)                                                  \
    WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

// (a, b, c) -> (f(a), f(b), f(c)) (note : f(x) = (g(x))) -> g(a) g(b) g(c)
#define WJR_PP_TRANSFORM_UNWRAP_PUT(queue, op)                                           \
    WJR_PP_QUEUE_UNWRAP_PUT(WJR_PP_QUEUE_TRANSFORM(queue, op))

#define WJR_ATTRIBUTES(...) WJR_PP_TRANSFORM_PUT((__VA_ARGS__), WJR_ATTRIBUTES_CALLER)
#define WJR_ATTRIBUTES_CALLER(x) WJR_ATTRIBUTE(x)

#define WJR_PRAGMA_I(expr) _Pragma(#expr)
#if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_MSVC)
#define WJR_PRAGMA(expr) WJR_PRAGMA_I(expr)
#else
#define WJR_PRAGMA(expr)
#endif

#if WJR_HAS_FEATURE(PRAGMA_UNROLL)
#if defined(WJR_COMPILER_GCC)
#define WJR_UNROLL(loop) WJR_PRAGMA(GCC unroll(loop))
#else
#define WJR_UNROLL(loop) WJR_PRAGMA(unroll(loop))
#endif
#else
#define WJR_UNROLL(loop)
#endif

#define WJR_IS_OVERLAP_P(p, pn, q, qn) ((p) + (pn) > (q) && (q) + (qn) > (p))
#define WJR_IS_SEPARATE_P(p, pn, q, qn) (!WJR_IS_OVERLAP_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_SEPARATE_P(p, pn, q, qn)                                          \
    (((p) == (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_INCR_P(p, pn, q, qn)                                              \
    (((p) <= (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_DECR_P(p, pn, q, qn)                                              \
    (((p) >= (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))

#define WJR_ASM_PIC_JMPL(LABEL, TABLE) ".long " #LABEL "-" #TABLE
#define WJR_ASM_NOPIC_JMPL(LABEL) ".quad " #LABEL

#if WJR_HAS_FEATURE(INLINE_ASM_CCCOND)
#define WJR_ASM_CCSET(c) "/* set condition codes */\n\t"
#define WJR_ASM_CCOUT(c) "=@cc" #c
#else
#define WJR_ASM_CCSET(c) "set" #c " %[_cc_" #c "]\n\t"
#define WJR_ASM_CCOUT(c) [_cc_##c] "=r"
#endif

// WJR_EXCEPTIONS_LEVEL : 0 ~ 3
// 0 : Disable exceptions
// 1 ~ 3 : Enable exceptions
#ifndef WJR_EXCEPTIONS_LEVEL
#define WJR_EXCEPTIONS_LEVEL 1 // enable exceptions by default
#endif

#define WJR_EXCEPTIONS_IF(level, expr0, expr1)                                           \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_EXCEPTIONS_LEVEL, level), expr0, expr1)

#define WJR_EXCEPTIONS_EXPR_L(level, expr) WJR_EXCEPTIONS_IF(level, expr, )
#define WJR_EXCEPTIONS_EXPR(expr) WJR_EXCEPTIONS_EXPR_L(0, expr)

#define WJR_ENABLE_EXCEPTIONS_TRY_I try
#define WJR_ENABLE_EXCEPTIONS_CATCH_I(X) catch (X)
#define WJR_ENABLE_EXCEPTIONS_THROW_I(X) throw X

#define WJR_DISABLE_EXCEPTIONS_TRY_I if (true)
#define WJR_DISABLE_EXCEPTIONS_CATCH_I(X) if (false)
#define WJR_DISABLE_EXCEPTIONS_THROW_I(X)

#define WJR_TRY_L(level)                                                                 \
    WJR_EXCEPTIONS_IF(level, WJR_ENABLE_EXCEPTIONS_TRY_I, WJR_DISABLE_EXCEPTIONS_TRY_I)
#define WJR_CATCH_L(level, X)                                                            \
    WJR_EXCEPTIONS_IF(level, WJR_ENABLE_EXCEPTIONS_CATCH_I(X),                           \
                      WJR_DISABLE_EXCEPTIONS_CATCH_I(X))
#define WJR_THROW_L(level, X)                                                            \
    WJR_EXCEPTIONS_IF(level, WJR_ENABLE_EXCEPTIONS_THROW_I(X),                           \
                      WJR_DISABLE_EXCEPTIONS_THROW_I(X))

#define WJR_TRY WJR_TRY_L(0)
#define WJR_CATCH(X) WJR_CATCH_L(0, X)
#define WJR_THROW(X) WJR_THROW_L(0, X)

#define WJR_TRY_L1 WJR_TRY_L(1)
#define WJR_CATCH_L1(X) WJR_CATCH_L(1, X)
#define WJR_THROW_L1(X) WJR_THROW_L(1, X)

#define WJR_TRY_L2 WJR_TRY_L(2)
#define WJR_CATCH_L2(X) WJR_CATCH_L(2, X)
#define WJR_THROW_L2(X) WJR_THROW_L(2, X)

#define WJR_TRY_L3 WJR_TRY_L(3)
#define WJR_CATCH_L3(X) WJR_CATCH_L(3, X)
#define WJR_THROW_L3(X) WJR_THROW_L(3, X)

#endif // ! WJR_PREPROCESSOR_PREVIEW_HPP__

#endif // WJR_PREPROCESSOR_HPP__

namespace wjr {

struct in_place_empty_t {};

inline constexpr in_place_empty_t in_place_empty = {};

struct in_place_default_construct_t {};

inline constexpr in_place_default_construct_t in_place_default_construct = {};

struct in_place_value_construct_t {};

inline constexpr in_place_value_construct_t in_place_value_construct = {};

struct in_place_reallocate_t {};

inline constexpr in_place_reallocate_t in_place_reallocate = {};

struct in_place_max_t {
    template <typename T>
    WJR_CONST constexpr operator T() const {
        return std::numeric_limits<T>::max();
    }
};

inline constexpr in_place_max_t in_place_max = {};

struct in_place_min_t {
    template <typename T>
    WJR_CONST constexpr operator T() const {
        return std::numeric_limits<T>::min();
    }
};

inline constexpr in_place_min_t in_place_min = {};

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

template <size_t n>
struct __uint_selector {};

template <>
struct __uint_selector<8> {
    using type = std::uint8_t;
};

template <>
struct __uint_selector<16> {
    using type = std::uint16_t;
};

template <>
struct __uint_selector<32> {
    using type = std::uint32_t;
};

template <>
struct __uint_selector<64> {
    using type = std::uint64_t;
};

template <size_t n>
struct __int_selector {
    using type = std::make_signed_t<typename __uint_selector<n>::type>;
};

#if WJR_HAS_FEATURE(INT128)
template <>
struct __uint_selector<128> {
    using type = __uint128_t;
};

template <>
struct __int_selector<128> {
    using type = __int128_t;
};
#endif

template <size_t n>
using uint_t = typename __uint_selector<n>::type;

template <size_t n>
using int_t = typename __int_selector<n>::type;

#if WJR_HAS_FEATURE(INT128)
using int128_t = int_t<128>;
#endif

#if WJR_HAS_FEATURE(INT128)
using uint128_t = uint_t<128>;
#endif

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

template <typename T, typename U>
struct __is_nothrow_swappable_with
    : std::bool_constant<noexcept(std::swap(std::declval<T &>(), std::declval<U &>())) &&
                         noexcept(std::swap(std::declval<U &>(), std::declval<T &>()))> {
};

template <typename T, typename U>
struct is_nothrow_swappable_with
    : std::conjunction<is_swappable_with<T, U>, __is_nothrow_swappable_with<T, U>> {};

template <typename T, typename U>
inline constexpr bool is_nothrow_swappable_with_v =
    is_nothrow_swappable_with<T, U>::value;

template <typename T>
struct is_nothrow_swappable : is_nothrow_swappable_with<std::add_lvalue_reference_t<T>,
                                                        std::add_lvalue_reference_t<T>> {
};

template <typename T>
inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

template <typename T>
struct unref_wrapper {
    using type = T;
};

template <typename T>
struct unref_wrapper<std::reference_wrapper<T>> {
    using type = T &;
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

// ....

template <class P, class M>
WJR_INTRINSIC_CONSTEXPR20 size_t container_of_offset(const M P::*member) {
    return reinterpret_cast<size_t>(&(reinterpret_cast<P *>(nullptr)->*member));
}

template <class P, class M>
WJR_INTRINSIC_CONSTEXPR20 P *container_of_offset_impl(M *ptr, const M P::*member) {
    return reinterpret_cast<P *>(reinterpret_cast<char *>(ptr) -
                                 container_of_offset(member));
}

#define WJR_CONTAINER_OF(ptr, type, member) container_of_offset_impl(ptr, &type::member)

// C++ 17 concept adapt

template <typename Derived, typename Base>
struct is_derived_from
    : std::conjunction<
          std::is_base_of<Base, Derived>,
          std::is_convertible<const volatile Derived *, const volatile Base *>> {};

template <typename Derived, typename Base>
inline constexpr bool is_derived_from_v = is_derived_from<Derived, Base>::Value;

template <typename From, typename To, typename = void>
struct __is_convertible_to_helper : std::false_type {};

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

template <typename T>
using iter_reference_t = decltype(*std::declval<T &>());

template <typename iter, typename = void>
struct is_contiguous_iterator_impl
    : std::disjunction<std::is_pointer<iter>, std::is_array<iter>> {};

template <typename iter>
struct is_contiguous_iterator_impl<iter, typename iter::is_contiguous_iterator>
    : std::true_type {};

#if defined(WJR_CPP_20)
template <typename iter>
struct is_contiguous_iterator
    : std::bool_constant<std::contiguous_iterator<iter> ||
                         is_contiguous_iterator_impl<iter>::value> {};
#else
template <typename iter>
struct is_contiguous_iterator : is_contiguous_iterator_impl<iter> {};
#endif

template <typename iter>
inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<iter>::value;

template <typename T, typename = void>
struct __is_iterator_helper : std::false_type {};

template <typename T>
struct __is_iterator_helper<
    T, std::void_t<typename std::iterator_traits<T>::iterator_category>>
    : std::true_type {};

template <typename T>
struct is_iterator : __is_iterator_helper<T> {};

template <typename T>
inline constexpr bool is_iterator_v = is_iterator<T>::value;

// TODO : move __is_in_i32_range to other header.
WJR_INTRINSIC_CONSTEXPR bool __is_in_i32_range(int64_t value) noexcept {
    return value >= (int32_t)in_place_min && value <= (int32_t)in_place_max;
}

#define __WJR_REGISTER_TYPENAMES_EXPAND(x) __WJR_REGISTER_TYPENAMES_EXPAND_I x
#define __WJR_REGISTER_TYPENAMES_EXPAND_I(...) __VA_ARGS__

#define __WJR_REGISTER_TYPENAMES(...)                                                    \
    __WJR_REGISTER_TYPENAMES_EXPAND(                                                     \
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

template <typename Value,
          std::enable_if_t<is_nonbool_integral_v<remove_cvref_t<Value>>, int> = 0>
constexpr decltype(auto) make_signed_value(Value &&value) noexcept {
    return static_cast<std::make_signed_t<remove_cvref_t<Value>>>(
        std::forward<Value>(value));
}

template <typename Value,
          std::enable_if_t<is_nonbool_integral_v<remove_cvref_t<Value>>, int> = 0>
constexpr decltype(auto) make_unsigned_value(Value &&value) noexcept {
    return static_cast<std::make_unsigned_t<remove_cvref_t<Value>>>(
        std::forward<Value>(value));
}

template <typename T>
struct get_integral_constant {
    using type = T;
};

template <typename T, T v>
struct get_integral_constant<std::integral_constant<T, v>> {
    using type = T;
};

template <typename T>
using get_integral_constant_t = typename get_integral_constant<T>::type;

template <typename T>
WJR_INTRINSIC_CONSTEXPR auto get_integral_constant_v(T &&v) noexcept {
    using value_type = get_integral_constant_t<remove_cvref_t<T>>;

    if constexpr (std::is_same_v<value_type, remove_cvref_t<T>>) {
        return std::forward<T>(v);
    } else {
        return static_cast<value_type>(std::forward<T>(v));
    }
}

template <typename T, typename U>
struct is_integral_constant_same : std::false_type {};

template <typename T, T v>
struct is_integral_constant_same<std::integral_constant<T, v>, T> : std::true_type {};

template <typename T, typename U>
inline constexpr bool is_integral_constant_same_v =
    is_integral_constant_same<T, U>::value;

} // namespace wjr

#endif // ! WJR_TYPE_TRAITS_HPP__

namespace wjr {

/**
 * @brief A helper class to compress the size of a pair.
 *
 * @note T is not an empty class.
 */
template <size_t index, typename T>
struct comp_pair_wrapper1 {

    template <typename Ty = T,
              std::enable_if_t<std::is_default_constructible_v<Ty>, int> = 0>
    constexpr comp_pair_wrapper1() noexcept(std::is_nothrow_default_constructible_v<Ty>)
        : val() {}

    template <typename Ty = T,
              std::enable_if_t<std::is_copy_constructible_v<Ty>, int> = 0>
    constexpr explicit comp_pair_wrapper1(const Ty &other) noexcept(
        std::is_nothrow_copy_constructible_v<Ty>)
        : val(other) {}

    template <typename Ty = T,
              std::enable_if_t<std::is_move_constructible_v<Ty>, int> = 0>
    constexpr explicit comp_pair_wrapper1(Ty &&other) noexcept(
        std::is_nothrow_move_constructible_v<Ty>)
        : val(std::forward<Ty>(other)) {}

    template <typename... Args,
              std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
    constexpr comp_pair_wrapper1(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : val(std::forward<Args>(args)...) {}

    constexpr comp_pair_wrapper1(const comp_pair_wrapper1 &) = default;
    constexpr comp_pair_wrapper1(comp_pair_wrapper1 &&) noexcept = default;
    constexpr comp_pair_wrapper1 &operator=(const comp_pair_wrapper1 &) = default;
    constexpr comp_pair_wrapper1 &operator=(comp_pair_wrapper1 &&) noexcept = default;

    constexpr T &value() noexcept { return val; }
    constexpr const T &value() const noexcept { return val; }

private:
    T val;
};

/**
 * @brief A helper class to compress the size of a pair.
 *
 * @note T is an empty class.
 *
 * @tparam index
 * @tparam T
 */
template <size_t index, typename T>
struct comp_pair_wrapper2 : private T {
    using Mybase = T;

    template <typename Ty = T,
              std::enable_if_t<std::is_default_constructible_v<Ty>, int> = 0>
    constexpr comp_pair_wrapper2() noexcept(std::is_nothrow_default_constructible_v<Ty>)
        : Mybase() {}

    template <typename Ty = T,
              std::enable_if_t<std::is_copy_constructible_v<Ty>, int> = 0>
    constexpr explicit comp_pair_wrapper2(const Ty &other) noexcept(
        std::is_nothrow_copy_constructible_v<Ty>)
        : Mybase(other) {}

    template <typename Ty = T,
              std::enable_if_t<std::is_move_constructible_v<Ty>, int> = 0>
    constexpr explicit comp_pair_wrapper2(Ty &&other) noexcept(
        std::is_nothrow_move_constructible_v<Ty>)
        : Mybase(std::forward<Ty>(other)) {}

    template <typename... Args,
              std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
    constexpr comp_pair_wrapper2(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : Mybase(std::forward<Args>(args)...) {}

    constexpr comp_pair_wrapper2(const comp_pair_wrapper2 &) = default;
    constexpr comp_pair_wrapper2(comp_pair_wrapper2 &&) noexcept = default;
    constexpr comp_pair_wrapper2 &operator=(const comp_pair_wrapper2 &) = default;
    constexpr comp_pair_wrapper2 &operator=(comp_pair_wrapper2 &&) noexcept = default;

    constexpr T &value() noexcept { return *this; }
    constexpr const T &value() const noexcept { return *this; }
};

template <typename T>
using comp_pair_wrapper_helper =
    std::conjunction<std::is_class<T>, std::is_empty<T>, std::negation<std::is_final<T>>>;

template <size_t index, typename T, typename U>
using comp_pair_wrapper =
    std::conditional_t<comp_pair_wrapper_helper<T>::value &&
                           (index == 0 || !comp_pair_wrapper_helper<U>::value),
                       comp_pair_wrapper2<index, T>, comp_pair_wrapper1<index, T>>;

/**
 * @class compressed_pair
 * @brief A pair used empty base optimization to reduce the size of the pair.
 *
 * @details When `T` or `U` is an empty class, compressed_pair will use empty base
 * optimization to reduce the size of the pair. Otherwise, compressed_pair
 * is equivalent to `std::pair`.
 */
template <typename T, typename U>
class compressed_pair : private comp_pair_wrapper<0, T, U>,
                        private comp_pair_wrapper<1, U, T> {

    template <typename Ty, typename Uy>
    using __is_all_default_constructible =
        std::conjunction<std::is_default_constructible<Ty>,
                         std::is_default_constructible<Uy>>;

    template <typename Ty, typename Uy>
    using __is_all_default_convertible =
        std::conjunction<is_default_convertible<Ty>, is_default_convertible<Uy>>;

    template <typename Ty, typename Uy>
    using __is_all_copy_constructible =
        std::conjunction<std::is_copy_constructible<Ty>, std::is_copy_constructible<Uy>>;

    template <typename Ty, typename Uy, typename Vty, typename Wuy>
    using __is_all_convertible =
        std::conjunction<std::is_convertible<Vty, Ty>, std::is_convertible<Wuy, Uy>>;

    template <typename Ty, typename Uy, typename Vty, typename Wuy>
    using __is_all_constructible =
        std::conjunction<std::is_constructible<Ty, Vty>, std::is_constructible<Uy, Wuy>>;

public:
    using Mybase1 = comp_pair_wrapper<0, T, U>;
    using Mybase2 = comp_pair_wrapper<1, U, T>;

    using first_type = T;
    using second_type = U;

    template <typename Ty = T, typename Uy = U,
              std::enable_if_t<std::conjunction_v<__is_all_default_constructible<Ty, Uy>,
                                                  __is_all_default_convertible<Ty, Uy>>,
                               bool> = true>
    constexpr compressed_pair() noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<Ty>,
                           std::is_nothrow_default_constructible<Uy>>)
        : Mybase1(), Mybase2() {}

    template <typename Ty = T, typename Uy = U,
              std::enable_if_t<
                  std::conjunction_v<__is_all_default_constructible<Ty, Uy>,
                                     std::negation<__is_all_default_convertible<Ty, Uy>>>,
                  bool> = false>
    constexpr explicit compressed_pair() noexcept(
        std::conjunction_v<std::is_nothrow_default_constructible<Ty>,
                           std::is_nothrow_default_constructible<Uy>>)
        : Mybase1(), Mybase2() {}

    template <typename Ty = T, typename Uy = U,
              std::enable_if_t<std::conjunction_v<
                                   __is_all_copy_constructible<Ty, Uy>,
                                   __is_all_convertible<Ty, Uy, const Ty &, const Uy &>>,
                               bool> = true>
    constexpr compressed_pair(const T &_First, const U &_Second) noexcept(
        std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                           std::is_nothrow_copy_constructible<Uy>>)
        : Mybase1(_First), Mybase2(_Second) {}

    template <typename Ty = T, typename Uy = U,
              std::enable_if_t<std::conjunction_v<__is_all_copy_constructible<Ty, Uy>,
                                                  std::negation<__is_all_convertible<
                                                      Ty, Uy, const Ty &, const Uy &>>>,
                               bool> = false>
    constexpr explicit compressed_pair(const T &_First, const U &_Second) noexcept(
        std::conjunction_v<std::is_nothrow_copy_constructible<Ty>,
                           std::is_nothrow_copy_constructible<Uy>>)
        : Mybase1(_First), Mybase2(_Second) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<std::conjunction_v<__is_all_constructible<T, U, Other1, Other2>,
                                            __is_all_convertible<T, U, Other1, Other2>>,
                         bool> = true>
    constexpr compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Other1>,
                           std::is_nothrow_constructible<U, Other2>>)
        : Mybase1(std::forward<Other1>(_First)), Mybase2(std::forward<Other2>(_Second)) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<
            std::conjunction_v<__is_all_constructible<T, U, Other1, Other2>,
                               std::negation<__is_all_convertible<T, U, Other1, Other2>>>,
            bool> = false>
    constexpr explicit compressed_pair(Other1 &&_First, Other2 &&_Second) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Other1>,
                           std::is_nothrow_constructible<U, Other2>>)
        : Mybase1(std::forward<Other1>(_First)), Mybase2(std::forward<Other2>(_Second)) {}

    constexpr compressed_pair(const compressed_pair &) = default;
    constexpr compressed_pair(compressed_pair &&) noexcept = default;

    template <
        typename Other1, typename Other2,
        std::enable_if_t<std::conjunction_v<
                             __is_all_constructible<T, U, const Other1 &, const Other2 &>,
                             __is_all_convertible<T, U, const Other1 &, const Other2 &>>,
                         bool> = true>
    constexpr compressed_pair(const compressed_pair<Other1, Other2> &other) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, const Other1 &>,
                           std::is_nothrow_constructible<U, const Other2 &>>)
        : Mybase1(other.first()), Mybase2(other.second()) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<std::conjunction_v<
                             __is_all_constructible<T, U, const Other1 &, const Other2 &>,
                             std::negation<__is_all_convertible<T, U, const Other1 &,
                                                                const Other2 &>>>,
                         bool> = false>
    constexpr explicit compressed_pair(
        const compressed_pair<Other1, Other2>
            &other) noexcept(std::
                                 conjunction_v<
                                     std::is_nothrow_constructible<T, const Other1 &>,
                                     std::is_nothrow_constructible<U, const Other2 &>>)
        : Mybase1(other.first()), Mybase2(other.second()) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<std::conjunction_v<__is_all_constructible<T, U, Other1, Other2>,
                                            __is_all_convertible<T, U, Other1, Other2>>,
                         bool> = true>
    constexpr compressed_pair(compressed_pair<Other1, Other2> &&other) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Other1>,
                           std::is_nothrow_constructible<U, Other2>>)
        : Mybase1(std::forward<Other1>(other.first())),
          Mybase2(std::forward<Other2>(other.second())) {}

    template <
        typename Other1, typename Other2,
        std::enable_if_t<
            std::conjunction_v<__is_all_constructible<T, U, Other1, Other2>,
                               std::negation<__is_all_convertible<T, U, Other1, Other2>>>,
            bool> = false>
    constexpr explicit compressed_pair(compressed_pair<Other1, Other2> &&other) noexcept(
        std::conjunction_v<std::is_nothrow_constructible<T, Other1>,
                           std::is_nothrow_constructible<U, Other2>>)
        : Mybase1(std::forward<Other1>(other.first())),
          Mybase2(std::forward<Other2>(other.second())) {}

    template <typename Tuple1, typename Tuple2, size_t... N1, size_t... N2>
    constexpr compressed_pair(Tuple1 &tp1, Tuple2 &tp2, std::index_sequence<N1...>,
                              std::index_sequence<N2...>)
        : Mybase1(std::get<N1>(std::move(tp1))...),
          Mybase2(std::get<N2>(std::move(tp2))...) {}

    template <typename... Args1, typename... Args2>
    constexpr compressed_pair(std::piecewise_construct_t, std::tuple<Args1...> tp1,
                              std::tuple<Args2...> tp2)
        : compressed_pair(tp1, tp2, std::index_sequence_for<Args1...>{},
                          std::index_sequence_for<Args2...>{}) {}

    template <typename Myself = compressed_pair,
              std::enable_if_t<std::conjunction_v<std::is_copy_assignable<T>,
                                                  std::is_copy_assignable<U>>,
                               int> = 0>
    constexpr compressed_pair &operator=(type_identity_t<const Myself &> other) noexcept(
        std::conjunction_v<std::is_nothrow_copy_assignable<T>,
                           std::is_nothrow_copy_assignable<U>>) {
        first() = other.first();
        second() = other.second();
        return *this;
    }

    template <typename Myself = compressed_pair,
              std::enable_if_t<std::conjunction_v<std::is_copy_assignable<T>,
                                                  std::is_copy_assignable<U>>,
                               int> = 0>
    constexpr compressed_pair &operator=(type_identity_t<Myself &&> other) noexcept(
        std::conjunction_v<std::is_nothrow_move_assignable<T>,
                           std::is_nothrow_move_assignable<U>>) {
        first() = std::forward<T>(other.first());
        second() = std::forward<U>(other.second());
        return *this;
    }

    template <typename Other1, typename Other2,
              std::enable_if_t<std::conjunction_v<
                                   std::negation<std::is_same<
                                       compressed_pair, compressed_pair<Other1, Other2>>>,
                                   std::is_assignable<T &, const Other1 &>,
                                   std::is_assignable<U &, const Other2 &>>,
                               int> = 0>
    constexpr compressed_pair &
    operator=(const compressed_pair<Other1, Other2> &other) noexcept(
        std::conjunction_v<std::is_nothrow_assignable<T &, const Other1 &>,
                           std::is_nothrow_assignable<U &, const Other2 &>>) {
        first() = other.first();
        second() = other.second();
        return *this;
    }

    template <typename Other1, typename Other2,
              std::enable_if_t<
                  std::conjunction_v<
                      std::negation<
                          std::is_same<compressed_pair, compressed_pair<Other1, Other2>>>,
                      std::is_assignable<T &, Other1>, std::is_assignable<U &, Other2>>,
                  int> = 0>
    constexpr compressed_pair &
    operator=(compressed_pair<Other1, Other2> &&other) noexcept(
        std::conjunction_v<std::is_nothrow_assignable<T &, Other1>,
                           std::is_nothrow_assignable<U &, Other2>>) {
        first() = std::forward<Other1>(other.first());
        second() = std::forward<Other2>(other.second());
        return *this;
    }

    template <
        typename Myself = compressed_pair,
        std::enable_if_t<std::conjunction_v<is_swappable<T>, is_swappable<U>>, int> = 0>
    constexpr void swap(type_identity_t<compressed_pair &> other) noexcept(
        std::conjunction_v<is_nothrow_swappable<T>, is_nothrow_swappable<U>>) {
        using std::swap;
        swap(first(), other.first());
        swap(second(), other.second());
    }

    constexpr T &first() noexcept { return Mybase1::value(); }
    constexpr const T &first() const noexcept { return Mybase1::value(); }
    constexpr U &second() noexcept { return Mybase2::value(); }
    constexpr const U &second() const noexcept { return Mybase2::value(); }
};

template <typename T, typename U>
compressed_pair(T, U) -> compressed_pair<T, U>;

template <typename T, typename U>
constexpr bool operator==(const compressed_pair<T, U> &lhs,
                          const compressed_pair<T, U> &rhs) {
    return lhs.first() == rhs.first() && lhs.second() == rhs.second();
}

template <typename T, typename U>
constexpr bool operator!=(const compressed_pair<T, U> &lhs,
                          const compressed_pair<T, U> &rhs) {
    return !(lhs == rhs);
}

template <typename T, typename U>
constexpr bool operator<(const compressed_pair<T, U> &lhs,
                         const compressed_pair<T, U> &rhs) {
    return lhs.first() < rhs.first() ||
           (!(rhs.first() < lhs.first()) && lhs.second() < rhs.second());
}

template <typename T, typename U>
constexpr bool operator>(const compressed_pair<T, U> &lhs,
                         const compressed_pair<T, U> &rhs) {
    return rhs < lhs;
}

template <typename T, typename U>
constexpr bool operator<=(const compressed_pair<T, U> &lhs,
                          const compressed_pair<T, U> &rhs) {
    return !(rhs < lhs);
}

template <typename T, typename U>
constexpr bool operator>=(const compressed_pair<T, U> &lhs,
                          const compressed_pair<T, U> &rhs) {
    return !(lhs < rhs);
}

template <typename T, typename U>
constexpr compressed_pair<unref_wrapper_t<T>, unref_wrapper_t<U>>
make_compressed_pair(T &&t, U &&u) noexcept(
    std::conjunction_v<std::is_nothrow_constructible<unref_wrapper_t<T>, T>,
                       std::is_nothrow_constructible<unref_wrapper_t<U>, U>>) {
    return compressed_pair<unref_wrapper_t<T>, unref_wrapper_t<U>>(std::forward<T>(t),
                                                           std::forward<U>(u));
}

} // namespace wjr

namespace std {

template <typename T, typename U,
          std::enable_if_t<std::conjunction_v<wjr::is_swappable<T>, wjr::is_swappable<U>>,
                           int> = 0>
constexpr void swap(wjr::compressed_pair<T, U> &lhs,
                    wjr::compressed_pair<T, U> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr std::tuple_element_t<I, wjr::compressed_pair<T, U>> &
get(wjr::compressed_pair<T, U> &pr) noexcept {
    if constexpr (I == 0) {
        return pr.first();
    } else {
        return pr.second();
    }
}

template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr const std::tuple_element_t<I, wjr::compressed_pair<T, U>> &
get(const wjr::compressed_pair<T, U> &pr) noexcept {
    if constexpr (I == 0) {
        return pr.first();
    } else {
        return pr.second();
    }
}

template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr std::tuple_element_t<I, wjr::compressed_pair<T, U>> &&
get(wjr::compressed_pair<T, U> &&pr) noexcept {
    if constexpr (I == 0) {
        return std::forward<T>(pr.first());
    } else {
        return std::forward<U>(pr.second());
    }
}

template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr const std::tuple_element_t<I, wjr::compressed_pair<T, U>> &&
get(const wjr::compressed_pair<T, U> &&pr) noexcept {
    if constexpr (I == 0) {
        return std::forward<T>(pr.first());
    } else {
        return std::forward<U>(pr.second());
    }
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &get(wjr::compressed_pair<T, U> &pr) noexcept {
    return std::get<0>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &get(const wjr::compressed_pair<T, U> &pr) noexcept {
    return std::get<0>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &&get(wjr::compressed_pair<T, U> &&pr) noexcept {
    return std::get<0>(std::move(pr));
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &&get(const wjr::compressed_pair<T, U> &&pr) noexcept {
    return std::get<0>(std::move(pr));
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &get(wjr::compressed_pair<U, T> &pr) noexcept {
    return std::get<1>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &get(const wjr::compressed_pair<U, T> &pr) noexcept {
    return std::get<1>(pr);
}

template <typename T, typename U>
WJR_NODISCARD constexpr T &&get(wjr::compressed_pair<U, T> &&pr) noexcept {
    return std::get<1>(std::move(pr));
}

template <typename T, typename U>
WJR_NODISCARD constexpr const T &&get(const wjr::compressed_pair<U, T> &&pr) noexcept {
    return std::get<1>(std::move(pr));
}

template <typename T, typename U>
struct tuple_size<wjr::compressed_pair<T, U>> : std::integral_constant<size_t, 2> {};

template <size_t I, typename T, typename U>
struct tuple_element<I, wjr::compressed_pair<T, U>> {
    static_assert(I < 2, "wjr::compressed_pair has only 2 elements!");
};

template <typename T, typename U>
struct tuple_element<0, wjr::compressed_pair<T, U>> {
    using type = T;
};

template <typename T, typename U>
struct tuple_element<1, wjr::compressed_pair<T, U>> {
    using type = U;
};

} // namespace std

#endif // WJR_COMPRESSED_PAIR_HPP__
#ifndef WJR_MATH_HPP__
#define WJR_MATH_HPP__

#ifndef WJR_MATH_BIT_HPP__
#define WJR_MATH_BIT_HPP__

#ifndef WJR_MATH_CLZ_HPP__
#define WJR_MATH_CLZ_HPP__

#ifndef WJR_MATH_POPCOUNT_HPP__
#define WJR_MATH_POPCOUNT_HPP__

#ifndef WJR_MATH_DETAILS_HPP__
#define WJR_MATH_DETAILS_HPP__

#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <algorithm>

// Already included
#ifndef WJR_CRTP_NONCOPYABLE_HPP__
#define WJR_CRTP_NONCOPYABLE_HPP__

namespace wjr {

/**
 * @brief A type to disable copying the object.
 *
 */
class noncopyable {
protected:
    noncopyable() = default;
    noncopyable(const noncopyable &) = delete;
    noncopyable(noncopyable &&) = default;
    noncopyable &operator=(const noncopyable &) = delete;
    noncopyable &operator=(noncopyable &&) = default;
    ~noncopyable() = default;
};

} // namespace wjr

#endif // WJR_CRTP_NONCOPYABLE_HPP__
#ifndef WJR_CRTP_NONSENDABLE_HPP__
#define WJR_CRTP_NONSENDABLE_HPP__

#include <memory>
#include <thread>
#include <type_traits>

#ifndef WJR_ASSERT_HPP__
#define WJR_ASSERT_HPP__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

// Already included

namespace wjr {

// ASSERT_LEVEL : 0 ~ 3
// 0 : Release (defined(NDEBUG) && ! defined(WJR_DEBUG_LEVEL))
// 1 : Some simple runtime checks, such as boundary checks (default)
// 2 : Most runtime checks
// 3 : Maximize runtime checks
#ifndef WJR_DEBUG_LEVEL
#if defined(NDEBUG)
#define WJR_DEBUG_LEVEL 0
#else
#define WJR_DEBUG_LEVEL 1
#endif
#endif

// use WJR_THROW instead of std::abort
#ifdef WJR_ASSERT_THROW
#define WJR_ASSERT_NORETURN
#define WJR_ASSERT_ABORT() WJR_THROW(std::runtime_error("assertion failed"))
#else
#define WJR_ASSERT_NORETURN WJR_NORETURN
#define WJR_ASSERT_ABORT() std::abort()
#endif

#define WJR_DEBUG_IF(level, expr0, expr1)                                                \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_DEBUG_LEVEL, level), expr0, expr1)

#define WJR_DEBUG_EXPR_L(level, expr) WJR_DEBUG_IF(level, expr, )
#define WJR_DEBUG_EXPR(expr) WJR_DEBUG_EXPR_L(0, expr)

template <typename... Args>
struct assert_format {
    assert_format(const char *const fmt, std::tuple<Args...> &&args)
        : m_fmt(fmt), m_args(std::move(args)) {}

    const char *const m_fmt;
    std::tuple<Args...> m_args;
};

template <typename... Args>
assert_format(const char *const, std::tuple<Args...> &&) -> assert_format<Args...>;

#define WJR_ASSERT_FORMAT(fmt, ...)                                                      \
    ::wjr::assert_format(fmt, std::make_tuple(__VA_ARGS__))

template <typename T>
struct __is_assert_format : std::false_type {};

template <typename... Args>
struct __is_assert_format<assert_format<Args...>> : std::true_type {};

class __assert_handler_t {
private:
    template <typename... Args>
    void __handler_format(const assert_format<Args...> &fmt) const {
        const char *const fmt_str = fmt.m_fmt;
        std::apply(
            [fmt_str](auto &&...args) {
                (void)fprintf(stderr, fmt_str, std::forward<decltype(args)>(args)...);
            },
            fmt.m_args);
    }

    template <typename T>
    void __handler(T &&t) const {
        if constexpr (__is_assert_format<std::decay_t<T>>::value) {
            __handler_format(std::forward<T>(t));
        } else {
            std::cerr << t;
        }
    }

    void handler() const {}

    template <typename... Args>
    void handler(Args &&...args) const {
        (void)fprintf(stderr, "Additional information:\n");
        (void)((__handler(std::forward<Args>(args)), ...));
        (void)fprintf(stderr, "\n");
    }

public:
    template <typename... Args>
    WJR_ASSERT_NORETURN WJR_NOINLINE void operator()(const char *expr, const char *file,
                                                     int line, Args &&...args) const {
        (void)fprintf(stderr, "Assertion failed: %s", expr);
        if ((file != nullptr) && (file[0] != '\0')) {
            (void)fprintf(stderr, ", file %s", file);
        }
        if (line != -1) {
            (void)fprintf(stderr, ", line %d", line);
        }
        (void)fprintf(stderr, "\n");
        handler(std::forward<Args>(args)...);
        WJR_ASSERT_ABORT();
    }
};

inline constexpr __assert_handler_t __assert_handler{};

#define WJR_ASSERT_NOMESSAGE_FAIL(handler, exprstr) handler(exprstr, WJR_FILE, WJR_LINE)
#define WJR_ASSERT_MESSAGE_FAIL(handler, exprstr, ...)                                   \
    handler(exprstr, WJR_FILE, WJR_LINE, __VA_ARGS__)

#define WJR_ASSERT_CHECK_I_NOMESSAGE(handler, expr)                                      \
    do {                                                                                 \
        if (WJR_UNLIKELY(!(expr))) {                                                     \
            WJR_ASSERT_NOMESSAGE_FAIL(handler, #expr);                                   \
        }                                                                                \
    } while (0)
#define WJR_ASSERT_CHECK_I_MESSAGE(handler, expr, ...)                                   \
    do {                                                                                 \
        if (WJR_UNLIKELY(!(expr))) {                                                     \
            WJR_ASSERT_MESSAGE_FAIL(handler, #expr, __VA_ARGS__);                        \
        }                                                                                \
    } while (0)

#define WJR_ASSERT_CHECK_I(...)                                                          \
    WJR_ASSERT_CHECK_I_N(WJR_PP_ARGS_LEN(__VA_ARGS__), __VA_ARGS__)
#define WJR_ASSERT_CHECK_I_N(N, ...)                                                     \
    WJR_PP_BOOL_IF(WJR_PP_EQ(N, 1), WJR_ASSERT_CHECK_I_NOMESSAGE,                        \
                   WJR_ASSERT_CHECK_I_MESSAGE)                                           \
    (::wjr::__assert_handler, __VA_ARGS__)

// do nothing
#define WJR_ASSERT_UNCHECK_I(expr, ...)

// level = [0, 2]
// The higher the level, the less likely it is to be detected
// Runtime detect  : 1
// Maximize detect : 2
#define WJR_ASSERT_L(level, ...)                                                         \
    WJR_DEBUG_IF(level, WJR_ASSERT_CHECK_I, WJR_ASSERT_UNCHECK_I)                        \
    (__VA_ARGS__)

// level of assert is zero at default.
#define WJR_ASSERT(...) WJR_ASSERT_L(0, __VA_ARGS__)
#define WJR_ASSERT_L1(...) WJR_ASSERT_L(1, __VA_ARGS__)
#define WJR_ASSERT_L2(...) WJR_ASSERT_L(2, __VA_ARGS__)
#define WJR_ASSERT_L3(...) WJR_ASSERT_L(3, __VA_ARGS__)

#define WJR_ASSERT_ASSUME_L(level, ...)                                                  \
    WJR_ASSERT_L(level, __VA_ARGS__);                                                    \
    __WJR_ASSERT_ASSUME_L_ASSUME(__VA_ARGS__)
#define __WJR_ASSERT_ASSUME_L_ASSUME(expr, ...) WJR_ASSUME(expr)

#define WJR_ASSERT_ASSUME(...) WJR_ASSERT_ASSUME_L(0, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L1(...) WJR_ASSERT_ASSUME_L(1, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L2(...) WJR_ASSERT_ASSUME_L(2, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L3(...) WJR_ASSERT_ASSUME_L(3, __VA_ARGS__)

} // namespace wjr

#endif // WJR_ASSERT_HPP__

namespace wjr {

/**
 * @brief Disable sending the object to another thread and check the thread id.
 *
 * @note Only check if WJR_DEBUG_LEVEL > 2.
 */
class __debug_nonsendable {
protected:
    __debug_nonsendable() : m_thread_id(std::this_thread::get_id()) {}
    __debug_nonsendable(const __debug_nonsendable &) = default;
    __debug_nonsendable(__debug_nonsendable &&) = default;
    __debug_nonsendable &operator=(const __debug_nonsendable &) = default;
    __debug_nonsendable &operator=(__debug_nonsendable &&) = default;
    ~__debug_nonsendable() { check(); }

    void check() const {
        WJR_ASSERT_L2(m_thread_id == std::this_thread::get_id(),
                      "Cross-thread access detected.");
    }

    friend bool operator==(const __debug_nonsendable &lhs,
                           const __debug_nonsendable &rhs) {
        return lhs.m_thread_id == rhs.m_thread_id;
    }

    friend bool operator!=(const __debug_nonsendable &lhs,
                           const __debug_nonsendable &rhs) {
        return lhs.m_thread_id != rhs.m_thread_id;
    }

private:
    std::thread::id m_thread_id;
};

/**
 * @brief Disable sending the object to another thread without checking.
 *
 */
class __release_nonsendable {
protected:
    void check() const {};

    friend bool operator==(const __release_nonsendable &, const __release_nonsendable &) {
        return true;
    }

    friend bool operator!=(const __release_nonsendable &, const __release_nonsendable &) {
        return false;
    }
};

/**
 * @brief A type to disable sending the object to another thread.
 *
 * @note By default, only check if object is destroyed and WJR_DEBUG_LEVEL > 2.
 * Use nonsendable::check() to manually check.
 *
 */
using nonsendable = WJR_DEBUG_IF(2, __debug_nonsendable, __release_nonsendable);

template <typename T>
struct is_nonsendable : std::is_base_of<nonsendable, T> {};

template <typename T>
inline constexpr bool is_nonsendable_v = is_nonsendable<T>::value;

template <typename T>
struct is_sendable : std::negation<is_nonsendable<T>> {};

template <typename T>
inline constexpr bool is_sendable_v = is_sendable<T>::value;

} // namespace wjr

#endif // WJR_CRTP_NONSENDABLE_HPP__
#ifndef WJR_CRTP_TRIVIAL_ALLOCATOR_BASE_HPP__
#define WJR_CRTP_TRIVIAL_ALLOCATOR_BASE_HPP__

#include <memory>
#include <type_traits>

namespace wjr {

struct trivial_allocator_base {};

template <typename T>
struct is_trivially_allocator : std::is_base_of<trivial_allocator_base, T> {};

template <typename T>
struct is_trivially_allocator<std::allocator<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_trivially_allocator_v = is_trivially_allocator<T>::value;

} // namespace wjr

#endif // WJR_CRTP_TRIVIAL_ALLOCATOR_BASE_HPP__
// Already included

namespace wjr {

template <size_t threshold, size_t cache>
class stack_allocator_object : noncopyable {
    static_assert(threshold <= cache, "threshold must be less than or equal to cache.");

    constexpr static uint16_t bufsize = 5;

    struct alloc_node {
        char *ptr;
        char *end;
    };

    struct large_stack_top {
        large_stack_top *prev;
        char buffer[];
    };

public:
    struct stack_top {
        char *ptr = nullptr;
        char *end;
        large_stack_top *large;
        uint16_t idx;
    };

private:
    WJR_NOINLINE WJR_CONSTEXPR20 void __small_reallocate(stack_top &top) {
        if (WJR_UNLIKELY(top.end == nullptr)) {
            top.end = m_cache.end;
            top.idx = m_idx;
        }

        ++m_idx;
        if (WJR_UNLIKELY(m_idx == m_size)) {

            if (WJR_UNLIKELY(m_size == m_capacity)) {
                uint16_t new_capacity = m_idx + 2 * (bufsize - 1);
                auto new_ptr =
                    static_cast<alloc_node *>(malloc(new_capacity * sizeof(alloc_node)));
                if (WJR_LIKELY(m_idx != 0)) {
                    std::copy_n(m_ptr, m_idx, new_ptr);
                    free(m_ptr);
                }
                m_ptr = new_ptr;
                m_capacity = new_capacity;
            }

            ++m_size;

            size_t capacity = cache << ((3 * m_idx + 2) / 5);
            auto buffer = static_cast<char *>(malloc(capacity));
            alloc_node node = {buffer, buffer + capacity};
            m_ptr[m_idx] = node;

            if (WJR_UNLIKELY(m_idx == 0)) {
                top.ptr = node.ptr;
                top.end = node.end;
                top.idx = 0;
            }

            m_cache = node;
        } else {
            m_cache = m_ptr[m_idx];
        }

        WJR_ASSERT(top.ptr != nullptr);
        WJR_ASSERT(top.end != nullptr);
    }

    WJR_CONSTEXPR20 void *__small_allocate(size_t n, stack_top &top) {
        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            __small_reallocate(top);
            WJR_ASSERT_ASSUME(top.end != nullptr);
        }

        WJR_ASSERT_ASSUME(m_cache.ptr != nullptr);
        WJR_ASSERT_ASSUME(top.ptr != nullptr);

        auto ptr = m_cache.ptr;
        m_cache.ptr += n;
        return ptr;
    }

    WJR_COLD WJR_CONSTEXPR20 void __small_redeallocate() {
        uint16_t new_size = m_idx + bufsize - 1;

        for (uint16_t i = new_size; i < m_size; ++i) {
            free(m_ptr[i].ptr);
        }

        m_size = new_size;
    }

    WJR_CONSTEXPR20 void __small_deallocate_change_idx(const stack_top &top) {
        uint16_t idx = top.idx;
        m_cache = {top.ptr, top.end};
        m_idx = idx;
        if (WJR_UNLIKELY(m_size - idx >= bufsize)) {
            __small_redeallocate();
        }
    }

    WJR_CONSTEXPR20 void __small_deallocate(const stack_top &top) {
        if (WJR_UNLIKELY(top.ptr == invalid)) {
            return;
        }

        if (WJR_LIKELY(top.end == nullptr)) {
            m_cache.ptr = top.ptr;
        } else {
            __small_deallocate_change_idx(top);
        }
    }

    WJR_CONSTEXPR20 void *__large_allocate(size_t n, stack_top &top) {
        auto buffer = (large_stack_top *)malloc(sizeof(large_stack_top) + n);
        buffer->prev = top.large;
        top.large = buffer;
        return buffer->buffer;
    }

public:
    using value_type = void;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    stack_allocator_object() = default;
    stack_allocator_object(stack_allocator_object &&) = default;
    stack_allocator_object &operator=(stack_allocator_object &&) = default;
    ~stack_allocator_object() {
        for (uint16_t i = 0; i < m_size; ++i) {
            free(m_ptr[i].ptr);
        }

        free(m_ptr);
    }

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n, stack_top &top) {
        if (WJR_UNLIKELY(n >= threshold)) {
            return __large_allocate(n, top);
        }

        return __small_allocate(n, top);
    }

    WJR_CONSTEXPR20 void deallocate(const stack_top &top) {
        __small_deallocate(top);

        auto buffer = top.large;
        while (WJR_UNLIKELY(buffer != nullptr)) {
            auto prev = buffer->prev;
            free(buffer);
            buffer = prev;
        }
    }

    WJR_CONSTEXPR20 void set(stack_top &top) {
        top.ptr = m_cache.ptr;
        top.end = nullptr;
        top.large = nullptr;
    }

private:
    static char *const invalid;

    alloc_node m_cache = {invalid, invalid};
    uint16_t m_idx = in_place_max;
    alignas(32) uint16_t m_size = 0;
    uint16_t m_capacity = 0;
    alloc_node *m_ptr = nullptr;
};

template <size_t threshold, size_t cache>
char *const stack_allocator_object<threshold, cache>::invalid = (char *)(0x0c);

template <typename StackAllocator>
class singleton_stack_allocator_adapter {
public:
    using value_type = typename StackAllocator::value_type;
    using size_type = typename StackAllocator::size_type;
    using difference_type = typename StackAllocator::difference_type;
    using propagate_on_container_move_assignment =
        typename StackAllocator::propagate_on_container_move_assignment;
    using allocator_type = StackAllocator;
    using stack_top = typename StackAllocator::stack_top;

    static StackAllocator &get_instance() {
        static thread_local StackAllocator instance;
        return instance;
    }
};

template <typename Alloc>
constexpr bool operator==(const singleton_stack_allocator_adapter<Alloc> &,
                          const singleton_stack_allocator_adapter<Alloc> &) {
    return true;
}

template <typename Alloc>
constexpr bool operator!=(const singleton_stack_allocator_adapter<Alloc> &,
                          const singleton_stack_allocator_adapter<Alloc> &) {
    return false;
}

/**
 * @brief A stack allocator for fast simulation of stack memory on the heap, singleton
 * mode.
 *
 * @details When allocating memory less than threadshold,
 * use pre-allocated heap memory, otherwise use malloc to allocate heap memory. \n
 * Notice that the pre-allocated heap memory is not released until the program exits. \n
 * This allocator is not thread-safe and can't be used in container.
 *
 * @tparam threshold The threshold for using malloc to allocate heap memory
 * @tparam cache The size of the first heap memory allocation
 */
template <size_t threshold, size_t cache>
using singleton_stack_allocator_object =
    singleton_stack_allocator_adapter<stack_allocator_object<threshold, cache>>;

template <typename StackAllocator>
class unique_stack_allocator;

/**
 * @brief A unique stack allocator for fast simulation of stack memory on the heap.
 *
 * @details When a unique_stack_allocator object is destroyed, all the memory it allocates
 * is released.\n And a new unique_stack_allocator constructed in the lifetime of a
 * unique_stack_allocator object must be destroyed in the current lifetime.
 *
 */
template <size_t threshold, size_t cache>
class unique_stack_allocator<singleton_stack_allocator_object<threshold, cache>>
    : nonsendable {
    using StackAllocatorObject = singleton_stack_allocator_object<threshold, cache>;
    using stack_top = typename StackAllocatorObject::stack_top;
    using allocator_type = typename StackAllocatorObject::allocator_type;

public:
    unique_stack_allocator(const StackAllocatorObject &al) : m_obj(&al) {}
    ~unique_stack_allocator() {
        if (m_top.ptr != nullptr) {
            m_instance->deallocate(m_top);
        }
    }

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n) {
        nonsendable::check();

        if (WJR_UNLIKELY(m_top.ptr == nullptr)) {
            m_instance = &m_obj->get_instance();
            m_instance->set(m_top);
            WJR_ASSERT_ASSUME(m_top.ptr != nullptr);
        }

        return m_instance->allocate(n, m_top);
    }

private:
    union {
        const StackAllocatorObject *m_obj;
        allocator_type *m_instance;
    };

    stack_top m_top;
};

template <typename StackAllocator>
unique_stack_allocator(const StackAllocator &) -> unique_stack_allocator<StackAllocator>;

template <typename T, typename StackAllocator>
class weak_stack_allocator;

template <typename T, size_t threshold, size_t cache>
class weak_stack_allocator<T, singleton_stack_allocator_object<threshold, cache>>
    : trivial_allocator_base {
    using StackAllocator = singleton_stack_allocator_object<threshold, cache>;
    using UniqueStackAllocator = unique_stack_allocator<StackAllocator>;

public:
    using value_type = T;
    using size_type = typename StackAllocator::size_type;
    using difference_type = typename StackAllocator::difference_type;
    using propagate_on_container_move_assignment =
        typename StackAllocator::propagate_on_container_move_assignment;

    weak_stack_allocator() = default;
    weak_stack_allocator(UniqueStackAllocator &alloc) : m_alloc(&alloc) {}
    weak_stack_allocator(const weak_stack_allocator &) = default;
    weak_stack_allocator &operator=(const weak_stack_allocator &) = default;
    weak_stack_allocator(weak_stack_allocator &&) = default;
    weak_stack_allocator &operator=(weak_stack_allocator &&) = default;
    ~weak_stack_allocator() = default;

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 T *allocate(size_type n) {
        return static_cast<T *>(m_alloc->allocate(n * sizeof(T)));
    }

    WJR_CONSTEXPR20 void deallocate(WJR_MAYBE_UNUSED T *ptr,
                                    WJR_MAYBE_UNUSED size_type n) {}

private:
    UniqueStackAllocator *m_alloc = nullptr;
};

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_DETAILS_HPP__
#define WJR_X86_MATH_DETAILS_HPP__

#include <cstdint>

// Already included

namespace wjr {

} // namespace wjr

#endif // WJR_X86_MATH_DETAILS_HPP__
#endif

namespace wjr {

namespace math_details {

template <typename T, T seed>
class de_bruijn {
public:
    constexpr static uint8_t digits = std::numeric_limits<T>::digits;
    constexpr static uint8_t mv = digits == 32 ? 27 : 58;
    constexpr de_bruijn() : lookup(), lookupr() { initialize(); }

    WJR_INTRINSIC_CONSTEXPR int get(T idx) const { return lookup[(idx * seed) >> mv]; }
    WJR_INTRINSIC_CONSTEXPR int getr(T idx) const { return lookupr[(idx * seed) >> mv]; }

private:
    constexpr void initialize() {
        for (uint8_t i = 0; i < digits; ++i) {
            auto idx = (seed << i) >> mv;
            lookup[idx] = i;
            lookupr[idx] = i == 0 ? 0 : digits - i;
        }
    }

    uint8_t lookup[digits];
    uint8_t lookupr[digits];
};

inline constexpr de_bruijn<uint32_t, 0x077C'B531> de_bruijn32 = {};
inline constexpr de_bruijn<uint64_t, 0x03f7'9d71'b4ca'8b09> de_bruijn64 = {};

using stack_alloc_object = singleton_stack_allocator_object<16 * 1024, 36 * 1024>;
using unique_stack_alloc = unique_stack_allocator<stack_alloc_object>;
template <typename T>
using weak_stack_alloc = weak_stack_allocator<T, stack_alloc_object>;
inline constexpr stack_alloc_object stack_alloc = {};

} // namespace math_details

// preview ...

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool is_zero_or_single_bit(T n) noexcept {
    return (n & (n - 1)) == 0;
}

/**
 * @brief
 *
 * @note `n & -n` is the lowest bit of n.
 */
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T lowbit(T n) noexcept {
    return n & -n;
}

// preview :

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool __has_high_bit(T n) noexcept {
    return n >> (std::numeric_limits<T>::digits - 1);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T __align_down(T n, type_identity_t<T> alignment) {
    WJR_ASSERT_ASSUME_L1(is_zero_or_single_bit(alignment));
    return n & (-alignment);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T __align_up(T n, type_identity_t<T> alignment) {
    WJR_ASSERT_ASSUME_L1(is_zero_or_single_bit(alignment));
    return (n + alignment - 1) & (-alignment);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T __align_up_offset(T n, type_identity_t<T> alignment) {
    WJR_ASSERT_ASSUME_L1(is_zero_or_single_bit(alignment));
    return (-n) & (alignment - 1);
}

template <typename T, typename U = std::make_unsigned_t<T>,
          std::enable_if_t<std::is_integral_v<T>, int> = 0>
WJR_CONST constexpr U __fasts_sign_mask() {
    return (U)(1) << (std::numeric_limits<U>::digits - 1);
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr T __fasts_get_sign_mask(T x) {
    return x & __fasts_sign_mask<T>();
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr bool __fasts_is_negative(T x) {
    return __fasts_get_sign_mask<T>(x) != 0;
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr bool __fasts_is_positive(T x) {
    return __fasts_get_sign_mask<T>(x) == 0;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST constexpr std::make_signed_t<T> __fasts_from_unsigned(T x) {
    WJR_ASSERT_ASSUME_L1(!(x & __fasts_sign_mask<T>()));
    std::make_signed_t<T> ret = x;
    WJR_ASSERT_ASSUME_L1(__fasts_is_positive(ret));
    return ret;
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr T __fasts_abs(T x) {
    return x & ~__fasts_sign_mask<T>();
}

template <typename T, std::enable_if_t<is_signed_integral_v<T>, int> = 0>
WJR_CONST constexpr T __fasts_negate(T x) {
    return x ^ __fasts_sign_mask<T>();
}

template <typename T, typename U = std::make_unsigned_t<T>,
          std::enable_if_t<std::is_integral_v<T>, int> = 0>
WJR_CONST constexpr U __fasts_conditional_negate(bool condition, T x) {
    return (U)x ^ ((U)(condition) << (std::numeric_limits<U>::digits - 1));
}

} // namespace wjr

#endif // WJR_MATH_DETAILS_HPP__

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR int fallback_popcount(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    if constexpr (nd < 32) {
        return fallback_popcount(static_cast<uint32_t>(x));
    } else {
        if constexpr (nd == 32) {
            x -= (x >> 1) & 0x5555'5555;
            x = (x & 0x3333'3333) + ((x >> 2) & 0x3333'3333);
            x = (x + (x >> 4)) & 0x0f0f'0f0f;
            return (x * 0x0101'0101) >> 24;
        } else {
            x -= (x >> 1) & 0x5555'5555'5555'5555;
            x = (x & 0x3333'3333'3333'3333) + ((x >> 2) & 0x3333'3333'3333'3333);
            x = (x + (x >> 4)) & 0x0f0f'0f0f'0f0f'0f0f;
            return (x * 0x0101'0101'0101'0101) >> 56;
        }
    }
}

#if WJR_HAS_BUILTIN(__builtin_popcount)
#define WJR_HAS_BUILTIN_POPCOUNT WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(POPCOUNT)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_popcount(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    if constexpr (nd < 32) {
        return builtin_popcount(static_cast<uint32_t>(x));
    } else {
        if constexpr (nd <= std::numeric_limits<unsigned int>::digits) {
            return __builtin_popcount(x);
        } else if constexpr (nd <= std::numeric_limits<unsigned long>::digits) {
            return __builtin_popcountl(x);
        }
        if constexpr (nd <= std::numeric_limits<unsigned long long>::digits) {
            return __builtin_popcountll(x);
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
}

#endif // WJR_HAS_BUILTIN(POPCOUNT)

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int popcount_impl(T x) {
    if (WJR_BUILTIN_CONSTANT_P(is_zero_or_single_bit(x)) && is_zero_or_single_bit(x)) {
        return x != 0;
    }

#if WJR_HAS_BUILTIN(POPCOUNT)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_popcount(x);
    }

    return builtin_popcount(x);
#else
    return fallback_popcount(x);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int popcount(T x) {
    int ret = popcount_impl(x);
    WJR_ASSUME(0 <= ret && ret <= std::numeric_limits<T>::digits);
    return ret;
}

} // namespace wjr

#endif // WJR_MATH_POPCOUNT_HPP__

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int fallback_clz_impl(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#if !(WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT))
    if constexpr (nd >= 32) {
#endif
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);

        if constexpr (nd >= 16) {
            x |= (x >> 8);
        }

        if constexpr (nd >= 32) {
            x |= (x >> 16);
        }

        if constexpr (nd >= 64) {
            x |= (x >> 32);
        }
#if !(WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT))
    }
#endif

#if WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT)
    return popcount<T>(~x);
#else
    if constexpr (nd < 32) {
        return fallback_clz_impl(static_cast<uint32_t>(x)) - (32 - nd);
    } else {
        ++x;

        if constexpr (nd <= 32) {
            return math_details::de_bruijn32.getr(x);
        } else if constexpr (nd <= 64) {
            return math_details::de_bruijn64.getr(x);
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
#endif
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int fallback_clz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

    return fallback_clz_impl(x);
}

#if WJR_HAS_BUILTIN(__builtin_clz)
#define WJR_HAS_BUILTIN_CLZ WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(CLZ)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_clz_impl(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return builtin_clz_impl(static_cast<uint32_t>(x)) - (32 - nd);
    } else {
        if constexpr (nd <= std::numeric_limits<unsigned int>::digits) {
            constexpr auto delta = std::numeric_limits<unsigned int>::digits - nd;
            return __builtin_clz(static_cast<unsigned int>(x)) - delta;
        } else if constexpr (nd <= std::numeric_limits<unsigned long>::digits) {
            constexpr auto delta = std::numeric_limits<unsigned long>::digits - nd;
            return __builtin_clzl(static_cast<unsigned long>(x)) - delta;
        } else if constexpr (nd <= std::numeric_limits<unsigned long long>::digits) {
            constexpr auto delta = std::numeric_limits<unsigned long long>::digits - nd;
            return __builtin_clzll(static_cast<unsigned long long>(x)) - delta;
        } else {
            static_assert(nd <= 64, "not supported yet");
        }
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_clz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

    return builtin_clz_impl(x);
}

#endif

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int clz_impl(T x) {
#if WJR_HAS_BUILTIN(CLZ)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_clz(x);
    }

    return builtin_clz(x);
#else
    return fallback_clz(x);
#endif
}

/**
 * @brief Fast count leading zeros
 *
 * @tparam T Must be an unsigned integral type
 */
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int clz(T x) {
    int ret = clz_impl(x);
    WJR_ASSUME(0 <= ret && ret <= std::numeric_limits<T>::digits);
    return ret;
}

} // namespace wjr

#endif // WJR_MATH_CLZ_HPP__
#ifndef WJR_MATH_CTZ_HPP__
#define WJR_MATH_CTZ_HPP__

// Already included

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int fallback_ctz_impl(T x) {
#if WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT)
    return popcount<T>(lowbit(x) - 1);
#else
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return fallback_ctz_impl(static_cast<uint32_t>(x));
    } else {
        x = lowbit(x);

        if constexpr (nd <= 32) {
            return math_details::de_bruijn32.get(x);
        } else if constexpr (nd <= 64) {
            return math_details::de_bruijn64.get(x);
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
#endif //
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int fallback_ctz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

    return fallback_ctz_impl(x);
}

#if WJR_HAS_BUILTIN(__builtin_ctz)
#define WJR_HAS_BUILTIN_CTZ WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(CTZ)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_ctz_impl(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return builtin_ctz_impl(static_cast<uint32_t>(x));
    } else {
        if constexpr (nd <= std::numeric_limits<unsigned int>::digits) {
            return __builtin_ctz(static_cast<unsigned int>(x));
        } else if constexpr (nd <= std::numeric_limits<unsigned long>::digits) {
            return __builtin_ctzl(static_cast<unsigned long>(x));
        } else if constexpr (nd <= std::numeric_limits<unsigned long long>::digits) {
            return __builtin_ctzll(static_cast<unsigned long long>(x));
        } else {
            static_assert(nd <= 64, "not supported yet");
        }
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_ctz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

    return builtin_ctz_impl(x);
}

#endif

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int ctz_impl(T x) {
#if WJR_HAS_BUILTIN(CTZ)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_ctz(x);
    }

    return builtin_ctz(x);
#else
    return fallback_ctz(x);
#endif
}

/**
 * @brief Fast count trailing zeros
 *
 * @details Very fast even on non-optimized platforms by using a De Bruijn sequence. \n
 * Try __builtin_clz if available, otherwise fallback to a portable implementation. \n
 * In fallback_clz, use popcount and lowbit if POPCOUNT and POPCNT are available, make
 * sure popcount is fast. \n
 * Then use De Bruijn sequence, just a bit slower than popcount + lowbit.
 *
 * @tparam T Must be an unsigned integral type
 */
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int ctz(T x) {
    int ret = ctz_impl(x);
    WJR_ASSUME(0 <= ret && ret <= std::numeric_limits<T>::digits);
    return ret;
}

} // namespace wjr

#endif // WJR_MATH_CTZ_HPP__
#ifndef WJR_MEMORY_TO_ADDRESS_HPP__
#define WJR_MEMORY_TO_ADDRESS_HPP__

#include <cstring>

// Already included

namespace wjr {

namespace to_address_details {

template <typename Enable, typename Ptr, typename... Args>
struct __has_to_address : std::false_type {};
template <typename Ptr, typename... Args>
struct __has_to_address<
    std::void_t<decltype(typename std::pointer_traits<Ptr>::to_address(
        std::declval<const Ptr &>()))>,
    Ptr, Args...> : std::true_type {};
template <typename Ptr, typename... Args>
struct has_to_address : __has_to_address<void, Ptr, Args...> {};
template <typename Ptr, typename... Args>
constexpr bool has_to_address_v = has_to_address<Ptr, Args...>::value;

} // namespace to_address_details

template <typename T>
constexpr T *to_address(T *p) noexcept {
    static_assert(!std::is_function_v<T>, "T cannot be a function.");
    return p;
}

template <typename Ptr>
constexpr auto to_address(const Ptr &p) noexcept {
    if constexpr (to_address_details::has_to_address_v<Ptr>) {
        return std::pointer_traits<Ptr>::to_address(p);
    } else {
        return to_address(p.operator->());
    }
}

class __is_little_endian_helper {
    constexpr static std::uint32_t u4 = 1;
    constexpr static std::uint8_t u1 = static_cast<const std::uint8_t &>(u4);

public:
    constexpr static bool value = u1 != 0;
};

// constexpr endian
enum class endian {
    little = 0,
    big = 1,
    native = __is_little_endian_helper::value ? little : big
};

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T fallback_byteswap(T x) noexcept {
    constexpr auto digits = std::numeric_limits<T>::digits;
    auto val = static_cast<uint_t<digits>>(x);
    if constexpr (digits == 8) {
        return val;
    } else if constexpr (digits == 16) {
        return (val >> 8) | (val << 8);
    } else if constexpr (digits == 32) {
        return ((val >> 24) & 0xff) | ((val >> 8) & 0xff00) | ((val << 8) & 0xff0000) |
               ((val << 24));
    } else if constexpr (digits == 64) {
        return ((val >> 56) & 0xff) | ((val >> 40) & 0xff00) | ((val >> 24) & 0xff0000) |
               ((val >> 8) & 0xff000000) | ((val << 8) & 0xff00000000) |
               ((val << 24) & 0xff0000000000) | ((val << 40) & 0xff000000000000) |
               ((val << 56));
    } else {
        static_assert(digits <= 64, "Unsupported bit width");
    }
}

#if WJR_HAS_BUILTIN(__builtin_bswap16)
#define WJR_HAS_BUILTIN_BYTESWAP WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(BYTESWAP)

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T builtin_byteswap(T x) noexcept {
    constexpr auto digits = std::numeric_limits<T>::digits;
    auto val = static_cast<uint_t<digits>>(x);
    if constexpr (digits == 8) {
        return val;
    } else if constexpr (digits == 16) {
        return __builtin_bswap16(val);
    } else if constexpr (digits == 32) {
        return __builtin_bswap32(val);
    } else if constexpr (digits == 64) {
        return __builtin_bswap64(val);
    } else {
        static_assert(digits <= 64, "Unsupported bit width");
    }
}

#endif

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T byteswap(T x, endian to = endian::little) noexcept {
    if (to == endian::native) {
        return x;
    }

#if WJR_HAS_BUILTIN(BYTESWAP)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_byteswap(x);
    }

    return builtin_byteswap(x);
#else
    return fallback_byteswap(x);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_INLINE T read_memory(const void *ptr,
                                            endian to = endian::little) noexcept {
    T x;
    std::memcpy(&x, ptr, sizeof(T));

    if (to != endian::native) {
        x = byteswap(x);
    }

    return x;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_INLINE void write_memory(void *ptr, T x,
                                       endian to = endian::little) noexcept {
    if (to != endian::native) {
        x = byteswap(x);
    }

    std::memcpy(ptr, &x, sizeof(T));
}

template <class Pointer, class SizeType = std::size_t>
struct allocation_result {
    Pointer ptr;
    SizeType count;
};

template <typename Enable, typename Allocator, typename SizeType, typename... Args>
struct __has_allocate_at_least : std::false_type {};
template <typename Allocator, typename SizeType, typename... Args>
struct __has_allocate_at_least<
    std::void_t<decltype(std::declval<Allocator>().allocate_at_least(
        std::declval<SizeType>()))>,
    Allocator, SizeType, Args...> : std::true_type {};
template <typename Allocator, typename SizeType, typename... Args>
struct has_allocate_at_least
    : __has_allocate_at_least<void, Allocator, SizeType, Args...> {};
template <typename Allocator, typename SizeType, typename... Args>
constexpr bool has_allocate_at_least_v =
    has_allocate_at_least<Allocator, SizeType, Args...>::value;

template <typename Allocator, typename SizeType,
          typename Pointer = typename std::allocator_traits<Allocator>::pointer>
WJR_NODISCARD allocation_result<Pointer, SizeType> allocate_at_least(Allocator &alloc,
                                                                     SizeType count) {
    if constexpr (has_allocate_at_least_v<Allocator, SizeType>) {
        auto result = alloc.allocate_at_least(count);
        WJR_ASSUME(result.count >= count);
        return result;
    } else {
        auto ptr = std::allocator_traits<Allocator>::allocate(alloc, count);
        return {ptr, count};
    }
}

} // namespace wjr

#endif // WJR_MEMORY_TO_ADDRESS_HPP__

namespace wjr {

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool has_single_bit(T n) noexcept {
    return (n != 0) && is_zero_or_single_bit(n);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countl_zero(T x) noexcept {
    return clz(x);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countr_zero(T x) noexcept {
    return ctz(x);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countl_one(T x) noexcept {
    return countl_zero(static_cast<T>(~x));
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countr_one(T x) noexcept {
    return countr_zero(static_cast<T>(~x));
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int bit_width(T x) noexcept {
    return std::numeric_limits<T>::digits - countl_zero(x);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T bit_ceil(T x) noexcept {
    if (x <= 1) {
        return T(1);
    }
    if constexpr (std::is_same_v<T, decltype(+x)>) {
        return T(1) << bit_width(T(x - 1));
    } else {
        constexpr int offset_for_ub =
            std::numeric_limits<unsigned>::digits - std::numeric_limits<T>::digits;
        return T(1 << (bit_width(T(x - 1)) + offset_for_ub) >> offset_for_ub);
    }
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T bit_floor(T x) noexcept {
    if (x != 0) {
        return T{1} << (bit_width(x) - 1);
    }
    return 0;
}

} // namespace wjr

#endif // WJR_MATH_BIT_HPP__
#ifndef WJR_MATH_DIV_HPP__
#define WJR_MATH_DIV_HPP__

#ifndef WJR_MATH_CMP_HPP__
#define WJR_MATH_CMP_HPP__

#ifndef WJR_MATH_SUB_HPP__
#define WJR_MATH_SUB_HPP__

// Already included
#ifndef WJR_MATH_REPLACE_HPP__
#define WJR_MATH_REPLACE_HPP__

#ifndef WJR_MATH_FIND_HPP__
#define WJR_MATH_FIND_HPP__

// Already included

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_FIND_HPP__
#define WJR_X86_MATH_FIND_HPP__

#ifndef WJR_SIMD_SIMD_HPP__
#define WJR_SIMD_SIMD_HPP__

#include <cstring>

#ifndef WJR_MATH_BROADCAST_HPP__
#define WJR_MATH_BROADCAST_HPP__

#include <cstdint>

// Already included

namespace wjr {

template <typename From, typename To>
struct broadcast_fn {};

/**
 * @brief Broadcast a value to a wider type.
 * 
 * @note From must be a smaller type than To.
 */
template <typename From, typename To>
inline constexpr broadcast_fn<From, To> broadcast{};

template <>
struct broadcast_fn<uint8_t, uint8_t> {
    WJR_INTRINSIC_CONSTEXPR uint8_t operator()(uint8_t x) const { return x; }
};
template <>
struct broadcast_fn<uint16_t, uint16_t> {
    WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint16_t x) const { return x; }
};
template <>
struct broadcast_fn<uint32_t, uint32_t> {
    WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint32_t x) const { return x; }
};
template <>
struct broadcast_fn<uint64_t, uint64_t> {
    WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint64_t x) const { return x; }
};

template <>
struct broadcast_fn<uint8_t, uint16_t> {
    WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint8_t x) const {
        return static_cast<uint16_t>(static_cast<uint32_t>(x) |
                                     (static_cast<uint16_t>(x) << 8));
    }
};

template <>
struct broadcast_fn<uint16_t, uint32_t> {
    WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint16_t x) const {
        return x | (static_cast<uint32_t>(x) << 16);
    }
};
template <>
struct broadcast_fn<uint32_t, uint64_t> {
    WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint32_t x) const {
        return static_cast<uint64_t>(x) | (static_cast<uint64_t>(x) << 32);
    }
};

template <>
struct broadcast_fn<uint8_t, uint32_t> {
    WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint8_t x) const {
        return x * static_cast<uint32_t>(0x01010101u);
    }
};
template <>
struct broadcast_fn<uint16_t, uint64_t> {
    WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint16_t x) const {
        return x * static_cast<uint64_t>(0x0001000100010001ull);
    }
};

template <>
struct broadcast_fn<uint8_t, uint64_t> {
    WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint8_t x) const {
        return x * static_cast<uint64_t>(0x0101010101010101ull);
    }
};

} // namespace wjr

#endif // WJR_MATH_BROADCAST_HPP__
// Already included
#ifndef WJR_SIMD_SIMD_CAST_HPP__
#define WJR_SIMD_SIMD_CAST_HPP__

// Already included

#define WJR_HAS_SIMD_SIMD WJR_HAS_DEF

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#error "Not support yet."
#include <arm_neon.h>
#else
#undef WJR_HAS_SIMD_SIMD
#endif

namespace wjr {

template <typename From, typename To>
struct simd_cast_fn;

template <typename From, typename To>
inline constexpr simd_cast_fn<From, To> simd_cast;

// simd type can't be directly used on template
template <typename T>
struct simd_wrapper {
    using type = T;
};

template <typename T>
using simd_wrapper_t = typename simd_wrapper<T>::type;

#if WJR_HAS_SIMD(SSE)

struct __m128_t {
    using type = __m128;
};

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

struct __m128i_t {
    using type = __m128i;
};
struct __m128d_t {
    using type = __m128d;
};

template <>
struct simd_cast_fn<__m128_t, __m128i_t> {
    __m128i operator()(__m128 v) const { return _mm_castps_si128(v); }
};
template <>
struct simd_cast_fn<__m128_t, __m128d_t> {
    __m128d operator()(__m128 v) const { return _mm_castps_pd(v); }
};
template <>
struct simd_cast_fn<__m128i_t, __m128_t> {
    __m128 operator()(__m128i v) const { return _mm_castsi128_ps(v); }
};
template <>
struct simd_cast_fn<__m128i_t, __m128d_t> {
    __m128d operator()(__m128i v) const { return _mm_castsi128_pd(v); }
};
template <>
struct simd_cast_fn<__m128d_t, __m128_t> {
    __m128 operator()(__m128d v) const { return _mm_castpd_ps(v); }
};
template <>
struct simd_cast_fn<__m128d_t, __m128i_t> {
    __m128i operator()(__m128d v) const { return _mm_castpd_si128(v); }
};

template <>
struct simd_cast_fn<int8_t, __m128i_t> {
    __m128i operator()(int8_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<uint8_t, __m128i_t> {
    __m128i operator()(uint8_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<__m128i_t, int8_t> {
    int8_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};
template <>
struct simd_cast_fn<__m128i_t, uint8_t> {
    uint8_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};

template <>
struct simd_cast_fn<int16_t, __m128i_t> {
    __m128i operator()(int16_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<uint16_t, __m128i_t> {
    __m128i operator()(uint16_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<__m128i_t, int16_t> {
    int16_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};
template <>
struct simd_cast_fn<__m128i_t, uint16_t> {
    uint16_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};

template <>
struct simd_cast_fn<int32_t, __m128i_t> {
    __m128i operator()(int32_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<uint32_t, __m128i_t> {
    __m128i operator()(uint32_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<__m128i_t, int32_t> {
    int32_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};
template <>
struct simd_cast_fn<__m128i_t, uint32_t> {
    uint32_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};

template <>
struct simd_cast_fn<int64_t, __m128i_t> {
    __m128i operator()(int64_t v) const { return _mm_cvtsi64_si128(v); }
};
template <>
struct simd_cast_fn<uint64_t, __m128i_t> {
    __m128i operator()(uint64_t v) const { return _mm_cvtsi64_si128(v); }
};
template <>
struct simd_cast_fn<__m128i_t, int64_t> {
    int64_t operator()(__m128i v) const { return _mm_cvtsi128_si64(v); }
};
template <>
struct simd_cast_fn<__m128i_t, uint64_t> {
    uint64_t operator()(__m128i v) const { return _mm_cvtsi128_si64(v); }
};

#endif // SSE2

#if WJR_HAS_SIMD(AVX)

struct __m256_t {
    using type = __m256;
};
struct __m256i_t {
    using type = __m256i;
};
struct __m256d_t {
    using type = __m256d;
};

template <>
struct simd_cast_fn<__m256_t, __m256i_t> {
    __m256i operator()(__m256 v) const { return _mm256_castps_si256(v); }
};
template <>
struct simd_cast_fn<__m256_t, __m256d_t> {
    __m256d operator()(__m256 v) const { return _mm256_castps_pd(v); }
};
template <>
struct simd_cast_fn<__m256i_t, __m256_t> {
    __m256 operator()(__m256i v) const { return _mm256_castsi256_ps(v); }
};
template <>
struct simd_cast_fn<__m256i_t, __m256d_t> {
    __m256d operator()(__m256i v) const { return _mm256_castsi256_pd(v); }
};
template <>
struct simd_cast_fn<__m256d_t, __m256_t> {
    __m256 operator()(__m256d v) const { return _mm256_castpd_ps(v); }
};
template <>
struct simd_cast_fn<__m256d_t, __m256i_t> {
    __m256i operator()(__m256d v) const { return _mm256_castpd_si256(v); }
};
template <>
struct simd_cast_fn<__m128i_t, __m256i_t> {
    __m256i operator()(__m128i v) const { return _mm256_castsi128_si256(v); }
};
template <>
struct simd_cast_fn<__m256i_t, __m128i_t> {
    __m128i operator()(__m256i v) const { return _mm256_castsi256_si128(v); }
};

template <>
struct simd_cast_fn<int8_t, __m256i_t> {
    __m256i operator()(int8_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<uint8_t, __m256i_t> {
    __m256i operator()(uint8_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, int8_t> {
    int8_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, uint8_t> {
    uint8_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<int16_t, __m256i_t> {
    __m256i operator()(int16_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<uint16_t, __m256i_t> {
    __m256i operator()(uint16_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, int16_t> {
    int16_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, uint16_t> {
    uint16_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<int32_t, __m256i_t> {
    __m256i operator()(int32_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<uint32_t, __m256i_t> {
    __m256i operator()(uint32_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, int32_t> {
    int32_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, uint32_t> {
    uint32_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<int64_t, __m256i_t> {
    __m256i operator()(int64_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint64_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<uint64_t, __m256i_t> {
    __m256i operator()(uint64_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint64_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, int64_t> {
    int64_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint64_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, uint64_t> {
    uint64_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint64_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

#endif // AVX

} // namespace wjr

#endif // WJR_SIMD_SIMD_CAST_HPP__

namespace wjr {

struct sse {
    using mask_type = uint16_t;

#if WJR_HAS_SIMD(SSE)

    using float_type = __m128;
    using float_tag_type = __m128_t;

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

    using int_type = __m128i;
    using int_tag_type = __m128i_t;
    using double_type = __m128d;
    using double_tag_type = __m128d_t;

#endif // SSE2

    constexpr static size_t width();

    constexpr static mask_type mask();

#if WJR_HAS_SIMD(SSE)

    WJR_INTRINSIC_INLINE static mask_type movemask_ps(__m128 v);

    WJR_INTRINSIC_INLINE static void sfence();

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128 shuffle_ps(__m128 a, __m128 b);

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

    WJR_INTRINSIC_INLINE static __m128i add_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i add_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i add_epi32(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i add_epi64(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int64_t);
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint64_t);

    WJR_INTRINSIC_INLINE static int8_t add_epi8(__m128i a);
    WJR_INTRINSIC_INLINE static int16_t add_epi16(__m128i a);
    WJR_INTRINSIC_INLINE static int32_t add_epi32(__m128i a);
    WJR_INTRINSIC_INLINE static int64_t add_epi64(__m128i a);

    WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m128i a);
    WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m128i a);
    WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m128i a);
    WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m128i a);

    WJR_INTRINSIC_INLINE static int8_t add(__m128i a, int8_t);
    WJR_INTRINSIC_INLINE static int16_t add(__m128i a, int16_t);
    WJR_INTRINSIC_INLINE static int32_t add(__m128i a, int32_t);
    WJR_INTRINSIC_INLINE static int64_t add(__m128i a, int64_t);
    WJR_INTRINSIC_INLINE static uint8_t add(__m128i a, uint8_t);
    WJR_INTRINSIC_INLINE static uint16_t add(__m128i a, uint16_t);
    WJR_INTRINSIC_INLINE static uint32_t add(__m128i a, uint32_t);
    WJR_INTRINSIC_INLINE static uint64_t add(__m128i a, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i adds_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i adds_epi16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i adds_epu8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i adds_epu16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint16_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i alignr_epi16(__m128i a, __m128i b, int c);
    WJR_INTRINSIC_INLINE static __m128i alignr_epi32(__m128i a, __m128i b, int c);
    WJR_INTRINSIC_INLINE static __m128i alignr_epi64(__m128i a, __m128i b, int c);

    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int16_t);
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int32_t);
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int64_t);
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i And(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i AndNot(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i avg_epu8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i avg_epu16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint16_t);

    // notice that mask must be 0 or 255(every byte)
    WJR_INTRINSIC_INLINE static __m128i blendv_epi8(__m128i a, __m128i b, __m128i mask);
    WJR_INTRINSIC_INLINE static __m128i blendv_epi16(__m128i a, __m128i b, __m128i mask);
    WJR_INTRINSIC_INLINE static __m128i blendv_epi32(__m128i a, __m128i b, __m128i mask);

    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               int8_t);
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               int16_t);
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               int32_t);
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               uint8_t);
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               uint16_t);
    WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask,
                                               uint32_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i bslli(__m128i val);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i bsrli(__m128i val);

    WJR_INTRINSIC_INLINE static __m128i cmpeq_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpeq_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpeq_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint32_t);

    WJR_INTRINSIC_INLINE static __m128i cmpge_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpge_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpge_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmpge_epu8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpge_epu16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpge_epu32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint32_t);

    WJR_INTRINSIC_INLINE static __m128i cmpgt_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpgt_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpgt_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmpgt_epu8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpgt_epu16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpgt_epu32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint32_t);

    WJR_INTRINSIC_INLINE static __m128i cmple_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmple_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmple_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmple_epu8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmple_epu16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmple_epu32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint32_t);

    WJR_INTRINSIC_INLINE static __m128i cmplt_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmplt_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmplt_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmplt_epu8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmplt_epu16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmplt_epu32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint32_t);

    WJR_INTRINSIC_INLINE static __m128i cmpne_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpne_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i cmpne_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint32_t);

    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::equal_to<>, T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::not_equal_to<>, T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater<>, T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater_equal<>,
                                            T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less<>, T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less_equal<>, T);

    WJR_INTRINSIC_INLINE static __m128i concat(uint64_t lo, uint64_t hi);

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi8(__m128i a);

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi16(__m128i a);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi32(__m128i a);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m128i a);

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, int8_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, int16_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, int32_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, int64_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, uint8_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, uint16_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m128i a, uint32_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, uint64_t);

    WJR_INTRINSIC_INLINE static uint64_t getlow(__m128i v);
    WJR_INTRINSIC_INLINE static uint64_t gethigh(__m128i v);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert_epi16(__m128i a, int i);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int16_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint16_t);

    WJR_INTRINSIC_INLINE static void lfence();

    WJR_INTRINSIC_INLINE static __m128i load(const __m128i *ptr);
    WJR_INTRINSIC_INLINE static __m128i loadu(const __m128i *ptr);
    WJR_INTRINSIC_INLINE static __m128i loadu_si16(const void *ptr);
    WJR_INTRINSIC_INLINE static __m128i loadu_si32(const void *ptr);
    WJR_INTRINSIC_INLINE static __m128i loadu_si64(const void *ptr);

    template <typename T,
              std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                           uint16_t, uint32_t, uint64_t>,
                               int> = 0>
    WJR_INTRINSIC_INLINE static __m128i logical_and(__m128i a, __m128i b, T);

    template <typename T,
              std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                           uint16_t, uint32_t, uint64_t>,
                               int> = 0>
    WJR_INTRINSIC_INLINE static __m128i logical_not(__m128i v, T);

    template <typename T,
              std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                           uint16_t, uint32_t, uint64_t>,
                               int> = 0>
    WJR_INTRINSIC_INLINE static __m128i logical_or(__m128i a, __m128i b, T);

    WJR_INTRINSIC_INLINE static __m128i madd_epi16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static void maskmoveu(__m128i a, __m128i mask, char *mem_addr);

    WJR_INTRINSIC_INLINE static __m128i max_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i max_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i max_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i max_epu8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i max_epu16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i max_epu32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint32_t);

    WJR_INTRINSIC_INLINE static int8_t max_epi8(__m128i a);
    WJR_INTRINSIC_INLINE static int16_t max_epi16(__m128i a);
    WJR_INTRINSIC_INLINE static int32_t max_epi32(__m128i a);

    WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m128i a);
    WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m128i a);
    WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m128i a);

    WJR_INTRINSIC_INLINE static int8_t max(__m128i a, int8_t);
    WJR_INTRINSIC_INLINE static int16_t max(__m128i a, int16_t);
    WJR_INTRINSIC_INLINE static int32_t max(__m128i a, int32_t);
    WJR_INTRINSIC_INLINE static uint8_t max(__m128i a, uint8_t);
    WJR_INTRINSIC_INLINE static uint16_t max(__m128i a, uint16_t);
    WJR_INTRINSIC_INLINE static uint32_t max(__m128i a, uint32_t);

    WJR_INTRINSIC_INLINE static void mfence();

    WJR_INTRINSIC_INLINE static __m128i min_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i min_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i min_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i min_epu8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i min_epu16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i min_epu32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint32_t);

    WJR_INTRINSIC_INLINE static int8_t min_epi8(__m128i a);
    WJR_INTRINSIC_INLINE static int16_t min_epi16(__m128i a);
    WJR_INTRINSIC_INLINE static int32_t min_epi32(__m128i a);

    WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m128i a);
    WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m128i a);
    WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m128i a);

    WJR_INTRINSIC_INLINE static int8_t min(__m128i a, int8_t);
    WJR_INTRINSIC_INLINE static int16_t min(__m128i a, int16_t);
    WJR_INTRINSIC_INLINE static int32_t min(__m128i a, int32_t);

    WJR_INTRINSIC_INLINE static uint8_t min(__m128i a, uint8_t);
    WJR_INTRINSIC_INLINE static uint16_t min(__m128i a, uint16_t);
    WJR_INTRINSIC_INLINE static uint32_t min(__m128i a, uint32_t);

    WJR_INTRINSIC_INLINE static __m128i move_epi64(__m128i a);

    WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m128i a);
    WJR_INTRINSIC_INLINE static mask_type movemask_pd(__m128d v);

    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int8_t);
    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int32_t);
    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int64_t);

    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint8_t);
    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint32_t);
    WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i mul_epu32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i mulhi_epi16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i mulhi_epu16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i mullo_epi16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i negate_epi8(__m128i a);
    WJR_INTRINSIC_INLINE static __m128i negate_epi16(__m128i a);
    WJR_INTRINSIC_INLINE static __m128i negate_epi32(__m128i a);
    WJR_INTRINSIC_INLINE static __m128i negate_epi64(__m128i a);

    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int8_t);
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int16_t);
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int32_t);
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int64_t);
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i Not(__m128i v);

    WJR_INTRINSIC_INLINE static __m128i Or(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i packs_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i packs_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i packus_epi16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i preloadu_si16(const void *ptr);
    WJR_INTRINSIC_INLINE static __m128i preloadu_si32(const void *ptr);
    WJR_INTRINSIC_INLINE static __m128i preloadu_si48(const void *ptr);
    WJR_INTRINSIC_INLINE static __m128i preloadu_si64(const void *ptr);
    WJR_INTRINSIC_INLINE static __m128i preloadu_si80(const void *ptr);
    WJR_INTRINSIC_INLINE static __m128i preloadu_si96(const void *ptr);
    WJR_INTRINSIC_INLINE static __m128i preloadu_si112(const void *ptr);
    WJR_INTRINSIC_INLINE static __m128i preloadu_si128(const void *ptr);

    WJR_INTRINSIC_INLINE static __m128i preloadu_si16x(const void *ptr, int n);

    WJR_INTRINSIC_INLINE static __m128i sad_epu8(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i zeros();
    WJR_INTRINSIC_INLINE static __m128i ones();

    WJR_INTRINSIC_INLINE static __m128i set_epi8(char e15, char e14, char e13, char e12,
                                                 char e11, char e10, char e9, char e8,
                                                 char e7, char e6, char e5, char e4,
                                                 char e3, char e2, char e1, char e0);

    WJR_INTRINSIC_INLINE static __m128i set_epi16(short e7, short e6, short e5, short e4,
                                                  short e3, short e2, short e1, short e0);
    WJR_INTRINSIC_INLINE static __m128i set_epi32(int e3, int e2, int e1, int e0);
    WJR_INTRINSIC_INLINE static __m128i set_epi64x(long long e1, long long e0);

    WJR_INTRINSIC_INLINE static __m128i setr_epi8(char e15, char e14, char e13, char e12,
                                                  char e11, char e10, char e9, char e8,
                                                  char e7, char e6, char e5, char e4,
                                                  char e3, char e2, char e1, char e0);

    WJR_INTRINSIC_INLINE static __m128i setr_epi16(short e7, short e6, short e5, short e4,
                                                   short e3, short e2, short e1,
                                                   short e0);
    WJR_INTRINSIC_INLINE static __m128i setr_epi32(int e3, int e2, int e1, int e0);

    WJR_INTRINSIC_INLINE static __m128i set1_epi8(int8_t val);
    WJR_INTRINSIC_INLINE static __m128i set1_epi16(int16_t val);
    WJR_INTRINSIC_INLINE static __m128i set1_epi32(int32_t val);
    WJR_INTRINSIC_INLINE static __m128i set1_epi64(int64_t val);

    WJR_INTRINSIC_INLINE static __m128i set1(int8_t val, int8_t);
    WJR_INTRINSIC_INLINE static __m128i set1(int16_t val, int16_t);
    WJR_INTRINSIC_INLINE static __m128i set1(int32_t val, int32_t);
    WJR_INTRINSIC_INLINE static __m128i set1(int64_t val, int64_t);
    WJR_INTRINSIC_INLINE static __m128i set1(uint8_t val, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i set1(uint16_t val, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i set1(uint32_t val, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i set1(uint64_t val, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i setmin_epi8();
    WJR_INTRINSIC_INLINE static __m128i setmin_epi16();
    WJR_INTRINSIC_INLINE static __m128i setmin_epi32();

    WJR_INTRINSIC_INLINE static __m128i setmin(int8_t);
    WJR_INTRINSIC_INLINE static __m128i setmin(int16_t);
    WJR_INTRINSIC_INLINE static __m128i setmin(int32_t);
    WJR_INTRINSIC_INLINE static __m128i setmin(uint8_t);
    WJR_INTRINSIC_INLINE static __m128i setmin(uint16_t);
    WJR_INTRINSIC_INLINE static __m128i setmin(uint32_t);

    WJR_INTRINSIC_INLINE static __m128i setmax_epi8();
    WJR_INTRINSIC_INLINE static __m128i setmax_epi16();
    WJR_INTRINSIC_INLINE static __m128i setmax_epi32();

    WJR_INTRINSIC_INLINE static __m128i setmax(int8_t);
    WJR_INTRINSIC_INLINE static __m128i setmax(int16_t);
    WJR_INTRINSIC_INLINE static __m128i setmax(int32_t);
    WJR_INTRINSIC_INLINE static __m128i setmax(uint8_t);
    WJR_INTRINSIC_INLINE static __m128i setmax(uint16_t);
    WJR_INTRINSIC_INLINE static __m128i setmax(uint32_t);

    template <int imm>
    WJR_INTRINSIC_INLINE static __m128i shl(__m128i a);

    template <int imm>
    WJR_INTRINSIC_INLINE static __m128i shr(__m128i b);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i shuffle_epi32(__m128i v);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i shufflehi_epi16(__m128i v);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i shufflelo_epi16(__m128i v);

    WJR_INTRINSIC_INLINE static __m128i sll_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i sll_epi32(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i sll_epi64(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int64_t);
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint64_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i v);
    WJR_INTRINSIC_INLINE static __m128i slli_epi16(__m128i a, int imm8);
    WJR_INTRINSIC_INLINE static __m128i slli_epi32(__m128i a, int imm8);
    WJR_INTRINSIC_INLINE static __m128i slli_epi64(__m128i a, int imm8);

    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int16_t);
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int32_t);
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int64_t);
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i sra_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i sra_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int32_t);

    WJR_INTRINSIC_INLINE static __m128i srai_epi16(__m128i a, int imm8);
    WJR_INTRINSIC_INLINE static __m128i srai_epi32(__m128i a, int imm8);

    WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int16_t);
    WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int32_t);

    WJR_INTRINSIC_INLINE static __m128i srl_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i srl_epi32(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i srl_epi64(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int64_t);
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint64_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i v);
    WJR_INTRINSIC_INLINE static __m128i srli_epi16(__m128i a, int imm8);
    WJR_INTRINSIC_INLINE static __m128i srli_epi32(__m128i a, int imm8);
    WJR_INTRINSIC_INLINE static __m128i srli_epi64(__m128i a, int imm8);

    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int16_t);
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int32_t);
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int64_t);
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint64_t);

    WJR_INTRINSIC_INLINE static void stream(__m128i *ptr, __m128i v);

    WJR_INTRINSIC_INLINE static void store(__m128i *ptr, __m128i val);
    WJR_INTRINSIC_INLINE static void storeu(__m128i *ptr, __m128i val);

    WJR_INTRINSIC_INLINE static __m128i sub_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i sub_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i sub_epi32(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i sub_epi64(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int64_t);
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i subs_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i subs_epi16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i subs_epu8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i subs_epu16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint16_t);

    WJR_INTRINSIC_INLINE static __m128i unpackhi_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i unpackhi_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i unpackhi_epi32(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i unpackhi_epi64(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int64_t);
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i unpacklo_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i unpacklo_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i unpacklo_epi32(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i unpacklo_epi64(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int64_t);
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint32_t);

    WJR_INTRINSIC_INLINE static __m128i Xor(__m128i a, __m128i b);

#endif // SSE2

#if WJR_HAS_SIMD(SSE3)

    WJR_INTRINSIC_INLINE static __m128i lddqu(const __m128i *ptr);

#endif // SSE3

#if WJR_HAS_SIMD(SSSE3)

    WJR_INTRINSIC_INLINE static __m128i abs_epi8(__m128i val);
    WJR_INTRINSIC_INLINE static __m128i abs_epi16(__m128i val);
    WJR_INTRINSIC_INLINE static __m128i abs_epi32(__m128i val);

    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int8_t);
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int16_t);
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int32_t);
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint32_t);

    WJR_INTRINSIC_INLINE static __m128i shuffle_epi8(__m128i v, __m128i imm8);

    WJR_INTRINSIC_INLINE static __m128i sign_epi8(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i sign_epi16(__m128i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m128i sign_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int8_t);
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint32_t);

#endif // SSSE3

#if WJR_HAS_SIMD(SSE4_1)

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i blend_epi16(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmpeq_epi64(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int64_t);
    WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i cmpgt_epi64(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int64_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert_epi8(__m128i a, int i);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert_epi32(__m128i a, int i);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert_epi64(__m128i a, int64_t i);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int8_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int32_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, int64_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint8_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint32_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, uint64_t);

    WJR_INTRINSIC_INLINE static __m128i minpos_epu16(__m128i a);

    WJR_INTRINSIC_INLINE static __m128i mul_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i mullo_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i packus_epi32(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m128i stream_load(__m128i *p);

    WJR_INTRINSIC_INLINE static int test_all_ones(__m128i a);

    WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a);

    WJR_INTRINSIC_INLINE static int test_mix_ones_zeros(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static int testc(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static int testnzc(__m128i a, __m128i b);

    WJR_INTRINSIC_INLINE static int testz(__m128i a, __m128i b);

#endif // SSE4_1
};

struct avx {
    using mask_type = uint32_t;

#if WJR_HAS_SIMD(AVX)

    using float_type = __m256;
    using float_tag_type = __m256_t;
    using int_type = __m256i;
    using int_tag_type = __m256i_t;
    using double_type = __m256d;
    using double_tag_type = __m256d_t;

#endif // AVX

    constexpr static size_t width();

    constexpr static mask_type mask();

#if WJR_HAS_SIMD(AVX)

    WJR_INTRINSIC_INLINE static __m256i concat(__m128i a, __m128i b);

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi32(__m256i v);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m256i v);

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m256i v, int32_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract(__m256i v, int64_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i extract_si128(__m256i v);

    WJR_INTRINSIC_INLINE static __m128i getlow(__m256i a);

    WJR_INTRINSIC_INLINE static __m128i gethigh(__m256i a);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_epi8(__m256i v, int8_t i);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_epi16(__m256i v, int16_t i);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_epi32(__m256i v, int32_t i);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_epi64(__m256i v, int64_t i);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_si128(__m256i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m256i load(const __m256i *p);
    WJR_INTRINSIC_INLINE static __m256i loadu(const __m256i *p);

    WJR_INTRINSIC_INLINE static __m256i ones();

    WJR_INTRINSIC_INLINE static __m256i preloadu_si16(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si32(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si48(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si64(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si80(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si96(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si112(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si128(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si144(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si160(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si176(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si192(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si208(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si224(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si240(const void *ptr);
    WJR_INTRINSIC_INLINE static __m256i preloadu_si256(const void *ptr);

    WJR_INTRINSIC_INLINE static __m256i preloadu_si16x(const void *ptr, int n);

    WJR_INTRINSIC_INLINE static __m256i
    set_epi8(char e31, char e30, char e29, char e28, char e27, char e26, char e25,
             char e24, char e23, char e22, char e21, char e20, char e19, char e18,
             char e17, char e16, char e15, char e14, char e13, char e12, char e11,
             char e10, char e9, char e8, char e7, char e6, char e5, char e4, char e3,
             char e2, char e1, char e0);

    WJR_INTRINSIC_INLINE static __m256i set_epi16(short e15, short e14, short e13,
                                                  short e12, short e11, short e10,
                                                  short e9, short e8, short e7, short e6,
                                                  short e5, short e4, short e3, short e2,
                                                  short e1, short e0);

    WJR_INTRINSIC_INLINE static __m256i set_epi32(int e7, int e6, int e5, int e4, int e3,
                                                  int e2, int e1, int e0);

    WJR_INTRINSIC_INLINE static __m256i set_epi64x(long long e3, long long e2,
                                                   long long e1, long long e0);

    WJR_INTRINSIC_INLINE static __m256i
    setr_epi8(char e31, char e30, char e29, char e28, char e27, char e26, char e25,
              char e24, char e23, char e22, char e21, char e20, char e19, char e18,
              char e17, char e16, char e15, char e14, char e13, char e12, char e11,
              char e10, char e9, char e8, char e7, char e6, char e5, char e4, char e3,
              char e2, char e1, char e0);

    WJR_INTRINSIC_INLINE static __m256i setr_epi16(short e15, short e14, short e13,
                                                   short e12, short e11, short e10,
                                                   short e9, short e8, short e7, short e6,
                                                   short e5, short e4, short e3, short e2,
                                                   short e1, short e0);

    WJR_INTRINSIC_INLINE static __m256i setr_epi32(int e7, int e6, int e5, int e4, int e3,
                                                   int e2, int e1, int e0);

    WJR_INTRINSIC_INLINE static __m256i setr_epi64x(long long e3, long long e2,
                                                    long long e1, long long e0);

    WJR_INTRINSIC_INLINE static __m256i set1_epi8(int8_t a);
    WJR_INTRINSIC_INLINE static __m256i set1_epi16(int16_t a);
    WJR_INTRINSIC_INLINE static __m256i set1_epi32(int32_t a);
    WJR_INTRINSIC_INLINE static __m256i set1_epi64(int64_t a);

    WJR_INTRINSIC_INLINE static __m256i set1(int8_t a, int8_t);
    WJR_INTRINSIC_INLINE static __m256i set1(int16_t a, int16_t);
    WJR_INTRINSIC_INLINE static __m256i set1(int32_t a, int32_t);
    WJR_INTRINSIC_INLINE static __m256i set1(int64_t a, int64_t);
    WJR_INTRINSIC_INLINE static __m256i set1(uint8_t a, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i set1(uint16_t a, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i set1(uint32_t a, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i set1(uint64_t a, uint64_t);

    WJR_INTRINSIC_INLINE static __m256i setmin_epi8();
    WJR_INTRINSIC_INLINE static __m256i setmin_epi16();
    WJR_INTRINSIC_INLINE static __m256i setmin_epi32();
    WJR_INTRINSIC_INLINE static __m256i setmin_epi64();

    WJR_INTRINSIC_INLINE static __m256i setmin(int8_t);
    WJR_INTRINSIC_INLINE static __m256i setmin(int16_t);
    WJR_INTRINSIC_INLINE static __m256i setmin(int32_t);
    WJR_INTRINSIC_INLINE static __m256i setmin(int64_t);

    WJR_INTRINSIC_INLINE static __m256i setmax_epi8();
    WJR_INTRINSIC_INLINE static __m256i setmax_epi16();
    WJR_INTRINSIC_INLINE static __m256i setmax_epi32();
    WJR_INTRINSIC_INLINE static __m256i setmax_epi64();

    WJR_INTRINSIC_INLINE static __m256i setmax(int8_t);
    WJR_INTRINSIC_INLINE static __m256i setmax(int16_t);
    WJR_INTRINSIC_INLINE static __m256i setmax(int32_t);
    WJR_INTRINSIC_INLINE static __m256i setmax(int64_t);

    WJR_INTRINSIC_INLINE static void stream(__m256i *p, __m256i a);

    WJR_INTRINSIC_INLINE static void store(__m256i *p, __m256i a);
    WJR_INTRINSIC_INLINE static void storeu(__m256i *p, __m256i a);

    WJR_INTRINSIC_INLINE static int test_all_zeros(__m256i a);

    WJR_INTRINSIC_INLINE static int testc(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static int testnzc(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static int testz(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i zeros();

#endif // AVX

#if WJR_HAS_SIMD(AVX2)

    WJR_INTRINSIC_INLINE static __m256i And(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i AndNot(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i Or(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i Xor(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i Not(__m256i v);

    WJR_INTRINSIC_INLINE static __m256i abs_epi8(__m256i v);
    WJR_INTRINSIC_INLINE static __m256i abs_epi16(__m256i v);
    WJR_INTRINSIC_INLINE static __m256i abs_epi32(__m256i v);

    WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int8_t);
    WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int16_t);
    WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int32_t);
    WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int64_t);

    WJR_INTRINSIC_INLINE static __m256i add_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i add_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i add_epi32(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i add_epi64(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int64_t);
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint64_t);

    WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m256i v);
    WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m256i v);
    WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m256i v);
    WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m256i v);

    WJR_INTRINSIC_INLINE static int8_t add_epi8(__m256i v);
    WJR_INTRINSIC_INLINE static int16_t add_epi16(__m256i v);
    WJR_INTRINSIC_INLINE static int32_t add_epi32(__m256i v);
    WJR_INTRINSIC_INLINE static int64_t add_epi64(__m256i v);

    WJR_INTRINSIC_INLINE static int8_t add(__m256i v, int8_t);
    WJR_INTRINSIC_INLINE static int16_t add(__m256i v, int16_t);
    WJR_INTRINSIC_INLINE static int32_t add(__m256i v, int32_t);
    WJR_INTRINSIC_INLINE static int64_t add(__m256i v, int64_t);
    WJR_INTRINSIC_INLINE static uint8_t add(__m256i v, uint8_t);
    WJR_INTRINSIC_INLINE static uint16_t add(__m256i v, uint16_t);
    WJR_INTRINSIC_INLINE static uint32_t add(__m256i v, uint32_t);
    WJR_INTRINSIC_INLINE static uint64_t add(__m256i v, uint64_t);

    WJR_INTRINSIC_INLINE static __m256i adds_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i adds_epi16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i adds_epu8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i adds_epu16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint16_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i alignr_epi16(__m256i a, __m256i b, int c);
    WJR_INTRINSIC_INLINE static __m256i alignr_epi32(__m256i a, __m256i b, int c);
    WJR_INTRINSIC_INLINE static __m256i alignr_epi64(__m256i a, __m256i b, int c);

    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int16_t);
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int32_t);
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int64_t);
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint64_t);

    WJR_INTRINSIC_INLINE static __m256i avg_epu8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i avg_epu16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint16_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i blend_epi16(__m256i a, __m256i b);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i blend_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i blendv_epi8(__m256i a, __m256i b, __m256i mask);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i bslli_epi128(__m256i a);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i bsrli_epi128(__m256i a);

    WJR_INTRINSIC_INLINE static __m256i cmpeq_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpeq_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpeq_epi32(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpeq_epi64(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int64_t);
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint64_t);

    WJR_INTRINSIC_INLINE static __m256i cmpge_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpge_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpge_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmpge_epu8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpge_epu16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpge_epu32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint32_t);

    WJR_INTRINSIC_INLINE static __m256i cmpgt_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epi32(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epi64(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmpgt_epu8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epu16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epu32(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epu64(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int64_t);
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint64_t);

    WJR_INTRINSIC_INLINE static __m256i cmple_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmple_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmple_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmple_epu8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmple_epu16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmple_epu32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint32_t);

    WJR_INTRINSIC_INLINE static __m256i cmplt_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmplt_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmplt_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmplt_epu8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmplt_epu16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmplt_epu32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint32_t);

    WJR_INTRINSIC_INLINE static __m256i cmpne_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpne_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i cmpne_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint32_t);

    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::equal_to<>, T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::not_equal_to<>, T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater<>, T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater_equal<>,
                                            T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less<>, T);
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less_equal<>, T);

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi8(__m256i v);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi16(__m256i v);

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m256i v, int8_t);
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m256i v, int16_t);

    WJR_INTRINSIC_INLINE static __m256i hadd_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i hadd_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int32_t);

    WJR_INTRINSIC_INLINE static __m256i hadds_epi16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i hsub_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i hsub_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int32_t);

    WJR_INTRINSIC_INLINE static __m256i hsubs_epi16(__m256i a, __m256i b);

    template <typename T,
              std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                           uint16_t, uint32_t, uint64_t>,
                               int> = 0>
    WJR_INTRINSIC_INLINE static __m256i logical_and(__m256i a, __m256i b, T);

    template <typename T,
              std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                           uint16_t, uint32_t, uint64_t>,
                               int> = 0>
    WJR_INTRINSIC_INLINE static __m256i logical_not(__m256i v, T);

    template <typename T,
              std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                           uint16_t, uint32_t, uint64_t>,
                               int> = 0>
    WJR_INTRINSIC_INLINE static __m256i logical_or(__m256i a, __m256i b, T);

    WJR_INTRINSIC_INLINE static __m256i madd_epi16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i max_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i max_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i max_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i max_epu8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i max_epu16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i max_epu32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint32_t);

    WJR_INTRINSIC_INLINE static int8_t max_epi8(__m256i a);
    WJR_INTRINSIC_INLINE static int16_t max_epi16(__m256i a);
    WJR_INTRINSIC_INLINE static int32_t max_epi32(__m256i a);
    WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m256i a);
    WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m256i a);
    WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m256i a);

    WJR_INTRINSIC_INLINE static int8_t max(__m256i a, int8_t);
    WJR_INTRINSIC_INLINE static int16_t max(__m256i a, int16_t);
    WJR_INTRINSIC_INLINE static int32_t max(__m256i a, int32_t);

    WJR_INTRINSIC_INLINE static uint8_t max(__m256i a, uint8_t);
    WJR_INTRINSIC_INLINE static uint16_t max(__m256i a, uint16_t);
    WJR_INTRINSIC_INLINE static uint32_t max(__m256i a, uint32_t);

    WJR_INTRINSIC_INLINE static __m256i min_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i min_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i min_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i min_epu8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i min_epu16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i min_epu32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint32_t);

    WJR_INTRINSIC_INLINE static int8_t min_epi8(__m256i a);
    WJR_INTRINSIC_INLINE static int16_t min_epi16(__m256i a);
    WJR_INTRINSIC_INLINE static int32_t min_epi32(__m256i a);

    WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m256i a);
    WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m256i a);
    WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m256i a);

    WJR_INTRINSIC_INLINE static int8_t min(__m256i a, int8_t);
    WJR_INTRINSIC_INLINE static int16_t min(__m256i a, int16_t);
    WJR_INTRINSIC_INLINE static int32_t min(__m256i a, int32_t);
    WJR_INTRINSIC_INLINE static uint8_t min(__m256i a, uint8_t);
    WJR_INTRINSIC_INLINE static uint16_t min(__m256i a, uint16_t);
    WJR_INTRINSIC_INLINE static uint32_t min(__m256i a, uint32_t);

    WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m256i a);

    WJR_INTRINSIC_INLINE static __m256i mul_epi32(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i mul_epu32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i mulhi_epi16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i mulhi_epu16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i mullo_epi16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i packs_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i packs_epi32(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i packus_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i packus_epi32(__m256i a, __m256i b);

    template <int imm>
    WJR_INTRINSIC_INLINE static __m256i shl(__m256i a);

    template <int imm>
    WJR_INTRINSIC_INLINE static __m256i shr(__m256i a);

    WJR_INTRINSIC_INLINE static __m256i shuffle_epi8(__m256i a, __m256i b);
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i shuffle_epi32(__m256i a);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i shufflehi_epi16(__m256i a);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i shufflelo_epi16(__m256i a);

    WJR_INTRINSIC_INLINE static __m256i sll_epi16(__m256i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m256i sll_epi32(__m256i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m256i sll_epi64(__m256i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int64_t);
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint64_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a);
    WJR_INTRINSIC_INLINE static __m256i slli_epi16(__m256i a, int imm8);
    WJR_INTRINSIC_INLINE static __m256i slli_epi32(__m256i a, int imm8);
    WJR_INTRINSIC_INLINE static __m256i slli_epi64(__m256i a, int imm8);

    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int16_t);
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int32_t);
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int64_t);
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint64_t);

    WJR_INTRINSIC_INLINE static __m256i sra_epi16(__m256i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m256i sra_epi32(__m256i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int32_t);

    WJR_INTRINSIC_INLINE static __m256i srai_epi16(__m256i a, int imm8);
    WJR_INTRINSIC_INLINE static __m256i srai_epi32(__m256i a, int imm8);

    WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int16_t);
    WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int32_t);

    WJR_INTRINSIC_INLINE static __m256i stream_load(__m256i const *p);

    WJR_INTRINSIC_INLINE static __m256i srl_epi16(__m256i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m256i srl_epi32(__m256i a, __m128i b);
    WJR_INTRINSIC_INLINE static __m256i srl_epi64(__m256i a, __m128i b);

    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int64_t);
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint64_t);

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a);
    WJR_INTRINSIC_INLINE static __m256i srli_epi16(__m256i a, int imm8);
    WJR_INTRINSIC_INLINE static __m256i srli_epi32(__m256i a, int imm8);
    WJR_INTRINSIC_INLINE static __m256i srli_epi64(__m256i a, int imm8);

    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int16_t);
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int32_t);
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int64_t);
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint64_t);

    WJR_INTRINSIC_INLINE static __m256i sub_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i sub_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i sub_epi32(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i sub_epi64(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int64_t);
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint64_t);

    WJR_INTRINSIC_INLINE static __m256i subs_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i subs_epi16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i subs_epu8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i subs_epu16(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint16_t);

    WJR_INTRINSIC_INLINE static int test_all_ones(__m256i a);

    WJR_INTRINSIC_INLINE static __m256i unpackhi_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i unpackhi_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i unpackhi_epi32(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i unpackhi_epi64(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int64_t);
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint32_t);
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint64_t);

    WJR_INTRINSIC_INLINE static __m256i unpacklo_epi8(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i unpacklo_epi16(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i unpacklo_epi32(__m256i a, __m256i b);
    WJR_INTRINSIC_INLINE static __m256i unpacklo_epi64(__m256i a, __m256i b);

    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int8_t);
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int16_t);
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int32_t);
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int64_t);
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint8_t);
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint16_t);
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint32_t);

#endif // AVX2
};

#if WJR_HAS_SIMD(SSE2)

template <>
struct broadcast_fn<uint8_t, __m128i_t> {
    WJR_INTRINSIC_INLINE __m128i operator()(uint8_t v) const { return _mm_set1_epi8(v); }
};

template <>
struct broadcast_fn<uint16_t, __m128i_t> {
    WJR_INTRINSIC_INLINE __m128i operator()(uint16_t v) const {
        return _mm_set1_epi16(v);
    }
};

template <>
struct broadcast_fn<uint32_t, __m128i_t> {
    WJR_INTRINSIC_INLINE __m128i operator()(uint32_t v) const {
        return _mm_set1_epi32(v);
    }
};

template <>
struct broadcast_fn<uint64_t, __m128i_t> {
    WJR_INTRINSIC_INLINE __m128i operator()(uint64_t v) const {
        return _mm_set1_epi64x(v);
    }
};

template <>
struct broadcast_fn<__m128i_t, __m128i_t> {
    WJR_INTRINSIC_INLINE __m128i operator()(__m128i v) const { return v; }
};

#endif // SSE2

#if WJR_HAS_SIMD(AVX)

template <>
struct broadcast_fn<uint8_t, __m256i_t> {
    WJR_INTRINSIC_INLINE __m256i operator()(uint8_t v) const {
        return _mm256_set1_epi8(v);
    }
};

template <>
struct broadcast_fn<uint16_t, __m256i_t> {
    WJR_INTRINSIC_INLINE __m256i operator()(uint16_t v) const {
        return _mm256_set1_epi16(v);
    }
};

template <>
struct broadcast_fn<uint32_t, __m256i_t> {
    WJR_INTRINSIC_INLINE __m256i operator()(uint32_t v) const {
        return _mm256_set1_epi32(v);
    }
};

template <>
struct broadcast_fn<uint64_t, __m256i_t> {
    WJR_INTRINSIC_INLINE __m256i operator()(uint64_t v) const {
        return _mm256_set1_epi64x(v);
    }
};

template <>
struct broadcast_fn<__m256i_t, __m256i_t> {
    WJR_INTRINSIC_INLINE __m256i operator()(__m256i v) const { return v; }
};

template <>
struct broadcast_fn<__m128i_t, __m256i_t> {
    WJR_INTRINSIC_INLINE __m256i operator()(__m128i v) const {
#if WJR_HAS_SIMD(AVX2)
        return _mm256_broadcastsi128_si256(v);
#else
        return _mm256_insertf128_si256(_mm256_castsi128_si256(v), v, 1);
#endif
    }
};

#endif // AVX

/*------------------------sse------------------------*/

constexpr size_t sse::width() { return 128; }

constexpr sse::mask_type sse::mask() { return 0xFFFF; }

#if WJR_HAS_SIMD(SSE)

sse::mask_type sse::movemask_ps(__m128 v) {
    return static_cast<sse::mask_type>(_mm_movemask_ps(v));
}

void sse::sfence() { return _mm_sfence(); }

template <int imm8>
__m128 sse::shuffle_ps(__m128 a, __m128 b) {
    static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
    return _mm_shuffle_ps(a, b, imm8);
}

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

__m128i sse::add_epi8(__m128i a, __m128i b) { return _mm_add_epi8(a, b); }
__m128i sse::add_epi16(__m128i a, __m128i b) { return _mm_add_epi16(a, b); }
__m128i sse::add_epi32(__m128i a, __m128i b) { return _mm_add_epi32(a, b); }
__m128i sse::add_epi64(__m128i a, __m128i b) { return _mm_add_epi64(a, b); }

__m128i sse::add(__m128i a, __m128i b, int8_t) { return add_epi8(a, b); }
__m128i sse::add(__m128i a, __m128i b, int16_t) { return add_epi16(a, b); }
__m128i sse::add(__m128i a, __m128i b, int32_t) { return add_epi32(a, b); }
__m128i sse::add(__m128i a, __m128i b, int64_t) { return add_epi64(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint8_t) { return add_epi8(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint16_t) { return add_epi16(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint32_t) { return add_epi32(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint64_t) { return add_epi64(a, b); }

int8_t sse::add_epi8(__m128i a) { return static_cast<int8_t>(add_epu8(a)); }
int16_t sse::add_epi16(__m128i a) { return static_cast<int16_t>(add_epu16(a)); }
int32_t sse::add_epi32(__m128i a) { return static_cast<int32_t>(add_epu32(a)); }
int64_t sse::add_epi64(__m128i a) { return static_cast<int64_t>(add_epu64(a)); }

uint8_t sse::add_epu8(__m128i a) {
    auto b = shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a);
    a = add(a, b, uint8_t());
    b = zeros();
    a = sad_epu8(a, b);
    return simd_cast<__m128i_t, uint8_t>(a);
}

uint16_t sse::add_epu16(__m128i a) {
    a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint16_t());
    a = add(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a), uint16_t());
    a = add(a, srli<2>(a), uint16_t());
    return simd_cast<__m128i_t, uint16_t>(a);
}

uint32_t sse::add_epu32(__m128i a) {
    a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint32_t());
    a = add(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a), uint32_t());
    return simd_cast<__m128i_t, uint32_t>(a);
}

uint64_t sse::add_epu64(__m128i a) {
    a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint64_t());
    return simd_cast<__m128i_t, uint64_t>(a);
}

int8_t sse::add(__m128i a, int8_t) { return add_epi8(a); }
int16_t sse::add(__m128i a, int16_t) { return add_epi16(a); }
int32_t sse::add(__m128i a, int32_t) { return add_epi32(a); }
int64_t sse::add(__m128i a, int64_t) { return add_epi64(a); }
uint8_t sse::add(__m128i a, uint8_t) { return add_epu8(a); }
uint16_t sse::add(__m128i a, uint16_t) { return add_epu16(a); }
uint32_t sse::add(__m128i a, uint32_t) { return add_epu32(a); }
uint64_t sse::add(__m128i a, uint64_t) { return add_epu64(a); }

__m128i sse::adds_epi8(__m128i a, __m128i b) { return _mm_adds_epi8(a, b); }
__m128i sse::adds_epi16(__m128i a, __m128i b) { return _mm_adds_epi16(a, b); }

__m128i sse::adds_epu8(__m128i a, __m128i b) { return _mm_adds_epu8(a, b); }
__m128i sse::adds_epu16(__m128i a, __m128i b) { return _mm_adds_epu16(a, b); }

__m128i sse::adds(__m128i a, __m128i b, int8_t) { return adds_epi8(a, b); }
__m128i sse::adds(__m128i a, __m128i b, int16_t) { return adds_epi16(a, b); }
__m128i sse::adds(__m128i a, __m128i b, uint8_t) { return adds_epu8(a, b); }
__m128i sse::adds(__m128i a, __m128i b, uint16_t) { return adds_epu16(a, b); }

template <int imm8>
__m128i sse::alignr(__m128i a, __m128i b) {
    constexpr int s = imm8 & 0x1F;
#if WJR_HAS_SIMD(SSSE3)
    return _mm_alignr_epi8(a, b, s);
#else
    if constexpr (s == 0) {
        return b;
    }
    if constexpr (s == 16) {
        return a;
    }
    if constexpr (s < 16) {
        return Or(slli<16 - s>(a), srli<s>(b));
    }
    return srli<s - 16>(a);
#endif // SSSE3
}

__m128i sse::alignr_epi16(__m128i a, __m128i b, int c) {
    return Or(slli(a, 16 - c, uint16_t()), srli(b, c, uint16_t()));
}

__m128i sse::alignr_epi32(__m128i a, __m128i b, int c) {
    return Or(slli(a, 32 - c, uint32_t()), srli(b, c, uint32_t()));
}

__m128i sse::alignr_epi64(__m128i a, __m128i b, int c) {
    return Or(slli(a, 64 - c, uint64_t()), srli(b, c, uint64_t()));
}

__m128i sse::alignr(__m128i a, __m128i b, int c, int16_t) {
    return alignr_epi16(a, b, c);
}
__m128i sse::alignr(__m128i a, __m128i b, int c, int32_t) {
    return alignr_epi32(a, b, c);
}
__m128i sse::alignr(__m128i a, __m128i b, int c, int64_t) {
    return alignr_epi64(a, b, c);
}
__m128i sse::alignr(__m128i a, __m128i b, int c, uint16_t) {
    return alignr_epi16(a, b, c);
}
__m128i sse::alignr(__m128i a, __m128i b, int c, uint32_t) {
    return alignr_epi32(a, b, c);
}
__m128i sse::alignr(__m128i a, __m128i b, int c, uint64_t) {
    return alignr_epi64(a, b, c);
}

__m128i sse::And(__m128i a, __m128i b) { return _mm_and_si128(a, b); }

__m128i sse::AndNot(__m128i a, __m128i b) { return _mm_andnot_si128(a, b); }

__m128i sse::avg_epu8(__m128i a, __m128i b) { return _mm_avg_epu8(a, b); }
__m128i sse::avg_epu16(__m128i a, __m128i b) { return _mm_avg_epu16(a, b); }

__m128i sse::avg(__m128i a, __m128i b, int8_t) { return avg_epu8(a, b); }
__m128i sse::avg(__m128i a, __m128i b, int16_t) { return avg_epu16(a, b); }
__m128i sse::avg(__m128i a, __m128i b, uint8_t) { return avg_epu8(a, b); }
__m128i sse::avg(__m128i a, __m128i b, uint16_t) { return avg_epu16(a, b); }

// notice that mask must be 0 or 255(every byte)
__m128i sse::blendv_epi8(__m128i a, __m128i b, __m128i mask) {
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_blendv_epi8(a, b, mask);
#elif defined(WJR_COMPILER_GCC)
    return ((~mask) & a) | (mask & b);
#else
    return Or(AndNot(mask, a), And(mask, b));
#endif
}

__m128i sse::blendv_epi16(__m128i a, __m128i b, __m128i mask) {
    return blendv_epi8(b, a, logical_not(mask, uint16_t()));
}

__m128i sse::blendv_epi32(__m128i a, __m128i b, __m128i mask) {
    return blendv_epi8(b, a, logical_not(mask, uint32_t()));
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int8_t) {
    return blendv_epi8(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int16_t) {
    return blendv_epi16(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int32_t) {
    return blendv_epi32(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint8_t) {
    return blendv_epi8(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint16_t) {
    return blendv_epi16(a, b, mask);
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint32_t) {
    return blendv_epi32(a, b, mask);
}

template <int imm8>
__m128i sse::bslli(__m128i val) {
    return _mm_bslli_si128(val, imm8);
}

template <int imm8>
__m128i sse::bsrli(__m128i val) {
    return _mm_bsrli_si128(val, imm8);
}

__m128i sse::cmpeq_epi8(__m128i a, __m128i b) { return _mm_cmpeq_epi8(a, b); }
__m128i sse::cmpeq_epi16(__m128i a, __m128i b) { return _mm_cmpeq_epi16(a, b); }
__m128i sse::cmpeq_epi32(__m128i a, __m128i b) { return _mm_cmpeq_epi32(a, b); }

__m128i sse::cmpeq(__m128i a, __m128i b, int8_t) { return cmpeq_epi8(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int16_t) { return cmpeq_epi16(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int32_t) { return cmpeq_epi32(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint8_t) { return cmpeq_epi8(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint16_t) { return cmpeq_epi16(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint32_t) { return cmpeq_epi32(a, b); }

__m128i sse::cmpge_epi8(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comge_epi8(a, b);
#elif WJR_HAS_SIMD(SSE4_1)
    return cmpeq(min(a, b, int8_t()), b, uint8_t());
#else
    return Not(cmpgt(b, a, int8_t()));
#endif
}

__m128i sse::cmpge_epi16(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comge_epi16(a, b);
#else
    return cmpeq(min(a, b, int16_t()), b, uint16_t());
#endif
}

__m128i sse::cmpge_epi32(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comge_epi32(a, b);
#elif WJR_HAS_SIMD(SSE4_1)
    return cmpeq(min(a, b, int32_t()), b, uint32_t());
#else
    return Not(cmpgt(b, a, int32_t()));
#endif
}

__m128i sse::cmpge_epu8(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comge_epu8(a, b);
#else
    return cmpeq(min(a, b, uint8_t()), b, uint8_t());
#endif
}

__m128i sse::cmpge_epu16(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comge_epu16(a, b);
#elif WJR_HAS_SIMD(SSE4_1)
    return cmpeq(min(a, b, uint16_t()), b, uint16_t());
#else
    return logical_not(subs(b, a, uint16_t()), uint16_t());
#endif
}

__m128i sse::cmpge_epu32(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comge_epu32(a, b);
#elif WJR_HAS_SIMD(SSE4_1)
    return cmpeq(min(a, b, uint32_t()), b, uint32_t());
#else
    return Not(cmpgt(b, a, uint32_t()));
#endif
}

__m128i sse::cmpge(__m128i a, __m128i b, int8_t) { return cmpge_epi8(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, int16_t) { return cmpge_epi16(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, int32_t) { return cmpge_epi32(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint8_t) { return cmpge_epu8(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint16_t) { return cmpge_epu16(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint32_t) { return cmpge_epu32(a, b); }

__m128i sse::cmpgt_epi8(__m128i a, __m128i b) { return _mm_cmpgt_epi8(a, b); }
__m128i sse::cmpgt_epi16(__m128i a, __m128i b) { return _mm_cmpgt_epi16(a, b); }
__m128i sse::cmpgt_epi32(__m128i a, __m128i b) { return _mm_cmpgt_epi32(a, b); }

__m128i sse::cmpgt_epu8(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comgt_epu8(a, b);
#else
    return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
#endif
}

__m128i sse::cmpgt_epu16(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comgt_epu16(a, b);
#else
    return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
#endif
}

__m128i sse::cmpgt_epu32(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comgt_epu32(a, b);
#else
    return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
#endif
}

__m128i sse::cmpgt(__m128i a, __m128i b, int8_t) { return cmpgt_epi8(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, int16_t) { return cmpgt_epi16(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, int32_t) { return cmpgt_epi32(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint8_t) { return cmpgt_epu8(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint16_t) { return cmpgt_epu16(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint32_t) { return cmpgt_epu32(a, b); }

__m128i sse::cmple_epi8(__m128i a, __m128i b) { return cmpge_epi8(b, a); }
__m128i sse::cmple_epi16(__m128i a, __m128i b) { return cmpge_epi16(b, a); }
__m128i sse::cmple_epi32(__m128i a, __m128i b) { return cmpge_epi32(b, a); }

__m128i sse::cmple_epu8(__m128i a, __m128i b) { return cmpge_epu8(b, a); }
__m128i sse::cmple_epu16(__m128i a, __m128i b) { return cmpge_epu16(b, a); }
__m128i sse::cmple_epu32(__m128i a, __m128i b) { return cmpge_epu32(b, a); }

__m128i sse::cmple(__m128i a, __m128i b, int8_t) { return cmple_epi8(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, int16_t) { return cmple_epi16(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, int32_t) { return cmple_epi32(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint8_t) { return cmple_epu8(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint16_t) { return cmple_epu16(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint32_t) { return cmple_epu32(a, b); }

__m128i sse::cmplt_epi8(__m128i a, __m128i b) { return _mm_cmplt_epi8(a, b); }
__m128i sse::cmplt_epi16(__m128i a, __m128i b) { return _mm_cmplt_epi16(a, b); }
__m128i sse::cmplt_epi32(__m128i a, __m128i b) { return _mm_cmplt_epi32(a, b); }

__m128i sse::cmplt_epu8(__m128i a, __m128i b) { return cmpgt_epu8(b, a); }
__m128i sse::cmplt_epu16(__m128i a, __m128i b) { return cmpgt_epu16(b, a); }
__m128i sse::cmplt_epu32(__m128i a, __m128i b) { return cmpgt_epu32(b, a); }

__m128i sse::cmplt(__m128i a, __m128i b, int8_t) { return cmplt_epi8(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, int16_t) { return cmplt_epi16(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, int32_t) { return cmplt_epi32(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint8_t) { return cmplt_epu8(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint16_t) { return cmplt_epu16(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint32_t) { return cmplt_epu32(a, b); }

__m128i sse::cmpne_epi8(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comneq_epi8(a, b);
#else
    return Not(cmpeq_epi8(a, b));
#endif
}

__m128i sse::cmpne_epi16(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comneq_epi16(a, b);
#else
    return Not(cmpeq_epi16(a, b));
#endif
}

__m128i sse::cmpne_epi32(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(XOP)
    return _mm_comneq_epi32(a, b);
#else
    return Not(cmpeq_epi32(a, b));
#endif
}

__m128i sse::cmpne(__m128i a, __m128i b, int8_t) { return cmpne_epi8(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, int16_t) { return cmpne_epi16(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, int32_t) { return cmpne_epi32(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint8_t) { return cmpne_epi8(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint16_t) { return cmpne_epi16(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint32_t) { return cmpne_epi32(a, b); }

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::equal_to<>, T) {
    return cmpeq(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::not_equal_to<>, T) {
    return cmpne(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::greater<>, T) {
    return cmpgt(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::greater_equal<>, T) {
    return cmpge(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::less<>, T) {
    return cmplt(a, b, T());
}

template <typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::less_equal<>, T) {
    return cmple(a, b, T());
}

__m128i sse::concat(uint64_t lo, uint64_t hi) { return set_epi64x(hi, lo); }

template <int imm8>
int sse::extract_epi8(__m128i a) {
    static_assert(imm8 >= 0 && imm8 < 16, "imm8 must be in range [0, 15]");
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_extract_epi8(a, imm8);
#else
    if constexpr (imm8 & 1) {
        return extract_epi16<(imm8 >> 1)>(a) >> 8;
    } else {
        return extract_epi16<(imm8 >> 1)>(a) & 0xff;
    }
#endif
}

template <int imm8>
int sse::extract_epi16(__m128i a) {
    static_assert(imm8 >= 0 && imm8 < 8, "imm8 must be in range [0, 7]");
    return _mm_extract_epi16(a, imm8);
}

template <int imm8>
int sse::extract_epi32(__m128i a) {
    static_assert(imm8 >= 0 && imm8 < 4, "imm8 must be in range [0, 3]");
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_extract_epi32(a, imm8);
#else
    if constexpr (imm8 == 0) {
        return simd_cast<__m128i_t, uint32_t>(a);
    } else if constexpr (imm8 == 1) {
        return static_cast<uint32_t>(simd_cast<__m128i_t, uint64_t>(a) >> 32);
    } else if constexpr (imm8 == 2) {
        return simd_cast<__m128i_t, uint32_t>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    } else {
        return simd_cast<__m128i_t, uint32_t>(shuffle_epi32<_MM_SHUFFLE(3, 3, 3, 3)>(a));
    }
#endif
}

template <int imm8>
int64_t sse::extract_epi64(__m128i a) {
    static_assert(imm8 >= 0 && imm8 < 2, "imm8 must be in range [0, 1]");
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_extract_epi64(a, imm8);
#else
    if constexpr (imm8 == 0) {
        return simd_cast<__m128i_t, uint64_t>(a);
    } else {
        return simd_cast<__m128i_t, uint64_t>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    }
#endif
}

template <int imm8>
int sse::extract(__m128i a, int8_t) {
    return extract_epi8<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, int16_t) {
    return extract_epi16<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, int32_t) {
    return extract_epi32<imm8>(a);
}

template <int imm8>
int64_t sse::extract(__m128i a, int64_t) {
    return extract_epi64<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, uint8_t) {
    return extract_epi8<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, uint16_t) {
    return extract_epi16<imm8>(a);
}

template <int imm8>
int sse::extract(__m128i a, uint32_t) {
    return extract_epi32<imm8>(a);
}

template <int imm8>
int64_t sse::extract(__m128i a, uint64_t) {
    return extract_epi64<imm8>(a);
}

uint64_t sse::getlow(__m128i v) { return simd_cast<__m128i_t, uint64_t>(v); }
uint64_t sse::gethigh(__m128i v) { return extract_epi64<1>(v); }

template <int imm8>
__m128i sse::insert_epi16(__m128i a, int i) {
    return _mm_insert_epi16(a, i, imm8);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, int16_t) {
    return insert_epi16<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, uint16_t) {
    return insert_epi16<imm8>(a, i);
}

void sse::lfence() { _mm_lfence(); }

__m128i sse::load(const __m128i *ptr) { return _mm_load_si128(ptr); }
__m128i sse::loadu(const __m128i *ptr) { return _mm_loadu_si128(ptr); }
__m128i sse::loadu_si16(const void *ptr) {
    return simd_cast<uint16_t, __m128i_t>(read_memory<uint16_t>(ptr));
}

__m128i sse::loadu_si32(const void *ptr) {
    return simd_cast<uint32_t, __m128i_t>(read_memory<uint32_t>(ptr));
}

__m128i sse::loadu_si64(const void *ptr) {
    return simd_cast<uint64_t, __m128i_t>(read_memory<uint64_t>(ptr));
}

template <typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
                                                   uint8_t, uint16_t, uint32_t, uint64_t>,
                                       int>>
__m128i sse::logical_and(__m128i a, __m128i b, T) {
    return Not(Or(logical_not(a, T()), logical_not(b, T())));
}

template <typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
                                                   uint8_t, uint16_t, uint32_t, uint64_t>,
                                       int>>
__m128i sse::logical_not(__m128i v, T) {
    auto Zero = zeros();
    return cmpeq(v, Zero, T());
}

template <typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
                                                   uint8_t, uint16_t, uint32_t, uint64_t>,
                                       int>>
__m128i sse::logical_or(__m128i a, __m128i b, T) {
    return Not(logical_not(Or(a, b), T()));
}

__m128i sse::madd_epi16(__m128i a, __m128i b) { return _mm_madd_epi16(a, b); }

void sse::maskmoveu(__m128i a, __m128i mask, char *mem_addr) {
    return _mm_maskmoveu_si128(a, mask, mem_addr);
}

__m128i sse::max_epi8(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_max_epi8(a, b);
#else
    return blendv_epi8(b, a, cmpgt_epi8(a, b));
#endif
}

__m128i sse::max_epi16(__m128i a, __m128i b) { return _mm_max_epi16(a, b); }

__m128i sse::max_epi32(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_max_epi32(a, b);
#else
    return blendv_epi8(b, a, cmpgt_epi32(a, b));
#endif
}

__m128i sse::max_epu8(__m128i a, __m128i b) { return _mm_max_epu8(a, b); }

__m128i sse::max_epu16(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_max_epu16(a, b);
#else
    return add(subs_epu16(b, a), a, uint16_t());
#endif
}

__m128i sse::max_epu32(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_max_epu32(a, b);
#else
    return blendv_epi8(b, a, cmpgt_epu32(a, b));
#endif
}

__m128i sse::max(__m128i a, __m128i b, int8_t) { return max_epi8(a, b); }
__m128i sse::max(__m128i a, __m128i b, int16_t) { return max_epi16(a, b); }
__m128i sse::max(__m128i a, __m128i b, int32_t) { return max_epi32(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint8_t) { return max_epu8(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint16_t) { return max_epu16(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint32_t) { return max_epu32(a, b); }

int8_t sse::max_epi8(__m128i a) { return 0x7fu ^ min_epu8(Xor(a, set1_epi8(0x7fu))); }

int16_t sse::max_epi16(__m128i a) {
#if WJR_HAS_SIMD(SSE4_1)
    return 0x7fffu ^ min_epu16(Xor(a, set1_epi16(0x7fffu)));
#else
    a = max_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    return simd_cast<__m128i_t, int16_t>(a);
#endif
}

int32_t sse::max_epi32(__m128i a) {
    a = max_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    return simd_cast<__m128i_t, int32_t>(a);
}

uint8_t sse::max_epu8(__m128i a) {
#if WJR_HAS_SIMD(SSE4_1)
    return 0xffu ^ min_epu8(Xor(a, ones()));
#else
    a = max_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    auto X = simd_cast<__m128i_t, uint32_t>(a);
    return std::max((uint8_t)X, (uint8_t)(X >> 8));
#endif
}

uint16_t sse::max_epu16(__m128i a) {
#if WJR_HAS_SIMD(SSE4_1)
    return 0xffffu ^ min_epu16(Xor(a, ones()));
#else
    a = max_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    return simd_cast<__m128i_t, uint16_t>(a);
#endif
}

uint32_t sse::max_epu32(__m128i a) {
    a = max_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = max_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    return simd_cast<__m128i_t, uint32_t>(a);
}

int8_t sse::max(__m128i a, int8_t) { return max_epi8(a); }
int16_t sse::max(__m128i a, int16_t) { return max_epi16(a); }
int32_t sse::max(__m128i a, int32_t) { return max_epi32(a); }
uint8_t sse::max(__m128i a, uint8_t) { return max_epu8(a); }
uint16_t sse::max(__m128i a, uint16_t) { return max_epu16(a); }
uint32_t sse::max(__m128i a, uint32_t) { return max_epu32(a); }

void sse::mfence() { _mm_mfence(); }

__m128i sse::min_epi8(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_min_epi8(a, b);
#else
    return blendv_epi8(a, b, cmpgt_epi8(a, b));
#endif
}

__m128i sse::min_epi16(__m128i a, __m128i b) { return _mm_min_epi16(a, b); }

__m128i sse::min_epi32(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_min_epi32(a, b);
#else
    return blendv_epi8(a, b, cmpgt_epi32(a, b));
#endif
}

__m128i sse::min_epu8(__m128i a, __m128i b) { return _mm_min_epu8(a, b); }

__m128i sse::min_epu16(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_min_epu16(a, b);
#else
    return blendv_epi8(a, b, cmpgt_epu16(a, b));
#endif
}

__m128i sse::min_epu32(__m128i a, __m128i b) {
#if WJR_HAS_SIMD(SSE4_1)
    return _mm_min_epu32(a, b);
#else
    return blendv_epi8(a, b, cmpgt_epu32(a, b));
#endif
}

__m128i sse::min(__m128i a, __m128i b, int8_t) { return min_epi8(a, b); }
__m128i sse::min(__m128i a, __m128i b, int16_t) { return min_epi16(a, b); }
__m128i sse::min(__m128i a, __m128i b, int32_t) { return min_epi32(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint8_t) { return min_epu8(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint16_t) { return min_epu16(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint32_t) { return min_epu32(a, b); }

int8_t sse::min_epi8(__m128i a) { return 0x80u ^ min_epu8(Xor(a, setmin_epi8())); }

int16_t sse::min_epi16(__m128i a) {
#if WJR_HAS_SIMD(SSE4_1)
    return 0x8000u ^ min_epu16(Xor(a, setmin_epi16()));
#else
    a = min_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    return simd_cast<__m128i_t, int16_t>(a);
#endif
}

int32_t sse::min_epi32(__m128i a) {
    a = min_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    return simd_cast<__m128i_t, int32_t>(a);
}

uint8_t sse::min_epu8(__m128i a) {
#if WJR_HAS_SIMD(SSE4_1)
    a = min_epu8(a, srli_epi16(a, 8));
    a = _mm_minpos_epu16(a);
    return simd_cast<__m128i_t, uint8_t>(a);
#else
    a = min_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    auto X = simd_cast<__m128i_t, uint32_t>(a);
    return std::min((uint8_t)X, (uint8_t)(X >> 8));
#endif
}

uint16_t sse::min_epu16(__m128i a) {
#if WJR_HAS_SIMD(SSE4_1)
    return simd_cast<__m128i_t, uint16_t>(_mm_minpos_epu16(a));
#else
    a = min_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
    return simd_cast<__m128i_t, uint16_t>(a);
#endif
}

uint32_t sse::min_epu32(__m128i a) {
    a = min_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
    a = min_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
    return simd_cast<__m128i_t, uint32_t>(a);
}

int8_t sse::min(__m128i a, int8_t) { return min_epi8(a); }
int16_t sse::min(__m128i a, int16_t) { return min_epi16(a); }
int32_t sse::min(__m128i a, int32_t) { return min_epi32(a); }
uint8_t sse::min(__m128i a, uint8_t) { return min_epu8(a); }
uint16_t sse::min(__m128i a, uint16_t) { return min_epu16(a); }
uint32_t sse::min(__m128i a, uint32_t) { return min_epu32(a); }

__m128i sse::move_epi64(__m128i a) { return _mm_move_epi64(a); }

sse::mask_type sse::movemask_epi8(__m128i a) {
    return static_cast<mask_type>(_mm_movemask_epi8(a));
}
sse::mask_type sse::movemask_pd(__m128d v) {
    return static_cast<mask_type>(_mm_movemask_pd(v));
}

sse::mask_type sse::movemask(__m128i v, int8_t) { return movemask_epi8(v); }
sse::mask_type sse::movemask(__m128i v, int32_t) {
    return movemask_ps(simd_cast<__m128i_t, __m128_t>(v));
}
sse::mask_type sse::movemask(__m128i v, int64_t) {
    return movemask_pd(simd_cast<__m128i_t, __m128d_t>(v));
}
sse::mask_type sse::movemask(__m128i v, uint8_t) { return movemask(v, int8_t()); }
sse::mask_type sse::movemask(__m128i v, uint32_t) { return movemask(v, int32_t()); }
sse::mask_type sse::movemask(__m128i v, uint64_t) { return movemask(v, int64_t()); }

__m128i sse::mul_epu32(__m128i a, __m128i b) { return _mm_mul_epu32(a, b); }

__m128i sse::mulhi_epi16(__m128i a, __m128i b) { return _mm_mulhi_epi16(a, b); }

__m128i sse::mulhi_epu16(__m128i a, __m128i b) { return _mm_mulhi_epu16(a, b); }

__m128i sse::mullo_epi16(__m128i a, __m128i b) { return _mm_mullo_epi16(a, b); }

__m128i sse::negate_epi8(__m128i a) {
#if WJR_HAS_SIMD(SSSE3)
    return sign_epi8(a, ones());
#else
    return sub_epi8(zeros(), a);
#endif
}

__m128i sse::negate_epi16(__m128i a) {
#if WJR_HAS_SIMD(SSSE3)
    return sign_epi16(a, ones());
#else
    return sub_epi16(zeros(), a);
#endif
}

__m128i sse::negate_epi32(__m128i a) {
#if WJR_HAS_SIMD(SSSE3)
    return sign_epi32(a, ones());
#else
    return sub_epi32(zeros(), a);
#endif
}

__m128i sse::negate_epi64(__m128i a) { return sub_epi64(zeros(), a); }

__m128i sse::negate(__m128i a, int8_t) { return negate_epi8(a); }
__m128i sse::negate(__m128i a, int16_t) { return negate_epi16(a); }
__m128i sse::negate(__m128i a, int32_t) { return negate_epi32(a); }
__m128i sse::negate(__m128i a, int64_t) { return negate_epi64(a); }
__m128i sse::negate(__m128i a, uint8_t) { return negate_epi8(a); }
__m128i sse::negate(__m128i a, uint16_t) { return negate_epi16(a); }
__m128i sse::negate(__m128i a, uint32_t) { return negate_epi32(a); }
__m128i sse::negate(__m128i a, uint64_t) { return negate_epi64(a); }

__m128i sse::Not(__m128i v) { return Xor(v, ones()); }

__m128i sse::Or(__m128i a, __m128i b) { return _mm_or_si128(a, b); }

__m128i sse::packs_epi16(__m128i a, __m128i b) { return _mm_packs_epi16(a, b); }
__m128i sse::packs_epi32(__m128i a, __m128i b) { return _mm_packs_epi32(a, b); }

__m128i sse::packus_epi16(__m128i a, __m128i b) { return _mm_packus_epi16(a, b); }

__m128i sse::preloadu_si16(const void *ptr) { return loadu_si16(ptr); }
__m128i sse::preloadu_si32(const void *ptr) { return loadu_si32(ptr); }

__m128i sse::preloadu_si48(const void *ptr) {
    return insert_epi16<2>(preloadu_si32(ptr),
                           reinterpret_cast<const uint16_t *>(ptr)[2]);
}

__m128i sse::preloadu_si64(const void *ptr) { return loadu_si64(ptr); }

__m128i sse::preloadu_si80(const void *ptr) {
    return insert_epi16<4>(preloadu_si64(ptr),
                           reinterpret_cast<const uint16_t *>(ptr)[4]);
}

__m128i sse::preloadu_si96(const void *ptr) {
#if WJR_HAS_SIMD(SSE4_1)
    return insert_epi32<2>(preloadu_si64(ptr),
                           reinterpret_cast<const uint32_t *>(ptr)[2]);
#else
    return insert_epi16<5>(preloadu_si80(ptr),
                           reinterpret_cast<const uint16_t *>(ptr)[5]);
#endif
}

__m128i sse::preloadu_si112(const void *ptr) {
    return insert_epi16<6>(preloadu_si96(ptr),
                           reinterpret_cast<const uint16_t *>(ptr)[6]);
}

__m128i sse::preloadu_si128(const void *ptr) { return loadu((__m128i *)ptr); }

__m128i sse::preloadu_si16x(const void *ptr, int n) {
    // preloadu_si(n * 16)
    switch (n) {
    case 0:
        return zeros();
    case 1:
        return preloadu_si16(ptr);
    case 2:
        return preloadu_si32(ptr);
    case 3:
        return preloadu_si48(ptr);
    case 4:
        return preloadu_si64(ptr);
    case 5:
        return preloadu_si80(ptr);
    case 6:
        return preloadu_si96(ptr);
    case 7:
        return preloadu_si112(ptr);
    default:
        return preloadu_si128(ptr);
    }
}

__m128i sse::sad_epu8(__m128i a, __m128i b) { return _mm_sad_epu8(a, b); }

__m128i sse::zeros() { return _mm_setzero_si128(); }
__m128i sse::ones() { return _mm_set1_epi32(-1); }

__m128i sse::set_epi8(char e15, char e14, char e13, char e12, char e11, char e10, char e9,
                      char e8, char e7, char e6, char e5, char e4, char e3, char e2,
                      char e1, char e0) {
    return _mm_set_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1,
                        e0);
}

__m128i sse::set_epi16(short e7, short e6, short e5, short e4, short e3, short e2,
                       short e1, short e0) {
    return _mm_set_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::set_epi32(int e3, int e2, int e1, int e0) {
    return _mm_set_epi32(e3, e2, e1, e0);
}
__m128i sse::set_epi64x(long long e1, long long e0) { return _mm_set_epi64x(e1, e0); }

__m128i sse::setr_epi8(char e15, char e14, char e13, char e12, char e11, char e10,
                       char e9, char e8, char e7, char e6, char e5, char e4, char e3,
                       char e2, char e1, char e0) {
    return _mm_setr_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1,
                         e0);
}

__m128i sse::setr_epi16(short e7, short e6, short e5, short e4, short e3, short e2,
                        short e1, short e0) {
    return _mm_setr_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::setr_epi32(int e3, int e2, int e1, int e0) {
    return _mm_setr_epi32(e3, e2, e1, e0);
}

__m128i sse::set1_epi8(int8_t val) { return _mm_set1_epi8(val); }
__m128i sse::set1_epi16(int16_t val) { return _mm_set1_epi16(val); }
__m128i sse::set1_epi32(int32_t val) { return _mm_set1_epi32(val); }
__m128i sse::set1_epi64(int64_t val) { return _mm_set1_epi64x(val); }

__m128i sse::set1(int8_t val, int8_t) { return set1_epi8(val); }
__m128i sse::set1(int16_t val, int16_t) { return set1_epi16(val); }
__m128i sse::set1(int32_t val, int32_t) { return set1_epi32(val); }
__m128i sse::set1(int64_t val, int64_t) { return set1_epi64(val); }
__m128i sse::set1(uint8_t val, uint8_t) { return set1_epi8(val); }
__m128i sse::set1(uint16_t val, uint16_t) { return set1_epi16(val); }
__m128i sse::set1(uint32_t val, uint32_t) { return set1_epi32(val); }
__m128i sse::set1(uint64_t val, uint64_t) { return set1_epi64(val); }

__m128i sse::setmin_epi8() { return set1_epi8(0x80u); }
__m128i sse::setmin_epi16() { return set1_epi16(0x8000u); }
__m128i sse::setmin_epi32() { return set1_epi32(0x80000000u); }

__m128i sse::setmin(int8_t) { return setmin_epi8(); }
__m128i sse::setmin(int16_t) { return setmin_epi16(); }
__m128i sse::setmin(int32_t) { return setmin_epi32(); }
__m128i sse::setmin(uint8_t) { return set1_epi32(0); }
__m128i sse::setmin(uint16_t) { return set1_epi32(0); }
__m128i sse::setmin(uint32_t) { return set1_epi32(0); }

__m128i sse::setmax_epi8() { return set1_epi8(0x7F); }
__m128i sse::setmax_epi16() { return set1_epi16(0x7FFF); }
__m128i sse::setmax_epi32() { return set1_epi32(0x7FFFFFFF); }

__m128i sse::setmax(int8_t) { return setmax_epi8(); }
__m128i sse::setmax(int16_t) { return setmax_epi16(); }
__m128i sse::setmax(int32_t) { return setmax_epi32(); }
__m128i sse::setmax(uint8_t) { return set1_epi32(0xFFFFFFFF); }
__m128i sse::setmax(uint16_t) { return set1_epi32(0xFFFFFFFF); }
__m128i sse::setmax(uint32_t) { return set1_epi32(0xFFFFFFFF); }

template <int imm>
__m128i sse::shl(__m128i a) {
    if constexpr (imm >= 64) {
        a = slli<8>(a);
        a = slli_epi64(a, imm - 64);
        return a;
    } else {
        auto b = slli_epi64(a, imm);
        auto c = slli<8>(a);
        c = srli_epi64(c, 64 - imm);
        return Or(b, c);
    }
}

template <int imm>
__m128i sse::shr(__m128i a) {
    if constexpr (imm >= 64) {
        a = srli<8>(a);
        a = srli_epi64(a, imm - 64);
        return a;
    } else {
        auto b = srli_epi64(a, imm);
        auto c = srli<8>(a);
        c = slli_epi64(c, 64 - imm);
        return Or(b, c);
    }
}

template <int imm8>
__m128i sse::shuffle_epi32(__m128i v) {
    static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
    return _mm_shuffle_epi32(v, imm8);
}

template <int imm8>
__m128i sse::shufflehi_epi16(__m128i v) {
    return _mm_shufflehi_epi16(v, imm8);
}

template <int imm8>
__m128i sse::shufflelo_epi16(__m128i v) {
    return _mm_shufflelo_epi16(v, imm8);
}

__m128i sse::sll_epi16(__m128i a, __m128i b) { return _mm_sll_epi16(a, b); }
__m128i sse::sll_epi32(__m128i a, __m128i b) { return _mm_sll_epi32(a, b); }
__m128i sse::sll_epi64(__m128i a, __m128i b) { return _mm_sll_epi64(a, b); }

__m128i sse::sll(__m128i a, __m128i b, int16_t) { return sll_epi16(a, b); }
__m128i sse::sll(__m128i a, __m128i b, int32_t) { return sll_epi32(a, b); }
__m128i sse::sll(__m128i a, __m128i b, int64_t) { return sll_epi64(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint64_t) { return sll_epi64(a, b); }

template <int imm8>
__m128i sse::slli(__m128i v) {
    return _mm_slli_si128(v, imm8);
}
__m128i sse::slli_epi16(__m128i a, int imm8) {
    if (WJR_BUILTIN_CONSTANT_P(imm8 == 1) && imm8 == 1) {
        return sse::add_epi16(a, a);
    }

    return _mm_slli_epi16(a, imm8);
}
__m128i sse::slli_epi32(__m128i a, int imm8) {
    if (WJR_BUILTIN_CONSTANT_P(imm8 == 1) && imm8 == 1) {
        return sse::add_epi32(a, a);
    }

    return _mm_slli_epi32(a, imm8);
}
__m128i sse::slli_epi64(__m128i a, int imm8) {
    if (WJR_BUILTIN_CONSTANT_P(imm8 == 1) && imm8 == 1) {
        return sse::add_epi64(a, a);
    }

    return _mm_slli_epi64(a, imm8);
}

__m128i sse::slli(__m128i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }

__m128i sse::sra_epi16(__m128i a, __m128i b) { return _mm_sra_epi16(a, b); }
__m128i sse::sra_epi32(__m128i a, __m128i b) { return _mm_sra_epi32(a, b); }

__m128i sse::sra(__m128i a, __m128i b, int16_t) { return sra_epi16(a, b); }
__m128i sse::sra(__m128i a, __m128i b, int32_t) { return sra_epi32(a, b); }

__m128i sse::srai_epi16(__m128i a, int imm8) { return _mm_srai_epi16(a, imm8); }
__m128i sse::srai_epi32(__m128i a, int imm8) { return _mm_srai_epi32(a, imm8); }

__m128i sse::srai(__m128i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
__m128i sse::srai(__m128i a, int imm8, int32_t) { return srai_epi32(a, imm8); }

__m128i sse::srl_epi16(__m128i a, __m128i b) { return _mm_srl_epi16(a, b); }
__m128i sse::srl_epi32(__m128i a, __m128i b) { return _mm_srl_epi32(a, b); }
__m128i sse::srl_epi64(__m128i a, __m128i b) { return _mm_srl_epi64(a, b); }

__m128i sse::srl(__m128i a, __m128i b, int16_t) { return srl_epi16(a, b); }
__m128i sse::srl(__m128i a, __m128i b, int32_t) { return srl_epi32(a, b); }
__m128i sse::srl(__m128i a, __m128i b, int64_t) { return srl_epi64(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint64_t) { return srl_epi64(a, b); }

template <int imm8>
__m128i sse::srli(__m128i v) {
    return _mm_srli_si128(v, imm8);
}
__m128i sse::srli_epi16(__m128i a, int imm8) { return _mm_srli_epi16(a, imm8); }
__m128i sse::srli_epi32(__m128i a, int imm8) { return _mm_srli_epi32(a, imm8); }
__m128i sse::srli_epi64(__m128i a, int imm8) { return _mm_srli_epi64(a, imm8); }

__m128i sse::srli(__m128i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }

void sse::stream(__m128i *ptr, __m128i v) { _mm_stream_si128(ptr, v); }

void sse::store(__m128i *ptr, __m128i val) { _mm_store_si128(ptr, val); }
void sse::storeu(__m128i *ptr, __m128i val) { _mm_storeu_si128(ptr, val); }

__m128i sse::sub_epi8(__m128i a, __m128i b) { return _mm_sub_epi8(a, b); }
__m128i sse::sub_epi16(__m128i a, __m128i b) { return _mm_sub_epi16(a, b); }
__m128i sse::sub_epi32(__m128i a, __m128i b) { return _mm_sub_epi32(a, b); }
__m128i sse::sub_epi64(__m128i a, __m128i b) { return _mm_sub_epi64(a, b); }

__m128i sse::sub(__m128i a, __m128i b, int8_t) { return sub_epi8(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int16_t) { return sub_epi16(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int32_t) { return sub_epi32(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int64_t) { return sub_epi64(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint8_t) { return sub_epi8(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint16_t) { return sub_epi16(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint32_t) { return sub_epi32(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint64_t) { return sub_epi64(a, b); }

__m128i sse::subs_epi8(__m128i a, __m128i b) { return _mm_subs_epi8(a, b); }
__m128i sse::subs_epi16(__m128i a, __m128i b) { return _mm_subs_epi16(a, b); }

__m128i sse::subs_epu8(__m128i a, __m128i b) { return _mm_subs_epu8(a, b); }
__m128i sse::subs_epu16(__m128i a, __m128i b) { return _mm_subs_epu16(a, b); }

__m128i sse::subs(__m128i a, __m128i b, int8_t) { return subs_epi8(a, b); }
__m128i sse::subs(__m128i a, __m128i b, int16_t) { return subs_epi16(a, b); }
__m128i sse::subs(__m128i a, __m128i b, uint8_t) { return subs_epu8(a, b); }
__m128i sse::subs(__m128i a, __m128i b, uint16_t) { return subs_epu16(a, b); }

__m128i sse::unpackhi_epi8(__m128i a, __m128i b) { return _mm_unpackhi_epi8(a, b); }
__m128i sse::unpackhi_epi16(__m128i a, __m128i b) { return _mm_unpackhi_epi16(a, b); }
__m128i sse::unpackhi_epi32(__m128i a, __m128i b) { return _mm_unpackhi_epi32(a, b); }
__m128i sse::unpackhi_epi64(__m128i a, __m128i b) { return _mm_unpackhi_epi64(a, b); }

__m128i sse::unpackhi(__m128i a, __m128i b, int8_t) { return unpackhi_epi8(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int16_t) { return unpackhi_epi16(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int32_t) { return unpackhi_epi32(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int64_t) { return unpackhi_epi64(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint8_t) { return unpackhi_epi8(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint16_t) { return unpackhi_epi16(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint32_t) { return unpackhi_epi32(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint64_t) { return unpackhi_epi64(a, b); }

__m128i sse::unpacklo_epi8(__m128i a, __m128i b) { return _mm_unpacklo_epi8(a, b); }
__m128i sse::unpacklo_epi16(__m128i a, __m128i b) { return _mm_unpacklo_epi16(a, b); }
__m128i sse::unpacklo_epi32(__m128i a, __m128i b) { return _mm_unpacklo_epi32(a, b); }
__m128i sse::unpacklo_epi64(__m128i a, __m128i b) { return _mm_unpacklo_epi64(a, b); }

__m128i sse::unpacklo(__m128i a, __m128i b, int8_t) { return unpacklo_epi8(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int16_t) { return unpacklo_epi16(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int32_t) { return unpacklo_epi32(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int64_t) { return unpacklo_epi64(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint8_t) { return unpacklo_epi8(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint16_t) { return unpacklo_epi16(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint32_t) { return unpacklo_epi32(a, b); }

__m128i sse::Xor(__m128i a, __m128i b) { return _mm_xor_si128(a, b); }

#endif

#if WJR_HAS_SIMD(SSE3)

__m128i sse::lddqu(const __m128i *ptr) { return _mm_lddqu_si128(ptr); }

#endif

#if WJR_HAS_SIMD(SSSE3)

__m128i sse::abs_epi8(__m128i val) { return _mm_abs_epi8(val); }
__m128i sse::abs_epi16(__m128i val) { return _mm_abs_epi16(val); }
__m128i sse::abs_epi32(__m128i val) { return _mm_abs_epi32(val); }

__m128i sse::abs(__m128i val, int8_t) { return abs_epi8(val); }
__m128i sse::abs(__m128i val, int16_t) { return abs_epi16(val); }
__m128i sse::abs(__m128i val, int32_t) { return abs_epi32(val); }
__m128i sse::abs(__m128i val, uint8_t) { return val; }
__m128i sse::abs(__m128i val, uint16_t) { return val; }
__m128i sse::abs(__m128i val, uint32_t) { return val; }

__m128i sse::shuffle_epi8(__m128i v, __m128i imm8) { return _mm_shuffle_epi8(v, imm8); }

__m128i sse::sign_epi8(__m128i a, __m128i b) { return _mm_sign_epi8(a, b); }
__m128i sse::sign_epi16(__m128i a, __m128i b) { return _mm_sign_epi16(a, b); }
__m128i sse::sign_epi32(__m128i a, __m128i b) { return _mm_sign_epi32(a, b); }

__m128i sse::sign(__m128i a, __m128i b, int8_t) { return sign_epi8(a, b); }
__m128i sse::sign(__m128i a, __m128i b, int16_t) { return sign_epi16(a, b); }
__m128i sse::sign(__m128i a, __m128i b, int32_t) { return sign_epi32(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint8_t) { return sign_epi8(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint16_t) { return sign_epi16(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint32_t) { return sign_epi32(a, b); }

#endif

#if WJR_HAS_SIMD(SSE4_1)

template <int imm8>
__m128i sse::blend_epi16(__m128i a, __m128i b) {
    return _mm_blend_epi16(a, b, imm8);
}

__m128i sse::cmpeq_epi64(__m128i a, __m128i b) { return _mm_cmpeq_epi64(a, b); }

__m128i sse::cmpeq(__m128i a, __m128i b, int64_t) { return cmpeq_epi64(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint64_t) { return cmpeq_epi64(a, b); }

__m128i sse::cmpgt_epi64(__m128i a, __m128i b) { return _mm_cmpgt_epi64(a, b); }

__m128i sse::cmpgt(__m128i a, __m128i b, int64_t) { return cmpgt_epi64(a, b); }

template <int imm8>
__m128i sse::insert_epi8(__m128i a, int i) {
    return _mm_insert_epi8(a, i, imm8);
}

template <int imm8>
__m128i sse::insert_epi32(__m128i a, int i) {
    return _mm_insert_epi32(a, i, imm8);
}

template <int imm8>
__m128i sse::insert_epi64(__m128i a, int64_t i) {
    return _mm_insert_epi64(a, i, imm8);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, int8_t) {
    return insert_epi8<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, int32_t) {
    return insert_epi32<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int64_t i, int64_t) {
    return insert_epi64<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, uint8_t) {
    return insert_epi8<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int i, uint32_t) {
    return insert_epi32<imm8>(a, i);
}

template <int imm8>
__m128i sse::insert(__m128i a, int64_t i, uint64_t) {
    return insert_epi64<imm8>(a, i);
}

__m128i sse::minpos_epu16(__m128i a) { return _mm_minpos_epu16(a); }

__m128i sse::mul_epi32(__m128i a, __m128i b) { return _mm_mul_epi32(a, b); }

__m128i sse::mullo_epi32(__m128i a, __m128i b) { return _mm_mullo_epi32(a, b); }

__m128i sse::packus_epi32(__m128i a, __m128i b) { return _mm_packus_epi32(a, b); }

__m128i sse::stream_load(__m128i *p) { return _mm_stream_load_si128(p); }

int sse::test_all_ones(__m128i a) { return _mm_test_all_ones(a); }

int sse::test_all_zeros(__m128i a, __m128i b) { return _mm_test_all_zeros(a, b); }

int sse::test_all_zeros(__m128i a) { return _mm_test_all_zeros(a, a); }

int sse::test_mix_ones_zeros(__m128i a, __m128i b) {
    return _mm_test_mix_ones_zeros(a, b);
}

int sse::testc(__m128i a, __m128i b) { return _mm_testc_si128(a, b); }

int sse::testnzc(__m128i a, __m128i b) { return _mm_testnzc_si128(a, b); }

int sse::testz(__m128i a, __m128i b) { return _mm_testz_si128(a, b); }

#endif

/*------------------------avx------------------------*/

constexpr size_t avx::width() { return 256; }

constexpr avx::mask_type avx::mask() { return 0xffffffff; }

#if WJR_HAS_SIMD(AVX)

__m256i avx::concat(__m128i a, __m128i b) {
    return insert_si128<1>(simd_cast<__m128i_t, __m256i_t>(a), b);
}

template <int imm8>
int avx::extract_epi32(__m256i v) {
    return _mm256_extract_epi32(v, imm8);
}

template <int imm8>
int64_t avx::extract_epi64(__m256i v) {
    return _mm256_extract_epi64(v, imm8);
}

template <int imm8>
int avx::extract(__m256i v, int32_t) {
    return extract_epi32<imm8>(v);
}

template <int imm8>
int64_t avx::extract(__m256i v, int64_t) {
    return extract_epi64<imm8>(v);
}

template <int imm8>
__m128i avx::extract_si128(__m256i v) {
#if WJR_HAS_SIMD(AV2)
    return _mm256_extracti128_si256(v, imm8);
#else
    return _mm256_extractf128_si256(v, imm8);
#endif
}

__m128i avx::getlow(__m256i a) { return simd_cast<__m256i_t, __m128i_t>(a); }

__m128i avx::gethigh(__m256i a) { return extract_si128<1>(a); }

template <int imm8>
__m256i avx::insert_epi8(__m256i v, int8_t i) {
    return _mm256_insert_epi8(v, i, imm8);
}

template <int imm8>
__m256i avx::insert_epi16(__m256i v, int16_t i) {
    return _mm256_insert_epi16(v, i, imm8);
}

template <int imm8>
__m256i avx::insert_epi32(__m256i v, int32_t i) {
    return _mm256_insert_epi32(v, i, imm8);
}

template <int imm8>
__m256i avx::insert_epi64(__m256i v, int64_t i) {
    return _mm256_insert_epi64(v, i, imm8);
}

template <int imm8>
__m256i avx::insert_si128(__m256i a, __m128i b) {
#if WJR_HAS_SIMD(AVX2)
    return _mm256_inserti128_si256(a, b, imm8);
#else
    return _mm256_insertf128_si256(a, b, imm8);
#endif
}

__m256i avx::load(const __m256i *p) { return _mm256_load_si256(p); }
__m256i avx::loadu(const __m256i *p) { return _mm256_loadu_si256(p); }

__m256i avx::ones() { return _mm256_set1_epi32(-1); }

__m256i avx::preloadu_si16(const void *ptr) {
    return simd_cast<__m128i_t, __m256i_t>(sse::preloadu_si16(ptr));
}

__m256i avx::preloadu_si32(const void *ptr) {
    return simd_cast<__m128i_t, __m256i_t>(sse::preloadu_si32(ptr));
}

__m256i avx::preloadu_si48(const void *ptr) {
    return simd_cast<__m128i_t, __m256i_t>(sse::preloadu_si48(ptr));
}

__m256i avx::preloadu_si64(const void *ptr) {
    return simd_cast<__m128i_t, __m256i_t>(sse::preloadu_si64(ptr));
}

__m256i avx::preloadu_si80(const void *ptr) {
    return simd_cast<__m128i_t, __m256i_t>(sse::preloadu_si80(ptr));
}

__m256i avx::preloadu_si96(const void *ptr) {
    return simd_cast<__m128i_t, __m256i_t>(sse::preloadu_si96(ptr));
}

__m256i avx::preloadu_si112(const void *ptr) {
    return simd_cast<__m128i_t, __m256i_t>(sse::preloadu_si112(ptr));
}

__m256i avx::preloadu_si128(const void *ptr) {
    return simd_cast<__m128i_t, __m256i_t>(sse::preloadu_si128(ptr));
}

__m256i avx::preloadu_si144(const void *ptr) {
    return concat(sse::preloadu_si128(ptr), sse::preloadu_si16((const char *)ptr + 16));
}

__m256i avx::preloadu_si160(const void *ptr) {
    return concat(sse::preloadu_si128(ptr), sse::preloadu_si32((const char *)ptr + 16));
}

__m256i avx::preloadu_si176(const void *ptr) {
    return concat(sse::preloadu_si128(ptr), sse::preloadu_si48((const char *)ptr + 16));
}

__m256i avx::preloadu_si192(const void *ptr) {
    return concat(sse::preloadu_si128(ptr), sse::preloadu_si64((const char *)ptr + 16));
}

__m256i avx::preloadu_si208(const void *ptr) {
    return concat(sse::preloadu_si128(ptr), sse::preloadu_si80((const char *)ptr + 16));
}

__m256i avx::preloadu_si224(const void *ptr) {
    return concat(sse::preloadu_si128(ptr), sse::preloadu_si96((const char *)ptr + 16));
}
__m256i avx::preloadu_si240(const void *ptr) {
    return concat(sse::preloadu_si128(ptr), sse::preloadu_si112((const char *)ptr + 16));
}

__m256i avx::preloadu_si256(const void *ptr) { return loadu((const __m256i *)ptr); }

__m256i avx::preloadu_si16x(const void *ptr, int n) {
    switch (n) {
    case 0:
        return zeros();
    case 1:
        return preloadu_si16(ptr);
    case 2:
        return preloadu_si32(ptr);
    case 3:
        return preloadu_si48(ptr);
    case 4:
        return preloadu_si64(ptr);
    case 5:
        return preloadu_si80(ptr);
    case 6:
        return preloadu_si96(ptr);
    case 7:
        return preloadu_si112(ptr);
    case 8:
        return preloadu_si128(ptr);
    case 9:
        return preloadu_si144(ptr);
    case 10:
        return preloadu_si160(ptr);
    case 11:
        return preloadu_si176(ptr);
    case 12:
        return preloadu_si192(ptr);
    case 13:
        return preloadu_si208(ptr);
    case 14:
        return preloadu_si224(ptr);
    case 15:
        return preloadu_si240(ptr);
    default:
        return preloadu_si256(ptr);
    }
}

__m256i avx::set_epi8(char e31, char e30, char e29, char e28, char e27, char e26,
                      char e25, char e24, char e23, char e22, char e21, char e20,
                      char e19, char e18, char e17, char e16, char e15, char e14,
                      char e13, char e12, char e11, char e10, char e9, char e8, char e7,
                      char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
    return _mm256_set_epi8(e31, e30, e29, e28, e27, e26, e25, e24, e23, e22, e21, e20,
                           e19, e18, e17, e16, e15, e14, e13, e12, e11, e10, e9, e8, e7,
                           e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::set_epi16(short e15, short e14, short e13, short e12, short e11, short e10,
                       short e9, short e8, short e7, short e6, short e5, short e4,
                       short e3, short e2, short e1, short e0) {
    return _mm256_set_epi16(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2,
                            e1, e0);
}

__m256i avx::set_epi32(int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
    return _mm256_set_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::set_epi64x(long long e3, long long e2, long long e1, long long e0) {
    return _mm256_set_epi64x(e3, e2, e1, e0);
}

__m256i avx::setr_epi8(char e31, char e30, char e29, char e28, char e27, char e26,
                       char e25, char e24, char e23, char e22, char e21, char e20,
                       char e19, char e18, char e17, char e16, char e15, char e14,
                       char e13, char e12, char e11, char e10, char e9, char e8, char e7,
                       char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
    return _mm256_setr_epi8(e31, e30, e29, e28, e27, e26, e25, e24, e23, e22, e21, e20,
                            e19, e18, e17, e16, e15, e14, e13, e12, e11, e10, e9, e8, e7,
                            e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::setr_epi16(short e15, short e14, short e13, short e12, short e11, short e10,
                        short e9, short e8, short e7, short e6, short e5, short e4,
                        short e3, short e2, short e1, short e0) {
    return _mm256_setr_epi16(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2,
                             e1, e0);
}

__m256i avx::setr_epi32(int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
    return _mm256_setr_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::setr_epi64x(long long e3, long long e2, long long e1, long long e0) {
    return _mm256_setr_epi64x(e3, e2, e1, e0);
}

__m256i avx::set1_epi8(int8_t a) { return _mm256_set1_epi8(a); }
__m256i avx::set1_epi16(int16_t a) { return _mm256_set1_epi16(a); }
__m256i avx::set1_epi32(int32_t a) { return _mm256_set1_epi32(a); }
__m256i avx::set1_epi64(int64_t a) { return _mm256_set1_epi64x(a); }

__m256i avx::set1(int8_t a, int8_t) { return set1_epi8(a); }
__m256i avx::set1(int16_t a, int16_t) { return set1_epi16(a); }
__m256i avx::set1(int32_t a, int32_t) { return set1_epi32(a); }
__m256i avx::set1(int64_t a, int64_t) { return set1_epi64(a); }
__m256i avx::set1(uint8_t a, uint8_t) { return set1_epi8(a); }
__m256i avx::set1(uint16_t a, uint16_t) { return set1_epi16(a); }
__m256i avx::set1(uint32_t a, uint32_t) { return set1_epi32(a); }
__m256i avx::set1(uint64_t a, uint64_t) { return set1_epi64(a); }

__m256i avx::setmin_epi8() { return set1_epi8(0x80u); }
__m256i avx::setmin_epi16() { return set1_epi16(0x8000u); }
__m256i avx::setmin_epi32() { return set1_epi32(0x80000000u); }
__m256i avx::setmin_epi64() { return set1_epi64(0x8000000000000000ull); }

__m256i avx::setmin(int8_t) { return setmin_epi8(); }
__m256i avx::setmin(int16_t) { return setmin_epi16(); }
__m256i avx::setmin(int32_t) { return setmin_epi32(); }
__m256i avx::setmin(int64_t) { return setmin_epi64(); }

__m256i avx::setmax_epi8() { return set1_epi8(0x7f); }
__m256i avx::setmax_epi16() { return set1_epi16(0x7fff); }
__m256i avx::setmax_epi32() { return set1_epi32(0x7fffffff); }
__m256i avx::setmax_epi64() { return set1_epi64(0x7fffffffffffffff); }

__m256i avx::setmax(int8_t) { return setmax_epi8(); }
__m256i avx::setmax(int16_t) { return setmax_epi16(); }
__m256i avx::setmax(int32_t) { return setmax_epi32(); }
__m256i avx::setmax(int64_t) { return setmax_epi64(); }

void avx::stream(__m256i *p, __m256i a) { _mm256_stream_si256(p, a); }

void avx::store(__m256i *p, __m256i a) { _mm256_store_si256(p, a); }
void avx::storeu(__m256i *p, __m256i a) { _mm256_storeu_si256(p, a); }

int avx::test_all_zeros(__m256i a) { return testz(a, a); }

int avx::testc(__m256i a, __m256i b) { return _mm256_testc_si256(a, b); }

int avx::testnzc(__m256i a, __m256i b) { return _mm256_testnzc_si256(a, b); }

int avx::testz(__m256i a, __m256i b) { return _mm256_testz_si256(a, b); }

__m256i avx::zeros() { return _mm256_setzero_si256(); }

#endif

#if WJR_HAS_SIMD(AVX2)

__m256i avx::And(__m256i a, __m256i b) { return _mm256_and_si256(a, b); }

__m256i avx::AndNot(__m256i a, __m256i b) { return _mm256_andnot_si256(a, b); }

__m256i avx::Or(__m256i a, __m256i b) { return _mm256_or_si256(a, b); }

__m256i avx::Xor(__m256i a, __m256i b) { return _mm256_xor_si256(a, b); }

__m256i avx::Not(__m256i v) { return _mm256_xor_si256(v, ones()); }

__m256i avx::abs_epi8(__m256i v) { return _mm256_abs_epi8(v); }
__m256i avx::abs_epi16(__m256i v) { return _mm256_abs_epi16(v); }
__m256i avx::abs_epi32(__m256i v) { return _mm256_abs_epi32(v); }

__m256i avx::abs(__m256i v, int8_t) { return abs_epi8(v); }
__m256i avx::abs(__m256i v, int16_t) { return abs_epi16(v); }
__m256i avx::abs(__m256i v, int32_t) { return abs_epi32(v); }
__m256i avx::abs(__m256i v, int64_t) { return abs_epi32(v); }

__m256i avx::add_epi8(__m256i a, __m256i b) { return _mm256_add_epi8(a, b); }
__m256i avx::add_epi16(__m256i a, __m256i b) { return _mm256_add_epi16(a, b); }
__m256i avx::add_epi32(__m256i a, __m256i b) { return _mm256_add_epi32(a, b); }
__m256i avx::add_epi64(__m256i a, __m256i b) { return _mm256_add_epi64(a, b); }

__m256i avx::add(__m256i a, __m256i b, int8_t) { return add_epi8(a, b); }
__m256i avx::add(__m256i a, __m256i b, int16_t) { return add_epi16(a, b); }
__m256i avx::add(__m256i a, __m256i b, int32_t) { return add_epi32(a, b); }
__m256i avx::add(__m256i a, __m256i b, int64_t) { return add_epi64(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint8_t) { return add_epi8(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint16_t) { return add_epi16(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint32_t) { return add_epi32(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint64_t) { return add_epi64(a, b); }

uint8_t avx::add_epu8(__m256i v) {
    return sse::add_epu8(sse::add_epi8(getlow(v), gethigh(v)));
}

uint16_t avx::add_epu16(__m256i v) {
    return sse::add_epu16(sse::add_epi16(getlow(v), gethigh(v)));
}

uint32_t avx::add_epu32(__m256i v) {
    return sse::add_epu32(sse::add_epi32(getlow(v), gethigh(v)));
}

uint64_t avx::add_epu64(__m256i v) {
    return sse::add_epu64(sse::add_epi64(getlow(v), gethigh(v)));
}

int8_t avx::add_epi8(__m256i v) { return add_epu8(v); }
int16_t avx::add_epi16(__m256i v) { return add_epu16(v); }
int32_t avx::add_epi32(__m256i v) { return add_epu32(v); }
int64_t avx::add_epi64(__m256i v) { return add_epu64(v); }

int8_t avx::add(__m256i v, int8_t) { return add_epi8(v); }
int16_t avx::add(__m256i v, int16_t) { return add_epi16(v); }
int32_t avx::add(__m256i v, int32_t) { return add_epi32(v); }
int64_t avx::add(__m256i v, int64_t) { return add_epi64(v); }
uint8_t avx::add(__m256i v, uint8_t) { return add_epu8(v); }
uint16_t avx::add(__m256i v, uint16_t) { return add_epu16(v); }
uint32_t avx::add(__m256i v, uint32_t) { return add_epu32(v); }
uint64_t avx::add(__m256i v, uint64_t) { return add_epu64(v); }

__m256i avx::adds_epi8(__m256i a, __m256i b) { return _mm256_adds_epi8(a, b); }
__m256i avx::adds_epi16(__m256i a, __m256i b) { return _mm256_adds_epi16(a, b); }

__m256i avx::adds_epu8(__m256i a, __m256i b) { return _mm256_adds_epu8(a, b); }
__m256i avx::adds_epu16(__m256i a, __m256i b) { return _mm256_adds_epu16(a, b); }

__m256i avx::adds(__m256i a, __m256i b, int8_t) { return adds_epi8(a, b); }
__m256i avx::adds(__m256i a, __m256i b, int16_t) { return adds_epi16(a, b); }
__m256i avx::adds(__m256i a, __m256i b, uint8_t) { return adds_epu8(a, b); }
__m256i avx::adds(__m256i a, __m256i b, uint16_t) { return adds_epu16(a, b); }

template <int imm8>
__m256i avx::alignr(__m256i a, __m256i b) {
    return _mm256_alignr_epi8(a, b, imm8);
}

__m256i avx::alignr_epi16(__m256i a, __m256i b, int c) {
    return Or(slli_epi16(a, 16 - c), srli_epi16(b, c));
}

__m256i avx::alignr_epi32(__m256i a, __m256i b, int c) {
    return Or(slli_epi32(a, 32 - c), srli_epi32(b, c));
}

__m256i avx::alignr_epi64(__m256i a, __m256i b, int c) {
    return Or(slli_epi64(a, 64 - c), srli_epi64(b, c));
}

__m256i avx::alignr(__m256i a, __m256i b, int c, int16_t) {
    return alignr_epi16(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, int32_t) {
    return alignr_epi32(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, int64_t) {
    return alignr_epi64(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, uint16_t) {
    return alignr_epi16(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, uint32_t) {
    return alignr_epi32(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, uint64_t) {
    return alignr_epi64(a, b, c);
}

__m256i avx::avg_epu8(__m256i a, __m256i b) { return _mm256_avg_epu8(a, b); }
__m256i avx::avg_epu16(__m256i a, __m256i b) { return _mm256_avg_epu16(a, b); }

__m256i avx::avg(__m256i a, __m256i b, int8_t) { return avg_epu8(a, b); }
__m256i avx::avg(__m256i a, __m256i b, int16_t) { return avg_epu16(a, b); }
__m256i avx::avg(__m256i a, __m256i b, uint8_t) { return avg_epu8(a, b); }
__m256i avx::avg(__m256i a, __m256i b, uint16_t) { return avg_epu16(a, b); }

template <int imm8>
__m256i avx::blend_epi16(__m256i a, __m256i b) {
    return _mm256_blend_epi16(a, b, imm8);
}

template <int imm8>
__m256i avx::blend_epi32(__m256i a, __m256i b) {
    return _mm256_blend_epi32(a, b, imm8);
}

__m256i avx::blendv_epi8(__m256i a, __m256i b, __m256i mask) {
    return _mm256_blendv_epi8(a, b, mask);
}

template <int imm8>
__m256i avx::bslli_epi128(__m256i a) {
    return _mm256_bslli_epi128(a, imm8);
}

template <int imm8>
__m256i avx::bsrli_epi128(__m256i a) {
    return _mm256_bsrli_epi128(a, imm8);
}

__m256i avx::cmpeq_epi8(__m256i a, __m256i b) { return _mm256_cmpeq_epi8(a, b); }
__m256i avx::cmpeq_epi16(__m256i a, __m256i b) { return _mm256_cmpeq_epi16(a, b); }
__m256i avx::cmpeq_epi32(__m256i a, __m256i b) { return _mm256_cmpeq_epi32(a, b); }
__m256i avx::cmpeq_epi64(__m256i a, __m256i b) { return _mm256_cmpeq_epi64(a, b); }

__m256i avx::cmpeq(__m256i a, __m256i b, int8_t) { return cmpeq_epi8(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int16_t) { return cmpeq_epi16(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int32_t) { return cmpeq_epi32(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int64_t) { return cmpeq_epi64(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint8_t) { return cmpeq_epi8(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint16_t) { return cmpeq_epi16(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint32_t) { return cmpeq_epi32(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint64_t) { return cmpeq_epi64(a, b); }

__m256i avx::cmpge_epi8(__m256i a, __m256i b) {
    return cmpeq(min(a, b, int8_t()), b, uint8_t());
}

__m256i avx::cmpge_epi16(__m256i a, __m256i b) {
    return cmpeq(min(a, b, int16_t()), b, uint16_t());
}

__m256i avx::cmpge_epi32(__m256i a, __m256i b) {
    return cmpeq(min(a, b, int32_t()), b, uint8_t());
}

__m256i avx::cmpge_epu8(__m256i a, __m256i b) {
    return cmpeq(min(a, b, uint8_t()), b, uint8_t());
}

__m256i avx::cmpge_epu16(__m256i a, __m256i b) {
    return cmpeq(min(a, b, uint16_t()), b, uint16_t());
}

__m256i avx::cmpge_epu32(__m256i a, __m256i b) {
    return cmpeq(min(a, b, uint32_t()), b, uint32_t());
}

__m256i avx::cmpge(__m256i a, __m256i b, int8_t) { return cmpge_epi8(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, int16_t) { return cmpge_epi16(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, int32_t) { return cmpge_epi32(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint8_t) { return cmpge_epu8(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint16_t) { return cmpge_epu16(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint32_t) { return cmpge_epu32(a, b); }

__m256i avx::cmpgt_epi8(__m256i a, __m256i b) { return _mm256_cmpgt_epi8(a, b); }
__m256i avx::cmpgt_epi16(__m256i a, __m256i b) { return _mm256_cmpgt_epi16(a, b); }
__m256i avx::cmpgt_epi32(__m256i a, __m256i b) { return _mm256_cmpgt_epi32(a, b); }
__m256i avx::cmpgt_epi64(__m256i a, __m256i b) { return _mm256_cmpgt_epi64(a, b); }

__m256i avx::cmpgt_epu8(__m256i a, __m256i b) {
    return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
}

__m256i avx::cmpgt_epu16(__m256i a, __m256i b) {
    return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
}

__m256i avx::cmpgt_epu32(__m256i a, __m256i b) {
    return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
}

__m256i avx::cmpgt_epu64(__m256i a, __m256i b) {
    return cmpgt_epi64(Xor(a, setmin_epi64()), Xor(b, setmin_epi64()));
}

__m256i avx::cmpgt(__m256i a, __m256i b, int8_t) { return cmpgt_epi8(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int16_t) { return cmpgt_epi16(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int32_t) { return cmpgt_epi32(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int64_t) { return cmpgt_epi64(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint8_t) { return cmpgt_epu8(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint16_t) { return cmpgt_epu16(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint32_t) { return cmpgt_epu32(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint64_t) { return cmpgt_epu64(a, b); }

__m256i avx::cmple_epi8(__m256i a, __m256i b) { return cmpge_epi8(b, a); }

__m256i avx::cmple_epi16(__m256i a, __m256i b) { return cmpge_epi16(b, a); }

__m256i avx::cmple_epi32(__m256i a, __m256i b) { return cmpge_epi32(b, a); }

__m256i avx::cmple_epu8(__m256i a, __m256i b) { return cmpge_epu8(b, a); }

__m256i avx::cmple_epu16(__m256i a, __m256i b) { return cmpge_epu16(b, a); }

__m256i avx::cmple_epu32(__m256i a, __m256i b) { return cmpge_epu32(b, a); }

__m256i avx::cmple(__m256i a, __m256i b, int8_t) { return cmple_epi8(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, int16_t) { return cmple_epi16(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, int32_t) { return cmple_epi32(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint8_t) { return cmple_epu8(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint16_t) { return cmple_epu16(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint32_t) { return cmple_epu32(a, b); }

__m256i avx::cmplt_epi8(__m256i a, __m256i b) { return cmpgt_epi8(b, a); }
__m256i avx::cmplt_epi16(__m256i a, __m256i b) { return cmpgt_epi16(b, a); }
__m256i avx::cmplt_epi32(__m256i a, __m256i b) { return cmpgt_epi32(b, a); }

__m256i avx::cmplt_epu8(__m256i a, __m256i b) { return cmpgt_epu8(b, a); }
__m256i avx::cmplt_epu16(__m256i a, __m256i b) { return cmpgt_epu16(b, a); }
__m256i avx::cmplt_epu32(__m256i a, __m256i b) { return cmpgt_epu32(b, a); }

__m256i avx::cmplt(__m256i a, __m256i b, int8_t) { return cmplt_epi8(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, int16_t) { return cmplt_epi16(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, int32_t) { return cmplt_epi32(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint8_t) { return cmplt_epu8(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint16_t) { return cmplt_epu16(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint32_t) { return cmplt_epu32(a, b); }

__m256i avx::cmpne_epi8(__m256i a, __m256i b) { return Not(cmpeq_epi8(a, b)); }
__m256i avx::cmpne_epi16(__m256i a, __m256i b) { return Not(cmpeq_epi16(a, b)); }
__m256i avx::cmpne_epi32(__m256i a, __m256i b) { return Not(cmpeq_epi32(a, b)); }

__m256i avx::cmpne(__m256i a, __m256i b, int8_t) { return cmpne_epi8(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, int16_t) { return cmpne_epi16(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, int32_t) { return cmpne_epi32(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint8_t) { return cmpne_epi8(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint16_t) { return cmpne_epi16(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint32_t) { return cmpne_epi32(a, b); }

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::equal_to<>, T) {
    return cmpeq(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::not_equal_to<>, T) {
    return cmpne(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::greater<>, T) {
    return cmpgt(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::greater_equal<>, T) {
    return cmpge(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::less<>, T) {
    return cmplt(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::less_equal<>, T) {
    return cmple(a, b, T());
}

template <int imm8>
int avx::extract_epi8(__m256i v) {
    return _mm256_extract_epi8(v, imm8);
}

template <int imm8>
int avx::extract_epi16(__m256i v) {
    return _mm256_extract_epi16(v, imm8);
}

template <int imm8>
int avx::extract(__m256i v, int8_t) {
    return extract_epi8<imm8>(v);
}

template <int imm8>
int avx::extract(__m256i v, int16_t) {
    return extract_epi16<imm8>(v);
}

__m256i avx::hadd_epi16(__m256i a, __m256i b) { return _mm256_hadd_epi16(a, b); }
__m256i avx::hadd_epi32(__m256i a, __m256i b) { return _mm256_hadd_epi32(a, b); }

__m256i avx::hadd(__m256i a, __m256i b, int16_t) { return hadd_epi16(a, b); }
__m256i avx::hadd(__m256i a, __m256i b, int32_t) { return hadd_epi32(a, b); }

__m256i avx::hadds_epi16(__m256i a, __m256i b) { return _mm256_hadds_epi16(a, b); }

__m256i avx::hsub_epi16(__m256i a, __m256i b) { return _mm256_hsub_epi16(a, b); }
__m256i avx::hsub_epi32(__m256i a, __m256i b) { return _mm256_hsub_epi32(a, b); }

__m256i avx::hsub(__m256i a, __m256i b, int16_t) { return hsub_epi16(a, b); }
__m256i avx::hsub(__m256i a, __m256i b, int32_t) { return hsub_epi32(a, b); }

__m256i avx::hsubs_epi16(__m256i a, __m256i b) { return _mm256_hsubs_epi16(a, b); }

template <typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
                                                   uint8_t, uint16_t, uint32_t, uint64_t>,
                                       int>>
__m256i avx::logical_and(__m256i a, __m256i b, T) {
    return Not(Or(logical_not(a, T()), logical_not(b, T())));
}

template <typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
                                                   uint8_t, uint16_t, uint32_t, uint64_t>,
                                       int>>
__m256i avx::logical_not(__m256i v, T) {
    auto Zero = zeros();
    return cmpeq(v, Zero, T());
}

template <typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
                                                   uint8_t, uint16_t, uint32_t, uint64_t>,
                                       int>>
__m256i avx::logical_or(__m256i a, __m256i b, T) {
    return Not(logical_not(Or(a, b), T()));
}

__m256i avx::madd_epi16(__m256i a, __m256i b) { return _mm256_madd_epi16(a, b); }

__m256i avx::max_epi8(__m256i a, __m256i b) { return _mm256_max_epi8(a, b); }
__m256i avx::max_epi16(__m256i a, __m256i b) { return _mm256_max_epi16(a, b); }
__m256i avx::max_epi32(__m256i a, __m256i b) { return _mm256_max_epi32(a, b); }

__m256i avx::max_epu8(__m256i a, __m256i b) { return _mm256_max_epu8(a, b); }
__m256i avx::max_epu16(__m256i a, __m256i b) { return _mm256_max_epu16(a, b); }
__m256i avx::max_epu32(__m256i a, __m256i b) { return _mm256_max_epu32(a, b); }

__m256i avx::max(__m256i a, __m256i b, int8_t) { return max_epi8(a, b); }
__m256i avx::max(__m256i a, __m256i b, int16_t) { return max_epi16(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint8_t) { return max_epu8(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint16_t) { return max_epu16(a, b); }
__m256i avx::max(__m256i a, __m256i b, int32_t) { return max_epi32(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint32_t) { return max_epu32(a, b); }

int8_t avx::max_epi8(__m256i a) {
    return sse::max_epi8(sse::max_epi8(getlow(a), gethigh(a)));
}

int16_t avx::max_epi16(__m256i a) {
    return sse::max_epi16(sse::max_epi16(getlow(a), gethigh(a)));
}

int32_t avx::max_epi32(__m256i a) {
    return sse::max_epi32(sse::max_epi32(getlow(a), gethigh(a)));
}

uint8_t avx::max_epu8(__m256i a) {
    return sse::max_epu8(sse::max_epu8(getlow(a), gethigh(a)));
}

uint16_t avx::max_epu16(__m256i a) {
    return sse::max_epu16(sse::max_epu16(getlow(a), gethigh(a)));
}

uint32_t avx::max_epu32(__m256i a) {
    return sse::max_epu32(sse::max_epu32(getlow(a), gethigh(a)));
}

int8_t avx::max(__m256i a, int8_t) { return max_epi8(a); }
int16_t avx::max(__m256i a, int16_t) { return max_epi16(a); }
int32_t avx::max(__m256i a, int32_t) { return max_epi32(a); }
uint8_t avx::max(__m256i a, uint8_t) { return max_epu8(a); }
uint16_t avx::max(__m256i a, uint16_t) { return max_epu16(a); }
uint32_t avx::max(__m256i a, uint32_t) { return max_epu32(a); }

__m256i avx::min_epi8(__m256i a, __m256i b) { return _mm256_min_epi8(a, b); }
__m256i avx::min_epi16(__m256i a, __m256i b) { return _mm256_min_epi16(a, b); }
__m256i avx::min_epi32(__m256i a, __m256i b) { return _mm256_min_epi32(a, b); }

__m256i avx::min_epu8(__m256i a, __m256i b) { return _mm256_min_epu8(a, b); }
__m256i avx::min_epu16(__m256i a, __m256i b) { return _mm256_min_epu16(a, b); }
__m256i avx::min_epu32(__m256i a, __m256i b) { return _mm256_min_epu32(a, b); }

__m256i avx::min(__m256i a, __m256i b, int8_t) { return min_epi8(a, b); }
__m256i avx::min(__m256i a, __m256i b, int16_t) { return min_epi16(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint8_t) { return min_epu8(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint16_t) { return min_epu16(a, b); }
__m256i avx::min(__m256i a, __m256i b, int32_t) { return min_epi32(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint32_t) { return min_epu32(a, b); }

int8_t avx::min_epi8(__m256i a) {
    return sse::min_epi8(sse::min_epi8(getlow(a), gethigh(a)));
}

int16_t avx::min_epi16(__m256i a) {
    return sse::min_epi16(sse::min_epi16(getlow(a), gethigh(a)));
}

int32_t avx::min_epi32(__m256i a) {
    return sse::min_epi32(sse::min_epi32(getlow(a), gethigh(a)));
}

uint8_t avx::min_epu8(__m256i a) {
    return sse::min_epu8(sse::min_epu8(getlow(a), gethigh(a)));
}

uint16_t avx::min_epu16(__m256i a) {
    return sse::min_epu16(sse::min_epu16(getlow(a), gethigh(a)));
}

uint32_t avx::min_epu32(__m256i a) {
    return sse::min_epu32(sse::min_epu32(getlow(a), gethigh(a)));
}

int8_t avx::min(__m256i a, int8_t) { return min_epi8(a); }
int16_t avx::min(__m256i a, int16_t) { return min_epi16(a); }
int32_t avx::min(__m256i a, int32_t) { return min_epi32(a); }
uint8_t avx::min(__m256i a, uint8_t) { return min_epu8(a); }
uint16_t avx::min(__m256i a, uint16_t) { return min_epu16(a); }
uint32_t avx::min(__m256i a, uint32_t) { return min_epu32(a); }

avx::mask_type avx::movemask_epi8(__m256i a) { return _mm256_movemask_epi8(a); }

__m256i avx::mul_epi32(__m256i a, __m256i b) { return _mm256_mul_epi32(a, b); }
__m256i avx::mul_epu32(__m256i a, __m256i b) { return _mm256_mul_epu32(a, b); }

__m256i avx::mulhi_epi16(__m256i a, __m256i b) { return _mm256_mulhi_epi16(a, b); }

__m256i avx::mulhi_epu16(__m256i a, __m256i b) { return _mm256_mulhi_epu16(a, b); }

__m256i avx::mullo_epi16(__m256i a, __m256i b) { return _mm256_mullo_epi16(a, b); }

__m256i avx::packs_epi16(__m256i a, __m256i b) { return _mm256_packs_epi16(a, b); }
__m256i avx::packs_epi32(__m256i a, __m256i b) { return _mm256_packs_epi32(a, b); }

__m256i avx::packus_epi16(__m256i a, __m256i b) { return _mm256_packus_epi16(a, b); }
__m256i avx::packus_epi32(__m256i a, __m256i b) { return _mm256_packus_epi32(a, b); }

template <int imm>
__m256i avx::shl(__m256i a) {
    if constexpr (imm >= 64 * 3) {
        a = slli<8 * 3>(a);
        a = slli_epi64(a, imm - 64 * 3);
        return a;
    } else if constexpr (imm >= 64 * 2) {
        a = slli<8 * 2>(a);
        constexpr auto I = imm - 64 * 2;
        auto b = slli_epi64(a, I);
        auto c = slli<8>(a);
        c = srli_epi64(c, 64 - I);
        return Or(b, c);
    } else if constexpr (imm >= 64) {
        a = slli<8>(a);
        constexpr auto I = imm - 64;
        auto b = slli_epi64(a, I);
        auto c = slli<8>(a);
        c = srli_epi64(c, 64 - I);
        return Or(b, c);
    } else {
        auto b = slli_epi64(a, imm);
        auto c = slli<8>(a);
        c = srli_epi64(c, 64 - imm);
        return Or(b, c);
    }
}

template <int imm>
__m256i avx::shr(__m256i a) {
    if constexpr (imm >= 64 * 3) {
        a = srli<8 * 3>(a);
        a = srli_epi64(a, imm - 64 * 3);
        return a;
    } else if constexpr (imm >= 64 * 2) {
        a = srli<8 * 2>(a);
        constexpr auto I = imm - 64 * 2;
        auto b = srli_epi64(a, I);
        auto c = srli<8>(a);
        c = slli_epi64(c, 64 - I);
        return Or(b, c);
    } else if constexpr (imm >= 64) {
        a = srli<8>(a);
        constexpr auto I = imm - 64;
        auto b = srli_epi64(a, I);
        auto c = srli<8>(a);
        c = slli_epi64(c, 64 - I);
        return Or(b, c);
    } else {
        auto b = srli_epi64(a, imm);
        auto c = srli<8>(a);
        c = slli_epi64(c, 64 - imm);
        return Or(b, c);
    }
}

__m256i avx::shuffle_epi8(__m256i a, __m256i b) { return _mm256_shuffle_epi8(a, b); }

template <int imm8>
__m256i avx::shuffle_epi32(__m256i a) {
    return _mm256_shuffle_epi32(a, imm8);
}

template <int imm8>
__m256i avx::shufflehi_epi16(__m256i a) {
    return _mm256_shufflehi_epi16(a, imm8);
}

template <int imm8>
__m256i avx::shufflelo_epi16(__m256i a) {
    return _mm256_shufflelo_epi16(a, imm8);
}

__m256i avx::sll_epi16(__m256i a, __m128i b) { return _mm256_sll_epi16(a, b); }
__m256i avx::sll_epi32(__m256i a, __m128i b) { return _mm256_sll_epi32(a, b); }
__m256i avx::sll_epi64(__m256i a, __m128i b) { return _mm256_sll_epi64(a, b); }

__m256i avx::sll(__m256i a, __m128i b, int16_t) { return sll_epi16(a, b); }
__m256i avx::sll(__m256i a, __m128i b, int32_t) { return sll_epi32(a, b); }
__m256i avx::sll(__m256i a, __m128i b, int64_t) { return sll_epi64(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint64_t) { return sll_epi64(a, b); }

template <int imm8>
__m256i avx::slli(__m256i a) {
    return _mm256_slli_si256(a, imm8);
}
__m256i avx::slli_epi16(__m256i a, int imm8) { return _mm256_slli_epi16(a, imm8); }
__m256i avx::slli_epi32(__m256i a, int imm8) { return _mm256_slli_epi32(a, imm8); }
__m256i avx::slli_epi64(__m256i a, int imm8) { return _mm256_slli_epi64(a, imm8); }

__m256i avx::slli(__m256i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }

__m256i avx::sra_epi16(__m256i a, __m128i b) { return _mm256_sra_epi16(a, b); }
__m256i avx::sra_epi32(__m256i a, __m128i b) { return _mm256_sra_epi32(a, b); }

__m256i avx::sra(__m256i a, __m128i b, int16_t) { return sra_epi16(a, b); }
__m256i avx::sra(__m256i a, __m128i b, int32_t) { return sra_epi32(a, b); }

__m256i avx::srai_epi16(__m256i a, int imm8) { return _mm256_srai_epi16(a, imm8); }
__m256i avx::srai_epi32(__m256i a, int imm8) { return _mm256_srai_epi32(a, imm8); }

__m256i avx::srai(__m256i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
__m256i avx::srai(__m256i a, int imm8, int32_t) { return srai_epi32(a, imm8); }

__m256i avx::stream_load(__m256i const *p) { return _mm256_stream_load_si256(p); }

__m256i avx::srl_epi16(__m256i a, __m128i b) { return _mm256_srl_epi16(a, b); }
__m256i avx::srl_epi32(__m256i a, __m128i b) { return _mm256_srl_epi32(a, b); }
__m256i avx::srl_epi64(__m256i a, __m128i b) { return _mm256_srl_epi64(a, b); }

__m256i avx::srl(__m256i a, __m128i b, int16_t) { return srl_epi16(a, b); }
__m256i avx::srl(__m256i a, __m128i b, int32_t) { return srl_epi32(a, b); }
__m256i avx::srl(__m256i a, __m128i b, int64_t) { return srl_epi64(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint64_t) { return srl_epi64(a, b); }

template <int imm8>
__m256i avx::srli(__m256i a) {
    return _mm256_srli_si256(a, imm8);
}

__m256i avx::srli_epi16(__m256i a, int imm8) { return _mm256_srli_epi16(a, imm8); }
__m256i avx::srli_epi32(__m256i a, int imm8) { return _mm256_srli_epi32(a, imm8); }
__m256i avx::srli_epi64(__m256i a, int imm8) { return _mm256_srli_epi64(a, imm8); }

__m256i avx::srli(__m256i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }

__m256i avx::sub_epi8(__m256i a, __m256i b) { return _mm256_sub_epi8(a, b); }
__m256i avx::sub_epi16(__m256i a, __m256i b) { return _mm256_sub_epi16(a, b); }
__m256i avx::sub_epi32(__m256i a, __m256i b) { return _mm256_sub_epi32(a, b); }
__m256i avx::sub_epi64(__m256i a, __m256i b) { return _mm256_sub_epi64(a, b); }

__m256i avx::sub(__m256i a, __m256i b, int8_t) { return sub_epi8(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int16_t) { return sub_epi16(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int32_t) { return sub_epi32(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int64_t) { return sub_epi64(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint8_t) { return sub_epi8(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint16_t) { return sub_epi16(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint32_t) { return sub_epi32(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint64_t) { return sub_epi64(a, b); }

__m256i avx::subs_epi8(__m256i a, __m256i b) { return _mm256_subs_epi8(a, b); }
__m256i avx::subs_epi16(__m256i a, __m256i b) { return _mm256_subs_epi16(a, b); }

__m256i avx::subs_epu8(__m256i a, __m256i b) { return _mm256_subs_epu8(a, b); }
__m256i avx::subs_epu16(__m256i a, __m256i b) { return _mm256_subs_epu16(a, b); }

__m256i avx::subs(__m256i a, __m256i b, int8_t) { return subs_epi8(a, b); }
__m256i avx::subs(__m256i a, __m256i b, int16_t) { return subs_epi16(a, b); }
__m256i avx::subs(__m256i a, __m256i b, uint8_t) { return subs_epu8(a, b); }
__m256i avx::subs(__m256i a, __m256i b, uint16_t) { return subs_epu16(a, b); }

int avx::test_all_ones(__m256i a) { return testc(a, cmpeq_epi32(a, a)); }

__m256i avx::unpackhi_epi8(__m256i a, __m256i b) { return _mm256_unpackhi_epi8(a, b); }
__m256i avx::unpackhi_epi16(__m256i a, __m256i b) { return _mm256_unpackhi_epi16(a, b); }
__m256i avx::unpackhi_epi32(__m256i a, __m256i b) { return _mm256_unpackhi_epi32(a, b); }
__m256i avx::unpackhi_epi64(__m256i a, __m256i b) { return _mm256_unpackhi_epi64(a, b); }

__m256i avx::unpackhi(__m256i a, __m256i b, int8_t) { return unpackhi_epi8(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int16_t) { return unpackhi_epi16(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int32_t) { return unpackhi_epi32(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int64_t) { return unpackhi_epi64(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint8_t) { return unpackhi_epi8(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint16_t) { return unpackhi_epi16(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint32_t) { return unpackhi_epi32(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint64_t) { return unpackhi_epi64(a, b); }

__m256i avx::unpacklo_epi8(__m256i a, __m256i b) { return _mm256_unpacklo_epi8(a, b); }
__m256i avx::unpacklo_epi16(__m256i a, __m256i b) { return _mm256_unpacklo_epi16(a, b); }
__m256i avx::unpacklo_epi32(__m256i a, __m256i b) { return _mm256_unpacklo_epi32(a, b); }
__m256i avx::unpacklo_epi64(__m256i a, __m256i b) { return _mm256_unpacklo_epi64(a, b); }

__m256i avx::unpacklo(__m256i a, __m256i b, int8_t) { return unpacklo_epi8(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int16_t) { return unpacklo_epi16(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int32_t) { return unpacklo_epi32(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int64_t) { return unpacklo_epi64(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint8_t) { return unpacklo_epi8(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint16_t) { return unpacklo_epi16(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint32_t) { return unpacklo_epi32(a, b); }

#endif

} // namespace wjr

#endif // WJR_SIMD_SIMD_HPP__

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
#define WJR_REGISTER_FIND_N_AVX(index)                                                   \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 + (index)));                                \
        auto y = avx::loadu((__m256i *)(src1 + (index)));                                \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = avx::movemask_epi8(r);                                     \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index) + ctz(mask) / 8;                                              \
        }                                                                                \
    } while (0)

    size_t rem = n & 7;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src0 + (rem - 4)));
        auto x1 = sse::loadu((__m128i *)(src0 + (rem - 2)));
        auto y1 = sse::loadu((__m128i *)(src1 + (rem - 2)));
        auto y0 = sse::loadu((__m128i *)(src1 + (rem - 4)));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);

        if (WJR_LIKELY(!sse::test_all_zeros(sse::Or(r0, r1)))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return (rem - 4) + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r1);
            return (rem - 2) + (mask == 0xFF00);
        }
#else
        WJR_REGISTER_FIND_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return n;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src0 + rem));
        auto x1 = sse::loadu((__m128i *)(src0 + rem + 2));
        auto x2 = sse::loadu((__m128i *)(src0 + rem + 4));
        auto x3 = sse::loadu((__m128i *)(src0 + rem + 6));
        auto y0 = sse::loadu((__m128i *)(src1 + rem));
        auto y1 = sse::loadu((__m128i *)(src1 + rem + 2));
        auto y2 = sse::loadu((__m128i *)(src1 + rem + 4));
        auto y3 = sse::loadu((__m128i *)(src1 + rem + 6));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto r3 = sse::cmpeq_epi64(x3, y3);

        auto z = sse::Or(sse::Or(r0, r1), sse::Or(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_zeros(z))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 2 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 4 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r3);
            return rem + 6 + (mask == 0xFF00);
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_FIND_N_AVX(rem);
        WJR_REGISTER_FIND_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return n;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src0 + rem));
        auto x2 = avx::loadu((__m256i *)(src0 + rem + 8));
        auto x3 = avx::loadu((__m256i *)(src0 + rem + 12));
        auto x1 = avx::loadu((__m256i *)(src0 + rem + 4));
        auto y0 = avx::loadu((__m256i *)(src1 + rem));
        auto y1 = avx::loadu((__m256i *)(src1 + rem + 4));
        auto y2 = avx::loadu((__m256i *)(src1 + rem + 8));
        auto y3 = avx::loadu((__m256i *)(src1 + rem + 12));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto r3 = avx::cmpeq_epi64(x3, y3);

        auto z = avx::Or(avx::Or(r0, r1), avx::Or(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_zeros(z))) {
            avx::mask_type mask = avx::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 8 + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r3);
            return rem + 12 + ctz(mask) / 8;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return n;

#undef WJR_REGISTER_FIND_N_AVX
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_n(const T *src0, const T *src1, size_t n) {
    if (WJR_UNLIKELY(n == 0 || src0[0] == src1[0])) {
        return 0;
    }

    if (n == 1 || WJR_UNLIKELY(src0[1] == src1[1])) {
        return 1;
    }

    if (n == 2 || WJR_UNLIKELY(src0[2] == src1[2])) {
        return 2;
    }

    if (n == 3 || WJR_UNLIKELY(src0[3] == src1[3])) {
        return 3;
    }

    if (n == 4) {
        return 4;
    }

    size_t ret = large_builtin_find_n(src0, src1, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

template <typename T>
WJR_COLD size_t large_builtin_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_N_AVX(index)                                                   \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src + (index)));                                 \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = avx::movemask_epi8(r);                                               \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index) + ctz(mask) / 8;                                              \
        }                                                                                \
    } while (0)

#if !WJR_HAS_SIMD(AVX2)
    auto y = sse::set1(val, T());
#else
    auto y = avx::set1(val, T());
#endif

    size_t rem = n & 7;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src + (rem - 4)));
        auto x1 = sse::loadu((__m128i *)(src + (rem - 2)));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);

        if (WJR_LIKELY(!sse::test_all_zeros(sse::Or(r0, r1)))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem - 4 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r1);
            return rem - 2 + (mask == 0xFF00);
        }
#else
        WJR_REGISTER_FIND_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return n;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src + rem));
        auto x1 = sse::loadu((__m128i *)(src + rem + 2));
        auto x2 = sse::loadu((__m128i *)(src + rem + 4));
        auto x3 = sse::loadu((__m128i *)(src + rem + 6));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);
        auto r2 = sse::cmpeq_epi64(x2, y);
        auto r3 = sse::cmpeq_epi64(x3, y);

        auto z = sse::Or(sse::Or(r0, r1), sse::Or(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_zeros(z))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 2 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 4 + (mask == 0xFF00);
            }

            mask = sse::movemask_epi8(r3);
            return rem + 6 + (mask == 0xFF00);
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_FIND_N_AVX(rem);
        WJR_REGISTER_FIND_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return n;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src + rem));
        auto x1 = avx::loadu((__m256i *)(src + rem + 4));
        auto x2 = avx::loadu((__m256i *)(src + rem + 8));
        auto x3 = avx::loadu((__m256i *)(src + rem + 12));

        auto r0 = avx::cmpeq_epi64(x0, y);
        auto r1 = avx::cmpeq_epi64(x1, y);
        auto r2 = avx::cmpeq_epi64(x2, y);
        auto r3 = avx::cmpeq_epi64(x3, y);

        auto z = avx::Or(avx::Or(r0, r1), avx::Or(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_zeros(z))) {
            avx::mask_type mask = avx::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 8 + ctz(mask) / 8;
            }

            mask = avx::movemask_epi8(r3);
            return rem + 12 + ctz(mask) / 8;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return n;

#undef WJR_REGISTER_FIND_N_AVX
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0 || src[0] == val)) {
        return 0;
    }

    if (n == 1 || WJR_UNLIKELY(src[1] == val)) {
        return 1;
    }

    if (n == 2 || WJR_UNLIKELY(src[2] == val)) {
        return 2;
    }

    if (n == 3 || WJR_UNLIKELY(src[3] == val)) {
        return 3;
    }

    if (n == 4) {
        return 4;
    }

    size_t ret = large_builtin_find_n(src, val, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(FIND_N)

#if WJR_HAS_BUILTIN(FIND_NOT_N)

template <typename T>
WJR_COLD size_t large_builtin_find_not_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_FIND_NOT_N_AVX(index)                                               \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 + (index)));                                \
        auto y = avx::loadu((__m256i *)(src1 + (index)));                                \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index) + ctz(mask) / 8;                                              \
        }                                                                                \
    } while (0)

    size_t rem = n & 7;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src0 + (rem - 4)));
        auto x1 = sse::loadu((__m128i *)(src0 + (rem - 2)));
        auto y0 = sse::loadu((__m128i *)(src1 + (rem - 4)));
        auto y1 = sse::loadu((__m128i *)(src1 + (rem - 2)));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem - 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            return rem - 2 + ctz(mask) / 8;
        }
#else
        WJR_REGISTER_FIND_NOT_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return n;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src0 + rem));
        auto x1 = sse::loadu((__m128i *)(src0 + rem + 2));
        auto x2 = sse::loadu((__m128i *)(src0 + rem + 4));
        auto x3 = sse::loadu((__m128i *)(src0 + rem + 6));
        auto y0 = sse::loadu((__m128i *)(src1 + rem));
        auto y1 = sse::loadu((__m128i *)(src1 + rem + 2));
        auto y2 = sse::loadu((__m128i *)(src1 + rem + 4));
        auto y3 = sse::loadu((__m128i *)(src1 + rem + 6));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto r3 = sse::cmpeq_epi64(x3, y3);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 2 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r3);
            return rem + 6 + ctz(mask) / 8;
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_FIND_NOT_N_AVX(rem);
        WJR_REGISTER_FIND_NOT_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return n;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src0 + rem));
        auto x1 = avx::loadu((__m256i *)(src0 + rem + 4));
        auto x2 = avx::loadu((__m256i *)(src0 + rem + 8));
        auto x3 = avx::loadu((__m256i *)(src0 + rem + 12));
        auto y0 = avx::loadu((__m256i *)(src1 + rem));
        auto y1 = avx::loadu((__m256i *)(src1 + rem + 4));
        auto y2 = avx::loadu((__m256i *)(src1 + rem + 8));
        auto y3 = avx::loadu((__m256i *)(src1 + rem + 12));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto r3 = avx::cmpeq_epi64(x3, y3);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 8 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r3);
            return rem + 12 + ctz(mask) / 8;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return n;

#undef WJR_REGISTER_FIND_NOT_N_AVX
}

extern template WJR_COLD size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src0,
                                                                   const uint64_t *src1,
                                                                   size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_not_n(const T *src0, const T *src1, size_t n) {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src0[0] != src1[0])) {
        return 0;
    }

    if (n == 1 || WJR_LIKELY(src0[1] != src1[1])) {
        return 1;
    }

    if (n == 2 || WJR_LIKELY(src0[2] != src1[2])) {
        return 2;
    }

    if (n == 3 || WJR_LIKELY(src0[3] != src1[3])) {
        return 3;
    }

    if (n == 4) {
        return 4;
    }

    size_t ret = large_builtin_find_not_n(src0, src1, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

template <typename T>
WJR_COLD size_t large_builtin_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_FIND_NOT_N_AVX(index)                                               \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src + (index)));                                 \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        auto mask = ~avx::movemask_epi8(r);                                              \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index) + ctz(mask) / 8;                                              \
        }                                                                                \
    } while (0)

#if !WJR_HAS_SIMD(AVX2)
    auto y = sse::set1(val, T());
#else
    auto y = avx::set1(val, T());
#endif

    size_t rem = n & 7;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src + (rem - 4)));
        auto x1 = sse::loadu((__m128i *)(src + (rem - 2)));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem - 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            return rem - 2 + ctz(mask) / 8;
        }
#else
        WJR_REGISTER_FIND_NOT_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return n;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src + rem));
        auto x1 = sse::loadu((__m128i *)(src + rem + 2));
        auto x2 = sse::loadu((__m128i *)(src + rem + 4));
        auto x3 = sse::loadu((__m128i *)(src + rem + 6));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);
        auto r2 = sse::cmpeq_epi64(x2, y);
        auto r3 = sse::cmpeq_epi64(x3, y);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 2 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = ~sse::movemask_epi8(r3);
            return rem + 6 + ctz(mask) / 8;
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_FIND_NOT_N_AVX(rem);
        WJR_REGISTER_FIND_NOT_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return n;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src + rem));
        auto x1 = avx::loadu((__m256i *)(src + rem + 4));
        auto x2 = avx::loadu((__m256i *)(src + rem + 8));
        auto x3 = avx::loadu((__m256i *)(src + rem + 12));

        auto r0 = avx::cmpeq_epi64(x0, y);
        auto r1 = avx::cmpeq_epi64(x1, y);
        auto r2 = avx::cmpeq_epi64(x2, y);
        auto r3 = avx::cmpeq_epi64(x3, y);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r0);
            if (mask != 0) {
                return rem + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return rem + 4 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return rem + 8 + ctz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r3);
            return rem + 12 + ctz(mask) / 8;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return n;

#undef WJR_REGISTER_FIND_NOT_N_AVX
}

extern template WJR_COLD size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src,
                                                                   uint64_t val,
                                                                   size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_find_not_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src[0] != val)) {
        return 0;
    }

    if (n == 1 || WJR_LIKELY(src[1] != val)) {
        return 1;
    }

    if (n == 2 || WJR_LIKELY(src[2] != val)) {
        return 2;
    }

    if (n == 3 || WJR_LIKELY(src[3] != val)) {
        return 3;
    }

    if (n == 4) {
        return 4;
    }

    size_t ret = large_builtin_find_not_n(src, val, n);
    WJR_ASSUME(ret >= 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(FIND_NOT_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_N)

template <typename T>
WJR_COLD size_t large_builtin_reverse_find_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_AVX(index)                                           \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 - 4 + (index)));                            \
        auto y = avx::loadu((__m256i *)(src1 - 4 + (index)));                            \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = avx::movemask_epi8(r);                                     \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index)-clz(mask) / 8;                                                \
        }                                                                                \
    } while (0)

    const size_t rem = n & 7;
    n -= rem;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src0 + n + 2));
        auto x1 = sse::loadu((__m128i *)(src0 + n));
        auto y0 = sse::loadu((__m128i *)(src1 + n + 2));
        auto y1 = sse::loadu((__m128i *)(src1 + n));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);

        if (WJR_LIKELY(!sse::test_all_zeros(sse::Or(r0, r1)))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return n + 4 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r1);
            return n + 2 - (mask == 0x00FF);
        }
#else
        WJR_REGISTER_REVERSE_FIND_N_AVX(n + 4);
#endif
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src0 + n - 8));
        auto x1 = sse::loadu((__m128i *)(src0 + n - 6));
        auto x2 = sse::loadu((__m128i *)(src0 + n - 4));
        auto x3 = sse::loadu((__m128i *)(src0 + n - 2));
        auto y0 = sse::loadu((__m128i *)(src1 + n - 8));
        auto y1 = sse::loadu((__m128i *)(src1 + n - 6));
        auto y2 = sse::loadu((__m128i *)(src1 + n - 4));
        auto y3 = sse::loadu((__m128i *)(src1 + n - 2));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto r3 = sse::cmpeq_epi64(x3, y3);

        auto z = sse::Or(sse::Or(r0, r1), sse::Or(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_zeros(z))) {
            sse::mask_type mask = sse::movemask_epi8(r3);
            if (mask != 0) {
                return n - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if ((n & 8) != 0) {
        WJR_REGISTER_REVERSE_FIND_N_AVX(n);
        WJR_REGISTER_REVERSE_FIND_N_AVX(n - 4);

        n -= 8;

        if (WJR_UNLIKELY(n == 0)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src0 + n - 16));
        auto x1 = avx::loadu((__m256i *)(src0 + n - 12));
        auto x2 = avx::loadu((__m256i *)(src0 + n - 8));
        auto x3 = avx::loadu((__m256i *)(src0 + n - 4));
        auto y0 = avx::loadu((__m256i *)(src1 + n - 16));
        auto y1 = avx::loadu((__m256i *)(src1 + n - 12));
        auto y2 = avx::loadu((__m256i *)(src1 + n - 8));
        auto y3 = avx::loadu((__m256i *)(src1 + n - 4));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto r3 = avx::cmpeq_epi64(x3, y3);

        auto z = avx::Or(avx::Or(r0, r1), avx::Or(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_zeros(z))) {
            avx::mask_type mask = avx::movemask_epi8(r3);
            if (mask != 0) {
                return n - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N_AVX
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_n(const T *src0, const T *src1,
                                                   size_t n) {
    if (WJR_UNLIKELY(n == 0 || src0[n - 1] == src1[n - 1])) {
        return n;
    }

    if (n == 1 || WJR_UNLIKELY(src0[n - 2] == src1[n - 2])) {
        return n - 1;
    }

    if (n == 2 || WJR_UNLIKELY(src0[n - 3] == src1[n - 3])) {
        return n - 2;
    }

    if (n == 3 || WJR_UNLIKELY(src0[n - 4] == src1[n - 4])) {
        return n - 3;
    }

    if (n == 4) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_n(src0, src1, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

template <typename T>
WJR_COLD size_t large_builtin_reverse_find_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_AVX(index)                                           \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src - 4 + (index)));                             \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = avx::movemask_epi8(r);                                     \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index)-clz(mask) / 8;                                                \
        }                                                                                \
    } while (0)

#if !WJR_HAS_SIMD(AVX2)
    auto y = sse::set1(val, T());
#else
    auto y = avx::set1(val, T());
#endif

    const size_t rem = n & 7;
    n -= rem;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src + n + 2));
        auto x1 = sse::loadu((__m128i *)(src + n));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);

        if (WJR_LIKELY(!sse::test_all_zeros(sse::Or(r0, r1)))) {
            sse::mask_type mask = sse::movemask_epi8(r0);
            if (mask != 0) {
                return n + 4 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r1);
            return n + 2 - (mask == 0x00FF);
        }
#else
        WJR_REGISTER_REVERSE_FIND_N_AVX(n + 4);
#endif
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src + n - 8));
        auto x1 = sse::loadu((__m128i *)(src + n - 6));
        auto x2 = sse::loadu((__m128i *)(src + n - 4));
        auto x3 = sse::loadu((__m128i *)(src + n - 2));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);
        auto r2 = sse::cmpeq_epi64(x2, y);
        auto r3 = sse::cmpeq_epi64(x3, y);

        auto z = sse::Or(sse::Or(r0, r1), sse::Or(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_zeros(z))) {
            sse::mask_type mask = sse::movemask_epi8(r3);
            if (mask != 0) {
                return n - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if ((n & 8) != 0) {
        WJR_REGISTER_REVERSE_FIND_N_AVX(n);
        WJR_REGISTER_REVERSE_FIND_N_AVX(n - 4);

        n -= 8;

        if (WJR_UNLIKELY(n == 0)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src + n - 16));
        auto x1 = avx::loadu((__m256i *)(src + n - 12));
        auto x2 = avx::loadu((__m256i *)(src + n - 8));
        auto x3 = avx::loadu((__m256i *)(src + n - 4));

        auto r0 = avx::cmpeq_epi64(x0, y);
        auto r1 = avx::cmpeq_epi64(x1, y);
        auto r2 = avx::cmpeq_epi64(x2, y);
        auto r3 = avx::cmpeq_epi64(x3, y);

        auto z = avx::Or(avx::Or(r0, r1), avx::Or(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_zeros(z))) {
            avx::mask_type mask = avx::movemask_epi8(r3);
            if (mask != 0) {
                return n - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N_AVX
}

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0 || src[n - 1] == val)) {
        return n;
    }

    if (n == 1 || WJR_UNLIKELY(src[n - 2] == val)) {
        return n - 1;
    }

    if (n == 2 || WJR_UNLIKELY(src[n - 3] == val)) {
        return n - 2;
    }

    if (n == 3 || WJR_UNLIKELY(src[n - 4] == val)) {
        return n - 3;
    }

    if (n == 4) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_n(src, val, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

template <typename T>
WJR_COLD size_t large_builtin_reverse_find_not_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_AVX(index)                                           \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 - 4 + (index)));                            \
        auto y = avx::loadu((__m256i *)(src1 - 4 + (index)));                            \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index)-clz(mask) / 8;                                                \
        }                                                                                \
    } while (0)

    const size_t rem = n & 7;
    n -= rem;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src0 + n + 2));
        auto x1 = sse::loadu((__m128i *)(src0 + n));
        auto y0 = sse::loadu((__m128i *)(src1 + n + 2));
        auto y1 = sse::loadu((__m128i *)(src1 + n));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return n + 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            return n + 2 - (mask == 0x00FF);
        }
#else
        WJR_REGISTER_REVERSE_FIND_N_AVX(n + 4);
#endif
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src0 + n - 8));
        auto x1 = sse::loadu((__m128i *)(src0 + n - 6));
        auto x2 = sse::loadu((__m128i *)(src0 + n - 4));
        auto x3 = sse::loadu((__m128i *)(src0 + n - 2));
        auto y0 = sse::loadu((__m128i *)(src1 + n - 8));
        auto y1 = sse::loadu((__m128i *)(src1 + n - 6));
        auto y2 = sse::loadu((__m128i *)(src1 + n - 4));
        auto y3 = sse::loadu((__m128i *)(src1 + n - 2));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto r3 = sse::cmpeq_epi64(x3, y3);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r3);
            if (mask != 0) {
                return n - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if ((n & 8) != 0) {
        WJR_REGISTER_REVERSE_FIND_N_AVX(n);
        WJR_REGISTER_REVERSE_FIND_N_AVX(n - 4);

        n -= 8;

        if (WJR_UNLIKELY(n == 0)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src0 + n - 16));
        auto x1 = avx::loadu((__m256i *)(src0 + n - 12));
        auto x2 = avx::loadu((__m256i *)(src0 + n - 8));
        auto x3 = avx::loadu((__m256i *)(src0 + n - 4));
        auto y0 = avx::loadu((__m256i *)(src1 + n - 16));
        auto y1 = avx::loadu((__m256i *)(src1 + n - 12));
        auto y2 = avx::loadu((__m256i *)(src1 + n - 8));
        auto y3 = avx::loadu((__m256i *)(src1 + n - 4));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto r3 = avx::cmpeq_epi64(x3, y3);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r3);
            if (mask != 0) {
                return n - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N_AVX
}

extern template WJR_COLD size_t large_builtin_reverse_find_not_n<uint64_t>(
    const uint64_t *src0, const uint64_t *src1, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_not_n(const T *src0, const T *src1,
                                                       size_t n) {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src0[n - 1] != src1[n - 1])) {
        return n;
    }

    if (n == 1 || WJR_LIKELY(src0[n - 2] != src1[n - 2])) {
        return n - 1;
    }

    if (n == 2 || WJR_LIKELY(src0[n - 3] != src1[n - 3])) {
        return n - 2;
    }

    if (n == 3 || WJR_LIKELY(src0[n - 4] != src1[n - 4])) {
        return n - 3;
    }

    if (n == 4) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_not_n(src0, src1, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

template <typename T>
WJR_COLD size_t large_builtin_reverse_find_not_n(const T *src, T val, size_t n) {
#define WJR_REGISTER_REVERSE_FIND_N_AVX(index)                                           \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src - 4 + (index)));                             \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            return (index)-clz(mask) / 8;                                                \
        }                                                                                \
    } while (0)

#if !WJR_HAS_SIMD(AVX2)
    auto y = sse::set1(val, T());
#else
    auto y = avx::set1(val, T());
#endif

    const size_t rem = n & 7;
    n -= rem;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src + n + 2));
        auto x1 = sse::loadu((__m128i *)(src + n));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                return n + 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            return n + 2 - (mask == 0x00FF);
        }
#else
        WJR_REGISTER_REVERSE_FIND_N_AVX(n + 4);
#endif
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src + n - 8));
        auto x1 = sse::loadu((__m128i *)(src + n - 6));
        auto x2 = sse::loadu((__m128i *)(src + n - 4));
        auto x3 = sse::loadu((__m128i *)(src + n - 2));

        auto r0 = sse::cmpeq_epi64(x0, y);
        auto r1 = sse::cmpeq_epi64(x1, y);
        auto r2 = sse::cmpeq_epi64(x2, y);
        auto r3 = sse::cmpeq_epi64(x3, y);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r3);
            if (mask != 0) {
                return n - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                return n - 2 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                return n - 4 - (mask == 0x00FF);
            }

            mask = ~sse::movemask_epi8(r0);
            return n - 6 - (mask == 0x00FF);
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if ((n & 8) != 0) {
        WJR_REGISTER_REVERSE_FIND_N_AVX(n);
        WJR_REGISTER_REVERSE_FIND_N_AVX(n - 4);

        n -= 8;

        if (WJR_UNLIKELY(n == 0)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src + n - 16));
        auto x1 = avx::loadu((__m256i *)(src + n - 12));
        auto x2 = avx::loadu((__m256i *)(src + n - 8));
        auto x3 = avx::loadu((__m256i *)(src + n - 4));

        auto r0 = avx::cmpeq_epi64(x0, y);
        auto r1 = avx::cmpeq_epi64(x1, y);
        auto r2 = avx::cmpeq_epi64(x2, y);
        auto r3 = avx::cmpeq_epi64(x3, y);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r3);
            if (mask != 0) {
                return n - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                return n - 4 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                return n - 8 - clz(mask) / 8;
            }

            mask = ~avx::movemask_epi8(r0);
            return n - 12 - clz(mask) / 8;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_FIND_N_AVX
}

extern template WJR_COLD size_t
large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src, uint64_t val, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE size_t builtin_reverse_find_not_n(const T *src, T val, size_t n) {
    if (WJR_UNLIKELY(n == 0) || WJR_LIKELY(src[n - 1] != val)) {
        return n;
    }

    if (n == 1 || WJR_LIKELY(src[n - 2] != val)) {
        return n - 1;
    }

    if (n == 2 || WJR_LIKELY(src[n - 3] != val)) {
        return n - 2;
    }

    if (n == 3 || WJR_LIKELY(src[n - 4] != val)) {
        return n - 3;
    }

    if (n == 4) {
        return n - 4;
    }

    size_t ret = large_builtin_reverse_find_not_n(src, val, n);
    WJR_ASSUME(n > 4);
    WJR_ASSUME(ret <= n - 4);
    return ret;
}

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

} // namespace wjr

#endif // WJR_X86_MATH_FIND_HPP__
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_n(const T *src0, const T *src1, size_t n) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[idx] == src1[idx]) {
            break;
        }
    }

    return idx;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t find_n_impl(const T *src0, const T *src1,
                                                      size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return 0;
    }

#if WJR_HAS_BUILTIN(FIND_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_n(src0, src1, n);
        }

        return builtin_find_n(src0, src1, n);
    } else {
        return fallback_find_n(src0, src1, n);
    }
#else
    return fallback_find_n(src0, src1, n);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t find_n(const T *src0, const T *src1, size_t n) {
    using uT = std::make_unsigned_t<T>;
    return find_n_impl<uT>(reinterpret_cast<const uT *>(src0),
                           reinterpret_cast<const uT *>(src1), n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_n(const T *src, T val, size_t n) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] == val) {
            break;
        }
    }

    return idx;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t find_n_impl(const T *src,
                                                      type_identity_t<T> val, size_t n) {
#if WJR_HAS_BUILTIN(FIND_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_n(src, val, n);
        }

        return builtin_find_n(src, val, n);
    } else {
        return fallback_find_n(src, val, n);
    }
#else
    return fallback_find_n(src, val, n);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t find_n(const T *src, type_identity_t<T> val,
                                                 size_t n) {
    using uT = std::make_unsigned_t<T>;
    return find_n_impl<uT>(reinterpret_cast<const uT *>(src), val, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_not_n(const T *src0, const T *src1,
                                                   size_t n) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[idx] != src1[idx]) {
            break;
        }
    }

    return idx;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t find_not_n_impl(const T *src0, const T *src1,
                                                          size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return n;
    }

#if WJR_HAS_BUILTIN(FIND_NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_not_n(src0, src1, n);
        }

        return builtin_find_not_n(src0, src1, n);
    } else {
        return fallback_find_not_n(src0, src1, n);
    }
#else
    return fallback_find_not_n(src0, src1, n);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t find_not_n(const T *src0, const T *src1,
                                                     size_t n) {
    using uT = std::make_unsigned_t<T>;
    return find_not_n_impl<uT>(reinterpret_cast<const uT *>(src0),
                               reinterpret_cast<const uT *>(src1), n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_find_not_n(const T *src, T val, size_t n) {
    size_t idx = 0;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[idx] != val) {
            break;
        }
    }

    return idx;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t find_not_n_impl(const T *src,
                                                          type_identity_t<T> val,
                                                          size_t n) {
#if WJR_HAS_BUILTIN(FIND_NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_find_not_n(src, val, n);
        }

        return builtin_find_not_n(src, val, n);
    } else {
        return fallback_find_not_n(src, val, n);
    }
#else
    return fallback_find_not_n(src, val, n);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t find_not_n(const T *src, type_identity_t<T> val,
                                                     size_t n) {
    using uT = std::make_unsigned_t<T>;
    return find_not_n_impl<uT>(reinterpret_cast<const uT *>(src), val, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_reverse_find_n(const T *src0, const T *src1,
                                                       size_t n) {
    size_t idx = 0;
    src0 += n;
    src1 += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[-1 - idx] == src1[-1 - idx]) {
            break;
        }
    }

    return n - idx;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_n_impl(const T *src0,
                                                              const T *src1, size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return n;
    }

#if WJR_HAS_BUILTIN(REVERSE_FIND_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_n(src0, src1, n);
        }

        return builtin_reverse_find_n(src0, src1, n);
    } else {
        return fallback_reverse_find_n(src0, src1, n);
    }
#else
    return fallback_reverse_find_n(src0, src1, n);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_n(const T *src0, const T *src1,
                                                         size_t n) {
    using uT = std::make_unsigned_t<T>;
    return reverse_find_n_impl<uT>(reinterpret_cast<const uT *>(src0),
                                   reinterpret_cast<const uT *>(src1), n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_reverse_find_n(const T *src, T val, size_t n) {
    size_t idx = 0;
    src += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[-1 - idx] == val) {
            break;
        }
    }

    return n - idx;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_n_impl(const T *src,
                                                              type_identity_t<T> val,
                                                              size_t n) {
#if WJR_HAS_BUILTIN(REVERSE_FIND_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_n(src, val, n);
        }

        return builtin_reverse_find_n(src, val, n);
    } else {
        return fallback_reverse_find_n(src, val, n);
    }
#else
    return fallback_reverse_find_n(src, val, n);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_n(const T *src,
                                                         type_identity_t<T> val,
                                                         size_t n) {
    using uT = std::make_unsigned_t<T>;
    return reverse_find_n_impl<uT>(reinterpret_cast<const uT *>(src), val, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_reverse_find_not_n(const T *src0, const T *src1,
                                                           size_t n) {
    size_t idx = 0;
    src0 += n;
    src1 += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src0[-1 - idx] != src1[-1 - idx]) {
            break;
        }
    }

    return n - idx;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_not_n_impl(const T *src0,
                                                                  const T *src1,
                                                                  size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return 0;
    }

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_not_n(src0, src1, n);
        }

        return builtin_reverse_find_not_n(src0, src1, n);
    } else {
        return fallback_reverse_find_not_n(src0, src1, n);
    }
#else
    return fallback_reverse_find_not_n(src0, src1, n);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_not_n(const T *src0, const T *src1,
                                                             size_t n) {
    using uT = std::make_unsigned_t<T>;
    return reverse_find_not_n_impl<uT>(reinterpret_cast<const uT *>(src0),
                                       reinterpret_cast<const uT *>(src1), n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t fallback_reverse_find_not_n(const T *src, T val,
                                                           size_t n) {
    size_t idx = 0;
    src += n;

    WJR_UNROLL(4)
    for (; idx < n; ++idx) {
        if (src[-1 - idx] != val) {
            break;
        }
    }

    return n - idx;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_not_n_impl(const T *src,
                                                                  type_identity_t<T> val,
                                                                  size_t n) {
#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_find_not_n(src, val, n);
        }

        return builtin_reverse_find_not_n(src, val, n);
    } else {
        return fallback_reverse_find_not_n(src, val, n);
    }
#else
    return fallback_reverse_find_not_n(src, val, n);
#endif
}

template <typename T, std::enable_if_t<is_nonbool_integral_v<T>, int> = 0>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E size_t reverse_find_not_n(const T *src,
                                                             type_identity_t<T> val,
                                                             size_t n) {
    using uT = std::make_unsigned_t<T>;
    return reverse_find_not_n_impl<uT>(reinterpret_cast<const uT *>(src), val, n);
}

} // namespace wjr

#endif // WJR_MATH_FIND_HPP__
#ifndef WJR_MATH_SET_HPP__
#define WJR_MATH_SET_HPP__

#include <cstring>

// Already included
// Already included

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_SET_HPP__
#define WJR_X86_MATH_SET_HPP__

#include <cstring>

// Already included

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_SET_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SET_N)

template <typename T>
WJR_COLD void large_builtin_set_n(T *dst, T val, size_t n) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto is_avx = WJR_PP_BOOL(WJR_HAS_SIMD(AVX2));

    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto simd_loop = simd_width / nd;
    constexpr auto u8_loop = simd_width / 8;
    constexpr auto mask = u8_loop * 4;

    WJR_ASSUME(n > simd_loop * 4);

    auto y = simd::set1(val, T());

    simd::storeu((simd_int *)(dst), y);
    simd::storeu((simd_int *)(dst + n - simd_loop), y);
    simd::storeu((simd_int *)(dst + simd_loop), y);
    simd::storeu((simd_int *)(dst + n - simd_loop * 2), y);
    simd::storeu((simd_int *)(dst + simd_loop * 2), y);
    simd::storeu((simd_int *)(dst + n - simd_loop * 3), y);
    simd::storeu((simd_int *)(dst + simd_loop * 3), y);
    simd::storeu((simd_int *)(dst + n - simd_loop * 4), y);

    uintptr_t ps = reinterpret_cast<uintptr_t>(dst);
    uintptr_t pe = reinterpret_cast<uintptr_t>(dst + n);

    ps += mask;
    ps &= -mask;
    pe &= -mask;

    if (WJR_UNLIKELY(ps == pe)) {
        return;
    }

    uintptr_t mo = reinterpret_cast<uintptr_t>(dst) % sizeof(T);

    if (WJR_UNLIKELY(mo != 0)) {
        T stk[2] = {val, val};
        ::memcpy(&val, (char *)(stk) + mo, sizeof(T));
        y = simd::set1(val, T());
    }

    do {
        simd::store((simd_int *)(ps), y);
        simd::store((simd_int *)(ps + u8_loop * 1), y);
        simd::store((simd_int *)(ps + u8_loop * 2), y);
        simd::store((simd_int *)(ps + u8_loop * 3), y);

        ps += u8_loop * 4;
    } while (WJR_LIKELY(ps != pe));
    return;
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_set_n(T *dst, T val, size_t n) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto is_avx = WJR_PP_BOOL(WJR_HAS_SIMD(AVX2));

    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto simd_loop = simd_width / nd;

    using sse_int = typename sse::int_type;
    constexpr auto sse_width = sse::width();
    constexpr auto sse_loop = sse_width / nd;

    if (WJR_UNLIKELY(n == 0)) {
        return;
    }

    if (WJR_BUILTIN_CONSTANT_P(n) && n <= 4) {
        for (size_t i = 0; i < n; ++i) {
            dst[i] = val;
        }

        return;
    }

    if (WJR_UNLIKELY(n > simd_loop * 2)) {

        if (WJR_UNLIKELY(n > simd_loop * 4)) {
            return large_builtin_set_n(dst, val, n);
        }

        auto y = simd::set1(val, T());

        simd::storeu((simd_int *)(dst), y);
        simd::storeu((simd_int *)(dst + simd_loop), y);
        simd::storeu((simd_int *)(dst + n - simd_loop), y);
        simd::storeu((simd_int *)(dst + n - simd_loop * 2), y);
        return;
    }

    auto x = broadcast<T, uint64_t>(val);
    auto y = broadcast<uint64_t, __m128i_t>(x);

    if (WJR_UNLIKELY(n <= sse_loop * 2)) {
        if (WJR_UNLIKELY(n >= sse_loop)) {
            sse::storeu((sse_int *)(dst), y);
            sse::storeu((sse_int *)(dst + n - sse_loop), y);
            return;
        }

        if (WJR_UNLIKELY(n >= sse_loop / 2)) {
            ::memcpy(dst, &x, 8);
            if constexpr (sse_loop != 2) {
                ::memcpy(dst + n - sse_loop / 2, &x, 8);
            }
            return;
        }

        if constexpr (sse_loop >= 4) {
            if (WJR_UNLIKELY(n >= sse_loop / 4)) {
                ::memcpy(dst, &x, 4);
                if constexpr (sse_loop != 4) {
                    ::memcpy(dst + n - sse_loop / 4, &x, 4);
                }
                return;
            }
        }

        if constexpr (sse_loop >= 8) {
            if (WJR_UNLIKELY(n >= sse_loop / 8)) {
                ::memcpy(dst, &x, 2);
                if constexpr (sse_loop != 8) {
                    ::memcpy(dst + n - sse_loop / 8, &x, 2);
                }
                return;
            }
        }

        if constexpr (sse_loop >= 16) {
            if (WJR_UNLIKELY(n >= sse_loop / 16)) {
                ::memcpy(dst, &x, 1);
                if constexpr (sse_loop != 16) {
                    ::memcpy(dst + n - sse_loop / 16, &x, 1);
                }
                return;
            }
        }

        return;
    }

#if WJR_HAS_SIMD(AVX2)
    if constexpr (is_avx) {
        auto z = broadcast<__m128i_t, __m256i_t>(y);
        avx::storeu((simd_int *)(dst), z);
        avx::storeu((simd_int *)(dst + n - simd_loop), z);
        return;
    }
#endif

    WJR_UNREACHABLE();
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_SET_HPP__
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_set_n(T *dst, T val, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = val;
    }
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E void set_n(T *dst, type_identity_t<T> val, size_t n) {
#if WJR_HAS_BUILTIN(SET_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_set_n(dst, val, n);
        }

        if (WJR_BUILTIN_CONSTANT_P(val) && broadcast<uint8_t, T>(val) == val) {
            if (WJR_UNLIKELY(n >= 2048 / sizeof(T))) {
                ::memset(dst, static_cast<uint8_t>(val), n * sizeof(T));
                return;
            }
        }

        return builtin_set_n(dst, val, n);
    } else {
        return fallback_set_n(dst, val, n);
    }
#else
    return fallback_set_n(dst, val, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SET_HPP__

namespace wjr {

// find the first position(ret) that is not equal to number "from"
// and replace [0, ret) to number "to"
// For example, inc replaces a continuous segment of -1 with 0. And dec replaces a
// continuous segment of 0 with -1
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t replace_find_not(T *dst, const T *src, size_t n,
                                                  type_identity_t<T> from,
                                                  type_identity_t<T> to) {

    size_t ret = find_not_n(src, from, n);
    if (WJR_UNLIKELY(ret != 0) && WJR_LIKELY(dst != src || from != to)) {
        set_n(dst, to, ret);
    }

    return ret;
}

// find the last position(ret-1) that is not equal to number "from"
// and replace [ret, n) to number "to"
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E size_t reverse_replace_find_not(T *dst, const T *src, size_t n,
                                                          type_identity_t<T> from,
                                                          type_identity_t<T> to) {
    size_t ret = reverse_find_not_n(src, from, n);
    if (WJR_UNLIKELY(ret != n) && WJR_LIKELY(dst != src || from != to)) {
        set_n(dst + ret, to, n - ret);
    }

    return ret;
}

} // namespace wjr

#endif // WJR_MATH_REPLACE_HPP__
#ifndef WJR_MATH_SUB_IMPL_HPP__
#define WJR_MATH_SUB_IMPL_HPP__

// Already included

namespace wjr {

template <typename T, typename U,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E T subc(T a, T b, type_identity_t<U> c_in, U &c_out);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E T subc_cc(T a, T b, uint8_t c_in, uint8_t &c_out);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E bool sub_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret);

template <typename T, typename U = T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U subc_1(T *dst, const T *src0, size_t n,
                                   type_identity_t<T> src1, U c_in = 0);

template <typename T, typename U = T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U subc_n(T *dst, const T *src0, const T *src1, size_t n,
                                   U c_in = 0);

template <typename T, typename U = T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U subc_s(T *dst, const T *src0, size_t n, const T *src1,
                                   size_t m, U c_in = 0);

template <typename T, typename U = T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U subc_sz(T *dst, const T *src0, size_t n, const T *src1,
                                    size_t m, U c_in = 0);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_n(T *dst, const T *src0, const T *src1,
                                             size_t n);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_s(T *dst, const T *src0, size_t n,
                                             const T *src1, size_t m);

template <typename T, typename U,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_n(T *dst, const T *src0, const T *src1,
                                             size_t n, U &c_out, type_identity_t<U> cf0,
                                             type_identity_t<U> cf1);

// preview :

WJR_INTRINSIC_CONSTEXPR_E void __sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1, uint64_t hi1);

WJR_INTRINSIC_CONSTEXPR_E uint64_t __subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in);

WJR_INTRINSIC_CONSTEXPR_E uint8_t __subc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                                uint8_t c_in);

} // namespace wjr

#endif // WJR_MATH_SUB_IMPL_HPP__

#if defined(WJR_X86)
#ifndef WJR_X86_SUB_HPP__
#define WJR_X86_SUB_HPP__

// Already included

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_SUBC WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_SUBC_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN___ASM_SUB_128 WJR_HAS_DEF
#define WJR_HAS_BUILTIN___ASM_SUBC_128 WJR_HAS_DEF

#if WJR_HAS_FEATURE(INLINE_ASM_CCCOND)
#define WJR_HAS_BUILTIN_ASM_SUBC_CC WJR_HAS_DEF
#define WJR_HAS_BUILTIN___ASM_SUBC_CC_128 WJR_HAS_DEF
#endif

#endif

#if WJR_HAS_BUILTIN(ASM_SUBC)

template <typename U>
WJR_INTRINSIC_INLINE uint64_t asm_subc(uint64_t a, uint64_t b, U c_in, U &c_out) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 1) && c_in == 1) {
        if (WJR_BUILTIN_CONSTANT_P(b) && __is_in_i32_range(b)) {
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
                : "ri"(b), "0"(a)
                : "cc");
        } else {
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
                : "r"(b), "0"(a)
                : "cc");
        }
        c_out = c_in;
        return a;
    }

    if (WJR_BUILTIN_CONSTANT_P(b) && __is_in_i32_range(b)) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+&r"(c_in)
            : "ri"(b), "0"(a)
            : "cc");
    } else {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+&r"(c_in)
            : "r"(b), "0"(a)
            : "cc");
    }
    c_out = c_in;
    return a;
}

#endif

#if WJR_HAS_BUILTIN(ASM_SUBC_CC)

WJR_INTRINSIC_INLINE uint64_t asm_subc_cc(uint64_t a, uint64_t b, uint8_t c_in,
                                          uint8_t &c_out) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 1) && c_in == 1) {
        if (WJR_BUILTIN_CONSTANT_P(b) && __is_in_i32_range(b)) {
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t" WJR_ASM_CCSET(c)
                : "=r"(a), WJR_ASM_CCOUT(c)(c_out)
                : "ri"(b), "0"(a)
                : "cc");
        } else {
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t" WJR_ASM_CCSET(c)
                : "=r"(a), WJR_ASM_CCOUT(c)(c_out)
                : "r"(b), "0"(a)
                : "cc");
        }
        return a;
    }

    if (WJR_BUILTIN_CONSTANT_P(b) && __is_in_i32_range(b)) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
            : "=r"(a), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : "ri"(b), "0"(a)
            : "cc");
    } else {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
            : "=r"(a), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : "r"(b), "0"(a)
            : "cc");
    }
    return a;
}

#endif

#if WJR_HAS_BUILTIN(ASM_SUBC_N)
#define WJR_ADDSUB_I 0
// WJR_ADDSUB_I :
// 0 : SUB
// 1 : ADD

// Already included

#ifndef WJR_ADDSUB_I
#error "abort"
#endif

#define WJR_addcsubc WJR_PP_BOOL_IF(WJR_ADDSUB_I, addc, subc)
#define WJR_adcsbb WJR_PP_BOOL_IF(WJR_ADDSUB_I, adc, sbb)

inline uint64_t WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addcsubc, _n))(
    uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n, uint64_t c_in) {
    if (WJR_BUILTIN_CONSTANT_P(n)) {
        if (n == 1) {
            dst[0] = WJR_PP_CONCAT(asm_, WJR_addcsubc)(src0[0], src1[0], c_in, c_in);
            return c_in;
        }
    }

    size_t rcx = n / 8;
    uint64_t r8 = c_in, r9, r10 = n & 7, r11;

    asm volatile(
        "add{b $255, %b[r8]| %b[r8], 255}\n\t"
        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r9] + %[r10]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"
        
        ".align 8\n\t"
        ".Llookup%=:\n\t"
        ".long .Ll0%=-.Llookup%=\n\t"
        ".long .Ll1%=-.Llookup%=\n\t"
        ".long .Ll2%=-.Llookup%=\n\t"
        ".long .Ll3%=-.Llookup%=\n\t"
        ".long .Ll4%=-.Llookup%=\n\t"
        ".long .Ll5%=-.Llookup%=\n\t"
        ".long .Ll6%=-.Llookup%=\n\t"
        ".long .Ll7%=-.Llookup%=\n\t"
        ".align 16\n\t"
        
        ".Ll0%=:\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ld1%=:\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mov{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "jrcxz .Ld1%=\n\t"
        "mov{q 8(%[src0]), %[r9]| %[r9], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "lea{q 8(%[src0]), %[src0]| %[src0], [%[src0] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jmp .Lb1%=\n\t"

        ".Ll3%=:\n\t"
        "mov{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r8]| %[r8], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "lea{q -40(%[src0]), %[src0]| %[src0], [%[src0] - 40]}\n\t"
        "lea{q -40(%[src1]), %[src1]| %[src1], [%[src1] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "lea{q -32(%[src0]), %[src0]| %[src0], [%[src0] - 32]}\n\t"
        "lea{q -32(%[src1]), %[src1]| %[src1], [%[src1] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mov{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r9]| %[r9], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "lea{q -24(%[src0]), %[src0]| %[src0], [%[src0] - 24]}\n\t"
        "lea{q -24(%[src1]), %[src1]| %[src1], [%[src1] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mov{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r10]| %[r10], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "lea{q -16(%[src0]), %[src0]| %[src0], [%[src0] - 16]}\n\t"
        "lea{q -16(%[src1]), %[src1]| %[src1], [%[src1] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mov{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r8]| %[r8], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "lea{q -8(%[src0]), %[src0]| %[src0], [%[src0] - 8]}\n\t"
        "lea{q -8(%[src1]), %[src1]| %[src1], [%[src1] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld2%=:\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src1]), %[r10]| %[r10], [%[src1] + 8]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "mov{q %[r10], 8(%[dst])| [%[dst] + 8], %[r10]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll2%=:\n\t"
        "mov{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r10]| %[r10], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "jrcxz .Ld2%=\n\t"
        "lea{q 16(%[src0]), %[src0]| %[src0], [%[src0] + 16]}\n\t"
        "lea{q 16(%[src1]), %[src1]| %[src1], [%[src1] + 16]}\n\t"
        "lea{q 16(%[dst]), %[dst]| %[dst], [%[dst] + 16]}\n\t"

        ".align 32\n\t"
        ".Lloop%=:\n\t"

        ".Lb2%=:\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src1]), %[r10]| %[r10], [%[src1] - 8]}\n\t"
        "mov{q %[r8], -16(%[dst])| [%[dst] - 16], %[r8]}\n\t"

        ".Lb1%=:\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Lb0%=:\n\t"
        "mov{q 16(%[src0]), %[r8]| %[r8], [%[src0] + 16]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src1]), %[r11]| %[r11], [%[src1] + 8]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Lb7%=:\n\t"
        "mov{q 24(%[src0]), %[r10]| %[r10], [%[src0] + 24]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 16(%[src1]), %[r8]| %[r8], [%[src1] + 16]}\n\t"
        "mov{q %[r11], 8(%[dst])| [%[dst] + 8], %[r11]}\n\t"

        ".Lb6%=:\n\t"
        "mov{q 32(%[src0]), %[r9]| %[r9], [%[src0] + 32]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 24(%[src1]), %[r10]| %[r10], [%[src1] + 24]}\n\t"
        "mov{q %[r8], 16(%[dst])| [%[dst] + 16], %[r8]}\n\t"

        ".Lb5%=:\n\t"
        "mov{q 40(%[src0]), %[r11]| %[r11], [%[src0] + 40]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 32(%[src1]), %[r9]| %[r9], [%[src1] + 32]}\n\t"
        "mov{q %[r10], 24(%[dst])| [%[dst] + 24], %[r10]}\n\t"

        ".Lb4%=:\n\t"
        "mov{q 48(%[src0]), %[r8]| %[r8], [%[src0] + 48]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 40(%[src1]), %[r11]| %[r11], [%[src1] + 40]}\n\t"
        "mov{q %[r9], 32(%[dst])| [%[dst] + 32], %[r9]}\n\t"

        ".Lb3%=:\n\t"
        "mov{q 56(%[src0]), %[r10]| %[r10], [%[src0] + 56]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 48(%[src1]), %[r8]| %[r8], [%[src1] + 48]}\n\t"
        "mov{q %[r11], 40(%[dst])| [%[dst] + 40], %[r11]}\n\t"

        // TODO : optimize pipeline
        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "lea{q 64(%[src1]), %[src1]| %[src1], [%[src1] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"
        "dec %[rcx]\n\t"
        
        "jne .Lloop%=\n\t"

        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src1]), %[r10]| %[r10], [%[src1] - 8]}\n\t"
        "mov{q %[r8], -16(%[dst])| [%[dst] - 16], %[r8]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Ldone%=:\n\t"
        "mov %k[rcx], %k[r9]\n\t"
        "adc{l %k[rcx], %k[r9]| %k[r9], %k[rcx]}"

        : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+r"(src1), [rcx] "+c"(rcx), 
          [r8] "+r"(r8), [r9] "=&r"(r9), [r10] "+r"(r10), [r11] "=&r"(r11)
        :
        : "cc", "memory");

    WJR_ASSERT_ASSUME(rcx == 0);
    WJR_ASSERT_ASSUME(r9 <= 1);

    return r9;
}

#undef WJR_adcsbb
#undef WJR_addcsubc

#undef WJR_ADDSUB_I
#endif

#if WJR_HAS_BUILTIN(__ASM_SUB_128)

WJR_INTRINSIC_INLINE void __asm_sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                        uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return;
    }
    asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_SUBC_128) || WJR_HAS_BUILTIN(__ASM_SUBC_CC_128)

WJR_INTRINSIC_INLINE uint8_t __asm_subc_cc_zero_128(uint64_t &al, uint64_t &ah,
                                                    uint64_t lo0, uint64_t hi0,
                                                    uint64_t lo1, uint64_t hi1) {
    uint8_t c_out;
    if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_out;
    }

    asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), WJR_ASM_CCOUT(c)(c_out)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_out;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_SUBC_128)

WJR_INTRINSIC_INLINE uint64_t __asm_subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                             uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                             uint64_t c_in) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 0) && c_in == 0) {
        return __asm_subc_cc_zero_128(al, ah, lo0, hi0, lo1, hi1);
    }

    if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "sbb{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
            "setb %b[c_in]"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_in;
    }

    asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
        "sbb{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
        "setb %b[c_in]"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_in;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_SUBC_CC_128)

WJR_INTRINSIC_INLINE uint8_t __asm_subc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                               uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                               uint8_t c_in) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 0) && c_in == 0) {
        return __asm_subc_cc_zero_128(al, ah, lo0, hi0, lo1, hi1);
    }

    uint8_t c_out;
    if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "sbb{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in),
              WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_out;
    }

    asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
        "sbb{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_out;
}

#endif

} // namespace wjr

#endif // WJR_X86_SUB_HPP__
#endif

namespace wjr {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_subc(T a, T b, U c_in, U &c_out) {
    T ret = a - b;
    U c = ret > a;
    a = ret;
    ret -= c_in;
    c |= ret > a;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_subc)
#define WJR_HAS_BUILTIN_SUBC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_subc(T a, T b, U c_in, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_SUBC(suffix, type)                                          \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        type __c_out;                                                                    \
        T ret = __builtin_subc##suffix(a, b, static_cast<type>(c_in), &__c_out);         \
        c_out = static_cast<U>(__c_out);                                                 \
        return ret;                                                                      \
    } else

    WJR_REGISTER_BUILTIN_SUBC(b, unsigned char)
    WJR_REGISTER_BUILTIN_SUBC(s, unsigned short)
    WJR_REGISTER_BUILTIN_SUBC(, unsigned int)
    WJR_REGISTER_BUILTIN_SUBC(l, unsigned long)
    WJR_REGISTER_BUILTIN_SUBC(ll, unsigned long long) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_SUBC
}

#endif // WJR_HAS_BUILTIN(SUBC)

template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E T subc(T a, T b, type_identity_t<U> c_in, U &c_out) {
    WJR_ASSERT_ASSUME_L1(c_in <= 1);

#if !WJR_HAS_BUILTIN(SUBC) && !WJR_HAS_BUILTIN(ASM_SUBC)
    return fallback_subc(a, b, c_in, c_out);
#else
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P(x == 0) && x == 0 ? 2
               : WJR_BUILTIN_CONSTANT_P(x)              ? 1
                                                        : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_subc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ASM_SUBC), asm_subc,
                              WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(SUBC), builtin_subc,
                                             fallback_subc))(a, b, c_in, c_out);
    } else {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(SUBC), builtin_subc,
                              fallback_subc)(a, b, c_in, c_out);
    }
#endif
}

/*
 Used for subc and then jump according to cc flag. Therefore, the types of c_in and
 c_out are limited to uint8_t, while the default c_in and c_out types of normal subc are
 the same as T, so that the high register is not cleared. Currently, GCC/Clang @=cccond
 cannot know that the high register is not cleared, so the performance is worse than the
 normal version when cc flag is not needed immediately.
*/
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E T subc_cc(T a, T b, uint8_t c_in, uint8_t &c_out) {
    WJR_ASSERT_ASSUME_L1(c_in <= 1);

#if WJR_HAS_BUILTIN(ASM_SUBC_CC)
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P(x == 0) && x == 0 ? 2
               : WJR_BUILTIN_CONSTANT_P(x)              ? 1
                                                        : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_subc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return asm_subc_cc(a, b, c_in, c_out);
    } else {
        return subc(a, b, c_in, c_out);
    }
#else
    return subc(a, b, c_in, c_out);
#endif
}

#if WJR_HAS_BUILTIN(__builtin_sub_overflow)
#define WJR_HAS_BUILTIN_SUB_OVERFLOW WJR_HAS_DEF
#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E bool fallback_sub_overflow(T a, T b, T &ret) {
    ret = a - b;
    return ret > a;
}

#if WJR_HAS_BUILTIN(SUB_OVERFLOW)
template <typename T>
WJR_INTRINSIC_INLINE bool builtin_sub_overflow(T a, T b, T &ret) {
    return __builtin_sub_overflow(a, b, &ret);
}
#endif

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E bool sub_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret) {
#if WJR_HAS_BUILTIN(SUB_OVERFLOW)
    if (is_constant_evaluated() ||
        (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
        return fallback_sub_overflow(a, b, ret);
    }

    return builtin_sub_overflow(a, b, ret);
#else
    return fallback_sub_overflow(a, b, ret);
#endif
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
*/
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U subc_1(T *dst, const T *src0, size_t n,
                                   type_identity_t<T> src1, U c_in) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));

    uint8_t overflow = 0;
    dst[0] = subc_cc(src0[0], src1, c_in, overflow);

    if (overflow) {
        size_t idx = 1 + replace_find_not(dst + 1, src0 + 1, n - 1, 0, -1);

        if (WJR_UNLIKELY(idx == n)) {
            return static_cast<U>(1);
        }

        dst[idx] = src0[idx] - 1;

        dst += idx;
        src0 += idx;
        n -= idx;
    }

    if (src0 != dst) {
        std::copy(src0 + 1, src0 + n, dst + 1);
    }

    return static_cast<U>(0);
}

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U fallback_subc_n(T *dst, const T *src0, const T *src1, size_t n,
                                          U c_in) {
    size_t m = n / 4;

    for (size_t i = 0; i < m; ++i) {
        dst[0] = subc(src0[0], src1[0], c_in, c_in);
        dst[1] = subc(src0[1], src1[1], c_in, c_in);
        dst[2] = subc(src0[2], src1[2], c_in, c_in);
        dst[3] = subc(src0[3], src1[3], c_in, c_in);

        dst += 4;
        src0 += 4;
        src1 += 4;
    }

    n &= 3;
    if (WJR_UNLIKELY(n == 0)) {
        return c_in;
    }

    dst += n;
    src0 += n;
    src1 += n;

    switch (n) {
    case 3: {
        dst[-3] = subc(src0[-3], src1[-3], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 2: {
        dst[-2] = subc(src0[-2], src1[-2], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 1: {
        dst[-1] = subc(src0[-1], src1[-1], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    default: {
        break;
    }
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
3. WJR_IS_SAME_OR_INCR_P(dst, n, src1, n)
*/
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U subc_n(T *dst, const T *src0, const T *src1, size_t n,
                                   U c_in) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

#if WJR_HAS_BUILTIN(ASM_SUBC_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_subc_n(dst, src0, src1, n, c_in);
        }

        return asm_subc_n(dst, src0, src1, n, c_in);
    } else {
        return fallback_subc_n(dst, src0, src1, n, c_in);
    }
#else
    return fallback_subc_n(dst, src0, src1, n, c_in);
#endif
}

/*
require :
1. m >= 1
2. n >= m
3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
*/
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U subc_s(T *dst, const T *src0, size_t n, const T *src1,
                                   size_t m, U c_in) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    c_in = subc_n(dst, src0, src1, m, c_in);

    if (n != m) {
        c_in = subc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

/*
require :
1. n >= 0
2. n >= m
3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
*/
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U subc_sz(T *dst, const T *src0, size_t n, const T *src1,
                                    size_t m, U c_in) {
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_LIKELY(m != 0)) {
        c_in = subc_n(dst, src0, src1, m, c_in);
    }

    if (n != m) {
        c_in = subc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_SEPARATE_P(dst, n, src0, n)
3. WJR_IS_SAME_OR_SEPARATE_P(dst, n, src1, n)
return :
dst = abs(src0 - src1)
Absolute value represents non-zero pos
> 0 : src0 > src1
== 0 : src0 == src1
< 0 : src0 < src1
*/
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_n(T *dst, const T *src0, const T *src1,
                                             size_t n) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src1, n));

    size_t idx = reverse_find_not_n(src0, src1, n);

    if (WJR_UNLIKELY(idx != n)) {
        set_n(dst + idx, 0, n - idx);

        if (WJR_UNLIKELY(idx == 0)) {
            return 0;
        }
    }

    ssize_t ret = __fasts_from_unsigned(n);
    WJR_ASSUME(ret > 0);

    if (src0[idx - 1] < src1[idx - 1]) {
        std::swap(src0, src1);
        ret = __fasts_negate(ret);
    }

    (void)subc_n(dst, src0, src1, idx);
    return ret;
}

/*
require :
1. m >= 1
2. n >= m
3. WJR_IS_SAME_OR_SEPARATE_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_SEPARATE_P(dst, n, src1, m)
return :
dst = abs(src0 - src1)
Absolute value represents non-zero pos
> 0 : src0 > src1
== 0 : src0 == src1
< 0 : src0 < src1
*/
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_s(T *dst, const T *src0, size_t n,
                                             const T *src1, size_t m) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_BUILTIN_CONSTANT_P(n == m) && n == m) {
        return abs_subc_n(dst, src0, src1, m);
    }

    if (WJR_BUILTIN_CONSTANT_P(n - m <= 1) && n - m <= 1) {
        do {
            if (n == m) {
                break;
            }

            if (WJR_UNLIKELY(src0[m] == 0)) {
                dst[m] = 0;
                break;
            }

            (void)subc_s(dst, src0, m + 1, src1, m);
            return __fasts_from_unsigned(m + 1);
        } while (0);

        return abs_subc_n(dst, src0, src1, m);
    }

    size_t idx = reverse_replace_find_not(dst + m, src0 + m, n - m, 0, 0);

    if (WJR_UNLIKELY(idx == 0)) {
        return abs_subc_n(dst, src0, src1, m);
    }

    (void)subc_s(dst, src0, m + idx, src1, m);
    return __fasts_from_unsigned(m + idx);
}

// just like abs_subc_n.
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_n(T *dst, const T *src0, const T *src1,
                                             size_t n, U &c_out, type_identity_t<U> cf0,
                                             type_identity_t<U> cf1) {
    WJR_ASSERT_ASSUME(n >= 1);
    if (cf0 != cf1) {
        ssize_t ret = __fasts_from_unsigned(n);
        U cf = 0;
        if (cf0 < cf1) {
            std::swap(src0, src1);
            ret = __fasts_negate(ret);
            cf = cf1 - cf0;
        } else {
            cf = cf0 - cf1;
        }

        c_out = cf - subc_n(dst, src0, src1, n);
        return ret;
    } else {
        c_out = 0;
        return abs_subc_n(dst, src0, src1, n);
    }
}

WJR_INTRINSIC_CONSTEXPR void __fallback_sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1,
                                                uint64_t hi1) {
    uint64_t __al = lo0 - lo1;
    ah = hi0 - hi1 - (__al > lo0);
    al = __al;
}

#if WJR_HAS_FEATURE(FAST_INT128_ADDSUB)
#define WJR_HAS_BUILTIN___BUILTIN_SUB_128 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(__BUILTIN_SUBC_128)

WJR_INTRINSIC_INLINE void __builtin_sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                            uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    const auto x0 = static_cast<__uint128_t>(hi0) << 64 | lo0;
    const auto x1 = static_cast<__uint128_t>(hi1) << 64 | lo1;
    x0 -= x1;

    al = x0;
    ah = x0 >> 64;
}

#endif

// <ah, al> = <hi0, lo0> - <hi1, lo1>
WJR_INTRINSIC_CONSTEXPR_E void __sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1, uint64_t hi1) {
#if WJR_HAS_BUILTIN(__BUILTIN_SUB_128) || WJR_HAS_BUILTIN(__ASM_SUB_128)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(lo0 == 0) && lo0 == 0) ||
        (WJR_BUILTIN_CONSTANT_P(lo1 == 0) && lo1 == 0)) {
        return __fallback_sub_128(al, ah, lo0, hi0, lo1, hi1);
    }

    return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(__BUILTIN_SUB_128), __builtin_sub_128,
                          __asm_sub_128)(al, ah, lo0, hi0, lo1, hi1);
#else
    return __fallback_sub_128(al, ah, lo0, hi0, lo1, hi1);
#endif
}

WJR_INTRINSIC_CONSTEXPR_E uint64_t __fallback_subc_128(uint64_t &al, uint64_t &ah,
                                                       uint64_t lo0, uint64_t hi0,
                                                       uint64_t lo1, uint64_t hi1,
                                                       uint64_t c_in) {
    al = subc(lo0, lo1, c_in, c_in);
    ah = subc(hi0, hi1, c_in, c_in);
    return c_in;
}

// return c_out
WJR_INTRINSIC_CONSTEXPR_E uint64_t __subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in) {
#if WJR_HAS_BUILTIN(__ASM_ADDC_128)
    if (is_constant_evaluated()) {
        return __fallback_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return __asm_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return __fallback_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#endif
}

WJR_INTRINSIC_CONSTEXPR_E uint8_t __subc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                                uint8_t c_in) {
#if WJR_HAS_BUILTIN(__ASM_ADDC_CC_128)
    if (is_constant_evaluated()) {
        return __fallback_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return __asm_subc_cc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return __subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SUB_HPP__
// Already included

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_COMPARE_HPP__
#define WJR_X86_MATH_COMPARE_HPP__

// Already included
// Already included

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_COMPARE_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_REVERSE_COMPARE_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(COMPARE_N)

template <typename T>
WJR_COLD int large_builtin_compare_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_COMPARE_NOT_N_AVX(index)                                            \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 + (index)));                                \
        auto y = avx::loadu((__m256i *)(src1 + (index)));                                \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            auto offset = ctz(mask) / 8;                                                 \
            return src0[(index) + offset] < src1[(index) + offset] ? -1 : 1;             \
        }                                                                                \
    } while (0)

    size_t rem = n & 7;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src0 + (rem - 4)));
        auto x1 = sse::loadu((__m128i *)(src0 + (rem - 2)));
        auto y0 = sse::loadu((__m128i *)(src1 + (rem - 4)));
        auto y1 = sse::loadu((__m128i *)(src1 + (rem - 2)));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[(rem - 4) + 1] < src1[(rem - 4) + 1] ? -1 : 1;
                }
                return src0[(rem - 4)] < src1[(rem - 4)] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask == 0xFF00) {
                return src0[(rem - 2) + 1] < src1[(rem - 2) + 1] ? -1 : 1;
            }
            return src0[(rem - 2)] < src1[(rem - 2)] ? -1 : 1;
        }
#else
        WJR_REGISTER_COMPARE_NOT_N_AVX(rem - 4);
#endif
    }

    if (WJR_UNLIKELY(rem == n)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src0 + rem));
        auto x1 = sse::loadu((__m128i *)(src0 + rem + 2));
        auto x2 = sse::loadu((__m128i *)(src0 + rem + 4));
        auto x3 = sse::loadu((__m128i *)(src0 + rem + 6));
        auto y0 = sse::loadu((__m128i *)(src1 + rem));
        auto y1 = sse::loadu((__m128i *)(src1 + rem + 2));
        auto y2 = sse::loadu((__m128i *)(src1 + rem + 4));
        auto y3 = sse::loadu((__m128i *)(src1 + rem + 6));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto r3 = sse::cmpeq_epi64(x3, y3);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[rem + 1] < src1[rem + 1] ? -1 : 1;
                }
                return src0[rem] < src1[rem] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[rem + 3] < src1[rem + 3] ? -1 : 1;
                }
                return src0[rem + 2] < src1[rem + 2] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                if (mask == 0xFF00) {
                    return src0[rem + 5] < src1[rem + 5] ? -1 : 1;
                }
                return src0[rem + 4] < src1[rem + 4] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r3);
            if (mask == 0xFF00) {
                return src0[rem + 7] < src1[rem + 7] ? -1 : 1;
            }
            return src0[rem + 6] < src1[rem + 6] ? -1 : 1;
        }

        rem += 8;
    } while (WJR_LIKELY(rem != n));
#else
    if ((n - rem) & 8) {
        WJR_REGISTER_COMPARE_NOT_N_AVX(rem);
        WJR_REGISTER_COMPARE_NOT_N_AVX(rem + 4);

        rem += 8;

        if (WJR_UNLIKELY(rem == n)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src0 + rem));
        auto x1 = avx::loadu((__m256i *)(src0 + rem + 4));
        auto x2 = avx::loadu((__m256i *)(src0 + rem + 8));
        auto x3 = avx::loadu((__m256i *)(src0 + rem + 12));
        auto y0 = avx::loadu((__m256i *)(src1 + rem));
        auto y1 = avx::loadu((__m256i *)(src1 + rem + 4));
        auto y2 = avx::loadu((__m256i *)(src1 + rem + 8));
        auto y3 = avx::loadu((__m256i *)(src1 + rem + 12));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto r3 = avx::cmpeq_epi64(x3, y3);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r0);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = ctz(mask) / 8;
                return src0[rem + offset] < src1[rem + offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r1);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = ctz(mask) / 8;
                return src0[rem + offset + 4] < src1[rem + offset + 4] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r2);
            if (WJR_UNLIKELY(mask != 0)) {
                auto offset = ctz(mask) / 8;
                return src0[rem + offset + 8] < src1[rem + offset + 8] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r3);
            auto offset = ctz(mask) / 8;
            return src0[rem + offset + 12] < src1[rem + offset + 12] ? -1 : 1;
        }

        rem += 16;
    } while (WJR_LIKELY(rem != n));
#endif

    return 0;

#undef WJR_REGISTER_COMPARE_NOT_N_AVX
}

extern template WJR_COLD int
large_builtin_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE int builtin_compare_n(const T *src0, const T *src1, size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_LIKELY(src0[0] != src1[0])) {
        return src0[0] < src1[0] ? -1 : 1;
    }

    if (n == 1) {
        return 0;
    }

    if (WJR_LIKELY(src0[1] != src1[1])) {
        return src0[1] < src1[1] ? -1 : 1;
    }

    if (n == 2) {
        return 0;
    }

    if (WJR_LIKELY(src0[2] != src1[2])) {
        return src0[2] < src1[2] ? -1 : 1;
    }

    if (n == 3) {
        return 0;
    }

    if (WJR_LIKELY(src0[3] != src1[3])) {
        return src0[3] < src1[3] ? -1 : 1;
    }

    if (n == 4) {
        return 0;
    }

    return large_builtin_compare_n(src0, src1, n);
}

#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)

template <typename T>
WJR_COLD int large_builtin_reverse_compare_n(const T *src0, const T *src1, size_t n) {
#define WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(index)                                    \
    do {                                                                                 \
        auto x = avx::loadu((__m256i *)(src0 - 4 + (index)));                            \
        auto y = avx::loadu((__m256i *)(src1 - 4 + (index)));                            \
        auto r = avx::cmpeq_epi64(x, y);                                                 \
                                                                                         \
        avx::mask_type mask = ~avx::movemask_epi8(r);                                    \
        if (WJR_LIKELY(mask != 0)) {                                                     \
            auto offset = clz(mask) / 8;                                                 \
            return src0[(index)-1 - offset] < src1[(index)-1 - offset] ? -1 : 1;         \
        }                                                                                \
    } while (0)

    const size_t rem = n & 7;
    n -= rem;

    if (rem > 4) {
#if !WJR_HAS_SIMD(AVX2)
        auto x0 = sse::loadu((__m128i *)(src0 + n + 2));
        auto x1 = sse::loadu((__m128i *)(src0 + n));
        auto y0 = sse::loadu((__m128i *)(src1 + n + 2));
        auto y1 = sse::loadu((__m128i *)(src1 + n));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);

        if (WJR_LIKELY(!sse::test_all_ones(sse::And(r0, r1)))) {
            sse::mask_type mask = ~sse::movemask_epi8(r0);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[n + 2] < src1[n + 2] ? -1 : 1;
                }
                return src0[n + 3] < src1[n + 3] ? -1 : 1;
            }
            mask = ~sse::movemask_epi8(r1);
            if (mask == 0x00FF) {
                return src0[n] < src1[n] ? -1 : 1;
            }
            return src0[n + 1] < src1[n + 1] ? -1 : 1;
        }
#else
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(n + 4);
#endif
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

#if !WJR_HAS_SIMD(AVX2)
    do {
        auto x0 = sse::loadu((__m128i *)(src0 + n - 8));
        auto x1 = sse::loadu((__m128i *)(src0 + n - 6));
        auto x2 = sse::loadu((__m128i *)(src0 + n - 4));
        auto x3 = sse::loadu((__m128i *)(src0 + n - 2));
        auto y0 = sse::loadu((__m128i *)(src1 + n - 8));
        auto y1 = sse::loadu((__m128i *)(src1 + n - 6));
        auto y2 = sse::loadu((__m128i *)(src1 + n - 4));
        auto y3 = sse::loadu((__m128i *)(src1 + n - 2));

        auto r0 = sse::cmpeq_epi64(x0, y0);
        auto r1 = sse::cmpeq_epi64(x1, y1);
        auto r2 = sse::cmpeq_epi64(x2, y2);
        auto r3 = sse::cmpeq_epi64(x3, y3);

        auto z = sse::And(sse::And(r0, r1), sse::And(r2, r3));

        if (WJR_UNLIKELY(!sse::test_all_ones(z))) {
            sse::mask_type mask = ~sse::movemask_epi8(r3);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[n - 2] < src1[n - 2] ? -1 : 1;
                }
                return src0[n - 1] < src1[n - 1] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r2);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[n - 4] < src1[n - 4] ? -1 : 1;
                }
                return src0[n - 3] < src1[n - 3] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r1);
            if (mask != 0) {
                if (mask == 0x00FF) {
                    return src0[n - 6] < src1[n - 6] ? -1 : 1;
                }
                return src0[n - 5] < src1[n - 5] ? -1 : 1;
            }

            mask = ~sse::movemask_epi8(r0);
            if (mask == 0x00FF) {
                return src0[n - 8] < src1[n - 8] ? -1 : 1;
            }
            return src0[n - 7] < src1[n - 7] ? -1 : 1;
        }

        n -= 8;
    } while (WJR_LIKELY(n != 0));
#else
    if (n & 8) {
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(n);
        WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX(n - 4);

        n -= 8;

        if (WJR_UNLIKELY(n == 0)) {
            return 0;
        }
    }

    do {
        auto x0 = avx::loadu((__m256i *)(src0 + n - 16));
        auto x1 = avx::loadu((__m256i *)(src0 + n - 12));
        auto x2 = avx::loadu((__m256i *)(src0 + n - 8));
        auto x3 = avx::loadu((__m256i *)(src0 + n - 4));
        auto y0 = avx::loadu((__m256i *)(src1 + n - 16));
        auto y1 = avx::loadu((__m256i *)(src1 + n - 12));
        auto y2 = avx::loadu((__m256i *)(src1 + n - 8));
        auto y3 = avx::loadu((__m256i *)(src1 + n - 4));

        auto r0 = avx::cmpeq_epi64(x0, y0);
        auto r1 = avx::cmpeq_epi64(x1, y1);
        auto r2 = avx::cmpeq_epi64(x2, y2);
        auto r3 = avx::cmpeq_epi64(x3, y3);

        auto z = avx::And(avx::And(r0, r1), avx::And(r2, r3));

        if (WJR_UNLIKELY(!avx::test_all_ones(z))) {
            avx::mask_type mask = ~avx::movemask_epi8(r3);
            if (mask != 0) {
                auto offset = clz(mask) / 8;
                return src0[n - 1 - offset] < src1[n - 1 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r2);
            if (mask != 0) {
                auto offset = clz(mask) / 8;
                return src0[n - 5 - offset] < src1[n - 5 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r1);
            if (mask != 0) {
                auto offset = clz(mask) / 8;
                return src0[n - 9 - offset] < src1[n - 9 - offset] ? -1 : 1;
            }

            mask = ~avx::movemask_epi8(r0);
            auto offset = clz(mask) / 8;
            return src0[n - 13 - offset] < src1[n - 13 - offset] ? -1 : 1;
        }

        n -= 16;
    } while (WJR_LIKELY(n != 0));
#endif

    return 0;

#undef WJR_REGISTER_REVERSE_COMPARE_NOT_N_AVX
}

extern template WJR_COLD int
large_builtin_reverse_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                          size_t n);

template <typename T>
WJR_INTRINSIC_INLINE int builtin_reverse_compare_n(const T *src0, const T *src1,
                                                   size_t n) {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_LIKELY(src0[n - 1] != src1[n - 1])) {
        return src0[n - 1] < src1[n - 1] ? -1 : 1;
    }

    if (n == 1) {
        return 0;
    }

    if (WJR_LIKELY(src0[n - 2] != src1[n - 2])) {
        return src0[n - 2] < src1[n - 2] ? -1 : 1;
    }

    if (n == 2) {
        return 0;
    }

    if (WJR_LIKELY(src0[n - 3] != src1[n - 3])) {
        return src0[n - 3] < src1[n - 3] ? -1 : 1;
    }

    if (n == 3) {
        return 0;
    }

    if (WJR_LIKELY(src0[n - 4] != src1[n - 4])) {
        return src0[n - 4] < src1[n - 4] ? -1 : 1;
    }

    if (n == 4) {
        return 0;
    }

    return large_builtin_reverse_compare_n(src0, src1, n);
}

#endif

// __uint128_t has certain bugs in GCC 13.2, resulting in low performance
#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN___ASM_LESS_128 WJR_HAS_DEF
#define WJR_HAS_BUILTIN___ASM_LESS_EQUAL_128 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(__ASM_LESS_128)

WJR_INTRINSIC_INLINE bool __asm_less_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                         uint64_t hi1) {
    bool ret;
    asm("cmp{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(b)
        : WJR_ASM_CCOUT(b)(ret), [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    return ret;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_LESS_EQUAL_128)

WJR_INTRINSIC_INLINE bool __asm_less_equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                               uint64_t hi1) {
    bool ret;
    asm("cmp{q %[lo0], %[lo1]| %[lo1], %[lo0]}\n\t"
        "sbb{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t" WJR_ASM_CCSET(ae)
        : WJR_ASM_CCOUT(ae)(ret), [lo1] "+&r"(lo1), [hi1] "+r"(hi1)
        : [lo0] "r"(lo0), [hi0] "r"(hi0)
        : "cc");
    return ret;
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_COMPARE_HPP__
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR int fallback_compare_n(const T *src0, const T *src1, size_t n) {
    for (size_t idx = 0; idx < n; ++idx) {
        if (src0[idx] != src1[idx]) {
            return src0[idx] < src1[idx] ? -1 : 1;
        }
    }

    return 0;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E int compare_n(const T *src0, const T *src1, size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return 0;
    }

#if WJR_HAS_BUILTIN(COMPARE_N)
    if constexpr (sizeof(T) == 8) {
        static_assert(sizeof(T) != 8 || std::is_unsigned_v<T>, "T must be unsigned if sizeof(T) == 8");

        if (is_constant_evaluated()) {
            return fallback_compare_n(src0, src1, n);
        }

        return builtin_compare_n(src0, src1, n);
    } else {
        return fallback_compare_n(src0, src1, n);
    }
#else
    return fallback_compare_n(src0, src1, n);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR int fallback_reverse_compare_n(const T *src0, const T *src1,
                                                       size_t n) {
    src0 += n;
    src1 += n;

    for (size_t idx = 0; idx < n; ++idx) {
        if (src0[-1 - idx] != src1[-1 - idx]) {
            return src0[-1 - idx] < src1[-1 - idx] ? -1 : 1;
        }
    }

    return 0;
}

template <typename T>
WJR_PURE WJR_INTRINSIC_CONSTEXPR_E int reverse_compare_n(const T *src0, const T *src1,
                                                         size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return 0;
    }

#if WJR_HAS_BUILTIN(COMPARE_N)
    if constexpr (sizeof(T) == 8) {
        static_assert(sizeof(T) != 8 || std::is_unsigned_v<T>, "T must be unsigned if sizeof(T) == 8");
        
        if (is_constant_evaluated()) {
            return fallback_reverse_compare_n(src0, src1, n);
        }

        return builtin_reverse_compare_n(src0, src1, n);
    } else {
        return fallback_reverse_compare_n(src0, src1, n);
    }
#else
    return fallback_reverse_compare_n(src0, src1, n);
#endif
}

#if WJR_HAS_FEATURE(FAST_INT128_COMPARE)
#define WJR_HAS_BUILTIN___BUILTIN_LESS_128 WJR_HAS_DEF
#define WJR_HAS_BUILTIN___BUILTIN_LESS_EQUAL_128 WJR_HAS_DEF
#endif

WJR_INTRINSIC_CONSTEXPR_E bool __fallback_less_128(uint64_t lo0, uint64_t hi0,
                                                   uint64_t lo1, uint64_t hi1) {
    uint8_t f = lo0 < lo1;
    (void)subc_cc(hi0, hi1, f, f);
    return f;
}

#if WJR_HAS_BUILTIN(__BUILTIN_LESS_128)

WJR_INTRINSIC_INLINE bool __builtin_less_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                             uint64_t hi1) {
    const auto x0 = static_cast<__uint128_t>(hi0) << 64 | lo0;
    const auto x1 = static_cast<__uint128_t>(hi1) << 64 | lo1;

    return x0 < x1;
}

#endif

// return <hi0, lo0> < <hi1, lo1>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E bool __less_128(uint64_t lo0, uint64_t hi0,
                                                    uint64_t lo1, uint64_t hi1) {
#if WJR_HAS_BUILTIN(__BUILTIN_LESS_128) || WJR_HAS_BUILTIN(__ASM_LESS_128)
    if (is_constant_evaluated()) {
        return __fallback_less_128(lo0, hi0, lo1, hi1);
    }

    return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(__BUILTIN_LESS_128), __builtin_less_128,
                          __asm_less_128)(lo0, hi0, lo1, hi1);
#else
    return __fallback_less_128(lo0, hi0, lo1, hi1);
#endif
}

WJR_INTRINSIC_CONSTEXPR_E bool __fallback_less_equal_128(uint64_t lo0, uint64_t hi0,
                                                         uint64_t lo1, uint64_t hi1) {
    return !__less_128(lo1, hi1, lo0, hi0);
}

#if WJR_HAS_BUILTIN(__BUILTIN_LESS_EQUAL_128)

WJR_INTRINSIC_INLINE bool __builtin_less_equal_128(uint64_t lo0, uint64_t hi0,
                                                   uint64_t lo1, uint64_t hi1) {
    const auto x0 = static_cast<__uint128_t>(hi0) << 64 | lo0;
    const auto x1 = static_cast<__uint128_t>(hi1) << 64 | lo1;

    return x0 <= x1;
}

#endif

// return <hi0, lo0> < <hi1, lo1>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E bool __less_equal_128(uint64_t lo0, uint64_t hi0,
                                                          uint64_t lo1, uint64_t hi1) {
#if WJR_HAS_BUILTIN(__BUILTIN_LESS_EQUAL_128) || WJR_HAS_BUILTIN(__ASM_LESS_EQUAL_128)
    if (is_constant_evaluated()) {
        return __fallback_less_equal_128(lo0, hi0, lo1, hi1);
    }

    return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(__BUILTIN_LESS_EQUAL_128),
                          __builtin_less_equal_128,
                          __asm_less_equal_128)(lo0, hi0, lo1, hi1);
#else
    return __fallback_less_equal_128(lo0, hi0, lo1, hi1);
#endif
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR_E bool __greater_128(uint64_t lo0, uint64_t hi0,
                                                       uint64_t lo1, uint64_t hi1) {
    return __less_128(lo1, hi1, lo0, hi0);
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR_E bool __greater_equal_128(uint64_t lo0, uint64_t hi0,
                                                             uint64_t lo1, uint64_t hi1) {
    return __less_equal_128(lo1, hi1, lo0, hi0);
}

} // namespace wjr

#endif // WJR_MATH_CMP_HPP__
#ifndef WJR_MATH_DIV_IMPL_HPP__
#define WJR_MATH_DIV_IMPL_HPP__

#include <utility>

// Already included

namespace wjr {

template <typename T>
class div1by1_divider;

template <typename T>
class div2by1_divider;

template <typename T>
class div3by2_divider;

template <typename T>
class divexact1_divider;

inline uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                               const div2by1_divider<uint64_t> &divider);

inline uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi, uint64_t div);

inline std::pair<uint64_t, uint64_t>
div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                const div2by1_divider<uint64_t> &divider);

inline std::pair<uint64_t, uint64_t> div128by64to128(uint64_t &rem, uint64_t lo,
                                                     uint64_t hi, uint64_t div);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        const div2by1_divider<T> &div);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        type_identity_t<T> div);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                        const div3by2_divider<T> &div);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                        const T *div);

template <typename T>
void div_qr_s(T *dst, T *rem, const T *src, size_t n, const T *div, size_t m);

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E T divexact_dbm1c(T *dst, const T *src, size_t n, T bd, T h);

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E void divexact_by3(T *dst, const T *src, size_t n);

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E void divexact_by5(T *dst, const T *src, size_t n);

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E void divexact_by15(T *dst, const T *src, size_t n);

template <typename T, T c, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E void divexact_byc(T *dst, const T *src, size_t n,
                                  std::integral_constant<T, c>);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          const divexact1_divider<T> &div);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          type_identity_t<T> div);

} // namespace wjr

#endif // WJR_MATH_DIV_IMPL_HPP__
#ifndef WJR_MATH_DIVIDER_HPP__
#define WJR_MATH_DIVIDER_HPP__

#ifndef WJR_MATH_MUL_HPP__
#define WJR_MATH_MUL_HPP__

/**
 * @todo optimize temporary memory usage of mul_s, mul_n, sqr
 *
 */

#ifndef WJR_MATH_BIGNUM_CONFIG_HPP__
#define WJR_MATH_BIGNUM_CONFIG_HPP__

#ifndef WJR_TOOM22_MUL_THRESHOLD
#define WJR_TOOM22_MUL_THRESHOLD 26
#endif

#ifndef WJR_TOOM33_MUL_THRESHOLD
#define WJR_TOOM33_MUL_THRESHOLD 72
#endif

#ifndef WJR_TOOM44_MUL_THRESHOLD
#define WJR_TOOM44_MUL_THRESHOLD 208
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

#ifndef WJR_TOOM2_SQR_THRESHOLD
#define WJR_TOOM2_SQR_THRESHOLD 32
#endif

#ifndef WJR_TOOM3_SQR_THRESHOLD
#define WJR_TOOM3_SQR_THRESHOLD 117
#endif

#ifndef WJR_TOOM4_SQR_THRESHOLD
#define WJR_TOOM4_SQR_THRESHOLD 336
#endif

#ifndef WJR_DC_DIV_QR_THRESHOLD
#define WJR_DC_DIV_QR_THRESHOLD (WJR_TOOM22_MUL_THRESHOLD * 2)
#endif // WJR_DC_DIV_QR_THRESHOLD

#ifndef WJR_DC_BIGNUM_TO_CHARS_THRESHOLD
#define WJR_DC_BIGNUM_TO_CHARS_THRESHOLD 20
#endif

#ifndef WJR_DC_BIGNUM_TO_CHARS_PRECOMPUTE_THRESHOLD
#define WJR_DC_BIGNUM_TO_CHARS_PRECOMPUTE_THRESHOLD 20
#endif

#ifndef WJR_DC_BIGNUM_FROM_CHARS_THRESHOLD
#define WJR_DC_BIGNUM_FROM_CHARS_THRESHOLD 1670
#endif

#ifndef WJR_DC_BIGNUM_FROM_CHARS_PRECOMPUTE_THRESHOLD
#define WJR_DC_BIGNUM_FROM_CHARS_PRECOMPUTE_THRESHOLD 3105
#endif

#endif // WJR_MATH_BIGNUM_CONFIG_HPP__
// Already included

#ifndef WJR_MATH_ADD_HPP__
#define WJR_MATH_ADD_HPP__

// Already included
#ifndef WJR_MATH_ADD_IMPL_HPP__
#define WJR_MATH_ADD_IMPL_HPP__

// Already included

namespace wjr {

template <typename T, typename U,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E T addc(T a, T b, type_identity_t<U> c_in, U &c_out);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E T addc_cc(T a, T b, uint8_t c_in, uint8_t &c_out);

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E bool add_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret);

template <typename T, typename U = T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U addc_1(T *dst, const T *src0, size_t n,
                                   type_identity_t<T> src1, U c_in = 0);

template <typename T, typename U = T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U addc_n(T *dst, const T *src0, const T *src1, size_t n,
                                   U c_in = 0);

template <typename T, typename U = T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U addc_s(T *dst, const T *src0, size_t n, const T *src1,
                                   size_t m, U c_in = 0);

// m can be zero
template <typename T, typename U = T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>,
                           int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U addc_sz(T *dst, const T *src0, size_t n, const T *src1,
                                    size_t m, U c_in = 0);

WJR_INTRINSIC_CONSTEXPR_E void __add_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1, uint64_t hi1);

WJR_INTRINSIC_CONSTEXPR_E uint64_t __addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in);

WJR_INTRINSIC_CONSTEXPR_E uint8_t __addc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                                uint8_t c_in);

} // namespace wjr

#endif // WJR_MATH_ADD_IMPL_HPP__
// Already included

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_ADD_HPP__
#define WJR_X86_MATH_ADD_HPP__

// Already included

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_ADDC WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_ADDC_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN___ASM_ADD_128 WJR_HAS_DEF
#define WJR_HAS_BUILTIN___ASM_ADDC_128 WJR_HAS_DEF

#if WJR_HAS_FEATURE(INLINE_ASM_CCCOND)
#define WJR_HAS_BUILTIN_ASM_ADDC_CC WJR_HAS_DEF
#define WJR_HAS_BUILTIN___ASM_ADDC_CC_128 WJR_HAS_DEF
#endif

#endif

#if WJR_HAS_BUILTIN(ASM_ADDC)

/**
 * @brief Use inline assembly to add two 64-bit integers with carry-in and return the
 * carry-out.
 *
 * @details The carry-in and carry-out flags are both 0 or 1. \n
 * The carry-out flag is set to 1 if the result overflows. \n
 * Optimization: \n
 * 1. Use constraint "i" if a or b is a constant and is in i32 range. \n
 * 2. If c_in is a constant and c_in == 1, use "stc" to set the carry flag.
 *
 * @tparam U The type of the carry.
 * @param[in] c_in The carry-in flag.
 * @param[out] c_out The carry-out flag.
 * @return a + b + c_in
 */
template <typename U>
WJR_INTRINSIC_INLINE uint64_t asm_addc(uint64_t a, uint64_t b, U c_in, U &c_out) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 1) && c_in == 1) {
        if (WJR_BUILTIN_CONSTANT_P(b) && __is_in_i32_range(b)) {
            asm("stc\n\t"
                "adc{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
                : "ri"(b), "0"(a)
                : "cc");
        } else {
            asm("stc\n\t"
                "adc{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
                : "r"(b), "0"(a)
                : "cc");
        }
        c_out = c_in;
        return a;
    }

    if (WJR_BUILTIN_CONSTANT_P(a)) {
        if (__is_in_i32_range(a)) {
            asm("add{b $255, %b1| %b1, 255}\n\t"
                "adc{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(b), "+&r"(c_in)
                : "ri"(a), "0"(b)
                : "cc");
        } else {
            asm("add{b $255, %b1| %b1, 255}\n\t"
                "adc{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(b), "+&r"(c_in)
                : "r"(a), "0"(b)
                : "cc");
        }
        c_out = c_in;
        return b;
    }

    if (WJR_BUILTIN_CONSTANT_P(b) && __is_in_i32_range(b)) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "adc{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+&r"(c_in)
            : "ri"(b), "0"(a)
            : "cc");
    } else {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "adc{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+&r"(c_in)
            : "r"(b), "0"(a)
            : "cc");
    }
    c_out = c_in;
    return a;
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDC_CC)

/**
 * @brief Use inline assembly to add two 64-bit integers with carry-in and return the
 * carry-out.
 *
 * @details Similar to asm_addc, but the carry-out flag is set by using constraint
 * "=@cccond" instead of "setb". \n
 *
 * @param[in] c_in
 * @param[out] c_out
 */
WJR_INTRINSIC_INLINE uint64_t asm_addc_cc(uint64_t a, uint64_t b, uint8_t c_in,
                                          uint8_t &c_out) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 1) && c_in == 1) {
        if (WJR_BUILTIN_CONSTANT_P(b) && __is_in_i32_range(b)) {
            asm("stc\n\t"
                "adc{q %2, %0| %0, %2}\n\t" WJR_ASM_CCSET(c)
                : "=r"(a), WJR_ASM_CCOUT(c)(c_out)
                : "ri"(b), "0"(a)
                : "cc");
        } else {
            asm("stc\n\t"
                "adc{q %2, %0| %0, %2}\n\t" WJR_ASM_CCSET(c)
                : "=r"(a), WJR_ASM_CCOUT(c)(c_out)
                : "r"(b), "0"(a)
                : "cc");
        }
        return a;
    }

    if (WJR_BUILTIN_CONSTANT_P(a)) {
        if (__is_in_i32_range(a)) {
            asm("add{b $255, %b1| %b1, 255}\n\t"
                "adc{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
                : "=r"(b), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
                : "ri"(a), "0"(b)
                : "cc");
        } else {
            asm("add{b $255, %b1| %b1, 255}\n\t"
                "adc{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
                : "=r"(b), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
                : "r"(a), "0"(b)
                : "cc");
        }
        return b;
    }

    if (WJR_BUILTIN_CONSTANT_P(b) && __is_in_i32_range(b)) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "adc{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
            : "=r"(a), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : "ri"(b), "0"(a)
            : "cc");
    } else {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "adc{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
            : "=r"(a), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : "r"(b), "0"(a)
            : "cc");
    }
    return a;
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDC_N)
#define WJR_ADDSUB_I 1
// WJR_ADDSUB_I :
// 0 : SUB
// 1 : ADD

// Already included

#ifndef WJR_ADDSUB_I
#error "abort"
#endif

#define WJR_addcsubc WJR_PP_BOOL_IF(WJR_ADDSUB_I, addc, subc)
#define WJR_adcsbb WJR_PP_BOOL_IF(WJR_ADDSUB_I, adc, sbb)

inline uint64_t WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addcsubc, _n))(
    uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n, uint64_t c_in) {
    if (WJR_BUILTIN_CONSTANT_P(n)) {
        if (n == 1) {
            dst[0] = WJR_PP_CONCAT(asm_, WJR_addcsubc)(src0[0], src1[0], c_in, c_in);
            return c_in;
        }
    }

    size_t rcx = n / 8;
    uint64_t r8 = c_in, r9, r10 = n & 7, r11;

    asm volatile(
        "add{b $255, %b[r8]| %b[r8], 255}\n\t"
        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r9] + %[r10]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"
        
        ".align 8\n\t"
        ".Llookup%=:\n\t"
        ".long .Ll0%=-.Llookup%=\n\t"
        ".long .Ll1%=-.Llookup%=\n\t"
        ".long .Ll2%=-.Llookup%=\n\t"
        ".long .Ll3%=-.Llookup%=\n\t"
        ".long .Ll4%=-.Llookup%=\n\t"
        ".long .Ll5%=-.Llookup%=\n\t"
        ".long .Ll6%=-.Llookup%=\n\t"
        ".long .Ll7%=-.Llookup%=\n\t"
        ".align 16\n\t"
        
        ".Ll0%=:\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ld1%=:\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mov{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "jrcxz .Ld1%=\n\t"
        "mov{q 8(%[src0]), %[r9]| %[r9], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "lea{q 8(%[src0]), %[src0]| %[src0], [%[src0] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jmp .Lb1%=\n\t"

        ".Ll3%=:\n\t"
        "mov{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r8]| %[r8], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "lea{q -40(%[src0]), %[src0]| %[src0], [%[src0] - 40]}\n\t"
        "lea{q -40(%[src1]), %[src1]| %[src1], [%[src1] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "lea{q -32(%[src0]), %[src0]| %[src0], [%[src0] - 32]}\n\t"
        "lea{q -32(%[src1]), %[src1]| %[src1], [%[src1] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mov{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r9]| %[r9], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "lea{q -24(%[src0]), %[src0]| %[src0], [%[src0] - 24]}\n\t"
        "lea{q -24(%[src1]), %[src1]| %[src1], [%[src1] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mov{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r10]| %[r10], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "lea{q -16(%[src0]), %[src0]| %[src0], [%[src0] - 16]}\n\t"
        "lea{q -16(%[src1]), %[src1]| %[src1], [%[src1] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mov{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r8]| %[r8], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "lea{q -8(%[src0]), %[src0]| %[src0], [%[src0] - 8]}\n\t"
        "lea{q -8(%[src1]), %[src1]| %[src1], [%[src1] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld2%=:\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src1]), %[r10]| %[r10], [%[src1] + 8]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "mov{q %[r10], 8(%[dst])| [%[dst] + 8], %[r10]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll2%=:\n\t"
        "mov{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r10]| %[r10], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "jrcxz .Ld2%=\n\t"
        "lea{q 16(%[src0]), %[src0]| %[src0], [%[src0] + 16]}\n\t"
        "lea{q 16(%[src1]), %[src1]| %[src1], [%[src1] + 16]}\n\t"
        "lea{q 16(%[dst]), %[dst]| %[dst], [%[dst] + 16]}\n\t"

        ".align 32\n\t"
        ".Lloop%=:\n\t"

        ".Lb2%=:\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src1]), %[r10]| %[r10], [%[src1] - 8]}\n\t"
        "mov{q %[r8], -16(%[dst])| [%[dst] - 16], %[r8]}\n\t"

        ".Lb1%=:\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Lb0%=:\n\t"
        "mov{q 16(%[src0]), %[r8]| %[r8], [%[src0] + 16]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src1]), %[r11]| %[r11], [%[src1] + 8]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Lb7%=:\n\t"
        "mov{q 24(%[src0]), %[r10]| %[r10], [%[src0] + 24]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 16(%[src1]), %[r8]| %[r8], [%[src1] + 16]}\n\t"
        "mov{q %[r11], 8(%[dst])| [%[dst] + 8], %[r11]}\n\t"

        ".Lb6%=:\n\t"
        "mov{q 32(%[src0]), %[r9]| %[r9], [%[src0] + 32]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 24(%[src1]), %[r10]| %[r10], [%[src1] + 24]}\n\t"
        "mov{q %[r8], 16(%[dst])| [%[dst] + 16], %[r8]}\n\t"

        ".Lb5%=:\n\t"
        "mov{q 40(%[src0]), %[r11]| %[r11], [%[src0] + 40]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 32(%[src1]), %[r9]| %[r9], [%[src1] + 32]}\n\t"
        "mov{q %[r10], 24(%[dst])| [%[dst] + 24], %[r10]}\n\t"

        ".Lb4%=:\n\t"
        "mov{q 48(%[src0]), %[r8]| %[r8], [%[src0] + 48]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 40(%[src1]), %[r11]| %[r11], [%[src1] + 40]}\n\t"
        "mov{q %[r9], 32(%[dst])| [%[dst] + 32], %[r9]}\n\t"

        ".Lb3%=:\n\t"
        "mov{q 56(%[src0]), %[r10]| %[r10], [%[src0] + 56]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 48(%[src1]), %[r8]| %[r8], [%[src1] + 48]}\n\t"
        "mov{q %[r11], 40(%[dst])| [%[dst] + 40], %[r11]}\n\t"

        // TODO : optimize pipeline
        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "lea{q 64(%[src1]), %[src1]| %[src1], [%[src1] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"
        "dec %[rcx]\n\t"
        
        "jne .Lloop%=\n\t"

        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src1]), %[r10]| %[r10], [%[src1] - 8]}\n\t"
        "mov{q %[r8], -16(%[dst])| [%[dst] - 16], %[r8]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Ldone%=:\n\t"
        "mov %k[rcx], %k[r9]\n\t"
        "adc{l %k[rcx], %k[r9]| %k[r9], %k[rcx]}"

        : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+r"(src1), [rcx] "+c"(rcx), 
          [r8] "+r"(r8), [r9] "=&r"(r9), [r10] "+r"(r10), [r11] "=&r"(r11)
        :
        : "cc", "memory");

    WJR_ASSERT_ASSUME(rcx == 0);
    WJR_ASSERT_ASSUME(r9 <= 1);

    return r9;
}

#undef WJR_adcsbb
#undef WJR_addcsubc

#undef WJR_ADDSUB_I
#endif

#if WJR_HAS_BUILTIN(__ASM_ADD_128)

/**
 * @brief Use inline assembly to add two 64-bit integers and return the result.
 *
 */
WJR_INTRINSIC_INLINE void __asm_add_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                        uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= UINT32_MAX) {
        asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi0], %[hi1]| %[hi1], %[hi0]}"
            : [lo0] "+&r"(lo0), [hi1] "+r"(hi1)
            : [lo1] "r"(lo1), [hi0] "i"(hi0)
            : "cc");
        al = lo0;
        ah = hi1;
        return;
    } else if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return;
    }

    asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_ADDC_128) || WJR_HAS_BUILTIN(__ASM_ADDC_CC_128)

/**
 * @brief Use inline assembly to add two 64-bit integers and return the
 * carry-out.
 *
 * @details Optimzation for __asm_addc_cc_128 and __asm_addc_128 when the carry-in is 0.
 *
 */
WJR_INTRINSIC_INLINE uint8_t __asm_addc_cc_zero_128(uint64_t &al, uint64_t &ah,
                                                    uint64_t lo0, uint64_t hi0,
                                                    uint64_t lo1, uint64_t hi1) {
    uint8_t c_out;
    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= UINT32_MAX) {
        asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi1] "+r"(hi1), WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi0] "i"(hi0)
            : "cc");
        al = lo0;
        ah = hi1;
        return c_out;
    } else if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_out;
    }

    asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), WJR_ASM_CCOUT(c)(c_out)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_out;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_ADDC_128)

/**
 * @brief Use inline assembly to add two 64-bit integers with carry-in and return the
 * carry-out.
 *
 */
WJR_INTRINSIC_INLINE uint64_t __asm_addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                             uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                             uint64_t c_in) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 0) && c_in == 0) {
        return __asm_addc_cc_zero_128(al, ah, lo0, hi0, lo1, hi1);
    }

    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t"
            "setb %b[c_in]"
            : [lo0] "+&r"(lo0), [hi1] "+r"(hi1), [c_in] "+&r"(c_in)
            : [lo1] "r"(lo1), [hi0] "i"(hi0)
            : "cc");
        al = lo0;
        ah = hi1;
        return c_in;
    } else if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
            "setb %b[c_in]"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_in;
    }

    asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
        "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
        "setb %b[c_in]"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_in;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_ADDC_CC_128)

/**
 * @brief Use inline assembly to add two 64-bit integers with carry-in and return the
 * carry-out.
 *
 * @details Similar to __asm_addc_128, but the carry-out flag is set by using constraint
 * "=@cccond" instead of "setb".
 *
 */
WJR_INTRINSIC_INLINE uint8_t __asm_addc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                               uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                               uint8_t c_in) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 0) && c_in == 0) {
        return __asm_addc_cc_zero_128(al, ah, lo0, hi0, lo1, hi1);
    }

    uint8_t c_out;
    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi1] "+r"(hi1), [c_in] "+&r"(c_in),
              WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi0] "i"(hi0)
            : "cc");
        al = lo0;
        ah = hi1;
        return c_out;
    } else if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in),
              WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_out;
    }

    asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
        "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_out;
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_ADD_HPP__
#endif

namespace wjr {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_addc(T a, T b, U c_in, U &c_out) {
    T ret = a;
    ret += b;
    U c = ret < b;
    ret += c_in;
    c |= ret < c_in;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_addc)
#define WJR_HAS_BUILTIN_ADDC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ADDC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_addc(T a, T b, U c_in, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ADDC(suffix, type)                                          \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        type __c_out;                                                                    \
        T ret = __builtin_addc##suffix(a, b, static_cast<type>(c_in), &__c_out);         \
        c_out = static_cast<U>(__c_out);                                                 \
        return ret;                                                                      \
    } else

    WJR_REGISTER_BUILTIN_ADDC(b, unsigned char)
    WJR_REGISTER_BUILTIN_ADDC(s, unsigned short)
    WJR_REGISTER_BUILTIN_ADDC(, unsigned int)
    WJR_REGISTER_BUILTIN_ADDC(l, unsigned long)
    WJR_REGISTER_BUILTIN_ADDC(ll, unsigned long long) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ADDC
}

#endif // WJR_HAS_BUILTIN(ADDC)

/**
 * @brief Add two numbers with carry-in, and return the result and carry-out
 *
 * @note The carry-in and carry-out are limited to 0 and 1
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral type.
 * the default type is the same as `T`
 * @param[in] c_in The carry-in flag.
 * @param[out] c_out The carry-out flag.
 */
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E T addc(T a, T b, type_identity_t<U> c_in, U &c_out) {
    WJR_ASSERT_ASSUME_L1(c_in <= 1);

#if !WJR_HAS_BUILTIN(ADDC) && !WJR_HAS_BUILTIN(ASM_ADDC)
    return fallback_addc(a, b, c_in, c_out);
#else
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P(x == 0) && x == 0 ? 2
               : WJR_BUILTIN_CONSTANT_P(x)              ? 1
                                                        : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_addc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ASM_ADDC), asm_addc,
                              WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ADDC), builtin_addc,
                                             fallback_addc))(a, b, c_in, c_out);
    } else {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ADDC), builtin_addc,
                              fallback_addc)(a, b, c_in, c_out);
    }
#endif
}

/**
 * @brief Performs addition with carry-in and carry-out, optimized for subsequent
 * branching based on carry-out.
 *
 * @details This function, `addc_cc`, adds two numbers with a carry-in, and returns the
 * result and a carry-out. The carry-out (`c_out`) is optimized for subsequent code that
 * branches based on its value. For example, it can be used with jump instructions like
 * `je` or `jne`. This is in contrast to the `addc` function, which may use instructions
 * like `setc` or `test` for branching.
 *
 * @note The carry-in and carry-out are limited to 0 and 1
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral type.
 * @param[in] c_in The carry-in flag.
 * @param[out] c_out The carry-out flag.
 */
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E T addc_cc(T a, T b, uint8_t c_in, uint8_t &c_out) {
    WJR_ASSERT_ASSUME_L1(c_in <= 1);

#if WJR_HAS_BUILTIN(ASM_ADDC_CC)
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P(x == 0) && x == 0 ? 2
               : WJR_BUILTIN_CONSTANT_P(x)              ? 1
                                                        : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_addc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return asm_addc_cc(a, b, c_in, c_out);
    } else {
        return addc(a, b, c_in, c_out);
    }
#else
    return addc(a, b, c_in, c_out);
#endif
}

#if WJR_HAS_BUILTIN(__builtin_add_overflow)
#define WJR_HAS_BUILTIN_ADD_OVERFLOW WJR_HAS_DEF
#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E bool fallback_add_overflow(T a, T b, T &ret) {
    ret = a + b;
    return ret < a;
}

#if WJR_HAS_BUILTIN(ADD_OVERFLOW)
template <typename T>
WJR_INTRINSIC_INLINE bool builtin_add_overflow(T a, T b, T &ret) {
    return __builtin_add_overflow(a, b, &ret);
}
#endif

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E bool add_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret) {
#if WJR_HAS_BUILTIN(ADD_OVERFLOW)
    if (is_constant_evaluated() ||
        (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
        return fallback_add_overflow(a, b, ret);
    }

    return builtin_add_overflow(a, b, ret);
#else
    return fallback_add_overflow(a, b, ret);
#endif
}

/**
 * @brief Add biginteger(src0) and number with carry-in, and return the result(dst) and
 * carry-out.
 *
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral type.
 * @param[out] dst The result of the addition.
 * @param[in] src0 The biginteger to be added.
 * @param[in] n The number of elements in the biginteger.
 * @param[in] src1 The number to be added.
 * @param[in] c_in The carry-in flag.
 * @return The carry-out flag.
 */
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U addc_1(T *dst, const T *src0, size_t n,
                                   type_identity_t<T> src1, U c_in) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_ASSUME(c_in <= 1);

    uint8_t overflow = 0;
    dst[0] = addc_cc(src0[0], src1, c_in, overflow);

    if (overflow) {
        size_t idx = 1 + replace_find_not(dst + 1, src0 + 1, n - 1, -1, 0);

        if (WJR_UNLIKELY(idx == n)) {
            return static_cast<U>(1);
        }

        dst[idx] = src0[idx] + 1;

        dst += idx;
        src0 += idx;
        n -= idx;
    }

    if (src0 != dst) {
        std::copy(src0 + 1, src0 + n, dst + 1);
    }

    return static_cast<U>(0);
}

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U fallback_addc_n(T *dst, const T *src0, const T *src1, size_t n,
                                          U c_in) {
    size_t m = n / 4;

    for (size_t i = 0; i < m; ++i) {
        dst[0] = addc(src0[0], src1[0], c_in, c_in);
        dst[1] = addc(src0[1], src1[1], c_in, c_in);
        dst[2] = addc(src0[2], src1[2], c_in, c_in);
        dst[3] = addc(src0[3], src1[3], c_in, c_in);

        dst += 4;
        src0 += 4;
        src1 += 4;
    }

    n &= 3;
    if (WJR_UNLIKELY(n == 0)) {
        return c_in;
    }

    dst += n;
    src0 += n;
    src1 += n;

    switch (n) {
    case 3: {
        dst[-3] = addc(src0[-3], src1[-3], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 2: {
        dst[-2] = addc(src0[-2], src1[-2], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 1: {
        dst[-1] = addc(src0[-1], src1[-1], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    default: {
        break;
    }
    }

    return c_in;
}

/**
 * @brief Add biginteger(src0) and biginteger(src1) with carry-in, and return the result
 * (dst) and carry-out.
 *
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral type.
 * @param[out] dst The result of the addition.
 * @param[in] src0 The biginteger to be added.
 * @param[in] src1 The biginteger to be added.
 * @param[in] n The number of elements in the biginteger.
 * @param[in] c_in The carry-in flag.
 * @return The carry-out flag.
 */
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U addc_n(T *dst, const T *src0, const T *src1, size_t n,
                                   U c_in) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

#if WJR_HAS_BUILTIN(ASM_ADDC_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_addc_n(dst, src0, src1, n, c_in);
        }

        return asm_addc_n(dst, src0, src1, n, c_in);
    } else {
        return fallback_addc_n(dst, src0, src1, n, c_in);
    }
#else
    return fallback_addc_n(dst, src0, src1, n, c_in);
#endif
}

/*
require :
1. m >= 1
2. n >= m
3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
*/
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U addc_s(T *dst, const T *src0, size_t n, const T *src1,
                                   size_t m, U c_in) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    c_in = addc_n(dst, src0, src1, m, c_in);

    if (n != m) {
        c_in = addc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

/*
require :
1. n >= 0
2. n >= m
3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
*/
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U addc_sz(T *dst, const T *src0, size_t n, const T *src1,
                                    size_t m, U c_in) {
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_LIKELY(m != 0)) {
        c_in = addc_n(dst, src0, src1, m, c_in);
    }

    if (n != m) {
        c_in = addc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

WJR_INTRINSIC_CONSTEXPR void __fallback_add_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1,
                                                uint64_t hi1) {
    uint64_t __al = lo0 + lo1;
    ah = hi0 + hi1 + (__al < lo0);
    al = __al;
}

#if WJR_HAS_FEATURE(FAST_INT128_ADDSUB)
#define WJR_HAS_BUILTIN___BUILTIN_ADD_128 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(__BUILTIN_ADD_128)

WJR_INTRINSIC_INLINE void __builtin_add_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                            uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    const auto x0 = static_cast<__uint128_t>(hi0) << 64 | lo0;
    const auto x1 = static_cast<__uint128_t>(hi1) << 64 | lo1;
    x0 += x1;

    al = x0;
    ah = x0 >> 64;
}

#endif

// <ah, al> = <hi0, lo0> + <hi1, lo1>
WJR_INTRINSIC_CONSTEXPR_E void __add_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1, uint64_t hi1) {
#if WJR_HAS_BUILTIN(__BUILTIN_ADD_128) || WJR_HAS_BUILTIN(__ASM_ADD_128)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(lo0 == 0) && lo0 == 0) ||
        (WJR_BUILTIN_CONSTANT_P(lo1 == 0) && lo1 == 0)) {
        return __fallback_add_128(al, ah, lo0, hi0, lo1, hi1);
    }

    return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(__BUILTIN_ADD_128), __builtin_add_128,
                          __asm_add_128)(al, ah, lo0, hi0, lo1, hi1);
#else
    return __fallback_add_128(al, ah, lo0, hi0, lo1, hi1);
#endif
}

WJR_INTRINSIC_CONSTEXPR_E uint64_t __fallback_addc_128(uint64_t &al, uint64_t &ah,
                                                       uint64_t lo0, uint64_t hi0,
                                                       uint64_t lo1, uint64_t hi1,
                                                       uint64_t c_in) {
    al = addc(lo0, lo1, c_in, c_in);
    ah = addc(hi0, hi1, c_in, c_in);
    return c_in;
}

// return c_out
WJR_INTRINSIC_CONSTEXPR_E uint64_t __addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in) {
#if WJR_HAS_BUILTIN(__ASM_ADDC_128)
    if (is_constant_evaluated()) {
        return __fallback_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return __asm_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return __fallback_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#endif
}

WJR_INTRINSIC_CONSTEXPR_E uint8_t __addc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                                uint8_t c_in) {
#if WJR_HAS_BUILTIN(__ASM_ADDC_CC_128)
    if (is_constant_evaluated()) {
        return __fallback_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return __asm_addc_cc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return __addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#endif
}

} // namespace wjr

#endif // WJR_MATH_ADD_HPP__
// Already included
#ifndef WJR_MATH_SHIFT_HPP__
#define WJR_MATH_SHIFT_HPP__

// Already included
// Already included

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_SHIFT_HPP__
#define WJR_X86_MATH_SHIFT_HPP__

// Already included

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T shld(T hi, T lo, unsigned int c);

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T shrd(T lo, T hi, unsigned int c);

template <bool is_constant>
WJR_INTRINSIC_INLINE auto __mm_get_shift(unsigned int c) {
    if constexpr (is_constant) {
        return c;
    } else {
        return simd_cast<unsigned int, __m128i_t>(c);
    }
}

WJR_INTRINSIC_INLINE __m128i __mm_sll_epi64(__m128i x, unsigned int c) {
    return sse::slli_epi64(x, c);
}

WJR_INTRINSIC_INLINE __m128i __mm_sll_epi64(__m128i x, __m128i c) {
    return sse::sll_epi64(x, c);
}

WJR_INTRINSIC_INLINE __m128i __mm_srl_epi64(__m128i x, unsigned int c) {
    return sse::srli_epi64(x, c);
}

WJR_INTRINSIC_INLINE __m128i __mm_srl_epi64(__m128i x, __m128i c) {
    return sse::srl_epi64(x, c);
}

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_LSHIFT_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_RSHIFT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(LSHIFT_N)

#define WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(index)                                      \
    do {                                                                                 \
        __m128i x1 = sse::loadu((__m128i *)(src - 3 - (index)));                         \
        x0 = simd_cast<__m128_t, __m128i_t>(sse::template shuffle_ps<78>(                \
            simd_cast<__m128i_t, __m128_t>(x1), simd_cast<__m128i_t, __m128_t>(x0)));    \
                                                                                         \
        __m128i r0 = __mm_sll_epi64(x0, y);                                              \
        __m128i r1 = __mm_srl_epi64(x1, z);                                              \
                                                                                         \
        __m128i r = sse::Or(r0, r1);                                                     \
                                                                                         \
        sse::storeu((__m128i *)(dst - 2 - (index)), r);                                  \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)

template <bool is_constant, typename T>
void large_builtin_lshift_n_impl(T *dst, const T *src, size_t n, unsigned int c) {
    auto y = __mm_get_shift<is_constant>(c);
    auto z = __mm_get_shift<is_constant>(64 - c);

    if (n & 1) {
        dst[-1] = shld(src[-1], src[-2], c);
        --src;
        --dst;
    }

    __m128i x0 = sse::set1_epi64(src[-1]);

    if (n & 2) {
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(0);

        src -= 2;
        dst -= 2;
    }

    if (n & 4) {
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(0);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(2);

        src -= 4;
        dst -= 4;
    }

    size_t idx = n / 8;

    if (WJR_UNLIKELY(idx == 0)) {
        return;
    }

    do {
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(0);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(2);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(4);
        WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED(6);

        src -= 8;
        dst -= 8;
        --idx;
    } while (WJR_LIKELY(idx != 0));
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_lshift_n_impl(T *dst, const T *src, size_t n,
                                                unsigned int c) {
    if (WJR_UNLIKELY(n < 4)) {
        switch (n) {
        case 3: {
            dst[2] = shld(src[2], src[1], c);
            WJR_FALLTHROUGH;
        }
        case 2: {
            dst[1] = shld(src[1], src[0], c);
            WJR_FALLTHROUGH;
        }
        case 1: {
            dst[0] = shld(src[0], src[-1], c);
            WJR_FALLTHROUGH;
        }
        case 0: {
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return;
    }

    src += n;
    dst += n;

    if (WJR_BUILTIN_CONSTANT_P(c)) {
        return large_builtin_lshift_n_impl<true>(dst, src, n, c);
    }

    return large_builtin_lshift_n_impl<false>(dst, src, n, c);
}

#undef WJR_REGISTER_LSHIFT_N_IMPL_UNALIGNED

template <typename T>
WJR_INTRINSIC_INLINE T builtin_lshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                        T lo) {
    T ret = src[n - 1] >> (64 - c);
    builtin_lshift_n_impl(dst + 1, src + 1, n - 1, c);
    dst[0] = shld(src[0], lo, c);
    return ret;
}

#endif

#if WJR_HAS_BUILTIN(RSHIFT_N)

#define WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(index)                                      \
    do {                                                                                 \
        __m128i x1 = sse::loadu((__m128i *)(src + 1 + (index)));                         \
        x0 = simd_cast<__m128_t, __m128i_t>(sse::template shuffle_ps<78>(                \
            simd_cast<__m128i_t, __m128_t>(x0), simd_cast<__m128i_t, __m128_t>(x1)));    \
                                                                                         \
        __m128i r0 = __mm_srl_epi64(x0, y);                                              \
        __m128i r1 = __mm_sll_epi64(x1, z);                                              \
                                                                                         \
        __m128i r = sse::Or(r0, r1);                                                     \
                                                                                         \
        sse::storeu((__m128i *)(dst + (index)), r);                                      \
                                                                                         \
        x0 = x1;                                                                         \
    } while (0)

template <bool is_constant, typename T>
void large_builtin_rshift_n_impl(T *dst, const T *src, size_t n, unsigned int c) {
    auto y = __mm_get_shift<is_constant>(c);
    auto z = __mm_get_shift<is_constant>(64 - c);

    if (n & 1) {
        dst[0] = shrd(src[0], src[1], c);
        ++src;
        ++dst;
    }

    __m128i x0 = sse::set1_epi64(src[0]);

    if (n & 2) {
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(0);

        src += 2;
        dst += 2;
    }

    if (n & 4) {
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(0);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(2);

        src += 4;
        dst += 4;
    }

    size_t idx = n / 8;

    if (WJR_UNLIKELY(idx == 0)) {
        return;
    }

    do {
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(0);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(2);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(4);
        WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED(6);

        dst += 8;
        src += 8;
        --idx;
    } while (WJR_LIKELY(idx != 0));
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_rshift_n_impl(T *dst, const T *src, size_t n,
                                                unsigned int c) {
    if (WJR_UNLIKELY(n < 4)) {
        dst += n - 3;
        src += n - 3;

        switch (n) {
        case 3: {
            dst[0] = shrd(src[0], src[1], c);
            WJR_FALLTHROUGH;
        }
        case 2: {
            dst[1] = shrd(src[1], src[2], c);
            WJR_FALLTHROUGH;
        }
        case 1: {
            dst[2] = shrd(src[2], src[3], c);
            WJR_FALLTHROUGH;
        }
        case 0: {
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return;
    }

    if (WJR_BUILTIN_CONSTANT_P(c)) {
        return large_builtin_rshift_n_impl<true>(dst, src, n, c);
    }

    return large_builtin_rshift_n_impl<false>(dst, src, n, c);
}

#undef WJR_REGISTER_RSHIFT_N_IMPL_UNALIGNED

template <typename T>
WJR_INTRINSIC_INLINE T builtin_rshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                        T hi) {
    T ret = src[0] << (64 - c);
    builtin_rshift_n_impl(dst, src, n - 1, c);
    dst[n - 1] = shrd(src[n - 1], hi, c);
    return ret;
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_SHIFT_HPP__
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shld(T hi, T lo, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return hi << c | lo >> (digits - c);
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T shld(T hi, T lo, unsigned int c) {
    return fallback_shld(hi, lo, c);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shrd(T lo, T hi, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return lo >> c | hi << (digits - c);
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T shrd(T lo, T hi, unsigned int c) {
    return fallback_shrd(lo, hi, c);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_lshift_n(T *dst, const T *src, size_t n,
                                            unsigned int c, T lo) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[n - 1] >> (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[n - i - 1] = fallback_shld(src[n - i - 1], src[n - i - 2], c);
    }
    dst[0] = fallback_shld(src[0], lo, c);
    return ret;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_DECR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T lshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                     T lo = 0) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

    if (WJR_UNLIKELY(c == 0)) {
        if (WJR_LIKELY(dst != src)) {
            std::copy_backward(src, src + n, dst + n);
        }

        return 0;
    }

#if WJR_HAS_BUILTIN(LSHIFT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_lshift_n(dst, src, n, c, lo);
        }

        return builtin_lshift_n(dst, src, n, c, lo);
    } else {
        return fallback_lshift_n(dst, src, n, c, lo);
    }
#else
    return fallback_lshift_n(dst, src, n, c, lo);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_rshift_n(T *dst, const T *src, size_t n,
                                            unsigned int c, T hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[0] << (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[i] = fallback_shrd(src[i], src[i + 1], c);
    }
    dst[n - 1] = fallback_shrd(src[n - 1], hi, c);
    return ret;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T rshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                     T hi = 0) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_UNLIKELY(c == 0)) {
        if (WJR_LIKELY(dst != src)) {
            std::copy(src, src + n, dst);
        }

        return 0;
    }

#if WJR_HAS_BUILTIN(RSHIFT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_rshift_n(dst, src, n, c, hi);
        }

        return builtin_rshift_n(dst, src, n, c, hi);
    } else {
        return fallback_rshift_n(dst, src, n, c, hi);
    }
#else
    return fallback_rshift_n(dst, src, n, c, hi);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SHIFT_HPP__
// Already included

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_MUL_HPP__
#define WJR_X86_MATH_MUL_HPP__

// Already included
#ifndef WJR_X86_MATH_MUL_IMPL_HPP__
#define WJR_X86_MATH_MUL_IMPL_HPP__

// Already included

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#define WJR_HAS_BUILTIN_MUL64 WJR_HAS_DEF

#if WJR_HAS_FEATURE(INT128)
#define WJR_HAS_BUILTIN_INT128_MUL64 WJR_HAS_DEF
#elif WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_MUL64 WJR_HAS_DEF
#else
#undef WJR_HAS_BUILTIN_MUL64
#endif

#if defined(__BMI2__)
#define WJR_HAS_BUILTIN_MULX_U64 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(MULX_U64) && WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_MUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1) && defined(__ADX__)
#define WJR_HAS_BUILTIN_ASM_ADDMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1) && WJR_HAS_BUILTIN(ASM_ADDMUL_1)
#define WJR_HAS_BUILTIN_ASM_BASECASE_MUL_S WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_BASECASE_SQR WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
#define WJR_HAS_BUILTIN_ASM_SUBMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)
#define WJR_HAS_BUILTIN_ASM_ADDLSH_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_RSBLSH_N WJR_HAS_DEF
#endif

} // namespace wjr

#endif // WJR_X86_MATH_MUL_IMPL_HPP__

namespace wjr {

#if WJR_HAS_BUILTIN(MUL64)

WJR_INTRINSIC_INLINE uint64_t builtin_mul64(uint64_t a, uint64_t b, uint64_t &hi) {
#if WJR_HAS_BUILTIN(INT128_MUL64)
    __uint128_t x = static_cast<__uint128_t>(a) * b;
    hi = x >> 64;
    return static_cast<uint64_t>(x);
#elif WJR_HAS_BUILTIN(ASM_MUL64)
    uint64_t lo;
    asm("mul{q %3| %3}\n\t" : "=a,a"(lo), "=d,d"(hi) : "%a,r"(a), "r,a"(b) : "cc");
    return lo;
#endif
}

#endif

#if WJR_HAS_BUILTIN(MULX_U64)

template <typename T>
WJR_INTRINSIC_INLINE T mulx(T a, T b, T &hi) {
    static_assert(sizeof(T) == 8, "");

#if defined(WJR_COMPILER_GCC)
    T lo;
    asm("mulx{q %3, %0, %1| %1, %0, %3}" : "=r"(lo), "=r"(hi) : "%d"(a), "r"(b));
    return lo;
#else
    unsigned long long hi_;
    unsigned long long lo = _mulx_u64(a, b, &hi_);
    hi = hi_;
    return lo;
#endif
}

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)

inline uint64_t asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t rdx) {
    size_t rcx = n / 8;
    uint64_t r8, r9, r10 = n, r11;

    const auto pdst = dst;
    const auto psrc = src;

    asm volatile(
        "and{l $7, %k[r10]| %k[r10], 7}\n\t"
        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + "
        "%[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r10] + %[r9]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"

        ".align 8\n\t"
        ".Llookup%=:\n\t"
        ".long .Ll0%=-.Llookup%=\n\t"
        ".long .Ll1%=-.Llookup%=\n\t"
        ".long .Ll2%=-.Llookup%=\n\t"
        ".long .Ll3%=-.Llookup%=\n\t"
        ".long .Ll4%=-.Llookup%=\n\t"
        ".long .Ll5%=-.Llookup%=\n\t"
        ".long .Ll6%=-.Llookup%=\n\t"
        ".long .Ll7%=-.Llookup%=\n\t"
        ".align 16\n\t"

        ".Ll0%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -48(%[src]), %[src]| %[src], [%[src] - 48]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -40(%[src]), %[src]| %[src], [%[src] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -32(%[src]), %[src]| %[src], [%[src] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -24(%[src]), %[src]| %[src], [%[src] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jrcxz .Ld1%=\n\t"

        ".align 32\n\t"
        ".Lloop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb0%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb7%=:\n\t"
        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb6%=:\n\t"
        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb5%=:\n\t"
        "mulx{q 32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 32]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb4%=:\n\t"
        "mulx{q 40(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 40]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb3%=:\n\t"
        "mulx{q 48(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 48]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb2%=:\n\t"
        "mulx{q 56(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 56]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "dec %[rcx]\n\t"
        "jne .Lloop%=\n\t"

        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        ".Ldone%=:\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        : [dst] "+&r"(dst), [src] "+&r"(src), [rcx] "+&c"(rcx), [r8] "=&r"(r8),
          [r9] "=&r"(r9), [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(rdx)
        : "cc", "memory");

    WJR_ASSERT_ASSUME(rcx == 0);
    WJR_ASSERT_ASSUME(dst == pdst + n);
    WJR_ASSERT_ASSUME(src == psrc + n);

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)

inline uint64_t asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t rdx) {
    size_t rcx = n / 8;
    uint64_t r8, r9, r10 = n, r11;

    const auto pdst = dst;
    const auto psrc = src;

    asm volatile(
        "and{l $7, %k[r10]| %k[r10], 7}\n\t"
        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + "
        "%[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r10] + %[r9]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"

        ".align 8\n\t"
        ".Llookup%=:\n\t"
        ".long .Ll0%=-.Llookup%=\n\t"
        ".long .Ll1%=-.Llookup%=\n\t"
        ".long .Ll2%=-.Llookup%=\n\t"
        ".long .Ll3%=-.Llookup%=\n\t"
        ".long .Ll4%=-.Llookup%=\n\t"
        ".long .Ll5%=-.Llookup%=\n\t"
        ".long .Ll6%=-.Llookup%=\n\t"
        ".long .Ll7%=-.Llookup%=\n\t"
        ".align 16\n\t"

        ".Ll0%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q 16(%[src]), %[src]| %[src], [%[src] + 16]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 24(%[src]), %[src]| %[src], [%[src] + 24]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q 32(%[src]), %[src]| %[src], [%[src] + 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 40(%[src]), %[src]| %[src], [%[src] + 40]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "add{q -8(%[dst]), %[r8]| [%[dst] - 8], %[r8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jrcxz .Ld1%=\n\t"

        ".align 32\n\t"
        ".Lloop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".Lb0%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        ".Lb7%=:\n\t"
        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        ".Lb6%=:\n\t"
        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        ".Lb5%=:\n\t"
        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        ".Lb4%=:\n\t"
        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        ".Lb3%=:\n\t"
        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".Lb2%=:\n\t"
        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Lloop_out%=\n\t"
        "jmp .Lloop%=\n\t"
        ".Lloop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"

        ".Ldone%=:"

        : [dst] "+&r"(dst), [src] "+&r"(src), [rcx] "+&c"(rcx), [r8] "=&r"(r8),
          [r9] "=&r"(r9), [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(rdx)
        : "cc", "memory");

    WJR_ASSERT_ASSUME(rcx == 0);
    WJR_ASSERT_ASSUME(dst == pdst + n);
    WJR_ASSERT_ASSUME(src == psrc + n);

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)

extern void __asm_basecase_mul_s_impl(uint64_t *dst, const uint64_t *src0, size_t rdx,
                                      const uint64_t *src1, size_t m);

inline void asm_basecase_mul_s(uint64_t *dst, const uint64_t *src0, size_t n,
                               const uint64_t *src1, size_t m) {
    WJR_ASSERT(n >= m);
    WJR_ASSERT(m >= 1);

    return __asm_basecase_mul_s_impl(dst, src0, n, src1, m);
}

#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)

extern void __asm_basecase_sqr_impl(uint64_t *dst, const uint64_t *src, size_t rdx);

inline void asm_basecase_sqr(uint64_t *dst, const uint64_t *src, size_t n) {
    WJR_ASSERT(n >= 1);

    return __asm_basecase_sqr_impl(dst, src, n);
}

#endif

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)

// slower than asm_addmul_1
inline uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t rdx) {
    WJR_ASSERT(n != 0);

    size_t rcx = n / 8;
    uint64_t r8, r9, r10 = static_cast<uint32_t>(n), r11;

    asm volatile(
        // set CF = 1, OF = 0
        "and{l $7, %k[r10]| %k[r10], 7}\n\t"
        "stc\n\t"

        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r10] * "
        "4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r10] + %[r9]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"

        ".align 8\n\t"
        ".Llookup%=:\n\t"
        ".long .Ll0%=-.Llookup%=\n\t"
        ".long .Ll1%=-.Llookup%=\n\t"
        ".long .Ll2%=-.Llookup%=\n\t"
        ".long .Ll3%=-.Llookup%=\n\t"
        ".long .Ll4%=-.Llookup%=\n\t"
        ".long .Ll5%=-.Llookup%=\n\t"
        ".long .Ll6%=-.Llookup%=\n\t"
        ".long .Ll7%=-.Llookup%=\n\t"
        ".align 16\n\t"

        ".Ll0%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q 16(%[src]), %[src]| %[src], [%[src] + 16]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 24(%[src]), %[src]| %[src], [%[src] + 24]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q 32(%[src]), %[src]| %[src], [%[src] + 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 40(%[src]), %[src]| %[src], [%[src] + 40]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "adc{q (%[dst]), %[r8]| [%[dst]], %[r8]}\n\t"
        "sbb{q $-1, %[r9]| %[r9], -1}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "not %[r8]\n\t"
        "jrcxz .Ld1%=\n\t"
        "lea{q 8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"

        ".align 32\n\t"
        ".Lloop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".Lb0%=:\n\t"
        "not %[r10]\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        ".Lb7%=:\n\t"
        "not %[r8]\n\t"
        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        ".Lb6%=:\n\t"
        "not %[r10]\n\t"
        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        ".Lb5%=:\n\t"
        "not %[r8]\n\t"
        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        ".Lb4%=:\n\t"
        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "not %[r10]\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        ".Lb3%=:\n\t"
        "not %[r8]\n\t"
        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".Lb2%=:\n\t"
        "not %[r10]\n\t"
        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "not %[r8]\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Lloop_out%=\n\t"
        "jmp .Lloop%=\n\t"
        ".Lloop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "sbb{q $-1, %[r9]| %[r9], -1}\n\t"

        ".Ldone%=:"

        : [dst] "+&r"(dst), [src] "+&r"(src), [rcx] "+&c"(rcx), [r8] "=&r"(r8),
          [r9] "=&r"(r9), [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(rdx)
        : "cc", "memory");

    WJR_ASSUME(rcx == 0);

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDLSH_N)
#define WJR_ADDSUB_I 1
// WJR_ADDSUB_I :
// 0 : SUB
// 1 : ADD

#ifndef WJR_ADDSUB_I
#error "abort"
#endif

#define WJR_addsub WJR_PP_BOOL_IF(WJR_ADDSUB_I, add, rsb)
#define WJR_adcsbb WJR_PP_BOOL_IF(WJR_ADDSUB_I, adc, sbb)

inline uint64_t WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addsub, lsh_n))(
    uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n, uint64_t cl) {
    WJR_ASSERT(cl != 0);

    size_t rcx = n / 8;
    uint64_t tcl = 64 - cl;
    uint64_t r8, r9 = n, r10, r11;

    asm volatile(
        "and{l $7, %k[r9]| %k[r9], 7}\n\t"
        "lea{q| %[r8], [rip +} .Llookup%={(%%rip), %[r8]|]}\n\t"
        "movs{lq (%[r8], %[r9], 4), %[r9]|xd %[r9], DWORD PTR [%[r8] + "
        "%[r9] * 4]}\n\t"
        "lea{q (%[r8], %[r9], 1), %[r9]| %[r9], [%[r9] + %[r8]]}\n\t"
        "jmp{q *%[r9]| %[r9]}\n\t"

        ".align 8\n\t"
        ".Llookup%=:\n\t"
        ".long .Ll0%=-.Llookup%=\n\t"
        ".long .Ll1%=-.Llookup%=\n\t"
        ".long .Ll2%=-.Llookup%=\n\t"
        ".long .Ll3%=-.Llookup%=\n\t"
        ".long .Ll4%=-.Llookup%=\n\t"
        ".long .Ll5%=-.Llookup%=\n\t"
        ".long .Ll6%=-.Llookup%=\n\t"
        ".long .Ll7%=-.Llookup%=\n\t"
        ".align 16\n\t"

        ".Ll0%=:\n\t"
        "mov{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r8], %[r11]| %[r11], %[r8], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r8], %[r8]| %[r8], %[r8], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r9]| %[r9], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r9], %[r10]| %[r10], %[r9], %[cl]}\n\t"
        "mov{q %[r11], (%[dst])| [%[dst]], %[r11]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ld1%=:\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q %[rcx], %[r11]| %[r11], %[rcx]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mov{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r11], %[r10]| %[r10], %[r11], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "jrcxz .Ld1%=\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r8]| %[r8], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r8], %[r9]| %[r9], %[r8], %[cl]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "lea{q 8(%[src0]), %[src0]| %[src0], [%[src0] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jmp .Lb1%=\n\t"

        ".Ll3%=:\n\t"
        "mov{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9], %[r9], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r10]| %[r10], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r10], %[r11]| %[r11], %[r10], %[cl]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "lea{q -40(%[src0]), %[src0]| %[src0], [%[src0] - 40]}\n\t"
        "lea{q -40(%[src1]), %[src1]| %[src1], [%[src1] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mov{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r8], %[r11]| %[r11], %[r8], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r8], %[r8]| %[r8], %[r8], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r9]| %[r9], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r9], %[r10]| %[r10], %[r9], %[cl]}\n\t"
        "mov{q %[r11], (%[dst])| [%[dst]], %[r11]}\n\t"
        "lea{q -32(%[src0]), %[src0]| %[src0], [%[src0] - 32]}\n\t"
        "lea{q -32(%[src1]), %[src1]| %[src1], [%[src1] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mov{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r11], %[r10]| %[r10], %[r11], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r8]| %[r8], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r8], %[r9]| %[r9], %[r8], %[cl]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "lea{q -24(%[src0]), %[src0]| %[src0], [%[src0] - 24]}\n\t"
        "lea{q -24(%[src1]), %[src1]| %[src1], [%[src1] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mov{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r10], %[r9]| %[r9], %[r10], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10], %[r10], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r11]| %[r11], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r11], %[r8]| %[r8], %[r11], %[cl]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"
        "lea{q -16(%[src0]), %[src0]| %[src0], [%[src0] - 16]}\n\t"
        "lea{q -16(%[src1]), %[src1]| %[src1], [%[src1] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mov{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9], %[r9], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r10]| %[r10], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r10], %[r11]| %[r11], %[r10], %[cl]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "lea{q -8(%[src0]), %[src0]| %[src0], [%[src0] - 8]}\n\t"
        "lea{q -8(%[src1]), %[src1]| %[src1], [%[src1] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld2%=:\n\t"
        "lea{q (%[r8], %[r10]), %[r10]| %[r10], [%[r10] + %[r8]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src0]), %[r10]| %[r10], [%[src0] + 8]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q %[rcx], %[r11]| %[r11], %[rcx]}\n\t"
        "mov{q %[r10], 8(%[dst])| [%[dst] + 8], %[r10]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll2%=:\n\t"
        "mov{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r10], %[r9]| %[r9], %[r10], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10], %[r10], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r11]| %[r11], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r11], %[r8]| %[r8], %[r11], %[cl]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"
        "jrcxz .Ld2%=\n\t"
        "lea{q 16(%[src0]), %[src0]| %[src0], [%[src0] + 16]}\n\t"
        "lea{q 16(%[src1]), %[src1]| %[src1], [%[src1] + 16]}\n\t"
        "lea{q 16(%[dst]), %[dst]| %[dst], [%[dst] + 16]}\n\t"

        ".align 32\n\t"
        ".Lloop%=:\n\t"

        ".Lb2%=:\n\t"
        "lea{q (%[r8], %[r10]), %[r10]| %[r10], [%[r10] + %[r8]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src0]), %[r10]| %[r10], [%[src0] - 8]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        "mov{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r8], %[r9]| %[r9], %[r8], %[cl]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Lb1%=:\n\t"
        "lea{q (%[r9], %[r11]), %[r11]| %[r11], [%[r11] + %[r9]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r8], %[r8]| %[r8], %[r8], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r9]| %[r9], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r9], %[r10]| %[r10], %[r9], %[cl]}\n\t"
        "mov{q %[r11], (%[dst])| [%[dst]], %[r11]}\n\t"

        ".Lb0%=:\n\t"
        "lea{q (%[r10], %[r8]), %[r8]| %[r8], [%[r8] + %[r10]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src0]), %[r8]| %[r8], [%[src0] + 8]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9], %[r9], %[tcl]}\n\t"
        "mov{q 16(%[src1]), %[r10]| %[r10], [%[src1] + 16]}\n\t"
        "shlx{q %[cl], %[r10], %[r11]| %[r11], %[r10], %[cl]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"

        ".Lb7%=:\n\t"
        "lea{q (%[r11], %[r9]), %[r9]| %[r9], [%[r9] + %[r11]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 16(%[src0]), %[r9]| %[r9], [%[src0] + 16]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10], %[r10], %[tcl]}\n\t"
        "mov{q 24(%[src1]), %[r11]| %[r11], [%[src1] + 24]}\n\t"
        "shlx{q %[cl], %[r11], %[r8]| %[r8], %[r11], %[cl]}\n\t"
        "mov{q %[r9], 16(%[dst])| [%[dst] + 16], %[r9]}\n\t"

        ".Lb6%=:\n\t"
        "lea{q (%[r8], %[r10]), %[r10]| %[r10], [%[r10] + %[r8]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 24(%[src0]), %[r10]| %[r10], [%[src0] + 24]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        "mov{q 32(%[src1]), %[r8]| %[r8], [%[src1] + 32]}\n\t"
        "shlx{q %[cl], %[r8], %[r9]| %[r9], %[r8], %[cl]}\n\t"
        "mov{q %[r10], 24(%[dst])| [%[dst] + 24], %[r10]}\n\t"

        ".Lb5%=:\n\t"
        "lea{q (%[r9], %[r11]), %[r11]| %[r11], [%[r11] + %[r9]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 32(%[src0]), %[r11]| %[r11], [%[src0] + 32]}\n\t"
        "shrx{q %[tcl], %[r8], %[r8]| %[r8], %[r8], %[tcl]}\n\t"
        "mov{q 40(%[src1]), %[r9]| %[r9], [%[src1] + 40]}\n\t"
        "shlx{q %[cl], %[r9], %[r10]| %[r10], %[r9], %[cl]}\n\t"
        "mov{q %[r11], 32(%[dst])| [%[dst] + 32], %[r11]}\n\t"

        ".Lb4%=:\n\t"
        "lea{q (%[r10], %[r8]), %[r8]| %[r8], [%[r8] + %[r10]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 40(%[src0]), %[r8]| %[r8], [%[src0] + 40]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9], %[r9], %[tcl]}\n\t"
        "mov{q 48(%[src1]), %[r10]| %[r10], [%[src1] + 48]}\n\t"
        "shlx{q %[cl], %[r10], %[r11]| %[r11], %[r10], %[cl]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".Lb3%=:\n\t"
        "lea{q (%[r11], %[r9]), %[r9]| %[r9], [%[r9] + %[r11]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 48(%[src0]), %[r9]| %[r9], [%[src0] + 48]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10], %[r10], %[tcl]}\n\t"
        "mov{q 56(%[src1]), %[r11]| %[r11], [%[src1] + 56]}\n\t"
        "shlx{q %[cl], %[r11], %[r8]| %[r8], %[r11], %[cl]}\n\t"
        "mov{q %[r9], 48(%[dst])| [%[dst] + 48], %[r9]}\n\t"

        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "lea{q 64(%[src1]), %[src1]| %[src1], [%[src1] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Lloop_out%=\n\t"
        "jmp .Lloop%=\n\t"
        ".Lloop_out%=:\n\t"

        "lea{q (%[r8], %[r10]), %[r10]| %[r10], [%[r10] + %[r8]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src0]), %[r10]| %[r10], [%[src0] - 8]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q %[rcx], %[r11]| %[r11], %[rcx]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Ldone%=:"

        : [dst] "+&r"(dst), [src0] "+&r"(src0), [src1] "+&r"(src1), [rcx] "+&c"(rcx),
          [r8] "=&r"(r8), [r9] "+&r"(r9), [r10] "=&r"(r10), [r11] "=&r"(r11)
        : [cl] "r"(cl), [tcl] "r"(tcl)
        : "cc", "memory");

    WJR_ASSERT_ASSUME(rcx == 0);

    return r11;
}

#undef WJR_adcsbb
#undef WJR_addcsubc

#undef WJR_ADDSUB_I
#endif

#if WJR_HAS_BUILTIN(ASM_RSBLSH_N)
#define WJR_ADDSUB_I 0
// WJR_ADDSUB_I :
// 0 : SUB
// 1 : ADD

#ifndef WJR_ADDSUB_I
#error "abort"
#endif

#define WJR_addsub WJR_PP_BOOL_IF(WJR_ADDSUB_I, add, rsb)
#define WJR_adcsbb WJR_PP_BOOL_IF(WJR_ADDSUB_I, adc, sbb)

inline uint64_t WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addsub, lsh_n))(
    uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n, uint64_t cl) {
    WJR_ASSERT(cl != 0);

    size_t rcx = n / 8;
    uint64_t tcl = 64 - cl;
    uint64_t r8, r9 = n, r10, r11;

    asm volatile(
        "and{l $7, %k[r9]| %k[r9], 7}\n\t"
        "lea{q| %[r8], [rip +} .Llookup%={(%%rip), %[r8]|]}\n\t"
        "movs{lq (%[r8], %[r9], 4), %[r9]|xd %[r9], DWORD PTR [%[r8] + "
        "%[r9] * 4]}\n\t"
        "lea{q (%[r8], %[r9], 1), %[r9]| %[r9], [%[r9] + %[r8]]}\n\t"
        "jmp{q *%[r9]| %[r9]}\n\t"

        ".align 8\n\t"
        ".Llookup%=:\n\t"
        ".long .Ll0%=-.Llookup%=\n\t"
        ".long .Ll1%=-.Llookup%=\n\t"
        ".long .Ll2%=-.Llookup%=\n\t"
        ".long .Ll3%=-.Llookup%=\n\t"
        ".long .Ll4%=-.Llookup%=\n\t"
        ".long .Ll5%=-.Llookup%=\n\t"
        ".long .Ll6%=-.Llookup%=\n\t"
        ".long .Ll7%=-.Llookup%=\n\t"
        ".align 16\n\t"

        ".Ll0%=:\n\t"
        "mov{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r8], %[r11]| %[r11], %[r8], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r8], %[r8]| %[r8], %[r8], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r9]| %[r9], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r9], %[r10]| %[r10], %[r9], %[cl]}\n\t"
        "mov{q %[r11], (%[dst])| [%[dst]], %[r11]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ld1%=:\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q %[rcx], %[r11]| %[r11], %[rcx]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mov{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r11], %[r10]| %[r10], %[r11], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "jrcxz .Ld1%=\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r8]| %[r8], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r8], %[r9]| %[r9], %[r8], %[cl]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "lea{q 8(%[src0]), %[src0]| %[src0], [%[src0] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jmp .Lb1%=\n\t"

        ".Ll3%=:\n\t"
        "mov{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9], %[r9], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r10]| %[r10], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r10], %[r11]| %[r11], %[r10], %[cl]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "lea{q -40(%[src0]), %[src0]| %[src0], [%[src0] - 40]}\n\t"
        "lea{q -40(%[src1]), %[src1]| %[src1], [%[src1] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mov{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r8], %[r11]| %[r11], %[r8], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r8], %[r8]| %[r8], %[r8], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r9]| %[r9], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r9], %[r10]| %[r10], %[r9], %[cl]}\n\t"
        "mov{q %[r11], (%[dst])| [%[dst]], %[r11]}\n\t"
        "lea{q -32(%[src0]), %[src0]| %[src0], [%[src0] - 32]}\n\t"
        "lea{q -32(%[src1]), %[src1]| %[src1], [%[src1] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mov{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r11], %[r10]| %[r10], %[r11], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r8]| %[r8], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r8], %[r9]| %[r9], %[r8], %[cl]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "lea{q -24(%[src0]), %[src0]| %[src0], [%[src0] - 24]}\n\t"
        "lea{q -24(%[src1]), %[src1]| %[src1], [%[src1] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mov{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r10], %[r9]| %[r9], %[r10], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10], %[r10], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r11]| %[r11], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r11], %[r8]| %[r8], %[r11], %[cl]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"
        "lea{q -16(%[src0]), %[src0]| %[src0], [%[src0] - 16]}\n\t"
        "lea{q -16(%[src1]), %[src1]| %[src1], [%[src1] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mov{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9], %[r9], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r10]| %[r10], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r10], %[r11]| %[r11], %[r10], %[cl]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "lea{q -8(%[src0]), %[src0]| %[src0], [%[src0] - 8]}\n\t"
        "lea{q -8(%[src1]), %[src1]| %[src1], [%[src1] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld2%=:\n\t"
        "lea{q (%[r8], %[r10]), %[r10]| %[r10], [%[r10] + %[r8]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src0]), %[r10]| %[r10], [%[src0] + 8]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q %[rcx], %[r11]| %[r11], %[rcx]}\n\t"
        "mov{q %[r10], 8(%[dst])| [%[dst] + 8], %[r10]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll2%=:\n\t"
        "mov{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r10], %[r9]| %[r9], %[r10], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10], %[r10], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r11]| %[r11], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r11], %[r8]| %[r8], %[r11], %[cl]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"
        "jrcxz .Ld2%=\n\t"
        "lea{q 16(%[src0]), %[src0]| %[src0], [%[src0] + 16]}\n\t"
        "lea{q 16(%[src1]), %[src1]| %[src1], [%[src1] + 16]}\n\t"
        "lea{q 16(%[dst]), %[dst]| %[dst], [%[dst] + 16]}\n\t"

        ".align 32\n\t"
        ".Lloop%=:\n\t"

        ".Lb2%=:\n\t"
        "lea{q (%[r8], %[r10]), %[r10]| %[r10], [%[r10] + %[r8]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src0]), %[r10]| %[r10], [%[src0] - 8]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        "mov{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "shlx{q %[cl], %[r8], %[r9]| %[r9], %[r8], %[cl]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Lb1%=:\n\t"
        "lea{q (%[r9], %[r11]), %[r11]| %[r11], [%[r11] + %[r9]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "shrx{q %[tcl], %[r8], %[r8]| %[r8], %[r8], %[tcl]}\n\t"
        "mov{q 8(%[src1]), %[r9]| %[r9], [%[src1] + 8]}\n\t"
        "shlx{q %[cl], %[r9], %[r10]| %[r10], %[r9], %[cl]}\n\t"
        "mov{q %[r11], (%[dst])| [%[dst]], %[r11]}\n\t"

        ".Lb0%=:\n\t"
        "lea{q (%[r10], %[r8]), %[r8]| %[r8], [%[r8] + %[r10]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src0]), %[r8]| %[r8], [%[src0] + 8]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9], %[r9], %[tcl]}\n\t"
        "mov{q 16(%[src1]), %[r10]| %[r10], [%[src1] + 16]}\n\t"
        "shlx{q %[cl], %[r10], %[r11]| %[r11], %[r10], %[cl]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"

        ".Lb7%=:\n\t"
        "lea{q (%[r11], %[r9]), %[r9]| %[r9], [%[r9] + %[r11]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 16(%[src0]), %[r9]| %[r9], [%[src0] + 16]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10], %[r10], %[tcl]}\n\t"
        "mov{q 24(%[src1]), %[r11]| %[r11], [%[src1] + 24]}\n\t"
        "shlx{q %[cl], %[r11], %[r8]| %[r8], %[r11], %[cl]}\n\t"
        "mov{q %[r9], 16(%[dst])| [%[dst] + 16], %[r9]}\n\t"

        ".Lb6%=:\n\t"
        "lea{q (%[r8], %[r10]), %[r10]| %[r10], [%[r10] + %[r8]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 24(%[src0]), %[r10]| %[r10], [%[src0] + 24]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        "mov{q 32(%[src1]), %[r8]| %[r8], [%[src1] + 32]}\n\t"
        "shlx{q %[cl], %[r8], %[r9]| %[r9], %[r8], %[cl]}\n\t"
        "mov{q %[r10], 24(%[dst])| [%[dst] + 24], %[r10]}\n\t"

        ".Lb5%=:\n\t"
        "lea{q (%[r9], %[r11]), %[r11]| %[r11], [%[r11] + %[r9]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 32(%[src0]), %[r11]| %[r11], [%[src0] + 32]}\n\t"
        "shrx{q %[tcl], %[r8], %[r8]| %[r8], %[r8], %[tcl]}\n\t"
        "mov{q 40(%[src1]), %[r9]| %[r9], [%[src1] + 40]}\n\t"
        "shlx{q %[cl], %[r9], %[r10]| %[r10], %[r9], %[cl]}\n\t"
        "mov{q %[r11], 32(%[dst])| [%[dst] + 32], %[r11]}\n\t"

        ".Lb4%=:\n\t"
        "lea{q (%[r10], %[r8]), %[r8]| %[r8], [%[r8] + %[r10]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 40(%[src0]), %[r8]| %[r8], [%[src0] + 40]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9], %[r9], %[tcl]}\n\t"
        "mov{q 48(%[src1]), %[r10]| %[r10], [%[src1] + 48]}\n\t"
        "shlx{q %[cl], %[r10], %[r11]| %[r11], %[r10], %[cl]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".Lb3%=:\n\t"
        "lea{q (%[r11], %[r9]), %[r9]| %[r9], [%[r9] + %[r11]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 48(%[src0]), %[r9]| %[r9], [%[src0] + 48]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10], %[r10], %[tcl]}\n\t"
        "mov{q 56(%[src1]), %[r11]| %[r11], [%[src1] + 56]}\n\t"
        "shlx{q %[cl], %[r11], %[r8]| %[r8], %[r11], %[cl]}\n\t"
        "mov{q %[r9], 48(%[dst])| [%[dst] + 48], %[r9]}\n\t"

        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "lea{q 64(%[src1]), %[src1]| %[src1], [%[src1] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Lloop_out%=\n\t"
        "jmp .Lloop%=\n\t"
        ".Lloop_out%=:\n\t"

        "lea{q (%[r8], %[r10]), %[r10]| %[r10], [%[r10] + %[r8]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src0]), %[r10]| %[r10], [%[src0] - 8]}\n\t"
        "shrx{q %[tcl], %[r11], %[r11]| %[r11], %[r11], %[tcl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q %[rcx], %[r11]| %[r11], %[rcx]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Ldone%=:"

        : [dst] "+&r"(dst), [src0] "+&r"(src0), [src1] "+&r"(src1), [rcx] "+&c"(rcx),
          [r8] "=&r"(r8), [r9] "+&r"(r9), [r10] "=&r"(r10), [r11] "=&r"(r11)
        : [cl] "r"(cl), [tcl] "r"(tcl)
        : "cc", "memory");

    WJR_ASSERT_ASSUME(rcx == 0);

    return r11;
}

#undef WJR_adcsbb
#undef WJR_addcsubc

#undef WJR_ADDSUB_I
#endif

} // namespace wjr

#endif // WJR_X86_MATH_MUL_HPP__
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul(T a, T b, T &hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    using T2 = uint_t<nd * 2>;
    T2 x = static_cast<T2>(a) * b;
    hi = x >> nd;
    return static_cast<T>(x);
}

WJR_INTRINSIC_CONSTEXPR_E uint64_t fallback_mul64(uint64_t a, uint64_t b, uint64_t &hi) {
    uint64_t ah = a >> 32;
    uint64_t al = a & 0xFFFFFFFF;
    uint64_t bh = b >> 32;
    uint64_t bl = b & 0xFFFFFFFF;

    uint64_t rh = ah * bh;
    uint64_t rm0 = ah * bl;
    uint64_t rm1 = al * bh;
    uint64_t rl = al * bl;

    __add_128(rl, rh, rl, rh, rm0 << 32, rm0 >> 32);
    __add_128(rl, rh, rl, rh, rm1 << 32, rm1 >> 32);

    hi = rh;
    return rl;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E T mul(T a, T b, T &hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if ((WJR_BUILTIN_CONSTANT_P(a == 0) && a == 0) ||
        (WJR_BUILTIN_CONSTANT_P(b == 0) && b == 0)) {
        hi = 0;
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(a == 1) && a == 1) {
        hi = 0;
        return b;
    }

    if (WJR_BUILTIN_CONSTANT_P(b == 1) && b == 1) {
        hi = 0;
        return a;
    }

    if constexpr (nd < 64) {
        return fallback_mul(a, b, hi);
    } else {

#if WJR_HAS_BUILTIN(MUL64)
        if (is_constant_evaluated()
#if WJR_HAS_BUILTIN(ASM_MUL64)
            || (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))
#endif
        ) {
            return fallback_mul64(a, b, hi);
        }

#if WJR_HAS_BUILTIN(ASM_MUL64)
        // mov b to rax, then mul a
        // instead of mov a to rax, mov b to register, then mul
        if (WJR_BUILTIN_CONSTANT_P(b)) {
            return builtin_mul64(b, a, hi);
        }
#endif

        return builtin_mul64(a, b, hi);
#else
        return fallback_mul64(a, b, hi);
#endif
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T fallback_mulhi(T a, T b) {
    T hi = 0;
    (void)fallback_mul(a, b, hi);
    return hi;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T mulhi(T a, T b) {
    T ret = 0;
    (void)mul(a, b, ret);
    return ret;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T mullo(T a, T b) {
    return a * b;
}

#if WJR_HAS_BUILTIN(__builtin_mul_overflow)
#define WJR_HAS_BUILTIN_MUL_OVERFLOW WJR_HAS_DEF
#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E bool fallback_mul_overflow(T a, T b, T &ret) {
    T hi;
    ret = mul(a, b, hi);
    return hi != 0;
}

#if WJR_HAS_BUILTIN(MUL_OVERFLOW)
template <typename T>
WJR_INTRINSIC_INLINE bool builtin_mul_overflow(T a, T b, T &ret) {
    return __builtin_mul_overflow(a, b, &ret);
}
#endif

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E bool mul_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret) {
#if WJR_HAS_BUILTIN(MUL_OVERFLOW)
    if (is_constant_evaluated() ||
        (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
        return fallback_mul_overflow(a, b, ret);
    }

    return builtin_mul_overflow(a, b, ret);
#else
    return fallback_mul_overflow(a, b, ret);
#endif
}

// TODO : optimize
template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul_1(T *dst, const T *src, size_t n, T ml) {
    T lo = 0, hi = 0;
    T c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        dst[i] = addc<T>(lo, c_in, 0, c_in);
        c_in += hi;
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T mul_1(T *dst, const T *src, size_t n, type_identity_t<T> ml) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 0) && ml == 0) {
        set_n(dst, 0, n);
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        if (src != dst) {
            std::copy(src, src + n, dst);
        }

        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(is_zero_or_single_bit(ml)) && is_zero_or_single_bit(ml)) {
        unsigned int k = ctz(ml);
        return lshift_n(dst, src, n, k);
    }

#if WJR_HAS_BUILTIN(ASM_MUL_1)
    if (is_constant_evaluated()) {
        return fallback_mul_1(dst, src, n, ml);
    }

    return asm_mul_1(dst, src, n, ml);
#else
    return fallback_mul_1(dst, src, n, ml);
#endif
}

// dst = src0 + (src1 << cl)
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addlsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl);

// dst = (src1 << cl) - src0
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T rsblsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl);

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_addmul_1(T *dst, const T *src, size_t n, T ml) {
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        lo = addc<T>(lo, c_in, 0, c_in);
        dst[i] = addc<T>(lo, dst[i], 0, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addmul_1(T *dst, const T *src, size_t n,
                                     type_identity_t<T> ml) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 0) && ml == 0) {
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        return addc_n(dst, dst, src, n);
    }

    if (WJR_BUILTIN_CONSTANT_P(is_zero_or_single_bit(ml)) && is_zero_or_single_bit(ml)) {
        unsigned int c = ctz(ml);
        return addlsh_n(dst, dst, src, n, c);
    }

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_addmul_1(dst, src, n, ml);
        }

        return asm_addmul_1(dst, src, n, ml);
    } else {
        return fallback_addmul_1(dst, src, n, ml);
    }
#else
    return fallback_addmul_1(dst, src, n, ml);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_submul_1(T *dst, const T *src, size_t n, T ml) {
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        lo = addc<T>(lo, c_in, 0, c_in);
        dst[i] = subc<T>(dst[i], lo, 0, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T submul_1(T *dst, const T *src, size_t n,
                                     type_identity_t<T> ml) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 0) && ml == 0) {
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        return subc_n(dst, dst, src, n);
    }

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)
    if (is_constant_evaluated()) {
        return fallback_submul_1(dst, src, n, ml);
    }

    return asm_submul_1(dst, src, n, ml);
#else
    return fallback_submul_1(dst, src, n, ml);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_addlsh_n(T *dst, const T *src0, const T *src1,
                                            size_t n, type_identity_t<T> cl) {
    T tcl = std::numeric_limits<T>::digits - cl;
    T lo = 0, hi = 0;
    T c_in = 0, x = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = src1[i] << cl;
        hi = src1[i] >> tcl;

        lo += x;
        dst[i] = addc<T>(lo, src0[i], c_in, c_in);
        x = hi;
    }

    return x + c_in;
}

/*
dst = src0 + (src1 << cl);

require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
3. WJR_IS_SAME_OR_INCR_P(sdt, n, src1, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addlsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<T>::digits);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    if (WJR_UNLIKELY(cl == 0)) {
        return addc_n(dst, src0, src1, n);
    }

#if WJR_HAS_BUILTIN(ASM_ADDLSH_N)
    if (is_constant_evaluated()) {
        return fallback_addlsh_n(dst, src0, src1, n, cl);
    }

    return asm_addlsh_n(dst, src0, src1, n, cl);
#else
    return fallback_addlsh_n(dst, src0, src1, n, cl);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_rsblsh_n(T *dst, const T *src0, const T *src1,
                                            size_t n, type_identity_t<T> cl) {
    T tcl = std::numeric_limits<T>::digits - cl;
    T lo = 0, hi = 0;
    T c_in = 0, x = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = src1[i] << cl;
        hi = src1[i] >> tcl;

        lo += x;
        dst[i] = subc<T>(lo, src0[i], c_in, c_in);
        x = hi;
    }

    return x - c_in;
}

/*
dst = (src1 << cl) - src0;

require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
3. WJR_IS_SAME_OR_INCR_P(sdt, n, src1, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T rsblsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<T>::digits);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    if (WJR_UNLIKELY(cl == 0)) {
        return T{0} - subc_n(dst, src1, src0, n);
    }

#if WJR_HAS_BUILTIN(ASM_RSBLSH_N)
    if (is_constant_evaluated()) {
        return fallback_rsblsh_n(dst, src0, src1, n, cl);
    }

    return asm_rsblsh_n(dst, src0, src1, n, cl);
#else
    return fallback_rsblsh_n(dst, src0, src1, n, cl);
#endif
}

template <typename T, T maxn = in_place_max>
WJR_INTRINSIC_CONSTEXPR_E T try_addmul_1(T *dst, const T *src, size_t n,
                                         type_identity_t<T> ml,
                                         std::integral_constant<T, maxn> = {}) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    WJR_ASSERT_ASSUME(ml <= maxn);

    if constexpr (maxn == 0) {
        return 0;
    }

    if (ml == 0) {
        return 0;
    }

    if constexpr (maxn == 1) {
        return addc_n(dst, dst, src, n);
    } else {
        if (ml == 1) {
            return addc_n(dst, dst, src, n);
        }

        if constexpr (maxn <= 3) {
            // addlsh is slightly faster than addmul
            if constexpr (maxn == 2) {
                return addlsh_n(dst, dst, src, n, 1);
            } else {
                if (ml == 2) {
                    return addlsh_n(dst, dst, src, n, 1);
                }

                return addmul_1(dst, src, n, ml);
            }
        } else {
            return addmul_1(dst, src, n, ml);
        }
    }
}

inline constexpr size_t toom22_mul_threshold = WJR_TOOM22_MUL_THRESHOLD;
inline constexpr size_t toom33_mul_threshold = WJR_TOOM33_MUL_THRESHOLD;
inline constexpr size_t toom44_mul_threshold = WJR_TOOM44_MUL_THRESHOLD;
inline constexpr size_t toom32_to_toom43_mul_threshold =
    WJR_TOOM32_TO_TOOM43_MUL_THRESHOLD;
inline constexpr size_t toom32_to_toom53_mul_threshold =
    WJR_TOOM32_TO_TOOM53_MUL_THRESHOLD;
inline constexpr size_t toom42_to_toom53_mul_threshold =
    WJR_TOOM42_TO_TOOM53_MUL_THRESHOLD;
inline constexpr size_t toom2_sqr_threshold = WJR_TOOM2_SQR_THRESHOLD;
inline constexpr size_t toom3_sqr_threshold = WJR_TOOM3_SQR_THRESHOLD;
inline constexpr size_t toom4_sqr_threshold = WJR_TOOM4_SQR_THRESHOLD;

// only toom22 is optimized to inline
enum class __mul_mode : uint8_t {
    toom22 = 0x00,
    toom33 = 0x01,
    toom44 = 0x02,
    all = 0x03,
};

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__mul_s_impl(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
             std::conditional_t<reserved, T *, in_place_empty_t> mal);

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                  const T *src1, size_t m, T *stk);

template <typename T>
WJR_INTRINSIC_INLINE void mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                const T *src1, size_t m);

template <typename T>
WJR_INTRINSIC_INLINE void mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                const T *src1, size_t m, T *stk);

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__mul_n_impl(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n,
             std::conditional_t<reserved, T *, in_place_empty_t> mal);

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                  size_t n, T *stk);

template <__mul_mode mode, typename T, T m0 = in_place_max, T m1 = in_place_max>
WJR_INTRINSIC_INLINE void
__mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n, T *stk, T &c_out,
        type_identity_t<T> cf0, type_identity_t<T> cf1,
        std::integral_constant<T, m0> = {}, std::integral_constant<T, m1> = {});

template <typename T>
WJR_INTRINSIC_INLINE void mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                size_t n);

template <typename T>
WJR_INTRINSIC_INLINE void mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                size_t n, T *stk);

template <typename T>
void basecase_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                    size_t m);

template <typename T>
void basecase_sqr(T *WJR_RESTRICT dst, const T *src, size_t n);

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__sqr_impl(T *WJR_RESTRICT dst, const T *src, size_t n,
           std::conditional_t<reserved, T *, in_place_empty_t> mal);

template <typename T>
WJR_INTRINSIC_INLINE void __sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

template <__mul_mode mode, typename T, T m = in_place_max>
WJR_INTRINSIC_INLINE void __sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk,
                                T &c_out, type_identity_t<T> cf,
                                std::integral_constant<T, m> x = {});

template <typename T>
WJR_INTRINSIC_INLINE void sqr(T *WJR_RESTRICT dst, const T *src, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE void sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

/*
 all toom-cook need to ensure rn + rm >= l to reserve memory
 for toom-cook-u-v
 1. (v-1) * n + g <= u * m
    g = (u-1)^2 + (u-1)*v
    e.g.
    TOOM-COOK32 :
    g = 8
    n + 8 <= 3 * m
 2. v * n >= (u - 1) * m + g
    g = (v-1)^2 + (v-1)*u
    e.g.
    TOOM-COOK32 :
    g = 4
    2 * n >= 2 * m + 4 => n >= m + 2
*/

/*
 l = ceIl(n/2)
 stk usage : l * 2
*/
template <typename T>
void toom22_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom22_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
void toom2_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

extern template void toom2_sqr<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                         size_t n, uint64_t *stk);

/*
 l = max(ceil(n/3), ceil(m/2))
 stk usage : l * 4
*/
template <typename T>
void toom32_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom32_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
struct toom_interpolation_5p_struct;

template <typename T>
void toom_interpolation_5p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_5p_struct<T> &&flag);

extern template void
toom_interpolation_5p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_5p_struct<uint64_t> &&flag);

/*
 l = max(ceil(n/4), ceil(m/2))
 stk usage : l * 4
*/
template <typename T>
void toom42_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom42_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

/*
 l = ceil(n/3)
 stk usage : l * 4
*/
template <typename T>
void toom33_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom33_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
void toom3_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

extern template void toom3_sqr<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                         size_t n, uint64_t *stk);

template <typename T>
struct toom_interpolation_6p_struct;

template <typename T>
void toom_interpolation_6p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_6p_struct<T> &&flag);

extern template void
toom_interpolation_6p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_6p_struct<uint64_t> &&flag);

/*
 l = max(ceil(n/4), ceil(m/3))
 stk usage : l * 6
*/
template <typename T>
void toom43_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom43_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
struct toom_interpolation_7p_struct;

template <typename T>
void toom_interpolation_7p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_7p_struct<T> &&flag);

extern template void
toom_interpolation_7p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_7p_struct<uint64_t> &&flag);

/*
 l = max(ceil(n/5), ceil(m/3))
 stk usage : l * 6
*/
template <typename T>
void toom53_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom53_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

/*
 l = ceil(n/4)
 stk usage : l * 6
*/
template <typename T>
void toom44_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom44_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
void toom4_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

extern template void toom4_sqr<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                         size_t n, uint64_t *stk);

struct __mul_s_unique_stack_allocator {
    template <typename... Args>
    constexpr __mul_s_unique_stack_allocator(Args &&...) {}
};

template <typename P, typename T, typename U>
WJR_INTRINSIC_INLINE P *__mul_s_allocate(WJR_MAYBE_UNUSED T &mal,
                                         WJR_MAYBE_UNUSED U &alloc, size_t n) {
    if constexpr (std::is_same_v<remove_cvref_t<U>, __mul_s_unique_stack_allocator>) {
        static_assert(std::is_pointer_v<T>, "");
        (void)(alloc);
        P *ret = mal;
        mal += n;
        return ret;
    } else {
        (void)(mal);
        return static_cast<P *>(alloc.allocate(n));
    }
}

template <bool reserved, typename T>
void __toom22_mul_s_impl(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                         size_t m,
                         std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n + m, src0, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n + m, src1, m));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (m < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, m);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (n >= 3 * m) {
        T *tmp = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (4 * m));
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * m + 67));

        toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
        n -= 2 * m;
        src0 += 2 * m;
        dst += 2 * m;

        T cf = 0;

        while (n >= 3 * m) {
            toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + 3 * m, dst + m);
            cf = addc_1(dst + m, dst + m, 2 * m, 0, cf);

            dst += 2 * m;
        }

        if (4 * n < 5 * m) {
            toom22_mul_s(tmp, src0, n, src1, m, stk);
        } else if (4 * n < 7 * m) {
            toom32_mul_s(tmp, src0, n, src1, m, stk);
        } else {
            toom42_mul_s(tmp, src0, n, src1, m, stk);
        }

        cf = addc_n(dst, dst, tmp, m, cf);
        std::copy(tmp + m, tmp + m + n, dst + m);
        cf = addc_1(dst + m, dst + m, n, 0, cf);
        WJR_ASSERT(cf == 0);
    } else {
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * m + 67));

        if (4 * n < 5 * m) {
            toom22_mul_s(dst, src0, n, src1, m, stk);
        } else if (4 * n < 7 * m) {
            toom32_mul_s(dst, src0, n, src1, m, stk);
        } else {
            toom42_mul_s(dst, src0, n, src1, m, stk);
        }
    }

    return;
}

template <bool reserved, typename T>
void __noinline_mul_s_impl(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                           size_t m,
                           std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n + m, src0, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n + m, src1, m));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (m < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, m);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (m < toom33_mul_threshold) {
        if (n >= 3 * m) {
            T *tmp = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (4 * m));
            T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * m + 67));

            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;
            dst += 2 * m;

            T cf = 0;

            while (n >= 3 * m) {
                toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
                n -= 2 * m;
                src0 += 2 * m;

                cf = addc_n(dst, dst, tmp, m, cf);
                std::copy(tmp + m, tmp + 3 * m, dst + m);
                cf = addc_1(dst + m, dst + m, 2 * m, 0, cf);

                dst += 2 * m;
            }

            if (4 * n < 5 * m) {
                toom22_mul_s(tmp, src0, n, src1, m, stk);
            } else if (4 * n < 7 * m) {
                toom32_mul_s(tmp, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(tmp, src0, n, src1, m, stk);
            }

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + m + n, dst + m);
            cf = addc_1(dst + m, dst + m, n, 0, cf);
            WJR_ASSERT(cf == 0);
        } else {
            T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * m + 67));

            if (4 * n < 5 * m) {
                toom22_mul_s(dst, src0, n, src1, m, stk);
            } else if (4 * n < 7 * m) {
                toom32_mul_s(dst, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(dst, src0, n, src1, m, stk);
            }
        }

        return;
    }

    if (m < toom44_mul_threshold || 3 * n + 21 > 4 * m) {
        if (n >= 3 * m) {
            T *tmp = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (4 * m));
            T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * m + 288));

            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;
            dst += 2 * m;

            T cf = 0;

            while (n >= 3 * m) {
                toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
                n -= 2 * m;
                src0 += 2 * m;

                cf = addc_n(dst, dst, tmp, m, cf);
                std::copy(tmp + m, tmp + 3 * m, dst + m);
                cf = addc_1(dst + m, dst + m, 2 * m, 0, cf);

                dst += 2 * m;
            }

            __mul_s_impl<__mul_mode::all, true>(tmp, src0, n, src1, m, stk);

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + m + n, dst + m);
            cf = addc_1(dst + m, dst + m, n, 0, cf);
            WJR_ASSERT(cf == 0);
        } else {
            T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * m + 288));
            if (6 * n < 7 * m) {
                toom33_mul_s(dst, src0, n, src1, m, stk);
            } else if (2 * n < 3 * m) {
                if (m < toom32_to_toom43_mul_threshold) {
                    toom32_mul_s(dst, src0, n, src1, m, stk);
                } else {
                    toom43_mul_s(dst, src0, n, src1, m, stk);
                }
            } else if (6 * n < 11 * m) {
                if (4 * n < 7 * m) {
                    if (m < toom32_to_toom53_mul_threshold) {
                        toom32_mul_s(dst, src0, n, src1, m, stk);
                    } else {
                        toom53_mul_s(dst, src0, n, src1, m, stk);
                    }
                } else {
                    if (m < toom42_to_toom53_mul_threshold) {
                        toom42_mul_s(dst, src0, n, src1, m, stk);
                    } else {
                        toom53_mul_s(dst, src0, n, src1, m, stk);
                    }
                }
            } else {
                toom42_mul_s(dst, src0, n, src1, m, stk);
            }
        }

        return;
    }

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * m + 288));
    toom44_mul_s(dst, src0, n, src1, m, stk);
    return;
}

extern template void __noinline_mul_s_impl<true, uint64_t>(uint64_t *WJR_RESTRICT dst,
                                                           const uint64_t *src0, size_t n,
                                                           const uint64_t *src1, size_t m,
                                                           uint64_t *mal);

extern template void
__noinline_mul_s_impl<false, uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                       size_t n, const uint64_t *src1, size_t m,
                                       in_place_empty_t mal);

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__mul_s_impl(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
             std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    static_assert((int)__mul_mode::toom22 == 0, "");
    if (WJR_BUILTIN_CONSTANT_P(n == m) && n == m) {
        return __mul_n_impl<mode, reserved>(dst, src0, src1, n, mal);
    }

    if constexpr (mode == __mul_mode::toom22) {
        return __toom22_mul_s_impl<reserved>(dst, src0, n, src1, m, mal);
    } else {
        return __noinline_mul_s_impl<reserved>(dst, src0, n, src1, m, mal);
    }
}

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                  const T *src1, size_t m, T *stk) {
    return __mul_s_impl<mode, true>(dst, src0, n, src1, m, stk);
}

template <typename T>
WJR_INTRINSIC_INLINE void mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                const T *src1, size_t m) {
    return __mul_s_impl<__mul_mode::all, false>(dst, src0, n, src1, m, in_place_empty);
}

template <typename T>
WJR_INTRINSIC_INLINE void mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                const T *src1, size_t m, T *stk) {
    return __mul_s_impl<__mul_mode::all, true>(dst, src0, n, src1, m, stk);
}

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__inline_mul_n_impl(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n,
                    std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src0, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src1, n));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, n);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (mode <= __mul_mode::toom22 || n < toom33_mul_threshold) {
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * n + 67));
        return toom22_mul_s(dst, src0, n, src1, n, stk);
    }

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
    return toom33_mul_s(dst, src0, n, src1, n, stk);
}

template <bool reserved, typename T>
void __noinline_mul_n_impl(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n,
                           std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src0, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src1, n));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, n);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (n < toom33_mul_threshold) {
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * n + 67));
        return toom22_mul_s(dst, src0, n, src1, n, stk);
    }

    if (n < toom44_mul_threshold) {
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
        return toom33_mul_s(dst, src0, n, src1, n, stk);
    }

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
    return toom44_mul_s(dst, src0, n, src1, n, stk);
}

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__mul_n_impl(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n,
             std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return __sqr_impl<mode, reserved>(dst, src0, n, mal);
    }

    if constexpr (mode <= __mul_mode::toom33) {
        return __inline_mul_n_impl<mode, reserved, T>(dst, src0, src1, n, mal);
    } else {
        return __noinline_mul_n_impl<reserved>(dst, src0, src1, n, mal);
    }
}

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                  size_t n, T *stk) {
    return __mul_n_impl<mode, true>(dst, src0, src1, n, stk);
}

template <__mul_mode mode, typename T, T m0, T m1>
void __mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n, T *stk,
             T &c_out, type_identity_t<T> cf0, type_identity_t<T> cf1,
             std::integral_constant<T, m0> x0, std::integral_constant<T, m1> x1) {
    WJR_ASSERT_ASSUME(cf0 <= m0);
    WJR_ASSERT_ASSUME(cf1 <= m1);
    __mul_n<mode>(dst, src0, src1, n, stk);
    if constexpr (m0 == 0 || m1 == 0) {
        c_out = 0;
    } else if constexpr (m0 == 1 || m1 == 1) {
        if constexpr (m0 == 1 && m1 == 1) {
            c_out = cf0 && cf1;
        } else if constexpr (m0 == 1) {
            c_out = cf0 ? cf1 : 0;
        } else {
            c_out = cf1 ? cf0 : 0;
        }
    } else {
        c_out = cf0 * cf1;
    }
    c_out += try_addmul_1(dst + n, src1, n, cf0, x0);
    c_out += try_addmul_1(dst + n, src0, n, cf1, x1);
}

template <typename T>
WJR_INTRINSIC_INLINE void mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                size_t n) {
    return __mul_n_impl<__mul_mode::all, false>(dst, src0, src1, n, in_place_empty);
}

template <typename T>
WJR_INTRINSIC_INLINE void mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                size_t n, T *stk) {
    return __mul_n_impl<__mul_mode::all, true>(dst, src0, src1, n, stk);
}

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__inline_sqr_impl(T *WJR_RESTRICT dst, const T *src, size_t n,
                  std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src, n));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (n < toom2_sqr_threshold) {
        return basecase_sqr(dst, src, n);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (mode <= __mul_mode::toom22 || n < toom3_sqr_threshold) {
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * n + 67));
        return toom2_sqr(dst, src, n, stk);
    }

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
    return toom3_sqr(dst, src, n, stk);
}

template <bool reserved, typename T>
void __noinline_sqr_impl(T *WJR_RESTRICT dst, const T *src, size_t n,
                         std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src, n));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (n < toom2_sqr_threshold) {
        return basecase_sqr(dst, src, n);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (n < toom3_sqr_threshold) {
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * n + 67));
        return toom2_sqr(dst, src, n, stk);
    }

    if (n < toom4_sqr_threshold) {
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
        return toom3_sqr(dst, src, n, stk);
    }

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
    return toom4_sqr(dst, src, n, stk);
}

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__sqr_impl(T *WJR_RESTRICT dst, const T *src, size_t n,
           std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    if constexpr (mode <= __mul_mode::toom33) {
        return __inline_sqr_impl<mode, reserved>(dst, src, n, mal);
    } else {
        return __noinline_sqr_impl<reserved>(dst, src, n, mal);
    }
}

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    return __sqr_impl<mode, true>(dst, src, n, stk);
}

template <__mul_mode mode, typename T, T m>
void __sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk, T &c_out,
           type_identity_t<T> cf, std::integral_constant<T, m>) {
    WJR_ASSERT_ASSUME(cf <= m);
    __sqr<mode>(dst, src, n, stk);
    if constexpr (m == 0) {
        c_out = 0;
    } else if constexpr (m == 1) {
        c_out = cf;
    } else {
        c_out = cf * cf;
    }

    constexpr auto m2 = m <= ((uint32_t)in_place_max) ? m * 2 : m;

    c_out += try_addmul_1(dst + n, src, n, 2 * cf, std::integral_constant<T, m2>{});
}

template <typename T>
WJR_INTRINSIC_INLINE void sqr(T *WJR_RESTRICT dst, const T *src, size_t n) {
    __sqr_impl<__mul_mode::all, false>(dst, src, n, in_place_empty);
}

template <typename T>
WJR_INTRINSIC_INLINE void sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    __sqr_impl<__mul_mode::all, true>(dst, src, n, stk);
}

template <typename T>
void fallback_basecase_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                             size_t m) {
    dst[n] = mul_1(dst, src0, n, src1[0]);
    for (size_t i = 1; i < m; ++i) {
        ++dst;
        dst[n] = addmul_1(dst, src0, n, src1[i]);
    }
}

/*
require :
1. m >= 1
2. n >= m
3. WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src0, n)
4. WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src1, m)
*/
template <typename T>
void basecase_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                    size_t m) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src1, m));

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)
    return asm_basecase_mul_s(dst, src0, n, src1, m);
#else
    return fallback_basecase_mul_s(dst, src0, n, src1, m);
#endif
}

// TODO : optimize
template <typename T>
void basecase_sqr(T *WJR_RESTRICT dst, const T *src, size_t n) {
#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)
    return asm_basecase_sqr(dst, src, n);
#else
    return basecase_mul_s(dst, src, n, src, n);
#endif
}

template <typename T>
void toom22_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    // [u0(l), u1(rn)] (n)
    // [v0(l), v1(rm)] (m)

    // (u0 - u1) * (v0 - v1)

    WJR_ASSERT_ASSUME(m <= n && n < 2 * m);

    const size_t l = (n + 1) / 2;
    const size_t rn = n - l;
    const size_t rm = m - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);
    WJR_ASSERT_ASSUME(l - rn <= 1);

    const auto u0 = src0;
    const auto u1 = src0 + l;

    const auto v0 = src1;
    const auto v1 = src1 + l;

    const auto p0 = dst;
    const auto p1 = dst + l;
    const auto p2 = dst + l * 2;
    const auto p3 = dst + l * 3;

    const auto wp = stk;
    stk += l * 2;

    bool f = 0;

    do {
        ssize_t p;
        p = abs_subc_s(p0, u0, l, u1, rn);
        if (__fasts_is_negative(p)) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        p = abs_subc_s(p1, v0, l, v1, rm);
        if (__fasts_is_negative(p)) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        __mul_n<__mul_mode::toom22>(wp, p0, p1, l, stk);
        break;
    ZERO:
        set_n(wp, 0, l * 2);
        break;
    } while (0);

    __mul_n<__mul_mode::toom22>(p0, u0, v0, l, stk);
    __mul_s<__mul_mode::toom22>(p2, u1, rn, v1, rm, stk);

    T cf = 0, cf2 = 0;

    cf = addc_n(p2, p1, p2, l);
    cf2 = cf + addc_n(p1, p0, p2, l);
    cf += addc_sz(p2, p2, l, p3, rn + rm - l);

    if (!f) {
        cf -= subc_n(p1, p1, wp, l * 2);
    } else {
        cf += addc_n(p1, p1, wp, l * 2);
    }

    cf2 = addc_1(p2, p2, l, cf2);
    if (WJR_LIKELY(rn + rm != l)) {
        cf = addc_1(p3, p3, rn + rm - l, cf, cf2);
    }
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom2_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    const size_t l = (n + 1) / 2;
    const size_t rn = n - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(rn * 2 >= l);

    const auto u0 = src;
    const auto u1 = src + l;

    const auto p0 = dst;
    const auto p1 = dst + l;
    const auto p2 = dst + l * 2;
    const auto p3 = dst + l * 3;

    const auto wp = stk;
    stk += l * 2;

    do {
        ssize_t p = abs_subc_s(p0, u0, l, u1, rn);
        if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        __sqr<__mul_mode::toom22>(wp, p0, l, stk);
        break;
    ZERO:
        set_n(wp, 0, l * 2);
        break;
    } while (0);

    __sqr<__mul_mode::toom22>(p0, u0, l, stk);
    __sqr<__mul_mode::toom22>(p2, u1, rn, stk);

    T cf = 0, cf2 = 0;

    cf = addc_n(p2, p1, p2, l);
    cf2 = cf + addc_n(p1, p0, p2, l);
    cf += addc_sz(p2, p2, l, p3, rn * 2 - l);

    cf -= subc_n(p1, p1, wp, l * 2);

    cf2 = addc_1(p2, p2, l, cf2);
    if (WJR_LIKELY(rn * 2 != l)) {
        cf = addc_1(p3, p3, rn * 2 - l, cf, cf2);
    }
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom32_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    WJR_ASSERT_ASSUME(m + 2 <= n && n + 8 <= 3 * m);

    const size_t l = (2 * n >= 3 * m ? (n + 2) / 3 : (m + 1) / 2);
    const size_t rn = n - l * 2;
    const size_t rm = m - l;
    const size_t maxr = std::max(rn, rm);

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;

    const auto v0p = src1;
    const auto v1p = src1 + l;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = stk + l * 2;
    const auto w3p = dst + l * 3;

    stk += l * 4;

    T cf = 0;
    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0;
    bool neg0 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);

    // W3 = V0 + V1 : (non-negative) v(1)
    cf3 = addc_s(w3p, v0p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf3 <= 1);

    // W2 = W0 + U1 : (non-negative) u(1)
    cf2 = cf0 + addc_n(w2p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf2 <= 2);

    // W1 = W2 * W3 : (non-negative) f(1) = r1
    __mul_n<__mul_mode::toom33, T, 2, 1>(w1p, w2p, w3p, l, stk, cf1, cf2, cf3);

    // W0 = W0 - U1 : u(-1)
    {
        ssize_t p = abs_subc_n(w0p, w0p, u1p, l, cf0, cf0, 0);
        neg0 = __fasts_is_negative(p);
    }
    WJR_ASSERT_ASSUME(cf0 <= 1);

    // W3 = V0 - V1 : v(-1)
    {
        ssize_t p = abs_subc_s(w3p, v0p, l, v1p, rm);
        neg3 = __fasts_is_negative(p);
    }

    // W2 = W0 * W3 : f(-1) = r2
    neg0 ^= neg3;
    __mul_n<__mul_mode::toom33, T, 1, 0>(w2p, w0p, w3p, l, stk, cf2, cf0, 0);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W3 = U2 * V1 : (non-negative) f(inf) = r3
    if (rn >= rm) {
        __mul_s<__mul_mode::toom33>(w3p, u2p, rn, v1p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom33>(w3p, v1p, rm, u2p, rn, stk);
    }

    // W1 = (W1 - W2) >> 1 : (non-negative) (f(1) - f(-1)) / 2
    {
        if (!neg0) {
            cf1 = cf1 - cf2 - subc_n(w1p, w1p, w2p, l * 2);
        } else {
            cf1 = cf1 + cf2 + addc_n(w1p, w1p, w2p, l * 2);
        }

        rshift_n(w1p, w1p, l * 2, 1, cf1);
        cf1 >>= 1;
    }

    // W2 = (W1 + W2) - W0 : (non-negative) r2
    {
        if (!neg0) {
            cf2 = cf1 + cf2 + addc_n(w2p, w1p, w2p, l * 2);
        } else {
            cf2 = cf1 - cf2 - subc_n(w2p, w1p, w2p, l * 2);
        }

        cf2 -= subc_n(w2p, w2p, w0p, l * 2);
        if (l != maxr) {
            WJR_ASSERT(cf2 == 0);
            cf2 = w2p[l + maxr];
        }
    }

    // W1 = W1 - W3 : (non-negative) r1
    cf1 -= subc_s(w1p, w1p, l * 2, w3p, rn + rm);

    // W = W3*x^3+W2*x^2+W1*x+W0
    cf = addc_n(w0p + l, w0p + l, w1p, l);
    cf = addc_n(dst + l * 2, w1p + l, w2p, l, cf);
    cf = addc_n(w3p, w3p, w2p + l, maxr, cf);
    cf2 += addc_1(w3p, w3p, maxr, cf1);
    cf = addc_1(w3p + maxr, w3p + maxr, (rn + rm) - maxr, cf2, cf);
    WJR_ASSERT(cf == 0);
}

template <typename T>
struct toom_interpolation_5p_struct {
    bool neg1;
    T cf1;
    T cf2;
    T cf3;
};

template <typename T>
void toom_interpolation_5p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_5p_struct<T> &&flag) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(2);
     W4 = f(inf);
    */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    const auto w0p = dst;
    const auto w2p = w0p + l * 2;
    const auto w3p = w1p + l * 2;
    const auto w4p = w0p + l * 4;

    T cf;
    auto [neg1, cf1, cf2, cf3] = flag;

    // W3 = (W3 - W1) / 3 : (non-negative) (f(2) - f(-1)) / 3
    {
        if (!neg1) {
            cf3 -= cf1 + subc_n(w3p, w3p, w1p, l * 2);
        } else {
            cf3 += cf1 + addc_n(w3p, w3p, w1p, l * 2);
        }

        divexact_by3(w3p, w3p, l * 2);
        cf3 /= 3;
    }

    // W1 = (W2 - W1) >> 1 : (non-negative) (f(1) - f(-1)) / 2
    {
        if (!neg1) {
            cf1 = cf2 - cf1 - subc_n(w1p, w2p, w1p, l * 2);
        } else {
            cf1 = cf2 + cf1 + addc_n(w1p, w2p, w1p, l * 2);
        }

        rshift_n(w1p, w1p, l * 2, 1, cf1);
        cf1 >>= 1;
    }

    // W2 = W2 - W0 : (non-negative) f(1) - f(0)
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);

    // W3 = ((W3 - W2) >> 1) - (W4 << 1) : (non-negative) r3
    {
        cf3 -= cf2 + subc_n(w3p, w3p, w2p, l * 2);

        rshift_n(w3p, w3p, l * 2, 1, cf3);
        cf3 >>= 1;

        cf = submul_1(w3p, w4p, rn + rm, 2);
        if (l * 2 != (rn + rm)) {
            cf3 -= subc_1(w3p + rn + rm, w3p + rn + rm, l * 2 - (rn + rm), cf);
            if (l != maxr) {
                WJR_ASSERT(cf3 == 0);
                cf3 = w3p[l + maxr];
            }
        } else {
            cf3 -= cf;
        }
    }

    // W2 = W2 - W1 : (non-negative) f(1) / 2 - f(0) + f(-1) / 2
    cf2 -= cf1 + subc_n(w2p, w2p, w1p, l * 2);

    // W3 = W4 * x + W3 : r4 * x + r3
    cf = addc_n(w4p, w4p, w3p + l, maxr);
    cf = addc_1(w4p + maxr, w4p + maxr, rn + rm - maxr, cf3, cf);
    WJR_ASSERT(cf == 0);

    // W1 = W2 * x + W1 :
    cf = addc_n(w2p, w2p, w1p + l, l);
    cf2 += addc_1(w2p + l, w2p + l, l, cf1, cf);

    // W1 = W1 - W3 : // r2 * x + r1
    cf = subc_n(w1p, w1p, w3p, l);
    cf2 -= subc_s(w2p, w2p, l * 2, w4p, rn + rm, cf);

    // W  = W3*x^3 + W1*x + W0
    cf = addc_n(dst + l, dst + l, w1p, l);
    cf = addc_1(dst + l * 2, dst + l * 2, l, cf);
    cf = addc_n(dst + l * 3, dst + l * 3, w3p, l, cf);
    cf = addc_1(dst + l * 4, dst + l * 4, rn + rm, cf2, cf);
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom42_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(2);
     W4 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - U1 + U2 - U3) * (V0 - v1);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1);
     W3 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V1);
     W4 = U3 * V1;
    */

    /*
     7 add/sub + 3 addlsh

     Not implemented according to the following steps

     T0 = U0 + U2;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W3 = V0 - V1; v(-1)
     T2 = V0 + V1; v(1)
     W1 = T1 * W3; f(-1)
     W2 = T0 * T2; f(1)
     T0 = ((2U3 + U2) << 1 + U1) << 1 + U0; u(2)
     T2 = T0 + V1; v(2)
     W3 = T0 * T2; f(2)
     W0 = U0 * V0;
     W4 = U3 * V1;
    */

    WJR_ASSERT_ASSUME(3 * m + 5 <= 2 * n && n + 15 <= 4 * m);

    const size_t l = (n >= 2 * m ? (n + 3) / 4 : (m + 1) / 2);
    const size_t rn = n - l * 3;
    const size_t rm = m - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;

    const auto v0p = src1;
    const auto v1p = src1 + l;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;

    stk += l * 4;

    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_n(w0p, u0p, u2p, l);

    // W4 = V0 + V1 : (non-negative) v(1)
    cf4 = addc_s(w4p, v0p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf4 <= 1);

    // W1 = U1 + U3 : (non-negative)
    cf1 = addc_s(w1p, u1p, l, u3p, rn);

    // W3 = W0 - W1 : u(-1)
    {
        ssize_t p = abs_subc_n(w3p, w0p, w1p, l, cf3, cf0, cf1);
        neg3 = __fasts_is_negative(p);
    }
    WJR_ASSERT_ASSUME(cf3 <= 1);

    // W2 = V0 - V1 : v(-1)
    {
        ssize_t p = abs_subc_s(w2p, v0p, l, v1p, rm);
        neg2 = __fasts_is_negative(p);
    }

    // W0 = W0 + W1 : (non-negative) u(1)
    cf0 += cf1 + addc_n(w0p, w0p, w1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 3);

    // W1 = W3 * W2 : f(-1)
    neg2 ^= neg3;
    __mul_n<__mul_mode::toom33, T, 1, 0>(w1p, w3p, w2p, l, stk, cf1, cf3, 0);

    // W2 = W0 * W4 : (non-negative) f(1)
    __mul_n<__mul_mode::toom33, T, 3, 1>(w2p, w0p, w4p, l, stk, cf2, cf0, cf4);

    // W0 = U0 +(U1 +(U2 + U3<<1)<<1)<<1 : (non-negative) u(2)
    {
        cf0 = addlsh_n(w0p, u2p, u3p, rn, 1);
        if (l != rn) {
            cf0 = addc_1(w0p + rn, u2p + rn, l - rn, cf0);
        }

        cf0 += cf0 + addlsh_n(w0p, u1p, w0p, l, 1);
        cf0 += cf0 + addlsh_n(w0p, u0p, w0p, l, 1);
        WJR_ASSERT_ASSUME(cf0 <= 14);
    }

    // W4 = W4 + V1 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf4 <= 2);

    // W3 = W0 * W4 : (non-negative) f(2)
    __mul_n<__mul_mode::toom33, T, in_place_max, 2>(w3p, w0p, w4p, l, stk, cf3, cf0, cf4);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W4 = U3 * V1 : (non-negative) f(inf) = r4
    if (rn >= rm) {
        __mul_s<__mul_mode::toom33>(w4p, u3p, rn, v1p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom33>(w4p, v1p, rm, u3p, rn, stk);
    }

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct<T>{neg2, cf1, cf2, cf3});
}

template <typename T>
void toom33_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    WJR_ASSERT_ASSUME(2 * n + 10 <= 3 * m);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;

    stk += l * 4;

    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);
    // W4 = V0 + V2 : (non-negative)
    cf4 = addc_s(w4p, v0p, l, v2p, rm);

    // W3 = W0 - U1 : u(-1)
    {
        ssize_t p = abs_subc_n(w3p, w0p, u1p, l, cf3, cf0, 0);
        neg3 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf3 <= 1);
    }

    // W2 = W4 - V1 : v(-1)
    {
        ssize_t p = abs_subc_n(w2p, w4p, v1p, l, cf2, cf4, 0);
        neg2 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf2 <= 1);
    }

    // W0 = W0 + U1 : (non-negative) u(1)
    cf0 += addc_n(w0p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 2);

    // W4 = W4 + V1 : (non-negative) v(1)
    cf4 += addc_n(w4p, w4p, v1p, l);
    WJR_ASSERT_ASSUME(cf4 <= 2);

    // W1 = W3 * W2 : f(-1)
    neg2 ^= neg3;
    __mul_n<__mul_mode::toom33, T, 1, 1>(w1p, w3p, w2p, l, stk, cf1, cf3, cf2);

    // W2 = W0 * W4 : (non-negative) f(1)
    __mul_n<__mul_mode::toom33, T, 2, 2>(w2p, w0p, w4p, l, stk, cf2, cf0, cf4);

    // W0 = (W0 + U2) << 1 - U0 : (non-negative) u(2)
    cf0 += addc_s(w0p, w0p, l, u2p, rn);
    cf0 += cf0 + rsblsh_n(w0p, u0p, w0p, l, 1);
    WJR_ASSERT_ASSUME(cf0 <= 6);

    // W4 = (W4 + V2) << 1 - V0 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v2p, rm);
    cf4 += cf4 + rsblsh_n(w4p, v0p, w4p, l, 1);
    WJR_ASSERT_ASSUME(cf4 <= 6);

    // W3 = W0 * W4 : (non-negative) f(2)
    __mul_n<__mul_mode::toom33>(w3p, w0p, w4p, l, stk, cf3, cf0, cf4);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W4 = U2 * V2 : (non-negative) f(inf) = r4
    __mul_s<__mul_mode::toom33>(w4p, u2p, rn, v2p, rm, stk);

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct<T>{neg2, cf1, cf2, cf3});
}

template <typename T>
void toom3_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    WJR_ASSERT_ASSUME(10 <= n);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);

    const auto u0p = src;
    const auto u1p = src + l;
    const auto u2p = src + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;

    stk += l * 4;

    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);

    // W3 = W0 - U1 : u(-1)
    {
        (void)abs_subc_n(w3p, w0p, u1p, l, cf3, cf0, 0);
        WJR_ASSERT_ASSUME(cf3 <= 1);
    }

    // W0 = W0 + U1 : (non-negative) u(1)
    cf0 += addc_n(w0p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 2);

    // W1 = W3 * W3 : f(-1)
    __sqr<__mul_mode::toom33, T, 1>(w1p, w3p, l, stk, cf1, cf3);

    // W2 = W0 * W9 : (non-negative) f(1)
    __sqr<__mul_mode::toom33, T, 2>(w2p, w0p, l, stk, cf2, cf0);

    // W0 = (W0 + U2) << 1 - U0 : (non-negative) u(2)
    cf0 += addc_s(w0p, w0p, l, u2p, rn);
    cf0 += cf0 + rsblsh_n(w0p, u0p, w0p, l, 1);
    WJR_ASSERT_ASSUME(cf0 <= 6);

    // W3 = W0 * W0 : (non-negative) f(2)
    __sqr<__mul_mode::toom33>(w3p, w0p, l, stk, cf3, cf0);

    // W0 = U0 * U0 : (non-negative) f(0) = r0
    __sqr<__mul_mode::toom33>(w0p, u0p, l, stk);

    // W4 = U2 * U2 : (non-negative) f(inf) = r4
    __sqr<__mul_mode::toom33>(w4p, u2p, rn, stk);

    return toom_interpolation_5p_s(dst, w1p, l, rn, rn,
                                   toom_interpolation_5p_struct<T>{0, cf1, cf2, cf3});
}

template <typename T>
struct toom_interpolation_6p_struct {
    uint8_t neg1 : 1;
    uint8_t neg3 : 1;
    T cf1;
    T cf2;
    T cf3;
    T cf4;
};

template <typename T>
void toom_interpolation_6p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_6p_struct<T> &&flag) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(-2);
     W4 = f(2);
     W5 = f(inf)
    */

    /*
     11 add/sub + 4 shift + 2 divby3 + 1 submul

     W3 = (W3 - W4) >> 1;
     W4 = W4 + W3;
     W4 = W4 - W0;
     W1 = W2 - W1;
     W2 = W2 - W0;
     W3 = W3 + W1;
     W1 >>= 1;
     W3 = W3 / -6; => W3 = W3 / -2 / 3;
     W2 = W2 - W1;
     W3 = W3 - 4 * W5;
     W1 = W1 - W3;
     W4 = W4 >> 2;
     W4 = W4 - W2;
     W4 = W4 / 3;

     W4 = W5 * x + W4;
     W2 = W3 * x + W2;
     W2 = W2 - W4;
     W = W4 * x ^ 4 + W2 * x ^ 2 + W1 * x + W0;
    */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    const auto w0p = dst;
    const auto w2p = w0p + l * 2;
    const auto w3p = w1p + l * 2;
    const auto w4p = w1p + l * 4;
    const auto w5p = w0p + l * 5;

    T cf;
    auto [neg1, neg3, cf1, cf2, cf3, cf4] = flag;

    // W3 = (W3 - W4) >> 1;
    {
        if (!neg3) {
            cf3 = cf4 - cf3 - subc_n(w3p, w4p, w3p, l * 2);
        } else {
            cf3 += cf4 + addc_n(w3p, w3p, w4p, l * 2);
        }

        (void)rshift_n(w3p, w3p, l * 2, 1, cf3);
        cf3 >>= 1;
    }

    // W4 = W4 + W3;
    cf4 -= cf3 + subc_n(w4p, w4p, w3p, l * 2);

    // W4 = W4 - W0;
    cf4 -= subc_n(w4p, w4p, w0p, l * 2);

    // W1 = W2 - W1;
    if (!neg1) {
        cf1 = cf2 - cf1 - subc_n(w1p, w2p, w1p, l * 2);
    } else {
        cf1 = cf2 + cf1 + addc_n(w1p, w2p, w1p, l * 2);
    }

    // W2 = W2 - W0;
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);

    // W3 = W3 + W1;
    cf3 -= cf1 + subc_n(w3p, w3p, w1p, l * 2);

    // W1 >>= 1;
    (void)rshift_n(w1p, w1p, l * 2, 1, cf1);
    cf1 >>= 1;

    // W3 = W3 / -6; = > W3 = W3 / -2 / 3;
    {
        (void)rshift_n(w3p, w3p, l * 2, 1, cf3);
        divexact_by3(w3p, w3p, l * 2);
        cf3 /= 6;
    }

    // W2 = W2 - W1;
    cf2 -= cf1 + subc_n(w2p, w2p, w1p, l * 2);

    // W3 = W3 - 4 * W5;
    {
        cf = submul_1(w3p, w5p, rn + rm, 4);
        if (rn + rm != l * 2) {
            cf3 -= subc_1(w3p + rn + rm, w3p + rn + rm, l * 2 - (rn + rm), cf);
        } else {
            cf3 -= cf;
        }
    }

    // W1 = W1 - W3;
    cf1 -= cf3 + subc_n(w1p, w1p, w3p, l * 2);

    // W4 = W4 >> 2;
    rshift_n(w4p, w4p, l * 2, 2, cf4);
    cf4 >>= 2;

    // W4 = W4 - W2;
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, l * 2);

    // W4 = W4 / 3;
    divexact_by3(w4p, w4p, l * 2);
    if (l != maxr) {
        cf4 = w4p[l + maxr];
    } else {
        cf4 /= 3;
    }

    // W4 = W5 * x + W4;
    cf = addc_n(w5p, w5p, w4p + l, maxr);
    cf = addc_1(w5p + maxr, w5p + maxr, rn + rm - maxr, cf4, cf);
    WJR_ASSERT(cf == 0);

    // W2 = W3 * x + W2;
    cf = addc_n(w3p, w3p, w2p + l, l);
    cf3 += addc_1(w3p + l, w3p + l, l, cf2, cf);

    // W2 = W2 - W4;
    cf = subc_n(w2p, w2p, w4p, l);
    cf3 -= subc_s(dst + l * 3, w3p, l * 2, w5p, rn + rm, cf);

    // W = W4 * x ^ 4 + W2 * x ^ 2 + W1 * x + W0;
    cf = addc_n(dst + l, dst + l, w1p, l * 2);
    cf = addc_1(dst + l * 3, dst + l * 3, l, cf1, cf);
    cf = addc_n(dst + l * 4, dst + l * 4, w4p, l, cf);
    cf = addc_1(dst + l * 5, dst + l * 5, rn + rm, cf3, cf);
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom43_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(-2);
     W4 = f(2);
     W5 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - U1 + U2 - U3) * (V0 - V1 + V2);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1 + V2);
     W3 = (U0 - 2U1 + 4U2 - 8U3) * (V0 - 2V2 + 4V3);
     W4 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V2 + 4V3);
     W5 = U3 * V2;
    */

    /*
        dst :
        |--- l * 2 ---|--- l * 2 ---|- l --|--- rn+rm ---|
        W0             W2            T2    W5
        |- l --|- l --|
        T0      T1
        stk :
        |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
        W1             W3            W4
     */

    /*
     11 add/sub + 4 addlsh/rsblsh + 1 shift

     W3 = U0 + U2;
     W4 = U1 + U3;
     T0 = W3 - W4; u(-1)
     T1 = W3 + W4; u(1)
     W3 = V0 + V2;
     W4 = W3 - V1; v(-1)
     W1 = T0 * W4; f(-1)
     W5 = W3 + V1; v(1)
     W2 = T1 * W5; f(1)
     W4 = (W4 + V2) << 1 - V0; v(-2)
     W5 = (W5 + V2) << 1 - V0; v(2)
     T0 = U0 + 4U2;
     T1 = (U1 + 4U3) << 1;
     T2 = T0 - T1; u(-2)
     W3 = T2 * W4; f(-2)
     T2 = T0 + T1; u(2)
     W4 = T2 * W5; f(2)
     W0 = U0 * V0;
     W5 = U3 * V2;
     */

    WJR_ASSERT_ASSUME(3 * m + 8 <= 3 * n && 2 * n + 18 <= 4 * m);

    const size_t l = (3 * n >= 4 * m ? (n + 3) / 4 : (m + 2) / 3);
    const size_t rn = n - l * 3;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = u0p + l;
    const auto u2p = u0p + l * 2;
    const auto u3p = u0p + l * 3;

    const auto v0p = src1;
    const auto v1p = v0p + l;
    const auto v2p = v0p + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = stk + l * 4;
    const auto w5p = w0p + l * 5;

    const auto t0p = w0p;
    const auto t1p = t0p + l;
    const auto t2p = w0p + l * 4;

    stk += l * 6;

    // Hand it over to the compiler to optimize reusable variables
    T cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    T cft0 = 0, cft1 = 0, cft2 = 0;
    // Reusable
    // not point to real W0...W5 or T0...T2
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  W3 = U0 + U2;
    cf3 = addc_n(w3p, u0p, u2p, l);

    //  W4 = U1 + U3;
    cf4 = addc_s(w4p, u1p, l, u3p, rn);

    //  T0 = W3 - W4; u(-1)
    {
        ssize_t p = abs_subc_n(t0p, w3p, w4p, l, cft0, cf3, cf4);
        neg0 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft0 <= 1);
    }

    //  T1 = W3 + W4; u(1)
    cft1 = cf3 + cf4 + addc_n(t1p, w3p, w4p, l);
    WJR_ASSERT_ASSUME(cft1 <= 3);

    //  W3 = V0 + V2;
    cf3 = addc_s(w3p, v0p, l, v2p, rm);

    //  W4 = W3 - V1; v(-1)
    {
        ssize_t p = abs_subc_n(w4p, w3p, v1p, l, cf4, cf3, 0);
        neg1 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf4 <= 1);
    }

    //  W1 = T0 * W4; f(-1)
    neg0 ^= neg1;
    __mul_n<__mul_mode::toom33, T, 1, 1>(w1p, t0p, w4p, l, stk, cf1, cft0, cf4);

    //  W5 = W3 + V1; v(1)
    cf5 = cf3 + addc_n(w5p, w3p, v1p, l);
    WJR_ASSERT_ASSUME(cf5 <= 2);

    //  W2 = T1 * W5; f(1)
    __mul_n<__mul_mode::toom33, T, 3, 2>(w2p, t1p, w5p, l, stk, cf2, cft1, cf5);

    //  W4 = (W4 + V2) << 1 - V0; v(-2)
    {
        if (!neg1) {
            cf4 += addc_s(w4p, w4p, l, v2p, rm);
        } else {
            WJR_ASSERT_ASSUME(cf4 == 0);
            ssize_t p = abs_subc_s(w4p, w4p, l, v2p, rm);
            neg1 = p > 0;
        }

        if (!neg1) {
            // W4 maybe less than V0
            // use lshfit + abs_sub instead of rsblsh_n
            cf4 += cf4 + lshift_n(w4p, w4p, l, 1);
            {
                ssize_t p = abs_subc_n(w4p, w4p, v0p, l, cf4, cf4, 0);
                neg1 = __fasts_is_negative(p);
            }
        } else {
            WJR_ASSERT_ASSUME(cf4 == 0);
            cf4 += addlsh_n(w4p, v0p, w4p, l, 1);
        }
        WJR_ASSERT_ASSUME(cf4 <= 4);
    }

    //  W5 = (W5 + V2) << 1 - V0; v(2)
    cf5 += addc_s(w5p, w5p, l, v2p, rm);
    cf5 += cf5 + rsblsh_n(w5p, v0p, w5p, l, 1);
    WJR_ASSERT_ASSUME(cf5 <= 6);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  T1 = (U1 + 4U3) << 1;
    cft1 = addlsh_n(t1p, u1p, u3p, rn, 2);
    if (l != rn) {
        cft1 = addc_1(t1p + rn, u1p + rn, l - rn, cft1);
    }
    cft1 += cft1 + lshift_n(t1p, t1p, l, 1);
    WJR_ASSERT_ASSUME(cft1 <= 9);

    //  T2 = T0 - T1; u(-2)
    {
        ssize_t p = abs_subc_n(t2p, t0p, t1p, l, cft2, cft0, cft1);
        neg2 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft2 <= 9);
    }

    //  W3 = T2 * W4; f(-2)
    neg1 ^= neg2;
    __mul_n<__mul_mode::toom33>(w3p, t2p, w4p, l, stk, cf3, cft2, cf4);

    //  T2 = T0 + T1; u(2)
    cft2 = cft0 + cft1 + addc_n(t2p, t0p, t1p, l);
    WJR_ASSERT_ASSUME(cft2 <= 14);

    //  W4 = T2 * W5; f(2)
    __mul_n<__mul_mode::toom33>(w4p, t2p, w5p, l, stk, cf4, cft2, cf5);

    //  W0 = U0 * V0;
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    //  W5 = U3 * V2;
    if (rn >= rm) {
        __mul_s<__mul_mode::toom33>(w5p, u3p, rn, v2p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom33>(w5p, v2p, rm, u3p, rn, stk);
    }

    toom_interpolation_6p_s(
        dst, w1p, l, rn, rm,
        toom_interpolation_6p_struct<T>{neg0, neg1, cf1, cf2, cf3, cf4});
}

template <typename T>
struct toom_interpolation_7p_struct {
    uint8_t neg1 : 1;
    uint8_t neg3 : 1;
    T cf1;
    T cf2;
    T cf3;
    T cf4;
    T cf5;
};

template <typename T>
void toom_interpolation_7p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_7p_struct<T> &&flag) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);
    */

    /*
    [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   1, -2,  4, -8, 16,-32, 64
    W2   1,  1,  1,  1,  1,  1,  1
    W3   1, -1,  1, -1,  1, -1,  1
    W4   1,  2,  4,  8, 16, 32, 64
    W5  64, 32, 16,  8,  4,  2,  1
    W6   0,  0,  0,  0,  0,  0,  1
    ]

     W5 = W5 + W4
     W1 =(W4 - W1)/2
     W4 = W4 - W0
     W4 =(W4 - W1)/4 - W6*16
     W3 =(W2 - W3)/2
     W2 = W2 - W3

     [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   0,  2,  0,  8,  0, 32,  0
    W2   1,  0,  1,  0,  1,  0,  1
    W3   0,  1,  0,  1,  0,  1,  0
    W4   0,  0,  1,  0,  4,  0,  0
    W5  65, 34, 20, 16, 20, 34, 65
    W6   0,  0,  0,  0,  0,  0,  1
    ]

     W5 = W5 - W2*65      May be negative.
     W2 = W2 - W6 - W0
     W5 =(W5 + W2*45)/2   Now >= 0 again.
     W4 =(W4 - W2)/3
     W2 = W2 - W4

     [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   0,  2,  0,  8,  0, 32,  0
    W2   0,  0,  1,  0,  0,  0,  0
    W3   0,  1,  0,  1,  0,  1,  0
    W4   0,  0,  0,  0,  1,  0,  0
    W5   0, 17,  0,  8,  0, 17,  0
    W6   0,  0,  0,  0,  0,  0,  1
    ]

     W1 = W5 - W1         May be negative.
     W5 =(W5 - W3*8)/9
     W3 = W3 - W5
     W1 =(W1/15 + W5)/2   Now >= 0 again.
     W5 = W5 - W1

     [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   0,  1,  0,  0,  0,  0,  0
    W2   0,  0,  1,  0,  0,  0,  0
    W3   0,  0,  0,  1,  0,  0,  0
    W4   0,  0,  0,  0,  1,  0,  0
    W5   0,  0,  0,  0,  0,  1,  0
    W6   0,  0,  0,  0,  0,  0,  1
    ]

    */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    const auto w0p = dst;
    const auto w2p = w0p + l * 2;
    const auto w3p = w1p + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = w1p + l * 4;
    const auto w6p = w0p + l * 6;

    T cf = 0;
    auto [neg1, neg3, cf1, cf2, cf3, cf4, cf5] = flag;

    //  W5 = W5 + W4
    cf5 += cf4 + addc_n(w5p, w5p, w4p, l * 2);

    //  W1 =(W4 - W1)/2
    if (!neg1) {
        WJR_ASSERT(cf4 >= cf1);
        cf1 = cf4 - cf1 - subc_n(w1p, w4p, w1p, l * 2);
    } else {
        cf1 += cf4 + addc_n(w1p, w4p, w1p, l * 2);
    }
    (void)rshift_n(w1p, w1p, l * 2, 1, cf1);
    cf1 >>= 1;

    //  W4 = W4 - W0
    cf4 -= subc_n(w4p, w4p, w0p, l * 2);

    //  W4 =(W4 - W1)/4 - W6*16
    cf4 -= cf1 + subc_n(w4p, w4p, w1p, l * 2);
    (void)rshift_n(w4p, w4p, l * 2, 2, cf4);
    cf4 >>= 2;
    cf = submul_1(w4p, w6p, rn + rm, 16);
    if (rn + rm != l * 2) {
        cf4 -= subc_1(w4p + rn + rm, w4p + rn + rm, l * 2 - (rn + rm), cf);
    } else {
        cf4 -= cf;
    }

    //  W3 =(W2 - W3)/2
    if (!neg3) {
        WJR_ASSERT(cf2 >= cf3);
        cf3 = cf2 - cf3 - subc_n(w3p, w2p, w3p, l * 2);
    } else {
        cf3 += cf2 + addc_n(w3p, w2p, w3p, l * 2);
    }
    (void)rshift_n(w3p, w3p, l * 2, 1, cf3);
    cf3 >>= 1;

    //  W2 = W2 - W3
    WJR_ASSERT(cf2 >= cf3);
    cf2 -= cf3 + subc_n(w2p, w2p, w3p, l * 2);

    //  W5 = W5 - W2*65      May be negative.
    cf5 -= cf2 * 65 + submul_1(w5p, w2p, l * 2, 65);

    //  W2 = W2 - W6 - W0
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);
    cf2 -= subc_s(w2p, w2p, l * 2, w6p, rn + rm);

    //  W5 =(W5 + W2*45)/2   Now >= 0 again.
    cf5 += cf2 * 45 + addmul_1(w5p, w2p, l * 2, 45);
    (void)rshift_n(w5p, w5p, l * 2, 1, cf5);
    cf5 >>= 1;

    //  W4 =(W4 - W2)/3
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, l * 2);
    divexact_by3(w4p, w4p, l * 2);
    cf4 /= 3;

    //  W2 = W2 - W4
    cf2 -= cf4 + subc_n(w2p, w2p, w4p, l * 2);

    //  W1 = W5 - W1         May be negative.
    {
        ssize_t p = abs_subc_n(w1p, w5p, w1p, l * 2, cf1, cf5, cf1);
        neg1 = __fasts_is_negative(p);
    }

    //  W5 =(W5 - W3*8)/9
    cf5 -= cf3 * 8 + submul_1(w5p, w3p, l * 2, 8);
    divexact_byc(w5p, w5p, l * 2, std::integral_constant<T, 9>{});
    cf5 /= 9;

    //  W3 = W3 - W5
    cf3 -= cf5 + subc_n(w3p, w3p, w5p, l * 2);

    //  W1 =(W1/15 + W5)/2   Now >= 0 again.
    divexact_by15(w1p, w1p, l * 2);
    cf1 /= 15;
    if (!neg1) {
        cf1 += cf5 + addc_n(w1p, w1p, w5p, l * 2);
    } else {
        cf1 = cf5 - cf1 - subc_n(w1p, w5p, w1p, l * 2);
    }
    rshift_n(w1p, w1p, l * 2, 1, cf1);
    cf1 >>= 1;

    //  W5 = W5 - W1
    cf5 -= cf1 + subc_n(w5p, w5p, w1p, l * 2);
    if (l != maxr) {
        cf5 = w5p[l + maxr];
    }

    cf = addc_n(dst + l, dst + l, w1p, l * 2);
    cf = addc_n(dst + l * 3, dst + l * 3, w3p, l, cf);
    cf2 += addc_1(dst + l * 3, dst + l * 3, l, cf1);
    cf = addc_n(dst + l * 4, dst + l * 4, w3p + l, l, cf);
    cf3 += addc_1(dst + l * 4, dst + l * 4, l, cf2);
    cf = addc_n(dst + l * 5, dst + l * 5, w5p, l, cf);
    cf4 += addc_1(dst + l * 5, dst + l * 5, l, cf3);
    cf = addc_n(dst + l * 6, dst + l * 6, w5p + l, maxr, cf);
    cf5 += addc_1(dst + l * 6, dst + l * 6, maxr, cf4);
    cf = addc_1(dst + l * 6 + maxr, dst + l * 6 + maxr, (rn + rm) - maxr, cf5, cf);
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom53_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - 2U1 + 4U2 + 8U3 - 16U4) * (V0 - 2V1 + 4V2);
     W2 = (U0 + U1 + U2 + U3 + U4) * (V0 + V1 + V2);
     W3 = (U0 - U1 + U2 - U3 + U4) * (V0 - V1 + V2);
     W4 = (U0 + 2U1 + 4U2 + 8U3 + 16U4) * (V0 + 2V1 + 4V2);
     W5 = (16U0 + 8U1 + 4U2 + 2U3 + U4) * (4V0 + 2V1 + V2);
     W6 = U4 * V2;
    */

    /*
          dst :
          |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|--- rn+rm ---|
          W0             W2            W4            W6
          |- l --|- l --|                           |- l --|
          T0      T1                                 T2
          stk :
          |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
          W1             W3            W5
     */

    /*
     11 add/sub + 11 addlsh/rsblsh + 1 shift

     T0 = U0 + U2 + U4;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W5 = V0 + V2;
     T2 = W5 + V1; v(1)
     W5 = W5 - V1; v(-1)
     W2 = T0 * T2; f(1)
     W3 = T1 * W5; f(-1)
     W5 = (W5 + V2) << 1 - V0; v(-2)
     T2 = W5 + 4V1; v(2)
     T1 = U0 + (U2 + 4U4) << 2;
     W1 = (U1 + 4U3) << 1;
     T0 = T1 + W1; u(2)
     T1 = T1 - W1; u(-2)
     W1 = T1 * W5; f(-2)
     W4 = T0 * T2; f(2)
     T0 = (((2U0 + U1) << 1 + U2) << 1 + U3) << 1 + U4; 16 * u(1/2)
     T1 = (2V0 + V1) << 1 + V2; 4 * v(1/2)
     W5 = T0 * T1; 64 * f(1/2)
     W0 = U0 * V0; f(0)
     W6 = U4 * V2; f(inf)
    */

    WJR_ASSERT_ASSUME(4 * m + 14 <= 3 * n && 2 * n + 28 <= 5 * m);

    const size_t l = 3 * n >= 5 * m ? (n + 4) / 5 : (m + 2) / 3;
    const size_t rn = n - l * 4;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;
    const auto u4p = src0 + l * 4;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = stk + l * 4;
    const auto w6p = w0p + l * 6;

    const auto t0p = w0p;
    const auto t1p = w0p + l;
    const auto t2p = w6p;

    stk += l * 6;

    T cf = 0;
    T cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    T cft0 = 0, cft1 = 0, cft2 = 0;
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  T0 = U0 + U2 + U4;
    cft0 = addc_n(t0p, u0p, u2p, l);
    cft0 += addc_s(t0p, t0p, l, u4p, rn);

    //  T2 = U1 + U3;
    cft2 = addc_n(t2p, u1p, u3p, l);

    //  T1 = T0 - T2; u(-1)
    {
        ssize_t p = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        neg0 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft1 <= 2);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  W5 = V0 + V2;
    cf5 = addc_s(w5p, v0p, l, v2p, rm);

    //  T2 = W5 + V1; v(1)
    cft2 = cf5 + addc_n(t2p, w5p, v1p, l);
    WJR_ASSERT_ASSUME(cft2 <= 2);

    //  W5 = W5 - V1; v(-1)
    {
        ssize_t p = abs_subc_n(w5p, w5p, v1p, l, cf5, cf5, 0);
        neg1 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf5 <= 1);
    }

    //  W2 = T0 * T2; f(1)
    __mul_n<__mul_mode::toom44, T, 4, 2>(w2p, t0p, t2p, l, stk, cf2, cft0, cft2);

    //  W3 = T1 * W5; f(-1)
    neg0 ^= neg1;
    __mul_n<__mul_mode::toom44, T, 2, 1>(w3p, t1p, w5p, l, stk, cf3, cft1, cf5);

    //  W5 = (W5 + V2) << 1 - V0; v(-2)
    if (!neg1) {
        cf5 += addc_s(w5p, w5p, l, v2p, rm);
    } else {
        WJR_ASSERT_ASSUME(cf5 == 0);
        ssize_t p = abs_subc_s(w5p, w5p, l, v2p, rm);
        neg1 = p > 0;
    }

    if (!neg1) {
        cf5 += cf5 + lshift_n(w5p, w5p, l, 1);
        {
            ssize_t p = abs_subc_n(w5p, w5p, v0p, l, cf5, cf5, 0);
            neg1 = __fasts_is_negative(p);
        }
    } else {
        WJR_ASSERT_ASSUME(cf5 == 0);
        cf5 += addlsh_n(w5p, v0p, w5p, l, 1);
    }
    WJR_ASSERT_ASSUME(cf5 <= 4);

    //  T2 = W5 + 4V1; v(2)
    if (!neg1) {
        cft2 = cf5 + addlsh_n(t2p, w5p, v1p, l, 2);
    } else {
        cft2 = rsblsh_n(t2p, w5p, v1p, l, 2) - cf5;
    }
    WJR_ASSERT_ASSUME(cft2 <= 6);

    //  T1 = U0 + (U2 + 4U4) << 2;
    cft1 = addlsh_n(t1p, u2p, u4p, rn, 2);
    if (l != rn) {
        cft1 = addc_1(t1p + rn, u2p + rn, l - rn, cft1);
    }
    cft1 = cft1 * 4 + addlsh_n(t1p, u0p, t1p, l, 2);

    //  W1 = (U1 + 4U3) << 1;
    cf1 = addlsh_n(w1p, u1p, u3p, l, 2);
    cf1 += cf1 + lshift_n(w1p, w1p, l, 1);

    //  T0 = T1 + W1; u(2)
    cft0 = cft1 + cf1 + addc_n(t0p, t1p, w1p, l);
    WJR_ASSERT_ASSUME(cft0 <= 30);

    //  T1 = T1 - W1; u(-2)
    {
        ssize_t p = abs_subc_n(t1p, t1p, w1p, l, cft1, cft1, cf1);
        neg2 = __fasts_is_negative(p);
    }
    WJR_ASSERT_ASSUME(cft1 <= 20);

    //  W1 = T1 * W5; f(-2)
    neg1 ^= neg2;
    __mul_n<__mul_mode::toom44, T, in_place_max, 4>(w1p, t1p, w5p, l, stk, cf1, cft1,
                                                    cf5);

    //  W4 = T0 * T2; f(2)
    __mul_n<__mul_mode::toom44, T, in_place_max, 6>(w4p, t0p, t2p, l, stk, cf4, cft0,
                                                    cft2);

    //  T0 = (((2U0 + U1) << 1 + U2) << 1 + U3) << 1 + U4; 16 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u3p, t0p, l, 1);
    cf = addlsh_n(t0p, u4p, t0p, rn, 1);
    if (l != rn) {
        cft0 += cft0 + lshift_n(t0p + rn, t0p + rn, l - rn, 1);
        cft0 += addc_1(t0p + rn, t0p + rn, l - rn, cf);
    } else {
        cft0 += cft0 + cf;
    }
    WJR_ASSERT_ASSUME(cft0 <= 30);

    //  T1 = (2V0 + V1) << 1 + V2; 4 * v(1/2)
    cft1 = addlsh_n(t1p, v1p, v0p, l, 1);
    cf = addlsh_n(t1p, v2p, t1p, rm, 1);
    if (l != rm) {
        cft1 += cft1 + lshift_n(t1p + rm, t1p + rm, l - rm, 1);
        cft1 += addc_1(t1p + rm, t1p + rm, l - rm, cf);
    } else {
        cft1 += cft1 + cf;
    }
    WJR_ASSERT_ASSUME(cft1 <= 6);

    //  W5 = T0 * T1; 64 * f(1/2)
    __mul_n<__mul_mode::toom44>(w5p, t0p, t1p, l, stk, cf5, cft0, cft1);

    //  W0 = U0 * V0; f(0)
    __mul_n<__mul_mode::toom44>(w0p, u0p, v0p, l, stk);

    //  W6 = U4 * V2; f(inf)
    if (rn >= rm) {
        __mul_s<__mul_mode::toom44>(w6p, u4p, rn, v2p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom44>(w6p, v2p, rm, u4p, rn, stk);
    }

    toom_interpolation_7p_s(
        dst, w1p, l, rn, rm,
        toom_interpolation_7p_struct<T>{neg1, neg0, cf1, cf2, cf3, cf4, cf5});
}

template <typename T>
void toom44_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - 2U1 + 4U2 + 8U3) * (V0 - 2V1 + 4V2 - 8V3);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1 + V2 + V3);
     W3 = (U0 - U1 + U2 - U3) * (V0 - V1 + V2 - V3);
     W4 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V1 + 4V2 + 8V3);
     W5 = (8U0 + 4U1 + 2U2 + U3) * (8V0 + 4V1 + 2V2 + V3);
     W6 = U3 * V3;
    */

    /*
         dst :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|--- rn+rm ---|
         W0             W2            W4            W6
         |- l --|- l --|                           |- l --|
         T0      T1                                 T2
         stk :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
         W1             W3            W5
    */

    /*
     12 add/sub + 10 addlsh + 2shift

     T0 = U0 + U2;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W1 = V0 + V2;
     T2 = V1 + V3;
     W4 = W1 + T2; v(1)
     W1 = W1 - T2; v(-1)
     W2 = T0 * W4; f(1)
     W3 = T1 * W1; f(-1)
     T0 = U0 + 4U2;
     T2 = (U1 + 4U3) << 1;
     T1 = T0 - T2; u(-2)
     T0 = T0 + T2; u(2)
     W5 = V0 + 4V2;
     T2 = (V1 + 4V3) << 1;
     W1 = W5 + T2; v(2)
     W5 = W5 - T2; v(-2)
     W4 = T0 * W1; f(2)
     W1 = T1 * W5; f(-2)
     T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
     T1 = ((2V0 + V1) << 1 + V2) << 1 + V3; 8 * v(1/2)
     W5 = T0 * T1; 64 * f(1/2)
     W0 = U0 * V0; f(0)
     W6 = U3 * V3; f(inf)
    */

    WJR_ASSERT_ASSUME(3 * n + 21 <= 4 * m);

    const size_t l = (n + 3) / 4;
    const size_t rn = n - l * 3;
    const size_t rm = m - l * 3;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;
    const auto v3p = src1 + l * 3;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = stk + l * 4;
    const auto w6p = w0p + l * 6;

    const auto t0p = w0p;
    const auto t1p = w0p + l;
    const auto t2p = w6p;

    stk += l * 6;

    T cf = 0;
    T cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    T cft0 = 0, cft1 = 0, cft2 = 0;
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  T0 = U0 + U2;
    cft0 = addc_n(t0p, u0p, u2p, l);

    //  T2 = U1 + U3;
    cft2 = addc_s(t2p, u1p, l, u3p, rn);

    //  T1 = T0 - T2; u(-1)
    {
        ssize_t p = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        neg0 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft1 <= 1);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 3);

    //  W1 = V0 + V2;
    cf1 = addc_n(w1p, v0p, v2p, l);

    //  T2 = V1 + V3;
    cft2 = addc_s(t2p, v1p, l, v3p, rm);

    //  W4 = W1 + T2; v(1)
    cf4 = cf1 + cft2 + addc_n(w4p, w1p, t2p, l);
    WJR_ASSERT_ASSUME(cf4 <= 3);

    //  W1 = W1 - T2; v(-1)
    {
        ssize_t p = abs_subc_n(w1p, w1p, t2p, l, cf1, cf1, cft2);
        neg1 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf1 <= 1);
    }

    //  W2 = T0 * W4; f(1)
    __mul_n<__mul_mode::toom44, T, 3, 3>(w2p, t0p, w4p, l, stk, cf2, cft0, cf4);

    //  W3 = T1 * W1; f(-1)
    neg0 ^= neg1;
    __mul_n<__mul_mode::toom44, T, 1, 1>(w3p, t1p, w1p, l, stk, cf3, cft1, cf1);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  T2 = (U1 + 4U3) << 1;
    cft2 = addlsh_n(t2p, u1p, u3p, rn, 2);
    if (l != rn) {
        cft2 = addc_1(t2p + rn, u1p + rn, l - rn, cft2);
    }
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT_ASSUME(cft2 <= 9);

    //  T1 = T0 - T2; u(-2)
    {
        ssize_t p = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        neg1 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft1 <= 9);
    }

    //  T0 = T0 + T2; u(2)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  W5 = V0 + 4V2;
    cf5 = addlsh_n(w5p, v0p, v2p, l, 2);
    WJR_ASSERT_ASSUME(cf5 <= 4);

    //  T2 = (V1 + 4V3) << 1;
    cft2 = addlsh_n(t2p, v1p, v3p, rm, 2);
    if (l != rm) {
        cft2 = addc_1(t2p + rm, v1p + rm, l - rm, cft2);
    }
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT_ASSUME(cft2 <= 9);

    //  W1 = W5 + T2; v(2)
    cf1 = cf5 + cft2 + addc_n(w1p, w5p, t2p, l);
    WJR_ASSERT_ASSUME(cf1 <= 14);

    //  W5 = W5 - T2; v(-2)
    {
        ssize_t p = abs_subc_n(w5p, w5p, t2p, l, cf5, cf5, cft2);
        neg2 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf5 <= 9);
    }

    //  W4 = T0 * W1; f(2)
    __mul_n<__mul_mode::toom44>(w4p, t0p, w1p, l, stk, cf4, cft0, cf1);

    //  W1 = T1 * W5; f(-2)
    neg1 ^= neg2;
    __mul_n<__mul_mode::toom44>(w1p, t1p, w5p, l, stk, cf1, cft1, cf5);

    //  T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cf = addlsh_n(t0p, u3p, t0p, rn, 1);
    if (l != rn) {
        cft0 += cft0 + lshift_n(t0p + rn, t0p + rn, l - rn, 1);
        cft0 += addc_1(t0p + rn, t0p + rn, l - rn, cf);
    } else {
        cft0 += cft0 + cf;
    }
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  T1 = ((2V0 + V1) << 1 + V2) << 1 + V3; 8 * v(1/2)
    cft1 = addlsh_n(t1p, v1p, v0p, l, 1);
    cft1 += cft1 + addlsh_n(t1p, v2p, t1p, l, 1);
    cf = addlsh_n(t1p, v3p, t1p, rm, 1);
    if (l != rm) {
        cft1 += cft1 + lshift_n(t1p + rm, t1p + rm, l - rm, 1);
        cft1 += addc_1(t1p + rm, t1p + rm, l - rm, cf);
    } else {
        cft1 += cft1 + cf;
    }
    WJR_ASSERT_ASSUME(cft1 <= 14);

    //  W5 = T0 * T1; 64 * f(1/2)
    __mul_n<__mul_mode::toom44>(w5p, t0p, t1p, l, stk, cf5, cft0, cft1);

    //  W0 = U0 * V0; f(0)
    __mul_n<__mul_mode::toom44>(w0p, u0p, v0p, l, stk);

    //  W6 = U3 * V3; f(inf)
    if (rn >= rm) {
        __mul_s<__mul_mode::toom44>(w6p, u3p, rn, v3p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom44>(w6p, v3p, rm, u3p, rn, stk);
    }

    toom_interpolation_7p_s(
        dst, w1p, l, rn, rm,
        toom_interpolation_7p_struct<T>{neg1, neg0, cf1, cf2, cf3, cf4, cf5});
}

template <typename T>
void toom4_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);

     W0 = U0 ^ 2;
     W1 = (U0 - 2U1 + 4U2 + 8U3) ^ 2;
     W2 = (U0 + U1 + U2 + U3) ^ 2;
     W3 = (U0 - U1 + U2 - U3) ^ 2;
     W4 = (U0 + 2U1 + 4U2 + 8U3) ^ 2;
     W5 = (8U0 + 4U1 + 2U2 + U3) ^ 2;
     W6 = U3 ^ 2;
    */

    /*
         dst :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|--- rn+rm ---|
         W0             W2            W4            W6
         |- l --|- l --|                           |- l --|
         T0      T1                                 T2
         stk :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
         W1             W3            W5
    */

    /*
     12 add/sub + 10 addlsh + 2shift

     T0 = U0 + U2;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W2 = T0 ^ 2; f(1)
     W3 = T1 ^ 2; f(-1)
     T0 = U0 + 4U2;
     T2 = (U1 + 4U3) << 1;
     T1 = T0 - T2; u(-2)
     T0 = T0 + T2; u(2)
     W4 = T0 ^ 2; f(2)
     W1 = T1 ^ 2; f(-2)
     T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
     W5 = T0 ^ 2; 64 * f(1/2)
     W0 = U0 ^ 2; f(0)
     W6 = U3 ^ 2; f(inf)
    */

    WJR_ASSERT_ASSUME(21 <= n);

    const size_t l = (n + 3) / 4;
    const size_t rn = n - l * 3;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(rn * 2 >= l);

    const auto u0p = src;
    const auto u1p = src + l;
    const auto u2p = src + l * 2;
    const auto u3p = src + l * 3;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = stk + l * 4;
    const auto w6p = w0p + l * 6;

    const auto t0p = w0p;
    const auto t1p = w0p + l;
    const auto t2p = w6p;

    stk += l * 6;

    T cf = 0;
    T cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    T cft0 = 0, cft1 = 0, cft2 = 0;

    //  T0 = U0 + U2;
    cft0 = addc_n(t0p, u0p, u2p, l);

    //  T2 = U1 + U3;
    cft2 = addc_s(t2p, u1p, l, u3p, rn);

    //  T1 = T0 - T2; u(-1)
    {
        (void)abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        WJR_ASSERT_ASSUME(cft1 <= 1);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 3);

    //  W2 = T0 ^ 2; f(1)
    __sqr<__mul_mode::toom44, T, 3>(w2p, t0p, l, stk, cf2, cft0);

    //  W3 = T1 ^ 2; f(-1)
    __sqr<__mul_mode::toom44, T, 1>(w3p, t1p, l, stk, cf3, cft1);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  T2 = (U1 + 4U3) << 1;
    cft2 = addlsh_n(t2p, u1p, u3p, rn, 2);
    if (l != rn) {
        cft2 = addc_1(t2p + rn, u1p + rn, l - rn, cft2);
    }
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT_ASSUME(cft2 <= 9);

    //  T1 = T0 - T2; u(-2)
    {
        (void)abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        WJR_ASSERT_ASSUME(cft1 <= 9);
    }

    //  T0 = T0 + T2; u(2)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  W4 = T0 ^ 2; f(2)
    __sqr<__mul_mode::toom44>(w4p, t0p, l, stk, cf4, cft0);

    //  W1 = T1 * W5; f(-2)
    __sqr<__mul_mode::toom44>(w1p, t1p, l, stk, cf1, cft1);

    //  T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cf = addlsh_n(t0p, u3p, t0p, rn, 1);
    if (l != rn) {
        cft0 += cft0 + lshift_n(t0p + rn, t0p + rn, l - rn, 1);
        cft0 += addc_1(t0p + rn, t0p + rn, l - rn, cf);
    } else {
        cft0 += cft0 + cf;
    }
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  W5 = T0 ^ 2; 64 * f(1/2)
    __sqr<__mul_mode::toom44>(w5p, t0p, l, stk, cf5, cft0);

    //  W0 = U0 * V0; f(0)
    __sqr<__mul_mode::toom44>(w0p, u0p, l, stk);

    //  W6 = U3 * V3; f(inf)
    __sqr<__mul_mode::toom44>(w6p, u3p, rn, stk);

    toom_interpolation_7p_s(
        dst, w1p, l, rn, rn,
        toom_interpolation_7p_struct<T>{0, 0, cf1, cf2, cf3, cf4, cf5});
}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_DIVIDER_HPP__
#define WJR_X86_MATH_DIVIDER_HPP__

// Already included

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_DIV2BY1_ADJUST WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_DIV2BY1_ADJUST)

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_INTRINSIC_INLINE void asm_div2by1_adjust(T rax, T div, T &r8, T &rdx) {
    T r9 = r8 + div;
    asm("cmp{q %[rax], %[r8]| %[r8], %[rax]}\n\t"
        "cmovb{q %[r8], %[r9]| %[r9], %[r8]}\n\t"
        "adc{q $-1, %[rdx]| %[rdx], -1}"
        : [r9] "+r"(r9), [r8] "+r"(r8), [rdx] "+r"(rdx)
        : [rax] "r"(rax)
        : "cc", "memory");
    r8 = r9;
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_DIVIDER_HPP__
#endif

namespace wjr {

namespace math_details {

inline constexpr std::array<uint16_t, 0x100> div2by1_u64_lookup = {
    0x7fd, 0x7f5, 0x7ed, 0x7e5, 0x7dd, 0x7d5, 0x7ce, 0x7c6, 0x7bf, 0x7b7, 0x7b0, 0x7a8,
    0x7a1, 0x79a, 0x792, 0x78b, 0x784, 0x77d, 0x776, 0x76f, 0x768, 0x761, 0x75b, 0x754,
    0x74d, 0x747, 0x740, 0x739, 0x733, 0x72c, 0x726, 0x720, 0x719, 0x713, 0x70d, 0x707,
    0x700, 0x6fa, 0x6f4, 0x6ee, 0x6e8, 0x6e2, 0x6dc, 0x6d6, 0x6d1, 0x6cb, 0x6c5, 0x6bf,
    0x6ba, 0x6b4, 0x6ae, 0x6a9, 0x6a3, 0x69e, 0x698, 0x693, 0x68d, 0x688, 0x683, 0x67d,
    0x678, 0x673, 0x66e, 0x669, 0x664, 0x65e, 0x659, 0x654, 0x64f, 0x64a, 0x645, 0x640,
    0x63c, 0x637, 0x632, 0x62d, 0x628, 0x624, 0x61f, 0x61a, 0x616, 0x611, 0x60c, 0x608,
    0x603, 0x5ff, 0x5fa, 0x5f6, 0x5f1, 0x5ed, 0x5e9, 0x5e4, 0x5e0, 0x5dc, 0x5d7, 0x5d3,
    0x5cf, 0x5cb, 0x5c6, 0x5c2, 0x5be, 0x5ba, 0x5b6, 0x5b2, 0x5ae, 0x5aa, 0x5a6, 0x5a2,
    0x59e, 0x59a, 0x596, 0x592, 0x58e, 0x58a, 0x586, 0x583, 0x57f, 0x57b, 0x577, 0x574,
    0x570, 0x56c, 0x568, 0x565, 0x561, 0x55e, 0x55a, 0x556, 0x553, 0x54f, 0x54c, 0x548,
    0x545, 0x541, 0x53e, 0x53a, 0x537, 0x534, 0x530, 0x52d, 0x52a, 0x526, 0x523, 0x520,
    0x51c, 0x519, 0x516, 0x513, 0x50f, 0x50c, 0x509, 0x506, 0x503, 0x500, 0x4fc, 0x4f9,
    0x4f6, 0x4f3, 0x4f0, 0x4ed, 0x4ea, 0x4e7, 0x4e4, 0x4e1, 0x4de, 0x4db, 0x4d8, 0x4d5,
    0x4d2, 0x4cf, 0x4cc, 0x4ca, 0x4c7, 0x4c4, 0x4c1, 0x4be, 0x4bb, 0x4b9, 0x4b6, 0x4b3,
    0x4b0, 0x4ad, 0x4ab, 0x4a8, 0x4a5, 0x4a3, 0x4a0, 0x49d, 0x49b, 0x498, 0x495, 0x493,
    0x490, 0x48d, 0x48b, 0x488, 0x486, 0x483, 0x481, 0x47e, 0x47c, 0x479, 0x477, 0x474,
    0x472, 0x46f, 0x46d, 0x46a, 0x468, 0x465, 0x463, 0x461, 0x45e, 0x45c, 0x459, 0x457,
    0x455, 0x452, 0x450, 0x44e, 0x44b, 0x449, 0x447, 0x444, 0x442, 0x440, 0x43e, 0x43b,
    0x439, 0x437, 0x435, 0x432, 0x430, 0x42e, 0x42c, 0x42a, 0x428, 0x425, 0x423, 0x421,
    0x41f, 0x41d, 0x41b, 0x419, 0x417, 0x414, 0x412, 0x410, 0x40e, 0x40c, 0x40a, 0x408,
    0x406, 0x404, 0x402, 0x400};

// invert of (2 * i + 1) mod 256
inline constexpr std::array<uint64_t, 0x80> divexact1_lookup = {
    0x01, 0xAB, 0xCD, 0xB7, 0x39, 0xA3, 0xC5, 0xEF, 0xF1, 0x1B, 0x3D, 0xA7, 0x29,
    0x13, 0x35, 0xDF, 0xE1, 0x8B, 0xAD, 0x97, 0x19, 0x83, 0xA5, 0xCF, 0xD1, 0xFB,
    0x1D, 0x87, 0x09, 0xF3, 0x15, 0xBF, 0xC1, 0x6B, 0x8D, 0x77, 0xF9, 0x63, 0x85,
    0xAF, 0xB1, 0xDB, 0xFD, 0x67, 0xE9, 0xD3, 0xF5, 0x9F, 0xA1, 0x4B, 0x6D, 0x57,
    0xD9, 0x43, 0x65, 0x8F, 0x91, 0xBB, 0xDD, 0x47, 0xC9, 0xB3, 0xD5, 0x7F, 0x81,
    0x2B, 0x4D, 0x37, 0xB9, 0x23, 0x45, 0x6F, 0x71, 0x9B, 0xBD, 0x27, 0xA9, 0x93,
    0xB5, 0x5F, 0x61, 0x0B, 0x2D, 0x17, 0x99, 0x03, 0x25, 0x4F, 0x51, 0x7B, 0x9D,
    0x07, 0x89, 0x73, 0x95, 0x3F, 0x41, 0xEB, 0x0D, 0xF7, 0x79, 0xE3, 0x05, 0x2F,
    0x31, 0x5B, 0x7D, 0xE7, 0x69, 0x53, 0x75, 0x1F, 0x21, 0xCB, 0xED, 0xD7, 0x59,
    0xC3, 0xE5, 0x0F, 0x11, 0x3B, 0x5D, 0xC7, 0x49, 0x33, 0x55, 0xFF};

} // namespace math_details

template <typename T>
class div2by1_divider_noshift {
    static_assert(std::is_same_v<T, uint64_t>, "Currently only support uint64_t");

public:
    div2by1_divider_noshift() = default;
    div2by1_divider_noshift(const div2by1_divider_noshift &) = default;
    div2by1_divider_noshift &operator=(const div2by1_divider_noshift &) = default;
    ~div2by1_divider_noshift() = default;

    explicit div2by1_divider_noshift(T divisor) : m_divisor(divisor) {
        m_value = reciprocal(divisor);
    }

    constexpr div2by1_divider_noshift(T divisor, T value)
        : m_divisor(divisor), m_value(value) {}

    constexpr T get_divisor() const { return m_divisor; }
    constexpr T get_value() const { return m_value; }

    constexpr bool is_zero_or_single_bit() const { return m_divisor == (1ull << 63); }

    WJR_INTRINSIC_CONSTEXPR20 T divide(T lo, T &hi) const {
        return divide(m_divisor, m_value, lo, hi);
    }

    WJR_INTRINSIC_CONSTEXPR20 static T divide(T divisor, T value, T lo, T &hi) {
        WJR_ASSERT_ASSUME(__has_high_bit(divisor));

        if (WJR_BUILTIN_CONSTANT_P(lo == 0) && lo == 0) {
            return divide_lo0(divisor, value, lo, hi);
        }

        return basic_divide(divisor, value, lo, hi);
    }

    WJR_CONST WJR_CONSTEXPR_E static T reciprocal(T d);

private:
    WJR_INTRINSIC_CONSTEXPR static void fallback_div2by1_adjust(T rax, T div, T &r8,
                                                                T &rdx) {
        T r9 = r8 + div;
        bool f = r8 < rax;
        r8 = f ? r8 : r9;
        rdx += -1 + f;
    }

    // see fallback_div2by1_adjust
    WJR_INTRINSIC_CONSTEXPR20 static void div2by1_adjust(T rax, T div, T &r8, T &rdx) {
#if WJR_HAS_BUILTIN(ASM_DIV2BY1_ADJUST)
        if (is_constant_evaluated()) {
            return fallback_div2by1_adjust(rax, div, r8, rdx);
        }

        return asm_div2by1_adjust(rax, div, r8, rdx);
#else
        return fallback_div2by1_adjust(rax, div, r8, rdx);
#endif
    }

    WJR_INTRINSIC_CONSTEXPR20 static T basic_divide(T divisor, T value, T lo, T &hi) {
        T hi1 = hi + 1;

        T rax, rdx;

        rax = mul(hi, value, rdx);
        __add_128(rax, rdx, rax, rdx, lo, hi1);

        lo -= mullo(rdx, divisor);

        div2by1_adjust(rax, divisor, lo, rdx);

        if (WJR_UNLIKELY(lo >= divisor)) {
            WJR_FORCE_BRANCH_BARRIER();
            lo -= divisor;
            ++rdx;
        }

        hi = lo;
        return rdx;
    }

    WJR_INTRINSIC_CONSTEXPR20 static T divide_lo0(T divisor, T value, T lo, T &hi) {
        T hi1 = hi + 1;
        T rax, rdx;

        rax = mul(hi, value, rdx);
        rdx += hi1;

        lo -= mullo(rdx, divisor);

        div2by1_adjust(rax, divisor, lo, rdx);

        hi = lo;
        return rdx;
    }

protected:
    T m_divisor;
    T m_value;
};

template <typename T>
WJR_CONST WJR_CONSTEXPR_E T div2by1_divider_noshift<T>::reciprocal(T d) {
    WJR_ASSERT_ASSUME(__has_high_bit(d));

    uint64_t d40 = 0, d63 = 0;
    uint32_t v0 = 0;
    uint64_t v1 = 0, v2 = 0, v3 = 0, v4 = 0;
    uint64_t t0 = 0, t1 = 0;

    // 40 bit
    d40 = (d >> 24) + 1;
    // 63 bit
    d63 = (d + 1) >> 1;
    // 11 bit
    v0 = math_details::div2by1_u64_lookup[((d >> 55) - 0x100)];
    // 22 bit
    v1 = (v0 << 11) - (mullo<uint64_t>(mullo<uint32_t>(v0, v0), d40) >> 40) - 1;

    t1 = mulhi<uint64_t>(v1 << 17, (1ull << 60) - mullo<uint64_t>(v1, d40));
    // 35 bit
    v2 = (v1 << 13) + t1;

    t0 = 0 - mul<uint64_t>(v2, d63, t1);
    if (d & 1) {
        t0 += v2 >> 1;
    }

    v3 = (v2 << 31) + (mulhi<uint64_t>(t0, v2) >> 1);

    v1 = v3 + 1;

    if (WJR_UNLIKELY(v1 == 0)) {
        v4 = ~(d * 2);
    } else {
        v4 = v3 - mulhi<uint64_t>(v1, d) - d;
    }

    return v4;
}

template <typename T>
class div2by1_divider : public div2by1_divider_noshift<T> {
private:
    using Mybase = div2by1_divider_noshift<T>;
    // disable member divide function of Mybase
    using Mybase::divide;
    using Mybase::m_divisor;
    using Mybase::m_value;

public:
    static_assert(std::is_same_v<T, uint64_t>, "Currently only support uint64_t");

    div2by1_divider() = default;
    div2by1_divider(const div2by1_divider &) = default;
    div2by1_divider &operator=(const div2by1_divider &) = default;
    ~div2by1_divider() = default;

    constexpr explicit div2by1_divider(const Mybase &base) : Mybase(base), m_shift(0) {}

    WJR_INTRINSIC_CONSTEXPR_E explicit div2by1_divider(T divisor) {
        m_divisor = divisor;
        initialize();
    }

    constexpr div2by1_divider(T divisor, T value, unsigned int shift)
        : Mybase(divisor, value), m_shift(shift) {}

    constexpr unsigned int get_shift() const { return m_shift; }

    constexpr const Mybase &get_base() const { return *this; }

    // enable static divide function of Mybase
    // This function needs to ensure that lo and hi have been adjusted
    WJR_INTRINSIC_CONSTEXPR20 static T divide(T divisor, T value, T lo, T &hi) {
        return Mybase::divide(divisor, value, lo, hi);
    }

private:
    // make sure m_shift/one_single_bit(divisor) can be inlined
    WJR_INTRINSIC_CONSTEXPR_E void initialize() {
        if (WJR_UNLIKELY(!__has_high_bit(m_divisor))) {
            m_shift = clz(m_divisor);
            m_divisor <<= m_shift;

            WJR_ASSUME(m_shift != 0);
        } else {
            WJR_ASSUME(m_shift == 0);
        }

        WJR_ASSUME(__has_high_bit(m_divisor));

        if (WJR_UNLIKELY(m_divisor == 1ull << 63)) {
            m_value = -1;
            return;
        }

        m_value = Mybase::reciprocal(m_divisor);
    }

    unsigned int m_shift = 0;
};

template <typename T>
class div3by2_divider_noshift {
public:
    static_assert(std::is_same_v<T, uint64_t>, "");

    div3by2_divider_noshift() = default;
    div3by2_divider_noshift(const div3by2_divider_noshift &) = default;
    div3by2_divider_noshift &operator=(const div3by2_divider_noshift &) = default;
    ~div3by2_divider_noshift() = default;

    WJR_INTRINSIC_CONSTEXPR_E div3by2_divider_noshift(T d0, T d1)
        : m_divisor0(d0), m_divisor1(d1) {
        m_value = reciprocal(d0, d1);
    }

    WJR_INTRINSIC_CONSTEXPR div3by2_divider_noshift(T d0, T d1, T value)
        : m_divisor0(d0), m_divisor1(d1), m_value(value) {}

    constexpr T get_divisor0() const { return m_divisor0; }
    constexpr T get_divisor1() const { return m_divisor1; }
    constexpr T get_value() const { return m_value; }

    WJR_INTRINSIC_CONSTEXPR20 T divide(T u0, T &u1, T &u2) const {
        return divide(m_divisor0, m_divisor1, m_value, u0, u1, u2);
    }

    WJR_INTRINSIC_CONSTEXPR20 static T divide(T divisor0, T divisor1, T value, T u0,
                                              T &u1, T &u2);

    WJR_CONST WJR_CONSTEXPR_E static T reciprocal(T d0, T d1);

protected:
    T m_divisor0 = 0;
    T m_divisor1 = 0;
    T m_value = 0;
};

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 T div3by2_divider_noshift<T>::divide(T divisor0, T divisor1,
                                                               T value, T u0, T &u1,
                                                               T &u2) {
    WJR_ASSERT_ASSUME(__has_high_bit(divisor1));

    T q1, q0;
    q0 = mul<T>(value, u2, q1);
    __add_128(q0, q1, q0, q1, u1, u2);

    T r1, r0;
    r1 = u1 - mullo<T>(q1, divisor1);
    T t1;
    r0 = mul<T>(divisor0, q1, t1);

    __sub_128(r0, r1, u0, r1, r0, t1);
    __sub_128(r0, r1, r0, r1, divisor0, divisor1);
    ++q1;

    if (r1 >= q0) {
        --q1;
        __add_128(r0, r1, r0, r1, divisor0, divisor1);
    }

    if (WJR_UNLIKELY(__less_equal_128(divisor0, divisor1, r0, r1))) {
        ++q1;
        __sub_128(r0, r1, r0, r1, divisor0, divisor1);
    }

    u1 = r0;
    u2 = r1;
    return q1;
}

template <typename T>
WJR_CONST WJR_CONSTEXPR_E T div3by2_divider_noshift<T>::reciprocal(T d0, T d1) {
    WJR_ASSERT_ASSUME(__has_high_bit(d1));

    T v = div2by1_divider<T>::reciprocal(d1);
    T p = mullo<T>(d1, v);
    p += d0;
    if (p < d0) {
        --v;
        if (p >= d1) {
            --v;
            p -= d1;
        }
        p -= d1;
    }

    T t0 = 0, t1 = 0;
    t0 = mul<T>(d0, v, t1);
    p += t1;
    if (p < t1) {
        --v;
        if (__less_equal_128(d0, d1, t0, p)) {
            --v;
        }
    }

    return v;
}

template <typename T>
class div3by2_divider : public div3by2_divider_noshift<T> {
    using Mybase = div3by2_divider_noshift<T>;
    using Mybase::divide;
    using Mybase::m_divisor0;
    using Mybase::m_divisor1;
    using Mybase::m_value;

public:
    static_assert(std::is_same_v<T, uint64_t>, "");

    div3by2_divider() = default;
    div3by2_divider(const div3by2_divider &) = default;
    div3by2_divider &operator=(const div3by2_divider &) = default;
    ~div3by2_divider() = default;

    constexpr explicit div3by2_divider(const Mybase &base) : Mybase(base), m_shift(0) {}

    WJR_INTRINSIC_CONSTEXPR_E div3by2_divider(T d0, T d1) {
        m_divisor0 = d0;
        m_divisor1 = d1;
        initialize();
    }

    WJR_INTRINSIC_CONSTEXPR div3by2_divider(T d0, T d1, T value, unsigned int shift)
        : Mybase(d0, d1, value), m_shift(shift) {}

    constexpr unsigned int get_shift() const { return m_shift; }

    constexpr const Mybase &get_base() const { return *this; }

    WJR_INTRINSIC_CONSTEXPR20 static T divide(T divisor0, T divisor1, T value, T u0,
                                              T &u1, T &u2) {
        return Mybase::divide(divisor0, divisor1, value, u0, u1, u2);
    }

private:
    WJR_INTRINSIC_CONSTEXPR_E void initialize() {
        if (WJR_UNLIKELY(!__has_high_bit(m_divisor1))) {
            m_shift = clz(m_divisor1);
            m_divisor1 = shld(m_divisor1, m_divisor0, m_shift);
            m_divisor0 <<= m_shift;

            WJR_ASSUME(m_shift != 0);
        } else {
            WJR_ASSUME(m_shift == 0);
        }

        WJR_ASSUME(__has_high_bit(m_divisor1));

        m_value = Mybase::reciprocal(m_divisor0, m_divisor1);
    }

    unsigned int m_shift = 0;
};

// divexact1_divider
// m_value is invert of m_divisor mod 2 ^ N
// m_divisor is odd
template <typename T>
class divexact1_divider {
public:
    static_assert(std::is_same_v<T, uint64_t>, "Currently only support uint64_t");

    divexact1_divider() = default;
    divexact1_divider(const divexact1_divider &) = default;
    divexact1_divider &operator=(const divexact1_divider &) = default;
    ~divexact1_divider() = default;

    WJR_INTRINSIC_CONSTEXPR_E explicit divexact1_divider(T divisor) : m_divisor(divisor) {
        initialize();
    }

    constexpr divexact1_divider(T divisor, T value, unsigned int shift)
        : m_divisor(divisor), m_value(value), m_shift(shift) {}

    constexpr T get_divisor() const { return m_divisor; }
    constexpr T get_value() const { return m_value; }
    constexpr unsigned int get_shift() const { return m_shift; }

    constexpr bool is_zero_or_single_bit() const { return m_divisor == 1; }

    WJR_CONST constexpr static T reciprocal(T divisor) {
        T inv = math_details::divexact1_lookup[(divisor & 0xFF) >> 1];
        inv = inv * (2 - inv * divisor);
        inv = inv * (2 - inv * divisor);
        inv = inv * (2 - inv * divisor);
        return inv;
    }

private:
    WJR_INTRINSIC_CONSTEXPR_E void initialize() {

        if (WJR_UNLIKELY(!(m_divisor & 1))) {
            m_shift = ctz(m_divisor);
            m_divisor >>= m_shift;

            WJR_ASSUME(m_shift != 0);
        } else {
            WJR_ASSUME(m_shift == 0);
        }

        WJR_ASSUME((m_divisor & 1) != 0);

        m_value = reciprocal(m_divisor);
    }

    T m_divisor = 0;
    T m_value = 0;
    unsigned int m_shift = 0;
};

} // namespace wjr

#endif // WJR_MATH_DIVIDER_HPP__

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_DIV_HPP__
#define WJR_X86_MATH_DIV_HPP__

// Already included

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_DIVEXACT_DBM1C WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_DIVEXACT_DBM1C)

// TODO : optimize pipeline
inline uint64_t asm_divexact_dbm1c(uint64_t *dst, const uint64_t *src, size_t n,
                                   uint64_t bd, uint64_t h) {
    uint64_t r8 = h, r9 = n, r10, r11 = static_cast<uint32_t>(n);

    src += r9;
    dst += r9;
    r9 = -r9;

    asm volatile(
        "and{l $3, %k[r11]| %k[r11], 3}\n\t"
        "je .Lb0%=\n\t"
        "lea{q -4(%[r9], %[r11], 1), %[r9]| %[r9], [%[r9] + %[r11] * 1 - 4]}\n\t"
        "cmp{l $2, %k[r11]| %k[r11], 2}\n\t"
        "jb .Lb1%=\n\t"
        "je .Lb2%=\n\t"
        "jmp .Lb3%=\n\t"

        ".Lloop%=:\n\t"

        ".Lb0%=:\n\t"
        "mulx{q (%[src], %[r9], 8), %[r10], %[r11]| %[r11], %[r10], [%[src] + %[r9] * 8]}\n\t"
        "sub{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
        "mov{q %[r8], (%[dst], %[r9], 8)| [%[dst] + %[r9] * 8], %[r8]}\n\t"
        "sbb{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb3%=:\n\t"
        "mulx{q 8(%[src], %[r9], 8), %[r10], %[r11]| %[r11], %[r10], [%[src] + %[r9] * 8 + 8]}\n\t"
        "sub{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
        "mov{q %[r8], 8(%[dst], %[r9], 8)| [%[dst] + %[r9] * 8 + 8], %[r8]}\n\t"
        "sbb{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb2%=:\n\t"
        "mulx{q 16(%[src], %[r9], 8), %[r10], %[r11]| %[r11], %[r10], [%[src] + %[r9] * 8 + 16]}\n\t"
        "sub{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
        "mov{q %[r8], 16(%[dst], %[r9], 8)| [%[dst] + %[r9] * 8 + 16], %[r8]}\n\t"
        "sbb{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb1%=:\n\t"
        "mulx{q 24(%[src], %[r9], 8), %[r10], %[r11]| %[r11], %[r10], [%[src] + %[r9] * 8 + 24]}\n\t"
        "sub{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
        "mov{q %[r8], 24(%[dst], %[r9], 8)| [%[dst] + %[r9] * 8 + 24], %[r8]}\n\t"
        "sbb{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        "add $4, %[r9]\n\t"
        "jne .Lloop%=\n\t"

        : [dst] "+&r"(dst), [src] "+&r"(src), [r8] "+&r"(r8), [r9] "+&r"(r9),
          [r10] "=&r"(r10), [r11] "+&r"(r11)
        : "d"(bd)
        : "cc", "memory");

    return r8;
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_DIV_HPP__
#endif

namespace wjr {

/*
 TODO :
 1. __div_constant_128
 2. __mod_constant_128
 3. __div_qr_constant_128
 4. optimize constant divisor of div_qr_1
 1, 2, 3, 4: constant numbers that can be divisible by (uint64_t)(-1),
*/

inline uint64_t
div128by64to64_noshift(uint64_t &rem, uint64_t lo, uint64_t hi,
                       const wjr::div2by1_divider_noshift<uint64_t> &divider) {
    uint64_t result = divider.divide(lo, hi);
    rem = hi;
    return result;
}

inline uint64_t div128by64to64_shift(uint64_t &rem, uint64_t lo, uint64_t hi,
                                     const wjr::div2by1_divider<uint64_t> &divider) {
    auto shift = divider.get_shift();
    hi = shld(hi, lo, shift);
    lo <<= shift;
    uint64_t result = divider.get_base().divide(lo, hi);
    rem = hi >> shift;
    return result;
}

inline uint64_t div128by64to64_impl(uint64_t &rem, uint64_t lo, uint64_t hi,
                                    const wjr::div2by1_divider<uint64_t> &divider) {
    if (divider.get_shift() == 0) {
        return div128by64to64_noshift(rem, lo, hi, divider);
    }

    return div128by64to64_shift(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                               const div2by1_divider<uint64_t> &divider) {
    return div128by64to64_impl(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi, uint64_t div) {
    return div128by64to64_impl(rem, lo, hi, wjr::div2by1_divider<uint64_t>(div));
}

inline std::pair<uint64_t, uint64_t>
div128by64to128_noshift(uint64_t &rem, uint64_t lo, uint64_t hi,
                        const div2by1_divider_noshift<uint64_t> &divider) {
    auto divisor = divider.get_divisor();
    uint64_t q0, q1 = 0;

    if (hi >= divisor) {
        q1 = 1;
        hi -= divisor;
    }

    q0 = divider.divide(lo, hi);
    rem = hi;
    return std::make_pair(q0, q1);
}

inline std::pair<uint64_t, uint64_t>
div128by64to128_shift(uint64_t &rem, uint64_t lo, uint64_t hi,
                      const div2by1_divider<uint64_t> &divider) {
    auto shift = divider.get_shift();
    uint64_t u0, u1, u2;
    uint64_t q0, q1;

    u2 = hi >> (64 - shift);
    u1 = shld(hi, lo, shift);
    u0 = lo << shift;

    const auto &div = divider.get_base();
    q1 = div.divide(u1, u2);
    q0 = div.divide(u0, u2);

    rem = u2 >> shift;
    return std::make_pair(q0, q1);
}

inline std::pair<uint64_t, uint64_t>
div128by64to128_impl(uint64_t &rem, uint64_t lo, uint64_t hi,
                     const div2by1_divider<uint64_t> &divider) {
    if (divider.get_shift() == 0) {
        return div128by64to128_noshift(rem, lo, hi, divider);
    }

    return div128by64to128_shift(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline std::pair<uint64_t, uint64_t>
div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                const div2by1_divider<uint64_t> &divider) {
    return div128by64to128_impl(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline std::pair<uint64_t, uint64_t> div128by64to128(uint64_t &rem, uint64_t lo,
                                                     uint64_t hi, uint64_t div) {
    return div128by64to128_impl(rem, lo, hi, div2by1_divider<uint64_t>(div));
}

template <>
class div1by1_divider<uint64_t> {
public:
private:
};

// reference : https://ieeexplore.ieee.org/document/5487506
template <typename T>
WJR_CONSTEXPR20 T div_qr_1_noshift(T *dst, T &rem, const T *src, size_t n,
                                   const div2by1_divider_noshift<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

    T divisor = div.get_divisor();
    T value = div.get_value();

    T qh = 0;
    T lo, hi;

    hi = src[n - 1];

    if (hi >= divisor) {
        hi -= divisor;
        qh = 1;
    }

    do {
        if (WJR_UNLIKELY(n == 1)) {
            break;
        }

        --n;

        do {
            lo = src[n - 1];
            dst[n - 1] = div.divide(divisor, value, lo, hi);
            --n;
        } while (WJR_LIKELY(n != 0));

    } while (0);

    rem = hi;
    return qh;
}

template <typename T>
WJR_CONSTEXPR20 T div_qr_1_shift(T *dst, T &rem, const T *src, size_t n,
                                 const div2by1_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(div.get_shift() != 0);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

    T divisor = div.get_divisor();
    T value = div.get_value();
    unsigned int shift = div.get_shift();

    T qh;
    T lo, hi;

    T rbp = src[n - 1];
    --n;
    hi = rbp >> (64 - shift);

    do {
        if (WJR_UNLIKELY(n == 0)) {
            qh = div.divide(divisor, value, rbp << shift, hi);
            break;
        }

        lo = src[n - 1];
        qh = div.divide(divisor, value, shld(rbp, lo, shift), hi);
        rbp = lo;
        --n;

        if (WJR_LIKELY(n != 0)) {
            do {
                lo = src[n - 1];
                dst[n] = div.divide(divisor, value, shld(rbp, lo, shift), hi);
                rbp = lo;
                --n;
            } while (WJR_LIKELY(n != 0));
        }

        dst[0] = div.divide(divisor, value, rbp << shift, hi);
    } while (0);

    rem = hi >> shift;
    return qh;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 T div_qr_1_impl(T *dst, T &rem, const T *src, size_t n,
                                          const div2by1_divider<T> &div) {
    if (div.get_shift() == 0) {
        return div_qr_1_noshift(dst, rem, src, n, div);
    }

    return div_qr_1_shift(dst, rem, src, n, div);
}

// return high quotient limb
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        const div2by1_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(div.is_zero_or_single_bit())) {
        unsigned int c = 63 - div.get_shift();
        rem = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    if (WJR_BUILTIN_CONSTANT_P(n == 2) && n == 2) {
        auto [ax, dx] = div128by64to128(rem, src[0], src[1], div);
        dst[0] = ax;
        dst[1] = dx;
        return;
    }

    dst[n - 1] = div_qr_1_impl(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        type_identity_t<T> div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(is_zero_or_single_bit(div))) {
        unsigned int c = ctz(div);
        rem = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    if (WJR_UNLIKELY(n == 1)) {
        if (__has_high_bit(div)) {
            T tmp = src[0];
            if (tmp >= div) {
                rem = tmp - div;
                dst[0] = 1;
                return;
            }
            rem = tmp;
            dst[0] = 0;
            return;
        }

        T tmp = src[0];
        dst[0] = tmp / div;
        rem = tmp % div;
        return;
    }

    if (WJR_BUILTIN_CONSTANT_P(n == 2) && n == 2) {
        auto [ax, dx] = div128by64to128(rem, src[0], src[1], div);
        dst[0] = ax;
        dst[1] = dx;
        return;
    }

    dst[n - 1] = div_qr_1_impl(dst, rem, src, n, div2by1_divider<T>(div));
}

template <typename T>
WJR_CONSTEXPR20 T div_qr_2_noshift(T *dst, T *rem, const T *src, size_t n,
                                   const div3by2_divider_noshift<T> &div) {
    WJR_ASSERT_ASSUME(n >= 2);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n, rem, n));

    T divisor0 = div.get_divisor0();
    T divisor1 = div.get_divisor1();
    T value = div.get_value();

    T qh = 0;
    T u0, u1, u2;

    u2 = src[n - 1];
    u1 = src[n - 2];

    if (__less_equal_128(divisor0, divisor1, u1, u2)) {
        __sub_128(u1, u2, u1, u2, divisor0, divisor1);
        qh = 1;
    }

    do {
        if (WJR_UNLIKELY(n == 2)) {
            break;
        }

        n -= 2;

        do {
            u0 = src[n - 1];
            dst[n - 1] = div.divide(divisor0, divisor1, value, u0, u1, u2);
            --n;
        } while (WJR_LIKELY(n != 0));

    } while (0);

    rem[0] = u1;
    rem[1] = u2;
    return qh;
}

template <typename T>
WJR_CONSTEXPR20 T div_qr_2_shift(T *dst, T *rem, const T *src, size_t n,
                                 const div3by2_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 2);
    WJR_ASSERT(div.get_shift() != 0);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n, rem, n));

    T divisor0 = div.get_divisor0();
    T divisor1 = div.get_divisor1();
    T value = div.get_value();
    unsigned int shift = div.get_shift();

    T qh;
    T u0, u1, u2;
    T rbp;

    rbp = src[n - 2];
    u2 = src[n - 1];
    u1 = shld(u2, rbp, shift);
    u2 >>= (64 - shift);

    n -= 2;

    do {
        if (WJR_UNLIKELY(n == 0)) {
            qh = div.divide(divisor0, divisor1, value, rbp << shift, u1, u2);
            break;
        }

        u0 = src[n - 1];
        qh = div.divide(divisor0, divisor1, value, shld(rbp, u0, shift), u1, u2);
        rbp = u0;
        --n;

        if (WJR_LIKELY(n != 0)) {
            do {
                u0 = src[n - 1];
                dst[n] =
                    div.divide(divisor0, divisor1, value, shld(rbp, u0, shift), u1, u2);
                rbp = u0;
                --n;
            } while (WJR_LIKELY(n != 0));
        }

        dst[0] = div.divide(divisor0, divisor1, value, rbp << shift, u1, u2);
    } while (0);

    rem[0] = shrd(u1, u2, shift);
    rem[1] = u2 >> shift;
    return qh;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 T div_qr_2_impl(T *dst, T *rem, const T *src, size_t n,
                                          const div3by2_divider<T> &div) {
    if (div.get_shift() == 0) {
        return div_qr_2_noshift(dst, rem, src, n, div);
    }

    return div_qr_2_shift(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                        const div3by2_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 2);

    dst[n - 2] = div_qr_2_impl(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                        const T *div) {
    WJR_ASSERT_ASSUME(n >= 2);

    dst[n - 2] = div_qr_2_impl(dst, rem, src, n, div3by2_divider<T>(div[0], div[1]));
}

// reference : GMP
// return qh;
template <typename T>
T sb_div_qr_s(T *dst, T *src, size_t n, const T *div, size_t m, T dinv) {
    using divider = div3by2_divider<T>;
    constexpr T mask = in_place_max;

    WJR_ASSERT_ASSUME(m > 2);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT(__has_high_bit(div[m - 1]));

    T qh;
    T n1, n0;
    T d1, d0;
    T cy;
    T q;

    src += n;

    qh = reverse_compare_n(src - m, div, m) >= 0;
    if (qh != 0) {
        (void)subc_n(src - m, src - m, div, m);
    }

    dst += n - m;

    m -= 2;
    d1 = div[m + 1];
    d0 = div[m + 0];

    src -= 2;

    n1 = src[1];

    for (size_t i = n - (m + 2); i > 0; i--) {
        src--;
        if (WJR_UNLIKELY(n1 == d1) && src[1] == d0) {
            q = mask;
            submul_1(src - m, div, m + 2, q);
            n1 = src[1];
        } else {
            n0 = src[1];
            q = divider::divide(d0, d1, dinv, src[0], n0, n1);

            cy = submul_1(src - m, div, m, q);
            cy = __subc_cc_128(n0, n1, n0, n1, cy, 0, 0);
            src[0] = n0;

            if (WJR_UNLIKELY(cy != 0)) {
                n1 += d1 + addc_n(src - m, src - m, div, m + 1);
                q--;
            }
        }

        *--dst = q;
    }

    src[1] = n1;

    return qh;
}

extern template uint64_t sb_div_qr_s<uint64_t>(uint64_t *dst, uint64_t *src, size_t n,
                                               const uint64_t *div, size_t m,
                                               uint64_t dinv);

inline constexpr size_t dc_div_qr_threshold = WJR_DC_DIV_QR_THRESHOLD;

template <typename T>
T dc_div4by2_qr(T *dst, T *src, const T *div, size_t m, T dinv, T *stk) {
    size_t lo, hi;
    T cy, qh, ql;

    lo = m >> 1; /* floor(n/2) */
    hi = m - lo; /* ceil(n/2) */

    if (hi < dc_div_qr_threshold) {
        qh = sb_div_qr_s(dst + lo, src + 2 * lo, 2 * hi, div + lo, hi, dinv);
    } else {
        qh = dc_div4by2_qr(dst + lo, src + 2 * lo, div + lo, hi, dinv, stk);
    }

    mul_s(stk, dst + lo, hi, div, lo);

    cy = subc_n(src + lo, src + lo, stk, m);
    if (qh != 0) {
        cy += subc_n(src + m, src + m, div, lo);
    }

    while (cy != 0) {
        qh -= subc_1(dst + lo, dst + lo, hi, 1);
        cy -= addc_n(src + lo, src + lo, div, m);
    }

    if (lo < dc_div_qr_threshold) {
        ql = sb_div_qr_s(dst, src + hi, 2 * lo, div + hi, lo, dinv);
    } else {
        ql = dc_div4by2_qr(dst, src + hi, div + hi, lo, dinv, stk);
    }

    mul_s(stk, div, hi, dst, lo);

    cy = subc_n(src, src, stk, m);
    if (ql != 0) {
        cy += subc_n(src + lo, src + lo, div, hi);
    }

    while (cy != 0) {
        subc_1(dst, dst, lo, 1);
        cy -= addc_n(src, src, div, m);
    }

    return qh;
}

template <typename T>
T dc_div_qr_s(T *dst, T *src, size_t n, const T *div, size_t m, T dinv) {
    WJR_ASSERT(m >= 6);
    WJR_ASSERT(n - m >= 3);
    WJR_ASSERT(__has_high_bit(div[m - 1]));

    size_t qn;
    T qh, cy;
    T *tp;

    unique_stack_allocator stkal(math_details::stack_alloc);
    tp = static_cast<T *>(stkal.allocate(sizeof(T) * m));

    qn = n - m;
    dst += qn;
    src += n;
    div += m;

    if (qn > m) {
        /* Reduce qn mod m without division, optimizing small operations.  */
        if (qn >= 8 * m) {
            qn %= m;
            if (qn == 0) {
                qn = m;
            }
        } else {
            do {
                qn -= m;
            } while (qn > m);
        }

        dst -= qn; /* point at low limb of next quotient block */
        src -= qn; /* point in the middle of partial remainder */

        /* Perform the typically smaller block first.  */
        if (qn == 1) {
            T q, n1, n0, d1, d0;

            /* Handle qh up front, for simplicity. */
            qh = reverse_compare_n(src - m + 1, div - m, m) >= 0;
            if (qh) {
                (void)subc_n(src - m + 1, src - m + 1, div - m, m);
            }

            /* A single iteration of schoolbook: One 3/2 division,
               followed by the bignum update and adjustment. */
            n1 = src[0];
            n0 = src[-1];
            d1 = div[-1];
            d0 = div[-2];

            WJR_ASSERT(n1 < d1 || (n1 == d1 && n0 <= d0));

            if (WJR_UNLIKELY(n1 == d1) && n0 == d0) {
                q = in_place_max;
                cy = submul_1(src - m, div - m, m, q);
            } else {
                q = wjr::div3by2_divider<T>::divide(d0, d1, dinv, src[-2], n0, n1);

                if (m > 2) {
                    T cy;
                    cy = submul_1(src - m, div - m, m - 2, q);
                    cy = __subc_cc_128(n0, n1, n0, n1, cy, 0, 0);
                    src[-2] = n0;

                    if (WJR_UNLIKELY(cy != 0)) {
                        n1 += d1 + addc_n(src - m, src - m, div - m, m - 1);
                        qh -= (q == 0);
                        --q;
                    }
                } else {
                    src[-2] = n0;
                }

                src[-1] = n1;
            }
            dst[0] = q;
        } else {
            /* Do a 2qn / qn division */
            if (qn == 2) {
                qh = div_qr_2_noshift(dst, src - 2, src - 2, 4,
                                      div3by2_divider_noshift<T>(div[-2], div[-1], dinv));
            } else if (qn < dc_div_qr_threshold) {
                qh = sb_div_qr_s(dst, src - qn, 2 * qn, div - qn, qn, dinv);
            } else {
                qh = dc_div4by2_qr(dst, src - qn, div - qn, qn, dinv, tp);
            }

            if (qn != m) {
                if (qn > m - qn) {
                    mul_s(tp, dst, qn, div - m, m - qn);
                } else {
                    mul_s(tp, div - m, m - qn, dst, qn);
                }

                cy = subc_n(src - m, src - m, tp, m);
                if (qh != 0) {
                    cy += subc_n(src - m + qn, src - m + qn, div - m, m - qn);
                }

                while (cy != 0) {
                    qh -= subc_1(dst, dst, qn, 1);
                    cy -= addc_n(src - m, src - m, div - m, m);
                }
            }
        }

        qn = n - m - qn;
        do {
            dst -= m;
            src -= m;
            dc_div4by2_qr(dst, src - m, div - m, m, dinv, tp);
            qn -= m;
        } while (qn > 0);
    } else {
        dst -= qn; /* point at low limb of next quotient block */
        src -= qn; /* point in the middle of partial remainder */

        if (qn < dc_div_qr_threshold) {
            qh = sb_div_qr_s(dst, src - qn, 2 * qn, div - qn, qn, dinv);
        } else {
            qh = dc_div4by2_qr(dst, src - qn, div - qn, qn, dinv, tp);
        }

        if (qn != m) {
            if (qn > m - qn) {
                mul_s(tp, dst, qn, div - m, m - qn);
            } else {
                mul_s(tp, div - m, m - qn, dst, qn);
            }

            cy = subc_n(src - m, src - m, tp, m);
            if (qh != 0) {
                cy += subc_n(src - m + qn, src - m + qn, div - m, m - qn);
            }

            while (cy != 0) {
                qh -= subc_1(dst, dst, qn, 1);
                cy -= addc_n(src - m, src - m, div - m, m);
            }
        }
    }

    return qh;
}

extern template uint64_t dc_div_qr_s<uint64_t>(uint64_t *dst, uint64_t *src, size_t n,
                                               const uint64_t *div, size_t m,
                                               uint64_t dinv);

template <typename T>
void div_qr_s(T *dst, T *rem, const T *src, size_t n, const T *div, size_t m) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    switch (m) {
    case 0: {
        WJR_UNREACHABLE();
        break;
    }
    case 1: {
        return div_qr_1(dst, rem[0], src, n, div[0]);
    }
    case 2: {
        return div_qr_2(dst, rem, src, n, div);
    }
    default: {
        break;
    }
    }

    unsigned int adjust = src[n - 1] >= div[m - 1];
    if (n + adjust >= 2 * m) {
        T *sp;
        T *dp;

        dst[n - m] = 0;

        const auto shift = clz(div[m - 1]);
        const size_t alloc = n + 1 + (shift != 0 ? m : 0);
        unique_stack_allocator stkal(math_details::stack_alloc);
        auto stk = static_cast<T *>(stkal.allocate(sizeof(T) * alloc));
        sp = stk;

        if (shift != 0) {
            dp = stk + (n + 1);
            lshift_n(dp, div, m, shift);
            sp[n] = lshift_n(sp, src, n, shift);
        } else {
            dp = const_cast<T *>(div);
            std::copy_n(src, n, sp);
            sp[n] = 0;
        }

        n += adjust;

        const auto dinv = div3by2_divider<T>::reciprocal(dp[m - 2], dp[m - 1]);

        if (m < dc_div_qr_threshold) {
            sb_div_qr_s(dst, sp, n, dp, m, dinv);
        } else {
            dc_div_qr_s(dst, sp, n, dp, m, dinv);
        }

        rshift_n(rem, sp, m, shift);
        return;
    }

    // 2 * m > n + adjust

    size_t qn = n - m;
    dst[qn] = 0;
    qn += adjust;

    if (qn == 0) {
        std::copy_n(src, m, rem);
        return;
    }

    T *sp, *dp;
    size_t st;

    st = m - qn; // st = m - qn = 2 * m - n + adjust > 2 * adjust

    const auto shift = clz(div[m - 1]);

    const size_t alloc = 2 * qn + (shift != 0 ? qn : 0);
    unique_stack_allocator stkal(math_details::stack_alloc);
    auto stk = static_cast<T *>(stkal.allocate(sizeof(T) * alloc));
    sp = stk;

    if (shift != 0) {
        dp = stk + 2 * qn;
        lshift_n(dp, div + st, qn, shift, div[st - 1]);
        if (adjust) {
            sp[2 * qn - 1] =
                lshift_n(sp, src + n - 2 * qn + 1, 2 * qn - 1, shift, src[n - 2 * qn]);
        } else {
            lshift_n(sp, src + n - 2 * qn, 2 * qn, shift, src[n - 2 * qn - 1]);
        }
    } else {
        dp = const_cast<T *>(div + st);
        if (adjust) {
            std::copy_n(src + n - 2 * qn + 1, 2 * qn - 1, sp);
            sp[2 * qn - 1] = 0;
        } else {
            std::copy_n(src + n - 2 * qn, 2 * qn, sp);
        }
    }

    if (qn == 1) {
        const auto dinv = div2by1_divider<T>::reciprocal(dp[0]);
        auto hi = sp[1];
        dst[0] = div2by1_divider<T>::divide(dp[0], dinv, sp[0], hi);
        sp[0] = hi;
    } else if (qn == 2) {
        const auto lo = dp[0];
        const auto hi = dp[1];
        const auto dinv = div3by2_divider<T>::reciprocal(lo, hi);
        // maybe inlined ?
        div_qr_2_noshift(dst, sp, sp, 4, div3by2_divider_noshift<T>(lo, hi, dinv));
    } else {
        const auto lo = dp[qn - 2];
        const auto hi = dp[qn - 1];
        const auto dinv = div3by2_divider<T>::reciprocal(lo, hi);
        if (qn < dc_div_qr_threshold) {
            sb_div_qr_s(dst, sp, 2 * qn, dp, qn, dinv);
        } else {
            dc_div_qr_s(dst, sp, 2 * qn, dp, qn, dinv);
        }
    }

    WJR_ASSUME(st >= 1);

    auto stk2 = static_cast<T *>(stkal.allocate(sizeof(T) * m));
    auto rp = stk2;

    unsigned int cf;

    if (!shift) {
        if (qn >= st) {
            mul_s(rp, dst, qn, div, st);
        } else {
            mul_s(rp, div, st, dst, qn);
        }
        cf = subc_n(rem, src, rp, st);
    } else {
        constexpr auto digits = std::numeric_limits<T>::digits;
        T fix = rshift_n(sp, sp, qn, shift);
        T mask = (1ull << (digits - shift)) - 1;

        if (st != 1) {
            if (qn >= st - 1) {
                mul_s(rp, dst, qn, div, st - 1);
            } else {
                mul_s(rp, div, st - 1, dst, qn);
            }
            rp[m - 1] = addmul_1(rp + st - 1, dst, qn, div[st - 1] & mask);
            cf = subc_n(rem, src, rp, st - 1);
            rem[st - 1] = subc((src[st - 1] & mask) | fix, rp[st - 1], cf, cf);
        } else {
            rp[m - 1] = mul_1(rp, dst, qn, div[0] & mask);
            rem[0] = subc((src[0] & mask) | fix, rp[0], 0, cf);
        }
    }

    cf = subc_n(rem + st, sp, rp + st, qn, cf);

    while (cf != 0) {
        subc_1(dst, dst, qn, 1);
        cf -= addc_n(rem, rem, div, m);
    }

    return;
}

extern template void div_qr_s<uint64_t>(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                        size_t n, const uint64_t *div, size_t m);

template <typename T>
WJR_CONSTEXPR_E T fallback_divexact_dbm1c(T *dst, const T *src, size_t n, T bd, T h) {
    T a = 0, p0 = 0, p1 = 0, cf = 0;

    // GCC can't optimize well
    WJR_UNROLL(4)
    for (size_t i = 0; i < n; i++) {
        a = src[i];
        p0 = mul(a, bd, p1);
        h = subc(h, p0, 0, cf);
        dst[i] = h;
        h -= p1 + cf;
    }

    return h;
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E T divexact_dbm1c(T *dst, const T *src, size_t n, T bd, T h) {
#if WJR_HAS_BUILTIN(ASM_DIVEXACT_DBM1C)
    if (is_constant_evaluated()) {
        return fallback_divexact_dbm1c(dst, src, n, bd, h);
    }

    return asm_divexact_dbm1c(dst, src, n, bd, h);
#else
    return fallback_divexact_dbm1c(dst, src, n, bd, h);
#endif
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_by3(T *dst, const T *src, size_t n) {
    constexpr T max = in_place_max;
    (void)divexact_dbm1c<T>(dst, src, n, max / 3, 0);
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_by5(T *dst, const T *src, size_t n) {
    constexpr T max = in_place_max;
    (void)divexact_dbm1c<T>(dst, src, n, max / 5, 0);
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_by15(T *dst, const T *src, size_t n) {
    constexpr T max = in_place_max;
    (void)divexact_dbm1c<T>(dst, src, n, max / 15, 0);
}

template <typename T, T c, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_byc(T *dst, const T *src, size_t n,
                                  std::integral_constant<T, c>) {

    // cost : divexact_dbm1c * 2 + shift * 1 <= divexact_1

    constexpr auto __is_fast = [](auto cr) {
        constexpr T r = get_place_index_v<remove_cvref_t<decltype(cr)>>;
        return c % r == 0 && is_zero_or_single_bit(c / r);
    };

    auto __resolve = [dst, n](auto cr) {
        constexpr T r = get_place_index_v<remove_cvref_t<decltype(cr)>>;
        constexpr auto p = fallback_ctz(c / r);
        if constexpr (p != 0) {
            rshift_n(dst, dst, n, p);
        } else {
            (void)(dst);
            (void)(n);
        }
    };

    if constexpr (__is_fast(std::in_place_index<1>)) {
        __resolve(std::in_place_index<1>);
    } else if constexpr (__is_fast(std::in_place_index<3>)) {
        divexact_by3(dst, src, n);
        __resolve(std::in_place_index<3>);
    } else if constexpr (__is_fast(std::in_place_index<5>)) {
        divexact_by5(dst, src, n);
        __resolve(std::in_place_index<5>);
    } else if constexpr (__is_fast(std::in_place_index<15>)) {
        divexact_by15(dst, src, n);
        __resolve(std::in_place_index<15>);
    } else if constexpr (__is_fast(std::in_place_index<9>)) {
        divexact_by3(dst, src, n);
        divexact_by3(dst, dst, n);
        __resolve(std::in_place_index<9>);
    } else if constexpr (__is_fast(std::in_place_index<25>)) {
        divexact_by5(dst, src, n);
        divexact_by5(dst, dst, n);
        __resolve(std::in_place_index<25>);
    } else if constexpr (__is_fast(std::in_place_index<45>)) {
        divexact_by3(dst, src, n);
        divexact_by15(dst, dst, n);
        __resolve(std::in_place_index<45>);
    } else if constexpr (__is_fast(std::in_place_index<75>)) {
        divexact_by5(dst, src, n);
        divexact_by15(dst, dst, n);
        __resolve(std::in_place_index<75>);
    } else if constexpr (__is_fast(std::in_place_index<225>)) {
        divexact_by15(dst, src, n);
        divexact_by15(dst, dst, n);
        __resolve(std::in_place_index<225>);
    } else {
        constexpr auto shift = fallback_ctz(c);
        using divider_t = divexact1_divider<T>;
        constexpr auto divisor = c >> shift;
        constexpr auto value = divider_t::reciprocal(divisor);
        constexpr auto divider = divider_t(divisor, value, shift);
        divexact_1(dst, src, n, divider);
    }
}

// reference : ftp://ftp.risc.uni-linz.ac.at/pub/techreports/1992/92-35.ps.gz
template <typename T>
WJR_CONSTEXPR_E void fallback_divexact_1_noshift(T *dst, const T *src, size_t n,
                                                 const divexact1_divider<T> &div) {
    uint64_t divisor = div.get_divisor();
    uint64_t value = div.get_value();

    uint64_t rdx = 0, r10 = 0;
    uint64_t cf = 0;
    size_t idx = 0;

    --n;

    if (WJR_LIKELY(n != 0)) {
        do {
            r10 = src[idx];
            r10 = subc(r10, rdx, cf, cf);
            r10 = mullo(r10, value);
            dst[idx] = r10;
            ++idx;
            rdx = mulhi(r10, divisor);
        } while (WJR_LIKELY(idx != n));
    }

    r10 = src[n];
    r10 -= rdx + cf;
    r10 = mullo(r10, value);
    dst[n] = r10;
    return;
}

template <typename T>
WJR_CONSTEXPR_E void fallback_divexact_1_shift(T *dst, const T *src, size_t n,
                                               const divexact1_divider<T> &div) {
    uint64_t divisor = div.get_divisor();
    uint64_t value = div.get_value();
    unsigned int shift = div.get_shift();

    uint64_t rdx = 0, r10 = 0;
    uint64_t cf = 0;
    size_t idx = 0;

    --n;
    r10 = src[0];

    if (WJR_LIKELY(n != 0)) {
        do {
            uint64_t r11 = src[idx + 1];
            r10 = shrd(r10, r11, shift);
            r10 = subc(r10, rdx, cf, cf);
            r10 = mullo(r10, value);
            dst[idx] = r10;
            ++idx;
            rdx = mulhi(r10, divisor);
            r10 = r11;
        } while (WJR_LIKELY(idx != n));
    }

    r10 = r10 >> shift;
    r10 -= rdx + cf;
    r10 = mullo(r10, value);
    dst[n] = r10;
    return;
}

template <typename T>
WJR_CONSTEXPR_E void fallback_divexact_1(T *dst, const T *src, size_t n,
                                         const divexact1_divider<T> &div) {
    if (div.get_shift() == 0) {
        return fallback_divexact_1_noshift(dst, src, n, div);
    }

    return fallback_divexact_1_shift(dst, src, n, div);
}

template <typename T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          const divexact1_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(div.is_zero_or_single_bit())) {
        unsigned int c = div.get_shift();
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, div);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          type_identity_t<T> div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(is_zero_or_single_bit(div))) {
        unsigned int c = ctz(div);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, divexact1_divider<T>(div));
}

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__
#ifndef WJR_MATH_NEG_HPP__
#define WJR_MATH_NEG_HPP__

#ifndef WJR_MATH_NOT_HPP__
#define WJR_MATH_NOT_HPP__

// Already included

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_NOT_HPP__
#define WJR_X86_MATH_NOT_HPP__

// Already included
// Already included

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_NOT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(NOT_N)

template <typename T>
WJR_COLD void large_builtin_not_n(T *dst, const T *src, size_t n) {
    constexpr auto is_avx = WJR_PP_BOOL(WJR_HAS_SIMD(AVX2));

    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto simd_loop = simd_width / 64;

    uintptr_t ptr = reinterpret_cast<uintptr_t>(dst);
    WJR_ASSUME(ptr % sizeof(T) == 0);
    size_t offset = __align_up_offset(ptr, 32) / sizeof(T);

    WJR_ASSUME(offset < 4);

    auto y = sse::ones();

    switch (offset) {
    case 0: {
        break;
    }
    case 1: {
        dst[0] = ~src[0];

        ++dst;
        ++src;
        --n;
        break;
    }
    case 2: {
        sse::storeu((__m128i *)(dst), sse::Xor(sse::loadu((__m128i *)(src)), y));

        dst += 2;
        src += 2;
        n -= 2;
        break;
    }

    case 3: {
        sse::storeu((__m128i *)(dst), sse::Xor(sse::loadu((__m128i *)(src)), y));
        dst[2] = ~src[2];

        dst += 3;
        src += 3;
        n -= 3;
        break;
    }

    default: {
        WJR_UNREACHABLE();
    }
    }

    auto z = broadcast<__m128i_t, typename simd::int_tag_type>(y);

    size_t idx = 0;
    size_t m = n & (-simd_loop * 4);

    WJR_ASSUME(idx != m);

    do {
        auto x0 = simd::loadu((simd_int *)(src + idx));
        auto x1 = simd::loadu((simd_int *)(src + idx + simd_loop));
        auto x2 = simd::loadu((simd_int *)(src + idx + simd_loop * 2));
        auto x3 = simd::loadu((simd_int *)(src + idx + simd_loop * 3));

        simd::store((simd_int *)(dst + idx), simd::Xor(x0, z));
        simd::store((simd_int *)(dst + idx + simd_loop), simd::Xor(x1, z));
        simd::store((simd_int *)(dst + idx + simd_loop * 2), simd::Xor(x2, z));
        simd::store((simd_int *)(dst + idx + simd_loop * 3), simd::Xor(x3, z));

        idx += simd_loop * 4;
    } while (idx != m);

    if (WJR_UNLIKELY(n == m)) {
        return;
    }

    dst += m;
    src += m;
    n -= m;

    m = n / simd_loop;
    WJR_ASSUME(m < 4);

    switch (m) {
    case 3: {
        simd::store((simd_int *)(dst), simd::Xor(simd::loadu((simd_int *)(src)), z));
        WJR_FALLTHROUGH;
    }
    case 2: {
        simd::store((simd_int *)(dst + simd_loop * (m - 2)),
                    simd::Xor(simd::loadu((simd_int *)(src + simd_loop * (m - 2))), z));
        WJR_FALLTHROUGH;
    }
    case 1: {
        simd::store((simd_int *)(dst + simd_loop * (m - 1)),
                    simd::Xor(simd::loadu((simd_int *)(src + simd_loop * (m - 1))), z));
        WJR_FALLTHROUGH;
    }
    case 0: {
        break;
    }
    default: {
        WJR_UNREACHABLE();
    }
    }

    m = n & (-simd_loop);

    if (WJR_UNLIKELY(n == m)) {
        return;
    }

    WJR_ASSUME(n - m < 4);

    switch (n - m) {
    case 1: {
        dst[m] = ~src[m];
        break;
    }
    case 2: {
        sse::store((__m128i *)(dst + m), sse::Xor(sse::loadu((__m128i *)(src + m)), y));
        break;
    }

    case 3: {
        sse::store((__m128i *)(dst + m), sse::Xor(sse::loadu((__m128i *)(src + m)), y));
        dst[m + 2] = ~src[m + 2];
        break;
    }

    default: {
        WJR_UNREACHABLE();
    }
    }

    return;
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_not_n(T *dst, const T *src, size_t n) {
    static_assert(sizeof(T) == 8, "");

    if (WJR_UNLIKELY(n < 4)) {
        switch (n) {
        case 3: {
            dst[0] = ~src[0];
            WJR_FALLTHROUGH;
        }
        case 2: {
            dst[n - 2] = ~src[n - 2];
            WJR_FALLTHROUGH;
        }
        case 1: {
            dst[n - 1] = ~src[n - 1];
            WJR_FALLTHROUGH;
        }
        case 0: {
            break;
        }
        }

        return;
    }

    if (WJR_UNLIKELY(n >= 35)) {
        // Can be aligned
        // TODO : Align those that cannot be aligned with T through uint8_t
        if (WJR_LIKELY(reinterpret_cast<uintptr_t>(dst) % sizeof(T) == 0)) {
            return large_builtin_not_n(dst, src, n);
        }
    }

    size_t idx = 0;

    auto y = sse::ones();

    if (n & 4) {
        auto x0 = sse::loadu((__m128i *)(src + idx));
        auto x1 = sse::loadu((__m128i *)(src + idx + 2));

        sse::storeu((__m128i *)(dst + idx), sse::Xor(x0, y));
        sse::storeu((__m128i *)(dst + idx + 2), sse::Xor(x1, y));

        idx += 4;
    }

    if (n & 2) {
        sse::storeu((__m128i *)(dst + idx),
                    sse::Xor(sse::loadu((__m128i *)(src + idx)), y));

        idx += 2;
    }

    if (n & 1) {
        dst[idx] = ~src[idx];

        ++idx;
    }

    if (WJR_UNLIKELY(idx == n)) {
        return;
    }

    WJR_ASSUME((n - idx) % 8 == 0);

    do {
        auto x0 = sse::loadu((__m128i *)(src + idx));
        auto x1 = sse::loadu((__m128i *)(src + idx + 2));
        auto x2 = sse::loadu((__m128i *)(src + idx + 4));
        auto x3 = sse::loadu((__m128i *)(src + idx + 6));

        sse::storeu((__m128i *)(dst + idx), sse::Xor(x0, y));
        sse::storeu((__m128i *)(dst + idx + 2), sse::Xor(x1, y));
        sse::storeu((__m128i *)(dst + idx + 4), sse::Xor(x2, y));
        sse::storeu((__m128i *)(dst + idx + 6), sse::Xor(x3, y));

        idx += 8;
    } while (idx != n);
}

#endif //

} // namespace wjr

#endif // WJR_X86_MATH_NOT_HPP__
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_not_n(T *dst, const T *src, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = ~src[i];
    }
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E void not_n(T *dst, const T *src, size_t n) {
#if WJR_HAS_BUILTIN(NOT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_not_n(dst, src, n);
        }

        return builtin_not_n(dst, src, n);
    } else {
        return fallback_not_n(dst, src, n);
    }
#else
    return fallback_not_n(dst, src, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_NOT_HPP__
// Already included

namespace wjr {

/*
  return true if src is all zero
  calculations : stable n instead of not + inc which maybe n * 2
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E bool neg_n(T *dst, const T *src, size_t n) {
    size_t idx = replace_find_not(dst, src, n, 0, 0);

    if (idx == n) {
        return true;
    }

    dst[idx] = -src[idx];

    not_n(dst + idx + 1, src + idx + 1, n - idx - 1);
    return false;
}

} // namespace wjr

#endif // WJR_MATH_NEG_HPP__
#ifndef WJR_MATH_CONVERT_HPP__
#define WJR_MATH_CONVERT_HPP__

#include <system_error>

// Already included
#ifndef WJR_MATH_CONVERT_IMPL_HPP__
#define WJR_MATH_CONVERT_IMPL_HPP__

#include <array>

// Already included

namespace wjr {

struct char_converter_t {
private:
    static constexpr std::array<char, 36> to_table = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    static constexpr std::array<uint8_t, 256> from_table = {
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   127, 127, 127, 127, 127, 127,
        127, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
        25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  127, 127, 127, 127, 127,
        127, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
        25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};

public:
    template <uint64_t Base = 0>
    WJR_CONST constexpr static char to(uint8_t x) {
        if constexpr (Base == 0) {
            WJR_ASSERT_L2(x < 36);
        } else {
            WJR_ASSERT_L2(x < Base);
        }

        if constexpr (Base == 0 || Base > 10) {

            if (WJR_BUILTIN_CONSTANT_P(x < 10) && x < 10) {
                return x + '0';
            }

            return to_table[x];
        } else {
            return x + '0';
        }
    }

    WJR_CONST constexpr static uint8_t from(uint8_t x) { return from_table[x]; }
};

/**
 * @brief 36 hexadecimal character<->number converter
 *
 * @note Range of characters: `[0-9a-zA-Z]`, range of numbers: `[0-35]`
 */
inline constexpr char_converter_t char_converter = {};

struct origin_converter_t {
    template <uint64_t Base = 0>
    WJR_CONST constexpr static char to(uint8_t x) {
        return x;
    }

    WJR_CONST constexpr static uint8_t from(char x) { return x; }
};

/**
 * @brief Original converter that does not change the number or character.
 *
 * @note Range of characters: `[0-35]`, range of numbers: `[0-35]`
 */
inline constexpr origin_converter_t origin_converter = {};

} // namespace wjr

#endif // WJR_MATH_CONVERT_IMPL_HPP__

#ifndef WJR_MATH_PRECOMPUTE_CHARS_CONVERT_HPP__
#define WJR_MATH_PRECOMPUTE_CHARS_CONVERT_HPP__

// Already included

namespace wjr {

struct precompute_chars_convert_16n_t {
    uint64_t big_base;
    size_t n;
    int digits_in_one_base;
    int digits_in_sixteen_base;
    uint64_t arr[16];
};

struct precompute_chars_convert_t {
    const uint64_t *ptr;
    size_t n;
    size_t shift;
    size_t digits_in_base;
    unsigned int base;
};

extern "C" precompute_chars_convert_16n_t *precompute_chars_convert_16n_ptr[37];

extern "C" precompute_chars_convert_t *precompute_chars_convert(precompute_chars_convert_t *pre,
                                                      size_t n, unsigned int base,
                                                      uint64_t *table_mem);

} // namespace wjr

#endif // WJR_MATH_PRECOMPUTE_CHARS_CONVERT_HPP__

#if defined(WJR_X86)
#ifndef WJR_X86_MATH_CONVERT_HPP__
#define WJR_X86_MATH_CONVERT_HPP__

// Already included
// Already included

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_TO_CHARS_UNROLL_8_FAST WJR_HAS_DEF

#define WJR_HAS_BUILTIN_FROM_CHARS_UNROLL_8_FAST WJR_HAS_DEF
#define WJR_HAS_BUILTIN_FROM_CHARS_UNROLL_16_FAST WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(TO_CHARS_UNROLL_8_FAST)

namespace to_chars_details {

static __m128i mul10p4 = simd_cast<uint32_t, __m128i_t>(3518437209);
static __m128i mul10p4x = simd_cast<uint32_t, __m128i_t>(10000);
static __m128i mul10p2 = sse::set1_epi16(5243);
static __m128i mul10p2x = sse::set1_epi16(100);
static __m128i mul10p1 = sse::set1_epi16((short)52429u);
static __m128i mul10p1x = sse::set1_epi16(10);

static __m128i shuf = sse::setr_epi8(0, 8, 4, 12, 2, 10, 6, 14, 1, 1, 1, 1, 1, 1, 1, 1);

} // namespace to_chars_details

#endif

#if WJR_HAS_BUILTIN(TO_CHARS_UNROLL_8_FAST)

inline uint64_t builtin_to_chars_unroll_8_fast_10(uint32_t in) {

    __m128i x = simd_cast<uint32_t, __m128i_t>(in);
    __m128i q, r;

    q = _mm_mul_epu32(x, to_chars_details::mul10p4);
    q = _mm_srli_epi64(q, 45);

    r = _mm_sub_epi32(x, _mm_mul_epu32(q, to_chars_details::mul10p4x));
    x = _mm_packus_epi32(q, r);

    q = _mm_mulhi_epu16(x, to_chars_details::mul10p2);
    q = _mm_srli_epi16(q, 3);

    r = _mm_sub_epi16(x, _mm_mullo_epi16(q, to_chars_details::mul10p2x));
    x = _mm_packus_epi16(q, r);

    q = _mm_mulhi_epu16(x, to_chars_details::mul10p1);
    q = _mm_srli_epi16(q, 3);

    r = _mm_sub_epi16(x, _mm_mullo_epi16(q, to_chars_details::mul10p1x));
    x = _mm_packus_epi16(q, r);

    return simd_cast<__m128i_t, uint64_t>(sse::shuffle_epi8(x, to_chars_details::shuf));
}

template <uint64_t Base>
uint64_t builtin_to_chars_unroll_8_fast(uint32_t in) {
    if constexpr (Base == 10) {
        return builtin_to_chars_unroll_8_fast_10(in);
    } else {
        static_assert(Base == 10, "");
    }
}

template <uint64_t Base>
void builtin_to_chars_unroll_8_fast(void *ptr, uint32_t in, char_converter_t) {
    uint64_t x = builtin_to_chars_unroll_8_fast<Base>(in) + 0x3030303030303030ull;

    write_memory<uint64_t>(ptr, x);
}

template <uint64_t Base>
void builtin_to_chars_unroll_8_fast(void *ptr, uint32_t in, origin_converter_t) {
    uint64_t x = builtin_to_chars_unroll_8_fast<Base>(in);

    write_memory<uint64_t>(ptr, x);
}

#endif

#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST) ||                                         \
    WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_16_FAST)

namespace from_chars_details {

template <uint64_t Base>
inline constexpr uint64_t __base2 = Base * Base;

template <uint64_t Base>
inline constexpr uint64_t __base4 = __base2<Base> * __base2<Base>;

template <uint64_t Base>
inline constexpr uint64_t __base8 = __base4<Base> * __base4<Base>;

template <uint64_t Base>
static __m128i mulp1x = sse::setr_epi8(Base, 1, Base, 1, Base, 1, Base, 1, Base, 1, Base,
                                       1, Base, 1, Base, 1);

template <uint64_t Base>
static __m128i mulp2x = sse::setr_epi16(__base2<Base>, 1, __base2<Base>, 1, __base2<Base>,
                                        1, __base2<Base>, 1);

template <uint64_t Base>
static __m128i mulp4x = sse::setr_epi16(__base4<Base>, 1, __base4<Base>, 1, __base4<Base>,
                                        1, __base4<Base>, 1);

static __m128i ascii = sse::set1_epi8(0x30);

} // namespace from_chars_details

#endif

#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)

template <uint64_t Base>
uint32_t builtin_from_chars_unroll_8_fast(__m128i in) {
    __m128i t1 = _mm_maddubs_epi16(in, from_chars_details::mulp1x<Base>);
    __m128i t2 = _mm_madd_epi16(t1, from_chars_details::mulp2x<Base>);
    __m128i t3 = _mm_packus_epi32(t2, t2);
    __m128i t4 = _mm_madd_epi16(t3, from_chars_details::mulp4x<Base>);

    return simd_cast<__m128i_t, uint32_t>(t4);
}

template <uint64_t Base>
uint32_t builtin_from_chars_unroll_8_fast(const void *ptr, char_converter_t) {
    static_assert(Base <= 10, "");

    __m128i in = _mm_sub_epi8(sse::loadu_si64(ptr), from_chars_details::ascii);
    return builtin_from_chars_unroll_8_fast<Base>(in);
}

template <uint64_t Base>
uint32_t builtin_from_chars_unroll_8_fast(const void *ptr, origin_converter_t) {
    return builtin_from_chars_unroll_8_fast<Base>(sse::loadu_si64(ptr));
}

#endif

#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_16_FAST)

template <uint64_t Base>
uint64_t builtin_from_chars_unroll_16_fast(__m128i in) {
    __m128i t1 = _mm_maddubs_epi16(in, from_chars_details::mulp1x<Base>);
    __m128i t2 = _mm_madd_epi16(t1, from_chars_details::mulp2x<Base>);
    __m128i t3 = _mm_packus_epi32(t2, t2);
    __m128i t4 = _mm_madd_epi16(t3, from_chars_details::mulp4x<Base>);

    uint64_t val = simd_cast<__m128i_t, uint64_t>(t4);
    uint32_t lo = val;
    uint32_t hi = val >> 32;

    return lo * from_chars_details::__base8<Base> + hi;
}

template <uint64_t Base>
uint64_t builtin_from_chars_unroll_16_fast(const void *ptr, char_converter_t) {
    static_assert(Base <= 10, "");

    __m128i in = _mm_sub_epi8(sse::loadu((__m128i *)(ptr)), from_chars_details::ascii);
    return builtin_from_chars_unroll_16_fast<Base>(in);
}

template <uint64_t Base>
uint64_t builtin_from_chars_unroll_16_fast(const void *ptr, origin_converter_t) {
    return builtin_from_chars_unroll_16_fast<Base>(sse::loadu((__m128i *)(ptr)));
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_CONVERT_HPP__
#endif

namespace wjr {

inline constexpr size_t dc_bignum_to_chars_threshold = WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_to_chars_precompute_threshold =
    WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;

inline constexpr size_t dc_bignum_from_chars_threshold =
    WJR_DC_BIGNUM_FROM_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_from_chars_precompute_threshold =
    WJR_DC_BIGNUM_FROM_CHARS_PRECOMPUTE_THRESHOLD;

inline constexpr auto div2by1_divider_noshift_of_big_base_10 =
    div2by1_divider_noshift<uint64_t>(10'000'000'000'000'000'000ull,
                                      15'581'492'618'384'294'730ull);

// require operator() of Converter is constexpr
template <typename Converter, uint64_t Base, int Unroll>
class __char_converter_table_t {
    static constexpr uint64_t pw2 = Unroll == 2 ? Base * Base : Base * Base * Base * Base;

public:
    constexpr __char_converter_table_t() : table() {
        constexpr auto conv = Converter{};
        for (uint64_t i = 0, j = 0; i < pw2; ++i, j += Unroll) {
            int x = i;
            for (int k = Unroll - 1; ~k; --k) {
                table[j + k] = conv.to(x % Base);
                x /= Base;
            }
        }
    }

    WJR_CONST constexpr char operator[](unsigned int idx) const { return table[idx]; }

    WJR_CONST constexpr const char *data() const { return table.data(); }

private:
    std::array<char, pw2 * Unroll> table;
};

template <typename Converter, uint64_t Base, int Unroll>
inline constexpr __char_converter_table_t<Converter, Base, Unroll>
    __char_converter_table = {};

template <typename Iter, typename Converter>
struct __is_fast_convert_iterator
    : std::conjunction<std::is_pointer<Iter>,
                       std::bool_constant<
                           sizeof(typename std::iterator_traits<Iter>::value_type) == 1>,
                       is_any_of<Converter, char_converter_t, origin_converter_t>> {};

template <typename Iter, typename Converter>
inline constexpr bool __is_fast_convert_iterator_v =
    __is_fast_convert_iterator<Iter, Converter>::value;

namespace convert_details {

template <typename Enable, typename Base, typename Value, typename Converter,
          typename... Args>
struct __has_to_chars_fast_fn_fast_conv : std::false_type {};
template <typename Base, typename Value, typename Converter, typename... Args>
struct __has_to_chars_fast_fn_fast_conv<
    std::void_t<decltype(Base::__fast_conv(std::declval<void *>(), std::declval<Value>(),
                                           std::declval<Converter>()))>,
    Base, Value, Converter, Args...> : std::true_type {};
template <typename Base, typename Value, typename Converter, typename... Args>
struct has_to_chars_fast_fn_fast_conv
    : __has_to_chars_fast_fn_fast_conv<void, Base, Value, Converter, Args...> {};
template <typename Base, typename Value, typename Converter, typename... Args>
constexpr bool has_to_chars_fast_fn_fast_conv_v =
    has_to_chars_fast_fn_fast_conv<Base, Value, Converter, Args...>::value;

template <typename Enable, typename Base, typename Converter, typename... Args>
struct __has_from_chars_fast_fn_fast_conv : std::false_type {};
template <typename Base, typename Converter, typename... Args>
struct __has_from_chars_fast_fn_fast_conv<
    std::void_t<decltype(Base::__fast_conv(std::declval<const void *>(),
                                           std::declval<Converter>()))>,
    Base, Converter, Args...> : std::true_type {};
template <typename Base, typename Converter, typename... Args>
struct has_from_chars_fast_fn_fast_conv
    : __has_from_chars_fast_fn_fast_conv<void, Base, Converter, Args...> {};
template <typename Base, typename Converter, typename... Args>
constexpr bool has_from_chars_fast_fn_fast_conv_v =
    has_from_chars_fast_fn_fast_conv<Base, Converter, Args...>::value;

WJR_REGISTER_HAS_TYPE(from_chars_validate_fast_fn_fast_conv,
                      Base::__fast_conv(std::declval<const void *&>(),
                                        std::declval<Args>()...),
                      Base);

} // namespace convert_details

template <uint64_t Base>
class __to_chars_unroll_2_fast_fn_impl_base {
public:
    template <typename Converter>
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val, Converter) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Converter, Base, 4>;
            ::memcpy(str, table.data() + val * 4 + 2, 2);
        } else {
            constexpr auto &table = __char_converter_table<Converter, Base, 2>;
            ::memcpy(str, table.data() + val * 2, 2);
        }
    }
};

template <uint64_t Base>
class __to_chars_unroll_2_fast_fn_impl {};

template <>
class __to_chars_unroll_2_fast_fn_impl<2>
    : public __to_chars_unroll_2_fast_fn_impl_base<2> {};

template <>
class __to_chars_unroll_2_fast_fn_impl<8>
    : public __to_chars_unroll_2_fast_fn_impl_base<8> {};

template <>
class __to_chars_unroll_2_fast_fn_impl<10>
    : public __to_chars_unroll_2_fast_fn_impl_base<10> {};

template <>
class __to_chars_unroll_2_fast_fn_impl<16>
    : public __to_chars_unroll_2_fast_fn_impl_base<16> {};

template <uint64_t Base>
class __to_chars_unroll_4_fast_fn_impl_base {
public:
    template <typename Converter>
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val, Converter) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Converter, Base, 4>;
            ::memcpy(str, table.data() + val * 4, 4);
        } else {
            constexpr auto &table = __char_converter_table<Converter, Base, 2>;
            constexpr auto Base2 = Base * Base;
            uint32_t hi = val / Base2;
            uint32_t lo = val % Base2;

            ::memcpy(str, table.data() + hi * 2, 2);
            ::memcpy(str + 2, table.data() + lo * 2, 2);
        }
    }
};

template <uint64_t Base>
class __to_chars_unroll_4_fast_fn_impl {};

template <>
class __to_chars_unroll_4_fast_fn_impl<2>
    : public __to_chars_unroll_4_fast_fn_impl_base<2> {};

template <>
class __to_chars_unroll_4_fast_fn_impl<8>
    : public __to_chars_unroll_4_fast_fn_impl_base<8> {};

template <>
class __to_chars_unroll_4_fast_fn_impl<10>
    : public __to_chars_unroll_4_fast_fn_impl_base<10> {};

template <>
class __to_chars_unroll_4_fast_fn_impl<16>
    : public __to_chars_unroll_4_fast_fn_impl_base<16> {};

template <uint64_t Base>
class __to_chars_unroll_8_fast_fn_impl_base {
#if WJR_HAS_BUILTIN(TO_CHARS_UNROLL_8_FAST)
public:
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint64_t val,
                                                 char_converter_t conv) {
        builtin_to_chars_unroll_8_fast<Base>(ptr, val, conv);
    }

    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint64_t val,
                                                 origin_converter_t conv) {
        builtin_to_chars_unroll_8_fast<Base>(ptr, val, conv);
    }
#endif
};

template <uint64_t Base>
class __to_chars_unroll_8_fast_fn_impl {};

template <>
class __to_chars_unroll_8_fast_fn_impl<10>
    : public __to_chars_unroll_8_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __to_chars_unroll_2_fn : public __to_chars_unroll_2_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_2_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter ptr, uint32_t val, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase, uint32_t,
                                                                        Converter>) {
            Mybase::__fast_conv(to_address(ptr), val, conv);
        } else {
            ptr[0] = conv.template to<Base>(val / Base);
            ptr[1] = conv.template to<Base>(val % Base);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_2_fn<Base> __to_chars_unroll_2{};

template <uint64_t Base>
class __to_chars_unroll_4_fn_impl : public __to_chars_unroll_4_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_4_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter ptr, uint32_t val, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase, uint32_t,
                                                                        Converter>) {
            Mybase::__fast_conv(to_address(ptr), val, conv);
        } else {
            constexpr auto Base2 = Base * Base;
            __to_chars_unroll_2<Base>(ptr, val / Base2, conv);
            __to_chars_unroll_2<Base>(ptr + 2, val % Base2, conv);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_4_fn_impl<Base> __to_chars_unroll_4{};

template <uint64_t Base>
class __to_chars_unroll_8_fn_impl : public __to_chars_unroll_8_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_8_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter ptr, uint64_t val, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase, uint64_t,
                                                                        Converter>) {
            Mybase::__fast_conv(to_address(ptr), val, conv);
        } else {
            constexpr auto Base4 = Base * Base * Base * Base;
            __to_chars_unroll_4<Base>(ptr, val / Base4, conv);
            __to_chars_unroll_4<Base>(ptr + 4, val % Base4, conv);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_8_fn_impl<Base> __to_chars_unroll_8{};

template <uint64_t Base>
class __from_chars_unroll_4_fast_fn_impl_base {
    WJR_CONST WJR_INTRINSIC_INLINE static uint32_t __fast_conv(uint32_t val) {
        constexpr uint32_t Base2 = Base * Base;

        constexpr uint32_t mask = 0x00FF00FF;
        constexpr uint32_t mul = 1 + (Base2 << 16);
        val = (val * Base) + (val >> 8);
        val = ((val & mask) * mul) >> 16;
        return uint32_t(val);
    }

public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
                                                              origin_converter_t) {
        return __fast_conv(read_memory<uint32_t>(ptr));
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
                                                              char_converter_t) {
        return __fast_conv(read_memory<uint32_t>(ptr) - 0x30303030u);
    }
};

template <uint64_t Base>
class __from_chars_unroll_8_fast_fn_impl_base {
protected:
    WJR_CONST WJR_INTRINSIC_INLINE static uint32_t __fast_conv(uint64_t val) {
        constexpr uint64_t Base2 = Base * Base;
        constexpr uint64_t Base4 = Base2 * Base2;
        constexpr uint64_t Base6 = Base4 * Base2;

        constexpr uint64_t mask = 0x000000FF000000FF;
        constexpr uint64_t mul1 = Base2 + (Base6 << 32);
        constexpr uint64_t mul2 = 1 + (Base4 << 32);

        val = (val * Base) + (val >> 8);
        val = (((val & mask) * mul1) + (((val >> 16) & mask) * mul2)) >> 32;
        return val;
    }

public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
                                                              origin_converter_t) {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)
        return builtin_from_chars_unroll_8_fast<Base>(ptr, origin_converter);
#else
        return __fast_conv(read_memory<uint64_t>(ptr));
#endif
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
                                                              char_converter_t) {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)
        return builtin_from_chars_unroll_8_fast<Base>(ptr, char_converter);
#else
        return __fast_conv(read_memory<uint64_t>(ptr) - 0x3030303030303030ull);
#endif
    }
};

template <uint64_t Base>
class __from_chars_unroll_16_fast_fn_impl_base {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_16_FAST)
public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint64_t __fast_conv(const void *ptr,
                                                              origin_converter_t) {
        return builtin_from_chars_unroll_16_fast<Base>(ptr, origin_converter);
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint64_t __fast_conv(const void *ptr,
                                                              char_converter_t) {
        return builtin_from_chars_unroll_16_fast<Base>(ptr, char_converter);
    }
#endif
};

template <uint64_t Base>
class __from_chars_unroll_4_fast_fn_impl {};

template <>
class __from_chars_unroll_4_fast_fn_impl<2>
    : public __from_chars_unroll_4_fast_fn_impl_base<2> {};

template <>
class __from_chars_unroll_4_fast_fn_impl<8>
    : public __from_chars_unroll_4_fast_fn_impl_base<8> {};

template <>
class __from_chars_unroll_4_fast_fn_impl<10>
    : public __from_chars_unroll_4_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __from_chars_unroll_8_fast_fn_impl {};

template <>
class __from_chars_unroll_8_fast_fn_impl<2>
    : public __from_chars_unroll_8_fast_fn_impl_base<2> {};

template <>
class __from_chars_unroll_8_fast_fn_impl<8>
    : public __from_chars_unroll_8_fast_fn_impl_base<8> {};

template <>
class __from_chars_unroll_8_fast_fn_impl<10>
    : public __from_chars_unroll_8_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __from_chars_unroll_16_fast_fn_impl {};

template <>
class __from_chars_unroll_16_fast_fn_impl<2>
    : public __from_chars_unroll_16_fast_fn_impl_base<2> {};

template <>
class __from_chars_unroll_16_fast_fn_impl<8>
    : public __from_chars_unroll_16_fast_fn_impl_base<8> {};

template <>
class __from_chars_unroll_16_fast_fn_impl<10>
    : public __from_chars_unroll_16_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __from_chars_unroll_4_fn : public __from_chars_unroll_4_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_4_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(Iter first, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase,
                                                                          Converter>) {
            return Mybase::__fast_conv(to_address(first), conv);
        } else {
            uint64_t value = 0;
            value = conv.from(*first++);
            value = value * Base + conv.from(*first++);
            value = value * Base + conv.from(*first++);
            return value * Base + conv.from(*first++);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_4_fn<Base> __from_chars_unroll_4{};

template <uint64_t Base>
class __from_chars_unroll_8_fn : public __from_chars_unroll_8_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_8_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(Iter first, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase,
                                                                          Converter>) {
            return Mybase::__fast_conv(to_address(first), conv);
        } else {
            constexpr uint64_t Base4 = Base * Base * Base * Base;
            return __from_chars_unroll_4<Base>(first, conv) * Base4 +
                   __from_chars_unroll_4<Base>(first + 4, conv);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_8_fn<Base> __from_chars_unroll_8{};

template <uint64_t Base>
class __from_chars_unroll_16_fn : public __from_chars_unroll_16_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_16_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(Iter first, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase,
                                                                          Converter>) {
            return Mybase::__fast_conv(to_address(first), conv);
        } else {
            constexpr uint64_t Base4 = Base * Base * Base * Base;
            constexpr uint64_t Base8 = Base4 * Base4;
            return __from_chars_unroll_8<Base>(first, conv) * Base8 +
                   __from_chars_unroll_8<Base>(first + 8, conv);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_16_fn<Base> __from_chars_unroll_16{};

struct __from_chars_validate_result {
    bool ok;
    int offset;
};

template <uint64_t Base>
class __from_chars_validate_unroll_8_fast_fn_impl_base
    : private __from_chars_unroll_8_fast_fn_impl_base<Base> {

    WJR_INTRINSIC_INLINE static __from_chars_validate_result
    __fast_conv(uint64_t in, const void *&ptr, uint32_t &val) {
        constexpr uint64_t subt = broadcast<uint8_t, uint64_t>(Base);
        constexpr uint64_t mask = broadcast<uint8_t, uint64_t>(128);

        uint64_t xval = (in - subt) & mask;

        if (WJR_UNLIKELY(xval != mask)) {
            const auto offset = ctz(~xval);

            if (WJR_LIKELY(offset != 0)) {
                ptr = static_cast<const void *>(static_cast<const char *>(ptr) +
                                                offset / 8);
                in <<= 64 - offset;
                val = __from_chars_unroll_8_fast_fn_impl_base<Base>::__fast_conv(in);
            }

            return {false, offset / 8};
        }

        ptr = static_cast<const void *>(static_cast<const char *>(ptr) + 8);
        val = __from_chars_unroll_8_fast_fn_impl_base<Base>::__fast_conv(in);

        return {true, 0};
    }

public:
    WJR_INTRINSIC_INLINE static __from_chars_validate_result
    __fast_conv(const void *&ptr, uint32_t &val, origin_converter_t) {
        return __fast_conv(read_memory<uint64_t>(ptr), ptr, val);
    }

    WJR_INTRINSIC_INLINE static __from_chars_validate_result
    __fast_conv(const void *&ptr, uint32_t &val, char_converter_t) {
        return __fast_conv(read_memory<uint64_t>(ptr) - 0x3030303030303030ull, ptr, val);
    }
};

template <uint64_t Base>
class __from_chars_validate_unroll_8_fast_fn_impl {};

template <>
class __from_chars_validate_unroll_8_fast_fn_impl<2>
    : public __from_chars_validate_unroll_8_fast_fn_impl_base<2> {};

template <>
class __from_chars_validate_unroll_8_fast_fn_impl<8>
    : public __from_chars_validate_unroll_8_fast_fn_impl_base<8> {};

template <>
class __from_chars_validate_unroll_8_fast_fn_impl<10>
    : public __from_chars_validate_unroll_8_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __from_chars_validate_unroll_8_fn
    : public __from_chars_validate_unroll_8_fast_fn_impl<Base> {
    using Mybase = __from_chars_validate_unroll_8_fast_fn_impl<Base>;

public:
    /**
     * @return 0 if sucessed \n
     * otherwise return offset of error
     *
     */
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE __from_chars_validate_result operator()(Iter &first,
                                                                 uint32_t &val,
                                                                 Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_from_chars_validate_fast_fn_fast_conv_v<
                          Mybase, uint32_t &, Converter>) {
            const void *ptr = to_address(first);
            const void *end = ptr;
            auto ret = Mybase::__fast_conv(end, val, conv);
            first += static_cast<const char *>(end) - static_cast<const char *>(ptr);
            return ret;
        } else {
            uint8_t ch = conv.from(*first);
            if (WJR_UNLIKELY(ch >= Base)) {
                return {false, 0};
            }

            val = ch;

            for (int i = 1; i < 8; ++i) {
                ch = conv.from(*++first);
                if (WJR_UNLIKELY(ch >= Base)) {
                    return {false, i};
                }

                val = val * Base + ch;
            }

            return {true, 0};
        }
    }

    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter &first, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_from_chars_validate_fast_fn_fast_conv_v<
                          Mybase, Converter>) {
            const void *ptr = to_address(first);
            const void *end = ptr;
            Mybase::__fast_conv(end, conv);
            first += static_cast<const char *>(end) - static_cast<const char *>(ptr);
        } else {
            for (int i = 0; i < 8; ++i) {
                uint8_t ch = conv.from(*first);
                if (WJR_UNLIKELY(ch >= Base)) {
                    return;
                }

                ++first;
            }
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_validate_unroll_8_fn<Base> __from_chars_validate_unroll_8{};

template <typename UnsignedValue>
constexpr int fallback_count_digits10(UnsignedValue n) {
    int count = 1;

    if (WJR_UNLIKELY(n >= 1000)) {
        do {
            n /= 10000;
            count += 4;
        } while (n >= 1000);

        if (n == 0) {
            return count;
        }
    }

    if (n < 10) {
        return count;
    }

    if (n < 100) {
        return count + 1;
    }

    return count + 3;
}

inline int builtin_count_digits10_u32(uint32_t n) {
#define WJR_INC(T) (((sizeof(#T) - 1ull) << 32) - T)
    static constexpr uint64_t table[] = {
        WJR_INC(0),          WJR_INC(0),          WJR_INC(0),          // 8
        WJR_INC(10),         WJR_INC(10),         WJR_INC(10),         // 64
        WJR_INC(100),        WJR_INC(100),        WJR_INC(100),        // 512
        WJR_INC(1000),       WJR_INC(1000),       WJR_INC(1000),       // 4096
        WJR_INC(10000),      WJR_INC(10000),      WJR_INC(10000),      // 32k
        WJR_INC(100000),     WJR_INC(100000),     WJR_INC(100000),     // 256k
        WJR_INC(1000000),    WJR_INC(1000000),    WJR_INC(1000000),    // 2048k
        WJR_INC(10000000),   WJR_INC(10000000),   WJR_INC(10000000),   // 16M
        WJR_INC(100000000),  WJR_INC(100000000),  WJR_INC(100000000),  // 128M
        WJR_INC(1000000000), WJR_INC(1000000000), WJR_INC(1000000000), // 1024M
        WJR_INC(1000000000), WJR_INC(1000000000)                       // 4B
    };
    auto inc = table[clz(n | 1) ^ 31];
    return static_cast<int>((n + inc) >> 32);
#undef WJR_INC
}

inline int builtin_count_digits10_u64(uint64_t n) {
#define WJR_POWERS_OF_10(factor)                                                         \
    factor * 10, (factor)*100, (factor)*1000, (factor)*10000, (factor)*100000,           \
        (factor)*1000000, (factor)*10000000, (factor)*100000000, (factor)*1000000000
    static constexpr uint8_t bsr2log10[] = {
        1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
        6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,  10, 10, 10,
        10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15,
        15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20};
    auto t = bsr2log10[clz(n | 1) ^ 63];
    static constexpr const uint64_t zero_or_powers_of_10[] = {
        0, 0, WJR_POWERS_OF_10(1U), WJR_POWERS_OF_10(1000000000ULL),
        10000000000000000000ULL};
    return t - (n < zero_or_powers_of_10[t]);
#undef WJR_POWERS_OF_10
}

template <typename T>
WJR_CONSTEXPR_E int count_digits10_impl(T n) {
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(n)) {
        return fallback_count_digits10(n);
    }

    if constexpr (sizeof(T) <= sizeof(uint32_t)) {
        return builtin_count_digits10_u32(static_cast<uint32_t>(n));
    } else {
        return builtin_count_digits10_u64(static_cast<uint64_t>(n));
    }
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits10(T n) {
    int ret = count_digits10_impl(n);
    WJR_ASSUME(1 <= ret && ret <= std::numeric_limits<T>::digits10);
    return ret;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits_power_of_two(T n, int per_bit) {
    return (bit_width(n) + per_bit - 1) / per_bit;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits2(T n) {
    return bit_width(n);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits8(T n) {
    return count_digits_power_of_two(n, 3);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits16(T n) {
    return count_digits_power_of_two(n, 4);
}

template <typename Iter>
struct to_chars_result {
    Iter ptr;
    std::errc ec;

    friend bool operator==(const to_chars_result &lhs, const to_chars_result &rhs) {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    constexpr explicit operator bool() const { return ec == std::errc{}; }
};

template <typename Iter>
struct from_chars_result {
    Iter ptr;
    std::errc ec;

    friend bool operator==(const from_chars_result &lhs, const from_chars_result &rhs) {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    constexpr explicit operator bool() const { return ec == std::errc{}; }
};

template <typename Value, typename Converter>
struct __is_valid_chars_convert
    : std::disjunction<
          std::conjunction<std::is_same<remove_cvref_t<Converter>, char_converter_t>,
                           is_nonbool_integral<remove_cvref_t<Value>>>,
          std::conjunction<std::is_same<remove_cvref_t<Converter>, origin_converter_t>,
                           is_nonbool_unsigned_integral<remove_cvref_t<Value>>>> {};

template <typename Value, typename Converter>
inline constexpr bool __is_valid_chars_convert_v =
    __is_valid_chars_convert<Value, Converter>::value;

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_2_backward(Iter ptr, int n, UnsignedValue x,
                                    Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
    WJR_ASSERT(x != 0);
    WJR_ASSERT_ASSUME(1 <= n && n <= nd);
    (void)(nd);

    if (WJR_UNLIKELY(n >= 4)) {
        do {
            __to_chars_unroll_4<2>(ptr - 4, x & 0x0f, conv);
            ptr -= 4;
            x >>= 4;
            n -= 4;
        } while (WJR_LIKELY(n >= 4));

        if (n == 0) {
            return ptr;
        }
    }

    switch (n) {
    case 3: {
        *--ptr = conv.template to<2>(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<2>(ptr - 2, x, conv);
        ptr -= 2;
        break;
    }
    case 1: {
        *--ptr = conv.template to<2>(x);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    return ptr;
}

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_8_backward(Iter ptr, int n, UnsignedValue x,
                                    Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
    WJR_ASSERT(x != 0);
    WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 2) / 3);

    if constexpr (nd >= 16) {
        if (WJR_UNLIKELY(n >= 4)) {
            do {
                __to_chars_unroll_4<8>(ptr - 4, x & 0x0fff, conv);
                ptr -= 4;
                x >>= 12;
                n -= 4;
            } while (WJR_LIKELY(n >= 4));

            if (n == 0) {
                return ptr;
            }
        }
    }

    switch (n) {
    case 3: {
        *--ptr = conv.template to<8>(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<8>(ptr - 2, x, conv);
        ptr -= 2;
        break;
    }
    case 1: {
        *--ptr = conv.template to<8>(x);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    return ptr;
}

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_16_backward(Iter ptr, int n, UnsignedValue x,
                                     Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
    WJR_ASSERT(x != 0);
    WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 3) / 4);

    if constexpr (nd >= 16) {
        if (WJR_UNLIKELY(n >= 4)) {
            do {
                __to_chars_unroll_4<16>(ptr - 4, x & 0xffff, conv);
                ptr -= 4;
                x >>= 16;
                n -= 4;
            } while (WJR_LIKELY(n >= 4));

            if (n == 0) {
                return ptr;
            }
        }
    }

    switch (n) {
    case 3: {
        *--ptr = conv.to(x & 0x0f);
        x >>= 4;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<16>(ptr - 2, x, conv);
        ptr -= 2;
        break;
    }
    case 1: {
        *--ptr = conv.to(x);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    return ptr;
}

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_power_of_two_backward(Iter ptr, int n, UnsignedValue x,
                                               int per_bit, Converter conv = {}) {
    WJR_ASSERT(x != 0);
    WJR_ASSERT_ASSUME(1 <= n && n <= std::numeric_limits<UnsignedValue>::digits);

    unsigned int mask = (1u << per_bit) - 1;

    do {
        *--ptr = conv.to(x & mask);
        x >>= per_bit;
        --n;
    } while (WJR_LIKELY(n != 0));

    return ptr;
}

template <typename Iter, typename UnsignedValue, typename Converter,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_10_backward(Iter buf, UnsignedValue val, Converter conv) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits10;

    WJR_ASSERT_ASSUME(val != 0);

    if (val >= 100) {
#if WJR_HAS_BUILTIN(TO_CHARS_UNROLL_8_FAST)
        if constexpr (nd < 8) {
            // do nothing
        } else if constexpr (nd <= 9) {
            if (WJR_UNLIKELY(val >= 1000'0000)) {
                __to_chars_unroll_8<10>(buf - 8, val % 1'0000'0000, conv);
                buf -= 8;

                if (WJR_UNLIKELY(val <= 1'0000'0000)) {
                    return buf;
                }

                *--buf = conv.template to<10>(val / 1'0000'0000);
                return buf;
            }
        } else {
            if (WJR_UNLIKELY(val >= 1000'0000)) {
                do {
                    __to_chars_unroll_8<10>(buf - 8, val % 1'0000'0000, conv);
                    buf -= 8;
                    val /= 1'0000'0000;
                } while (val >= 1000'0000);

                if (WJR_LIKELY(val < 100)) {
                    if (WJR_UNLIKELY(val == 0)) {
                        return buf;
                    }

                    if (WJR_LIKELY(val >= 10)) {
                        __to_chars_unroll_2<10>(buf - 2, val, conv);
                        buf -= 2;
                        return buf;
                    }

                    *--buf = conv.template to<10>(val);
                    return buf;
                }
            }
        }
#endif

        do {
            __to_chars_unroll_2<10>(buf - 2, val % 100, conv);
            buf -= 2;
            val /= 100;
        } while (WJR_LIKELY(val >= 100));
    }

    if (WJR_LIKELY(val >= 10)) {
        __to_chars_unroll_2<10>(buf - 2, val, conv);
        buf -= 2;
        return buf;
    }

    *--buf = conv.template to<10>(val);
    return buf;
}

template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter> __to_chars_backward_validate_impl(Iter ptr, Iter first, Value val,
                                                        IBase ibase,
                                                        Converter conv = {}) {
    if (WJR_UNLIKELY(val == 0)) {
        if (WJR_LIKELY(first != ptr)) {
            *--ptr = conv.to(0);
            return {ptr, std::errc{}};
        }

        return {ptr, std::errc::value_too_large};
    }

    auto uVal = make_unsigned_value(val);
    auto size = std::distance(first, ptr);
    bool sign = false;

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            if (WJR_UNLIKELY(size <= 1)) {
                return {ptr, std::errc::value_too_large};
            }

            sign = true;
            uVal = -val;
            --size;
        }
    }

    const auto base = get_integral_constant_v(ibase);
    to_chars_result<Iter> ret;

    switch (base) {
    case 2: {
        int n = count_digits2(uVal);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_2_backward(ptr, n, uVal, conv), std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    case 8: {
        int n = count_digits8(uVal);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_8_backward(ptr, n, uVal, conv), std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    case 16: {
        int n = count_digits16(uVal);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_16_backward(ptr, n, uVal, conv), std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        int n = count_digits_power_of_two(uVal, per_bit);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_power_of_two_backward(ptr, n, uVal, per_bit, conv),
                   std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    case 10: {
        int n = count_digits10(uVal);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_10_backward(ptr, uVal, conv), std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    default: {
        WJR_UNREACHABLE();
        return {ptr, std::errc::invalid_argument};
    }
    }

    if constexpr (std::is_signed_v<Value>) {
        if (sign) {
            *--ret.ptr = '-';
        }
    }

    return ret;
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter>
to_chars_backward_validate(Iter ptr, Iter first, Value val,
                           std::integral_constant<BaseType, IBase> = {},
                           Converter conv = {}) {
    return __to_chars_backward_validate_impl(
        ptr, first, val, std::integral_constant<unsigned int, IBase>(), conv);
}

/**
 * @brief Convert an unsigned integer to a string in reverse order with checking buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 * @return to_chars_result<Iter> If the conversion is successful, return {ans,
 * std::errc{}}. Otherwise, return {ptr, std::errc::value_too_large}.
 *
 */
template <typename Iter, typename Value, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter> to_chars_backward_validate(Iter ptr, Iter first, Value val,
                                                 unsigned int base, Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_backward_validate(
                ptr, first, val, std::integral_constant<unsigned int, 2>{}, conv);
        }
        case 8: {
            return to_chars_backward_validate(
                ptr, first, val, std::integral_constant<unsigned int, 8>{}, conv);
        }
        case 16: {
            return to_chars_backward_validate(
                ptr, first, val, std::integral_constant<unsigned int, 16>{}, conv);
        }
        case 10: {
            return to_chars_backward_validate(
                ptr, first, val, std::integral_constant<unsigned int, 10>{}, conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_backward_validate_impl(ptr, first, val, base, conv);
}

template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter __to_chars_backward_impl(Iter ptr, Value val, IBase ibase, Converter conv = {}) {
    if (WJR_UNLIKELY(val == 0)) {
        *--ptr = conv.to(0);
        return ptr;
    }

    auto uVal = make_unsigned_value(val);
    bool sign = false;

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            sign = true;
            uVal = -val;
        }
    }

    unsigned int base = get_integral_constant_v(ibase);

    switch (base) {
    case 2: {
        ptr = __unsigned_to_chars_2_backward(ptr, count_digits2(uVal), uVal, conv);
        break;
    }
    case 8: {
        ptr = __unsigned_to_chars_8_backward(ptr, count_digits8(uVal), uVal, conv);
        break;
    }
    case 16: {
        ptr = __unsigned_to_chars_16_backward(ptr, count_digits16(uVal), uVal, conv);
        break;
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        ptr = __unsigned_to_chars_power_of_two_backward(
            ptr, count_digits_power_of_two(uVal, per_bit), uVal, per_bit, conv);
        break;
    }
    case 10: {
        ptr = __unsigned_to_chars_10_backward(ptr, uVal, conv);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        return ptr;
    }
    }

    if constexpr (std::is_signed_v<Value>) {
        if (sign) {
            *--ptr = '-';
        }
    }

    return ptr;
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter to_chars_backward(Iter ptr, Value val, std::integral_constant<BaseType, IBase> = {},
                       Converter conv = {}) {
    return __to_chars_backward_impl(ptr, val,
                                    std::integral_constant<unsigned int, IBase>(), conv);
}

/**
 * @brief Convert an unsigned integer to a string in reverse order without checking
 * buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 */
template <typename Iter, typename Value, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter to_chars_backward(Iter ptr, Value val, unsigned int base, Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_backward(ptr, val, std::integral_constant<unsigned int, 2>{},
                                     conv);
        }
        case 8: {
            return to_chars_backward(ptr, val, std::integral_constant<unsigned int, 8>{},
                                     conv);
        }
        case 16: {
            return to_chars_backward(ptr, val, std::integral_constant<unsigned int, 16>{},
                                     conv);
        }
        case 10: {
            return to_chars_backward(ptr, val, std::integral_constant<unsigned int, 10>{},
                                     conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_backward_impl(ptr, val, base, conv);
}

template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter> __to_chars_validate_impl(Iter ptr, Iter last, Value val,
                                               IBase ibase, Converter conv = {}) {
    if (WJR_UNLIKELY(val == 0)) {
        if (WJR_UNLIKELY(ptr == last)) {
            return {last, std::errc::value_too_large};
        }

        *ptr++ = conv.to(0);
        return {ptr, std::errc{}};
    }

    auto uVal = make_unsigned_value(val);

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            if (WJR_UNLIKELY(ptr == last)) {
                return {last, std::errc::value_too_large};
            }

            *ptr++ = '-';
            uVal = -val;
        }
    }

    const auto base = get_integral_constant_v(ibase);
    auto size = std::distance(ptr, last);

    switch (base) {
    case 2: {
        int n = count_digits2(uVal);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_2_backward(ptr, n, uVal, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    case 8: {
        int n = count_digits8(uVal);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_8_backward(ptr, n, uVal, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    case 16: {
        int n = count_digits16(uVal);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_16_backward(ptr, n, uVal, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        int n = count_digits_power_of_two(uVal, per_bit);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_power_of_two_backward(ptr, n, uVal, per_bit, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    case 10: {
        int n = count_digits10(uVal);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_10_backward(ptr, uVal, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    default: {
        WJR_UNREACHABLE();
        return {last, std::errc::invalid_argument};
    }
    }
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter> to_chars_validate(Iter ptr, Iter last, Value val,
                                        std::integral_constant<BaseType, IBase> = {},
                                        Converter conv = {}) {
    return __to_chars_validate_impl(ptr, last, val,
                                    std::integral_constant<unsigned int, IBase>(), conv);
}

/**
 * @brief Convert an unsigned integer to a string with checking buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 * @return to_chars_result<Iter> If the conversion is successful, return {ans,
 * std::errc{}}. Otherwise, return {last, std::errc::value_too_large}.
 *
 */
template <typename Iter, typename Value, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter> to_chars_validate(Iter ptr, Iter last, Value val, unsigned int base,
                                        Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 2>{}, conv);
        }
        case 8: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 8>{}, conv);
        }
        case 16: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 16>{}, conv);
        }
        case 10: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 10>{}, conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_validate_impl(ptr, last, val, base, conv);
}

template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter __to_chars_impl(Iter ptr, Value val, IBase ibase, Converter conv = {}) {
    if (WJR_UNLIKELY(val == 0)) {
        *ptr++ = conv.to(0);
        return ptr;
    }

    auto uVal = make_unsigned_value(val);

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            *ptr++ = '-';
            uVal = -val;
        }
    }

    const auto base = get_integral_constant_v(ibase);

    switch (base) {
    case 2: {
        int n = count_digits2(uVal);
        ptr += n;
        (void)__unsigned_to_chars_2_backward(ptr, n, uVal, conv);
        return ptr;
    }
    case 8: {
        int n = count_digits8(uVal);
        ptr += n;
        (void)__unsigned_to_chars_8_backward(ptr, n, uVal, conv);
        return ptr;
    }
    case 16: {
        int n = count_digits16(uVal);
        ptr += n;
        (void)__unsigned_to_chars_16_backward(ptr, n, uVal, conv);
        return ptr;
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        int n = count_digits_power_of_two(uVal, per_bit);
        ptr += n;
        (void)__unsigned_to_chars_power_of_two_backward(ptr, n, uVal, per_bit, conv);
        return ptr;
    }
    case 10: {
        int n = count_digits10(uVal);
        ptr += n;
        (void)__unsigned_to_chars_10_backward(ptr, uVal, conv);
        return ptr;
    }
    default: {
        WJR_UNREACHABLE();
        return ptr;
    }
    }
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter to_chars(Iter ptr, Value val, std::integral_constant<BaseType, IBase> = {},
              Converter conv = {}) {
    return __to_chars_impl(ptr, val, std::integral_constant<unsigned int, IBase>(), conv);
}

/**
 * @brief Convert an unsigned integer to a string without checking buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 */
template <typename Iter, typename Value, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter to_chars(Iter ptr, Value val, unsigned int base, Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 2>{}, conv);
        }
        case 8: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 8>{}, conv);
        }
        case 16: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 16>{}, conv);
        }
        case 10: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 10>{}, conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_impl(ptr, val, base, conv);
}

template <typename Iter, typename Converter>
size_t __biginteger_to_chars_2_impl(Iter first, uint64_t *up, size_t n, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    size_t len = hbits + 64 * n;
    first += len;

    do {
        x = *up;

        for (int i = 0; i < 8; ++i) {
            __to_chars_unroll_8<2>(first - 8, x & 0xff, conv);
            first -= 8;
            x >>= 8;
        }

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)__unsigned_to_chars_2_backward(first, hbits, x, conv);
    return len;
}

template <typename Iter, typename Converter>
size_t __biginteger_to_chars_8_impl(Iter first, uint64_t *up, size_t n, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    size_t len = (hbits + 64 * n + 2) / 3;
    first += len;

    int rest = 0;
    unsigned int last = 0;

    do {
        x = *up;

        switch (rest) {
        case 0: {
            rest = 2;
            break;
        }
        case 2: {
            __to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4), conv);
            first -= 2;
            x >>= 2;
            rest = 4;
            break;
        }
        case 4: {
            __to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2), conv);
            first -= 2;
            x >>= 4;
            rest = 0;
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        __to_chars_unroll_8<8>(first - 8, x & 0xff'ffff, conv);
        x >>= 24;
        __to_chars_unroll_8<8>(first - 16, x & 0xff'ffff, conv);
        x >>= 24;
        __to_chars_unroll_4<8>(first - 20, x & 0x0fff, conv);
        x >>= 12;
        first -= 20;

        last = x;

        ++up;
        --n;
    } while (n);
    x = *up;

    switch (rest) {
    case 0: {
        break;
    }
    case 2: {
        __to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4), conv);
        first -= 2;
        if (hbits <= 2) {
            goto DONE;
        }
        hbits -= 2;
        x >>= 2;
        break;
    }
    case 4: {
        if (WJR_UNLIKELY(hbits == 1)) {
            *--first = conv.to(x << 2 | last);
            goto DONE;
        }

        __to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2), conv);
        first -= 2;
        if (hbits <= 4) {
            goto DONE;
        }
        hbits -= 4;
        x >>= 4;
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    if (WJR_LIKELY(hbits + 2 >= 12)) {
        do {
            __to_chars_unroll_4<8>(first - 4, x & 0x0fff, conv);
            first -= 4;
            x >>= 12;
            hbits -= 12;
        } while (WJR_LIKELY(hbits + 2 >= 12));
    }

    switch ((hbits + 2) / 3) {
    case 3: {
        *--first = conv.to(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<8>(first - 2, x, conv);
        break;
    }
    case 1: {
        *--first = conv.to(x);
        WJR_FALLTHROUGH;
    }
    case 0: {
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

DONE:
    return len;
}

template <typename Iter, typename Converter>
size_t __biginteger_to_chars_16_impl(Iter first, uint64_t *up, size_t n, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    hbits = (hbits + 3) / 4;

    size_t len = hbits + 16 * n;
    first += len;

    do {
        x = *up;

        __to_chars_unroll_8<16>(first - 8, x & 0xffff'ffff, conv);
        __to_chars_unroll_8<16>(first - 16, x >> 32, conv);
        first -= 16;

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)__unsigned_to_chars_16_backward(first, hbits, x, conv);

    return len;
}

template <typename Iter, typename Converter>
size_t __biginteger_to_chars_power_of_two_impl(Iter first, uint64_t *up, size_t n,
                                               unsigned int base, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    int per_bit = ctz(base);
    unsigned int mask = (1u << per_bit) - 1;

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    size_t len = (hbits + 64 * n + per_bit - 1) / per_bit;
    first += len;

    int rest = 0;
    unsigned int last = 0;

    do {
        x = *up;

        if (rest) {
            int fix = per_bit - rest;
            unsigned int val = ((x & ((1u << fix) - 1)) << rest) | last;
            x >>= fix;
            rest = 64 - fix;
            *--first = conv.to(val);
        } else {
            rest = 64;
        }

        do {
            *--first = conv.to(x & mask);
            x >>= per_bit;
            rest -= per_bit;
        } while (rest >= per_bit);

        last = x;

        ++up;
        --n;
    } while (n);
    x = *up;

    WJR_ASSERT_ASSUME(rest < per_bit);

    if (WJR_UNLIKELY(rest != 0)) {
        int fix = per_bit - rest;
        unsigned int val = ((x & ((1u << fix) - 1)) << rest) | last;
        x >>= fix;
        *--first = conv.to(val);
        rest = hbits - fix;
        if (WJR_UNLIKELY(rest == 0)) {
            goto DONE;
        }
    } else {
        rest = hbits;
    }

    do {
        *--first = conv.to(x & mask);
        x >>= per_bit;
        rest -= per_bit;
    } while (WJR_LIKELY(rest > 0));

DONE:
    return len;
}

template <typename Converter>
char *basecase_to_chars_10(char *buf, uint64_t *up, size_t n, Converter conv) {
    do {
        if (WJR_UNLIKELY(n == 1)) {
            return __unsigned_to_chars_10_backward(buf, up[0], conv);
        }

        uint64_t q, rem;

        q = div_qr_1_noshift(up, rem, up, n, div2by1_divider_noshift_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }

        __to_chars_unroll_8<10>(buf - 8, rem % 1'0000'0000, conv);
        rem /= 1'0000'0000;
        __to_chars_unroll_8<10>(buf - 16, rem % 1'0000'0000, conv);
        rem /= 1'0000'0000;
        __to_chars_unroll_2<10>(buf - 18, rem % 100, conv);
        rem /= 100;
        buf[-19] = conv.to(rem);

        buf -= 19;
    } while (n);

    return buf;
}

template <typename Iter, typename Converter>
Iter basecase_to_chars(Iter first, size_t len, uint64_t *up, size_t n, unsigned int base,
                       Converter conv) {
    constexpr size_t buf_len = dc_bignum_to_chars_precompute_threshold * 64 * 7 / 11;
    char buf[buf_len];
    char *end = buf + buf_len;
    char *start;

    if (WJR_LIKELY(base == 10)) {
        start = basecase_to_chars_10(end, up, n, conv);
    } else {
        start = end;
    }

    size_t rlen = end - start;
    if (len > rlen) {
        first = std::fill_n(first, len - rlen, conv.to(0));
    }

    return std::copy(start, end, first);
}

template <typename Iter, typename Converter>
Iter dc_to_chars(Iter first, size_t len, uint64_t *up, size_t n,
                 precompute_chars_convert_t *pre, uint64_t *stk, Converter conv) {
    WJR_ASSERT_ASSUME(n >= 1);
    if (n < dc_bignum_to_chars_threshold) {
        return basecase_to_chars(first, len, up, n, pre->base, conv);
    } else {
        const auto pp = pre->ptr;
        const auto pn = pre->n;
        const auto ps = pre->shift;

        WJR_ASSERT((pn + ps) * 5 >= n * 2);

        if (n < pn + ps || (n == pn + ps && reverse_compare_n(up + ps, pp, pn) < 0)) {
            return dc_to_chars(first, len, up, n, pre - 1, stk, conv);
        }

        const auto pd = pre->digits_in_base;
        auto qp = stk;

        div_qr_s(qp, up + ps, up + ps, n - ps, pp, pn);

        size_t qn = n - pn - ps;
        qn += qp[qn] != 0;

        if (len != 0) {
            len = len - pd;
        }

        pre -= qn * 2 <= n;

        first = dc_to_chars(first, len, qp, qn, pre, stk + qn, conv);
        first = dc_to_chars(first, pd, up, pn + ps, pre, stk, conv);
        return first;
    }
}

template <typename Iter, typename Converter>
Iter __large_basecase_to_chars(Iter first, uint64_t *up, size_t n, unsigned int base,
                               Converter conv) {
    if (WJR_LIKELY(n < dc_bignum_to_chars_precompute_threshold)) {
        uint64_t upbuf[dc_bignum_to_chars_precompute_threshold];
        std::copy_n(up, n, upbuf);
        return basecase_to_chars(first, 0, upbuf, n, base, conv);
    }

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal(math_details::stack_alloc);
    auto stk =
        static_cast<uint64_t *>(stkal.allocate((n * 18 / 5 + 192) * sizeof(uint64_t)));
    auto __up = stk;
    std::copy_n(up, n, __up);
    stk += n;
    auto mpre = precompute_chars_convert(pre, n, base, stk);
    stk += n * 8 / 5 + 128;
    return dc_to_chars(first, 0, __up, n, mpre, stk, conv);
}

/**
 * @brief Use by `to_chars` to convert a large integer to a string.
 *
 * @note The length of the biginteger is greater than or equal to 2.
 */
template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
Iter __biginteger_to_chars_impl(Iter first, uint64_t *up, size_t n,
                                unsigned int base = 10, Converter conv = {}) {
    switch (base) {
    case 2: {
        return first + __biginteger_to_chars_2_impl(first, up, n, conv);
    }
    case 8: {
        return first + __biginteger_to_chars_8_impl(first, up, n, conv);
    }
    case 16: {
        return first + __biginteger_to_chars_16_impl(first, up, n, conv);
    }
    case 4:
    case 32: {
        return first + __biginteger_to_chars_power_of_two_impl(first, up, n, base, conv);
    }
    default: {
        break;
    }
    }

    return __large_basecase_to_chars(first, up, n, base, conv);
}

/**
 * @brief Convert a biginteger to a string by a given base.
 *
 * @tparam Iter Output iterator type
 * @tparam Converter char_converter_t or origin_converter_t. The default type is
 * char_converter_t.
 * @param[out] first Output iterator
 * @param[in] up Pointer to the biginteger
 * @param[in] n Length of the biginteger
 * @param[in] base Base of the output string. Range: `[2, 36]`,
 * Only support 10 and power of two currently.
 * @param[in] conv Converter, only support char_converter_t or origin_converter_t. The
 * default value is char_converter.
 * @return Output iterator after the conversion
 */
template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
Iter biginteger_to_chars(Iter first, uint64_t *up, size_t n, unsigned int base = 10,
                         Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));
    WJR_ASSERT_ASSUME(up[n - 1] != 0);

    if (n == 1) {
        return to_chars(first, up[0], base, conv);
    }

    return __biginteger_to_chars_impl(first, up, n, base, conv);
}

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
void __unsigned_from_chars_2(Iter first, Iter last, UnsignedValue &val, Converter conv) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

    auto n = std::distance(first, last);
    WJR_ASSERT_ASSUME(1 <= n && n <= nd);

    if constexpr (nd >= 16) {
        if (WJR_UNLIKELY(n >= 8)) {
            do {
                val = (val << 8) + __from_chars_unroll_8<2>(first, conv);
                first += 8;
                n -= 8;
            } while (WJR_LIKELY(n >= 8));

            if (n == 0) {
                return;
            }
        }
    } else if constexpr (nd == 8) {
        if (WJR_UNLIKELY(n == 8)) {
            val = __from_chars_unroll_8<2>(first, conv);
            first += 8;
            return;
        }
    }

    switch (n) {
    case 7: {
        val = (val << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        val = (val << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        val = (val << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        val <<= 4;
        val += __from_chars_unroll_4<2>(first, conv);
        first += 4;
        break;
    }
    case 3: {
        val = (val << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        val = (val << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        val = (val << 1) + conv.from(*first++);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }
}

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
void __unsigned_from_chars_8(Iter first, Iter last, UnsignedValue &val,
                             Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

    auto n = std::distance(first, last);
    WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 2) / 3);

    if constexpr (nd >= 32) {
        if (WJR_UNLIKELY(n >= 8)) {
            do {
                val = (val << 24) + __from_chars_unroll_8<8>(first, conv);
                first += 8;
                n -= 8;
            } while (WJR_LIKELY(n >= 8));

            if (n == 0) {
                return;
            }
        }
    }

    switch (n) {
    case 7: {
        val = (val << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        val = (val << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        val = (val << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        val <<= 12;
        val += __from_chars_unroll_4<8>(first, conv);
        first += 4;
        break;
    }
    case 3: {
        val = (val << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        val = (val << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        val = (val << 3) + conv.from(*first++);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }
}

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
void __unsigned_from_chars_16(Iter first, Iter last, UnsignedValue &val,
                              Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

    auto n = std::distance(first, last);
    WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 3) / 4);

    if constexpr (nd >= 64) {
        if (WJR_UNLIKELY(n >= 8)) {
            do {
                val = (val << 32) + __from_chars_unroll_8<16>(first, conv);
                first += 8;
                n -= 8;
            } while (WJR_LIKELY(n >= 8));

            if (n == 0) {
                return;
            }
        }
    } else if constexpr (nd == 32) {
        if (WJR_UNLIKELY(n == 8)) {
            val = __from_chars_unroll_8<16>(first, conv);
            first += 8;
            return;
        }
    }

    switch (n) {
    case 7: {
        val = (val << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        val = (val << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        val = (val << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        val <<= 16;
        val += __from_chars_unroll_4<16>(first, conv);
        first += 4;
        break;
    }
    case 3: {
        val = (val << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        val = (val << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        val = (val << 4) + conv.from(*first++);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }
}

template <typename Iter, typename UnsignedValue, typename Converter,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
void __unsigned_from_chars_10(Iter first, Iter last, UnsignedValue &val, Converter conv) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits10;

    auto n = std::distance(first, last);
    WJR_ASSUME(1 <= n && n <= nd);

    if constexpr (nd >= 8) {
        if (WJR_UNLIKELY(n >= 8)) {
            if (WJR_UNLIKELY(n >= 16)) {
                val = __from_chars_unroll_16<10>(first, conv);
                first += 16;
                n -= 16;
                WJR_ASSUME(n <= nd - 16);
            } else {
                val = __from_chars_unroll_8<10>(first, conv);
                first += 8;
                n -= 8;
            }
        }
    }

    switch (n) {
    case 7: {
        val = val * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        val = val * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        val = val * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        val = (val * 10000) + __from_chars_unroll_4<10>(first, conv);
        break;
    }
    case 3: {
        val = val * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        val = val * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        val = val * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 0: {
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }
}

template <typename Iter, typename UnsignedValue, typename Converter,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
bool __unsigned_from_chars_10_validate(Iter &first, Iter last, UnsignedValue &val,
                                       Converter conv) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits10;
    static_assert(nd < 24, "The number of digits of UnsignedValue must be less than 24.");
    constexpr uint32_t table[8] = {1, 10, 1000, 1'0000, 10'0000, 100'0000, 1000'0000};

    if (WJR_UNLIKELY(first + 8 <= last)) {
        do {
            uint32_t xval = 0;
            auto ret = __from_chars_validate_unroll_8<10>(first, xval, conv);

            if (WJR_UNLIKELY(!ret.ok)) {
                if (WJR_UNLIKELY(mul_overflow(val, table[ret.offset], val) ||
                                 add_overflow(val, xval, val))) {
                    return false;
                }

                return true;
            }

            if (WJR_UNLIKELY(mul_overflow(val, 1'0000'0000, val) ||
                             add_overflow(val, xval, val))) {
                while (first != last && conv.from(*first) < 10) {
                    ++first;
                }

                return false;
            }

        } while (first + 8 <= last);
    }

    while (first != last) {
        uint8_t ch = conv.from(*first);
        if (WJR_UNLIKELY(ch >= 10)) {
            return true;
        }

        ++first;
        if (WJR_UNLIKELY(mul_overflow(val, 10, val) || add_overflow(val, ch, val))) {
            while (first != last && conv.from(*first) < 10) {
                ++first;
            }

            return false;
        }
    }

    return true;
}

template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
void __from_chars_impl(Iter first, Iter last, Value &val, IBase ibase,
                       Converter conv = {}) {
    bool sign = false;

    if constexpr (std::is_signed_v<Value>) {
        WJR_ASSERT(first != last);

        if (*first == '-') {
            ++first;
            sign = true;
        }
    }

    std::make_unsigned_t<Value> uVal = 0;

    const auto base = get_integral_constant_v(ibase);

    switch (base) {
    case 2: {
        __unsigned_from_chars_2(first, last, uVal, conv);
        break;
    }
    case 8: {
        __unsigned_from_chars_8(first, last, uVal, conv);
        break;
    }
    case 16: {
        __unsigned_from_chars_16(first, last, uVal, conv);
        break;
    }
    case 10: {
        __unsigned_from_chars_10(first, last, uVal, conv);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    if (sign) {
        val = -static_cast<Value>(uVal);
    } else {
        val = static_cast<Value>(uVal);
    }
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
void from_chars(Iter first, Iter last, Value &val,
                std::integral_constant<BaseType, IBase> = {}, Converter conv = {}) {
    __from_chars_impl(first, last, val, std::integral_constant<unsigned int, IBase>(),
                      conv);
}

template <typename Iter, typename Value, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
void from_chars(Iter first, Iter last, Value &val, unsigned int base,
                Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            __from_chars_impl(first, last, val, std::integral_constant<unsigned int, 2>(),
                              conv);
            return;
        }
        case 8: {
            __from_chars_impl(first, last, val, std::integral_constant<unsigned int, 8>(),
                              conv);
            return;
        }
        case 16: {
            __from_chars_impl(first, last, val,
                              std::integral_constant<unsigned int, 16>(), conv);
            return;
        }
        case 10: {
            __from_chars_impl(first, last, val,
                              std::integral_constant<unsigned int, 10>(), conv);
            return;
        }
        default: {
            break;
        }
        }
    }

    __from_chars_impl(first, last, val, base, conv);
}

template <typename Iter, typename Converter = char_converter_t>
size_t __biginteger_from_chars_2_impl(Iter first, size_t n, uint64_t *up,
                                      Converter conv = {}) {
    size_t hbits = (n - 1) % 64 + 1;
    size_t len = (n - 1) / 64 + 1;

    uint64_t x = 0;
    up += len;

    __unsigned_from_chars_2(first, first + hbits, x, conv);
    first += hbits;

    *--up = x;

    size_t idx = len - 1;

    if (idx) {
        do {
            x = 0;

            for (int i = 0; i < 4; ++i) {
                x = (x << 16) + __from_chars_unroll_16<2>(first, conv);
                first += 16;
            }

            *--up = x;
        } while (WJR_LIKELY(--idx));
    }

    return len;
}

template <typename Iter, typename Converter = char_converter_t>
size_t __biginteger_from_chars_8_impl(Iter first, size_t n, uint64_t *up,
                                      Converter conv = {}) {
    size_t len = (n * 3 + 63) / 64;
    size_t lbits = (64 * (len - 1)) / 3;
    size_t rest = (64 * (len - 1)) % 3;
    size_t hbits = n - lbits - 1;

    auto unroll = [conv](uint64_t &x, auto &first) {
        auto x0 = conv.from(first[0]);
        auto x1 = conv.from(first[1]);
        auto x2 = conv.from(first[2]);
        auto x3 = conv.from(first[3]);

        x = x << 12 | x0 << 9 | x1 << 6 | x2 << 3 | x3;
        first += 4;
    };

    uint64_t x = 0;
    up += len;
    size_t idx = len - 1;

    if (WJR_UNLIKELY(hbits == 0)) {
    } else {
        __unsigned_from_chars_8(first, first + hbits, x, conv);
        first += hbits;
    }

    uint64_t nx = conv.from(*first++);
    switch (rest) {
    case 0: {
        *--up = x << 3 | nx;
        x = 0;
        break;
    }
    case 1: {
        x = x << 2 | nx >> 1;
        if (WJR_UNLIKELY(x == 0)) {
            --len;
        }

        *--up = x;
        x = nx & 1;
        break;
    }
    case 2: {
        x = x << 1 | nx >> 2;
        if (WJR_UNLIKELY(x == 0)) {
            --len;
        }
        *--up = x;
        x = nx & 3;
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    if (idx) {
        do {
            for (int i = 0; i < 5; ++i) {
                unroll(x, first);
            }

            switch (rest) {
            case 0: {
                x = x << 3 | conv.from(*first++);
                uint64_t nx = conv.from(*first++);
                x = x << 1 | nx >> 2;
                *--up = x;
                x = nx & 3;
                rest = 2;
                break;
            }
            case 1: {
                x = x << 3 | conv.from(*first++);
                *--up = x;
                x = 0;
                rest = 0;
                break;
            }
            case 2: {
                uint64_t nx = conv.from(*first++);
                x = x << 2 | nx >> 1;
                *--up = x;
                x = nx & 1;
                rest = 1;
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }

        } while (WJR_LIKELY(--idx));
    }

    return len;
}

template <typename Iter, typename Converter = char_converter_t>
size_t __biginteger_from_chars_16_impl(Iter first, size_t n, uint64_t *up,
                                       Converter conv = {}) {
    size_t hbits = (n - 1) % 16 + 1;
    size_t len = (n - 1) / 16 + 1;

    auto unroll = [conv](uint64_t &x, auto &first) {
        auto x0 = conv.from(first[0]);
        auto x1 = conv.from(first[1]);
        auto x2 = conv.from(first[2]);
        auto x3 = conv.from(first[3]);

        x = x << 16 | x0 << 12 | x1 << 8 | x2 << 4 | x3;
        first += 4;
    };

    uint64_t x = 0;
    up += len;

    __unsigned_from_chars_16(first, first + hbits, x, conv);
    first += hbits;

    *--up = x;

    size_t idx = len - 1;

    if (idx) {
        do {
            x = 0;

            for (int i = 0; i < 4; ++i) {
                unroll(x, first);
            }

            *--up = x;
        } while (WJR_LIKELY(--idx));
    }

    return len;
}

template <typename Iter, typename Converter>
size_t basecase_from_chars_10(Iter first, size_t n, uint64_t *up, Converter conv) {
    uint64_t x = 0;

    if (n <= 19) {
        __unsigned_from_chars_10(first, first + n, x, conv);
        up[0] = x;

        return up[0] != 0;
    }

    size_t m = (n - 1) % 19 + 1;

    __unsigned_from_chars_10(first, first + m, x, conv);
    up[0] = x;

    first += m;
    n -= m;

    m = up[0] != 0;

    do {
        x = 0;

        x = __from_chars_unroll_16<10>(first, conv);
        first += 16;

        x = x * 10 + conv.from(*first++);
        x = x * 10 + conv.from(*first++);
        x = x * 10 + conv.from(*first++);

        uint64_t cf;

        if (WJR_LIKELY(m == 0)) {
            cf = x;
        } else {
            cf = mul_1(up, up, m, div2by1_divider_noshift_of_big_base_10.get_divisor());
            cf += addc_1(up, up, m, x);
        }

        if (WJR_LIKELY(cf != 0)) {
            up[m++] = cf;
        }

        n -= 19;
    } while (WJR_LIKELY(n != 0));

    return m;
}

template <typename Iter, typename Converter>
size_t basecase_from_chars(Iter first, size_t n, uint64_t *up, unsigned int base,
                           Converter conv) {
    if (base == 10) {
        return basecase_from_chars_10(first, n, up, conv);
    } else {
        return 0;
    }
}

template <typename Iter, typename Converter>
size_t dc_from_chars(Iter first, size_t n, uint64_t *up, precompute_chars_convert_t *pre,
                     uint64_t *stk, Converter conv) {
    size_t lo = pre->digits_in_base;
    if (n <= lo) {
        if (n < dc_bignum_from_chars_threshold) {
            return basecase_from_chars(first, n, up, pre->base, conv);
        } else {
            return dc_from_chars(first, n, up, pre - 1, stk, conv);
        }
    }

    size_t hi = n - lo;
    size_t hn, ln;

    if (hi < dc_bignum_from_chars_threshold) {
        hn = basecase_from_chars(first, hi, stk, pre->base, conv);
    } else {
        hn = dc_from_chars(first, hi, stk, pre - (lo * 2 >= n), up, conv);
    }

    size_t ps = pre->shift;
    size_t pn = pre->n;

    if (WJR_LIKELY(hn != 0)) {
        if (pn >= hn) {
            mul_s(up + ps, pre->ptr, pn, stk, hn);
        } else {
            mul_s(up + ps, stk, hn, pre->ptr, pn);
        }
        set_n(up, 0, ps);
    }

    std::advance(first, hi);
    if (lo < dc_bignum_from_chars_threshold) {
        ln = basecase_from_chars(first, lo, stk, pre->base, conv);
    } else {
        ln = dc_from_chars(first, lo, stk, pre - (lo * 2 >= n), stk + pn + ps + 1, conv);
    }

    WJR_ASSERT(ps + pn + 1 >= ln);

    if (WJR_LIKELY(hn != 0)) {
        if (WJR_LIKELY(ln != 0)) {
            auto cf = addc_s(up, up, ps + pn + hn, stk, ln);
            WJR_ASSERT(cf == 0);
            (void)(cf);
        }

        n = ps + pn + hn;
        return n - (up[n - 1] == 0);
    }

    if (WJR_LIKELY(ln != 0)) {
        std::copy_n(stk, ln, up);
    }

    return ln;
}

/**
 * @brief Convert a string to a biginteger by a given base.
 *
 * @tparam Iter Input iterator type
 * @tparam Converter char_converter_t or origin_converter_t. The default type is
 * char_converter_t.
 * @param[in] first Input iterator
 * @param[in] last Input iterator
 * @param[out] up Pointer to the biginteger
 * @param[in] base Base of the input string. Range: `[2, 36]`,
 * Only support 10 and power of two currently.
 * @param[in] conv Converter, only support char_converter_t or origin_converter_t. The
 * default value is char_converter.
 * @return uint64_t* Pointer after the conversion
 */
template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
uint64_t *biginteger_from_chars(Iter first, Iter last, uint64_t *up,
                                unsigned int base = 10, Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));
    size_t n = std::distance(first, last);

    if (is_zero_or_single_bit(base)) {
        switch (base) {
        case 2: {
            return up + __biginteger_from_chars_2_impl(first, n, up, conv);
        }
        case 8: {
            return up + __biginteger_from_chars_8_impl(first, n, up, conv);
        }
        case 16: {
            return up + __biginteger_from_chars_16_impl(first, n, up, conv);
        }
        default: {
            WJR_UNREACHABLE();
            return up;
        }
        }
    }

    if (WJR_LIKELY(n < dc_bignum_from_chars_precompute_threshold)) {
        return up + basecase_from_chars(first, n, up, base, conv);
    }

    const auto per_digits = precompute_chars_convert_16n_ptr[base]->digits_in_one_base;

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal(math_details::stack_alloc);
    size_t un = n / per_digits + 1;
    auto stk =
        static_cast<uint64_t *>(stkal.allocate((un * 16 / 5 + 192) * sizeof(uint64_t)));
    auto mpre = precompute_chars_convert(pre, un, base, stk);
    stk += un * 8 / 5 + 128;
    return up + dc_from_chars(first, n, up, mpre, stk, conv);
}

} // namespace wjr

#endif // WJR_MATH_CONVERT_HPP__

#endif // WJR_MATH_HPP__
#ifndef WJR_VECTOR_HPP__
#define WJR_VECTOR_HPP__

#ifndef WJR_CONTAINER_GENERIC_CONTAINER_VECTOR_HPP__
#define WJR_CONTAINER_GENERIC_CONTAINER_VECTOR_HPP__

/**
 * @file vector.hpp
 * @brief Vector container with definable internal structure
 *
 * @details
 * Customized internal structure needs to follow the following function signature: \n
 * -# storage()
 * -# template <typename _Alloc>  \n
 * storage(_Alloc&& al)
 * -# template <typename _Alloc> \n
 * storage(_Alloc&& al, size_type size, size_type capacity, in_place_reallocate_t)
 * -# ~storage() noexcept
 * -# auto& get_allocator() noexcept
 * -# const auto& get_allocator() const noexcept
 * -# void destroy() noexcept
 * -# void destroy_and_deallocate()
 * -# void uninitialized_construct(size_type size, size_type capacity)
 * -# void take_storage(storage&& other)
 * -# void swap_storage(storage& other)
 * -# decltype(auto) size() noexcept
 * -# size_type capacity() const noexcept
 * -# pointer data() noexcept
 * -# const_pointer data() const noexcept
 *
 * 1 : should not allocate memory. \n
 * 2 : just store the allocator. \n
 * 3 : allocate memory and set the size and capacity. this function is used for
 * reallocation. if the storage is not reallocatable, this function won't be
 * implemented. \n
 * 4 : don't need to destroy or deallocate. \n
 * 5-6 : \n
 * 7 : destroy all elements. don't change ptr, size and capacity. \n
 * 8 : destroy and deallocate. \n
 * 9 : uninitialized construct the storage. allocate memory and set the size and
 * capacity. \n
 * 10 : take the storage from other. set other to empty. \n
 * 11 : swap the storage with other. \n
 * 12 : get the size. the return type must be reference,
 * such as size_type&, std::reference_wrapper<size_type> and so on. \n
 * 13 : get the capacity. \n
 * 14-15 : get the pointer. \n
 *
 * the size type of 14 need to implement the following function signature: \n
 * -# auto& operator=(size_type) noexcept
 * -# operator size_type() const noexcept
 * -# size_type operator++() noexcept
 * -# size_type operator--() noexcept
 * -# size_type operator+=(size_type) noexcept
 * -# size_type operator-=(size_type) noexcept
 *
 * @version 0.1
 * @date 2024-03-19
 *
 */

// Already included
// Already included
#ifndef WJR_CONTAINER_GENERIC_CONTAINER_TRAITS_HPP__
#define WJR_CONTAINER_GENERIC_CONTAINER_TRAITS_HPP__

// Already included

namespace wjr {

/**
 * @class container_traits<Alloc>
 * @brief The same characteristics and behavior of all allocator containers
 *
 * @details container must have the following member functions:
 * -# auto& __get_allocator() noexcept
 * -# void __destroy() noexcept
 * -# void __destroy_and_deallocate() noexcept
 * -# void __copy_element(const container& other)
 * -# void __take_storage(container&& other)
 * -# void __move_element(container&& other)
 * -# void __swap_storage(container& other)
 *
 * 1 : is used to manage the allocator of the container. \n
 * 2-3 : is used to destroy the container and deallocate the memory. \n
 * 4-7 : is used to assign the container data. Shouldn't change the allocator.
 *
 */
template <typename Alloc>
class container_traits {
private:
    using allocator_type = Alloc;
    using allocator_traits = std::allocator_traits<allocator_type>;
    using is_always_equal = typename allocator_traits::is_always_equal;
    using propagate_on_container_copy_assignment =
        typename allocator_traits::propagate_on_container_copy_assignment;
    using propagate_on_container_move_assignment =
        typename allocator_traits::propagate_on_container_move_assignment;
    using propagate_on_container_swap =
        typename allocator_traits::propagate_on_container_swap;

public:
    template <typename Container>
    WJR_CONSTEXPR20 static void copy_assign(Container &lhs, const Container &rhs) {
        if constexpr (propagate_on_container_copy_assignment::value) {
            auto &lhs_allocator = lhs.__get_allocator();
            auto &rhs_allocator = rhs.__get_allocator();
            if constexpr (!is_always_equal::value) {
                if (lhs_allocator != rhs_allocator) {
                    lhs.__destroy_and_deallocate();
                }
            }

            lhs_allocator = rhs_allocator;
        }

        lhs.__copy_element(rhs);
    }

    template <typename Container>
    WJR_CONSTEXPR20 static void move_assign(Container &lhs, Container &&rhs) {
        if constexpr (std::disjunction_v<propagate_on_container_move_assignment,
                                         is_always_equal>) {
            lhs.__destroy_and_deallocate();
            if constexpr (propagate_on_container_move_assignment::value) {
                lhs.__get_allocator() = std::move(rhs.__get_allocator());
            }
            lhs.__take_storage(std::move(rhs));
        } else {
            if (lhs.__get_allocator() != rhs.__get_allocator()) {
                lhs.__destroy();
                lhs.__move_element(std::move(rhs));
            } else {
                lhs.__destroy_and_deallocate();
                lhs.__take_storage(std::move(rhs));
            }
        }
    }

    template <typename Container>
    WJR_CONSTEXPR20 static void swap(Container &lhs, Container &rhs) {
        if constexpr (std::conjunction_v<propagate_on_container_swap,
                                         std::negation<is_always_equal>>) {
            auto &lhs_allocator = lhs.__get_allocator();
            auto &rhs_allocator = rhs.__get_allocator();
            if (lhs_allocator != rhs_allocator) {
                std::swap(lhs_allocator, rhs_allocator);
            }
        }

        lhs.__swap_storage(rhs);
    }
};

namespace try_reserve_details {

template <typename Enable, typename Container, typename Size, typename... Args>
struct __has_container_reserve : std::false_type {};
template <typename Container, typename Size, typename... Args>
struct __has_container_reserve<
    std::void_t<decltype(std::declval<Container>().reserve(std::declval<Size>()))>,
    Container, Size, Args...> : std::true_type {};
template <typename Container, typename Size, typename... Args>
struct has_container_reserve : __has_container_reserve<void, Container, Size, Args...> {};
template <typename Container, typename Size, typename... Args>
constexpr bool has_container_reserve_v =
    has_container_reserve<Container, Size, Args...>::value;

} // namespace try_reserve_details

template <typename Container, typename Size>
void try_reserve(Container &c, Size s) {
    if constexpr (try_reserve_details::has_container_reserve_v<Container, Size>) {
        c.reserve(s);
    }
}

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_CONTAINER_TRAITS_HPP__
#ifndef WJR_MEMORY_TEMPORARY_VALUE_ALLOCATOR_HPP__
#define WJR_MEMORY_TEMPORARY_VALUE_ALLOCATOR_HPP__

// Already included
#ifndef WJR_MEMORY_UNINITIALIZED_HPP__
#define WJR_MEMORY_UNINITIALIZED_HPP__

/**
 * @file uninitialized.hpp
 * @brief The header file for uninitialized memory operations using allocator.
 *
 * @version 0.0.1
 * @date 2024-03-18
 *
 */

// Already included
// Already included

namespace wjr {

template <typename Iter, typename Alloc, typename... Args>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             Args &&...args) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        using value_type = typename std::iterator_traits<Iter>::value_type;
        ::new (static_cast<void *>(to_address(iter)))
            value_type(std::forward<Args>(args)...);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, to_address(iter),
                                                std::forward<Args>(args)...);
    }
}

template <typename InputIter, typename OutputIter, typename Alloc>
WJR_CONSTEXPR20 OutputIter uninitialized_copy_using_allocator(InputIter first,
                                                              InputIter last,
                                                              OutputIter result,
                                                              Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        return std::uninitialized_copy(first, last, result);
    } else {
        for (; first != last; ++first, ++result) {
            std::allocator_traits<Alloc>::construct(alloc, to_address(result), *first);
        }
        return result;
    }
}

template <typename InputIter, typename Size, typename OutputIter, typename Alloc>
WJR_CONSTEXPR20 OutputIter uninitialized_copy_n_using_allocator(InputIter first, Size n,
                                                                OutputIter result,
                                                                Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        return std::uninitialized_copy_n(first, n, result);
    } else {
        for (; n > 0; ++first, ++result, --n) {
            std::allocator_traits<Alloc>::construct(alloc, to_address(result), *first);
        }
        return result;
    }
}

template <typename InputIter, typename OutputIter, typename Alloc>
WJR_CONSTEXPR20 OutputIter uninitialized_move_using_allocator(InputIter first,
                                                              InputIter last,
                                                              OutputIter result,
                                                              Alloc &alloc) {
    return uninitialized_copy_using_allocator(
        std::make_move_iterator(first), std::make_move_iterator(last), result, alloc);
}

template <typename InputIter, typename Size, typename OutputIter, typename Alloc>
WJR_CONSTEXPR20 OutputIter uninitialized_move_n_using_allocator(InputIter first, Size n,
                                                                OutputIter result,
                                                                Alloc &alloc) {
    return uninitialized_copy_n_using_allocator(std::make_move_iterator(first), n, result,
                                                alloc);
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void
uninitialized_default_construct_using_allocator(Iter first, Iter last, Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::uninitialized_default_construct(first, last);
    } else {
        using value_type = typename std::iterator_traits<Iter>::value_type;
        if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
            for (; first != last; ++first) {
                std::allocator_traits<Alloc>::construct(alloc, to_address(first),
                                                        value_type());
            }
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_default_construct_n_using_allocator(Iter first, Size n,
                                                                       Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::uninitialized_default_construct_n(first, n);
    } else {
        using value_type = typename std::iterator_traits<Iter>::value_type;
        if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
            for (; n > 0; ++first, --n) {
                std::allocator_traits<Alloc>::construct(alloc, to_address(first),
                                                        value_type());
            }
        }
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_using_allocator(Iter first, Iter last,
                                                                   Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::uninitialized_value_construct(first, last);
    } else {
        using value_type = typename std::iterator_traits<Iter>::value_type;
        for (; first != last; ++first) {
            std::allocator_traits<Alloc>::construct(alloc, to_address(first),
                                                    value_type());
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_n_using_allocator(Iter first, Size n,
                                                                     Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::uninitialized_value_construct_n(first, n);
    } else {
        using value_type = typename std::iterator_traits<Iter>::value_type;
        for (; n > 0; ++first, --n) {
            std::allocator_traits<Alloc>::construct(alloc, to_address(first),
                                                    value_type());
        }
    }
}

/**
 * @brief Fill the range [first, last) with value using allocator.
 *
 * @tparam T The value type. Use `in_place_default_construct_t` to default construct the
 * elements. Use `in_place_value_construct_t` to value construct the elements.
 */
template <typename Iter, typename Alloc, typename T>
WJR_CONSTEXPR20 void uninitialized_fill_using_allocator(Iter first, Iter last,
                                                        Alloc &alloc, const T &value) {
    if constexpr (std::is_same_v<T, in_place_default_construct_t>) {
        uninitialized_default_construct_using_allocator(first, last, alloc);
    } else if constexpr (std::is_same_v<T, in_place_value_construct_t>) {
        uninitialized_value_construct_using_allocator(first, last, alloc);
    } else {
        if constexpr (is_trivially_allocator_v<Alloc>) {
            std::uninitialized_fill(first, last, value);
        } else {
            for (; first != last; ++first) {
                std::allocator_traits<Alloc>::construct(alloc, to_address(first), value);
            }
        }
    }
}

/**
 * @brief Fill the range [first, first + n) with value using allocator.
 *
 * @tparam T The value type. Use `in_place_default_construct_t` to default construct the
 * elements. Use `in_place_value_construct_t` to value construct the elements.
 */
template <typename Iter, typename Size, typename Alloc, typename T>
WJR_CONSTEXPR20 void uninitialized_fill_n_using_allocator(Iter first, Size n,
                                                          Alloc &alloc, const T &value) {
    if constexpr (std::is_same_v<T, in_place_default_construct_t>) {
        uninitialized_default_construct_n_using_allocator(first, n, alloc);
    } else if constexpr (std::is_same_v<T, in_place_value_construct_t>) {
        uninitialized_value_construct_n_using_allocator(first, n, alloc);
    } else {
        if constexpr (is_trivially_allocator_v<Alloc>) {
            std::uninitialized_fill_n(first, n, value);
        } else {
            for (; n > 0; ++first, --n) {
                std::allocator_traits<Alloc>::construct(alloc, to_address(first), value);
            }
        }
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void destroy_at_using_allocator(Iter iter, Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::destroy_at(to_address(iter));
    } else {
        std::allocator_traits<Alloc>::destroy(alloc, to_address(iter));
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void destroy_using_allocator(Iter first, Iter last, Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::destroy(first, last);
    } else {
        for (; first != last; ++first) {
            std::allocator_traits<Alloc>::destroy(alloc, to_address(first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void destroy_n_using_allocator(Iter first, Size n, Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::destroy_n(first, n);
    } else {
        for (; n > 0; ++first, --n) {
            std::allocator_traits<Alloc>::destroy(alloc, to_address(first));
        }
    }
}

} // namespace wjr

#endif // WJR_MEMORY_UNINITIALIZED_HPP__

namespace wjr {

template <typename Alloc>
class temporary_value_allocator : noncopyable {
public:
    using value_type = typename std::allocator_traits<Alloc>::value_type;
    using pointer = value_type *;
    using const_pointer = const value_type *;

    template <typename... Args>
    temporary_value_allocator(Alloc &al, Args &&...args) : al(al) {
        uninitialized_construct_using_allocator(get(), al, std::forward<Args>(args)...);
    }

    ~temporary_value_allocator() { destroy_at_using_allocator(get(), al); }

    pointer get() noexcept { return reinterpret_cast<pointer>(storage); }
    const_pointer get() const noexcept {
        return reinterpret_cast<const_pointer>(storage);
    }

private:
    Alloc &al;
    std::aligned_storage_t<sizeof(value_type), alignof(value_type)> storage[1];
};

template <typename Alloc, typename... Args>
temporary_value_allocator(Alloc &, Args &&...) -> temporary_value_allocator<Alloc>;

} // namespace wjr

#endif // WJR_MEMORY_TEMPORARY_VALUE_ALLOCATOR_HPP__

namespace wjr {

/**
 * @brief Default vector storage
 *
 * @details Use one pointer ans two size_type currently.
 *
 */
template <typename T, typename Alloc>
class default_vector_storage : noncopyable {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = T;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using allocator_type = Alloc;
    using is_reallocatable = std::true_type;

private:
    struct Data {
        pointer m_data = {};
        size_type m_size = 0;
        size_type m_capacity = 0;
    };

    using data_type = Data;

public:
    default_vector_storage() noexcept = default;

    template <typename _Alloc>
    WJR_CONSTEXPR20 default_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    template <typename _Alloc>
    WJR_CONSTEXPR20 default_vector_storage(_Alloc &&al, size_type size,
                                           size_type capacity,
                                           in_place_reallocate_t) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {
        uninitialized_construct(size, capacity);
    }

    ~default_vector_storage() noexcept = default;

    WJR_PURE WJR_CONSTEXPR20 _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_PURE WJR_CONSTEXPR20 const _Alty &get_allocator() const noexcept {
        return m_pair.first();
    }

    WJR_CONSTEXPR20 void destroy() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(data() == nullptr) && data() == nullptr) {
            return;
        }

        if (WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0) {
            return;
        }

        destroy_n_using_allocator(data(), size(), get_allocator());
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(capacity() == 0) && capacity() == 0) {
            return;
        }

        if (data()) {
            destroy();
            get_allocator().deallocate(data(), capacity());
        }
    }

    WJR_CONSTEXPR20 void uninitialized_construct(size_type size, size_type capacity) {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        auto result = allocate_at_least(al, capacity);

        m_storage.m_data = result.ptr;
        m_storage.m_size = size;
        m_storage.m_capacity = result.count;
    }

    WJR_CONSTEXPR20 void take_storage(default_vector_storage &&other) noexcept {
        auto &other_storage = other.__get_data();
        __get_data() = std::move(other_storage);
        other_storage = {};
    }

    WJR_CONSTEXPR20 void swap_storage(default_vector_storage &other) noexcept {
        std::swap(__get_data(), other.__get_data());
    }

    WJR_PURE WJR_CONSTEXPR20 size_type &size() noexcept { return __get_data().m_size; }
    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return __get_data().m_size;
    }
    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return __get_data().m_capacity;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return __get_data().m_data; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return __get_data().m_data;
    }

private:
    WJR_PURE WJR_CONSTEXPR20 data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_PURE WJR_CONSTEXPR20 const data_type &__get_data() const noexcept {
        return m_pair.second();
    }

    compressed_pair<_Alty, data_type> m_pair;
};

/**
 * @brief Static vector storage
 *
 * @details Use static storage for small size. \n
 *
 * @tparam Capacity Static capacity
 */
template <typename T, size_t Capacity, typename Alloc>
class static_vector_storage : noncopyable {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = T;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using allocator_type = Alloc;
    using is_reallocatable = std::false_type;

private:
    static constexpr auto max_alignment =
        std::max<size_type>(alignof(T), alignof(size_type));

    struct Data {
        size_type m_size = 0;
        alignas(
            max_alignment) std::aligned_storage_t<sizeof(T), alignof(T)> m_data[Capacity];
    };

    using data_type = Data;

public:
    static_vector_storage() noexcept = default;

    template <typename _Alloc>
    WJR_CONSTEXPR20 static_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    ~static_vector_storage() noexcept = default;

    WJR_PURE WJR_CONSTEXPR20 _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_PURE WJR_CONSTEXPR20 const _Alty &get_allocator() const noexcept {
        return m_pair.first();
    }

    WJR_CONSTEXPR20 void destroy() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0) {
            return;
        }

        destroy_n_using_allocator(data(), size(), get_allocator());
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate() noexcept { destroy(); }

    WJR_CONSTEXPR20 void uninitialized_construct(size_type size,
                                                 WJR_MAYBE_UNUSED size_type capacity) {
        WJR_ASSERT_ASSUME(capacity <= Capacity,
                          "capacity must be less than or equal to Capacity");
        auto &m_storage = __get_data();
        m_storage.m_size = size;
    }

    WJR_CONSTEXPR20 void take_storage(static_vector_storage &&other) noexcept {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        auto &other_storage = other.__get_data();
        m_storage.m_size = other_storage.m_size;
        uninitialized_move_n_using_allocator(other_storage.m_data, m_storage.m_size,
                                             m_storage.m_data, al);
        other_storage.m_size = 0;
    }

    WJR_CONSTEXPR20 void swap_storage(static_vector_storage &other) noexcept {
        auto &al = get_allocator();
        static_vector_storage tmp;
        tmp.take_storage(std::move(other));
        other.take_storage(std::move(*this));
        take_storage(std::move(tmp));
    }

    WJR_PURE WJR_CONSTEXPR20 size_type &size() noexcept { return __get_data().m_size; }
    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return __get_data().m_size;
    }
    WJR_CONST WJR_CONSTEXPR20 size_type capacity() const noexcept { return Capacity; }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept {
        return reinterpret_cast<pointer>(__get_data().m_data);
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return reinterpret_cast<const_pointer>(__get_data().m_data);
    }

private:
    WJR_PURE WJR_CONSTEXPR20 data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_PURE WJR_CONSTEXPR20 const data_type &__get_data() const noexcept {
        return m_pair.second();
    }

    compressed_pair<_Alty, data_type> m_pair;
};

template <typename T, typename Alloc>
class fixed_vector_storage : noncopyable {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = T;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using allocator_type = Alloc;
    using is_reallocatable = std::false_type;

private:
    struct Data {
        pointer m_data = {};
        size_type m_size = 0;
        size_type m_capacity = 0;
    };

    using data_type = Data;

public:
    fixed_vector_storage() noexcept = default;

    template <typename _Alloc>
    WJR_CONSTEXPR20 fixed_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    ~fixed_vector_storage() noexcept = default;

    WJR_PURE WJR_CONSTEXPR20 _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_PURE WJR_CONSTEXPR20 const _Alty &get_allocator() const noexcept {
        return m_pair.first();
    }

    WJR_CONSTEXPR20 void destroy() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(data() == nullptr) && data() == nullptr) {
            return;
        }

        if (WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0) {
            return;
        }

        destroy_n_using_allocator(data(), size(), get_allocator());
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(capacity() == 0) && capacity() == 0) {
            return;
        }

        if (data()) {
            destroy();
            get_allocator().deallocate(data(), capacity());
        }
    }

    WJR_CONSTEXPR20 void uninitialized_construct(size_type size, size_type capacity) {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        auto result = allocate_at_least(al, capacity);

        m_storage.m_data = result.ptr;
        m_storage.m_size = size;
        m_storage.m_capacity = result.count;
    }

    WJR_CONSTEXPR20 void take_storage(fixed_vector_storage &&other) noexcept {
        auto &other_storage = other.__get_data();
        __get_data() = std::move(other_storage);
        other_storage = {};
    }

    WJR_CONSTEXPR20 void swap_storage(fixed_vector_storage &other) noexcept {
        std::swap(__get_data(), other.__get_data());
    }

    WJR_PURE WJR_CONSTEXPR20 size_type &size() noexcept { return __get_data().m_size; }
    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return __get_data().m_size;
    }
    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return __get_data().m_capacity;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return __get_data().m_data; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return __get_data().m_data;
    }

private:
    WJR_PURE WJR_CONSTEXPR20 data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_PURE WJR_CONSTEXPR20 const data_type &__get_data() const noexcept {
        return m_pair.second();
    }

    compressed_pair<_Alty, data_type> m_pair;
};

namespace vector_details {

template <typename Enable, typename Storage, typename... Args>
struct __has_vector_storage_shrink_to_fit : std::false_type {};
template <typename Storage, typename... Args>
struct __has_vector_storage_shrink_to_fit<
    std::void_t<decltype(std::declval<Storage>().shrink_to_fit())>, Storage, Args...>
    : std::true_type {};
template <typename Storage, typename... Args>
struct has_vector_storage_shrink_to_fit
    : __has_vector_storage_shrink_to_fit<void, Storage, Args...> {};
template <typename Storage, typename... Args>
constexpr bool has_vector_storage_shrink_to_fit_v =
    has_vector_storage_shrink_to_fit<Storage, Args...>::value;

} // namespace vector_details

/**
 * @brief Customized vector by storage.
 *
 * @details Type of pointer is same as iterator.
 *
 */
template <typename Storage>
class basic_vector {
public:
    using value_type = typename Storage::value_type;
    using allocator_type = typename Storage::allocator_type;

private:
    using _Alty =
        typename std::allocator_traits<allocator_type>::template rebind_alloc<value_type>;
    using _Alty_traits = std::allocator_traits<_Alty>;

    using storage_type = Storage;
    using storage_traits_type = container_traits<_Alty>;
    using is_storage_reallocatable = typename storage_type::is_reallocatable;
    using __get_size_t = decltype(std::declval<storage_type>().size());

    static_assert(std::is_reference_v<unref_wrapper_t<__get_size_t>>,
                  "return type of "
                  "storage::size() must be "
                  "reference type");

    friend class container_traits<_Alty>;

public:
    static_assert(std::is_same_v<typename _Alty_traits::value_type, value_type>,
                  "allocator_type::value_type must be the same as value_type");

    using size_type = typename storage_type::size_type;
    using difference_type = typename storage_type::difference_type;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert(std::is_pointer_v<pointer>, "");
    static_assert(std::is_pointer_v<const_pointer>, "");

    WJR_CONSTEXPR20
    basic_vector() noexcept(std::is_nothrow_default_constructible_v<storage_type>) =
        default;

    WJR_CONSTEXPR20 explicit basic_vector(const allocator_type &al) noexcept(
        std::is_nothrow_default_constructible_v<storage_type>)
        : m_storage(al) {}

    WJR_CONSTEXPR20 explicit basic_vector(const size_type n,
                                          const allocator_type &al = allocator_type())
        : basic_vector(al) {
        __construct_n(n, in_place_value_construct);
    }

    WJR_CONSTEXPR20 basic_vector(size_type n, const value_type &val,
                                 const allocator_type &al = allocator_type())
        : basic_vector(al) {
        __construct_n(n, val);
    }

private:
    template <typename _Alloc>
    WJR_CONSTEXPR20
    basic_vector(const basic_vector &other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<storage_type, const storage_type &, _Alloc &&>)
        : m_storage(std::forward<_Alloc>(al)) {
        auto size = other.size();
        m_storage.uninitialized_construct(size, other.capacity());
        uninitialized_copy_n_using_allocator(other.data(), size, data(),
                                             __get_allocator());
    }

    template <typename _Alloc>
    WJR_CONSTEXPR20
    basic_vector(basic_vector &&other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<storage_type, const storage_type &, _Alloc &&>)
        : m_storage(std::forward<_Alloc>(al)) {
        __take_storage(std::move(other));
    }

public:
    WJR_CONSTEXPR20 basic_vector(const basic_vector &other)
        : basic_vector(other,
                       _Alty_traits::select_on_container_copy_construction(
                           other.__get_allocator()),
                       in_place_empty) {}

    WJR_CONSTEXPR20 basic_vector(const basic_vector &other, const allocator_type &al)
        : basic_vector(other, al, in_place_empty) {}

    WJR_CONSTEXPR20 basic_vector(basic_vector &&other)
        : basic_vector(std::move(other), std::move(other.__get_allocator()),
                       in_place_empty) {}

    WJR_CONSTEXPR20 basic_vector(basic_vector &&other, const allocator_type &al) noexcept
        : basic_vector(std::move(other), al, in_place_empty) {}

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 basic_vector(Iter first, Iter last,
                                 const allocator_type &al = allocator_type())
        : basic_vector(al) {
        __range_construct(first, last,
                          typename std::iterator_traits<Iter>::iterator_category());
    }

    WJR_CONSTEXPR20 basic_vector(std::initializer_list<value_type> il,
                                 const allocator_type &al = allocator_type())
        : basic_vector(il.begin(), il.end(), al) {}

    WJR_CONSTEXPR20 ~basic_vector() noexcept { __destroy_and_deallocate(); }

    WJR_CONSTEXPR20 basic_vector &operator=(const basic_vector &other) {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_traits_type::copy_assign(*this, other);
        }

        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &operator=(basic_vector &&other) noexcept(
        _Alty_traits::propagate_on_container_move_assignment::value ||
        _Alty_traits::is_always_equal::value) {

        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_traits_type::move_assign(*this, std::move(other));
        }

        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &operator=(std::initializer_list<value_type> il) {
        return assign(il);
    }

    WJR_CONSTEXPR20 basic_vector &assign(size_type n, const value_type &val) {
        __fill_assign(n, val);
        return *this;
    }

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 basic_vector &assign(Iter first, Iter last) {
        __range_assign(first, last,
                       typename std::iterator_traits<Iter>::iterator_category());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &assign(std::initializer_list<value_type> il) {
        return assign(il.begin(), il.end());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 iterator begin() noexcept { return data(); }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_iterator begin() const noexcept {
        return data();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_iterator cbegin() const noexcept {
        return data();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 iterator end() noexcept { return data() + size(); }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_iterator end() const noexcept {
        return data() + size();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_iterator cend() const noexcept {
        return data() + size();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept { return m_storage.size(); }

    WJR_CONSTEXPR20 void resize(const size_type new_size) {
        __resize(new_size, in_place_value_construct);
    }

    WJR_CONSTEXPR20 void resize(const size_type new_size, const value_type &val) {
        __resize(new_size, val);
    }

    /**
     * @todo designed shrink_to_fit for storage.
     */
    WJR_CONSTEXPR20 void shrink_to_fit() {
        if constexpr (vector_details::has_vector_storage_shrink_to_fit_v<storage_type>) {
            m_storage.shrink_to_fit();
        } else if constexpr (is_storage_reallocatable::value) {
            if (size() < capacity()) {
                auto &al = __get_allocator();
                storage_type new_storage(al, size(), size(), in_place_reallocate);
                uninitialized_move_n_using_allocator(data(), size(), new_storage.data(),
                                                     al);
                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            }
        }
    }

    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return m_storage.capacity();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 bool empty() const noexcept { return size() == 0; }

    WJR_CONST WJR_CONSTEXPR20 static size_type
    get_growth_capacity(size_type old_capacity, size_type new_size) noexcept {
        return std::max(old_capacity + old_capacity / 2, new_size);
    }

    WJR_CONSTEXPR20 void reserve(size_type n) {
        if constexpr (is_storage_reallocatable::value) {
            const size_type old_size = size();
            const size_type old_capacity = capacity();
            if (WJR_UNLIKELY(old_capacity < n)) {
                auto &al = __get_allocator();
                const size_type new_capacity = get_growth_capacity(old_capacity, n);

                storage_type new_storage(al, old_size, new_capacity, in_place_reallocate);
                uninitialized_move_n_using_allocator(data(), old_size, new_storage.data(),
                                                     al);
                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            }
        }
    }

    WJR_INLINE_CONSTEXPR20 reference operator[](size_type pos) noexcept {
        return *(data() + pos);
    }

    WJR_INLINE_CONSTEXPR20 const_reference operator[](size_type pos) const noexcept {
        return *(data() + pos);
    }

    WJR_INLINE_CONSTEXPR20 reference at(size_type pos) {
        if (WJR_UNLIKELY(pos >= size())) {
            WJR_THROW(std::out_of_range("basic_vector::at"));
        }

        return (*this)[pos];
    }

    WJR_INLINE_CONSTEXPR20 const_reference at(size_type pos) const {
        if (WJR_UNLIKELY(pos >= size())) {
            WJR_THROW(std::out_of_range("basic_vector::at"));
        }

        return (*this)[pos];
    }

    WJR_INLINE_CONSTEXPR20 reference front() noexcept { return *data(); }

    WJR_INLINE_CONSTEXPR20 const_reference front() const noexcept { return *data(); }

    WJR_INLINE_CONSTEXPR20 reference back() noexcept { return *(data() + size() - 1); }

    WJR_INLINE_CONSTEXPR20 const_reference back() const noexcept {
        return *(data() + size() - 1);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return m_storage.data(); }

    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return m_storage.data();
    }

    WJR_PURE WJR_CONSTEXPR20 const_pointer cdata() const noexcept { return data(); }

    template <typename... Args>
    WJR_INLINE_CONSTEXPR20 reference emplace_back(Args &&...args) {
        const pointer __last = data() + size();
        const pointer __end = data() + capacity();

        if (WJR_LIKELY(__last != __end)) {
            uninitialized_construct_using_allocator(__last, __get_allocator(),
                                                    std::forward<Args>(args)...);
            ++__get_size();
        } else {
            __realloc_insert_at_end(std::forward<Args>(args)...);
        }

        return back();
    }

    WJR_INLINE_CONSTEXPR20 void push_back(const value_type &val) { emplace_back(val); }

    WJR_INLINE_CONSTEXPR20 void push_back(value_type &&val) {
        emplace_back(std::move(val));
    }

    WJR_INLINE_CONSTEXPR20 void pop_back() noexcept {
        size_type __size = --__get_size();
        destroy_at_using_allocator(data() + __size, __get_allocator());
    }

    template <typename... Args>
    WJR_INLINE_CONSTEXPR20 iterator emplace(const_iterator pos, Args &&...args) {
        return __emplace_aux(pos, std::forward<Args>(args)...);
    }

    WJR_INLINE_CONSTEXPR20 iterator insert(const_iterator pos, const value_type &val) {
        return emplace(pos, val);
    }

    WJR_INLINE_CONSTEXPR20 iterator insert(const_iterator pos, value_type &&val) {
        return emplace(pos, std::move(val));
    }

    WJR_CONSTEXPR20 iterator insert(const_iterator pos,
                                    std::initializer_list<value_type> il) {
        return insert(pos, il.begin(), il.end());
    }

    WJR_CONSTEXPR20 iterator insert(const_iterator pos, size_type n,
                                    const value_type &val) {
        const auto old_pos = static_cast<size_type>(pos - cbegin());
        __fill_insert(begin() + old_pos, n, val);
        return begin() + old_pos;
    }

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 iterator insert(const_iterator pos, Iter first, Iter last) {
        const auto old_pos = static_cast<size_type>(pos - cbegin());
        __range_insert(begin() + old_pos, first, last,
                       typename std::iterator_traits<Iter>::iterator_category());
        return begin() + old_pos;
    }

    WJR_INLINE_CONSTEXPR20 iterator erase(const_iterator pos) {
        return __erase(begin() + (pos - cbegin()));
    }

    WJR_INLINE_CONSTEXPR20 iterator erase(const_iterator first, const_iterator last) {
        const iterator __beg = begin();
        const const_iterator __cbeg = cbegin();
        return __erase(__beg + (first - __cbeg), __beg + (last - __cbeg));
    }

    WJR_INLINE_CONSTEXPR20 void swap(basic_vector &other) noexcept {
        storage_traits_type::swap(*this, other);
    }

    WJR_CONSTEXPR20 void clear() {
        __erase_at_end(data());
        WJR_ASSUME(size() == 0);
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 allocator_type &get_allocator() noexcept {
        return __get_allocator();
    }
    WJR_PURE WJR_INLINE_CONSTEXPR20 const allocator_type &get_allocator() const noexcept {
        return __get_allocator();
    }

    // extension

    WJR_CONSTEXPR20 basic_vector(size_type n, in_place_default_construct_t,
                                 const allocator_type &al = allocator_type())
        : basic_vector(al) {
        __construct_n(n, in_place_default_construct);
    }

    WJR_CONSTEXPR20 void resize(const size_type new_size, in_place_default_construct_t) {
        __resize(new_size, in_place_default_construct);
    }

    WJR_CONSTEXPR20 void push_back(in_place_default_construct_t) {
        emplace_back(in_place_default_construct);
    }

    WJR_CONSTEXPR20 basic_vector &append(const value_type &val) {
        emplace_back(val);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(value_type &&val) {
        emplace_back(std::move(val));
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(in_place_default_construct_t) {
        emplace_back(in_place_default_construct);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(const size_type n, const value_type &val) {
        __append(n, val);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(const size_type n,
                                         in_place_default_construct_t) {
        __append(n, in_place_default_construct);
        return *this;
    }

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 basic_vector &append(Iter first, Iter last) {
        __range_append(first, last,
                       typename std::iterator_traits<Iter>::iterator_category());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(std::initializer_list<value_type> il) {
        return append(il.begin(), il.end());
    }

    /**
     * @brief Pop n elements from the end
     *
     */
    WJR_CONSTEXPR20 basic_vector &chop(const size_type n) {
        __erase_at_end(end() - n);
        return *this;
    }

    /**
     * @brief Truncate the size to n
     *
     */
    WJR_CONSTEXPR20 basic_vector &truncate(const size_type n) { return chop(size() - n); }

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 basic_vector &replace(const_iterator from, const_iterator to,
                                          Iter first, Iter last) {
        const pointer __beg = begin();
        const const_iterator __cbeg = cbegin();
        const auto __offset1 = static_cast<size_type>(from - __cbeg);
        const auto __offset2 = static_cast<size_type>(to - __cbeg);
        __range_replace(__beg + __offset1, __beg + __offset2, first, last,
                        typename std::iterator_traits<Iter>::iterator_category());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &replace(const_iterator from, const_iterator to,
                                          const size_type n, const value_type &val) {
        const pointer __beg = begin();
        const const_iterator __cbeg = cbegin();
        const auto __offset1 = static_cast<size_type>(from - __cbeg);
        const auto __offset2 = static_cast<size_type>(to - __cbeg);
        __fill_replace(__beg + __offset1, __beg + __offset2, n, val);
        return *this;
    }

    WJR_PURE WJR_CONSTEXPR20 storage_type &get_storage() noexcept { return m_storage; }
    WJR_PURE WJR_CONSTEXPR20 const storage_type &get_storage() const noexcept {
        return m_storage;
    }

private:
    // member function for container_traits (START)

    WJR_PURE WJR_CONSTEXPR20 _Alty &__get_allocator() noexcept {
        return m_storage.get_allocator();
    }

    WJR_PURE WJR_CONSTEXPR20 const _Alty &__get_allocator() const noexcept {
        return m_storage.get_allocator();
    }

    WJR_CONSTEXPR20 void __destroy() noexcept { m_storage.destroy(); }

    WJR_CONSTEXPR20 void __destroy_and_deallocate() noexcept {
        m_storage.destroy_and_deallocate();
    }

    WJR_CONSTEXPR20 void __copy_element(const basic_vector &other) {
        assign(other.begin(), other.end());
    }

    WJR_CONSTEXPR20 void __take_storage(basic_vector &&other) {
        __take_storage(std::move(other.m_storage));
    }

    WJR_CONSTEXPR20 void __move_element(basic_vector &&other) {
        assign(std::make_move_iterator(other.begin()),
               std::make_move_iterator(other.end()));
    }

    WJR_CONSTEXPR20 void __swap_storage(basic_vector &other) {
        m_storage.swap_storage(other.m_storage);
    }

    // member function for container_traits (END)

    WJR_PURE WJR_CONSTEXPR20 __get_size_t __get_size() noexcept {
        return m_storage.size();
    }

    WJR_CONSTEXPR20 void __take_storage(storage_type &&other) {
        m_storage.take_storage(std::move(other));
    }

    WJR_CONSTEXPR20 void
    __unreallocatable_unreachable(WJR_MAYBE_UNUSED size_type new_capacity) const {
        WJR_ASSERT(
            new_capacity <= capacity(),
            "new_capacity must be less than or equal to capacity if the storage is not reallocatable.\nnew_capacity = ",
            new_capacity, ", capacity = ", capacity());
        WJR_UNREACHABLE();
    }

    template <typename... Args,
              std::enable_if_t<sizeof...(Args) == 1 || sizeof...(Args) == 2, int> = 0>
    WJR_CONSTEXPR20 void __construct_n(const size_type n, Args &&...args) {
        if (n != 0) {
            auto &al = __get_allocator();
            m_storage.uninitialized_construct(n, n);
            if constexpr (sizeof...(Args) == 1) {
                uninitialized_fill_n_using_allocator(data(), n, al,
                                                     std::forward<Args>(args)...);
            } else if constexpr (sizeof...(Args) == 2) {
                uninitialized_copy_using_allocator(std::forward<Args>(args)..., data(),
                                                   al);
            }
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_construct(Iter first, Iter last,
                                           std::input_iterator_tag) {
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_construct(Iter first, Iter last,
                                           std::forward_iterator_tag) {
        const auto n = static_cast<size_type>(std::distance(first, last));
        __construct_n(n, first, last);
    }

    WJR_CONSTEXPR20 void __erase_at_end(const_pointer pos) noexcept {
        const pointer __first = data();
        const pointer __last = data() + size();
        WJR_ASSERT(pos >= __first && pos <= __last,
                   "pos must be in the range of [begin(), end()]");
        const auto new_size = static_cast<size_type>(pos - __first);
        destroy_using_allocator(__first + new_size, __last, __get_allocator());
        __get_size() = new_size;
    }

    WJR_CONSTEXPR20 iterator __erase(iterator pos) noexcept {
        const pointer __last = data() + size();
        if (pos + 1 != __last) {
            std::move(pos + 1, __last, pos);
        }

        destroy_at_using_allocator(__last - 1, __get_allocator());
        --__get_size();
        return pos;
    }

    WJR_CONSTEXPR20 iterator __erase(iterator first, iterator last) noexcept {
        const pointer __last = data() + size();
        if (first != last) {
            if (last != __last) {
                std::move(last, __last, first);
            }

            __erase_at_end(__last - (last - first));
        }

        return first;
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_insert(iterator pos, Iter first, Iter last,
                                        std::input_iterator_tag) {
        if (pos == end()) {
            __range_append(first, last, std::input_iterator_tag{});
        } else if (first != last) {
            basic_vector tmp(first, last, __get_allocator());
            insert(pos, tmp.begin(), tmp.end());
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_insert(iterator pos, Iter first, Iter last,
                                        std::forward_iterator_tag) {
        if (first != last) {
            auto &al = __get_allocator();
            const pointer __first = data();
            const pointer __last = data() + size();
            const pointer __end = data() + capacity();

            const auto n = static_cast<size_type>(std::distance(first, last));
            const auto __rest = static_cast<size_type>(__end - __last);

            if (WJR_LIKELY(__rest >= n)) {
                const auto __elements_after = static_cast<size_type>(__last - pos);
                if (__elements_after > n) {
                    uninitialized_move_n_using_allocator(__last - n, n, __last, al);
                    std::move_backward(pos, __last - n, __last);
                    std::copy(first, last, pos);
                } else {
                    auto mid = first;
                    std::advance(mid, __elements_after);

                    uninitialized_copy_using_allocator(mid, last, __last, al);
                    uninitialized_move_using_allocator(pos, __last,
                                                       __last + n - __elements_after, al);
                    std::copy(first, mid, pos);
                }

                __get_size() += n;
            } else {
                if constexpr (is_storage_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__last - __first);
                    const auto old_pos = static_cast<size_type>(pos - __first);
                    const size_type new_capacity =
                        get_growth_capacity(capacity(), old_size + n);
                    storage_type new_storage(al, old_size + n, new_capacity,
                                             in_place_reallocate);
                    const pointer new_first = new_storage.data();

                    uninitialized_copy_using_allocator(first, last, new_first + old_pos,
                                                       al);
                    uninitialized_move_using_allocator(__first, pos, new_first, al);
                    uninitialized_move_using_allocator(pos, __last,
                                                       new_first + old_pos + n, al);

                    __destroy_and_deallocate();
                    __take_storage(std::move(new_storage));
                } else {
                    __unreallocatable_unreachable(size() + n);
                }
            }
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_append(Iter first, Iter last, std::input_iterator_tag) {
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_append(Iter first, Iter last,
                                        std::forward_iterator_tag) {
        if (first != last) {
            auto &al = __get_allocator();
            const pointer __first = data();
            const pointer __last = data() + size();
            const pointer __end = data() + capacity();

            const auto n = static_cast<size_type>(std::distance(first, last));
            const auto __rest = static_cast<size_type>(__end - __last);

            if (WJR_LIKELY(__rest >= n)) {
                uninitialized_copy_n_using_allocator(first, n, __last, al);
                __get_size() += n;
            } else {
                if constexpr (is_storage_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__last - __first);
                    const size_type new_capacity =
                        get_growth_capacity(capacity(), old_size + n);

                    storage_type new_storage(al, old_size + n, new_capacity,
                                             in_place_reallocate);
                    const pointer new_first = new_storage.data();

                    uninitialized_copy_using_allocator(first, last, new_first + old_size,
                                                       al);
                    uninitialized_move_using_allocator(__first, __last, new_first, al);

                    __destroy_and_deallocate();
                    __take_storage(std::move(new_storage));
                } else {
                    __unreallocatable_unreachable(size() + n);
                }
            }
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last, std::input_iterator_tag) {
        const pointer __first = data();
        const pointer __last = data() + size();

        pointer cur = __first;

        for (; first != last && cur != __last; ++first, ++cur) {
            *cur = *first;
        }

        if (first == last) {
            __erase_at_end(cur);
        } else {
            __range_append(first, last, std::input_iterator_tag{});
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last,
                                        std::forward_iterator_tag) {
        auto n = static_cast<size_type>(std::distance(first, last));
        auto &al = __get_allocator();
        const pointer __first = data();
        const pointer __last = data() + size();

        if (n <= size()) {
            std::copy(first, last, __first);
            __erase_at_end(__first + n);
        } else if (WJR_LIKELY(n <= capacity())) {
            auto mid = first;
            std::advance(mid, size());
            std::copy(first, mid, begin());
            uninitialized_copy_using_allocator(mid, last, __last, al);
            __get_size() = n;
        } else {
            if constexpr (is_storage_reallocatable::value) {
                size_type new_capacity = get_growth_capacity(capacity(), n);
                storage_type new_storage(al, n, new_capacity, in_place_reallocate);
                const pointer new_first = new_storage.data();
                uninitialized_copy_n_using_allocator(first, n, new_first, al);

                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            } else {
                __unreallocatable_unreachable(n);
            }
        }
    }

    WJR_CONSTEXPR20 void __fill_assign(size_type n, const value_type &val) {
        auto &al = __get_allocator();

        if (WJR_UNLIKELY(n > capacity())) {
            if constexpr (is_storage_reallocatable::value) {
                __destroy_and_deallocate();
                storage_type new_storage(al, n, n, in_place_reallocate);
                uninitialized_fill_n_using_allocator(new_storage.data(), n, al, val);
                __take_storage(std::move(new_storage));
                return;
            } else {
                __unreallocatable_unreachable(n);
            }
        }

        if (n > size()) {
            std::fill(begin(), end(), val);
            uninitialized_fill_n_using_allocator(end(), n - size(), al, val);
            __get_size() = n;
        } else {
            __erase_at_end(std::fill_n(begin(), n, val));
        }
    }

    template <typename... Args>
    WJR_CONSTEXPR20 void __realloc_insert(iterator pos, Args &&...args) {
        if constexpr (is_storage_reallocatable::value) {
            auto &al = __get_allocator();
            const pointer __first = data();
            const pointer __last = data() + size();

            const auto old_pos_size = static_cast<size_type>(pos - __first);
            const auto old_size = static_cast<size_type>(__last - __first);
            const size_type new_size = old_size + 1;
            const size_type new_capacity = get_growth_capacity(old_size, new_size);

            storage_type new_storage(al, new_size, new_capacity, in_place_reallocate);

            const pointer new_first = new_storage.data();
            const pointer new_pos = new_first + old_pos_size;

            uninitialized_construct_using_allocator(new_pos, al,
                                                    std::forward<Args>(args)...);

            uninitialized_move_n_using_allocator(__first, old_pos_size, new_first, al);
            uninitialized_move_using_allocator(pos, __last, new_pos + 1, al);

            __destroy_and_deallocate();
            __take_storage(std::move(new_storage));
        } else {
            __unreallocatable_unreachable(size() + 1);
        }
    }

    template <typename... Args>
    WJR_CONSTEXPR20 void __realloc_insert_at_end(Args &&...args) {
        if constexpr (is_storage_reallocatable::value) {
            auto &al = __get_allocator();
            const pointer __first = data();
            const pointer __last = data() + size();

            const auto old_size = static_cast<size_type>(__last - __first);
            const auto new_size = old_size + 1;
            const size_type new_capacity = get_growth_capacity(old_size, new_size);

            storage_type new_storage(al, new_size, new_capacity, in_place_reallocate);
            const pointer new_first = new_storage.data();

            const pointer new_pos = new_first + old_size;
            uninitialized_construct_using_allocator(new_pos, al,
                                                    std::forward<Args>(args)...);

            uninitialized_move_using_allocator(__first, __last, new_first, al);

            __destroy_and_deallocate();
            __take_storage(std::move(new_storage));
        } else {
            __unreallocatable_unreachable(size() + 1);
        }
    }

    WJR_CONSTEXPR20 void __fill_insert(iterator pos, size_type n, const value_type &val) {
        if (n == 0) {
            return;
        }

        auto &al = __get_allocator();
        const pointer __first = data();
        const pointer __last = data() + size();
        const pointer __end = data() + capacity();

        const auto __rest = static_cast<size_type>(__end - __last);

        if (WJR_LIKELY(__rest >= n)) {
            const temporary_value_allocator tmp(al, val);
            const auto &real_val = *tmp.get();

            const auto __elements_after = static_cast<size_type>(__last - pos);
            if (__elements_after > n) {
                uninitialized_move_n_using_allocator(__last - n, n, __last, al);
                std::move_backward(pos, __last - n, __last);
                std::fill_n(pos, n, real_val);
            } else {
                uninitialized_fill_n_using_allocator(__last, n - __elements_after, al,
                                                     real_val);
                uninitialized_move_using_allocator(pos, __last, pos + n, al);
                std::fill(pos, __last, real_val);
            }

            __get_size() += n;
        } else {
            if constexpr (is_storage_reallocatable::value) {
                const auto new_capacity = get_growth_capacity(capacity(), size() + n);
                storage_type new_storage(al, size() + n, new_capacity,
                                         in_place_reallocate);
                const pointer new_first = new_storage.data();

                const auto old_pos = static_cast<size_type>(pos - __first);

                uninitialized_fill_n_using_allocator(new_first + old_pos, n, al, val);
                uninitialized_move_using_allocator(__first, pos, new_first, al);
                uninitialized_move_using_allocator(pos, __last, new_first + old_pos + n,
                                                   al);

                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            } else {
                __unreallocatable_unreachable(size() + n);
            }
        }
    }

    template <typename Ty>
    WJR_CONSTEXPR20 void __resize(const size_type new_size, const Ty &val) {
        const auto old_size = size();
        if (new_size > old_size) {
            __append(new_size - old_size, val);
        } else if (new_size < old_size) {
            __erase_at_end(data() + new_size);
        }
    }

    template <typename Ty>
    WJR_CONSTEXPR20 void __append(size_type n, const Ty &val) {
        auto &al = __get_allocator();

        const auto old_size = size();
        const auto old_capacity = capacity();

        const pointer __first = data();
        const pointer __last = __first + old_size;

        const auto __rest = old_capacity - old_size;
        const auto new_size = old_size + n;

        if (WJR_LIKELY(__rest >= n)) {
            uninitialized_fill_n_using_allocator(__last, n, al, val);
            __get_size() = new_size;
        } else {
            if constexpr (is_storage_reallocatable::value) {
                auto new_capacity = get_growth_capacity(old_capacity, new_size);
                storage_type new_storage(al, new_size, new_capacity, in_place_reallocate);
                const pointer new_first = new_storage.data();

                uninitialized_fill_n_using_allocator(new_first + old_size, n, al, val);
                uninitialized_move_using_allocator(__first, __last, new_first, al);

                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            } else {
                __unreallocatable_unreachable(new_size);
            }
        }
    }

    template <typename Args>
    WJR_CONSTEXPR20 void __insert_aux(iterator pos, Args &&args) {
        auto &al = __get_allocator();
        const pointer __last = data() + size();

        uninitialized_construct_using_allocator(__last, al, std::move(*(__last - 1)));

        std::move_backward(pos, __last - 1, __last);
        *pos = std::forward<Args>(args);

        ++__get_size();
    }

    template <typename... Args>
    WJR_CONSTEXPR20 iterator __emplace_aux(const_iterator pos, Args &&...args) {
        const auto __offset = pos - cbegin();

        auto &al = __get_allocator();
        const pointer __last = data() + size();
        const pointer __end = data() + capacity();

        if (WJR_LIKELY(__last != __end)) {
            if (pos == __last) {
                uninitialized_construct_using_allocator(__last, al,
                                                        std::forward<Args>(args)...);
                ++__get_size();
            } else {
                temporary_value_allocator tmp(al, std::forward<Args>(args)...);
                __insert_aux(begin() + __offset, std::move(*tmp.get()));
            }
        } else {
            __realloc_insert(begin() + __offset, std::forward<Args>(args)...);
        }

        return begin() + __offset;
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_replace(iterator old_first, iterator old_last,
                                         Iter new_first, Iter new_last,
                                         std::input_iterator_tag) {
        for (; old_first != old_last && new_first != new_last; ++old_first, ++new_first) {
            *old_first = *new_first;
        }

        if (new_first == new_last) {
            __erase_at_end(old_first, old_last);
        } else {
            __range_insert(old_last, new_first, new_last, std::input_iterator_tag());
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_replace(iterator old_first, iterator old_last,
                                         Iter new_first, Iter new_last,
                                         std::forward_iterator_tag) {
        const auto n = static_cast<size_type>(old_last - old_first);
        const auto m = static_cast<size_type>(std::distance(new_first, new_last));

        if (m <= n) {
            erase(std::copy_n(new_first, m, old_first), old_last);
        } else {
            const auto __delta = m - n;

            auto &al = __get_allocator();
            const auto __first = data();
            const auto __last = data() + size();
            const auto __end = data() + capacity();

            const auto __rest = static_cast<size_type>(__end - __last);

            if (WJR_LIKLELY(__rest >= __delta)) {
                const auto __elements_after = static_cast<size_type>(__last - old_first);
                if (__elements_after > m) {
                    // uninitialized_move(al, __last - __delta, __last, __last);
                    uninitialized_move_using_allocator(__last - __delta, __last, __last,
                                                       al);
                    std::move_backward(old_last, __last - __delta, __last);
                    std::copy(new_first, new_last, old_first);
                } else {
                    auto mid = new_first;
                    std::advance(mid, __elements_after);
                    uninitialized_copy_using_allocator(mid, new_last, __last, al);
                    uninitialized_move_using_allocator(old_last, __last, old_first + m,
                                                       al);
                    std::copy(new_first, mid, old_first);
                }
                __get_size() += __delta;
            } else {
                if constexpr (is_storage_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__last - __first);
                    const auto old_pos = static_cast<size_type>(old_first - __first);
                    const auto new_capacity =
                        get_growth_capacity(capacity(), old_size + __delta);
                    storage_type new_storage(al, old_size + __delta, new_capacity,
                                             in_place_reallocate);
                    const pointer __ptr = new_storage.data();

                    uninitialized_copy_using_allocator(new_first, new_last,
                                                       __ptr + old_pos, al);
                    uninitialized_move_using_allocator(__first, old_first, __ptr, al);
                    uninitialized_move_using_allocator(old_last, __last,
                                                       __ptr + old_pos + m, al);

                    __destroy_and_deallocate();
                    __take_storage(std::move(new_storage));
                } else {
                    __unreallocatable_unreachable(size() + __delta);
                }
            }
        }
    }

    WJR_CONSTEXPR20 void __fill_replace(iterator old_first, iterator old_last,
                                        size_type m, const value_type &val) {

        const auto n = static_cast<size_type>(old_last - old_first);

        if (m <= n) {
            erase(std::fill_n(old_first, m, val), old_last);
        } else {
            const auto __delta = m - n;

            auto &al = __get_allocator();
            const auto __first = data();
            const auto __last = data() + size();
            const auto __end = data() + capacity();

            const auto __rest = static_cast<size_type>(__end - __last);

            if (WJR_LILKELY(__rest >= __delta)) {
                const temporary_value_allocator tmp(al, val);
                const auto &real_value = *tmp.get();

                const auto __elements_after = static_cast<size_type>(__last - old_first);
                if (__elements_after > m) {
                    // uninitialized_move(al, __last - __delta, __last, __last);
                    uninitialized_move_using_allocator(__last - __delta, __last, __last,
                                                       al);
                    std::move_backward(old_last, __last - __delta, __last);
                    std::fill_n(old_first, m, real_value);
                } else {
                    uninitialized_fill_n_using_allocator(__last, m - __elements_after, al,
                                                         real_value);
                    uninitialized_move_using_allocator(old_last, __last, old_first + m,
                                                       al);
                    std::fill(old_first, __last, real_value);
                }
                __get_size() += __delta;
            } else {
                if constexpr (is_storage_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__last - __first);
                    const auto old_pos = static_cast<size_type>(old_first - __first);
                    const auto new_capacity =
                        get_growth_capacity(capacity(), old_size + __delta);
                    storage_type new_storage(al, old_size + __delta, new_capacity,
                                             in_place_reallocate);
                    const pointer __ptr = new_storage.data();

                    uninitialized_fill_n_using_allocator(__ptr + old_pos, m, al, val);
                    uninitialized_move_using_allocator(__first, old_first, __ptr, al);
                    uninitialized_move_using_allocator(old_last, __last,
                                                       __ptr + old_pos + m, al);

                    __destroy_and_deallocate();
                    __take_storage(std::move(new_storage));
                } else {
                    __unreallocatable_unreachable(size() + __delta);
                }
            }
        }
    }

private:
    storage_type m_storage;
};

template <typename T, typename Alloc = std::allocator<T>>
using vector = basic_vector<default_vector_storage<T, Alloc>>;

/**
 * @brief A vector with elements stored on the stack.
 *
 */
template <typename T, size_t Capacity, typename Alloc = std::allocator<T>>
using static_vector = basic_vector<static_vector_storage<T, Capacity, Alloc>>;

/**
 * @brief A vector with fixed capacity by construction.
 *
 * @details Only allocate memory on construction and deallocation on destruction.
 * After construction, it cannot be expanded and can only be modified through move
 * assignment.
 */
template <typename T, typename Alloc = std::allocator<T>>
using fixed_vector = basic_vector<fixed_vector_storage<T, Alloc>>;

template <typename Iter, typename T = typename std::iterator_traits<Iter>::value_type,
          typename Alloc = std::allocator<T>,
          std::enable_if_t<is_iterator_v<Iter>, int> = 0>
basic_vector(Iter, Iter, Alloc = Alloc())
    -> basic_vector<default_vector_storage<T, Alloc>>;

template <typename Storage>
void swap(basic_vector<Storage> &lhs, basic_vector<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Storage>
bool operator==(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Storage>
bool operator!=(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return !(lhs == rhs);
}

template <typename Storage>
bool operator<(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Storage>
bool operator>(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return rhs < lhs;
}

template <typename Storage>
bool operator<=(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return !(rhs < lhs);
}

template <typename Storage>
bool operator>=(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return !(lhs < rhs);
}

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_CONTAINER_VECTOR_HPP__

#endif // WJR_VECTOR_HPP__

namespace wjr {

namespace biginteger_details {

inline uint32_t normalize(uint64_t *ptr, uint32_t n) {
    return reverse_find_not_n(ptr, 0, n);
}

} // namespace biginteger_details

class default_biginteger_size_reference : noncopyable {
public:
    default_biginteger_size_reference() = delete;
    explicit default_biginteger_size_reference(int32_t &size) noexcept : m_size(&size) {}
    ~default_biginteger_size_reference() = default;
    default_biginteger_size_reference(default_biginteger_size_reference &&) = default;
    default_biginteger_size_reference &
    operator=(default_biginteger_size_reference &&) = default;

    default_biginteger_size_reference &operator=(uint32_t size) noexcept {
        *m_size = __fasts_get_sign_mask(*m_size) | size;
        return *this;
    }

    WJR_PURE operator uint32_t() const noexcept { return __fasts_abs(*m_size); }

    default_biginteger_size_reference &operator++() noexcept {
        ++*m_size;
        return *this;
    }

    default_biginteger_size_reference &operator--() noexcept {
        --*m_size;
        return *this;
    }

    default_biginteger_size_reference &operator+=(uint32_t size) noexcept {
        *m_size += size;
        return *this;
    }

    default_biginteger_size_reference &operator-=(uint32_t size) noexcept {
        *m_size -= size;
        return *this;
    }

private:
    int32_t *m_size;
};

/**
 * @struct biginteger_data
 * @brief The data structure for biginteger
 *
 */
template <typename Alloc>
class default_biginteger_vector_storage : noncopyable {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<uint64_t>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = uint64_t;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = uint32_t;
    using difference_type = int32_t;
    using allocator_type = Alloc;
    using is_reallocatable = std::true_type;

private:
    struct Data {
        pointer m_data = {};
        int32_t m_size = 0;
        uint32_t m_capacity = 0;
    };

    using data_type = Data;

public:
    default_biginteger_vector_storage() noexcept = default;

    template <typename _Alloc>
    default_biginteger_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    template <typename _Alloc>
    default_biginteger_vector_storage(_Alloc &&al, size_type size, size_type capacity,
                                      in_place_reallocate_t) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {
        uninitialized_construct(size, capacity);
    }

    ~default_biginteger_vector_storage() noexcept = default;

    WJR_PURE _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_PURE const _Alty &get_allocator() const noexcept { return m_pair.first(); }

    void destroy() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(data() == nullptr) && data() == nullptr) {
            return;
        }

        const size_type __size = size();

        if (WJR_BUILTIN_CONSTANT_P(__size == 0) && __size == 0) {
            return;
        }

        destroy_n_using_allocator(data(), __size, get_allocator());
    }

    void destroy_and_deallocate() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(capacity() == 0) && capacity() == 0) {
            return;
        }

        if (data()) {
            destroy();
            get_allocator().deallocate(data(), capacity());
        }
    }

    void uninitialized_construct(size_type _size, size_type capacity) {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        m_storage.m_data = al.allocate(capacity);
        size() = _size;
        m_storage.m_capacity = capacity;
    }

    void take_storage(default_biginteger_vector_storage &&other) noexcept {
        auto &other_storage = other.__get_data();
        auto &__storage = __get_data();
        __storage.m_data = other_storage.m_data;
        size() = other_storage.m_size;
        __storage.m_capacity = other_storage.m_capacity;
        other_storage = {};
    }

    void swap_storage(default_biginteger_vector_storage &other) noexcept {
        std::swap(__get_data(), other.__get_data());
    }

    WJR_PURE default_biginteger_size_reference size() noexcept {
        return default_biginteger_size_reference(__get_data().m_size);
    }
    WJR_PURE size_type size() const noexcept { return __fasts_abs(__get_data().m_size); }
    WJR_PURE size_type capacity() const noexcept { return __get_data().m_capacity; }

    WJR_PURE pointer data() noexcept { return __get_data().m_data; }
    WJR_PURE const_pointer data() const noexcept { return __get_data().m_data; }

    // extension

    WJR_PURE int32_t get_ssize() const noexcept { return __get_data().m_size; }
    void set_ssize(int32_t size) noexcept {
        WJR_ASSUME(__fasts_abs(size) <= capacity());
        __get_data().m_size = size;
    }

private:
    WJR_PURE data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_PURE const data_type &__get_data() const noexcept { return m_pair.second(); }

    compressed_pair<_Alty, data_type> m_pair;
};

template <>
struct unref_wrapper<default_biginteger_size_reference> {
    using type = uint32_t &;
};

template <typename Storage>
class basic_biginteger {
    using storage_type = Storage;
    using vector_type = basic_vector<storage_type>;

public:
    using value_type = typename vector_type::value_type;
    using size_type = typename vector_type::size_type;
    using difference_type = typename vector_type::difference_type;
    using reference = typename vector_type::reference;
    using const_reference = typename vector_type::const_reference;
    using pointer = typename vector_type::pointer;
    using const_pointer = typename vector_type::const_pointer;
    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;
    using reverse_iterator = typename vector_type::reverse_iterator;
    using const_reverse_iterator = typename vector_type::const_reverse_iterator;
    using allocator_type = typename vector_type::allocator_type;

    static_assert(std::is_same_v<value_type, uint64_t>, "value_type must be uint64_t");

    basic_biginteger() = default;
    basic_biginteger(const basic_biginteger &other) = default;
    basic_biginteger(basic_biginteger &&other) = default;
    basic_biginteger &operator=(const basic_biginteger &other) = default;
    basic_biginteger &operator=(basic_biginteger &&other) = default;
    ~basic_biginteger() = default;

    explicit basic_biginteger(const allocator_type &al) : m_vec(al) {}

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    basic_biginteger(Iter first, Iter last, const allocator_type &al)
        : m_vec(first, last, al) {
        __check_high_bit();
    }

    basic_biginteger(const basic_biginteger &other, const allocator_type &al)
        : m_vec(other.m_vec, al) {}

    basic_biginteger(basic_biginteger &&other, const allocator_type &al)
        : m_vec(std::move(other.m_vec), al) {}

    basic_biginteger(std::initializer_list<value_type> il, const allocator_type &al)
        : m_vec(il, al) {
        __check_high_bit();
    }

    basic_biginteger(size_type n, in_place_default_construct_t, const allocator_type &al)
        : m_vec(n, in_place_default_construct, al) {}

    basic_biginteger &operator=(std::initializer_list<value_type> il) {
        m_vec = il;
        __check_high_bit();
        return *this;
    }

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    void assign(Iter first, Iter last) {
        m_vec.assign(first, last);
        __check_high_bit();
    }

    void assign(std::initializer_list<value_type> il) {
        m_vec.assign(il);
        __check_high_bit();
    }

    allocator_type &get_allocator() noexcept { return m_vec.get_allocator(); }
    const allocator_type &get_allocator() const noexcept { return m_vec.get_allocator(); }

    reference at(size_type pos) { return m_vec.at(pos); }
    const_reference at(size_type pos) const { return m_vec.at(pos); }

    reference operator[](size_type pos) { return m_vec[pos]; }
    const_reference operator[](size_type pos) const { return m_vec[pos]; }

    reference front() { return m_vec.front(); }
    const_reference front() const { return m_vec.front(); }

    reference back() { return m_vec.back(); }
    const_reference back() const { return m_vec.back(); }

    WJR_PURE pointer data() noexcept { return m_vec.data(); }
    WJR_PURE const_pointer data() const noexcept { return m_vec.data(); }

    WJR_PURE iterator begin() noexcept { return m_vec.begin(); }
    WJR_PURE const_iterator begin() const noexcept { return m_vec.begin(); }

    WJR_PURE iterator end() noexcept { return m_vec.end(); }
    WJR_PURE const_iterator end() const noexcept { return m_vec.end(); }

    WJR_PURE const_iterator cbegin() const noexcept { return m_vec.cbegin(); }

    WJR_PURE const_iterator cend() const noexcept { return m_vec.cend(); }

    WJR_PURE reverse_iterator rbegin() noexcept { return m_vec.rbegin(); }
    WJR_PURE const_reverse_iterator rbegin() const noexcept { return m_vec.rbegin(); }

    WJR_PURE reverse_iterator rend() noexcept { return m_vec.rend(); }
    WJR_PURE const_reverse_iterator rend() const noexcept { return m_vec.rend(); }

    WJR_PURE const_reverse_iterator crbegin() const noexcept { return m_vec.crbegin(); }
    WJR_PURE const_reverse_iterator crend() const noexcept { return m_vec.crend(); }

    WJR_PURE bool empty() const noexcept { return m_vec.empty(); }
    WJR_PURE size_type size() const noexcept { return m_vec.size(); }
    WJR_PURE size_type capacity() const noexcept { return m_vec.capacity(); }

    WJR_CONST static size_type get_growth_capacity(size_type old_capacity,
                                                   size_type new_size) noexcept {
        return vector_type::get_growth_capacity(old_capacity, new_size);
    }

    void reserve(size_type new_capacity) { m_vec.reserve(new_capacity); }

    void shrink_to_fit() { m_vec.shrink_to_fit(); }

    void clear() { m_vec.clear(); }

    void swap(basic_biginteger &other) noexcept { m_vec.swap(other.m_vec); }

    friend bool operator==(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec == rhs.m_vec;
    }

    friend bool operator!=(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec != rhs.m_vec;
    }

    friend bool operator<(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec < rhs.m_vec;
    }

    friend bool operator>(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec > rhs.m_vec;
    }

    friend bool operator<=(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec <= rhs.m_vec;
    }

    friend bool operator>=(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec >= rhs.m_vec;
    }

    friend void add(basic_biginteger &dst, const basic_biginteger &lhs, uint64_t rhs) {
        __addsub<false>(&dst, &lhs, rhs);
    }

    friend void add(basic_biginteger &dst, uint64_t lhs, const basic_biginteger &rhs) {
        __addsub<false>(&dst, &rhs, lhs);
    }

    friend void sub(basic_biginteger &dst, const basic_biginteger &lhs, uint64_t rhs) {
        __addsub<true>(&dst, &lhs, rhs);
    }

    friend void sub(basic_biginteger &dst, uint64_t lhs, const basic_biginteger &rhs) {
        __ui_sub(&dst, lhs, &rhs);
    }

    friend void add(basic_biginteger &dst, const basic_biginteger &lhs,
                    const basic_biginteger &rhs) {
        __addsub<false>(&dst, &lhs, &rhs);
    }

    friend void sub(basic_biginteger &dst, const basic_biginteger &lhs,
                    const basic_biginteger &rhs) {
        __addsub<true>(&dst, &lhs, &rhs);
    }

    friend void mul(basic_biginteger &dst, const basic_biginteger &lhs,
                    const basic_biginteger &rhs) {
        __mul(&dst, &lhs, &rhs);
    }

    /**
     * @brief Get the sign of biginteger
     *
     * @return
     * false : if the biginteger is positive \n
     * true : if the biginteger is negative
     */
    WJR_PURE bool get_sign() const noexcept { return get_ssize() < 0; }

    WJR_PURE int32_t get_ssize() const { return __get_storage().get_ssize(); }
    void set_ssize(int32_t new_size) { __get_storage().set_ssize(new_size); }

private:
    WJR_PURE storage_type &__get_storage() noexcept { return m_vec.get_storage(); }
    WJR_PURE const storage_type &__get_storage() const noexcept {
        return m_vec.get_storage();
    }

    void __check_high_bit() const {
        WJR_ASSERT(size() == 0 || back() != 0, "biginteger should not have leading zero");
    }

    template <bool xsign>
    static void __addsub(basic_biginteger *dst, const basic_biginteger *lhs,
                         uint64_t rhs);

    static void __ui_sub(basic_biginteger *dst, uint64_t lhs,
                         const basic_biginteger *rhs);

    template <bool xsign>
    static void __addsub(basic_biginteger *dst, const basic_biginteger *lhs,
                         const basic_biginteger *rhs);

    static void __mul(basic_biginteger *dst, const basic_biginteger *lhs,
                      const basic_biginteger *rhs);

    vector_type m_vec;
};

using biginteger =
    basic_biginteger<default_biginteger_vector_storage<std::allocator<uint64_t>>>;

template <typename Storage>
void swap(basic_biginteger<Storage> &lhs, basic_biginteger<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Storage>
template <bool xsign>
void basic_biginteger<Storage>::__addsub(basic_biginteger *dst,
                                         const basic_biginteger *lhs, uint64_t rhs) {
    int32_t lssize = lhs->get_ssize();
    if (lssize == 0) {
        dst->reserve(1);
        dst->front() = rhs;
        dst->set_ssize(__fasts_conditional_negate<int32_t>(xsign, rhs != 0));
        return;
    }

    uint32_t lusize = __fasts_abs(lssize);
    dst->reserve(lusize + 1);

    auto dp = dst->data();
    auto lp = lhs->data();
    int32_t dssize;

    using compare =
        std::conditional_t<xsign, std::less<uint64_t>, std::greater<uint64_t>>;

    if (compare{}(dssize, 0)) {
        auto cf = addc_1(dp, lp, lusize, rhs);
        dssize = __fasts_conditional_negate(xsign, lusize + cf);
        if (cf) {
            dp[lusize] = 1;
        }
    } else {
        if (lusize == 1 && dp[0] < rhs) {
            dp[0] = rhs - dp[0];
            dssize = __fasts_conditional_negate(xsign, 1);
        } else {
            (void)subc_1(dp, lp, lusize, rhs);
            dssize = __fasts_conditional_negate(!xsign, lusize - (dp[lusize - 1] == 0));
        }
    }

    dst->set_ssize(dssize);
}

template <typename Storage>
void basic_biginteger<Storage>::__ui_sub(basic_biginteger *dst, uint64_t lhs,
                                         const basic_biginteger *rhs) {
    int32_t rssize = rhs->get_ssize();
    if (rssize == 0) {
        dst->reserve(1);
        dst->front() = lhs;
        dst->set_ssize(1);
        return;
    }

    uint32_t rusize = __fasts_abs(rssize);
    dst->reserve(rusize);

    auto dp = dst->data();
    auto rp = rhs->data();
    int32_t dssize;

    if (rssize < 0) {
        auto cf = addc_1(dp, rp, rusize, lhs);
        dssize = rusize + cf;
        if (cf) {
            dp[rusize] = 1;
        }
    } else {
        // lhs >= rhs
        if (rusize == 1 && lhs >= rp[0]) {
            dp[0] = lhs - rp[0];
            dssize = dp[0] != 0;
        }
        // lhs < rhs
        else {
            (void)subc_1(dp, rp, rusize, lhs);
            dssize = __fasts_conditional_negate(1, rusize - (dp[rusize - 1] == 0));
        }
    }

    dst->set_ssize(dssize);
}

template <typename Storage>
template <bool xsign>
void basic_biginteger<Storage>::__addsub(basic_biginteger *dst,
                                         const basic_biginteger *lhs,
                                         const basic_biginteger *rhs) {
    int32_t lssize = lhs->get_ssize();
    int32_t rssize = __fasts_conditional_negate(xsign, rhs->get_ssize());
    uint32_t lusize = __fasts_abs(lssize);
    uint32_t rusize = __fasts_abs(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lssize, rssize);
        std::swap(lusize, rusize);
    }

    dst->reserve(lusize + 1);

    auto dp = dst->data();
    auto lp = lhs->data();
    auto rp = rhs->data();
    int32_t dssize;

    // different sign
    if ((lssize ^ rssize) < 0) {
        if (lusize != rusize) {
            subc_sz(dp, lp, lusize, rp, rusize);
            dssize =
                __fasts_get_sign_mask(lssize) | biginteger_details::normalize(dp, lusize);
        } else {
            if (WJR_UNLIKELY(lusize == 0)) {
                dssize = 0;
            } else {
                dssize = __fasts_get_sign_mask(lssize) ^ abs_subc_n(dp, lp, rp, rusize);
            }
        }
    } else {
        auto cf = addc_sz(dp, lp, lusize, rp, rusize);
        // seems can be optimized
        dssize = __fasts_get_sign_mask(lssize) | (lusize + cf);
        if (cf) {
            dp[lusize] = 1;
        }
    }

    dst->set_ssize(dssize);
}

template <typename Storage>
void basic_biginteger<Storage>::__mul(basic_biginteger *dst, const basic_biginteger *lhs,
                                      const basic_biginteger *rhs) {
    using namespace biginteger_details;

    int32_t lssize = lhs->get_ssize();
    int32_t rssize = rhs->get_ssize();
    int32_t mask = __fasts_get_sign_mask(lssize ^ rssize);
    uint32_t lusize = __fasts_abs(lssize);
    uint32_t rusize = __fasts_abs(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lusize, rusize);
    }

    if (WJR_UNLIKELY(rusize == 0)) {
        dst->set_ssize(0);
        return;
    }

    int32_t dssize;

    if (rusize == 1) {
        dst->reserve(lusize + 1);
        auto cf = mul_1(dst->data(), lhs->data(), lusize, rhs->front());
        dssize = mask | (lusize + (cf != 0));
        if (cf != 0) {
            (*dst)[lusize] = cf;
        }
        dst->set_ssize(dssize);
        return;
    }

    dssize = lusize + rusize;

    auto dp = dst->data();
    auto lp = (pointer)lhs->data();
    auto rp = (pointer)rhs->data();

    std::optional<basic_biginteger> temp;

    unique_stack_allocator stkal(math_details::stack_alloc);

    if (dst->capacity() < dssize) {
        temp.emplace(dst->get_growth_capacity(dst->capacity(), dssize),
                     in_place_default_construct, dst->get_allocator());
        dp = temp.value().data();
    } else {
        if (dp == lp) {
            lp = (pointer)stkal.allocate(lusize * sizeof(uint64_t));
            if (dp == rp) {
                rp = lp;
            }
            std::copy_n(dp, lusize, lp);
        } else if (dp == rp) {
            rp = (pointer)stkal.allocate(rusize * sizeof(uint64_t));
            std::copy_n(dp, rusize, rp);
        }
    }

    if (WJR_UNLIKELY(lp == rp)) {
        sqr(dp, lp, lusize);
    } else {
        mul_s(dp, lp, lusize, rp, rusize);
    }

    bool cf = dp[dssize - 1] == 0;
    dssize = mask | (dssize - cf);

    if (temp.has_value()) {
        *dst = std::move(temp).value();
    }

    dst->set_ssize(dssize);
}

} // namespace wjr

#endif

#endif // WJR_BIGINTEGER_HPP__
#ifndef WJR_CONTAINER_GENERIC_DYNAMIC_BITSET_HPP__
#define WJR_CONTAINER_GENERIC_DYNAMIC_BITSET_HPP__

// Already included
#ifndef WJR_SPAN_HPP__
#define WJR_SPAN_HPP__

#include <stdexcept>

// Already included
// Already included

namespace wjr {

template <typename T, size_t Extent>
struct __span_static_storage {

    __span_static_storage() = default;
    __span_static_storage(const __span_static_storage &) = default;
    __span_static_storage &operator=(const __span_static_storage &) = default;

    __span_static_storage(T *p, WJR_MAYBE_UNUSED size_t s) : ptr(p) {
        WJR_ASSERT(s == size);
    }

    T *ptr = nullptr;
    static constexpr size_t size = Extent;
};

template <typename T>
struct __span_dynamic_storage {

    __span_dynamic_storage() = default;
    __span_dynamic_storage(const __span_dynamic_storage &) = default;
    __span_dynamic_storage &operator=(const __span_dynamic_storage &) = default;

    __span_dynamic_storage(T *p, size_t s) : ptr(p), size(s) {}

    T *ptr = nullptr;
    size_t size = 0;
};

template <typename Iter, typename Elem>
struct __is_span_iterator
    : std::conjunction<is_contiguous_iterator<Iter>,
                       std::is_convertible<
                           std::remove_reference_t<iter_reference_t<Iter>> *, Elem *>> {};

template <typename Array, typename Elem, typename = void>
struct __is_span_array_helper : std::false_type {};

template <typename Array, typename Elem>
struct __is_span_array_helper<Array, Elem,
                              std::void_t<decltype(std::data(std::declval<Array>()))>>
    : std::is_convertible<std::remove_reference_t<std::remove_pointer_t<
                              decltype(std::data(std::declval<Array>()))>> *,
                          Elem *> {};

template <typename Array, typename Elem>
struct __is_span_array : __is_span_array_helper<Array, Elem, void> {};

/**
 * @class span
 * @brief A view over a contiguous sequence of objectsd.
 * @tparam Extent if Extent is `dynamic_extent`, the span is a runtime-sized view.
 * Otherwise, the span is a compile-time-sized view.
 */
template <typename T, size_t Extent = dynamic_extent>
class span {
    static constexpr bool __is_dynamic = Extent == dynamic_extent;
    using __storage = std::conditional_t<__is_dynamic, __span_dynamic_storage<T>,
                                         __span_static_storage<T, Extent>>;

public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    template <size_t Ex = Extent,
              std::enable_if_t<Ex == dynamic_extent || Ex == 0, int> = 0>
    constexpr span() noexcept : storage() {}

    template <typename It,
              std::enable_if_t<
                  __is_span_iterator<It, element_type>::value && __is_dynamic, int> = 0>
    constexpr span(It first, size_type count) : storage(to_address(first), count) {}

    template <typename It,
              std::enable_if_t<
                  __is_span_iterator<It, element_type>::value && !__is_dynamic, int> = 0>
    constexpr explicit span(It first, size_type count)
        : storage(to_address(first), count) {}

    template <typename It,
              std::enable_if_t<
                  __is_span_iterator<It, element_type>::value && __is_dynamic, int> = 0>
    constexpr span(It first, It last)
        : storage(to_address(first), static_cast<size_type>(last - first)) {}

    template <typename It,
              std::enable_if_t<
                  __is_span_iterator<It, element_type>::value && !__is_dynamic, int> = 0>
    constexpr explicit span(It first, It last)
        : storage(to_address(first), static_cast<size_type>(last - first)) {}

    template <size_t N,
              std::enable_if_t<(__is_dynamic || N == Extent) &&
                                   __is_span_array<type_identity_t<element_type> (&)[N],
                                                   element_type>::value,
                               int> = 0>
    constexpr span(std::add_const_t<type_identity_t<element_type>> (&arr)[N]) noexcept
        : storage(std::data(arr), N) {}

    template <
        typename U, size_t N,
        std::enable_if_t<(__is_dynamic || N == Extent) &&
                             __is_span_array<std::array<U, N> &, element_type>::value,
                         int> = 0>
    constexpr span(std::array<U, N> &arr) noexcept
        : storage(std::data(arr), std::size(arr)) {}

    template <typename U, size_t N,
              std::enable_if_t<
                  (__is_dynamic || N == Extent) &&
                      __is_span_array<const std::array<U, N> &, element_type>::value,
                  int> = 0>
    constexpr span(const std::array<U, N> &arr) noexcept
        : storage(std::data(arr), std::size(arr)) {}

    template <
        size_t Ex = Extent,
        std::enable_if_t<std::is_const_v<element_type> && Ex == dynamic_extent, int> = 0>
    constexpr span(std::initializer_list<value_type> il) noexcept
        : storage(std::data(il), std::size(il)) {}

    template <
        size_t Ex = Extent,
        std::enable_if_t<std::is_const_v<element_type> && Ex != dynamic_extent, int> = 0>
    constexpr explicit span(std::initializer_list<value_type> il) noexcept
        : storage(std::data(il), std::size(il)) {}

    template <typename U, size_t N,
              std::enable_if_t<(__is_dynamic || N == dynamic_extent || N == Extent) &&
                                   std::is_convertible_v<U *, T *> && __is_dynamic,
                               int> = 0>
    constexpr span(const span<U, N> &source) noexcept
        : storage(source.data(), source.size()) {}

    template <typename U, size_t N,
              std::enable_if_t<(__is_dynamic || N == dynamic_extent || N == Extent) &&
                                   std::is_convertible_v<U *, T *> && !__is_dynamic,
                               int> = 0>
    constexpr explicit span(const span<U, N> &source) noexcept
        : storage(source.data(), source.size()) {}

    constexpr span(const span &other) noexcept = default;

    constexpr span &operator=(const span &other) noexcept = default;

    ~span() = default;

    constexpr iterator begin() const noexcept { return data(); }
    constexpr const_iterator cbegin() const noexcept { return begin(); }

    constexpr iterator end() const noexcept { return begin() + size(); }
    constexpr const_iterator cend() const noexcept { return end(); }

    constexpr reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }
    constexpr const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    constexpr reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(begin());
    }
    constexpr const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    constexpr reference front() const { return *begin(); }
    constexpr reference back() const { return *(--end()); }

    constexpr reference at(size_type pos) const {
        if (WJR_UNLIKELY(pos >= size())) {
            WJR_THROW(std::out_of_range("span at out of range"));
        }

        return this->operator[](pos);
    }

    constexpr reference operator[](size_type pos) const { return data()[pos]; }

    constexpr pointer data() const { return storage.ptr; }
    constexpr size_type size() const { return storage.size; }
    constexpr size_type size_bytes() const { return size() * sizeof(element_type); }
    constexpr bool empty() const { return size() == 0; }

    template <size_t Count>
    constexpr span<element_type, Count> first() const {
        static_assert(Count <= Extent, "");

        return {begin(), Count};
    }

    constexpr span<element_type, dynamic_extent> first(size_type Count) const {
        WJR_ASSERT(Count <= size());

        return {begin(), Count};
    }

    template <size_t Count>
    constexpr span<element_type, Count> last() const {
        static_assert(Count <= Extent, "");

        return {end() - Count, Count};
    }

    constexpr span<element_type, dynamic_extent> last(size_type Count) const {
        WJR_ASSERT(Count <= size());

        return {data() - Count, Count};
    }

    template <size_t Offset, size_t Count = dynamic_extent>
    constexpr span<element_type, Count != dynamic_extent    ? Count
                                 : Extent != dynamic_extent ? Extent - Offset
                                                            : dynamic_extent>
    subspan() const {
        if constexpr (Extent != dynamic_extent) {
            static_assert(Offset <= Extent, "");
            static_assert(Count == dynamic_extent || Count <= Extent - Offset, "");
        } else {
            WJR_ASSERT(Offset <= size());
            if constexpr (Count != dynamic_extent) {
                WJR_ASSERT(Count <= size() - Offset);
            }
        }
        return {begin() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

    constexpr span<element_type, dynamic_extent>
    subspan(size_type Offset, size_type Count = dynamic_extent) const {
        WJR_ASSERT(Offset <= size());

        return {begin() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

private:
    __storage storage;
};

template <typename T, size_t Extent>
span(T (&)[Extent]) -> span<T, Extent>;

template <typename T, size_t Size>
span(std::array<T, Size> &) -> span<T, Size>;

template <typename T, size_t Size>
span(const std::array<T, Size> &) -> span<const T, Size>;

template <typename It, typename End,
          std::enable_if_t<is_contiguous_iterator_v<It>, int> = 0>
span(It, End) -> span<std::remove_reference_t<iter_reference_t<It>>>;

namespace span_details {

WJR_REGISTER_HAS_TYPE(data, std::data(std::declval<Container &>()), Container);
WJR_REGISTER_HAS_TYPE(size, std::size(std::declval<Container &>()), Container);

} // namespace span_details

template <typename Container, std::enable_if_t<span_details::has_data_v<Container> &&
                                                   span_details::has_size_v<Container>,
                                               int> = 0>
auto make_span(Container &c) {
    return span(std::data(c), std::size(c));
}

template <typename T, size_t Extent>
auto make_span(T (&arr)[Extent]) {
    return span(arr);
}

template <typename T, size_t Size>
auto make_span(std::array<T, Size> &arr) {
    return span(arr);
}

template <typename T, size_t Size>
auto make_span(const std::array<T, Size> &arr) {
    return span(arr);
}

} // namespace wjr

#endif // WJR_SPAN_HPP__
// Already included

namespace wjr {

/**
 * @brief Dynamic bitset.
 *
 * @note TODO: Add more functions.
 */
template <typename Allocator = std::allocator<uint64_t>>
class basic_dynamic_bitset {
public:
    using block_type = uint64_t;
    using allocator_type = Allocator;
    constexpr static size_t block_size = 64;

private:
    using vector_type = vector<block_type, allocator_type>;

public:
    using size_type = typename vector_type::size_type;

    class reference {
        friend class basic_dynamic_bitset;

    public:
        reference(block_type *val, size_type pos)
            : m_val(val), m_mask((block_type)1 << pos) {}

        ~reference() = default;

        reference &operator=(bool val) {
            if (val) {
                *m_val |= m_mask;
            } else {
                *m_val &= ~m_mask;
            }
            return *this;
        }

        bool operator~() const { return (*m_val & m_mask) == 0; }
        operator bool() const { return (*m_val & m_mask) != 0; }

        reference &operator=(const reference &other) { return *this = bool(other); }

        reference &flip() {
            *m_val ^= m_mask;
            return *this;
        }

    private:
        block_type *m_val;
        block_type m_mask;
    };

    using const_reference = bool;

    basic_dynamic_bitset() = default;

    /**
     * @brief Use from_chars_2 to optimize the performance.
     *
     * @details Very fast conversion from string to uint64_t.
     *
     * @note Only use from_chars_2 when type of T is uint64_t and CharT is char.
     */
    template <typename CharT, size_t Extent,
              std::enable_if_t<std::is_same_v<std::remove_const_t<CharT>, char>, int> = 0>
    explicit basic_dynamic_bitset(const span<CharT, Extent> &sp)
        : m_vec((sp.size() + block_size - 1) / block_size, in_place_default_construct),
          m_bits(sp.size()) {
        from_chars_2(sp.begin(), sp.size(), m_vec.data(), char_converter);
    }

    template <
        typename CharT, size_t Extent,
        std::enable_if_t<!std::is_same_v<std::remove_const_t<CharT>, char>, int> = 0>
    explicit basic_dynamic_bitset(const span<CharT, Extent> &sp)
        : basic_dynamic_bitset(sp, '0') {}

    template <typename CharT, size_t Extent>
    basic_dynamic_bitset(const span<CharT, Extent> &sp, type_identity_t<CharT> zero,
                         type_identity_t<CharT> one = '1')
        : basic_dynamic_bitset(sp, zero, one, std::equal_to<CharT>()) {}

    template <typename CharT, size_t Extent, typename Equal>
    basic_dynamic_bitset(const span<CharT, Extent> &sp,
                         WJR_MAYBE_UNUSED type_identity_t<CharT> zero,
                         type_identity_t<CharT> one, Equal equal)
        : m_vec((sp.size() + block_size - 1) / block_size, in_place_default_construct),
          m_bits(sp.size()) {
        auto ptr = sp.data();
        size_type n = sp.size();
        size_type idx = 0;

        block_type value = 0;

        while (n >= block_size) {
            value = 0;

            for (size_type i = block_size - 1; ~i; --i) {
                value <<= 1;
                if (equal(ptr[i], one)) {
                    value |= 1;
                } else {
                    WJR_ASSERT(equal(ptr[i], zero));
                }
            }

            m_vec[idx] = value;
            ptr += block_size;
            n -= block_size;
            ++idx;
        }

        if (n) {
            value = 0;

            for (size_type i = n - 1; ~i; --i) {
                value <<= 1;
                if (equal(ptr[i], one)) {
                    value |= 1;
                } else {
                    WJR_ASSERT(equal(ptr[i], zero));
                }
            }

            m_vec[idx] = value;
        }
    }

    basic_dynamic_bitset(const basic_dynamic_bitset &other) = default;
    basic_dynamic_bitset(basic_dynamic_bitset &&other)
        : m_vec(std::move(other.m_vec)), m_bits(std::exchange(other.m_bits, 0)) {}
    basic_dynamic_bitset &operator=(const basic_dynamic_bitset &other) = default;
    basic_dynamic_bitset &operator=(basic_dynamic_bitset &&other) {
        m_vec = std::move(other.m_vec);
        m_bits = std::exchange(other.m_bits, 0);
        return *this;
    }
    ~basic_dynamic_bitset() = default;

    void swap(basic_dynamic_bitset &other) {
        m_vec.swap(other.m_vec);
        std::swap(m_bits, other.m_bits);
    }

    bool operator==(const basic_dynamic_bitset &other) const {
        return m_vec == other.m_vec;
    }

    bool operator!=(const basic_dynamic_bitset &other) const {
        return m_vec != other.m_vec;
    }

    reference operator[](size_type pos) {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        return reference(&m_vec[pos / block_size], pos % block_size);
    }

    const_reference operator[](size_type pos) const {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        return (m_vec[pos / block_size] >> (pos % block_size)) & 1;
    }

    bool test(size_type pos) const { return (*this)[pos]; }

    bool all() const {
        size_type n = m_vec.size();
        if (WJR_UNLIKELY(n == 0)) {
            return false;
        }

        if (m_vec.back() != ((block_type)1 << (m_bits % block_size)) - 1) {
            return false;
        }

        return find_not_n(m_vec.data(), ~(block_type)0, n - 1) == n - 1;
    }

    bool any() const {
        size_type n = m_vec.size();
        return find_n(m_vec.data(), (block_type)0, n) != n;
    }

    bool none() const { return !any(); }

    size_type count() const {
        size_type sum = 0;
        for (auto item : m_vec) {
            sum += popcount(item);
        }
        return sum;
    }

    size_type size() const { return m_bits; }

    allocator_type &get_allocator() noexcept { return m_vec.get_allocator(); }
    const allocator_type &get_allocator() const noexcept { return m_vec.get_allocator(); }

    basic_dynamic_bitset &operator&=(const basic_dynamic_bitset &other) {
        if (other.m_bits < m_bits) {
            m_bits = other.m_bits;
            m_vec.truncate(other.m_vec.size());
        }

        size_type n = m_vec.size();
        for (size_type i = 0; i < n; ++i) {
            m_vec[i] &= other.m_vec[i];
        }

        return *this;
    }

    basic_dynamic_bitset &operator|=(const basic_dynamic_bitset &other) {
        size_type n = m_vec.size();

        if (other.m_bits > m_bits) {
            m_bits = other.m_bits;
            m_vec.append(other.begin() + n, other.end());
        }

        for (size_type i = 0; i < n; ++i) {
            m_vec[i] |= other.m_vec[i];
        }

        return *this;
    }

    basic_dynamic_bitset &operator^=(const basic_dynamic_bitset &other) {
        size_type n = m_vec.size();

        if (other.m_bits > m_bits) {
            m_bits = other.m_bits;
            m_vec.append(other.begin() + n, other.end());
        }

        for (size_type i = 0; i < n; ++i) {
            m_vec[i] ^= other.m_vec[i];
        }

        return *this;
    }

    basic_dynamic_bitset operator~() const {
        basic_dynamic_bitset result(*this);
        result.flip();
        return result;
    }

    basic_dynamic_bitset &flip() {
        size_type n = m_vec.size();
        if (WJR_UNLIKELY(n == 0)) {
            return *this;
        }

        m_vec.back() = ((~m_vec.back()) & ((block_type)1 << (m_bits % block_size))) - 1;

        for (size_type i = 0; i < n - 1; ++i) {
            m_vec[i] = ~m_vec[i];
        }

        return *this;
    }

    basic_dynamic_bitset &flip(size_type pos) {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        m_vec[pos / block_size] ^= (block_type)1 << (pos % block_size);
        return *this;
    }

    basic_dynamic_bitset &set() {
        size_type n = m_vec.size();
        if (WJR_UNLIKELY(n == 0)) {
            return *this;
        }

        m_vec.back() = ((block_type)1 << (m_bits % block_size)) - 1;
        set_n(m_vec.data(), ~block_type(0), n - 1);

        return *this;
    }

    basic_dynamic_bitset &set(size_type pos, bool val = true) {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        if (val) {
            m_vec[pos / block_size] |= (block_type)1 << (pos % block_size);
        } else {
            m_vec[pos / block_size] &= ~((block_type)1 << (pos % block_size));
        }
        return *this;
    }

    basic_dynamic_bitset &reset() {
        set_n(m_vec.data(), 0, m_vec.size());
        return *this;
    }

    basic_dynamic_bitset &reset(size_type pos) {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        m_vec[pos / block_size] &= ~((block_type)1 << (pos % block_size));
        return *this;
    }

    friend basic_dynamic_bitset operator&(const basic_dynamic_bitset &lhs,
                                          const basic_dynamic_bitset &rhs) {
        basic_dynamic_bitset result(lhs);
        result &= rhs;
        return result;
    }

    friend basic_dynamic_bitset operator|(const basic_dynamic_bitset &lhs,
                                          const basic_dynamic_bitset &rhs) {
        basic_dynamic_bitset result(lhs);
        result |= rhs;
        return result;
    }

    friend basic_dynamic_bitset operator^(const basic_dynamic_bitset &lhs,
                                          const basic_dynamic_bitset &rhs) {
        basic_dynamic_bitset result(lhs);
        result ^= rhs;
        return result;
    }

private:
    vector<block_type, allocator_type> m_vec;
    size_type m_bits = 0;
};

using bitset = basic_dynamic_bitset<>;

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_DYNAMIC_BITSET_HPP__
// Already included
// Already included
#ifndef WJR_TP_HPP__
#define WJR_TP_HPP__

#ifndef WJR_TP_LIST_HPP__
#define WJR_TP_LIST_HPP__

// Already included

namespace wjr {

template <typename... Args>
struct tp_list {};

template <typename T>
struct tp_is_list : std::false_type {};

template <typename... Args>
struct tp_is_list<tp_list<Args...>> : std::true_type {};

// check if is tp_list
template <typename T>
inline constexpr bool tp_is_list_v = tp_is_list<T>::value;

template <typename T>
struct tp_is_container : std::false_type {};

template <template <typename...> typename C, typename... Args>
struct tp_is_container<C<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool tp_is_container_v = tp_is_container<T>::value;

template <typename T>
struct tp_size;

template <template <typename...> typename C, typename... Args>
struct tp_size<C<Args...>> : std::integral_constant<size_t, sizeof...(Args)> {};

// get size of C<Args...>
template <typename T>
inline constexpr size_t tp_size_v = tp_size<T>::value;

template <typename T>
struct tp_is_fn : std::false_type {};

template <typename T>
inline constexpr bool tp_is_fn_v = tp_is_fn<T>::value;

template <typename _Enable, template <typename...> typename F, typename... Args>
struct __tp_is_valid_helper : std::false_type {};

template <template <typename...> typename F, typename... Args>
struct __tp_is_valid_helper<std::void_t<F<Args...>>, F, Args...> : std::true_type {};

template <template <typename...> typename F, typename... Args>
struct tp_is_valid : __tp_is_valid_helper<void, F, Args...> {};

template <template <typename...> typename F, typename... Args>
inline constexpr bool tp_is_valid_v = tp_is_valid<F, Args...>::value;

template <typename F, typename... Args>
inline constexpr bool tp_is_valid_f = tp_is_valid_v<F::template fn, Args...>;

template <template <typename...> typename F, typename... Args>
struct __tp_defer_helper {
    using type = F<Args...>;
};

template <template <typename...> typename F, typename... Args>
struct tp_defer {
    using type = std::enable_if_t<tp_is_valid_v<F, Args...>,
                                  typename __tp_defer_helper<F, Args...>::type>;
};

// use std::enable_if_t to defer the instantiation of F<Args...>
template <template <typename...> typename F, typename... Args>
using tp_defer_t = typename tp_defer<F, Args...>::type;

template <typename F, typename... Args>
using tp_defer_f = tp_defer_t<F::template fn, Args...>;

template <typename T>
struct tp_type_identity {
    using type = T;
};

// tp_type_identity_t<T> is T
template <typename T>
using tp_type_identity_t = typename tp_type_identity<T>::type;

// F1<F2<Args...>>
template <template <typename...> typename F1, template <typename...> typename F2>
struct tp_bind_fn {
    template <typename... Args>
    using fn = tp_defer_t<F1, tp_defer_t<F2, Args...>>;
};

// make F can be used as fn
template <template <typename...> typename F>
struct tp_make_fn {
    template <typename... Args>
    using fn = tp_defer_t<F, Args...>;
};

// std::negation<F<Args...>>
template <template <typename...> typename F>
struct tp_not_fn {
    template <typename... Args>
    using fn = typename tp_bind_fn<std::negation, F>::template fn<Args...>;
};

template <typename... Args>
using tp_true_type = std::true_type;

template <typename... Args>
using tp_false_type = std::false_type;

template <typename T>
struct tp_is_empty : std::bool_constant<tp_size_v<T> == 0> {};

template <typename T>
inline constexpr bool tp_is_empty_v = tp_is_empty<T>::value;

template <typename T, typename U>
struct tp_assign;

template <typename... Args1, template <typename...> typename T1, typename... Args2,
          template <typename...> typename T2>
struct tp_assign<T1<Args1...>, T2<Args2...>> {
    using type = T1<Args2...>;
};

// f(L1<Args1...>, L2<Args2...>) -> L1<Args2...>
template <typename T, typename U>
using tp_assign_t = typename tp_assign<T, U>::type;

template <typename T>
struct tp_clear;

template <template <typename...> typename T, typename... Args>
struct tp_clear<T<Args...>> {
    using type = T<>;
};

// f(L<Args...>) -> L<>
template <typename T>
using tp_clear_t = typename tp_clear<T>::type;

template <typename T, typename... Args>
struct tp_push_front;

template <template <typename...> typename C, typename... Args1, typename... Args2>
struct tp_push_front<C<Args1...>, Args2...> {
    using type = C<Args2..., Args1...>;
};

// f(L<Args1...>, Args2...) -> L<Args1..., Args2...)
template <typename T, typename... Args>
using tp_push_front_t = typename tp_push_front<T, Args...>::type;

template <typename T, typename... Args>
struct tp_push_back;

template <template <typename...> typename C, typename... Args1, typename... Args2>
struct tp_push_back<C<Args1...>, Args2...> {
    using type = C<Args1..., Args2...>;
};

// f(L<Args1...>, Args2...) -> L<Args2..., Args1...)
template <typename T, typename... Args>
using tp_push_back_t = typename tp_push_back<T, Args...>::type;

template <typename _Enable, size_t I, size_t N, typename... Args>
struct __tp_cut_helper;

template <size_t I, size_t N, typename T, typename... Args>
struct __tp_cut_helper<std::enable_if_t<N != 0, void>, I, N, T, Args...> {
    using type = typename __tp_cut_helper<void, I - 1, N, Args...>::type;
};

template <size_t I, size_t N, typename T, typename... Args>
struct __tp_cut_helper<std::enable_if_t<N == 0, void>, I, N, T, Args...> {
    using type = tp_list<>;
};

template <size_t N, typename... Args2>
struct __tp_cut_helper2;

template <size_t N, typename T, typename... Args>
struct __tp_cut_helper2<N, T, Args...> {
    using type = tp_push_front_t<typename __tp_cut_helper2<N - 1, Args...>::type, T>;
};

template <typename... Args>
struct __tp_cut_helper2<0, Args...> {
    using type = tp_list<>;
};

template <typename T, typename... Args>
struct __tp_cut_helper2<0, T, Args...> {
    using type = tp_list<>;
};

template <size_t N, typename... Args>
struct __tp_cut_helper<std::enable_if_t<N != 0>, 0, N, Args...> {
    using type = typename __tp_cut_helper2<N, Args...>::type;
};

template <size_t N, typename T, typename... Args>
struct __tp_cut_helper<std::enable_if_t<N != 0>, 0, N, T, Args...> {
    using type = typename __tp_cut_helper2<N, T, Args...>::type;
};

template <typename T, template <typename...> typename U>
struct tp_rename;

template <template <typename...> typename C, typename... Args,
          template <typename...> typename U>
struct tp_rename<C<Args...>, U> {
    using type = U<Args...>;
};

// f(L1<Args1...>, L2) -> L2<Args1...>
template <typename T, template <typename...> typename U>
using tp_rename_t = typename tp_rename<T, U>::type;

template <typename T, size_t I, size_t N>
struct tp_cut;

template <template <typename...> typename C, typename... Args, size_t I, size_t N>
struct tp_cut<C<Args...>, I, N> {
    static_assert(N <= sizeof...(Args) && I <= (sizeof...(Args) - N),
                  "tp_cut: invalid index");
    using type = tp_rename_t<typename __tp_cut_helper<void, I, N, Args...>::type, C>;
};

// f(L<Args...>, I, N) -> L<Args[I ~ I + N - 1]>
template <typename T, size_t I, size_t N>
using tp_cut_t = typename tp_cut<T, I, N>::type;

template <typename T>
struct tp_pop_front : tp_cut<T, 1, tp_size_v<T> - 1> {};

// f(L<T, Args...>) -> L<Args...>
template <typename T>
using tp_pop_front_t = typename tp_pop_front<T>::type;

template <typename T>
struct tp_pop_back : tp_cut<T, 0, tp_size_v<T> - 1> {};

// f(L<Args..., T>) -> L<Args...>
template <typename T>
using tp_pop_back_t = typename tp_pop_back<T>::type;

template <size_t index, typename... Args>
struct __tp_at_helper;

template <size_t index, typename T, typename... Args>
struct __tp_at_helper<index, T, Args...> {
    using type = typename __tp_at_helper<index - 1, Args...>::type;
};

template <typename T, typename... Args>
struct __tp_at_helper<0, T, Args...> {
    using type = T;
};

//
template <typename T, size_t index>
struct tp_at;

template <template <typename... Args> typename C, typename... Args, size_t index>
struct tp_at<C<Args...>, index> {
    static_assert(index < sizeof...(Args), "tp_at: invalid index");
    using type = typename __tp_at_helper<index, Args...>::type;
};

// f(L<Args...>, index) - > Args(index)
template <typename T, size_t index>
using tp_at_t = typename tp_at<T, index>::type;

template <typename T>
struct tp_front {
    using type = tp_at_t<T, 0>;
};

// tp_at_t(T, 0)
template <typename T>
using tp_front_t = typename tp_front<T>::type;

template <typename T>
struct tp_back {
    using type = tp_at_t<T, tp_size_v<T> - 1>;
};

// tp_at_t(T, tp_size_v<T> - 1)
template <typename T>
using tp_back_t = typename tp_back<T>::type;

template <typename T, size_t idx>
struct tp_prefix {
    using type = tp_cut_t<T, 0, idx>;
};

// f(L<Args...>, idx) -> L<Args[0 ~ idx - 1]>
template <typename T, size_t idx>
using tp_prefix_t = typename tp_prefix<T, idx>::type;

template <typename T, size_t idx>
struct tp_suffix {
    using type = tp_cut_t<T, tp_size_v<T> - idx, idx>;
};

// f(L<Args...>, idx) -> L<Args[tp_size_v<T> - idx ~ tp_size_v<T> - 1]>
template <typename T, size_t idx>
using tp_suffix_t = typename tp_suffix<T, idx>::type;

template <typename T, size_t idx>
struct tp_remove_prefix {
    using type = tp_suffix_t<T, tp_size_v<T> - idx>;
};

template <typename T, size_t idx>
using tp_remove_prefix_t = typename tp_remove_prefix<T, idx>::type;

template <typename T, size_t idx>
struct tp_remove_suffix {
    using type = tp_prefix_t<T, tp_size_v<T> - idx>;
};

template <typename T, size_t idx>
using tp_remove_suffix_t = typename tp_remove_suffix<T, idx>::type;

template <typename... Args>
struct tp_concat;

template <typename T>
struct tp_concat<T> {
    using type = T;
};

template <template <typename...> typename C1, typename... Args1,
          template <typename...> typename C2, typename... Args2>
struct tp_concat<C1<Args1...>, C2<Args2...>> {
    using type = C1<Args1..., Args2...>;
};

template <typename T, typename U, typename... Args3>
struct tp_concat<T, U, Args3...> {
    using type = typename tp_concat<typename tp_concat<T, U>::type, Args3...>::type;
};

// f(L1<Args...>, L2<Args2...>, ... Ln<Argsn...>) -> L1<Args..., Args2..., Argsn...>
template <typename... Args>
using tp_concat_t = typename tp_concat<Args...>::type;

template <typename T, size_t idx, typename U>
struct tp_replace_at {
    using type = tp_concat_t<tp_push_back_t<tp_cut_t<T, 0, idx>, U>,
                             tp_cut_t<T, idx + 1, tp_size_v<T> - idx - 1>>;
};

template <typename T, typename U>
struct tp_replace_at<T, 0, U> {
    using type = tp_push_front_t<tp_pop_front_t<T>, U>;
};

// f(L<Args...>, idx, U) -> L<Args[0 ~ idx - 1], U, Args[idx + 1 ~ tp_size_v<T> - 1]>
template <typename T, size_t idx, typename U>
using tp_replace_at_t = typename tp_replace_at<T, idx, U>::type;

template <typename T, typename U>
struct tp_replace_front_at {
    using type = tp_replace_at_t<T, 0, U>;
};

template <typename T, typename U>
using tp_replace_front_at_t = typename tp_replace_front_at<T, U>::type;

template <typename T, typename U>
struct tp_replace_back_at {
    using type = tp_replace_at_t<T, tp_size_v<T> - 1, U>;
};

template <typename T, typename U>
using tp_replace_back_at_t = typename tp_replace_back_at<T, U>::type;

template <typename V, typename T, typename... Args>
struct tp_conditional {
    using type = std::conditional_t<V::value, T, typename tp_conditional<Args...>::type>;
};

template <typename V, typename T1, typename T2>
struct tp_conditional<V, T1, T2> {
    using type = std::conditional_t<V::value, T1, T2>;
};

// f(V, T, U) -> std::conditional_t<V::value, T, U>
// f(V, T, Args...) -> std::conditional_t<V::value, T, f(Args...)>
template <typename V, typename T, typename... Args>
using tp_conditional_t = typename tp_conditional<V, T, Args...>::type;

template <size_t idx>
struct tp_arg;

template <template <typename...> typename F, typename... Args>
struct tp_bind;

template <template <typename...> typename F, typename... Args>
struct tp_bind_front;

template <template <typename...> typename F, typename... Args>
struct tp_bind_back;

template <size_t idx>
struct tp_is_fn<tp_arg<idx>> : std::true_type {};

template <template <typename...> typename F, typename... Args>
struct tp_is_fn<tp_bind<F, Args...>> : std::true_type {};

template <template <typename...> typename F, typename... Args>
struct tp_is_fn<tp_bind_front<F, Args...>> : std::true_type {};

template <template <typename...> typename F, typename... Args>
struct tp_is_fn<tp_bind_back<F, Args...>> : std::true_type {};

template <size_t idx>
struct tp_arg {
    template <typename... Args>
    using fn = tp_at_t<tp_list<Args...>, idx>;
};

template <template <typename...> typename F, typename T>
struct tp_apply {
    using type = tp_rename_t<T, F>;
};

// f(F, L<Args...>) -> F<Args...>
// same as tp_rename_t(L<Args...>, F)
template <template <typename...> typename F, typename T>
using tp_apply_t = typename tp_apply<F, T>::type;

template <typename F, typename T>
using tp_apply_f = tp_apply_t<F::template fn, T>;

template <typename _Enable, typename T, typename... Args>
struct __tp_bind_helper {
    using type = T;
};

template <typename F, typename... Args>
struct __tp_bind_helper<std::enable_if_t<tp_is_fn_v<F>, void>, F, Args...> {
    using type = typename F::template fn<Args...>;
};

template <template <typename...> typename F, typename... Args>
struct tp_bind {
    template <typename... Args2>
    using fn = F<typename __tp_bind_helper<void, Args, Args2...>::type...>;
};

template <typename F, typename... Args>
using tp_bind_f = tp_bind<F::template fn, Args...>;

template <template <typename...> typename F, typename... Args>
struct tp_bind_front {
    template <typename... Args2>
    using fn = tp_defer_t<F, Args..., Args2...>;
};

template <typename F, typename... Args>
using tp_bind_front_f = tp_bind_front<F::template fn, Args...>;

template <template <typename...> typename F, typename... Args>
struct tp_bind_back {
    template <typename... Args2>
    using fn = tp_defer_t<F, Args2..., Args...>;
};

template <typename F, typename... Args>
using tp_bind_back_f = tp_bind_back<F::template fn, Args...>;

template <typename T, template <typename...> typename F>
struct tp_transform;

template <template <typename...> typename C, typename... Args,
          template <typename...> typename F>
struct tp_transform<C<Args...>, F> {
    using type = C<F<Args>...>;
};

// f(L<Args...>, Fn) -> L<Fn(Args)...>
// use with apply, bind, bind_front, bind_back...
// for example:
// tp_transform_f<tp_bind_front<tp_apply_f, tp_bind_front<std::is_same>>,
// tp_list<tp_list<int, float>, tp_list<float, float>, tp_list<int, double>>>
// -> tp_list<std::is_same<int, float>, std::is_same<float, float>, std::is_same<int,
// double>>
template <typename T, template <typename...> typename F>
using tp_transform_t = typename tp_transform<T, F>::type;

template <typename T, typename F>
using tp_transform_f = typename tp_transform<T, F::template fn>::type;

template <template <typename...> typename C, typename... Args>
struct tp_zip;

template <template <typename...> typename C, typename T>
struct __tp_zip_helper;

template <template <typename...> typename C, size_t... idx>
struct __tp_zip_helper<C, std::index_sequence<idx...>> {
    template <size_t I, typename... Args>
    using __type = C<tp_at_t<Args, I>...>;
    template <typename... Args>
    using type = tp_list<__type<idx, Args...>...>;
};

template <template <typename...> typename C>
struct tp_zip<C> {
    using type = tp_list<>;
};

template <template <typename...> typename C, typename T>
struct tp_zip<C, T> {
    using type = typename __tp_zip_helper<
        C, std::make_index_sequence<tp_size_v<T>>>::template type<T>;
};

template <template <typename...> typename C, typename T, typename... Args>
struct tp_zip<C, T, Args...> {
    constexpr static size_t size = tp_size_v<T>;
    static_assert(((size == tp_size_v<Args>)&&...),
                  "tp_zip arguments must have same size, \
		you can make all arguments have same size by tp_");
    using type = typename __tp_zip_helper<
        C, std::make_index_sequence<tp_size_v<T>>>::template type<T, Args...>;
};

// f(C, L<A1, A2, ... An>, L<B1, B2, ..., Bn> ...)
// -> L<C<A1, B1, ...>, C<A2, B2, ...>, ..., C<An, Bn, ...>>
template <template <typename...> typename C, typename... Args>
using tp_zip_t = typename tp_zip<C, Args...>::type;

template <typename... Args>
struct __tp_max_size_helper;

template <typename T>
struct __tp_max_size_helper<T> {
    constexpr static size_t value = tp_size_v<T>;
};

template <typename T, typename... Args>
struct __tp_max_size_helper<T, Args...> {
    constexpr static size_t value =
        std::max(tp_size_v<T>, __tp_max_size_helper<Args...>::value);
};

template <typename T, typename... Args>
struct tp_max_size {
    constexpr static size_t value = __tp_max_size_helper<T, Args...>::value;
};

// tp_max_size_v<T, Args...> -> size_t
template <typename T, typename... Args>
inline constexpr size_t tp_max_size_v = tp_max_size<T, Args...>::value;

template <typename T>
struct tp_unwrap {
    static_assert(tp_size_v<T> == 1, "only container that size = 1 can use unwrap");
};

template <template <typename...> typename C, typename T>
struct tp_unwrap<C<T>> {
    using type = T;
};

// f(C<T>) -> T
template <typename T>
using tp_unwrap_t = typename tp_unwrap<T>::type;

template <typename T, template <typename...> typename P, typename U>
struct tp_replace_if;

template <template <typename...> typename C, typename... Args,
          template <typename...> typename P, typename U>
struct tp_replace_if<C<Args...>, P, U> {
    using type = C<tp_conditional_t<P<Args>, U, Args>...>;
};

// f(L<Args...>, P, U) -> L<if P(Args)::value then U else Args...>
template <typename T, template <typename...> typename P, typename U>
using tp_replace_if_t = typename tp_replace_if<T, P, U>::type;

template <typename T, typename P, typename U>
using tp_replace_if_f = tp_replace_if_t<T, P::template fn, U>;

template <typename T, typename U>
struct tp_replace_if_true {
    using type = tp_replace_if_t<T, tp_type_identity_t, U>;
};

template <typename T, typename U>
using tp_replace_if_true_t = typename tp_replace_if_true<T, U>::type;

template <typename T, typename U>
struct tp_replace_if_false {
    using type = tp_replace_if_f<T, tp_not_fn<tp_type_identity_t>, U>;
};

template <typename T, typename U>
using tp_replace_if_false_t = typename tp_replace_if_false<T, U>::type;

template <typename T, typename O, typename N>
struct tp_replace {
    using type = tp_replace_if_f<T, tp_bind_front<std::is_same, O>, N>;
};

template <typename T, typename O, typename N>
using tp_replace_t = typename tp_replace<T, O, N>::type;

template <typename T, typename U>
struct tp_fill {
    using type = tp_replace_if_t<T, tp_true_type, U>;
};

// f(L<Args...>, U) -> L<U, U, ..., U>
template <typename T, typename U>
using tp_fill_t = typename tp_fill<T, U>::type;

template <typename T, template <typename...> typename P>
struct tp_count_if;

template <template <typename...> typename C, template <typename...> typename P>
struct tp_count_if<C<>, P> {
    static constexpr size_t value = 0;
};

template <template <typename...> typename C, typename... Args,
          template <typename...> typename P>
struct tp_count_if<C<Args...>, P> {
    static constexpr size_t value = (P<Args>::value + ...);
};

// f(L<Args...>, P) -> count(P(Args)::value)
template <typename T, template <typename...> typename P>
constexpr size_t tp_count_if_v = tp_count_if<T, P>::value;

template <typename T, typename P>
constexpr size_t tp_count_if_f_v = tp_count_if_v<T, P::template fn>;

template <typename T, typename V>
struct tp_count {
    static constexpr size_t value = tp_count_if_f_v<T, tp_bind_front<std::is_same, V>>;
};

template <typename T, typename V>
constexpr size_t tp_count_v = tp_count<T, V>::value;

template <typename T, typename V>
struct tp_contains {
    static constexpr bool value = tp_count_v<T, V> != 0;
};

template <typename T, typename V>
constexpr bool tp_contains_v = tp_contains<T, V>::value;

template <typename T, template <typename...> typename P>
struct tp_remove_if;

template <template <typename...> typename C, typename... Args,
          template <typename...> typename P>
struct tp_remove_if<C<Args...>, P> {
    using type = tp_concat_t<C<>, tp_conditional_t<P<Args>, C<>, C<Args>>...>;
};

// f(L<Args...>, P) -> L<if P(Args)::value then L<> else L<Args>...>
template <typename T, template <typename...> typename P>
using tp_remove_if_t = typename tp_remove_if<T, P>::type;

template <typename T, typename P>
using tp_remove_if_f = tp_remove_if_t<T, P::template fn>;

template <typename T, typename V>
struct tp_remove {
    using type = tp_remove_if_f<T, tp_bind_front<std::is_same, V>>;
};

template <typename T, typename V>
using tp_remove_t = typename tp_remove<T, V>::type;

template <typename T, template <typename...> typename P>
struct tp_filter {
    using type = tp_remove_if_f<T, tp_not_fn<P>>;
};

template <typename T, template <typename...> typename P>
using tp_filter_t = typename tp_filter<T, P>::type;

template <typename T, typename P>
using tp_filter_f = tp_filter_t<T, P::template fn>;

template <typename T, typename U>
struct tp_equal;

template <typename _Enable, typename T, typename U>
struct __tp_equal_helper : std::false_type {};

template <template <typename...> typename C, typename... Args,
          template <typename...> typename D, typename... Args2>
struct __tp_equal_helper<std::enable_if_t<sizeof...(Args) == sizeof...(Args2), void>,
                         C<Args...>, D<Args2...>>
    : std::conjunction<std::is_same<Args, Args2>...> {};

template <typename T, typename U>
struct tp_equal : __tp_equal_helper<void, T, U> {};

template <typename T, typename U>
inline constexpr bool tp_equal_v = tp_equal<T, U>::value;

template <typename T, size_t N>
struct tp_repeat {
    using type = tp_concat_t<T, typename tp_repeat<T, N - 1>::type>;
};

template <typename T>
struct tp_repeat<T, 0> {
    using type = tp_clear_t<T>;
};

template <typename C, size_t N>
using tp_repeat_t = typename tp_repeat<C, N>::type;

template <typename _Enable, typename C, size_t N, typename V>
struct __tp_resize_helper {
    using type = tp_cut_t<C, 0, N>;
};

template <typename C, size_t N, typename V>
struct __tp_resize_helper<std::enable_if_t<N >= tp_size_v<C>, void>, C, N, V> {
    using type = tp_concat_t<C, tp_repeat_t<V, N - tp_size_v<C>>>;
};

template <typename C, size_t N, typename V>
struct tp_resize {
    using tyep = typename __tp_resize_helper<void, C, N, V>::type;
};

template <typename C, size_t N, typename V>
using tp_resize_t = typename tp_resize<C, N, V>::type;

template <template <typename...> typename C, typename... Args>
struct tp_product;

template <typename _Enable, template <typename...> typename C, typename... Args>
struct __tp_product_helper {
    using type = tp_list<>;
};

template <typename _Enable, template <typename...> typename C, typename T>
struct __tp_product_helper<_Enable, C, T> {
    using type = tp_list<tp_rename_t<T, C>>;
};

template <template <typename...> typename C, typename T,
          template <typename...> typename C1, typename... Args1, typename... Args>
struct __tp_product_helper<std::enable_if_t<sizeof...(Args1) != 0, void>, C, T,
                           C1<Args1...>, Args...> {
    using type =
        tp_concat_t<typename __tp_product_helper<void, C, tp_push_back_t<T, Args1>,
                                                 Args...>::type...>;
};

template <template <typename...> typename C, typename... Args>
struct tp_product {
    using type = typename __tp_product_helper<void, C, tp_list<>, Args...>::type;
};

// for example
// f(C, L<A1, A2>, L<B1, B2, B3>) -> L<C<A1, B1>, C<A1, B2>, C<A1, B3>, C<A2, B1>, C<A2,
// B2>, C<A2, B3>>
template <template <typename...> typename C, typename... Args>
using tp_product_t = typename tp_product<C, Args...>::type;

template <typename C, size_t I, typename... Args>
struct tp_insert {
    static_assert(I <= tp_size_v<C>, "tp insert index out of range");
    using type = tp_concat_t<tp_push_back_t<tp_prefix_t<C, I>, Args...>,
                             tp_suffix_t<C, tp_size_v<C> - I>>;
};

template <typename C, size_t I, typename... Args>
using tp_insert_t = typename tp_insert<C, I, Args...>::type;

template <typename C, size_t I, size_t N>
struct tp_erase {
    static_assert(N <= tp_size_v<C> && I <= tp_size_v<C> - N,
                  "tp erase index out of range");
    using type = tp_concat_t<tp_prefix_t<C, I>, tp_suffix_t<C, tp_size_v<C> - I - N>>;
};

template <typename C, size_t I, size_t N>
using tp_erase_t = typename tp_erase<C, I, N>::type;

template <typename C>
struct tp_reverse;

template <template <typename...> typename C>
struct tp_reverse<C<>> {
    using type = C<>;
};

template <template <typename...> typename C, typename T, typename... Args>
struct tp_reverse<C<T, Args...>> {
    using type = tp_push_back_t<typename tp_reverse<C<Args...>>::type, T>;
};

template <typename C>
using tp_reverse_t = typename tp_reverse<C>::type;

template <typename _Enable, size_t idx, typename C, template <typename...> typename P>
struct __tp_find_if_helper;

template <typename _Enable, size_t idx, template <typename...> typename C, typename T,
          typename... Args, template <typename...> typename P>
struct __tp_find_if_helper<_Enable, idx, C<T, Args...>, P> {
    constexpr static size_t value =
        __tp_find_if_helper<void, idx + 1, C<Args...>, P>::value;
};

template <typename _Enable, size_t idx, template <typename...> typename C,
          template <typename...> typename P>
struct __tp_find_if_helper<_Enable, idx, C<>, P> {
    constexpr static size_t value = -1;
};

template <size_t idx, template <typename...> typename C, typename T, typename... Args,
          template <typename...> typename P>
struct __tp_find_if_helper<std::enable_if_t<P<T>::value, void>, idx, C<T, Args...>, P> {
    constexpr static size_t value = idx;
};

template <typename C, template <typename...> typename P>
struct tp_find_if {
    constexpr static size_t value = __tp_find_if_helper<void, 0, C, P>::value;
};

template <typename C, template <typename...> typename P>
inline constexpr size_t tp_find_if_v = tp_find_if<C, P>::value;

template <typename C, typename P>
inline constexpr size_t tp_find_if_f = tp_find_if<C, P::template fn>::value;

template <typename C, template <typename...> typename P>
struct tp_find_if_not {
    constexpr static size_t value = tp_find_if_f<C, tp_not_fn<P>>;
};

template <typename C, template <typename...> typename P>
inline constexpr size_t tp_find_if_not_v = tp_find_if_not<C, P>::value;

template <typename C, typename P>
inline constexpr size_t tp_find_if_not_f = tp_find_if_not<C, P::template fn>::value;

template <typename C, typename V>
struct tp_find {
    constexpr static size_t value = tp_find_if_f<C, tp_bind_front<std::is_same, V>>;
};

template <typename C, typename V>
inline constexpr size_t tp_find_v = tp_find<C, V>::value;

template <typename C, typename E, template <typename...> typename F>
struct tp_left_fold;

template <template <typename...> typename C, typename E,
          template <typename...> typename F>
struct tp_left_fold<C<>, E, F> {
    using type = E;
};

template <template <typename...> typename C, typename T, typename... Args, typename E,
          template <typename...> typename F>
struct tp_left_fold<C<T, Args...>, E, F> {
    using type = typename tp_left_fold<C<Args...>, F<E, T>, F>::type;
};

// f(L<A1, A2, ... An>, E, F) -> F<F<F...<F<E, A1>, A2>, ...>, An>
template <typename C, typename E, template <typename...> typename F>
using tp_left_fold_t = typename tp_left_fold<C, E, F>::type;

template <typename C, typename E, typename F>
using tp_left_fold_f = typename tp_left_fold<C, E, F::template fn>::type;

template <typename C, typename E, template <typename...> typename F>
struct tp_right_fold;

template <template <typename...> typename C, typename E,
          template <typename...> typename F>
struct tp_right_fold<C<>, E, F> {
    using type = E;
};

template <template <typename...> typename C, typename T, typename... Args, typename E,
          template <typename...> typename F>
struct tp_right_fold<C<T, Args...>, E, F> {
    using next_type = typename tp_right_fold<C<Args...>, E, F>::type;
    using type = F<T, next_type>;
};

// f(L<A1, A2, ... An>, E, F) -> F<A1, F<A2, ... F<An, E>...>>
template <typename C, typename E, template <typename...> typename F>
using tp_right_fold_t = typename tp_right_fold<C, E, F>::type;

template <typename C, typename E, typename F>
using tp_right_fold_f = typename tp_right_fold<C, E, F::template fn>::type;

template <typename C, template <typename...> typename P>
struct tp_unique_if {
    using type = tp_left_fold_f<C, tp_clear_t<C>,
                                tp_bind<tp_conditional_t, tp_bind_front<P>, tp_arg<0>,
                                        tp_bind_front<tp_push_back_t>>>;
};

// using NOW_LIST = tp_prefix_t<C, I + 1>;
// using PRE_LIST = tp_prefix_t<C, I>;
// using PRE_UNIQUE_IF_LIST = tp_unique_if_t<PRE_LIST>;
// then :
// tp_unique_if_t<NOW_LIST, P>
// = tp_conditonal_t<
// P<PRE_UNIQUE_IF_LIST, tp_at_t<C, I>>,
// PRE_UNIQUE_IF_LIST,
// tp_push_back_t<PRE_UNIQUE_IF_LIST, tp_at_t<C, I>>>
//
// It is equivalent to calling P every time on the results
// of the previous processing and the new value.
// If P is false, the new value is added
template <typename C, template <typename...> typename P>
using tp_unique_if_t = typename tp_unique_if<C, P>::type;

template <typename C, typename P>
using tp_unique_if_f = typename tp_unique_if<C, P::template fn>::type;

template <typename C>
struct tp_unique {
    using type = tp_unique_if_t<C, tp_contains>;
};

// same as tp_unique_if_t<C, tp_contains>
// remove the same type
template <typename C>
using tp_unique_t = typename tp_unique<C>::type;

template <typename _Enable, typename C, typename C1, typename C2,
          template <typename...> typename P>
struct __tp_merge_helper;

template <typename _Enable, template <typename...> typename C, typename... Args,
          template <typename...> typename C1, template <typename...> typename C2,
          typename... Args2, template <typename...> typename P>
struct __tp_merge_helper<_Enable, C<Args...>, C1<>, C2<Args2...>, P> {
    using type = tp_list<Args..., Args2...>;
};

template <typename _Enable, template <typename...> typename C, typename... Args,
          template <typename...> typename C1, typename... Args1,
          template <typename...> typename C2, template <typename...> typename P>
struct __tp_merge_helper<_Enable, C<Args...>, C1<Args1...>, C2<>, P> {
    using type = tp_list<Args..., Args1...>;
};

template <typename _Enable, template <typename...> typename C, typename... Args,
          template <typename...> typename C1, template <typename...> typename C2,
          template <typename...> typename P>
struct __tp_merge_helper<_Enable, C<Args...>, C1<>, C2<>, P> {
    using type = tp_list<Args...>;
};

template <template <typename...> typename C, typename... Args,
          template <typename...> typename C1, typename T1, typename... Args1,
          template <typename...> typename C2, typename T2, typename... Args2,
          template <typename...> typename P>
struct __tp_merge_helper<std::enable_if_t<P<T1, T2>::value, void>, C<Args...>,
                         C1<T1, Args1...>, C2<T2, Args2...>, P> {
    using type = typename __tp_merge_helper<void, C<Args..., T1>, C1<Args1...>,
                                            C2<T2, Args2...>, P>::type;
};

template <template <typename...> typename C, typename... Args,
          template <typename...> typename C1, typename T1, typename... Args1,
          template <typename...> typename C2, typename T2, typename... Args2,
          template <typename...> typename P>
struct __tp_merge_helper<std::enable_if_t<!P<T1, T2>::value, void>, C<Args...>,
                         C1<T1, Args1...>, C2<T2, Args2...>, P> {
    using type = typename __tp_merge_helper<void, C<Args..., T2>, C1<T1, Args1...>,
                                            C2<Args2...>, P>::type;
};

template <typename C1, typename C2, template <typename...> typename P>
struct tp_merge {
    using type = typename __tp_merge_helper<void, tp_list<>, C1, C2, P>::type;
};

// like std::merge
// merge two list with P
template <typename C1, typename C2, template <typename...> typename P>
using tp_merge_t = typename tp_merge<C1, C2, P>::type;

template <typename C1, typename C2, typename P>
using tp_merge_f = typename tp_merge<C1, C2, P::template fn>::type;

template <typename C, template <typename...> typename P>
struct tp_sort;

template <typename C, template <typename...> typename P>
struct __tp_sort_helper;

template <template <typename...> typename C, typename... Args,
          template <typename...> typename P>
struct __tp_sort_helper<C<Args...>, P> {
    using _Container = C<Args...>;
    constexpr static size_t size = tp_size_v<_Container>;
    constexpr static size_t mid = size / 2;
    using type1 = typename __tp_sort_helper<tp_prefix_t<_Container, mid>, P>::type;
    using type2 = typename __tp_sort_helper<tp_suffix_t<_Container, size - mid>, P>::type;
    using type = tp_merge_t<type1, type2, P>;
};

template <template <typename...> typename C, typename T,
          template <typename...> typename P>
struct __tp_sort_helper<C<T>, P> {
    using type = C<T>;
};

template <template <typename...> typename C, template <typename...> typename P>
struct __tp_sort_helper<C<>, P> {
    using type = C<>;
};

template <template <typename...> typename C, typename... Args,
          template <typename...> typename P>
struct tp_sort<C<Args...>, P> {
    using type = tp_rename_t<typename __tp_sort_helper<C<Args...>, P>::type, C>;
};

// list std::sort
template <typename C, template <typename...> typename P>
using tp_sort_t = typename tp_sort<C, P>::type;

template <typename C, typename P>
using tp_sort_f = typename tp_sort<C, P::template fn>::type;

} // namespace wjr

#endif // WJR_TP_LIST_HPP__

#endif // WJR_TP_HPP__
// Already included
