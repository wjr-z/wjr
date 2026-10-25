#include <array>
#include <iostream>

#include <wjr/np/histogram.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::dextents<size_t, 1>;
    const double values[] = {-1., 0., .25, .5, .75, 1., 2.};
    const double edges[] = {0., .5, 1.};
    std::array<unsigned, 2> counts;
    np::histogram_to(np::tensor_view<unsigned, shape>(counts.data(), counts.size()),
                     np::tensor_view<const double, shape>(values, 7),
                     np::tensor_view<const double, shape>(edges, 3));
    std::cout << "[0, 0.5): " << counts[0] << "\n[0.5, 1]: " << counts[1] << '\n';
    return counts == std::array{2u, 3u} ? 0 : 1;
}
