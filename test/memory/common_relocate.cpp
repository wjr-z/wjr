#include <tuple>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/compressed_pair.hpp>
#include <wjr/memory/uninitialized.hpp>
#include <wjr/tuple.hpp>

namespace common_relocate_test {
struct normal {
    static inline int moves = 0;
    static inline int destructions = 0;
    const normal *self = this;
    normal() = default;
    normal(normal &&other) noexcept {
        EXPECT_EQ(other.self, &other);
        ++moves;
    }
    ~normal() noexcept {
        EXPECT_EQ(self, this);
        ++destructions;
    }
};
struct maybe : normal {};
struct incomplete;
} // namespace common_relocate_test

template <>
struct wjr::get_relocate_mode<common_relocate_test::maybe> {
    static constexpr wjr::relocate_t value = wjr::relocate_t::maybe_trivial;
};

template <>
struct wjr::get_relocate_mode<common_relocate_test::incomplete> {
    static constexpr wjr::relocate_t value = wjr::relocate_t::maybe_trivial;
};

namespace {
using common_relocate_test::maybe;
using common_relocate_test::normal;
using wjr::get_common_relocate_mode_v;
using wjr::get_relocate_mode_v;
using wjr::relocate_t;
static_assert(get_common_relocate_mode_v<> == relocate_t::trivial);
static_assert(get_common_relocate_mode_v<int, long, char> == relocate_t::trivial);
static_assert(get_common_relocate_mode_v<normal, int, int> == relocate_t::normal);
static_assert(get_common_relocate_mode_v<int, normal, int> == relocate_t::normal);
static_assert(get_common_relocate_mode_v<int, int, normal> == relocate_t::normal);
static_assert(get_common_relocate_mode_v<int, maybe, int> == relocate_t::maybe_trivial);
static_assert(get_common_relocate_mode_v<maybe, int, normal> == relocate_t::normal);
static_assert(get_common_relocate_mode_v<int, normal, maybe> == relocate_t::normal);
static_assert(get_relocate_mode_v<std::pair<const int, normal>> == relocate_t::normal);
static_assert(get_relocate_mode_v<wjr::compressed_pair<int, normal>> == relocate_t::normal);
static_assert(get_relocate_mode_v<std::tuple<int, maybe, int>> == relocate_t::maybe_trivial);
static_assert(get_relocate_mode_v<wjr::tuple<int, int, normal>> == relocate_t::normal);
static_assert(get_relocate_mode_v<std::pair<int, int>> == relocate_t::trivial);
static_assert(get_common_relocate_mode_v<maybe> == relocate_t::maybe_trivial);
static_assert(get_common_relocate_mode_v<maybe, normal, int> == relocate_t::normal);
static_assert(get_common_relocate_mode_v<normal, int, maybe> == relocate_t::normal);
static_assert(get_common_relocate_mode_v<normal, maybe, int> == relocate_t::normal);
static_assert(get_common_relocate_mode_v<int, common_relocate_test::incomplete> ==
              relocate_t::maybe_trivial);
static_assert(std::is_same_v<decltype(wjr::get_common_relocate_mode<>::value), const relocate_t>);
static_assert(wjr::_get_common_relocate_mode_impl<relocate_t::normal>::value == relocate_t::normal);
static_assert(wjr::_get_common_relocate_mode_impl<relocate_t::maybe_trivial, int, int>::value ==
              relocate_t::maybe_trivial);
static_assert(wjr::_get_common_relocate_mode_impl<relocate_t::normal, int, maybe>::value ==
              relocate_t::normal);

template <size_t>
using indexed_int = int;

template <typename>
struct wide_composites;

template <size_t... Indices>
struct wide_composites<std::index_sequence<Indices...>> {
    static_assert(get_relocate_mode_v<wjr::tuple<indexed_int<Indices>...>> == relocate_t::trivial);
    static_assert(get_relocate_mode_v<std::tuple<indexed_int<Indices>...>> == relocate_t::trivial);
    static_assert(get_relocate_mode_v<wjr::tuple<normal, indexed_int<Indices>...>> ==
                  relocate_t::normal);
    static_assert(get_relocate_mode_v<std::tuple<indexed_int<Indices>..., normal>> ==
                  relocate_t::normal);
    static_assert(get_relocate_mode_v<wjr::tuple<indexed_int<Indices>..., maybe>> ==
                  relocate_t::maybe_trivial);
    static_assert(
        get_common_relocate_mode_v<indexed_int<Indices>..., common_relocate_test::incomplete> ==
        relocate_t::maybe_trivial);
};

static_assert(sizeof(wide_composites<std::make_index_sequence<256>>) != 0);

template <class Composite>
void relocate_composite() {
    alignas(Composite) unsigned char source_bytes[sizeof(Composite)];
    alignas(Composite) unsigned char target_bytes[sizeof(Composite)];
    auto *source = std::construct_at(reinterpret_cast<Composite *>(source_bytes));
    auto *target = reinterpret_cast<Composite *>(target_bytes);
    normal::moves = 0;
    normal::destructions = 0;
    wjr::relocate_at(source, target);
    EXPECT_EQ(normal::moves, 1);
    EXPECT_EQ(normal::destructions, 1);
    std::destroy_at(target);
    EXPECT_EQ(normal::destructions, 2);
}

TEST(CommonRelocate, PairAndTupleMoveAndDestroyEveryNontrivialMember) {
    relocate_composite<std::pair<const int, normal>>();
    relocate_composite<wjr::compressed_pair<int, normal>>();
    relocate_composite<std::tuple<int, normal, int>>();
    relocate_composite<wjr::tuple<int, int, normal>>();
    relocate_composite<std::pair<int, maybe>>();
}
} // namespace
