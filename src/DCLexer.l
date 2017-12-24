%{
#include <string>

#include "general.hpp"
#include "DCScanner.hpp"
#undef  YY_DECL
#define YY_DECL int DC::DCScanner::yylex( DC::DCParser::semantic_type * const lval, DC::DCParser::location_type *loc )

using token = DC::DCParser::token;

#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="DC::DCScanner"
%option noyywrap
%option c++

%%
%{
            yylval = lval;
%}

[ \t]+      ;

VAR         {
               return( token::VAR );
            }

BEGIN       {
               return( token::BEG );
            }

END         {
               return( token::END );
            }

READ        {
               return( token::READ );
            }

WRITE       {
               return( token::WRITE );
            }

;           {
               return( token::SEMICOLON );
            }

:=          {
               return( token::SET );
            }

\+          {
               return( token::PLUS );
            }

-           {
               return( token::MINUS );
            }

\*          {
               return( token::STAR );
            }

\/          {
               return( token::SLASH );
            }

\%          {
               return( token::PERCENT );
            }

\[          {
               return( token::LSBR );
            }

\]          {
               return( token::RSBR );
            }

[_a-z]+     {
               yylval->build< std::string >( yytext );
               return( token::PID );
            }
            
[0-9]+      {
               yylval->build< lint >( std::stoll(yytext) );
               return( token::NUM );
            }

\n          {
               loc->lines();
            }

.           {
               return( token::ERROR );
            }
%%
