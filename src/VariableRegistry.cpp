#include <utility>
#include <stdexcept>
#include <string>

#include "VariableRegistry.hpp"
#include "CompilerException.hpp"

VariableRegistry::VariableRegistry()
{
   for(int i = 0;i < INT_TEMPS + ASSEMBLER_TEMPS; i++)
   {
      this->indexes.insert(std::make_pair("TMP"+std::to_string(i),i));
   }
}

/**
  * @return index of added variable
 **/
lint VariableRegistry::addVariable(std::string name)
{
   if(this->indexes.count(name) > 0)
   {
      throw CompilerException("Redeclaration of variable '" + name + "'!");
   }
   this->indexes.insert(std::make_pair(name,lastFreeIndex));

   return lastFreeIndex++;

}

/**
  * @return index of added variable
 **/
lint VariableRegistry::addArrayVariable(std::string name, lint size)
{
   if(this->indexes.count(name) > 0)
   {
      throw CompilerException("Redeclaration of variable '" + name + "'!");
   }
   this->indexes.insert(std::make_pair(name,lastFreeIndex));
   this->arrays.insert(name);

   lint currIndex = lastFreeIndex;
   lastFreeIndex += size + 1;

   return currIndex;
}

lint VariableRegistry::getIndex(std::string name)
{
   if(VariableRegistry::isPointer(name)) name = VariableRegistry::stripPointer(name);
   try
   {
      return this->indexes.at(name);
   }
   catch(std::out_of_range e)
   {
      throw CompilerException("Trying to access undeclared variable '"+name+"'");
   }
}

std::string VariableRegistry::getIntTemp()
{
   if(lastIntTempAddr == INT_TEMPS)
   {
      throw CompilerException("Internal error! Maximum number of intermediate form temporary variables exceeded.");
   }

   return "TMP"+std::to_string(lastIntTempAddr++);
}

void VariableRegistry::freeIntTemps()
{
   lastIntTempAddr = 0;
}

std::string VariableRegistry::getAssemblerTemp()
{
   if(lastAssemblerTempAddr == INT_TEMPS + ASSEMBLER_TEMPS)
   {
      throw CompilerException("Internal error! Maximum number of assembler form temporary variables exceeded.");
   }

   return "TMP"+std::to_string(lastAssemblerTempAddr++);
}

void VariableRegistry::freeAssemblerTemps()
{
   lastAssemblerTempAddr = INT_TEMPS;
}

/**
  * @return index of added variable
 **/
lint VariableRegistry::setIterator(std::string name)
{
   lint index;
   if(this->activeIterators.count(name) > 0)
   {
      throw CompilerException("Redeclaration of iterator '" + name + "'!'");
   }

   if(this->indexes.count(name) > 0)
   {
      if(this->iterators.count(name) == 0)
      {
         throw CompilerException("Redeclaration of variable '" + name + "'!");
      }
      else
      {
         index = this->getIndex(name);
      }
   }
   else
   {
      this->indexes.insert(std::make_pair(name,lastFreeIndex));
      this->iterators.insert(name);
      index = lastFreeIndex++;
   }

   this->activeIterators.insert(name);

   return index;
}

void VariableRegistry::unsetIterator(std::string name)
{
   this->activeIterators.erase(name);
}

lint VariableRegistry::getForCounter()
{
   if(this->indexes.count("CTR"+this->lastFreeCounter) > 0)
   {
      throw new CompilerException("Internal Error: Trying to declare already declared counter.");
   }
   return this->addVariable("CTR"+this->lastFreeCounter++);
}

lint VariableRegistry::newLabel()
{
   return --this->lastLabelId;
}

void VariableRegistry::assertLoadableVariable(std::string name)
{
   if(VariableRegistry::isConst(name)) return;
   if(VariableRegistry::isPointer(name)) name = VariableRegistry::stripPointer(name);
   if(this->indexes.count(name) == 0 || (this->iterators.count(name) > 0 && this->activeIterators.count(name) == 0))
   {
      throw CompilerException("Trying to access undeclared variable '"+name+"'");
   }
}

void VariableRegistry::assertStorableVariable(std::string name)
{
   if(VariableRegistry::isPointer(name)) name = VariableRegistry::stripPointer(name);
   if(this->indexes.count(name) == 0 || (this->iterators.count(name) > 0 && this->activeIterators.count(name) == 0))
   {
      throw CompilerException("Trying to write to undeclared variable '"+name+"'");
   }
   if(this->activeIterators.count(name) > 0)
   {
      throw CompilerException("Trying to modify iterator '"+name+"'");
   }
}

void VariableRegistry::assertArrayVariable(std::string name)
{
   this->assertLoadableVariable(name);
   if(this->arrays.count(name) == 0)
   {
      throw CompilerException("'"+name+"' not an array!");
   }
}

bool VariableRegistry::isPointer(std::string variable)
{
   if(!variable.empty() && variable[0] == '@') return true;
   else return false;
}

bool VariableRegistry::isConst(std::string variable)
{
   try
   {
         std::stoll(variable);
         return true;
   }
   catch(std::invalid_argument e)
   {
      return false;
   }
}

std::string VariableRegistry::stripPointer(std::string variable)
{
   if(variable.empty() || !(variable[0] == '@')) return variable;

   variable.erase(variable.begin());

   return variable;
}

lint VariableRegistry::getConstVal(std::string value)
{
   return std::stoll(value);
}

std::string VariableRegistry::toConst(lint val)
{
   return std::to_string(val);
}

std::set<std::string> VariableRegistry::getArrays()
{
   return this->arrays;
}
