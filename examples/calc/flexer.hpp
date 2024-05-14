#ifndef WJR_FLEXER_HPP__
#define WJR_FLEXER_HPP__

#include <variant>

#include <wjr/biginteger.hpp>

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

namespace wjr {

class flexer : public yyFlexLexer {
public:
    int yylex(biginteger *yylval);
};

} // namespace wjr

#endif // WJR_FLEXER_HPP__