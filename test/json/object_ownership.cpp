#include <gtest/gtest.h>

#include <cstddef>
#include <limits>
#include <map>
#include <memory>
#include <memory_resource>
#include <new>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <wjr/json/document.hpp>
#include <wjr/json/pmr.hpp>
#include <wjr/vector.hpp>

namespace {
using namespace wjr::json;

class counted_resource final : public std::pmr::memory_resource {
public:
    std::size_t calls = 0;
    std::size_t live = 0;
    std::size_t remaining = std::numeric_limits<std::size_t>::max();

private:
    void *do_allocate(std::size_t bytes, std::size_t alignment) override {
        if (remaining == 0)
            throw std::bad_alloc();
        if (remaining != std::numeric_limits<std::size_t>::max())
            --remaining;
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

class default_resource_guard {
public:
    explicit default_resource_guard(std::pmr::memory_resource *resource)
        : old(std::pmr::set_default_resource(resource)) {}
    ~default_resource_guard() { std::pmr::set_default_resource(old); }
    default_resource_guard(const default_resource_guard &) = delete;
    default_resource_guard &operator=(const default_resource_guard &) = delete;

private:
    std::pmr::memory_resource *old;
};

template <typename Document>
void fill(Document &value) {
    value.emplace_object();
    typename Document::string_type key(96, 'k');
    value[std::move(key)] = std::string(128, 'v');
    value["counter"] = 17;
}

struct tagged_less {
    unsigned tag = 0;
    bool operator()(std::string_view left, std::string_view right) const noexcept {
        return left < right;
    }
};

template <typename Key, typename Value, typename>
using tagged_object = std::map<Key, Value, tagged_less>;

using tagged_document =
    basic_document<detail::basic_document_traits<std::basic_string, tagged_object, wjr::vector>>;
} // namespace

TEST(json, exact_object_copy_assignment_to_its_own_storage) {
    document value;
    fill(value);
    const auto expected = value.dump();
    auto &object = value.get<object_t>();
    const auto *node = std::addressof(*object.begin());
    value.get_to(object);
    EXPECT_EQ(value.dump(), expected);
    ASSERT_FALSE(object.empty());
    EXPECT_EQ(std::addressof(*object.begin()), node);
}

TEST(json, exact_object_move_construction_transfers_existing_nodes) {
    document source;
    fill(source);
    const auto expected = source.dump();
    const auto *node = std::addressof(*source.get<object_t>().begin());
    auto object = static_cast<document::object_type>(std::move(source));
    ASSERT_EQ(object.size(), 2);
    EXPECT_EQ(std::addressof(*object.begin()), node);
    source.reset();
    document destination(std::move(object));
    EXPECT_EQ(destination.dump(), expected);
    EXPECT_EQ(std::addressof(*destination.get<object_t>().begin()), node);
}

TEST(json, exact_object_move_assignment_transfers_existing_nodes) {
    document source;
    fill(source);
    const auto expected = source.dump();
    const auto *node = std::addressof(*source.get<object_t>().begin());
    document::object_type object;
    object.emplace("old", document(false));
    std::move(source).get_to(object);
    ASSERT_EQ(object.size(), 2);
    EXPECT_EQ(std::addressof(*object.begin()), node);
    source.reset();
    document destination(std::string(256, 'x'));
    destination = std::move(object);
    EXPECT_EQ(destination.dump(), expected);
    EXPECT_EQ(std::addressof(*destination.get<object_t>().begin()), node);
}

TEST(json, pmr_object_extraction_preserves_resource_and_ownership) {
    counted_resource resource, other;
    default_resource_guard guard(&other);
    const auto direct_move_calls = [&] {
        pmr_document reference(std::allocator_arg, pmr_document::allocator_type(&resource));
        fill(reference);
        const auto before = resource.calls;
        pmr_document::object_type moved(std::move(reference.get<object_t>()));
        return resource.calls - before;
    }();
    {
        pmr_document source(std::allocator_arg, pmr_document::allocator_type(&resource));
        fill(source);
        const auto *node = std::addressof(*source.get<object_t>().begin());
        const auto before = resource.calls;
        const auto other_before = other.calls;
        auto object = static_cast<pmr_document::object_type>(std::move(source));
        EXPECT_EQ(object.get_allocator().resource(), &resource);
        EXPECT_EQ(resource.calls - before, direct_move_calls);
        EXPECT_EQ(other.calls, other_before);
        EXPECT_EQ(std::addressof(*object.begin()), node);
        source.reset();
        EXPECT_EQ(object.at("counter").get<number_signed_t>(), 17);
    }
    EXPECT_EQ(resource.live, 0);
    EXPECT_EQ(other.live, 0);
}

TEST(json, exact_object_copy_construction_and_assignment_are_independent) {
    document source;
    fill(source);
    const auto expected = source.dump();
    const auto *source_node = std::addressof(*source.get<object_t>().begin());
    const auto &input = source;
    auto object = static_cast<document::object_type>(input);
    EXPECT_NE(std::addressof(*object.begin()), source_node);
    document copied(object);
    EXPECT_EQ(copied.dump(), expected);
    EXPECT_NE(std::addressof(*copied.get<object_t>().begin()), std::addressof(*object.begin()));
    document assigned(true);
    assigned = object;
    EXPECT_EQ(assigned.dump(), expected);
    object.clear();
    input.get_to(object);
    EXPECT_EQ(object.size(), 2);
    source.reset();
    EXPECT_EQ(copied.dump(), expected);
    EXPECT_EQ(assigned.dump(), expected);
}

TEST(json, pmr_object_assignment_keeps_destination_resource) {
    counted_resource source_resource, destination_resource;
    {
        pmr_document source(std::allocator_arg, pmr_document::allocator_type(&source_resource));
        fill(source);
        const auto *node = std::addressof(*source.get<object_t>().begin());
        using object_allocator = pmr_document::object_type::allocator_type;
        pmr_document::object_type equal{object_allocator(&source_resource)};
        std::move(source).get_to(equal);
        EXPECT_EQ(std::addressof(*equal.begin()), node);
        EXPECT_EQ(equal.get_allocator().resource(), &source_resource);

        fill(source);
        pmr_document::object_type unequal{object_allocator(&destination_resource)};
        std::move(source).get_to(unequal);
        EXPECT_EQ(unequal.get_allocator().resource(), &destination_resource);
        EXPECT_EQ(unequal.at("counter").get<number_signed_t>(), 17);
        for (const auto &[key, value] : unequal) {
            EXPECT_EQ(key.get_allocator().resource(), &destination_resource);
            EXPECT_EQ(value.get_allocator().resource(), &destination_resource);
        }

        pmr_document destination(std::allocator_arg,
                                 pmr_document::allocator_type(&destination_resource));
        destination = equal;
        EXPECT_EQ(destination.get_allocator().resource(), &destination_resource);
        EXPECT_EQ(destination.at("counter").get<number_signed_t>(), 17);
        destination = std::move(equal);
        EXPECT_EQ(destination.get_allocator().resource(), &destination_resource);
        EXPECT_EQ(destination.at("counter").get<number_signed_t>(), 17);
        for (const auto &[key, value] : destination.get<object_t>()) {
            EXPECT_EQ(key.get_allocator().resource(), &destination_resource);
            EXPECT_EQ(value.get_allocator().resource(), &destination_resource);
        }
    }
    EXPECT_EQ(source_resource.live, 0);
    EXPECT_EQ(destination_resource.live, 0);
}

TEST(json, pmr_object_copy_construction_selects_default_resource) {
    counted_resource resource, selected;
    default_resource_guard guard(&selected);
    {
        pmr_document source(std::allocator_arg, pmr_document::allocator_type(&resource));
        fill(source);
        const auto &input = source;
        auto copied = static_cast<pmr_document::object_type>(input);
        EXPECT_EQ(copied.get_allocator().resource(), &selected);
        EXPECT_NE(std::addressof(*copied.begin()), std::addressof(*source.get<object_t>().begin()));
        for (const auto &[key, value] : copied) {
            EXPECT_EQ(key.get_allocator().resource(), &selected);
            EXPECT_EQ(value.get_allocator().resource(), &selected);
        }
    }
    EXPECT_EQ(resource.live, 0);
    EXPECT_EQ(selected.live, 0);
}

TEST(json, nonexact_unordered_object_roundtrip) {
    using object_type = std::unordered_map<std::string, int>;
    const object_type input{{"one", 1}, {"two", 2}};
    document source(input);
    EXPECT_TRUE(source.is_object());
    EXPECT_EQ(source.dump(), R"({"one":1,"two":2})");
    auto copied = static_cast<object_type>(source);
    EXPECT_EQ(copied, input);
    object_type assigned{{"old", 0}};
    source.get_to(assigned);
    EXPECT_EQ(assigned, input);
    auto moved = static_cast<object_type>(std::move(source));
    EXPECT_EQ(moved, input);
    document destination(std::move(moved));
    EXPECT_EQ(destination.dump(), R"({"one":1,"two":2})");
}

TEST(json, nonexact_map_and_multimap_keep_object_conversion) {
    const std::map<std::string, int> expected{{"one", 1}, {"two", 2}};
    document source(expected);
    EXPECT_EQ(static_cast<decltype(expected)>(source), expected);
    std::multimap<std::string, int> multiple;
    source.get_to(multiple);
    EXPECT_EQ(multiple.size(), 2);
    document destination(std::move(multiple));
    EXPECT_EQ(destination.dump(), source.dump());
}

TEST(json, exact_object_keeps_stateful_comparator) {
    tagged_document::object_type object(tagged_less{73});
    object.emplace("number", tagged_document(17));
    tagged_document source(std::move(object));
    EXPECT_EQ(source.get<object_t>().key_comp().tag, 73);
    const auto &input = source;
    auto copy = static_cast<tagged_document::object_type>(input);
    EXPECT_EQ(copy.key_comp().tag, 73);
    tagged_document::object_type assigned(tagged_less{11});
    input.get_to(assigned);
    EXPECT_EQ(assigned.key_comp().tag, 73);
    auto moved = static_cast<tagged_document::object_type>(std::move(source));
    EXPECT_EQ(moved.key_comp().tag, 73);
    tagged_document destination(false);
    destination = std::move(moved);
    EXPECT_EQ(destination.get<object_t>().key_comp().tag, 73);
}

TEST(json, pmr_object_copy_failure_releases_partial_nodes) {
    counted_resource resource, selected;
    default_resource_guard guard(&selected);
    {
        pmr_document source(std::allocator_arg, pmr_document::allocator_type(&resource));
        fill(source);
        const auto expected = source.dump();
        const auto &input = source;
        const auto before = selected.calls;
        {
            auto copy = static_cast<pmr_document::object_type>(input);
            EXPECT_EQ(copy.size(), 2);
        }
        const auto allocations = selected.calls - before;
        ASSERT_GT(allocations, 0);
        for (std::size_t failure = 0; failure != allocations; ++failure) {
            selected.remaining = failure;
            EXPECT_THROW((void)static_cast<pmr_document::object_type>(input), std::bad_alloc);
            selected.remaining = std::numeric_limits<std::size_t>::max();
            EXPECT_EQ(selected.live, 0);
            EXPECT_EQ(source.dump(), expected);
        }
    }
    EXPECT_EQ(resource.live, 0);
    EXPECT_EQ(selected.live, 0);
}
