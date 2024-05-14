%{
	#include "calc.tab.h"
  #undef yylex
  #undef YY_DECL
  #define YY_DECL int wjr::flexer::yylex(wjr::biginteger* yylval)
%}

%option noyywrap c++ 

%option yyclass="wjr::flexer"

%%
[a-z] {*yylval = *yytext - 'a';return yy::parser::token::VARIABLE;}
\.?[0-9]+|[0-9]+\.[0-9]* {*yylval = wjr::span<const char>(yytext, yyleng);return yy::parser::token::INTEGER;}
"-" {return yy::parser::token::SUB;}
"+" {return yy::parser::token::ADD;}
[()=/*%,\n] {return *yytext;}
"pow" {return yy::parser::token::POW;}
[ \t] ;
. {}
%%