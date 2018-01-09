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
   this->arrays.insert(std::make_pair(name,size));

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

std::string VariableRegistry::genVariable()
{
   std::string name = "VAR"+std::to_string(this->varCounter);

   this->addVariable(name);

   this->varCounter++;

   return name;
}

std::string VariableRegistry::getArrayIndexVar(std::string array, lint index)
{
   this->assertArrayVariable(array, VariableRegistry::toConst(index));

   lint varIndex = this->indexes.at(array) + index + 1;
   std::string varName = array + "["+std::to_string(index)+"]";
   this->indexes.insert(std::make_pair(varName, varIndex));

   return varName;
}

lint VariableRegistry::getForCounter()
{
   std::string name = "CTR"+std::to_string(this->lastFreeCounter);
   this->lastFreeCounter++;
   if(this->indexes.count(name) > 0)
   {
      throw CompilerException("Internal Error: Trying to declare already declared counter.");
   }
   return this->addVariable(name);
}

lint VariableRegistry::newLabel()
{
   return --this->lastLabelId;
}

void VariableRegistry::setInitialized(std::string name)
{
   if(VariableRegistry::isConst(name))
   {
      throw CompilerException("Internal error: Cannot initialize constant value.");
   }

   if(!VariableRegistry::isPointer(name))
   {
      if(this->indexes.count(name) == 0)
         throw CompilerException("Internal error: Cannot initialize non-existent variable.");
      this->initialized.insert(name);
   }
}

void VariableRegistry::assertInitialized(std::string name)
{
   if(!VariableRegistry::isPointer(name) && !VariableRegistry::isConst(name) && (name.find_first_of("[") == std::string::npos) && this->initialized.count(name) == 0)
   {
      throw CompilerException("Variable '"+name+"' is not initialized!");
   }
}

void VariableRegistry::assertLoadableVariable(std::string name)
{
   if(VariableRegistry::isConst(name)) return;
   if(VariableRegistry::isPointer(name)) name = VariableRegistry::stripPointer(name);
   if(this->indexes.count(name) == 0 || (this->iterators.count(name) > 0 && this->activeIterators.count(name) == 0))
   {
      throw CompilerException("Trying to access undeclared variable '"+name+"'");
   }

   if(this->arrays.count(name) > 0)
   {
      throw CompilerException("Trying to access array variable '"+name+"' as a normal variable.");
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

void VariableRegistry::assertArrayVariable(std::string name, std::string index)
{
   if(this->indexes.count(name) == 0 || (this->iterators.count(name) > 0 && this->activeIterators.count(name) == 0))
   {
      throw CompilerException("Trying to access undeclared variable '"+name+"'");
   }

   if(this->arrays.count(name) == 0)
   {
      throw CompilerException("'"+name+"' not an array!");
   }

   if(VariableRegistry::isConst(index))
   {
      lint indexVal = VariableRegistry::getConstVal(index);
      if(indexVal >= this->arrays[name])
      {
         throw CompilerException("Index "+index+" greater than array '"+name+"' bounds.");
      }
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

std::unordered_map<std::string,lint> VariableRegistry::getArrays()
{
   return this->arrays;
}
