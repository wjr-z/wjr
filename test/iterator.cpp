#include <array>
#include <iterator>
#include <list>
#include <memory>
#include <sstream>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/ring_buffer.hpp>
#include <wjr/container/vector.hpp>
#include <wjr/memory/copy.hpp>
#include <wjr/memory/uninitialized.hpp>
#include <wjr/span.hpp>

namespace iterator_test {
struct traits_cursor {
    int &operator*() const;
};
} // namespace iterator_test

template <>
struct std::iterator_traits<iterator_test::traits_cursor> {
    using value_type = long;
    using difference_type = long long;
};

namespace {

// A C++20 input iterator need not supply the legacy iterator_traits members.
struct input_cursor {
    using iterator_concept = std::input_iterator_tag;
    using value_type = int;
    using difference_type = std::ptrdiff_t;

    int *ptr = nullptr;

    int &operator*() const { return *ptr; }
    input_cursor &operator++() {
        ++ptr;
        return *this;
    }
    void operator++(int) { ++*this; }
};

struct proxy_cursor {
    using element_type = int;
    using difference_type = std::ptrdiff_t;

    int operator*() const { return 0; }
};

struct nontrivial {
    nontrivial() = default;
    nontrivial(const nontrivial &) {}
};

using counted = std::counted_iterator<int *>;
using const_counted = std::counted_iterator<const int *>;
using volatile_counted = std::counted_iterator<volatile int *>;

static_assert(std::input_iterator<input_cursor>);
static_assert(!wjr::is_iterator_v<input_cursor>);
static_assert(std::same_as<wjr::iterator_difference_t<input_cursor>, std::ptrdiff_t>);
static_assert(std::same_as<wjr::iterator_value_t<input_cursor>, int>);
static_assert(std::same_as<wjr::iterator_reference_t<input_cursor>, int &>);
static_assert(std::same_as<wjr::iterator_value_t<proxy_cursor>, int>);
static_assert(std::same_as<wjr::iterator_reference_t<proxy_cursor>, int>);
static_assert(std::same_as<wjr::iterator_difference_t<iterator_test::traits_cursor>, long long>);
static_assert(std::same_as<wjr::iterator_value_t<iterator_test::traits_cursor>, long>);
static_assert(std::same_as<wjr::iterator_reference_t<iterator_test::traits_cursor>, int &>);
static_assert(std::same_as<wjr::iterator_difference_t<const int *>, std::ptrdiff_t>);
static_assert(std::same_as<wjr::iterator_value_t<volatile int *>, int>);
static_assert(std::same_as<wjr::iterator_reference_t<const int *>, const int &>);
static_assert(std::same_as<wjr::iterator_reference_t<volatile int *>, volatile int &>);
static_assert(
    std::same_as<wjr::iterator_value_t<std::back_insert_iterator<std::vector<int>>>, void>);

static_assert(std::contiguous_iterator<counted>);
static_assert(wjr::is_contiguous_iterator_v<counted>);
static_assert(wjr::is_contiguous_iterator_v<const_counted>);
static_assert(wjr::is_contiguous_iterator_v<volatile_counted>);
static_assert(wjr::is_contiguous_iterator_v<const int *>);
static_assert(wjr::is_contiguous_iterator_v<volatile int *>);
static_assert(!wjr::is_contiguous_iterator_v<void *>);
static_assert(!wjr::is_contiguous_iterator_v<const void *>);
static_assert(!wjr::is_contiguous_iterator_v<void (*)()>);
static_assert(!wjr::is_contiguous_iterator_v<int>);
// These remain WJR extensions, not claims of standard concept conformance.
static_assert(wjr::is_contiguous_iterator_v<int[3]>);
static_assert(wjr::is_contiguous_iterator_v<std::move_iterator<int *>>);
static_assert(!wjr::is_contiguous_iterator_v<std::move_iterator<nontrivial *>>);
static_assert(!std::contiguous_iterator<std::move_iterator<int *>>);

template <typename... Iters>
constexpr bool admission_matches_traits =
    ((wjr::legacy_iterator<Iters> == wjr::is_iterator_v<Iters> &&
      wjr::contiguous_storage_iterator<Iters> == wjr::is_contiguous_iterator_v<Iters>) &&
     ...);

static_assert(admission_matches_traits<int *, const int *, volatile int *, int[3], void *,
                                       void (*)(), int, void, input_cursor, counted,
                                       std::move_iterator<int *>, std::move_iterator<nontrivial *>,
                                       std::list<int>::iterator, std::istream_iterator<int>,
                                       std::back_insert_iterator<std::vector<int>>>);
static_assert(!wjr::legacy_iterator<input_cursor>);
static_assert(wjr::legacy_iterator<std::back_insert_iterator<std::vector<int>>>);
static_assert(wjr::contiguous_storage_iterator<int[3]>);
static_assert(wjr::contiguous_storage_iterator<std::move_iterator<int *>>);

template <typename Iter>
concept has_contiguous_aliases = requires {
    typename wjr::iterator_contiguous_value_t<Iter>;
    typename wjr::iterator_contiguous_pointer_t<Iter>;
};

static_assert(has_contiguous_aliases<const int *>);
static_assert(has_contiguous_aliases<std::move_iterator<int *>>);
static_assert(!has_contiguous_aliases<int>);
static_assert(!has_contiguous_aliases<void *>);
static_assert(!has_contiguous_aliases<std::list<int>::iterator>);
static_assert(!has_contiguous_aliases<std::move_iterator<nontrivial *>>);
static_assert(std::contiguous_iterator<wjr::span<int>::iterator>);
static_assert(wjr::is_contiguous_iterator_v<wjr::span<int>::iterator>);
static_assert(std::same_as<wjr::iterator_contiguous_pointer_t<const_counted>, const int *>);
static_assert(std::same_as<wjr::iterator_contiguous_pointer_t<volatile_counted>, volatile int *>);

template <typename Function>
void check_counted_result(Function function) {
    const int source[] = {2, 4, 6};
    int destination[] = {0, 0, 0, 0, 0};
    const auto result = function(const_counted(source, 3), counted(destination + 1, 4));
    EXPECT_EQ(result.base(), destination + 4);
    EXPECT_EQ(result.count(), 1);
    EXPECT_EQ(destination[0], 0);
    EXPECT_EQ(destination[1], 2);
    EXPECT_EQ(destination[2], 4);
    EXPECT_EQ(destination[3], 6);
    EXPECT_EQ(destination[4], 0);
}

} // namespace

TEST(iterator, counted_copy_preserves_output_state) {
    check_counted_result([](auto first, auto dest) { return wjr::copy(first, first + 3, dest); });
    check_counted_result([](auto first, auto dest) { return wjr::copy_n(first, 3, dest); });
    check_counted_result(
        [](auto first, auto dest) { return wjr::copy_restrict(first, first + 3, dest); });
    check_counted_result(
        [](auto first, auto dest) { return wjr::copy_n_restrict(first, 3, dest); });
    check_counted_result([](auto first, auto dest) { return wjr::move(first, first + 3, dest); });
    check_counted_result([](auto first, auto dest) { return wjr::move_n(first, 3, dest); });
}

TEST(iterator, counted_copy_empty_and_negative_count) {
    const counted empty(nullptr, 0);
    EXPECT_EQ(wjr::copy(empty, empty, empty).count(), 0);
    EXPECT_EQ(wjr::copy_n(empty, 0, empty).base(), nullptr);
    EXPECT_EQ(wjr::copy_n(empty, -1, empty).count(), 0);
    EXPECT_EQ(wjr::copy_restrict(empty, empty, empty).base(), nullptr);
    EXPECT_EQ(wjr::copy_n_restrict(empty, 0, empty).count(), 0);
}

TEST(iterator, wrapped_and_volatile_outputs_remain_usable) {
    const int source[] = {1, 3, 5};
    std::vector<int> destination(5);
    EXPECT_EQ(wjr::copy(source, source + 3, destination.begin()), destination.begin() + 3);
    EXPECT_EQ(wjr::copy_n(source, 2, destination.begin() + 3), destination.end());
    wjr::span view(destination.data(), destination.size());
    EXPECT_EQ(wjr::copy_n(source, 3, view.begin()), view.begin() + 3);

    volatile int observed[3] = {};
    const auto end = wjr::copy_n(source, 3, volatile_counted(observed, 3));
    EXPECT_EQ(end.base(), observed + 3);
    EXPECT_EQ(end.count(), 0);
    EXPECT_EQ(observed[2], 5);

    std::vector<int> appended;
    wjr::copy(source, source + 3, std::back_inserter(appended));
    EXPECT_EQ(appended, (std::vector<int>{1, 3, 5}));
}

TEST(iterator, nontrivial_moves_keep_move_semantics) {
    std::array<std::unique_ptr<int>, 2> source{std::make_unique<int>(7), std::make_unique<int>(9)};
    std::array<std::unique_ptr<int>, 2> destination;
    EXPECT_EQ(wjr::move(source.begin(), source.end(), destination.begin()), destination.end());
    EXPECT_EQ(source[0], nullptr);
    EXPECT_EQ(source[1], nullptr);
    EXPECT_EQ(*destination[0], 7);
    EXPECT_EQ(*destination[1], 9);
}

TEST(iterator, counted_uninitialized_copy_preserves_output_state) {
    const int source[] = {2, 4, 6};
    std::allocator<int> alloc;
    int *destination = alloc.allocate(5);
    const auto output = counted(destination, 5);
    const auto result = wjr::uninitialized_copy_n(const_counted(source, 3), 3, output);
    EXPECT_EQ(result.base(), destination + 3);
    EXPECT_EQ(result.count(), 2);
    EXPECT_EQ(destination[2], 6);
    std::destroy_n(destination, 3);
    const auto range_result =
        wjr::uninitialized_copy(const_counted(source, 3), const_counted(source + 3, 0), output);
    EXPECT_EQ(range_result.base(), destination + 3);
    EXPECT_EQ(range_result.count(), 2);
    EXPECT_EQ(destination[0], 2);
    std::destroy_n(destination, 3);
    alloc.deallocate(destination, 5);
}

TEST(iterator, counted_relocation_preserves_output_state) {
    std::allocator<int> alloc;
    int *source = alloc.allocate(3);
    int *destination = alloc.allocate(5);
    for (int i = 0; i < 3; ++i) {
        std::construct_at(source + i, i + 1);
    }
    const auto result =
        wjr::uninitialized_relocate_n(counted(source, 3), 3, counted(destination, 5));
    EXPECT_EQ(result.base(), destination + 3);
    EXPECT_EQ(result.count(), 2);
    EXPECT_EQ(destination[2], 3);
    std::destroy_n(destination, 3);
    for (int i = 0; i < 3; ++i) {
        std::construct_at(source + i, i + 4);
    }
    const auto range_result = wjr::uninitialized_relocate(
        counted(source, 3), counted(source + 3, 0), counted(destination, 5));
    EXPECT_EQ(range_result.base(), destination + 3);
    EXPECT_EQ(range_result.count(), 2);
    EXPECT_EQ(destination[0], 4);
    std::destroy_n(destination, 3);
    alloc.deallocate(source, 3);
    alloc.deallocate(destination, 5);
}

TEST(iterator, legacy_pairs_keep_container_deduction_and_category_dispatch) {
    const int source[] = {2, 4, 6};
    wjr::basic_vector values(std::begin(source), std::end(source));
    static_assert(std::same_as<decltype(values), wjr::vector<int>>);
    const std::list<int> nodes = {7, 9};
    values.assign(nodes.begin(), nodes.end());
    values.append(std::begin(source), std::end(source));
    values.insert(values.begin() + 1, nodes.begin(), nodes.end());
    values.replace(values.begin(), values.begin() + 2, std::begin(source), std::end(source));
    EXPECT_EQ((std::vector<int>(values.begin(), values.end())),
              (std::vector<int>{2, 4, 6, 9, 9, 2, 4, 6}));

    std::istringstream stream("11 13 17");
    wjr::basic_ring_buffer ring(std::istream_iterator<int>{stream}, std::istream_iterator<int>{});
    static_assert(std::same_as<decltype(ring), wjr::ring_buffer<int>>);
    EXPECT_EQ((std::vector<int>(ring.begin(), ring.end())), (std::vector<int>{11, 13, 17}));
    ring.assign(nodes.begin(), nodes.end());
    EXPECT_EQ((std::vector<int>(ring.begin(), ring.end())), (std::vector<int>{7, 9}));
}
