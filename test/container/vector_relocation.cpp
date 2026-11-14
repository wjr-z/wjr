#include <gtest/gtest.h>

#include <array>
#include <memory_resource>
#include <ranges>
#include <stdexcept>
#include <vector>

#include <wjr/json/pmr.hpp>

namespace vector_relocation_test {

template <bool Replaceable>
struct value {
    static inline int resources = 0;
    static inline int assignments = 0;
    static inline int copies_before_throw = -1;
    int *resource;

    explicit value(int number = 0) : resource(new int(number)) { ++resources; }
    value(const value &other) : resource(nullptr) {
        if (copies_before_throw == 0)
            throw std::runtime_error("copy");
        if (copies_before_throw > 0)
            --copies_before_throw;
        resource = new int(*other.resource);
        ++resources;
    }
    value(value &&other) noexcept : resource(std::exchange(other.resource, nullptr)) {}
    value &operator=(const value &other) {
        ++assignments;
        if (resource == nullptr) {
            resource = new int(*other.resource);
            ++resources;
        } else {
            *resource = *other.resource;
        }
        return *this;
    }
    value &operator=(value &&other) noexcept {
        ++assignments;
        release();
        resource = std::exchange(other.resource, nullptr);
        return *this;
    }
    ~value() { release(); }
    void release() noexcept {
        if (resource != nullptr) {
            delete resource;
            --resources;
        }
    }
};

template <typename T>
struct hook_allocator : std::allocator<T> {
    using value_type = T;
    using is_trivially_allocator = std::false_type;
    static inline int constructed = 0;
    static inline int destroyed = 0;
    template <typename U>
    struct rebind {
        using other = hook_allocator<U>;
    };
    template <typename U, typename... Args>
    void construct(U *pointer, Args &&...args) {
        std::construct_at(pointer, std::forward<Args>(args)...);
        ++constructed;
    }
    template <typename U>
    void destroy(U *pointer) noexcept {
        ++destroyed;
        std::destroy_at(pointer);
    }
};

using replaceable = value<true>;
using legacy = value<false>;

struct converted_source {
    static inline int constructed = 0;
    static inline int assigned = 0;
    int number;
};

template <typename T>
struct lvalue_hook_allocator : std::allocator<T> {
    using value_type = T;
    static inline int constructed = 0;
    template <typename U>
    struct rebind {
        using other = lvalue_hook_allocator<U>;
    };
    void construct(T *pointer, T &other) {
        ++constructed;
        std::construct_at(pointer, other);
    }
};

struct move_only : replaceable {
    using replaceable::replaceable;
    move_only(const move_only &) = delete;
    move_only(move_only &&) = default;
    move_only &operator=(const move_only &) = delete;
    move_only &operator=(move_only &&) = default;
};

struct input {
    std::span<const replaceable> source;
    std::size_t position = 0;
    int increments_before_throw = -1;
    struct iterator {
        using iterator_concept = std::input_iterator_tag;
        using value_type = replaceable;
        using difference_type = std::ptrdiff_t;
        input *owner;
        const replaceable &operator*() const { return owner->source[owner->position]; }
        iterator &operator++() {
            if (owner->increments_before_throw == 0)
                throw std::runtime_error("increment");
            if (owner->increments_before_throw > 0)
                --owner->increments_before_throw;
            ++owner->position;
            return *this;
        }
        void operator++(int) { ++*this; }
        bool operator==(std::default_sentinel_t) const {
            return owner->position == owner->source.size();
        }
    };
    iterator begin() { return {this}; }
    std::default_sentinel_t end() const { return {}; }
    std::size_t size() const { return source.size(); }
};

template <typename Vector>
std::vector<int> numbers(const Vector &values) {
    std::vector<int> result;
    for (const auto &item : values)
        result.push_back(*item.resource);
    return result;
}

wjr::json::document::array_type documents() {
    using namespace wjr::json;
    document::array_type result;
    result.reserve(32);
    result.emplace_back(nullptr);
    result.emplace_back(true);
    result.emplace_back(42);
    result.emplace_back(1.25);
    result.emplace_back(std::string(96, 'x'));
    result.emplace_back(document::object_type{{"key", document(std::string(80, 'y'))}});
    result.emplace_back(document::array_type{document(false), document(123)});
    return result;
}

} // namespace vector_relocation_test

namespace wjr {
template <>
struct get_relocate_mode<vector_relocation_test::replaceable> {
    static constexpr relocate_t value = relocate_t::trivial;
    static constexpr bool is_replaceable = true;
};
template <>
struct get_relocate_mode<vector_relocation_test::legacy> {
    static constexpr relocate_t value = relocate_t::trivial;
};
template <>
struct get_relocate_mode<vector_relocation_test::move_only> {
    static constexpr relocate_t value = relocate_t::trivial;
    static constexpr bool is_replaceable = true;
};
} // namespace wjr

namespace wjr::json {
template <>
struct document_serializer_impl<vector_relocation_test::converted_source> {
    using value_type = vector_relocation_test::converted_source;
    template <typename Document>
    static Document construct(value_type source, in_place_document_serializer_to_t) {
        ++value_type::constructed;
        return Document(number_signed_t{}, source.number);
    }
    template <typename Document>
    static void assign(value_type source, Document &target, in_place_document_serializer_to_t) {
        ++value_type::assigned;
        target = source.number;
    }
};
} // namespace wjr::json

namespace {
using namespace vector_relocation_test;

TEST(vector_relocation, counted_document_insert_and_replace) {
    const auto source = documents();
    for (std::size_t position = 0; position <= source.size(); ++position) {
        for (std::size_t count = 0; count <= source.size(); ++count) {
            auto values = source;
            values.reserve(32);
            auto *storage = values.data();
            auto *prefix = position != 0 ? &values.front() : nullptr;
            const auto incoming = std::span(source.data(), count);
            std::vector<wjr::json::document> expected(source.begin(), source.end());
            expected.insert(expected.begin() + position, incoming.begin(), incoming.end());
            auto result = values.insert_range(values.cbegin() + position, incoming);
            EXPECT_EQ(result, values.begin() + position);
            EXPECT_EQ(values.data(), storage);
            if (prefix != nullptr)
                EXPECT_EQ(&values.front(), prefix);
            EXPECT_TRUE(std::ranges::equal(values, expected));

            values = source;
            values.reserve(32);
            expected.assign(source.begin(), source.end());
            const auto removed = std::min(std::size_t(2), source.size() - position);
            expected.erase(expected.begin() + position, expected.begin() + position + removed);
            expected.insert(expected.begin() + position, incoming.begin(), incoming.end());
            values.replace_range(values.cbegin() + position, values.cbegin() + position + removed,
                                 incoming);
            EXPECT_TRUE(std::ranges::equal(values, expected));
        }
    }
}

TEST(vector_relocation, construction_and_iterator_failure_restore_shifted_tail) {
    for (bool iterator_failure : {false, true}) {
        for (std::size_t position : {std::size_t(0), std::size_t(2), std::size_t(4)}) {
            for (int budget = 0; budget != 3; ++budget) {
                {
                    wjr::vector<replaceable> values;
                    values.reserve(16);
                    for (int number = 0; number != 5; ++number)
                        values.emplace_back(number);
                    const std::array incoming{replaceable(10), replaceable(11), replaceable(12)};
                    const auto resources = replaceable::resources;
                    const auto expected = numbers(values);
                    auto *storage = values.data();
                    const auto capacity = values.capacity();
                    input source{incoming};
                    if (iterator_failure)
                        source.increments_before_throw = budget;
                    else
                        replaceable::copies_before_throw = budget;
                    EXPECT_THROW(values.insert_range(values.cbegin() + position, source),
                                 std::runtime_error);
                    replaceable::copies_before_throw = -1;
                    EXPECT_EQ(values.data(), storage);
                    EXPECT_EQ(values.capacity(), capacity);
                    EXPECT_EQ(numbers(values), expected);
                    EXPECT_EQ(replaceable::resources, resources);
                    values.insert(values.cbegin() + position, incoming.begin(), incoming.end());
                    EXPECT_EQ(values.size(), 8);
                    EXPECT_EQ(*values[position].resource, 10);
                }
                EXPECT_EQ(replaceable::resources, 0);
            }
        }
    }
}

TEST(vector_relocation, noncommon_proxy_and_moving_sources) {
    {
        wjr::vector<replaceable> values;
        values.reserve(16);
        values.emplace_back(1);
        values.emplace_back(2);
        auto source = std::views::iota(7, 10) |
                      std::views::transform([](int number) { return replaceable(number); }) |
                      std::views::take_while([](const auto &) { return true; });
        static_assert(!std::ranges::common_range<decltype(source)>);
        values.insert_range(values.cbegin() + 1, source);
        EXPECT_EQ(numbers(values), (std::vector<int>{1, 7, 8, 9, 2}));
        std::array incoming{replaceable(10), replaceable(11)};
        auto moving = std::ranges::subrange(std::make_move_iterator(incoming.begin()),
                                            std::make_move_iterator(incoming.end()));
        values.insert_range(values.cbegin() + 4, moving);
        EXPECT_EQ(numbers(values), (std::vector<int>{1, 7, 8, 9, 10, 11, 2}));
        EXPECT_EQ(incoming[0].resource, nullptr);
        EXPECT_EQ(incoming[1].resource, nullptr);
        const auto empty_self = std::span(values.data(), std::size_t(0));
        EXPECT_EQ(values.insert_range(values.cbegin() + 1, empty_self), values.begin() + 1);
    }
    EXPECT_EQ(replaceable::resources, 0);
    {
        wjr::vector<move_only> values;
        values.reserve(8);
        values.emplace_back(1);
        values.emplace_back(2);
        std::array incoming{move_only(10), move_only(11)};
        auto moving = std::ranges::subrange(std::make_move_iterator(incoming.begin()),
                                            std::make_move_iterator(incoming.end()));
        values.insert_range(values.cbegin() + 1, moving);
        EXPECT_EQ(numbers(values), (std::vector<int>{1, 10, 11, 2}));
    }
    EXPECT_EQ(replaceable::resources, 0);
}

TEST(vector_relocation, registered_opt_in_skips_assignment_and_old_registration_preserves_it) {
    {
        wjr::vector<replaceable> values;
        values.reserve(16);
        for (int number = 0; number != 5; ++number)
            values.emplace_back(number);
        const std::array incoming{replaceable(10), replaceable(11)};
        replaceable::assignments = 0;
        values.insert_range(values.cbegin() + 1, incoming);
        EXPECT_EQ(replaceable::assignments, 0);
        EXPECT_EQ(numbers(values), (std::vector<int>{0, 10, 11, 1, 2, 3, 4}));
    }
    EXPECT_EQ(replaceable::resources, 0);
    {
        wjr::vector<legacy> values;
        values.reserve(16);
        for (int number = 0; number != 5; ++number)
            values.emplace_back(number);
        legacy::assignments = 0;
        const std::array incoming{legacy(10)};
        values.insert_range(values.cbegin() + 1, incoming);
        EXPECT_GT(legacy::assignments, 0);
    }
    EXPECT_EQ(legacy::resources, 0);
}

TEST(vector_relocation, allocator_hooks_keep_elementwise_shift) {
    using allocator = hook_allocator<replaceable>;
    allocator::constructed = allocator::destroyed = 0;
    {
        wjr::vector<replaceable, allocator> values;
        values.reserve(16);
        for (int number = 0; number != 5; ++number)
            values.emplace_back(number);
        const std::array incoming{replaceable(10), replaceable(11)};
        replaceable::assignments = 0;
        values.insert_range(values.cbegin() + 1, incoming);
        EXPECT_GT(replaceable::assignments, 0);
        EXPECT_EQ(allocator::constructed, 7);
        EXPECT_EQ(allocator::destroyed, 0);
    }
    EXPECT_EQ(allocator::constructed, allocator::destroyed);
    EXPECT_EQ(replaceable::resources, 0);
}

TEST(vector_relocation, pmr_document_assignment_retains_destination_resource) {
    using document = wjr::json::pmr_document;
    using allocator = document::allocator_type;
    static_assert(!wjr::get_relocate_mode<document>::is_replaceable);
    struct custom_traits : wjr::json::detail::default_document_traits {};
    static_assert(
        !wjr::get_relocate_mode<wjr::json::basic_document<custom_traits>>::is_replaceable);
    std::pmr::unsynchronized_pool_resource first, second, third;
    wjr::vector<document> values;
    values.reserve(8);
    values.emplace_back(std::allocator_arg, allocator(&first), std::string_view("first"));
    values.emplace_back(std::allocator_arg, allocator(&second), std::string_view("second"));
    values.emplace_back(std::allocator_arg, allocator(&third), std::string_view("third"));
    std::array incoming{document(std::allocator_arg, allocator(&third), std::string_view("new"))};
    values.insert_range(values.cbegin(), incoming);
    EXPECT_EQ(values[0].get_allocator().resource(), &first);
    EXPECT_EQ(values[0].get<wjr::json::string_t>(), "new");
}

TEST(vector_relocation, incoming_lvalue_hook_keeps_assignment_path) {
    using allocator = lvalue_hook_allocator<replaceable>;
    static_assert(wjr::is_trivially_allocator_construct_v<allocator, replaceable, replaceable &&>);
    static_assert(!wjr::is_trivially_allocator_construct_v<allocator, replaceable, replaceable &>);
    allocator::constructed = 0;
    {
        wjr::vector<replaceable, allocator> values;
        values.reserve(16);
        for (int number = 0; number != 5; ++number)
            values.emplace_back(number);
        std::array incoming{replaceable(10), replaceable(11)};
        replaceable::assignments = 0;
        values.insert_range(values.cbegin() + 1, incoming);
        EXPECT_EQ(allocator::constructed, 0);
        EXPECT_GT(replaceable::assignments, 0);
        EXPECT_EQ(numbers(values), (std::vector<int>{0, 10, 11, 1, 2, 3, 4}));
    }
    EXPECT_EQ(replaceable::resources, 0);
}

TEST(vector_relocation, converting_iterator_preserves_serializer_assignment) {
    using document = wjr::json::document;
    auto values = documents();
    std::array incoming{converted_source{10}, converted_source{11}};
    converted_source::constructed = converted_source::assigned = 0;
    values.insert(values.cbegin() + 1, incoming.begin(), incoming.end());
    EXPECT_EQ(converted_source::constructed, 0);
    EXPECT_EQ(converted_source::assigned, 2);
    EXPECT_EQ(values[1], document(10));
    EXPECT_EQ(values[2], document(11));
}
} // namespace
