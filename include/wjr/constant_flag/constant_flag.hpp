#ifndef WJR_CONSTANT_FLAG_CONSTANT_FLAG_HPP__
#define WJR_CONSTANT_FLAG_CONSTANT_FLAG_HPP__

#include <wjr/compressed_pair.hpp>
#include <wjr/tp.hpp>

namespace wjr {

struct constant_flag_base {};

template <typename Flag>
struct is_constant_flag : std::is_base_of_v<constant_flag_base, Flag> {};

template <typename Flag>
inline constexpr bool is_constant_flag_v = is_constant_flag<Flag>::value;

} // namespace wjr

#endif // WJR_CONSTANT_FLAG_CONSTANT_FLAG_HPP__