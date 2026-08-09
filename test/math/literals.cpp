#include "../detail.hpp"

#include <wjr/math/literals.hpp>

using namespace wjr;

TEST(literals, binary_size_units) {
    EXPECT_EQ(1_KB, 1024ull);
    EXPECT_EQ(2_KB, 2048ull);
    EXPECT_EQ(1_MB, 1024ull * 1024);
    EXPECT_EQ(3_MB, 3ull * 1024 * 1024);
    EXPECT_EQ(1_GB, 1024ull * 1024 * 1024);
    EXPECT_EQ(2_GB, 2ull * 1024 * 1024 * 1024);

    static_assert(4_KB == 4096ull);
    static_assert(5_MB == 5ull * 1024 * 1024);
    static_assert(6_GB == 6ull * 1024 * 1024 * 1024);
}
