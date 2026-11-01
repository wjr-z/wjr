#include <gtest/gtest.h>

#include <string>

TEST(WjrGTestProvider, Link) {
    EXPECT_EQ(std::string("wjr"), (testing::Message() << "wjr").GetString());
}
