#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>

#include <wjr/np/pairwise.hpp>

int main() {
    namespace np = wjr::np;
    std::array<float, 4096> contributions;
    contributions.fill(1.f);
    contributions.front() = 16777216.f;
    contributions.back() = -16777216.f;
    const np::tensor_view<const float, np::extents<size_t, 4096>> source(contributions.data());
    const auto ordered = np::sum(source);
    const auto pairwise = np::pairwise_sum(source);
    const auto wide = np::pairwise_sum<double>(source);
    constexpr double exact = 4094.; // Every input is an exactly representable integer.
    std::cout << std::fixed << std::setprecision(0) << "ordered float: " << ordered
              << "\npairwise float: " << pairwise << "\npairwise double: " << wide
              << "\nexact sum: " << exact << '\n';
    // Pairwise improves this cancellation case; it is not correctly rounded
    // summation and does not improve every possible input.
    return std::abs(double(pairwise) - exact) < std::abs(double(ordered) - exact) && wide == exact
               ? 0
               : 1;
}
