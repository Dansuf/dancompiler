#pragma once

#include "IntInstr.hpp"
#include "IntInstrBlock.hpp"

class IntInstrFor : IntInstrAbstr
{
  IntInstrBlock valInitBlock;

  std::string iterator;
  std::string val1, val2;
  bool downTo;

  IntInstrBlock block;

public:
  IntInstrFor(std::string iterator, std::string val1, std::string val2, bool downTo, IntInstrBlock block, IntInstrBlock valInitBlock);

  void dbgPrint() override;

  InstructionRegistry translate(VariableRegistry& variables) override;

  void optimize() override;
};
