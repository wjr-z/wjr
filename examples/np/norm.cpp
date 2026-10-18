#include <array>
#include <cmath>
#include <iostream>

#include <wjr/np/norm.hpp>
#include <wjr/np/subview.hpp>

int main() {
    namespace np = wjr::np;
    const std::array<double, 6> samples{3e200, 3e-200, 0, 4e-200, 4e200, 0};
    const np::tensor_view<const double, np::extents<size_t, 3, 2>> panel(samples.data());
    const auto large = np::norm_l2(*np::try_column_view(panel, 0));
    const auto small = np::norm_l2(*np::try_column_view(panel, 1));
    std::cout << "column norms: " << large << ' ' << small << '\n';
    // Neither column is copied; naive squaring would overflow or underflow.
    return std::abs(large / 5e200 - 1) < 1e-14 && std::abs(small / 5e-200 - 1) < 1e-14 ? 0 : 1;
}
