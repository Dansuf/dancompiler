#include <cctype>
#include <fstream>
#include <cassert>
#include <algorithm>

#include "DCDriver.hpp"
#include "CompilerException.hpp"
#include "IntInstrIf.hpp"

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

IntInstrBlock DC::DCDriver::parseRead(Value variable)
{
   IntInstrBlock block;

   this->variables.assertStorableVariable(variable.get());

   variable.appendInitInstr(block);
   block.addInstr((IntInstrAbstr*) new IntInstr(IntInstrType::GET,variable.get()));

   this->variables.freeIntTemps();

   return block;
}

IntInstrBlock DC::DCDriver::parseWrite(Value variable)
{
   IntInstrBlock block;

   this->variables.assertLoadableVariable(variable.get());

   variable.appendInitInstr(block);

   block.addInstr((IntInstrAbstr*) new IntInstr(IntInstrType::PUT,variable.get()));

   this->variables.freeIntTemps();

   return block;
}

IntInstrBlock DC::DCDriver::parseAssign(Value variable,Expression expr)
{
   IntInstrBlock block;

   this->variables.assertStorableVariable(variable.get());

   Value val1 = expr.getVal1();
   this->variables.assertLoadableVariable(val1.get()); //TODO change for sth more universal

   if(expr.onlyOneVal())
   {
      variable.appendInitInstr(block);
      val1.appendInitInstr(block);
      block.addInstr((IntInstrAbstr*) new IntInstr(IntInstrType::SET,variable.get(),val1.get()));
   }
   else
   {
      Value val2 = expr.getVal2();
      this->variables.assertLoadableVariable(val2.get()); //TODO change for sth more universal
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

      variable.appendInitInstr(block);
      val1.appendInitInstr(block);
      val2.appendInitInstr(block);

      block.addInstr((IntInstrAbstr*) new IntInstr(type,variable.get(),val1.get(),val2.get()));
   }
   this->variables.freeIntTemps();

   return block;
}

IntInstrBlock DC::DCDriver::parseIf(Condition cond, IntInstrBlock instructions)
{
   this->variables.assertLoadableVariable(cond.getVal1().get()); //Asserts most probably redundand but better be safe
   this->variables.assertLoadableVariable(cond.getVal2().get());

   IntInstrBlock block;

   cond.getVal1().appendInitInstr(block);
   cond.getVal2().appendInitInstr(block);

   block.addInstr((IntInstrAbstr*) new IntInstrIf(cond.getVal1().get(),cond.getVal2().get(),cond.getCom(),instructions));

   return block;
}

IntInstrBlock DC::DCDriver::parseIfElse(Condition cond, IntInstrBlock instructions, IntInstrBlock elseInstructions)
{
   this->variables.assertLoadableVariable(cond.getVal1().get()); //Asserts most probably redundand but better be safe
   this->variables.assertLoadableVariable(cond.getVal2().get());

   IntInstrBlock block;

   cond.getVal1().appendInitInstr(block);
   cond.getVal2().appendInitInstr(block);

   block.addInstr((IntInstrAbstr*) new IntInstrIf(cond.getVal1().get(),cond.getVal2().get(),cond.getCom(),instructions,elseInstructions));

   return block;
}

Value DC::DCDriver::parseVariable(std::string variable)
{
   this->variables.assertLoadableVariable(variable);

   return Value(variable);
}

Value DC::DCDriver::parseArrayLookup(std::string variable,std::string index)
{
   this->variables.assertArrayVariable(variable);

   if(!VariableRegistry::isConst(index))
   {
      this->variables.assertLoadableVariable(index);
   }

   std::string temp = this->variables.getIntTemp();

   return Value("@" + temp,(IntInstrAbstr*)new IntInstr(IntInstrType::ADD,temp,index,variable));
}

Value DC::DCDriver::parseArrayLookup(std::string variable, lint index)
{
   return this->parseArrayLookup(variable, std::to_string(index));
}

void DC::DCDriver::halt(IntInstrBlock block)
{
   // Generate array addressess
   IntInstrBlock arrayInit;

   auto arrays = this->variables.getArrays();
   for(std::string arr : arrays)
   {
      arrayInit.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::SET,arr,VariableRegistry::toConst(variables.getIndex(arr)+1)));
   }


   block.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::HALT));

   arrayInit.append(block);

   arrayInit.dbgPrint();

   arrayInit.optimize();
   this->instructions = arrayInit.translate(variables);
   this->instructions.translateLabels();
}

std::ostream& DC::DCDriver::print( std::ostream &stream )
{
   this->instructions.print(stream);
   return(stream);
}
