#include "../detail.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory_resource>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <wjr/json/document.hpp>
#include <wjr/json/pmr.hpp>

namespace {
using namespace wjr::json;

template <typename Document>
void seed(Document &value, unsigned kind) {
    switch (kind) {
    case 0:
        value = true;
        break;
    case 1:
        value = std::numeric_limits<std::uint64_t>::max();
        break;
    case 2:
        value = std::numeric_limits<std::int64_t>::min();
        break;
    case 3:
        value = -7.25;
        break;
    case 4:
        value = std::string(256, 's');
        break;
    case 5:
        value.emplace_object();
        value["nested"] = std::vector<std::string>{std::string(128, 'o')};
        break;
    case 6:
        value = std::vector<std::string>{std::string(128, 'a'), std::string(128, 'b')};
        break;
    }
}

template <typename Document>
void verify_default(Document &value, unsigned kind) {
    using method = void (Document::*)() noexcept;
    static_assert(std::is_same_v<decltype(&Document::emplace_boolean), method>);
    static_assert(std::is_same_v<decltype(&Document::emplace_number_unsigned), method>);
    static_assert(std::is_same_v<decltype(&Document::emplace_number_signed), method>);
    static_assert(std::is_same_v<decltype(&Document::emplace_number_float), method>);
    switch (kind) {
    case 0:
        value.emplace_boolean();
        ASSERT_TRUE(value.is_boolean());
        EXPECT_FALSE(value.template get<boolean_t>());
        break;
    case 1:
        value.emplace_number_unsigned();
        ASSERT_TRUE(value.is_number_unsigned());
        EXPECT_EQ(value.template get<number_unsigned_t>(), 0);
        break;
    case 2:
        value.emplace_number_signed();
        ASSERT_TRUE(value.is_number_signed());
        EXPECT_EQ(value.template get<number_signed_t>(), 0);
        break;
    case 3:
        value.emplace_number_float();
        ASSERT_TRUE(value.is_number_float());
        EXPECT_EQ(value.template get<number_float_t>(), 0.0);
        break;
    }
}

class counting_resource final : public std::pmr::memory_resource {
public:
    std::size_t calls = 0;
    std::size_t live = 0;

private:
    void *do_allocate(std::size_t bytes, std::size_t alignment) override {
        auto *pointer = std::pmr::new_delete_resource()->allocate(bytes, alignment);
        ++calls;
        live += bytes;
        return pointer;
    }
    void do_deallocate(void *pointer, std::size_t bytes, std::size_t alignment) override {
        live -= bytes;
        std::pmr::new_delete_resource()->deallocate(pointer, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
};

template <typename Document>
void verify_typed_helpers() {
    static_assert(
        std::is_same_v<decltype(&Document::_emplace_number_unsigned),
                       void (Document::*)(typename Document::number_unsigned_type) noexcept>);
    static_assert(
        std::is_same_v<decltype(&Document::_emplace_number_signed),
                       void (Document::*)(typename Document::number_signed_type) noexcept>);
    static_assert(
        std::is_same_v<decltype(&Document::_emplace_number_float),
                       void (Document::*)(typename Document::number_float_type) noexcept>);
    Document value;
    value._emplace_number_unsigned(std::numeric_limits<std::uint64_t>::max());
    EXPECT_EQ(value.template get<number_unsigned_t>(), std::numeric_limits<std::uint64_t>::max());
    value._emplace_number_signed(std::numeric_limits<std::int64_t>::min());
    EXPECT_EQ(value.template get<number_signed_t>(), std::numeric_limits<std::int64_t>::min());
    value._emplace_number_float(-12.75);
    EXPECT_EQ(value.template get<number_float_t>(), -12.75);
    value._emplace_number_float(-0.0);
    EXPECT_TRUE(std::signbit(value.template get<number_float_t>()));
}
} // namespace

TEST(json, scalar_emplace_defaults_replace_every_kind) {
    static_assert(noexcept(std::declval<document &>().emplace_boolean()));
    static_assert(noexcept(std::declval<document &>().emplace_number_unsigned()));
    static_assert(noexcept(std::declval<document &>().emplace_number_signed()));
    static_assert(noexcept(std::declval<document &>().emplace_number_float()));
    for (unsigned original = 0; original != 8; ++original) {
        for (unsigned scalar = 0; scalar != 4; ++scalar) {
            document value;
            seed(value, original);
            verify_default(value, scalar);
        }
    }
}

TEST(json, scalar_emplace_releases_pmr_payload_and_retains_resource) {
    counting_resource resource;
    for (unsigned original = 0; original != 8; ++original) {
        for (unsigned scalar = 0; scalar != 4; ++scalar) {
            pmr_document value(std::allocator_arg, pmr_document::allocator_type(&resource));
            seed(value, original);
            const auto before = resource.calls;
            verify_default(value, scalar);
            EXPECT_EQ(resource.calls, before);
            EXPECT_EQ(resource.live, 0);
            EXPECT_EQ(value.get_allocator().resource(), &resource);
        }
    }
    EXPECT_EQ(resource.live, 0);
}

TEST(json, numeric_emplace_helpers_preserve_typed_values) {
    verify_typed_helpers<document>();
    verify_typed_helpers<pmr_document>();
}
