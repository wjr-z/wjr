#include <array>

#include <wjr/parallel/tbb.hpp>

int main() {
    std::array<unsigned, 257> output{};
    wjr::tbb::parallel_for_chunks(0, output.size(), 13, [&](size_t first, size_t last) {
        for (auto i = first; i != last; ++i)
            output[i] = static_cast<unsigned>(i * 2);
    });
    for (size_t i = 0; i != output.size(); ++i)
        if (output[i] != i * 2)
            return 1;
    return 0;
}
