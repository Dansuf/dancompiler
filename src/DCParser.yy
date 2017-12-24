%skeleton "lalr1.cc"
%require  "3.0"
%debug
%defines
%define api.namespace {DC}
%define parser_class_name {DCParser}

%code requires{
   #include "Expression.hpp"
   #include "general.hpp"
   #include "values.hpp"


   namespace DC {
      class DCDriver;
      class DCScanner;
   }

}

%parse-param { DCScanner &scanner }
%parse-param { DCDriver &driver }

%code{
   #include <cstdio>
   #include <cstdlib>
   #include <fstream>

   #include "CompilerException.hpp"

   #include "DCDriver.hpp"

   #undef yylex
   #define yylex scanner.yylex

   #define CATCH(call,loc)  try \
                        { \
                           call; \
                        } \
                        catch(CompilerException e) \
                        { \
                           error(loc,e.what()); \
                           return 0; \
                        }
}

%define api.value.type variant
%define parse.assert

%type<std::string>      identifier
%type<std::string>      value
%type<Expression>       expression

%token                  VAR
%token                  BEG
%token                  END
%token                  READ
%token                  WRITE
%token                  SEMICOLON
%token                  LSBR
%token                  RSBR
%token                  SET
%token                  PLUS
%token                  MINUS
%token                  STAR
%token                  SLASH
%token                  PERCENT

%token<lint>            NUM
%token<std::string>     PID

%token                  ERROR
%locations

%%

program: VAR vdeclarations BEG                  { CATCH(driver.postDecl(),@3) }
               commands END                     { CATCH(driver.halt(),@5) }

vdeclarations: vdeclarations PID                { CATCH(driver.declareVariable($2),@2) }
         | vdeclarations PID LSBR NUM RSBR      { CATCH(driver.declareArray($2,$4),@2) }
         |
         ;
commands:  commands command
         | command
         ;

command:   identifier SET expression            { CATCH(driver.parseAssign($1,$3),@1) }
         | READ identifier SEMICOLON            { CATCH(driver.parseRead($2),@1) }
         | WRITE identifier SEMICOLON           { CATCH(driver.parseWrite($2),@1) }
         ;

expression: value                               { $$ = Expression($1); }
         | value PLUS value                     { $$ = Expression($1,Operator::PLUS,$3); }
         | value MINUS value                    { $$ = Expression($1,Operator::MINUS,$3); }
         | value STAR value                     { $$ = Expression($1,Operator::STAR,$3); }
         | value SLASH value                    { $$ = Expression($1,Operator::SLASH,$3); }
         | value PERCENT value                  { $$ = Expression($1,Operator::PERCENT,$3); }

value:      NUM                                 { $$ = std::to_string($1); }
         | identifier                           { $$ = $1; }

identifier: PID                                 { CATCH(driver.parseVariable($1),@1);$$ = $1; }
         | PID LSBR PID RSBR                    { CATCH($$ = driver.parseArrayLookup($1,$3),@1) }
         | PID LSBR NUM RSBR                    { CATCH($$ = driver.parseArrayLookup($1,std::to_string($3)),@1) }
         ;

%%


void DC::DCParser::error( const location_type &l, const std::string &err_message )
{
   fprintf(stderr, "Error at %d:%d: %s\n", l.begin.line,l.begin.column,err_message.c_str());
}
