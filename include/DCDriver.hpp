#pragma once

#include <string>
#include <cstddef>
#include <istream>
#include <vector>

#include "DCScanner.hpp"
#include "InstructionRegistry.hpp"
#include "VariableRegistry.hpp"
#include "IntInstrBlock.hpp"
#include "Expression.hpp"

#include "DCParser.tab.hh"

namespace DC{

class DCDriver{

   DC::DCParser  *parser  = nullptr;
   DC::DCScanner *scanner = nullptr;

   IntInstrBlock intInstructions;
   InstructionRegistry instructions;
   VariableRegistry variables;

   void parseHelper( std::istream &stream );

public:
   DCDriver() = default;

   virtual ~DCDriver();

   void parse( const char * const filename );

   void declareVariable(std::string variable);
   void declareArray(std::string variable, lint size);
   void postDecl();

   void parseRead(std::string variable);
   void parseWrite(std::string variable);
   void parseAssign(std::string variable,Expression expr);
   void parseVariable(std::string variable);
   std::string parseArrayLookup(std::string variable,std::string index);
   void halt();

   std::ostream& print(std::ostream &stream);
};

}
