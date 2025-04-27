// Copyright 2020-2024 Junekey Jeon
//
// The contents of this file may be used under the terms of
// the Apache License v2.0 with LLVM Exceptions.
//
//    (See accompanying file LICENSE-Apache or copy at
//     https://llvm.org/foundation/relicensing/LICENSE.txt)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

#include <wjr/format/charconv.hpp>
#include <wjr/format/dragonbox.hpp>

#ifndef JKJ_STATIC_DATA_SECTION
    #define JKJ_STATIC_DATA_SECTION
#endif

namespace wjr {
namespace dragonbox {
namespace detail {

inline constexpr auto &radix_100_table = __char_converter_table<char_converter_t, 10, 2>;

inline constexpr char radix_100_head_table[200] JKJ_STATIC_DATA_SECTION = {
    '0', '.', '1', '.', '2', '.', '3', '.', '4', '.', //
    '5', '.', '6', '.', '7', '.', '8', '.', '9', '.', //
    '1', '.', '1', '.', '1', '.', '1', '.', '1', '.', //
    '1', '.', '1', '.', '1', '.', '1', '.', '1', '.', //
    '2', '.', '2', '.', '2', '.', '2', '.', '2', '.', //
    '2', '.', '2', '.', '2', '.', '2', '.', '2', '.', //
    '3', '.', '3', '.', '3', '.', '3', '.', '3', '.', //
    '3', '.', '3', '.', '3', '.', '3', '.', '3', '.', //
    '4', '.', '4', '.', '4', '.', '4', '.', '4', '.', //
    '4', '.', '4', '.', '4', '.', '4', '.', '4', '.', //
    '5', '.', '5', '.', '5', '.', '5', '.', '5', '.', //
    '5', '.', '5', '.', '5', '.', '5', '.', '5', '.', //
    '6', '.', '6', '.', '6', '.', '6', '.', '6', '.', //
    '6', '.', '6', '.', '6', '.', '6', '.', '6', '.', //
    '7', '.', '7', '.', '7', '.', '7', '.', '7', '.', //
    '7', '.', '7', '.', '7', '.', '7', '.', '7', '.', //
    '8', '.', '8', '.', '8', '.', '8', '.', '8', '.', //
    '8', '.', '8', '.', '8', '.', '8', '.', '8', '.', //
    '9', '.', '9', '.', '9', '.', '9', '.', '9', '.', //
    '9', '.', '9', '.', '9', '.', '9', '.', '9', '.'  //
};

static void print_1_digit(int n, char *buffer) noexcept { *buffer = char_converter.to(n); }

static void print_2_digits(int n, char *buffer) noexcept {
    __to_chars_unroll_2<10>(reinterpret_cast<uint8_t *>(buffer), n, char_converter);
}

// These digit generation routines are inspired by James Anhalt's itoa
// algorithm: https://github.com/jeaiii/itoa The main idea is for given n, find
// y such that floor(10^k * y / 2^32) = n holds, where k is an appropriate
// integer depending on the length of n. For example, if n = 1234567, we set k
// = 6. In this case, we have floor(y / 2^32) = 1, floor(10^2 * ((10^0 * y) mod
// 2^32) / 2^32) = 23, floor(10^2 * ((10^2 * y) mod 2^32) / 2^32) = 45, and
// floor(10^2 * ((10^4 * y) mod 2^32) / 2^32) = 67.
// See https://jk-jeon.github.io/posts/2022/02/jeaiii-algorithm/ for more
// explanation.

WJR_INTRINSIC_INLINE static void print_9_digits(std::uint32_t s32, int &exponent,
                                                char *&buffer) noexcept {
    // -- IEEE-754 binary32
    // Since we do not cut trailing zeros in advance, s32 must be of 6~9 digits
    // unless the original input was subnormal.
    // In particular, when it is of 9 digits it shouldn't have any trailing
    // zeros.
    // -- IEEE-754 binary64
    // In this case, s32 must be of 7~9 digits unless the input is subnormal,
    // and it shouldn't have any trailing zeros if it is of 9 digits.
    if (s32 >= UINT32_C(100000000)) {
        // 9 digits.
        // 1441151882 = ceil(2^57 / 1'0000'0000) + 1
        auto prod = s32 * UINT64_C(1441151882);
        prod >>= 25;
        builtin_memcpy(buffer, radix_100_head_table + int(prod >> 32) * 2, 2);

        prod = (prod & UINT32_C(0xffffffff)) * 100;
        print_2_digits(int(prod >> 32), buffer + 2);
        prod = (prod & UINT32_C(0xffffffff)) * 100;
        print_2_digits(int(prod >> 32), buffer + 4);
        prod = (prod & UINT32_C(0xffffffff)) * 100;
        print_2_digits(int(prod >> 32), buffer + 6);
        prod = (prod & UINT32_C(0xffffffff)) * 100;
        print_2_digits(int(prod >> 32), buffer + 8);

        exponent += 8;
        buffer += 10;
    } else if (s32 >= UINT32_C(1000000)) {
        // 7 or 8 digits.
        // 281474978 = ceil(2^48 / 100'0000) + 1
        auto prod = s32 * UINT64_C(281474978);
        prod >>= 16;
        auto const head_digits = int(prod >> 32);
        // If s32 is of 8 digits, increase the exponent by 7.
        // Otherwise, increase it by 6.
        exponent += (6 + int(head_digits >= 10));

        // Write the first digit and the decimal point.
        builtin_memcpy(buffer, radix_100_head_table + head_digits * 2, 2);
        // This third character may be overwritten later but we don't care.
        buffer[2] = radix_100_table[head_digits * 2 + 1];

        // Remaining 6 digits are all zero?
        if ((prod & UINT32_C(0xffffffff)) <=
            std::uint32_t((std::uint64_t(1) << 32) / UINT32_C(1000000))) {
            // The number of characters actually need to be written is:
            //   1, if only the first digit is nonzero, which means that either
            //   s32 is of 7 digits or it is of 8 digits but the second digit is
            //   zero, or 3, otherwise.
            // Note that buffer[2] is never '0' if s32 is of 7 digits, because
            // the input is never zero.
            buffer += (1 + (int(head_digits >= 10) & int(buffer[2] > '0')) * 2);
        } else {
            // At least one of the remaining 6 digits are nonzero.
            // After this adjustment, now the first destination becomes buffer
            // + 2.
            buffer += int(head_digits >= 10);

            // Obtain the next two digits.
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 2);

            // Remaining 4 digits are all zero?
            if ((prod & UINT32_C(0xffffffff)) <= std::uint32_t((std::uint64_t(1) << 32) / 10000)) {
                buffer += (3 + int(buffer[3] > '0'));
            } else {
                // At least one of the remaining 4 digits are nonzero.

                // Obtain the next two digits.
                prod = (prod & UINT32_C(0xffffffff)) * 100;
                print_2_digits(int(prod >> 32), buffer + 4);

                // Remaining 2 digits are all zero?
                if ((prod & UINT32_C(0xffffffff)) <=
                    std::uint32_t((std::uint64_t(1) << 32) / 100)) {
                    buffer += (5 + int(buffer[5] > '0'));
                } else {
                    // Obtain the last two digits.
                    prod = (prod & UINT32_C(0xffffffff)) * 100;
                    print_2_digits(int(prod >> 32), buffer + 6);

                    buffer += (7 + int(buffer[7] > '0'));
                }
            }
        }
    } else if (s32 >= 10000) {
        // 5 or 6 digits.
        // 429497 = ceil(2^32 / 1'0000)
        auto prod = s32 * UINT64_C(429497);
        auto const head_digits = int(prod >> 32);

        // If s32 is of 6 digits, increase the exponent by 5.
        // Otherwise, increase it by 4.
        exponent += (4 + int(head_digits >= 10));

        // Write the first digit and the decimal point.
        builtin_memcpy(buffer, radix_100_head_table + head_digits * 2, 2);
        // This third character may be overwritten later but we don't care.
        buffer[2] = radix_100_table[head_digits * 2 + 1];

        // Remaining 4 digits are all zero?
        if ((prod & UINT32_C(0xffffffff)) <= std::uint32_t((std::uint64_t(1) << 32) / 10000)) {
            // The number of characters actually written is 1 or 3, similarly to
            // the case of 7 or 8 digits.
            buffer += (1 + (int(head_digits >= 10) & int(buffer[2] > '0')) * 2);
        } else {
            // At least one of the remaining 4 digits are nonzero.
            // After this adjustment, now the first destination becomes buffer
            // + 2.
            buffer += int(head_digits >= 10);

            // Obtain the next two digits.
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 2);

            // Remaining 2 digits are all zero?
            if ((prod & UINT32_C(0xffffffff)) <= std::uint32_t((std::uint64_t(1) << 32) / 100)) {
                buffer += (3 + int(buffer[3] > '0'));
            } else {
                // Obtain the last two digits.
                prod = (prod & UINT32_C(0xffffffff)) * 100;
                print_2_digits(int(prod >> 32), buffer + 4);

                buffer += (5 + int(buffer[5] > '0'));
            }
        }
    } else if (s32 >= 100) {
        // 3 or 4 digits.
        // 42949673 = ceil(2^32 / 100)
        auto prod = s32 * UINT64_C(42949673);
        auto const head_digits = int(prod >> 32);

        // If s32 is of 4 digits, increase the exponent by 3.
        // Otherwise, increase it by 2.
        exponent += (2 + int(head_digits >= 10));

        // Write the first digit and the decimal point.
        builtin_memcpy(buffer, radix_100_head_table + head_digits * 2, 2);
        // This third character may be overwritten later but we don't care.
        buffer[2] = radix_100_table[head_digits * 2 + 1];

        // Remaining 2 digits are all zero?
        if ((prod & UINT32_C(0xffffffff)) <= std::uint32_t((std::uint64_t(1) << 32) / 100)) {
            // The number of characters actually written is 1 or 3, similarly to
            // the case of 7 or 8 digits.
            buffer += (1 + (int(head_digits >= 10) & int(buffer[2] > '0')) * 2);
        } else {
            // At least one of the remaining 2 digits are nonzero.
            // After this adjustment, now the first destination becomes buffer
            // + 2.
            buffer += int(head_digits >= 10);

            // Obtain the last two digits.
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 2);

            buffer += (3 + int(buffer[3] > '0'));
        }
    } else {
        // 1 or 2 digits.
        // If s32 is of 2 digits, increase the exponent by 1.
        exponent += int(s32 >= 10);

        // Write the first digit and the decimal point.
        builtin_memcpy(buffer, radix_100_head_table + s32 * 2, 2);
        // This third character may be overwritten later but we don't care.
        buffer[2] = radix_100_table[s32 * 2 + 1];

        // The number of characters actually written is 1 or 3, similarly to the
        // case of 7 or 8 digits.
        buffer += (1 + (int(s32 >= 10) & int(buffer[2] > '0')) * 2);
    }
}

template <>
char *to_chars<ieee754_binary32, std::uint32_t>(std::uint32_t s32, int exponent,
                                                char *buffer) noexcept {
    // Print significand.
    print_9_digits(s32, exponent, buffer);

    // Print exponent and return
    if (exponent < 0) {
        builtin_memcpy(buffer, "E-", 2);
        buffer += 2;
        exponent = -exponent;
    } else {
        buffer[0] = 'E';
        buffer += 1;
    }

    if (exponent >= 10) {
        print_2_digits(exponent, buffer);
        buffer += 2;
    } else {
        print_1_digit(exponent, buffer);
        buffer += 1;
    }

    return buffer;
}

template <>
char *to_chars<ieee754_binary64, std::uint64_t>(std::uint64_t const significand, int exponent,
                                                char *buffer) noexcept {
    // Print significand by decomposing it into a 9-digit block and a 8-digit
    // block.
    std::uint32_t first_block, second_block;
    bool no_second_block;

    if (significand >= UINT64_C(100000000)) {
        first_block = std::uint32_t(significand / UINT64_C(100000000));
        second_block = std::uint32_t(significand) - first_block * UINT32_C(100000000);
        exponent += 8;
        no_second_block = (second_block == 0);
    } else {
        first_block = std::uint32_t(significand);
        no_second_block = true;
    }

    if (no_second_block) {
        print_9_digits(first_block, exponent, buffer);
    } else {
        // We proceed similarly to print_9_digits(), but since we do not need to
        // remove trailing zeros, the procedure is a bit simpler.
        if (first_block >= UINT32_C(100000000)) {
            // The input is of 17 digits, thus there should be no trailing zero
            // at all. The first block is of 9 digits. 1441151882 = ceil(2^57 /
            // 1'0000'0000) + 1
            auto prod = first_block * UINT64_C(1441151882);
            prod >>= 25;
            builtin_memcpy(buffer, radix_100_head_table + int(prod >> 32) * 2, 2);
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 2);
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 4);
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 6);
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 8);

            // The second block is of 8 digits.
            // 281474978 = ceil(2^48 / 100'0000) + 1
            prod = second_block * UINT64_C(281474978);
            prod >>= 16;
            prod += 1;
            print_2_digits(int(prod >> 32), buffer + 10);
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 12);
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 14);
            prod = (prod & UINT32_C(0xffffffff)) * 100;
            print_2_digits(int(prod >> 32), buffer + 16);

            exponent += 8;
            buffer += 18;
        } else {
            if (first_block >= UINT32_C(1000000)) {
                // 7 or 8 digits.
                // 281474978 = ceil(2^48 / 100'0000) + 1
                auto prod = first_block * UINT64_C(281474978);
                prod >>= 16;
                auto const head_digits = int(prod >> 32);

                builtin_memcpy(buffer, radix_100_head_table + head_digits * 2, 2);
                buffer[2] = radix_100_table[head_digits * 2 + 1];

                exponent += (6 + int(head_digits >= 10));
                buffer += int(head_digits >= 10);

                // Print remaining 6 digits.
                prod = (prod & UINT32_C(0xffffffff)) * 100;
                print_2_digits(int(prod >> 32), buffer + 2);
                prod = (prod & UINT32_C(0xffffffff)) * 100;
                print_2_digits(int(prod >> 32), buffer + 4);
                prod = (prod & UINT32_C(0xffffffff)) * 100;
                print_2_digits(int(prod >> 32), buffer + 6);

                buffer += 8;
            } else if (first_block >= 10000) {
                // 5 or 6 digits.
                // 429497 = ceil(2^32 / 1'0000)
                auto prod = first_block * UINT64_C(429497);
                auto const head_digits = int(prod >> 32);

                builtin_memcpy(buffer, radix_100_head_table + head_digits * 2, 2);
                buffer[2] = radix_100_table[head_digits * 2 + 1];

                exponent += (4 + int(head_digits >= 10));
                buffer += int(head_digits >= 10);

                // Print remaining 4 digits.
                prod = (prod & UINT32_C(0xffffffff)) * 100;
                print_2_digits(int(prod >> 32), buffer + 2);
                prod = (prod & UINT32_C(0xffffffff)) * 100;
                print_2_digits(int(prod >> 32), buffer + 4);

                buffer += 6;
            } else if (first_block >= 100) {
                // 3 or 4 digits.
                // 42949673 = ceil(2^32 / 100)
                auto prod = first_block * UINT64_C(42949673);
                auto const head_digits = int(prod >> 32);

                builtin_memcpy(buffer, radix_100_head_table + head_digits * 2, 2);
                buffer[2] = radix_100_table[head_digits * 2 + 1];

                exponent += (2 + int(head_digits >= 10));
                buffer += int(head_digits >= 10);

                // Print remaining 2 digits.
                prod = (prod & UINT32_C(0xffffffff)) * 100;
                print_2_digits(int(prod >> 32), buffer + 2);

                buffer += 4;
            } else {
                // 1 or 2 digits.
                builtin_memcpy(buffer, radix_100_head_table + first_block * 2, 2);
                buffer[2] = radix_100_table[first_block * 2 + 1];

                exponent += int(first_block >= 10);
                buffer += (2 + int(first_block >= 10));
            }

            // Next, print the second block.
            // The second block is of 8 digits, but we may have trailing zeros.
            // 281474978 = ceil(2^48 / 100'0000) + 1
            auto prod = second_block * UINT64_C(281474978);
            prod >>= 16;
            prod += 1;
            print_2_digits(int(prod >> 32), buffer);

            // Remaining 6 digits are all zero?
            if ((prod & UINT32_C(0xffffffff)) <=
                std::uint32_t((std::uint64_t(1) << 32) / UINT64_C(1000000))) {
                buffer += (1 + int(buffer[1] > '0'));
            } else {
                // Obtain the next two digits.
                prod = (prod & UINT32_C(0xffffffff)) * 100;
                print_2_digits(int(prod >> 32), buffer + 2);

                // Remaining 4 digits are all zero?
                if ((prod & UINT32_C(0xffffffff)) <=
                    std::uint32_t((std::uint64_t(1) << 32) / 10000)) {
                    buffer += (3 + int(buffer[3] > '0'));
                } else {
                    // Obtain the next two digits.
                    prod = (prod & UINT32_C(0xffffffff)) * 100;
                    print_2_digits(int(prod >> 32), buffer + 4);

                    // Remaining 2 digits are all zero?
                    if ((prod & UINT32_C(0xffffffff)) <=
                        std::uint32_t((std::uint64_t(1) << 32) / 100)) {
                        buffer += (5 + int(buffer[5] > '0'));
                    } else {
                        // Obtain the last two digits.
                        prod = (prod & UINT32_C(0xffffffff)) * 100;
                        print_2_digits(int(prod >> 32), buffer + 6);
                        buffer += (7 + int(buffer[7] > '0'));
                    }
                }
            }
        }
    }

    // Print exponent and return
    if (exponent < 0) {
        builtin_memcpy(buffer, "E-", 2);
        buffer += 2;
        exponent = -exponent;
    } else {
        buffer[0] = 'E';
        buffer += 1;
    }

    if (exponent >= 100) {
        // d1 = exponent / 10; d2 = exponent % 10;
        // 6554 = ceil(2^16 / 10)
        auto prod = std::uint32_t(exponent) * UINT32_C(6554);
        auto d1 = int(prod >> 16);
        prod = (prod & UINT16_C(0xffff)) * 5; // * 10
        auto d2 = int(prod >> 15);            // >> 16
        print_2_digits(d1, buffer);
        print_1_digit(d2, buffer + 2);
        buffer += 3;
    } else if (exponent >= 10) {
        print_2_digits(exponent, buffer);
        buffer += 2;
    } else {
        print_1_digit(exponent, buffer);
        buffer += 1;
    }

    return buffer;
}
} // namespace detail
} // namespace dragonbox
} // namespace wjr