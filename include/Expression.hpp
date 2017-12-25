#pragma once

#include <memory>

#include "Value.hpp"

enum class Operator
{
  PLUS, MINUS, STAR, SLASH, PERCENT
};

class Expression
{
  Value val1;
  Value val2;
  Operator op;

public:
  Expression(Value val1, Operator op, Value val2): val1(val1), val2(val2), op(op) {};
  Expression(Value val1): val1(val1) {};
  Expression() {};

  bool onlyOneVal()
  {
    if(this->val2.get().empty())
    {
      return true;
    }
    return false;
  }

  Value getVal1()
  {
    return this->val1;
  }

  Value getVal2()
  {
    return this->val2;
  }

  Operator getOp()
  {
    return this->op;
  }
};
