#pragma once

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "DCParser.tab.hh"
#include "location.hh"

namespace DC{

class DCScanner : public yyFlexLexer{
public:

   DCScanner(std::istream *in) : yyFlexLexer(in)
   {
   };
   virtual ~DCScanner() {
   };

   using FlexLexer::yylex;

   virtual
   int yylex( DC::DCParser::semantic_type * const lval,
              DC::DCParser::location_type *location );


private:
   DC::DCParser::semantic_type *yylval = nullptr;
};

}
