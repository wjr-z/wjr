#include "../detail.hpp"

#include <algorithm>
#include <memory>
#include <string>

#include <wjr/json/lexer.hpp>

using namespace wjr;

TEST(json, minify_returns_output_end) {
    const std::pair<std::string, std::string> cases[] = {
        {"", ""},
        {" \t\r\n ", ""},
        {"  null \n", "null"},
        {" [ 1, 2, 3 ] ", "[1,2,3]"},
        {R"( { "text" : "a b\" c", "path" : "x\\ y" } )", R"({"text":"a b\" c","path":"x\\ y"})"},
    };

    const size_t padding_lengths[] = {0, 1, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128};
    for (const auto &[content, expected] : cases) {
        for (const size_t padding : padding_lengths) {
            SCOPED_TRACE(content);
            SCOPED_TRACE(padding);
            const auto input = std::string(padding, ' ') + content + std::string(padding, ' ');
            auto output = std::make_unique<char[]>(std::max(input.size(), size_t{1}));

            const auto end = json::minify(output.get(), input.data(), input.data() + input.size());
            EXPECT_EQ(end, output.get() + expected.size());
            EXPECT_EQ(std::string(output.get(), expected.size()), expected);

            auto inplace = input;
            const auto inplace_end =
                json::minify(inplace.data(), inplace.data(), inplace.data() + inplace.size());
            EXPECT_EQ(inplace_end, inplace.data() + expected.size());
            EXPECT_EQ(inplace.substr(0, expected.size()), expected);
        }
    }
}

TEST(json, minify_exact_block_lengths) {
    const size_t lengths[] = {2, 3, 4, 7, 8, 15, 16, 17, 31, 32, 33, 63, 64, 128, 192};
    for (const size_t n : lengths) {
        SCOPED_TRACE(n);
        const auto input = '"' + std::string(n - 2, ' ') + '"';
        auto output = std::make_unique<char[]>(n);
        const auto end = json::minify(output.get(), input.data(), input.data() + n);
        EXPECT_EQ(end, output.get() + n);
        EXPECT_EQ(std::string(output.get(), n), input);

        auto inplace = input;
        EXPECT_EQ(json::minify(inplace.data(), inplace.data(), inplace.data() + n),
                  inplace.data() + n);
        EXPECT_EQ(inplace, input);
    }
}

TEST(json, minify_unclosed_string_stays_bounded) {
    const size_t lengths[] = {1, 2, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128};
    for (const size_t n : lengths) {
        SCOPED_TRACE(n);
        const auto input = '"' + std::string(n - 1, ' ');
        auto output = std::make_unique<char[]>(n);
        EXPECT_EQ(json::minify(output.get(), input.data(), input.data() + n), output.get() + n);
        EXPECT_EQ(std::string(output.get(), n), input);
    }
}

TEST(json, minify_preserved_blocks_after_compaction) {
    const auto text = std::string(193, ' ') + R"(\"\\)" + std::string(129, 'x');
    const auto expected = R"({"text":")" + text + R"(","items":[1,2,3]})";

    for (size_t padding = 0; padding < 128; ++padding) {
        SCOPED_TRACE(padding);
        const auto content = std::string(padding, ' ') + R"( { "text" : ")" + text +
                             R"(" , "items" : [ 1 , 2 , 3 ] } )";
        auto input = std::make_unique<char[]>(content.size());
        auto output = std::make_unique<char[]>(content.size());
        std::copy(content.begin(), content.end(), input.get());

        const auto end = json::minify(output.get(), input.get(), input.get() + content.size());
        EXPECT_EQ(end, output.get() + expected.size());
        EXPECT_EQ(std::string(output.get(), expected.size()), expected);

        const auto inplace_end =
            json::minify(input.get(), input.get(), input.get() + content.size());
        EXPECT_EQ(inplace_end, input.get() + expected.size());
        EXPECT_EQ(std::string(input.get(), expected.size()), expected);
    }
}
