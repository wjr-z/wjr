/**
 * @file json/lexer.hpp
 * @brief JSON lexer with architecture-specific optimizations
 * @author wjr
 *
 * Provides JSON lexical analysis with SIMD-optimized implementations
 * for supported architectures.
 */

#ifndef WJR_JSON_LEXER_HPP__
#define WJR_JSON_LEXER_HPP__

#include <wjr/json/lexer-impl.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/json/lexer.hpp>
#endif

#endif // WJR_JSON_LEXER_HPP__