#include <array>
#include <cstdint>

#include <wjr/simd/transform.hpp>
#include <wjr/simd/xsimd.hpp>

int main() {
    const std::array<std::uint64_t, 7> data = {1, 2, 3, 4, 5, 6, 42};
    namespace backend = wjr::simd::xsimd_backend;
    std::array<std::uint64_t, 7> output{};
    backend::transform_to(output.data(), data.data(), output.size(),
                          [](auto value) { return value + decltype(value)(1); });
    backend::transform_to(output.data(), output.data(), data.data(), output.size(),
                          [](auto lhs, auto rhs) { return lhs + rhs; });
    if (output[0] != 3 || output[6] != 85) {
        return 1;
    }
    return backend::find_equal(data.data(), std::uint64_t{42}, data.size()) != 6 ||
           backend::find_equal(data.data(), std::uint64_t{0}, data.size()) != data.size();
}
