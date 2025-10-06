/**
 * @file utility/tag.hpp
 * @brief Tag types for construction and initialization control
 * @author wjr
 *
 * Provides tag types to control constructor selection and initialization behavior,
 * including default_construct, value_construct, in_place_reserve, and from_range.
 */

#ifndef WJR_UTILITY_TAG_HPP__
#define WJR_UTILITY_TAG_HPP__

#include <cstdint>

namespace wjr {

enum class branch {
    free,
    full,
};

struct in_place_empty_t {};

inline constexpr in_place_empty_t in_place_empty = {};

/**
 * @brief Tag of default construct.
 *
 * @details Use default_construct to indicate default construct. \n
 * Used to avoid value initialization.  \n
 * For example : \n
 * @code
 * wjr::vector<int> vec(10, default_construct); // default construct with 10
 * elements. wjr::vector<int> vec2(10); // value construct with 10 elements.
 * wjr::vector<int> vec3(10, 0); // value construct with 10 elements.
 * wjr::vector<int> vec4(10, value_construct); // value construct with 10
 * elements.
 * @endcode
 * elements of vec are not initialized. \n
 * elements of vec2, vec3, vec4 are initialized with 0.
 */
struct default_construct_t {};

/**
 * @see default_construct_t
 */
inline constexpr default_construct_t default_construct = {};

/**
 * @brief Tag of value construct.
 *
 */
struct value_construct_t {};

/**
 * @see value_construct_t
 *
 */
inline constexpr value_construct_t value_construct = {};

struct in_place_reserve_t {};

inline constexpr in_place_reserve_t in_place_reserve = {};

struct from_range_t {};

inline constexpr from_range_t from_range = {};

inline constexpr size_t dynamic_extent = SIZE_MAX;

} // namespace wjr

#endif // WJR_UTILITY_TAG_HPP__