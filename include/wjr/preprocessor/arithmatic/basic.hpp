#ifndef WJR_PREPROCESSOR_ARITHMATIC_BASIC_HPP
#define WJR_PREPROCESSOR_ARITHMATIC_BASIC_HPP

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

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_BASIC_HPP