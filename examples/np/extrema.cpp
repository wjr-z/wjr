#include <array>
#include <iostream>

#include <wjr/np/extrema.hpp>
#include <wjr/np/slice.hpp>

int main() {
    namespace np = wjr::np;
    // The window borrows two columns from a padded matrix. Returned indices
    // refer to the window's logical row-major order, rather than storage offsets.
    const std::array<double, 12> values{99., 3., -2., 99., 99., -2., 7., 99., 99., 4., 7., 99.};
    const np::tensor_view<const double, np::extents<size_t, 3, 4>> matrix(values.data());
    const auto window = np::slice_view(matrix, np::full_extent, std::pair{1, 3});
    const auto low = np::min(window);
    const auto high = np::max(window);
    const auto low_index = np::argmin(window);
    const auto high_index = np::argmax(window);
    std::cout << "minimum: " << low << " at (" << low_index / 2 << ", " << low_index % 2
              << ")\nmaximum: " << high << " at (" << high_index / 2 << ", " << high_index % 2
              << ")\n";
    return low == -2. && high == 7. && low_index == 1 && high_index == 3 ? 0 : 1;
}
