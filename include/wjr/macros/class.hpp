/**
 * @file macros/class.hpp
 * @brief Class definition helper macros
 * @author wjr
 *
 * Provides macros to generate default special member functions.
 */

#ifndef WJR_MACROS_CLASS_HPP__
#define WJR_MACROS_CLASS_HPP__

/**
 * @brief Enable all default special member functions
 * @param CLASS Class name
 *
 * Generates default constructor, copy/move constructors,
 * copy/move assignment operators, and destructor.
 */
#define WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(CLASS)                                                  \
    CLASS() = default;                                                                             \
    CLASS(const CLASS &) = default;                                                                \
    CLASS(CLASS &&) = default;                                                                     \
    CLASS &operator=(const CLASS &) = default;                                                     \
    CLASS &operator=(CLASS &&) = default;                                                          \
    ~CLASS() = default

#endif // WJR_MACROS_CLASS_HPP__