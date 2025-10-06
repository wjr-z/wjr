/**
 * @file preprocessor/preview.hpp
 * @brief Preprocessor metaprogramming aggregate header
 * @author wjr
 *
 * Aggregates preprocessor utilities for arithmetic, logical operations,
 * and queue algorithms.
 */

#ifndef WJR_PREPROCESSOR_PREVIEW_HPP__
#define WJR_PREPROCESSOR_PREVIEW_HPP__

#include <wjr/preprocessor/arithmatic.hpp>
#include <wjr/preprocessor/detail.hpp>
#include <wjr/preprocessor/logical.hpp>

// Due to the fact that call is not a simple expansion, but takes the previous
// output as the next input, the difficulty of implementing recursion is also
// high.
#include <wjr/preprocessor/queue/algorithm.hpp>

#endif // ! WJR_PREPROCESSOR_PREVIEW_HPP__