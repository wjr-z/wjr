#include "random.hpp"

#include <wjr/bitset.hpp>

#include <sstream>

using namespace wjr;

TEST_CASE("bitset - constructor") {
    // Default constructor
    bitset<32> bs1;
    CHECK_EQ(bs1.count(), 0);
    CHECK(bs1.none());

    // Construct from uint64_t
    bitset<32> bs2(0x12345678);
    CHECK_EQ(bs2.count(), 13);
    CHECK(bs2.test(3));
    CHECK(bs2.test(4));
    CHECK_FALSE(bs2.test(0));

    bitset<64> bs3(0xFFFFFFFFFFFFFFFFull);
    CHECK_EQ(bs3.count(), 64);
    CHECK(bs3.all());

    // Construct from string
    bitset<8> bs4("10101010");
    CHECK_EQ(bs4.count(), 4);
    CHECK(bs4.test(1));
    CHECK(bs4.test(3));
    CHECK(bs4.test(5));
    CHECK(bs4.test(7));
    CHECK_FALSE(bs4.test(0));

    // String with custom zero/one chars
    std::string_view sv("ABABABAB");
    bitset<8> bs5(sv, 0, 8, 'A', 'B');
    CHECK_EQ(bs5.count(), 4);
    CHECK(bs5.test(0));
    CHECK(bs5.test(2));
    CHECK(bs5.test(4));
    CHECK(bs5.test(6));
}

TEST_CASE("bitset - equality") {
    bitset<32> bs1(0x12345678);
    bitset<32> bs2(0x12345678);
    bitset<32> bs3(0x87654321);

    CHECK(bs1 == bs2);
    CHECK_FALSE(bs1 == bs3);
    CHECK_FALSE(bs1 != bs2);
    CHECK(bs1 != bs3);
}

TEST_CASE("bitset - element_access") {
    bitset<32> bs;

    // Test operator[]
    CHECK_FALSE(bs[0]);
    CHECK_FALSE(bs[15]);
    CHECK_FALSE(bs[31]);

    bs.set(5);
    CHECK(bs[5]);
    CHECK_FALSE(bs[4]);

    // Test reference assignment
    bs[10] = true;
    CHECK(bs[10]);
    bs[10] = false;
    CHECK_FALSE(bs[10]);
}

TEST_CASE("bitset - set_reset") {
    bitset<32> bs;

    // Set all bits
    bs.set();
    CHECK(bs.all());
    CHECK_EQ(bs.count(), 32);

    // Reset all bits
    bs.reset();
    CHECK(bs.none());
    CHECK_EQ(bs.count(), 0);

    // Set individual bits
    bs.set(5);
    CHECK(bs.test(5));
    CHECK_EQ(bs.count(), 1);

    bs.set(10, true);
    CHECK(bs.test(10));
    CHECK_EQ(bs.count(), 2);

    bs.set(10, false);
    CHECK_FALSE(bs.test(10));
    CHECK_EQ(bs.count(), 1);

    // Reset individual bits
    bs.reset(5);
    CHECK_FALSE(bs.test(5));
    CHECK_EQ(bs.count(), 0);
}

TEST_CASE("bitset - flip") {
    bitset<8> bs("10101010");

    // Flip all
    bs.flip();
    CHECK_EQ(bs.to_string(), "01010101");

    // Flip individual bit
    bs.flip(0);
    CHECK_FALSE(bs.test(0));
    bs.flip(0);
    CHECK(bs.test(0));
}

TEST_CASE("bitset - bitwise_operations") {
    bitset<8> bs1("10101010");
    bitset<8> bs2("11001100");

    // AND
    auto bs_and = bs1 & bs2;
    CHECK_EQ(bs_and.to_string(), "10001000");

    bs1 &= bs2;
    CHECK_EQ(bs1.to_string(), "10001000");

    // OR
    bitset<8> bs3("10101010");
    bitset<8> bs4("11001100");
    auto bs_or = bs3 | bs4;
    CHECK_EQ(bs_or.to_string(), "11101110");

    bs3 |= bs4;
    CHECK_EQ(bs3.to_string(), "11101110");

    // XOR
    bitset<8> bs5("10101010");
    bitset<8> bs6("11001100");
    auto bs_xor = bs5 ^ bs6;
    CHECK_EQ(bs_xor.to_string(), "01100110");

    bs5 ^= bs6;
    CHECK_EQ(bs5.to_string(), "01100110");

    // NOT
    bitset<8> bs7("10101010");
    auto bs_not = ~bs7;
    CHECK_EQ(bs_not.to_string(), "01010101");
}

TEST_CASE("bitset - shift_operations") {
    // Left shift
    bitset<8> bs1("00001111");
    auto bs_lshift = bs1 << 2;
    CHECK_EQ(bs_lshift.to_string(), "00111100");

    bs1 <<= 2;
    CHECK_EQ(bs1.to_string(), "00111100");

    // Right shift
    bitset<8> bs2("11110000");
    auto bs_rshift = bs2 >> 2;
    CHECK_EQ(bs_rshift.to_string(), "00111100");

    bs2 >>= 2;
    CHECK_EQ(bs2.to_string(), "00111100");

    // Edge cases
    bitset<8> bs3("10101010");
    bs3 <<= 0;
    CHECK_EQ(bs3.to_string(), "10101010");

    bs3 >>= 0;
    CHECK_EQ(bs3.to_string(), "10101010");
}

TEST_CASE("bitset - count_operations") {
    bitset<32> bs;

    // Empty bitset
    CHECK_EQ(bs.count(), 0);
    CHECK(bs.none());
    CHECK_FALSE(bs.any());
    CHECK_FALSE(bs.all());

    // Single bit
    bs.set(5);
    CHECK_EQ(bs.count(), 1);
    CHECK_FALSE(bs.none());
    CHECK(bs.any());
    CHECK_FALSE(bs.all());

    // All bits
    bs.set();
    CHECK_EQ(bs.count(), 32);
    CHECK_FALSE(bs.none());
    CHECK(bs.any());
    CHECK(bs.all());

    // Half bits
    bitset<8> bs2("10101010");
    CHECK_EQ(bs2.count(), 4);
    CHECK_FALSE(bs2.none());
    CHECK(bs2.any());
    CHECK_FALSE(bs2.all());
}

TEST_CASE("bitset - countr_one") {
    bitset<8> bs1("00000000");
    CHECK_EQ(bs1.countr_one(), 0);

    bitset<8> bs2("00000001");
    CHECK_EQ(bs2.countr_one(), 1);

    bitset<8> bs3("00000111");
    CHECK_EQ(bs3.countr_one(), 3);

    bitset<8> bs4("11111111");
    CHECK_EQ(bs4.countr_one(), 8);

    bitset<8> bs5("00001110");
    CHECK_EQ(bs5.countr_one(), 0);
}

TEST_CASE("bitset - to_string") {
    bitset<8> bs1("10101010");
    CHECK_EQ(bs1.to_string(), "10101010");

    bitset<8> bs2(0xFF);
    CHECK_EQ(bs2.to_string(), "11111111");

    bitset<8> bs3(0);
    CHECK_EQ(bs3.to_string(), "00000000");

    // Custom characters
    bitset<8> bs4("10101010");
    CHECK_EQ(bs4.to_string<char>('A', 'B'), "BABABABA");
}

TEST_CASE("bitset - stream_output") {
    bitset<8> bs("10101010");
    std::ostringstream oss;
    oss << bs;
    CHECK_EQ(oss.str(), "10101010");
}

TEST_CASE("bitset - different_sizes") {
    // Small bitset (< 32 bits)
    bitset<7> bs1("1010101");
    CHECK_EQ(bs1.count(), 4);
    CHECK_EQ(bs1.to_string(), "1010101");

    // 32-bit bitset
    bitset<32> bs2(0xFFFFFFFF);
    CHECK_EQ(bs2.count(), 32);
    CHECK(bs2.all());

    // 64-bit bitset
    bitset<64> bs3(0xFFFFFFFFFFFFFFFFull);
    CHECK_EQ(bs3.count(), 64);
    CHECK(bs3.all());

    // Large bitset (> 64 bits)
    bitset<100> bs4;
    bs4.set();
    CHECK_EQ(bs4.count(), 100);
    CHECK(bs4.all());

    // Very large bitset
    bitset<256> bs5;
    bs5.set(255);
    bs5.set(0);
    CHECK_EQ(bs5.count(), 2);
    CHECK(bs5.test(0));
    CHECK(bs5.test(255));
}

TEST_CASE("bitset - edge_cases") {
    // Single bit bitset
    bitset<1> bs1;
    CHECK_FALSE(bs1.test(0));
    bs1.set(0);
    CHECK(bs1.test(0));
    CHECK_EQ(bs1.count(), 1);

    // Non-power-of-2 sizes
    bitset<13> bs2;
    bs2.set();
    CHECK_EQ(bs2.count(), 13);
    CHECK(bs2.all());

    bitset<33> bs3;
    bs3.set();
    CHECK_EQ(bs3.count(), 33);
    CHECK(bs3.all());

    bitset<127> bs4;
    bs4.set();
    CHECK_EQ(bs4.count(), 127);
    CHECK(bs4.all());
}

TEST_CASE("bitset - reference_behavior") {
    bitset<8> bs;

    // Reference assignment
    auto ref = bs[5];
    ref = true;
    CHECK(bs.test(5));

    // Reference flip
    bs[3] = true;
    auto ref2 = bs[3];
    ref2.flip();
    CHECK_FALSE(bs.test(3));

    // Reference to bool conversion
    bs.set(7);
    bool val = bs[7];
    CHECK(val);

    // Reference complement
    bs.set(2);
    CHECK_FALSE(~bs[2]);
    CHECK(~bs[1]);
}

TEST_CASE("bitset - complex_operations") {
    bitset<16> bs1;
    bitset<16> bs2;

    // Set some pattern
    for (size_t i = 0; i < 16; i += 2) {
        bs1.set(i);
    }

    for (size_t i = 1; i < 16; i += 2) {
        bs2.set(i);
    }

    CHECK_EQ(bs1.count(), 8);
    CHECK_EQ(bs2.count(), 8);

    // They should be complementary
    auto combined = bs1 | bs2;
    CHECK(combined.all());

    auto intersection = bs1 & bs2;
    CHECK(intersection.none());
}

TEST_CASE("bitset - shift_large") {
    bitset<128> bs;
    bs.set(64);

    auto shifted = bs << 32;
    CHECK_FALSE(shifted.test(64));
    CHECK(shifted.test(96));

    shifted = bs >> 32;
    CHECK_FALSE(shifted.test(64));
    CHECK(shifted.test(32));
}

TEST_CASE("bitset - data_access") {
    bitset<32> bs(0x12345678);

    auto *data = bs.data();
    CHECK_NE(data, nullptr);
    CHECK_EQ(data[0], 0x12345678u);

    const auto &cbs = bs;
    const auto *cdata = cbs.data();
    CHECK_NE(cdata, nullptr);
    CHECK_EQ(cdata[0], 0x12345678u);
}