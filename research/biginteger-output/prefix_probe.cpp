#include <array>
#include <cstdio>
#include <iterator>
#include <string>
#include <string_view>
#include <wjr/biginteger/detail/convert.hpp>

int main() {
    const std::array<uint64_t, 3> input{0, 0, 1};
    std::string output = "prefix:";
    output.reserve(7 + 64 * input.size());
    wjr::biginteger_to_chars(std::back_inserter(output), input.data(), input.size(), 32);
    const bool prefix_preserved = std::string_view(output).substr(0, 7) == "prefix:";
    std::printf("base32 n=3 limbs={0,0,1}: prefix_preserved=%s actual_prefix=%.*s\n",
                prefix_preserved ? "true" : "false", 7, output.data());
    return prefix_preserved ? 0 : 1;
}
