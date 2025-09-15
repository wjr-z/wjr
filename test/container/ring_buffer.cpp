#include "../detail.hpp"

#include <wjr/container/ring_buffer.hpp>

using namespace wjr;

TEST(ring_buffer, simple_test) {
    ring_buffer<int> rb;
    for (int i = 0; i < 8; ++i) {
        rb.emplace_back(i);
    }

    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(rb[i], i);
    }

    for (int i = 8; i < 12; ++i) {
        rb.pop_front();
        rb.emplace_back(i);
    }

    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(rb[i], i + 4);
    }

    EXPECT_EQ(rb.size(), 8);

    ring_buffer<int> rb2 = rb;
    EXPECT_EQ(rb2.size(), rb.size());
    EXPECT_TRUE(std::equal(rb.begin(), rb.end(), rb2.begin()));
}