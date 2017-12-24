#pragma once

#include <memory>

#include "values.hpp"

enum class Operator
{
  PLUS, MINUS, STAR, SLASH, PERCENT
};

class Expression
{
  std::string val1;
  std::string val2;
  Operator op;

public:
  Expression(std::string val1, Operator op, std::string val2): val1(val1), val2(val2), op(op) {};
  Expression(std::string val1): val1(val1) {};
  Expression() {};

  bool onlyOneVal()
  {
    if(this->val2.empty())
    {
      return true;
    }
    return false;
  }

  std::string getVal1()
  {
    return this->val1;
  }

  std::string getVal2()
  {
    return this->val2;
  }

  Operator getOp()
  {
    return this->op;
  }
};
