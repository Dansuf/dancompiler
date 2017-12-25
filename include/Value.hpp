#pragma once

#include <string>

#include "general.hpp"
#include "IntInstr.hpp"
#include "IntInstrBlock.hpp"

class Value
{
  std::string val;
  IntInstrAbstr* initInstr;

  bool hazInitInstr;

public:

  Value(std::string val, IntInstrAbstr* initInstr): val(val), initInstr(initInstr) { this->hazInitInstr = true; }
  Value(std::string val): val(val) { this->hazInitInstr = false; }
  Value(lint val): val(std::to_string(val)) { this->hazInitInstr = false; }
  Value() { this->hazInitInstr = false; }

  std::string get()
  {
    return val;
  }

  bool hasInitInstr()
  {
    return this->hazInitInstr;
  }

  void appendInitInstr(IntInstrBlock& block)
  {
    if(this->hazInitInstr)
      block.addInstr(initInstr);
  }
};
