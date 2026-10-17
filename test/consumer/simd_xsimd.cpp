#include <array>
#include <cstdint>

#include <wjr/simd/xsimd.hpp>

int main() {
    const std::array<std::uint64_t, 7> data = {1, 2, 3, 4, 5, 6, 42};
    namespace backend = wjr::simd::xsimd_backend;
    return backend::find_equal(data.data(), std::uint64_t{42}, data.size()) != 6 ||
           backend::find_equal(data.data(), std::uint64_t{0}, data.size()) != data.size();
}
