#include <type_traits>
#include <utility>

#include <wjr/tp/base.hpp>

namespace {

template <typename... Types>
struct pack {};

struct incomplete;
using function = int(double);
using types = pack<void, incomplete, const volatile int, function, int &, incomplete *>;

static_assert(std::is_same_v<wjr::tp_at_t<types, 0>, void>);
static_assert(std::is_same_v<wjr::tp_at_t<types, 1>, incomplete>);
static_assert(std::is_same_v<wjr::tp_at_t<types, 2>, const volatile int>);
static_assert(std::is_same_v<wjr::tp_at_t<types, 3>, function>);
static_assert(std::is_same_v<wjr::tp_at_t<types, 4>, int &>);
static_assert(std::is_same_v<wjr::tp_at_t<types, 5>, incomplete *>);
static_assert(std::is_same_v<wjr::tp_at_t<std::pair<int, long>, 1>, long>);
static_assert(std::is_same_v<wjr::tp_at_t<pack<int, int, int>, 2>, int>);
static_assert(std::is_same_v<wjr::tp_at_t<pack<const int[3]>, 0>, const int[3]>);
static_assert(std::is_same_v<wjr::tp_front_t<types>, void>);
static_assert(std::is_same_v<wjr::tp_back_t<types>, incomplete *>);

static_assert(std::is_same_v<wjr::tp_cut_t<types, 0, 6>, types>);
static_assert(std::is_same_v<wjr::tp_cut_t<types, 1, 4>,
                             pack<incomplete, const volatile int, function, int &>>);
static_assert(std::is_same_v<wjr::tp_cut_t<types, 5, 1>, pack<incomplete *>>);
static_assert(std::is_same_v<wjr::tp_cut_t<types, 0, 0>, pack<>>);
static_assert(std::is_same_v<wjr::tp_cut_t<types, 3, 0>, pack<>>);
static_assert(std::is_same_v<wjr::tp_cut_t<types, 6, 0>, pack<>>);
static_assert(std::is_same_v<wjr::tp_cut_t<pack<int, long, int, long>, 1, 2>, pack<long, int>>);
static_assert(std::is_same_v<wjr::tp_cut_t<pack<>, 0, 0>, pack<>>);
static_assert(std::is_same_v<wjr::tp_cut_t<std::pair<int, long>, 0, 2>, std::pair<int, long>>);
static_assert(
    std::is_same_v<wjr::tp_cut_t<wjr::tp_list<int, char, long>, 1, 1>, wjr::tp_list<char>>);

static_assert(std::is_same_v<wjr::tp_prefix_t<pack<>, 0>, pack<>>);
static_assert(std::is_same_v<wjr::tp_suffix_t<pack<>, 0>, pack<>>);
static_assert(std::is_same_v<wjr::tp_remove_prefix_t<types, 6>, pack<>>);
static_assert(std::is_same_v<wjr::tp_remove_suffix_t<types, 6>, pack<>>);
static_assert(std::is_same_v<wjr::tp_pop_front_t<pack<int>>, pack<>>);
static_assert(std::is_same_v<wjr::tp_pop_back_t<pack<int>>, pack<>>);
static_assert(
    std::is_same_v<wjr::tp_replace_at_t<pack<int, char, long>, 1, short>, pack<int, short, long>>);
static_assert(std::is_same_v<wjr::tp_insert_t<pack<int, long>, 1, char>, pack<int, char, long>>);
static_assert(std::is_same_v<wjr::tp_erase_t<types, 0, 6>, pack<>>);

template <size_t Index>
struct token;

template <typename>
struct long_pack;

template <size_t... Indices>
struct long_pack<std::index_sequence<Indices...>> {
    using type = pack<token<Indices>...>;
};

using indices = std::make_index_sequence<256>;
using many = typename long_pack<indices>::type;

static_assert(std::is_same_v<wjr::tp_at_t<many, 255>, token<255>>);

template <size_t... Indices>
consteval bool last_half(std::index_sequence<Indices...>) {
    using expected = pack<token<128 + Indices>...>;
    return std::is_same_v<wjr::tp_cut_t<many, 128, 128>, expected>;
}

static_assert(last_half(std::make_index_sequence<128>{}));

} // namespace
