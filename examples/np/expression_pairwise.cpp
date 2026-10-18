#include <array>
#include <iostream>

#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_pairwise.hpp>
#include <wjr/np/expression_reduce.hpp>

int main() {
    namespace np = wjr::np;
    std::array<float, 4096> samples{};
    samples.fill(1.f);
    samples.front() = 16777216.f;
    samples.back() = -16777216.f;
    const auto x =
        np::as_expr(np::tensor_view<const float, np::extents<size_t, 4096>>(samples.data()));

    // No squared-values tensor: evaluate each point in float, then accumulate.
    const auto energy = np::pairwise_sum<double>(np::square(x));
    const auto ordered = np::sum(x);
    const auto paired = np::pairwise_sum(x);
    const auto wider = np::pairwise_sum<double>(x);
    const auto scaled =
        np::map([](float value, float scale) noexcept { return value * scale; }, x, 0.5f);
    const auto scaled_sum = np::pairwise_sum<double>(scaled);
    std::cout << "ordered=" << ordered << " pairwise=" << paired << " wider=" << wider
              << " squared energy=" << energy << " scaled sum=" << scaled_sum << '\n';
    // This data demonstrates grouping and accumulator precision as separate choices.
    return wider == 4094. && scaled_sum == 2047. ? 0 : 1;
}
