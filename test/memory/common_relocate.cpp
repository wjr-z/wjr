#include <tuple>

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
} // namespace common_relocate_test

template <>
struct wjr::get_relocate_mode<common_relocate_test::maybe> {
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
