#include <array>
#include <iostream>

#include <wjr/np/take.hpp>

int main() {
    namespace np = wjr::np;
    const double values[] = {1.5, 2.5, 3.5, 4.5};
    const unsigned indices[] = {3, 0, 3, 1};
    std::array<double, 4> output;
    np::take_to(np::tensor_view<double, np::extents<size_t, 2, 2>>(output.data()),
                np::tensor_view<const double, np::extents<size_t, 4>>(values),
                np::tensor_view<const unsigned, np::extents<size_t, 2, 2>>(indices));
    for (const auto value : output)
        std::cout << value << ' ';
    std::cout << '\n';
    return output == std::array{4.5, 1.5, 4.5, 2.5} ? 0 : 1;
}
