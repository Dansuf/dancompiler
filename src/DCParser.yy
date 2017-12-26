%skeleton "lalr1.cc"
%require  "3.0"
%debug
%defines
%define api.namespace {DC}
%define parser_class_name {DCParser}

%code requires{
   #include "Expression.hpp"
   #include "Condition.hpp"
   #include "Value.hpp"
   #include "IntInstr.hpp"
   #include "IntInstrBlock.hpp"
   #include "general.hpp"


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

%type<Value>            identifier
%type<Value>            value
%type<Expression>       expression
%type<Condition>        condition
%type<IntInstrBlock>   command
%type<IntInstrBlock>    commands

%token                  VAR
%token                  BEG
%token                  END

%token                  LSBR
%token                  RSBR
%token                  SET

%token                  PLUS
%token                  MINUS
%token                  STAR
%token                  SLASH
%token                  PERCENT

%token                  EQ
%token                  NEQ
%token                  LT
%token                  GT
%token                  LTE
%token                  GTE

%token                  READ
%token                  WRITE
%token                  SEMICOLON

%token                  IF
%token                  THEN
%token                  ELSE
%token                  ENDIF

%token<lint>            NUM
%token<std::string>     PID

%token                  ERROR
%locations

%%

program: VAR vdeclarations BEG commands END                 { CATCH(driver.halt($4),@5) }

vdeclarations: vdeclarations PID                            { CATCH(driver.declareVariable($2),@2) }
         | vdeclarations PID LSBR NUM RSBR                  { CATCH(driver.declareArray($2,$4),@2) }
         |
         ;
commands:  commands command                                 { $1.append($2); $$ = $1; }
         | command                                          { $$ = $1; }
         ;

command:   identifier SET expression                        { CATCH($$ = driver.parseAssign($1,$3),@1) }
         | READ identifier SEMICOLON                        { CATCH($$ = driver.parseRead($2),@1) }
         | WRITE identifier SEMICOLON                       { CATCH($$ = driver.parseWrite($2),@1) /*TODO WRITE value*/ }
         | IF condition THEN commands ENDIF                 { CATCH($$ = driver.parseIf($2,$4),@1) }
         | IF condition THEN commands ELSE commands ENDIF   { CATCH($$ = driver.parseIfElse($2,$4,$6),@1) }
         ;

expression: value                                           { $$ = Expression($1); }
         | value PLUS value                                 { $$ = Expression($1,Operator::PLUS,$3); }
         | value MINUS value                                { $$ = Expression($1,Operator::MINUS,$3); }
         | value STAR value                                 { $$ = Expression($1,Operator::STAR,$3); }
         | value SLASH value                                { $$ = Expression($1,Operator::SLASH,$3); }
         | value PERCENT value                              { $$ = Expression($1,Operator::PERCENT,$3); }
         ;

condition: value EQ value                                   { $$ = Condition($1,Comparator::EQ,$3); }
         | value NEQ value                                  { $$ = Condition($1,Comparator::NEQ,$3); }
         | value LT value                                   { $$ = Condition($1,Comparator::LT,$3); }
         | value GT value                                   { $$ = Condition($1,Comparator::GT,$3); }
         | value LTE value                                  { $$ = Condition($1,Comparator::LTE,$3); }
         | value GTE value                                  { $$ = Condition($1,Comparator::GTE,$3); }
         ;

value:      NUM                                             { $$ = Value($1); }
         | identifier                                       { $$ = $1; }
         ;

identifier: PID                                             { CATCH($$ = driver.parseVariable($1),@1) }
         | PID LSBR PID RSBR                                { CATCH($$ = driver.parseArrayLookup($1,$3),@1) }
         | PID LSBR NUM RSBR                                { CATCH($$ = driver.parseArrayLookup($1,$3),@1) }
         ;

%%


void DC::DCParser::error( const location_type &l, const std::string &err_message )
{
   fprintf(stderr, "Error at %d:%d: %s\n", l.begin.line,l.begin.column,err_message.c_str());
}
