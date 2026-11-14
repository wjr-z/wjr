#include <gtest/gtest.h>

#include <bit>
#include <stdexcept>

#include <wjr/json/document.hpp>
#include <wjr/json/inplace_parser.hpp>

namespace {
using namespace wjr::json;

struct point {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(point);
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(point, x, y)
    double x = 0;
    double y = 0;
};

struct record {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(record);
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(record, id, name, active, points, samples)
    uint64_t id = 0;
    std::string name;
    bool active = false;
    std::vector<point> points;
    wjr::vector<int> samples;
};

template <typename T>
concept can_parse = requires(ondemand_reader &reader) { wjr::json::parse<T>(reader); };
static_assert(can_parse<record>);
static_assert(!can_parse<std::string_view>);
static_assert(!can_parse<wjr::fixed_vector<int>>);
static_assert(!can_parse<wjr::inplace_vector<int, 4>>);
static_assert(!can_parse<wjr::small_vector<int, 4>>);

struct recursive_record {
    WJR_REGISTER_JSON_FIELDS(recursive_record, id, children)
    int id = 0;
    std::vector<recursive_record> children;
};

struct constructible_record {
    WJR_REGISTER_JSON_FIELDS(constructible_record, value)
    constructible_record() = default;
    constructible_record(constructible_record &&) = default;
    constructible_record &operator=(constructible_record &&) = delete;
    int value = 0;
};
static_assert(can_parse<constructible_record>);

template <typename T>
auto parsed(std::string_view input) {
    ondemand_reader reader;
    reader.read(input);
    return wjr::json::parse<T>(reader);
}

TEST(json_direct_parser, nested_owned_values_and_array_growth) {
    const auto input = R"({"samples":[1,-2,3],"points":[{"y":2.5,"x":1},{"x":3,"y":4}],)"
                       R"("name":"hello\n\u4e2d","active":true,"id":7})";
    auto result = parsed<record>(input);
    ASSERT_TRUE(result);
    EXPECT_EQ(result->id, 7);
    EXPECT_EQ(result->name, "hello\n\xe4\xb8\xad");
    EXPECT_TRUE(result->active);
    ASSERT_EQ(result->points.size(), 2);
    EXPECT_EQ(result->points[0].x, 1);
    EXPECT_EQ(result->points[0].y, 2.5);
    EXPECT_EQ(result->points[1].x, 3);
    EXPECT_EQ(result->samples[1], -2);

    std::string array = "[";
    for (int i = 0; i != 512; ++i) {
        if (i != 0)
            array += ',';
        array += R"({"x":1,"y":2})";
    }
    array += ']';
    auto values = parsed<std::vector<point>>(array);
    ASSERT_TRUE(values);
    ASSERT_EQ(values->size(), 512);
    EXPECT_EQ(values->back().y, 2);
}

TEST(json_direct_parser, duplicates_replace_and_unknown_values_are_checked) {
    auto value = parsed<point>(R"({"x":1,"extra":{"a":[true,null,"ok",1.5]},"y":2,"x":3})");
    ASSERT_TRUE(value);
    EXPECT_EQ(value->x, 3);
    EXPECT_EQ(value->y, 2);
    EXPECT_FALSE(parsed<point>(R"({"x":"bad","y":2,"x":3})"));
    auto arrays = parsed<recursive_record>(
        R"({"id":1,"children":[{"id":2,"children":[]}],"children":[]})");
    ASSERT_TRUE(arrays);
    EXPECT_TRUE(arrays->children.empty());
    const char *invalid[] = {
        R"({"x":1,"y":2,"extra":{"a":[0,]}})",
        R"({"x":1,"y":2,"extra":"\uD800"})",
        R"({"x":1,"y":2,"extra":1e9999})",
        R"({"x":1,"y":2} [])",
    };
    for (const auto input : invalid) {
        SCOPED_TRACE(input);
        EXPECT_FALSE(parsed<point>(input));
    }
}

template <size_t Index>
struct numbered_field {
    int value = 0;
};

template <typename Sequence>
struct wide_record;

template <size_t... Index>
struct wide_record<std::index_sequence<Index...>> : numbered_field<Index>... {
    static constexpr auto names = [] {
        std::array<std::array<char, 3>, sizeof...(Index)> output{};
        for (size_t index = 0; index != output.size(); ++index) {
            output[index] = {'f', static_cast<char>('0' + index / 10),
                             static_cast<char>('0' + index % 10)};
        }
        return output;
    }();

    template <typename Owner = wide_record>
    static constexpr auto wjr_json_fields() {
        return std::tuple{field_descriptor<&numbered_field<Index>::value>{
            std::string_view(names[Index].data(), names[Index].size())}...};
    }
};

TEST(json_direct_parser, wide_inherited_fields_track_required_bits_across_words) {
    using wide = wide_record<std::make_index_sequence<65>>;
    std::string input = "{";
    for (size_t index = 0; index != 65; ++index) {
        if (index != 0) {
            input += ',';
        }
        input += '"';
        input.append(wide::names[index].data(), wide::names[index].size());
        input += "\":" + std::to_string(index);
    }
    input += '}';
    auto value = parsed<wide>(input);
    ASSERT_TRUE(value);
    EXPECT_EQ(static_cast<const numbered_field<0> &>(*value).value, 0);
    EXPECT_EQ(static_cast<const numbered_field<64> &>(*value).value, 64);
    input.erase(input.rfind(','));
    input += '}';
    auto missing = parsed<wide>(input);
    ASSERT_FALSE(missing);
    EXPECT_EQ(missing.error().code, error_code::NO_SUCH_FIELD);
    EXPECT_EQ(missing.error().offset, input.size() - 1);
}

TEST(json_direct_parser, required_fields_and_error_offsets) {
    auto missing = parsed<point>(R"({"x":1})");
    ASSERT_FALSE(missing);
    EXPECT_EQ(missing.error().code, error_code::NO_SUCH_FIELD);
    EXPECT_EQ(missing.error().offset, 6);
    auto type = parsed<point>(R"({"x":"bad","y":1})");
    ASSERT_FALSE(type);
    EXPECT_EQ(type.error().code, error_code::INCORRECT_TYPE);
    EXPECT_EQ(type.error().offset, 5);
    auto trailing = parsed<point>(R"({"x":1,"y":2} true)");
    ASSERT_FALSE(trailing);
    EXPECT_EQ(trailing.error().code, error_code::TRAILING_CONTENT);
    EXPECT_EQ(trailing.error().offset, 14);
    auto end = parsed<point>(R"({"x":1,)");
    ASSERT_FALSE(end);
    EXPECT_EQ(end.error().offset, 7);
}

TEST(json_direct_parser, numeric_types_are_strict_and_boolean_arrays_work) {
    EXPECT_TRUE(parsed<int64_t>("-9223372036854775808"));
    EXPECT_TRUE(parsed<uint64_t>("18446744073709551615"));
    EXPECT_FALSE(parsed<int>("1.0"));
    EXPECT_FALSE(parsed<unsigned>("-1"));
    EXPECT_FALSE(parsed<int8_t>("128"));
    EXPECT_FALSE(parsed<float>("1e100"));
    EXPECT_FALSE(parsed<bool>("1"));
    auto bits = parsed<std::vector<bool>>("[true,false,true]");
    ASSERT_TRUE(bits);
    ASSERT_EQ(bits->size(), 3);
    EXPECT_TRUE((*bits)[0]);
    EXPECT_FALSE((*bits)[1]);
}

TEST(json_direct_parser, floats_round_at_destination_precision_and_preserve_zero_sign) {
    auto rounded = parsed<float>("1.0000000596046448");
    ASSERT_TRUE(rounded);
    EXPECT_EQ(std::bit_cast<uint32_t>(*rounded), UINT32_C(0x3f800001));
    for (const char *input : {"-0", "-0.0", "-0e3"}) {
        auto value = parsed<float>(input);
        ASSERT_TRUE(value);
        EXPECT_TRUE(std::signbit(*value));
        EXPECT_EQ(*value, 0);
    }
    EXPECT_FALSE(parsed<float>("1e-999"));
    auto overflow = parsed<int>("18446744073709551616");
    ASSERT_FALSE(overflow);
    EXPECT_EQ(overflow.error().code, error_code::NUMBER_OUT_OF_RANGE);
}

TEST(json_direct_parser, deep_container_and_seen_storage_growth_preserve_parents) {
    std::string input;
    for (int depth = 0; depth != 40; ++depth) {
        input += "{\"id\":" + std::to_string(depth) + ",\"children\":[";
    }
    for (int depth = 0; depth != 40; ++depth)
        input += "]}";
    auto value = parsed<recursive_record>(input);
    ASSERT_TRUE(value);
    const recursive_record *node = std::addressof(*value);
    for (int depth = 0; depth != 40; ++depth) {
        EXPECT_EQ(node->id, depth);
        if (depth != 39) {
            ASSERT_EQ(node->children.size(), 1);
            node = std::addressof(node->children[0]);
        }
    }
    EXPECT_TRUE(node->children.empty());
    const auto too_deep =
        "{\"x\":1,\"y\":2,\"unknown\":" + std::string(300, '[') + std::string(300, ']') + "}";
    auto rejected = parsed<point>(too_deep);
    ASSERT_FALSE(rejected);
    EXPECT_EQ(rejected.error().code, error_code::DEPTH_ERROR);
}

TEST(json_direct_parser, escaped_keys_and_deleted_move_assignment) {
    auto value = parsed<point>(R"({"\u0078":1,"y":2})");
    ASSERT_TRUE(value);
    EXPECT_EQ(value->x, 1);
    auto record = parsed<constructible_record>(R"({"value":3})");
    ASSERT_TRUE(record);
    EXPECT_EQ(record->value, 3);
}

struct reference_record {
    int &value;
    explicit reference_record(int &reference) : value(reference) {}
    WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER(reference_record, value)
};

TEST(json_direct_parser, descriptor_generation_keeps_existing_reference_projection) {
    int value = 0;
    reference_record output(value);
    ondemand_reader reader;
    reader.read(R"({"value":42})");
    auto source = document::parse(reader);
    ASSERT_TRUE(source);
    output = *source;
    EXPECT_EQ(value, 42);
}

struct immobile_record {
    WJR_REGISTER_JSON_FIELDS(immobile_record, value)
    int value;
    explicit immobile_record(int initial) : value(initial) {}
    immobile_record(const immobile_record &) = delete;
    immobile_record(immobile_record &&) = delete;
};

struct legacy_callbacks : inplace_parser<int> {
    using inplace_parser<int>::visit_root_true;
    using inplace_parser<int>::visit_object_key_string;
    using inplace_parser<int>::visit_end_array_to_root;
};

TEST(json_direct_parser, legacy_entry_reuses_live_targets_and_preserves_callback_surface) {
    static_assert(std::is_base_of_v<inplace_parser_base, inplace_parser_impl<int>>);
    static_assert(noexcept(inplace_parser<immobile_record>::parse(
        std::declval<immobile_record *>(), std::declval<ondemand_reader &>())));
    immobile_record output(0);
    ondemand_reader reader;
    reader.read(R"({"value":7})");
    ASSERT_TRUE(inplace_parser<immobile_record>::parse(&output, reader));
    EXPECT_EQ(output.value, 7);
    std::string text;
    text.reserve(128);
    const auto capacity = text.capacity();
    reader.read(R"("hello")");
    ASSERT_TRUE(inplace_parser<std::string>::parse(&text, reader));
    EXPECT_EQ(text, "hello");
    EXPECT_EQ(text.capacity(), capacity);
    legacy_callbacks callbacks;
    EXPECT_TRUE(callbacks.visit_root_true("true"));
    const std::string_view key = "key";
    EXPECT_TRUE(callbacks.visit_object_key_string(key.data(), key.data() + key.size()));
    EXPECT_TRUE(callbacks.visit_end_array_to_root(0));
}

#ifndef WJR_DISABLE_EXCEPTIONS
struct construction_error {};
struct tracked {
    static inline int live = 0;
    static inline int attempts = 0;
    static inline int fail_on = 0;
    WJR_REGISTER_JSON_FIELDS(tracked, value)
    int value = 0;
    tracked() {
        if (++attempts == fail_on)
            throw construction_error{};
        ++live;
    }
    tracked(tracked &&other) noexcept : value(other.value) { ++live; }
    tracked &operator=(tracked &&) = default;
    ~tracked() { --live; }
};

TEST(json_direct_parser, user_constructor_exceptions_unwind_existing_elements) {
    tracked::attempts = 0;
    tracked::fail_on = 3;
    EXPECT_THROW(parsed<std::vector<tracked>>(R"([{"value":1},{"value":2},{"value":3}])"),
                 construction_error);
    EXPECT_EQ(tracked::live, 0);
    tracked::fail_on = 0;
    EXPECT_FALSE(parsed<std::vector<tracked>>(R"([{"value":1},{"value":"wrong"}])"));
    EXPECT_EQ(tracked::live, 0);
}

struct throwing_move_record {
    static inline int live = 0;
    WJR_REGISTER_JSON_FIELDS(throwing_move_record, value)
    int value = 0;
    throwing_move_record() { ++live; }
    throwing_move_record(throwing_move_record &&) { throw construction_error{}; }
    ~throwing_move_record() { --live; }
};

TEST(json_direct_parser, result_move_exceptions_destroy_the_factory_target) {
    EXPECT_THROW(parsed<throwing_move_record>(R"({"value":7})"), construction_error);
    EXPECT_EQ(throwing_move_record::live, 0);
}

struct allocation_state {
    static inline int calls = 0;
    static inline int fail_on = 0;
    static inline size_t live_bytes = 0;
};

template <typename T>
struct failing_allocator {
    using value_type = T;
    failing_allocator() = default;
    template <typename U>
    failing_allocator(const failing_allocator<U> &) noexcept {}
    T *allocate(size_t count) {
        if (++allocation_state::calls == allocation_state::fail_on)
            throw std::bad_alloc{};
        T *result = std::allocator<T>{}.allocate(count);
        allocation_state::live_bytes += count * sizeof(T);
        return result;
    }
    void deallocate(T *pointer, size_t count) noexcept {
        allocation_state::live_bytes -= count * sizeof(T);
        std::allocator<T>{}.deallocate(pointer, count);
    }
    template <typename U>
    friend bool operator==(const failing_allocator &, const failing_allocator<U> &) noexcept {
        return true;
    }
};

using counted_string = std::basic_string<char, std::char_traits<char>, failing_allocator<char>>;
struct allocated_record {
    WJR_REGISTER_JSON_FIELDS(allocated_record, name, labels)
    counted_string name;
    std::vector<counted_string> labels;
};

TEST(json_direct_parser, short_escaped_strings_use_decoded_size_before_growth) {
    const std::string input = R"("\u4e2d\u4e2d\u4e2d\u4e2d")";
    allocation_state::calls = 0;
    allocation_state::fail_on = 1;
    {
        auto value = parsed<counted_string>(input);
        ASSERT_TRUE(value);
        EXPECT_EQ(value->size(), 12);
        EXPECT_EQ(allocation_state::calls, 0);
    }
    EXPECT_EQ(allocation_state::live_bytes, 0);
    allocation_state::fail_on = 0;
    ondemand_reader reader;
    reader.read(input);
    auto source = document::parse(reader);
    ASSERT_TRUE(source);
    EXPECT_EQ(source->get<string_t>().size(), 12);
    EXPECT_EQ(source->get<string_t>().capacity(), std::string{}.capacity());
    EXPECT_FALSE(parsed<counted_string>(R"("\u4e2d\u4e2d\u4e2d\uD800")"));
    EXPECT_EQ(allocation_state::live_bytes, 0);
}

TEST(json_direct_parser, every_owned_string_allocation_failure_cleans_partial_output) {
    const std::string text(128, 'x');
    const std::string input =
        "{\"name\":\"" + text + "\",\"labels\":[\"" + text + "\",\"" + text + "\"]}";
    allocation_state::calls = 0;
    allocation_state::fail_on = 0;
    {
        auto value = parsed<allocated_record>(input);
        ASSERT_TRUE(value);
        EXPECT_EQ(value->labels.size(), 2);
    }
    ASSERT_EQ(allocation_state::live_bytes, 0);
    const int count = allocation_state::calls;
    ASSERT_GT(count, 0);
    for (int fail = 1; fail <= count; ++fail) {
        allocation_state::calls = 0;
        allocation_state::fail_on = fail;
        EXPECT_THROW(parsed<allocated_record>(input), std::bad_alloc);
        EXPECT_EQ(allocation_state::live_bytes, 0);
    }
    allocation_state::fail_on = 0;
}
#endif
} // namespace
