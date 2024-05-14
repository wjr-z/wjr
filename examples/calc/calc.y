%language "c++"

%code requires{
    #include "flexer.hpp"
    wjr::biginteger sym[26];
    uint32_t mode;
    #define yylex lexer.yylex
}

%define api.value.type {wjr::biginteger}

%parse-param {wjr::flexer &lexer}

%token  INTEGER VARIABLE ADD SUB POW
%left   '+' '-'
%left   '*' '/' '%'
%right ADD SUB

%%
program:
    program statement '\n'
    |
    ;
statement:
     expr    {std::cout << $1 << '\n';}
     |VARIABLE '=' expr    {sym[(int)$1] = $3;}
     ;
expr:
    INTEGER
    |VARIABLE	{$$ = sym[(int)$1];}
    |expr ADD expr    {wjr::add($$, $1, $3);}
    |expr SUB expr    {wjr::sub($$, $1, $3);}
    |expr '*' expr    {wjr::mul($$, $1, $3);}
    |expr '/' expr    {wjr::tdiv_q($$, $1, $3);}
    |expr '%' expr    {wjr::tdiv_r($$, $1, $3);}
    | ADD expr {$$ = $2;}
    | SUB expr {$$ = $2; $$.negate();}
    |POW'('expr','expr')'      {pow($$, $3, (uint32_t)$5);}
	|'('expr')'       {$$ = $2;}
    ;
	
%%
void yy::parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}

int main()
{
    wjr::flexer lexer;
    yy::parser parser(lexer);
    return parser.parse();
}