#include <array>
#include <iostream>

#include <wjr/np/put.hpp>

int main() {
    namespace np = wjr::np;
    // A caller-owned dense buffer receives sparse updates in their given order.
    std::array<double, 5> output{10, 20, 30, 40, 50};
    const unsigned indices[] = {3, 1, 3};
    const double updates[] = {4.5, 2.5, 7.5};
    np::put_to(np::tensor_view<double, np::extents<size_t, 5>>(output.data()),
               np::tensor_view<const unsigned, np::extents<size_t, 3>>(indices),
               np::tensor_view<const double, np::extents<size_t, 3>>(updates));
    for (const auto value : output)
        std::cout << value << ' ';
    std::cout << '\n';
    return output == std::array{10.0, 2.5, 30.0, 7.5, 50.0} ? 0 : 1;
}
