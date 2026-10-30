#include "../detail.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <memory_resource>
#include <new>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <wjr/json/pmr.hpp>

namespace {
using pmr_value = wjr::json::pmr_document;
using allocator = pmr_value::allocator_type;
using namespace wjr::json;

static_assert(sizeof(wjr::json::document) == 16);
static_assert(std::uses_allocator_v<pmr_value, allocator>);
static_assert(!std::uses_allocator_v<wjr::json::document, allocator>);
static_assert(std::is_nothrow_move_constructible_v<pmr_value>);
static_assert(!std::is_nothrow_move_assignable_v<pmr_value>);
static_assert(std::is_nothrow_move_assignable_v<wjr::json::document>);
static_assert(std::is_nothrow_swappable_v<pmr_value>);
static_assert(!std::is_constructible_v<pmr_value, string_t, pmr_value::string_type *>);
static_assert(
    std::is_constructible_v<wjr::json::document, string_t, wjr::json::document::string_type *>);
static_assert(!std::is_constructible_v<std::string_view, pmr_value &&>);
static_assert(wjr::get_relocate_mode<pmr_value>::value == wjr::relocate_t::trivial);

struct allocation_record {
    size_t bytes;
    size_t alignment;
};

struct allocation_ledger {
    std::map<void *, allocation_record> blocks;
    size_t calls = 0;
    size_t live_bytes = 0;
};

class tracking_resource : public std::pmr::memory_resource {
public:
    explicit tracking_resource(
        std::pmr::memory_resource *upstream = std::pmr::new_delete_resource())
        : upstream(upstream), ledger(&owned_ledger) {}
    explicit tracking_resource(allocation_ledger &shared)
        : upstream(std::pmr::new_delete_resource()), ledger(&shared) {}

    size_t remaining = std::numeric_limits<size_t>::max();
    size_t live_bytes() const { return ledger->live_bytes; }
    size_t calls() const { return ledger->calls; }

    bool owns(const void *pointer) const {
        const auto address = reinterpret_cast<uintptr_t>(pointer);
        for (const auto &[base, record] : ledger->blocks) {
            const auto first = reinterpret_cast<uintptr_t>(base);
            if (address >= first && address - first < record.bytes) {
                return true;
            }
        }
        return false;
    }

private:
    void *do_allocate(size_t bytes, size_t alignment) override {
        if (remaining == 0) {
            WJR_THROW(std::bad_alloc());
        }
        if (remaining != std::numeric_limits<size_t>::max()) {
            --remaining;
        }
        auto *pointer = upstream->allocate(bytes, alignment);
        ledger->blocks.emplace(pointer, allocation_record{bytes, alignment});
        ++ledger->calls;
        ledger->live_bytes += bytes;
        return pointer;
    }

    void do_deallocate(void *pointer, size_t bytes, size_t alignment) override {
        const auto found = ledger->blocks.find(pointer);
        ASSERT_NE(found, ledger->blocks.end());
        EXPECT_EQ(found->second.bytes, bytes);
        EXPECT_EQ(found->second.alignment, alignment);
        ledger->live_bytes -= bytes;
        ledger->blocks.erase(found);
        upstream->deallocate(pointer, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        const auto *resource = dynamic_cast<const tracking_resource *>(&other);
        return resource != nullptr && resource->ledger == ledger;
    }

    std::pmr::memory_resource *upstream;
    allocation_ledger owned_ledger;
    allocation_ledger *ledger;
};

void expect_resource(const pmr_value &value, const tracking_resource &resource) {
    EXPECT_EQ(value.get_allocator().resource(), &resource);
    if (value.is_string()) {
        const auto &string = value.get<string_t>();
        EXPECT_EQ(string.get_allocator().resource(), &resource);
        EXPECT_TRUE(resource.owns(std::addressof(string)));
        if (!string.empty()) {
            EXPECT_TRUE(resource.owns(string.data()));
        }
    } else if (value.is_object()) {
        const auto &object = value.get<object_t>();
        EXPECT_EQ(object.get_allocator().resource(), &resource);
        EXPECT_TRUE(resource.owns(std::addressof(object)));
        for (const auto &[key, child] : object) {
            EXPECT_EQ(key.get_allocator().resource(), &resource);
            EXPECT_TRUE(resource.owns(std::addressof(key)));
            if (!key.empty()) {
                EXPECT_TRUE(resource.owns(key.data()));
            }
            expect_resource(child, resource);
        }
    } else if (value.is_array()) {
        const auto &array = value.get<array_t>();
        EXPECT_EQ(array.get_allocator().resource(), &resource);
        EXPECT_TRUE(resource.owns(std::addressof(array)));
        if (!array.empty()) {
            EXPECT_TRUE(resource.owns(array.data()));
        }
        for (const auto &child : array) {
            expect_resource(child, resource);
        }
    }
}

pmr_value parse_owned(std::string_view input, tracking_resource &resource) {
    ondemand_reader reader;
    reader.read(input);
    auto parsed = pmr_value::parse_with_allocator(reader, allocator(&resource));
    EXPECT_TRUE(parsed);
    return std::move(parsed).value();
}

struct registered_value {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(registered_value);
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(registered_value, description, numbers)
    std::string description = std::string(80, 'x');
    std::vector<int> numbers = {1, 2, 3};
};
} // namespace

TEST(json_pmr, parse_and_mutation_propagate_all_persistent_layers) {
    tracking_resource resource;
    {
        std::string input =
            R"({"name":"first","items":[null,true,false,1,-2,3.5,"\ud83d\ude00",{},[]],)"
            R"("long key which allocates its own character buffer":"long value which also allocates its own character buffer",)"
            R"("n\u0061me":"last"})";
        auto value = parse_owned(input, resource);
        const auto expected = value.to_string();
        input.assign(input.size(), 'x');
        EXPECT_EQ(value.to_string(), expected);
        EXPECT_EQ(value.at("name").get<string_t>(), "last");
        expect_resource(value, resource);
        value.get<object_t>()["new"] = std::string_view("added after a null insertion");
        auto &array = value.at("items").get<array_t>();
        array.emplace_back(wjr::default_construct);
        array.back() = std::vector<std::string>{"one", std::string(128, 'b')};
        expect_resource(value, resource);
        auto detached = std::move(value.at("new"));
        value.reset();
        expect_resource(detached, resource);
        detached = 4;
        detached = std::string_view("resource retained across scalar assignment");
        expect_resource(detached, resource);
    }
    EXPECT_EQ(resource.live_bytes(), 0);
}

TEST(json_pmr, explicit_clone_copy_selection_moves_and_swap) {
    tracking_resource one, two;
    {
        auto source = parse_owned(R"({"a":[1,"a long string that allocates a buffer"]})", one);
        pmr_value copied(std::allocator_arg, allocator(&two), source);
        expect_resource(copied, two);
        EXPECT_EQ(copied.to_string(), source.to_string());
        pmr_value selected(source);
        EXPECT_EQ(selected.get_allocator().resource(), std::pmr::get_default_resource());
        EXPECT_EQ(selected.at("a").get_allocator().resource(), std::pmr::get_default_resource());
        const auto calls = one.calls();
        auto *payload = std::addressof(source.get<object_t>());
        pmr_value moved(std::move(source));
        EXPECT_TRUE(source.is_null());
        EXPECT_EQ(std::addressof(moved.get<object_t>()), payload);
        EXPECT_EQ(one.calls(), calls);
        copied = std::move(moved);
        EXPECT_TRUE(moved.is_null());
        expect_resource(copied, two);
        pmr_value other(std::allocator_arg, allocator(&two), std::string_view("other"));
        const auto before = two.calls();
        copied.swap(other);
        EXPECT_EQ(two.calls(), before);
        expect_resource(copied, two);
        expect_resource(other, two);
    }
    EXPECT_EQ(one.live_bytes(), 0);
    EXPECT_EQ(two.live_bytes(), 0);
}

TEST(json_pmr, equal_distinct_resources_can_transfer_storage) {
    allocation_ledger ledger;
    tracking_resource one(ledger), two(ledger);
    {
        pmr_value source(
            std::allocator_arg, allocator(&one),
            std::string_view("owned string with heap storage retained by its equal allocator"));
        auto *payload = std::addressof(source.get<string_t>());
        pmr_value moved(std::allocator_arg, allocator(&two), std::move(source));
        EXPECT_TRUE(source.is_null());
        EXPECT_EQ(std::addressof(moved.get<string_t>()), payload);
        EXPECT_EQ(moved.get_allocator(), allocator(&one));
    }
    EXPECT_EQ(ledger.live_bytes, 0);
}

TEST(json_pmr, moving_a_child_into_its_parent_preserves_the_payload) {
    tracking_resource resource;
    {
        auto value =
            parse_owned(R"({"child":{"name":"retained subtree"},"other":[1,2]})", resource);
        value = std::move(value.at("child"));
        EXPECT_EQ(value.at("name").get<string_t>(), "retained subtree");
        expect_resource(value, resource);
    }
    EXPECT_EQ(resource.live_bytes(), 0);
}

TEST(json_pmr, stock_and_registered_serializers_keep_the_resource) {
    tracking_resource resource;
    {
        registered_value input;
        pmr_value value(std::allocator_arg, allocator(&resource), input);
        expect_resource(value, resource);
        registered_value output(value);
        EXPECT_EQ(output.description, input.description);
        EXPECT_EQ(output.numbers, input.numbers);
        value = std::map<std::string, std::vector<std::string>>{
            {"items", {std::string(64, 'a'), "short"}}};
        expect_resource(value, resource);
        pmr_value default_selected(std::string_view("standard construction"));
        EXPECT_EQ(default_selected.get_allocator().resource(), std::pmr::get_default_resource());
    }
    EXPECT_EQ(resource.live_bytes(), 0);
}

#ifndef WJR_DISABLE_EXCEPTIONS
TEST(json_pmr, copy_and_registered_constructor_failures_roll_back) {
    tracking_resource source_resource, target_resource;
    {
        auto source = parse_owned(
            R"({"a":[1,"long string data that allocates beyond small-string capacity"],"b":{"c":"value"}})",
            source_resource);
        const auto expected = source.to_string();
        size_t allocations;
        {
            const auto before = target_resource.calls();
            pmr_value baseline(std::allocator_arg, allocator(&target_resource), source);
            allocations = target_resource.calls() - before;
        }
        ASSERT_GT(allocations, 0);
        for (size_t limit = 0; limit < allocations; ++limit) {
            SCOPED_TRACE(limit);
            target_resource.remaining = limit;
            EXPECT_THROW((void)pmr_value(std::allocator_arg, allocator(&target_resource), source),
                         std::bad_alloc);
            EXPECT_EQ(target_resource.live_bytes(), 0);
            EXPECT_EQ(source.to_string(), expected);
        }
        target_resource.remaining = std::numeric_limits<size_t>::max();
        pmr_value destination(std::allocator_arg, allocator(&target_resource),
                              std::string_view("original"));
        const auto bytes = target_resource.live_bytes();
        target_resource.remaining = allocations / 2;
        EXPECT_THROW(destination = source, std::bad_alloc);
        EXPECT_EQ(destination.get<string_t>(), "original");
        EXPECT_EQ(target_resource.live_bytes(), bytes);
        target_resource.remaining = std::numeric_limits<size_t>::max();
    }
    EXPECT_EQ(source_resource.live_bytes(), 0);
    EXPECT_EQ(target_resource.live_bytes(), 0);
    registered_value input;
    size_t registered_allocations;
    {
        const auto before = target_resource.calls();
        pmr_value baseline(std::allocator_arg, allocator(&target_resource), input);
        registered_allocations = target_resource.calls() - before;
    }
    for (size_t limit = 0; limit < registered_allocations; ++limit) {
        target_resource.remaining = limit;
        EXPECT_THROW((void)pmr_value(std::allocator_arg, allocator(&target_resource), input),
                     std::bad_alloc);
        EXPECT_EQ(target_resource.live_bytes(), 0);
    }
}
#endif

TEST(json_pmr, invalid_input_matches_default_and_releases_partial_values) {
    const char *cases[] = {
        "",
        "[",
        R"({"a":"\q"})",
        R"({"a":[{},"\uD800"]})",
        R"({"\u00":1})",
        R"({"a":{"b":"text"},"a":[] } trailing)",
        R"([1e309])",
        R"([1e-400])",
    };
    tracking_resource resource;
    for (const char *input : cases) {
        ondemand_reader reader;
        reader.read(input);
        const auto expected = wjr::json::document::parse(reader);
        reader.read(input);
        const auto actual = pmr_value::parse_with_allocator(reader, allocator(&resource));
        ASSERT_FALSE(expected);
        ASSERT_FALSE(actual);
        EXPECT_EQ(actual.error(), expected.error());
        EXPECT_EQ(resource.live_bytes(), 0);
    }
}

TEST(json_pmr, pool_reuse_releases_clients_before_upstream) {
    tracking_resource upstream;
    {
        std::pmr::unsynchronized_pool_resource pool(&upstream);
        tracking_resource client(&pool);
        for (size_t i = 0; i < 8; ++i) {
            auto value = parse_owned(R"({"a":["first","second"],"b":"third"})", client);
            expect_resource(value, client);
        }
        EXPECT_EQ(client.live_bytes(), 0);
        EXPECT_GT(upstream.live_bytes(), 0);
        pool.release();
        EXPECT_EQ(upstream.live_bytes(), 0);
    }
    EXPECT_EQ(upstream.live_bytes(), 0);
}

TEST(json_pmr, parse_allocator_failure_keeps_terminating_mode) {
    EXPECT_DEATH_IF_SUPPORTED(
        {
            tracking_resource resource;
            resource.remaining = 0;
            ondemand_reader reader;
            reader.read("{}");
            (void)pmr_value::parse_with_allocator(reader, allocator(&resource));
        },
        "");
}
