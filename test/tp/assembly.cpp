#include <type_traits>
#include <utility>

#include <wjr/tp/algorithm.hpp>

namespace {

template <typename... Types>
struct pack {};

template <typename First = int, typename... Rest>
struct default_pack {};

struct incomplete;
using function = int(double);
using unusual = pack<void, incomplete, int &, function, const int[3]>;

static_assert(std::is_same_v<wjr::tp_concat_t<int>, int>);
static_assert(std::is_same_v<wjr::tp_concat_t<pack<>>, pack<>>);
static_assert(std::is_same_v<wjr::tp_concat_t<unusual>, unusual>);
static_assert(std::is_same_v<wjr::tp_concat_t<pack<>, unusual>, unusual>);
static_assert(std::is_same_v<wjr::tp_concat_t<unusual, pack<>>, unusual>);
static_assert(
    std::is_same_v<wjr::tp_concat_t<pack<int>, std::pair<char, long>>, pack<int, char, long>>);

using seven = wjr::tp_concat_t<pack<int>, wjr::tp_list<char>, pack<>, pack<long>,
                               wjr::tp_list<short>, pack<>, pack<void>>;
using eight = wjr::tp_concat_t<pack<int>, wjr::tp_list<char>, pack<>, pack<long>,
                               wjr::tp_list<short>, pack<>, pack<void>, unusual>;
using nine = wjr::tp_concat_t<pack<int>, wjr::tp_list<char>, pack<>, pack<long>,
                              wjr::tp_list<short>, pack<>, pack<void>, unusual, pack<int>>;
static_assert(std::is_same_v<seven, pack<int, char, long, short, void>>);
static_assert(std::is_same_v<eight, pack<int, char, long, short, void, void, incomplete, int &,
                                         function, const int[3]>>);
static_assert(std::is_same_v<nine, pack<int, char, long, short, void, void, incomplete, int &,
                                        function, const int[3], int>>);
static_assert(std::is_same_v<wjr::tp_concat_t<pack<>, pack<>, pack<>, pack<>, pack<>, pack<>,
                                              pack<>, pack<>, pack<>>,
                             pack<>>);
static_assert(std::is_same_v<wjr::tp_repeat_t<unusual, 0>, pack<>>);
static_assert(std::is_same_v<wjr::tp_repeat_t<unusual, 1>, unusual>);
static_assert(std::is_same_v<wjr::tp_repeat_t<pack<int, char>, 2>, pack<int, char, int, char>>);
static_assert(
    std::is_same_v<wjr::tp_repeat_t<pack<int, char>, 3>, pack<int, char, int, char, int, char>>);
static_assert(
    std::is_same_v<wjr::tp_repeat_t<pack<int>, 7>, pack<int, int, int, int, int, int, int>>);
static_assert(std::is_same_v<wjr::tp_repeat_t<pack<>, 256>, pack<>>);
static_assert(std::is_same_v<wjr::tp_repeat_t<unusual, 2>,
                             pack<void, incomplete, int &, function, const int[3], void, incomplete,
                                  int &, function, const int[3]>>);

/// @brief Preserve the original clear-container tail, including default arguments.
static_assert(std::is_same_v<wjr::tp_repeat_t<default_pack<char>, 0>, default_pack<int>>);
static_assert(std::is_same_v<wjr::tp_repeat_t<default_pack<char>, 1>, default_pack<char, int>>);
static_assert(std::is_same_v<wjr::tp_repeat_t<default_pack<char, long>, 3>,
                             default_pack<char, long, char, long, char, long, int>>);

template <size_t Index>
struct token;

template <typename>
struct long_assembly;

template <size_t... Indices>
struct long_assembly<std::index_sequence<Indices...>> {
    using type = wjr::tp_concat_t<pack<>, wjr::tp_list<token<Indices>>...>;
    static_assert(std::is_same_v<type, pack<token<Indices>...>>);
};

using assembled = typename long_assembly<std::make_index_sequence<256>>::type;
static_assert(wjr::tp_size_v<assembled> == 256);
using repeated = wjr::tp_repeat_t<pack<incomplete, int &>, 256>;
static_assert(wjr::tp_size_v<repeated> == 512);
static_assert(std::is_same_v<wjr::tp_at_t<repeated, 510>, incomplete>);
static_assert(std::is_same_v<wjr::tp_at_t<repeated, 511>, int &>);

template <typename T>
using is_pointer = std::is_pointer<T>;

static_assert(std::is_same_v<wjr::tp_filter_t<pack<int, int *, char *, void>, is_pointer>,
                             pack<int *, char *>>);
static_assert(std::is_same_v<wjr::tp_remove_if_t<pack<int, int *, char *, void>, is_pointer>,
                             pack<int, void>>);
static_assert(std::is_same_v<
              wjr::tp_product_t<pack, pack<int, char>, pack<void, long>>,
              wjr::tp_list<pack<int, void>, pack<int, long>, pack<char, void>, pack<char, long>>>);

} // namespace
