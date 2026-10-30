#include <wjr/config.hpp>

#if defined(WJR_ARCH_X86)

    #include <array>
    #include <cstring>
    #include <memory>

    #include <gtest/gtest.h>

    #include <wjr/arch/x86/simd/avx.hpp>

namespace {

// Keep each fixed entry point independent from the runtime switch, and keep the
// length runtime-dependent even in optimized sanitizer builds.
template <auto Load>
WJR_NOINLINE auto fixed_load(const void *ptr) noexcept {
    return Load(ptr);
}

template <class Simd>
WJR_NOINLINE auto dynamic_load(const void *ptr, int count) noexcept {
    return Simd::loadu_si16x(ptr, count);
}

template <class Vector>
void expect_prefix(Vector value, const unsigned char *ptr, size_t bytes) {
    std::array<unsigned char, sizeof(Vector)> actual;
    // Only copy the specified prefix; AVX cast results may have unspecified
    // upper lanes. Do not make zero extension part of the tested contract.
    std::memcpy(actual.data(), &value, bytes);
    for (size_t i = 0; i < bytes; ++i) {
        EXPECT_EQ(actual[i], ptr[i]) << "byte " << i;
    }
}

template <class Simd, class Load, size_t Size>
void check_partial_loads(const std::array<Load, Size> &fixed) {
    constexpr int max_count = static_cast<int>(Size);
    for (int count = 1; count <= max_count; ++count) {
        const size_t bytes = static_cast<size_t>(count) * 2;
        for (size_t offset = 0; offset < 32; ++offset) {
            SCOPED_TRACE(::testing::Message() << "count=" << count << ", offset=" << offset);
            // The readable prefix ends exactly at the allocation boundary, so
            // ASan detects even a one-byte overread at every address offset.
            auto storage = std::make_unique<unsigned char[]>(bytes + offset);
            auto *ptr = storage.get() + offset;
            for (size_t i = 0; i < bytes; ++i) {
                ptr[i] = static_cast<unsigned char>(i * 37 + offset * 13 + count);
            }
            expect_prefix(fixed[count - 1](ptr), ptr, bytes);
            expect_prefix(dynamic_load<Simd>(ptr, count), ptr, bytes);
        }
    }

    // Zero length accepts a null pointer without reading it. Its zero result is
    // already guaranteed by the existing switch and is fully specified.
    const auto empty = dynamic_load<Simd>(nullptr, 0);
    std::array<unsigned char, sizeof(empty)> actual;
    std::memcpy(actual.data(), &empty, sizeof(empty));
    for (auto byte : actual) {
        EXPECT_EQ(byte, 0);
    }
}

} // namespace

    #if WJR_HAS_SIMD(SSE2)
TEST(simd_partial_load, sse) {
    using simd = wjr::sse;
    using load_type = __m128i (*)(const void *) noexcept;
    const std::array<load_type, 8> fixed = {
        fixed_load<simd::loadu_si16>,  fixed_load<simd::loadu_si32>,  fixed_load<simd::loadu_si48>,
        fixed_load<simd::loadu_si64>,  fixed_load<simd::loadu_si80>,  fixed_load<simd::loadu_si96>,
        fixed_load<simd::loadu_si112>, fixed_load<simd::loadu_si128>,
    };
    check_partial_loads<simd>(fixed);
}
    #endif

    #if WJR_HAS_SIMD(AVX)
TEST(simd_partial_load, avx) {
    using simd = wjr::avx;
    using load_type = __m256i (*)(const void *) noexcept;
    const std::array<load_type, 16> fixed = {
        fixed_load<simd::loadu_si16>,  fixed_load<simd::loadu_si32>,  fixed_load<simd::loadu_si48>,
        fixed_load<simd::loadu_si64>,  fixed_load<simd::loadu_si80>,  fixed_load<simd::loadu_si96>,
        fixed_load<simd::loadu_si112>, fixed_load<simd::loadu_si128>, fixed_load<simd::loadu_si144>,
        fixed_load<simd::loadu_si160>, fixed_load<simd::loadu_si176>, fixed_load<simd::loadu_si192>,
        fixed_load<simd::loadu_si208>, fixed_load<simd::loadu_si224>, fixed_load<simd::loadu_si240>,
        fixed_load<simd::loadu_si256>,
    };
    check_partial_loads<simd>(fixed);
}
    #endif

#endif // WJR_ARCH_X86
