#pragma once

#include <string>

#include "general.hpp"

class Value
{

};

class ConstValue:Value
{
  lint val;

public:
  ConstValue(lint val): val(val){};

  inline lint get()
  {
    return this->val;
  }
};

class VariableValue:Value
{
  std::string name;

public:
  VariableValue(std::string name): name(name){};

  inline std::string get()
  {
    return this->name;
  }
};

class ArrVariableValue:Value
{
  std::string name;
  std::string index;

public:
  ArrVariableValue(std::string name, std::string index): name(name), index(index){};

  inline std::string getName()
  {
    return this->name;
  }

  inline std::string getIndex()
  {
    return this->index;
  }
};

class ArrConstValue:Value
{
  std::string name;
  lint constVal;

public:
  ArrConstValue(std::string name, lint constVal): name(name), constVal(constVal){};

  inline std::string getName()
  {
    return this->name;
  }

  inline lint getConstVal()
  {
    return this->constVal;
  }
};
