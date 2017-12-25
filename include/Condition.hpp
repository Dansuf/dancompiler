#pragma once

#include <memory>

#include "Value.hpp"

enum class Comparator
{
  EQ, NEQ, LT, GT, LTE, GTE
};

class Condition
{
  Value val1;
  Value val2;
  Comparator com;

public:
  Condition(Value val1, Comparator com, Value val2): val1(val1), val2(val2), com(com) {};
  Condition() {};

  Value getVal1()
  {
    return this->val1;
  }

  Value getVal2()
  {
    return this->val2;
  }

  Comparator getCom()
  {
    return this->com;
  }
};
