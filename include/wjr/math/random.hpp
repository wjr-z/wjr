#ifndef WJR_MATH_RANDOM_HPP__
#define WJR_MATH_RANDOM_HPP__

#include <algorithm>

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename Engine, typename = void>
struct __uniform_random_bit_generator_impl : std::false_type {};

template <typename Engine>
struct __uniform_random_bit_generator_impl<Engine, std::enable_if_t<std::is_invocable_v<Engine &>>>
    : std::conjunction<is_nonbool_unsigned_integral<std::invoke_result_t<Engine &>>,
                       std::is_same<decltype(Engine::min()), std::invoke_result_t<Engine &>>,
                       std::is_same<decltype(Engine::max()), std::invoke_result_t<Engine &>>> {};

template <typename Engine>
struct uniform_random_bit_generator : __uniform_random_bit_generator_impl<Engine> {};

template <typename Engine>
inline constexpr bool uniform_random_bit_generator_v = uniform_random_bit_generator<Engine>::value;

template <typename Engine>
struct biginteger_uniform_random_bit_generator
    : std::conjunction<__uniform_random_bit_generator_impl<Engine>,
                       std::is_same<std::invoke_result_t<Engine &>, uint64_t>,
                       std::bool_constant<Engine::min() == std::numeric_limits<uint64_t>::min()>,
                       std::bool_constant<Engine::max() == std::numeric_limits<uint64_t>::max()>> {
};

template <typename Engine>
inline constexpr bool biginteger_uniform_random_bit_generator_v =
    biginteger_uniform_random_bit_generator<Engine>::value;

template <typename Iter, typename Rand>
void random(Iter first, Iter last, Rand &&rd) {
    std::generate(first, last, std::ref(rd));
}

template <typename Iter, typename Rand>
void random_n(Iter First, size_t Count, Rand &&rd) {
    std::generate_n(First, Count, std::ref(rd));
}

} // namespace wjr

#endif // WJR_MATH_RANDOM_HPP__