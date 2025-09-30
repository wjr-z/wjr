/**
 * @file expression.hpp  
 * @author wjr
 * @brief Simplified BigInteger expression system
 * @version 0.2
 * @date 2025-09-30
 * 
 * @details This header provides a simplified lazy evaluation expression system
 * for biginteger operations with:
 * 
 * - Lazy expression evaluation with automatic stack allocator usage
 * - Mixed scalar and biginteger operations using biginteger_view
 * - Seamless integration with existing biginteger API
 * 
 * Usage:
 * ```cpp
 * #include <wjr/biginteger/expression.hpp>
 * using namespace wjr;
 * 
 * biginteger a(123), b(456), c(789);
 * biginteger result = (a + b) * c - square(a);
 * ```
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef WJR_BIGINTEGER_EXPRESSION_HPP__
#define WJR_BIGINTEGER_EXPRESSION_HPP__

// Core biginteger functionality
#include <wjr/biginteger/biginteger.hpp>

// Expression system
#include <wjr/biginteger/expression/expression.hpp>
#include <wjr/biginteger/expression/integration.hpp>

namespace wjr {

/**
 * @brief High-level expression evaluation with automatic stack allocator usage
 * 
 * This function automatically uses stack allocator when available for
 * better performance with temporary variables.
 * 
 * @tparam Storage Result biginteger storage type
 * @tparam Expr Expression type
 * @param result Destination biginteger
 * @param expression Expression to evaluate
 */
template <typename Storage, typename Expr>
void eval(basic_biginteger<Storage>& result, const Expr& expression) {
    static_assert(biginteger_expression_detail::__is_biginteger_expr_v<Expr>, 
                  "Expression must be a valid biginteger expression");
    
    // Always use stack allocator when possible
    unique_stack_allocator stack_alloc;
    biginteger_expression_detail::__evaluate_biginteger_expr(result, expression, &stack_alloc);
}

// Make common expression functions available in wjr namespace
using biginteger_expression_detail::__square;

} // namespace wjr

#endif // WJR_BIGINTEGER_EXPRESSION_HPP__