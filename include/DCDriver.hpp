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

   InstructionRegistry instructions;
   VariableRegistry variables;

   void parseHelper( std::istream &stream );

public:
   DCDriver() = default;

   virtual ~DCDriver();

   void parse( const char * const filename );

   void declareVariable(std::string variable);
   void declareArray(std::string variable, lint size);
   void declareIterator(std::string variable);

   IntInstrBlock parseRead(Value variable);
   IntInstrBlock parseWrite(Value variable);
   IntInstrBlock parseAssign(Value variable,Expression expr);
   IntInstrBlock parseIf(Condition cond, IntInstrBlock instructions);
   IntInstrBlock parseIfElse(Condition cond, IntInstrBlock instructions, IntInstrBlock elseInstructions);
   IntInstrBlock parseWhile(Condition cond, IntInstrBlock instructions);
   IntInstrBlock parseFor(std::string iterator, Value val1, Value val2, bool downTo, IntInstrBlock instructions);

   Value parseVariable(std::string variable);
   Value parseArrayLookup(std::string variable,std::string index);
   Value parseArrayLookup(std::string variable,lint index);
   void halt(IntInstrBlock block);

   std::ostream& print(std::ostream &stream);
};

}
