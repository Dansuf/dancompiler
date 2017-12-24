#include <cctype>
#include <fstream>
#include <cassert>
#include <algorithm>

#include "DCDriver.hpp"
#include "CompilerException.hpp"

DC::DCDriver::~DCDriver()
{
   delete(scanner);
   scanner = nullptr;
   delete(parser);
   parser = nullptr;
}

void DC::DCDriver::parse( const char * const filename )
{
   assert(filename != nullptr);
   std::ifstream inFile(filename);
   if(!inFile.good())
   {
       exit( EXIT_FAILURE );
   }
   parseHelper(inFile);
   return;
}


void DC::DCDriver::parseHelper( std::istream &stream )
{
   delete(scanner);
   try
   {
      scanner = new DC::DCScanner( &stream );
   }
   catch( std::bad_alloc &ba )
   {
      std::cerr << "Failed to allocate scanner: (" << ba.what() << "), exiting!!\n";
      exit( EXIT_FAILURE );
   }

   delete(parser);
   try
   {
      parser = new DC::DCParser(*scanner, *this);
   }
   catch( std::bad_alloc &ba )
   {
      std::cerr << "Failed to allocate parser: (" << ba.what() << "), exiting!!\n";
      exit( EXIT_FAILURE );
   }

   const int accept( 0 );
   if( parser->parse() != accept )
   {
      std::cerr << "Parse failed!!\n";
   }
   return;
}

void DC::DCDriver::declareVariable(std::string variable)
{
   this->variables.addVariable(variable);
}

void DC::DCDriver::declareArray(std::string variable, lint size)
{
   this->variables.addArrayVariable(variable,size);
}

void DC::DCDriver::postDecl()
{
   auto arrays = this->variables.getArrays();
   for(std::string arr : arrays)
   {
      this->intInstructions.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::SET,arr,VariableRegistry::toConst(variables.getIndex(arr)+1)));
   }
}

void DC::DCDriver::parseRead(std::string variable)
{
   //lint index = this->variables.get_index(variable);
   this->variables.assertStorableVariable(variable);

   intInstructions.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::GET,variable,"",""));
   //instructions.add_instruction(Instr::GET);
   //instructions.add_instruction(Instr::STORE,index);
   this->variables.freeIntTemps();
}

void DC::DCDriver::parseWrite(std::string variable)
{
   //lint index = this->variables.get_index(variable);
   this->variables.assertLoadableVariable(variable);

   intInstructions.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::PUT,variable));
   //instructions.add_instruction(Instr::LOAD,index);
   //instructions.add_instruction(Instr::PUT);
   this->variables.freeIntTemps();
}

void DC::DCDriver::parseAssign(std::string variable,Expression expr)
{
   this->variables.assertStorableVariable(variable);

   std::string val1_name = expr.getVal1();
   this->variables.assertLoadableVariable(val1_name); //TODO change for sth more universal


   if(expr.onlyOneVal())
   {
      intInstructions.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::SET,variable,val1_name));
   }
   else
   {
      std::string val2_name = expr.getVal2();
      this->variables.assertLoadableVariable(val2_name); //TODO change for sth more universal
      IntInstrType type;

      switch(expr.getOp())
      {
         case Operator::PLUS:
            type = IntInstrType::ADD;
            break;
         case Operator::MINUS:
            type = IntInstrType::SUB;
            break;
         case Operator::STAR:
            type = IntInstrType::MUL;
            break;
         case Operator::SLASH:
            type = IntInstrType::DIV;
            break;
         case Operator::PERCENT:
            type = IntInstrType::MOD;
            break; //TODO default
      }

      intInstructions.addInstr((IntInstrAbstr*)new IntInstr(type,variable,val1_name,val2_name));
   }
   this->variables.freeIntTemps();
}

void DC::DCDriver::parseVariable(std::string variable)
{
   this->variables.assertLoadableVariable(variable);
}

std::string DC::DCDriver::parseArrayLookup(std::string variable,std::string index)
{
   this->variables.assertArrayVariable(variable);

   if(!VariableRegistry::isConst(index))
   {
      this->variables.assertLoadableVariable(index);
   }

   std::string temp = this->variables.getIntTemp();
   intInstructions.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::ADD,temp,index,variable));

   return "@" + temp;
}

void DC::DCDriver::halt()
{
   intInstructions.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::HALT));
   //instructions.add_instruction(Instr::HALT);

   intInstructions.dbgPrint();

   intInstructions.optimize();
   this->instructions = intInstructions.translate(variables);
   this->instructions.translateLabels();
}

std::ostream& DC::DCDriver::print( std::ostream &stream )
{
   this->instructions.print(stream);
   return(stream);
}
