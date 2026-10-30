#include <wjr/config.hpp>

#if defined(WJR_ARCH_X86) && WJR_HAS_SIMD(SSE2)

    #include <array>
    #include <charconv>
    #include <iostream>
    #include <memory>
    #include <string_view>

    #include <wjr/arch/x86/simd/avx.hpp>
    #include <wjr/json/lexer.hpp>

int main(int argc, char **argv) {
    #if WJR_HAS_SIMD(AVX)
    using simd = wjr::avx;
    #else
    using simd = wjr::sse;
    #endif
    constexpr unsigned int width = simd::width() / 8;
    unsigned int count = 7;
    if (argc > 1) {
        const std::string_view argument = argv[1];
        const auto parsed =
            std::from_chars(argument.data(), argument.data() + argument.size(), count);
        if (parsed.ec != std::errc{} || parsed.ptr != argument.data() + argument.size() ||
            count > width) {
            std::cerr << "Expected a byte count from 0 to " << width << '\n';
            return 1;
        }
    }

    std::array<char, width> block{};
    for (unsigned int i = 0; i < width; ++i) {
        block[i] = static_cast<char>('A' + i % 26);
    }
    const auto value = simd::loadu(block.data());
    std::unique_ptr<char[]> output;
    if (count != 0) {
        output = std::make_unique<char[]>(count);
    }
    simd::storeu_bytes(output.get(), value, count);
    if (count != 0) {
        std::cout.write(output.get(), count);
    }
    std::cout << '\n';

    std::array<char, 7> fixed{};
    simd::storeu_bytes<7>(fixed.data(), value);
    std::cout.write(fixed.data(), fixed.size());
    std::cout << '\n';

    const std::string_view json = R"( { "message" : "bounded output", "n" : 7 } )";
    auto compact = std::make_unique<char[]>(json.size());
    const auto end = wjr::json::minify(compact.get(), json.data(), json.data() + json.size());
    std::cout.write(compact.get(), end - compact.get());
    std::cout << '\n';
}

#else
    #error "This example requires an x86 target with SSE2."
#endif
