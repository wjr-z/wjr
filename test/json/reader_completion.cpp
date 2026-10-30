#include "../detail.hpp"

#include <string>

#include <wjr/json/document.hpp>

using namespace wjr;

TEST(json, reader_whitespace_only) {
    const size_t lengths[] = {0, 1, 2, 63, 64, 65, 4095, 4096, 8192};
    json::ondemand_reader rd;
    for (const size_t n : lengths) {
        SCOPED_TRACE(n);
        const auto input = std::string(n, ' ');
        rd.read(input);
        ASSERT_EQ(rd.get(), json::ondemand_reader::npos);
        EXPECT_EQ(rd.get(), json::ondemand_reader::npos);

        rd.read("null");
        EXPECT_EQ(rd.get(), 0);
        EXPECT_EQ(rd.get(), json::ondemand_reader::npos);

        rd.read(input);
        EXPECT_FALSE(json::document::parse(rd));
        rd.read(input);
        EXPECT_FALSE(json::check(rd));
    }
}

TEST(json, reader_whitespace_after_token_refill) {
    const auto input = std::string(4096, '[') + std::string(512, ' ');
    json::ondemand_reader rd;
    rd.read(input);
    for (uint32_t i = 0; i < 4096; ++i) {
        ASSERT_EQ(rd.get(), i);
    }
    EXPECT_EQ(rd.get(), json::ondemand_reader::npos);
    EXPECT_EQ(rd.get(), json::ondemand_reader::npos);
}

TEST(json, root_containers_reject_trailing_content) {
    const char *roots[] = {"[]", "{}", "[1,{}]", "{\"a\":[]}"};
    const char *suffixes[] = {"x", " []", " {}", " null", " true", " false", " 0", " \"x\""};
    for (const auto root : roots) {
        for (const auto suffix : suffixes) {
            const auto input = std::string(root) + suffix;
            SCOPED_TRACE(input);
            json::ondemand_reader rd;
            rd.read(input);
            const auto parsed = json::document::parse(rd);
            ASSERT_FALSE(parsed);
            EXPECT_EQ(parsed.error(), json::error_code::TRAILING_CONTENT);
            rd.read(input);
            const auto checked = json::check(rd);
            ASSERT_FALSE(checked);
            EXPECT_EQ(checked.error(), json::error_code::TRAILING_CONTENT);
        }
    }
}

TEST(json, root_containers_allow_trailing_whitespace) {
    const char *roots[] = {"[]", "{}", "[1,{}]", "{\"a\":[]}"};
    const size_t lengths[] = {0, 1, 63, 64, 65, 4096};
    for (const auto root : roots) {
        for (const size_t n : lengths) {
            SCOPED_TRACE(root);
            SCOPED_TRACE(n);
            const auto input = std::string(root) + std::string(n, ' ') + "\t\r\n";
            json::ondemand_reader rd;
            rd.read(input);
            EXPECT_TRUE(json::document::parse(rd));
            rd.read(input);
            EXPECT_TRUE(json::check(rd));
        }
    }
}
