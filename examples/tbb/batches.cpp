#include <array>
#include <iostream>

#include <oneapi/tbb/task_arena.h>

#include <wjr/parallel/tbb.hpp>

int main() {
    constexpr size_t rows = 31, columns = 64;
    std::array<double, rows * columns> input;
    std::array<double, rows> norms;
    for (size_t i = 0; i != input.size(); ++i)
        input[i] = static_cast<double>(i % 7);

    oneapi::tbb::task_arena arena(2); // Reused; the adapter never creates an arena.
    for (unsigned repeat = 0; repeat != 2; ++repeat)
        arena.execute([&] {
            wjr::tbb::parallel_for_chunks(0, rows, 4, [&](size_t first, size_t last) {
                for (auto row = first; row != last; ++row) {
                    double total = 0;
                    for (size_t col = 0; col != columns; ++col) {
                        const auto value = input[row * columns + col];
                        total += value * value;
                    }
                    norms[row] = total;
                }
            });
        });

    for (size_t row = 0; row != rows; ++row) {
        double expected = 0;
        for (size_t col = 0; col != columns; ++col) {
            const auto value = input[row * columns + col];
            expected += value * value;
        }
        if (norms[row] != expected)
            return 1;
    }
    std::cout << "row squared norms: " << norms[0] << " ... " << norms.back() << '\n';
}
