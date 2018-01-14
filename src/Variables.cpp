#include "Variables.hpp"
#include "CompilerException.hpp"

void Variables::addVariable(std::string name)
{
  if(this->variables.count(name) > 0 || this->arrays.count(name) > 0)
  {
    throw CompilerException("Redeclaration of variable '" + name + "'!");
  }
  this->variables.insert(name);
}

void Variables::addArrayVariable(std::string name, lint size)
{
  if(this->variables.count(name) > 0 || this->arrays.count(name) > 0)
  {
    throw CompilerException("Redeclaration of variable '" + name + "'!");
  }

  this->arrays.insert(std::make_pair(name,size));
}
