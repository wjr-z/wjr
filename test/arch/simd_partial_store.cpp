#include <wjr/config.hpp>

#if defined(WJR_ARCH_X86)

    #include <algorithm>
    #include <array>
    #include <memory>
    #include <utility>

    #include <gtest/gtest.h>

    #include <wjr/arch/x86/simd/avx.hpp>

namespace {
template <class Simd>
WJR_NOINLINE void dynamic_store(void *dst, typename Simd::int_type value,
                                unsigned int count) noexcept {
    Simd::storeu_bytes(dst, value, count);
}

template <class Simd, unsigned int Count>
WJR_NOINLINE void fixed_store(void *dst, typename Simd::int_type value) noexcept {
    Simd::template storeu_bytes<Count>(dst, value);
}

template <class Simd, size_t... Count>
auto fixed_stores(std::index_sequence<Count...>) {
    using store_type = void (*)(void *, typename Simd::int_type) noexcept;
    return std::array<store_type, sizeof...(Count)>{fixed_store<Simd, Count>...};
}

template <class Simd>
void check_partial_stores() {
    constexpr unsigned int width = Simd::width() / 8;
    const auto fixed = fixed_stores<Simd>(std::make_index_sequence<width + 1>{});
    std::array<unsigned char, width> source{};
    for (unsigned int i = 0; i < width; ++i) {
        source[i] = static_cast<unsigned char>(i * 37 + 11);
    }
    const auto value = Simd::loadu(source.data());

    dynamic_store<Simd>(nullptr, value, 0);
    fixed[0](nullptr, value);

    for (unsigned int count = 0; count <= width; ++count) {
        for (size_t offset = 0; offset < 32; ++offset) {
            SCOPED_TRACE(::testing::Message() << "count=" << count << ", offset=" << offset);
            std::array<unsigned char, width + 64> guarded{};
            const auto check_guarded = [&] {
                for (size_t i = 0; i < guarded.size(); ++i) {
                    const auto expected =
                        i >= offset && i < offset + count ? source[i - offset] : 0xcc;
                    EXPECT_EQ(guarded[i], expected) << "byte " << i;
                }
            };
            guarded.fill(0xcc);
            dynamic_store<Simd>(guarded.data() + offset, value, count);
            check_guarded();
            guarded.fill(0xcc);
            fixed[count](guarded.data() + offset, value);
            check_guarded();

            if (offset + count <= width) {
                auto overlapping = value;
                auto *bytes = reinterpret_cast<unsigned char *>(std::addressof(overlapping));
                const auto check_overlapping = [&] {
                    for (size_t i = 0; i < width; ++i) {
                        const auto expected =
                            i >= offset && i < offset + count ? source[i - offset] : source[i];
                        EXPECT_EQ(bytes[i], expected) << "byte " << i;
                    }
                };
                dynamic_store<Simd>(bytes + offset, overlapping, count);
                check_overlapping();
                overlapping = value;
                fixed[count](bytes + offset, overlapping);
                check_overlapping();
            }

            if (count != 0) {
                /// End the writable range at the heap boundary for sanitizer coverage.
                auto exact = std::make_unique<unsigned char[]>(offset + count);
                std::fill_n(exact.get(), offset + count, 0xcc);
                dynamic_store<Simd>(exact.get() + offset, value, count);
                EXPECT_TRUE(
                    std::equal(source.begin(), source.begin() + count, exact.get() + offset));
                std::fill_n(exact.get(), offset + count, 0xcc);
                fixed[count](exact.get() + offset, value);
                EXPECT_TRUE(
                    std::equal(source.begin(), source.begin() + count, exact.get() + offset));
            }
        }
    }
}
} // namespace

    #if WJR_HAS_SIMD(SSE2)
TEST(simd_partial_store, sse) { check_partial_stores<wjr::sse>(); }
    #endif

    #if WJR_HAS_SIMD(AVX)
TEST(simd_partial_store, avx) { check_partial_stores<wjr::avx>(); }
    #endif

#endif // WJR_ARCH_X86
