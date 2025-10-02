/**
 * @file expression.cpp
 * @brief Unit tests for biginteger expression template system using Google Test
 */

#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/biginteger/biginteger.hpp>
#include <wjr/biginteger/expression/expression.hpp>
#include <wjr/biginteger/expression/integration.hpp>

using namespace wjr;
using namespace biginteger_detail;

TEST(BigintegerExpression, TypeTraits) {
    // Test is_biginteger_expression
    static_assert(!is_biginteger_expression_v<int>, "int should not be expression");
    static_assert(!is_biginteger_expression_v<biginteger>,
                  "biginteger should not be expression directly");
    static_assert(is_biginteger_expression_v<ref_expr<int>>, "ref_expr should be expression");
    static_assert(is_biginteger_expression_v<binary_expr<add_tag, ref_expr<int>, ref_expr<int>>>,
                  "binary_expr should be expression");

    // Test is_convertible_to_expr
    static_assert(is_convertible_to_expr_v<int>, "int should be convertible to expr");
    static_assert(is_convertible_to_expr_v<int64_t>, "int64_t should be convertible to expr");
    static_assert(is_convertible_to_expr_v<biginteger>, "biginteger should be convertible to expr");
    static_assert(is_convertible_to_expr_v<biginteger_view>,
                  "biginteger_view should be convertible to expr");
    static_assert(!is_convertible_to_expr_v<ref_expr<int>>,
                  "ref_expr is already an expression, not convertible");

    // Test is_expr_operand
    static_assert(is_expr_operand_v<int>, "int should be valid operand");
    static_assert(is_expr_operand_v<biginteger>, "biginteger should be valid operand");
    static_assert(is_expr_operand_v<ref_expr<int>>, "ref_expr should be valid operand");
    static_assert(is_expr_operand_v<biginteger &>, "biginteger& should be valid operand");
    static_assert(is_expr_operand_v<const biginteger &>,
                  "const biginteger& should be valid operand");
    static_assert(is_expr_operand_v<biginteger &&>, "biginteger&& should be valid operand");
}

TEST(BigintegerExpression, ValueCategory) {
    // Test integral types
    static_assert(std::is_same_v<expr_value_category_t<int>, int>, "int should be stored by value");
    static_assert(std::is_same_v<expr_value_category_t<int64_t>, int64_t>,
                  "int64_t should be stored by value");

    // Test biginteger_view
    static_assert(std::is_same_v<expr_value_category_t<biginteger_view>, biginteger_view>,
                  "biginteger_view should be stored by value");

    // Test biginteger lvalue
    static_assert(std::is_same_v<expr_value_category_t<biginteger &>, const biginteger &>,
                  "biginteger& should be stored by const reference");
    static_assert(std::is_same_v<expr_value_category_t<const biginteger &>, const biginteger &>,
                  "const biginteger& should be stored by const reference");

    // Test biginteger rvalue
    static_assert(std::is_same_v<expr_value_category_t<biginteger &&>, biginteger>,
                  "biginteger&& should be stored by value");
}

TEST(BigintegerExpression, RefExpr) {
    // Test with integer
    int value = 42;
    ref_expr<int> expr(value);
    EXPECT_EQ(expr.value(), 42);
    static_assert(ref_expr<int>::is_leaf, "ref_expr should be leaf");

    // Test with biginteger reference
    biginteger bi_value(100);
    ref_expr<const biginteger &> bi_expr(bi_value);
    EXPECT_EQ(bi_expr.value(), biginteger(100));
    static_assert(ref_expr<const biginteger &>::is_leaf, "ref_expr should be leaf");

    // Test with biginteger rvalue
    ref_expr<biginteger> rvalue_expr(biginteger(200));
    EXPECT_EQ(rvalue_expr.value(), biginteger(200));
}

TEST(BigintegerExpression, BinaryExpr) {
    // Test expression construction
    {
        int a = 10, b(20);
        auto left = ref_expr<int>(a);
        auto right = ref_expr<int>(b);

        binary_expr<add_tag, ref_expr<int>, ref_expr<int>> expr(left, right);

        static_assert(!decltype(expr)::is_leaf, "binary_expr should not be leaf");
        EXPECT_EQ(expr.left().value(), 10);
        EXPECT_EQ(expr.right().value(), 20);
    }
}

TEST(BigintegerExpression, MakeExpr) {
    // Test with integer
    int value = 42;
    auto expr = make_expr(value);
    static_assert(std::is_same_v<decltype(expr), ref_expr<int>>,
                  "make_expr(int) should return ref_expr<int>");
    EXPECT_EQ(expr.value(), 42);

    // Test with biginteger lvalue
    biginteger bi_value(100);
    auto bi_expr = make_expr(bi_value);
    static_assert(std::is_same_v<decltype(bi_expr), ref_expr<const biginteger &>>,
                  "make_expr(biginteger&) should return ref_expr<const biginteger&>");
    EXPECT_EQ(bi_expr.value(), biginteger(100));

    // Test with biginteger rvalue
    auto rvalue_expr = make_expr(biginteger(200));
    static_assert(std::is_same_v<decltype(rvalue_expr), ref_expr<biginteger>>,
                  "make_expr(biginteger&&) should return ref_expr<biginteger>");
    EXPECT_EQ(rvalue_expr.value(), biginteger(200));

    // Test with already an expression
    ref_expr<int> original(42);
    auto &&expr_forwarded = make_expr(std::move(original));
    // When passing an expression, it should be forwarded as-is
    static_assert(is_biginteger_expression_v<std::decay_t<decltype(expr_forwarded)>>,
                  "make_expr(expr) should return an expression");
}

TEST(BigintegerExpression, Operators) {
    // Test addition
    biginteger a(100), b(200);
    auto expr = a + b;

    using expr_t = decltype(expr);
    static_assert(biginteger_detail::is_biginteger_expression_v<expr_t>,
                  "a + b should be an expression");
    static_assert(!expr_t::is_leaf, "binary expression should not be leaf");

    // Test subtraction
    biginteger a2(100), b2(200);
    auto expr2 = a2 - b2;

    static_assert(biginteger_detail::is_biginteger_expression_v<decltype(expr2)>,
                  "a - b should be an expression");

    // Test multiplication
    biginteger a3(100), b3(200);
    auto expr3 = a3 * b3;

    static_assert(biginteger_detail::is_biginteger_expression_v<decltype(expr3)>,
                  "a * b should be an expression");

    // Test mixed types
    biginteger a4(100);
    int b4 = 50;
    auto expr4 = a4 + b4;

    static_assert(biginteger_detail::is_biginteger_expression_v<decltype(expr4)>,
                  "a + int should be an expression");

    // Test complex expression
    biginteger a5(10), b5(20), c5(30);
    auto expr5 = a5 + b5 * c5;

    using expr5_t = decltype(expr5);
    static_assert(biginteger_detail::is_biginteger_expression_v<expr5_t>,
                  "complex expression should be an expression");
}

TEST(BigintegerExpression, Evaluation) {

    // Test simple addition
    biginteger a(100), b(200);
    biginteger result = a + b;
    EXPECT_EQ(result, biginteger(300));

    // Test simple subtraction
    biginteger a2(300), b2(100);
    biginteger result2 = a2 - b2;
    EXPECT_EQ(result2, biginteger(200));

    // Test simple multiplication
    biginteger a3(10), b3(20);
    biginteger result3 = a3 * b3;
    EXPECT_EQ(result3, biginteger(200));

    // Test complex expression
    biginteger a4(10), b4(20), c4(30);
    biginteger result4 = a4 + b4 * c4;
    EXPECT_EQ(result4, biginteger(610)); // 10 + 20 * 30 = 10 + 600 = 610

    // Test with parentheses
    biginteger a5(10), b5(20), c5(30);
    biginteger result5 = (a5 + b5) * c5;
    EXPECT_EQ(result5, biginteger(900)); // (10 + 20) * 30 = 30 * 30 = 900

    // Test mixed with integers
    biginteger a6(100);
    biginteger result6 = a6 * 2 + 50;
    EXPECT_EQ(result6, biginteger(250)); // 100 * 2 + 50 = 250

    // Test rvalue optimization
    biginteger result7 = biginteger(1000) + biginteger(2000);
    EXPECT_EQ(result7, biginteger(3000));

    // Test assignment
    biginteger a8(100), b8(200), c8(300);
    biginteger result8;
    result8 = a8 + b8 * c8;
    EXPECT_EQ(result8, biginteger(60100));

    // Test chained operations
    biginteger a9(10), b9(20), c9(30);
    a9 = a9 + b9;
    EXPECT_EQ(a9, biginteger(30));
    b9 = a9 * c9;
    EXPECT_EQ(b9, biginteger(900));
    c9 = b9 - a9;
    EXPECT_EQ(c9, biginteger(870));
}

TEST(BigintegerExpression, IsLeaf) {
    // ref_expr is leaf
    static_assert(is_leaf_expr_v<ref_expr<int>>, "ref_expr should be leaf");
    static_assert(is_leaf_expr_v<ref_expr<biginteger>>, "ref_expr should be leaf");

    // binary_expr is not leaf
    using binary_t = binary_expr<add_tag, ref_expr<int>, ref_expr<int>>;
    static_assert(!is_leaf_expr_v<binary_t>, "binary_expr should not be leaf");

    // unary_expr is not leaf
    using unary_t = unary_expr<shift_left_tag, ref_expr<biginteger>>;
    static_assert(!is_leaf_expr_v<unary_t>, "unary_expr should not be leaf");
}

TEST(BigintegerExpression, ShiftLeft) {
    // Test shift left expression construction
    biginteger a(5);
    auto expr = a << 2u;
    static_assert(is_biginteger_expression_v<decltype(expr)>, "a << 2 should be an expression");

    // Test shift left evaluation: 5 << 2 = 20
    biginteger result1 = a << 2u;
    EXPECT_EQ(result1, biginteger(20));

    // Test shift left with uint64_t: 7 << 3 = 56
    biginteger b(7);
    biginteger result2 = b << 3ull;
    EXPECT_EQ(result2, biginteger(56));

    // Test shift left with larger values: 100 << 10 = 102400
    biginteger c(100);
    biginteger result3 = c << 10u;
    EXPECT_EQ(result3, biginteger(102400));

    // Test complex expression with shift: (a + b) << 2
    biginteger a4(10), b4(20);
    biginteger result4 = (a4 + b4) << 2u;
    EXPECT_EQ(result4, biginteger(120)); // (10 + 20) << 2 = 30 * 4 = 120

    // Test chained operations: a * 2 + 10 with shift
    biginteger a5(5);
    biginteger result5 = (a5 << 1u) + 10; // (5 << 1) + 10 = 10 + 10 = 20
    EXPECT_EQ(result5, biginteger(20));

    // Test shift with expression: (a * b) << shift
    biginteger a6(3), b6(4);
    biginteger result6 = (a6 * b6) << 2u; // (3 * 4) << 2 = 12 * 4 = 48
    EXPECT_EQ(result6, biginteger(48));

    // Test assignment with shift expression
    biginteger result7;
    biginteger a7(15);
    result7 = a7 << 4u; // 15 << 4 = 240
    EXPECT_EQ(result7, biginteger(240));
}

TEST(BigintegerExpression, UnaryNegate) {
    // Test unary negation expression construction
    biginteger a(100);
    auto expr = -a;
    static_assert(is_biginteger_expression_v<decltype(expr)>, "-a should be an expression");

    // Test basic negation: -100 = -100
    biginteger result1 = -a;
    EXPECT_EQ(result1, biginteger(-100));

    // Test double negation: -(-100) = 100
    biginteger result2 = -(-a);
    EXPECT_EQ(result2, biginteger(100));

    // Test negation with zero
    biginteger zero(0);
    biginteger result3 = -zero;
    EXPECT_EQ(result3, biginteger(0));

    // Test negation with negative number
    biginteger negative(-50);
    biginteger result4 = -negative;
    EXPECT_EQ(result4, biginteger(50));

    // Test negation in addition: -a + b
    biginteger b(150);
    biginteger result5 = -a + b; // -100 + 150 = 50
    EXPECT_EQ(result5, biginteger(50));

    // Test negation in subtraction: a - (-b)
    biginteger result6 = a - (-b); // 100 - (-150) = 100 + 150 = 250
    EXPECT_EQ(result6, biginteger(250));

    // Test negation with multiplication: -(a * b)
    biginteger c(2);
    biginteger result7 = -(a * c); // -(100 * 2) = -200
    EXPECT_EQ(result7, biginteger(-200));

    // Test complex expression: -(a + b) * c
    biginteger result8 = -(a + b) * c; // -(100 + 150) * 2 = -250 * 2 = -500
    EXPECT_EQ(result8, biginteger(-500));

    // Test negation with shift: -(a << 2)
    biginteger d(10);
    biginteger result9 = -(d << 2u); // -(10 << 2) = -40
    EXPECT_EQ(result9, biginteger(-40));

    // Test multiple negations in expression: -a + -b
    biginteger result10 = -a + (-b); // -100 + (-150) = -250
    EXPECT_EQ(result10, biginteger(-250));

    // Test assignment with negation expression
    biginteger result11;
    result11 = -a;
    EXPECT_EQ(result11, biginteger(-100));

    // Test negation with rvalue: -(biginteger(42))
    biginteger result12 = -(biginteger(42));
    EXPECT_EQ(result12, biginteger(-42));
}
